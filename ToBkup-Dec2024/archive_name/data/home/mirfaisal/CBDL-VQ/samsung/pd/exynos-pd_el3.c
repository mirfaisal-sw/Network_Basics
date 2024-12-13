// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *            http://www.samsung.com/
 *
 * EXYNOS - EL3 monitor power domain save/restore support
 * Author: Jang Hyunsung <hs79.jang@samsung.com>
 *
 */

#include <linux/arm-smccc.h>
#include <linux/module.h>
#include <soc/samsung/exynos-el3_mon.h>
#include <soc/samsung/exynos-smc.h>

int exynos_pd_tz_save(unsigned int addr)
{
	struct arm_smccc_res res;

	arm_smccc_smc(SMC_CMD_PREPARE_PD_ONOFF, EXYNOS_GET_IN_PD_DOWN, addr,
		      RUNTIME_PM_TZPC_GROUP, 0, 0, 0, 0, &res);
	if (res.a0)
		return (int)res.a0;

	return 0;
}
EXPORT_SYMBOL(exynos_pd_tz_save);

int exynos_pd_tz_restore(unsigned int addr)
{
	struct arm_smccc_res res;

	arm_smccc_smc(SMC_CMD_PREPARE_PD_ONOFF, EXYNOS_WAKEUP_PD_DOWN, addr,
		      RUNTIME_PM_TZPC_GROUP, 0, 0, 0, 0, &res);
	if (res.a0)
		return (int)res.a0;

	return 0;
}
EXPORT_SYMBOL(exynos_pd_tz_restore);

MODULE_DESCRIPTION("Exynos EL3 Monitor PD support driver");
MODULE_LICENSE("GPL v2");
