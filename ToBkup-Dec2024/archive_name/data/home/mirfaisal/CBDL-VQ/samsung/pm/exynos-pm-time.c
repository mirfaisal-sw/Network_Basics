// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * Exynos PM TIME - support measurement time period of Suspend to RAM.
 */

#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/syscore_ops.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/pm_opp.h>
#include <linux/jiffies.h>

#define EXYNOS_PM_TIME_PREFIX	"EXYNOS-PM-TIME:"

/* ktime related variables */
static u64 suspend_start_t, suspend_end_t;
static u64 resume_start_t, resume_end_t;
struct wakeup_source	*wakelock;

/**
 * @cnotice
 * @prdcode
 * @unit_name{time_stamp}
 * @purpose "store jiffies for suspend operation and calculate resume time for resume operation"
 * @logic "read current jiffies and calculate resume time with jiffies when the syscore_resume is called."
 * @params
 * @param{in, pm_event, unsigned long, >=0}
 * @endparam
 * @retval{return, -, -, >=0, <0}
 */
static int exynos_suspend_time_notifier(struct notifier_block *notifier,
				       unsigned long pm_event, void *v)
{
	switch (pm_event) {
	case PM_SUSPEND_PREPARE:
		suspend_start_t = jiffies;
		pr_info("%s start to measure suspend time\n",
				EXYNOS_PM_TIME_PREFIX);
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static int exynos_resume_time_notifier(struct notifier_block *notifier,
				       unsigned long pm_event, void *v)
{
	switch (pm_event) {
	case PM_POST_SUSPEND:
		resume_end_t = jiffies;
		pr_info("%s resume time %d ms\n", EXYNOS_PM_TIME_PREFIX,
			jiffies_to_msecs(resume_end_t - resume_start_t));
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block exynos_suspend_time_notifier_block = {
	.notifier_call = exynos_suspend_time_notifier,
	/* The highest priority for being notified first */
	.priority = INT_MAX,
};

static struct notifier_block exynos_resume_time_notifier_block = {
	.notifier_call = exynos_resume_time_notifier,
	/* The lowest priority for being notified last */
	.priority = -INT_MAX,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{time_stamp}
 * @purpose "calculate suspend time"
 * @logic "read current jiffies and calculate suspend time with jiffies when the notifier is called."
 * @noparam
 * @retval{return, -, -, >=0, <0}
 */
static int exynos_pm_time_syscore_suspend(void)
{
	suspend_end_t = jiffies;
	pr_info("%s suspend time %d ms\n", EXYNOS_PM_TIME_PREFIX,
			jiffies_to_msecs(suspend_end_t - suspend_start_t));
	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{time_stamp}
 * @purpose "store jiffies value to measure resume time"
 * @logic "store jiffies value to be used for measuring resume time"
 * @noparam
 * @noret
 */
static void exynos_pm_time_syscore_resume(void)
{
	resume_start_t = jiffies;
	pr_info("%s start to measure resume time\n", EXYNOS_PM_TIME_PREFIX);
}

static struct syscore_ops exynos_pm_time_syscore_ops = {
	.suspend	= exynos_pm_time_syscore_suspend,
	.resume		= exynos_pm_time_syscore_resume,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{time_stamp}
 * @purpose "register ops and notifier"
 * @logic "register ops and notifier"
 * @noparam
 * @retval{ret, int, 0, >=0, <0}
 */
static int exynos_pm_time_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	register_syscore_ops(&exynos_pm_time_syscore_ops);
	register_pm_notifier(&exynos_suspend_time_notifier_block);
	register_pm_notifier(&exynos_resume_time_notifier_block);
	wakelock = wakeup_source_register(dev, "wakelock-exynos");
	__pm_stay_awake(wakelock);
	dev_info(dev, "probe done.\n");

	return 0;
}

static const struct of_device_id exynos_pm_time_match[] = {
	{
		.compatible = "samsung,exynos-pm-time",
	},
	{},
};
MODULE_DEVICE_TABLE(of, exynos_pm_time_match);

static const struct platform_device_id exynos_pm_time_ids[] = {
	{ "exynos-pm-time", },
	{ }
};

static struct platform_driver exynos_pm_time_drv = {
	.probe		= exynos_pm_time_probe,
	.id_table	= exynos_pm_time_ids,
	.driver		= {
		.name	= "exynos-pm-time",
		.owner	= THIS_MODULE,
		.of_match_table = exynos_pm_time_match,
	},
};

static int exynos_pm_time_init(void)
{
	return platform_driver_register(&exynos_pm_time_drv);
}
arch_initcall(exynos_pm_time_init);

static void exynos_pm_time_exit(void)
{
	return platform_driver_unregister(&exynos_pm_time_drv);
}
module_exit(exynos_pm_time_exit);

MODULE_DESCRIPTION("Samsung EXYNOS Soc series exynos-pm-time driver");
MODULE_LICENSE("GPL v2");
