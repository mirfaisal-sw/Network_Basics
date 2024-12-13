// SPDX-License-Identifier: GPL-2.0
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include <linux/module.h>
#include <soc/samsung/ect_parser.h>
#include <soc/samsung/cal-if.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#if IS_ENABLED(CONFIG_EXYNOS_BTS)
#include <soc/samsung/bts.h>
#endif

#include "pwrcal-env.h"
#include "pwrcal-rae.h"
#include "cmucal.h"
#include "ra.h"
#include "acpm_dvfs.h"
#include "fvmap.h"
#include "asv.h"

#include "pmucal/include/pmucal_system.h"
#include "pmucal/include/pmucal_local.h"
#include "pmucal/include/pmucal_dbg.h"
#include "pmucal/include/pmucal_rae.h"
#include "pmucal/include/pmucal_powermode.h"

#include "../acpm/acpm.h"

extern void exynosauto9_cal_data_init(void);
extern void exynosautov920_evt0_cal_data_init(void);
extern void exynosautov920_evt2_cal_data_init(void);
extern void exynosautov620_evt0_cal_data_init(void);

unsigned int asv_table_ver;
unsigned long long FIN_HZ;
unsigned int cal_variant;

void (*exynos_cal_pd_bts_sync)(unsigned int id, int on);
EXPORT_SYMBOL(exynos_cal_pd_bts_sync);

unsigned long cal_dfs_get_max_freq(unsigned int id)
{
	return vclk_get_max_freq(id);
}
EXPORT_SYMBOL_GPL(cal_dfs_get_max_freq);

unsigned long cal_dfs_get_min_freq(unsigned int id)
{
	return vclk_get_min_freq(id);
}
EXPORT_SYMBOL_GPL(cal_dfs_get_min_freq);

unsigned int cal_dfs_get_lv_num(unsigned int id)
{
	return vclk_get_lv_num(id);
}
EXPORT_SYMBOL_GPL(cal_dfs_get_lv_num);

unsigned long cal_dfs_cached_get_rate(unsigned int id)
{
	unsigned long ret;

	ret = vclk_get_rate(id);

	return ret;
}
EXPORT_SYMBOL_GPL(cal_dfs_cached_get_rate);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To set clock rate through APM"
 * @logic "proceed CMU_CAL for setting clock rate"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @endparam
 * @retval{ret, int, 0, >=0, <0}
 */
int cal_dfs_set_rate(unsigned int id, unsigned long rate)
{
	struct vclk *vclk;
	int ret;

	if (IS_ACPM_VCLK(id)) {
		ret = exynos_acpm_set_rate(GET_IDX(id), rate);
		if (!ret) {
			vclk = cmucal_get_node(id);
			if (vclk)
				vclk->vrate = (unsigned int)rate;
		}
	} else {
		ret = vclk_set_rate(id, rate);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(cal_dfs_set_rate);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To get clock rate through APM"
 * @logic "proceed CMU_CAL for getting clock rate"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @endparam
 * @retval{ret, int, 0, >=0, <0}
 */
unsigned long cal_dfs_get_rate(unsigned int id)
{
	unsigned long ret = 0;

	ret = vclk_recalc_rate(id);

	return ret;
}
EXPORT_SYMBOL_GPL(cal_dfs_get_rate);

int cal_dfs_get_rate_table(unsigned int id, unsigned long *table)
{
	int ret;

	ret = vclk_get_rate_table(id, table);

	return ret;
}
EXPORT_SYMBOL_GPL(cal_dfs_get_rate_table);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To set clock rate"
 * @logic "proceed CMU_CAL for setting clock rate"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @param{in, rate, unsigned long, >=0}
 * @endparam
 * @retval{ret, int, -22, >=0, <0}
 */
int cal_clk_setrate(unsigned int id, unsigned long rate)
{
	int ret = -EINVAL;

	ret = vclk_set_rate(id, rate);

	return ret;
}
EXPORT_SYMBOL_GPL(cal_clk_setrate);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To get clock rate"
 * @logic "proceed CMU_CAL for getting clock rate"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @endparam
 * @retval{ret, int, 0, >=0, <0}
 */
unsigned long cal_clk_getrate(unsigned int id)
{
	unsigned long ret = 0;

	ret = vclk_recalc_rate(id);

	return ret;
}
EXPORT_SYMBOL_GPL(cal_clk_getrate);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To enable clock"
 * @logic "proceed CMU_CAL for enabling clock"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @endparam
 * @retval{ret, int, 0, >=0, <0}
 */
int cal_clk_enable(unsigned int id)
{
	int ret = 0;

	ret = vclk_set_enable(id);

	return ret;
}
EXPORT_SYMBOL_GPL(cal_clk_enable);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To disable clock"
 * @logic "proceed CMU_CAL for disabling clock"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @endparam
 * @retval{ret, int, 0, >=0, <0}
 */
int cal_clk_disable(unsigned int id)
{
	int ret = 0;

	ret = vclk_set_disable(id);

	return ret;
}
EXPORT_SYMBOL_GPL(cal_clk_disable);

int cal_qch_init(unsigned int id, unsigned int use_qch)
{
	int ret = 0;

	ret = ra_set_qch(id, use_qch, 0, 0);

	return ret;
}
EXPORT_SYMBOL_GPL(cal_qch_init);

unsigned int cal_dfs_get_boot_freq(unsigned int id)
{
	return vclk_get_boot_freq(id);
}
EXPORT_SYMBOL_GPL(cal_dfs_get_boot_freq);

unsigned int cal_dfs_get_resume_freq(unsigned int id)
{
	return vclk_get_resume_freq(id);
}
EXPORT_SYMBOL_GPL(cal_dfs_get_resume_freq);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To provide interface for local power control"
 * @logic "getting the power domain id and on/off information and proceed to PMU_CAL"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @param{in, on, int, >=0}
 * @endparam
 * @retval{ret, int, -, >=0, <0}
 */
int cal_pd_control(unsigned int id, int on)
{
	unsigned int index;
	int ret;

	if ((id & 0xFFFF0000) != BLKPWR_MAGIC)
		return -1;

	index = id & 0x0000FFFF;

	if (on) {
		ret = pmucal_local_enable(index);

#if IS_ENABLED(CONFIG_EXYNOS_BTS)
		if (exynos_cal_pd_bts_sync && cal_pd_status(id))
			exynos_cal_pd_bts_sync(id, on);
#endif
	} else {
#if IS_ENABLED(CONFIG_EXYNOS_BTS)
		if (exynos_cal_pd_bts_sync && cal_pd_status(id))
			exynos_cal_pd_bts_sync(id, on);
#endif
		ret = pmucal_local_disable(index);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(cal_pd_control);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To provide interface for checking local power status"
 * @logic "getting the power domain id and proceed to PMU_CAL for checking status"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @endparam
 * @retval{-, -, -, >=0, <0}
 */
int cal_pd_status(unsigned int id)
{
	unsigned int index;

	if ((id & 0xFFFF0000) != BLKPWR_MAGIC)
		return -1;

	index = id & 0x0000FFFF;

	return pmucal_local_is_enabled(index);
}
EXPORT_SYMBOL_GPL(cal_pd_status);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To provide interface for saving tzpc address for local power status"
 * @logic "getting the power domain id and need_smc value and proceed to PMU_CAL"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @param{in, need_smc, int, >=0}
 * @endparam
 * @retval{-, -, -, >=0, <0}
 */
int cal_pd_set_smc_id(unsigned int id, int need_smc)
{
	unsigned int index;

	if (need_smc && ((id & 0xFFFF0000) != BLKPWR_MAGIC))
		return -1;

	index = id & 0x0000FFFF;

	pmucal_local_set_smc_id(index, need_smc);

	return 0;
}
EXPORT_SYMBOL_GPL(cal_pd_set_smc_id);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To provide interface for setting first_on boolean variant"
 * @logic "getting the power domain id and initial_state value and proceed to PMU_CAL"
 * @params
 * @param{in, id, unsigned int, >=0}
 * @param{in, initial_state, int, >=0}
 * @endparam
 * @retval{-, -, -, >=0, <0}
 */
int cal_pd_set_first_on(unsigned int id, int initial_state)
{
	unsigned int index;

	if ((id & 0xFFFF0000) != BLKPWR_MAGIC)
		return -1;

	index = id & 0x0000FFFF;

	pmucal_local_set_first_on(index, initial_state);

	return 0;
}
EXPORT_SYMBOL_GPL(cal_pd_set_first_on);
/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To interface PMU_CAL for suspend operation"
 * @logic "execute and return pmucal_system_enter function"
 * @params
 * @param{in, mode, int, >=0}
 * @endparam
 * @retval{-, -, -, >=0, <0}
 */
int cal_pm_enter(int mode)
{
	return pmucal_system_enter(mode);
}
EXPORT_SYMBOL_GPL(cal_pm_enter);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To interface PMU_CAL for resume operation"
 * @logic "execute and return pmucal_system_exit function"
 * @params
 * @param{in, mode, int, >=0}
 * @endparam
 * @retval{-, -, -, >=0, <0}
 */
int cal_pm_exit(int mode)
{
	return pmucal_system_exit(mode);
}
EXPORT_SYMBOL_GPL(cal_pm_exit);

/**
 * @cnotice
 * @prdcode
 * @unit_name{CAL_IF}
 * @purpose "To interface PMU_CAL for early wakeup"
 * @logic "execute and return pmucal_system_earlywakeup function"
 * @params
 * @param{in, mode, int, >=0}
 * @endparam
 * @retval{-, -, -, >=0, <0}
 */
int cal_pm_earlywakeup(int mode)
{
	return pmucal_system_earlywakeup(mode);
}
EXPORT_SYMBOL_GPL(cal_pm_earlywakeup);

int cal_dfs_get_asv_table(unsigned int id, unsigned int *table)
{
	return fvmap_get_voltage_table(id, table);
}
EXPORT_SYMBOL_GPL(cal_dfs_get_asv_table);

void cal_dfs_set_volt_margin(unsigned int id, int volt)
{
	if (IS_ACPM_VCLK(id))
		exynos_acpm_set_volt_margin(id, volt);
}
EXPORT_SYMBOL_GPL(cal_dfs_set_volt_margin);

int cal_dfs_get_rate_asv_table(unsigned int id,
					struct dvfs_rate_volt *table)
{
	unsigned long rate[TBL_SIZE] = {0, };
	unsigned int volt[TBL_SIZE] = {0, };
	int num_of_entry;
	int idx;

	num_of_entry = cal_dfs_get_rate_table(id, rate);
	if (num_of_entry == 0)
		return 0;

	if (num_of_entry != cal_dfs_get_asv_table(id, volt))
		return 0;

	for (idx = 0; idx < num_of_entry; idx++) {
		table[idx].rate = rate[idx];
		table[idx].volt = volt[idx];
	}

	return num_of_entry;
}
EXPORT_SYMBOL_GPL(cal_dfs_get_rate_asv_table);

/**
 * @cnotice
 * @prdcode
 * @unit_name{cal_if}
 * @purpose "to initialize cmu_cal and pmu_cal"
 * @logic "calls functions for initialization in each component"
 * @params
 * @param{in, dev, void *, -}
 * @endparam
 * @retval{-, -, -, >=0, <0}
 */
int cal_if_init(void *dev)
{
	static int cal_initialized;
	int ret;

	if (cal_initialized == 1)
		return 0;

	ect_parse_binary_header();

	if (cal_data_init)
		cal_data_init();

	vclk_initialize();

	ret = pmucal_rae_init();
	if (ret < 0)
		return ret;

	ret = pmucal_system_init();
	if (ret < 0)
		return ret;

	ret = pmucal_local_init();
	if (ret < 0)
		return ret;

	ret = pmucal_cpuinform_init();
	if (ret < 0)
		return ret;

	exynos_acpm_set_device(dev);

	cal_initialized = 1;
#ifdef CONFIG_DEBUG_FS
	vclk_debug_init();
#endif

	pr_info("CAL Interface initialize Done \n");

	return 0;
}

static const struct of_device_id cal_if_match[] = {
	{
		.compatible = "samsung,exynos_cal_if",
	},
	{},
};
MODULE_DEVICE_TABLE(of, cal_if_match);

static int cal_if_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int ret, variant;
	bool skip_dvfs_fvmap = false;

	skip_dvfs_fvmap = of_property_read_bool(np, "skip_dvfs_fvmap");

	ret = of_property_read_u32(np, "variant", &variant);
	if (ret)
		goto out;
	cal_variant = variant;

	switch (variant) {
	case EXYNOSAUTO_V9:
		clk_variant = &clk_variant_v9;
		pmucal_system_variant_model = &pmucal_system_variant_v9;
		pmucal_local_variant_model = &pmucal_local_variant_v9;
		pmucal_powermode_variant_model = &pmucal_powermode_variant_v9;
		pmucal_rae_variant_model = &pmucal_rae_variant_v9;
		cal_data_init = exynosauto9_cal_data_init;
		asv_table_ver = asv_table_init();
		FIN_HZ = 26 * 1000000;
		break;
	case EXYNOSAUTO_V920_EVT0:
	case EXYNOSAUTO_V920_EVT1:
		clk_variant = &clk_variant_v920_evt0;
		pmucal_system_variant_model = &pmucal_system_variant_v920_evt0;
		pmucal_local_variant_model = &pmucal_local_variant_v920_evt0;
		pmucal_powermode_variant_model = &pmucal_powermode_variant_v920_evt0;
		pmucal_rae_variant_model = &pmucal_rae_variant_v920_evt0;
		cal_data_init = exynosautov920_evt0_cal_data_init;
		asv_table_ver = v920_asv_table_init();
		FIN_HZ = 38.4 * 1000000;
		break;
	case EXYNOSAUTO_V920_EVT2:
		clk_variant = &clk_variant_v920_evt2;
		pmucal_system_variant_model = &pmucal_system_variant_v920_evt2;
		pmucal_local_variant_model = &pmucal_local_variant_v920_evt2;
		pmucal_powermode_variant_model = &pmucal_powermode_variant_v920_evt2;
		pmucal_rae_variant_model = &pmucal_rae_variant_v920_evt2;
		cal_data_init = exynosautov920_evt2_cal_data_init;
		asv_table_ver = v920_asv_table_init();
		FIN_HZ = 38.4 * 1000000;
		break;
	case EXYNOSAUTO_V620_EVT0:
		clk_variant = &clk_variant_v620_evt0;
		pmucal_system_variant_model = &pmucal_system_variant_v620_evt0;
		pmucal_local_variant_model = &pmucal_local_variant_v620_evt0;
		pmucal_powermode_variant_model = &pmucal_powermode_variant_v620_evt0;
		pmucal_rae_variant_model = &pmucal_rae_variant_v620_evt0;
		cal_data_init = exynosautov620_evt0_cal_data_init;
		asv_table_ver = v920_asv_table_init();
		FIN_HZ = 38.4 * 1000000;
		break;
	default:
		break;
	}

	ret = cal_if_init(np);
	if (ret)
		goto out;

	if (!skip_dvfs_fvmap) {
		ret = exynos_acpm_dvfs_init();
		if (ret) {
			pr_err("exynos_acpm_dvfs_init fail\n");
		}
		ret = fvmap_init(get_fvmap_base());
	} else
		pr_err("exynos_acpm_dvfs_init & fvmap_init are skipped.\n");

out:
	return ret;
}

static struct platform_driver samsung_cal_if_driver = {
	.probe	= cal_if_probe,
	.driver	= {
		.name = "exynos-cal-if",
		.owner	= THIS_MODULE,
		.of_match_table	= cal_if_match,
	},
};

static int exynos_cal_if_init(void)
{
	int ret;
	ret = platform_driver_register(&samsung_cal_if_driver);
	if (ret) {
		pr_err("samsung_cal_if_driver probe fail\n");
		goto err_out;
	}

err_out:
	return ret;
}
arch_initcall(exynos_cal_if_init);

MODULE_LICENSE("GPL");
