// SPDX-License-Identifier: GPL-2.0
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include <soc/samsung/cal-if.h>
#include "../pwrcal-env.h"
#include "include/pmucal_system.h"
#include "include/pmucal_powermode.h"
#include "include/pmucal_rae.h"

struct pmucal_powermode_variant *pmucal_powermode_variant_model;

struct pmucal_powermode_variant pmucal_powermode_variant_v9 = {
	.p_pmucal_cpuinform_list = pmucal_cpuinform_list_v9,
	.p_cpu_inform_list_size = &cpu_inform_list_size_v9,
};

struct pmucal_powermode_variant pmucal_powermode_variant_v920_evt0 = {
	.p_pmucal_cpuinform_list = pmucal_cpuinform_list_v920_evt0,
	.p_cpu_inform_list_size = &cpu_inform_list_size_v920_evt0,
};

struct pmucal_powermode_variant pmucal_powermode_variant_v920_evt2 = {
	.p_pmucal_cpuinform_list = pmucal_cpuinform_list,
	.p_cpu_inform_list_size = &cpu_inform_list_size,
};

struct pmucal_powermode_variant pmucal_powermode_variant_v620_evt0 = {
	.p_pmucal_cpuinform_list = pmucal_cpuinform_list_v620_evt0,
	.p_cpu_inform_list_size = &cpu_inform_list_size_v620_evt0,
};

void pmucal_powermode_hint(unsigned int mode)
{
	unsigned int cpu = smp_processor_id();

	__raw_writel(mode, pmucal_powermode_variant_model->p_pmucal_cpuinform_list[cpu].base_va
			+ pmucal_powermode_variant_model->p_pmucal_cpuinform_list[cpu].offset);
}

u32 pmucal_get_powermode_hint(int cpu)
{
	return __raw_readl(pmucal_powermode_variant_model->p_pmucal_cpuinform_list[cpu].base_va
			+ pmucal_powermode_variant_model->p_pmucal_cpuinform_list[cpu].offset);
}

void pmucal_powermode_hint_clear(void)
{
	unsigned int cpu = smp_processor_id();

	__raw_writel(0, pmucal_powermode_variant_model->p_pmucal_cpuinform_list[cpu].base_va
			+ pmucal_powermode_variant_model->p_pmucal_cpuinform_list[cpu].offset);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To map physical and virtual address for CPU_INFORM"
 * @logic "store virtual address in cpuinform structure"
 * @noparam
 * @retval{-, -, -, >=0, <0}
 */
int pmucal_cpuinform_init(void)
{
	int i, j;

	for (i = 0; i < *pmucal_powermode_variant_model->p_cpu_inform_list_size; i++) {
		for (j = 0; j < *pmucal_rae_variant_model->p_pmucal_p2v_list_size; j++)
			if (pmucal_rae_variant_model->p_pmucal_p2v_list[j].pa == (phys_addr_t)pmucal_powermode_variant_model->p_pmucal_cpuinform_list[i].base_pa)
				break;

		if (j != *pmucal_rae_variant_model->p_pmucal_p2v_list_size) {
			pmucal_powermode_variant_model->p_pmucal_cpuinform_list[i].base_va = pmucal_rae_variant_model->p_pmucal_p2v_list[j].va;
		} else {
			pr_err("%s %s: there is no such PA in p2v_list (idx:%d)\n",
					PMUCAL_PREFIX, __func__, i);
			return -ENOENT;
		}

	}

	return 0;
}
