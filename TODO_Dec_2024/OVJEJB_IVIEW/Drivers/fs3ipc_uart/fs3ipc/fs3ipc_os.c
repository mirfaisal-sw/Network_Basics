/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_os.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * OS source file.
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_os.h"
#include "fs3ipc_timer.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#if FS3IPC_OS_AUTOSAR == 1
#include "Os.h"
#define HDLC_TIMER_CYCLIC_DISABLED (0)

#elif FS3IPC_OS_UCOSII == 1
#include "ucos_ii.h"
#endif

#ifdef LOG_MODULE
#undef LOG_MODULE
#endif
#define LOG_MODULE "fs3ipc_os"


/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
#if FS3IPC_OS_LINUX_KERNEL == 1
static fs3ipc_os_EventMaskType GetFlagsHelper(fs3ipc_os_event_t *event)
{
	unsigned long irqctx = 0;
	fs3ipc_os_EventMaskType ret;

	spin_lock_irqsave(&event->lock, irqctx);
	ret = event->flags;
	spin_unlock_irqrestore(&event->lock, irqctx);

	return ret;
}
#endif

/**
 * fs3ipc_os_WaitEvent() - Waits for the given event.
 * @event: The group of the events in case if it applies.
 * @mask: Mask of the events waited for.
 *
 * The state of the current task is set to WAITING, unless at least one of the
 * given events is set.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_os_WaitEvent(fs3ipc_os_event_t *event,
												  fs3ipc_os_EventMaskType mask)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

#if FS3IPC_OS_AUTOSAR == 1
	StatusType stat;

	stat = WaitEvent(mask);

	if (stat != E_OK) {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("CRIT! WaitEvent %d", ret)
	}

#elif FS3IPC_OS_UCOSII == 1
	fs3ipc_u8 stat = OS_NO_ERR;

	OSFlagPend(*event,
				  mask,
				  OS_FLAG_WAIT_SET_ANY,
				  0,
				  &stat);
	if (stat != OS_NO_ERR) {
		LOG_ERROR("CRIT! GetEvent stat = %d", stat);
		ret = fs3ipc_StatusType_ErrorGen;
	}
#elif FS3IPC_OS_LINUX_KERNEL == 1

	if (event) {
		fs3ipc_s32 waitStat = 0;

		waitStat = wait_event_interruptible(event->wq, mask &
			GetFlagsHelper(event));
		if (waitStat != 0) {
			ret = fs3ipc_StatusType_ErrorInterrupted;
		}
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
	}

#elif FS3IPC_OS_POSIX_US == 1
	/* todo: implement for posix user space*/
#endif

	return ret;
}

/* todo - only enable for kernel */
fs3ipc_StatusType fs3ipc_os_WaitEventTimeout(fs3ipc_os_event_t *event,
					     fs3ipc_os_EventMaskType mask,
					     uint32_t timeout_msec)
{
#if FS3IPC_OS_AUTOSAR == 1
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	StatusType stat;

	stat = WaitEvent(mask);

	if (stat != E_OK) {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("CRIT! WaitEvent %d", ret)
	}

	return ret;
#elif FS3IPC_OS_UCOSII == 1
	/* todo: implement for uCOS-ii*/
#elif FS3IPC_OS_LINUX_KERNEL == 1
	long ret = 0;
	/* timeout_jiffies can never be < 0 */
	long timeout_jiffies = (timeout_msec == FS3IPC_INFINITE_TIMEOUT) ?
		MAX_SCHEDULE_TIMEOUT : msecs_to_jiffies(timeout_msec);

	if (unlikely(!event)) {
		LOG_ERROR("NULL event");
		return fs3ipc_StatusType_ErrorGen;
	}

	ret = wait_event_interruptible_timeout(event->wq,
					       mask & GetFlagsHelper(event),
					       timeout_jiffies);
	if (ret == 0)
		return fs3ipc_StatusType_ErrorTimeout;
	if (ret == -ERESTARTSYS)
		return fs3ipc_StatusType_ErrorInterrupted;
	if (unlikely(ret < 0))   /* should never hit */
		return fs3ipc_StatusType_ErrorGen;

	return fs3ipc_StatusType_OK;
#elif FS3IPC_OS_POSIX_US == 1
	/* todo: implement for posix user space*/
#endif
}

/**
 * fs3ipc_os_GetEvent() - Returns all event bits.
 * @event: The task/flag group which shall be queried.
 * @mask: Events which are set.
 *
 * This service returns the state of all event bits of the given task.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_os_GetEvent(fs3ipc_os_event_t *event,
												 fs3ipc_os_EventMaskType *mask)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

#if FS3IPC_OS_AUTOSAR == 1
	StatusType stat;

	if (event) {
		stat = GetEvent(*event, mask);

		if (stat != E_OK) {
			/* todo: print error*/
			ret = fs3ipc_StatusType_ErrorGen;
			LOG_ERROR("CRIT! GetEvent %d", ret);
		}
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Null Arg");
	}

#elif FS3IPC_OS_UCOSII == 1
	fs3ipc_u8 stat = OS_NO_ERR;
	*mask = OSFlagAccept(*event,
								0xffffu, /*check bit16*/
								OS_FLAG_WAIT_SET_ANY,
								&stat);
	if ((stat != OS_NO_ERR) && (stat != OS_FLAG_ERR_NOT_RDY)) {
		LOG_ERROR("CRIT! GetEvent stat = %d", stat);
		ret = fs3ipc_StatusType_ErrorGen;
	}
#elif FS3IPC_OS_LINUX_KERNEL == 1

	if (event && mask)
		*mask = GetFlagsHelper(event);
	else
		ret = fs3ipc_StatusType_ErrorGen;

	/* todo: implement for linux kernel*/
#elif FS3IPC_OS_POSIX_US == 1
	/* todo: implement for posix user space*/
#endif
	return ret;
}

/**
 * fs3ipc_os_ClearEvent() - Clears the event with given mask.
 * @event: The group of the events in case if it applies.
 * @mask: The events which shall be set.
 *
 * The events of the calling task are cleared according to the given event mask.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_os_ClearEvent(fs3ipc_os_event_t *event,
													fs3ipc_os_EventMaskType mask)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

#if FS3IPC_OS_AUTOSAR == 1
	StatusType stat;

	stat = ClearEvent(mask);

	if (stat != E_OK) {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("CRIT! ClearEvent %d", ret);
	}

#elif FS3IPC_OS_UCOSII == 1
	/* todo: implement for uCOS-ii*/
	fs3ipc_u8 stat = OS_NO_ERR;

	OSFlagPost(*event, mask, OS_FLAG_CLR, &stat);
	if (stat != OS_NO_ERR) {
		LOG_ERROR("CRIT! ClearEvent %d", stat);
		ret = fs3ipc_StatusType_ErrorGen;
	}

#elif FS3IPC_OS_LINUX_KERNEL == 1
	if (event) {
		unsigned long irqctx = 0;

		spin_lock_irqsave(&event->lock, irqctx);
		event->flags &= ~mask;
		spin_unlock_irqrestore(&event->lock, irqctx);
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
	}

#elif FS3IPC_OS_POSIX_US == 1
	/* todo: implement for posix user space*/
#endif
	return ret;
}

/**
 * fs3ipc_os_SetEvent() - Events of the calling task are set.
 * @event: The task/event group which shall be modified.
 * @mask: The events which shall be set.
 *
 * The events of the calling task are set  according to the given event mask.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_os_SetEvent(fs3ipc_os_event_t *event,
												 fs3ipc_os_EventMaskType mask)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

#if FS3IPC_OS_AUTOSAR == 1
	StatusType stat;

	if (event) {
		stat = SetEvent(*event, mask);

		if (stat != E_OK) {
			ret = fs3ipc_StatusType_ErrorGen;
			LOG_ERROR("CRIT! SetEvent %d", ret);
		}
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Null Arg");
	}

#elif FS3IPC_OS_UCOSII == 1
	/* todo: implement for uCOS-ii*/
	fs3ipc_u8 stat = OS_NO_ERR;

	OSFlagPost(*event, mask, OS_FLAG_SET, &stat);
	if (stat != OS_NO_ERR) {
		LOG_ERROR("CRIT! SetEvent stat = %d", stat);
		ret = fs3ipc_StatusType_ErrorGen;
	}

#elif FS3IPC_OS_LINUX_KERNEL == 1
	if (event) {
		unsigned long irqctx = 0;

		spin_lock_irqsave(&event->lock, irqctx);
		event->flags |= mask;
		spin_unlock_irqrestore(&event->lock, irqctx);

		wake_up_interruptible(&event->wq);
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
	}

#elif FS3IPC_OS_POSIX_US == 1
	/* todo: implement for posix user space*/
#endif
	return ret;
}



/**
 * fs3ipc_os_GetResource() - Occupies the given resource.
 * @resource: The resource which shall be occupied.
 *
 * This API serves to enter critical sections in the code. A critical section
 * shall always be left using fs3ipc_os_ReleaseResource(). the function will
 * call the suspend all interruption.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_os_GetResource(fs3ipc_os_resource_t *resource)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

#if FS3IPC_OS_AUTOSAR == 1
	StatusType status;

	if (resource) {
		status = GetResource(*resource);

		if (status != E_OK) {
			ret = fs3ipc_StatusType_ErrorGen;
			LOG_ERROR("CRIT! GetResource %d", ret);
		}
	} else {
		/* for null pointer, fall back to disable all interrupts*/
		SuspendAllInterrupts();
	}

#elif FS3IPC_OS_UCOSII == 1
	if (resource) {
		fs3ipc_u8 stat = OS_NO_ERR;

		OSSemPend(*resource, 0, &stat);
		if (stat != OS_NO_ERR)
			ret = fs3ipc_StatusType_ErrorGen;
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("CRIT! GetResource NULL");
	}
#elif FS3IPC_OS_LINUX_KERNEL == 1
	if (resource) {
		mutex_lock(resource);
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("NULL mutex");
	}

#elif FS3IPC_OS_POSIX_US == 1
	/* todo: implement for posix user space*/
#endif
	return ret;
}

/**
 * fs3ipc_os_ReleaseResource() - Releases the given resource.
 * @resource: The resource which shall be released.
 *
 * This API is the counterpart of fs3ipc_os_GetResource() and serves to leave
 * critical sections in the code, if the parameter is NULL, the function will
 * call the enable  all interruption.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_os_ReleaseResource(fs3ipc_os_resource_t *resource)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

#if FS3IPC_OS_AUTOSAR == 1
	StatusType status;

	if (resource) {
		status = ReleaseResource(*resource);

		if (status != E_OK) {
			ret = fs3ipc_StatusType_ErrorGen;
			LOG_ERROR("CRIT! ReleaseResource %d", ret);
		}
	} else {
		/* for null pointer, fall back to disable all interrupts*/
		ResumeAllInterrupts();
	}

#elif FS3IPC_OS_UCOSII == 1
	if (resource) {
		fs3ipc_u8 stat = OS_NO_ERR;

		stat = OSSemPost(*resource);
		if (stat != OS_NO_ERR)
			ret = fs3ipc_StatusType_ErrorGen;
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("CRIT! ReleaseResource NULL");
	}
#elif FS3IPC_OS_LINUX_KERNEL == 1
	if (resource) {
		mutex_unlock(resource);
	} else {
		ret = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("NULL mutex");
	}

#elif FS3IPC_OS_POSIX_US == 1
	/* todo: implement for posix user space*/
#endif
	return ret;
}

void fs3ipc_msleep(unsigned int msecs)
{
#if FS3IPC_OS_AUTOSAR == 1
	/* TODO: Implement for AUTOSAR */
#elif FS3IPC_OS_UCOSII == 1
	/* TODO: Implement for UCOSII */
#elif FS3IPC_OS_LINUX_KERNEL == 1
	msleep(msecs);
#elif FS3IPC_OS_POSIX_US == 1
	/* TODO: Implement for POSIX user space */
#endif
}

fs3ipc_StatusType fs3ipc_os_init_resource(fs3ipc_os_resource_t *resource)
{
	if (!resource) {
		LOG_ERROR("NULL argument");
		return fs3ipc_StatusType_ErrorGen;
	}

#if FS3IPC_OS_AUTOSAR == 1
	/* TODO: Implement for AUTOSAR */

#elif FS3IPC_OS_UCOSII == 1
	/* TODO: Implement for UCOSII */

#elif FS3IPC_OS_LINUX_KERNEL == 1
	mutex_init(resource);

#elif FS3IPC_OS_POSIX_US == 1
	/* TODO: Implement for POSIX */
#endif
	return fs3ipc_StatusType_OK;
}

#ifndef FS3IPC_PERIODIC_TIMER_HANDLER

#if FS3IPC_OS_LINUX_KERNEL == 1
/**
 * fs3ipc_hrtimer_handler() - Timer expiry handler callback.
 * @hrt: Pointer to struct hrtimer of the timer that expired.
 *
 * It is assumed that the argument is valid.
 *
 * Return: HRTIMER_NORESTART
 */
static enum hrtimer_restart fs3ipc_hrtimer_handler(struct hrtimer *hrt)
{
	const struct fs3ipc_timer_ctx *ctx =
		container_of(hrt, struct fs3ipc_timer_ctx, os_timer);

	handle_timer_expiry(ctx->timer);

	return HRTIMER_NORESTART;
}

/** It is assumed that the argument is valid. */
fs3ipc_StatusType fs3ipc_os_init_timer(struct fs3ipc_timer_ctx *ctx)
{
	hrtimer_init(&ctx->os_timer, FS3IPC_TIMER_TYPE, FS3IPC_TIMER_MODE);
	ctx->os_timer.function = fs3ipc_hrtimer_handler;

	return fs3ipc_StatusType_OK;
}

#endif /* FS3IPC_OS_LINUX_KERNEL == 1 */

#endif /* !(FS3IPC_PERIODIC_TIMER_HANDLER) */
