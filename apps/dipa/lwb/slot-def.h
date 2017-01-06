#ifndef SLOT_DEF_H_
#define SLOT_DEF_H_


/****** Application defined parameters that controls LWB process *****/

/**
 * Initial network cooloff period, default is 30 seconds
 */
#define COOLOFF_PERIOD				30

/**
 * Initial network stabilization period, default is 30 seconds
 */
#define STABILIZATION_PERIOD		30

/**
 * Maximum payload length of a LWB data packet
 */
#define MAX_PAYLOAD_LEN				40

/**
 * NodeId of the initiator.
 * Default value: 1
 */
#define SINK_NODE_ID       			31


/**************** SYSTEM parameters, change with care *****************/

/**
 * \brief Maximum number of times sync packets will be sent N.
 *        Default value: 5.
 */
#define N_TX                    	5

/**
 * \brief Period with which a Glossy phase is scheduled.
 *        Default value: 1000 ms.
 */
#define GLOSSY_PERIOD           (RTIMER_SECOND)         // 1000 ms

/**
 * \brief Duration of each Glossy phase.
 *        Default value: 15 ms.
 */
#define GLOSSY_DURATION      	(RTIMER_SECOND / 30)    // 33.33 ms

#define GLOSSY_SYNC_GUARD		GLOSSY_DURATION +(RTIMER_SECOND / 100)	// 43.33 ms


/**
 * \brief Guard-time at receivers for the sync packet.
 *        Default value: 2000 us.
 */
#define GLOSSY_GUARD_TIME       (RTIMER_SECOND / 125)   // 8 ms


/**
 * \brief Number of consecutive Glossy phases with successful computation of reference time required to exit from bootstrapping.
 *        Default value: 3.
 */
#define GLOSSY_BOOTSTRAP_PERIODS 3

/**
 * \brief Period during bootstrapping at receivers.
 *        It should not be an exact fraction of \link GLOSSY_PERIOD \endlink.
 *        Default value: 69.474 ms.
 */
#define GLOSSY_INIT_PERIOD      (GLOSSY_INIT_DURATION + RTIMER_SECOND / 100)                   //  69.474 ms

/**
 * \brief Duration during bootstrapping at receivers.
 *        Default value: 59.474 ms.
 */
#define GLOSSY_INIT_DURATION    (GLOSSY_DURATION - GLOSSY_GUARD_TIME + GLOSSY_INIT_GUARD_TIME) //  59.474 ms

/**
 * \brief Guard-time during bootstrapping at receivers.
 *        Default value: 50 ms.
 */
#define GLOSSY_INIT_GUARD_TIME  (RTIMER_SECOND / 20)                                           //  50 ms

/**
 * \brief Application-specific header.
 *        Default value: 0x0
 */
#define APPLICATION_HEADER      	0


/********** Do not chnage these until absolutely necessary ***********/

/** @} */

/**
 * \defgroup glossy-test-defines Application internal defines
 * @{
 */


/**
 * \brief Check if the nodeId matches the one of the initiator.
 */
#define IS_SINK()              (node_id == SINK_NODE_ID)

/**
 * \brief Check if Glossy is still bootstrapping.
 * \sa \link GLOSSY_BOOTSTRAP_PERIODS \endlink.
 */
#define GLOSSY_IS_BOOTSTRAPPING()   (skew_estimated < GLOSSY_BOOTSTRAP_PERIODS)

/**
 * \brief Check if Glossy is synchronized.
 *
 * The application assumes that a node is synchronized if it updated the reference time
 * during the last Glossy phase.
 * \sa \link is_t_ref_l_updated \endlink
 */
#define GLOSSY_IS_SYNCED()          (is_t_ref_l_updated())

/**
 * \brief Get Glossy reference time.
 * \sa \link get_t_ref_l \endlink
 */
#define GLOSSY_REFERENCE_TIME       (get_t_ref_l())

/** @} */

/** @} */

#endif /* SLOT_DEF_H_ */
