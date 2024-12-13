// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * Exynos PM domain debugfs support.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <soc/samsung/exynos-pd.h>

#ifdef CONFIG_DEBUG_FS
static struct dentry *exynos_pd_dbg_root;

static int exynos_pd_dbg_long_test(struct device *dev)
{
	int ret, i;

	pr_info("%s %s: test start.\n", EXYNOS_PD_DBG_PREFIX, __func__);

	if (pm_runtime_enabled(dev) && pm_runtime_active(dev)) {
		ret = pm_runtime_put_sync(dev);
		if (ret) {
			pr_err("%s %s: put sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
			return ret;
		}
	}

	for (i = 0; i < 100; i++) {
		ret = pm_runtime_get_sync(dev);
		if (ret) {
			pr_err("%s %s: get sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
			return ret;
		}
		mdelay(50);
		ret = pm_runtime_put_sync(dev);
		if (ret) {
			pr_err("%s %s: put sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
			return ret;
		}
		mdelay(50);
	}

	pr_info("%s %s: test done.\n", EXYNOS_PD_DBG_PREFIX, __func__);

	return ret;
}

static struct generic_pm_domain *exynos_pd_dbg_dev_to_genpd(struct device *dev)
{
	if (IS_ERR_OR_NULL(dev->pm_domain))
		return ERR_PTR(-EINVAL);

	return pd_to_genpd(dev->pm_domain);
}

static void exynos_pd_dbg_genpd_lock_spin(struct generic_pm_domain *genpd)
	__acquires(&genpd->slock)
{
	unsigned long flags;

	spin_lock_irqsave(&genpd->slock, flags);
	genpd->lock_flags = flags;
}

static void exynos_pd_dbg_genpd_unlock_spin(struct generic_pm_domain *genpd)
	__releases(&genpd->slock)
{
	spin_unlock_irqrestore(&genpd->slock, genpd->lock_flags);
}

static void exynos_pd_dbg_genpd_lock(struct generic_pm_domain *genpd)
{
	if (genpd->flags & GENPD_FLAG_IRQ_SAFE)
		exynos_pd_dbg_genpd_lock_spin(genpd);
	else
		mutex_lock(&genpd->mlock);
}

static void exynos_pd_dbg_genpd_unlock(struct generic_pm_domain *genpd)
{
	if (genpd->flags & GENPD_FLAG_IRQ_SAFE)
		exynos_pd_dbg_genpd_unlock_spin(genpd);
	else
		mutex_unlock(&genpd->mlock);
}

static void exynos_pd_dbg_summary_show(struct generic_pm_domain *genpd)
{
	static const char * const gpd_status_lookup[] = {
		[GENPD_STATE_ON] = "on",
		[GENPD_STATE_OFF] = "off"
	};
	static const char * const rpm_status_lookup[] = {
		[RPM_ACTIVE] = "active",
		[RPM_RESUMING] = "resuming",
		[RPM_SUSPENDED] = "suspended",
		[RPM_SUSPENDING] = "suspending"
	};
	const char *p = "";
	struct pm_domain_data *pm_data;
	struct gpd_link *link;

	exynos_pd_dbg_genpd_lock(genpd);

	if (genpd->status >= ARRAY_SIZE(gpd_status_lookup) ||
			(genpd->status < 0)) {
		pr_err("%s invalid GPD_STATUS\n", EXYNOS_PD_DBG_PREFIX);
		exynos_pd_dbg_genpd_unlock(genpd);
		return;
	}

	pr_info("[GENPD] : %-30s [GPD_STATUS] : %-15s [ALWAYS-ON] : %-15s\n",
			genpd->name, gpd_status_lookup[genpd->status],
			(genpd->flags & GENPD_FLAG_ALWAYS_ON) ? "on" : "off");

	list_for_each_entry(pm_data, &genpd->dev_list, list_node) {
		if (pm_data->dev->power.runtime_error)
			p = "error";
		else if (pm_data->dev->power.disable_depth)
			p = "unsupported";
		else if (pm_data->dev->power.runtime_status < ARRAY_SIZE(rpm_status_lookup)
				&& (pm_data->dev->power.runtime_status >= 0)) {
			p = rpm_status_lookup[pm_data->dev->power.runtime_status];
		} else
			WARN_ON(1);

		pr_info("  [DEV] : %-30s [RPM_STATUS] : %-15s\n",
					dev_name(pm_data->dev), p);
	}

	list_for_each_entry(link, &genpd->parent_links, parent_node)
		exynos_pd_dbg_summary_show(link->child);

	exynos_pd_dbg_genpd_unlock(genpd);
}

static ssize_t exynos_pd_dbg_read(struct file *file, char __user *user_buf,
				size_t count, loff_t *ppos)
{
	struct device *dev = file->private_data;
	struct generic_pm_domain *genpd = exynos_pd_dbg_dev_to_genpd(dev);

	exynos_pd_dbg_summary_show(genpd);

	return 0;
}

static int exynos_set_always_on_flag(struct device *dev)
{
	struct generic_pm_domain *genpd = exynos_pd_dbg_dev_to_genpd(dev);

	genpd->flags |= GENPD_FLAG_ALWAYS_ON;
	return genpd->flags;
}

static int exynos_disable_always_on_flag(struct device *dev)
{
	struct generic_pm_domain *genpd = exynos_pd_dbg_dev_to_genpd(dev);

	genpd->flags &= ~GENPD_FLAG_ALWAYS_ON;
	return genpd->flags;
}

static ssize_t exynos_pd_dbg_write(struct file *file, const char __user *user_buf,
				size_t count, loff_t *ppos)
{
	struct device *dev = file->private_data;
	struct generic_pm_domain *genpd = exynos_pd_dbg_dev_to_genpd(dev);
	char buf[32];
	size_t buf_size;

	memset(buf, 0, sizeof(buf));

	buf_size = min(count, (sizeof(buf)-1));
	if (copy_from_user(buf, user_buf, buf_size))
		return -EFAULT;

	switch (buf[0]) {
	case '0':
		if (pm_runtime_put_sync(dev))
			pr_err("%s %s: put sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
		break;
	case '1':
		if (pm_runtime_get_sync(dev))
			pr_err("%s %s: get sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
		break;
	case 'c':
		exynos_pd_dbg_long_test(dev);
		break;
	case 'e':
		if (exynos_set_always_on_flag(dev) & GENPD_FLAG_ALWAYS_ON)
			pr_info("%s %s: ALWAYS-ON flag has been set\n",
					EXYNOS_PD_DBG_PREFIX, genpd->name);
		break;
	case 'd':
		if (!(exynos_disable_always_on_flag(dev) & GENPD_FLAG_ALWAYS_ON))
			pr_info("%s %s: ALWAYS-ON flag has been cleared\n",
					EXYNOS_PD_DBG_PREFIX, genpd->name);
		break;
	case 'n':
		if (exynos_set_always_on_flag(dev) & GENPD_FLAG_ALWAYS_ON)
			pr_info("%s %s: ALWAYS-ON flag has been set\n",
					EXYNOS_PD_DBG_PREFIX, genpd->name);
		if (pm_runtime_get_sync(dev))
			pr_err("%s %s: get sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
		if (pm_runtime_put_sync(dev))
			pr_err("%s %s: put sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
		break;
	case 'f':
		if (!(exynos_disable_always_on_flag(dev) & GENPD_FLAG_ALWAYS_ON))
			pr_info("%s %s: ALWAYS-ON flag has been cleared\n",
					EXYNOS_PD_DBG_PREFIX, genpd->name);
		if (pm_runtime_get_sync(dev))
			pr_err("%s %s: get sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
		if (pm_runtime_put_sync(dev))
			pr_err("%s %s: put sync failed.\n",
					EXYNOS_PD_DBG_PREFIX, __func__);
		break;
	default:
		pr_err("%s %s: Invalid input ['0'|'1'|'c'|'e'|'d'|'n'|'f']\n",
				EXYNOS_PD_DBG_PREFIX, __func__);
		break;
	}

	return count;
}

static const struct file_operations exynos_pd_dbg_fops = {
	.open = simple_open,
	.read = exynos_pd_dbg_read,
	.write = exynos_pd_dbg_write,
	.llseek = default_llseek,
};
#endif

static int exynos_pd_dbg_probe(struct platform_device *pdev)
{
	int ret;
	struct exynos_pd_dbg_info *dbg_info;

	dbg_info = devm_kzalloc(&pdev->dev, sizeof(struct exynos_pd_dbg_info), GFP_KERNEL);
	if (!dbg_info) {
		dev_err(&pdev->dev, "could not allocate mem for dbg_info\n");
		return -ENOMEM;
	}
	dbg_info->dev = &pdev->dev;
#ifdef CONFIG_DEBUG_FS
	if (!exynos_pd_dbg_root) {
		exynos_pd_dbg_root = debugfs_create_dir("exynos-pd", NULL);
		if (!exynos_pd_dbg_root) {
			dev_err(&pdev->dev, "could not create debugfs dir\n");
			return -ENOMEM;
		}
	}

	dbg_info->fops = exynos_pd_dbg_fops;
	dbg_info->d = debugfs_create_file(dev_name(&pdev->dev), 0644,
			exynos_pd_dbg_root, dbg_info->dev, &dbg_info->fops);
	if (!dbg_info->d) {
		dev_err(&pdev->dev, "could not creatd debugfs file\n");
		ret = -ENOMEM;
		goto err_dbgfs_pd;
	}
#endif
	platform_set_drvdata(pdev, dbg_info);

	pm_runtime_enable(&pdev->dev);

	ret = pm_runtime_get_sync(&pdev->dev);
	if (ret) {
		dev_err(&pdev->dev, "get_sync failed.\n");
		goto err_get_sync;
	}

	ret = pm_runtime_put_sync(&pdev->dev);
	if (ret) {
		dev_err(&pdev->dev, "put sync failed.\n");
		goto err_put_sync;
	}
	return 0;

err_get_sync:
err_put_sync:
#ifdef CONFIG_DEBUG_FS
	debugfs_remove_recursive(dbg_info->d);
err_dbgfs_pd:
	debugfs_remove_recursive(exynos_pd_dbg_root);
#endif
	return ret;
}

static int exynos_pd_dbg_remove(struct platform_device *pdev)
{
	struct exynos_pd_dbg_info *dbg_info = platform_get_drvdata(pdev);
	struct device *dev = dbg_info->dev;

	if (pm_runtime_enabled(dev) && pm_runtime_active(dev))
		pm_runtime_put_sync(dev);

	pm_runtime_disable(dev);

#ifdef CONFIG_DEBUG_FS
	debugfs_remove_recursive(dbg_info->d);
	debugfs_remove_recursive(exynos_pd_dbg_root);
#endif
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static int exynos_pd_dbg_runtime_suspend(struct device *dev)
{
	dev_info(dev, "Runtime_Suspend\n");
	return 0;
}

static int exynos_pd_dbg_runtime_resume(struct device *dev)
{
	dev_info(dev, "Runtime_Resume\n");
	return 0;
}

static struct dev_pm_ops exynos_pd_dbg_pm_ops = {
	SET_RUNTIME_PM_OPS(exynos_pd_dbg_runtime_suspend,
			exynos_pd_dbg_runtime_resume,
			NULL)
};

#ifdef CONFIG_OF
static const struct of_device_id exynos_pd_dbg_match[] = {
	{
		.compatible = "samsung,exynos-pd-dbg",
	},
	{},
};
#endif

static struct platform_driver exynos_pd_dbg_drv = {
	.probe		= exynos_pd_dbg_probe,
	.remove		= exynos_pd_dbg_remove,
	.driver		= {
		.name	= "exynos_pd_dbg",
		.owner	= THIS_MODULE,
		.pm	= &exynos_pd_dbg_pm_ops,
#ifdef CONFIG_OF
		.of_match_table = exynos_pd_dbg_match,
#endif
	},
};

static int exynos_pd_dbg_init(void)
{
	return platform_driver_register(&exynos_pd_dbg_drv);
}
subsys_initcall(exynos_pd_dbg_init);

MODULE_LICENSE("GPL v2");