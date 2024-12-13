// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - sfi cli driver support to communicate with the SFI CPU
 */

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_reserved_mem.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/string.h>
#include <linux/uaccess.h> /* copy_from_user */
#include <linux/slab.h> /* kmalloc */
#include <asm/io.h> /*ioremap */
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <soc/samsung/sfi_ipc_ctrl.h>
#include <linux/reboot.h>

#define SFI_IPC_CLKMON_NAME "clkmon"


static int clkmon_count = 0;
static int clkmon_channel_id = 0;

static int clkmon_tx_data_buf[4];
static int clkmon_rx_data_buf[4];

static struct sfi_ipc_buf_info clkmon_tx_buf;
static struct sfi_ipc_buf_info clkmon_rx_buf;

static char sfi_clkmon_msg_buff[4];
static int sfi_clkmon_read_skip;

static void sfi_clkmon_ipc_rx_cb(struct sfi_ipc_buf_info *arg)
{
    clkmon_count++;
}

static ssize_t sfi_clkmon_ioctl(struct file *file, char __user *data, size_t count, loff_t *ppos)
{
	int i;
    int ret;
	ssize_t un_copied;

	if (sfi_clkmon_read_skip) {
		sfi_clkmon_read_skip = 0;
		return 0;
	}

	for (i = 0; i < 4; i++)
		sfi_clkmon_msg_buff[i] = 0;
    
    clkmon_tx_buf.addr = (void*)clkmon_tx_data_buf;
    clkmon_tx_buf.size = 4;
    ret = sfi_ipc_send(clkmon_channel_id, &clkmon_tx_buf);
	un_copied = copy_to_user(data, sfi_clkmon_msg_buff, 4);

	sfi_clkmon_read_skip = 1;

	return 4;
}

struct file_operations sfi_clkmon_fops = {
	.owner = THIS_MODULE,
	.read = sfi_clkmon_ioctl,
};

static struct miscdevice sfi_clkmon_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "sfi_clkmon",
	.fops		= &sfi_clkmon_fops,
};

static int sfi_clkmon_probe(struct platform_device *pdev)
{
    int result;
    clkmon_rx_buf.addr = (void*)clkmon_rx_data_buf;
    clkmon_rx_buf.size = 4;
	clkmon_channel_id = sfi_ipc_open(SFI_IPC_CLKMON_NAME, sfi_clkmon_ipc_rx_cb,
									&(clkmon_rx_buf));

    result = misc_register(&sfi_clkmon_miscdev);
	if (result) {
		dev_err(&pdev->dev, "failed to register misc driver\n");
		return result;
	}
	return 0;
}

static int sfi_clkmon_remove(struct platform_device *pdev)
{
	sfi_ipc_close(SFI_IPC_CLKMON_NAME);

	misc_deregister(&sfi_clkmon_miscdev);

    return 0;
}

int my_notifier(struct notifier_block *nb, unsigned long action, void *data) {
    int ret;
    clkmon_tx_buf.addr = (void*)clkmon_tx_data_buf;
    clkmon_tx_buf.size = 4;
    ret = sfi_ipc_send(clkmon_channel_id, &clkmon_tx_buf);
    
    return NOTIFY_OK;
}



static const struct of_device_id of_sfi_clkmon_ids[] = {
	{
		.compatible = "samsung,exynos-sfi-clkmon",
		.data = NULL,
	}, {
		/* sentinel */
	}
};

static struct platform_driver sfi_clkmon_driver = {
	.probe		= sfi_clkmon_probe,
    .remove		= sfi_clkmon_remove,
	.driver = {
		.name	= "sfi_clkmon",
		.of_match_table = of_match_ptr(of_sfi_clkmon_ids),
	}
};

static struct notifier_block notifier = {
    .notifier_call = my_notifier,
    .next = NULL,
    .priority = 0
};

static int __init sfi_clkmon_init(void)
{
	pr_info("[sfi_clkmon] %s\n", __func__);
    register_reboot_notifier(&notifier);

	return platform_driver_register(&sfi_clkmon_driver);
}

static void __exit sfi_clkmon_exit(void)
{
	pr_info("[sfi_clkmon] %s\n", __func__);

	platform_driver_unregister(&sfi_clkmon_driver);
}

late_initcall(sfi_clkmon_init);
module_exit(sfi_clkmon_exit);

MODULE_LICENSE("GPL v2");
