// SPDX-License-Identifier: GPL-2.0
/**
 * pcie-exynos-rc-auto-cal.c - Exynos PCIe RC cal
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Authors: Sanghoon Bae <sh86.bae@samsung.com>
 *	    Jiheon Oh <jiheon.oh@samsung.com>
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include "../pcie-designware.h"
#include "pcie-exynos-v920-rc-auto.h"
#include <soc/samsung/exynos-fw-sb.h>

void exynos_v920_host_pcie_others_set(struct pcie_port *pp);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_phypower}
 * @purpose "Power down phy"
 * @logic "Check state of link A/B\n
 *	Power down phy."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_phy_all_pwrdn(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	void __iomem *cmu_base_regs = exynos_pcie->cmu_base;

	writel(0x4, cmu_base_regs + 0x0);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_phypower}
 * @purpose "Power on phy"
 * @logic "Power on all phy"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_phy_all_pwrdn_clear(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	void __iomem *cmu_base_regs = exynos_pcie->cmu_base;

	writel(0x6, cmu_base_regs + 0x0);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_physet}
 * @purpose "Config PCIe RC phy for KITT2 Revision EVT2"
 * @logic "Reset global pcs\n
 *	Select RC\n
 *	Enable SRIS (if enabled)\n
 *	Config phy for each lane."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in/out, result, int, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_evt2_pcie_phy_config(struct pcie_port *pp, int *result, bool fw_update)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	u32 val;
	u32 phy_config_level = 1;
	int timeout;

	void __iomem *sysreg_base_regs = exynos_pcie->sysreg_base;
	void __iomem *elbi_base_regs = exynos_pcie->elbi_base;
	void __iomem *e32_phy_regs = exynos_pcie->e32_phy_base;
	void __iomem *gen_subsys_regs = exynos_pcie->gen_subsys_base;
	void __iomem *soc_ctrl_base_regs = exynos_pcie->soc_ctrl_base;
	void __iomem *pmu_base_regs = exynos_pcie->pmu_base;
	void __iomem *phy_base = exynos_pcie->phy_base;

	struct fw_context fw_ctx = {0, };
	int ret = 0;

	*result = 1;

	if (exynos_pcie->ch_num == 0 || exynos_pcie->ch_num == 1) {
		if ((readl(sysreg_base_regs + 0x670) & 0x1) == 0x1)
			phy_config_level = 0;

		if (phy_config_level)
			exynos_pcie->phy_fw_ver =  readl(phy_base + 0x660);
		else {
			/* PMU Isolation off */
			val = readl(pmu_base_regs + 0x700);
			writel(val | 0x1, pmu_base_regs + 0x700);

			/* phy test power on */
			val = readl(sysreg_base_regs + 0x670);
			writel(val & ~(0x1), sysreg_base_regs + 0x670);
		}
	} else if (exynos_pcie->ch_num == 2 || exynos_pcie->ch_num == 3) {
		if ((readl(sysreg_base_regs + 0x4) & 0x1) == 0x1)
			phy_config_level = 0;

		if (phy_config_level)
			exynos_pcie->phy_fw_ver =  readl(phy_base + 0x660);
		else {
			/* In 2L phy, 4L phy pmu should be isolation off. */
			val = readl(pmu_base_regs + 0x700);
			if (val != 0x1)
				writel(val | 0x1, pmu_base_regs + 0x700);

			/* PMU Isolation off */
			val = readl(pmu_base_regs + 0x704);
			writel(val | 0x1, pmu_base_regs + 0x704);

			/* phy test power on */
			val = readl(sysreg_base_regs + 0x4);
			writel(val & ~(0x1), sysreg_base_regs + 0x4);
		}
	}

	val = readl(soc_ctrl_base_regs + 0x200);
	writel(0x000103f5, soc_ctrl_base_regs + 0x200);

	val = readl(soc_ctrl_base_regs + 0x204);
	writel(0x00030C00, soc_ctrl_base_regs + 0x204);

	if (exynos_pcie->ch_num == 0 || exynos_pcie->ch_num == 2) {
		/*	PE0 COLD reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | 0x1, gen_subsys_regs + 0x48);
		udelay(10);
		writel(val & ~(0x1), gen_subsys_regs + 0x48);
	} else if (exynos_pcie->ch_num == 1 || exynos_pcie->ch_num == 3) {
		/*	PE0 COLD reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | (0x1<<8), gen_subsys_regs + 0x48);
		udelay(10);
		writel(val & ~(0x1<<8), gen_subsys_regs + 0x48);
	}

	if (exynos_pcie->ch_num == 0) {
		val = readl(sysreg_base_regs + 0x828);
		writel(val | (0x4<<24), sysreg_base_regs + 0x828);
	} else if (exynos_pcie->ch_num == 1) {
		val = readl(sysreg_base_regs + 0x848);
		writel(val | (0x4<<25), sysreg_base_regs + 0x848);
	} else if (exynos_pcie->ch_num == 2) {
		val = readl(sysreg_base_regs + 0x868);
		writel(val | (0x4<<24), sysreg_base_regs + 0x868);
	} else if (exynos_pcie->ch_num == 3) {
		val = readl(sysreg_base_regs + 0x888);
		writel(val | (0x4<<25), sysreg_base_regs + 0x888);
	}

	if (phy_config_level == 0) {
		/*	PLL Setting */
		val = readl(sysreg_base_regs + 0x600);
		writel(val | (0x1<<8), sysreg_base_regs + 0x600);

		val = readl(sysreg_base_regs + 0x604);
		writel(val | (0x1), sysreg_base_regs + 0x604);

		val = readl(sysreg_base_regs + 0x608);
		writel(val | (0x1<<24), sysreg_base_regs + 0x608);

		/*	Clock buffer TX enable	*/
		val = readl(sysreg_base_regs + 0x620);
		writel(val | (0x1<<0), sysreg_base_regs + 0x620);
		val = readl(sysreg_base_regs + 0x630);
		writel(val | (0x1<<0), sysreg_base_regs + 0x630);
		val = readl(sysreg_base_regs + 0x640);
		writel(val | (0x1<<0), sysreg_base_regs + 0x640);
		val = readl(sysreg_base_regs + 0x650);
		writel(val | (0x1<<0), sysreg_base_regs + 0x650);

		if (exynos_pcie->use_bifurcation) {
			if (exynos_pcie->ch_num == 0 || exynos_pcie->ch_num == 1) {
				val = readl(sysreg_base_regs + 0x828);
				val = val | (1 << 28);
				writel(val, (sysreg_base_regs + 0x828));
			} else if (exynos_pcie->ch_num == 2 || exynos_pcie->ch_num == 3) {
				val = readl(sysreg_base_regs + 0x868);
				val = val | (1 << 28);
				writel(val, (sysreg_base_regs + 0x868));
			}
		}
	}

	if (exynos_pcie->use_sris) {
		val = readl(elbi_base_regs + 0x0);
		writel(val | (0x1<<8), elbi_base_regs + 0x0);
	}

	if (phy_config_level == 0) {
		//Input REFCLK setting
		//PHY0_REFA_CLK_SEL
		val = readl(e32_phy_regs + 0x10);
		writel(val & ~(0x3<<16), e32_phy_regs + 0x10);
		if (exynos_pcie->use_bifurcation)
			writel(val & (0x2<<16), e32_phy_regs + 0x10);

		//PHY0_REFB_CLK_SEL
		val = readl(e32_phy_regs + 0x10);
		writel(val & ~(0x3<<18), e32_phy_regs + 0x10);
		if (exynos_pcie->use_bifurcation)
			writel(val & (0x2<<18), e32_phy_regs + 0x10);

		val = readl(e32_phy_regs + 0x10);

		udelay(100);

		/*	PHY resistor tune request	*/
		writel(0x10000001, e32_phy_regs);
		/*	PHY External Tx RoPLL PostDiv Control */
		writel(0x1249, e32_phy_regs + 0x1A8);
		/*	PHY External TX RoPLL PostDiv Override Enable	*/
		writel(0xF, e32_phy_regs + 0x1C4);
		/*	Lanepll Bypass Mode Control	*/
		writel(0x00, e32_phy_regs + 0x384);

		writel(0x000, e32_phy_regs + 0x384);

		if (exynos_pcie->use_bifurcation &&
			(exynos_pcie->ch_num == PCIE_CH1 || exynos_pcie->ch_num == PCIE_CH3))
			/*	Enable Reference Clock Detection for ref B	*/
			writel(0x061A0060, e32_phy_regs + 0x10);
		else
			/*	Enable Reference Clock Detection for refA/B	*/
			writel(0x06100060, e32_phy_regs + 0x10);
	}

	val = readl(elbi_base_regs + 0x0);
	writel(val | (0x1<<4), elbi_base_regs + 0x0);
	udelay(100);

	val = readl(elbi_base_regs + 0x0);
	writel(val | (0x1<<2), elbi_base_regs + 0x0);

	if (fw_update || (phy_config_level == 0)) {
		//only for 2l phy. S2R case. phy all reset for FW update
		/*	PE0 SW reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | (0x3<<1) | (0x3<<9), gen_subsys_regs + 0x48);
		udelay(10);

		if (fw_update) {
			/* phy contorl 10~12bit reset to dram->sram copy */
			val = readl(e32_phy_regs + 0x10);
			writel(val & ~(0x7<<10), e32_phy_regs + 0x10);
		}

		writel(val & ~(0x3<<1), gen_subsys_regs + 0x48);
		udelay(10);
		writel(val & ~(0x3<<9), gen_subsys_regs + 0x48);
		udelay(10);
	} else if (exynos_pcie->ch_num == 0 || exynos_pcie->ch_num == 2) {
		/*	PE0 SW reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | (0x3<<1), gen_subsys_regs + 0x48);
		udelay(10);
		if (exynos_pcie->ch_num == 2 && exynos_pcie->phy_fw_ver == FW_VER_205) {
			/* To Avoid ROM->SRAM copy */
			val = readl(e32_phy_regs + 0x10);
			writel(val | (0x1<<12), e32_phy_regs + 0x10);
		}
		writel(val & ~(0x3<<1), gen_subsys_regs + 0x48);
	} else if (exynos_pcie->ch_num == 1 || exynos_pcie->ch_num == 3) {
		/*	PE0 SW reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | (0x3<<9), gen_subsys_regs + 0x48);
		udelay(10);
		if (exynos_pcie->ch_num == 3 && exynos_pcie->phy_fw_ver
			== FW_VER_205) {
			/* To Avoid ROM->SRAM copy */
			val = readl(e32_phy_regs + 0x10);
			writel(val | (0x1<<12), e32_phy_regs + 0x10);
		}
		writel(val & ~(0x3<<9), gen_subsys_regs + 0x48);
	}

	/*	PE1 SW reset */
	val = readl(gen_subsys_regs + 0x48);
	val = 0x808;
	writel(val, gen_subsys_regs + 0x48);
	udelay(10);

	if (phy_config_level == 0) {
		if (exynos_pcie->ch_num == 0 || exynos_pcie->ch_num == 1) {
			/*	Set SRAM bypass	*/
			val = readl(e32_phy_regs + 0x10);
			writel(val | (0x1<<10), e32_phy_regs + 0x10);
		} else if ((exynos_pcie->ch_num == 2 || exynos_pcie->ch_num == 3) &&
			exynos_pcie->phy_fw_ver != FW_VER_205 && fw_update == false) {
			/*	Set SRAM bypass	*/
			val = readl(e32_phy_regs + 0x10);
			writel(val | (0x1<<10), e32_phy_regs + 0x10);
		}

		timeout = 0;
		do {
			udelay(1);
			timeout++;
			if (timeout == 2000) {
				dev_info(pci->dev, "SRAM bypass FAIL\n");
				*result = 0;
				break;
			}
		} while (!(readl(e32_phy_regs + 0x10) >> 31));


		if (exynos_pcie->ch_num == PCIE_CH2 && fw_update == true) {
			fw_ctx.phys_addr = exynos_pcie->fw_paddr;
			fw_ctx.virt_addr = (unsigned long long) exynos_pcie->pcie_fw_buf;

			fw_ctx.size = exynos_pcie->phy_fw_size;
			fw_ctx.fw_type = PCIE_0;
			ret = snprintf(fw_ctx.name, FW_NAME_LEN - 1, "pcie_fw_0");
			if (ret <= 0) {
				dev_err(pci->dev, "FW NAME set fail\n");
				*result = 0;
				return;
			}

			ret = (int)fw_sb_verify(&fw_ctx);

			if (ret != RV_SUCCESS) {
				dev_err(pci->dev, "PCIe FW is failed to verify : %d\n", ret);
				*result = 0;
				return;
			}

			val = readl(phy_base + 0x404);
			writel(val | (0x1<<1), phy_base + 0x404);

			fw_ctx.fw_type = PCIE_1;
			ret = snprintf(fw_ctx.name, FW_NAME_LEN - 1, "pcie_fw_1");
			if (ret <= 0) {
				dev_err(pci->dev, "FW NAME set fail\n");
				*result = 0;
				return;
			}

			ret = (int)fw_sb_verify(&fw_ctx);

			if (ret != RV_SUCCESS) {
				dev_err(pci->dev, "PCIe FW is failed to verify : %d\n", ret);
				*result = 0;
				return;
			}
		}

		timeout = 0;
		/*	Set PHY*_SRAM_EXT_LD_DONE */
		val = readl(e32_phy_regs + 0x10);
		writel(val | (0x1<<11), e32_phy_regs + 0x10);

		do {
			udelay(1);
			timeout++;
			if (timeout == 2000) {
				val = readl(e32_phy_regs + 0x10);
				dev_info(pci->dev, "CDM init FAIL\n");
				*result = 0;
				break;
			}
		} while ((readl(elbi_base_regs + 0x304) >> 24) & 0x1);
		timeout = 0;
	}

	mdelay(20);

	exynos_pcie->phy_fw_ver =  readl(phy_base + 0x660);
	if (exynos_pcie->ch_num == PCIE_CH2 && exynos_pcie->phy_fw_ver == FW_VER_205)
		exynos_pcie->evt2_minor_ver = EVT20;
	else if (exynos_pcie->ch_num == PCIE_CH2 && exynos_pcie->phy_fw_ver == FW_VER_240)
		exynos_pcie->evt2_minor_ver = EVT21;

	dev_info(pci->dev, "Final FW Version %x\n", exynos_pcie->phy_fw_ver);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_physet}
 * @purpose "Config PCIe RC phy"
 * @logic "Reset global pcs\n
 *	Select RC\n
 *	Enable SRIS (if enabled)\n
 *	Config phy for each lane."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in/out, result, int, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_phy_config(struct pcie_port *pp, int *result, bool fw_update)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	u32 val;
	int pcie_bifur_sel_reg;
	u32 linkA, linkB, phy_config_level = 1;
	int timeout;

	void __iomem *sysreg_base_regs = exynos_pcie->sysreg_base;
	void __iomem *elbi_base_regs = exynos_pcie->elbi_base;
	void __iomem *e32_phy_regs = exynos_pcie->e32_phy_base;
	void __iomem *gen_subsys_regs = exynos_pcie->gen_subsys_base;

	*result = 1;

	pcie_bifur_sel_reg = exynos_pcie->ch_num % 2;
	if (pcie_bifur_sel_reg) {
		linkA = readl(exynos_pcie->elbi_base_other) & 0x1f;
		linkB = readl(elbi_base_regs + 0x304) & 0x1f;
	} else {
		linkA = readl(elbi_base_regs + 0x304) & 0x1f;
		linkB = readl(exynos_pcie->elbi_base_other) & 0x1f;
	}

	dev_info(pci->dev, "linkA: %x and linkB: %x\n", linkA, linkB);
	if ((linkA != 0x11 && linkB != 0x11) || (!exynos_pcie->use_bifurcation))
		phy_config_level = 0;

	if (exynos_pcie->ch_num == 0 || exynos_pcie->ch_num == 2) {
		/*	PE0 COLD reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | 0x1, gen_subsys_regs + 0x48);
		udelay(10);
		writel(val & ~(0x1), gen_subsys_regs + 0x48);
	} else if (exynos_pcie->ch_num == 1 || exynos_pcie->ch_num == 3) {
		/*	PE0 COLD reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | (0x1<<8), gen_subsys_regs + 0x48);
		udelay(10);
		writel(val & ~(0x1<<8), gen_subsys_regs + 0x48);
	}

	if (exynos_pcie->ch_num == 0) {
		val = readl(sysreg_base_regs + 0x828);
		writel(val | (0x4<<24), sysreg_base_regs + 0x828);
	} else if (exynos_pcie->ch_num == 1) {
		val = readl(sysreg_base_regs + 0x848);
		writel(val | (0x4<<25), sysreg_base_regs + 0x848);
	} else if (exynos_pcie->ch_num == 2) {
		val = readl(sysreg_base_regs + 0x868);
		writel(val | (0x4<<24), sysreg_base_regs + 0x868);
	} else if (exynos_pcie->ch_num == 3) {
		val = readl(sysreg_base_regs + 0x888);
		writel(val | (0x4<<25), sysreg_base_regs + 0x888);
	}

	if (phy_config_level == 0) {
		/*	PLL Setting */
		val = readl(sysreg_base_regs + 0x600);
		writel(val | (0x1<<8), sysreg_base_regs + 0x600);

		val = readl(sysreg_base_regs + 0x604);
		writel(val | (0x1), sysreg_base_regs + 0x604);

		val = readl(sysreg_base_regs + 0x608);
		writel(val | (0x1<<24), sysreg_base_regs + 0x608);

		/*	Clock buffer TX enable	*/
		val = readl(sysreg_base_regs + 0x620);
		writel(val | (0x1<<0), sysreg_base_regs + 0x620);
		val = readl(sysreg_base_regs + 0x630);
		writel(val | (0x1<<0), sysreg_base_regs + 0x630);
		val = readl(sysreg_base_regs + 0x640);
		writel(val | (0x1<<0), sysreg_base_regs + 0x640);
		val = readl(sysreg_base_regs + 0x650);
		writel(val | (0x1<<0), sysreg_base_regs + 0x650);
	}

	if (exynos_pcie->use_sris) {
		val = readl(elbi_base_regs + 0x0);
		writel(val | (0x1<<8), elbi_base_regs + 0x0);
	}

	if (phy_config_level == 0) {
		//Input REFCLK setting
		//PHY0_REFA_CLK_SEL
		val = readl(e32_phy_regs + 0x10);
		writel(val & ~(0x3<<16), e32_phy_regs + 0x10);
		if (exynos_pcie->use_bifurcation)
			writel(val & (0x2<<16), e32_phy_regs + 0x10);

		//PHY0_REFB_CLK_SEL
		val = readl(e32_phy_regs + 0x10);
		writel(val & ~(0x3<<18), e32_phy_regs + 0x10);
		if (exynos_pcie->use_bifurcation)
			writel(val & (0x2<<18), e32_phy_regs + 0x10);

		val = readl(e32_phy_regs + 0x10);

		udelay(100);

		/*	PHY resistor tune request	*/
		writel(0x10000001, e32_phy_regs);
		/*	PHY External Tx RoPLL PostDiv Control */
		writel(0x1249, e32_phy_regs + 0x1A8);
		/*	PHY External TX RoPLL PostDiv Override Enable	*/
		writel(0xF, e32_phy_regs + 0x1C4);
		/*	Lanepll Bypass Mode Control	*/
		writel(0x00, e32_phy_regs + 0x384);

		writel(0x000, e32_phy_regs + 0x384);

		if (exynos_pcie->use_bifurcation) {
			/*	Enable Reference Clock Detection for refA/B	*/
			writel(0x061A0060, e32_phy_regs + 0x10);
		} else {
			/*	Enable Reference Clock Detection for refA/B	*/
			writel(0x06100060, e32_phy_regs + 0x10);
		}
	}

	val = readl(elbi_base_regs + 0x0);
	writel(val | (0x1<<4), elbi_base_regs + 0x0);
	udelay(100);

	val = readl(elbi_base_regs + 0x0);
	writel(val | (0x1<<2), elbi_base_regs + 0x0);

	if (exynos_pcie->ch_num == 0 || exynos_pcie->ch_num == 2) {
		/*	PE0 SW reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | (0x3<<1), gen_subsys_regs + 0x48);
		udelay(10);
		writel(val & ~(0x3<<1), gen_subsys_regs + 0x48);
	} else if (exynos_pcie->ch_num == 1 || exynos_pcie->ch_num == 3) {
		/*	PE0 SW reset */
		val = readl(gen_subsys_regs + 0x48);
		writel(val | (0x3<<9), gen_subsys_regs + 0x48);
		udelay(10);
		writel(val & ~(0x3<<9), gen_subsys_regs + 0x48);
	}

	/*	PE1 SW reset */
	val = readl(gen_subsys_regs + 0x48);
	val = 0x808;
	writel(val, gen_subsys_regs + 0x48);
	udelay(10);

	if (phy_config_level == 0) {

		/*	Set SRAM bypass	*/
		val = readl(e32_phy_regs + 0x10);
		writel(val | (0x1<<10), e32_phy_regs + 0x10);

		timeout = 0;
		do {
			udelay(1);
			timeout++;
			if (timeout == 2000) {
				dev_info(pci->dev, "SRAM bypass FAIL\n");
				*result = 0;
				break;
			}
		} while (!(readl(e32_phy_regs + 0x10) >> 31));

		timeout = 0;
		/*	Set PHY*_SRAM_EXT_LD_DONE */
		val = readl(e32_phy_regs + 0x10);
		writel(val | (0x1<<11), e32_phy_regs + 0x10);

		do {
			udelay(1);
			timeout++;
			if (timeout == 2000) {
				val = readl(e32_phy_regs + 0x10);
				dev_info(pci->dev, "CDM init FAIL\n");
				*result = 0;
				break;
			}
		} while ((readl(elbi_base_regs + 0x304) >> 24) & 0x1);
		timeout = 0;
	}

	if (exynos_pcie->use_bifurcation &&
			(exynos_pcie->ch_num == 0 || exynos_pcie->ch_num == 2)) {
		if (exynos_pcie->ch_num == 0) {
			val = readl(sysreg_base_regs + 0x828);
			val = val | (1 << 28);
			writel(val, (sysreg_base_regs + 0x828));
		} else if (exynos_pcie->ch_num == 2) {
			val = readl(sysreg_base_regs + 0x868);
			val = val | (1 << 28);
			writel(val, (sysreg_base_regs + 0x868));
		}
	}

	mdelay(20);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_physet}
 * @purpose "Misc setting for phy"
 * @logic "Select REFCLK\n
 *	Enable lane power."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_host_pcie_others_set(struct pcie_port *pp)
{
	/* For SRIS  PHY settings */
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_physet}
 * @purpose "Initialize phy ops"
 * @logic "Initialize phy ops"
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_phy_init(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	dev_info(pci->dev, "Initialize v920 PHY functions.\n");

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
		exynos_pcie->phy_ops.phy_all_pwrdn = NULL;
		exynos_pcie->phy_ops.phy_all_pwrdn_clear = NULL;
		exynos_pcie->phy_ops.phy_config = exynos_v920_evt2_pcie_phy_config;
	} else {
		exynos_pcie->phy_ops.phy_all_pwrdn = exynos_v920_phy_all_pwrdn;
		exynos_pcie->phy_ops.phy_all_pwrdn_clear = exynos_v920_phy_all_pwrdn_clear;
		exynos_pcie->phy_ops.phy_config = exynos_v920_pcie_phy_config;
	}
}
EXPORT_SYMBOL(exynos_v920_pcie_phy_init);
