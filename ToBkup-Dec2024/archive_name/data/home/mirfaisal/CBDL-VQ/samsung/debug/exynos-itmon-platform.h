/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * IPs Traffic Monitor(ITMON) Platform Header for Samsung Exynos SOC
 */

#ifndef _EXYNOS_ITMON_PLATFORM_H_
#define _EXYNOS_ITMON_PLATFORM_H_

#include <linux/notifier.h>
#include <linux/types.h>

#include "exynos-itmon-local-v1.h"

struct itmon_platform_ops {
	int (*probe)(struct platform_device *pdev, void *param);
	int (*remove)(struct platform_device *pdev);
	int (*suspend)(struct device *dev);
	int (*resume)(struct device *dev);
	void (*enable)(bool enabled);
	void (*pd_sync)(const char *pd_name, bool en);
};

struct itmon_info_table {
	struct itmon_platform_ops *ops;
	void *info_table;
};

extern struct atomic_notifier_head itmon_notifier_list;

#endif /*_EXYNOS_ITMON_V1_H_*/
