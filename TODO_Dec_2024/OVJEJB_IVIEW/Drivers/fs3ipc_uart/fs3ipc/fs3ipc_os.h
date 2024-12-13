/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_os.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 *****************************************************************************/

#ifndef FS3IPC_OS_H
#define FS3IPC_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"

#if FS3IPC_OS_AUTOSAR == 1
#include "os.h"
#elif FS3IPC_OS_UCOSII == 1
#include "ucos_ii.h"
#elif FS3IPC_OS_LINUX_KERNEL == 1
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#ifdef FS3IPC_PERIODIC_TIMER_HANDLER
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#else /* !(FS3IPC_PERIODIC_TIMER_HANDLER) */
#include <linux/hrtimer.h>
#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */
#elif FS3IPC_OS_POSIX_US == 1
#else
#error "invalid OS config"
#endif

#include "fs3ipc_types.h"
#include "fs3ipc_timer.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
#ifndef FS3IPC_PERIODIC_TIMER_HANDLER
/* Forward declaration of FS3IPC timer context */
struct fs3ipc_timer_ctx;
#endif /* !(FS3IPC_PERIODIC_TIMER_HANDLER) */

/* data types which are customized per OS*/
#if FS3IPC_OS_AUTOSAR == 1

typedef TaskType fs3ipc_os_event_t;
typedef EventMaskType fs3ipc_os_EventMaskType;
typedef ResourceType fs3ipc_os_resource_t;

#elif FS3IPC_OS_UCOSII == 1
typedef OS_FLAG_GRP * fs3ipc_os_event_t;
typedef OS_FLAGS fs3ipc_os_EventMaskType; /*events to process*/
typedef OS_EVENT * fs3ipc_os_resource_t;
#elif FS3IPC_OS_LINUX_KERNEL == 1

typedef struct {
	uint32_t flags;
	spinlock_t lock;
	wait_queue_head_t wq;
} fs3ipc_os_event_t;

typedef uint32_t fs3ipc_os_EventMaskType;
typedef struct mutex fs3ipc_os_resource_t;

#define NSEC_TO_MSEC(nsec)              ((nsec) / (NSEC_PER_MSEC))

#ifdef FS3IPC_PERIODIC_TIMER_HANDLER

/**
 * fs3ipc_os_get_system_time() - Get current system time in msec.
 * @current_time: Pointer to uint32_t to store current system time.
 *
 * This must get the current system time in milliseconds and return the least
 * significant 32 bits of the time. It is expected that the returned value will
 * roll over correctly.
 *
 * Return: None.
 */
static inline void fs3ipc_os_get_system_time(uint32_t *current_time)
{
	*current_time = (uint32_t)NSEC_TO_MSEC(ktime_get_ns());
}

#else /* !(FS3IPC_PERIODIC_TIMER_HANDLER) */

typedef struct hrtimer fs3ipc_os_timer;

#define FS3IPC_TIMER_TYPE               (CLOCK_MONOTONIC)
#define FS3IPC_TIMER_MODE               (HRTIMER_MODE_REL)

#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */

#elif FS3IPC_OS_POSIX_US == 1
#endif

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
fs3ipc_StatusType fs3ipc_os_WaitEvent(fs3ipc_os_event_t *event,
				      fs3ipc_os_EventMaskType mask);

fs3ipc_StatusType fs3ipc_os_WaitEventTimeout(fs3ipc_os_event_t *event,
					     fs3ipc_os_EventMaskType mask,
					     uint32_t timeout_msec);
fs3ipc_StatusType fs3ipc_os_GetEvent(fs3ipc_os_event_t *event,
				     fs3ipc_os_EventMaskType *mask);

fs3ipc_StatusType fs3ipc_os_ClearEvent(fs3ipc_os_event_t *event,
				       fs3ipc_os_EventMaskType mask);

/**
 * fs3ipc_msleep() - Sleep for the given msec
 * @msecs: Time in milliseconds to sleep for
 *
 * Return: None.
 */
void fs3ipc_msleep(unsigned int msecs);

fs3ipc_StatusType fs3ipc_os_SetEvent(fs3ipc_os_event_t *event,
				     fs3ipc_os_EventMaskType mask);

/* Commonalities of timer implementation */
#ifdef FS3IPC_PERIODIC_TIMER_HANDLER
/**
 * fs3ipc_os_get_system_time() - Get current system time in msec.
 * @current_time: Pointer to uint32_t to store current system time.
 *
 * This macro must get the current system time in milliseconds and return the
 * least significant 32 bits of the time. It is expected that the returned value
 * will roll over correctly.
 *
 * Context: Caller holds the timer lock.
 *
 * Return: None.
 */
#if (FS3IPC_OS_LINUX_KERNEL != 1) && !defined(fs3ipc_os_get_system_time)
#error "fs3ipc_os_get_system_time() is not defined."
#endif

#else /* !(FS3IPC_PERIODIC_TIMER_HANDLER) */

/**
 * struct fs3ipc_timer_ctx - Context of FS3IPC timer
 * @timer: FS3IPC timer to which this context belongs.
 * @os_timer: FS3IPC OS timer.
 */
struct fs3ipc_timer_ctx {
	struct fs3ipc_timer * const timer;
	fs3ipc_os_timer os_timer;
};

/**
 * fs3ipc_os_verify_timer_cfg() - Verify FS3IPC OS-specific timer configuration.
 * @ctx: Context of timer.
 *
 * This is no-op stub function. If any OS-specific verification of OS-specific
 * timer configuration is needed, it needs to be implemented.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure
 */
static inline
fs3ipc_StatusType fs3ipc_os_verify_timer_cfg(const struct fs3ipc_timer_ctx *ctx)
{
	(void)ctx;

	return fs3ipc_StatusType_OK;
}

#if FS3IPC_OS_LINUX_KERNEL == 1

/**
 * fs3ipc_os_init_timer() - OS-specific initialization of timer.
 * @ctx: Context of FS3IPC timer.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure
 */
fs3ipc_StatusType fs3ipc_os_init_timer(struct fs3ipc_timer_ctx *ctx);

/**
 * fs3ipc_os_restart_timer() - OS-specific restarting of timer.
 * @ctx: Context of FS3IPC timer.
 * @timeout: Expiry timeout in msec.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure
 */
static inline fs3ipc_StatusType fs3ipc_os_restart_timer(struct fs3ipc_timer_ctx *ctx,
							uint32_t timeout)
{
	/* It is assumed that the argument is valid. */
	hrtimer_start(&ctx->os_timer, ms_to_ktime(timeout), FS3IPC_TIMER_MODE);

	return fs3ipc_StatusType_OK;
}

/**
 * fs3ipc_os_stop_timer() - OS-specific stopping of timer.
 * @ctx: Context of FS3IPC timer.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure
 */
static inline fs3ipc_StatusType fs3ipc_os_stop_timer(struct fs3ipc_timer_ctx *ctx)
{
	/* It is assumed that the argument is valid. */
	const struct fs3ipc_timer *timer = ctx->timer;

	if (hrtimer_cancel(&ctx->os_timer))
		LOG_DEBUG("%u: '%s' was still running",
			  (unsigned int)fs3ipc_get_timer_instance(timer),
			  fs3ipc_get_timer_name(timer));

	return fs3ipc_StatusType_OK;
}

#else /* !(FS3IPC_OS_LINUX_KERNEL == 1) */

/*
 * These are no-op stub functions. OS-specific timer operations needs to be
 * implemented.
 */
static inline fs3ipc_StatusType fs3ipc_os_init_timer(struct fs3ipc_timer_ctx *ctx)
{
	(void)ctx;

	return fs3ipc_StatusType_OK;
}

static inline fs3ipc_StatusType fs3ipc_os_restart_timer(struct fs3ipc_timer_ctx *ctx,
							uint32_t timeout)
{
	(void)ctx;
	(void)timeout;

	return fs3ipc_StatusType_OK;
}

static inline fs3ipc_StatusType fs3ipc_os_stop_timer(struct fs3ipc_timer_ctx *ctx)
{
	(void)ctx;

	return fs3ipc_StatusType_OK;
}

#endif /* FS3IPC_OS_LINUX_KERNEL == 1 */

#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */

/**
 * fs3ipc_os_init_resource() - Initialize resource.
 * @resource: Pointer to resource.
 *
 * Return:
 * * fs3ipc_StatusType_OK       - OK
 * * fs3ipc_StatusType_ErrorGen - Invalid resource
 */
fs3ipc_StatusType fs3ipc_os_init_resource(fs3ipc_os_resource_t *resource);

fs3ipc_StatusType fs3ipc_os_GetResource(fs3ipc_os_resource_t *resource);

fs3ipc_StatusType fs3ipc_os_ReleaseResource(fs3ipc_os_resource_t *resource);

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_OS_H */
