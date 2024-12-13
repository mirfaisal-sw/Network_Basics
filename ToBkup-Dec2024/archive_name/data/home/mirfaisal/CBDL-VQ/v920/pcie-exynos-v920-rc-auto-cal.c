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

	writel(CMU_QCH_DOWN, cmu_base_regs);
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

	writel(CMU_QCH_UP, cmu_base_regs);
}

bool exynos_v920_phy_init_ongoing(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	if (exynos_pcie->ch_num == PCIE_CH0) {
		if (exynos_pcie->use_bifurcation) {
			/* 2lane01 Tx/Rx Phy Init check */
			if (((readl(exynos_pcie->phy_base + LN0_TX) & 0x1)
				== 0x1 ||
			(readl(exynos_pcie->phy_base + LN1_TX) & 0x1) == 0x1) ||
			((readl(exynos_pcie->phy_base + LN0_RX) & 0x1) == 0x1 ||
			(readl(exynos_pcie->phy_base + LN1_RX) & 0x1) == 0x1))
				return true;
		} else {
			/* 4lane Tx/Rx Phy Init check */
			if (((readl(exynos_pcie->phy_base + LN0_TX) & 0x1)
				== 0x1 ||
			(readl(exynos_pcie->phy_base + LN1_TX) & 0x1) == 0x1 ||
			(readl(exynos_pcie->phy_base + LN2_TX) & 0x1) == 0x1 ||
			(readl(exynos_pcie->phy_base + LN3_TX) & 0x1) == 0x1) ||
			((readl(exynos_pcie->phy_base + LN0_RX) & 0x1) == 0x1 ||
			(readl(exynos_pcie->phy_base + LN1_RX) & 0x1) == 0x1 ||
			(readl(exynos_pcie->phy_base + LN2_RX) & 0x1) == 0x1 ||
			(readl(exynos_pcie->phy_base + LN3_RX) & 0x1) == 0x1))
				return true;
		}
	} else if (exynos_pcie->ch_num == PCIE_CH1) {
		/* 2lane23 Tx/Rx Phy Init check */
		if (((readl(exynos_pcie->phy_base + LN2_TX) & 0x1)
			== 0x1 ||
		(readl(exynos_pcie->phy_base + LN3_TX) & 0x1) == 0x1) ||
		((readl(exynos_pcie->phy_base + LN2_RX) & 0x1) == 0x1 ||
		(readl(exynos_pcie->phy_base + LN3_RX) & 0x1) == 0x1))
			return true;
	} else if (exynos_pcie->ch_num == PCIE_CH2) {
		if (exynos_pcie->use_bifurcation) {
			/* 2lane0 Tx/Rx Phy Init check */
			if ((readl(exynos_pcie->phy_base + LN0_TX) & 0x1)
				== 0x1 ||
			(readl(exynos_pcie->phy_base + LN0_RX) & 0x1) == 0x1)
				return true;
		} else {
			/* 2lane Tx/Rx Phy Init check */
			if (((readl(exynos_pcie->phy_base + LN0_TX) & 0x1)
				== 0x1 ||
			(readl(exynos_pcie->phy_base + LN1_TX) & 0x1) == 0x1) ||
			((readl(exynos_pcie->phy_base + LN0_RX) & 0x1) == 0x1 ||
			(readl(exynos_pcie->phy_base + LN1_RX) & 0x1) == 0x1))
				return true;
		}
	} else if (exynos_pcie->ch_num == PCIE_CH3) {
		/* 2lane1 Tx/Rx Phy Init check */
		if ((readl(exynos_pcie->phy_base + LN1_TX) & 0x1) == 0x1 ||
			(readl(exynos_pcie->phy_base + LN1_RX) & 0x1) == 0x1)
			return true;
	}
	return false;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_physet}
 * @purpose "Config PCIe RC phy for KITT2"
 * @logic "Reset global pcs\n
 *	Select RC\n
 *	Enable SRIS (if enabled)\n
 *	Config phy for each lane."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in/out, result, int, not NULL}
 * @endparam
 * @retval{result, int, 0, -ERANGE~0, <0}
 */
int exynos_v920_pcie_phy_config(struct pcie_port *pp, bool fw_update)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	u32 val;
	bool phy_config_level = true;
	int timeout;
	int pcie_bifur_sel_reg = 0;
	u32 linkA, linkB;

	void __iomem *sysreg_base_regs = exynos_pcie->sysreg_base;
	void __iomem *elbi_base_regs = exynos_pcie->elbi_base;
	void __iomem *e32_phy_regs = exynos_pcie->e32_phy_base;
	void __iomem *gen_subsys_regs = exynos_pcie->gen_subsys_base;
	void __iomem *soc_ctrl_base_regs = exynos_pcie->soc_ctrl_base;
	void __iomem *pmu_base_regs = exynos_pcie->pmu_base;
	void __iomem *phy_base = exynos_pcie->phy_base;

	struct fw_context fw_ctx = {0, };
	int ret = 0;

	int result = 0;

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
		if (exynos_pcie->ch_num == PCIE_CH0 ||
				exynos_pcie->ch_num == PCIE_CH1) {
			if ((readl(sysreg_base_regs +
				HSI0_PCIE_GEN5_PHY_PWRDWN_4L) &
				HSI0_PCIE_PHY_TEST_PWRDWN_MSK) ==
				HSI0_PCIE_PHY_TEST_PWRDWN_EN)
				phy_config_level = false;

			if (phy_config_level)
				exynos_pcie->phy_fw_ver =
					readl(phy_base + PCIE_PHY_FW_VERSION_0);
			else {
				/* PMU Isolation off */
				val = readl(pmu_base_regs +
					PCIE_PHY_4L_CONFIGURATION);
				writel(val | PCIE_PHY_CFG_EN_PHY,
					pmu_base_regs +
					PCIE_PHY_4L_CONFIGURATION);

				/* phy test power on */
				val = readl(sysreg_base_regs +
					HSI0_PCIE_GEN5_PHY_PWRDWN_4L);
				writel(val & ~HSI0_PCIE_PHY_TEST_PWRDWN_MSK,
					sysreg_base_regs +
					HSI0_PCIE_GEN5_PHY_PWRDWN_4L);
			}
		} else if (exynos_pcie->ch_num == PCIE_CH2 ||
			exynos_pcie->ch_num == PCIE_CH3) {
			if ((readl(sysreg_base_regs +
				HSI0_PCIE_GEN5_PHY_PWRDWN_2L) &
				HSI0_PCIE_PHY_TEST_PWRDWN_MSK) ==
				HSI0_PCIE_PHY_TEST_PWRDWN_EN)
				phy_config_level = false;

			if (phy_config_level)
				exynos_pcie->phy_fw_ver =
					readl(phy_base + PCIE_PHY_FW_VERSION_0);
			else {
				/* In 2L phy, 4L phy pmu should isolation off.*/
				val = readl(pmu_base_regs +
					PCIE_PHY_4L_CONFIGURATION);
				if (val != PCIE_PHY_CFG_EN_PHY)
					writel(val | PCIE_PHY_CFG_EN_PHY,
						pmu_base_regs +
						PCIE_PHY_4L_CONFIGURATION);

				/* PMU Isolation off */
				val = readl(pmu_base_regs +
					PCIE_PHY_2L_CONFIGURATION);
				writel(val | PCIE_PHY_CFG_EN_PHY,
					pmu_base_regs +
					PCIE_PHY_2L_CONFIGURATION);

				/* phy test power on */
				val = readl(sysreg_base_regs +
					HSI0_PCIE_GEN5_PHY_PWRDWN_2L);
				writel(val & ~HSI0_PCIE_PHY_TEST_PWRDWN_MSK,
					sysreg_base_regs +
					HSI0_PCIE_GEN5_PHY_PWRDWN_2L);
			}
		}

		val = readl(soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_0);
		writel(PCIE_REFCLK_OPTION0_RC, soc_ctrl_base_regs +
			PCIE_REFCLK_CTRL_SOC_OPTION_0);

		val = readl(soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_1);
		writel(PCIE_REFCLK_OPTION1_RC, soc_ctrl_base_regs +
			PCIE_REFCLK_CTRL_SOC_OPTION_1);
	} else {
		pcie_bifur_sel_reg = exynos_pcie->ch_num % CH_SHARE_PHY;
		if (pcie_bifur_sel_reg) {
			linkA = readl(exynos_pcie->elbi_base_other) &
				PCIE_LINK_LTSSM_MASK;
			linkB = readl(elbi_base_regs + PCIE_LINK_DBG_2) &
				PCIE_LINK_LTSSM_MASK;
		} else {
			linkA = readl(elbi_base_regs + PCIE_LINK_DBG_2) &
				PCIE_LINK_LTSSM_MASK;
			linkB = readl(exynos_pcie->elbi_base_other) &
				PCIE_LINK_LTSSM_MASK;
		}

		dev_info(pci->dev, "linkA: %x and linkB: %x\n", linkA, linkB);
		if ((linkA != 0x11 && linkB != 0x11) ||
			(!exynos_pcie->use_bifurcation))
			phy_config_level = false;
	}

	if (exynos_pcie->ch_num == PCIE_CH0 ||
		 exynos_pcie->ch_num == PCIE_CH2) {
		/* PE0 COLD reset */
		val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		writel(val | GENERAL_RST_PE0_SOFT_COLD_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		udelay(10);
		writel(val & ~(GENERAL_RST_PE0_SOFT_COLD_RESET),
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
	} else if (exynos_pcie->ch_num == PCIE_CH1 ||
		exynos_pcie->ch_num == PCIE_CH3) {
		/* PE0 COLD reset */
		val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		writel(val | GENERAL_RST_PE1_SOFT_COLD_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		udelay(10);
		writel(val & ~GENERAL_RST_PE1_SOFT_COLD_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
	}

	if (exynos_pcie->ch_num == PCIE_CH0) {
		val = readl(sysreg_base_regs + HSI0_PCIE_GEN5_4LA_PHY_CTRL);
		writel(val | HSI0_PCIE_IP_CTRL_DEV_TYPE_RC_A, sysreg_base_regs +
			HSI0_PCIE_GEN5_4LA_PHY_CTRL);
	} else if (exynos_pcie->ch_num == PCIE_CH1) {
		val = readl(sysreg_base_regs + HSI0_PCIE_GEN5_4LB_PHY_CTRL);
		writel(val | HSI0_PCIE_IP_CTRL_DEV_TYPE_RC_B, sysreg_base_regs +
			HSI0_PCIE_GEN5_4LB_PHY_CTRL);
	} else if (exynos_pcie->ch_num == PCIE_CH2) {
		val = readl(sysreg_base_regs + HSI0_PCIE_GEN5_2LA_PHY_CTRL);
		writel(val | HSI0_PCIE_IP_CTRL_DEV_TYPE_RC_A, sysreg_base_regs +
			HSI0_PCIE_GEN5_2LA_PHY_CTRL);
	} else if (exynos_pcie->ch_num == PCIE_CH3) {
		val = readl(sysreg_base_regs + HSI0_PCIE_GEN5_2LB_PHY_CTRL);
		writel(val | HSI0_PCIE_IP_CTRL_DEV_TYPE_RC_B, sysreg_base_regs +
			HSI0_PCIE_GEN5_2LB_PHY_CTRL);
	}

	if (phy_config_level == false) {
		/* PLL Setting */
		val = readl(sysreg_base_regs + HSI0_PLL_REG0);
		writel(val | HSI0_PLL_FOUTEN, sysreg_base_regs + HSI0_PLL_REG0);

		val = readl(sysreg_base_regs + HSI0_PLL_REG1);
		writel(val | HSI0_PLL_FOUTPOSTDIVEN, sysreg_base_regs +
			HSI0_PLL_REG1);

		val = readl(sysreg_base_regs + HSI0_PLL_REG2);
		writel(val | HSI0_PLL_PLLEN, sysreg_base_regs + HSI0_PLL_REG2);

		/* Clock buffer TX enable	*/
		val = readl(sysreg_base_regs + HSI0_CLKBUF0_REG0);
		writel(val | HSI0_CLKBUF_IMP_CTRL,
			sysreg_base_regs + HSI0_CLKBUF0_REG0);
		val = readl(sysreg_base_regs + HSI0_CLKBUF1_REG0);
		writel(val | HSI0_CLKBUF_IMP_CTRL,
			sysreg_base_regs + HSI0_CLKBUF1_REG0);
		val = readl(sysreg_base_regs + HSI0_CLKBUF2_REG0);
		writel(val | HSI0_CLKBUF_IMP_CTRL,
			sysreg_base_regs + HSI0_CLKBUF2_REG0);
		val = readl(sysreg_base_regs + HSI0_CLKBUF3_REG0);
		writel(val | HSI0_CLKBUF_IMP_CTRL,
			sysreg_base_regs + HSI0_CLKBUF3_REG0);

		if (exynos_pcie->use_bifurcation) {
			if (exynos_pcie->ch_num == PCIE_CH0 ||
				exynos_pcie->ch_num == PCIE_CH1) {
				val = readl(sysreg_base_regs +
					HSI0_PCIE_GEN5_4LA_PHY_CTRL);
				val = val | HSI0_PCIE_IP_CTRL_SS_MODE;
				writel(val, (sysreg_base_regs +
					HSI0_PCIE_GEN5_4LA_PHY_CTRL));
			} else if (exynos_pcie->ch_num == PCIE_CH2 ||
				exynos_pcie->ch_num == PCIE_CH3) {
				val = readl(sysreg_base_regs +
					HSI0_PCIE_GEN5_2LA_PHY_CTRL);
				val = val | HSI0_PCIE_IP_CTRL_SS_MODE;
				writel(val, (sysreg_base_regs +
					HSI0_PCIE_GEN5_2LA_PHY_CTRL));
			}
		}
	}

	if (exynos_pcie->use_sris) {
		val = readl(elbi_base_regs);
		writel(val | PCIE_SRIS_MODE, elbi_base_regs);
	}

	if (phy_config_level == 0) {
		/* Input REFCLK setting */
		/* PHY0_REFA_CLK_SEL */
		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val & ~PCIE_PHY0_REFA_CLK_SEL_MASK, e32_phy_regs +
			PCIE_PHY0_GEN_CTRL_1);
		if (exynos_pcie->use_bifurcation)
			writel(val & PCIE_PHY0_REFA_CLK_SEL_ALT1, e32_phy_regs +
				PCIE_PHY0_GEN_CTRL_1);

		/* PHY0_REFB_CLK_SEL */
		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val & ~PCIE_PHY0_REFB_CLK_SEL_MASK, e32_phy_regs +
			PCIE_PHY0_GEN_CTRL_1);
		if (exynos_pcie->use_bifurcation)
			writel(val & PCIE_PHY0_REFB_CLK_SEL_ALT1, e32_phy_regs +
				PCIE_PHY0_GEN_CTRL_1);

		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);

		udelay(100);

		/* PHY resistor tune request */
		writel(PCIE_PHY_RTUNE_REQ, e32_phy_regs);
		/* PHY External Tx RoPLL PostDiv Control */
		writel(PCIE_PHY_ROPLL_POSTDIV_VAL, e32_phy_regs +
			PCIE_PHY_EXT_TX_ROPLL_POSTDIV_CTRL);
		/* PHY External TX RoPLL PostDiv Override Enable */
		writel(PCIE_PHY_ROPLL_POSTDIV_OVRD_EN_VAL, e32_phy_regs +
			PCIE_PHY_EXT_TX_ROPLL_POSTDIV_OVRD_EN_CTRL);
		/* Lanepll Bypass Mode Control */
		writel(PCIE_PIPE_BYPASS_MODE_CTRL_VAL1, e32_phy_regs +
			PCIE_PIPE_LANEX_LANEPLL_BYPASS_MODE_CTRL_1);

		writel(PCIE_PIPE_BYPASS_MODE_CTRL_VAL2, e32_phy_regs +
			PCIE_PIPE_LANEX_LANEPLL_BYPASS_MODE_CTRL_1);

		if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
			if (exynos_pcie->use_bifurcation &&
				(exynos_pcie->ch_num == PCIE_CH1 ||
				exynos_pcie->ch_num == PCIE_CH3))
				/* Enable Reference Clock Detection for ref B */
				writel(PCIE_PHY0_REFA_B_ALT1, e32_phy_regs +
					PCIE_PHY0_GEN_CTRL_1);
			else
				/* Enable REF Clock Detection for refA/B */
				writel(PCIE_PHY0_REFA_B_ALT0, e32_phy_regs +
					PCIE_PHY0_GEN_CTRL_1);
		} else {
			if (exynos_pcie->use_bifurcation) {
				/* Enable REF Clock Detection for refA/B */
				writel(PCIE_PHY0_REFA_B_ALT1, e32_phy_regs +
					PCIE_PHY0_GEN_CTRL_1);
			} else {
				/* Enable REF Clock Detection for refA/B */
				writel(PCIE_PHY0_REFA_B_ALT0, e32_phy_regs +
					PCIE_PHY0_GEN_CTRL_1);
			}
		}

		val = readl(elbi_base_regs);
		writel(val | PCIE_DEV_TYPE_OVR, elbi_base_regs);
		udelay(100);

		val = readl(elbi_base_regs);
		writel(val | PCIE_DEV_TYPE_RC, elbi_base_regs);
	}

	if (fw_update || (phy_config_level == false)) {
		//only for 2l phy. S2R case. phy all reset for FW update
		/*	PE0 SW reset */
		val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		writel(val | GENERAL_RST_PE0_SOFT_WARM_PHY_RESET |
			GENERAL_RST_PE1_SOFT_WARM_PHY_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		udelay(10);

		if (fw_update) {
			/* phy contorl 10~12bit reset to dram->sram copy */
			val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
			writel(val & ~PCIE_PHY0_ALL_BYPASS,
				e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		}

		writel(val & ~GENERAL_RST_PE0_SOFT_WARM_PHY_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		udelay(10);
		writel(val & ~GENERAL_RST_PE1_SOFT_WARM_PHY_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		udelay(10);
	} else if (exynos_pcie->ch_num == PCIE_CH0 ||
			exynos_pcie->ch_num == PCIE_CH2) {
		/*	PE0 SW reset */
		val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		writel(val | GENERAL_RST_PE0_SOFT_WARM_PHY_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		udelay(10);
		if (exynos_pcie->ch_num == PCIE_CH2 &&
			exynos_pcie->phy_fw_ver == FW_VER_205) {
			/* To Avoid ROM->SRAM copy */
			val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
			writel(val | PCIE_PHY0_SRAM_BL_BYPASS,
				e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		}
		writel(val & ~GENERAL_RST_PE0_SOFT_WARM_PHY_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
	} else if (exynos_pcie->ch_num == PCIE_CH1 ||
		exynos_pcie->ch_num == PCIE_CH3) {
		/*	PE0 SW reset */
		val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		writel(val | GENERAL_RST_PE1_SOFT_WARM_PHY_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		udelay(10);
		if (exynos_pcie->ch_num == PCIE_CH3 && exynos_pcie->phy_fw_ver
			== FW_VER_205) {
			/* To Avoid ROM->SRAM copy */
			val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
			writel(val | PCIE_PHY0_SRAM_BL_BYPASS,
				e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		}
		writel(val & ~GENERAL_RST_PE1_SOFT_WARM_PHY_RESET,
			gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
	}

	/*	PE1 SW reset */
	val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
	val = GENERAL_RST_PE0_1_PHY_EN;
	writel(val, gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
	udelay(10);

	if (phy_config_level == false) {
		if (exynos_pcie->ch_num == PCIE_CH0 ||
			exynos_pcie->ch_num == PCIE_CH1) {
			/*	Set SRAM bypass	*/
			val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
			writel(val | PCIE_PHY0_PHY0_SRAM_BYPASS,
				e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		} else if ((exynos_pcie->ch_num == PCIE_CH2 ||
			exynos_pcie->ch_num == PCIE_CH3) &&
			exynos_pcie->phy_fw_ver !=
				FW_VER_205 && fw_update == false) {
			/*	Set SRAM bypass	*/
			val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
			writel(val | PCIE_PHY0_PHY0_SRAM_BYPASS,
				e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		}

		timeout = 0;
		do {
			udelay(1);
			timeout++;
			if (timeout >= PHY_TIMEOUT) {
				dev_err(pci->dev, "SRAM bypass FAIL\n");
				result = -ETIME;
				return result;
			}
		} while (!(readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1) >>
				PCIE_PHY0_SRAM_INIT_DONE_BIT));


		if (exynos_pcie->ch_num == PCIE_CH2 && fw_update == true) {
			fw_ctx.phys_addr = exynos_pcie->fw_paddr;
			fw_ctx.virt_addr =
				(unsigned long long) exynos_pcie->pcie_fw_buf;

			fw_ctx.size = exynos_pcie->phy_fw_size;
			fw_ctx.fw_type = PCIE_0;
			ret =
			snprintf(fw_ctx.name, FW_NAME_LEN - 1, "pcie_fw_0");
			if (ret <= 0) {
				dev_err(pci->dev, "FW NAME set fail\n");
				result = -EINVAL;
				return result;
			}

			ret = (int)fw_sb_verify(&fw_ctx);

			if (ret != RV_SUCCESS) {
				dev_err(pci->dev,
				"PCIe FW is failed to verify : %d\n", ret);
				result = -EKEYREJECTED;
				return result;
			}

			val = readl(phy_base + 0x404);
			writel(val | (0x1<<1), phy_base + 0x404);

			fw_ctx.fw_type = PCIE_1;
			ret =
			snprintf(fw_ctx.name, FW_NAME_LEN - 1, "pcie_fw_1");
			if (ret <= 0) {
				dev_err(pci->dev, "FW NAME set fail\n");
				result = -EINVAL;
				return result;
			}

			ret = (int)fw_sb_verify(&fw_ctx);

			if (ret != RV_SUCCESS) {
				dev_err(pci->dev,
				"PCIe FW is failed to verify : %d\n", ret);
				result = -EKEYREJECTED;
				return result;
			}
		}

		timeout = 0;
		/*	Set PHY*_SRAM_EXT_LD_DONE */
		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val | PCIE_PHY0_PHY0_SRAM_EXT_LD_DONE,
			e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);

		do {
			udelay(1);
			timeout++;
			if (timeout >= PHY_TIMEOUT) {
				val =
				readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
				dev_err(pci->dev, "CDM init FAIL\n");
				result = -ETIME;
				return result;
			}
		} while ((readl(elbi_base_regs + PCIE_LINK_DBG_2) >>
			PCIE_LINK_CDM_IN_RESET_MASK)
				& 0x1);
		timeout = 0;
	}

	timeout = 0;
	do {
		udelay(1);
		timeout++;
		if (timeout >= MAX_PHY_INTI_TIMEOUT) {
			dev_err(pci->dev, "TX/RX PHY init not done.\n");
			result = -ETIME;
			return result;
		}
	} while (exynos_v920_phy_init_ongoing(pp));

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
		exynos_pcie->phy_fw_ver =
			readl(phy_base + PCIE_PHY_FW_VERSION_0);
		if (exynos_pcie->ch_num == PCIE_CH2 &&
				exynos_pcie->phy_fw_ver == FW_VER_205)
			exynos_pcie->evt2_minor_ver = EVT20;
		else if (exynos_pcie->ch_num == PCIE_CH2 &&
				exynos_pcie->phy_fw_ver == FW_VER_240)
			exynos_pcie->evt2_minor_ver = EVT21;

		dev_info(pci->dev, "Final FW Version %x\n",
			exynos_pcie->phy_fw_ver);
	}

	return result;
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
		exynos_pcie->phy_ops.phy_config = exynos_v920_pcie_phy_config;
	} else {
		exynos_pcie->phy_ops.phy_all_pwrdn = exynos_v920_phy_all_pwrdn;
		exynos_pcie->phy_ops.phy_all_pwrdn_clear =
			exynos_v920_phy_all_pwrdn_clear;
		exynos_pcie->phy_ops.phy_config = exynos_v920_pcie_phy_config;
	}
}
EXPORT_SYMBOL(exynos_v920_pcie_phy_init);
