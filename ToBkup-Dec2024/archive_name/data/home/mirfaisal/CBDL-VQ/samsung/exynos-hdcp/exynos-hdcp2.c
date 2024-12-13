// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/smc.h>
#include <asm/cacheflush.h>
#include <soc/samsung/exynos-smc.h>
#include "exynos-hdcp2.h"
#include "exynos-hdcp2-teeif.h"
#include "exynos-hdcp2-log.h"
#include "exynos-hdcp2-dplink-if.h"
#include "exynos-hdcp2-dplink.h"
#include "exynos-hdcp2-dplink-state.h"
#include "exynos-hdcp2-dplink-selftest.h"
#include "exynos-hdcp2-dplink-auth.h"
#include "exynos-hdcp2-dplink-inter.h"
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>
#if defined(CONFIG_HDCP2_EMULATION_MODE)
#include <exynos-hdcp2-crypto.h>
#endif

#define EXYNOS_HDCP_DEV_NAME   "hdcp2"

static struct miscdevice hdcp;
static DEFINE_MUTEX(hdcp_lock);
enum hdcp_result hdcp_link_ioc_authenticate(void);
extern enum dp_state dp_hdcp_state[MAX_NUM_OF_HDCP];
extern enum auth_state auth_proc_state[MAX_NUM_OF_HDCP];

#define HDCP_SYSFS_DEBUG
#ifdef HDCP_SYSFS_DEBUG
#define SMC_DRM_EXDISP_PROTECTED_CHECK		0x82002103
#define SMC_SET_STREAM_TYPE			0x82004021

static ssize_t show_hdcp(struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret = 0;
	return ret;
}

static ssize_t store_hdcp(struct device *dev, struct device_attribute *attr,
							const char *buf, size_t count)
{
	int cmd, node=0, type=0;
	int err;

	if ((sscanf(buf, "%d %d %d", &cmd, &node, &type) == 0)
		|| (node != 0 && node != 1) || (type != 0 && type != 1))
		return -EINVAL;

	switch (cmd) {
	case 0:
		HDCP_DP_CONN_STATE(node, dp_hdcp_state[node]);
		HDCP_PROC_STATE(node, auth_proc_state[node]);
		break;
	case 1:
		hdcp_info("hdcp_sysfs: SMC_SET_STREAM_TYPE node(%d) type(%d)\n", node, type);
		err = exynos_smc(SMC_SET_STREAM_TYPE, node, type, 0);
		hdcp_info("hdcp_sysfs: smc return: 0x%X\n", err);
		break;
	case 2:
		hdcp_info("hdcp_sysfs: SMC_DRM_HDCP_AUTH_INFO DISABLE\n");
		err = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, 2, 0, 0);
		hdcp_info("hdcp_sysfs: SMC_DRM_EXDISP_PROTECTED_CHECK\n");
		err = exynos_smc(SMC_DRM_EXDISP_PROTECTED_CHECK, node, 0, 0);
		hdcp_info("hdcp_sysfs: smc return: 0x%X\n", err);
		break;
	case 3:
		hdcp_info("hdcp_sysfs: SMC_DRM_HDCP_AUTH_INFO DISALBE\n");
		err = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, 2, 0, 0);
		break;
	default:
		hdcp_info(" hdcp_sysfs test command ====================\n");
		hdcp_info(" echo [cmd][node][type] > /sys/devices/platform/hdcp@[node]/hdcp_sysfs\n");
		hdcp_info(" cmd[1] : SMC_SET_STREAM_TYPE1\n");
		hdcp_info(" cmd[2] : SMC_DRM_HDCP_AUTH_INFO DP_DISABLE + SMC_DRM_EXDISP_PROTECTED_CHECK\n");
		hdcp_info(" cmd[3] : SMC_DRM_HDCP_AUTH_INFO DP_DISABLE\n");
		hdcp_info("============================================\n");
		break;
	}

	return count;
}

static DEVICE_ATTR(hdcp_sysfs, S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP, show_hdcp, store_hdcp);

static inline int create_hdcp_sys_file(struct device *dev)
{
	return device_create_file(dev, &dev_attr_hdcp_sysfs);
}

static inline void remove_hdcp_sys_file(struct device *dev)
{
	device_remove_file(dev, &dev_attr_hdcp_sysfs);
}
#endif //HDCP_SYSFS_DEBUG

struct hdcp_ctx g_hdcp[MAX_NUM_OF_HDCP];
static uint32_t inst_num;

static int hdcp_open(struct inode *inode, struct file *file)
{
	struct miscdevice *miscdev = file->private_data;
	struct device *dev = miscdev->this_device;
	struct hdcp_info *info;

	hdcp_info("Exynos HDCP driver open\n");

	info = kzalloc(sizeof(struct hdcp_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->dev = dev;
	file->private_data = info;

	mutex_lock(&hdcp_lock);
	inst_num++;
	/* todo: hdcp device initialize ? */
	mutex_unlock(&hdcp_lock);

	return 0;
}

static int hdcp_release(struct inode *inode, struct file *file)
{
	struct hdcp_info *info = file->private_data;

	/* disable drm if we were the one to turn it on */
	mutex_lock(&hdcp_lock);
	inst_num--;
	/* todo: hdcp device finalize ? */
	mutex_unlock(&hdcp_lock);

	kfree(info);
	hdcp_info("Exynos HDCP driver released\n");

	return 0;
}

static void exynos_hdcp_worker(struct work_struct *work)
{
	int ret;

	struct hdcp_ctx *h_ctx =
		container_of(work, struct hdcp_ctx, work.work);

	if (dp_hdcp_state[h_ctx->node] == DP_DISCONNECT) {
		hdcp_err("dp_disconnected\n");
		return;
	}

	ret = hdcp_dplink_auth_check(h_ctx->node, HDCP_DRM_ON);

	hdcp_info("Exynos HDCP driver worker node(%d) ret(%d)\n",
		h_ctx->node, ret);
}

static irqreturn_t exynos_hdcp_irq_handler(int irq, void *dev_id)
{
	struct hdcp_ctx *h_ctx = dev_id;

	hdcp_debug("Exynos HDCP driver irq#%d handler of node(%d) called\n",
		h_ctx->irq, h_ctx->node);

	if (h_ctx->irq == irq) {
		h_ctx->enable = 1;
	}

	if (h_ctx->enable) {
		if (dp_hdcp_state[h_ctx->node] == DP_HDCP_READY) {
			schedule_delayed_work(&h_ctx->work, msecs_to_jiffies(0));
		} else {
			schedule_delayed_work(&h_ctx->work, msecs_to_jiffies(2500));
		}
	}

	return IRQ_HANDLED;
}

static int exynos_hdcp_probe(struct platform_device *pdev)
{
	struct hdcp_ctx *h_ctx;
	struct irq_data *hdcp_irqd = NULL;
	irq_hw_number_t hwirq = 0;
	int node;
	int ret;

	ret = of_property_read_u32(pdev->dev.of_node, "channel", &node);
	if (ret) {
		dev_err(&pdev->dev,
			"Fail to get hdcp node number(%d) from dt\n", node);
		goto out;
	}

	h_ctx = &g_hdcp[node];
	h_ctx->node = node;
	h_ctx->dev = &pdev->dev;

	platform_set_drvdata(pdev, h_ctx);

	#if defined(CONFIG_HDCP2_DP_ENABLE)
	if (hdcp_dplink_init() < 0) {
		dev_err(&pdev->dev, "Fail to hdcp_dplink_init\n");
		return -EINVAL;
	}
	#endif

	ret = hdcp_tee_open(h_ctx->node);
	if (ret) {
		dev_err(&pdev->dev, "Fail to hdcp_tee_open\n");
		return -EINVAL;
	}

	h_ctx->irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
	if (!h_ctx->irq) {
		dev_err(&pdev->dev, "Fail to get irq from dt\n");
		return -EINVAL;
	}

	/* Get irq_data for secure log */
	hdcp_irqd = irq_get_irq_data(h_ctx->irq);
	if (!hdcp_irqd) {
		dev_err(&pdev->dev, "Fail to get irq_data\n");
		return -EINVAL;
	}

	/* Get hardware interrupt number */
	hwirq = irqd_to_hwirq(hdcp_irqd);
	ret = devm_request_irq(&pdev->dev, h_ctx->irq,
			exynos_hdcp_irq_handler,
			IRQF_TRIGGER_RISING, pdev->name, h_ctx);

	if (ret) {
		dev_err(&pdev->dev,
			"Fail to request IRQ handler. err(%d) irq(%d)\n",
			ret, h_ctx->irq);
		return ret;
	}

	pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask;
	dma_set_mask(&pdev->dev, DMA_BIT_MASK(36));

	/* Set workqueue for Secure log as bottom half */
	INIT_DELAYED_WORK(&h_ctx->work, exynos_hdcp_worker);

	ret = exynos_smc(SMC_HDCP_NOTIFY_INTR_NUM, h_ctx->node, 0, hwirq);
	dev_info(&pdev->dev, "Exynos HDCP driver node(%d) probe done! (ret:0x%X)\n", node, ret);

	#ifdef HDCP_SYSFS_DEBUG
	if (create_hdcp_sys_file(&pdev->dev))
		dev_warn(&pdev->dev, "Failed to create sysfs for hdcp");
	#endif
out:
	return ret;
}

static int exynos_hdcp_remove(struct platform_device *pdev)
{
	struct hdcp_ctx *h_ctx = platform_get_drvdata(pdev);

	cancel_delayed_work_sync(&h_ctx->work);
	hdcp_tee_close(h_ctx->node);

	#ifdef HDCP_SYSFS_DEBUG
	remove_hdcp_sys_file(&pdev->dev);
	#endif
	dev_info(&pdev->dev, "Exynos hdcp driver removed!\n");

	return 0;
}

static void exynos_hdcp_shutdown(struct platform_device *pdev)
{
	struct hdcp_ctx *h_ctx = platform_get_drvdata(pdev);

	if (IS_ERR_OR_NULL(&pdev->dev))
		dev_err(&pdev->dev, "HDCP is NULL\n");

	disable_irq(h_ctx->irq);

	dev_info(&pdev->dev, "Shutdown\n");
}

static int exynos_hdcp_resume(struct device *dev)
{
	struct hdcp_ctx *h_ctx = dev_get_drvdata(dev);

	enable_irq(h_ctx->irq);

	hdcp_info("Resumed hdcp node(%d)\n", h_ctx->node);

	return 0;
}

static int exynos_hdcp_suspend(struct device *dev)
{
	struct hdcp_ctx *h_ctx = dev_get_drvdata(dev);
	int ret;

	/* inform suspend status to LDFW, to update dp_hdcp_auth_state */
	ret = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, DP_HDCP22_RESUMED, h_ctx->node, 0);
	dplink_clear_irqflag_all(h_ctx->node);
	disable_irq(h_ctx->irq);

	hdcp_info("Suspended hdcp node(%d)\n", h_ctx->node);

	return 0;
}

static const struct of_device_id exynos_hdcp_of_match_table[] = {
	{ .compatible = "samsung,exynos-hdcp", },
	{ },
};

static const struct dev_pm_ops exynos_hdcp_pm = {
	SET_SYSTEM_SLEEP_PM_OPS(exynos_hdcp_suspend, exynos_hdcp_resume)
};

static struct platform_driver exynos_hdcp_driver = {
	.probe = exynos_hdcp_probe,
	.remove = exynos_hdcp_remove,
	.shutdown = exynos_hdcp_shutdown,
	.driver = {
		.name = "exynos-hdcp",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(exynos_hdcp_of_match_table),
		.pm = &exynos_hdcp_pm,
	}
};

static int __init hdcp_init(void)
{
	int ret;

	hdcp_info("hdcp2 driver init\n");

	ret = misc_register(&hdcp);
	if (ret) {
		hdcp_err("hdcp can't register misc on minor=%d\n",
				MISC_DYNAMIC_MINOR);
		return ret;
	}

	hdcp_session_list_init();

	return platform_driver_register(&exynos_hdcp_driver);
}

static void __exit hdcp_exit(void)
{
	hdcp_info("Exynos hdcp driver EXIT!\n");
	misc_deregister(&hdcp);
	hdcp_session_list_destroy();
	platform_driver_unregister(&exynos_hdcp_driver);
}

#if defined(CONFIG_HDCP2_EMULATION_MODE)
static ssize_t hdcp_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	s32 value = 1;
	int8_t test_msg[] = "SHA1 test message";
	uint8_t temp_buf[30];
	uint32_t id = 0;

	if (count == sizeof(s32)) {
		if (copy_from_user(&value, buf, sizeof(s32)))
			return -EFAULT;
	}

	dp_hdcp_state = DP_CONNECT;

	if (value == 0) {
		/* dummy call for TEM */
		hdcp_calc_sha1(temp_buf, (uint8_t *)test_msg, sizeof(test_msg));
		hdcp_dplink_set_paring_available(id);
		hdcp_dplink_set_hprime_available(id);
		hdcp_dplink_set_rp_ready(id);
		hdcp_dplink_set_reauth(id);
		hdcp_dplink_set_integrity_fail(id);
		hdcp_dplink_cancel_auth(id);
		hdcp_dplink_connect_state(DP_DISCONNECT);
		hdcp_dplink_clear_all(id);
	} else {
		exynos_hdcp_irq_handler(0, NULL);
	}

	return count;
}
#endif

static const struct file_operations hdcp_fops = {
	.owner		= THIS_MODULE,
	.open		= hdcp_open,
	.release	= hdcp_release,
#if defined(CONFIG_HDCP2_EMULATION_MODE)
	.write		= hdcp_write,
#endif
};

static struct miscdevice hdcp = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= EXYNOS_HDCP_DEV_NAME,
	.fops	= &hdcp_fops,
};

module_init(hdcp_init);
module_exit(hdcp_exit);
MODULE_DESCRIPTION("Exynos Secure hdcp driver");
MODULE_AUTHOR("<hakmin_1.kim@samsung.com>");
MODULE_LICENSE("GPL");
