/*****************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_cfg.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * FS3IPC configuration header file.
 *****************************************************************************/

#ifndef FS3IPC_CFG_H
#define FS3IPC_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/kernel.h>

#include "vlqueue.h"
#include "orrlist.h"
#include "fs3ipc_types.h"
#include "log.h"
#include "fs3ipc_api.h"
#include "fs3ipc_instances.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
/*****************************************************************************
 * GENERAL
 *****************************************************************************/
#define DbgPrint(a, b, c)
#define FS3_IPC_APP_GEN_NUM_PRIORITIES (10)
#define FS3IPC_MAX_NUMBER_CHANNELS (32)
#define FS3IPC_INTERNAL_CHANNELS (2)
#define FS3IPC_TRANS_SIZE_MAX (128)
#define LOG_DATA_MAX_BYTES (128)

#define fs3ipc_memset(dest, val, len)    memset(dest, val, len)
#define fs3ipc_memcpy(dest, src, len)    memcpy(dest, src, len)
#define fs3ipc_memcmp(dest, src, len)    memcmp(dest, src, len)

/* OS/Environment abstraction configuration*/
#define FS3IPC_OS_AUTOSAR (0)
#define FS3IPC_OS_POSIX (0)
#define FS3IPC_OS_UCOSII (0)
#define FS3IPC_OS_LINUX_KERNEL (1)

#define FS3IPC_PIC_EN (0)
#define FS3IPC_DATA_LAYER_ENABLE_TEST_SECTION (0)

/*****************************************************************************
 * os integration module configuration
 ****************************************************************************/
/**
 * fs3ipc_uart_transmit() - Write to serdev device.
 * @usr_ptr: Serdev device.
 * @data: Data to transmit.
 * @count: No. of bytes to transmit.
 *
 * Context: Process context. May sleep.
 *
 * Return:
 * > 0            - Number of bytes written (less than @count if interrupted)
 * * -ERESTARTSYS - Interrupted before any bytes were written
 * * -ETIMEDOUT   - Interrupted before any bytes were written
 * * -EINVAL      - Invalid argument(s)
 * * any other negative errno on errors.
 */
int fs3ipc_uart_transmit(void *usr_ptr, u8 *data, u32 count);

/*****************************************************************************
 * Timer configuration
 ****************************************************************************/

/*****************************************************************************
 * Physical Layer
 ****************************************************************************/

/**
 * fs3ipc_hw_write() - FS3IPC hardware write.
 * @handle: Driver context ID.
 * @buf: Data to transmit.
 * @count: No. of bytes to transmit.
 * @usr_ptr: Pointer to user data for h/w write.
 *
 * Return:
 * >= 0 - Number of bytes written for partial and complete writes
 * < 0  - Failure
 */
#define fs3ipc_hw_write(handle, buf, count, usr_ptr) \
	fs3ipc_uart_transmit(usr_ptr, buf, count)
#define fs3ipc_phys_ext_Decode(handle, buf, count) \
	fs3ipc_hdlc_Decode_Cb(handle, buf, count)

/*****************************************************************************
 * Data-link layer configuration - HDLC
 ****************************************************************************/
/* global */
#define FS3IPC_HDLC_NUM_OF_INSTANCES (FS3IPC_NUM_OF_INSTANCES)
/* external function linkage*/
#define fs3ipc_hdlc_ext_Decode(handle, buf, len)\
		fs3ipc_app_Decoder(handle, buf, len)

#if FS3IPC_DATA_LAYER_ENABLE_TEST_SECTION == 1
#define fs3ipc_hdlc_ext_Encode(handle, buf, len)\
		fs3ipc_test_app_Encoder(handle, buf, len)
#else
#define fs3ipc_hdlc_ext_Encode(handle, buf, len)\
		fs3ipc_app_Encoder(handle, buf, len)
#endif

#define fs3ipc_hdlc_ext_transmit(uLldId, dataPtr, length)\
	fs3ipc_phys_uart_Encode(uLldId, dataPtr, length)

#define fs3ipc_hdlc_ext_CalculateCRC(buf, len)\
		CalculateCRC(buf, len)

/*
 * TODO: implement me. the application layer should
 * 1) clear rx/tx queues,
 * 2) reset all flow control data regarding the other node
 * 3) Queue a flow control message
 */
#define fs3ipc_hdlc_ext_HandleExtNodeReset(handle) \
		fs3ipc_app_HandleExtNodeReset(handle)

/*Only in case if the lock conditions are enabled*/
/**
 * The Define disables the interrupts
 */
/**< Disable interrupts */
#define HDLC_EVENT_LOCK() /*SuspendAllInterrupts()*/
/**
 * The Define enables the interrupts
 */
/**< Return Interrupts to previous state */
#define HDLC_EVENT_UNLOCK() /*ResumeAllInterrupts()*/

/* Instance 0 for hdlc*/
#define FS3IPC_HDLC_I0_HANDLE (0)

/* Instance 0 for loopback*/
#define FS3IPC_LOOPBACK_I0_HANDLE (0)

#define WINDOW_SIZE (7)
/*****************************************************************************
 * Application layer configuration
 ****************************************************************************/
/* global */
#define FS3IPC_APP_NUM_OF_INSTANCES (FS3IPC_NUM_OF_INSTANCES)
#define FS3IPC_APP_WRAPPED_MESSAGE_ENABLED (1)
#define FS3_IPC_MAX_MSG_SIZE (255)
#define FS3IPC_APP_ALLOW_MSG_Q_WO_FC_CHECK (1)

#define fs3ipc_app_qStat_OK VLQUEUE_OK                 /* must be 0*/
#define fs3ipc_app_qStat_full VLQUEUE_ERR_QUEUE_FULL   /* must be negative*/
#define fs3ipc_app_qStat_empty VLQUEUE_ERR_QUEUE_EMTPY /* must be negative*/

/* external function linkage*/
#define fs3ipc_app_QueueInit(context, buf, len)\
		vlqueue_init(context, buf, len)
#define fs3ipc_app_QueuePop(context, buf, len)\
		vlqueue_pop(context, buf, len)
#define fs3ipc_app_QueuePush(context, buf, len)\
		vlqueue_push(context, buf, len)
#define fs3ipc_app_QueueClear(context)             vlqueue_empty(context)
#define FS3IPC_APP_QUEUE_HDR_SIZE (2)

#define fs3ipc_app_PriorityNodeInit(node, cfg, prio)\
		orrlist_init_node(node, cfg, prio)
#define fs3ipc_app_PriorityNodeInsert(head, node)   orrlist_insert(head, node)
#define fs3ipc_app_PriorityNodeNext(node)          orrlist_next(node)

/* Instance 0*/
#define FS3IPC_APP_I0_HANDLE (0)

/* Section defined for testing operation configuration*/

/*
 * Enables/disables the loopback mode
 * 1  : the module will copy the RX buffer to TX buffer
 * 0 : the module will operate normally
 */
#define FS3IPC_LOOPBACK_ENABLED      (0)

/*****************************************************************************
 * Debugging
 *****************************************************************************/
/* print every trasnfer - requires a lot of overhead*/
#define FS3IPC_PRINT_EACH_TRANSFER 0

/* Supported logging levels*/
#define FS3IPC_LOG_LVL_EMERG     (0)
#define FS3IPC_LOG_LVL_ALERT     (1)
#define FS3IPC_LOG_LVL_CRIT      (2)
#define FS3IPC_LOG_LVL_ERROR     (3)
#define FS3IPC_LOG_LVL_WARNING   (4)
#define FS3IPC_LOG_LVL_NOTICE    (5)
#define FS3IPC_LOG_LVL_INFO      (6)
#define FS3IPC_LOG_LVL_DEBUG     (7)

/*Enables disables the debug print operation
 * 1 : enables the debug operation
 * 0 : disables the debug operation
 */
#define FS3IPC_OS_FS3IPC_DBG_PRINT   (1)

/* Controls which print statements are compiled.
 * Larger number = more verbose
 */
#define FS3IPC_LOG_LVL (FS3IPC_LOG_LVL_ERROR)

/* helper macros*/
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


#if FS3IPC_OS_FS3IPC_DBG_PRINT == 1
	#define LOG_EMERG(str, ...) log_error(str, ##__VA_ARGS__)
	#define LOG_ALERT(str, ...) log_error(str, ##__VA_ARGS__)
	#define LOG_CRIT(str, ...) log_error(str, ##__VA_ARGS__)
	#define LOG_ERROR(str, ...) log_error(str, ##__VA_ARGS__)

	#define LOG_WARNING(str, ...) log_warning(str, ##__VA_ARGS__)
	#define LOG_NOTICE(str, ...) log_notice(str, ##__VA_ARGS__)
	#define LOG_INFO(str, ...) log_info(str, ##__VA_ARGS__)
	#define LOG_DEBUG(str, ...) log_debug(str, ##__VA_ARGS__)

	#define LOG_DATA(enabled, data, length, maxlength, str, ...)                      \
		do {                                                                      \
			if (enabled) {                                                    \
				char sdata[(maxlength * 3) + 5];                          \
				printData(sdata, sizeof(sdata), data, length, maxlength); \
				pr_notice("[ IPC:%-20.20s: %-1.1d ] [NTC] " str ": %s\n", \
				__func__, __LINE__,  ##__VA_ARGS__, sdata);               \
			}                                                                 \
		} while (0)
#else /*FS3IPC_OS_FS3IPC_DBG_PRINT != 1*/

#define LOG_ERROR(str, ...)
#define LOG_WARNING(str, ...)
#define LOG_DEBUG(str, ...)
#define LOG_DATA(enabled, data, length, maxlength, str, ...)
#endif

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
/* external type linkage*/
typedef VLQueue_ccb_t fs3ipc_app_queueType;
typedef orrlist_node_t fs3ipc_app_priorityNodeType;

enum {
	FS3IPC_0_CH2_HANDLE = 0,
	FS3IPC_0_CH3_HANDLE,
	FS3IPC_0_CH4_HANDLE,
	FS3IPC_0_CH5_HANDLE,
	FS3IPC_0_CH6_HANDLE,
	FS3IPC_0_CH7_HANDLE,
	FS3IPC_0_CH8_HANDLE,
	FS3IPC_0_CH9_HANDLE,
	FS3IPC_0_CH10_HANDLE,
	FS3IPC_0_CH11_HANDLE,
	FS3IPC_0_CH12_HANDLE,
	FS3IPC_0_CH13_HANDLE,
	FS3IPC_0_CH14_HANDLE,
	FS3IPC_0_CH15_HANDLE,
	FS3IPC_0_CH16_HANDLE,
	FS3IPC_0_CH17_HANDLE,
	FS3IPC_0_CH18_HANDLE,
	FS3IPC_0_CH19_HANDLE,
	FS3IPC_0_CH20_HANDLE,
	FS3IPC_0_CH21_HANDLE,
	FS3IPC_0_CH22_HANDLE,
	FS3IPC_0_CH23_HANDLE,
	FS3IPC_0_CH24_HANDLE,
	FS3IPC_0_CH25_HANDLE,
	FS3IPC_0_CH26_HANDLE,
	FS3IPC_0_CH27_HANDLE,
	FS3IPC_0_CH28_HANDLE,
	FS3IPC_0_CH29_HANDLE,
	FS3IPC_0_CH30_HANDLE,
	FS3IPC_0_CH31_HANDLE,
	FS3IPC_CLIENT_HANDLE_COUNT
};

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
/*TODO move me to specific place to print debug*/
extern fs3ipc_u8 fs3ipc_gLogLevel;

/*****************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/
static inline int32_t fs3ipc_app_QueueFrontSize(fs3ipc_app_queueType *appq)
{
	return vlqueue_fsize(appq);
}

static inline int32_t fs3ipc_app_QueueMsgCnt(fs3ipc_app_queueType *appq)
{
	return vlqueue_size(appq);
}

static inline int32_t fs3ipc_app_QueueFreeSpace(fs3ipc_app_queueType *appq)
{
	return vlqueue_bfree(appq);
}

static inline int32_t fs3ipc_app_queue_msg_freespace(fs3ipc_app_queueType *appq,
						     uint32_t size)
{
	return vlqueue_mfree(appq, size);
}

static inline int32_t fs3ipc_app_queue_total_size(fs3ipc_app_queueType *appq)
{
	return vlqueue_btotal(appq);
}

/*****************************************************************************
 * Supported module includes
 *****************************************************************************/
/*
 * all module/layer includes. These are necessary since it is not known by
 * each layer module what modules it will communicate with. This is specified
 * by this configuration file. These modules must be imported at the end of
 * the files since they may be dependant on configurations defined above.
 */
#include "../fs3ipc_os.h" /* os first*/
#include "../fs3ipc_app.h"
#include "../fs3ipc_phys.h"
#include "../fs3ipc_hdlc.h"

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_CFG_H */
