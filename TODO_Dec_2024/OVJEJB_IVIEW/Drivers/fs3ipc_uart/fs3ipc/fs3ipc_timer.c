/*****************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2022
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_timer.c
 * @ingroup      FS3IPC Component
 * @author       Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * FS3IPC timer component source file.
 * This implements functionality of timer component. Periodic timer handler is
 * modified version of FS3IPC OS periodic timer handler by David Rogala.
 ****************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_os.h"
#include "fs3ipc_timer_priv.h"
#include "fs3ipc_timer.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define FS3IPC_TIMER_MAX_TIMEOUT_MSEC   (3600000u)
#define FS3IPC_TIMER_MARGIN_MSEC        (2000u)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
static fs3ipc_StatusType fs3ipc_verify_timer_ctx_cfg(struct fs3ipc_timer *timer);
static fs3ipc_StatusType init_timer(struct fs3ipc_timer *timer);
static fs3ipc_StatusType fs3ipc_verify_timer_cfg(struct fs3ipc_timer *timer,
						 uint8_t fs3ipc_instance,
						 fs3ipc_timer_callback_t cb);

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
uint8_t fs3ipc_get_timer_instance(const struct fs3ipc_timer *timer)
{
	return timer->fs3ipc_instance;
}

const char *fs3ipc_get_timer_name(const struct fs3ipc_timer *timer)
{
	return timer->name;
}

#ifdef FS3IPC_PERIODIC_TIMER_HANDLER

/**
 * set_timer_active() - Set timer as active.
 * @timer: FS3IPC timer.
 *
 * It is assumed that the argument is valid.
 *
 * Context: Expects the timer lock to be held by caller.
 *
 * Return: None.
 */
static inline void set_timer_active(struct fs3ipc_timer *timer)
{
	timer->ctx.is_active = true;
}

/**
 * set_timer_inactive() - Set timer as inactive.
 * @timer: FS3IPC timer.
 *
 * It is assumed that the argument is valid.
 *
 * Context: Expects the timer lock to be held by caller.
 *
 * Return: None.
 */
static inline void set_timer_inactive(struct fs3ipc_timer *timer)
{
	timer->ctx.is_active = false;
}

fs3ipc_StatusType fs3ipc_verify_timer_table_cfg(uint8_t fs3ipc_instance)
{
	unsigned int timer_idx = 0u;
	const struct fs3ipc_timer *timer = NULL;
	const struct fs3ipc_timer_table *table = NULL;

	if (fs3ipc_instance >= FS3IPC_NUM_OF_INSTANCES) {
		LOG_ERROR("Invalid FS3IPC instance: %u",
			  (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorHandle;
	}

	table = &fs3ipc_timer_tables[fs3ipc_instance];
	if (!table->timers || !table->timer_count) {
		LOG_ERROR("%u: Timers are not configured",
			  (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorCfg;
	}

	while (timer_idx < table->timer_count) {
		timer = &table->timers[timer_idx];

		if (timer->fs3ipc_instance != fs3ipc_instance) {
			LOG_ERROR("%u: FS3IPC instance mismatch for '%s': %u",
				  (unsigned int)fs3ipc_instance, timer->name,
				  (unsigned int)(timer->fs3ipc_instance));
			return fs3ipc_StatusType_ErrorCfg;
		}

		timer_idx++;
	}

	return fs3ipc_StatusType_OK;
}

/**
 * timer_has_expired() - Check if timer has expired.
 * @expiry_time: Expiry time of timer.
 *
 * Return:
 * * true  - Timer has expired
 * * false - Timer has not expired
 */
static bool timer_has_expired(uint32_t expiry_time)
{
	bool ret = false;
	uint32_t current_time = 0u;
	uint32_t lower_cut_off = 0u;

	fs3ipc_os_get_system_time(&current_time);
	lower_cut_off = current_time - (FS3IPC_TIMER_MAX_TIMEOUT_MSEC +
		FS3IPC_TIMER_MARGIN_MSEC);

	if (lower_cut_off <= current_time) {
		/*
		 * Early timer cutoff does not rollover.
		 * (0 <= cutoff <= expiry_time <= current_time <= (2^32 -1))
		 */
		if (lower_cut_off <= expiry_time && expiry_time <= current_time)
			ret = true;
	} else {
		/*
		 * Early timer cutoff does rollover.
		 * (cutoff <= expiry_time <= (2^32 -1)) OR (0 <= expiry_time <=
		 * current_time)
		 */
		if (lower_cut_off <= expiry_time || expiry_time <= current_time)
			ret = true;
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_periodic_timer_handler(uint8_t fs3ipc_instance)
{
	unsigned int timer_idx = 0u;
	struct fs3ipc_timer *timer = NULL;
	struct fs3ipc_timer_ctx *ctx = NULL;
	const struct fs3ipc_timer_table *table = NULL;

	if (fs3ipc_instance >= FS3IPC_NUM_OF_INSTANCES) {
		LOG_ERROR("Invalid FS3IPC instance: %u",
			  (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorHandle;
	}

	table = &fs3ipc_timer_tables[fs3ipc_instance];
	while (timer_idx < table->timer_count) {
		timer = &table->timers[timer_idx];
		ctx = &timer->ctx;
		fs3ipc_os_GetResource(ctx->lock);
		if (ctx->is_active) {
			if (timer_has_expired(ctx->expiry_time))
				handle_timer_expiry(timer);
			else
				LOG_DEBUG("%u: '%s' is active & has not expired",
					  (unsigned int)fs3ipc_instance,
					  timer->name);
		} else {
			LOG_DEBUG("%u: '%s' is not active",
				  (unsigned int)fs3ipc_instance, timer->name);
		}
		fs3ipc_os_ReleaseResource(ctx->lock);
		timer_idx++;
	}

	return fs3ipc_StatusType_OK;
}

/**
 * fs3ipc_verify_timer_ctx_cfg() - Verify timer context configuration.
 * @timer: Timer whose context's configuration is to be verified.
 *
 * It is assumed that the argument is valid.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
static fs3ipc_StatusType fs3ipc_verify_timer_ctx_cfg(struct fs3ipc_timer *timer)
{
	const char *name = timer->name;
	uint8_t fs3ipc_instance = timer->fs3ipc_instance;
	struct fs3ipc_timer_ctx *ctx = &timer->ctx;

	if (!ctx->lock) {
		LOG_ERROR("%u: Lock is not configured for '%s'",
			  (unsigned int)fs3ipc_instance, name);
		return fs3ipc_StatusType_ErrorCfg;
	}

	/*
	 * Lock is not required as this is called during initialization.
	 */
	if (ctx->is_active) {
		LOG_WARNING("%u: '%s' is active, but must be inactive. So, setting it.",
			    (unsigned int)fs3ipc_instance, name);
		ctx->is_active = false;
	}

	return fs3ipc_StatusType_OK;
}

/** It is assumed that the argument is valid. */
static fs3ipc_StatusType init_timer(struct fs3ipc_timer *timer)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	ret = fs3ipc_os_init_resource(timer->ctx.lock);
	if (ret != fs3ipc_StatusType_OK) {
		LOG_ERROR("Timer lock initialization failed: %u", ret);
		return ret;
	}

	return fs3ipc_StatusType_OK;
}

/** It is assumed that the arguments are valid. */
static fs3ipc_StatusType restart_timer(struct fs3ipc_timer *timer,
				       uint32_t timeout)
{
	uint32_t current_time = 0u;
	struct fs3ipc_timer_ctx *ctx = &timer->ctx;

	fs3ipc_os_GetResource(ctx->lock);
	fs3ipc_os_get_system_time(&current_time);
	ctx->expiry_time = current_time + timeout;
	set_timer_active(timer);
	fs3ipc_os_ReleaseResource(ctx->lock);

	return fs3ipc_StatusType_OK;
}

/** It is assumed that the argument is valid. */
static fs3ipc_StatusType stop_timer(struct fs3ipc_timer *timer)
{
	struct fs3ipc_timer_ctx *ctx = &timer->ctx;

	fs3ipc_os_GetResource(ctx->lock);
	set_timer_inactive(timer);
	fs3ipc_os_ReleaseResource(ctx->lock);

	return fs3ipc_StatusType_OK;
}

#else /* !(FS3IPC_PERIODIC_TIMER_HANDLER) */

static inline void set_timer_active(struct fs3ipc_timer *timer) { (void)timer; };
static inline void set_timer_inactive(struct fs3ipc_timer *timer) { (void)timer; };

/**
 * fs3ipc_verify_timer_ctx_cfg() - Verify timer context configuration.
 * @timer: Timer whose context's configuration is to be verified.
 *
 * It is assumed that the argument is valid.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
static fs3ipc_StatusType fs3ipc_verify_timer_ctx_cfg(struct fs3ipc_timer *timer)
{
	const struct fs3ipc_timer_ctx *ctx = &timer->ctx;

	if (!ctx->timer || (ctx->timer != timer)) {
		LOG_ERROR("%u: FS3IPC timer mismatch for '%s': %p; expected: %p",
			  (unsigned int)(timer->fs3ipc_instance), timer->name,
			  (void *)(ctx->timer), (void *)timer);
		return fs3ipc_StatusType_ErrorCfg;
	}

	return fs3ipc_os_verify_timer_cfg(ctx);
}

/** It is assumed that the argument is valid. */
static fs3ipc_StatusType init_timer(struct fs3ipc_timer *timer)
{
	return fs3ipc_os_init_timer(&timer->ctx);
}

/** It is assumed that the arguments are valid. */
static inline fs3ipc_StatusType restart_timer(struct fs3ipc_timer *timer,
					      uint32_t timeout)
{
	return fs3ipc_os_restart_timer(&timer->ctx, timeout);
}

/** It is assumed that the argument is valid. */
static inline fs3ipc_StatusType stop_timer(struct fs3ipc_timer *timer)
{
	return fs3ipc_os_stop_timer(&timer->ctx);
}

#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */

void handle_timer_expiry(struct fs3ipc_timer *timer)
{
	fs3ipc_timer_callback_t cb = timer->callback;

	set_timer_inactive(timer);

#if FS3IPC_PIC_EN == 1
	/*
	 * If the image supports position independent code, add code offset to
	 * the linker time function pointer before executing.
	 */
	cb = (fs3ipc_timer_callback_t)((void *)cb + fs3ipc_getCodeOffset());
#endif /* FS3IPC_PIC_EN == 1 */

	cb(timer);
}

/**
 * fs3ipc_verify_timer_cfg() - Verify timer configuration.
 * @timer: Timer whose configuration is to be verified.
 * @fs3ipc_instance: FS3IPC instance.
 * @cb: Timer expiry callback.
 *
 * It is assumed that the arguments are valid.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK
 * * fs3ipc_StatusType_ErrorCfg    - Config issue
 */
static fs3ipc_StatusType fs3ipc_verify_timer_cfg(struct fs3ipc_timer *timer,
						 uint8_t fs3ipc_instance,
						 fs3ipc_timer_callback_t cb)
{
	const char *name = timer->name;

	if (!name) {
		LOG_ERROR("%u: Name is not assigned to timer", fs3ipc_instance);
		return fs3ipc_StatusType_ErrorCfg;
	}

	if (timer->fs3ipc_instance != fs3ipc_instance) {
		LOG_ERROR("%u: FS3IPC instance mismatch for '%s': %u",
			  (unsigned int)fs3ipc_instance, name,
			  (unsigned int)(timer->fs3ipc_instance));
		return fs3ipc_StatusType_ErrorCfg;
	}

	if (!timer->callback || (timer->callback != cb)) {
		LOG_ERROR("%u: Invalid callback configuration for '%s'",
			  (unsigned int)fs3ipc_instance, name);
		return fs3ipc_StatusType_ErrorCfg;
	}

	return fs3ipc_verify_timer_ctx_cfg(timer);
}

fs3ipc_StatusType fs3ipc_init_timer(struct fs3ipc_timer *timer,
				    uint8_t fs3ipc_instance,
				    fs3ipc_timer_callback_t cb)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	if (!timer) {
		LOG_ERROR("NULL argument");
		return fs3ipc_StatusType_ErrorGen;
	}

	if (fs3ipc_instance >= FS3IPC_NUM_OF_INSTANCES) {
		LOG_ERROR("Invalid FS3IPC instance: %u",
			  (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorHandle;
	}

	ret = fs3ipc_verify_timer_cfg(timer, fs3ipc_instance, cb);
	if (ret != fs3ipc_StatusType_OK) {
		LOG_ERROR("Timer config verification failed");
		return ret;
	}

	ret = init_timer(timer);
	if (ret == fs3ipc_StatusType_OK)
		LOG_DEBUG("%u: '%s' is initialized", (unsigned int)fs3ipc_instance,
			  timer->name);

	return ret;
}

fs3ipc_StatusType fs3ipc_restart_timer(struct fs3ipc_timer *timer,
				       uint32_t timeout)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	if (!timer) {
		LOG_ERROR("NULL argument");
		return fs3ipc_StatusType_ErrorGen;
	}

	if (timeout > FS3IPC_TIMER_MAX_TIMEOUT_MSEC) {
		LOG_ERROR("%u: Invalid timeout period for '%s': %u msec; max: %u msec",
			  (unsigned int)(timer->fs3ipc_instance), timer->name,
			  timeout, FS3IPC_TIMER_MAX_TIMEOUT_MSEC);
		return fs3ipc_StatusType_ErrorGen;
	}

	ret = restart_timer(timer, timeout);
	if (ret == fs3ipc_StatusType_OK)
		LOG_DEBUG("%u: '%s' is restarted",
			  (unsigned int)(timer->fs3ipc_instance), timer->name);

	return ret;
}

fs3ipc_StatusType fs3ipc_stop_timer(struct fs3ipc_timer *timer)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	if (!timer) {
		LOG_ERROR("NULL argument");
		return fs3ipc_StatusType_ErrorGen;
	}

	ret = stop_timer(timer);
	if (ret == fs3ipc_StatusType_OK)
		LOG_DEBUG("%u: '%s' is stopped",
			  (unsigned int)(timer->fs3ipc_instance), timer->name);

	return ret;
}
