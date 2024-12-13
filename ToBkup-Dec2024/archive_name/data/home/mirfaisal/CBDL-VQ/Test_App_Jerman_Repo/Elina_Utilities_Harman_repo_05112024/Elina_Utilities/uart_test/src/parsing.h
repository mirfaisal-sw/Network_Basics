/*****************************************************************************
 * Project              HARMAN Connected Car System
 * (c) copyright        2021
 * Company              Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                parsing.h
 * @ingroup             Kernel and Drivers
 * @author              Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * This file contains the option parsing routines.
 */

#ifndef SRC_PARSING_H
#define SRC_PARSING_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <inttypes.h>
#include <assert.h>

#include "log.h"
#include "uart_test.h"


/*****************************************************************************
 * DEFINES
 *****************************************************************************/


/*****************************************************************************
 * STRUCTURES
 *****************************************************************************/


/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

int opt_parse(int argc, char *argv[], args_t * argsp);
int wbuffer_init(uint8_t *buf, int packets, int len);

/**
 * Initializes the default options.
 *
 * @param  argsp - pointer to argument structure
 * @return None
 */
static inline void opt_init(args_t * argsp)
{
	assert(argsp != NULL);

	memset(argsp, 0, sizeof(*argsp));
	argsp->fd = -1;
	argsp->speed = DEFAULT_SPEED;
	argsp->mode = DEFAULT_MODE;
	argsp->packets = DEFAULT_NUM_PACKETS;
	argsp->len = DEFAULT_DATA_LEN;
	argsp->delay = DEFAULT_DELAY;

	return;
}

/**
 * Print the configured options.
 *
 * @param  argsp - pointer to argument structure
 * @return None
 */
static inline void opt_print(args_t * argsp)
{
	assert(argsp != NULL);

	log_info("Version :- %s", UT_VERSION);
	log_info("Configured Options are :-");
	log_info("Device : %s", argsp->name);
	log_info("Speed(bps) : %u", argsp->speed);
	log_info("Mode : %u", argsp->mode);
	log_info("Number of packets : %u", argsp->packets);
	log_info("Packet length(Bytes) : %u", argsp->len);
	log_info("Delay(usec) : %u", argsp->delay);
	log_info("Verbosity(min: %d, max: %d) : %d", TRACE_LEVEL_SHUTDOWN,
		TRACE_LEVEL_DEBUG2, log_level);

	return;
}


#endif /* SRC_PARSING_H */
