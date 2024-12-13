/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2023
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         ipc_cdev.c
 * @ingroup      FS3IPC Component
 * @author       Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * FS3IPC channel device source file.
 * This implements functionality of FS3IPC channels' devices as character
 * devices.
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/string.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/gfp.h>

#include "log.h"
#include "fs3ipc_cfg.h"
#include "fs3ipc.h"
#include "fs3ipc_app.h"
#include "ipc.h"
#include "fs3ipc_api.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define FS3IPC_APP_CHANNEL_NAME         "ipc"

#ifdef CONFIG_FS3IPC_DEBUG
#define FS3IPC_DEBUG_CHANNEL_NAME       "ipcdebug"
#define FS3IPC_DEBUG_CHANNEL_COUNT      (1)
#else
#define FS3IPC_DEBUG_CHANNEL_COUNT      (0)
#endif

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
static int dev_wait_timeout(const struct fs3ipc_client *client,
			    fs3ipc_os_event_t *wait_obj,
			    fs3ipc_os_EventMaskType event, const char *name,
			    uint32_t timeout_msec);
static int handle_timeout(const struct fs3ipc_client *client, const char *name,
			  int flags);
static int fs3ipc_dev_open(unsigned int instance, unsigned int ch,
			   unsigned int handle);
static int fs3ipc_dev_app_chan_open(struct inode *inode, struct file *filp);
static int fs3ipc_dev_timed_read(const struct fs3ipc_client *client, char *buf,
				 size_t length, uint32_t timeout);
static ssize_t fs3ipc_dev_app_chan_read(struct file *filp, char __user *buf,
					size_t length, loff_t *f_pos);
static int fs3ipc_dev_timed_write(const struct fs3ipc_client *client,
				  const char *buf, size_t length,
				  uint32_t timeout);
static ssize_t fs3ipc_dev_app_chan_write(struct file *filp, const char __user *buf,
					 size_t length, loff_t *f_pos);
static __poll_t fs3ipc_dev_app_chan_poll(struct file *filp,
					 struct poll_table_struct *pt);
static int fs3ipc_dev_app_chan_close(struct inode *inode, struct file *filp);
static int fs3ipc_dev_app_chan_create(struct dev_attr *dev_attr,
				      struct fs3ipc_uart *fs3ipc,
				      const fs3ipc_app_ltchancfg_t *chan_cfg,
				      unsigned int minor);
static void fs3ipc_dev_app_chan_destroy(struct dev_attr *dev_attr);

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
static const struct file_operations fs3ipc_dev_app_chan_fops = {
	.owner = THIS_MODULE,
	.open = fs3ipc_dev_app_chan_open,
	.read = fs3ipc_dev_app_chan_read,
	.write = fs3ipc_dev_app_chan_write,
	.poll = fs3ipc_dev_app_chan_poll,
	.release = fs3ipc_dev_app_chan_close,
};

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
/**
 * dev_wait_timeout() - FS3IPC channel device wait on event with timeout.
 * @client: FS3IPC client.
 * @wait_obj: OS object to wait on.
 * @event: Event mask to wait for.
 * @name: Name of event.
 * @timeout_msec: Timeout in msecs.
 *
 * It is assumed that @client & @name are valid.
 *
 * Return:
 * * > 0    - On success, event is set & available.
 * * 0      - On timeout and event is not set.
 * * -EINTR - Waiting interrupted.
 * * -EIO   - General input/output error.
 */
static int dev_wait_timeout(const struct fs3ipc_client *client,
			    fs3ipc_os_event_t *wait_obj,
			    fs3ipc_os_EventMaskType event, const char *name,
			    uint32_t timeout_msec)
{
	unsigned int ch = client->ch;
	unsigned int instance = client->instance;
	fs3ipc_StatusType ret = fs3ipc_os_WaitEventTimeout(wait_obj, event,
		timeout_msec);

	switch (ret) {
	case fs3ipc_StatusType_OK:
		log_debug("[%u] CH-%u: %s event set", instance, ch, name);
		fs3ipc_os_ClearEvent(wait_obj, event);
		return 1;

	case fs3ipc_StatusType_ErrorTimeout:
		log_debug("[%u] CH-%u: Waiting for %s timed out", instance, ch,
			  name);
		return 0;

	case fs3ipc_StatusType_ErrorInterrupted:
		log_error("[%u] CH-%u: Waiting for %s interrupted", instance,
			  ch, name);
		return -EINTR;

	default:
		log_error("[%u] CH-%u: Waiting for %s failed: %u", instance, ch,
			  name, ret);
	}

	return -EIO;
}

/**
 * handle_timeout() - Handle timeout in read/write operation.
 * @client: FS3IPC client.
 * @name: Operation name.
 * @flags: File status flags.
 *
 * Return:
 * * -EAGAIN - Resource temporarily unavailable.
 * * -EIO    - General input/output error.
 */
static int handle_timeout(const struct fs3ipc_client *client, const char *name,
			  int flags)
{
	if (flags & O_NONBLOCK) {
		log_info("[%u] CH-%u: %s() needs to wait; non-blocking mode reqd",
			 client->instance, client->ch, name);
		return -EAGAIN;
	}

	log_error("[%u] CH-%u: Timeout in blocking mode should never happen",
		  client->instance, client->ch);

	return -EIO;
}

/**
 * fs3ipc_dev_timed_read() - FS3IPC channel device read operation with timeout.
 * @client: FS3IPC client.
 * @msg: To store read message.
 * @length: Length of message in bytes.
 * @timeout: Timeout in msecs.
 *
 * It is assumed that the arguments are valid and @msg is of at least @length
 * byte sized.
 *
 * Return:
 * * > 0    - On success, number of bytes read.
 * * 0      - On timeout and no message received.
 * * -EINTR - Waiting interrupted.
 * * -EIO   - General input/output error.
 */
static int fs3ipc_dev_timed_read(const struct fs3ipc_client *client, char *msg,
				 size_t length, uint32_t timeout)
{
	int event_set = 0;
	unsigned int ch = client->ch;
	unsigned int instance = client->instance;
	uint32_t msg_len = FS3_IPC_MAX_MSG_SIZE;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const struct fs3ipc_client_ltcfg *ccfg = fs3ipc_app_get_client_cfg(client);

	while ((ret = fs3ipc_app_read_msg(client, msg, &msg_len)) ==
	       fs3ipc_StatusType_BufferEmpty) {
		event_set = dev_wait_timeout(client, ccfg->osCfg, ccfg->rxEvent,
					     "Rx-Q msg", timeout);
		if (event_set <= 0)
			return event_set;
	}

	if (ret != fs3ipc_StatusType_OK) {
		log_error("[%u] CH-%u: Reading msg failed: %u", instance,
			  ch, ret);
		return -EIO;
	}

	if (msg_len == 0u) {
		log_error("[%u] CH-%u: Msg of zero length received", instance,
			  ch);
		return -EIO;
	}

	if (msg_len > length) {
		log_notice("[%u] CH-%u: Requested(%zu) len < Received(%u) len",
			   instance, ch, length, msg_len);
		msg_len = length;
	}

	return msg_len;
}

int fs3ipc_client_timed_read(int handle, char *msg, size_t length,
			     uint32_t timeout)
{
	int ret = 0;
	unsigned int ch = 0u;
	uint8_t buf[FS3_IPC_MAX_MSG_SIZE] = {0};
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		&instance, &ch);

	if (!client) {
		log_error("Invalid client handle: %u", handle);
		return -EINVAL;
	}

	if (!msg || length == 0 || length > FS3_IPC_MAX_MSG_SIZE) {
		log_error("CH-%u: Invalid arguments:- msg: %p, length: %zd", ch,
			  (void *)msg, length);
		return -EINVAL;
	}

	ret = fs3ipc_dev_timed_read(client, buf, length, timeout);
	if (ret > 0)
		memcpy(msg, buf, ret);

	return ret;
}
EXPORT_SYMBOL_GPL(fs3ipc_client_timed_read);

static ssize_t fs3ipc_dev_app_chan_read(struct file *filp, char __user *buf,
					size_t length, loff_t *f_pos)
{
	int ret = 0;
	unsigned int ch = 0u;
	uint32_t timeout = 0u;
	struct dev_attr *dev_attr = NULL;
	uint8_t msg[FS3_IPC_MAX_MSG_SIZE] = {0};
	const struct fs3ipc_client *client = NULL;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;

	if (!filp || !filp->private_data) {
		log_error("NULL filp(%p) or NULL dev attr", (void *)filp);
		return -EBADF;
	}

	dev_attr = filp->private_data;
	ch = dev_attr->minor;
	instance = dev_attr->fs3ipc->fs3ipc_instance;

	if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
		log_error("[%u] CH-%u: No read access: %0x", instance, ch,
			  filp->f_flags);
		return -EBADF;
	}

	if (!buf) {
		log_error("[%u] CH-%u: NULL buf", instance, ch);
		return -EINVAL;
	}

	if (length == 0 || length > FS3_IPC_MAX_MSG_SIZE) {
		log_notice("[%u] CH-%u: Invalid msg size: %zu ", instance, ch,
			   length);
		/*
		 * Allow applications to call read() with msg size >
		 * FS3_IPC_MAX_MSG_SIZE till the behavior from applications is
		 * fixed.
		 * TODO: Return error once applications fix their behavior.
		 */
	}

	client = fs3ipc_get_client(dev_attr->client_handle, NULL, NULL);
	if (!client) {
		log_error("[%u] CH-%u: Invalid client handle", instance, ch);
		return -EIO;
	}

	timeout = (filp->f_flags & O_NONBLOCK) ? 0u : FS3IPC_INFINITE_TIMEOUT;
	ret = fs3ipc_dev_timed_read(client, msg, length, timeout);
	if (ret == 0)
		return handle_timeout(client, __func__, filp->f_flags);
	if (ret > 0 && copy_to_user(buf, msg, ret)) {
		log_error("[%u] CH-%u: Copying to user failed: msg length - %d",
			  instance, ch, ret);
		return -EFAULT;
	}

	return ret;
}

/**
 * fs3ipc_dev_timed_write() - FS3IPC channel device write operation with timeout.
 * @client: FS3IPC client.
 * @msg: Message to write.
 * @length: Length of message in bytes.
 * @timeout: Timeout in msecs.
 *
 * It is assumed that the arguments are valid and @msg is of at least @length
 * byte sized.
 *
 * Return:
 * * > 0    - On success, number of bytes written.
 * * 0      - On timeout and no space to write.
 * * -EINTR - Waiting interrupted.
 * * -EIO   - General input/output error.
 */
static int fs3ipc_dev_timed_write(const struct fs3ipc_client *client,
				  const char *msg, size_t length,
				  uint32_t timeout)
{
	int event_set = 0;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const struct fs3ipc_client_ltcfg *ccfg = fs3ipc_app_get_client_cfg(client);

	while ((ret = fs3ipc_app_write_msg(client, msg, length)) ==
	       fs3ipc_StatusType_BufferFull) {
		event_set = dev_wait_timeout(client, ccfg->osCfg, ccfg->txevent,
					     "Tx-Q space", timeout);
		if (event_set <= 0)
			return event_set;
	}

	if (ret != fs3ipc_StatusType_OK) {
		log_error("[%u] CH-%u: Writing msg failed: %u", client->instance,
			  client->ch, ret);
		return -EIO;
	}

	return length;
}

int fs3ipc_client_timed_write(int handle, const char *msg, size_t length,
			      uint32_t timeout)
{
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		NULL, NULL);

	if (!client) {
		log_error("Invalid client handle: %u", handle);
		return -EINVAL;
	}

	if (!msg || length == 0 || length > FS3_IPC_MAX_MSG_SIZE) {
		log_error("[%u] CH-%u: Invalid arguments:- msg: %p, length: %zu",
			  client->instance, client->ch, (void *)msg, length);
		return -EINVAL;
	}

	return fs3ipc_dev_timed_write(client, msg, length, timeout);
}
EXPORT_SYMBOL_GPL(fs3ipc_client_timed_write);

static ssize_t fs3ipc_dev_app_chan_write(struct file *filp, const char __user *buf,
					 size_t length, loff_t *f_pos)
{
	int ret = 0;
	unsigned int ch = 0u;
	uint32_t timeout = 0u;
	struct dev_attr *dev_attr = NULL;
	uint8_t msg[FS3_IPC_MAX_MSG_SIZE] = {0};
	const struct fs3ipc_client *client = NULL;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;

	if (!filp || !filp->private_data) {
		log_error("NULL filp(%p) or NULL dev attr", (void *)filp);
		return -EBADF;
	}

	dev_attr = filp->private_data;
	ch = dev_attr->minor;
	instance = dev_attr->fs3ipc->fs3ipc_instance;

	if ((filp->f_flags & O_ACCMODE) == O_RDONLY) {
		log_error("[%u] CH-%u: No write access: %0x", instance, ch,
			  filp->f_flags);
		return -EBADF;
	}

	if (!buf || length == 0 || length > FS3_IPC_MAX_MSG_SIZE) {
		log_error("[%u] CH-%u: Invalid arguments:- buf: %p, length: %zu",
			  instance, ch, (void __user *)buf, length);
		return -EINVAL;
	}

	client = fs3ipc_get_client(dev_attr->client_handle, NULL, NULL);
	if (!client) {
		log_error("[%u] CH-%u: Invalid client handle", instance, ch);
		return -EIO;
	}

	if (copy_from_user(msg, buf, length)) {
		log_error("[%u] CH-%u: Copying from user failed", instance, ch);
		return -EFAULT;
	}

	timeout = (filp->f_flags & O_NONBLOCK) ? 0u : FS3IPC_INFINITE_TIMEOUT;
	ret = fs3ipc_dev_timed_write(client, msg, length, timeout);

	return (ret == 0) ? handle_timeout(client, __func__, filp->f_flags) : ret;
}

static __poll_t fs3ipc_dev_app_chan_poll(struct file *filp,
					 struct poll_table_struct *pt)
{
	__poll_t mask = 0;
	unsigned int ch = 0u;
	fs3ipc_u32 info = 0u;
	struct dev_attr *dev_attr = NULL;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;

	if (!filp || !filp->private_data) {
		log_error("NULL filp(%p) or NULL dev attr", (void *)filp);
		return EPOLLERR;
	}

	dev_attr = filp->private_data;
	ch = dev_attr->minor;
	instance = dev_attr->fs3ipc->fs3ipc_instance;
	poll_wait(filp, dev_attr->wq, pt);

	ret = fs3ipc_get_rxq_msg_pending(dev_attr->client_handle, &info);
	if (ret == fs3ipc_StatusType_OK) {
		if (info > 0)
			mask = EPOLLIN | EPOLLRDNORM;
		else
			log_debug("[%u] CH-%u: No pending msgs in Rx-Q: %u",
				  instance, ch, info);
	} else {
		log_error("[%u] CH-%u: Polling for Rx-Q msg failed: %u", instance,
			  ch, ret);
		mask = EPOLLERR;
	}

	ret = fs3ipc_get_txq_free_msg_space(dev_attr->client_handle, &info);
	if (ret == fs3ipc_StatusType_OK) {
		if (info > 0)
			mask |= EPOLLOUT | EPOLLWRNORM;
		else
			log_debug("[%u] CH-%u: No free space in Tx-Q: %u",
				  instance, ch, info);
	} else {
		log_error("[%u] CH-%u: Polling for Tx-Q space failed: %u",
			  instance, ch, ret);
		mask |= EPOLLERR;
	}

	return mask;
}

/**
 * fs3ipc_dev_open() - FS3IPC channel device open operation.
 * @instance: FS3IPC instance.
 * @ch: Channel.
 * @handle: Client handle.
 *
 * Return:
 * * 0      - Success.
 * * -EBUSY - Device or resource busy.
 * * -EIO   - General input/output error.
 */
static int fs3ipc_dev_open(unsigned int instance, unsigned int ch,
			   unsigned int handle)
{
	fs3ipc_StatusType ret = fs3ipc_open_ch(handle);

	if (ret == fs3ipc_StatusType_ErrorClientState) {
		log_error("[%u] CH-%u: Already opened", instance, ch);
		return -EBUSY;
	}
	if (ret != fs3ipc_StatusType_OK) {
		log_error("[%u] CH-%u: Opening failed: %u", instance, ch, ret);
		return -EIO;
	}

	log_info("CH-%u: Opened", ch);

	return 0;
}

int fs3ipc_client_open(unsigned int instance, unsigned int ch)
{
	int ret = 0;
	unsigned int handle = FS3IPC_CLIENT_HANDLE_COUNT;
	unsigned int dev_idx = ch - FS3IPC_INTERNAL_CHANNELS;
	struct fs3ipc_uart *fs3ipc = fs3ipc_get(instance);

	if (!fs3ipc || dev_idx >= fs3ipc->app_chan_count) {
		log_error("Invalid arguments:- instance: %u, channel: %u",
			  instance, ch);
		return -EINVAL;
	}

	handle = fs3ipc->dev_attr[dev_idx].client_handle;
	ret = fs3ipc_dev_open(instance, ch, handle);

	return (ret == 0) ? handle : ret;
}
EXPORT_SYMBOL_GPL(fs3ipc_client_open);

static int fs3ipc_dev_app_chan_open(struct inode *inode, struct file *filp)
{
	unsigned int minor = 0u;
	struct dev_attr *dev_attr = NULL;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;

	if (!inode || !filp) {
		log_error("Invalid arguments:- inode: %p, filp: %p",
			  (void *)inode, (void *)filp);
		return -EBADF;
	}

	dev_attr = container_of(inode->i_cdev, struct dev_attr, cdev);
	instance = dev_attr->fs3ipc->fs3ipc_instance;
	minor = iminor(inode);
	if (minor != dev_attr->minor) {
		log_error("[%u] CH-%u: Channel number mismatch: %u", instance,
			  minor, (unsigned int)dev_attr->minor);
		return -EIO;
	}

	filp->private_data = dev_attr;

	return fs3ipc_dev_open(instance, minor, dev_attr->client_handle);
}

int fs3ipc_client_close(int handle)
{
	unsigned int ch = 0u;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		&instance, &ch);

	if (!client) {
		log_error("Invalid client handle: %u", handle);
		return -EINVAL;
	}

	ret = fs3ipc_app_close_ch(client);
	if (ret == fs3ipc_StatusType_ErrorClientState) {
		log_error("[%u] CH-%u: Already closed", instance, ch);
		return -EINVAL;
	}
	if (ret != fs3ipc_StatusType_OK) {
		log_error("[%u] CH-%u: Closing failed: %u", instance, ch, ret);
		return -EIO;
	}

	log_info("[%u] CH-%u: Closed", instance, ch);

	return 0;
}
EXPORT_SYMBOL_GPL(fs3ipc_client_close);

static int fs3ipc_dev_app_chan_close(struct inode *inode, struct file *filp)
{
	int ret = 0;
	struct dev_attr *dev_attr = NULL;

	if (!inode || !filp) {
		log_error("Invalid arguments:- inode: %p, filp: %p",
			  (void *)inode, (void *)filp);
		return -EBADF;
	}

	dev_attr = container_of(inode->i_cdev, struct dev_attr, cdev);
	ret = fs3ipc_client_close(dev_attr->client_handle);

	return (ret == -EINVAL) ? -EIO : ret;
}

#ifdef CONFIG_FS3IPC_DEBUG
/**
 * fs3ipc_dev_debug_chan_create() -  Create debug channel device.
 * @fs3ipc: FS3IPC structure.
 *
 * It is assumed that the argument is valid.
 *
 * Return: 0 if OK otherwise a negative error code.
 */
static int fs3ipc_dev_debug_chan_create(struct fs3ipc_uart *fs3ipc)
{
	dev_t devt;
	int ret = 0;
	struct device *dev = NULL;
	unsigned int instance = fs3ipc->fs3ipc_instance;

	fs3ipc->debug_ch = fs3ipc->app_chan_count + FS3IPC_INTERNAL_CHANNELS +
		FS3IPC_DEBUG_CHANNEL_COUNT - 1;
	devt = MKDEV(MAJOR(fs3ipc->dev_no), fs3ipc->debug_ch);

	cdev_init(&fs3ipc->debug_cdev, &fs3ipc_debug_fops);
	ret = cdev_add(&fs3ipc->debug_cdev, devt, FS3IPC_DEBUG_CHANNEL_COUNT);
	if (ret < 0) {
		log_error("[%u] CH-%u: Adding debug channel's cdev failed: %d",
			  instance, fs3ipc->debug_ch, ret);
		return ret;
	}

	/* TODO: Update the device name in case of multiple FS3IPC instances. */
	dev = device_create(fs3ipc_class, NULL, devt, NULL,
			    FS3IPC_DEBUG_CHANNEL_NAME);

	if (IS_ERR(dev)) {
		ret = (int)PTR_ERR(dev);
		log_error("[%u] CH-%u: Debug channel's device creation failed: %d",
			  instance, fs3ipc->debug_ch, ret);
		cdev_del(&fs3ipc->debug_cdev);
		return ret;
	}

	return 0;
}

/**
 * fs3ipc_dev_debug_chan_destroy() - Delete debug channel device.
 * @fs3ipc: FS3IPC structure.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
static void fs3ipc_dev_debug_chan_destroy(struct fs3ipc_uart *fs3ipc)
{
	dev_t devt = MKDEV(MAJOR(fs3ipc->dev_no), fs3ipc->debug_ch);

	device_destroy(fs3ipc_class, devt);
	cdev_del(&fs3ipc->debug_cdev);
}
#else /* !(CONFIG_FS3IPC_DEBUG) */

static inline int fs3ipc_dev_debug_chan_create(struct fs3ipc_uart *fs3ipc)
{
	(void)fs3ipc;

	return 0;
}

static inline void fs3ipc_dev_debug_chan_destroy(struct fs3ipc_uart *fs3ipc)
{
	(void)fs3ipc;
}
#endif /* CONFIG_FS3IPC_DEBUG */

/**
 * fs3ipc_dev_app_chan_create() -  Create application channel device.
 * @dev_attr: FS3IPC device attribute.
 * @fs3ipc: FS3IPC structure to which this device belongs.
 * @chan_cfg: Channel configuration for @dev_attr.
 * @minor: Device minor that is used as channel number.
 *
 * It is assumed that all the arguments are valid.
 *
 * Return: 0 if OK otherwise failure.
 */
static int fs3ipc_dev_app_chan_create(struct dev_attr *dev_attr,
				      struct fs3ipc_uart *fs3ipc,
				      const fs3ipc_app_ltchancfg_t *chan_cfg,
				      unsigned int minor)
{
	dev_t devt;
	int ret = 0;
	struct device *dev = NULL;
	unsigned int instance = fs3ipc->fs3ipc_instance;

	dev_attr->minor = minor;
	dev_attr->fs3ipc = fs3ipc;

	if (chan_cfg) {
		dev_attr->client_handle = chan_cfg->client_cfg.handle;
		dev_attr->wq = &chan_cfg->client_cfg.osCfg->wq;
		if (!dev_attr->wq) {
			log_error("[%u] CH-%u: Wait queue is not found", instance,
				  minor);
			return -EIO;
		}
		cdev_init(&dev_attr->cdev, &fs3ipc_dev_app_chan_fops);
		devt = MKDEV(MAJOR(fs3ipc->dev_no), minor);
		ret = cdev_add(&dev_attr->cdev, devt, 1);
		if (ret < 0) {
			log_error("[%u] CH-%u: Adding cdev failed: %d", instance,
				  minor, ret);
			return ret;
		}

		/* TODO: Update the device name in case of multiple FS3IPC instances. */
		dev = device_create(fs3ipc_class, NULL, devt, NULL, "%s%u",
				    FS3IPC_APP_CHANNEL_NAME, minor);
		if (IS_ERR(dev)) {
			cdev_del(&dev_attr->cdev);
			log_error("[%u] CH-%u: Device creation failed: %ld",
				  instance, minor, PTR_ERR(dev));
			return (int)PTR_ERR(dev);
		}
	} else {
		dev_attr->client_handle = FS3IPC_CLIENT_HANDLE_COUNT;
		dev_attr->wq = NULL;
		log_error("[%u] CH-%u: Not configured", instance, minor);
	}

	return 0;
}

int fs3ipc_dev_init(struct fs3ipc_uart *fs3ipc)
{
	int ret = 0;
	unsigned int i = 0u;
	char name[FS3IPC_NAME_LEN];
	struct dev_attr *dev_attr = NULL;
	unsigned int minor = FS3IPC_INTERNAL_CHANNELS;
	unsigned int instance = fs3ipc->fs3ipc_instance;
	const fs3ipc_app_ltcfg_t *app_cfg = &fs3ipc_app_appConfigs[instance];

	fs3ipc->app_chan_count = app_cfg->channelCount - FS3IPC_INTERNAL_CHANNELS;
	scnprintf(name, FS3IPC_NAME_LEN, "%s/%u", FS3IPC_UART_DRV_NAME, instance);
	ret = alloc_chrdev_region(&fs3ipc->dev_no, FS3IPC_INTERNAL_CHANNELS,
				  fs3ipc->app_chan_count +
				  FS3IPC_DEBUG_CHANNEL_COUNT, name);
	if (ret < 0) {
		log_error("[%u] alloc_chrdev_region() failed: %d", instance, ret);
		return ret;
	}

	fs3ipc->dev_attr = devm_kcalloc(&fs3ipc->serdev->dev,
					fs3ipc->app_chan_count,
					sizeof(struct dev_attr), GFP_KERNEL);
	if (!fs3ipc->dev_attr) {
		ret = -ENOMEM;
		goto err_dev_attr_alloc;
	}

	dev_attr = fs3ipc->dev_attr;
	for (i = 0u; i < fs3ipc->app_chan_count; i++, minor++) {
		ret = fs3ipc_dev_app_chan_create(&dev_attr[i], fs3ipc,
						 app_cfg->ltChannelCfg[minor],
						 minor);
		if (ret)
			goto err_app_chan_device_create;
	}

	ret = fs3ipc_dev_debug_chan_create(fs3ipc);
	if (ret == 0)
		return 0;

	i = fs3ipc->app_chan_count;

err_app_chan_device_create:
	while (i--) {
		if (dev_attr[i].client_handle >= FS3IPC_CLIENT_HANDLE_COUNT)
			continue;
		fs3ipc_dev_app_chan_destroy(&dev_attr[i]);
	}

err_dev_attr_alloc:
	unregister_chrdev_region(fs3ipc->dev_no, fs3ipc->app_chan_count +
				 FS3IPC_DEBUG_CHANNEL_COUNT);

	return ret;
}

/**
 * fs3ipc_dev_app_chan_destroy() - Delete application channel device.
 * @dev_attr: FS3IPC device attribute.
 *
 * It is assumed that the argument is valid.
 *
 * Return: 0 if OK otherwise a negative error code.
 */
static void fs3ipc_dev_app_chan_destroy(struct dev_attr *dev_attr)
{
	unsigned int minor = dev_attr->minor;
	struct fs3ipc_uart *fs3ipc = dev_attr->fs3ipc;
	dev_t devt = MKDEV(MAJOR(fs3ipc->dev_no), minor);

	device_destroy(fs3ipc_class, devt);
	cdev_del(&dev_attr->cdev);
}

void fs3ipc_dev_cleanup(struct fs3ipc_uart *fs3ipc)
{
	unsigned int i = 0u;
	struct dev_attr *dev_attr = fs3ipc->dev_attr;

	for (i = 0u; i < fs3ipc->app_chan_count; i++) {
		if (dev_attr[i].client_handle >= FS3IPC_CLIENT_HANDLE_COUNT)
			continue;
		fs3ipc_dev_app_chan_destroy(&dev_attr[i]);
	}

	fs3ipc_dev_debug_chan_destroy(fs3ipc);
	unregister_chrdev_region(fs3ipc->dev_no, fs3ipc->app_chan_count +
				 FS3IPC_DEBUG_CHANNEL_COUNT);
}
