// SPDX-License-Identifier: GPL-2.0
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <soc/samsung/exynos_pm_qos.h>
#include <linux/slab.h>
#include <linux/sched/clock.h>

#include <soc/samsung/acpm_ipc_ctrl.h>
#if IS_ENABLED(CONFIG_ARM_EXYNOS_DEVFREQ)
#include <soc/samsung/exynos-devfreq.h>
#endif
#include <linux/module.h>

#include "acpm_dvfs.h"
#include "cmucal.h"

static struct acpm_dvfs acpm_dvfs;
#if IS_ENABLED(CONFIG_ARM_EXYNOS_DEVFREQ)
static struct acpm_dvfs acpm_noti_mif;
static struct exynos_pm_qos_request mif_request_from_acpm;
#endif

int exynos_acpm_set_rate(unsigned int id, unsigned long rate)
{
	struct ipc_config config;
	unsigned int cmd[4];
	unsigned long long before, after, latency;
	int ret;

	config.cmd = cmd;
	config.response = true;
	config.indirection = false;
	config.cmd[0] = id;
	config.cmd[1] = (unsigned int)rate;
	config.cmd[2] = FREQ_REQ;
	config.cmd[3] = 0;

	before = sched_clock();
	ret = acpm_ipc_send_data(acpm_dvfs.ch_num, &config);
	after = sched_clock();
	latency = after - before;
	if (ret)
		pr_err("%s:[%d] latency = %llu ret = %d",
			__func__, id, latency, ret);

	return ret;
}
EXPORT_SYMBOL_GPL(exynos_acpm_set_rate);

unsigned long exynos_acpm_get_rate(unsigned int id)
{
	struct ipc_config config;
	unsigned int cmd[4];
	unsigned long long before, after, latency;
	int ret;

	config.cmd = cmd;
	config.response = true;
	config.indirection = false;
	config.cmd[0] = id;
	config.cmd[1] = 0;
	config.cmd[2] = FREQ_GET;
	config.cmd[3] = 0;

	before = sched_clock();
	ret = acpm_ipc_send_data(acpm_dvfs.ch_num, &config);
	after = sched_clock();
	latency = after - before;
	if (ret)
		pr_err("%s:[%d] latency = %llu ret = %d", __func__,
			id, latency, ret);

	return config.cmd[1];
}

int exynos_acpm_set_volt_margin(unsigned int id, int volt)
{
	struct ipc_config config;
	unsigned int cmd[4];
	unsigned long long before, after, latency;
	int ret;

	config.cmd = cmd;
	config.response = true;
	config.indirection = false;
	config.cmd[0] = id;
	config.cmd[1] = volt;
	config.cmd[2] = MARGIN_REQ;
	config.cmd[3] = 0;

	before = sched_clock();
	ret = acpm_ipc_send_data(acpm_dvfs.ch_num, &config);
	after = sched_clock();
	latency = after - before;
	if (ret)
		pr_err("%s:[%d] latency = %llu ret = %d",
			__func__, id, latency, ret);

	return ret;
}

#if IS_ENABLED(CONFIG_ARM_EXYNOS_DEVFREQ)
static void acpm_noti_mif_callback(unsigned int *cmd, unsigned int size)
{
	pr_info("%s : req %d KHz\n", __func__, cmd[1]);
	exynos_pm_qos_update_request(&mif_request_from_acpm, cmd[1]);
}
#endif

static int acpm_dvfs_probe(struct platform_device *pdev)
{
	int ret = 0;
#if IS_ENABLED(CONFIG_ARM_EXYNOS_DEVFREQ)
	struct device *dev = &pdev->dev;

	acpm_noti_mif.dev = dev->of_node;

	ret = acpm_ipc_request_channel(acpm_noti_mif.dev,
				 acpm_noti_mif_callback,
				 &acpm_noti_mif.ch_num,
				 &acpm_noti_mif.size);

	if (ret < 0)
		return ret;

	exynos_pm_qos_add_request(&mif_request_from_acpm, PM_QOS_BUS_THROUGHPUT, 0);
#endif

	dev_info(&pdev->dev, "acpm_dvfs_probe done\n");

	return ret;
}

void exynos_acpm_set_device(void *dev)
{
	acpm_dvfs.dev = dev;
}
EXPORT_SYMBOL_GPL(exynos_acpm_set_device);

static int acpm_dvfs_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id acpm_dvfs_match[] = {
	{ .compatible = "samsung,exynos-acpm-dvfs" },
	{},
};
MODULE_DEVICE_TABLE(of, acpm_dvfs_match);

static struct platform_driver samsung_acpm_dvfs_driver = {
	.probe	= acpm_dvfs_probe,
	.remove	= acpm_dvfs_remove,
	.driver	= {
		.name = "exynos-acpm-dvfs",
		.owner	= THIS_MODULE,
		.of_match_table	= acpm_dvfs_match,
	},
};

int exynos_acpm_dvfs_init(void)
{
	int ret;

	ret = acpm_ipc_request_channel(acpm_dvfs.dev,
				 NULL,
				 &acpm_dvfs.ch_num,
				 &acpm_dvfs.size);

	if (ret < 0)
		pr_err("acpm_dvfs_init fail ret = %d\n", ret);
	else {
		pr_info("acpm_dvfs_init sucess \n");
		pr_info("acpm_dvfs(cal-if).ch_num : %d, acpm_dvfs(cal-if).size : %d \n",
				acpm_dvfs.ch_num, acpm_dvfs.size);
	}

	return platform_driver_register(&samsung_acpm_dvfs_driver);
}

MODULE_LICENSE("GPL");
