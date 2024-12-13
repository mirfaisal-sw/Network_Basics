/*****************************************************************************
 * Project              HARMAN Connected Car System
 * (c) copyright        2021
 * Company              Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                log.h
 * @ingroup             Kernel and Drivers
 * @author              Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * This header file contains the definition of MACROS used for logging.
 */

#ifndef LOG_H
#define LOG_H


/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <stdio.h>
#include <inttypes.h>

#define	LOG_ENABLED


#if	defined(DLT_LOGGING)
#include <dlt/dlt.h>
extern DltContext dlt_ctx;

#elif	defined(SYSLOG_LOGGING)
#include <syslog.h>

#elif	defined(ANDROID_LOGGING)
#include <android/log.h>

#elif	defined(QNX_SLOG2_LOGGING)
#include <sys/slog2.h>
extern slog2_buffer_set_config_t slog_config;
extern slog2_buffer_t slog_handle;

#elif	defined(QNX_SLOG_LOGGING)
#include <sys/slog.h>
#include <sys/slogcodes.h>

#endif


/*****************************************************************************
 * DEFINES
 *****************************************************************************/


typedef	enum {
	/* Shut down the system NOW. eg: for OEM use */
	TRACE_LEVEL_SHUTDOWN = 1,
	/* Unexpected unrecoverable error. eg: hard disk error */
	TRACE_LEVEL_CRITICAL,
	/* Unexpected recoverable error. eg: needed to reset a hw controller */
	TRACE_LEVEL_ERROR,
	/* Expected error. eg: parity error on a serial port */
	TRACE_LEVEL_WARNING,
	/* Warnings. eg: Out of paper */
	TRACE_LEVEL_NOTICE,
	/* Information. eg: Printing page 3 */
	TRACE_LEVEL_INFO,
	/* Debug messages eg: Normal detail */
	TRACE_LEVEL_DEBUG1,
	/* Debug messages eg: Fine detail */
	TRACE_LEVEL_DEBUG2
} logging_t;

extern logging_t log_level;


#ifdef	LOG_ENABLED

#if	defined(DLT_LOGGING)
#define dbg_print(level, args...)                                              \
        do {                                                                   \
                char logBuffer[256];                                           \
                snprintf(logBuffer, 256, "UART_TEST: "args);                   \
                DLT_LOG(dlt_ctx, level, DLT_STRING(logBuffer));                \
        } while (0)

#define log_error(args...)              if (TRACE_LEVEL_ERROR <= log_level)    \
                                                dbg_print(DLT_LOG_ERROR, args)
#define log_warning(args...)            if (TRACE_LEVEL_WARNING <= log_level)  \
                                                dbg_print(DLT_LOG_WARN, args)
#define log_notice(args...)             if (TRACE_LEVEL_NOTICE <= log_level)   \
                                                dbg_print(DLT_LOG_WARN, args)
#define log_info(args...)               if (TRACE_LEVEL_INFO <= log_level)     \
                                                dbg_print(DLT_LOG_INFO, args)
#define log_debug(args...)              if (TRACE_LEVEL_DEBUG1 <= log_level)   \
                                                dbg_print(DLT_LOG_DEBUG, args)
#define log_debug2(args...)             if (TRACE_LEVEL_DEBUG2 <= log_level)   \
                                                dbg_print(DLT_LOG_VERBOSE, args)

#elif	defined(SYSLOG_LOGGING)
#define dbg_print(level, args...)                                              \
        do {                                                                   \
                syslog(level, "UART_TEST: "args);                              \
        } while (0)

#define log_error(args...)              if (TRACE_LEVEL_ERROR <= log_level)    \
                                                dbg_print(LOG_ERR, args)
#define log_warning(args...)            if (TRACE_LEVEL_WARNING <= log_level)  \
                                                dbg_print(LOG_WARNING, args)
#define log_notice(args...)             if (TRACE_LEVEL_NOTICE <= log_level)   \
                                                dbg_print(LOG_NOTICE, args)
#define log_info(args...)               if (TRACE_LEVEL_INFO <= log_level)     \
                                                dbg_print(LOG_INFO, args)
#define log_debug(args...)              if (TRACE_LEVEL_DEBUG1 <= log_level)   \
                                                dbg_print(LOG_DEBUG, args)
#define log_debug2(args...)             if (TRACE_LEVEL_DEBUG2 <= log_level)   \
                                                dbg_print(LOG_DEBUG, args)

#elif	defined(ANDROID_LOGGING)
#define dbg_print(level, args...)                                              \
        do {                                                                   \
                __android_log_print(level, "UART_TEST", "UART_TEST: "args);    \
        } while (0)

#define log_error(args...)              if (TRACE_LEVEL_ERROR <= log_level)    \
                                                dbg_print(ANDROID_LOG_ERROR, args)
#define log_warning(args...)            if (TRACE_LEVEL_WARNING <= log_level)  \
                                                dbg_print(ANDROID_LOG_WARN, args)
#define log_notice(args...)             if (TRACE_LEVEL_NOTICE <= log_level)   \
                                                dbg_print(ANDROID_LOG_WARN, args)
#define log_info(args...)               if (TRACE_LEVEL_INFO <= log_level)     \
                                                dbg_print(ANDROID_LOG_INFO, args)
#define log_debug(args...)              if (TRACE_LEVEL_DEBUG1 <= log_level)   \
                                                dbg_print(ANDROID_LOG_DEBUG, args)
#define log_debug2(args...)             if (TRACE_LEVEL_DEBUG2 <= log_level)   \
                                                dbg_print(ANDROID_LOG_VERBOSE, args)

#elif	defined(QNX_SLOG2_LOGGING)
#define dbg_print(level, args...)                                              \
        do {                                                                   \
                slog2f(slog_handle, 0, level, "UART_TEST: "args);              \
        } while (0)

#define log_error(args...)              if (TRACE_LEVEL_ERROR <= log_level)    \
                                                dbg_print(SLOG2_ERROR, args)
#define log_warning(args...)            if (TRACE_LEVEL_WARNING <= log_level)  \
                                                dbg_print(SLOG2_WARNING, args)
#define log_notice(args...)             if (TRACE_LEVEL_NOTICE <= log_level)   \
                                                dbg_print(SLOG2_NOTICE, args)
#define log_info(args...)               if (TRACE_LEVEL_INFO <= log_level)     \
                                                dbg_print(SLOG2_INFO, args)
#define log_debug(args...)              if (TRACE_LEVEL_DEBUG1 <= log_level)   \
                                                dbg_print(SLOG2_DEBUG1, args)
#define log_debug2(args...)             if (TRACE_LEVEL_DEBUG2 <= log_level)   \
                                                dbg_print(SLOG2_DEBUG2, args)

#elif	defined(QNX_SLOG_LOGGING)
#define dbg_print(level, args...)                                              \
        do {                                                                   \
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), level, "UART_TEST: "args);\
        } while (0)

#define log_error(args...)              if (TRACE_LEVEL_ERROR <= log_level)    \
                                                dbg_print(_SLOG_ERROR, args)
#define log_warning(args...)            if (TRACE_LEVEL_WARNING <= log_level)  \
                                                dbg_print(_SLOG_WARNING, args)
#define log_notice(args...)             if (TRACE_LEVEL_NOTICE <= log_level)   \
                                                dbg_print(_SLOG_NOTICE, args)
#define log_info(args...)               if (TRACE_LEVEL_INFO <= log_level)     \
                                                dbg_print(_SLOG_INFO, args)
#define log_debug(args...)              if (TRACE_LEVEL_DEBUG1 <= log_level)   \
                                                dbg_print(_SLOG_DEBUG1, args)
#define log_debug2(args...)             if (TRACE_LEVEL_DEBUG2 <= log_level)   \
                                                dbg_print(_SLOG_DEBUG2, args)

#else
#define dbg_print(args...)                                                     \
        do {                                                                   \
                printf("UART_TEST: "args);                                     \
                printf("\n");                                                  \
        } while (0)

#define log_error(args...)              if (TRACE_LEVEL_ERROR <= log_level)    \
                                                dbg_print(args)
#define log_warning(args...)            if (TRACE_LEVEL_WARNING <= log_level)  \
                                                dbg_print(args)
#define log_notice(args...)             if (TRACE_LEVEL_NOTICE <= log_level)   \
                                                dbg_print(args)
#define log_info(args...)               if (TRACE_LEVEL_INFO <= log_level)     \
                                                dbg_print(args)
#define log_debug(args...)              if (TRACE_LEVEL_DEBUG1 <= log_level)   \
                                                dbg_print(args)
#define log_debug2(args...)             if (TRACE_LEVEL_DEBUG2 <= log_level)   \
                                                dbg_print(args)
#endif

#else   //#ifdef LOG_ENABLED
#define log_error(args...)
#define log_warning(args...)
#define log_notice(args...)
#define log_info(args...)
#define log_debug(args...)
#define log_debug2(args...)
#endif   //#ifdef LOG_ENABLED


/*****************************************************************************
 * STRUCTURES
 *****************************************************************************/


/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/


#endif /* LOG_H */
