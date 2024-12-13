/*****************************************************************************
 * Project              HARMAN Connected Car System
 * (c) copyright        2021
 * Company              Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                uart_test.h
 * @ingroup             Kernel and Drivers
 * @author              Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * This file contains the definition of MACROS.
 */

#ifndef UART_TEST_H
#define UART_TEST_H


/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>



/*****************************************************************************
 * DEFINES
 *****************************************************************************/

#define	UT_VERSION                      "1.1"

#ifndef	EOK
#define	EOK                             (0)
#endif

#define	MAX_PATH_LEN                    (64)

#define	kHZ                             (1000)
#define	MHZ                             (1000*1000)

#define	NSEC_PER_USEC                   (1000)
#define	NSEC_PER_MSEC                   (1000*NSEC_PER_USEC)
#define	NSEC_PER_SEC                    (1000*NSEC_PER_MSEC)
#define	USEC_PER_MSEC                   (1000)
#define	USEC_PER_SEC                    (1000*USEC_PER_MSEC)

enum {
	RD,
	WR,
	RW
};

#define	DEFAULT_UART_DEVICE_NAME        "/dev/ser2"
#define	DEFAULT_SPEED                   (1*MHZ)
#define	DEFAULT_NUM_PACKETS             (8)
#define	DEFAULT_DATA_LEN                (256)
#define	DEFAULT_MODE                    RW
#define	DEFAULT_DELAY                   (20000)

#define	_STRINGIFY(x)                   #x
#define	STRINGIFY(x)                    _STRINGIFY(x)



/*****************************************************************************
 * STRUCTURES
 *****************************************************************************/

typedef struct args_s
{
	uint8_t* rbuf;
	uint8_t* wbuf;
	char *name;
	int fd;
	unsigned int speed;
	unsigned int mode;
	unsigned int packets;
	unsigned int len;
	unsigned int delay;
} args_t;


/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

static inline long msec_to_nsec(unsigned int msec)
{
	return ((long)msec * NSEC_PER_MSEC);
}

static inline long usec_to_nsec(unsigned int usec)
{
	return ((long)usec * NSEC_PER_USEC);
}

static inline long usec_to_sec(unsigned int usec)
{
	return (long)(usec/USEC_PER_SEC);
}

static inline unsigned int sec_to_usec(unsigned int sec)
{
	return (sec * USEC_PER_SEC);
}


#endif /* UART_TEST_H */
