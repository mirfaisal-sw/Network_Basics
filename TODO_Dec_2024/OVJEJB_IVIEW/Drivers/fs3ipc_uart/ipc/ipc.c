/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         ipc.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * FS3IPC over UART driver implementation for Linux Kernel.
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/serdev.h>
#include <linux/pm.h>
#include <linux/idr.h>

#include "fs3ipc.h"
#include "fs3ipc_cfg.h"
#include "fs3ipc_phys.h"
#include "fs3ipc_os.h"
#include "fs3ipc_hdlc.h"
#include "ipc.h"
#include "fs3ipc_timer.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define FS3IPC_UART_DRV_VERSION         "1.20_23Feb2024"
#define FS3IPC_DRV_CLASS_NAME           "fs3ipc"
#define FS3IPC_TX_THREAD_NAME           "fs3ipc_tx"
#define FS3IPC_TIMER_THREAD_NAME        "fs3ipc_timer"
#define FS3IPC_SERDEV_WRITE_TIMEOUT     (200)
#ifdef FS3IPC_PERIODIC_TIMER_HANDLER
#define FS3IPC_TIMER_POLL_RATE_MSEC     (50)
#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */

#define FS3IPC_SUSPEND_EVENT            (EVENT_SUSPEND)
#define FS3IPC_RESUME_EVENT             (EVENT_RESUME)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
static int tx_thread(void *arg);
static int fs3ipc_uart_receive(struct serdev_device *serdev,
			       const unsigned char *data, size_t count);
static void fs3ipc_uart_serdev_device_wakeup(struct serdev_device *serdev);
static struct fs3ipc_uart *__fs3ipc_get(unsigned int instance);
static void fs3ipc_set(unsigned int instance, struct fs3ipc_uart *fs3ipc);
static int fs3ipc_uart_probe(struct serdev_device *serdev);
static void fs3ipc_uart_remove(struct serdev_device *serdev);
static int fs3ipc_pm_op(struct device *dev, fs3ipc_os_EventMaskType event,
			const char *name);
static int fs3ipc_uart_suspend(struct device *dev);
static int fs3ipc_uart_resume(struct device *dev);
static int __init fs3ipc_uart_driver_init(void);
static void __exit fs3ipc_uart_driver_exit(void);

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
static const struct serdev_device_ops fs3ipc_uart_serdev_ops = {
	.receive_buf = fs3ipc_uart_receive,
	.write_wakeup = fs3ipc_uart_serdev_device_wakeup,
};

static const struct of_device_id fs3ipc_uart_of_match[] = {
	{
		.compatible = "harman,fs3ipcuart",
	},
	{}
};
MODULE_DEVICE_TABLE(of, fs3ipc_uart_of_match);

static const struct dev_pm_ops fs3ipc_uart_pm_ops = {
	.suspend = fs3ipc_uart_suspend,
	.resume  = fs3ipc_uart_resume,
};

static struct serdev_device_driver fs3ipc_uart_driver = {
	.probe = fs3ipc_uart_probe,
	.remove = fs3ipc_uart_remove,
	.driver = {
		.name = FS3IPC_UART_DRV_NAME,
		.of_match_table = of_match_ptr(fs3ipc_uart_of_match),
		.owner = THIS_MODULE,
		.pm = &fs3ipc_uart_pm_ops,
	},
};

static DEFINE_IDA(fs3ipc_instance_count);
static DEFINE_RWLOCK(fs3ipc_instances_lock);
static struct fs3ipc_uart *fs3ipc_instances[FS3IPC_NUM_OF_INSTANCES];
static unsigned int fs3ipc_num_instances = FS3IPC_NUM_OF_INSTANCES;
struct class *fs3ipc_class;

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
#ifdef FS3IPC_PERIODIC_TIMER_HANDLER
/**
 * timer_thread() - Timer task function.
 * @arg: FS3IPC uart structure.
 *
 * It is assumed that the argument is valid.
 *
 * Return: 0.
 */
static int timer_thread(void *arg)
{
	struct fs3ipc_uart *ipc = (struct fs3ipc_uart *)arg;
	unsigned long timeout = 0ul;

	dev_info(&ipc->serdev->dev, "FS3IPC[%u]: '%s' thread entered",
		 (unsigned int)ipc->fs3ipc_instance, FS3IPC_TIMER_THREAD_NAME);

	while (!kthread_should_stop()) {
		timeout = FS3IPC_TIMER_POLL_RATE_MSEC;
		/*
		 * If msleep is interrupted before completion, then it should be
		 * called for remaining time.
		 */
		while (timeout)
			timeout = msleep_interruptible(timeout);

		fs3ipc_periodic_timer_handler(ipc->fs3ipc_instance);
	}

	dev_info(&ipc->serdev->dev, "FS3IPC[%u]: '%s' thread exited",
		 (unsigned int)ipc->fs3ipc_instance, FS3IPC_TIMER_THREAD_NAME);

	return 0;
}

/**
 * timer_thread_create() - Create timer thread.
 * @ipc: FS3IPC uart structure.
 *
 * Verify timer table configuration & create timer thread for periodic timer
 * handler.
 *
 * It is assumed that the argument is valid.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure
 */
static fs3ipc_StatusType timer_thread_create(struct fs3ipc_uart *ipc)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	ret = fs3ipc_verify_timer_table_cfg(ipc->fs3ipc_instance);
	if (ret != fs3ipc_StatusType_OK) {
		dev_err(&ipc->serdev->dev,
			"FS3IPC[%u]: Timer table config verification failed",
			(unsigned int)ipc->fs3ipc_instance);
		return ret;
	}

	ipc->timer_task = kthread_run(timer_thread, (void *)ipc,
				      FS3IPC_TIMER_THREAD_NAME);
	if (!ipc->timer_task) {
		dev_err(&ipc->serdev->dev,
			"FS3IPC[%u]: '%s' thread creation failed",
			(unsigned int)ipc->fs3ipc_instance,
			FS3IPC_TIMER_THREAD_NAME);
		return fs3ipc_StatusType_ErrorGen;
	}

	return fs3ipc_StatusType_OK;
}

/**
 * timer_thread_destroy() - Destroy timer thread.
 * @ipc: FS3IPC uart structure.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None
 */
static void timer_thread_destroy(struct fs3ipc_uart *ipc)
{
	int ret = kthread_stop(ipc->timer_task);

	dev_info(&ipc->serdev->dev, "FS3IPC[%u]: '%s' thread stopped(ret: %d)",
		 (unsigned int)ipc->fs3ipc_instance, FS3IPC_TIMER_THREAD_NAME,
		 ret);
}

#else /* !(FS3IPC_PERIODIC_TIMER_HANDLER) */

static inline fs3ipc_StatusType timer_thread_create(struct fs3ipc_uart *ipc)
{
	(void)ipc;

	return fs3ipc_StatusType_OK;
}

static inline void timer_thread_destroy(struct fs3ipc_uart *ipc) { (void)ipc; }

#endif /* FS3IPC_PERIODIC_TIMER_HANDLER */

/**
 * tx_thread() - Transmit task function.
 * @arg: FS3IPC uart structure.
 *
 * It is assumed that the argument is valid.
 *
 * Return: 0.
 *
 * The function loops infinitely & does not return.
 */
static int tx_thread(void *arg)
{
	struct fs3ipc_uart *ipc = (struct fs3ipc_uart *)arg;

	dev_info(&ipc->serdev->dev, "FS3IPC[%u]: '%s' thread entered",
		 (unsigned int)ipc->fs3ipc_instance, FS3IPC_TX_THREAD_NAME);

	fs3ipc_TxThread(ipc->fs3ipc_instance);

	return 0;
}

/**
 * fs3ipc_uart_receive() - Serdev receive callback.
 * @serdev: Serdev device.
 * @data: Received data.
 * @count: No. of bytes received.
 *
 * It is assumed that all the arguments are valid.
 *
 * Return: @count
 */
static int fs3ipc_uart_receive(struct serdev_device *serdev,
			       const unsigned char *data, size_t count)
{
	struct fs3ipc_uart *ipc = serdev_device_get_drvdata(serdev);

	fs3ipc_RxThread(ipc->fs3ipc_instance, data, count);

	return count;
}

/**
 * fs3ipc_uart_serdev_device_wakeup() - Serdev device ready wakeup callback.
 * @serdev: Serdev device.
 *
 * It is assumed that the argument is valid.
 *
 * Context: Must not sleep.
 *
 * Return: None.
 */
static void fs3ipc_uart_serdev_device_wakeup(struct serdev_device *serdev)
{
	struct fs3ipc_uart *ipc = serdev_device_get_drvdata(serdev);

	dev_dbg(&serdev->dev,
		"FS3IPC[%u]: Serdev device ready wakeup callback called",
		(unsigned int)ipc->fs3ipc_instance);

	serdev_device_write_wakeup(serdev);
}

int fs3ipc_uart_transmit(void *usr_ptr, u8 *data, u32 count)
{
	struct serdev_device *serdev = (struct serdev_device *)usr_ptr;

	return serdev_device_write(serdev, data, count,
				   FS3IPC_SERDEV_WRITE_TIMEOUT);
}

/**
 * __fs3ipc_get() - Helper to get FS3IPC structure from FS3IPC instance number.
 * @instance: FS3IPC instance number.
 *
 * It is assumed that @instance is valid.
 *
 * Context: Acquires and releases the fs3ipc instances lock.
 *
 * Return: FS3IPC structure.
 */
static struct fs3ipc_uart *__fs3ipc_get(unsigned int instance)
{
	struct fs3ipc_uart *fs3ipc = NULL;

	read_lock(&fs3ipc_instances_lock);
	fs3ipc = fs3ipc_instances[instance];
	read_unlock(&fs3ipc_instances_lock);

	return fs3ipc;
}

struct fs3ipc_uart *fs3ipc_get(unsigned int instance)
{
	if (instance >= FS3IPC_NUM_OF_INSTANCES)
		return NULL;

	return __fs3ipc_get(instance);
}

/**
 * fs3ipc_set() - Set new FS3IPC instance structure.
 * @instance: FS3IPC instance number.
 * @fs3ipc: New FS3IPC structure.
 *
 * It is assumed that @instance is valid.
 *
 * Context: Acquires and releases the fs3ipc instances lock.
 *
 * Return: None.
 */
static void fs3ipc_set(unsigned int instance, struct fs3ipc_uart *fs3ipc)
{
	write_lock(&fs3ipc_instances_lock);
	fs3ipc_instances[instance] = fs3ipc;
	write_unlock(&fs3ipc_instances_lock);
}

/**
 * fs3ipc_uart_probe() - Probe FS3IPC uart driver.
 * @serdev: Serdev device.
 *
 * It is assumed that the argument is valid.
 *
 * Return:
 * * 0      - Ok
 * * ENOMEM - No memory available or driver error
 */
static int fs3ipc_uart_probe(struct serdev_device *serdev)
{
	int ret = 0;
	u32 baud_req = 1000000u;
	unsigned int baud_cfg = 0u;
	struct fs3ipc_uart *fs3ipc = NULL;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;

	dev_notice(&serdev->dev, "FS3IPC: '%s' driver - version '%s' probe called",
		   FS3IPC_UART_DRV_NAME, FS3IPC_UART_DRV_VERSION);

	fs3ipc = devm_kzalloc(&serdev->dev, sizeof(struct fs3ipc_uart), GFP_KERNEL);
	if (!fs3ipc)
		return -ENOMEM;

	ret = ida_alloc_max(&fs3ipc_instance_count, fs3ipc_num_instances - 1,
			    GFP_KERNEL);
	if (ret < 0) {
		dev_err(&serdev->dev, "FS3IPC: Instance allocation failed: %d",
			ret);
		return ret;
	}

	instance = ret;
	if (unlikely(__fs3ipc_get(instance))) {
		dev_err(&serdev->dev,
			"FS3IPC[%u]: Instance occupied; this should never happen",
			instance);
		ret = -ENXIO;
		goto cleanup_ida;
	}

	fs3ipc->fs3ipc_instance = instance;
	fs3ipc->serdev = serdev;
	serdev_device_set_drvdata(serdev, fs3ipc);
	serdev_device_set_client_ops(serdev, &fs3ipc_uart_serdev_ops);

	of_property_read_u32(serdev->dev.of_node, "current-speed", &baud_req);

	app_0_CustomInit(fs3ipc);
	if (fs3ipc_phys_uart_Init(instance, serdev) != fs3ipc_StatusType_OK) {
		dev_err(&serdev->dev, "FS3IPC[%u]: Phy initialization failed",
			instance);
		goto cleanup_ida;
	}
	if (fs3ipc_hdlc_init(instance) != fs3ipc_StatusType_OK) {
		dev_err(&serdev->dev, "FS3IPC[%u]: HDLC initialization failed",
			instance);
		goto cleanup_ida;
	}
	if (fs3ipc_init(instance) != fs3ipc_StatusType_OK) {
		dev_err(&serdev->dev,
			"FS3IPC[%u]: Client/Channel initialization failed",
			instance);
		goto cleanup_ida;
	}

	ret = serdev_device_open(serdev);
	if (ret) {
		dev_err(&serdev->dev, "FS3IPC[%u]: Serdev device open failed: %d",
			instance, ret);
		goto cleanup_ida;
	}

	baud_cfg = serdev_device_set_baudrate(serdev, baud_req);
	if (baud_cfg == 0) {
		dev_err(&serdev->dev, "FS3IPC[%u]: Setting baud rate(%u) failed",
			instance, baud_req);
		goto cleanup_ida;
	}
	dev_notice(&serdev->dev,
		   "FS3IPC[%u]: Baudrate - requested: %u, configured: %u",
		   instance, baud_req, baud_cfg);

	serdev_device_set_flow_control(serdev, false);

	fs3ipc->tx_task = kthread_run(tx_thread, fs3ipc, FS3IPC_TX_THREAD_NAME);
	if (!fs3ipc->tx_task) {
		dev_err(&serdev->dev, "FS3IPC[%u]: '%s' thread creation failed",
			instance, FS3IPC_TX_THREAD_NAME);
		goto cleanup_ida;
	}

	if (timer_thread_create(fs3ipc) != fs3ipc_StatusType_OK)
		goto cleanup_ida;

	if (fs3ipc_dev_init(fs3ipc) != 0) {
		dev_err(&serdev->dev,
			"FS3IPC[%u]: Channel devices initialization failed",
			instance);
		goto cleanup_ida;
	}

	fs3ipc_set(instance, fs3ipc);

	dev_info(&serdev->dev, "FS3IPC[%u]: Communication switched on", instance);

	return 0;

	/* TODO: Complete cleanup in case probe fails. */
cleanup_ida:
	ida_free(&fs3ipc_instance_count, instance);

	return -ENOMEM;
}

/**
 * fs3ipc_uart_remove() - Remove FS3IPC uart driver.
 * @serdev: Serdev device.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
static void fs3ipc_uart_remove(struct serdev_device *serdev)
{
	struct fs3ipc_uart *fs3ipc = serdev_device_get_drvdata(serdev);

	fs3ipc_set(fs3ipc->fs3ipc_instance, NULL);
	fs3ipc_dev_cleanup(fs3ipc);
	timer_thread_destroy(fs3ipc);
	serdev_device_close(serdev);
	ida_free(&fs3ipc_instance_count, fs3ipc->fs3ipc_instance);
}

/**
 * fs3ipc_pm_op() - PM operation.
 * @dev: Basic device.
 * @event: Event mask to wait for.
 * @name: Name of event.
 *
 * It is assumed that the argument is valid.
 *
 * Return:
 * * 0    - Success.
 * * -EIO - General input/output error.
 */
static int fs3ipc_pm_op(struct device *dev, fs3ipc_os_EventMaskType event,
			const char *name)
{
	int ret = 0;
	const char *status = "SUCCESS";
	fs3ipc_StatusType stat = fs3ipc_StatusType_OK;
	struct fs3ipc_uart *fs3ipc = dev_get_drvdata(dev);

	pr_alert("MARKER STR %s START - FS3IPC[%u]: %s\n", name,
		  fs3ipc->fs3ipc_instance, __func__);

	stat = fs3ipc_os_SetEvent(fs3ipc->pm_wait_obj, event);
	if (stat != fs3ipc_StatusType_OK) {
		status = "FAILED";
		ret = -EIO;
	}

	pr_alert("MARKER STR %s END - FS3IPC[%u]: %s, status: %s, ret: %d, fs3ipc_ret: %u\n",
		  name, fs3ipc->fs3ipc_instance, __func__, status, ret, stat);

	return ret;
}

static int fs3ipc_uart_suspend(struct device *dev)
{
	return fs3ipc_pm_op(dev, FS3IPC_SUSPEND_EVENT, "SUSPEND");
}

static int fs3ipc_uart_resume(struct device *dev)
{
	return fs3ipc_pm_op(dev, FS3IPC_RESUME_EVENT, "RESUME");
}

static int __init fs3ipc_uart_driver_init(void)
{
	fs3ipc_class = class_create(THIS_MODULE, FS3IPC_DRV_CLASS_NAME);
	if (IS_ERR(fs3ipc_class)) {
		pr_err("FS3IPC: Class creation failed: %ld",
		       PTR_ERR(fs3ipc_class));
		return (int)PTR_ERR(fs3ipc_class);
	}

	serdev_device_driver_register(&fs3ipc_uart_driver);

	pr_notice("FS3IPC: '%s' driver - version '%s' registered",
		  FS3IPC_UART_DRV_NAME, FS3IPC_UART_DRV_VERSION);

	return 0;
}
module_init(fs3ipc_uart_driver_init);

static void __exit fs3ipc_uart_driver_exit(void)
{
	serdev_device_driver_unregister(&fs3ipc_uart_driver);
	class_destroy(fs3ipc_class);
	ida_destroy(&fs3ipc_instance_count);

	pr_notice("FS3IPC: '%s' driver - version '%s' unregistered",
		  FS3IPC_UART_DRV_NAME, FS3IPC_UART_DRV_VERSION);
}
module_exit(fs3ipc_uart_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HARMAN FS3IPC over UART driver");
MODULE_VERSION(FS3IPC_UART_DRV_VERSION);
MODULE_AUTHOR("David Rogala<david.rogala@harman.com>");
