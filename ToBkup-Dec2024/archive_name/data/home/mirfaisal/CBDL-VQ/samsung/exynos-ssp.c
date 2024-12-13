/*
 * exynos-ssp.c - Samsung Secure Platform driver for the Exynos
 *
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/of.h>

#define SSP_RET_OK		        0
#define SSP_RET_FAIL		    -1

/* smc to call ldfw functions */
#define SMC_CMD_SSP		        (0xC2001040)
#define SSP_CMD_BOOT		    (0x1)
#define SSP_CMD_BACKUP		    (0x2)
#define SSP_CMD_RESTORE		    (0x3)
#define SSP_CMD_SELF_TEST	    (0x10)
#define SSP_CMD_BOOT_INIT	    (0x11)
#define SSP_CMD_BOOT_CHECK	    (0x12)
#define SSP_CMD_RESTORE_INIT	(0x31)
#define SSP_CMD_RESTORE_CHECK	(0x32)

static int exynos_ssp_probe(struct platform_device *pdev)
{
    pr_info("[Exynos][CMSSP]exynos-ssp: probe done!\n");
    return SSP_RET_OK;
}

static int exynos_ssp_remove(struct platform_device *pdev)
{
    pr_info("[Exynos][CMSSP]exynos-ssp: remove done\n");
    return SSP_RET_OK;
}


static int exynos_cm_smc(uint64_t *arg0, uint64_t *arg1,
			 uint64_t *arg2, uint64_t *arg3)
{
	struct arm_smccc_res res;

	arm_smccc_smc(*arg0, *arg1, *arg2, *arg3, 0, 0, 0, 0, &res);

	*arg0 = res.a0;
	*arg1 = res.a1;
	*arg2 = res.a2;
	*arg3 = res.a3;

	return *arg0;
}

static int exynos_ssp_suspend(struct device *dev)
{
	int ret = SSP_RET_OK;
	uint64_t reg0;
	uint64_t reg1;
	uint64_t reg2;
	uint64_t reg3;

	reg0 = SMC_CMD_SSP;
	reg1 = SSP_CMD_BACKUP;
	reg2 = 0;
	reg3 = 0;

	ret = exynos_cm_smc(&reg0, &reg1, &reg2, &reg3);

	if (ret != SSP_RET_OK)
		pr_err("[Exynos][CMSSP] %s: fail to backup at ldfw. ret = 0x%x\n", __func__, ret);
	else
		pr_info("[Exynos][CMSSP] ssp backup done\n");

	return ret;
}

static int exynos_ssp_resume(struct device *dev)
{
    int ret = SSP_RET_OK;
	uint64_t reg0;
	uint64_t reg1;
	uint64_t reg2;
	uint64_t reg3;

	reg0 = SMC_CMD_SSP;
	reg1 = SSP_CMD_RESTORE_INIT;
	reg2 = 0;
	reg3 = 0;

	ret = exynos_cm_smc(&reg0, &reg1, &reg2, &reg3);

	if (ret != SSP_RET_OK) {
		pr_err("[Exynos][CMSSP] %s: fail to restore check at ldfw. ret = 0x%x\n", __func__, ret);
		goto  END;
    } else
		pr_info("[Exynos][CMSSP] ssp restore init done\n");

	reg0 = SMC_CMD_SSP;
	reg1 = SSP_CMD_RESTORE_CHECK;
	reg2 = 0;
	reg3 = 0;

	ret = exynos_cm_smc(&reg0, &reg1, &reg2, &reg3);

	if (ret != SSP_RET_OK)
		pr_err("[Exynos][CMSSP] %s: fail to restore check at ldfw. ret = 0x%x\n", __func__, ret);
	else
		pr_info("[Exynos][CMSSP] ssp restore check done\n");

END:
	return ret;

}

static const struct of_device_id exynos_ssp_match[] = {
	{
		.compatible = "samsung,exynos-ssp",
	},
	{},
};

static const struct dev_pm_ops exynos_ssp_ops = {
	.suspend    = exynos_ssp_suspend,
	.resume     = exynos_ssp_resume,
};



static struct platform_driver exynos_ssp_drv = {
	.probe      = exynos_ssp_probe,
	.remove     = exynos_ssp_remove,
	.driver     = {
		.name   = "exynos-ssp",
		.owner  = THIS_MODULE,
		.pm = &exynos_ssp_ops,
		.of_match_table =  of_match_ptr(exynos_ssp_match),
	},
};


static int __init exynos_ssp_init(void)
{
	int ret = SSP_RET_OK;

	pr_info("[Exynos][CMSSP] driver, (c) 2021 Samsung Electronics\n");

	ret = platform_driver_register(&exynos_ssp_drv);

	if (ret) {
		pr_err("[Exynos][CMSSP] %s: fail to register driver. ret = 0x%x\n", __func__, ret);
	}

	pr_info("[Exynos][CMSSP] %s: exynos ssp driver init done. \n", __func__);

	return 0;
}

static void __exit exynos_ssp_exit(void)
{
	platform_driver_unregister(&exynos_ssp_drv);
}

module_init(exynos_ssp_init);
module_exit(exynos_ssp_exit);

MODULE_DESCRIPTION("Exynos Samsung Secure Platform(strongbox) driver");
MODULE_AUTHOR("<jin_soo.park@samsung.com>");
MODULE_LICENSE("GPL");

