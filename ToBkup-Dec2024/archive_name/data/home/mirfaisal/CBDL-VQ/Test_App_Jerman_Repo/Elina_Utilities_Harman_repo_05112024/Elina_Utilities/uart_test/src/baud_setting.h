/*****************************************************************************
 * Project              HARMAN Connected Car System
 * (c) copyright        2021
 * Company              Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                baud_setting.h
 * @ingroup             Kernel and Drivers
 * @author              Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * This file contains the routines to convert & set baud rate.
 */

#ifndef BAUD_SETTING_H
#define BAUD_SETTING_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/


#ifndef	QNX_ENABLED

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

int set_custom_baud_rate(args_t * argsp);
#endif	//#ifndef	QNX_ENABLED


#endif /* BAUD_SETTING_H */
