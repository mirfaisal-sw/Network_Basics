// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * Exynos PM support.
 */

#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/syscore_ops.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/debugfs.h>
#include <linux/mfd/samsung/s2vps01-regulator.h>

#include <soc/samsung/exynos-pm.h>
#include <soc/samsung/exynos-pmu.h>

static int sfi_str_check_skip;	/* skip SFI's suspend check during system-suspend */
static struct exynos_pm_info *pm_info;

static struct exynos_pm_dbg *pm_dbg;

static int exynos_pm_suspend_notifier(struct notifier_block *notifier,
				       unsigned long pm_event, void *v)
{
	switch (pm_event) {
	case PM_SUSPEND_PREPARE:
		if (pm_info->pmu_mon_req_offset) {
			exynos_pmu_update(pm_info->pmu_mon_req_offset,
					  (1 << (pm_info->pmu_mon_req_mask)), 0);
			pr_info("%s sends request SFI to stop ALIVE monitor\n",
				EXYNOS_PM_PREFIX);
		}
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static int exynos_pm_resume_notifier(struct notifier_block *notifier,
				       unsigned long pm_event, void *v)
{
	switch (pm_event) {
	case PM_POST_SUSPEND:
		if (pm_info->pmu_mon_req_offset) {
			exynos_pmu_update(pm_info->pmu_mon_req_offset,
					  (1 << (pm_info->pmu_mon_req_mask)),
					  (1 << (pm_info->pmu_mon_req_mask)));
			pr_info("%s sends request SFI to start ALIVE monitor\n",
				EXYNOS_PM_PREFIX);
		}
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block exynos_pm_suspend_notifier_block = {
	.notifier_call = exynos_pm_suspend_notifier,
	.priority = 0,
};

static struct notifier_block exynos_pm_resume_notifier_block = {
	.notifier_call = exynos_pm_resume_notifier,
	.priority = 0,
};

static int exynos_check_sfi_wfi_status(void)
{
	unsigned int i, val, acc_val = 1;
	unsigned int read_cnt = 1000;

	for (i = 0; i < read_cnt; i++) {
		exynos_pmu_read(pm_info->sfi_cpu0_offset, &val);
		val = (val & (1 << pm_info->sfi_sleeping_mask)) >> pm_info->sfi_sleeping_mask;
		acc_val = acc_val * val;
	}

	return acc_val;
}

static bool Is_sfi_off(void)
{
	unsigned int val;

	exynos_pmu_read(pm_info->sfi_status_offset, &val);
	val = (val & (1 << pm_info->sfi_status_mask)) >> pm_info->sfi_status_mask;
	if (val == pm_info->sfi_status_value)
		return true;
	else
		return false;
}

static void exynos_show_wakeup_reason_eint(void)
{
	int bit;
	int i, size;
	long unsigned int ext_int_pend;
	u64 eint_wakeup_mask;
	bool found = 0;
	unsigned int val0 = 0;

	exynos_pmu_read(pm_info->eint_wakeup_mask_offset, &val0);
	eint_wakeup_mask = val0;

	for (i = 0, size = 8; i < pm_info->num_eint; i += size) {
		ext_int_pend =
			__raw_readl(pm_info->eint_base + pm_info->weint_pend_offset[i/8]);

		for_each_set_bit(bit, &ext_int_pend, size) {
			if (eint_wakeup_mask & (1 << (i + bit)))
				continue;
			found = 1;
			pr_info("%s Resume caused by EINT%d\n", EXYNOS_PM_PREFIX, (i + bit));
		}
	}

	if (!found)
		pr_info("%s Resume caused by unknown EINT\n", EXYNOS_PM_PREFIX);
}

static void exynos_show_wakeup_registers(void)
{
	int i, size;
	unsigned int wakeup_stat;

	pr_info("WAKEUP_STAT:\n");
	for (i = 0; i < pm_info->num_wakeup_stat; i++) {
		exynos_pmu_read(pm_info->wakeup_stat_offset[i], &wakeup_stat);
		pr_info("0x%08x\n", wakeup_stat);
	}

	pr_info("EINT_PEND:\n");
	for (i = 0, size = 8; i < pm_info->num_eint; i += size)
		pr_info("0x%02x\n", __raw_readl(pm_info->eint_base + pm_info->weint_pend_offset[i/8]));
}

static void exynos_show_wakeup_reason(bool sleep_abort)
{
	unsigned int wakeup_stat0, wakeup_stat1;
	unsigned long wakeup_stat;
	int i, size;

	if (sleep_abort) {
		pr_info("%s early wakeup! Dumping pending registers...\n", EXYNOS_PM_PREFIX);

		pr_info("EINT_PEND:\n");
		for (i = 0, size = 8; i < pm_info->num_eint; i += size)
			pr_info("0x%02x\n", __raw_readl(pm_info->eint_base + pm_info->weint_pend_offset[i/8]));

		pr_info("GIC_PEND:\n");
		for (i = 0; i < pm_info->num_gic; i++)
			pr_info("GICD_ISPENDR[%d] = 0x%x\n",
						i, __raw_readl(pm_info->gic_base + i*4));

		pr_info("%s done.\n", EXYNOS_PM_PREFIX);
		return;
	}

	if (!pm_info->num_wakeup_stat)
		return;

	exynos_pmu_read(pm_info->wakeup_stat_offset[0], &wakeup_stat0);
	exynos_pmu_read(pm_info->wakeup_stat_offset[1], &wakeup_stat1);
	exynos_show_wakeup_registers();

	wakeup_stat = ((unsigned long)wakeup_stat1 << 32) | wakeup_stat0;

	if (wakeup_stat & (1UL << pm_info->wakeup_stat_rtc))
		pr_info("%s Resume caused by RTC alarm\n", EXYNOS_PM_PREFIX);
	else if (wakeup_stat & (1UL << pm_info->wakeup_stat_eint))
		exynos_show_wakeup_reason_eint();
	else {
		for (i = 0; i < pm_info->num_wakeup_stat; i++) {
			exynos_pmu_read(pm_info->wakeup_stat_offset[i], &wakeup_stat0);
			pr_info("%s Resume caused by wakeup%d_stat 0x%08x\n",
					EXYNOS_PM_PREFIX, i + 1, wakeup_stat0);

		}
	}
}

static void exynos_set_wakeup_interrupt_enable(enum sys_powerdown mode)
{
	int i;

	for (i = 0; i < pm_info->num_wakeup_int_en; i++) {
		exynos_pmu_write(pm_info->wakeup_stat_offset[i], 0);
		exynos_pmu_write(pm_info->wakeup_int_en_offset[i], pm_info->wakeup_int_en[i]);
	}
}

static int exynos_prepare_sys_powerdown(enum sys_powerdown mode)
{
	int ret;

	exynos_set_wakeup_interrupt_enable(mode);

	ret = cal_pm_enter(mode);
	if (ret)
		pr_err("CAL : Fail to set powermode\n");
	else
		pr_info("CAL : Success cal_pm_enter\n");

	return ret;
}

static void exynos_wakeup_sys_powerdown(enum sys_powerdown mode, bool early_wakeup)
{
	if (early_wakeup) {
		if (cal_pm_earlywakeup(mode))
			pr_err("CAL : Fail to restore earlywakeup\n");
		else
			pr_info("CAL : Success cal_pm_earlywakeup\n");
	} else {
		if (cal_pm_exit(mode))
			pr_err("CAL : Fail to restore powermode\n");
		else
			pr_info("CAL : Success cal_pm_exit\n");

	}
}

static int __init set_sfi_str_check_skip(char *arg)
{
	sfi_str_check_skip = 1;
	pr_info("skip SFI suspend check\n");
	return 0;
}

early_param("sfi_str_check_skip", set_sfi_str_check_skip);

/**
 * @cnotice
 * @prdcode
 * @unit_name{syscore_pm}
 * @purpose "handle suspend operation for syscore"
 * @logic "prepare power down and get information for S2R operation"
 * @noparam
 * @retval{return, -, -, >=0, <0}
 */
static int exynos_pm_syscore_suspend(void)
{
	if (pm_info->sfi_cpu0_offset) {
		if (!exynos_check_sfi_wfi_status()) {
			pr_info("%s %s: SFI is running.\n", EXYNOS_PM_PREFIX, __func__);
			return -EBUSY;
		}
	}
	if (pm_info->sfi_status_offset && !sfi_str_check_skip) {
		if (!Is_sfi_off()) {
			pr_info("%s %s: SFI is power on.\n", EXYNOS_PM_PREFIX, __func__);
			return -EBUSY;
		}
	}
	if (pm_info->clkreq_sleep) {
		if (s2vps01_master_chk_tcxo_always_on()) {
			pr_info("%s %s: Keeping CLKREQ state high.\n", EXYNOS_PM_PREFIX, __func__);
			exynos_pmu_update(EXYNOS_PMU_MIF_CTRL, MASK_TCXO_CTRL_ONLY_SW, 0);
		}
	}
	exynos_prepare_sys_powerdown(pm_info->suspend_mode_idx);
	pr_info("%s %s: Enter Suspend scenario. suspend_mode_idx = %d)\n",
			EXYNOS_PM_PREFIX, __func__, pm_info->suspend_mode_idx);

	pm_info->prev_mif = acpm_get_mifdn_count();
	pm_info->prev_apsoc = acpm_get_apsocdn_count();
	pm_info->prev_seq_early_wakeup = acpm_get_early_wakeup_count();
	pm_info->prev_req = acpm_get_mif_request();

	pr_info("%s: pm_info->prev mif_count:%d, apsoc_count:%d, seq_early_wakeup_count:%d\n",
			EXYNOS_PM_PREFIX, pm_info->prev_mif, pm_info->prev_apsoc, pm_info->prev_seq_early_wakeup);
	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{syscore_pm}
 * @purpose "handle resume operation for syscore"
 * @logic "get information for resume operation and prepare power up"
 * @noparam
 * @noret
 */
static void exynos_pm_syscore_resume(void)
{
	pm_info->post_mif = acpm_get_mifdn_count();
	pm_info->post_apsoc = acpm_get_apsocdn_count();

	if (pm_info->prev_apsoc == pm_info->post_apsoc)
		pm_info->is_early_wakeup  = true;
	else
		pm_info->is_early_wakeup  = false;

	pm_info->post_seq_early_wakeup = acpm_get_early_wakeup_count();
	pr_info("%s: pm_info->post mif_count:%d, apsoc_count:%d, seq_early_wakeup_count:%d\n",
			EXYNOS_PM_PREFIX, pm_info->post_mif, pm_info->post_apsoc, pm_info->post_seq_early_wakeup);

	if (pm_info->post_seq_early_wakeup != pm_info->prev_seq_early_wakeup)
		pr_info("%s: Sequence early wakeup!\n", EXYNOS_PM_PREFIX);

	if (pm_info->post_mif == pm_info->prev_mif)
		pr_info("%s: MIF blocked. MIF request Mster was  0x%x\n", EXYNOS_PM_PREFIX, pm_info->prev_req);
	else
		pr_info("%s: MIF down. cur_count: %d, acc_count: %d\n",
				EXYNOS_PM_PREFIX, pm_info->post_mif - pm_info->prev_mif, pm_info->post_mif);

	exynos_wakeup_sys_powerdown(pm_info->suspend_mode_idx, pm_info->is_early_wakeup);

	exynos_show_wakeup_reason(pm_info->is_early_wakeup);

	if (!pm_info->is_early_wakeup)
		pr_debug("%s %s: post sleep, preparing to return\n",
						EXYNOS_PM_PREFIX, __func__);
}

static struct syscore_ops exynos_pm_syscore_ops = {
	.suspend	= exynos_pm_syscore_suspend,
	.resume		= exynos_pm_syscore_resume,
};

#ifdef CONFIG_DEBUG_FS
static void exynos_pm_debugfs_init(void)
{
	struct dentry *root;

	root = debugfs_create_dir("exynos-pm", NULL);
	if (!root) {
		pr_err("%s %s: could't create debugfs dir\n", EXYNOS_PM_PREFIX, __func__);
		return;
	}

	debugfs_create_u32("test_early_wakeup", 0644, root, &pm_dbg->test_early_wakeup);
}
#endif

/**
 * @cnotice
 * @prdcode
 * @unit_name{syscore_pm}
 * @purpose "To parse device tree for handling suspend and resume"
 * @logic "parse device tree and initialize exynos_pm_info structure"
 * @noparam
 * @retval{ret, int, 0, >=0, <0}
 */
static int exynos_pm_probe(struct platform_device *pdev)
{
	int ret;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;

	pm_info = devm_kzalloc(dev, sizeof(struct exynos_pm_info), GFP_KERNEL);
	if (!pm_info) {
		dev_err(dev, "failed to allocate memory for exynos_pm_info\n");
		return -ENOMEM;
	}

	pm_dbg = devm_kzalloc(dev, sizeof(struct exynos_pm_dbg), GFP_KERNEL);
	if (!pm_dbg) {
		dev_err(dev, "failed to allocate memory for exynos_pm_dbg\n");
		return -ENOMEM;
	}

	pm_info->eint_base = of_iomap(np, 0);
	if (!pm_info->eint_base) {
		dev_err(dev, "unabled to ioremap EINT base address\n");
		return -EINVAL;
	}

	pm_info->gic_base = of_iomap(np, 1);
	if (!pm_info->gic_base) {
		dev_err(dev, "unbaled to ioremap GIC base address\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "num-eint", &pm_info->num_eint)) {
		dev_err (dev, "unabled to get the number of eint from DT\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "num-gic", &pm_info->num_gic)) {
		dev_err (dev, "unabled to get the number of gic from DT\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "wakeup-stat-eint", &pm_info->wakeup_stat_eint)) {
		dev_err(dev, "unabled to get the eint bit file of WAKEUP_STAT from DT\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "wakeup-stat-rtc", &pm_info->wakeup_stat_rtc)) {
		dev_err(dev, "unabled to get the rtc bit file of WAKEUP_STAT from DT\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "suspend_mode_idx", &pm_info->suspend_mode_idx)) {
		dev_err(dev, "unabled to get suspend_mode_idx from DT\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "eint_wakeup_mask_offset", &pm_info->eint_wakeup_mask_offset)) {
		dev_err(dev, "unabled to get eint_wakeup_mask_offset from DT\n");
		return -EINVAL;
	}

	ret = of_property_count_u32_elems(np, "wakeup_stat_offset");
	if (!ret) {
		dev_err(dev, "unabled to get wakeup_stat value from DT\n");
		return -EINVAL;
	} else if (ret > 0) {
		pm_info->num_wakeup_stat = ret;
		pm_info->wakeup_stat_offset = devm_kzalloc(dev, sizeof(unsigned int) * ret, GFP_KERNEL);
		of_property_read_u32_array(np, "wakeup_stat_offset", pm_info->wakeup_stat_offset, ret);
	}

	ret = of_property_count_u32_elems(np, "wakeup_int_en_offset");
	if (!ret) {
		dev_err(dev, "unabled to get wakeup_int_en_offset value from DT\n");
		return -EINVAL;
	} else if (ret > 0) {
		pm_info->num_wakeup_int_en = ret;
		pm_info->wakeup_int_en_offset = devm_kzalloc(dev, sizeof(unsigned int) * ret, GFP_KERNEL);
		of_property_read_u32_array(np, "wakeup_int_en_offset", pm_info->wakeup_int_en_offset, ret);
	}

	ret = of_property_count_u32_elems(np, "wakeup_int_en");
	if (!ret) {
		dev_err(dev, "unabled to get wakeup_int_en value from DT\n");
		return -EINVAL;
	} else if (ret > 0) {
		pm_info->wakeup_int_en = devm_kzalloc(dev, sizeof(unsigned int) * ret, GFP_KERNEL);
		of_property_read_u32_array(np, "wakeup_int_en", pm_info->wakeup_int_en, ret);
	}

	ret = of_property_count_u32_elems(np, "weint_pend_offset");
	if (!ret) {
		dev_err(dev, "unabled to get weint_pend_offset value from DT\n");
		return -EINVAL;
	} else if (ret > 0) {
		pm_info->weint_pend_offset = devm_kzalloc(dev, sizeof(unsigned int) * ret, GFP_KERNEL);
		of_property_read_u32_array(np, "weint_pend_offset", pm_info->weint_pend_offset, ret);
	}

	pm_info->clkreq_sleep = of_property_read_bool(np, "clkreq_sleep");

	if (pm_info->clkreq_sleep)
		dev_info(dev, "need to check if clkreq has to be on during sleep due to tcxo monitoring function.\n");

	if (of_property_read_u32(np, "sfi-cpu0-offset", &pm_info->sfi_cpu0_offset)) {
		dev_info(dev, "no sfi-cpu0-offset. Skip checking sfi cpu status\n");
		pm_info->sfi_cpu0_offset = 0;
	} else {
		if (of_property_read_u32(np, "sfi-sleeping-mask", &pm_info->sfi_sleeping_mask)) {
			dev_err(dev, "unable to get sfi-sleeping-mask\n");
			return -EINVAL;
		}
	}

	if (of_property_read_u32(np, "sfi-status-offset", &pm_info->sfi_status_offset)) {
		dev_info(dev, "no sfi-status-offset. Skip checking sfi status\n");
		pm_info->sfi_status_offset = 0;
	} else {
		if (of_property_read_u32(np, "sfi-status-mask", &pm_info->sfi_status_mask)) {
			dev_err(dev, "unable to get sfi-status-mask\n");
			return -EINVAL;
		}
		if (of_property_read_u32(np, "sfi-status-value", &pm_info->sfi_status_value)) {
			dev_err(dev, "unable to get sfi-status-value\n");
			return -EINVAL;
		}
	}

	if (of_property_read_u32(np, "pmu-mon-req-offset", &pm_info->pmu_mon_req_offset)) {
		dev_info(dev, "no pmu-mon-req-offset. Skip controlling pmu-mon-req.\n");
		pm_info->pmu_mon_req_offset = 0;
	} else {
		if (of_property_read_u32(np, "pmu-mon-req-mask", &pm_info->pmu_mon_req_mask)) {
			dev_err(dev, "unable to get pmu-mon-req-mask\n");
			return -EINVAL;
		}
	}

	register_syscore_ops(&exynos_pm_syscore_ops);
	register_pm_notifier(&exynos_pm_suspend_notifier_block);
	register_pm_notifier(&exynos_pm_resume_notifier_block);
#ifdef CONFIG_DEBUG_FS
	exynos_pm_debugfs_init();
#endif
	dev_info(dev, "probe done.\n");

	return 0;
}

static const struct of_device_id exynos_pm_match[] = {
	{
		.compatible = "samsung,exynos-pm",
	},
	{},
};
MODULE_DEVICE_TABLE(of, exynos_pm_match);

static const struct platform_device_id exynos_pm_ids[] = {
	{ "exynos-pm", },
	{ }
};

static struct platform_driver exynos_pm_drv = {
	.probe		= exynos_pm_probe,
	.id_table	= exynos_pm_ids,
	.driver		= {
		.name	= "exynos-pm",
		.owner	= THIS_MODULE,
		.of_match_table = exynos_pm_match,
	},
};

static int exynos_pm_init(void)
{
	return platform_driver_register(&exynos_pm_drv);
}
late_initcall(exynos_pm_init);

static void exynos_pm_exit(void)
{
	return platform_driver_unregister(&exynos_pm_drv);
}
module_exit(exynos_pm_exit);

MODULE_DESCRIPTION("Samsung EXYNOS Soc series exynos-pm driver");
MODULE_LICENSE("GPL v2");
