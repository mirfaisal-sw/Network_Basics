/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2023
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         ipc.h
 * @ingroup      FS3IPC Component
 * @author       Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * FS3IPC channel device header file.
 * This header file defines all the data structures used to handle the FS3IPC
 * driver and the channels' devices.
 *****************************************************************************/

#ifndef IPC_H
#define IPC_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/serdev.h>
#include <linux/sched.h>
#include <linux/fs.h>

#include "fs3ipc_cfg.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define FS3IPC_UART_DRV_NAME            "fs3ipcuart"
#define FS3IPC_INSTANCE_NUM_LEN         (2)
#define FS3IPC_NAME_LEN                 (sizeof(FS3IPC_UART_DRV_NAME) +        \
					 FS3IPC_INSTANCE_NUM_LEN + 1)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
struct fs3ipc_uart;

/**
 * struct dev_attr - Device attributes structure.
 * @minor: Device minor number.
 * @client_handle: FS3IPC client handle.
 * @wq: Wait queue.
 * @fs3ipc: FS3IPC structure to which this device belongs.
 * @cdev: Cdev structure.
 */
struct dev_attr {
	uint8_t minor;
	uint8_t client_handle;
	wait_queue_head_t *wq;
	struct fs3ipc_uart *fs3ipc;
	struct cdev cdev;
};

/**
 * struct fs3ipc_uart - FS3IPC uart structure.
 */
struct fs3ipc_uart {
	/** @fs3ipc_instance: FS3IPC instance. */
	uint8_t fs3ipc_instance;
	/** @app_chan_count: Application channel count */
	uint32_t app_chan_count;
	/** @dev_no: Device number */
	dev_t dev_no;
	/** @dev_attr: Array of FS3IPC channel device attributes */
	struct dev_attr *dev_attr;
	/** @serdev: Serdev device. */
	struct serdev_device *serdev;
	/** @tx_task: Transmit task. */
	struct task_struct *tx_task;
#ifdef FS3IPC_PERIODIC_TIMER_HANDLER
	/** @timer_task: Timer task. */
	struct task_struct *timer_task;
#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */
	/** @pm_wait_obj: Wait object to notify PM events. */
	fs3ipc_os_event_t *pm_wait_obj;
#ifdef CONFIG_FS3IPC_DEBUG
	/** @debug_ch: Debug channel number */
	unsigned int debug_ch;
	/** @debug_cdev: Debug channel cdev structure. */
	struct cdev debug_cdev;
#endif
};

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
#ifdef CONFIG_FS3IPC_DEBUG
extern const struct file_operations fs3ipc_debug_fops;
#endif
extern struct class *fs3ipc_class;

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
/**
 * fs3ipc_get() - Get FS3IPC instance structure from FS3IPC instance number.
 * @instance: FS3IPC instance number.
 *
 * Context: Acquires and releases the fs3ipc instances lock.
 *
 * Return: FS3IPC structure if OK otherwise NULL.
 */
struct fs3ipc_uart *fs3ipc_get(unsigned int instance);

/**
 * fs3ipc_dev_init() - Initialize & create FS3IPC channel devices.
 * @fs3ipc: FS3IPC structure.
 *
 * This must be called after all other initializations otherwise there is a
 * potential race condition where a user space client may perform an I/O call
 * causing an error to be returned.
 *
 * It is assumed that the argument is valid.
 *
 * Return: 0 if OK otherwise failure.
 */
int fs3ipc_dev_init(struct fs3ipc_uart *fs3ipc);

/**
 * fs3ipc_dev_cleanup() - Cleanup FS3IPC channel devices.
 * @fs3ipc: FS3IPC structure.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
void fs3ipc_dev_cleanup(struct fs3ipc_uart *fs3ipc);

/**
 * app_0_CustomInit() - FS3IPC configuration init
 * @fs3ipc: FS3IPC structure.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
void app_0_CustomInit(struct fs3ipc_uart *fs3ipc);

#ifdef __cplusplus
}
#endif

#endif /* IPC_H */
