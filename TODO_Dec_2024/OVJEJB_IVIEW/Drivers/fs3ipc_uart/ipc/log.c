/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         log.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <David Rogala@harman.com>
 *
 * This module contains helper logic and global variables used to configure
 * logging levels
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <linux/types.h>
#include <linux/string.h>

#include "log.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
/* Global logging level */
int gIpcLogLevel = LOG_DEFAULT_LVL;

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
/**
 * printData() - Converts a byte array to a hex string.
 * @str: Output string.
 * @slen: Length of output string.
 * @data: Pointer to data array.
 * @dlen: Length of input data array.
 * @max: Maximum number of bytes to print.
 */
void printData(char *str, int32_t slen, const uint8_t *data, int32_t dlen, int32_t max)
{
	int32_t i = 0;
	int32_t ret;

	if (slen >= 1) {
		str[0] = '\0';

		while (slen > 0 && i < max && i < dlen) {
			ret = snprintf(str, slen, "%02X ", data[i++]);

			if (ret > 0) {
				str += ret;
				slen -= ret;
			} else {
				break;
			}
		}

		if (max < dlen)
			snprintf(str, slen, "...");
	}
}
