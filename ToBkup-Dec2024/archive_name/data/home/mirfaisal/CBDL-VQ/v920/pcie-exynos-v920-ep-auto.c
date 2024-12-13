// SPDX-License-Identifier: GPL-2.0
/**
 * pcie-exynos-ep-auto.c - Exynos PCIe EP driver
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Authors: Sanghoon Bae <sh86.bae@samsung.com>
 *	    Jiheon Oh <jiheon.oh@samsung.com>
 *
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/resource.h>
#include <linux/signal.h>
#include <linux/types.h>
#include <dt-bindings/pci/pci.h>
#include <linux/random.h>
#include <linux/crc32.h>
#include <linux/of_device.h>
#include <linux/samsung_iommu.h>

#ifdef CONFIG_S2MPU
#include <vlx/s2mpu-protection.h>
#include <vlx/s2mpu-protection-v9.h>
#include <vlx/s2mpu-version.h>
#endif

#include "../pcie-designware.h"
#include "pcie-exynos-v920-ep-auto.h"
#include "pcie-exynos-v920-ep-auto-cal.h"
#include <linux/pm_runtime.h>

struct exynos_ep_pcie g_pcie_ep_v920[MAX_EP_NUM];

static void exynos_v920_pcie_ep_send_evt(struct exynos_ep_pcie *pcie_ep,
					 u16 evt);
static int exynos_v920_pcie_ep_msi_alloc(struct exynos_ep_pcie *pcie_ep);
static void exynos_v920_pcie_ep_msi_free(struct exynos_ep_pcie *pcie_ep);

#define MEASURE_SPEED
#ifdef MEASURE_SPEED
u64 getCurrentTimestampInNSec_v920(void)
{
	struct timespec64 timeStamp;

	ktime_get_real_ts64(&timeStamp);

	return (u64)timeStamp.tv_sec * 1000000000ul + (u64)timeStamp.tv_nsec;
}

void measureTransitionSpeed_v920(struct exynos_ep_pcie *pcie_ep, u64 startTime,
		u64 endTime, u64 size)
{
#if DMA_LOG_PRINT_EP
	struct platform_device *pdev = pcie_ep->pdev;
#endif
	int unit = 1; /*	0: KB, 1: MB	*/
	u64 convertedSize = (u64)(size >> 20); /* MByte unit	*/

	convertedSize = size >> 10; /* KByte unit	*/
	unit = 0;

#if DMA_LOG_PRINT_EP
	if (unit == 1) {
		dev_info(&pdev->dev, "###### size: %lldMB , time: %lld ns\n",
				convertedSize, endTime - startTime);
	} else {
		dev_info(&pdev->dev, "###### size: %lldKB , time: %lld ns\n",
				convertedSize, endTime-startTime);
	}
#endif
}
#endif

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Dump register info"
 * @logic "Print ELBI region\n
 *	Print DBI region\n
 *	Print device/link status."
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_register_link_dump(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct dw_pcie *pci = pcie_ep->pci;
	u32 i, j, val;

	dev_info(&pdev->dev, "Print ELBI region...\n");
	for (i = 1; i <= (PCIE_LINK_DBG_2 / 0x10); i++) {
		for (j = 0; j < 4; j++) {
			if (((i * 0x10) + (j * 4)) <= PCIE_LINK_DBG_2) {
				dev_info(&pdev->dev, "ELBI 0x%04x : 0x%08x\n",
					(i * 0x10) + (j * 4),
					readl(pcie_ep->elbi_base +
						(i * 0x10) + (j * 4)));
			}
		}
	}

	dev_info(&pdev->dev, "\n");

	/* RC Conf : 0x0 ~ 0x40 */
	dev_info(&pdev->dev, "Print DBI region...\n");
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			val = dw_pcie_readl_dbi(pci,
						(i * 0x10) + (j * 4));
			dev_info(&pdev->dev, "DBI 0x%04x : 0x%08x\n",
				(i * 0x10) + (j * 4), val);
		}
	}

	dev_info(&pdev->dev, "\n");

	val = dw_pcie_readl_dbi(pci, DEV_STS_CTRL);
	dev_info(&pdev->dev, "EP Conf 0x0078(Device Status Register): 0x%08x\n",
		val);
	val = dw_pcie_readl_dbi(pci, LNK_STS_CTRL);
	dev_info(&pdev->dev, "EP Conf 0x0080(Link Status Register): 0x%08x\n",
		val);
	val = dw_pcie_readl_dbi(pci, AER_UN_ERR_ST);
	dev_info(&pdev->dev, "EP Conf 0x0104(AER Register): 0x%08x\n", val);
	val = dw_pcie_readl_dbi(pci, AER_CO_ERR_ST);
	dev_info(&pdev->dev, "EP Conf 0x0110(AER Register): 0x%08x\n", val);
	val = dw_pcie_readl_dbi(pci, AER_RT_ERR_STS);
	dev_info(&pdev->dev, "EP Conf 0x0130(AER Register): 0x%08x\n", val);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Enable/disable clock"
 * @logic "Call clk_prepare_enable or clk_disable_unprepare"
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, enable, int, 0~1}
 * @endparam
 * @retval{ret, int, 0, <=0, <0}
 */
static int exynos_v920_pcie_ep_clk_enable(struct exynos_ep_pcie *pcie_ep,
	int enable)
{
	struct exynos_pcie_ep_clks *clks = &pcie_ep->clks;
	int i;
	int ret = 0;

	if (enable) {
		for (i = 0; i < pcie_ep->pcie_clk_num; i++)
			ret = clk_prepare_enable(clks->pcie_clks[i]);
#if !defined(CONFIG_SOC_EXYNOSAUTO9)
	} else {
		for (i = 0; i < pcie_ep->pcie_clk_num; i++)
			clk_disable_unprepare(clks->pcie_clks[i]);
#endif
	}

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Enable/disable phy clock"
 * @logic "Call clk_prepare_enable or clk_disable_unprepare"
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, enable, int, 0~1}
 * @endparam
 * @retval{ret, int, 0, <=0, <0}
 */
static int exynos_v920_pcie_ep_phyclk_enable(struct exynos_ep_pcie *pcie_ep,
						int enable)
{
	struct exynos_pcie_ep_clks *clks = &pcie_ep->clks;
	int i;
	int ret = 0;

	if (enable) {
		for (i = 0; i < pcie_ep->phy_clk_num; i++)
			ret = clk_prepare_enable(clks->phy_clks[i]);
#if !defined(CONFIG_SOC_EXYNOSAUTO9)
	} else {
		for (i = 0; i < pcie_ep->phy_clk_num; i++)
			clk_disable_unprepare(clks->phy_clks[i]);
#endif
	}

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Enable/disable PCIe DBI HWACG"
 * @logic "Call clk_prepare_enable or clk_disable_unprepare"
 * @params
 * @param{in, dev, struct device, not NULL}
 * @param{in, dev, struct exynos_pcie_ep_clks, not NULL}
 * @param{in, enable, bool, true/false}
 * @endparam
 * @retval{ret, int, 0, <=0, <0}
 */
static int exynos_v920_pcie_ep_dbiclk_enable(struct device *dev,
					     struct exynos_pcie_ep_clks *clks,
					     bool enable)
{
	int ret = 0;

	if (enable)
		ret = clk_prepare_enable(clks->dbi_hwacg);
	else
		clk_disable_unprepare(clks->dbi_hwacg);

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Initialize PCIe EP phy functions"
 * @logic "Allocate phy functions to ops."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_phyinit(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;

	dev_info(&pdev->dev, "Initialize PCIE EP PHY functions.\n");

	if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) {
		pcie_ep->phy_ops.phy_all_pwrdn = NULL;
		pcie_ep->phy_ops.phy_all_pwrdn_clear = NULL;
		pcie_ep->phy_ops.phy_config =
			exynos_v920_pcie_ep_phy_config;
		pcie_ep->phy_ops.phy_other_set =
			exynos_v920_pcie_ep_phy_others_set;
	} else {
		pcie_ep->phy_ops.phy_all_pwrdn =
			exynos_v920_pcie_ep_phy_all_pwrdn;
		pcie_ep->phy_ops.phy_all_pwrdn_clear =
			exynos_v920_pcie_ep_phy_all_pwrdn_clear;
		pcie_ep->phy_ops.phy_config =
			exynos_v920_pcie_ep_phy_config;
		pcie_ep->phy_ops.phy_other_set =
			exynos_v920_pcie_ep_phy_others_set;
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Get resources from dt"
 * @logic "Get resources from dt\n
 *	Perform I/O remap for each bases."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{ret, int, 0, >=0, >0}
 */
static int exynos_v920_pcie_ep_get_resource(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct resource *temp_rsc;
	int ret = 0;

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "elbi");
	pcie_ep->elbi_base = ioremap(temp_rsc->start, resource_size(temp_rsc));
	if (IS_ERR(pcie_ep->elbi_base)) {
		ret = PTR_ERR(pcie_ep->elbi_base);
		goto fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						"gen_subsys");
	pcie_ep->gen_subsys_base = ioremap(temp_rsc->start,
		resource_size(temp_rsc));

	if (IS_ERR(pcie_ep->gen_subsys_base))
		return PTR_ERR(pcie_ep->gen_subsys_base);

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						"e32_phy");
	pcie_ep->e32_phy_base = ioremap(temp_rsc->start,
		resource_size(temp_rsc));

	if (IS_ERR(pcie_ep->e32_phy_base))
		return PTR_ERR(pcie_ep->e32_phy_base);

	if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) {
		temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							"soc_ctrl");
		pcie_ep->soc_ctrl_base = ioremap(temp_rsc->start,
			resource_size(temp_rsc));

		if (IS_ERR(pcie_ep->soc_ctrl_base))
			return PTR_ERR(pcie_ep->soc_ctrl_base);

		temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							"pmu");
		pcie_ep->pmu_base = ioremap(temp_rsc->start,
			resource_size(temp_rsc));

		if (IS_ERR(pcie_ep->pmu_base))
			return PTR_ERR(pcie_ep->pmu_base);
	} else {
		temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							"cmu");
		pcie_ep->cmu_base = ioremap(temp_rsc->start,
				resource_size(temp_rsc));

		if (IS_ERR(pcie_ep->cmu_base)) {
			ret = PTR_ERR(pcie_ep->cmu_base);
			goto fail;
		}
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						"elbi_other");
	pcie_ep->elbi_base_other = ioremap(temp_rsc->start,
		resource_size(temp_rsc));

	if (IS_ERR(pcie_ep->elbi_base_other))
		return PTR_ERR(pcie_ep->elbi_base_other);

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "phy");
	pcie_ep->phy_base = ioremap(temp_rsc->start, resource_size(temp_rsc));
	if (IS_ERR(pcie_ep->phy_base)) {
		ret = PTR_ERR(pcie_ep->phy_base);
		goto fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "sysreg");
	pcie_ep->sysreg_base = ioremap(temp_rsc->start,
					resource_size(temp_rsc));
	if (IS_ERR(pcie_ep->sysreg_base)) {
		ret = PTR_ERR(pcie_ep->sysreg_base);
		goto fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dbi");
	pcie_ep->dbi_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(pcie_ep->dbi_base)) {
		ret = PTR_ERR(pcie_ep->dbi_base);
		goto fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "atu");
	pcie_ep->atu_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(pcie_ep->pci->atu_base)) {
		ret = PTR_ERR(pcie_ep->pci->atu_base);
		goto fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dma");
	pcie_ep->dma_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(pcie_ep->dma_base)) {
		ret = PTR_ERR(pcie_ep->dma_base);
		goto fail;
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						"doorbell");
	pcie_ep->doorbell_base = devm_ioremap_resource(&pdev->dev, temp_rsc);
	if (IS_ERR(pcie_ep->doorbell_base)) {
		ret = PTR_ERR(pcie_ep->doorbell_base);
		goto fail;
	}

fail:
	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Raise irq"
 * @logic "Dummy function for ops"
 * @params
 * @param{in, ep, struct ::dw_pcie_ep *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_ep_raise_irq(struct dw_pcie_ep *ep,
		u8 func_no, enum pci_epc_irq_type type, u16 interrupt_num)
{
	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "EP init"
 * @logic "Dummy function for ops"
 * @params
 * @param{in, ep, struct ::dw_pcie_ep *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_ep_init(struct dw_pcie_ep *ep)
{

}

static u32 exynos_v920_pcie_ep_readl_atu(struct dw_pcie *pci, u32 reg)
{
	int ret;
	u32 val;

	if (pci->ops && pci->ops->read_dbi)
		return pci->ops->read_dbi(pci, pci->atu_base, reg, 4);

	ret = dw_pcie_read(pci->atu_base + reg, 4, &val);
	if (ret)
		dev_err(pci->dev, "Read ATU address failed\n");

	return val;
}

static void exynos_v920_pcie_ep_writel_atu(struct dw_pcie *pci, u32 reg,
	u32 val)
{
	int ret;

	if (pci->ops && pci->ops->write_dbi) {
		pci->ops->write_dbi(pci, pci->atu_base, reg, 4, val);
		return;
	}

	ret = dw_pcie_write(pci->atu_base + reg, 4, val);
	if (ret)
		dev_err(pci->dev, "Write ATU address failed\n");
}


static void exynos_v920_pcie_ep_writel_ob_unroll(struct dw_pcie *pci,
	u32 index, u32 reg, u32 val)
{
	u32 offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	exynos_v920_pcie_ep_writel_atu(pci, offset + reg, val);
}

static u32 exynos_v920_pcie_ep_readl_ob_unroll(struct dw_pcie *pci, u32 index,
	u32 reg)
{
	u32 offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	return exynos_v920_pcie_ep_readl_atu(pci, offset + reg);
}


static inline u32 exynos_v920_pcie_ep_enable_ecrc(u32 val)
{
	return val | PCIE_ATU_TD;
}


static void exynos_v920_pcie_ep_outbound_atu(struct dw_pcie *pci, u8 func_no,
	u8 vfunc_no, int index, int type, u64 cpu_addr, u64 pci_addr, u64 size)
{
	u32 retries, val;
	u64 limit_addr;

	if (pci->ops && pci->ops->cpu_addr_fixup)
		cpu_addr = pci->ops->cpu_addr_fixup(pci, cpu_addr);

	limit_addr = cpu_addr + size - 1;

	exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
			PCIE_ATU_UNR_LOWER_BASE, lower_32_bits(cpu_addr));
	exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
			PCIE_ATU_UNR_UPPER_BASE, upper_32_bits(cpu_addr));
	exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
			PCIE_ATU_UNR_LOWER_LIMIT, lower_32_bits(limit_addr));
	exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
			PCIE_ATU_UNR_UPPER_LIMIT, upper_32_bits(limit_addr));
	exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
			PCIE_ATU_UNR_LOWER_TARGET, lower_32_bits(pci_addr));
	exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
			PCIE_ATU_UNR_UPPER_TARGET, upper_32_bits(pci_addr));

	val = type | PCIE_ATU_FUNC_NUM(func_no);
	if (upper_32_bits(limit_addr) > upper_32_bits(cpu_addr))
		val |= PCIE_ATU_INCREASE_REGION_SIZE;
	if (pci->version == 0x490A)
		val = exynos_v920_pcie_ep_enable_ecrc(val);
	exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
					PCIE_ATU_UNR_REGION_CTRL1, val);
	exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
				PCIE_ATU_UNR_REGION_CTRL2, PCIE_ATU_ENABLE);

	if (vfunc_no == 0)
		exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
						PCIE_ATU_CR3_OUT0, 0x0);
	else if (vfunc_no > 0)
		exynos_v920_pcie_ep_writel_ob_unroll(pci, index,
			PCIE_ATU_CR3_OUT0, BIT(31) | (vfunc_no-1));

	/*
	 * Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 */
	for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
		val = exynos_v920_pcie_ep_readl_ob_unroll(pci, index,
					      PCIE_ATU_UNR_REGION_CTRL2);
		if (val & PCIE_ATU_ENABLE)
			return;

		mdelay(LINK_WAIT_IATU);
	}
	dev_err(pci->dev, "Outbound iATU is not being enabled\n");
}


static int exynos_v920_pcie_ep_map_addr(struct dw_pcie_ep *ep, u8 func_no,
				u8 vfunc_no, phys_addr_t phys_addr,
				   u64 pci_addr, size_t size)
{
	u32 free_win;
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);

	free_win = find_first_zero_bit(ep->ob_window_map, pci->num_ob_windows);
	if (free_win >= pci->num_ob_windows) {
		dev_err(pci->dev, "No free outbound window\n");
		return -EINVAL;
	}

	exynos_v920_pcie_ep_outbound_atu(pci, func_no, vfunc_no, free_win,
		PCIE_ATU_TYPE_MEM, phys_addr, pci_addr, size);

	set_bit(free_win, ep->ob_window_map);
	ep->outbound_addr[free_win] = phys_addr;

	return 0;
}

static int exynos_v920_pcie_ep_epc_map_addr(struct pci_epc *epc, u8 func_no,
u8 vfunc_no,
		     phys_addr_t phys_addr, u64 pci_addr, size_t size)
{
	int ret;
	struct dw_pcie_ep *ep = epc_get_drvdata(epc);

	mutex_lock(&epc->lock);
	ret = exynos_v920_pcie_ep_map_addr(ep, func_no, vfunc_no, phys_addr,
					pci_addr, size);
	mutex_unlock(&epc->lock);

	return ret;
}

static struct dw_pcie_ep_ops pcie_ep_ops = {
	.ep_init = exynos_v920_pcie_ep_init,
	.raise_irq = exynos_v920_pcie_ep_raise_irq,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Initialize designware endpoint"
 * @logic "Get resource from dt\n
 *	Call dw_pcie_ep_init"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{ret, int, 0, 0/-EINVAL, -EINVAL}
 */
static int exynos_v920_add_pcie_ep(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep;
	struct resource *res;
	int ret = 0;

	ep = &pci->ep;
	ep->ops = &pcie_ep_ops;
	pci->dbi_base = pcie_ep->dbi_base;
	pci->atu_base = pcie_ep->atu_base;
	/* for designware mainline code */
	pci->dbi_base2 = pcie_ep->dbi_base;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "addr_space");
	if (!res) {
		dev_err(dev, "can't get addr_space for PCIe EP from DT\n");
		return -EINVAL;
	}
	ep->phys_base = res->start;
	ep->addr_size = resource_size(res);

	ret = dw_pcie_ep_init(ep);
	if (ret) {
		dev_err(dev, "failed to initialize PCIe dw endpoint\n");
		return ret;
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Setup ep phy"
 * @logic "Call exynos_pcie_reset_ep\n
 *	Setup ep phy config."
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_setup_ep(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct dw_pcie *pci = pcie_ep->pci;
	int ch_num = pcie_ep->ch_num;
	int lane = 0;
	int speed = (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) ?
		EXYNOS_PCIE_EVT2_SPEED_GEN4 : EXYNOS_PCIE_EVT01_SPEED_GEN3;
	int status = (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) ?
			EXYNOS_PCIE_EVT2_LNK_STS : EXYNOS_PCIE_EVT01_LNK_STS;

	u32 val;

	dev_dbg(&pdev->dev, "START PCIe setup EP\n");

	if (pci->dbi_base == NULL)
		exynos_v920_add_pcie_ep(pcie_ep);

	dw_pcie_dbi_ro_wr_en(pci);

	/* Port Link Control Register, set number of lanes to 1 */
	val = dw_pcie_readl_dbi(pci, PL_PT_LNK_CTRL_R);
	val &= PL_PT_LNK_CTRL_LANE_MASK;
	dw_pcie_writel_dbi(pci, PL_PT_LNK_CTRL_R, val);

	val = dw_pcie_readl_dbi(pci, PL_PT_LNK_CTRL_R);

	/* use_bifurcation => 1: bifurcation, 0:aggregation */
	if (ch_num == CH0_4L || ch_num == CH1_4L) {
		if (!pcie_ep->use_bifurcation) {
			lane = PL_PT_LANE_4;
			val |= PL_PT_LNK_CTRL_4LANE;
			dw_pcie_writel_dbi(pci, PL_PT_LNK_CTRL_R, val);
		} else {
			lane = PL_PT_LANE_2;
			val |= PL_PT_LNK_CTRL_2LANE;
			dw_pcie_writel_dbi(pci, PL_PT_LNK_CTRL_R, val);
		}
	} else {
		if (!pcie_ep->use_bifurcation) {
			lane = PL_PT_LANE_2;
			val |= PL_PT_LNK_CTRL_2LANE;
			dw_pcie_writel_dbi(pci, PL_PT_LNK_CTRL_R, val);
		} else {
			lane = PL_PT_LANE_1;
			val |= PL_PT_LNK_CTRL_1LANE;
			dw_pcie_writel_dbi(pci, PL_PT_LNK_CTRL_R, val);
		}
	}

	dev_info(&pdev->dev, "[%s] lane : %d\n", __func__, lane);

	/* Link Width and speed Change control Register */
	val = dw_pcie_readl_dbi(pci, PL_GEN2_CTRL);
	val &= ~PL_GEN2_CTRL_LANE_MASK;
	val |= (lane << 8);
	dw_pcie_writel_dbi(pci, PL_GEN2_CTRL, val);

	/* set max link width & speed : Gen1, lane1 */
	val = dw_pcie_readl_dbi(pci, LNK_CAP);
	val &= LNK_CAP_SPD_WIDTH_MASK;
	val |= (lane << 4);
	val |= (speed << 0);
	dw_pcie_writel_dbi(pci, LNK_CAP, val);
	dw_pcie_writel_dbi(pci, LNK_STS_CTRL2, status);

	dev_dbg(&pdev->dev,
		"[%s] PCIe EP lane number & speed selection\n", __func__);

	/* set N_FTS : 255 */
	val = dw_pcie_readl_dbi(pci, PL_ACK_FREQ_R);
	val &= PL_ACK_N_FTS_MSK;
	val |= PL_ACK_N_FTS_255;
	dw_pcie_writel_dbi(pci, PL_ACK_FREQ_R, val);

	/* enable error reporting */
	dw_pcie_writel_dbi(pci, AER_RT_ERR_CMD, RC_REGMASK_CORRECTABLE_ERR_EN
				| RC_REGMASK_NONFATAL_ERR_EN
				| RC_REGMASK_FATAL_ERR_EN);
	val = dw_pcie_readl_dbi(pci, AER_CO_ERR_MS);
	dw_pcie_writel_dbi(pci, AER_CO_ERR_MS,
			val | AER_CO_ERR_RPL_TIMER_TIMEOUT_MASK);

	/* setup interrupt pins */
	dw_pcie_readl_dbi(pci, DBI_BRIDGE_CTRL_INT_PIN);
	val &= DBI_INT_PIN_MASK;
	val |= DBI_INT_PIN_1;
	dw_pcie_writel_dbi(pci, DBI_BRIDGE_CTRL_INT_PIN, val);

	val = dw_pcie_readl_dbi(pci, DBI_BRIDGE_CTRL_INT_PIN);

	/* setup bus number */
	/* Subordinate 0x01, Secondary Bus no 0x01, Primary bus no 0x00 */
	val = dw_pcie_readl_dbi(pci, DBI_PRIM_SEC_BUS);
	val &= DBI_SEC_LAT_TIMER_MASK;
	val |= DBI_SUB_BUS_FF_SEC_BUS_1;
	dw_pcie_writel_dbi(pci, DBI_PRIM_SEC_BUS, val);

	/* setup command register */
	val = dw_pcie_readl_dbi(pci, PCI_COMMAND);
	val &= DBI_COMMAND_MASK;
	val |= (PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER |
			PCI_COMMAND_PARITY | PCI_COMMAND_SERR);
	dw_pcie_writel_dbi(pci, PCI_COMMAND, val);

	/* Turn off DUT error message */
	dw_pcie_writel_dbi(pci, DEV_STS_CTRL, 0);

	/* set max payload size to 256 bytes */
	val = dw_pcie_readl_dbi(pci, DEV_STS_CTRL);
	val |= DEV_STS_PAYLOAD_256;
	dw_pcie_writel_dbi(pci, DEV_STS_CTRL, val);
	val = dw_pcie_readl_dbi(pci, DEV_STS_CTRL);

	/* EQ off */
	dw_pcie_writel_dbi(pci, PL_LOGIC_GEN3, PL_LOGIC_GEN3_EQ_OFF);

	/* for dma transfer */
	dw_pcie_writel_dbi(pci, PL_COHERENCY_CTRL3, PL_COHERENCY_DMA_CACHE);

	val = dw_pcie_readl_dbi(pci, DBI_DEV_VENDOR_ID);
	val = val & DBI_VENDOR_ID_MASK;
	dw_pcie_writel_dbi(pci, 0x0, (0xa582)<<16|val);

	/* RC BAR disable */
	val = dw_pcie_readl_dbi(pci, CFG_MSI_CAP);
	val &= ~MULTIPLE_MSG_CAP_MASK;
	val |= MULTIPLE_MSG_CAP_32;
	dev_info(&pdev->dev,
		"[%s] Set to MSI count 32 = 0x%x\n", __func__, val);
	dw_pcie_writel_dbi(pci, CFG_MSI_CAP, val);

	dw_pcie_dbi_ro_wr_dis(pci);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Set link traning"
 * @logic "Enable/disable link training"
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, enable, int, 0~1}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_ep_set_ltssm(struct exynos_ep_pcie *pcie_ep,
		int enable)
{
	struct platform_device *pdev = pcie_ep->pdev;
	u32 val;

	if (enable) {
		dev_info(&pdev->dev, "PCIe EP Enable LTSSM\n");
		val = readl(pcie_ep->elbi_base + PCIE_GEN_CTRL_3);
		val |= PCIE_LTSSM_EN;
		writel(val, pcie_ep->elbi_base + PCIE_GEN_CTRL_3);
	} else {
		dev_info(&pdev->dev, "PCIe EP Disable LTSSM\n");
		val = readl(pcie_ep->elbi_base + PCIE_GEN_CTRL_3);
		val &= ~PCIE_LTSSM_EN;
		writel(val, pcie_ep->elbi_base + PCIE_GEN_CTRL_3);
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Setup interrupt"
 * @logic "Clear pending bit\n
 *	Enable interrupt."
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_ep_setup_interrupt(struct exynos_ep_pcie *pcie_ep,
					       bool enable)
{
	u32 val_intad = 0x0;
	u32 val_irq = 0x0;
	int offset = (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) ?
			EVT2_PCIE_MISC_INT_EN : EVT01_PCIE_MISC_INT_EN;

	if (enable) {
		val_intad = PCIE_EP_INTADMASK;
		val_irq = PCIE_EP_IRQMASK;
	}

	writel(val_intad, pcie_ep->elbi_base + PCIE_INT_STS);
	writel(val_irq, pcie_ep->elbi_base + offset);
	writel(val_irq, pcie_ep->elbi_base + PCIE_RX_MSG_INT_EN);

	if (enable)
		dev_info(NULL, "[%s]: Interrupt Enabled\n", __func__);
	else
		dev_info(NULL, "[%s]: Interrupt Disabled\n", __func__);

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Set inbound for doorbell"
 * @logic "Call pci_epc_set_bar to set inbound for doorbell"
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, bar_no, int, 0~7}
 * @endparam
 * @retval{ret, int, 0, <=0, not 0}
 */
static int exynos_v920_pcie_ep_set_bar_inbound(struct exynos_ep_pcie *pcie_ep,
					int bar_no, int pf_no, int vf_no)
{
	struct dw_pcie *pci = pcie_ep->pci;
	struct device *dev = &pcie_ep->pdev->dev;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;
	struct pci_epf_bar *epf_bar;
	int ret = 0;

	if (bar_no < 0 || bar_no > BAR_5) {
		dev_err(dev, "Invalid bar_no = %d\n", bar_no);
		return -EINVAL;
	}
	epf_bar = &pcie_ep->bar[bar_no].epf_bar;
	ret = pci_epc_set_bar(epc, 0, 0, epf_bar);
	dev_info(dev,
		"%s BAR%d PA:0x%llx, VA:0x%p, SIZE:0x%lx FLAGS:0x%x ret:%d\n",
		__func__, epf_bar->barno, epf_bar->phys_addr, epf_bar->addr,
		epf_bar->size, epf_bar->flags, ret);
	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Make EP driver ready for link"
 * @logic "Enable perst ineterrupt\n
 *	Phy power on\n
 *	Qch-sel disable."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_ep_ready_establish_link(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;

	if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) {
		if (pcie_ep->phy_ops.phy_all_pwrdn_clear != NULL)
			pcie_ep->phy_ops.phy_all_pwrdn_clear(
				pcie_ep->ep_phyinfo);

		pcie_ep->linkup_ready = true;

		/* INT enable getting the PREST irq from RC : SFR setting */
		exynos_v920_pcie_ep_setup_interrupt(pcie_ep, true);

		dev_info(&pdev->dev,
			"[%s] Ready to establish link\n", __func__);
	} else {
		if (gpio_get_value(pcie_ep->perst_gpio) ||
			(readl(pcie_ep->sysreg_base + pcie_ep->sysreg_offset) &
				HSI0_PCIE_IP_CTRL_PERST_MASK)
			>> 4 == 0x1) {
			if (pcie_ep->phy_ops.phy_all_pwrdn_clear != NULL)
				pcie_ep->phy_ops.phy_all_pwrdn_clear(
						pcie_ep->ep_phyinfo);

			pcie_ep->linkup_ready = true;

			/* INT EN PREST irq from RC : SFR setting */
			exynos_v920_pcie_ep_setup_interrupt(pcie_ep, true);

			dev_info(&pdev->dev, "[%s] Ready to establish link\n",
					__func__);
		} else
			dev_info(&pdev->dev,
				"[%s] Ready to establish link fail, PERST not high\n",
				__func__);
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Check PCIe link status"
 * @logic "Read and print link status"
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, prests, unsigned int, 0x0~0x1F}
 * @endparam
 * @retval{linksts, int, 0, 0~255, <0}
 */
static unsigned int exynos_v920_pcie_ep_check_linksts(
	struct exynos_ep_pcie *pcie_ep, unsigned int prests)
{
	struct platform_device *pdev = pcie_ep->pdev;
	unsigned int linksts =
		readl(pcie_ep->elbi_base + PCIE_LINK_DBG_2) &
			PCIE_LINK_LTSSM_MASK;

	if (linksts != prests)
		dev_info(&pdev->dev, "%s: (0x%x)\n", __func__, linksts);

	return linksts;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Set SR-IOV"
 * @logic "Set SR-IOV "
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_ep_sriov_setting(struct exynos_ep_pcie *pcie_ep)
{
	struct dw_pcie *pci = pcie_ep->pci;
	u16 val;

	dw_pcie_dbi_ro_wr_en(pci);
	dw_pcie_writew_dbi(pci, PCIE_SRIOV_CAP_POS + PCI_SRIOV_VF_OFFSET,
			VF_OFFSET);
	dw_pcie_writew_dbi(pci, PCIE_SRIOV_CAP_POS + PCI_SRIOV_VF_STRIDE,
			VF_STRIDE);
	dw_pcie_writew_dbi(pci, PCIE_SRIOV_CAP_POS + PCI_SRIOV_INITIAL_VF,
			INITIAL_VF);
	dw_pcie_writew_dbi(pci, PCIE_SRIOV_CAP_POS + PCI_SRIOV_TOTAL_VF,
			TOTAL_VF);
	val = dw_pcie_readw_dbi(pci, PCI_DEVICE_ID);
	dw_pcie_writew_dbi(pci, PCIE_SRIOV_CAP_POS + PCI_SRIOV_VF_DID,
			val + VF_DID_INTERVAL);
	dw_pcie_dbi_ro_wr_dis(pci);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Set PCIe BAR"
 * @logic "Set BAR information\n
 *	Set inbound ATU for BAR."
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_ep_bar_setting(struct exynos_ep_pcie *pcie_ep)
{
	struct dw_pcie *pci = pcie_ep->pci;
	struct device *dev = &pcie_ep->pdev->dev;
	struct pci_epf_bar *epf_bar;
	u32 i, reg, reg_sriov, reg1, reg_sriov1;
	size_t size;
	bool is_setup = false;
	int ret = 0;
	struct iommu_domain *domain;
	struct exynos_ep_ext_dd_ops *ext_ops = NULL;
	struct exynos_ep_ext_info *ext = NULL;
	u16 vf_num = 0;


	dev_info(pci->dev, "[%s] EP BAR setting Start.\n", __func__);

	for (i = 0 ; i < MAX_VF_NUM + 1 ; i++) {
		ext = pcie_ep->ext_node[i].ext;
		ext_ops = pcie_ep->ext_node[i].ext_ops;

		if (ext_ops && ext_ops->set_bar_mem) {
			ret = ext_ops->set_bar_mem(ext, dev,
						   pcie_ep->use_sysmmu);

			if (ret)
				dev_err(dev, "%s VF%d is unavail\n",
					__func__, i);
		}
	}

	dw_pcie_dbi_ro_wr_en(pci);
	/* to adjust PF, VF BAR0 size */
	dw_pcie_writel_dbi(pci, PCI_CLASS_REVISION,
					CLASS_ID<<0x8 | REVISION_ID);

	ext = pcie_ep->ext_node[vf_num].ext;
	ext_ops = pcie_ep->ext_node[vf_num].ext_ops;

	for (i = BAR_0 ; i <= BAR_5 ; i++) {
		is_setup = true;
		reg = PCIE_SHADOW_REG_POS + PCI_BASE_ADDRESS_0 + (i*4);
		reg1 = PCI_BASE_ADDRESS_0 + (i*4);
		reg_sriov =
			PCIE_SHADOW_REG_POS + PCIE_SRIOV_CAP_POS +
			PCI_SRIOV_BAR + (i*4);
		reg_sriov1 = PCIE_SRIOV_CAP_POS + PCI_SRIOV_BAR + (i*4);
		epf_bar = &pcie_ep->bar[i].epf_bar;

		epf_bar->size = pcie_ep->bar[i].size;
		epf_bar->barno = i;
		epf_bar->flags = 0x0;

		if (epf_bar->size > 0)
			size = epf_bar->size - 1;
		else
			size = epf_bar->size;

		switch (pcie_ep->bar[i].type) {
		case BAR_TYPE_NULL:
			epf_bar->addr = NULL;
			epf_bar->phys_addr = 0x0;
			break;
		case BAR_TYPE_EXT_64:
			is_setup = false;
			break;
		case BAR_TYPE_DOORBELL:
			epf_bar->addr = NULL;
			switch (pcie_ep->ch_num) {
			case CH0_4L:
				epf_bar->phys_addr =
					PCIE_0_DOORBELL_BASE_ADDRESS;
				break;
			case CH1_4L:
				epf_bar->phys_addr =
					PCIE_1_DOORBELL_BASE_ADDRESS;
				break;
			case CH2_2L:
				epf_bar->phys_addr =
					PCIE_2_DOORBELL_BASE_ADDRESS;
				break;
			case CH3_2L:
				epf_bar->phys_addr =
					PCIE_3_DOORBELL_BASE_ADDRESS;
				break;
			default:
				dev_err(dev, "%s: CH%d is invalid\n",
					__func__, pcie_ep->ch_num);
				is_setup = false;
				break;
			}

			if (!is_setup)
				continue;

			if (pcie_ep->use_sysmmu &&
				(pcie_ep->bar[i].is_mapped == false)) {
				dev_info(dev,
					"%s: sysmmu map BAR CH%d DB sz:0x%lx\n",
					__func__, pcie_ep->ch_num,
					epf_bar->size);
				domain = iommu_get_domain_for_dev(dev);

				if (domain == NULL) {
					dev_err(dev, "%s: domain is NULL\n", __func__);
					break;
				}
				ret = iommu_map(domain, epf_bar->phys_addr, epf_bar->phys_addr,
					epf_bar->size, IOMMU_READ | IOMMU_WRITE);
				if (ret) {
					dev_err(dev, "%s: iommu_map failed\n", __func__);
					break;
				}
				pcie_ep->bar[i].is_mapped = true;
			} else {
				dev_info(dev,
					"%s: DB use_sysmmu(%d) is_mapped(%d)\n",
					__func__, pcie_ep->use_sysmmu,
					pcie_ep->bar[i].is_mapped);
			}
			break;
		case BAR_TYPE_MEMORY_ALLOC:
		case BAR_TYPE_MEMORY_RESERVED:
		case BAR_TYPE_MEMORY_SFR:
			/* Need to add ops to get memory address data */
			if (ext_ops && ext_ops->get_bar_mem) {
				ext_ops->get_bar_mem(ext,
						     epf_bar->barno,
						     &epf_bar->phys_addr,
						     epf_bar->addr,
						     pcie_ep->bar[i].type,
						     pcie_ep->bar[i].size);
			} else {
				epf_bar->size = 0;
				size = 0;
			}
			break;
		case BAR_TYPE_MSIX:
			epf_bar->addr = NULL;
			epf_bar->phys_addr = MSIX_BAR5_BASE_ADDRESS;
			break;
		default:
			is_setup = false;
			break;
		}

		dev_info(dev, "%s BAR%d VA:0x%p PA:0x%llx\n",
			 __func__, epf_bar->barno, epf_bar->addr,
			 epf_bar->phys_addr);

		if (is_setup) {
			if (pcie_ep->bar[i].mem_type == BAR_ADDRESS_64) {
				epf_bar->flags =
					PCI_BASE_ADDRESS_SPACE_MEMORY |
					PCI_BASE_ADDRESS_MEM_TYPE_64;
				dw_pcie_writel_dbi(pci, reg + 4,
					upper_32_bits(size));
				dw_pcie_writel_dbi(pci, reg1 + 4, 0);
				dw_pcie_writel_dbi(pci, reg_sriov + 4,
					upper_32_bits(size));
				dw_pcie_writel_dbi(pci, reg_sriov1 + 4, 0);
			} else {
				epf_bar->flags =
					PCI_BASE_ADDRESS_SPACE_MEMORY |
					PCI_BASE_ADDRESS_MEM_TYPE_32;
			}
			dw_pcie_writel_dbi(pci, reg, lower_32_bits(size));
			dw_pcie_writel_dbi(pci, reg1, epf_bar->flags);
			dw_pcie_writel_dbi(pci, reg_sriov, lower_32_bits(size));
			dw_pcie_writel_dbi(pci, reg_sriov1, epf_bar->flags);

			if (epf_bar->size > 0)
				exynos_v920_pcie_ep_set_bar_inbound(pcie_ep, i,
					0, 0);
		}
	}

	dw_pcie_writel_dbi(pci, PCIE_SHADOW_REG_POS + PCI_ROM_ADDRESS, 0);
	dw_pcie_dbi_ro_wr_dis(pci);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Initialize doorbell inerrupt"
 * @logic "Clear doorbell flag; enable doorbell"
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_ep_doorbell_init(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	int i;

	dev_dbg(&pdev->dev, "Initialize Doorbell IRQ\n");
	for (i = 0; i < MAX_VF_NUM; i++) {
		writel(0xffffffff, pcie_ep->doorbell_base +
			PCIE_DOORBELL0_CLEAR + i * PCIE_DOORBELL_OFFSET);
		writel(0xffffffff, pcie_ep->doorbell_base +
			PCIE_DOORBELL0_ENABLE + i * PCIE_DOORBELL_OFFSET);
	}
}

/* PCIe lane status checking function */
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Check lane status of PCIe ch"
 * @logic "Print lane status of PCIe ch"
 * @params
 * @param{in, ch_num, int, 0~5}
 * @endparam
 * @retval{-, int, 0, 0/1, not 0/1}
 */
int exynos_v920_chk_ep_lane_status(int ch_num)
{
	struct exynos_ep_pcie *pcie_ep = &g_pcie_ep_v920[ch_num];

	pr_info("[%s] PCIe ch%d lane status =%d\n", __func__, ch_num,
		pcie_ep->lane_status);

	return pcie_ep->lane_status;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Update lane status in bifurcation"
 * @logic "Update lane status in bifurcation"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0/1, not 0/1}
 * @noret
 */
static void exynos_v920_update_ep_lane_status(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int other_lane_status;

	if (pcie_ep->ch_num % CH_SHARE_PHY == 0) {
		/* Current Link A */

		/* Current Link Status Update */
		pcie_ep->ep_phyinfo->linkA_status =
			LANE_STAT_MAX * (1 - pcie_ep->lane_status);

		other_lane_status = exynos_v920_chk_ep_lane_status
							(pcie_ep->ch_num + 1);

		if (other_lane_status == STATE_LANE_INIT) {
			/* Other link is RC */
			pcie_ep->ep_phyinfo->linkB_status =
					readl(pcie_ep->elbi_base_other) &
						PCIE_LINK_LTSSM_MASK;
		} else {
			/* Other link is EP */
			/* Other Link Status Update */
			pcie_ep->ep_phyinfo->linkB_status =
						LANE_STAT_MAX *
						(1 - other_lane_status);
		}
	} else {
		/* Current Link B */

		other_lane_status =	exynos_v920_chk_ep_lane_status
							(pcie_ep->ch_num - 1);

		if (other_lane_status == STATE_LANE_INIT) {
			/* Other link is RC */
			pcie_ep->ep_phyinfo->linkA_status =
					readl(pcie_ep->elbi_base_other) &
						PCIE_LINK_LTSSM_MASK;
		} else {
			/* Other link is EP */
			/* Other Link Status Update */
			pcie_ep->ep_phyinfo->linkA_status =
						LANE_STAT_MAX *
						(1 - other_lane_status);
		}

		/* Current Link Status Update */
		pcie_ep->ep_phyinfo->linkB_status =
					LANE_STAT_MAX *
					(1 - pcie_ep->lane_status);
	}
	dev_info(dev, "#### linkA:%d linkB:%d\n",
		pcie_ep->ep_phyinfo->linkA_status,
		pcie_ep->ep_phyinfo->linkB_status);

}


/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Link up PCIe link"
 * @logic "Check link in ready\n
 *	Initialize phy\n
 *	Setup EP\n
 *	Setup interrupt\n
 *	Doorbell initialize\n
 *	Enable LTSSM\n
 *	Check link status."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{linksts, int, 0, 0~0x91, not 0~0x91}
 */
static unsigned int exynos_v920_pcie_ep_link_start(
	struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	unsigned int linksts = 0;
	int cnt = EP_LINKUP_WAIT;
	int ret;
	u32 val;

start_linkup:
	if (pcie_ep->linkup_ready == true) {
		/* 1. phy initialize */
		ret = pcie_ep->phy_ops.phy_config(pcie_ep->ep_phyinfo);
		if (ret < 0) {
			dev_info(&pdev->dev,
			"PCIe EP didn't ready to start establish link\n");
			exynos_v920_ep_ready_establish_link(pcie_ep);
			return linksts;
		}

		/* 2. setup EP */
		exynos_v920_pcie_setup_ep(pcie_ep);
		exynos_v920_pcie_ep_bar_setting(pcie_ep);
		exynos_v920_pcie_ep_sriov_setting(pcie_ep);
		/* 3. setup interrupt */
		exynos_v920_pcie_ep_setup_interrupt(pcie_ep, true);
		/* 4. Doorbel irq init */
		exynos_v920_pcie_ep_doorbell_init(pcie_ep);
		/* 6. pcie_setiATU */
		/* 7. enable LTSSM */
		exynos_v920_pcie_ep_set_ltssm(pcie_ep, LTSSM_EN);

		while (cnt--) {
			linksts = exynos_v920_pcie_ep_check_linksts(pcie_ep,
								linksts);
			udelay(10);
			if ((linksts & PCIE_LINK_LTSSM_MASK) == S_L0)
				break;
		}

		if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT01) {
			if ((readl(pcie_ep->sysreg_base +
				pcie_ep->sysreg_offset)
				& HSI0_PCIE_IP_CTRL_PERST_MASK) >> 4 == 0x1) {
				val = readl(pcie_ep->sysreg_base +
					pcie_ep->sysreg_offset);
				val = val | HSI0_PCIE_IP_CTRL_PERST_IN;
				writel(val, (pcie_ep->sysreg_base +
					pcie_ep->sysreg_offset));
			}
		}
	} else {
		dev_info(&pdev->dev,
			"PCIe EP didn't ready to start establish link\n");
		exynos_v920_ep_ready_establish_link(pcie_ep);
		goto start_linkup;
	}

	return linksts;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Start PCIe link power down"
 * @logic "Change ep state to STATE_LINK_DOWN_TRY\n
 *	Phy all power down\n
 *	Qch-sel disable."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_ep_power_down(struct exynos_ep_pcie *pcie_ep)
{
	pcie_ep->state = STATE_LINK_DOWN_TRY;

	/* phy all power down */
	if (pcie_ep->phy_ops.phy_all_pwrdn != NULL)
		pcie_ep->phy_ops.phy_all_pwrdn(pcie_ep->ep_phyinfo);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Stop PCIe link"
 * @logic "Change ep state to STATE_LINK_DOWN_TRY\n
 *	If using sris, chage perst_in_mux setting\n
 *	Disable LTSSM\n
 *	Call exynos_v920_pcie_ep_power_down."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_ep_link_stop(struct exynos_ep_pcie *pcie_ep)
{
	pcie_ep->state = STATE_LINK_DOWN_TRY;

	/* LTSSM disable */
	exynos_v920_pcie_ep_set_ltssm(pcie_ep, LTSSM_DIS);

	pcie_ep->linkup_ready = false;

	exynos_v920_pcie_ep_power_down(pcie_ep);

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "force clear BAR"
 * @logic "if cannot access DBI, Clear EP BAR"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_ep_clear_bar_force(struct exynos_ep_pcie *pcie_ep)
{
	struct dw_pcie *pci = pcie_ep->pci;
	struct device *dev = pci->dev;
	struct dw_pcie_ep *ep = &pci->ep;
	u32 atu_index;
	int i;
	struct iommu_domain *domain;
	struct pci_epf_bar *epf_bar;

	for (i = BAR_0 ; i <= BAR_5 ; i++) {
		epf_bar = &pcie_ep->bar[i].epf_bar;
		if (epf_bar->size > 0 && epf_bar->barno >= 0 &&  epf_bar->barno <= BAR_5) {
			atu_index = ep->bar_to_atu[epf_bar->barno];

			dev_info(dev, "[%s] atu_idx:%d BAR%d\n", __func__,
				atu_index, epf_bar->barno);

			if (pcie_ep->use_sysmmu && pcie_ep->bar[i].is_mapped) {
				if (pcie_ep->bar[i].type == BAR_TYPE_DOORBELL) {
					dev_info(dev, "%s SYSMMU unmap BAR CH%d DB sz:0x%lx\n",
						__func__, pcie_ep->ch_num, epf_bar->size);
					domain = iommu_get_domain_for_dev(dev);
					if (domain) {
						iommu_unmap(domain, epf_bar->phys_addr,
								    epf_bar->size);
						pcie_ep->bar[i].is_mapped = false;
					}
				}
			}

			clear_bit(atu_index, ep->ib_window_map);
			pcie_ep->bar[i].epf_bar.size = 0;
		}
	}

	return 0;
}


/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle irq0 event"
 * @logic "In case of PM_TURNOFF, start link stop"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, value, u32, 0~0xffffffff}
 * @param{in, mask, u32, 0~0xffffffff}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_irq0_evt(struct exynos_ep_pcie *pcie_ep,
						u32 value, u32 mask)
{
	struct platform_device *pdev = pcie_ep->pdev;
	u32 val = 0x0;

	if ((value & mask) != mask)
		return;
	switch (mask) {
	case PM_TURNOFF:
		dev_info(&pdev->dev, "IRQ0: PM_TURNOFF\n");
		dev_info(&pdev->dev, "Check Link status\n");
		while (1) {
			val = readl(pcie_ep->elbi_base + PCIE_LINK_DBG_2);
			val &= PCIE_LINK_LTSSM_MASK;
			dev_info(&pdev->dev, "Link sts: 0x%x\n", val);
			if (val == S_L2_IDLE) {
				exynos_v920_pcie_ep_link_stop(pcie_ep);
				exynos_v920_ep_ready_establish_link(pcie_ep);
				break;
			}
		}
		break;
	default:
		dev_dbg(&pdev->dev, "IRQ0: unknown(0x%x)\n", mask);
			break;
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle irq1 event"
 * @logic "If value is EVT0/1: EVT01_PERST_N_1 or EVT2: EVT2_PERST_N_1\n
 *	start link up process. If value is linkdown,\n
 *	start link stop process or handle exception."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, value, u32, 0~0xffffffff}
 * @param{in, mask, u32, 0~0xffffffff}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_irq1_evt(struct exynos_ep_pcie *pcie_ep,
						u32 value, u32 mask)
{
	struct platform_device *pdev = pcie_ep->pdev;
	int perst_n_0, perst_n_1, link_down, pm_in_l2_rise;

	if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) {
		perst_n_0 = EVT2_PERST_N_0;
		perst_n_1 = EVT2_PERST_N_1;
		link_down = EVT2_LINK_DOWN;
		pm_in_l2_rise = EVT2_PM_IN_L2_RISE;
	} else {
		perst_n_0 = EVT01_PERST_N_0;
		perst_n_1 = EVT01_PERST_N_1;
		link_down = EVT01_LINK_DOWN;
		pm_in_l2_rise = EVT01_PM_IN_L2_RISE;
	}

	if (mask == perst_n_0)
		dev_info(&pdev->dev, "IRQ1: PERST_N_0\n");
	else if (mask == perst_n_1) {
		/* if PERST event is occurred, */
		/* PCIe EP starts several sequence to establish link */
		dev_info(&pdev->dev, "IRQ1: PERST_N_1\n");
		if (pcie_ep->state == STATE_LINK_DOWN) {
			pcie_ep->state = STATE_LINK_UP_TRY;
			queue_work(pcie_ep->pcie_ep_wq,
				&pcie_ep->irq1_evt_work.work);
		}
	} else if (mask == link_down) {
		dev_info(&pdev->dev, "IRQ1: LINK_DOWN !!! state(%d)\n",
			pcie_ep->state);

		if (pcie_ep->state == STATE_LINK_UP ||
				pcie_ep->state == STATE_LINK_UP_TRY) {

			exynos_v920_pcie_ep_clear_bar_force(pcie_ep);

			complete_all(&pcie_ep->wdma0);
			complete_all(&pcie_ep->rdma0);

			reinit_completion(&pcie_ep->wdma0);
			reinit_completion(&pcie_ep->rdma0);

			complete_all(&pcie_ep->wdma1);
			complete_all(&pcie_ep->wdma2);
			complete_all(&pcie_ep->wdma3);

			complete_all(&pcie_ep->rdma1);
			complete_all(&pcie_ep->rdma2);
			complete_all(&pcie_ep->rdma3);

			reinit_completion(&pcie_ep->wdma1);
			reinit_completion(&pcie_ep->wdma2);
			reinit_completion(&pcie_ep->wdma3);

			reinit_completion(&pcie_ep->rdma1);
			reinit_completion(&pcie_ep->rdma2);
			reinit_completion(&pcie_ep->rdma3);

			pcie_ep->state = STATE_LINK_DOWN_ABNORMAL;
			exynos_v920_pcie_ep_send_evt(pcie_ep,
						STATE_LINK_DOWN_ABNORMAL);
			dev_info(&pdev->dev,
				"IRQ1: STATE_LINK_DOWN_ABNORMAL\n");

			/* lane status off */
			pcie_ep->lane_status = STATE_LANE_DOWN;

			/* Register Link Reset Work Queue */
			pcie_ep->work_exception = WORK_LINK_RESET;
			queue_work(pcie_ep->pcie_ep_wq_exception,
				&pcie_ep->exception_work.work);
		} else {
			if (pcie_ep->state == STATE_LINK_DOWN_TRY)
				exynos_v920_pcie_ep_clear_bar_force(pcie_ep);

			pcie_ep->state = STATE_LINK_DOWN;
			exynos_v920_pcie_ep_send_evt(pcie_ep, STATE_LINK_DOWN);
		}
	} else if (mask == pm_in_l2_rise) {
		dev_info(&pdev->dev, "IRQ1: PM_LINKST_IN_L2\n");

		/* lane status off */
		pcie_ep->lane_status = STATE_LANE_DOWN;

		if (pcie_ep->use_bifurcation)
			exynos_v920_update_ep_lane_status(pcie_ep);

		exynos_v920_pcie_ep_link_stop(pcie_ep);
	} else
		dev_dbg(&pdev->dev, "IRQ1: unknown(0x%x)\n", mask);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle irq3 event"
 * @logic "Check which dma(write ch0 or read ch0) is completed\n
 *	Make completion for dma."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, value, u32, 0~0xffffffff}
 * @param{in, mask, u32, 0~0xffffffff}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_irq3_evt(struct exynos_ep_pcie *pcie_ep,
						u32 value, u32 mask)
{
	struct platform_device *pdev = pcie_ep->pdev;

	if ((value & mask) != mask)
		return;
	switch (mask) {
	case WR_CH0:
		dev_dbg(&pdev->dev, "IRQ3: Write DMA ch0 DONE!\n");
		complete(&pcie_ep->wdma0);
		break;
	case RD_CH0:
		dev_dbg(&pdev->dev, "IRQ3: Read DMA ch0 DONE!\n");
		complete(&pcie_ep->rdma0);
		break;
	default:
		dev_dbg(&pdev->dev, "IRQ3: unknown(0x%x)\n", mask);
		break;
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle doorbell event"
 * @logic "Start test work queue with WORK_DOORBELL command"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_doorbell_evt(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;

	dev_dbg(&pdev->dev, "DOORBELL: 0x%x\n", pcie_ep->doorbell);

	/* Received Test Read Request */
	pcie_ep->size = pcie_ep->doorbell;

	if (pcie_ep->size > MAX_TEST_WR_SIZE) {
		dev_info(&pdev->dev, "Maximum read size exceeded, fix to %x\n",
							MAX_TEST_WR_SIZE);
		pcie_ep->size = MAX_TEST_WR_SIZE;
	}

	pcie_ep->work_cmd = WORK_DOORBELL;
	queue_work(pcie_ep->pcie_ep_wq, &pcie_ep->test_work.work);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handler for PCIe interrupt1"
 * @logic "Start link up; check the link up result\n
 *	If result is success, change state STATE_LINK_UP and lane status \n
 *	If using sris, chage perst_in_mux setting."
 * @params
 * @param{in/out, work, struct ::work_struct *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_irq1_handler(struct work_struct *work)
{
	struct exynos_ep_pcie *pcie_ep =
		container_of(work, struct exynos_ep_pcie, irq1_evt_work.work);
	struct platform_device *pdev = pcie_ep->pdev;
	u32 irq1_status = pcie_ep->irq1_status;
	unsigned int ret;
	unsigned int other_ch = 0;
	int cnt = 2 * EP_LINKUP_WAIT;

	dev_dbg(&pdev->dev, "PCIe EP IRQ1 : 0x%x\n", irq1_status);

	if (pcie_ep->ch_num == CH0_4L)
		other_ch = CH1_4L;
	else if (pcie_ep->ch_num == CH1_4L)
		other_ch = CH0_4L;
	else if (pcie_ep->ch_num == CH2_2L)
		other_ch = CH3_2L;
	else if (pcie_ep->ch_num == CH3_2L)
		other_ch = CH2_2L;

	if (pcie_ep->use_bifurcation) {
		while (cnt--) {
			if (g_pcie_ep_v920[other_ch].state == STATE_LINK_UP ||
				g_pcie_ep_v920[other_ch].state ==
				STATE_LINK_DOWN)
				break;
			udelay(10);
		}
	}

	ret = exynos_v920_pcie_ep_link_start(pcie_ep);
	dev_info(&pdev->dev, "ret: 0x%x\n", ret);
	if ((ret & PCIE_LINK_LTSSM_MASK) == S_L0) {
		dev_info(&pdev->dev, "IRQ1: LINK UP SUCCESS\n");
		pcie_ep->state = STATE_LINK_UP;

		exynos_v920_pcie_ep_send_evt(pcie_ep, STATE_LINK_UP);

		pcie_ep->lane_status = STATE_LANE_UP;
	} else {
		/* link up fail case */
		pcie_ep->state = STATE_LINK_DOWN;
		exynos_v920_pcie_ep_send_evt(pcie_ep, STATE_LINK_DOWN);
		dev_info(&pdev->dev, "IRQ1: LINK UP FAIL\n");
		exynos_v920_pcie_ep_clear_bar_force(pcie_ep);
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handler for PCIe interrupt"
 * @logic "If ep state is STATE_LINK_DOWN, power on all phy\n
 *	Read irq0~4 status and clear interrupt\n
 *	If irq1 status is intended value, call irq1 event\n
 *	If irq2 status is PM_LINKST_IN_L2, start link stop\n
 *	If irq3 status is intended value, call irq3 event\n
 *	If doorbell interrupt occurred, call doorbell event."
 * @params
 * @param{in, irq, int, >0}
 * @param{in/out, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED, not IRQ_HANDLED}
 */
static irqreturn_t exynos_v920_pcie_ep_irq_handler(int irq, void *arg)
{
	struct exynos_ep_pcie *pcie_ep = arg;
	struct platform_device *pdev = pcie_ep->pdev;
	void __iomem	*elbi_base = pcie_ep->elbi_base;
	int loop;
	int offset = (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) ?
			EVT2_PCIE_MISC_INT_STS : EVT01_PCIE_MISC_INT_STS;
	u32 mask;

	if ((pcie_ep->state == STATE_LINK_DOWN) &&
		(pcie_ep->phy_ops.phy_all_pwrdn_clear != NULL))
		pcie_ep->phy_ops.phy_all_pwrdn_clear(pcie_ep->ep_phyinfo);

	/* read the status of each IRQ and do clear first */
	/* handle RX_MSG interrupt (PM Turn off) */
	pcie_ep->irq0_status = readl(elbi_base + PCIE_RX_MSG_INT_STS);
	writel(pcie_ep->irq0_status, elbi_base + PCIE_RX_MSG_INT_STS);

	/* handle MISC interrupt (PERST int) */

	pcie_ep->irq1_status = readl(elbi_base + offset);
	writel(pcie_ep->irq1_status, elbi_base + offset);

	if (pcie_ep->irq0_status & PM_TURNOFF)
		dev_info(&pdev->dev, "IRQ0: PM_TURNOFF\n");

	if (pcie_ep->irq1_status & PCIE_EP_IRQMASK) {
		dev_dbg(&pdev->dev, "[%s] irq1_status : 0x%x\n",
			__func__, pcie_ep->irq1_status);
		for (loop = 0; loop < 32; loop++) {
			mask = (1 << loop);
			if ((pcie_ep->irq1_status & mask) == mask) {
				exynos_v920_pcie_ep_irq1_evt(pcie_ep,
						pcie_ep->irq1_status, mask);
			}
		}
	}

	return IRQ_HANDLED;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle doorbell interrupt"
 * @logic "Find doorbell number from irq\n
 *	Read doorbell interrupt status and clear\n
 *	Call doorbell event."
 * @params
 * @param{in, irq, int, >0}
 * @param{in/out, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED/IRQ_NONE, IRQ_NONE}
 */
static irqreturn_t exynos_v920_pcie_ep_doorbell_irq_handler(int irq, void *arg)
{
	struct exynos_ep_pcie *pcie_ep = arg;
	void __iomem	*doorbell_base = pcie_ep->doorbell_base;

	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct exynos_ep_ext_info *ext = NULL;
	struct exynos_ep_ext_dd_ops *ext_ops = NULL;

	int i, db_no = -1;
	int ret = 0;
	u32 db_data = 0;
	u16 vf_num = 0;

	for (i = 0; i < MAX_VF_NUM; i++) {
		if (irq == pcie_ep->db_irq[i]) {
			db_no = i;
			break;
		}
	}

	if (db_no == -1) {
		dev_err(dev, "Invalid Doorbell IRQ: %d\n", irq);
		return IRQ_NONE;
	}

	db_data = readl(doorbell_base + PCIE_DOORBELL0_STATUS +
		db_no * PCIE_DOORBELL_OFFSET);
	writel(0xffffffff, doorbell_base + PCIE_DOORBELL0_CLEAR +
		db_no * PCIE_DOORBELL_OFFSET);

	if (db_data & PCIE_EP_IRQMASK) {
		ext = pcie_ep->ext_node[vf_num].ext;
		ext_ops = pcie_ep->ext_node[vf_num].ext_ops;

		if (ext_ops != NULL) {
			if (ext_ops->rvc_doorbell != NULL) {
				ret = ext_ops->rvc_doorbell(ext, db_data);
				if (ret)
					dev_err(dev,
						"%s rvc_doorbell error %d\n",
						__func__, ret);
			} else
				dev_err(dev, "%s no ops of ext node\n",
					__func__);
		} else {
			pcie_ep->doorbell = db_data;
			exynos_v920_pcie_ep_doorbell_evt(pcie_ep);
		}
	}

	return IRQ_HANDLED;
}

static void exynos_v920_pcie_ep_dma_resolve(struct exynos_ep_pcie *pcie_ep,
					    int dma_dir, int result)
{
	struct exynos_ep_ext_info *ext = NULL;
	struct exynos_ep_ext_dd_ops *ext_ops = NULL;
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int ll_start = 0;
	int ll_cnt = 0;
	u16 vf_num = 0;

	if (dma_dir == CHDRV_DMA_WR) {
		ll_start = pcie_ep->wr_ll_start;
		ll_cnt = pcie_ep->wr_ll_cnt;
	} else if (dma_dir == CHDRV_DMA_RD) {
		ll_start = pcie_ep->rd_ll_start;
		ll_cnt = pcie_ep->rd_ll_cnt;
	}

	if (pcie_ep->state == STATE_LINK_DOWN ||
	    pcie_ep->state == STATE_LINK_DOWN_ABNORMAL) {
		dev_err(dev, "%s: DMA work is closed by force\n", __func__);
		return;
	}

#if	DMA_LOG_PRINT_EP
	if (dma_dir == CHDRV_DMA_WR)
		dev_info(dev, "WR TEST SUCCESS\n");
	else if (dma_dir == CHDRV_DMA_RD)
		dev_info(dev, "RD TEST SUCCESS\n");
#endif

	/* call ops for more dma requests */
	ext = pcie_ep->ext_node[vf_num].ext;
	ext_ops = pcie_ep->ext_node[vf_num].ext_ops;

	if (ext_ops && ext_ops->dma_resolve)
		ext_ops->dma_resolve(ext, dma_dir, ll_start, ll_cnt, result);
	else
		dev_err(dev, "%s: no ops of ext node\n", __func__);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle dma0 interrupt"
 * @logic "Read and clear write dma0 status\n
 *	Check write is done or aborted and make completion\n
 *	Read and clear read dma0 status\n
 *	Check read is done or aborted and make completion."
 * @params
 * @param{in, irq, int, >0}
 * @param{in/out, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED/IRQ_NONE, IRQ_NONE}
 */
static irqreturn_t exynos_v920_pcie_ep_dma0_irq_handler(int irq, void *arg)
{
	struct exynos_ep_pcie *pcie_ep = arg;
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	u32 val;
	u32 mask;
	int dma_offset;

	dma_offset = 0;

	val = readl(pcie_ep->dma_base + PCIE_DMA_WR_INT_STS) &
		PCIE_DMA_STS_IRQ0_MASK;
	writel(val, pcie_ep->dma_base + PCIE_DMA_WR_INT_CLEAR);

	if (val != 0) {
		/* WR done Check */
		mask = (PCIE_DMA_STS_2CH_MASK << dma_offset);
		if (val & mask) {
			complete(&pcie_ep->wdma0);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Write done. DMA0 completion.\n");
#endif
			exynos_v920_pcie_ep_dma_resolve(pcie_ep, CHDRV_DMA_WR,
							CHDRV_ENOERR);

			return IRQ_HANDLED;
		}

		/* WR abort Check */
		mask = (PCIE_DMA_STS_2CH_MASK << (dma_offset + 16));
		if (val & mask) {
			complete(&pcie_ep->wdma0);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Write Aborted. DMA0 completion.\n");
#endif
			exynos_v920_pcie_ep_dma_resolve(pcie_ep, CHDRV_DMA_WR,
							-CHDRV_EDMAABORT);

			return IRQ_HANDLED;
		}

		dev_info(dev, "Can not found write int flag\n");
		return IRQ_NONE;
	}

	val = readl(pcie_ep->dma_base + PCIE_DMA_RD_INT_STS) &
		PCIE_DMA_STS_IRQ0_MASK;
	writel(val, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);

	if (val != 0) {
		/* RD done Check */
		mask = (PCIE_DMA_STS_2CH_MASK << dma_offset);
		if (val & mask) {
			complete(&pcie_ep->rdma0);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Read done. DMA0 completion.\n");
#endif
			exynos_v920_pcie_ep_dma_resolve(pcie_ep, CHDRV_DMA_RD,
							CHDRV_ENOERR);

			return IRQ_HANDLED;
		}

		/* RD abort Check */
		mask = (PCIE_DMA_STS_2CH_MASK << (dma_offset+16));
		if (val & mask) {
			complete(&pcie_ep->rdma0);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Read Aborted. DMA0 completion.\n");
#endif
			exynos_v920_pcie_ep_dma_resolve(pcie_ep, CHDRV_DMA_RD,
							-CHDRV_EDMAABORT);

			return IRQ_HANDLED;
		}
		dev_err(dev, "Can not found read int flag\n");
		return IRQ_NONE;
	}

	return IRQ_HANDLED;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle dma1 interrupt"
 * @logic "Read and clear write dma1 status\n
 *	Check write is done or aborted and make completion\n
 *	Read and clear read dma1 status\n
 *	Check read is done or aborted and make completion."
 * @params
 * @param{in, irq, int, >0}
 * @param{in/out, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED/IRQ_NONE, IRQ_NONE}
 */
static irqreturn_t exynos_v920_pcie_ep_dma1_irq_handler(int irq, void *arg)
{
	struct exynos_ep_pcie *pcie_ep = arg;
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	u32 val;
	u32 mask;
	int dma_offset;

	dma_offset = 2;

	val = readl(pcie_ep->dma_base + PCIE_DMA_WR_INT_STS) &
		PCIE_DMA_STS_IRQ1_MASK;
	writel(val, pcie_ep->dma_base + PCIE_DMA_WR_INT_CLEAR);

	if (val != 0) {
		/* WR done Check */
		mask = (PCIE_DMA_STS_2CH_MASK << dma_offset);
		if (val & mask) {
			complete(&pcie_ep->wdma1);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Write done. DMA1 completion.\n");
#endif
			exynos_v920_pcie_ep_dma_resolve(pcie_ep, CHDRV_DMA_WR,
							CHDRV_ENOERR);

			return IRQ_HANDLED;
		}

		/* WR abort Check */
		mask = (PCIE_DMA_STS_2CH_MASK << (dma_offset + 16));
		if (val & mask) {
			complete(&pcie_ep->wdma1);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Write Aborted. DMA1 completion.\n");
#endif
			exynos_v920_pcie_ep_dma_resolve(pcie_ep, CHDRV_DMA_WR,
							-CHDRV_EDMAABORT);

			return IRQ_HANDLED;
		}

		dev_err(dev, "Can not found write int flag\n");
		return IRQ_NONE;
	}

	val = readl(pcie_ep->dma_base + PCIE_DMA_RD_INT_STS) &
		PCIE_DMA_STS_IRQ1_MASK;
	writel(val, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);

	if (val != 0) {
		/* RD done Check */
		mask = (PCIE_DMA_STS_2CH_MASK << dma_offset);
		if (val & mask) {
			complete(&pcie_ep->rdma1);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Read done. DMA1 completion.\n");
#endif
			exynos_v920_pcie_ep_dma_resolve(pcie_ep, CHDRV_DMA_RD,
							CHDRV_ENOERR);

			return IRQ_HANDLED;
		}

		/* RD abort Check */
		mask = (PCIE_DMA_STS_2CH_MASK << (dma_offset+16));
		if (val & mask) {
			complete(&pcie_ep->rdma1);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Read Aborted. DMA1 completion.\n");
#endif
			exynos_v920_pcie_ep_dma_resolve(pcie_ep, CHDRV_DMA_RD,
							-CHDRV_EDMAABORT);

			return IRQ_HANDLED;
		}
		dev_err(dev, "Can not found read int flag\n");
		return IRQ_NONE;
	}

	return IRQ_HANDLED;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle dma2 interrupt"
 * @logic "Read and clear write dma2 status\n
 *	Check write is done or aborted and make completion\n
 *	Read and clear read dma2 status\n
 *	Check read is done or aborted and make completion."
 * @params
 * @param{in, irq, int, >0}
 * @param{in/out, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED/IRQ_NONE, IRQ_NONE}
 */
static irqreturn_t exynos_v920_pcie_ep_dma2_irq_handler(int irq, void *arg)
{
	struct exynos_ep_pcie *pcie_ep = arg;
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	u32 val;
	u32 mask;
	int dma_offset;

	dma_offset = 4;

	val = readl(pcie_ep->dma_base + PCIE_DMA_WR_INT_STS) &
		PCIE_DMA_STS_IRQ2_MASK;
	writel(val, pcie_ep->dma_base + PCIE_DMA_WR_INT_CLEAR);

	if (val != 0) {
		/* WR done Check */
		mask = (PCIE_DMA_STS_2CH_MASK << dma_offset);
		if (val & mask) {
			complete(&pcie_ep->wdma2);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Write done. DMA2 completion.\n");
#endif

			return IRQ_HANDLED;
		}

		/* WR abort Check */
		mask = (PCIE_DMA_STS_2CH_MASK << (dma_offset + 16));
		if (val & mask) {
			complete(&pcie_ep->wdma2);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Write Aborted. DMA2 completion.\n");
#endif

			return IRQ_HANDLED;
		}
		dev_err(dev, "Can not found write int flag\n");
		return IRQ_NONE;
	}

	val = readl(pcie_ep->dma_base + PCIE_DMA_RD_INT_STS) &
		PCIE_DMA_STS_IRQ2_MASK;
	writel(val, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);

	if (val != 0) {
		/* RD done Check */
		mask = (PCIE_DMA_STS_2CH_MASK << dma_offset);
		if (val & mask) {
			complete(&pcie_ep->rdma2);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Read done. DMA2 completion.\n");
#endif

			return IRQ_HANDLED;
		}

		/* RD abort Check */
		mask = (PCIE_DMA_STS_2CH_MASK << (dma_offset+16));
		if (val & mask) {
			complete(&pcie_ep->rdma2);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Read Aborted. DMA2 completion.\n");
#endif

			return IRQ_HANDLED;
		}
		dev_err(dev, "Can not found read int flag\n");
		return IRQ_NONE;
	}

	return IRQ_HANDLED;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handle dma3 interrupt"
 * @logic "Read and clear write dma3 status\n
 *	Check write is done or aborted and make completion\n
 *	Read and clear read dma3 status\n
 *	Check read is done or aborted and make completion."
 * @params
 * @param{in, irq, int, >0}
 * @param{in/out, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED/IRQ_NONE, IRQ_NONE}
 */
static irqreturn_t exynos_v920_pcie_ep_dma3_irq_handler(int irq, void *arg)
{
	struct exynos_ep_pcie *pcie_ep = arg;
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	u32 val;
	u32 mask;
	int dma_offset;

	dma_offset = 6;

	val = readl(pcie_ep->dma_base + PCIE_DMA_WR_INT_STS) &
		PCIE_DMA_STS_IRQ3_MASK;
	writel(val, pcie_ep->dma_base + PCIE_DMA_WR_INT_CLEAR);

	if (val != 0) {
		/* WR done Check */
		mask = (PCIE_DMA_STS_2CH_MASK << dma_offset);
		if (val & mask) {
			complete(&pcie_ep->wdma3);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Write done. DMA3 completion.\n");
#endif

			return IRQ_HANDLED;
		}

		/* WR abort Check */
		mask = (PCIE_DMA_STS_2CH_MASK << (dma_offset + 16));
		if (val & mask) {
			complete(&pcie_ep->wdma3);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Write Aborted. DMA3 completion.\n");
#endif

			return IRQ_HANDLED;
		}

		dev_err(dev, "Can not found write int flag\n");
		return IRQ_NONE;
	}

	val = readl(pcie_ep->dma_base + PCIE_DMA_RD_INT_STS) &
		PCIE_DMA_STS_IRQ3_MASK;
	writel(val, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);

	if (val != 0) {
		/* RD done Check */
		mask = (PCIE_DMA_STS_2CH_MASK << dma_offset);
		if (val & mask) {
			complete(&pcie_ep->rdma3);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Read done. DMA3 completion.\n");
#endif

			return IRQ_HANDLED;
		}

		/* RD abort Check */
		mask = (PCIE_DMA_STS_2CH_MASK << (dma_offset+16));
		if (val & mask) {
			complete(&pcie_ep->rdma3);
#if	DMA_LOG_PRINT_EP
			dev_info(dev, "Read Aborted. DMA3 completion.\n");
#endif

			return IRQ_HANDLED;
		}
		dev_err(dev, "Can not found read int flag\n");
		return IRQ_NONE;
	}

	return IRQ_HANDLED;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Parse data from dt"
 * @logic "Parse data from dt and save to drive structure."
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{-, int, 0, int, not 0}
 */
static int exynos_v920_pcie_ep_parse_dt(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	const char *use_bifuraction;
	const char *use_sris;
	const char *use_sysmmu;
	const char *use_sharability;

	if (of_property_read_u32(np, "pmu-phy-offset",
					&pcie_ep->pmu_phy_offset)) {
		dev_err(dev, "Failed to parse the pmu phy offset\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "pcie-clk-num",
					&pcie_ep->pcie_clk_num)) {
		dev_err(dev, "Failed to parse the number of pcie clock\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "phy-clk-num",
					&pcie_ep->phy_clk_num)) {
		dev_err(dev, "Failed to parse the number of phy clock\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "max-link-speed",
				&pcie_ep->max_link_speed)) {
		dev_warn(dev, "MAX Link Speed is NOT defined...(GEN1)\n");
		/* Default Link Speet is GEN1 */
		pcie_ep->max_link_speed = LINK_SPEED_GEN1;
	}

	if (of_property_read_u32(np, "num-lanes",
				&pcie_ep->lanes_num)) {
		dev_err(dev, "failed to parse a number of lanes\n");
		return -EINVAL;
	}

	/* use_bifurcation => 1: bifurcation, 0:aggregation */
	if (!of_property_read_string(np, "use-bifurcation", &use_bifuraction)) {
		if (!strcmp(use_bifuraction, "true")) {
			pcie_ep->use_bifurcation = true;
			dev_info(dev, "Bifurcation mode is ENABLED.\n");
		} else if (!strcmp(use_bifuraction, "false")) {
			pcie_ep->use_bifurcation = false;
		} else {
			dev_warn(dev, "Invalid use-bifurcation value");
			dev_warn(dev, "Set to default -> false)\n");
			pcie_ep->use_bifurcation = false;
		}
	} else {
		pcie_ep->use_bifurcation = true;
	}

	if (!of_property_read_string(np, "use-sris", &use_sris)) {
		if (!strcmp(use_sris, "true")) {
			pcie_ep->use_sris = true;
			dev_info(dev, "SRIS is ENABLED.\n");
		} else if (!strcmp(use_sris, "false")) {
			pcie_ep->use_sris = false;
		} else {
			dev_warn(dev, "Invalid use-sris value");
			dev_warn(dev, "Set to default -> false)\n");
			pcie_ep->use_sris = false;
		}
	} else {
		pcie_ep->use_sris = true;
	}

	if (of_property_read_u32(np, "s2mpu-base", &pcie_ep->s2mpu_base))
		pcie_ep->s2mpu_base = 0;

	if (of_property_read_u32(np, "vgen-base", &pcie_ep->vgen_base))
		pcie_ep->vgen_base = 0;

	pcie_ep->use_sysmmu = false;
	if (!(of_property_read_string(np, "use-sysmmu", &use_sysmmu)) &&
			!strcmp(use_sysmmu, "true"))
		pcie_ep->use_sysmmu = true;

	dev_info(dev, "PCIe EP SysMMU:%d(0:Disabled,1: Enabled)\n",
						pcie_ep->use_sysmmu);

	pcie_ep->use_sharability = false;
	if (!(of_property_read_string(np, "use-sharability", &use_sharability))
		&& !strcmp(use_sharability, "true"))
		pcie_ep->use_sharability = true;

	dev_info(dev, "PCIe EP RW Sharability:%d(0:Disabled,1: Enabled)\n",
						pcie_ep->use_sharability);

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{exynos_v920_pcie_ep_parse_dt_bar}
 * @purpose "Parse data from dt"
 * @logic "Parse data from dt and save to drive structure."
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{-, int, 0, int, not 0}
 */
static int exynos_v920_pcie_ep_parse_dt_bar(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	int i;
	u32 value;
	const char name[6][3][20] = {{"bar0", "bar0-size", "bar0-addr-format"},
				{"bar1", "bar1-size", "bar1-addr-format"},
				{"bar2", "bar2-size", "bar2-addr-format"},
				{"bar3", "bar3-size", "bar3-addr-format"},
				{"bar4", "bar4-size", "bar4-addr-format"},
				{"bar5", "bar5-size", "bar5-addr-format"}};

	for (i = BAR_0 ; i <= BAR_5 ; i++) {
		if (of_property_read_u32(np, name[i][0], &value)) {
			dev_err(dev, "Failed to parse the bar%d type\n", i);
			return -EINVAL;
		}

		pcie_ep->bar[i].type = value;

		if (of_property_read_u32(np, name[i][1], &value)) {
			dev_err(dev, "Failed to parse the bar%d size\n", i);
			return -EINVAL;
		}

		pcie_ep->bar[i].size = value;

		if (of_property_read_u32(np, name[i][2], &value)) {
			dev_err(dev, "Failed to parse the bar%d mem type\n", i);
			return -EINVAL;
		}

		pcie_ep->bar[i].mem_type = value;

		dev_info(dev, "BAR%d type(%d) size(0x%lx) mem_type(%d)\n",
			i, pcie_ep->bar[i].type, pcie_ep->bar[i].size,
			pcie_ep->bar[i].mem_type);
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Get clock info from dt"
 * @logic "Get clock info from dt."
 * @params
 * @param{in, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0/-ENODEV, -ENODEV}
 */
static int exynos_v920_pcie_ep_clk_get(struct exynos_ep_pcie *pcie_ep)
{
	struct exynos_pcie_ep_clks *clks = &pcie_ep->clks;
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int pcie_clk_num = pcie_ep->pcie_clk_num;
	int pcie_phyclk_num = pcie_ep->phy_clk_num;
	int i, total_clk_num, phy_cnt;
	int index = 0, offset = 0;
	int ret_hwacg = 0;

	/* Index[0] is for DBI HWACG enable/disable.
	 * V920 EVT2, EVT2.1 only.
	 * Need to check this in the next chip again.
	 */
	if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) {
		clks->dbi_hwacg = devm_clk_get(dev, "pcie_dbi_hwacg");
		if (IS_ERR(clks->dbi_hwacg)) {
			dev_err(dev, "%s Fail: dbi_hwacg!\n", __func__);
			return -ENODEV;
		}

		offset = 1;
		dev_info(dev, "%s CH%d dbi_hwacg(%d)\n", __func__,
			 pcie_ep->ch_num, offset);

		ret_hwacg = exynos_v920_pcie_ep_dbiclk_enable(dev, clks, true);
		if (ret_hwacg)
			dev_err(dev, "%s dbi_clk fail to enable(%d)\n",
				__func__, ret_hwacg);
		else
			dev_info(dev, "%s dbi_clk(%d) enable\n",
				 __func__, offset);
	}

	total_clk_num = pcie_clk_num + pcie_phyclk_num;

	/*
	 * CAUTION - PCIe and phy clock have to define in order.
	 * You must define related PCIe clock first in DT.
	 */
	for (i = 0; i < total_clk_num; i++) {
		index = i + offset;

		if (i < pcie_clk_num) {
			clks->pcie_clks[i] = of_clk_get(dev->of_node, index);
			if (IS_ERR(clks->pcie_clks[i])) {
				dev_err(dev, "Failed to get pcie clock\n");
				return -ENODEV;
			}
		} else {
			phy_cnt = i - pcie_clk_num;
			if (phy_cnt < 0) {
				dev_err(dev, "phy_cnt value negative\n");
				return -EINVAL;
			}
			clks->phy_clks[phy_cnt] =
				of_clk_get(dev->of_node, index);
			if (IS_ERR(clks->phy_clks[phy_cnt])) {
				dev_err(dev,
					"Failed to get PCIe EP PHY clock\n");
				return -ENODEV;
			}
		}
	}

	return 0;
}

void exynos_v920_pcie_ep_msi_init(struct exynos_ep_pcie *pcie_ep)
{
	pcie_ep->msi_region.msi_pa = 0x0;
	pcie_ep->msi_region.msi_va = NULL;
	pcie_ep->msi_region.msi_pciaddr = 0x0;
	pcie_ep->msi_region.umsi_pci_addr = 0x0;
	pcie_ep->msi_region.lmsi_pci_addr = 0x0;
	pcie_ep->msi_region.msi_size = 0x100;
}

static int exynos_v920_pcie_ep_msi_alloc(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;
	int ret = 0;

	if (pcie_ep->msi_region.msi_va) {
#if EXYNOS_PCIE_EP_DBG_MSI
		dev_info(dev,
			"%s already set. PA:0x%llx VA:0x%p PCI_ADDR:0x%llx\n",
			__func__, pcie_ep->msi_region.msi_pa,
			pcie_ep->msi_region.msi_va,
			pcie_ep->msi_region.msi_pciaddr);
#endif
		return ret;
	}

	pcie_ep->msi_region.msi_va = pci_epc_mem_alloc_addr(epc,
		&pcie_ep->msi_region.msi_pa, pcie_ep->msi_region.msi_size);

	if (pcie_ep->msi_region.msi_va == NULL) {
		dev_err(dev, "%s failed to allocate address\n", __func__);
		ret = -ENOMEM;
		goto msi_alloc_err;
	}

#if EXYNOS_PCIE_EP_DBG_MSI
	dev_info(dev, "%s ch%d msi epc alloc pa:0x%llx va:0x%p\n",
		__func__, pcie_ep->ch_num, pcie_ep->msi_region.msi_pa,
		pcie_ep->msi_region.msi_va);
#endif

	pcie_ep->msi_region.lmsi_pci_addr = dw_pcie_readl_dbi(pci, 0x54);
	pcie_ep->msi_region.umsi_pci_addr = dw_pcie_readl_dbi(pci, 0x58);
	pcie_ep->msi_region.msi_pciaddr =
		(pcie_ep->msi_region.umsi_pci_addr << 32)
		+ pcie_ep->msi_region.lmsi_pci_addr;

#if EXYNOS_PCIE_EP_DBG_MSI
	dev_info(dev, "%s msi pci addr: 0x%llx\n", __func__,
		pcie_ep->msi_region.msi_pciaddr);
#endif

	ret = exynos_v920_pcie_ep_epc_map_addr(epc, 0, 0,
		pcie_ep->msi_region.msi_pa, pcie_ep->msi_region.msi_pciaddr,
		pcie_ep->msi_region.msi_size);
	if (ret) {
		dev_err(dev, "%s failed to map address\n", __func__);
		ret = -ENOMEM;
		goto msi_alloc_map_err;
	}

	return 0;

msi_alloc_map_err:
	pci_epc_mem_free_addr(epc, pcie_ep->msi_region.msi_pa,
		pcie_ep->msi_region.msi_va,
		pcie_ep->msi_region.msi_size);
	pcie_ep->msi_region.msi_pa = 0x0;
	pcie_ep->msi_region.msi_va = NULL;
msi_alloc_err:
	return ret;
}

static void exynos_v920_pcie_ep_msi_free(struct exynos_ep_pcie *pcie_ep)
{
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;
	struct device *dev;

	dev = pcie_ep->pci->dev;

	if (pcie_ep->msi_region.msi_va) {
		pci_epc_unmap_addr(epc, 0, 0, pcie_ep->msi_region.msi_pa);
		pci_epc_mem_free_addr(epc, pcie_ep->msi_region.msi_pa,
			pcie_ep->msi_region.msi_va,
			pcie_ep->msi_region.msi_size);
		pcie_ep->msi_region.msi_pa = 0x0;
		pcie_ep->msi_region.msi_pciaddr = 0x0;
		pcie_ep->msi_region.msi_va = NULL;
		pcie_ep->msi_region.umsi_pci_addr = 0x0;
		pcie_ep->msi_region.lmsi_pci_addr = 0x0;
		dev_info(dev, "%s free and unmap for msi\n", __func__);
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform MSI test"
 * @logic "Allocate pci address\n
 *	Get msi address\n
 *	Map msi address to pci address\n
 *	Send msi."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, msi_data, int, >=0}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
int exynos_v920_pcie_ep_msi(struct exynos_ep_pcie *pcie_ep, int msi_data)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;

	int ret = 0;
	u64 aligned_offset;

	if (pcie_ep->msi_region.msi_va == NULL) {
		dev_err(dev, "%s ch%d msi epc alloc mem is NULL\n", __func__,
			pcie_ep->ch_num);
		ret = -ENOMEM;
		goto err;
	}

	aligned_offset = pcie_ep->msi_region.lmsi_pci_addr
		& (epc->mem->window.page_size - 1);
	writel(msi_data, pcie_ep->msi_region.msi_va + aligned_offset);

err:
	return ret;
}

int exynos_v920_pcie_ep_msix(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;

	int ret = 0;
	void __iomem *dst_addr;

	phys_addr_t phys_addr;
	u64 umsi_pci_addr;
	u64 lmsi_pci_addr;
	u64 pci_addr;
	u32 size = 0x100;
	u64 aligned_offset;

	dst_addr = pci_epc_mem_alloc_addr(epc, &phys_addr, size);

	if (!dst_addr) {
		dev_err(dev, "failed to allocate address\n");
		ret = -ENOMEM;
		return ret;
	}

	pci_addr = MSIX_ADDR;
	lmsi_pci_addr = lower_32_bits(pci_addr);
	umsi_pci_addr = upper_32_bits(pci_addr);

	dev_info(dev, "umsi: 0x%llx , lmsi: 0x%llx\n",
			umsi_pci_addr, lmsi_pci_addr);
	dev_info(dev, "pci_addr: 0x%llx\n", pci_addr);

	ret = exynos_v920_pcie_ep_epc_map_addr(epc, 0, pcie_ep->vf_num,
		phys_addr, pci_addr, size);
	if (ret) {
		dev_err(dev, "failed to map address\n");
		ret = -ENOMEM;
		goto err;
	}

	dev_info(dev, "write + allignd MSIX: %x VF:%d\n",
		pcie_ep->msi_data, pcie_ep->vf_num);

	aligned_offset = lmsi_pci_addr & (epc->mem->window.page_size - 1);
	writel(pcie_ep->msi_data, dst_addr + aligned_offset);

	pci_epc_unmap_addr(epc, 0, 0, phys_addr);
err:
	pci_epc_mem_free_addr(epc, phys_addr, dst_addr, size);

	return ret;
}

#if DMA_CONT_MODE_EP
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform dma write test"
 * @logic "Allocation pci address\n
 *	Set outbound\n
 *	Allocation dma write buffer\n
 *	Generate test data\n
 *	Set dma write and start dma\n
 *	Wait write dma complete."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, dst_pci_addr, u64, >=0}
 * @param{in, dma_num, int, 0~7}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
static int exynos_v920_pcie_ep_write(struct exynos_ep_pcie *pcie_ep,
	dma_addr_t dst_ob_pci_addr, dma_addr_t src_phys_addr,
	size_t size, int dma_num)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int ret = 0;
	struct completion *dma_comp = NULL;
	u32 dma_size = size;
#ifdef MEASURE_SPEED
	u64 start, end;
#endif

	switch (dma_num) {
	case 0:
	case 1:
		dma_comp = &pcie_ep->wdma0;
		break;
	case 2:
	case 3:
		dma_comp = &pcie_ep->wdma1;
		break;
	case 4:
	case 5:
		dma_comp = &pcie_ep->wdma2;
		break;
	case 6:
	case 7:
		dma_comp = &pcie_ep->wdma3;
		break;
	default:
		dev_err(dev, "%s dma_num is wrong:%d",
			__func__, dma_num);
		ret = -ENODEV;
		goto err;
	}

#ifdef MEASURE_SPEED
	start = getCurrentTimestampInNSec_v920();
#endif

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_WR_EN);
	writel(0x1, pcie_ep->dma_base + 0X128);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_INT_CLEAR);
	writel(0x0 | CH_CTRL1_LIE | CH_CTRL1_DMA_RESERVED5, pcie_ep->dma_base +
		PCIE_DMA_CH_CTRL1 + dma_num * PCIE_DMA_CH_OFFSET);

	/* size ragne : 1byte ~ 4Gbyte */
	writel(dma_size, pcie_ep->dma_base + PCIE_DMA_WR_XFER_SIZE +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* source : dram */
	writel(lower_32_bits(src_phys_addr),
			pcie_ep->dma_base + PCIE_DMA_WR_SARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(src_phys_addr),
			pcie_ep->dma_base + PCIE_DMA_WR_SARH +
				dma_num * PCIE_DMA_CH_OFFSET);
	/* destination : PCIe mem */
	writel(lower_32_bits(dst_ob_pci_addr),
			pcie_ep->dma_base + PCIE_DMA_WR_DARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(dst_ob_pci_addr),
			pcie_ep->dma_base + PCIE_DMA_WR_DARH +
				dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_WR_DBOFF);

	wait_for_completion(dma_comp);

	/* DMA Stop */
	writel(PCIE_DMA_DBOFF_STOP, pcie_ep->dma_base + PCIE_DMA_WR_DBOFF);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_EN);

	if (pcie_ep->state == STATE_LINK_DOWN ||
		pcie_ep->state == STATE_LINK_DOWN_ABNORMAL) {
		dev_info(&pdev->dev, "write work is closed forcedly\n");

		ret = -EPIPE;
		goto err;
	}

#ifdef MEASURE_SPEED
	end = getCurrentTimestampInNSec_v920();
	dev_info(&pdev->dev, "##### WRITE TEST #####\n");
	measureTransitionSpeed_v920(pcie_ep, start, end, size);
#endif

	dev_info(&pdev->dev, "WR TEST SUCCESS\n");

err:

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform dma read test"
 * @logic "Alloc pci address\n
 *	Set outbound\n
 *	Alloc read dma\n
 *	Set and start read dma\n
 *	Wait read dma completion"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, src_pci_addr, u64, >=0}
 * @param{in, size, u32, >=0}
 * @param{in, dma_num, int, 0~7}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
static int exynos_v920_pcie_ep_read(struct exynos_ep_pcie *pcie_ep,
	phys_addr_t src_ob_phys_addr, dma_addr_t dst_phys_addr,
	size_t size, int dma_num)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int ret = 0;
	struct completion *dma_comp = NULL;
	u32 dma_size = size;
#ifdef MEASURE_SPEED
	u64 start, end;
#endif

	switch (dma_num) {
	case 0:
	case 1:
		dma_comp = &pcie_ep->rdma0;
		break;
	case 2:
	case 3:
		dma_comp = &pcie_ep->rdma1;
		break;
	case 4:
	case 5:
		dma_comp = &pcie_ep->rdma2;
		break;
	case 6:
	case 7:
		dma_comp = &pcie_ep->rdma3;
		break;
	default:
		dev_err(dev, "%s dma_num is wrong:%d",
			__func__, dma_num);
		ret = -ENODEV;
		goto err;
	}

#ifdef MEASURE_SPEED
	start = getCurrentTimestampInNSec_v920();
#endif

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_RD_EN);
	writel(0x1, pcie_ep->dma_base + 0X168);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);
	writel(0x0 | CH_CTRL2_LIE | CH_CTRL2_DMA_RESERVED5, pcie_ep->dma_base +
		PCIE_DMA_CH_CTRL2 + dma_num*PCIE_DMA_CH_OFFSET);

	/* size ragne : 1byte ~ 4Gbyte */
	writel(dma_size, pcie_ep->dma_base + PCIE_DMA_RD_XFER_SIZE +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* source : PCIe mem */
	writel(lower_32_bits(src_ob_phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_SARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(src_ob_phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_SARH +
				dma_num * PCIE_DMA_CH_OFFSET);
	/* destination : Dram */
	writel(lower_32_bits(dst_phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_DARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(dst_phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_DARH +
				dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_RD_DBOFF);

	wait_for_completion(dma_comp);

	/* DMA Stop */
	writel(PCIE_DMA_DBOFF_STOP, pcie_ep->dma_base + PCIE_DMA_RD_DBOFF);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_EN);

	if (pcie_ep->state == STATE_LINK_DOWN ||
		pcie_ep->state == STATE_LINK_DOWN_ABNORMAL) {
		dev_info(&pdev->dev, "read work is closed forcedly\n");

		ret = -EPIPE;
		goto err;
	}

#ifdef MEASURE_SPEED
	end = getCurrentTimestampInNSec_v920();
	dev_info(&pdev->dev, "##### READ TEST #####\n");
	measureTransitionSpeed_v920(pcie_ep, start, end, size);
#endif

	dev_info(&pdev->dev, "RD TEST SUCCESS\n");

err:
	return ret;
}
#else
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform dma write test"
 * @logic "Allocation pci address\n
 *	Set outbound\n
 *	Allocation dma write buffer\n
 *	Generate test data\n
 *	Set dma write and start dma\n
 *	Wait write dma complete."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, dst_pci_addr, u64, >=0}
 * @param{in, dma_num, int, 0~7}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
static int exynos_v920_pcie_ep_write_ll(struct exynos_ep_pcie *pcie_ep,
	dma_addr_t dst_ob_pci_addr, dma_addr_t src_phys_addr,
	size_t size, int dma_num)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int ret = 0;
	struct completion *dma_comp = NULL;
#ifdef MEASURE_SPEED
	u64 start;
#endif

	switch (dma_num) {
	case 0:
	case 1:
		dma_comp = &pcie_ep->wdma0;
		break;
	case 2:
	case 3:
		dma_comp = &pcie_ep->wdma1;
		break;
	case 4:
	case 5:
		dma_comp = &pcie_ep->wdma2;
		break;
	case 6:
	case 7:
		dma_comp = &pcie_ep->wdma3;
		break;
	default:
		dev_err(dev, "%s dma_num is wrong:%d",
			__func__, dma_num);
		ret = -ENODEV;
		goto err;
	}

#ifdef MEASURE_SPEED
	start = getCurrentTimestampInNSec_v920();
#endif

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_WR_EN);
	/* enable DMA power */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_WR_PWR_EN +
		PWR_EN_OFFSET(dma_num));
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_INT_CLEAR);
	writel(0x0 | CH_CTRL1_LLE | CH_CTRL1_LIE | CH_CTRL1_DMA_RESERVED5,
		pcie_ep->dma_base + PCIE_DMA_CH_CTRL1 +
		dma_num * PCIE_DMA_CH_OFFSET);

	writel(lower_32_bits(src_phys_addr),
		pcie_ep->dma_base + PCIE_DMA_CH_CTRL1 +  0x1c +
		dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(src_phys_addr),
		pcie_ep->dma_base + PCIE_DMA_CH_CTRL1 +  0x20 +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_WR_DBOFF);

err:

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform dma read test"
 * @logic "Alloc pci address\n
 *	Set outbound\n
 *	Alloc read dma\n
 *	Set and start read dma\n
 *	Wait read dma completion"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, src_pci_addr, u64, >=0}
 * @param{in, size, u32, >=0}
 * @param{in, dma_num, int, 0~7}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
static int exynos_v920_pcie_ep_read_ll(struct exynos_ep_pcie *pcie_ep,
	phys_addr_t src_ob_phys_addr, dma_addr_t dst_phys_addr,
	size_t size, int dma_num)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int ret = 0;
	struct completion *dma_comp = NULL;
#ifdef MEASURE_SPEED
	u64 start;
#endif

	switch (dma_num) {
	case 0:
	case 1:
		dma_comp = &pcie_ep->rdma0;
		break;
	case 2:
	case 3:
		dma_comp = &pcie_ep->rdma1;
		break;
	case 4:
	case 5:
		dma_comp = &pcie_ep->rdma2;
		break;
	case 6:
	case 7:
		dma_comp = &pcie_ep->rdma3;
		break;
	default:
		dev_err(dev, "%s dma_num is wrong:%d",
			__func__, dma_num);
		ret = -ENODEV;
		goto err;
	}

#ifdef MEASURE_SPEED
	start = getCurrentTimestampInNSec_v920();
#endif

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_RD_EN);
	/* enable DMA power */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_RD_PWR_EN +
		PWR_EN_OFFSET(dma_num));
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);

	writel(0x0 | CH_CTRL1_LLE | CH_CTRL2_LIE | CH_CTRL2_DMA_RESERVED5,
		pcie_ep->dma_base + PCIE_DMA_CH_CTRL2 +
		dma_num * PCIE_DMA_CH_OFFSET);

	writel(lower_32_bits(src_ob_phys_addr),
		pcie_ep->dma_base + PCIE_DMA_CH_CTRL2 +  0x1c +
		dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(src_ob_phys_addr),
		pcie_ep->dma_base + PCIE_DMA_CH_CTRL2 +  0x20 +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_RD_DBOFF);

err:
	return ret;
}

#endif

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform dma write test"
 * @logic "Allocation pci address\n
 *	Set outbound\n
 *	Allocation dma write buffer\n
 *	Generate test data\n
 *	Set dma write and start dma\n
 *	Wait write dma complete."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, dst_pci_addr, u64, >=0}
 * @param{in, dma_num, int, 0~7}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
int exynos_v920_pcie_ep_write_test(struct exynos_ep_pcie *pcie_ep,
	u64 dst_pci_addr, int dma_num)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;
	void __iomem *dst_addr;
	int ret = 0;
	u32 size = pcie_ep->size;
	u8 headbuf[2];
#ifdef MEASURE_SPEED
	u64 start, end;
#endif

	/* EP PCIe mem address */
	phys_addr_t phys_addr;

	pcie_ep->work_state = STATE_DMA_BUSY;

	/* For the test, add data length 2 bytes */
	size = size + 2;

	dst_addr = pci_epc_mem_alloc_addr(epc, &phys_addr, size);
	if (!dst_addr) {
		dev_err(dev, "failed to allocate address\n");
		ret = -ENOMEM;
		goto err;
	}

	ret = exynos_v920_pcie_ep_epc_map_addr(epc, 0, 0, phys_addr,
						dst_pci_addr, size);
	if (ret) {
		dev_err(dev, "failed to map address\n");
		ret = -ENOMEM;
		goto err_addr;
	}

	pcie_ep->dma_wr_buf = dma_alloc_coherent(dev,
			size, &pcie_ep->wr_dma, GFP_KERNEL);
	if (!pcie_ep->dma_wr_buf) {
		ret = -ENOMEM;
		goto err_map_addr;
	}

	headbuf[0] = (u8)(pcie_ep->size >> 8);
	headbuf[1] = (u8) pcie_ep->size;

	memcpy(pcie_ep->dma_wr_buf, headbuf, 2);

	get_random_bytes(pcie_ep->dma_wr_buf + 2, pcie_ep->size);

#ifdef MEASURE_SPEED
	start = getCurrentTimestampInNSec_v920();
#endif

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_WR_EN);

	writel(0x1, pcie_ep->dma_base + 0X128);

	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_INT_CLEAR);
	writel(0x0 | CH_CTRL1_LIE | CH_CTRL1_DMA_RESERVED5, pcie_ep->dma_base +
		PCIE_DMA_CH_CTRL1 + dma_num * PCIE_DMA_CH_OFFSET);

	/* size ragne : 1byte ~ 4Gbyte */
	writel(size, pcie_ep->dma_base + PCIE_DMA_WR_XFER_SIZE +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* source : dram */
	writel(lower_32_bits(pcie_ep->wr_dma),
			pcie_ep->dma_base + PCIE_DMA_WR_SARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(pcie_ep->wr_dma),
			pcie_ep->dma_base + PCIE_DMA_WR_SARH +
				dma_num * PCIE_DMA_CH_OFFSET);
	/* destination : PCIe mem */
	writel(lower_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_WR_DARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_WR_DARH +
				dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_WR_DBOFF);

	wait_for_completion(&pcie_ep->wdma0);

	/* DMA Stop */
	writel(PCIE_DMA_DBOFF_STOP, pcie_ep->dma_base + PCIE_DMA_WR_DBOFF);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_EN);

	if (pcie_ep->state == STATE_LINK_DOWN ||
		pcie_ep->state == STATE_LINK_DOWN_ABNORMAL) {
		dev_info(&pdev->dev, "write work is closed forcedly\n");

		ret = -EPIPE;
		goto err_map_addr;
	}

#ifdef MEASURE_SPEED
	end = getCurrentTimestampInNSec_v920();
	dev_info(&pdev->dev, "##### WRITE TEST #####\n");
	measureTransitionSpeed_v920(pcie_ep, start, end, size);
#endif

	dev_info(&pdev->dev, "WR TEST SUCCESS\n");

err_map_addr:
	pci_epc_unmap_addr(epc, 0, 0, phys_addr);

err_addr:
	pci_epc_mem_free_addr(epc, phys_addr, dst_addr, size);

err:
	pcie_ep->work_state = STATE_DMA_IDLE;

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform dma read test"
 * @logic "Alloc pci address\n
 *	Set outbound\n
 *	Alloc read dma\n
 *	Set and start read dma\n
 *	Wait read dma completion"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, src_pci_addr, u64, >=0}
 * @param{in, size, u32, >=0}
 * @param{in, dma_num, int, 0~7}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
int exynos_v920_pcie_ep_read_test(struct exynos_ep_pcie *pcie_ep,
				u64 src_pci_addr, u32 size, int dma_num)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;

	int ret = 0;
	void __iomem *src_addr;
	u32 dma_xfer_size;
	/* EP PCIe mem address */
	phys_addr_t phys_addr;

	pcie_ep->work_state = STATE_DMA_BUSY;

	src_addr = pci_epc_mem_alloc_addr(epc, &phys_addr, size);
	if (!src_addr) {
		dev_err(dev, "failed to allocate address\n");
		ret = -ENOMEM;
		goto err;
	}

	ret = exynos_v920_pcie_ep_epc_map_addr(epc, 0, 0, phys_addr,
						src_pci_addr, size);
	if (ret) {
		dev_err(dev, "failed to map address\n");
		ret = -ENOMEM;
		goto err_addr;
	}

	if (!pcie_ep->dma_rd_buf) {
		pcie_ep->dma_rd_buf = dma_alloc_coherent(dev, size,
				&pcie_ep->rd_dma, GFP_KERNEL);
		if (!pcie_ep->dma_rd_buf) {
			ret = -ENOMEM;
			goto err_map_addr;
		}
	}

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_RD_EN);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);
	writel(0x0 | CH_CTRL2_LIE | CH_CTRL2_DMA_RESERVED5, pcie_ep->dma_base +
		PCIE_DMA_CH_CTRL2 + dma_num*PCIE_DMA_CH_OFFSET);

	/* size ragne : 1byte ~ 4Gbyte */
	dma_xfer_size = size;
	writel(dma_xfer_size, pcie_ep->dma_base + PCIE_DMA_RD_XFER_SIZE +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* source : PCIe mem */
	writel(lower_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_SARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_SARH +
				dma_num * PCIE_DMA_CH_OFFSET);
	/* destination : Dram */
	writel(lower_32_bits(pcie_ep->rd_dma),
			pcie_ep->dma_base + PCIE_DMA_RD_DARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(pcie_ep->rd_dma),
			pcie_ep->dma_base + PCIE_DMA_RD_DARH +
				dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_RD_DBOFF);

	wait_for_completion(&pcie_ep->rdma0);

	/* DMA Stop */
	writel(PCIE_DMA_DBOFF_STOP, pcie_ep->dma_base + PCIE_DMA_RD_DBOFF);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_EN);

	if (pcie_ep->state == STATE_LINK_DOWN ||
		pcie_ep->state == STATE_LINK_DOWN_ABNORMAL) {
		dev_info(&pdev->dev, "read work is closed forcedly\n");

		ret = -EPIPE;
		goto err_map_addr;
	}

	dev_info(&pdev->dev, "RD TEST SUCCESS\n");

err_map_addr:
	pci_epc_unmap_addr(epc, 0, 0, phys_addr);

err_addr:
	pci_epc_mem_free_addr(epc, phys_addr, src_addr, size);

err:
	pcie_ep->work_state = STATE_DMA_IDLE;

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform crc check test for write value"
 * @logic "Alloc read dma; set and start read dma\n
 *	Wait read dma completion\n
 *	Check crc"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, pci_addr, u64, >=0}
 * @param{in, phys_addr, struct ::phys_addr_t, >=0}
 * @param{in, size, u32, >=0}
 * @param{in, dma_num, int, 0~7}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
int exynos_v920_pcie_ep_test_write_check(struct exynos_ep_pcie *pcie_ep,
		u64 pci_addr, phys_addr_t phys_addr, u32 size, int dma_num)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int ret = 0;
	u32 crc32;
#ifdef MEASURE_SPEED
	u64 start, end;
#endif

	pcie_ep->work_state = STATE_DMA_BUSY;

	pcie_ep->dma_rd_buf = dma_alloc_coherent(dev,
			size, &pcie_ep->rd_dma, GFP_KERNEL);
	if (!pcie_ep->dma_rd_buf) {
		ret = -ENOMEM;
		goto err;
	}

#ifdef MEASURE_SPEED
	start = getCurrentTimestampInNSec_v920();
#endif

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_RD_EN);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);
	writel(0x0 | CH_CTRL2_LIE | CH_CTRL2_DMA_RESERVED5, pcie_ep->dma_base +
		PCIE_DMA_CH_CTRL2 + dma_num * PCIE_DMA_CH_OFFSET);

	/* size ragne : 1byte ~ 4Gbyte */
	writel(size, pcie_ep->dma_base + PCIE_DMA_RD_XFER_SIZE +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* source : PCIe mem */
	writel(lower_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_SARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_SARH +
				dma_num * PCIE_DMA_CH_OFFSET);
	/* destination : Dram */
	writel(lower_32_bits(pcie_ep->rd_dma),
			pcie_ep->dma_base + PCIE_DMA_RD_DARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(pcie_ep->rd_dma),
			pcie_ep->dma_base + PCIE_DMA_RD_DARH +
				dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_RD_DBOFF);

	wait_for_completion(&pcie_ep->rdma0);

	/* DMA Stop */
	writel(PCIE_DMA_DBOFF_STOP, pcie_ep->dma_base + PCIE_DMA_RD_DBOFF);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_EN);

#ifdef MEASURE_SPEED
	end = getCurrentTimestampInNSec_v920();
	dev_info(&pdev->dev, "##### READ TEST #####\n");
	measureTransitionSpeed_v920(pcie_ep, start, end, size);
#endif

	crc32 = crc32_le(~0, pcie_ep->dma_rd_buf, size);
	if (crc32 != pcie_ep->checksum) {
		ret = -EIO;
		goto err_dma;
	}
err_dma:
	dma_free_coherent(dev, size, pcie_ep->dma_rd_buf, pcie_ep->rd_dma);
err:
	pcie_ep->work_state = STATE_DMA_IDLE;

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform dma write and read test"
 * @logic "Alloc pci address; set outbound\n
 *	Alloc write dma\n
 *	Generate write data\n
 *	Set and start write dma\n
 *	Wait write dma completion\n
 *	Alloc read dma\n
 *	Set and start read dma\n
 *	Wait read dma completion\n
 *	Check crc"
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @param{in, dst_pci_addr, u64, >=0}
 * @param{in, dma_num, int, >=0}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
int exynos_v920_pcie_ep_write_read_check(struct exynos_ep_pcie *pcie_ep,
	u64 dst_pci_addr, int dma_num)
{
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;
	void __iomem *dst_addr;
	int ret = 0;
	u32 size = pcie_ep->size;
	u32 crc32_val;
#ifdef MEASURE_SPEED
	u64 start, end;
#endif
	/* EP PCIe mem address */
	phys_addr_t phys_addr;
	struct completion *dma_wrcomp = NULL;
	struct completion *dma_rdcomp = NULL;

	switch (dma_num) {
	case DMA_WR_CH0:
	case DMA_WR_CH1:
		dma_wrcomp = &pcie_ep->wdma0;
		dma_rdcomp = &pcie_ep->rdma0;
		break;
	case DMA_WR_CH2:
	case DMA_WR_CH3:
		dma_wrcomp = &pcie_ep->wdma1;
		dma_rdcomp = &pcie_ep->rdma1;
		break;
	case DMA_WR_CH4:
	case DMA_WR_CH5:
		dma_wrcomp = &pcie_ep->wdma2;
		dma_rdcomp = &pcie_ep->rdma2;
		break;
	case DMA_WR_CH6:
	case DMA_WR_CH7:
		dma_wrcomp = &pcie_ep->wdma3;
		dma_rdcomp = &pcie_ep->rdma3;
		break;
	default:
		dev_err(dev, "%s dma_num is wrong:%d",
			__func__, dma_num);
		ret = -ENODEV;
		goto err;
	}

	pcie_ep->work_state = STATE_DMA_BUSY;

	dst_addr = pci_epc_mem_alloc_addr(epc, &phys_addr, size);
	if (!dst_addr) {
		dev_err(dev, "failed to allocate address\n");
		ret = -ENOMEM;
		goto err;
	}

	ret = exynos_v920_pcie_ep_epc_map_addr(epc, 0, 0, phys_addr,
						dst_pci_addr, size);
	if (ret) {
		dev_err(dev, "failed to map address\n");
		ret = -ENOMEM;
		goto err_addr;
	}

	pcie_ep->dma_wr_buf = dma_alloc_coherent(dev,
			size, &pcie_ep->wr_dma, GFP_KERNEL);
	if (!pcie_ep->dma_wr_buf) {
		ret = -ENOMEM;
		goto err_map_addr;
	}

	get_random_bytes(pcie_ep->dma_wr_buf, pcie_ep->size);
	pcie_ep->checksum = crc32_le(~0, pcie_ep->dma_wr_buf, size);

#ifdef MEASURE_SPEED
	start = getCurrentTimestampInNSec_v920();
#endif

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_WR_EN);

	writel(0x1, pcie_ep->dma_base + 0X128 + dma_num * 0x4);

	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_WR_INT_CLEAR);
	writel(0x0 | CH_CTRL1_LIE | CH_CTRL1_DMA_RESERVED5, pcie_ep->dma_base +
		PCIE_DMA_CH_CTRL1 + dma_num * PCIE_DMA_CH_OFFSET);

	/* size ragne : 1byte ~ 4Gbyte */
	writel(size, pcie_ep->dma_base + PCIE_DMA_WR_XFER_SIZE +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* source : dram */
	writel(lower_32_bits(pcie_ep->wr_dma),
			pcie_ep->dma_base + PCIE_DMA_WR_SARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(pcie_ep->wr_dma),
			pcie_ep->dma_base + PCIE_DMA_WR_SARH +
				dma_num * PCIE_DMA_CH_OFFSET);
	/* destination : PCIe mem */
	writel(lower_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_WR_DARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_WR_DARH +
				dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_WR_DBOFF);

	wait_for_completion(dma_wrcomp);

	if (pcie_ep->state == STATE_LINK_DOWN ||
		pcie_ep->state == STATE_LINK_DOWN_ABNORMAL) {
		dev_info(&pdev->dev, "write work is closed forcedly\n");

		ret = -EPIPE;
		goto err_dma;
	}

#ifdef MEASURE_SPEED
	end = getCurrentTimestampInNSec_v920();
	dev_info(&pdev->dev, "##### WRITE TEST #####\n");
	measureTransitionSpeed_v920(pcie_ep, start, end, size);
#endif

	/* Read operation */
	pcie_ep->dma_rd_buf = dma_alloc_coherent(dev,
			size, &pcie_ep->rd_dma, GFP_KERNEL);
	if (!pcie_ep->dma_rd_buf) {
		ret = -ENOMEM;
		goto err_dma;
	}

#ifdef MEASURE_SPEED
	start = getCurrentTimestampInNSec_v920();
#endif

	/* DMA setting */
	writel(0x1, pcie_ep->dma_base + PCIE_DMA_RD_EN);

	writel(0x1, pcie_ep->dma_base + 0X168 + dma_num * 0x4);

	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_MASK);
	writel(0x0, pcie_ep->dma_base + PCIE_DMA_RD_INT_CLEAR);
	writel(0x0 | CH_CTRL2_LIE | CH_CTRL2_DMA_RESERVED5, pcie_ep->dma_base +
		PCIE_DMA_CH_CTRL2 + dma_num * PCIE_DMA_CH_OFFSET);

	/* size ragne : 1byte ~ 4Gbyte */
	writel(size, pcie_ep->dma_base + PCIE_DMA_RD_XFER_SIZE +
		dma_num * PCIE_DMA_CH_OFFSET);

	/* source : PCIe mem */
	writel(lower_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_SARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(phys_addr),
			pcie_ep->dma_base + PCIE_DMA_RD_SARH +
				dma_num * PCIE_DMA_CH_OFFSET);
	/* destination : Dram */
	writel(lower_32_bits(pcie_ep->rd_dma),
			pcie_ep->dma_base + PCIE_DMA_RD_DARL +
				dma_num * PCIE_DMA_CH_OFFSET);
	writel(upper_32_bits(pcie_ep->rd_dma),
			pcie_ep->dma_base + PCIE_DMA_RD_DARH +
				dma_num * PCIE_DMA_CH_OFFSET);

	/* start dma ch0 */
	writel(dma_num, pcie_ep->dma_base + PCIE_DMA_RD_DBOFF);

	wait_for_completion(dma_rdcomp);

	if (pcie_ep->state == STATE_LINK_DOWN ||
		pcie_ep->state == STATE_LINK_DOWN_ABNORMAL) {
		dev_info(&pdev->dev, "read work is closed forcedly\n");

		ret = -EPIPE;
		goto err_dma;
	}

#ifdef MEASURE_SPEED
	end = getCurrentTimestampInNSec_v920();
	dev_info(&pdev->dev, "##### READ TEST #####\n");
	measureTransitionSpeed_v920(pcie_ep, start, end, size);
#endif

	crc32_val = crc32_le(~0, pcie_ep->dma_rd_buf, size);
	if (crc32_val != pcie_ep->checksum) {
		ret = -EIO;
		goto err_dma;
	}
	dev_info(&pdev->dev, "WR/RD TEST SUCCESS\n");

err_dma:
	dma_free_coherent(dev, size, pcie_ep->dma_wr_buf, pcie_ep->wr_dma);

err_map_addr:
	pci_epc_unmap_addr(epc, 0, 0, phys_addr);

err_addr:
	pci_epc_mem_free_addr(epc, phys_addr, dst_addr, size);

err:
	pcie_ep->work_state = STATE_DMA_IDLE;

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Perform test functions"
 * @logic "Check test command\n
 *	Set testing address\n
 *	Call test function"
 * @params
 * @param{in/out, work, struct ::work_struct *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_work_cmd(struct work_struct *work)
{
	struct exynos_ep_pcie *pcie_ep =
		container_of(work, struct exynos_ep_pcie, test_work.work);
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	u64 pci_addr;

	dev_info(dev, "Current work cmd: %d\n", pcie_ep->work_cmd);
	switch (pcie_ep->work_cmd) {
	case WORK_MEM_WR:
		/* Destination Address: RC Reserved Memory Address */
		pci_addr = WR_DMA_TEST_ADDR;
		if (exynos_v920_pcie_ep_write_test(pcie_ep,
			pci_addr, pcie_ep->dma_num) == 0) {
			/* MSI Send to Notify End of Transaction */
			if (exynos_v920_pcie_ep_msi(pcie_ep, 0) != 0)
				dev_warn(dev, "MSI TEST Fail\n");
		} else {
			dev_warn(dev, "WR TEST Fail\n");
		}
		break;

	case WORK_MEM_RD:
		/* Source Address: RC Reserved Memory Address */
		pci_addr = RD_DMA_TEST_ADDR;
		if (exynos_v920_pcie_ep_read_test(pcie_ep, pci_addr,
					pcie_ep->size, pcie_ep->dma_num) != 0)
			dev_warn(dev, "RD TEST Fail\n");
		break;

	case WORK_MSI:
		exynos_v920_pcie_ep_msi_alloc(pcie_ep);
		if (exynos_v920_pcie_ep_msi(pcie_ep, pcie_ep->msi_data) != 0)
			dev_warn(dev, "MSI TEST Fail\n");
		break;

	case WORK_DOORBELL:
		/* Received Test Read Request */
		pci_addr = RD_DMA_TEST_ADDR;
		if (exynos_v920_pcie_ep_read_test(pcie_ep,
				pci_addr, pcie_ep->size, 0) == 0)
			dev_info(dev, "Read from RC RMEM Complete\n");
		else
			dev_warn(dev, "RD TEST Fail!\n");
		break;

	case WORK_MEM_WR_RD:
		pci_addr = WR_DMA_TEST_ADDR;
		if (exynos_v920_pcie_ep_write_read_check(pcie_ep, pci_addr,
			pcie_ep->dma_num) != 0)
			dev_warn(dev, "MEM WR/RD TEST Fail\n");
		break;

	case WORK_MSI_X:
		if (exynos_v920_pcie_ep_msix(pcie_ep) != 0)
			dev_warn(dev, "MSI-X TEST Fail\n");
		break;

	default:
		dev_warn(dev, "Unsupported work cmd: %d\n", pcie_ep->work_cmd);
		break;
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_link}
 * @purpose "Perform exception handling"
 * @logic "If exception is WORK_LINK_RESET, start link stop"
 * @params
 * @param{in/out, work, struct ::work_struct *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_work_exception(struct work_struct *work)
{
	struct exynos_ep_pcie *pcie_ep =
		container_of(work, struct exynos_ep_pcie, exception_work.work);
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	int count;

	dev_info(dev, "Current exception: %d\n", pcie_ep->work_exception);
	switch (pcie_ep->work_exception) {
	case WORK_LINK_RESET:

		count = 0;
		while (count < EXP_CNT) {
			if (pcie_ep->work_state != STATE_DMA_BUSY) {
				if (pcie_ep->use_bifurcation)
					exynos_v920_update_ep_lane_status(
						pcie_ep);

				dev_info(&pdev->dev, "Start Link Stop\n");
				exynos_v920_pcie_ep_link_stop(pcie_ep);

				pcie_ep->state = STATE_LINK_DOWN;
				break;
			}

			udelay(100);
			count++;
		}
		break;

	default:
		dev_info(dev, "Default exception: %d\n",
					pcie_ep->work_exception);
		break;
	}
}

void exynos_v920_pcie_ep_work_dislink(struct work_struct *work)
{
	struct exynos_ep_pcie *pcie_ep =
		container_of(work, struct exynos_ep_pcie, dislink_work.work);

	exynos_v920_pcie_ep_msi_free(pcie_ep);
}

static int exynos_v920_pcie_ep_test_bar(struct device *dev,
					struct exynos_ep_pcie *pcie_ep)
{
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;
	struct pci_epf_bar epf_bar;
	int flags;
	int bar;
	int ret = 0;
	dma_addr_t phys_addr;
	size_t size;


	flags = PCI_BASE_ADDRESS_SPACE_MEMORY | PCI_BASE_ADDRESS_MEM_TYPE_32;
	if (sizeof(dma_addr_t) == 0x8)
		flags |=  PCI_BASE_ADDRESS_MEM_TYPE_64;

	/* set bar 0 */
	bar = 0;
	phys_addr = 0x17000000;
	size = 0xffffff;

	epf_bar.barno = bar;
	epf_bar.size = size;
	epf_bar.flags = flags;
	epf_bar.phys_addr = phys_addr;

	ret = pci_epc_set_bar(epc, 0, 0, &epf_bar);
	if (ret) {
		dev_err(dev, "[%s] failed to set BAR\n", __func__);
		return ret;
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Show debug information"
 * @logic "Show debug information"
 * @params
 * @param{in, dev, struct ::device *, not NULL}
 * @param{in, attr, struct ::device_attribute *, not NULL}
 * @param{in, buf, char *, not NULL}
 * @endparam
 * @retval{-, ssize_t, 0, 0, not 0}
 */
static ssize_t ep_dbg_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	return 0;
}


/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Handler for sysfs test"
 * @logic "If command is 0, dump resistor values\n
 *	If command is 1, check link status\n
 *	If command is 2, call dma read test\n
 *	If command is 3, call dma write test\n
 *	If command is 4, call msi test\n
 *	If command is 5, call dma write/read test\n
 *	If command is 6, call bar allocation test"
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @param{in, attr, struct ::device_attribute *, not NULL}
 * @param{in, buf, char *, not NULL}
 * @param{in, count, size_t, >=0}
 * @endparam
 * @retval{ret, ssize_t, 0, -ERANGE~0, <0}
 */
static ssize_t ep_dbg_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int op_num;
	struct exynos_ep_pcie *pcie_ep = dev_get_drvdata(dev);
	u32 val;
	unsigned int size;
	int cnt;
	int dma_num = 0;
	int msi_data;
	int vf_num;

	if (sscanf(buf, "%d %d", &op_num, &dma_num) == 0)
		return -EINVAL;
	switch (op_num) {
	case 0:
		exynos_v920_pcie_ep_register_link_dump(pcie_ep);
		break;
	case 1:
		dev_info(dev, "Check Link status\n");
		val = readl(pcie_ep->elbi_base + PCIE_LINK_DBG_2) &
			PCIE_LINK_LTSSM_MASK;
		dev_info(dev, "0x%x\n", val);
		break;
	case 2:
		if (sscanf(buf, "%d %d", &op_num, &dma_num) == 2) {
			if (pcie_ep->state != STATE_LINK_UP) {
				dev_info(dev, "current state is not link up\n");
				return -1;
			} else if (pcie_ep->use_sysmmu) {
				dev_err(dev, "Sysmmu is enabled\n");
				return -1;
			}
			dev_info(dev, "mem read\n");
			pcie_ep->work_cmd = WORK_MEM_RD;
			pcie_ep->dma_num = dma_num;

			queue_work(pcie_ep->pcie_ep_wq,
						&pcie_ep->test_work.work);
		} else {
			dev_warn(dev, "Command 2 argument invalid\n");
			return -EINVAL;
		}

		break;
	case 3:
		if (pcie_ep->state != STATE_LINK_UP) {
			dev_info(dev, "current state is not link up\n");
			return -1;
		} else if (pcie_ep->use_sysmmu) {
			dev_err(dev, "Sysmmu is enabled\n");
			return -1;
		}

		if (sscanf(buf, "%d %d %d %x", &op_num, &cnt,
						&dma_num, &size) == 4) {
			dev_info(dev, "count : %u , transfer size: 0x%x\n",
					cnt, size);

			if (size > MAX_TEST_WR_SIZE) {
				dev_info(dev,
				"Maximum write size exceeded, fix to %x\n",
					MAX_TEST_WR_SIZE);
				size = MAX_TEST_WR_SIZE;
			}

			pcie_ep->size = size;
			pcie_ep->cnt = cnt;
			pcie_ep->dma_num = dma_num;

			pcie_ep->work_cmd = WORK_MEM_WR;
			queue_work(pcie_ep->pcie_ep_wq,
					&pcie_ep->test_work.work);
		} else {
			dev_warn(dev, "Command 3 argument invalid\n");
			return -EINVAL;
		}
		break;
	case 4:
		if (sscanf(buf, "%d %x", &op_num, &msi_data) == 2) {
			if (pcie_ep->state != STATE_LINK_UP) {
				dev_info(dev, "current state is not link up\n");
				return -1;
			}

			dev_info(dev, "MSI test\n");
			pcie_ep->work_cmd = WORK_MSI;
			pcie_ep->msi_data = msi_data;
			queue_work(pcie_ep->pcie_ep_wq,
					&pcie_ep->test_work.work);
		} else {
			dev_warn(dev, "Command 4 argument invalid\n");
			return -EINVAL;
		}
		break;
	case 5:
		if (pcie_ep->state != STATE_LINK_UP) {
			dev_info(dev, "current state is not link up\n");
			return -1;
		} else if (pcie_ep->use_sysmmu) {
			dev_err(dev, "Sysmmu is enabled\n");
			return -1;
		}

		if (sscanf(buf, "%d %d %x", &op_num, &dma_num, &size) == 3) {
			dev_info(dev, "transfer size: 0x%x\n", size);

			if (size > MAX_TEST_WR_SIZE) {
				dev_info(dev,
				"Maximum write size exceeded, fix to %x\n",
					MAX_TEST_WR_SIZE);
					size = MAX_TEST_WR_SIZE;
			}

			pcie_ep->size = size;
			pcie_ep->dma_num = dma_num;

			dev_info(dev, "MEM WR/RD test\n");
			pcie_ep->work_cmd = WORK_MEM_WR_RD;
			queue_work(pcie_ep->pcie_ep_wq,
					&pcie_ep->test_work.work);
		} else {
			dev_warn(dev, "Command 5 argument invalid\n");
			return -EINVAL;
		}
		break;
	case 6:
		dev_info(dev, "Inbound test\n");
		exynos_v920_pcie_ep_test_bar(dev, pcie_ep);
		break;

	case 7:
		dev_info(dev, "Ready Establish test\n");
		exynos_v920_ep_ready_establish_link(pcie_ep);
		break;

	case 8:
		if (sscanf(buf, "%d %x %d", &op_num, &msi_data, &vf_num) == 3) {
			if (pcie_ep->state != STATE_LINK_UP) {
				dev_info(dev, "current state is not link up\n");
				return -1;
			}

			dev_info(dev, "MSIX test\n");
			pcie_ep->work_cmd = WORK_MSI_X;
			pcie_ep->msi_data = msi_data;
			pcie_ep->vf_num = vf_num;
			queue_work(pcie_ep->pcie_ep_wq,
					&pcie_ep->test_work.work);
		} else {
			dev_warn(dev, "Command 5 argument invalid\n");
			return -EINVAL;
		}
		break;

	default:
		dev_warn(dev, "Unsupported Test Number(%d)...\n", op_num);
		break;
	}
	return count;
}

static DEVICE_ATTR_RW(ep_dbg);
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Create sysfile for ep driver"
 * @logic "Create device file"
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @endparam
 * @retval{-, int, 0, int, not 0}
 */
static inline int create_pcie_ep_sys_file_v920(struct device *dev)
{
	return device_create_file(dev, &dev_attr_ep_dbg);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Remove sysfile for ep driver"
 * @logic "Remove device file"
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @endparam
 * @noret
 */
static inline void remove_pcie_ep_sys_file_v920(struct device *dev)
{
	device_remove_file(dev, &dev_attr_ep_dbg);
}

static const struct dw_pcie_ops dw_pcie_ep_ops = {
	.read_dbi = NULL,
	.write_dbi = NULL,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Request irqs for PCIe EP"
 * @logic "Request irqs for PCIe EP."
 * @params
 * @param{in/out, pcie_ep, struct ::exynos_ep_pcie *, not NULL}
 * @endparam
 * @retval{ret, int, 0, -ERANGE~0, <0}
 */
static int exynos_v920_pcie_ep_request_irq(struct exynos_ep_pcie *pcie_ep)
{
	struct platform_device *pdev = pcie_ep->pdev;
	int ret = 0;
	int i;

	/* request irq */
	pcie_ep->irq = platform_get_irq(pdev, 0);
	if (!pcie_ep->irq) {
		dev_err(&pdev->dev, "failed to get irq for PCIe EP\n");
		return -ENODEV;
	}

	ret = devm_request_irq(&pdev->dev, pcie_ep->irq,
			exynos_v920_pcie_ep_irq_handler, IRQF_SHARED,
			"exynos-pcie-ep", pcie_ep);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq for PCIe EP\n");
		return -ENODEV;
	}

	for (i  = 0; i < MAX_VF_NUM; i++) {
		/* request Doorbell irq */
		pcie_ep->db_irq[i] = platform_get_irq(pdev, i + 1);
		if (!pcie_ep->db_irq[i]) {
			dev_err(&pdev->dev,
				"failed to get db_irq for PCIe EP\n");
			return -ENODEV;
		}

		ret = devm_request_irq(&pdev->dev, pcie_ep->db_irq[i],
			exynos_v920_pcie_ep_doorbell_irq_handler, IRQF_SHARED,
			"exynos-pcie-ep", pcie_ep);
		if (ret) {
			dev_err(&pdev->dev,
				"failed to request db_irq for PCIe EP\n");
			return -ENODEV;
		}
	}

	for (i  = 0; i < MAX_VF_NUM; i++) {
		/* request DMA irq */
		pcie_ep->dma_irq[i] =
			platform_get_irq(pdev, i + 1 + MAX_VF_NUM);
		if (!pcie_ep->dma_irq[i]) {
			dev_err(&pdev->dev,
				"failed to get dma_irq for PCIe EP\n");
			return -ENODEV;
		}

		if (i == 0) {
			/* DMA Interrupt 0 */
			ret = devm_request_irq
				(&pdev->dev, pcie_ep->dma_irq[i],
				exynos_v920_pcie_ep_dma0_irq_handler,
				IRQF_SHARED, "exynos-pcie-ep", pcie_ep);
			if (ret) {
				dev_err(&pdev->dev,
					"failed to request");
				dev_err(&pdev->dev,
					"dma0_irq for PCIe EP\n");
				return -ENODEV;
			}
		} else if (i == 1) {
			/* DMA Interrupt 1 */
			ret = devm_request_irq
				(&pdev->dev, pcie_ep->dma_irq[i],
					exynos_v920_pcie_ep_dma1_irq_handler,
						IRQF_SHARED,
					"exynos-pcie-ep", pcie_ep);
			if (ret) {
				dev_err(&pdev->dev,
					"failed to request");
				dev_err(&pdev->dev,
					"dma1_irq for PCIe EP\n");
				return -ENODEV;
			}
		} else if (i == 2) {
			/* DMA Interrupt 2 */
			ret = devm_request_irq(&pdev->dev,
					pcie_ep->dma_irq[i],
					exynos_v920_pcie_ep_dma2_irq_handler,
					IRQF_SHARED,
					"exynos-pcie-ep", pcie_ep);
			if (ret) {
				dev_err(&pdev->dev,
					"failed to request");
				dev_err(&pdev->dev,
					"dma2_irq for PCIe EP\n");
				return -ENODEV;
			}
		} else if (i == 3) {
			/* DMA Interrupt 3 */
			ret = devm_request_irq(&pdev->dev,
					pcie_ep->dma_irq[i],
					exynos_v920_pcie_ep_dma3_irq_handler,
					IRQF_SHARED,
					"exynos-pcie-ep", pcie_ep);
			if (ret) {
				dev_err(&pdev->dev,
					"failed to request");
				dev_err(&pdev->dev,
					"dma3_irq for PCIe EP\n");
				return -ENODEV;
			}
		}
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_init}
 * @purpose "Probe PCIe EP driver"
 * @logic "Get data from dt; alloc phyinfo\n
 *	Initialize work queue\n
 *	Initialize dma completion\n
 *	Get clock and enable\n
 *	Set pinctrl\n
 *	Request irqs\n
 *	Create sysfs file\n
 *	Initialize phy and ep device."
 * @params
 * @param{in/out, pdev, struct ::platform_device *, not NULL}
 * @endparam
 * @retval{ret, int, 0, <=0, not 0}
 */
static int exynos_v920_pcie_ep_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci;
	struct device_node *np;
	struct exynos_ep_pcie *pcie_ep;
	struct exynos_ep_pcie_phyinfo *phyinfo;
	int ret = 0;
	unsigned int ch_num = 0;
	u32 val;
	const int *soc_variant;

	if (dev == NULL) {
		ret = -EINVAL;
		goto probe_fail;
	}

	np = pdev->dev.of_node;

	dev_info(&pdev->dev, "[%s] V920\n", __func__);

	/* get channel number from Device Tree */
	if (of_property_read_u32(np, "ch-num", &ch_num)) {
		dev_err(dev, "Failed to parse the channel number\n");
		return -EINVAL;
	}

	if (ch_num >= MAX_EP_NUM) {
		dev_err(dev, "ch_num exceeds boundary of g_pcie_ep_v920\n");
		return -EINVAL;
	}
	pcie_ep = &g_pcie_ep_v920[ch_num];
	pcie_ep->ch_num = ch_num;
	pcie_ep->pdev = pdev;

	pcie_ep->work_state = STATE_DMA_IDLE;

	/* Set Onother lane status -1 (for RC EP Cross) */
	if (pcie_ep->ch_num % CH_SHARE_PHY == 0)
		g_pcie_ep_v920[ch_num + 1].lane_status = STATE_LANE_INIT;
	else
		g_pcie_ep_v920[ch_num - 1].lane_status = STATE_LANE_INIT;

	pcie_ep->lane_status = STATE_LANE_INIT;

	pci = devm_kzalloc(dev, sizeof(*pci), GFP_KERNEL);
	if (pci == NULL) {
		ret = -ENOMEM;
		goto probe_fail;
	}
	pci->dev = dev;
	pcie_ep->pci = pci;
	pci->ops = &dw_pcie_ep_ops;

	soc_variant = (int *)of_device_get_match_data(dev);
	if (!soc_variant) {
		pr_err("failed to look up compatible string\n");
		return -EINVAL;
	}
	pcie_ep->soc_variant = *soc_variant;
	dev_info(pci->dev, "%s: Version: %s\n", __func__,
	(pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) ? "EVT2" : "EVT01");

	/* parsing pcie ep dts data for exynos */
	ret = exynos_v920_pcie_ep_parse_dt(pcie_ep);
	if (ret) {
		dev_err(dev, "DT Parsing failed\n");
		goto probe_fail;
	}

	ret = exynos_v920_pcie_ep_get_resource(pcie_ep);
	if (ret) {
		dev_err(dev, "getting resource for PCIe EP is failed\n");
		ret = -EINVAL;
		goto probe_fail;
	}

	/* parsing pcie ep dts data for exynos */
	ret = exynos_v920_pcie_ep_parse_dt(pcie_ep);
	if (ret)
		goto probe_fail;

	ret = exynos_v920_pcie_ep_parse_dt_bar(pcie_ep);
	if (ret)
		goto probe_fail;

	exynos_v920_pcie_ep_msi_init(pcie_ep);

	/* set phyinfo for PHY CAL */
	phyinfo = devm_kzalloc(dev, sizeof(struct exynos_ep_pcie_phyinfo),
			GFP_KERNEL);
	if (!phyinfo)
		return -ENOMEM;

	if (pcie_ep->use_sysmmu) {
		samsung_sysmmu_activate(pci->dev);
		pm_runtime_enable(pci->dev);
		pm_runtime_get_sync(pci->dev);
	}

	phyinfo->phy_base = pcie_ep->phy_base;
	phyinfo->sysreg_base = pcie_ep->sysreg_base;
	phyinfo->elbi_base = pcie_ep->elbi_base;
	phyinfo->gen_subsys_base = pcie_ep->gen_subsys_base;
	phyinfo->e32_phy_base = pcie_ep->e32_phy_base;
	phyinfo->elbi_base_other = pcie_ep->elbi_base_other;
	phyinfo->dbi_base = pcie_ep->dbi_base;
	phyinfo->ch_num = ch_num;
	phyinfo->use_bifurcation = pcie_ep->use_bifurcation;
	phyinfo->lanes_num = pcie_ep->lanes_num;
	phyinfo->use_sris = pcie_ep->use_sris;
	phyinfo->elbi_cmn_base = pcie_ep->elbi_cmn_base;
	phyinfo->soc_variant = pcie_ep->soc_variant;

	if (phyinfo->soc_variant == EXYNOSAUTOV920_EVT2) {
		phyinfo->soc_ctrl_base = pcie_ep->soc_ctrl_base;
		phyinfo->pmu_base = pcie_ep->pmu_base;
	} else {
		phyinfo->cmu_base = pcie_ep->cmu_base;
	}

	pcie_ep->ep_phyinfo = phyinfo;

	/* create and initialize the workqueue for handling events*/
	pcie_ep->pcie_ep_wq = create_freezable_workqueue("pcie_ep_wq");
	if (IS_ERR(pcie_ep->pcie_ep_wq)) {
		dev_err(dev, "Creating workqueue ('pcie_ep_wq')");
		dev_err(dev, "is failed\n");
		ret = -EBUSY;
		goto probe_fail;
	}

	/* create and initialize the workqueue for exception handlers */
	pcie_ep->pcie_ep_wq_exception =
			create_freezable_workqueue("pcie_ep_wq_exception");
	if (IS_ERR(pcie_ep->pcie_ep_wq_exception)) {
		dev_err(dev, "Creating workqueue ('pcie_ep_wq_exception')");
		dev_err(dev, "is failed\n");
		ret = -EBUSY;
		goto probe_fail;
	}

	/* create and initialize the workqueue for dislink */
	pcie_ep->pcie_ep_wq_dislink =
			create_freezable_workqueue("pcie_ep_wq_dislink");
	if (IS_ERR(pcie_ep->pcie_ep_wq_dislink)) {
		dev_err(dev, "Creating workqueue ('pcie_ep_wq_dislink')");
		dev_err(dev, "is failed\n");
		ret = -EBUSY;
		goto probe_fail;
	}

	INIT_DELAYED_WORK(&pcie_ep->irq1_evt_work,
			exynos_v920_pcie_ep_irq1_handler);

	INIT_DELAYED_WORK(&pcie_ep->test_work,
			exynos_v920_pcie_ep_work_cmd);

	INIT_DELAYED_WORK(&pcie_ep->exception_work,
			exynos_v920_pcie_ep_work_exception);

	INIT_DELAYED_WORK(&pcie_ep->dislink_work,
			exynos_v920_pcie_ep_work_dislink);

	/* for eDMA test */
	init_completion(&pcie_ep->wdma0);
	init_completion(&pcie_ep->wdma1);
	init_completion(&pcie_ep->wdma2);
	init_completion(&pcie_ep->wdma3);

	init_completion(&pcie_ep->rdma0);
	init_completion(&pcie_ep->rdma1);
	init_completion(&pcie_ep->rdma2);
	init_completion(&pcie_ep->rdma3);

	pcie_ep->dma_wr_buf = NULL;
	pcie_ep->dma_rd_buf = NULL;

	/* Clock get & enable */
	ret = exynos_v920_pcie_ep_clk_get(pcie_ep);
	if (ret) {
		dev_err(dev, "getting clocks for PCIe EP is failed\n");
		ret = -EINVAL;
		goto probe_fail;
	}
	exynos_v920_pcie_ep_clk_enable(pcie_ep, 1);
	exynos_v920_pcie_ep_phyclk_enable(pcie_ep, 1);

	/* PREST pin set to function mode -> should get pin info. from DT */
	pcie_ep->perst_gpio = of_get_gpio(np, 0);

	if (pcie_ep->perst_gpio < 0) {
		dev_info(pci->dev, "cannot get perst_gpio\n");
	} else {
		/* set perst gpio to output mode low */
		ret = devm_gpio_request_one(pci->dev, pcie_ep->perst_gpio,
				GPIOF_IN,
				dev_name(pci->dev));
		if (ret)
			return ret;
	}

	pcie_ep->pcie_ep_pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(pcie_ep->pcie_ep_pinctrl)) {
		dev_err(&pdev->dev, "Can't get PCIe EP pinctrl\n");
		ret = -EINVAL;
		goto probe_fail;
	}
	pcie_ep->pin_perst =
		pinctrl_lookup_state(pcie_ep->pcie_ep_pinctrl, "perst_in");
	if (IS_ERR(pcie_ep->pin_perst)) {
		dev_err(&pdev->dev,
			"Can't set PERST pin to function mode for PCIe EP\n");
		ret = -EINVAL;
		goto probe_fail;
	}
	pinctrl_select_state(pcie_ep->pcie_ep_pinctrl,
			pcie_ep->pin_perst);

	pcie_ep->pin_clkreq =
		pinctrl_lookup_state(pcie_ep->pcie_ep_pinctrl, "clkreq");
	if (IS_ERR(pcie_ep->pin_clkreq)) {
		dev_err(&pdev->dev,
			"Can't set CLKREQ pin to function mode for PCIe EP\n");
		ret = -EINVAL;
		goto probe_fail;
	}

	pinctrl_select_state(pcie_ep->pcie_ep_pinctrl,
			pcie_ep->pin_clkreq);

	ret = exynos_v920_pcie_ep_request_irq(pcie_ep);
	if (ret)
		goto probe_fail;

	/* create sysfs for debugging */
	if (create_pcie_ep_sys_file_v920(&pdev->dev))
		dev_warn(&pdev->dev, "Failed to create pcie sys file\n");

	/* phy init : mapping phy function */
	exynos_v920_pcie_ep_phyinit(pcie_ep);

	/* initialize designware PCIe EP => mainline i/f should be added */

	if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT01) {
		if (pcie_ep->ch_num == CH0_4L)
			pcie_ep->sysreg_offset = HSI0_PCIE_GEN5_4LA_PHY_CTRL;
		else if (pcie_ep->ch_num == CH1_4L)
			pcie_ep->sysreg_offset = HSI0_PCIE_GEN5_4LB_PHY_CTRL;
		else if (pcie_ep->ch_num == CH2_2L)
			pcie_ep->sysreg_offset = HSI0_PCIE_GEN5_2LA_PHY_CTRL;
		else if (pcie_ep->ch_num == CH3_2L)
			pcie_ep->sysreg_offset = HSI0_PCIE_GEN5_2LB_PHY_CTRL;

		val = readl(pcie_ep->sysreg_base + pcie_ep->sysreg_offset);
		val = val & ~HSI0_PCIE_IP_CTRL_PERST_MASK;
		val = val | HSI0_PCIE_IP_CTRL_PERST_INIT;
		writel(val, (pcie_ep->sysreg_base  + pcie_ep->sysreg_offset));
	}
	exynos_v920_ep_ready_establish_link(pcie_ep);

	if (pcie_ep->use_sharability) {
		val = readl(pcie_ep->sysreg_base +
			PCIE_EP_SYSREG_SHARABILITY_CTRL +
			PCIE_EP_SYSREG_SHARABLE_RW_OFFSET +
			PCIE_EP_SYSREG_SHARABLE_CH_OFFSET * pcie_ep->ch_num);
		val |= PCIE_EP_SYSREG_SHARABLE_RW_ENABLE;
		writel(val, pcie_ep->sysreg_base +
			PCIE_EP_SYSREG_SHARABILITY_CTRL +
			PCIE_EP_SYSREG_SHARABLE_RW_OFFSET +
			PCIE_EP_SYSREG_SHARABLE_CH_OFFSET * pcie_ep->ch_num);
	}

	/* set drv data */
	platform_set_drvdata(pdev, pcie_ep);

#ifdef CONFIG_S2MPU
	if (pcie_ep->s2mpu_base) {
		if (hyp_call_s2mpu_vgen_v9_protection(
			pcie_ep->s2mpu_base, pcie_ep->vgen_base) != 0)
			dev_info(&pdev->dev, "%s Fail s2mpu v9=%08x\n",
				 __func__, pcie_ep->s2mpu_base);
		else
			dev_info(&pdev->dev, "%s Success s2mpu v9=%08x\n",
				__func__, pcie_ep->s2mpu_base);
	} else {
		dev_info(&pdev->dev, "%s s2mpu is disabled [s2mpu=%08x]\n",
			__func__, pcie_ep->s2mpu_base);
	}
#endif

probe_fail:
	if (ret)
		dev_err(&pdev->dev, "%s: pcie probe failed\n", __func__);
	else
		dev_info(&pdev->dev, "%s: pcie probe success\n", __func__);

	return ret;
}

/* PCIe link status checking function */
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Check ep link status"
 * @logic "Check ep link status"
 * @params
 * @param{in, ch_num, int, 0~5}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
int exynos_v920_check_pcie_ep_link_status(int ch_num)
{
	return 0;
}
EXPORT_SYMBOL(exynos_v920_check_pcie_ep_link_status);

static void exynos_pcie_ep_atu_set(struct dw_pcie_ep *ep, u8 atu_dir,
		int type, u16 vf_num, u64 cpu_addr, u64 pci_addr, u32 size)
{
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);
	u32 val;

	dev_info(pci->dev, "[%s] ob : %d\n", __func__, atu_dir);

	if (vf_num > 0)
		dev_info(pci->dev, "[%s] VF num : %d\n",
			__func__, vf_num);

	if (atu_dir == ATU_OB) {
		dw_pcie_writel_dbi(pci, PCIE_ATU_LOWER_BASE_OUT0,
				lower_32_bits(cpu_addr));
		dw_pcie_writel_dbi(pci, PCIE_ATU_UPPER_BASE_OUT0,
				upper_32_bits(cpu_addr));
		dw_pcie_writel_dbi(pci, PCIE_ATU_LIMIT_OUT0,
				lower_32_bits(cpu_addr + size - 1));
		dw_pcie_writel_dbi(pci, PCIE_ATU_LOWER_TARGET_OUT0,
				lower_32_bits(pci_addr));
		dw_pcie_writel_dbi(pci, PCIE_ATU_UPPER_TARGET_OUT0,
				upper_32_bits(pci_addr));
		dw_pcie_writel_dbi(pci, PCIE_ATU_CR1_OUT0, type);
		dw_pcie_writel_dbi(pci, PCIE_ATU_CR2_OUT0, PCIE_ATU_ENABLE);

		/*
		 * Make sure ATU enable takes effect
		 * before any subsequent config and I/O accesses.
		 */
		val = dw_pcie_readl_dbi(pci, PCIE_ATU_LIMIT_OUT0);
		dev_err(pci->dev, "PCIE_ATU_LIMIT : 0x%x\n", val);

		val = dw_pcie_readl_dbi(pci, PCIE_ATU_LOWER_TARGET_OUT0);
		dev_err(pci->dev, "PCIE_ATU_LOWER_TARGET : 0x%x\n", val);

		val = dw_pcie_readl_dbi(pci, PCIE_ATU_UPPER_TARGET_OUT0);
		dev_err(pci->dev, "PCIE_ATU_UPPER_TARGET : 0x%x\n", val);

		val = dw_pcie_readl_dbi(pci, PCIE_ATU_LOWER_BASE_OUT0);
		dev_err(pci->dev, "PCIE_ATU_LOWER_BASE : 0x%x\n", val);

		val = dw_pcie_readl_dbi(pci, PCIE_ATU_UPPER_BASE_OUT0);
		dev_err(pci->dev, "PCIE_ATU_UPPER_BASE : 0x%x\n", val);

		val = dw_pcie_readl_dbi(pci, PCIE_ATU_CR1_OUT0);
		dev_err(pci->dev, "PCIE_ATU_CR1 : 0x%x\n", val);

		val = dw_pcie_readl_dbi(pci, PCIE_ATU_CR2_OUT0);
		dev_err(pci->dev, "PCIE_ATU_CR2 : 0x%x\n", val);

		if (val & PCIE_ATU_ENABLE)
			return;

		dev_err(pci->dev, "outbound iATU is not being enabled\n");
	} else if (atu_dir == ATU_IB)  {
		dw_pcie_writel_dbi(pci, PCIE_ATU_LOWER_TARGET_IN0,
						lower_32_bits(cpu_addr));
		dw_pcie_writel_dbi(pci, PCIE_ATU_UPPER_TARGET_IN0,
						upper_32_bits(cpu_addr));
		dw_pcie_writel_dbi(pci, PCIE_ATU_CR1_IN0, type);
		dw_pcie_writel_dbi(pci, PCIE_ATU_CR2_IN0, PCIE_ATU_ENABLE
			   | PCIE_ATU_BAR_MODE_ENABLE | (0 << 8));

		/*
		 * Make sure ATU enable takes effect before
		 * any subsequent config and I/O accesses.
		 */
		val = dw_pcie_readl_dbi(pci, PCIE_ATU_CR2_IN0);
		if (val & PCIE_ATU_ENABLE)
			return;

		dev_err(pci->dev, "inbound iATU is not being enabled\n");
	}
}

static void exynos_v920_pcie_ep_send_evt(struct exynos_ep_pcie *pcie_ep,
					 u16 evt)
{
	struct exynos_ep_ext_info *ext = NULL;
	struct exynos_ep_ext_dd_ops *ext_ops = NULL;
	struct device *dev;
	int i;

	dev = pcie_ep->pci->dev;

	for (i = 0 ; i < MAX_VF_NUM + 1 ; i++) {
		ext = pcie_ep->ext_node[i].ext;
		ext_ops = pcie_ep->ext_node[i].ext_ops;

		if (ext_ops && ext_ops->event_cb) {
			ext_ops->event_cb(ext, evt);
			dev_info(dev, "%s ch%d vf%d evt(%d)\n",
				 __func__, ext->ch_num, i, evt);
		}
	}
}

struct device *exynos_v920_pcie_ep_get_dev(u8 ch_num)
{
	struct exynos_ep_pcie *pcie_ep = &g_pcie_ep_v920[ch_num];
	struct dw_pcie *pci = NULL;

	if (pcie_ep->pci == NULL)
		return NULL;

	pci = pcie_ep->pci;

	return pci->dev;
}
EXPORT_SYMBOL(exynos_v920_pcie_ep_get_dev);

static int exynos_v920_pcie_ep_send_msi(u8 ch_num, u16 vf_num, u8 bar_num,
					u32 data)
{
	int ret = 0;
	struct exynos_ep_pcie *pcie_ep = &g_pcie_ep_v920[ch_num];

	ret = exynos_v920_pcie_ep_msi(pcie_ep, data);

	return ret;
}

static int exynos_v920_pcie_ep_set_obatu(u8 ch_num, u16 vf_num, u8 bar_num,
					 dma_addr_t remote_dma_addr,
					 size_t size, phys_addr_t *ob_phy_addr,
					 void __iomem *ob_dest_vaddr)
{
	struct exynos_ep_pcie *pcie_ep = &g_pcie_ep_v920[ch_num];
	struct platform_device *pdev = pcie_ep->pdev;
	struct device *dev = &pdev->dev;
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;
	int ret = 0;

	ob_dest_vaddr = pci_epc_mem_alloc_addr(epc, ob_phy_addr, size);
	if (!ob_dest_vaddr) {
		dev_err(dev, "failed to allocate address\n");
		ret = -ENOMEM;
		goto err;
	}

	if (vf_num == 0) {
		/*	In PF case, use DW stack. */
		ret = exynos_v920_pcie_ep_epc_map_addr(epc, 0, 0, *ob_phy_addr,
						       remote_dma_addr, size);
		if (ret) {
			dev_err(dev, "failed to map address\n");
			ret = -ENOMEM;
			goto err_addr;
		}
	} else {
		/*	In VF case, use Exynos API */
		exynos_pcie_ep_atu_set(ep, ATU_OB, PCIE_ATU_TYPE_MEM, vf_num,
				       *ob_phy_addr, remote_dma_addr, size);
	}

	return 0;

err_addr:
	pci_epc_unmap_addr(epc, 0, 0, *ob_phy_addr);

err:
	pci_epc_mem_free_addr(epc, *ob_phy_addr, ob_dest_vaddr, size);

	return ret;
}

static void exynos_v920_pcie_ep_reset_obatu(u8 ch_num, u16 vf_num, u8 bar_num,
					    size_t size,
					    phys_addr_t ob_phy_addr,
					    void __iomem *ob_dest_vaddr)
{
	struct exynos_ep_pcie *pcie_ep = &g_pcie_ep_v920[ch_num];
	struct dw_pcie *pci = pcie_ep->pci;
	struct dw_pcie_ep *ep = &pci->ep;
	struct pci_epc *epc = ep->epc;

	pci_epc_unmap_addr(epc, 0, 0, ob_phy_addr);

	pci_epc_mem_free_addr(epc, ob_phy_addr, ob_dest_vaddr, size);
}

static int exynos_v920_pcie_ep_edma_read_once(u8 ch_num, u16 vf_num, u8 dma_num,
					      phys_addr_t remote_src_addr,
					      dma_addr_t local_dest_addr,
					      size_t size, int ll_start,
					      int ll_cnt)
{
	struct exynos_ep_pcie *pcie_ep = &g_pcie_ep_v920[ch_num];
#if DMA_LOG_PRINT_EP
	struct platform_device *pdev = pcie_ep->pdev;
#endif
	int ret = 0;

	pcie_ep->rd_ll_start = ll_start;
	pcie_ep->rd_ll_cnt = ll_cnt;

#if	DMA_LOG_PRINT_EP
	dev_info(&pdev->dev,
		"%s ch%d dma read once on [%llx -> %llx] size:%lx\n",
		__func__, ch_num, local_dest_addr, remote_src_addr, size);
#endif

	pcie_ep->work_state = STATE_DMA_BUSY;

#if DMA_CONT_MODE_EP
	ret = exynos_v920_pcie_ep_read(pcie_ep, remote_src_addr,
					local_dest_addr, size, dma_num);
#else
	ret = exynos_v920_pcie_ep_read_ll(pcie_ep, remote_src_addr,
					local_dest_addr, size, dma_num);
#endif

	pcie_ep->work_state = STATE_DMA_IDLE;

	return ret;
}

static int exynos_v920_pcie_ep_edma_write_once(u8 ch_num, u16 vf_num,
					       u8 dma_num,
					       dma_addr_t local_src_addr,
					       phys_addr_t remote_dest_addr,
					       size_t size, int ll_start,
					       int ll_cnt)
{
	struct exynos_ep_pcie *pcie_ep = &g_pcie_ep_v920[ch_num];
#if DMA_LOG_PRINT_EP
	struct platform_device *pdev = pcie_ep->pdev;
#endif
	int ret = 0;

	pcie_ep->wr_ll_start = ll_start;
	pcie_ep->wr_ll_cnt = ll_cnt;

#if	DMA_LOG_PRINT_EP
	dev_info(&pdev->dev,
		"%s ch%d dma write once on [%llx -> %llx] size:%lx\n",
		__func__, ch_num, local_src_addr, remote_dest_addr, size);
#endif

	pcie_ep->work_state = STATE_DMA_BUSY;

#if DMA_CONT_MODE_EP
	ret = exynos_v920_pcie_ep_write(pcie_ep, remote_dest_addr,
		local_src_addr, size, dma_num);
#else
	ret = exynos_v920_pcie_ep_write_ll(pcie_ep, remote_dest_addr,
		local_src_addr, size, dma_num);
#endif

	pcie_ep->work_state = STATE_DMA_IDLE;

	return ret;
}

static int exynos_v920_pcie_ep_msi_alloc_map(u8 ch_num)
{
	int ret = 0;
	struct exynos_ep_pcie *pcie_ep = &g_pcie_ep_v920[ch_num];
	struct device *dev;

	dev = pcie_ep->pci->dev;

	exynos_v920_pcie_ep_msi_free(pcie_ep);
	ret = exynos_v920_pcie_ep_msi_alloc(pcie_ep);
	if (ret)
		dev_err(dev, "%s failed to alloc and map msi(%d)\n",
			__func__, ret);

	return ret;
}

int ext_dd_ep_register(struct exynos_ep_ext_info *ext)
{
	struct exynos_ep_pcie *pcie_ep = NULL;
	struct exynos_ep_ext_node *epextnode = NULL;

	if (ext == NULL) {
		dev_err(NULL, "%s ERR ext:NULL\n", __func__);
		return -EINVAL;
	}

	if (ext->ch_num >= MAX_EP_NUM) {
		dev_err(NULL, "%s ERR ch%d:Invalid\n", __func__, ext->ch_num);
		return -EINVAL;
	}
	pcie_ep = &g_pcie_ep_v920[ext->ch_num];

	if (ext->vf_num >= (MAX_VF_NUM + 1)) {
		dev_err(NULL, "%s ERR vf%d:Invalid\n", __func__, ext->vf_num);
		return -EINVAL;
	}
	epextnode = &pcie_ep->ext_node[ext->vf_num];

	if (epextnode->is_activated == false) {
		epextnode->is_activated = true;
		epextnode->ch_num = ext->ch_num;
		epextnode->vf_num = ext->vf_num;
		epextnode->ext = ext;
		epextnode->ext_ops = &ext->ext_ops;

		ext->dd_ops.get_dev = exynos_v920_pcie_ep_get_dev;
		ext->dd_ops.send_msi = exynos_v920_pcie_ep_send_msi;
		ext->dd_ops.set_obatu = exynos_v920_pcie_ep_set_obatu;
		ext->dd_ops.reset_obatu = exynos_v920_pcie_ep_reset_obatu;
		ext->dd_ops.edma_read_once = exynos_v920_pcie_ep_edma_read_once;
		ext->dd_ops.edma_write_once =
			exynos_v920_pcie_ep_edma_write_once;
		ext->dd_ops.msi_alloc = exynos_v920_pcie_ep_msi_alloc_map;
		dev_info(NULL, "%s CH%d VF%d Done\n", __func__, ext->ch_num,
			 ext->vf_num);
	} else {
		dev_err(NULL, "%s ERR CH%d VF%d:enabled already\n", __func__,
			ext->ch_num, ext->vf_num);
		return -EINVAL;
	}

	return 0;
}
EXPORT_SYMBOL(ext_dd_ep_register);

void ext_dd_ep_unregister(struct exynos_ep_ext_info *ext)
{
	struct exynos_ep_pcie *pcie_ep = NULL;
	struct exynos_ep_ext_node *epextnode = NULL;
	struct device *dev = NULL;

	if (ext == NULL) {
		dev_err(NULL, "%s ERR ext:NULL\n", __func__);
		return;
	}

	if (ext->ch_num >= MAX_EP_NUM) {
		dev_err(NULL, "%s ERR ch%d:Invalid\n", __func__, ext->ch_num);
		return;
	}
	pcie_ep = &g_pcie_ep_v920[ext->ch_num];

	if (pcie_ep->pci != NULL)
		dev = pcie_ep->pci->dev;

	if (ext->vf_num >= (MAX_VF_NUM + 1)) {
		dev_err(dev, "%s ERR vf(%d):invalid\n", __func__, ext->vf_num);
		return;
	}
	epextnode = &pcie_ep->ext_node[ext->vf_num];

	if (epextnode->is_activated == false) {
		dev_err(dev, "%s ch%d vf%d no ext_node\n", __func__,
			ext->ch_num, ext->vf_num);
		return;
	}

	if (ext->ch_num == epextnode->ch_num) {
		if (ext->vf_num == epextnode->vf_num) {
			epextnode->is_activated = false;
			epextnode->ext_ops = NULL;
			epextnode->ext = NULL;
			dev_info(dev, "%s ch%d vf%d: unregistered\n", __func__,
				ext->ch_num, ext->vf_num);
		} else {
			dev_err(dev, "%s ch%d vf%d: vf%d mismatch\n", __func__,
				ext->ch_num, ext->vf_num, epextnode->vf_num);
		}
	} else {
		dev_err(dev, "%s ch%d vf%d: ch%d mismatch\n", __func__,
			ext->ch_num, ext->vf_num, epextnode->ch_num);
	}

}
EXPORT_SYMBOL(ext_dd_ep_unregister);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Remove PCIe EP"
 * @logic "Remove PCIe EP"
 * @params
 * @param{in, pdev, struct ::platform_device *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int __exit exynos_v920_pcie_ep_remove(struct platform_device *pdev)
{
	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Shutdown PCIe EP"
 * @logic "Do nothing"
 * @params
 * @param{in, pdev, struct ::platform_device *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_ep_shutdown(struct platform_device *pdev)
{

}

static const struct of_device_id exynos_pcie_ep_of_match[] = {
	{ .compatible = "samsung,exynos-v920-evt01-pcie-ep",
		.data = &exynosautov920_evt01 },
	{ .compatible = "samsung,exynos-v920-evt2-pcie-ep",
		.data = &exynosautov920_evt2 },
	{},
};
MODULE_DEVICE_TABLE(of, exynos_pcie_ep_of_match);

#ifdef CONFIG_PM
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Suspend PCIe ep without irq"
 * @logic ""
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @endparam
 * @retval{-, int, -, 0, not 0}
 */
static int exynos_v920_pcie_ep_suspend_noirq(struct device *dev)
{
	struct exynos_ep_pcie *pcie_ep = dev_get_drvdata(dev);
	int ret_hwacg = 0;
	u32 val = 0x0;
	u32 sysreg_offset = 0x0;

	dev_info(dev, "PCIe EP suspend no irq\n");

	if (pcie_ep->state != STATE_LINK_DOWN) {
		/* lane status off */
		pcie_ep->lane_status = STATE_LANE_DOWN;

		if (pcie_ep->use_bifurcation)
			exynos_v920_update_ep_lane_status(pcie_ep);

		/* Link stop act */
		pcie_ep->state = STATE_LINK_DOWN_TRY;

		/* LTSSM disable */
		exynos_v920_pcie_ep_set_ltssm(pcie_ep, 0);

		pcie_ep->linkup_ready = false;

		pcie_ep->state = STATE_LINK_DOWN_TRY;

		/* phy all power down */
		if (pcie_ep->phy_ops.phy_all_pwrdn != NULL)
			pcie_ep->phy_ops.phy_all_pwrdn(pcie_ep->ep_phyinfo);

		pcie_ep->state = STATE_LINK_DOWN;
		exynos_v920_pcie_ep_send_evt(pcie_ep, STATE_LINK_DOWN);
	} else {
		/* LTSSM disable */
		exynos_v920_pcie_ep_set_ltssm(pcie_ep, 0);

		/* For S2R enter in EVT0/1, Phy clock turn off before suspend */
		if (pcie_ep->phy_ops.phy_all_pwrdn != NULL)
			pcie_ep->phy_ops.phy_all_pwrdn(pcie_ep->ep_phyinfo);
	}

	if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) {
		/*
		 * PERST is connected to CMU and make a problem with HWACG
		 * in EP mode when PERST is input mode.
		 * PCIe EP can't do suspend, if PCIe link is down.
		 * So we added this PERST override codes to fix this issue.
		 */
		exynos_v920_pcie_ep_setup_interrupt(pcie_ep, false);

		if (pcie_ep->ch_num == CH0_4L)
			sysreg_offset = HSI0_PCIE_GEN5_4LA_PHY_CTRL;
		else if (pcie_ep->ch_num == CH1_4L)
			sysreg_offset = HSI0_PCIE_GEN5_4LB_PHY_CTRL;
		else if (pcie_ep->ch_num == CH2_2L)
			sysreg_offset = HSI0_PCIE_GEN5_2LA_PHY_CTRL;
		else if (pcie_ep->ch_num == CH3_2L)
			sysreg_offset = HSI0_PCIE_GEN5_2LB_PHY_CTRL;

		val = readl(pcie_ep->sysreg_base + sysreg_offset);
		val = val & ~HSI0_PCIE_IP_CTRL_PERST_PAD;
		val = val | HSI0_PCIE_IP_CTRL_PERST_OVERRIDE;
		writel(val, (pcie_ep->sysreg_base + sysreg_offset));
		dev_info(dev, "%s PERST override as 1 PCI%d off(0x%x):0x%x\n",
			 __func__, pcie_ep->ch_num, sysreg_offset, val);

		ret_hwacg = exynos_v920_pcie_ep_dbiclk_enable(dev,
							      &pcie_ep->clks,
							      false);
		if (ret_hwacg)
			dev_err(dev, "%s Fail to disable dbi_clk(%d)\n",
				__func__, ret_hwacg);
		else
			dev_info(dev, "%s Success to disable dbi_clk\n",
				 __func__);
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_internal}
 * @purpose "Resume PCIe ep without irq"
 * @logic "call exynos_v920_ep_ready_establish_link"
 * @params
 * @param{out, dev, struct ::device *, not NULL}
 * @endparam
 * @retval{Name, int, -, 0, not 0}
 */
static int exynos_v920_pcie_ep_resume_noirq(struct device *dev)
{
	struct exynos_ep_pcie *pcie_ep = dev_get_drvdata(dev);
	int ret_hwacg = 0;
	u32 val = 0x0;
	u32 sysreg_offset = 0x0;

	dev_info(dev, "PCIe EP resume no irq\n");

	if (pcie_ep->soc_variant == EXYNOSAUTOV920_EVT2) {
		ret_hwacg = exynos_v920_pcie_ep_dbiclk_enable(dev,
							      &pcie_ep->clks,
							      true);
		if (ret_hwacg)
			dev_err(dev, "%s Fail to enable dbi_clk(%d)\n",
				__func__, ret_hwacg);
		else
			dev_info(dev, "%s Success to enable dbi_clk\n",
				 __func__);

		/*
		 * PERST is connected to CMU and make a problem with HWACG
		 * in EP mode when PERST is input mode.
		 * If suspend is failed, we need to make PERST as PAD in resume.
		 */
		if (pcie_ep->ch_num == CH0_4L)
			sysreg_offset = HSI0_PCIE_GEN5_4LA_PHY_CTRL;
		else if (pcie_ep->ch_num == CH1_4L)
			sysreg_offset = HSI0_PCIE_GEN5_4LB_PHY_CTRL;
		else if (pcie_ep->ch_num == CH2_2L)
			sysreg_offset = HSI0_PCIE_GEN5_2LA_PHY_CTRL;
		else if (pcie_ep->ch_num == CH3_2L)
			sysreg_offset = HSI0_PCIE_GEN5_2LB_PHY_CTRL;

		val = readl(pcie_ep->sysreg_base + sysreg_offset);
		val = val | HSI0_PCIE_IP_CTRL_PERST_PAD;
		writel(val, (pcie_ep->sysreg_base + sysreg_offset));
		dev_info(dev, "%s PERST PAD PCI%d off(0x%x):0x%x\n",
			 __func__, pcie_ep->ch_num, sysreg_offset, val);

		exynos_v920_ep_ready_establish_link(pcie_ep);
	} else {
		if (pcie_ep->phy_ops.phy_all_pwrdn_clear != NULL)
			pcie_ep->phy_ops.phy_all_pwrdn_clear(pcie_ep->ep_phyinfo);
		pcie_ep->linkup_ready = 1;
		pcie_ep->state = STATE_LINK_UP_TRY;
		exynos_v920_pcie_ep_setup_interrupt(pcie_ep, true);
		exynos_v920_pcie_ep_link_start(pcie_ep);
	}

	return 0;
}
#else
#define exynos_v920_pcie_ep_suspend_noirq	NULL
#define exynos_v920_pcie_ep_resume_noirq	NULL
#endif

static const struct dev_pm_ops exynos_pcie_ep_dev_pm_ops = {
	.suspend_noirq	= exynos_v920_pcie_ep_suspend_noirq,
	.resume_noirq	= exynos_v920_pcie_ep_resume_noirq,
};

static struct platform_driver exynos_pcie_ep_driver = {
	.probe = exynos_v920_pcie_ep_probe,
	.remove = exynos_v920_pcie_ep_remove,
	.shutdown = exynos_v920_pcie_ep_shutdown,
	.driver = {
		.name	= "exynos-v920-pcie-ep",
		.of_match_table = exynos_pcie_ep_of_match,
		.pm	= &exynos_pcie_ep_dev_pm_ops,
	},
};

module_platform_driver(exynos_pcie_ep_driver);

MODULE_AUTHOR("Kyounghye Yun <k-hye.yun@samsung.com>");
MODULE_AUTHOR("Jiheon Oh <jiheon.oh@samsung.com>");
MODULE_DESCRIPTION("Samsung PCIe Endpoint controller driver");
MODULE_LICENSE("GPL v2");
