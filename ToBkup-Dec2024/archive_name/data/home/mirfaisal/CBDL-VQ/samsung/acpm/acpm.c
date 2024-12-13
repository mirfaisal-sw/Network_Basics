/*
 * Samsung ExynosAuto SoC series ACPM driver
 *
 * exynosauto acpm functions
 *
 * Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/firmware.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/sched/clock.h>
#include <linux/module.h>

#include "acpm.h"
#include "acpm_ipc.h"
#include "fw_header/framework.h"

static int ipc_done;
static unsigned long long ipc_time_start;
static unsigned long long ipc_time_end;
static void __iomem *fvmap_base_address;

static struct acpm_info *exynos_acpm;

static int acpm_send_data(struct device_node *node, unsigned int check_id,
		struct ipc_config *config);
static struct plugin_ops *plg_ops;
struct plugin *plugins;

static void pr_plugin_info(void)
{
	const char *fw_name = NULL;
	int i;

	for (i = 0; i < acpm_initdata->num_plugins; i++) {
		if (plugins[i].is_attached == 1 && plugins[i].stay_attached == 1) {
			plg_ops = (struct plugin_ops *)(acpm_srambase + plugins[i].plugin_ops);
			fw_name = (const char *)(acpm_srambase + plugins[i].fw_name);

			pr_info("Plugin%d - %-10s  %-25s  %s\n", i, fw_name, plg_ops->info.build_version, plg_ops->info.build_time);
		}
	}
}

void *get_fvmap_base(void)
{
	return fvmap_base_address;
}
EXPORT_SYMBOL_GPL(get_fvmap_base);

static int plugins_init(struct device_node *np)
{
	int i, len, ret = 0;
	const char *fw_name = NULL;
	const __be32 *prop;
	unsigned int offset;

	plugins = (struct plugin *)(acpm_srambase + acpm_initdata->plugins);
	pr_plugin_info();

	for (i = 0; i < acpm_initdata->num_plugins; i++) {
		if (plugins[i].is_attached == 1 && plugins[i].stay_attached == 1) {
			fw_name = (const char *)(acpm_srambase + plugins[i].fw_name);
			if (plugins[i].fw_name && fw_name &&
					(!strncmp(fw_name, "DVFS", 4) || !strncmp(fw_name, "dvfs", 4))) {

				fvmap_base_address = acpm_srambase + (plugins[i].base_addr & ~0x1);
				prop = of_get_property(exynos_acpm->dev->of_node, "fvmap_offset", &len);
				if (prop) {
					offset = be32_to_cpup(prop);
					fvmap_base_address += offset;
					dev_info(exynos_acpm->dev, "acpm_sram_base: 0x%lx\n", (unsigned long)acpm_srambase);
					dev_info(exynos_acpm->dev, "plugins[i].base_addr & ~0x1: 0x%x\n", (plugins[i].base_addr & ~0x1));
					dev_info(exynos_acpm->dev, "fvmap_base_address: 0x%lx\n", (unsigned long)fvmap_base_address);
				}
			}
		}
	}

	return ret;
}

static int debug_log_level_get(void *data, unsigned long long *val)
{
	return 0;
}

static int debug_log_level_set(void *data, unsigned long long val)
{
	acpm_fw_log_level((unsigned int)val);
	return 0;
}

static int debug_ipc_loopback_test_get(void *data, unsigned long long *val)
{
	struct ipc_config config;
	int ret = 0;
	unsigned int cmd[4] = {0, };

	config.cmd = cmd;
	config.cmd[0] = (1 << ACPM_IPC_PROTOCOL_TEST);
	config.cmd[0] |= 0x3 << ACPM_IPC_PROTOCOL_ID;

	config.response = true;
	config.indirection = false;

	ipc_time_start = sched_clock();
	ret = acpm_send_data(exynos_acpm->dev->of_node, 3, &config);
	ipc_time_end = sched_clock();

	if (!ret)
		*val = ipc_time_end - ipc_time_start;
	else
		*val = 0;

	config.cmd = NULL;

	return 0;
}

static int debug_ipc_loopback_test_set(void *data, unsigned long long val)
{
	return 0;
}

static int debug_plugin_info_get(void *data, unsigned long long *val)
{
	val = NULL;
	pr_plugin_info();
	return 0;
}

static int debug_plugin_info_set(void *data, unsigned long long val)
{
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(debug_log_level_fops,
		debug_log_level_get, debug_log_level_set, "%llu\n");
DEFINE_SIMPLE_ATTRIBUTE(debug_ipc_loopback_test_fops,
		debug_ipc_loopback_test_get, debug_ipc_loopback_test_set, "%llu\n");
DEFINE_SIMPLE_ATTRIBUTE(debug_plugin_info_fops,
		debug_plugin_info_get, debug_plugin_info_set, "%llu\n");

static void acpm_debugfs_init(struct acpm_info *acpm)
{
	struct dentry *den;

	den = debugfs_create_dir("acpm_framework", NULL);
	debugfs_create_file("ipc_loopback_test", 0644, den, acpm, &debug_ipc_loopback_test_fops);
	debugfs_create_file("log_level", 0644, den, NULL, &debug_log_level_fops);
	debugfs_create_file("plugin_info", 0644, den, acpm, &debug_plugin_info_fops);
}

void *memcpy_align_4(void *dest, const void *src, unsigned int n)
{
	unsigned int *dp = dest;
	const unsigned int *sp = src;
	int i;

	if ((n % 4))
		BUG();

	n = n >> 2;

	for (i = 0; i < n; i++)
		*dp++ = *sp++;

	return dest;
}

void acpm_enter_wfi(void)
{
	struct ipc_config config;
	int ret = 0;
	unsigned int cmd[4] = {0, };

	if (exynos_acpm->enter_wfi)
		return;

	config.cmd = cmd;
	config.response = true;
	config.indirection = false;
	config.cmd[0] = 1 << ACPM_IPC_PROTOCOL_STOP;

	ret = acpm_send_data(exynos_acpm->dev->of_node, ACPM_IPC_PROTOCOL_STOP, &config);

	config.cmd = NULL;

	if (ret) {
		pr_err("[ACPM] acpm enter wfi fail!!\n");
	} else {
		pr_err("[ACPM] wfi done\n");
		exynos_acpm->enter_wfi++;
	}
}

u32 exynos_get_peri_timer_icvra(void)
{
	return (EXYNOS_PERI_TIMER_MAX - __raw_readl(exynos_acpm->timer_base + EXYNOS_TIMER_APM_TCVR)) & EXYNOS_PERI_TIMER_MAX;
}

void exynos_acpm_timer_clear(void)
{
	writel(exynos_acpm->timer_cnt, exynos_acpm->timer_base + EXYNOS_TIMER_APM_TCVR);
}

void exynos_acpm_reboot(void)
{
	acpm_ipc_set_waiting_mode(BUSY_WAIT);
	acpm_enter_wfi();
}
EXPORT_SYMBOL_GPL(exynos_acpm_reboot);

static int acpm_send_data(struct device_node *node, unsigned int check_id,
		struct ipc_config *config)
{
	unsigned int channel_num = 0;
	unsigned int size = 0;
	int ret = 0;
	int timeout_flag;
	unsigned int id = 0;

	if (!acpm_ipc_request_channel(node, NULL,
				&channel_num, &size)) {
		ipc_done = -1;

		ipc_time_start = sched_clock();
		ret = acpm_ipc_send_data(channel_num, config);

		if (config->cmd[0] & ACPM_IPC_PROTOCOL_DP_CMD) {
			id = config->cmd[0] & ACPM_IPC_PROTOCOL_IDX;
			id = id	>> ACPM_IPC_PROTOCOL_ID;
			ipc_done = id;
		} else if (config->cmd[0] & (1 << ACPM_IPC_PROTOCOL_STOP)) {
			ipc_done = ACPM_IPC_PROTOCOL_STOP;
		} else if (config->cmd[0] & (1 << ACPM_IPC_PROTOCOL_TEST)) {
			id = config->cmd[0] & ACPM_IPC_PROTOCOL_IDX;
			id = id	>> ACPM_IPC_PROTOCOL_ID;
			ipc_done = id;
		} else {
			id = config->cmd[0] & ACPM_IPC_PROTOCOL_IDX;
			id = id	>> ACPM_IPC_PROTOCOL_ID;
			ipc_done = id;
		}

		/* Response interrupt waiting */
		UNTIL_EQUAL(ipc_done, check_id, timeout_flag);

		if (timeout_flag)
			ret = -ETIMEDOUT;

		acpm_ipc_release_channel(node, channel_num);
	} else {
		pr_err("%s ipc request_channel fail, id:%u, size:%u\n",
				__func__, channel_num, size);
		ret = -EBUSY;
	}

	return ret;
}

static int acpm_probe(struct platform_device *pdev)
{
	struct acpm_info *acpm;
	struct device_node *node = pdev->dev.of_node;
	struct resource *res;
	int ret = 0;

	dev_info(&pdev->dev, "acpm probe\n");

	if (!node) {
		dev_err(&pdev->dev, "driver doesn't support non-dt devices\n");
		return -ENODEV;
	}

	acpm = devm_kzalloc(&pdev->dev,
			sizeof(struct acpm_info), GFP_KERNEL);
	if (IS_ERR(acpm))
		return PTR_ERR(acpm);
	if (!acpm)
		return -ENOMEM;

	acpm->dev = &pdev->dev;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "timer_apm");
	acpm->timer_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(acpm->timer_base))
		dev_warn(&pdev->dev, "could not find timer base addr\n");

	if (of_property_read_u32(node, "peritimer-cnt", &acpm->timer_cnt))
		dev_warn(&pdev->dev, "No matching property: peritiemr_cnt\n");

	exynos_acpm = acpm;

	acpm_debugfs_init(acpm);

	exynos_acpm_timer_clear();

	acpm_ipc_set_waiting_mode(BUSY_WAIT);
	ret = plugins_init(node);

	dev_info(&pdev->dev, "acpm probe done.\n");

	return ret;
}

static int acpm_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id acpm_ipc_match[] = {
	{ .compatible = "samsung,exynos-acpm-ipc" },
	{},
};
MODULE_DEVICE_TABLE(of, acpm_ipc_match);

static struct platform_driver samsung_acpm_ipc_driver = {
	.probe	= acpm_ipc_probe,
	.remove	= acpm_ipc_remove,
	.driver	= {
		.name = "exynos-acpm-ipc",
		.owner	= THIS_MODULE,
		.of_match_table	= acpm_ipc_match,
	},
};

static const struct of_device_id acpm_match[] = {
	{ .compatible = "samsung,exynos-acpm" },
	{},
};
MODULE_DEVICE_TABLE(of, acpm_match);

static struct platform_driver samsung_acpm_driver = {
	.probe	= acpm_probe,
	.remove	= acpm_remove,
	.driver	= {
		.name = "exynos-acpm",
		.owner	= THIS_MODULE,
		.of_match_table	= acpm_match,
	},
};

static int exynos_acpm_init(void)
{
	platform_driver_register(&samsung_acpm_ipc_driver);
	platform_driver_register(&samsung_acpm_driver);
	return 0;
}
postcore_initcall_sync(exynos_acpm_init);

static void exynos_acpm_exit(void)
{
	platform_driver_unregister(&samsung_acpm_ipc_driver);
	platform_driver_unregister(&samsung_acpm_driver);
}
module_exit(exynos_acpm_exit);

MODULE_LICENSE("GPL");