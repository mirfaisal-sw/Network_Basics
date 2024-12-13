// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * EXYNOS - CPU PMU(Power Management Unit) support
 *
 */

#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/smp.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <soc/samsung/exynos-pmu.h>

/**
 * "pmureg" has the mapped base address of PMU(Power Management Unit)
 */
static struct regmap *pmureg;
static void __iomem *pmu_alive;
static spinlock_t update_lock;

/* Atomic operation for PMU_ALIVE registers. (offset 0~0x3FFF)
 * When the targer register can be accessed by multiple masters,
 * This functions should be used.
 */
static inline void exynos_pmu_set_bit_atomic(unsigned int offset,
					     unsigned int val)
{
	__raw_writel(val, pmu_alive + (offset | 0xc000));
}

static inline void exynos_pmu_clr_bit_atomic(unsigned int offset,
					     unsigned int val)
{
	__raw_writel(val, pmu_alive + (offset | 0x8000));
}

/**
 * No driver refers the "pmureg" directly, through the only exported API.
 */
int exynos_pmu_read(unsigned int offset, unsigned int *val)
{
	return regmap_read(pmureg, offset, val);
}
EXPORT_SYMBOL(exynos_pmu_read);

int exynos_pmu_write(unsigned int offset, unsigned int val)
{
	return regmap_write(pmureg, offset, val);
}
EXPORT_SYMBOL(exynos_pmu_write);

int exynos_pmu_update(unsigned int offset, unsigned int mask, unsigned int val)
{
	int i;
	unsigned long flags;

	if (offset > 0x3fff)
		return regmap_update_bits(pmureg, offset, mask, val);
	else {
		spin_lock_irqsave(&update_lock, flags);
		for (i = 0; i < 32; i++) {
			if (mask & (1 << i)) {
				if (val & (1 << i))
					exynos_pmu_set_bit_atomic(offset, i);
				else
					exynos_pmu_clr_bit_atomic(offset, i);
			}
		}
		spin_unlock_irqrestore(&update_lock, flags);
		return 0;
	}
}
EXPORT_SYMBOL(exynos_pmu_update);

static const struct of_device_id of_exynos_pmu_match[] = {
	{ .compatible = "samsung,exynos-pmu", },
	{ },
};
MODULE_DEVICE_TABLE(of, of_exynos_pmu_match);

struct regmap *exynos_get_pmu_regmap(void)
{
	struct device_node *np;

	if (pmureg)
		return pmureg;

	pr_info("%s exynos_pmu_if was not probed yet. Parse from dt.\n", __func__);

	np = of_find_matching_node(NULL, of_exynos_pmu_match);
	if (np)
		return syscon_regmap_lookup_by_phandle(np, "samsung,syscon-phandle");

	return ERR_PTR(-ENODEV);
}
EXPORT_SYMBOL_GPL(exynos_get_pmu_regmap);

#ifdef CONFIG_DEBUG_FS
static void exynos_pmu_debugfs_init(void)
{
	struct dentry *root;

	root = debugfs_create_dir("exynos-pmu-if", NULL);
	if (!root) {
		pr_err("%s: could't create debugfs dir\n", __func__);
		return;
	}
}
#endif

static int exynos_pmu_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct resource *res;

	pmureg = syscon_regmap_lookup_by_phandle(dev->of_node,
						"samsung,syscon-phandle");
	if (IS_ERR(pmureg)) {
		pr_err("Fail to get regmap of PMU\n");
		return PTR_ERR(pmureg);
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pmu_alive");
	pmu_alive = devm_ioremap_resource(dev, res);
	if (IS_ERR(pmu_alive)) {
		pr_err("Failed to get address of PMU_ALIVE\n");
		return PTR_ERR(pmu_alive);
	}
	spin_lock_init(&update_lock);

#ifdef CONFIG_DEBUG_FS
	exynos_pmu_debugfs_init();
#endif
	dev_info(dev, "exynos_pmu_if probe\n");
	return 0;
}

static const struct platform_device_id exynos_pmu_ids[] = {
	{ "exynos-pmu", },
	{ }
};

static struct platform_driver exynos_pmu_if_driver = {
	.driver = {
		.name = "exynos-pmu-if",
		.of_match_table = of_exynos_pmu_match,
	},
	.probe		= exynos_pmu_probe,
	.id_table	= exynos_pmu_ids,
};

static int exynos_pmu_if_init(void)
{
	return platform_driver_register(&exynos_pmu_if_driver);
}
core_initcall(exynos_pmu_if_init);

static void exynos_pmu_if_exit(void)
{
	return platform_driver_unregister(&exynos_pmu_if_driver);
}
module_exit(exynos_pmu_if_exit);

MODULE_LICENSE("GPL");
