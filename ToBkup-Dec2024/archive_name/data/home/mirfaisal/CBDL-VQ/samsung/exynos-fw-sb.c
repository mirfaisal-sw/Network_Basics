// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - Firmware Secure Boot support.
 * This is driver do secure boot by sending SMC.
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/dma-mapping.h>
#include <soc/samsung/exynos-fw-sb.h>
#include <soc/samsung/exynos-smc.h>

struct sb_blk *sb_blk_ctx;
struct device_node *fw_sb_node;

static int init_sb_ctx(struct device *dev)
{
	sb_blk_ctx = kzalloc(sizeof(struct sb_blk), GFP_KERNEL);
	if (!sb_blk_ctx)
		goto out_alloc_fail;

	/* buffer init */
	sb_blk_ctx->vir_addr = dma_alloc_coherent(dev,
					sizeof(struct sb_v40_smc_ctx),
					&sb_blk_ctx->phy_addr, GFP_KERNEL);

	if (sb_blk_ctx->vir_addr && sb_blk_ctx->phy_addr) {
		dev_info(dev, "fw-sb: ctx initialized successfully\n");
	} else {
		dev_err(dev, "fw-sb: ctx initialized failed\n");
		goto out_dma_alloc_fail;
	}

	spin_lock_init(&sb_blk_ctx->lock);

	return RV_SUCCESS;

out_dma_alloc_fail:
	dma_free_coherent(dev, sizeof(struct sb_v40_smc_ctx),
				sb_blk_ctx->vir_addr, sb_blk_ctx->phy_addr);
out_alloc_fail:
	dev_err(dev, "kzalloc failed\n");

	kfree(sb_blk_ctx);

	return -1;
}

/* Handler for firmware secureboot failure.
 * The desired function must be implemented.
 */
static void fw_sb_fail_handler(uint64_t virt_addr,
							uint64_t len,
							const char *name)
{
	pr_err("fw-sb: Clear (%s) region.\n", name);

	/* error handling routine */
	memset((void *)virt_addr, 0, len);
}

uint64_t fw_sb_verify(struct fw_context *fw_ctx)
{
	uint64_t ret = RV_SUCCESS;
	struct sb_v40_smc_ctx *sb_ctx = NULL;
	uint64_t regs[4] = {
		SMC_AARCH64_PREFIX | SMC_CM_SECURE_BOOT,
		SB_CHECK_SIGN_NWD,
		(uint64_t)sb_blk_ctx->phy_addr,
		0
	};

	sb_ctx = (struct sb_v40_smc_ctx *)sb_blk_ctx->vir_addr;

	spin_lock(&sb_blk_ctx->lock);

	sb_ctx->signed_img_ptr = fw_ctx->phys_addr;
	sb_ctx->signed_img_len = fw_ctx->size;
	sb_ctx->firmware_type = fw_ctx->fw_type;

	ret = exynos_smc(regs[0], regs[1], regs[2], regs[3]);
	if (ret != RV_SUCCESS) {
		pr_err("fw-sb: (%s) Verification fail (ret: 0x%llx).\n",
				fw_ctx->name, ret);
		fw_sb_fail_handler(fw_ctx->virt_addr,
							fw_ctx->size,
							fw_ctx->name);

		goto out;
	}
	pr_info("fw-sb: (%s) Secure boot success!\n", fw_ctx->name);

out:
	spin_unlock(&sb_blk_ctx->lock);

	return ret;
}
EXPORT_SYMBOL(fw_sb_verify);

static int exynos_fw_sb_probe(struct platform_device *pdev)
{
	int ret = RV_SUCCESS;

	dev_info(&pdev->dev, "fw-sb: probe start.\n");

	ret = init_sb_ctx(&pdev->dev);
	if (ret) {
		dev_err(&pdev->dev, "fw-sb: ctx init failed: 0x%x\n", ret);
		return ret;
	}
	dev_info(&pdev->dev, "fw-sb: probe done!\n");

	return 0;
}

static int exynos_fw_sb_remove(struct platform_device *pdev)
{
	dma_free_coherent(&pdev->dev, sizeof(struct sb_v40_smc_ctx),
				sb_blk_ctx->vir_addr, sb_blk_ctx->phy_addr);

	kfree(sb_blk_ctx);

	dev_info(&pdev->dev, "fw-sb: remove done\n");

	return RV_SUCCESS;
}

static const struct of_device_id exynos_fw_sb_of_match_table[] = {
	{ .compatible = "samsung,exynos-fw-sb" },
	{},
};

static struct platform_driver exynos_fw_sb_driver = {
	.probe = exynos_fw_sb_probe,
	.remove = exynos_fw_sb_remove,
	.driver = {
		.name = "exynos-fw-sb",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(exynos_fw_sb_of_match_table),
	}
};

static int __init fw_sb_init(void)
{
	int ret = RV_SUCCESS;

	pr_info("fw-sb: exynos firmware secureboot driver init.\n");

	fw_sb_node = of_find_compatible_node(NULL, NULL, "samsung,exynos-fw-sb");
	if (fw_sb_node && of_device_is_available(fw_sb_node)) {
		ret = platform_driver_register(&exynos_fw_sb_driver);
		if (ret)
			pr_err("fw-sb: exynos firmware secureboot driver init fail.\n");

		pr_info("fw-sb: exynos firmware secureboot driver init done.\n");
	} else {
		pr_info("fw-sb: exynos firmware secureboot driver is disabled.\n");
	}

	return ret;
}

static void __exit fw_sb_exit(void)
{
	if (fw_sb_node)
		platform_driver_unregister(&exynos_fw_sb_driver);
}

module_init(fw_sb_init);
module_exit(fw_sb_exit);

MODULE_DESCRIPTION("Exynos Firmware Secureboot driver");
MODULE_AUTHOR("<sangkyu.kim@samsung.com>");
MODULE_LICENSE("GPL");
