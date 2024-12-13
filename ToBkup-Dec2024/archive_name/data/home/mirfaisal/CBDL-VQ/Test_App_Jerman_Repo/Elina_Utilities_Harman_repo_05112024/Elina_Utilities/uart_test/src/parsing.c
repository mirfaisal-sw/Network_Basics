/*****************************************************************************
 * Project              HARMAN Connected Car System
 * (c) copyright        2021
 * Company              Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                parsing.c
 * @ingroup             Kernel and Drivers
 * @author              Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * This file contains the option parsing routines.
 */

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <inttypes.h>
#include <assert.h>

#include "log.h"
#include "uart_test.h"
#include "parsing.h"


/*****************************************************************************
 * DEFINES
 *****************************************************************************/


/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

static inline void print_usage(const char *prog);
static inline int str_to_uint(char *str, unsigned int *val);
static inline int str_to_hexbyte(char *str, uint8_t *val);


/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

extern logging_t log_level;

static int gargc;
static char **gargv;


/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/

/**
 * Print the usage/help message.
 *
 * @param  prog - program name
 * @return none
 */
static inline void print_usage(const char *prog)
{
	assert(prog != NULL);

	printf("Usage: %s  [OPTION]...\nTest UART (Version %s)\n\nOptions:\n"
	       "  -n <name>    UART device name (default: %s)\n"
	       "  -s <speed>   Set UART speed/baud rate in bps (default: %d)\n"
	       "  -m <mode>    Set mode:  0: read-only; read inputted number "
	       "of packets\n"
	       "                          1: write-only; write inputted number"
	       " of packets\n"
	       "                          2: write-read; continuously write-"
	       "read; should be used for loopback test\n"
	       "                          (default: %u)\n"
	       "  -p <packets> Number of packets to transfer in read-only or "
	       "write-only mode; 0 for continuous modes (default: %u packets)\n"
	       "  -l <length>  Length of a packet in bytes (default: %u)\n"
	       "  -d <delay>   Delay in usec after which a packet will be "
	       "transmitted; applicable for write operations only (default: %u "
	       "usec)\n"
	       "  -v           Increase the verbosity of logs; accepted multiple"
	       " times to increase verbosity further (min: %d, max: %d, default:"
	       " %d)\n"
	       "  -h           Print this help message\n"
	       "  <hexdata>    Data to be written; enter data byte by byte in"
	       " HEX\n(default: [0x0...0x0] [0x1...0x0] [0x2...0x0] ... [0xFF..."
	       "0x0] [0x0...0x0] ...)\n", prog, UT_VERSION,
	       DEFAULT_UART_DEVICE_NAME, DEFAULT_SPEED, DEFAULT_MODE,
	       DEFAULT_NUM_PACKETS, DEFAULT_DATA_LEN, DEFAULT_DELAY,
	       TRACE_LEVEL_SHUTDOWN, TRACE_LEVEL_DEBUG2, log_level
	      );

	exit(EXIT_SUCCESS);
}

/**
 * Converts input string to hex byte value.
 *
 * @param  str - pointer to input string
 * @param  val - pointer to hold the converted value
 * @return EOK - On Success, -1 - On failure
 */
static inline int str_to_hexbyte(char *str, uint8_t *val)
{
	unsigned long ret;
	char *endptr = NULL;

	assert((str != NULL) && (val != NULL));

	errno = 0;
	ret = strtoul(str, &endptr, 16);
	if (((errno == ERANGE) && (ret == ULONG_MAX || ret == 0)) ||
		((errno != 0) && (ret == 0)))
		return -1;
	if (endptr == str) {
		errno = EINVAL;
		return -1;
	}

	if (ret > UINT8_MAX) {
		errno = ERANGE;
		return -1;
	}

	*val = (uint8_t)ret;

	return EOK;
}

/**
 * Converts input string to uin32_t value.
 *
 * @param  str - pointer to input string
 * @param  val - pointer to hold the converted value
 * @return EOK - On Success, -1 - On failure
 */
static inline int str_to_uint(char *str, unsigned int *val)
{
	unsigned long ret;
	char *endptr = NULL;

	assert((str != NULL) && (val != NULL));

	errno = 0;
	ret = strtoul(str, &endptr, 0);
	if (((errno == ERANGE) && (ret == ULONG_MAX || ret == 0)) ||
		((errno != 0) && (ret == 0)))
		return -1;
	if (endptr == str) {
		errno = EINVAL;
		return -1;
	}

	if (ret > UINT_MAX) {
		errno = ERANGE;
		return -1;
	}

	*val = (unsigned int)ret;

	return EOK;
}

/**
 * Parses the options.
 *
 * @param  argc - number of parameters
 * @param  argv - pointer to array of parameters
 * @param  argsp - pointer to argument structure
 * @return EOK - On Success, -1 - On failure
 */
int opt_parse(int argc, char *argv[], args_t * argsp)
{
	int c, ret;
	unsigned int len;

	assert((argv != NULL) && (argsp != NULL));

	gargc = argc;
	gargv = argv;

	while (1) {
		c = getopt(argc, argv, "n:s:d:m:p:l:vh");
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'n':
			len = (unsigned int)strnlen(optarg, MAX_PATH_LEN);
			if ((len == 0) || (len == (MAX_PATH_LEN))) {
				log_error("%s: Path is of invalid length(%s)",
					__func__, (len == 0) ? "0" : ">= "
					STRINGIFY(MAX_PATH_LEN));
				return -1;
			}
			argsp->name = calloc(1, len+1);
			if (argsp->name == NULL) {
				log_error("%s: Error allocating memory for path"
					"name: %s", __func__, strerror(errno));
				return -1;
			}
			snprintf(argsp->name, len+1, "%s", optarg);
			break;

		case 's':
			ret = str_to_uint(optarg, &(argsp->speed));
			if (ret != EOK) {
				log_error("%s: Incorrect value(%s) for speed"
					" inputted: %s", __func__, optarg,
					strerror(errno));
				return -1;
			}
			break;

		case 'm':
			ret = str_to_uint(optarg, &(argsp->mode));
			if (ret != EOK) {
				log_error("%s: Incorrect value(%s) for mode"
					" inputted: %s", __func__, optarg,
					strerror(errno));
				return -1;
			}
			break;

		case 'p':
			ret = str_to_uint(optarg, &(argsp->packets));
			if (ret != EOK) {
				log_error("%s: Incorrect value(%s) for packets"
					" inputted: %s", __func__, optarg,
					strerror(errno));
				return -1;
			}
			break;

		case 'l':
			ret = str_to_uint(optarg, &(argsp->len));
			if (ret != EOK) {
				log_error("%s: Incorrect value(%s) for length"
					" inputted: %s", __func__, optarg,
					strerror(errno));
				return -1;
			}
			if (argsp->len == 0) {
				log_error("%s: Invalid packet length(0) specified;"
					" Length should be greater than 0",
					__func__);
				return -1;
			}
			break;

		case 'd':
			ret = str_to_uint(optarg, &(argsp->delay));
			if (ret != EOK) {
				log_error("%s: Incorrect value(%s) for delay"
					" inputted: %s", __func__, optarg,
					strerror(errno));
				return -1;
			}
			break;

		case 'v':
			(log_level < TRACE_LEVEL_DEBUG2) ? log_level++ : log_level;
			break;

		default:
			printf("Invalid parameter : %d\n", c);
			/* fall through - no break to print help message */

		case 'h':
			print_usage(argv[0]);
		}
	}

	if (argsp->name == NULL) {
		argsp->name = calloc(1, MAX_PATH_LEN);
		if (argsp->name == NULL) {
			log_error("%s: Error allocating memory for pathname: %s",
				__func__, strerror(errno));
			return -1;
		}
		snprintf(argsp->name, MAX_PATH_LEN, "%s", DEFAULT_UART_DEVICE_NAME);
	}

	return EOK;
}

/**
 * Initializes the write buffer.
 *
 * @param  buf - pointer to write buffer
 * @param  packets - no. of packets
 * @param  len - length of each packet
 * @return EOK - On Success, -1 - On failure
 */
int wbuffer_init(uint8_t *buf, int packets, int len)
{
	int i, j, ret;

	assert((buf != NULL));

	memset((void *)buf, 0, packets*len);
	if (gargv[optind] != NULL) {
		len = ((packets*len) > (gargc-optind)) ? (gargc-optind) :
			(packets*len);
		for (i = optind, j = 0; j < len; i++, j++) {
			ret = str_to_hexbyte(gargv[i], &(buf[j]));
			if (ret != EOK) {
				log_error("%s: Incorrect value(%s) for %d hexdata"
					" inputted: %s", __func__, optarg, i,
					strerror(errno));
				return -1;
			}
		}
	} else {
		for (i = j = 0; j < (len * packets); j+= len, i++) {
			buf[j] = i;
		}
	}

	return EOK;
}
