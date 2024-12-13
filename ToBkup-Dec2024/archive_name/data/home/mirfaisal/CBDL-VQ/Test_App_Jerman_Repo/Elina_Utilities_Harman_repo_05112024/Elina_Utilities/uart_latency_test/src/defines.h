/*****************************************************************************
 * Project              HARMAN connected Car Systems
 *
 * c) copyright         2020
 * Company              Harman International Industries, Incorporated
 *                      All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                defines.h
 * @author              Mir Faisal <Mir.Faisal@harman.com>
 * @ingroup             Kernel and Drivers
 *
 * This header file contains the declarations of serial read/write APIs.
 */

/*****************************************************************************
 * Defines
 *****************************************************************************/

#ifndef DEFINES_H
#define DEFINES_H

/* Name of package */
#define PACKAGE 			"serial-latency-test"

/* Define to the full name of this package. */
#define PACKAGE_NAME 			"serial-latency-test"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING 			"serial-latency-test 1.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME 		"serial-latency-test"

/* Define to the home page for this package. */
#define PACKAGE_URL 			""

/* Define to the version of this package. */
#define PACKAGE_VERSION 		"1.0"

/* Version number of package */
#define VERSION				"1.0"
#define RELEASE_DATE			"28-Jul-2022"

#ifndef INFINITY
#define INFINITY                	(1.0/0.0)
#endif

#define DEBUG                       	1

#define HISTLEN  			10
#define TERMWIDTH               	50

#ifndef SQUARE
#define SQUARE(a)               	( (a) * (a) )
#endif

#ifndef MIN
#define MIN(a,b)                	( (a) < (b) ? (a) : (b) )
#endif

#ifndef MAX
#define MAX(a,b)                	( (a) > (b) ? (a) : (b) )
#endif

#ifndef RAIL
#define RAIL(v, min, max)   		(MIN((max), MAX((min), (v))))
#endif

#endif // DEFINES_H
