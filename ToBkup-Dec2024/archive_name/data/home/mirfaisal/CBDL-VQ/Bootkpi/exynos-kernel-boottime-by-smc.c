// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * EXYNOS - Exynosauto Boot time measurement support
 */

#include <linux/types.h>

#include <linux/soc/samsung/exynos-kernel-boottime.h>
#include <soc/samsung/exynos-smc.h>

#ifdef CONFIG_VLM
#include <nk/nkern.h>
#endif

static u32 get_vm_category(void)
{
	static u32 category = 0xffffffff;

	if (category == 0xffffffff) {
#ifdef CONFIG_VLM
		static int vmid_for_category = -1;

		if (vmid_for_category < 0)
			vmid_for_category = (int)hyp_call_id_get();

		if (vmid_for_category == 2)
			category = EXYNOSAUTO_TIMELINE_DOM_VM2_BASE;
		else if (vmid_for_category == 3)
			category = EXYNOSAUTO_TIMELINE_DOM_VM3_BASE;
		else if (vmid_for_category == 4)
			category = EXYNOSAUTO_TIMELINE_DOM_VM4_BASE;
		else if (vmid_for_category == 5)
			category = EXYNOSAUTO_TIMELINE_DOM_VM5_BASE;
		else
			return 0x0;

#else
		category = EXYNOSAUTO_TIMELINE_BARE_KERNEL_BASE;
#endif
	}

	return category;
}

void exynos_kernel_boottime_log(u32 index)
{
	exynos_smc(SMC_CMD_TIMESTAMP, get_vm_category(), index, 0);
}
EXPORT_SYMBOL(exynos_kernel_boottime_log);

void exynos_kernel_smc_boottime_log(u32 index)
{
	exynos_smc(SMC_CMD_TIMESTAMP, get_vm_category(), index, 0);
}
EXPORT_SYMBOL(exynos_kernel_smc_boottime_log);
