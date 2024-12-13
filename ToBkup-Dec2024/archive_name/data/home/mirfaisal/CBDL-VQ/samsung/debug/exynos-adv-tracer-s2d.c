// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - Scan2Dram support
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/syscore_ops.h>

#include <soc/samsung/debug-snapshot.h>
#include <soc/samsung/exynos-adv-tracer-ipc.h>
#include <soc/samsung/exynos-pmu.h>
#if IS_ENABLED(CONFIG_EXYNOS_CPUIDLE)
#include <soc/samsung/exynos-cpupm.h>
#endif

enum s2d_ipc_cmd {
	eS2D_IPC_CMD_SET_ALL_BLK = 1,
	eS2D_IPC_CMD_GET_ALL_BLK,
	eS2D_IPC_CMD_SET_ENABLE,
	eS2D_IPC_CMD_GET_ENABLE,
};

struct plugin_s2d_info {
	struct adv_tracer_plugin *s2d_dev;
	struct device *dev;
	unsigned int enable;
	unsigned int all_block;
} plugin_s2d;

static int arraydump_done;
static unsigned int pmu_burnin_ctrl;
static int sel_scanmode = -1;
static int enable_scandump_wdtreset = -1;
static int dbgsel_sw = -1;
static unsigned int pmu_rst_seq_config;
static int pmu_rst_seq_s2d_en = -1;

#define DONE_ARRYDUMP 0xADADADAD

void adv_tracer_s2d_scandump(void)
{
	if (!pmu_burnin_ctrl || (sel_scanmode < 0) || (dbgsel_sw < 0)) {
		dev_err(plugin_s2d.dev, "pmu offset no data\n");
		return;
	}
	exynos_pmu_update(pmu_burnin_ctrl, BIT(sel_scanmode), 0);
	dev_info(plugin_s2d.dev, "enter scandump mode!\n");
	exynos_pmu_update(pmu_burnin_ctrl, BIT(dbgsel_sw), BIT(dbgsel_sw));
}

int adv_tracer_s2d_arraydump(void)
{
	struct adv_tracer_ipc_cmd cmd;
	int ret = 0;
	u32 cpu_mask = (1U << num_possible_cpus()) - 1U;

	if (!plugin_s2d.s2d_dev)
		return -ENODEV;

	if (arraydump_done == DONE_ARRYDUMP) {
		dev_info(plugin_s2d.dev, "Arraydump already done(0x%x)\n",
				cpu_mask);
		return -1;
	}
	arraydump_done = DONE_ARRYDUMP;

	dev_info(plugin_s2d.dev, "Start Arraydump (0x%x)\n", cpu_mask);
	cmd.cmd_raw.cmd = EAT_IPC_CMD_ARRAYDUMP;
	cmd.cmd_raw.id = ARR_IPC_CMD_ID_KERNEL_ARRAYDUMP;
	cmd.buffer[1] = dbg_snapshot_get_item_paddr("log_arrdumppanic");
	cmd.buffer[2] = cpu_mask;
	ret = adv_tracer_ipc_send_data_polling_timeout(EAT_FRM_CHANNEL, &cmd, EAT_IPC_TIMEOUT * 100);
	if (ret < 0)
		goto end;

	dev_info(plugin_s2d.dev, "Finish Arraydump (0x%x)\n", cmd.buffer[1]);
end:
	return ret;
}
EXPORT_SYMBOL(adv_tracer_s2d_arraydump);

static int adv_tracer_s2d_get_enable(void)
{
	struct adv_tracer_ipc_cmd cmd;
	int ret = 0;

	cmd.cmd_raw.cmd = eS2D_IPC_CMD_GET_ENABLE;
	ret = adv_tracer_ipc_send_data(plugin_s2d.s2d_dev->id,
			(struct adv_tracer_ipc_cmd *)&cmd);
	if (ret < 0) {
		dev_err(plugin_s2d.dev, "ipc can't get enable\n");
		return ret;
	}
	plugin_s2d.enable = cmd.buffer[1];

	return 0;
}

static int adv_tracer_s2d_set_enable(int en)
{
	struct adv_tracer_ipc_cmd cmd;
	int ret = 0;

	cmd.cmd_raw.cmd = eS2D_IPC_CMD_SET_ENABLE;
	cmd.buffer[1] = en;
	ret = adv_tracer_ipc_send_data(plugin_s2d.s2d_dev->id, &cmd);
	if (ret < 0) {
		dev_err(plugin_s2d.dev, "ipc can't enable setting\n");
		return ret;
	}
	plugin_s2d.enable = en;
	return 0;
}

static int adv_tracer_s2d_set_blk_info(unsigned int all)
{
	struct adv_tracer_ipc_cmd cmd;
	int ret = 0;

	cmd.cmd_raw.cmd = eS2D_IPC_CMD_SET_ALL_BLK;
	cmd.buffer[1] = all;
	ret = adv_tracer_ipc_send_data(plugin_s2d.s2d_dev->id, &cmd);
	if (ret < 0) {
		dev_err(plugin_s2d.dev, "s2d ipc cannot select blk\n");
		return ret;
	}
	plugin_s2d.all_block = all;

	return 0;
}

static void adv_tracer_s2d_handler(struct adv_tracer_ipc_cmd *cmd,
		unsigned int len)
{
}

static ssize_t s2d_enable_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t size)
{
	unsigned long val = simple_strtoul(buf, NULL, 10);

	adv_tracer_s2d_set_enable(!!val);

	return size;
}

static ssize_t s2d_enable_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, 10, "%sable\n", plugin_s2d.enable ? "en" : "dis");
}

static ssize_t s2d_block_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t size)
{
	unsigned long val = simple_strtoul(buf, NULL, 16);

	adv_tracer_s2d_set_blk_info(!!val);

	return size;
}

static ssize_t s2d_block_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, 10, "%s\n", plugin_s2d.all_block ? "all" : "cpu");
}

static DEVICE_ATTR_RW(s2d_enable);
static DEVICE_ATTR_RW(s2d_block);

static struct attribute *adv_tracer_s2d_sysfs_attrs[] = {
	&dev_attr_s2d_enable.attr,
	&dev_attr_s2d_block.attr,
	NULL,
};
ATTRIBUTE_GROUPS(adv_tracer_s2d_sysfs);

#ifdef CONFIG_PM_SLEEP
static int en_status_before_suspend;

static int adv_tracer_s2d_syscore_suspend(void)
{
	if (!pmu_burnin_ctrl || (sel_scanmode < 0) || (enable_scandump_wdtreset < 0)
			|| !pmu_rst_seq_config || (pmu_rst_seq_s2d_en < 0)) {
		pr_err("Cannot disable s2d on suspend (Not enough config)\n");
		return -1;
	}
	pr_info("Disable s2d (Not support operation on suspend status\n");
	en_status_before_suspend = plugin_s2d.enable;
	exynos_pmu_update(pmu_burnin_ctrl, BIT(sel_scanmode), BIT(sel_scanmode));
	exynos_pmu_update(pmu_burnin_ctrl, BIT(enable_scandump_wdtreset), 0);
	exynos_pmu_update(pmu_rst_seq_config, BIT(pmu_rst_seq_s2d_en), 0);
	plugin_s2d.enable = false;
	return 0;
}

static void adv_tracer_s2d_syscore_resume(void)
{
	if (!pmu_burnin_ctrl || (sel_scanmode < 0) || (enable_scandump_wdtreset < 0)
			|| !pmu_rst_seq_config || (pmu_rst_seq_s2d_en < 0)) {
		pr_err("Cannot re-enable s2d on resume (Not enough config)\n");
		return;
	}

	if (en_status_before_suspend) {
		exynos_pmu_update(pmu_burnin_ctrl, BIT(sel_scanmode), 0);
		exynos_pmu_update(pmu_burnin_ctrl, BIT(enable_scandump_wdtreset), BIT(enable_scandump_wdtreset));
		exynos_pmu_update(pmu_rst_seq_config, BIT(pmu_rst_seq_s2d_en), BIT(pmu_rst_seq_s2d_en));
		plugin_s2d.enable = true;
		pr_info("Re-enable s2d: Enabled\n");
	}
}

static struct syscore_ops adv_tracer_s2d_syscore_ops = {
	.suspend	= adv_tracer_s2d_syscore_suspend,
	.resume		= adv_tracer_s2d_syscore_resume,
};
#endif

static int adv_tracer_s2d_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct adv_tracer_plugin *s2d = NULL;
	int ret, val;

	s2d = devm_kzalloc(&pdev->dev, sizeof(struct adv_tracer_plugin),
			GFP_KERNEL);
	if (!s2d) {
		dev_err(&pdev->dev, "can not allocate mem for s2d\n");
		ret = -ENOMEM;
		goto err_s2d_info;
	}

	plugin_s2d.dev = &pdev->dev;
	plugin_s2d.s2d_dev = s2d;

	if (of_property_read_u32(node, "pmu-burnin-ctrl", &pmu_burnin_ctrl))
		dev_err(&pdev->dev, "pmu-burnin-ctrl is no data\n");
	if (of_property_read_u32(node, "sel-scanmode-bit", &sel_scanmode))
		dev_err(&pdev->dev, "sel-scanmode-bit is no data\n");
	if (of_property_read_u32(node, "enable-scandump-wdtreset", &enable_scandump_wdtreset))
		dev_err(&pdev->dev, "enable-scandump-wdtreset is no data\n");
	if (of_property_read_u32(node, "dbgsel-sw-bit", &dbgsel_sw))
		dev_err(&pdev->dev, "dbgsel-sw-bit is no data\n");
	if (of_property_read_u32(node, "pmu-rst-seq-config", &pmu_rst_seq_config))
		dev_err(&pdev->dev, "pmu-rst-seq-config is no data\n");
	if (of_property_read_u32(node, "pmu-rst-seq-s2d-en", &pmu_rst_seq_s2d_en))
		dev_err(&pdev->dev, "pmu-rst-seq-s2d-en is no data\n");

	ret = adv_tracer_ipc_request_channel(node, adv_tracer_s2d_handler,
				&s2d->id, &s2d->len);
	if (ret < 0) {
		dev_err(&pdev->dev, "s2d ipc request fail(%d)\n", ret);
		ret = -ENODEV;
		goto err_s2d_info;
	}

	if (of_property_read_u32(node, "dump_en", &val))
		dev_info(&pdev->dev, "No dump_en config in DT\n");
	else
		adv_tracer_s2d_set_enable(!!val);

	ret = adv_tracer_s2d_get_enable();
	if (ret < 0)
		goto err_s2d_info;
	dev_info(&pdev->dev, "S2D %sabled\n", plugin_s2d.enable ? "en" : "dis");

	platform_set_drvdata(pdev, s2d);
	ret = sysfs_create_groups(&pdev->dev.kobj, adv_tracer_s2d_sysfs_groups);
	if (ret) {
		dev_err(&pdev->dev, "fail to register sysfs.\n");
		return ret;
	}

	dbg_snapshot_register_debug_ops(NULL,
			(void *)adv_tracer_s2d_arraydump,
			(void *)adv_tracer_s2d_scandump);

#ifdef CONFIG_PM_SLEEP
	register_syscore_ops(&adv_tracer_s2d_syscore_ops);
#endif
	dev_info(&pdev->dev, "%s successful.\n", __func__);
	return 0;

err_s2d_info:
	return ret;
}

static int adv_tracer_s2d_remove(struct platform_device *pdev)
{
	struct adv_tracer_plugin *s2d = platform_get_drvdata(pdev);

	adv_tracer_ipc_release_channel(s2d->id);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int en_status_before_suspend;

static int adv_tracer_s2d_suspend(struct device *dev)
{
	dev_info(dev, "Disable s2d (Not support operation on suspend status\n");
	en_status_before_suspend = plugin_s2d.enable;
	adv_tracer_s2d_set_enable(0);
	return 0;
}

static int adv_tracer_s2d_resume(struct device *dev)
{
	int ret;

	if (en_status_before_suspend) {
		adv_tracer_s2d_set_enable(!!en_status_before_suspend);
		ret = adv_tracer_s2d_get_enable();
		if (!ret) {
			dev_info(dev, "Re-enable s2d: %s\n",
				plugin_s2d.enable ? "Enabled" : "Disabled");
		}
	}
	return 0;
}
#else
#define adv_tracer_s2d_suspend	NULL
#define adv_tracer_s2d_resume	NULL
#endif

static SIMPLE_DEV_PM_OPS(adv_tracer_s2d_pm_ops, adv_tracer_s2d_suspend, adv_tracer_s2d_resume);

static const struct of_device_id adv_tracer_s2d_match[] = {
	{ .compatible = "samsung,exynos-adv-tracer-s2d", },
	{},
};
MODULE_DEVICE_TABLE(of, adv_tracer_s2d_match);

static struct platform_driver adv_tracer_s2d_driver = {
	.probe          = adv_tracer_s2d_probe,
	.remove         = adv_tracer_s2d_remove,
	.driver         = {
		.name   = "exynos-adv-tracer-s2d",
		.owner  = THIS_MODULE,
		.of_match_table = of_match_ptr(adv_tracer_s2d_match),
		.pm = &adv_tracer_s2d_pm_ops,
	},
};
module_platform_driver(adv_tracer_s2d_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("EXYNOS ADV-TRACER-S2D");
