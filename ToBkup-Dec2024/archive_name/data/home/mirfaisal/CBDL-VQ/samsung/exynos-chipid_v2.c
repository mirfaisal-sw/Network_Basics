// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * EXYNOS - CHIP ID support
 */

#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>
#include <linux/soc/samsung/exynos-soc.h>
#include <linux/module.h>

struct exynos_chipid_info exynos_soc_info;
EXPORT_SYMBOL(exynos_soc_info);

static const char *product_id_to_name(unsigned int product_id)
{
	const char *soc_name;
	unsigned int soc_id = product_id;

	switch (soc_id) {
	case EXYNOSAUTO9_SOC_ID:
		soc_name = "EXYNOSAUTO9";
		break;
	case EXYNOSAUTO915_SOC_ID:
		soc_name = "EXYNOSAUTO915";
		break;
	case EXYNOSAUTO7_SOC_ID:
	case EXYNOSAUTO7I_SOC_ID:
		soc_name = "EXYNOSAUTO7";
		break;
	case EXYNOSAUTOV920_SOC_ID:
		soc_name = "EXYNOSAUTOV920";
		break;
	case EXYNOSAUTOV820_SOC_ID:
		soc_name = "EXYNOSAUTOV820";
		break;
	case EXYNOSAUTOV720_SOC_ID:
		soc_name = "EXYNOSAUTOV720";
		break;
	case EXYNOSAUTOV620D_SOC_ID:
		soc_name = "EXYNOSAUTOV620D";
		break;
	case EXYNOSAUTOV620_SOC_ID:
		soc_name = "EXYNOSAUTOV620";
		break;
	default:
		soc_name = "UNKNOWN";
	}
	return soc_name;
}

static char lot_id[6];

static u32 chipid_reverse_value(u32 val, u32 bitcnt)
{
	u32 temp, ret = 0;
	u32 i;

	for (i = 0; i < bitcnt; i++) {
		temp = (val >> i) & 0x1;
		ret += temp << ((bitcnt - 1) - i);
	}

	return ret;
}

static void chipid_dec_to_36(u32 in, char *p)
{
	const struct exynos_chipid_variant *data = exynos_soc_info.drv_data;

	u32 mod;
	u32 i;
	u32 val;

	for (i = 4; i >= 1; i--) {
		mod = in % 36;
		in /= 36;
		p[i] = (mod < 10) ? (mod + '0') : (mod - 10 + 'A');
	}

	val = __raw_readl(exynos_soc_info.reg + data->unique_id_reg + 0x4);
	val = (val >> 10) & 0x3;

	switch (val) {
	case 0:
		p[0] = 'N';
		break;
	case 1:
		p[0] = 'S';
		break;
	case 2:
		p[0] = 'A';
		break;
	case 3:
	default:
		break;
	}

	p[5] = 0;
}

/*
 *  sysfs implementation for exynos-snapshot
 *  you can access the sysfs of exynos-snapshot to /sys/devices/system/chip-id
 *  path.
 */
static struct bus_type chipid_subsys = {
	.name = "chip-id",
	.dev_name = "chip-id",
};

static ssize_t product_id_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 10, "%08X\n", exynos_soc_info.product_id);
}

static ssize_t unique_id_show(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 20, "%010llX\n", exynos_soc_info.unique_id);
}

static ssize_t lot_id_show(struct device *dev,
			   struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 14, "%08X\n", exynos_soc_info.lot_id);
}

static ssize_t lot_id2_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 14, "%s\n", exynos_soc_info.lot_id2);
}

static ssize_t revision_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 14, "%08X\n", exynos_soc_info.revision);
}

static ssize_t evt_ver_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	if (exynos_soc_info.revision == 0)
		return snprintf(buf, 14, "EVT0\n");
	else
		return snprintf(buf, 14, "EVT%1X.%1X\n",
				exynos_soc_info.main_rev,
				exynos_soc_info.sub_rev);
}

static DEVICE_ATTR_RO(product_id);
static DEVICE_ATTR_RO(unique_id);
static DEVICE_ATTR_RO(lot_id);
static DEVICE_ATTR_RO(lot_id2);
static DEVICE_ATTR_RO(revision);
static DEVICE_ATTR_RO(evt_ver);

static struct attribute *chipid_sysfs_attrs[] = {
	&dev_attr_product_id.attr,
	&dev_attr_unique_id.attr,
	&dev_attr_lot_id.attr,
	&dev_attr_lot_id2.attr,
	&dev_attr_revision.attr,
	&dev_attr_evt_ver.attr,
	NULL,
};

static struct attribute_group chipid_sysfs_group = {
	.attrs = chipid_sysfs_attrs,
};

static const struct attribute_group *chipid_sysfs_groups[] = {
	&chipid_sysfs_group,
	NULL,
};

static int chipid_sysfs_init(void)
{
	int ret = 0;

	ret = subsys_system_register(&chipid_subsys, chipid_sysfs_groups);
	if (ret) {
		if (exynos_soc_info.pdev) {
			dev_err(&exynos_soc_info.pdev->dev,
				"fail to register chip-id subsys\n");
		} else {
			pr_err("fail to register chip-id subsys\n");
		}
	}

	return ret;
}

static void exynosauto9_chipid_get_chipid_info(void)
{
	const struct exynos_chipid_variant *data = exynos_soc_info.drv_data;
	u64 val;
	u32 temp;

	val = __raw_readl(exynos_soc_info.reg);

	switch (data->product_ver) {
	case 2:
		exynos_soc_info.product_id = val & EXYNOS_SOC_MASK_V2;
		break;
	case 1:
	default:
		exynos_soc_info.product_id = val & EXYNOS_SOC_MASK;
		break;
	}

	/* Variant */
	temp = __raw_readl(exynos_soc_info.reg + data->variant_reg);
	temp = (temp >> data->variant_bit) & data->variant_mask;
	exynos_soc_info.product_id |= temp;

	/* Revision */
	val = __raw_readl(exynos_soc_info.reg + data->rev_reg);
	exynos_soc_info.main_rev = (val >> data->main_rev_bit) &
				    EXYNOS_REV_MASK;
	exynos_soc_info.sub_rev = (val >> data->sub_rev_bit) &
				   EXYNOS_REV_MASK;
	exynos_soc_info.revision = (exynos_soc_info.main_rev << 4) |
				    exynos_soc_info.sub_rev;

	/* Unique ID */
	val = __raw_readl(exynos_soc_info.reg + data->unique_id_reg);
	val |= (u64)__raw_readl(exynos_soc_info.reg + data->unique_id_reg + 4)
		<< 32UL;
	exynos_soc_info.unique_id  = val;
	exynos_soc_info.lot_id = val & EXYNOS_LOTID_MASK;

	/* Lot ID */
	temp = __raw_readl(exynos_soc_info.reg + data->unique_id_reg);
	temp = chipid_reverse_value(temp, 32);
	temp = (temp >> 11) & EXYNOS_LOTID_MASK;
	chipid_dec_to_36(temp, lot_id);
	exynos_soc_info.lot_id2 = lot_id;
}

static void exynosautov920_emul_chipid_get_chipid_info(void)
{
	exynos_soc_info.product_id = EXYNOSAUTOV920_SOC_ID;
}

static void exynosautov620_emul_chipid_get_chipid_info(void)
{
	exynos_soc_info.product_id = EXYNOSAUTOV620_SOC_ID;
}

static const struct exynos_chipid_variant drv_data_exynosauto9 = {
	.product_ver	= 1,
	.unique_id_reg	= 0x04,
	.rev_reg	= 0x10,
	.main_rev_bit	= 20,
	.sub_rev_bit	= 16,
	.variant_reg	= 0x24,
	.variant_bit	= 0,
	.variant_mask	= 0x3,
	.get_chipid_info = exynosauto9_chipid_get_chipid_info,
};

static const struct exynos_chipid_variant drv_data_exynosautov920 = {
	.product_ver    = 1,
	.unique_id_reg  = 0x04,
	.rev_reg        = 0x10,
	.main_rev_bit   = 20,
	.sub_rev_bit    = 16,
	.variant_reg	= 0x14,
	.variant_bit	= 24,
	.variant_mask	= 0xF,
	.get_chipid_info = exynosauto9_chipid_get_chipid_info,
};

static const struct exynos_chipid_variant drv_data_exynosautov620 = {
	.product_ver    = 1,
	.unique_id_reg  = 0x04,
	.rev_reg        = 0x10,
	.main_rev_bit   = 20,
	.sub_rev_bit    = 16,
	.variant_reg	= 0x14,
	.variant_bit	= 24,
	.variant_mask	= 0xF,
	.get_chipid_info = exynosauto9_chipid_get_chipid_info,
};

static const struct exynos_chipid_variant drv_data_exynosautov920_emul = {
	.get_chipid_info = exynosautov920_emul_chipid_get_chipid_info,
};

static const struct exynos_chipid_variant drv_data_exynosautov620_emul = {
	.get_chipid_info = exynosautov620_emul_chipid_get_chipid_info,
};

static const struct of_device_id of_exynos_chipid_ids[] = {
	{
		.compatible	= "samsung,exynosauto9-chipid",
		.data		= &drv_data_exynosauto9,
	}, {
		.compatible	= "samsung,exynosautov920-chipid",
		.data		= &drv_data_exynosautov920,
	}, {
		.compatible	= "samsung,exynosautov920-emul-chipid",
		.data		= &drv_data_exynosautov920_emul,
	}, {
		.compatible	= "samsung,exynosautov620-chipid",
		.data		= &drv_data_exynosautov620,
	}, {
		.compatible	= "samsung,exynosautov620-emul-chipid",
		.data		= &drv_data_exynosautov620_emul,
	},
	{},
};

/**
 *  exynos_chipid_early_init: Early chipid initialization
 *  @dev: pointer to chipid device
 */
static int exynos_chipid_early_init(void)
{
	struct device_node *np;
	const struct of_device_id *match;

	if (exynos_soc_info.reg)
		return 0;

	np = of_find_matching_node_and_match(NULL, of_exynos_chipid_ids,
					     &match);
	if (!np || !match) {
		pr_warn("%s, failed to find chipid node or match\n", __func__);
		return -ENODEV;
	}

	if (!of_device_is_available(np)) {
		pr_warn("%s, chipid disabled\n", __func__);
		return -ENODEV;
	}

	exynos_soc_info.drv_data = (struct exynos_chipid_variant *)match->data;
	exynos_soc_info.reg = of_iomap(np, 0);
	if (!exynos_soc_info.reg) {
		pr_warn("%s: failed to map registers\n", __func__);
		return -ENXIO;
	}

	if (exynos_soc_info.drv_data &&
	    exynos_soc_info.drv_data->get_chipid_info)
		exynos_soc_info.drv_data->get_chipid_info();

	return 0;
}

static int exynos_chipid_probe(struct platform_device *pdev)
{
	struct soc_device_attribute *soc_dev_attr;
	struct soc_device *soc_dev;
	struct device_node *root;
	int ret;

	soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
	if (!soc_dev_attr)
		return -ENODEV;

	soc_dev_attr->family = "Samsung Exynos";

	root = of_find_node_by_path("/");
	ret = of_property_read_string(root, "model", &soc_dev_attr->machine);
	of_node_put(root);
	if (ret)
		goto free_soc;

	soc_dev_attr->revision = kasprintf(GFP_KERNEL, "EVT%d.%d",
			exynos_soc_info.main_rev, exynos_soc_info.sub_rev);
	if (!soc_dev_attr->revision)
		goto free_soc;

	soc_dev_attr->soc_id = product_id_to_name(exynos_soc_info.product_id);
	soc_dev = soc_device_register(soc_dev_attr);
	if (IS_ERR(soc_dev))
		goto free_rev;

	dev_info(&pdev->dev, "CPU[%s] CPU_REV[0x%x] Detected\n",
			product_id_to_name(exynos_soc_info.product_id),
			exynos_soc_info.revision);
	exynos_soc_info.pdev = pdev;

	chipid_sysfs_init();

	return 0;
free_rev:
	kfree(soc_dev_attr->revision);
free_soc:
	kfree(soc_dev_attr);
	return -EINVAL;
}

static struct platform_driver exynos_chipid_driver = {
	.driver = {
		.name = "exynos-chipid",
		.of_match_table = of_exynos_chipid_ids,
	},
	.probe = exynos_chipid_probe,
};

static int __init exynos_chipid_init(void)
{
	int ret;

	ret = exynos_chipid_early_init();
	if (ret)
		return ret;

	return platform_driver_register(&exynos_chipid_driver);
}
core_initcall(exynos_chipid_init);

MODULE_DESCRIPTION("Exynos ChipID driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:exynos-chipid");
