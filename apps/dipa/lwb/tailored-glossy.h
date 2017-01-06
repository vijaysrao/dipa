#ifndef TAILORED_TEST_H_
#define TAILORED_TEST_H_

#include <stdio.h>
#include <stdlib.h>

#include "contiki.h"

#include "glossy-core.h"
#include "slot-def.h"

#include "node-id.h"


/*********************************************************************************************/

PROCESS_NAME(tailored_glossy_process);

/*********************************************************************************************************/
/**----------various structures for various types of packets-----------**/

#define EMPTY_LEN_16			8
#define EMPTY_LEN_32			EMPTY_LEN_16 + 16
#define EMPTY_LEN_64			EMPTY_LEN_32 + 32
#define EMPTY_LEN_128			EMPTY_LEN_64 + 62

#define EMPTY_LEN				EMPTY_LEN_128

#define EMPTY_DATA  			0xFF

#define DIPA_ACK				0xFF
#define DIPA_NACK				0x00

/**
 * \brief Data structure used to send synchronization packet.
 */
typedef struct {
	uint16_t run_time;
	uint8_t empty[EMPTY_LEN];
	uint8_t feedback;
	//uint16_t sleep_slots;
} sync_data_struct;

/**
 * \brief Length of sync packet structure.
 */
#define SYNC_LEN                    sizeof(sync_data_struct)

/*********************************************************************************************/

/** @} */

/** @} */

#endif /* TAILORED_GLOSSY__H_ */
