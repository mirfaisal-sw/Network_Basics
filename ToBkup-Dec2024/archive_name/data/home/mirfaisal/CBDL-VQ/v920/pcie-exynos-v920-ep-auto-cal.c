// SPDX-License-Identifier: GPL-2.0
/**
 * pcie-exynos-ep-auto-cal.c - Exynos PCIe EP cal
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
#include <linux/of_gpio.h>
#include <linux/delay.h>

#include "pcie-exynos-v920-ep-auto.h"
#include "pcie-exynos-v920-ep-auto-cal.h"

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_phypower}
 * @purpose "Power off the PCIe phy power"
 * @logic "Check other lane status\n
 *	If condition satisfied, phy power down."
 * @params
 * @param{in, phyinfo, struct ::exynos_ep_pcie_phyinfo *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_phy_all_pwrdn(struct exynos_ep_pcie_phyinfo *phyinfo)
{
	void __iomem *cmu_base_regs = phyinfo->cmu_base;

	writel(CMU_QCH_DOWN, cmu_base_regs);
}
EXPORT_SYMBOL(exynos_v920_pcie_ep_phy_all_pwrdn);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_phypower}
 * @purpose "Power on all phy"
 * @logic "Power on all phy."
 * @params
 * @param{in, phyinfo, struct ::exynos_ep_pcie_phyinfo *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_phy_all_pwrdn_clear(struct exynos_ep_pcie_phyinfo *phyinfo)
{
	void __iomem *cmu_base_regs = phyinfo->cmu_base;

	writel(CMU_QCH_UP, cmu_base_regs);
}
EXPORT_SYMBOL(exynos_v920_pcie_ep_phy_all_pwrdn_clear);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_physet}
 * @purpose "Config pcie phy at link start or reset for KITT2"
 * @logic "Phy reset\n
 *	Set to ep selection\n
 *	SRIS enable if use SRIS feature\n
 *	Perform phy setting"
 * @params
 * @param{in/out, phyinfo, struct ::exynos_ep_pcie_phyinfo *, not NULL}
 * @param{in/out, result, int, not NULL}
 * @endparam
 * @retval{result, int, 0, -ERANGE~0, <0}
 */
int exynos_v920_pcie_ep_phy_config(struct exynos_ep_pcie_phyinfo *phyinfo)
{
	void __iomem *elbi_base_regs = phyinfo->elbi_base;
	void __iomem *sysreg_base_regs = phyinfo->sysreg_base;
	void __iomem *e32_phy_regs = phyinfo->e32_phy_base;
	void __iomem *soc_ctrl_base_regs = phyinfo->soc_ctrl_base;
	void __iomem *pmu_base_regs = phyinfo->pmu_base;

	bool phy_config_level = true;
	u32 val;
	int timeout;
	u32 linkA, linkB;
	int pcie_bifur_sel_reg = 0;
	int result = 0;

	if (phyinfo->soc_variant == EXYNOSAUTOV920_EVT2) {
		if ((phyinfo->ch_num == CH0_4L || phyinfo->ch_num == CH1_4L) &&
			(readl(sysreg_base_regs + HSI0_PCIE_GEN5_PHY_PWRDWN_4L) &
			HSI0_PCIE_PHY_TEST_PWRDWN_MSK) == HSI0_PCIE_PHY_TEST_PWRDWN_EN)
			phy_config_level = false;
		else if ((phyinfo->ch_num == CH2_2L || phyinfo->ch_num == CH3_2L) &&
			(readl(sysreg_base_regs + HSI0_PCIE_GEN5_PHY_PWRDWN_2L) &
			HSI0_PCIE_PHY_TEST_PWRDWN_MSK) == HSI0_PCIE_PHY_TEST_PWRDWN_EN)
			phy_config_level = false;
	} else {
		pcie_bifur_sel_reg = phyinfo->ch_num % CH_SHARE_PHY;

		if (pcie_bifur_sel_reg) {
			linkA = readl(phyinfo->elbi_base_other) &
					PCIE_LINK_LTSSM_MASK;
			linkB = readl(elbi_base_regs + PCIE_LINK_DBG_2) &
					PCIE_LINK_LTSSM_MASK;
		} else {
			linkA = readl(elbi_base_regs + PCIE_LINK_DBG_2) &
					PCIE_LINK_LTSSM_MASK;
			linkB = readl(phyinfo->elbi_base_other) &
					PCIE_LINK_LTSSM_MASK;
		}

		dev_info(NULL, "%s linkA: %x and linkB: %x\n",
			__func__, linkA, linkB);
		if ((linkA != S_L0 && linkB != S_L0) ||
			(!phyinfo->use_bifurcation))
			phy_config_level = false;
	}

	if (phyinfo->soc_variant == EXYNOSAUTOV920_EVT2) {
		if ((phyinfo->ch_num == CH0_4L || phyinfo->ch_num == CH1_4L) &&
			phy_config_level == false) {
			/* PMU Isolation off */
			val = readl(pmu_base_regs + PCIE_PHY_4L_CONFIGURATION);
			writel(val | PCIE_PHY_CFG_EN_PHY,
				pmu_base_regs + PCIE_PHY_4L_CONFIGURATION);

			/* phy test power on */
			val = readl(sysreg_base_regs +
				HSI0_PCIE_GEN5_PHY_PWRDWN_4L);
			writel(val & ~(HSI0_PCIE_PHY_TEST_PWRDWN_MSK),
				sysreg_base_regs +
				HSI0_PCIE_GEN5_PHY_PWRDWN_4L);
		} else if (phy_config_level == false) {
			/* In 2L phy, 4L phy pmu should be isolation off. */
			val = readl(pmu_base_regs + PCIE_PHY_4L_CONFIGURATION);
			if (val != PCIE_PHY_CFG_EN_PHY)
				writel(val | PCIE_PHY_CFG_EN_PHY,
					pmu_base_regs +
					PCIE_PHY_4L_CONFIGURATION);

			/* PMU Isolation off */
			val = readl(pmu_base_regs + PCIE_PHY_2L_CONFIGURATION);
			writel(val | PCIE_PHY_CFG_EN_PHY, pmu_base_regs +
				PCIE_PHY_2L_CONFIGURATION);

			/* phy test power on */
			val = readl(sysreg_base_regs +
				HSI0_PCIE_GEN5_PHY_PWRDWN_2L);
			writel(val & ~(HSI0_PCIE_PHY_TEST_PWRDWN_MSK),
				sysreg_base_regs +
				HSI0_PCIE_GEN5_PHY_PWRDWN_2L);
		}

		val = readl(soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_0);
		writel(PCIE_REFCLK_OPTION0_DEVICETYPE_EP |
			PCIE_REFCLK_OPTION0_DEFAULT | PCIE_REFCLK_OPTION0_PLLEN,
			soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_0);

		val = readl(soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_1);
		writel(PCIE_REFCLK_OPTION1_DEFAULT, soc_ctrl_base_regs +
			PCIE_REFCLK_CTRL_SOC_OPTION_1);
	}

	phyinfo->cold_rst = true;
	exynos_v920_pcie_ep_phy_g_rst(phyinfo);

	if (phy_config_level == false) {
		if (phyinfo->use_sris || phyinfo->soc_variant !=
			EXYNOSAUTOV920_EVT2) {
			pr_info("SRIS enabled. set internal pll.\n");
			/*	PLL Setting */
			val = readl(sysreg_base_regs + HSI0_PLL_REG0);
			writel(val | HSI0_PLL_FOUTEN, sysreg_base_regs +
				HSI0_PLL_REG0);

			val = readl(sysreg_base_regs + HSI0_PLL_REG1);
			writel(val | HSI0_PLL_FOUTPOSTDIVEN, sysreg_base_regs +
				HSI0_PLL_REG1);

			val = readl(sysreg_base_regs + HSI0_PLL_REG2);
			writel(val | HSI0_PLL_PLLEN, sysreg_base_regs +
				HSI0_PLL_REG2);

			/*	Clock buffer TX enable	*/
			val = readl(sysreg_base_regs + HSI0_CLKBUF0_REG0);
			writel(val | HSI0_CLKBUF_IMP_CTRL, sysreg_base_regs +
				HSI0_CLKBUF0_REG0);
			val = readl(sysreg_base_regs + HSI0_CLKBUF1_REG0);
			writel(val | HSI0_CLKBUF_IMP_CTRL, sysreg_base_regs +
				HSI0_CLKBUF1_REG0);
			val = readl(sysreg_base_regs + HSI0_CLKBUF2_REG0);
			writel(val | HSI0_CLKBUF_IMP_CTRL, sysreg_base_regs +
				HSI0_CLKBUF2_REG0);
			val = readl(sysreg_base_regs + HSI0_CLKBUF3_REG0);
			writel(val | HSI0_CLKBUF_IMP_CTRL, sysreg_base_regs +
				HSI0_CLKBUF3_REG0);
		}

		if (phyinfo->use_bifurcation) {
			if (phyinfo->ch_num == CH0_4L ||
				phyinfo->ch_num == CH1_4L) {
				val = readl(sysreg_base_regs +
					HSI0_PCIE_GEN5_4LA_PHY_CTRL);
				val = val | HSI0_PCIE_IP_CTRL_SS_MODE;
				writel(val, (sysreg_base_regs +
					HSI0_PCIE_GEN5_4LA_PHY_CTRL));
			} else if (phyinfo->ch_num == CH2_2L ||
				phyinfo->ch_num == CH3_2L) {
				val = readl(sysreg_base_regs +
					HSI0_PCIE_GEN5_2LA_PHY_CTRL);
				val = val | HSI0_PCIE_IP_CTRL_SS_MODE;
				writel(val, (sysreg_base_regs +
					HSI0_PCIE_GEN5_2LA_PHY_CTRL));
			}
		}
	}

	if (phyinfo->soc_variant == EXYNOSAUTOV920_EVT2) {
		val = readl(soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_0);
		writel(PCIE_REFCLK_OPTION0_DEFAULT |
			PCIE_REFCLK_OPTION0_PLLEN_PLL_LOCK,
			soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_0);

		val = readl(soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_1);
		writel(PCIE_REFCLK_OPTION1_DEFAULT, soc_ctrl_base_regs +
			PCIE_REFCLK_CTRL_SOC_OPTION_1);

		writel(PCIE_REFCLK_OPTION1_DEFAULT | PCIE_REFCLK_OPTION1_CG_PHY,
			soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_1);

		writel(PCIE_REFCLK_OPTION0_DEVICETYPE_EP |
			PCIE_REFCLK_OPTION0_DEFAULT |
			PCIE_REFCLK_OPTION0_PLLEN_PLL_LOCK,
			soc_ctrl_base_regs + PCIE_REFCLK_CTRL_SOC_OPTION_0);
	}

	if (phyinfo->use_sris) {
		val = readl(elbi_base_regs);
		writel(val | PCIE_SRIS_MODE, elbi_base_regs);
	}

	if (phy_config_level == false) {
		/* Input REFCLK setting */
		/* PHY0_REFA_CLK_SEL */
		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val & ~PCIE_PHY0_REFA_CLK_SEL_MASK, e32_phy_regs +
			PCIE_PHY0_GEN_CTRL_1);
		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val & PCIE_PHY0_REFA_CLK_SEL_PAD, e32_phy_regs +
			PCIE_PHY0_GEN_CTRL_1);

		/* PHY0_REFB_CLK_SEL */
		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val & ~PCIE_PHY0_REFB_CLK_SEL_MASK, e32_phy_regs +
			PCIE_PHY0_GEN_CTRL_1);
		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val & PCIE_PHY0_REFB_CLK_SEL_PAD, e32_phy_regs +
			PCIE_PHY0_GEN_CTRL_1);

		/* PHY0_REFB_CLK_SEL */
		udelay(100);

		/* PHY resistor tune request */
		writel(PCIE_PHY_RTUNE_REQ, e32_phy_regs);
		/* PHY External Tx RoPLL PostDiv Control */
		writel(PCIE_PHY_ROPLL_POSTDIV_VAL, e32_phy_regs +
			PCIE_PHY_EXT_TX_ROPLL_POSTDIV_CTRL);
		/* PHY External TX RoPLL PostDiv Override Enable */
		writel(PCIE_PHY_ROPLL_POSTDIV_OVRD_EN_VAL, e32_phy_regs +
			PCIE_PHY_EXT_TX_ROPLL_POSTDIV_OVRD_EN_CTRL);
		/* Lanepll Bypass Mode Control	*/
		writel(PCIE_PIPE_BYPASS_MODE_CTRL_VAL1, e32_phy_regs +
			PCIE_PIPE_LANEX_LANEPLL_BYPASS_MODE_CTRL_1);
		writel(PCIE_PIPE_BYPASS_MODE_CTRL_VAL2, e32_phy_regs +
			PCIE_PIPE_LANEX_LANEPLL_BYPASS_MODE_CTRL_1);

		if (phyinfo->soc_variant == EXYNOSAUTOV920_EVT2) {
			if (phyinfo->use_sris) {
				/* Enable Reference Clock Detection for ref B */
				if (phyinfo->ch_num == CH1_4L ||
					phyinfo->ch_num == CH3_2L)
					writel(PCIE_PHY0_REFA_B_ALT1,
						e32_phy_regs +
						PCIE_PHY0_GEN_CTRL_1);
				else
					writel(PCIE_PHY0_REFA_B_ALT0,
						e32_phy_regs +
						PCIE_PHY0_GEN_CTRL_1);
			} else
				writel(PCIE_PHY0_REFA_B_PAD, e32_phy_regs +
					PCIE_PHY0_GEN_CTRL_1);
		} else {
			/* EVT0/1 needs EP use internal clock PERST enable */
			if (phyinfo->use_bifurcation)
				/* Enable REF Clock Detection for refA/B */
				writel(PCIE_PHY0_REFA_B_ALT1, e32_phy_regs +
					PCIE_PHY0_GEN_CTRL_1);
			else
				/* Enable REF Clock Detection for refA/B */
				writel(PCIE_PHY0_REFA_B_ALT0, e32_phy_regs +
					PCIE_PHY0_GEN_CTRL_1);
		}
	}

	phyinfo->cold_rst = false;
	exynos_v920_pcie_ep_phy_g_rst(phyinfo);

	if (phy_config_level == false) {
		/*	SRAM Bypass Start	*/
		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val | PCIE_PHY0_PHY0_SRAM_BYPASS, e32_phy_regs +
			PCIE_PHY0_GEN_CTRL_1);

		timeout = 0;
		do {
			udelay(1);

			timeout++;
			if (timeout == PHY_TIMEOUT) {
				dev_info(NULL, "SRAM bypass FAIL\n");
				result = -ETIME;
				return result;
			}
		} while (!(readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1) >>
				PCIE_PHY0_SRAM_INIT_DONE_BIT));
		timeout = 0;

		val = readl(e32_phy_regs + PCIE_PHY0_GEN_CTRL_1);
		writel(val | PCIE_PHY0_PHY0_SRAM_EXT_LD_DONE, e32_phy_regs +
			PCIE_PHY0_GEN_CTRL_1);

		/*	WAIT CDM in reset	*/
		do {
			udelay(1);
			timeout++;
			if (timeout == PHY_TIMEOUT) {
				dev_info(NULL, "CDM init FAIL\n");
				result = -ETIME;
				return result;
			}
		} while (!(readl(elbi_base_regs + PCIE_LINK_DBG_2) >>
			PCIE_LINK_CDM_IN_RESET_MASK));
		timeout = 0;
	}

	return result;
}
EXPORT_SYMBOL(exynos_v920_pcie_ep_phy_config);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_physet}
 * @purpose "Additional phy setting"
 * @logic ""
 * @params
 * @param{in, phyinfo, struct ::exynos_ep_pcie_phyinfo *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_phy_pcs_rst_set(struct exynos_ep_pcie_phyinfo *phyinfo)
{
	/* For SRIS  PHY settings */
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_physet}
 * @purpose "Reset pcie global phy"
 * @logic "Reset pcie global phy"
 * @params
 * @param{in, phyinfo, struct ::exynos_ep_pcie_phyinfo *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_phy_g_rst(struct exynos_ep_pcie_phyinfo *phyinfo)
{
	void __iomem *gen_subsys_regs = phyinfo->gen_subsys_base;
	u32 val;

	if (phyinfo->cold_rst) {
		/*	Perform Cold Reset */
		if (phyinfo->ch_num == CH0_4L || phyinfo->ch_num == CH2_2L) {
			/*	PE0 COLD reset */
			val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
			writel(val | GENERAL_RST_PE0_SOFT_COLD_RESET,
				gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
			udelay(10);
			writel(val & ~(GENERAL_RST_PE0_SOFT_COLD_RESET),
				gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		} else if (phyinfo->ch_num == CH1_4L ||
			phyinfo->ch_num == CH3_2L) {
			/*	PE0 COLD reset */
			val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
			writel(val | GENERAL_RST_PE1_SOFT_COLD_RESET,
				gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
			udelay(10);
			writel(val & ~GENERAL_RST_PE1_SOFT_COLD_RESET,
				gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		}
	} else {
		/*	Perform Warm Reset/PHY Reset */
		if (phyinfo->ch_num == CH0_4L || phyinfo->ch_num == CH2_2L) {
		/*	PE0 SW reset */
			val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
			writel(val | GENERAL_RST_PE0_SOFT_WARM_PHY_RESET,
				gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
			udelay(10);
			writel(val & ~GENERAL_RST_PE0_SOFT_WARM_PHY_RESET,
				gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		} else if (phyinfo->ch_num == CH1_4L ||
			phyinfo->ch_num == CH3_2L) {
		/*	PE0 SW reset */
			val = readl(gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
			writel(val | GENERAL_RST_PE1_SOFT_WARM_PHY_RESET,
				gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
			udelay(10);
			writel(val & ~GENERAL_RST_PE1_SOFT_WARM_PHY_RESET,
				gen_subsys_regs + GENERAL_SS_RST_CTRL_1);
		}
	}

}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_ep_physet}
 * @purpose "Misc setting for pcie phy"
 * @logic "REFCLK setting; Lane power enable"
 * @params
 * @param{in, phyinfo, struct ::exynos_ep_pcie_phyinfo *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_ep_phy_others_set(struct exynos_ep_pcie_phyinfo *phyinfo)
{
	/* For MSIC PHY settings */
}
EXPORT_SYMBOL(exynos_v920_pcie_ep_phy_others_set);

