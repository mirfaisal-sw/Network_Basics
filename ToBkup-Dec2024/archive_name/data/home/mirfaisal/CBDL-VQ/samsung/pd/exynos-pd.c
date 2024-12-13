// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * Exynos PM domain support for PMUCAL 3.0 interface.
 */

#include <soc/samsung/exynos-pd.h>
#include <soc/samsung/cal-if.h>
#include <linux/module.h>
#include <linux/suspend.h>

static bool exynos_pd_log_enable;
static bool exynos_pd_notifier_registered;

ATOMIC_NOTIFIER_HEAD(exynos_pd_on_post_notifier_list);
EXPORT_SYMBOL(exynos_pd_on_post_notifier_list);

ATOMIC_NOTIFIER_HEAD(exynos_pd_off_pre_notifier_list);
EXPORT_SYMBOL(exynos_pd_off_pre_notifier_list);

static int exynos_pd_notifier(struct notifier_block *notifier,
				       unsigned long pm_event, void *v)
{
	switch (pm_event) {
	case PM_SUSPEND_PREPARE:
		exynos_pd_log_enable = true;
		break;
	case PM_POST_SUSPEND:
		exynos_pd_log_enable = false;
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block exynos_pd_notifier_block = {
	.notifier_call = exynos_pd_notifier,
};

struct exynos_pm_domain *exynos_pd_lookup_name(const char *domain_name)
{
	struct exynos_pm_domain *exypd = NULL;
	struct device_node *np;

	if (IS_ERR_OR_NULL(domain_name))
		return NULL;

	for_each_compatible_node(np, NULL, "samsung,exynos-pd") {
		struct platform_device *pdev;
		struct exynos_pm_domain *pd;

		if (of_device_is_available(np)) {
			pdev = of_find_device_by_node(np);
			if (!pdev)
				continue;
			pd = platform_get_drvdata(pdev);
			if (!strcmp(pd->name, domain_name)) {
				exypd = pd;
				break;
			}
		}
	}
	return exypd;
}
EXPORT_SYMBOL(exynos_pd_lookup_name);

int exynos_pd_status(struct exynos_pm_domain *pd)
{
	int status;

	if (unlikely(!pd))
		return -EINVAL;

	mutex_lock(&pd->access_lock);
	status = cal_pd_status(pd->cal_pdid);
	mutex_unlock(&pd->access_lock);

	return status;
}
EXPORT_SYMBOL(exynos_pd_status);
/* Power domain on sequence.
 * on_pre, on_post functions are registered as notification handler at CAL code.
 */
static void exynos_pd_power_on_pre(struct exynos_pm_domain *pd)
{
}

static void exynos_pd_power_on_post(struct exynos_pm_domain *pd)
{
	if (cal_pd_status(pd->cal_pdid))
		atomic_notifier_call_chain(&exynos_pd_on_post_notifier_list, 0,
				(void *)pd);
}

static void exynos_pd_power_off_pre(struct exynos_pm_domain *pd)
{
	if (cal_pd_status(pd->cal_pdid))
		atomic_notifier_call_chain(&exynos_pd_off_pre_notifier_list, 0,
				(void *)pd);
}

static void exynos_pd_power_off_post(struct exynos_pm_domain *pd)
{
}

static void exynos_pd_prepare_forced_off(struct exynos_pm_domain *pd)
{
}

static int exynos_pd_power_on(struct generic_pm_domain *genpd)
{
	struct exynos_pm_domain *pd = container_of(genpd, struct exynos_pm_domain, genpd);
	int ret = 0;

	mutex_lock(&pd->access_lock);

	DEBUG_PRINT_INFO("%s(%s)+\n", __func__, pd->name);

	if (pd->power_down_skipped) {
		pr_info(EXYNOS_PD_PREFIX "%s power-on is skipped.\n", pd->name);
		goto acc_unlock;
	}

	exynos_pd_power_on_pre(pd);

	if (exynos_pd_log_enable)
		pr_info(EXYNOS_PD_PREFIX "[%d] %s on\n",
				(pd->cal_pdid & ~BLKPWR_MAGIC), pd->name);
	ret = pd->pd_control(pd->cal_pdid, 1);
	if (ret) {
		pr_err(EXYNOS_PD_PREFIX "%s cannot be powered on\n", pd->name);
		exynos_pd_power_off_post(pd);
		ret = -EAGAIN;
		goto acc_unlock;
	}

	exynos_pd_power_on_post(pd);

acc_unlock:
	DEBUG_PRINT_INFO("%s(%s)-, ret = %d\n", __func__, pd->name, ret);
	mutex_unlock(&pd->access_lock);

	if (ret)
		panic(EXYNOS_PD_PREFIX "%s\n", pd->name);

	return ret;
}

static int exynos_pd_power_off(struct generic_pm_domain *genpd)
{
	struct exynos_pm_domain *pd = container_of(genpd, struct exynos_pm_domain, genpd);
	int ret = 0;

	mutex_lock(&pd->access_lock);

	DEBUG_PRINT_INFO("%s(%s)+\n", __func__, pd->name);

	if (pd->power_down_ok && !pd->power_down_ok()) {
		pr_info(EXYNOS_PD_PREFIX "%s power-off is skipped.\n", pd->name);
		pd->power_down_skipped = true;
		goto acc_unlock;
	}

	exynos_pd_power_off_pre(pd);

	if (exynos_pd_log_enable)
		pr_info(EXYNOS_PD_PREFIX "[%d] %s off\n",
				(pd->cal_pdid & ~BLKPWR_MAGIC), pd->name);
	ret = pd->pd_control(pd->cal_pdid, 0);
	if (unlikely(ret)) {
		if (ret == -4) {
			pr_err(EXYNOS_PD_PREFIX "Timed out during %s  power off! -> forced power off\n", genpd->name);
			exynos_pd_prepare_forced_off(pd);
			ret = pd->pd_control(pd->cal_pdid, 0);
			if (unlikely(ret)) {
				pr_err(EXYNOS_PD_PREFIX "%s occur error at power off!\n", genpd->name);
				goto acc_unlock;
			}
		} else {
			pr_err(EXYNOS_PD_PREFIX "%s occur error at power off!\n", genpd->name);
			goto acc_unlock;
		}
	}

	exynos_pd_power_off_post(pd);
	pd->power_down_skipped = false;

acc_unlock:
	DEBUG_PRINT_INFO("%s(%s)-, ret = %d\n", __func__, pd->name, ret);
	mutex_unlock(&pd->access_lock);

	if (ret)
		panic(EXYNOS_PD_PREFIX "%s\n", pd->name);

	return ret;
}

/**
 *  of_get_devfreq_sync_volt_idx - check devfreq sync voltage idx
 *
 *  Returns the index if the "devfreq-sync-voltage" is described in DT pd node,
 *  -ENOENT otherwise.
 */
static int of_get_devfreq_sync_volt_idx(const struct device_node *device)
{
	int ret;
	u32 val = -1;

	ret = of_property_read_u32(device, "devfreq-sync-voltage", &val);
	if (ret)
		return -ENOENT;

	return val;
}

static int exynos_pd_genpd_init(struct exynos_pm_domain *pd, int state)
{
	pd->genpd.name = pd->name;
	pd->genpd.power_off = exynos_pd_power_off;
	pd->genpd.power_on = exynos_pd_power_on;

	/* pd power on/off latency is less than 1ms */
	pm_genpd_init(&pd->genpd, NULL, state ? false : true);

	pd->genpd.states[0].power_on_latency_ns = 1000000;
	pd->genpd.states[0].power_off_latency_ns = 1000000;

	return 0;
}

static int exynos_pd_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	int ret = 0;
	struct exynos_pm_domain *pd;

	unsigned int index = 0;

	struct exynos_pm_domain *parent_pd;
	struct device_node *parent;
	struct platform_device *parent_pd_pdev;

	int initial_state;
	bool flags;

	if (!of_have_populated_dt()) {
		dev_err(dev, "PM Domain works along with Device Tree\n");
		return -EPERM;
	}

	/* skip unmanaged power domain */
	if (!of_device_is_available(np))
		return -EINVAL;

	pd = devm_kzalloc(dev, sizeof(*pd), GFP_KERNEL);
	if (!pd) {
		dev_err(dev, "%s: failed to allocate memory for domain\n",
				__func__);
		return -ENOMEM;
	}

	/* init exynos_pm_domain's members  */
	pd->name = kstrdup(np->name, GFP_KERNEL);
	ret = of_property_read_u32(np, "cal_id", (u32 *)&pd->cal_pdid);
	if (ret) {
		dev_err(dev, "%s: failed to get cal_pdid  from of %s\n",
				__func__, pd->name);
		return -ENODEV;
	}
	pd->of_node = np;
	pd->pd_control = cal_pd_control;
	pd->check_status = exynos_pd_status;
	pd->devfreq_index = of_get_devfreq_sync_volt_idx(pd->of_node);
	pd->power_down_skipped = false;

	ret = of_property_read_u32(np, "need_smc", (u32 *)&pd->need_smc);
	if (ret) {
		pd->need_smc = 0x0;
	} else {
		cal_pd_set_smc_id(pd->cal_pdid, pd->need_smc);
		dev_info(dev, "%s: %s read need_smc 0x%x successfully.!\n",
				__func__, pd->name, pd->need_smc);
	}
	initial_state = cal_pd_status(pd->cal_pdid);
	if (initial_state == -ENODEV)
		return -EPROBE_DEFER;
	if (initial_state == -EINVAL) {
		dev_err(dev, "%s: %s is in unknown state\n",
				__func__, pd->name);
		return -EINVAL;
	}

	ret = cal_pd_set_first_on(pd->cal_pdid, initial_state);
	if (ret) {
		dev_err(dev, "%s: cmu_init fail: %s, ret:%d\n",
				__func__, pd->name, ret);
		return ret;
	}

	mutex_init(&pd->access_lock);
	platform_set_drvdata(pdev, pd);

	ret = exynos_pd_genpd_init(pd, initial_state);
	if (ret) {
		dev_err(dev, "%s: exynos_pd_genpd_init fail: %s, ret:%d\n",
				__func__, pd->name, ret);
		return ret;
	}

	of_genpd_add_provider_simple(np, &pd->genpd);

	flags = of_property_read_bool(np, "always-on");
	if (flags) {
		pd->genpd.flags |= GENPD_FLAG_ALWAYS_ON;
		pd->always_on = flags;
		dev_info(dev, "    %-9s - %s\n", pd->genpd.name,
				"on,  always");
	} else {
		dev_info(dev, "    %-9s - %-3s\n", pd->genpd.name,
				cal_pd_status(pd->cal_pdid) ? "on" : "off");
	}

	parent = of_parse_phandle(np, "parent", index);
	if (parent) {
		parent_pd_pdev = of_find_device_by_node(parent);
		if (!parent_pd_pdev) {
			dev_info(dev, "parent pd pdev not found");
		} else {
			parent_pd = platform_get_drvdata(parent_pd_pdev);
			if (!parent_pd) {
				dev_info(dev, "parent pd not found");
			} else {
				if (pm_genpd_add_subdomain(&parent_pd->genpd, &pd->genpd))
					dev_err(dev, "%s cannot add subdomain %s\n",
							parent_pd->name, pd->name);
				else
					dev_info(dev, "%s have new subdomain %s\n",
							parent_pd->name, pd->name);
			}
		}
	}
	if (exynos_pd_notifier_registered == false) {
		register_pm_notifier(&exynos_pd_notifier_block);
		exynos_pd_notifier_registered = true;
	}

	dev_info(dev, "probe done.\n");
	return 0;
}

static int exynos_pd_suspend(struct device *dev)
{
	struct platform_device *pdev = container_of(dev,
					struct platform_device, dev);
	struct exynos_pm_domain *pd = platform_get_drvdata(pdev);

	if (IS_ERR(&pd->genpd))
		return 0;
	/* Suspend callback function might be registered if necessary */
	if (pd->always_on) {
		pd->genpd.flags &= ~GENPD_FLAG_ALWAYS_ON;
		dev_info(dev, "%s disable always-on flag\n", pd->genpd.name);
	}
	return 0;
}

static int exynos_pd_resume(struct device *dev)
{
	struct platform_device *pdev = container_of(dev,
					struct platform_device, dev);
	struct exynos_pm_domain *pd = (struct exynos_pm_domain *)platform_get_drvdata(pdev);

	if (IS_ERR(&pd->genpd))
		return 0;
	/* Resume callback function might be registered if necessary */
	if (pd->always_on) {
		pd->genpd.flags |= GENPD_FLAG_ALWAYS_ON;
		dev_info(dev, "%s enable always-on flag\n", pd->genpd.name);
	}
	return 0;
}

static const struct of_device_id exynos_pd_match[] = {
	{
		.compatible = "samsung,exynos-pd",
	},
	{},
};
MODULE_DEVICE_TABLE(of, exynos_pd_match);

static const struct dev_pm_ops exynos_pd_ops = {
	.suspend	= exynos_pd_suspend,
	.resume		= exynos_pd_resume,
};

static const struct platform_device_id exynos_pd_ids[] = {
	{ "exynos-pd", },
	{ }
};

static struct platform_driver exynos_pd_drv = {
	.probe		= exynos_pd_probe,
	.id_table 	= exynos_pd_ids,
	.driver		= {
		.name	= "exynos-pd",
		.owner	= THIS_MODULE,
		.pm	= &exynos_pd_ops,
		.of_match_table = exynos_pd_match,
	},
};

static int exynos_pd_init(void)
{
	return platform_driver_register(&exynos_pd_drv);
}
subsys_initcall(exynos_pd_init);

static void exynos_pd_exit(void)
{
	return platform_driver_unregister(&exynos_pd_drv);
}
module_exit(exynos_pd_exit);

MODULE_SOFTDEP("pre: clk_exynos");
MODULE_LICENSE("GPL");
