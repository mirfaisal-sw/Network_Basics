/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#ifndef __PMUCAL_POWERMODE_H___
#define __PMUCAL_POWERMODE_H___

#if IS_ENABLED(CONFIG_FLEXPMU)
#if IS_ENABLED(PWRCAL_TARGET_LINUX)
#include <linux/io.h>
#endif

extern void pmucal_powermode_hint(unsigned int mode);
extern void pmucal_powermode_hint_clear(void);
extern int pmucal_cpuinform_init(void);
#else

#define pmucal_powermode_hint(a) do {} while (0)
#define pmucal_powermode_hint_clear() do {} while (0)
static inline int pmucal_cpuinform_init(void)
{
	return 0;
}

#endif

struct cpu_inform {
	unsigned int cpu_num;
	phys_addr_t base_pa;
	void __iomem *base_va;
	u32 offset;
};

struct pmucal_powermode_variant {
	struct cpu_inform *p_pmucal_cpuinform_list;
	unsigned int *p_cpu_inform_list_size;
};

extern struct cpu_inform pmucal_cpuinform_list_v9[];
extern unsigned int cpu_inform_list_size_v9;

extern struct cpu_inform pmucal_cpuinform_list_v920_evt0[];
extern unsigned int cpu_inform_list_size_v920_evt0;

extern struct cpu_inform pmucal_cpuinform_list[];
extern unsigned int cpu_inform_list_size;

extern struct cpu_inform pmucal_cpuinform_list_v620_evt0[];
extern unsigned int cpu_inform_list_size_v620_evt0;

extern struct pmucal_powermode_variant *pmucal_powermode_variant_model;
extern struct pmucal_powermode_variant pmucal_powermode_variant_v9;
extern struct pmucal_powermode_variant pmucal_powermode_variant_v920_evt0;
extern struct pmucal_powermode_variant pmucal_powermode_variant_v920_evt2;
extern struct pmucal_powermode_variant pmucal_powermode_variant_v620_evt0;
#endif
