/*****************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2022
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_timer.h
 * @ingroup      FS3IPC Component
 * @author       Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * FS3IPC timer component header file.
 * This provides APIs of timer component.
 *****************************************************************************/

#ifndef FS3IPC_TIMER_H
#define FS3IPC_TIMER_H

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
struct fs3ipc_timer;

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
#ifdef FS3IPC_PERIODIC_TIMER_HANDLER

/**
 * fs3ipc_verify_timer_table_cfg() - Verify timer table configuration.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * Verify whether the timer table configuration is proper or not. This function
 * should be called during FS3IPC initialization after all the timers are
 * initialized.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK
 * * fs3ipc_StatusType_ErrorHandle - Invalid FS3IPC instance
 * * fs3ipc_StatusType_ErrorCfg    - Config issue
 */
fs3ipc_StatusType fs3ipc_verify_timer_table_cfg(uint8_t fs3ipc_instance);

/**
 * fs3ipc_periodic_timer_handler() - Periodic timer handler.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * This function should be called periodically. It will determine the timer
 * status and call the configured callback for all active timers which have
 * expired.
 *
 * Context: Acquires and releases the timer lock.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK
 * * fs3ipc_StatusType_ErrorHandle - Invalid FS3IPC instance
 */
fs3ipc_StatusType fs3ipc_periodic_timer_handler(uint8_t fs3ipc_instance);

#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */

/**
 * handle_timer_expiry() - Handle timer expiry.
 * @timer: FS3IPC timer.
 *
 * This function should be called when a timer expires. It will handle the timer
 * expiry and call the configured callback.
 *
 * It is assumed that the argument is valid.
 *
 * Context: (Interrupt based timers)Interrupt context.
 *          (Periodic timer handler)Expects the timer lock to be held by caller.
 *
 * Return: None.
 */
void handle_timer_expiry(struct fs3ipc_timer *timer);

/**
 * fs3ipc_init_timer() - Verify timer configuration & initialize the timer.
 * @timer: Timer to be initialized.
 * @fs3ipc_instance: FS3IPC instance.
 * @cb: Timer expiry callback.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_init_timer(struct fs3ipc_timer *timer,
				    uint8_t fs3ipc_instance,
				    fs3ipc_timer_callback_t cb);

/**
 * fs3ipc_restart_timer() - Restart timer.
 * @timer: Timer to be restarted.
 * @timeout: Timeout period in msec.
 *
 * Context: (Periodic timer handler)Acquires and releases the timer lock.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_restart_timer(struct fs3ipc_timer *timer,
				       uint32_t timeout);

/**
 * fs3ipc_stop_timer() - Stop timer.
 * @timer: Timer to be stopped.
 *
 * Context: (Periodic timer handler)Acquires and releases the timer lock.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_stop_timer(struct fs3ipc_timer *timer);

/**
 * fs3ipc_get_timer_instance() - Get FS3IPC instance of timer.
 * @timer: Timer whose FS3IPC instance is required.
 *
 * It is assumed that the argument is valid.
 *
 * Return: Timer FS3IPC instance
 */
uint8_t fs3ipc_get_timer_instance(const struct fs3ipc_timer *timer);

/**
 * fs3ipc_get_timer_name() - Get name of timer.
 * @timer: Timer whose name is required.
 *
 * It is assumed that the argument is valid.
 *
 * Return: Timer name
 */
const char *fs3ipc_get_timer_name(const struct fs3ipc_timer *timer);

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_TIMER_H */
