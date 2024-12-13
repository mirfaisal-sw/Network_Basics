/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         log.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <David Rogala@harman.com>
 *
 * This header file contains the definition for MACROS used for logging
 *****************************************************************************/

#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <linux/kernel.h>

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define LOG_NONE_LVL                    (0)
#define LOG_ERROR_LVL                   (3)
#define LOG_WARNING_LVL                 (4)
#define LOG_NOTICE_LVL                  (5)
#define LOG_INFO_LVL                    (6)
#define LOG_DEBUG_LVL                   (7)
#define LOG_DEFAULT_LVL                 (LOG_ERROR_LVL)

/* Logging */
#define log_debug(fmt, arg...)                                                \
	do {                                                                  \
		if (gIpcLogLevel >= LOG_DEBUG_LVL) {                          \
			pr_debug("[ IPC:%-20.20s: %-1.1d ] [DBG] " fmt "\n",  \
			__func__, __LINE__, ##arg);                           \
		}                                                             \
	} while (0)
#define log_error(fmt, arg...)                                                \
	do {                                                                  \
		if (gIpcLogLevel >= LOG_ERROR_LVL) {                          \
			pr_err("[ IPC:%-20.20s: %-1.1d ] [ERR] " fmt "\n",    \
			__func__, __LINE__, ##arg);                           \
		}                                                             \
	} while (0)
#define log_info(fmt, arg...)                                                 \
	do {                                                                  \
		if (gIpcLogLevel >= LOG_INFO_LVL) {                           \
			pr_info("[ IPC:%-20.20s: %-1.1d ] [INF] " fmt "\n",   \
			__func__, __LINE__, ##arg);                           \
		}                                                             \
	} while (0)
#define log_notice(fmt, arg...)                                               \
	do {                                                                  \
		if (gIpcLogLevel >= LOG_NOTICE_LVL) {                         \
			pr_notice("[ IPC:%-20.20s: %-1.1d ] [NTC] " fmt "\n", \
			__func__, __LINE__, ##arg);                           \
		}                                                             \
	} while (0)
#define log_warning(fmt, arg...)                                              \
	do {                                                                  \
		if (gIpcLogLevel >= LOG_WARNING_LVL) {                        \
			pr_warn("[ IPC:%-20.20s: %-1.1d ] [WRN] " fmt "\n",   \
			__func__, __LINE__, ##arg);                           \
		}                                                             \
	} while (0)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
extern int gIpcLogLevel;

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
void printData(char *str, int32_t slen, const uint8_t *data, int32_t dlen,
		int32_t max);

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
