/*****************************************************************************
 * Project         QNX driver
 *
 * (c) copyright   2002-2006
 * Company         Harman/Becker Automotive Systems GmbH
 *                 All rights reserved
 *****************************************************************************/
/**
 * @file          types.h
 * @ingroup       utilities
 * @author        Thorben Link
 *
 * Global type declarations.
 */

#ifndef __TYPES_H
#define __TYPES_H

#ifdef __QNXNTO__
/*---------------------------------------------------------------------
 * INCLUDES
 *--------------------------------------------------------------------*/
#include <inttypes.h>
#include <sys/syspage.h>
#include <sys/neutrino.h>
#include <time.h>


/*---------------------------------------------------------------------
 * DEFINES
 *--------------------------------------------------------------------*/
#define HB(w)        (((w)>>8) & 0xFF) /**< macro to get the high byte of the given word w */
#define LB(w)        ((w) & 0xFF)      /**< macro to get the low byte of the given word w */

#define MIN(a, b)    ((a > b) ? b:a) /**< macro to get the min value from a or b */
#define MAX(a, b)    ((a > b) ? a:b) /**< macro to get the max value from a or b */

#ifndef		FALSE
   #define	FALSE	0 /**< global false declaration */
#endif
#ifndef		TRUE
   #define	TRUE	1 /**< global true declaration */
#endif

#define CLOCK_GET(t)             (void)clock_gettime(CLOCK_REALTIME, &t)
#define CLOCK_DIFF_MS(s, e)      ((UInt32)(e.tv_sec - s.tv_sec)*(UInt32)1000 + \
                                  (UInt32)(e.tv_nsec - s.tv_nsec)/(UInt32)1000000)
#define CLOCK_DIFF_US(s, e)      ((UInt32)(e.tv_sec - s.tv_sec)*(UInt32)1000000 + \
                                  (UInt32)(e.tv_nsec - s.tv_nsec)/(UInt32)1000)
#define CLOCK_DIFF_NS(s, e)      ((UInt32)(e.tv_sec - s.tv_sec)*(UInt32)1000000000 + \
                                  (UInt32)(e.tv_nsec - s.tv_nsec))

#ifndef CLOCKCYCLES_INCR_BIT
	#if defined(__SH__)
      #define CLOCKCYCLES_INCR_BIT 32
   #else
      #define CLOCKCYCLES_INCR_BIT 0
   #endif
#endif

#define CLOCKCYCLES_GET()        ((UInt32)(ClockCycles() >> CLOCKCYCLES_INCR_BIT))
#define CLOCKCYCLES_GET_MS(t)    ((UInt32)((t) / ((SYSPAGE_ENTRY(qtime)->cycles_per_sec >> CLOCKCYCLES_INCR_BIT) / 1000)))
#define CLOCKCYCLES_GET_US(t)    ((UInt32)((t) / ((SYSPAGE_ENTRY(qtime)->cycles_per_sec >> CLOCKCYCLES_INCR_BIT) / 1000000)))
#define CLOCKCYCLES_GET_NS(t)    ((UInt32)((t) / ((SYSPAGE_ENTRY(qtime)->cycles_per_sec >> CLOCKCYCLES_INCR_BIT) / 1000000000)))
#define CLOCKCYCLES_DIFF(s)      (CLOCKCYCLES_GET() - s)
#define CLOCKCYCLES_DIFF2(s, e)  (e - s)

#else
/* Linux Related for now */
#include <linux/types.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/module.h>


#endif

/*---------------------------------------------------------------------
 * TYPE DEFINITIONS
 *--------------------------------------------------------------------*/
typedef int8_t   Int8;   /**< Signed 8-bit type */
typedef uint8_t  UInt8;  /**< Unsigned 8-bit type */
typedef int16_t  Int16;  /**< Signed 16-bit type */
typedef uint16_t UInt16; /**< Unsigned 16-bit type */
typedef int32_t  Int32;  /**< Signed 32-bit type */
typedef uint32_t UInt32; /**< Unsigned 32-bit type */
typedef int64_t  Int64;  /**< Signed 64-bit type */
typedef uint64_t UInt64; /**< Unsigned 64-bit type */

#endif /* __TYPES_H */
