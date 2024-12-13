/*****************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2022
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 ****************************************************************/
/**
 * @file         fs3ipc_timer_priv.h
 * @ingroup      FS3IPC Component
 * @author       Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * FS3IPC timer component private header file.
 * This is meant to be used only in timer component's implementation &
 * configuration and should not be directly included. Instead fs3ipc_timer.h
 * should be included.
 */

#ifndef FS3IPC_TIMER_PRIV_H
#define FS3IPC_TIMER_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_types.h"
#include "fs3ipc_cfg.h"
#include "fs3ipc_os.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
#ifdef FS3IPC_PERIODIC_TIMER_HANDLER

/**
 * struct fs3ipc_timer_ctx - FS3IPC timer context
 * @is_active: Timer state.
 * @expiry_time: Expiry time.
 * @lock: Lock to protect timer operations.
 */
struct fs3ipc_timer_ctx {
	bool is_active;
	uint32_t expiry_time;
	fs3ipc_os_resource_t * const lock;
};

/**
 * struct fs3ipc_timer_table - Table of FS3IPC timers
 * @timer_count: Total number of timers in array.
 * @timers: Array of timers.
 */
struct fs3ipc_timer_table {
	const unsigned int timer_count;
	struct fs3ipc_timer * const timers;
};

#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */

/**
 * struct fs3ipc_timer - FS3IPC timer
 * @fs3ipc_instance: FS3IPC instance.
 * @name: Name of timer. Recommended style: "<name_or_usage>/<fs3ipc_instance_no>".
 *        For e.g.: "uframe_timer/0".
 * @callback: Callback called when timer expires.
 * @ctx: Context of FS3IPC timer.
 */
struct fs3ipc_timer {
	const uint8_t fs3ipc_instance;
	const char * const name;
	const fs3ipc_timer_callback_t callback;
	struct fs3ipc_timer_ctx ctx;
};

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
#ifdef FS3IPC_PERIODIC_TIMER_HANDLER
extern const struct fs3ipc_timer_table fs3ipc_timer_tables[];
#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_TIMER_PRIV_H */
