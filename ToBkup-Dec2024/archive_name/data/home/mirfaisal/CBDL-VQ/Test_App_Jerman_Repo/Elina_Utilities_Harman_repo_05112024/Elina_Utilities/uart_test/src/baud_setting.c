/*****************************************************************************
 * Project              HARMAN Connected Car System
 * (c) copyright        2021
 * Company              Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                baud_setting.c
 * @ingroup             Kernel and Drivers
 * @author              Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * This file contains the routines to convert & set baud rate.
 */

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#ifndef	QNX_ENABLED

#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <asm/ioctls.h>
#include <assert.h>

#include "uart_test.h"
#include "baud_setting.h"


/*****************************************************************************
 * DEFINES
 *****************************************************************************/


/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/


/*****************************************************************************
 * VARIABLES
 *****************************************************************************/


/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/

/**
 * Set custom baudrate speed
 *
 * @param argsp - pointer to argument structure
 * @return EOK on success, < 0 on failure
 */
int set_custom_baud_rate(args_t * argsp)
{
	int ret;
	struct termios2 options;

	assert(argsp != NULL);

	/* get the current settings of the serial port */
	ret = ioctl(argsp->fd, TCGETS2, &options);
	if (ret == -1) {
		return ret;
	}

	/* set the baud rate */
	options.c_cflag &= ~CBAUD;
	options.c_cflag |= BOTHER;
	options.c_ispeed = argsp->speed;
	options.c_ospeed = argsp->speed;

	/* set the new settings */
	ret = ioctl(argsp->fd, TCSETS2, &options);
	if (ret == -1) {
		return -2;
	}

	return EOK;
}

#endif	//#ifndef	QNX_ENABLED
