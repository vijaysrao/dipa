#include "tailored-glossy.h"

#include "lib/random.h"


/**
 * \defgroup custom-glossy-variables Application variables
 * @{
 */

static sync_data_struct sync_data;     /**< \brief Flooding data. */
static struct rtimer rt;                   /**< \brief Rtimer used to schedule Glossy. */
static struct pt pt;                       /**< \brief Protothread used to schedule Glossy. */
static rtimer_clock_t t_ref_l_old = 0;     /**< \brief Reference time computed from the Glossy
                                                phase before the last one. \sa get_t_ref_l */
static uint8_t skew_estimated = 0;         /**< \brief Not zero if the clock skew over a period of length
                                                \link GLOSSY_PERIOD \endlink has already been estimated. */
static uint8_t sync_missed = 0;            /**< \brief Current number of consecutive phases without
                                                synchronization (reference time not computed). */
static int period_skew = 0;                /**< \brief Current estimation of clock skew over a period
                                                of length \link GLOSSY_PERIOD \endlink. */

/** @} */

/**
 * \defgroup glossy-test-variables-stats Statistics variables
 * @{
 */

/*---------------------------------------------------------------------------*/
static rtimer_clock_t REF_TIME;
/*---------------------------------------------------------------------------*/
static int     offset_err;
static uint16_t run_time, last_time;
/*---------------------------------------------------------------------------*/
static int8_t FLOODING_ROLE;
static uint16_t rx_count;
static uint8_t hop_to_sink;

static unsigned int bad_crc_prev, bad_length_prev, bad_header_prev;
static unsigned int pkt_lost, ber_total, pkt_recvd;

/* Version 2, 3 */
static unsigned int power_level[] = {19, 23, 27, 31};
static int power_value[] = {5, 3, 1, 0};
static uint8_t n_power_levels = sizeof(power_level)/sizeof(unsigned int);

static unsigned int packets_sent = 0;
static unsigned int n_good_packets = 0;
static unsigned int power = 3;
static unsigned int sum_power = 0;
/*---------------------------------------------------------------------------*/

char tailored_glossy_scheduler(struct rtimer *t, void *ptr);
/*---------------------------------------------------------------------------*/

/** @} */
/** @} */

/**
 * \defgroup glossy-test-processes Application processes and functions
 * @{
 */

static inline void estimate_period_skew(void) {
	// Estimate clock skew over a period only if the reference time has been updated.
	if (GLOSSY_IS_SYNCED()) {
		// Estimate clock skew based on previous reference time and the Glossy period.
		period_skew = get_t_ref_l() - (t_ref_l_old + (rtimer_clock_t)GLOSSY_PERIOD);
		// Update old reference time with the newer one.
		t_ref_l_old = get_t_ref_l();
		// If Glossy is still bootstrapping, count the number of consecutive updates of the reference time.
		if (GLOSSY_IS_BOOTSTRAPPING()) {
			// Increment number of consecutive updates of the reference time.
			skew_estimated++;
			// Check if Glossy has exited from bootstrapping.
			if (!GLOSSY_IS_BOOTSTRAPPING()) {
				// Glossy has exited from bootstrapping.
				leds_off(LEDS_RED);
				// Initialize Energest values.
				energest_init();
//#if GLOSSY_DEBUG
				high_T_irq = 0;
				bad_crc = 0;
				bad_length = 0;
				bad_header = 0;
//#endif /* GLOSSY_DEBUG */

				bad_crc = 0;
				bad_length = 0;
				bad_header = 0;
				
				bad_crc_prev = 0;
				bad_length_prev = 0;
				bad_header_prev = 0;
				
				pkt_lost = 0;
				ber_total = 0;
				pkt_recvd = 0;
				
				/* Version 2,3 */
				sum_power = 0; 				
				packets_sent = 0;
				n_good_packets = 0;
			}
		}
	}
}

/* Version 2, 3 */
/** 
 * DIPA functions
 */
 
inline void dipa_good_feedback () {
	n_good_packets++;
	if (n_good_packets >= 5) {
		if (power <= 0) {
			power = 0; 
		} 
		else { 
			power --; 
		}
		n_good_packets = 0;
	}
}

inline void dipa_bad_feedback () {
	n_good_packets = 0;
	if (power >= (n_power_levels - 1)) {
		power = (random_rand() % n_power_levels);		 
	}
	else {
		power ++;
	}
}

/**
 * 
 */
PROCESS(tailored_glossy_print_process, "Tailored Glossy print process");

PROCESS_THREAD(tailored_glossy_print_process, ev, data)
{
	PROCESS_BEGIN();
	
	unsigned long volatile ber = 0;
	uint8_t print = 0, i, d;
	
	while(1) {
		PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
		
		print = 0;
		ber = 0;
		
		if (bad_length > bad_length_prev || bad_header > bad_header_prev) {
			print = 0;
			
			bad_length_prev = bad_length;
			bad_header_prev = bad_header;
		}
		if(get_rx_cnt()) {		
			print = 2;
			pkt_recvd ++;
		}
		if (bad_crc > bad_crc_prev) {
			print = 1;
			bad_crc_prev = bad_crc;
			
			for (i = 0; i < EMPTY_LEN; i++) {
				d = sync_data.empty[i];
				d ^= EMPTY_DATA;
				while (d) {
					d &= d-1;
					ber++;
				}
			}	
			
			ber_total += ber;						
			
			if (ber <= 0 && (sync_data.feedback == DIPA_ACK || sync_data.feedback == DIPA_NACK)) {
				print = 2;
				pkt_recvd ++;
				bad_crc --;
				bad_crc_prev = bad_crc;
			}
		}	
		
		if (print <= 0) {
			pkt_lost++;			
		}
		
		if (run_time < STABILIZATION_PERIOD) {
			dipa_feedback = EMPTY_DATA;
		}
		else {
			if (print < 2)	 {
				dipa_feedback = DIPA_NACK;
			}
			else {
				dipa_feedback = DIPA_ACK;
				
				if (sync_data.feedback == DIPA_ACK)
					dipa_good_feedback ();
				else
					dipa_bad_feedback ();
			}
		}
		
		/* Version 1 */
		//~ printf("%d, %d, %d, %u, %u, %u, %u\n",  node_id, 
			//~ get_my_hop(), get_rssi(), pkt_recvd, bad_crc, pkt_lost, 
			//~ ber_total);
		/* ***** */
		/* Version 2, 3 */
		printf("%d, %d, %d, %u, %u, %u, %u, %u, %u\n",  node_id, 
			get_my_hop(), get_rssi(), pkt_recvd, bad_crc, pkt_lost, 
			ber_total, sum_power, packets_sent);
		//~ printf ("%x %x %x %x %x %x %x %x %x %x\n", sync_data.run_time, sync_data.empty[0], sync_data.empty[1], sync_data.empty[2],
		//~ sync_data.empty[3], sync_data.empty[4], sync_data.empty[5], sync_data.empty[6], sync_data.empty[7], sync_data.feedback);
	}

	PROCESS_END();
}

/**
 * 
 */
void node_init() {
	
	run_time = 0;
	last_time = run_time;
		
	if (IS_SINK()) {
		sync_data.run_time = 0;
	}
}

/**
 * 
 */
rtimer_clock_t prepare_sync_packet(struct rtimer *t) {
	
	leds_on(LEDS_GREEN);
		
	rtimer_clock_t t_stop;
			
	if (IS_SINK()) {
		t_stop = RTIMER_TIME(t) + GLOSSY_DURATION;
		FLOODING_ROLE = GLOSSY_INITIATOR;
		REF_TIME = RTIMER_TIME(t);
	} else {
		if (GLOSSY_IS_BOOTSTRAPPING()) {
			t_stop = RTIMER_TIME(t) + GLOSSY_INIT_DURATION;
		} else {
			t_stop = RTIMER_TIME(t) + GLOSSY_DURATION;
		}
		FLOODING_ROLE = GLOSSY_RECEIVER;	
	}
	
	return t_stop;
}

/**
 * 
 */
void process_sync_packet(struct rtimer *t, void *ptr) {
	
	leds_off(LEDS_GREEN);
	
	if(IS_SINK()) {
		if (!GLOSSY_IS_BOOTSTRAPPING()) {
			// Glossy has already successfully bootstrapped.
			if (!GLOSSY_IS_SYNCED()) {
				// The reference time was not updated: increment reference time by GLOSSY_PERIOD.
				set_t_ref_l(GLOSSY_REFERENCE_TIME + GLOSSY_PERIOD);
				set_t_ref_l_updated(1);
			}
		}
		
		// Estimate the clock skew over the last period.
		estimate_period_skew();
		
		rtimer_set(t, REF_TIME + GLOSSY_PERIOD, 1, (rtimer_callback_t)tailored_glossy_scheduler, ptr);
		sync_data.run_time = ++run_time;
		process_poll(&tailored_glossy_print_process);
	}
	else {
		if (GLOSSY_IS_BOOTSTRAPPING()) {
			// Glossy is still bootstrapping.
			if (!GLOSSY_IS_SYNCED()) {
				// The reference time was not updated: reset skew_estimated to zero.
				skew_estimated = 0;
			}
		} else {
			// Glossy has already successfully bootstrapped.
			if (!GLOSSY_IS_SYNCED()) {
				// The reference time was not updated:
				// increment reference time by GLOSSY_PERIOD + period_skew.
				set_t_ref_l(GLOSSY_REFERENCE_TIME + GLOSSY_PERIOD + period_skew);
				set_t_ref_l_updated(1);
				// Increment sync_missed.
				sync_missed++;
			} else {
				// The reference time was not updated: reset sync_missed to zero.
				sync_missed = 0;
			}
		}
		// Estimate the clock skew over the last period.
		estimate_period_skew();
		if (GLOSSY_IS_BOOTSTRAPPING()) {
			// Glossy is still bootstrapping.
			if (skew_estimated == 0) {
				rtimer_set(t, RTIMER_TIME(t) + GLOSSY_INIT_PERIOD, 1,
						(rtimer_callback_t)tailored_glossy_scheduler, ptr);
			} else {
				REF_TIME = GLOSSY_REFERENCE_TIME + GLOSSY_PERIOD;
				rtimer_set(t, REF_TIME - GLOSSY_INIT_GUARD_TIME, 1,
							(rtimer_callback_t)tailored_glossy_scheduler, ptr);
			}
		} else {
			offset_err = GLOSSY_REFERENCE_TIME - (rtimer_clock_t)REF_TIME;
			REF_TIME   = GLOSSY_REFERENCE_TIME + GLOSSY_PERIOD + GLOSSY_GUARD_TIME * (-1);
			rtimer_set(t, REF_TIME, 1, (rtimer_callback_t)tailored_glossy_scheduler, ptr);
			
			if(get_rx_cnt()) {
				run_time = sync_data.run_time;
			} else {
				run_time++;
			}
			
			hop_to_sink = get_my_hop();
			
			process_poll(&tailored_glossy_print_process);
		}
	}
}


/** @} */

/**
 * \defgroup glossy-test-scheduler Periodic scheduling
 * @{
 */

char tailored_glossy_scheduler(struct rtimer *t, void *ptr) {
	
	PT_BEGIN(&pt);

	node_init();

	uint8_t src, dst;
	int8_t TX_COUNT;
	
	while (1) {
		
		/* prepare the sync packet to decide the superframe structure */
		rtimer_clock_t t_stop = prepare_sync_packet(t);
		
		if(run_time < STABILIZATION_PERIOD) {
			TX_COUNT = N_TX;
			set_tx_power (31);
		} else {
			TX_COUNT = 1;
			
			if (IS_SINK()) {
				set_tx_power (31);
			}
			else {
				set_tx_power(power_level[power]);
				packets_sent++;
				sum_power += power_value[power];
			}
		}
		
		
		/* every node participate (using flooding) in every sync slot */
		tailored_glossy_start((uint8_t *)&sync_data, SYNC_LEN, FLOODING_ROLE, GLOSSY_SYNC, TX_COUNT,
					APPLICATION_HEADER, t_stop, (rtimer_callback_t)tailored_glossy_scheduler, t, ptr);
		PT_YIELD(&pt);
		tailored_glossy_stop(&src, &dst);
		
		/* process the sync packet and schedule the next radio activity */
		process_sync_packet(t, ptr);		
		
		if (IS_SINK()) 
		{
			uint8_t i;
			for (i = 0; i < EMPTY_LEN; i++)
				sync_data.empty[i] = EMPTY_DATA;
			sync_data.feedback = EMPTY_DATA;		
			
		}
		/* Yield the protothread. */
		PT_YIELD(&pt);
	}
	
	PT_END(&pt);
}
	

PROCESS(tailored_glossy_process, "Glossy Process");
PROCESS_THREAD(tailored_glossy_process, ev, data)
{
	static struct etimer et;
	uint8_t i = 0;
	
	PROCESS_BEGIN();
	
	/* Allow some time for the network to settle. */
	etimer_set(&et, 30 * CLOCK_SECOND);
	PROCESS_WAIT_UNTIL(etimer_expired(&et));
	
	printf("node_id: %d; packet_size: %d\n", node_id, (EMPTY_LEN+8));
	for (i = 0; i < EMPTY_LEN; i++)
		sync_data.empty[i] = EMPTY_DATA;
	sync_data.feedback = EMPTY_DATA;
		
	if(IS_SINK()) {
		sync_data.run_time = 0;
	} else {
		leds_on(LEDS_RED);
	}
	
	// Start print stats processes.
	process_start(&tailored_glossy_print_process, NULL);
	// Start Glossy busy-waiting process.
	process_start(&glossy_process, NULL);
	// Start Glossy experiment in one second.
	rtimer_set(&rt, RTIMER_NOW() + RTIMER_SECOND, 1, (rtimer_callback_t)tailored_glossy_scheduler, NULL);
	
	PROCESS_END();
}

/** @} */
/** @} */
/** @} */
