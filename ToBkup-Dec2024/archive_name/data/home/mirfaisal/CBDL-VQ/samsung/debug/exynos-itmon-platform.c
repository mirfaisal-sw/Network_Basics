// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - IPs Traffic Monitor(ITMON) support
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <soc/samsung/exynos-pd.h>
#include "exynos-itmon-platform.h"
#include "exynos-itmon-local-v1.h"
#include "exynos-itmon-local-v2.h"

#ifdef CONFIG_SOC_EXYNOSAUTO9
extern struct itmon_info_table exynosautov9_itmon_info;
extern struct itmon_info_table exynosautov920_evt0_itmon_info;
extern struct itmon_info_table exynosautov920_itmon_info;
extern struct itmon_info_table exynosautov620_itmon_info;
#endif

static const struct of_device_id itmon_dt_match[] = {
#ifdef CONFIG_SOC_EXYNOSAUTO9
	{
		.compatible = "samsung,exynosautov9-itmon",
		.data = &exynosautov9_itmon_info,
	},
	{
		.compatible = "samsung,exynosautov920-evt0-itmon",
		.data = &exynosautov920_evt0_itmon_info,
	},
	{
		.compatible = "samsung,exynosautov920-itmon",
		.data = &exynosautov920_itmon_info,
	},
	{
		.compatible = "samsung,exynosautov620-itmon",
		.data = &exynosautov620_itmon_info,
	},
#endif
	{},
};
MODULE_DEVICE_TABLE(of, itmon_dt_match);

static bool itmon_probed;
static struct itmon_platform_ops *g_ops;
static void *g_info_table;

/* declare notifier_list */
ATOMIC_NOTIFIER_HEAD(itmon_notifier_list);

void itmon_notifier_chain_register(struct notifier_block *block)
{
	atomic_notifier_chain_register(&itmon_notifier_list, block);
}
EXPORT_SYMBOL(itmon_notifier_chain_register);

void itmon_notifier_chain_unregister(struct notifier_block *block)
{
	atomic_notifier_chain_unregister(&itmon_notifier_list, block);
}
EXPORT_SYMBOL(itmon_notifier_chain_unregister);

void itmon_enable(bool enabled)
{
	if (!itmon_probed) {
		pr_err("%s - is not probed\n", __func__);
		return;
	}

	if (g_ops && g_ops->enable)
		g_ops->enable(enabled);
	else
		pr_warn("%s - g_ops or enable is not registered\n", __func__);
}
EXPORT_SYMBOL(itmon_enable);

void itmon_pd_sync(const char *pd_name, bool en)
{
	if (!itmon_probed) {
		pr_err("%s - is not probed\n", __func__);
		return;
	}

	if (g_ops && g_ops->pd_sync)
		g_ops->pd_sync(pd_name, en);
	else
		pr_warn("%s - g_ops or pd_sync is not registered\n", __func__);
}
EXPORT_SYMBOL(itmon_pd_sync);

#ifdef CONFIG_PM_SLEEP
static int itmon_suspend(struct device *dev)
{
	if (!itmon_probed) {
		dev_err(dev, "%s - is not probed\n", __func__);
		return -EINVAL;
	}

	if (g_ops && g_ops->suspend)
		return g_ops->suspend(dev);

	dev_info(dev, "%s - g_ops or callback is not registered\n", __func__);

	return -EINVAL;
}

static int itmon_resume(struct device *dev)
{
	if (!itmon_probed) {
		dev_err(dev, "%s - is not probed\n", __func__);
		return -EINVAL;
	}

	if (g_ops && g_ops->resume)
		return g_ops->resume(dev);

	dev_info(dev, "%s - g_ops or callback is not registered\n", __func__);

	return -EINVAL;
}
#else
#define itmon_suspend	(NULL)
#define itmon_resume	(NULL)
#endif
static SIMPLE_DEV_PM_OPS(itmon_pm_ops, itmon_suspend, itmon_resume);

#if IS_ENABLED(CONFIG_EXYNOS_PD)
static int exynos_itmon_pd_on_post_handler(struct notifier_block *nb,
						unsigned long l, void *p)
{
	struct exynos_pm_domain *pd = (struct exynos_pm_domain *)p;

	if (!pd)
		return -EINVAL;

	itmon_pd_sync(pd->name, true);

	return 0;
}

static struct notifier_block exynos_itmon_pd_on_post_block = {
	.notifier_call = exynos_itmon_pd_on_post_handler,
};

static int exynos_itmon_pd_off_pre_handler(struct notifier_block *nb,
						unsigned long l, void *p)
{
	struct exynos_pm_domain *pd = (struct exynos_pm_domain *)p;

	if (!pd)
		return -EINVAL;

	itmon_pd_sync(pd->name, false);

	return 0;
}

static struct notifier_block exynos_itmon_pd_off_pre_block = {
	.notifier_call = exynos_itmon_pd_off_pre_handler,
};
#endif

static int itmon_probe(struct platform_device *pdev)
{
	const struct of_device_id *match;
	struct itmon_info_table *itmon_info;
	int ret = -EINVAL;

	if (itmon_probed) {
		dev_err(&pdev->dev, "%s - already probed\n", __func__);
		return ret;
	}

	match = of_match_device(itmon_dt_match, &pdev->dev);
	if (match && match->data) {
		itmon_info = (struct itmon_info_table *)match->data;
		if (itmon_info && itmon_info->ops && itmon_info->info_table) {
			g_ops = itmon_info->ops;
			g_info_table = itmon_info->info_table;

			if (g_ops->probe)
				ret = g_ops->probe(pdev, g_info_table);
		}
	}

#if IS_ENABLED(CONFIG_EXYNOS_PD)
	if (atomic_notifier_chain_register(&exynos_pd_on_post_notifier_list,
			&exynos_itmon_pd_on_post_block) < 0)
		dev_warn(&pdev->dev, "%s failed to register pd_on_post\n",
				__func__);

	if (atomic_notifier_chain_register(&exynos_pd_off_pre_notifier_list,
			&exynos_itmon_pd_off_pre_block) < 0)
		dev_warn(&pdev->dev, "%s failed to register pd_off_pre\n",
				__func__);
#endif

	if (!ret)
		itmon_probed = true;
	else
		dev_err(&pdev->dev, "%s is failed\n", __func__);

	return ret;
}

static int itmon_remove(struct platform_device *pdev)
{
	int ret = -EINVAL;

	if (!itmon_probed) {
		dev_err(&pdev->dev, "%s - is not probed\n", __func__);
		return ret;
	}

	if (g_ops && g_ops->remove)
		ret = g_ops->remove(pdev);
	else
		dev_info(&pdev->dev,
			"%s - g_ops or callback is not registered\n", __func__);

	itmon_probed = false;
	g_ops = NULL;
	g_info_table = NULL;

	return ret;
}

static struct platform_driver exynos_itmon_platform_driver = {
	.probe = itmon_probe,
	.remove = itmon_remove,
	.driver = {
		.name = "exynos-itmon-platform",
		.of_match_table = itmon_dt_match,
		.pm = &itmon_pm_ops,
	},
};

module_platform_driver(exynos_itmon_platform_driver);

MODULE_DESCRIPTION("Samsung Exynos ITMON DRIVER");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:exynos-itmon");
