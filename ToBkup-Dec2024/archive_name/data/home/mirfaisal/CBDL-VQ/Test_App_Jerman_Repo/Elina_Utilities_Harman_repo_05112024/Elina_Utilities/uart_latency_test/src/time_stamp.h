/*****************************************************************************
 * Project              HARMAN connected Car Systems
 *
 * c) copyright         2020
 * Company              Harman International Industries, Incorporated
 *                      All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                time_stamp.h
 * @author              Mir Faisal <Mir.Faisal@harman.com>
 * @ingroup             Kernel and Drivers
 *
 * This header file contains the inline definition of time stamp functions.
 * Inline function is used to avoid error in timesatmp error due to function call
 * stack allocation and deallocation.
 */
#ifndef TIME_STAMP_H
#define TIME_STAMP_H

/*****************************************************************************
 * Includes
 *****************************************************************************/
#include <time.h>
#include "defines.h"

typedef struct timespec timerStruct_t;

/*****************************************************************************
 * Inline function definitions
 *****************************************************************************/
extern inline void GetHighResolutionTime(timerStruct_t *t);
extern inline void GetHighResolutionTime(timerStruct_t *t) {

	/* Please scheck URL - https://linux.die.net/man/2/clock_gettime for
	 * reference.
	 */
#if defined(CLOCK_MONOTONIC_RAW)
        clock_gettime(CLOCK_MONOTONIC_RAW, t);
#else
        clock_gettime(CLOCK_MONOTONIC, t);
#endif
}

extern inline double ConvertTimeDifferenceToSec(timerStruct_t *end, timerStruct_t *begin);
extern inline double ConvertTimeDifferenceToSec(timerStruct_t *end, timerStruct_t *begin) {

        timerStruct_t temp;

        if ((end->tv_nsec-begin->tv_nsec)<0) {
                temp.tv_sec = end->tv_sec-begin->tv_sec-1;
                temp.tv_nsec = 1000000000+end->tv_nsec-begin->tv_nsec;
        } else {
                temp.tv_sec = end->tv_sec-begin->tv_sec;
                temp.tv_nsec = end->tv_nsec-begin->tv_nsec;
        }

        return (temp.tv_sec) + (1e-9)*(temp.tv_nsec);
}

#endif // TIME_STAMP_H
