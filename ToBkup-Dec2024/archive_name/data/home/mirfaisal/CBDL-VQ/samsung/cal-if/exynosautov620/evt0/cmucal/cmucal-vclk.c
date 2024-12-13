// SPDX-License-Identifier: GPL-2.0
/*
 * Common Clock Framework support for exynosautov620 SoC.
 *
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */
#include "../../../cmucal.h"
#include "cmucal-node.h"
#include "cmucal-vclk.h"
#include "cmucal-vclklut.h"

/* DVFS VCLK -> Clock Node List */
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_vdd_int[] = {
	V620_EVT0_DIV_CLK_DSIM_OSCCLK,
	V620_EVT0_MUX_NOCL1_CMUREF,
	V620_EVT0_MUX_NOCL2_CMUREF,
	V620_EVT0_DIV_CLK_PERIC0_I3C,
	V620_EVT0_MUX_CLK_PERIC0_I3C,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_vdd_mif[] = {
	V620_EVT0_PLL_MIF_MAIN,
	V620_EVT0_MUX_MIF_CMUREF,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_vdd_g3d[] = {
	V620_EVT0_PLL_G3D,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_vdd_cpucl0[] = {
	V620_EVT0_DIV_CLK_CLUSTER0_ACLK,
	V620_EVT0_PLL_CPUCL0,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_vdd_sfi[] = {
	V620_EVT0_CLK_SFI_PLLCLKOUT,
	V620_EVT0_DIV_CLK_SFI_USI18,
	V620_EVT0_DIV_CLK_SFI_USI21,
	V620_EVT0_DIV_CLK_SFI_XSPI,
	V620_EVT0_DIV_CLK_SFI_USI19,
	V620_EVT0_DIV_CLK_SFI_USI20,
	V620_EVT0_DIV_CLK_SFI_NOCP,
	V620_EVT0_DIV_CLK_SFI_CAN0,
	V620_EVT0_DIV_CLK_SFI_CAN1,
	V620_EVT0_PLL_SFI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_vdd_aud[] = {
	V620_EVT0_MUX_CLK_AUD_AVB_ETH,
	V620_EVT0_DIV_CLK_AUD_AUDIF,
	V620_EVT0_DIV_CLK_AUD_NOCP,
	V620_EVT0_PLL_AUD,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_vdd_int_cmu[] = {
	V620_EVT0_MUX_CLKCMU_GNPU_NOC,
	V620_EVT0_MUX_CLKCMU_DNC_NOC,
	V620_EVT0_MUX_CLKCMU_DSP_NOC,
	V620_EVT0_MUX_CLKCMU_SDMA_NOC,
	V620_EVT0_CLKCMU_MFC_FG,
	V620_EVT0_MUX_CLKCMU_MFC_FG,
	V620_EVT0_MUX_CLKCMU_NOCL1_NOC,
	V620_EVT0_MUX_CLKCMU_NOCL2_NOC,
	V620_EVT0_MUX_CLKCMU_GNPU_XMAA,
	V620_EVT0_PLL_SHARED3,
	V620_EVT0_MUX_CLKCMU_CPUCL0_DBG,
	V620_EVT0_MUX_CLKCMU_G3D_SWITCH,
	V620_EVT0_CLKCMU_DPUB_DSIM,
	V620_EVT0_CLKCMU_DPTX_DPOSC,
	V620_EVT0_MUX_CLKCMU_DPTX_DPOSC,
	V620_EVT0_MUX_CLKCMU_MFC_MFC,
	V620_EVT0_MUX_CLKCMU_MFC_WFD,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_vdd_cpucl2[] = {
	V620_EVT0_DIV_CLK_CLUSTER2_ACLK,
	V620_EVT0_PLL_CPUCL2,
};
/* SPECIAL VCLK -> Clock Node List */
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_apm_timer[] = {
	V620_EVT0_MUX_CLK_APM_TIMER,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_aud_uaif3[] = {
	V620_EVT0_MUX_CLK_AUD_UAIF3,
	V620_EVT0_MUX_CLK_AUD_UAIFS1,
	V620_EVT0_MUX_CLK_AUD_UAIFS0,
	V620_EVT0_MUX_CLK_AUD_UAIF9,
	V620_EVT0_MUX_CLK_AUD_UAIF0,
	V620_EVT0_MUX_CLK_AUD_UAIF1,
	V620_EVT0_MUX_CLK_AUD_UAIF2,
	V620_EVT0_MUX_CLK_AUD_UAIF5,
	V620_EVT0_MUX_CLK_AUD_UAIF6,
	V620_EVT0_MUX_CLK_AUD_UAIF8,
	V620_EVT0_MUX_CLK_AUD_UAIF7,
	V620_EVT0_MUX_CLK_AUD_UAIF4,
	V620_EVT0_DIV_CLK_AUD_UAIF3,
	V620_EVT0_DIV_CLK_AUD_UAIF0,
	V620_EVT0_DIV_CLK_AUD_UAIF1,
	V620_EVT0_DIV_CLK_AUD_UAIF2,
	V620_EVT0_MUX_HCHGEN_CLK_AUD_CPU,
	V620_EVT0_DIV_CLK_AUD_CPU,
	V620_EVT0_DIV_HCHGEN_CLK_AUD_CPU,
	V620_EVT0_DIV_CLK_AUD_UAIF5,
	V620_EVT0_DIV_CLK_AUD_UAIF6,
	V620_EVT0_DIV_CLK_AUD_UAIF8,
	V620_EVT0_DIV_CLK_AUD_UAIF9,
	V620_EVT0_DIV_CLK_AUD_UAIFS0,
	V620_EVT0_DIV_CLK_AUD_UAIFS1,
	V620_EVT0_DIV_CLK_AUD_UAIF7,
	V620_EVT0_DIV_CLK_AUD_UAIF4,
	V620_EVT0_DIV_CLK_AUD_CNT,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_cmu_cmuref[] = {
	V620_EVT0_MUX_CMU_CMUREF,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_cpucl0_cmuref[] = {
	V620_EVT0_MUX_CPUCL0_CMUREF,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_cpucl2_cmuref[] = {
	V620_EVT0_MUX_CPUCL2_CMUREF,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_misc_gic[] = {
	V620_EVT0_MUX_CLK_MISC_GIC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_misc_noc[] = {
	V620_EVT0_CLKCMU_MISC_NOC,
	V620_EVT0_MUX_CLKCMU_MISC_NOC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_nocl0_cmuref[] = {
	V620_EVT0_MUX_NOCL0_CMUREF,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_dptx_dpgtc[] = {
	V620_EVT0_CLKCMU_DPTX_DPGTC,
	V620_EVT0_MUX_CLKCMU_DPTX_DPGTC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clkcmu_dpub_dsim[] = {
	V620_EVT0_MUX_CLKCMU_DPUB_DSIM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_gnpu_xmaa[] = {
	V620_EVT0_CLKCMU_GNPU_XMAA,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_apm_spmi[] = {
	V620_EVT0_DIV_CLK_APM_SPMI,
	V620_EVT0_MUX_CLK_APM_SPMI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_apm_noc[] = {
	V620_EVT0_CLKCMU_APM_NOC,
	V620_EVT0_MUX_CLKCMU_APM_NOC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_aud_mclk[] = {
	V620_EVT0_DIV_CLK_AUD_MCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clk_aud_avb_eth[] = {
	V620_EVT0_CLK_AUD_AVB_ETH,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clk_cmu_pllclkout[] = {
	V620_EVT0_CLK_CMU_PLLCLKOUT,
	V620_EVT0_MUX_CLK_CMU_PLLCLKOUT,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clk_add_ch_clk[] = {
	V620_EVT0_CLK_ADD_CH_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_cmu_nocp[] = {
	V620_EVT0_DIV_CLK_CMU_NOCP,
	V620_EVT0_MUX_CLK_CMU_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_cis_mclk3[] = {
	V620_EVT0_CLKCMU_CIS_MCLK3,
	V620_EVT0_MUX_CLKCMU_CIS_MCLK3,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_cis_mclk0[] = {
	V620_EVT0_CLKCMU_CIS_MCLK0,
	V620_EVT0_MUX_CLKCMU_CIS_MCLK0,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_cis_mclk1[] = {
	V620_EVT0_CLKCMU_CIS_MCLK1,
	V620_EVT0_MUX_CLKCMU_CIS_MCLK1,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_cis_mclk2[] = {
	V620_EVT0_CLKCMU_CIS_MCLK2,
	V620_EVT0_MUX_CLKCMU_CIS_MCLK2,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_cpucl0_shortstop[] = {
	V620_EVT0_DIV_CLK_CPUCL0_SHORTSTOP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_cpucl0_cluster[] = {
	V620_EVT0_CLKCMU_CPUCL0_CLUSTER,
	V620_EVT0_MUX_CLKCMU_CPUCL0_CLUSTER,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_cpucl0_switch[] = {
	V620_EVT0_CLKCMU_CPUCL0_SWITCH,
	V620_EVT0_MUX_CLKCMU_CPUCL0_SWITCH,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_cpucl2_shortstop[] = {
	V620_EVT0_DIV_CLK_CPUCL2_SHORTSTOP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_cpucl2_cluster[] = {
	V620_EVT0_CLKCMU_CPUCL2_CLUSTER,
	V620_EVT0_MUX_CLKCMU_CPUCL2_CLUSTER,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_cpucl2_switch[] = {
	V620_EVT0_CLKCMU_CPUCL2_SWITCH,
	V620_EVT0_MUX_CLKCMU_CPUCL2_SWITCH,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_dpuf1_nocp[] = {
	V620_EVT0_DIV_CLK_DPUF1_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_dpuf1_noc[] = {
	V620_EVT0_CLKCMU_DPUF1_NOC,
	V620_EVT0_MUX_CLKCMU_DPUF1_NOC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_nocl0_sci_div2[] = {
	V620_EVT0_DIV_CLK_NOCL0_SCI_DIV2,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_nocl0_noc[] = {
	V620_EVT0_CLKCMU_NOCL0_NOC,
	V620_EVT0_MUX_CLKCMU_NOCL0_NOC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric0_usi00_usi[] = {
	V620_EVT0_DIV_CLK_PERIC0_USI00_USI,
	V620_EVT0_MUX_CLK_PERIC0_USI00_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_peric0_ip[] = {
	V620_EVT0_CLKCMU_PERIC0_IP,
	V620_EVT0_MUX_CLKCMU_PERIC0_IP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric0_usi01_usi[] = {
	V620_EVT0_DIV_CLK_PERIC0_USI01_USI,
	V620_EVT0_MUX_CLK_PERIC0_USI01_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric0_usi02_usi[] = {
	V620_EVT0_DIV_CLK_PERIC0_USI02_USI,
	V620_EVT0_MUX_CLK_PERIC0_USI02_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric0_usi07_usi[] = {
	V620_EVT0_DIV_CLK_PERIC0_USI07_USI,
	V620_EVT0_MUX_CLK_PERIC0_USI07_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric0_usi03_usi[] = {
	V620_EVT0_DIV_CLK_PERIC0_USI03_USI,
	V620_EVT0_MUX_CLK_PERIC0_USI03_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric1_usi09_usi[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI09_USI,
	V620_EVT0_MUX_CLK_PERIC1_USI09_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_clkcmu_peric1_ip[] = {
	V620_EVT0_CLKCMU_PERIC1_IP,
	V620_EVT0_MUX_CLKCMU_PERIC1_IP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric1_usi11_usi[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI11_USI,
	V620_EVT0_MUX_CLK_PERIC1_USI11_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric1_usi12_usi[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI12_USI,
	V620_EVT0_MUX_CLK_PERIC1_USI12_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric1_usi13_usi[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI13_USI,
	V620_EVT0_MUX_CLK_PERIC1_USI13_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric1_usi14_usi[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI14_USI,
	V620_EVT0_MUX_CLK_PERIC1_USI14_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric1_usi15_usi[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI15_USI,
	V620_EVT0_MUX_CLK_PERIC1_USI15_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric1_usi16_usi[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI16_USI,
	V620_EVT0_MUX_CLK_PERIC1_USI16_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_peric1_usi17_usi[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI17_USI,
	V620_EVT0_MUX_CLK_PERIC1_USI17_USI,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_sfi_pllclkout[] = {
	V620_EVT0_MUX_CLK_SFI_PLLCLKOUT,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_sfi_usi20[] = {
	V620_EVT0_MUX_CLK_SFI_USI20,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_sfi_usi19[] = {
	V620_EVT0_MUX_CLK_SFI_USI19,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_div_clk_sfi_cpu_cntclk[] = {
	V620_EVT0_DIV_CLK_SFI_CPU_CNTCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_sfi_usi21[] = {
	V620_EVT0_MUX_CLK_SFI_USI21,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_sfi_can0[] = {
	V620_EVT0_MUX_CLK_SFI_CAN0,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_sfi_can1[] = {
	V620_EVT0_MUX_CLK_SFI_CAN1,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_cpucl0_core[] = {
	V620_EVT0_MUX_CLK_CPUCL0_CORE,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_mux_clk_cpucl2_core[] = {
	V620_EVT0_MUX_CLK_CPUCL2_CORE,
};
/* COMMON VCLK -> Clock Node List */
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_aud[] = {
	V620_EVT0_MUX_CLK_AUD_AUDIF,
	V620_EVT0_MUX_CLK_AUD_AVB,
	V620_EVT0_PLL_AVB,
	V620_EVT0_DIV_CLK_AUD_NOC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_cmu[] = {
	V620_EVT0_MUX_CLKCMU_MIF_SWITCH,
	V620_EVT0_CLKCMU_G3D_SWITCH,
	V620_EVT0_CLKCMU_DNC_NOC,
	V620_EVT0_CLKCMU_SDMA_NOC,
	V620_EVT0_CLKCMU_GNPU_NOC,
	V620_EVT0_CLKCMU_DSP_NOC,
	V620_EVT0_CLKCMU_ACC_NOC,
	V620_EVT0_MUX_CLKCMU_ACC_NOC,
	V620_EVT0_CLKCMU_ISP_NOC,
	V620_EVT0_MUX_CLKCMU_ISP_NOC,
	V620_EVT0_CLKCMU_SNW_NOC,
	V620_EVT0_MUX_CLKCMU_SNW_NOC,
	V620_EVT0_CLKCMU_TAA_NOC,
	V620_EVT0_MUX_CLKCMU_TAA_NOC,
	V620_EVT0_CLKCMU_DPUB_NOC,
	V620_EVT0_MUX_CLKCMU_DPUB_NOC,
	V620_EVT0_CLKCMU_DPUF0_NOC,
	V620_EVT0_MUX_CLKCMU_DPUF0_NOC,
	V620_EVT0_CLKCMU_NOCL1_NOC,
	V620_EVT0_CLKCMU_NOCL2_NOC,
	V620_EVT0_CLKCMU_MFC_MFC,
	V620_EVT0_CLKCMU_MIF_NOCP,
	V620_EVT0_MUX_CLKCMU_MIF_NOCP,
	V620_EVT0_DIV_CLKCMU_CMU_BOOST,
	V620_EVT0_MUX_CLKCMU_CMU_BOOST,
	V620_EVT0_CLKCMU_HSI1_NOC,
	V620_EVT0_MUX_CLKCMU_HSI1_NOC,
	V620_EVT0_CLKCMU_HSI0_NOC,
	V620_EVT0_MUX_CLKCMU_HSI0_NOC,
	V620_EVT0_CLKCMU_DPTX_NOC,
	V620_EVT0_MUX_CLKCMU_DPTX_NOC,
	V620_EVT0_CLKCMU_HSI2_NOC_UFS,
	V620_EVT0_MUX_CLKCMU_HSI2_NOC_UFS,
	V620_EVT0_CLKCMU_MFC_WFD,
	V620_EVT0_CLKCMU_SSP_NOC,
	V620_EVT0_MUX_CLKCMU_SSP_NOC,
	V620_EVT0_CLKCMU_M2M_NOC,
	V620_EVT0_MUX_CLKCMU_M2M_NOC,
	V620_EVT0_CLKCMU_M2M_JPEG,
	V620_EVT0_MUX_CLKCMU_M2M_JPEG,
	V620_EVT0_CLKCMU_HSI2_NOC,
	V620_EVT0_MUX_CLKCMU_HSI2_NOC,
	V620_EVT0_CLKCMU_G3D_NOCP,
	V620_EVT0_MUX_CLKCMU_G3D_NOCP,
	V620_EVT0_PLL_SHARED1,
	V620_EVT0_CLKCMU_CPUCL0_DBG,
	V620_EVT0_CLKCMU_HSI1_MMC_CARD,
	V620_EVT0_MUX_CLKCMU_HSI1_MMC_CARD,
	V620_EVT0_CLKCMU_HSI1_USBDRD,
	V620_EVT0_MUX_CLKCMU_HSI1_USBDRD,
	V620_EVT0_CLKCMU_HSI2_UFS_EMBD,
	V620_EVT0_MUX_CLKCMU_HSI2_UFS_EMBD,
	V620_EVT0_PLL_SHARED4,
	V620_EVT0_CLKCMU_PERIC0_NOC,
	V620_EVT0_MUX_CLKCMU_PERIC0_NOC,
	V620_EVT0_CLKCMU_PERIC1_NOC,
	V620_EVT0_MUX_CLKCMU_PERIC1_NOC,
	V620_EVT0_PLL_SHARED2,
	V620_EVT0_PLL_SHARED0,
	V620_EVT0_PLL_SHARED5,
	V620_EVT0_PLL_MMC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_hsi2[] = {
	V620_EVT0_DIV_CLK_HSI2_ETHERNET,
	V620_EVT0_DIV_CLK_HSI2_ETHERNET_PTP,
	V620_EVT0_PLL_ETH,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_s2d[] = {
	V620_EVT0_MUX_CLK_S2D_CORE,
	V620_EVT0_PLL_MIF_S2D,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_apm[] = {
	V620_EVT0_MUX_CLK_APM_NOC,
	V620_EVT0_DIV_CLK_APM_DBGCORE_NOC,
	V620_EVT0_MUX_CLK_APM_DBGCORE_NOC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_cpucl0[] = {
	V620_EVT0_DIV_CLK_CLUSTER0_ATCLK,
	V620_EVT0_DIV_CLK_CLUSTER0_PERIPHCLK,
	V620_EVT0_DIV_CLK_CLUSTER0_MPCLK,
	V620_EVT0_DIV_CLK_CLUSTER0_PCLK,
	V620_EVT0_DIV_CLK_CPUCL0_NOCP,
	V620_EVT0_MUX_CLK_CPUCL0_CLUSTER,
	V620_EVT0_DIV_CLK_CPUCL0_DBG_PCLKDBG,
	V620_EVT0_DIV_CLK_CPUCL0_DBG_NOC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_cpucl2[] = {
	V620_EVT0_DIV_CLK_CLUSTER2_PCLK,
	V620_EVT0_DIV_CLK_CLUSTER2_PERIPHCLK,
	V620_EVT0_DIV_CLK_CLUSTER2_ATCLK,
	V620_EVT0_DIV_CLK_CLUSTER2_MPCLK,
	V620_EVT0_DIV_CLK_CPUCL2_NOCP,
	V620_EVT0_MUX_CLK_CPUCL2_CLUSTER,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_g3d[] = {
	V620_EVT0_MUX_CLK_G3D_NOC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_hsi1[] = {
	V620_EVT0_MUX_CLK_HSI1_USBDRD,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_peric0[] = {
	V620_EVT0_DIV_CLK_PERIC0_USI_I2C,
	V620_EVT0_MUX_CLK_PERIC0_USI_I2C,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_peric1[] = {
	V620_EVT0_DIV_CLK_PERIC1_USI_I2C,
	V620_EVT0_MUX_CLK_PERIC1_USI_I2C,
	V620_EVT0_DIV_CLK_PERIC1_I3C,
	V620_EVT0_MUX_CLK_PERIC1_I3C,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_sfi[] = {
	V620_EVT0_MUX_CLK_SFI_USI18,
	V620_EVT0_MUX_CLK_SFI_XSPI,
	V620_EVT0_DIV_CLK_SFI_NOCD,
	V620_EVT0_DIV_CLK_SFI_CPU_ACLK,
	V620_EVT0_DIV_CLK_SFI_CPU_ATCLK,
	V620_EVT0_DIV_CLK_SFI_CPU_PCLKDBG,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_acc[] = {
	V620_EVT0_DIV_CLK_ACC_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_dnc[] = {
	V620_EVT0_DIV_CLK_DNC_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_dpub[] = {
	V620_EVT0_DIV_CLK_DPUB_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_dpuf[] = {
	V620_EVT0_DIV_CLK_DPUF_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_dsp[] = {
	V620_EVT0_DIV_CLK_DSP_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_gnpu[] = {
	V620_EVT0_DIV_CLK_GNPU_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_hsi0[] = {
	V620_EVT0_DIV_CLK_HSI0_PCIE_APB,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_isp[] = {
	V620_EVT0_DIV_CLK_ISP_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_m2m[] = {
	V620_EVT0_DIV_CLK_M2M_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_mfc[] = {
	V620_EVT0_DIV_CLK_MFC_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_misc[] = {
	V620_EVT0_DIV_CLK_MISC_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_nocl0[] = {
	V620_EVT0_DIV_CLK_NOCL0_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_nocl1[] = {
	V620_EVT0_DIV_CLK_NOCL1_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_nocl2[] = {
	V620_EVT0_DIV_CLK_NOCL2_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_sdma[] = {
	V620_EVT0_DIV_CLK_SDMA_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_snw[] = {
	V620_EVT0_DIV_CLK_SNW_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_ssp[] = {
	V620_EVT0_DIV_CLK_SSP_NOCP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_blk_taa[] = {
	V620_EVT0_DIV_CLK_TAA_NOCP,
};
/* GATE VCLK -> Clock Node List */
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_acc[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_SLH_AXI_MI_P_ACC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d_acc[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_LH_AXI_SI_D_ACC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_acc[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_SYSREG_ACC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_l_acc_snw[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_LH_AST_SI_L_ACC_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_acc[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_D_TZPC_ACC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_l_acc_taa[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_LH_AST_SI_L_ACC_TAA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d_acc[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_PPMU_D_ACC_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_ACC_UID_PPMU_D_ACC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_orbmch[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_QE_D_ORBMCH_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_ACC_UID_QE_D_ORBMCH_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_acc[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_SYSMMU_S0_PMMU0_ACC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apb_async_isppre0[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_APB_ASYNC_ISPPRE0_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_isppre[] = {
	V620_EVT0_GOUT_BLK_ACC_UID_VGEN_D_ISPPRE_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_acc_cmu_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_ACC_CMU_ACC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_d_orbmch[] = {
	V620_EVT0_CLK_BLK_ACC_UID_VGEN_LITE_D_ORBMCH_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_csis_link_mux2x4[] = {
	V620_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX2X4_IPCLKPORT_CSIS3_I_PCLK,
	V620_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX2X4_IPCLKPORT_CSIS4_I_PCLK,
	V620_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX2X4_IPCLKPORT_CSIS2_I_PCLK,
	V620_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX2X4_IPCLKPORT_CSIS5_I_PCLK,
	V620_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX2X4_IPCLKPORT_CSIS2_I_ACLK,
	V620_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX2X4_IPCLKPORT_CSIS3_I_ACLK,
	V620_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX2X4_IPCLKPORT_CSIS4_I_ACLK,
	V620_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX2X4_IPCLKPORT_CSIS5_I_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_isppre[] = {
	V620_EVT0_CLK_BLK_ACC_UID_QE_D_ISPPRE_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_ACC_UID_QE_D_ISPPRE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_isppre[] = {
	V620_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_CLK,
	V620_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_CLK_CSIS,
	V620_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_CLK_CSIS,
	V620_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_CLK_SENSORVSYNC,
	V620_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_CLK_SENSORVSYNC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_XIU_D0_ACC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_XIU_D1_ACC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_SYSMMU_S0_ACC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_us_128_256_d0_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_US_128_256_D0_ACC_IPCLKPORT_MAINCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_BICS_APB_P_ACC_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_SFMPU_P_ACC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d2_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_XIU_D2_ACC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d0_acc[] = {
	V620_EVT0_CLK_BLK_ACC_UID_BICM_AXI_D0_ACC_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_ACC_UID_BICM_AXI_D0_ACC_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_orbmch[] = {
	V620_EVT0_CLK_BLK_ACC_UID_ORBMCH_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d_alive[] = {
	V620_EVT0_GOUT_BLK_APM_UID_LH_AXI_SI_D_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_alive[] = {
	V620_EVT0_GOUT_BLK_APM_UID_SLH_AXI_MI_P_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wdt_apm[] = {
	V620_EVT0_GOUT_BLK_APM_UID_WDT_APM_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_apm[] = {
	V620_EVT0_GOUT_BLK_APM_UID_SYSREG_APM_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_apm_ap[] = {
	V620_EVT0_GOUT_BLK_APM_UID_MAILBOX_APM_AP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_pmu_alive[] = {
	V620_EVT0_GOUT_BLK_APM_UID_APBIF_PMU_ALIVE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_intmem_alive[] = {
	V620_EVT0_GOUT_BLK_APM_UID_INTMEM_ALIVE_IPCLKPORT_I_ACLK,
	V620_EVT0_GOUT_BLK_APM_UID_INTMEM_ALIVE_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pmu_intr_gen[] = {
	V620_EVT0_GOUT_BLK_APM_UID_PMU_INTR_GEN_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_dp_alive[] = {
	V620_EVT0_GOUT_BLK_APM_UID_XIU_DP_ALIVE_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apm_cmu_apm[] = {
	V620_EVT0_CLK_BLK_APM_UID_APM_CMU_APM_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_grebe_apm[] = {
	V620_EVT0_GOUT_BLK_APM_UID_GREBE_APM_IPCLKPORT_HCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_gpio_alive[] = {
	V620_EVT0_GOUT_BLK_APM_UID_APBIF_GPIO_ALIVE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_top_rtc[] = {
	V620_EVT0_GOUT_BLK_APM_UID_APBIF_TOP_RTC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_apm[] = {
	V620_EVT0_GOUT_BLK_APM_UID_D_TZPC_APM_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap_dbgcore[] = {
	V620_EVT0_GOUT_BLK_APM_UID_MAILBOX_AP_DBGCORE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_rtc[] = {
	V620_EVT0_GOUT_BLK_APM_UID_APBIF_RTC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_rom_crc32_host[] = {
	V620_EVT0_GOUT_BLK_APM_UID_ROM_CRC32_HOST_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_APM_UID_ROM_CRC32_HOST_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_apm_sfi0[] = {
	V620_EVT0_GOUT_BLK_APM_UID_MAILBOX_APM_SFI0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_apm_sfi1[] = {
	V620_EVT0_GOUT_BLK_APM_UID_MAILBOX_APM_SFI1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_spmi_master_pmic[] = {
	V620_EVT0_CLK_BLK_APM_UID_SPMI_MASTER_PMIC_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_APM_UID_SPMI_MASTER_PMIC_IPCLKPORT_I_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_rom_crc32_hcu[] = {
	V620_EVT0_CLK_BLK_APM_UID_ROM_CRC32_HCU_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_hw_scandump_clkstop_ctrl[] = {
	V620_EVT0_CLK_BLK_APM_UID_HW_SCANDUMP_CLKSTOP_CTRL_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apm_dta[] = {
	V620_EVT0_CLK_BLK_APM_UID_APM_DTA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_alive[] = {
	V620_EVT0_CLK_BLK_APM_UID_SYSMMU_S0_PMMU0_ALIVE_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_apm[] = {
	V620_EVT0_CLK_BLK_APM_UID_VGEN_LITE_APM_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_alive[] = {
	V620_EVT0_CLK_BLK_APM_UID_SYSMMU_S0_ALIVE_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_alive[] = {
	V620_EVT0_CLK_BLK_APM_UID_SFMPU_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_spc_apm[] = {
	V620_EVT0_CLK_BLK_APM_UID_SPC_APM_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_intcomb_vgpio2pmu[] = {
	V620_EVT0_CLK_BLK_APM_UID_APBIF_INTCOMB_VGPIO2PMU_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_intcomb_vgpio2apm[] = {
	V620_EVT0_CLK_BLK_APM_UID_APBIF_INTCOMB_VGPIO2APM_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_intcomb_vgpio2ap[] = {
	V620_EVT0_CLK_BLK_APM_UID_APBIF_INTCOMB_VGPIO2AP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d_alive[] = {
	V620_EVT0_CLK_BLK_APM_UID_XIU_D_ALIVE_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_intmem[] = {
	V620_EVT0_CLK_BLK_APM_UID_SFMPU_INTMEM_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_alive[] = {
	V620_EVT0_CLK_BLK_APM_UID_BIC_APB_ALIVE_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ip_alive[] = {
	V620_EVT0_CLK_BLK_APM_UID_LH_AXI_SI_IP_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_dbgcore_alive[] = {
	V620_EVT0_CLK_BLK_APM_UID_LH_AXI_MI_ID_DBGCORE_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pmu[] = {
	V620_EVT0_CLK_BLK_APM_UID_PMU_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_asyncahb_mi_apm[] = {
	V620_EVT0_CLK_BLK_APM_UID_ASYNCAHB_MI_APM_IPCLKPORT_HCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_aud_cmu_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_AUD_CMU_AUD_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d_aud[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_LH_AXI_SI_D_AUD_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d_aud[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_PPMU_D_AUD_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_AUD_UID_PPMU_D_AUD_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_aud[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_SYSREG_AUD_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_abox[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF0,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF1,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF3,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF2,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK_IRQ,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK_IRQ,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_CNT,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF5,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF6,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI5,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI5_B,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI5_B,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_DAP,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP0,
	V620_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP1,
	V620_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF8,
	V620_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF9,
	V620_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIFS0,
	V620_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIFS1,
	V620_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF4,
	V620_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF7,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpio_aud[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_GPIO_AUD_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_axi_us_32to128[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_AXI_US_32TO128_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_aud[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_SLH_AXI_MI_P_AUD_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wdt_aud0[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_WDT_AUD0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dftmux_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_DFTMUX_AUD_IPCLKPORT_AUD_CODEC_MCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_aud[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_SYSMMU_S0_PMMU0_AUD_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ad_apb_smmu_aud_s1_ns[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_AD_APB_SMMU_AUD_S1_NS_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wdt_aud1[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_WDT_AUD1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_aud[] = {
	V620_EVT0_GOUT_BLK_AUD_UID_D_TZPC_AUD_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_LH_AXI_MI_IP_AUD_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ip_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_LH_AXI_SI_IP_AUD_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_XIU_D_AUD_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_SYSMMU_S0_AUD_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_aud0[] = {
	V620_EVT0_CLK_BLK_AUD_UID_VGEN_AUD0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_aud1[] = {
	V620_EVT0_CLK_BLK_AUD_UID_VGEN_AUD1_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_VGEN_LITE_AUD_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon_pll_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_MON_CLK10,
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_MON_CLK00,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon_pll_avb[] = {
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_MON_CLK00,
	V620_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_MON_CLK10,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p0_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_BICS_APB_P0_AUD_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p1_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_BICS_APB_P1_AUD_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_aud[] = {
	V620_EVT0_CLK_BLK_AUD_UID_SFMPU_AUD_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_aud2[] = {
	V620_EVT0_CLK_BLK_AUD_UID_VGEN_AUD2_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_add_ch_clk[] = {
	V620_EVT0_CLK_BLK_CMU_UID_ADD_CH_CLK_IPCLKPORT_ADD_CH_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cmu[] = {
	V620_EVT0_CLK_BLK_CMU_UID_SLH_AXI_MI_P_CMU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_cmu[] = {
	V620_EVT0_CLK_BLK_CMU_UID_D_TZPC_CMU_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfr_apbif_cmu[] = {
	V620_EVT0_CLK_BLK_CMU_UID_SFR_APBIF_CMU_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon0_pll_shared[] = {
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK00,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK01,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK02,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK03,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK10,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK11,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK12,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK13,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon1_pll_shared[] = {
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK02,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK12,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK03,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK13,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK00,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK01,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK10,
	V620_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK11,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_SYSREG_CPUCL0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cssys[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_CSSYS_IPCLKPORT_PCLKDBG,
	V620_EVT0_GOUT_BLK_CPUCL0_UID_CSSYS_IPCLKPORT_ATCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_t_bdu[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_T_BDU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_it0_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_IT0_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_it1_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_IT1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_it2_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_IT2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_SLH_AXI_MI_P_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_chi_si_d_cluster0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_CHI_SI_D_CLUSTER0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_it0_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_SI_IT0_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_it1_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_SI_IT1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_it2_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_SI_IT2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_it3_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_SI_IT3_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cpucl0_cmu_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_CPUCL0_CMU_CPUCL0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cluster0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_PERIPHCLK,
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_SCLK,
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_ATCLK,
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_MPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_D_TZPC_CPUCL0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ig_cssys[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_SI_IG_CSSYS_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ig_cssys[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_MI_IG_CSSYS_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_p_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_XIU_P_CPUCL0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_dp_cssys[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_XIU_DP_CSSYS_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_g_cssys[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_SI_G_CSSYS_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apb_async_p_cssys_0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_APB_ASYNC_P_CSSYS_0_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ig_etr[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_SI_IG_ETR_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ig_etr[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_MI_IG_ETR_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bps_cpucl0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_BPS_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt0_sfi[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT0_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt1_sfi[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT1_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt2_sfi[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT2_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt3_sfi[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT3_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_adm_apb_g_cluster0[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_ADM_APB_G_CLUSTER0_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ig_stm[] = {
	V620_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_MI_IG_STM_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_PPMU_CPUCL0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_CPUCL0_UID_PPMU_CPUCL0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt1_cpucl2_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_LH_ATB_MI_LT1_CPUCL2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt0_cpucl2_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_LH_ATB_MI_LT0_CPUCL2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cpucl0_con[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_CPUCL0_CON_IPCLKPORT_I_PERIPHCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_iri_gic_cluster0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_LH_AST_MI_L_IRI_GIC_CLUSTER0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_l_icc_cluster0_gic[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_LH_AST_SI_L_ICC_CLUSTER0_GIC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ig_stm[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_LH_AXI_SI_IG_STM_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_g_dbgcore_alive_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_SLH_AXI_MI_G_DBGCORE_ALIVE_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cluster0_nocl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_SLH_AXI_SI_P_CLUSTER0_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_it3_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_LH_ATB_MI_IT3_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon_pll_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_MON_CLK00,
	V620_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_MON_CLK10,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_secjtag[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_SECJTAG_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qos_overr_d_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_QOS_OVERR_D_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_SFMPU_P_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_p_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_BICM_AXI_P_CPUCL0_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_CPUCL0_UID_BICM_AXI_P_CPUCL0_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_BICS_APB_P_CPUCL0_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_p1_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL0_UID_XIU_P1_CPUCL0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cluster2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLUSTER2_IPCLKPORT_SCLK,
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLUSTER2_IPCLKPORT_ATCLK,
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLUSTER2_IPCLKPORT_PERIPHCLK,
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLUSTER2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cpucl2_cmu_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_CPUCL2_CMU_CPUCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_adm_apb_g_cluster2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_ADM_APB_G_CLUSTER2_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_PPMU_CPUCL2_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_CPUCL2_UID_PPMU_CPUCL2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_SLH_AXI_MI_P_CPUCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cpucl2_con[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_CPUCL2_CON_IPCLKPORT_I_PERIPHCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_iri_gic_cluster2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_LH_AST_MI_L_IRI_GIC_CLUSTER2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_l_icc_cluster2_gic[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_LH_AST_SI_L_ICC_CLUSTER2_GIC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cluster2_nocl0[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_SLH_AXI_SI_P_CLUSTER2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_chi_si_d_cluster2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_LH_CHI_SI_D_CLUSTER2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_lt0_cpucl2_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_LH_ATB_SI_LT0_CPUCL2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_lt1_cpucl2_cpucl0[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_LH_ATB_SI_LT1_CPUCL2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_SYSREG_CPUCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_D_TZPC_CPUCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon_pll_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_MON_CLK00,
	V620_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_MON_CLK10,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qos_overr_d_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_QOS_OVERR_D_CPUCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_BICS_APB_P_CPUCL2_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_SFMPU_P_CPUCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_p_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_BICM_AXI_P_CPUCL2_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_CPUCL2_UID_BICM_AXI_P_CPUCL2_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_p_cpucl2[] = {
	V620_EVT0_CLK_BLK_CPUCL2_UID_XIU_P_CPUCL2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dbgcore_cmu_dbgcore[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_DBGCORE_CMU_DBGCORE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_grebeintegration_dbgcore[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_GREBEINTEGRATION_DBGCORE_IPCLKPORT_HCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_id_dbgcore_alive[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_LH_AXI_SI_ID_DBGCORE_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip_alive[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_LH_AXI_MI_IP_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_g_scan2dram_alive_mif0[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_SLH_AXI_SI_G_SCAN2DRAM_ALIVE_MIF0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_dbgcore[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_SYSREG_DBGCORE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_g_dbgcore_alive_cpucl0[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_SLH_AXI_SI_G_DBGCORE_ALIVE_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wdt_dbgcore[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_WDT_DBGCORE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ahb_busmatrix_dbgcore[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_AHB_BUSMATRIX_DBGCORE_IPCLKPORT_HCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xhb_dbgcore[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_XHB_DBGCORE_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_cssys_alive[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_APBIF_CSSYS_ALIVE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_dbgcore_core[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_SYSREG_DBGCORE_CORE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_dbgcore[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_D_TZPC_DBGCORE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_apbif_s2d_dbgcore[] = {
	V620_EVT0_CLK_BLK_DBGCORE_UID_APBIF_S2D_DBGCORE_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dnc_cmu_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_DNC_CMU_DNC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_D_TZPC_DNC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_gnpu_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD_CTRL_GNPU_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld3_mmu_sdma_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD3_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_dnc_gnpu[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_LD_CTRL_DNC_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_sram_dnc_sdma[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_LD_SRAM_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_ipdnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_ID_IPDNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld2_mmu_sdma_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD2_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_SYSREG_DNC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_trex_d_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_TREX_D_DNC_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_DNC_UID_TREX_D_DNC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_dram_dsp_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD_DRAM_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld1_mmu_sdma_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD1_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_dram_gnpu_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD_DRAM_GNPU_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_PPMU_D0_DNC_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_DNC_UID_PPMU_D0_DNC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_PPMU_D1_DNC_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_DNC_UID_PPMU_D1_DNC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d2_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_PPMU_D2_DNC_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_DNC_UID_PPMU_D2_DNC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d3_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_PPMU_D3_DNC_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_DNC_UID_PPMU_D3_DNC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d4_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_PPMU_D4_DNC_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_DNC_UID_PPMU_D4_DNC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_id_ipdnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_ID_IPDNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_dnc[] = {
	V620_EVT0_GOUT_BLK_DNC_UID_SLH_AXI_MI_P_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_dnc_sdma[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_AXI_SI_LD_CTRL_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_dnc_dsp[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_AXI_SI_LD_CTRL_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ip_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_IP_DNC_IPCLKPORT_CLK,
	V620_EVT0_CLK_BLK_DNC_UID_IP_DNC_IPCLKPORT_DAPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_vgen_lite_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_AS_APB_VGEN_LITE_DNC_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_VGEN_LITE_DNC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_lp_dnc_dsp[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SLH_AXI_SI_LP_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_lp_dnc_sdma[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SLH_AXI_SI_LP_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_adm_dap_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_ADM_DAP_DNC_IPCLKPORT_DAPCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_PMMU0_DNC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_PMMU1_DNC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu2_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_PMMU2_DNC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu3_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_PMMU3_DNC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s1_pmmu0_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SYSMMU_S1_PMMU0_DNC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld0_mmu_sdma_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_AXI_MI_LD0_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_dsp_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_AXI_MI_LD_CTRL_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_lp_dnc_gnpu[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SLH_AXI_SI_LP_DNC_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d0_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_TAXI_SI_D0_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d1_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_TAXI_SI_D1_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d2_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_TAXI_SI_D2_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d3_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_TAXI_SI_D3_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_DNC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s1_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SYSMMU_S1_DNC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_XIU_D_DNC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_lg_dnc_nocl0[] = {
	V620_EVT0_CLK_BLK_DNC_UID_LH_AST_SI_LG_DNC_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p0_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SFMPU_P0_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p1_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_SFMPU_P1_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_s0_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_BIC_APB_S0_DNC_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_s1_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_BIC_APB_S1_DNC_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_baaw_p_dnc[] = {
	V620_EVT0_CLK_BLK_DNC_UID_BAAW_P_DNC_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_dptx[] = {
	V620_EVT0_GOUT_BLK_DPTX_UID_SYSREG_DPTX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_dptx[] = {
	V620_EVT0_GOUT_BLK_DPTX_UID_SLH_AXI_MI_P_DPTX_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_dptx[] = {
	V620_EVT0_GOUT_BLK_DPTX_UID_D_TZPC_DPTX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dptx_cmu_dptx[] = {
	V620_EVT0_CLK_BLK_DPTX_UID_DPTX_CMU_DPTX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dp_link0[] = {
	V620_EVT0_CLK_BLK_DPTX_UID_DP_LINK0_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_DPTX_UID_DP_LINK0_IPCLKPORT_I_DP_GTC_CLK,
	V620_EVT0_CLK_BLK_DPTX_UID_DP_LINK0_IPCLKPORT_I_DP_OSC_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dwc_usbc31dptxphy_upcs_x4_ns_x1_x4_pipe_wrap_inst0[] = {
	V620_EVT0_CLK_BLK_DPTX_UID_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST0_IPCLKPORT_USBC31_DP_CTRL_PCLK,
	V620_EVT0_CLK_BLK_DPTX_UID_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST0_IPCLKPORT_TCA_APB_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_dptx[] = {
	V620_EVT0_CLK_BLK_DPTX_UID_SFMPU_DPTX_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_dptx[] = {
	V620_EVT0_CLK_BLK_DPTX_UID_BIC_DPTX_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dpub_cmu_dpub[] = {
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_CMU_DPUB_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_dpub[] = {
	V620_EVT0_GOUT_BLK_DPUB_UID_SYSREG_DPUB_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_dpub[] = {
	V620_EVT0_GOUT_BLK_DPUB_UID_SLH_AXI_MI_P_DPUB_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_dpub[] = {
	V620_EVT0_GOUT_BLK_DPUB_UID_D_TZPC_DPUB_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ad_apb_decon_main_dpub0[] = {
	V620_EVT0_CLK_BLK_DPUB_UID_AD_APB_DECON_MAIN_DPUB0_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dpub[] = {
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ACLK_DECON_DPUB0,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ALVCLK_DSIM0_DPUB0,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ALVCLK_DSIM1_DPUB0,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ALVCLK_DSIM0_DPUB1,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ALVCLK_DSIM1_DPUB1,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ACLK_DECON_DPUB1,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_OSCCLK_DSIM0_DPUB0,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_OSCCLK_DSIM0_DPUB1,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_OSCCLK_DSIM1_DPUB0,
	V620_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_OSCCLK_DSIM1_DPUB1,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_dpub[] = {
	V620_EVT0_CLK_BLK_DPUB_UID_SFMPU_DPUB_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p0_dpub[] = {
	V620_EVT0_CLK_BLK_DPUB_UID_BICS_APB_P0_DPUB_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dpuf_cmu_dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_DPUF_CMU_DPUF_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_SYSREG_DPUF_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu3_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_SYSMMU_S0_PMMU3_DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu2_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_SYSMMU_S0_PMMU2_DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_SYSMMU_S0_PMMU1_DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_SYSMMU_S0_PMMU0_DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_dpuf1[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_PPMU_D0_DPUF1_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_DPUF_UID_PPMU_D0_DPUF1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_dpuf0[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_PPMU_D1_DPUF0_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_DPUF_UID_PPMU_D1_DPUF0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_dpuf1[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_PPMU_D1_DPUF1_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_DPUF_UID_PPMU_D1_DPUF1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_dpuf0[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_PPMU_D0_DPUF0_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_DPUF_UID_PPMU_D0_DPUF0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_SLH_AXI_MI_P_DPUF_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d0_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_LH_AXI_SI_D0_DPUF_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d1_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_LH_AXI_SI_D1_DPUF_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_D_TZPC_DPUF_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dpuf[] = {
	V620_EVT0_GOUT_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_DPUF0,
	V620_EVT0_GOUT_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_DPUF1,
	V620_EVT0_GOUT_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_VOTF0,
	V620_EVT0_CLK_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_SRAMC,
	V620_EVT0_CLK_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_VOTF1,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_XIU_D0_DPUF_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_XIU_D1_DPUF_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ad_apb_dpuf0_dma[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_AD_APB_DPUF0_DMA_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_SYSMMU_S0_DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_SFMPU_DPUF_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_BIC_DPUF_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen0_0dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_VGEN0_0DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen0_1dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_VGEN0_1DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen1_0dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_VGEN1_0DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen1_1dpuf[] = {
	V620_EVT0_CLK_BLK_DPUF_UID_VGEN1_1DPUF_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dpuf1_cmu_dpuf1[] = {
	V620_EVT0_CLK_BLK_DPUF1_UID_DPUF1_CMU_DPUF1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dsp_cmu_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_DSP_CMU_DSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_SYSREG_DSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_dnc_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_LH_AXI_MI_LD_CTRL_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_sram_sdma_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_LH_AXI_MI_LD_SRAM_SDMA_DSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_lp_dnc_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_SLH_AXI_MI_LP_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_dsp_sdma[] = {
	V620_EVT0_CLK_BLK_DSP_UID_LH_AST_SI_LD_SRAM_DSP_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_dsp_dnc[] = {
	V620_EVT0_CLK_BLK_DSP_UID_LH_AXI_SI_LD_CTRL_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_dram_dsp_dnc[] = {
	V620_EVT0_CLK_BLK_DSP_UID_LH_AXI_SI_LD_DRAM_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_D_TZPC_DSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ip_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_IP_DSP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_SFMPU_P_DSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_s_dsp[] = {
	V620_EVT0_CLK_BLK_DSP_UID_BIC_APB_S_DSP_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_g3d_cmu_g3d[] = {
	V620_EVT0_CLK_BLK_G3D_UID_G3D_CMU_G3D_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_g3d[] = {
	V620_EVT0_GOUT_BLK_G3D_UID_SYSREG_G3D_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ip_g3d[] = {
	V620_EVT0_GOUT_BLK_G3D_UID_LH_AXI_SI_IP_G3D_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_g3d[] = {
	V620_EVT0_GOUT_BLK_G3D_UID_SLH_AXI_MI_P_G3D_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_g3d[] = {
	V620_EVT0_GOUT_BLK_G3D_UID_D_TZPC_G3D_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bg3d_pwrctl[] = {
	V620_EVT0_CLK_BLK_G3D_UID_BG3D_PWRCTL_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_adm_dap_g_g3d[] = {
	V620_EVT0_CLK_BLK_G3D_UID_ADM_DAP_G_G3D_IPCLKPORT_DAPCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpu[] = {
	V620_EVT0_CLK_BLK_G3D_UID_GPU_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_G3D_UID_GPU_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_asb_g3d[] = {
	V620_EVT0_CLK_BLK_G3D_UID_ASB_G3D_IPCLKPORT_PCLK_PPMU_G3D0,
	V620_EVT0_CLK_BLK_G3D_UID_ASB_G3D_IPCLKPORT_PCLK_PPMU_G3D1,
	V620_EVT0_CLK_BLK_G3D_UID_ASB_G3D_IPCLKPORT_PCLK_PPMU_G3D2,
	V620_EVT0_CLK_BLK_G3D_UID_ASB_G3D_IPCLKPORT_PCLK_PPMU_G3D3,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gnpu_cmu_gnpu[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_GNPU_CMU_GNPU_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_lp_dnc_gnpu[] = {
	V620_EVT0_GOUT_BLK_GNPU_UID_SLH_AXI_MI_LP_DNC_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_gnpu[] = {
	V620_EVT0_GOUT_BLK_GNPU_UID_D_TZPC_GNPU_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp0_sdma_gnpu[] = {
	V620_EVT0_GOUT_BLK_GNPU_UID_LH_AST_MI_LD_SRAM_RDRSP0_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_dnc_gnpu[] = {
	V620_EVT0_GOUT_BLK_GNPU_UID_LH_AXI_MI_LD_CTRL_DNC_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_dram_gnpu_dnc[] = {
	V620_EVT0_GOUT_BLK_GNPU_UID_LH_AXI_SI_LD_DRAM_GNPU_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_gnpu[] = {
	V620_EVT0_GOUT_BLK_GNPU_UID_SYSREG_GNPU_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_gnpu_dnc[] = {
	V620_EVT0_GOUT_BLK_GNPU_UID_LH_AXI_SI_LD_CTRL_GNPU_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp1_sdma_gnpu[] = {
	V620_EVT0_GOUT_BLK_GNPU_UID_LH_AST_MI_LD_SRAM_RDRSP1_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ip_npucore[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_IP_NPUCORE_IPCLKPORT_CLK_CORE,
	V620_EVT0_CLK_BLK_GNPU_UID_IP_NPUCORE_IPCLKPORT_CLK_XMAA0,
	V620_EVT0_CLK_BLK_GNPU_UID_IP_NPUCORE_IPCLKPORT_CLK_XMAA1,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp2_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_MI_LD_SRAM_RDRSP2_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp3_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_MI_LD_SRAM_RDRSP3_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp4_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_MI_LD_SRAM_RDRSP4_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp5_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_MI_LD_SRAM_RDRSP5_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_cstfifo_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_MI_LD_SRAM_CSTFIFO_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_wr0_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_WR0_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_wr1_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_WR1_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_wr2_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_WR2_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq0_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_RDREQ0_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq1_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_RDREQ1_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq2_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_RDREQ2_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq3_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_RDREQ3_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq5_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_RDREQ5_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq4_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_LH_AST_SI_LD_SRAM_RDREQ4_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_gnpu[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_SFMPU_P_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_s_gnpu[] = {
	V620_EVT0_CLK_BLK_GNPU_UID_BIC_APB_S_GNPU_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_hsi0_cmu_hsi0[] = {
	V620_EVT0_CLK_BLK_HSI0_UID_HSI0_CMU_HSI0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_si_d0_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_LH_ACEL_SI_D0_HSI0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_SLH_AXI_MI_P_HSI0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpio_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_GPIO_HSI0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_SYSREG_HSI0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_XIU_D0_HSI0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_PPMU_D0_HSI0_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_HSI0_UID_PPMU_D0_HSI0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_p0_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_XIU_P0_HSI0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pcie_gen5_2l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE1_DBI_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE1_MSTR_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE1_SLV_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE0_DBI_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE0_MSTR_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE0_SLV_ACLK_UG,
	V620_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_APB_PCLK,
	V620_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE0_DOORBELL_IPCLK,
	V620_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE1_DOORBELL_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pcie_ia_gen5a_2l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_IA_GEN5A_2L_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pcie_ia_gen5b_2l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_IA_GEN5B_2L_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_hsi0_0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_D_TZPC_HSI0_0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_XIU_D1_HSI0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_PPMU_D1_HSI0_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_HSI0_UID_PPMU_D1_HSI0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pcie_ia_gen5a_4l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_IA_GEN5A_4L_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pcie_ia_gen5b_4l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_IA_GEN5B_4L_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_si_d1_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_LH_ACEL_SI_D1_HSI0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pcie_gen5_4l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_DBI_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE1_MSTR_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_SLV_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE1_DBI_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_MSTR_ACLK_UG,
	V620_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE1_SLV_ACLK_UG,
	V620_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_APB_PCLK,
	V620_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_CXS_CLK,
	V620_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_DOORBELL_IPCLK,
	V620_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE1_DOORBELL_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_hsi0_1[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_D_TZPC_HSI0_1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_pcie_gen5a_2l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_VGEN_PCIE_GEN5A_2L_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_pcie_gen5a_4l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_VGEN_PCIE_GEN5A_4L_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_pcie_gen5b_2l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_VGEN_PCIE_GEN5B_2L_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_pcie_gen5b_4l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_VGEN_PCIE_GEN5B_4L_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_pcie_gen5a_2l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_QE_PCIE_GEN5A_2L_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI0_UID_QE_PCIE_GEN5A_2L_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_pcie_gen5a_4l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_QE_PCIE_GEN5A_4L_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI0_UID_QE_PCIE_GEN5A_4L_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_pcie_gen5b_2l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_QE_PCIE_GEN5B_2L_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI0_UID_QE_PCIE_GEN5B_2L_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_pcie_gen5b_4l[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_QE_PCIE_GEN5B_4L_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI0_UID_QE_PCIE_GEN5B_4L_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_SYSMMU_S0_PMMU0_HSI0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_hsi0[] = {
	V620_EVT0_GOUT_BLK_HSI0_UID_SYSMMU_S0_PMMU1_HSI0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d2_hsi0[] = {
	V620_EVT0_CLK_BLK_HSI0_UID_XIU_D2_HSI0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_hsi0[] = {
	V620_EVT0_CLK_BLK_HSI0_UID_SYSMMU_S0_HSI0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_async_apb_pcie_gen5_4l_sub_ctrl[] = {
	V620_EVT0_CLK_BLK_HSI0_UID_ASYNC_APB_PCIE_GEN5_4L_SUB_CTRL_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_hsi1_cmu_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_HSI1_CMU_HSI1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mmc_card[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_MMC_CARD_IPCLKPORT_SDCLKIN,
	V620_EVT0_GOUT_BLK_HSI1_UID_MMC_CARD_IPCLKPORT_I_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_SYSREG_HSI1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpio_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_GPIO_HSI1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_si_d_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_LH_ACEL_SI_D_HSI1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_SLH_AXI_MI_P_HSI1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_XIU_D0_HSI1_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_p_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_XIU_P_HSI1_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_PPMU_HSI1_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_HSI1_UID_PPMU_HSI1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_VGEN_LITE_HSI1_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_D_TZPC_HSI1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usb20drd_0[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_0_IPCLKPORT_BUS_CLK_EARLY,
	V620_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_0_IPCLKPORT_ACLK_PHYCTRL,
	V620_EVT0_CLK_BLK_HSI1_UID_USB20DRD_0_IPCLKPORT_I_USB20DRD_ALIVE_CLK,
	V620_EVT0_CLK_BLK_HSI1_UID_USB20DRD_0_IPCLKPORT_I_USB20DRD_REF_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usb20drd_1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_1_IPCLKPORT_BUS_CLK_EARLY,
	V620_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_1_IPCLKPORT_ACLK_PHYCTRL,
	V620_EVT0_CLK_BLK_HSI1_UID_USB20DRD_1_IPCLKPORT_I_USB20DRD_ALIVE_CLK,
	V620_EVT0_CLK_BLK_HSI1_UID_USB20DRD_1_IPCLKPORT_I_USB20DRD_REF_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usb30drd_0[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USB31PHY_USB31_CTRL_PCLK,
	V620_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBSUBCTL_I_APB_PCLK,
	V620_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBLINK_ACLK,
	V620_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBSUBCTL_ALIVE_CLK,
	V620_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBLINK_SUSPEND_CLK,
	V620_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBSUBCTL_U3REWA_ALV_CLK,
	V620_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBLINK_REF_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_hsi1[] = {
	V620_EVT0_GOUT_BLK_HSI1_UID_SYSMMU_S0_PMMU0_HSI1_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_hsi1[] = {
	V620_EVT0_CLK_BLK_HSI1_UID_XIU_D1_HSI1_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_hsi1[] = {
	V620_EVT0_CLK_BLK_HSI1_UID_SYSMMU_S0_HSI1_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ethernet0[] = {
	V620_EVT0_GOUT_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_SYS_CLK,
	V620_EVT0_CLK_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_PCLK_S0,
	V620_EVT0_CLK_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_PCLK_S1,
	V620_EVT0_CLK_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_CLK_PTP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ethernet1[] = {
	V620_EVT0_GOUT_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_SYS_CLK,
	V620_EVT0_CLK_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_PCLK_S0,
	V620_EVT0_CLK_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_PCLK_S1,
	V620_EVT0_CLK_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_CLK_PTP,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_si_d0_hsi2[] = {
	V620_EVT0_GOUT_BLK_HSI2_UID_LH_ACEL_SI_D0_HSI2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_ufs_embd0[] = {
	V620_EVT0_GOUT_BLK_HSI2_UID_QE_UFS_EMBD0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI2_UID_QE_UFS_EMBD0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_hsi2[] = {
	V620_EVT0_GOUT_BLK_HSI2_UID_SYSMMU_S0_PMMU0_HSI2_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ufs_embd0[] = {
	V620_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD0_IPCLKPORT_I_ACLK,
	V620_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD0_IPCLKPORT_I_CLK_UNIPRO,
	V620_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD0_IPCLKPORT_I_FMP_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_ufs_embd0[] = {
	V620_EVT0_GOUT_BLK_HSI2_UID_PPMU_UFS_EMBD0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI2_UID_PPMU_UFS_EMBD0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_SYSMMU_S0_HSI2_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d2_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_XIU_D2_HSI2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_ufs_embd0[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_VGEN_UFS_EMBD0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d3_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_XIU_D3_HSI2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_si_d1_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_LH_ACEL_SI_D1_HSI2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_ethernet0[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_VGEN_ETHERNET0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_ethernet1[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_VGEN_ETHERNET1_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_ethernet0[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_PPMU_ETHERNET0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI2_UID_PPMU_ETHERNET0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_ethernet1[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_PPMU_ETHERNET1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI2_UID_PPMU_ETHERNET1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_ethernet0[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_QE_ETHERNET0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI2_UID_QE_ETHERNET0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_ethernet1[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_QE_ETHERNET1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_HSI2_UID_QE_ETHERNET1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s1_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_SYSMMU_S1_HSI2_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s1_pmmu0_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_SYSMMU_S1_PMMU0_HSI2_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_XIU_D1_HSI2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_asyncapb_sysmmu_s0_hsi2_ns[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_ASYNCAPB_SYSMMU_S0_HSI2_NS_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_D_TZPC_HSI2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpio_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_GPIO_HSI2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_hsi2_cmu_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_HSI2_CMU_HSI2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_SLH_AXI_MI_P_HSI2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_SYSREG_HSI2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon_pll_eth[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_MON_CLK00,
	V620_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_MON_CLK10,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpio_hsi2ufs[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_GPIO_HSI2UFS_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d4_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_XIU_D4_HSI2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d5_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_XIU_D5_HSI2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_p_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_XIU_P_HSI2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d0_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_BICM_AXI_D0_HSI2_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_HSI2_UID_BICM_AXI_D0_HSI2_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d1_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_BICM_AXI_D1_HSI2_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_HSI2_UID_BICM_AXI_D1_HSI2_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p0_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_BICS_APB_P0_HSI2_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p1_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_BICS_APB_P1_HSI2_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_SFMPU_HSI2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_spc_hsi2[] = {
	V620_EVT0_CLK_BLK_HSI2_UID_SPC_HSI2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_isp[] = {
	V620_EVT0_GOUT_BLK_ISP_UID_VGEN_D_ISP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_D_TZPC_ISP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_SYSREG_ISP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_SYSMMU_S0_PMMU0_ISP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_PPMU_D_ISP_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_ISP_UID_PPMU_D_ISP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d0_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_QE_D0_ISP_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_ISP_UID_QE_D0_ISP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_ISP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_LH_AXI_SI_D_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_SLH_AXI_MI_P_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_isp0[] = {
	V620_EVT0_CLK_BLK_ISP_UID_AS_APB_ISP0_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_l0_taa_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_LH_AST_MI_L0_TAA_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_l0_isp_snw[] = {
	V620_EVT0_CLK_BLK_ISP_UID_LH_AST_SI_L0_ISP_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_SYSMMU_S0_ISP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_isp_cmu_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_ISP_CMU_ISP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_BICS_APB_P_ISP_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_SFMPU_P_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_XIU_D1_ISP_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_ast_mi_l1_taa_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_SLH_AST_MI_L1_TAA_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_ast_si_l1_isp_snw[] = {
	V620_EVT0_CLK_BLK_ISP_UID_SLH_AST_SI_L1_ISP_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d0_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_BICM_AXI_D0_ISP_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_ISP_UID_BICM_AXI_D0_ISP_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_isp[] = {
	V620_EVT0_CLK_BLK_ISP_UID_XIU_D0_ISP_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_m2m_cmu_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_M2M_CMU_M2M_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_D_TZPC_M2M_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_SYSREG_M2M_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_PPMU_D0_M2M_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_M2M_UID_PPMU_D0_M2M_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_SYSMMU_S0_PMMU0_M2M_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_SYSMMU_S0_M2M_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_SLH_AXI_MI_P_M2M_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_m2m0[] = {
	V620_EVT0_CLK_BLK_M2M_UID_AS_APB_M2M0_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_XIU_D0_M2M_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_XIU_D1_M2M_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_jpeg[] = {
	V620_EVT0_CLK_BLK_M2M_UID_QE_D_JPEG_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_M2M_UID_QE_D_JPEG_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_QE_D_M2M_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_M2M_UID_QE_D_M2M_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_d_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_VGEN_LITE_D_M2M_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_m2m_d0[] = {
	V620_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK_2X1,
	V620_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK_2X1,
	V620_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK_VOTF,
	V620_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK_VOTF,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_jpeg[] = {
	V620_EVT0_CLK_BLK_M2M_UID_JPEG_IPCLKPORT_I_SMFC_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip_jpeg[] = {
	V620_EVT0_CLK_BLK_M2M_UID_LH_AXI_MI_IP_JPEG_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ip_jpeg[] = {
	V620_EVT0_CLK_BLK_M2M_UID_LH_AXI_SI_IP_JPEG_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_si_d0_m2m[] = {
	V620_EVT0_CLK_BLK_M2M_UID_LH_ACEL_SI_D0_M2M_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_m2m_jpeg[] = {
	V620_EVT0_CLK_BLK_M2M_UID_AS_APB_M2M_JPEG_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mfc_cmu_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_MFC_CMU_MFC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_SYSREG_MFC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d1_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_LH_AXI_SI_D1_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_SLH_AXI_MI_P_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_SYSMMU_S0_PMMU0_MFC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_SYSMMU_S0_PMMU1_MFC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_PPMU_D0_MFC_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_MFC_UID_PPMU_D0_MFC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_PPMU_D1_MFC_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_MFC_UID_PPMU_D1_MFC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d2_wfd[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_PPMU_D2_WFD_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_MFC_UID_PPMU_D2_WFD_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_XIU_D1_MFC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_wfd_ns[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_AS_APB_WFD_NS_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_MFC_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MFC_UID_MFC_IPCLKPORT_C2CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wfd[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_WFD_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_D_TZPC_MFC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_mi_it_mfc[] = {
	V620_EVT0_GOUT_BLK_MFC_UID_LH_ATB_MI_IT_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_AS_APB_MFC_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_d_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_VGEN_LITE_D_MFC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_LH_AXI_MI_ID_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_SYSMMU_S0_MFC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_XIU_D0_MFC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_it_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_LH_ATB_SI_IT_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d0_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_LH_AXI_SI_D0_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_id_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_LH_AXI_SI_ID_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_fg[] = {
	V620_EVT0_CLK_BLK_MFC_UID_FG_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d3_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_PPMU_D3_MFC_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MFC_UID_PPMU_D3_MFC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d4_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_PPMU_D4_MFC_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MFC_UID_PPMU_D4_MFC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_d_fg_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_VGEN_LITE_D_FG_MFC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d2_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_XIU_D2_MFC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d3_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_XIU_D3_MFC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s1_pmmu0_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_SYSMMU_S1_PMMU0_MFC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s1_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_SYSMMU_S1_MFC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d2_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_LH_AXI_SI_D2_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_fg_mfc[] = {
	V620_EVT0_CLK_BLK_MFC_UID_AS_APB_FG_MFC_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mif_cmu_mif[] = {
	V620_EVT0_CLK_BLK_MIF_UID_MIF_CMU_MIF_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ddrphy0[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_DDRPHY0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_mif[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_SYSREG_MIF_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_mif[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_SLH_AXI_MI_P_MIF_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppc_debug0[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_PPC_DEBUG0_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_MIF_UID_PPC_DEBUG0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_smc0[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_SMC0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qch_adapter_ppc_debug0[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_QCH_ADAPTER_PPC_DEBUG0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_mif[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_D_TZPC_MIF_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_smc1[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_SMC1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ddrphy1[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_DDRPHY1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppc_debug1[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_PPC_DEBUG1_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_MIF_UID_PPC_DEBUG1_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qch_adapter_ppc_debug1[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_QCH_ADAPTER_PPC_DEBUG1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_mif[] = {
	V620_EVT0_GOUT_BLK_MIF_UID_SFMPU_P_MIF_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_spc_mif[] = {
	V620_EVT0_CLK_BLK_MIF_UID_SPC_MIF_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_spmpu_p_mif[] = {
	V620_EVT0_CLK_BLK_MIF_UID_SPMPU_P_MIF_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qch_adapter_smc0[] = {
	V620_EVT0_CLK_BLK_MIF_UID_QCH_ADAPTER_SMC0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qch_adapter_smc1[] = {
	V620_EVT0_CLK_BLK_MIF_UID_QCH_ADAPTER_SMC1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qch_adapter_ddrphy0[] = {
	V620_EVT0_CLK_BLK_MIF_UID_QCH_ADAPTER_DDRPHY0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qch_adapter_ddrphy1[] = {
	V620_EVT0_CLK_BLK_MIF_UID_QCH_ADAPTER_DDRPHY1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon_pll_mif[] = {
	V620_EVT0_CLK_BLK_MIF_UID_CLKMON_PLL_MIF_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_MIF_UID_CLKMON_PLL_MIF_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_MIF_UID_CLKMON_PLL_MIF_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_MIF_UID_CLKMON_PLL_MIF_IPCLKPORT_MON_CLK00,
	V620_EVT0_CLK_BLK_MIF_UID_CLKMON_PLL_MIF_IPCLKPORT_MON_CLK10,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_s_mif[] = {
	V620_EVT0_CLK_BLK_MIF_UID_BIC_APB_S_MIF_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_misc_cmu_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_MISC_CMU_MISC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_si_d_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_LH_ACEL_SI_D_MISC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pdma0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_PDMA0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pdma1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_PDMA1_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pdma2[] = {
	V620_EVT0_CLK_BLK_MISC_UID_PDMA2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pdma3[] = {
	V620_EVT0_CLK_BLK_MISC_UID_PDMA3_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pdma4[] = {
	V620_EVT0_CLK_BLK_MISC_UID_PDMA4_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_PPMU_D0_MISC_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_PPMU_D0_MISC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_pdma0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_pdma1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_pdma2[] = {
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA2_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_pdma3[] = {
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA3_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA3_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_pdma4[] = {
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA4_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_PDMA4_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_spdma0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_SPDMA0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_SPDMA0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_spdma1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_SPDMA1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_SPDMA1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SLH_AXI_MI_P_MISC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_spdma0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SPDMA0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_spdma1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SPDMA1_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SYSMMU_S0_PMMU0_MISC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SYSREG_MISC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_pdma0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VGEN_D_PDMA0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_pdma1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VGEN_D_PDMA1_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_pdma2[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VGEN_D_PDMA2_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_pdma3[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VGEN_D_PDMA3_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_pdma4[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VGEN_D_PDMA4_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_spdma0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VGEN_D_SPDMA0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_spdma1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VGEN_D_SPDMA1_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_XIU_D0_MISC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_D_TZPC_MISC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ad_apb_sysmmu_s0_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_AD_APB_SYSMMU_S0_MISC_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SYSMMU_S0_MISC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_misc_1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SYSREG_MISC_1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_misc_2[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SYSREG_MISC_2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_XIU_D1_MISC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mct0_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_MCT0_MISC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_otp_con_bira[] = {
	V620_EVT0_CLK_BLK_MISC_UID_OTP_CON_BIRA_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_MISC_UID_OTP_CON_BIRA_IPCLKPORT_I_OSCCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_otp_con_bisr[] = {
	V620_EVT0_CLK_BLK_MISC_UID_OTP_CON_BISR_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_MISC_UID_OTP_CON_BISR_IPCLKPORT_I_OSCCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_otp_con_top[] = {
	V620_EVT0_CLK_BLK_MISC_UID_OTP_CON_TOP_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_MISC_UID_OTP_CON_TOP_IPCLKPORT_I_OSCCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wdt_cluster0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_WDT_CLUSTER0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wdt_cluster1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_WDT_CLUSTER1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_icc_cluster0_gic[] = {
	V620_EVT0_CLK_BLK_MISC_UID_LH_AST_MI_L_ICC_CLUSTER0_GIC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_icc_cluster2_gic[] = {
	V620_EVT0_CLK_BLK_MISC_UID_LH_AST_MI_L_ICC_CLUSTER2_GIC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_l_iri_gic_cluster0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_LH_AST_SI_L_IRI_GIC_CLUSTER0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_l_iri_gic_cluster2[] = {
	V620_EVT0_CLK_BLK_MISC_UID_LH_AST_SI_L_IRI_GIC_CLUSTER2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_misc_gic[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SLH_AXI_MI_P_MISC_GIC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d_gic[] = {
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_GIC_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_QE_D_GIC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_gic[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VGEN_D_GIC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_tmu_1[] = {
	V620_EVT0_CLK_BLK_MISC_UID_TMU_1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_tmu_0[] = {
	V620_EVT0_CLK_BLK_MISC_UID_TMU_0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_volmon_int[] = {
	V620_EVT0_CLK_BLK_MISC_UID_VOLMON_INT_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_id_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_LH_AXI_SI_ID_MISC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_LH_AXI_MI_ID_MISC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_adc_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_ADC_MISC_IPCLKPORT_PCLK_S1,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_PPMU_D1_MISC_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_MISC_UID_PPMU_D1_MISC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s1_pmmu0_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SYSMMU_S1_PMMU0_MISC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s1_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SYSMMU_S1_MISC_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d2_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_XIU_D2_MISC_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_SFMPU_P_MISC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_BICM_AXI_D_MISC_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_MISC_UID_BICM_AXI_D_MISC_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_BICS_APB_P_MISC_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gic[] = {
	V620_EVT0_CLK_BLK_MISC_UID_GIC_IPCLKPORT_GCLK,
	V620_EVT0_CLK_BLK_MISC_UID_GIC_IPCLKPORT_GCLK_FDC,
	V620_EVT0_CLK_BLK_MISC_UID_GIC_IPCLKPORT_GCLK_FDC,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mct1_misc[] = {
	V620_EVT0_CLK_BLK_MISC_UID_MCT1_MISC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_axi_us_64to128_d_gic[] = {
	V620_EVT0_CLK_BLK_MISC_UID_AXI_US_64TO128_D_GIC_IPCLKPORT_MAINCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_nocl0_cmu_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_NOCL0_CMU_NOCL0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_SYSREG_NOCL0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mpace2axi_dp0_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_MPACE2AXI_DP0_NOCL0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mpace2axi_dp1_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_MPACE2AXI_DP1_NOCL0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppc_g_sci[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_PPC_G_SCI_IPCLKPORT_CLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPC_G_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_trex_p_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_TREX_P_NOCL0_IPCLKPORT_ACLK_NOCL0,
	V620_EVT0_GOUT_BLK_NOCL0_UID_TREX_P_NOCL0_IPCLKPORT_PCLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_TREX_P_NOCL0_IPCLKPORT_PCLK_NOCL0,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_t_bdu[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_ATB_SI_T_BDU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bdu[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_BDU_IPCLKPORT_I_CLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_BDU_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_G3D_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cpucl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_CPUCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d0_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_ACEL_MI_D0_G3D_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d1_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_ACEL_MI_D1_G3D_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d2_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_ACEL_MI_D2_G3D_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d3_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_ACEL_MI_D3_G3D_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_trex_d_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_TREX_D_NOCL0_IPCLKPORT_PCLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_TREX_D_NOCL0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_alive[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_D_TZPC_NOCL0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d_cpucl2[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL2_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d2_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D2_G3D_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D2_G3D_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d3_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D3_G3D_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D3_G3D_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d0_trex[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D0_TREX_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D0_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d1_trex[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D1_TREX_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D1_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_g_nocl1[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_AST_MI_G_NOCL1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mpace_asb_d0_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D0_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mpace_asb_d1_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mpace_asb_d2_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mpace_asb_d3_nocl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D3_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_chi_mi_d_cluster0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_CHI_MI_D_CLUSTER0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_mif0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_MIF0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d0_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D0_G3D_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D0_G3D_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d1_g3d[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D1_G3D_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D1_G3D_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d2_trex[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D2_TREX_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D2_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d3_trex[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D3_TREX_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D3_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cpucl2[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_CPUCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_g_nocl2[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_AST_MI_G_NOCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d_cpucl0[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL0_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_chi_mi_d_cluster2[] = {
	V620_EVT0_GOUT_BLK_NOCL0_UID_LH_CHI_MI_D_CLUSTER2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_mif1[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_MIF1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_peric0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_PERIC0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_peric1[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_PERIC1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_misc_gic[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_MISC_GIC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D0_SCI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D1_SCI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d2_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D2_SCI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d3_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D3_SCI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d0_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D0_SCI_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D0_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d1_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D1_SCI_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D1_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d2_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D2_SCI_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D2_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d3_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D3_SCI_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D3_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cluster0_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_MI_P_CLUSTER0_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cluster2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_MI_P_CLUSTER2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_misc[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_MISC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d0_nocl1_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D0_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d1_nocl1_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D1_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d2_nocl1_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D2_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d3_nocl1_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D3_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_p_nocl0_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_SI_P_NOCL0_NOCL1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_p_nocl0_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_SI_P_NOCL0_NOCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_trex[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D0_TREX_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D0_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_trex[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D1_TREX_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D1_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d2_trex[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D2_TREX_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D2_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d3_trex[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D3_TREX_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_D3_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_dp0_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP0_SCI_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP0_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_dp1_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP1_SCI_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP1_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_p_cpucl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL0_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL0_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_p_cpucl2[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL2_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wow_d_trex_qurgent[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D_TREX_QURGENT_IPCLKPORT_CLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_WOW_D_TREX_QURGENT_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_dp2_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP2_SCI_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP2_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cmu[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_CMU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_lg_dnc_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_AST_MI_LG_DNC_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_dp_trex[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP_TREX_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP_TREX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d0_nocl2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D0_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d1_nocl2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D1_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d2_nocl2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D2_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d3_nocl2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D3_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_adm_apb_g_bdu[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_ADM_APB_G_BDU_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_p_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_BIC_APB_P_NOCL0_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SFMPU_P_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sci[] = {
	V620_EVT0_CLK_BLK_NOCL0_UID_SCI_IPCLKPORT_I_TEST_CLK_DIV2,
	V620_EVT0_CLK_BLK_NOCL0_UID_SCI_IPCLKPORT_CLK,
	V620_EVT0_CLK_BLK_NOCL0_UID_SCI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_nocl1_cmu_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_NOCL1_CMU_NOCL1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_nocl1[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_SYSREG_NOCL1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_trex_d_nocl1[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_TREX_D_NOCL1_IPCLKPORT_PCLK,
	V620_EVT0_GOUT_BLK_NOCL1_UID_TREX_D_NOCL1_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d0_hsi0[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_LH_ACEL_MI_D0_HSI0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d1_hsi0[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_LH_ACEL_MI_D1_HSI0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_alive[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_LH_AXI_MI_D_ALIVE_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_trex_p_nocl1[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_TREX_P_NOCL1_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_TREX_P_NOCL1_IPCLKPORT_PCLK_NOCL1,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_nocl1[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_D_TZPC_NOCL1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_g_nocl1[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_LH_AST_SI_G_NOCL1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_sfi[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_LH_AXI_MI_D_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_aud[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_LH_AXI_MI_D_AUD_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_hsi2[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_SLH_AXI_SI_P_HSI2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_g_cssys[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_LH_AXI_MI_G_CSSYS_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d_ssp[] = {
	V620_EVT0_GOUT_BLK_NOCL1_UID_LH_ACEL_MI_D_SSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_aud[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_AUD_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dnc[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_ssp[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_SSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_hsi0[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_HSI0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_g3d_ptw[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_AXI_MI_D_G3D_PTW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cacheaid_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_CACHEAID_NOCL1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_CACHEAID_NOCL1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_sfi[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d0_hsi2[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_ACEL_MI_D0_HSI2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d1_hsi2[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_ACEL_MI_D1_HSI2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d0_dnc[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_D0_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d1_dnc[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_D1_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d2_dnc[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_D2_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d3_dnc[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_D3_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_p_nocl0_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_P_NOCL0_NOCL1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d0_nocl1_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_SI_D0_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d2_nocl1_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_SI_D2_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d1_nocl1_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_SI_D1_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d3_nocl1_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_SI_D3_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d_alive[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D_ALIVE_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D_ALIVE_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_g_cssys[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_G_CSSYS_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_G_CSSYS_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d_g3dmmu[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D_G3DMMU_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D_G3DMMU_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D0_NOCL1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D0_NOCL1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D1_NOCL1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D1_NOCL1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d2_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D2_NOCL1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D2_NOCL1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d3_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D3_NOCL1_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_D3_NOCL1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_SFMPU_P_NOCL1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_p_nocl1[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_BIC_APB_P_NOCL1_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d_misc[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_LH_ACEL_MI_D_MISC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_p_hsi0[] = {
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_P_HSI0_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL1_UID_PPMU_P_HSI0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_trex_d_nocl2[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_TREX_D_NOCL2_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_NOCL2_UID_TREX_D_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_nocl2[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SYSREG_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_nocl2[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_D_TZPC_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d_hsi1[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_ACEL_MI_D_HSI1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d0_dpuf0[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_DPUF0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d0_dpuf1[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_DPUF1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d1_dpuf0[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D1_DPUF0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d1_dpuf1[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D1_DPUF1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_acc[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D_ACC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d0_snw[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_isp[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_taa[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D_TAA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_g_nocl2[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_LH_AST_SI_G_NOCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_trex_p_nocl2[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_TREX_P_NOCL2_IPCLKPORT_PCLK_NOCL2,
	V620_EVT0_GOUT_BLK_NOCL2_UID_TREX_P_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_acc[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_ACC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dpuf1[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPUF1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dptx[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPTX_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dpub[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPUB_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dpuf0[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPUF0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_hsi1[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_HSI1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_isp[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_taa[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_TAA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_nocl2_cmu_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_NOCL2_CMU_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_snw[] = {
	V620_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d1_snw[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_AXI_MI_D1_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d2_snw[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_AXI_MI_D2_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_mi_d0_m2m[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_ACEL_MI_D0_M2M_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_m2m[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_SLH_AXI_SI_P_M2M_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_PPMU_D0_NOCL2_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_NOCL2_UID_PPMU_D0_NOCL2_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_PPMU_D1_NOCL2_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL2_UID_PPMU_D1_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d2_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_PPMU_D2_NOCL2_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL2_UID_PPMU_D2_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d3_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_PPMU_D3_NOCL2_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL2_UID_PPMU_D3_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d0_nocl2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_SI_D0_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d1_nocl2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_SI_D1_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d2_nocl2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_SI_D2_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_si_d3_nocl2_nocl0[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_SI_D3_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_taxi_mi_p_nocl0_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_MI_P_NOCL0_NOCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cacheaid_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_CACHEAID_NOCL2_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_NOCL2_UID_CACHEAID_NOCL2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_SFMPU_P_NOCL2_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_p_nocl2[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_BIC_APB_P_NOCL2_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_p_mfc[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_SLH_AXI_SI_P_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d0_mfc[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_AXI_MI_D0_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d1_mfc[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_AXI_MI_D1_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_d2_mfc[] = {
	V620_EVT0_CLK_BLK_NOCL2_UID_LH_AXI_MI_D2_MFC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpio_peric0[] = {
	V620_EVT0_GOUT_BLK_PERIC0_UID_GPIO_PERIC0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_peric0[] = {
	V620_EVT0_GOUT_BLK_PERIC0_UID_SYSREG_PERIC0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_peric0_cmu_peric0[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_PERIC0_CMU_PERIC0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_peric0[] = {
	V620_EVT0_GOUT_BLK_PERIC0_UID_SLH_AXI_MI_P_PERIC0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_peric0[] = {
	V620_EVT0_GOUT_BLK_PERIC0_UID_D_TZPC_PERIC0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi00_usi[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI00_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI00_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi00_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI00_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI00_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi01_usi[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI01_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI01_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi01_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI01_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI01_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi02_usi[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI02_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI02_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi02_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI02_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI02_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_pwm[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_PWM_IPCLKPORT_I_PCLK_S0,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_peric0[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_SFMPU_P_PERIC0_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi03_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI03_I2C_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI03_I2C_IPCLKPORT_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi07_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI07_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI07_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi03_usi[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI03_USI_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI03_USI_IPCLKPORT_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi07_usi[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_USI07_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_USI07_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_i3c2[] = {
	V620_EVT0_CLK_BLK_PERIC0_UID_I3C2_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_PERIC0_UID_I3C2_IPCLKPORT_I_SCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpio_peric1[] = {
	V620_EVT0_GOUT_BLK_PERIC1_UID_GPIO_PERIC1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_peric1[] = {
	V620_EVT0_GOUT_BLK_PERIC1_UID_SYSREG_PERIC1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_peric1_cmu_peric1[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_PERIC1_CMU_PERIC1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_peric1[] = {
	V620_EVT0_GOUT_BLK_PERIC1_UID_SLH_AXI_MI_P_PERIC1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_peric1[] = {
	V620_EVT0_GOUT_BLK_PERIC1_UID_D_TZPC_PERIC1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi09_usi[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI09_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI09_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi09_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI09_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI09_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi11_usi[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI11_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI11_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi11_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI11_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI11_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi12_usi[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI12_USI_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI12_USI_IPCLKPORT_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi12_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI12_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI12_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi13_usi[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI13_USI_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI13_USI_IPCLKPORT_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi13_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI13_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI13_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi14_usi[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI14_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI14_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi15_usi[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI15_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI15_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi14_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI14_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI14_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi15_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI15_I2C_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI15_I2C_IPCLKPORT_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi16_usi[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI16_USI_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI16_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi16_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI16_I2C_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI16_I2C_IPCLKPORT_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi17_usi[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI17_USI_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI17_USI_IPCLKPORT_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi17_i2c[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_USI17_I2C_IPCLKPORT_IPCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_USI17_I2C_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_i3c4[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_I3C4_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_I3C4_IPCLKPORT_I_SCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_i3c5[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_I3C5_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_I3C5_IPCLKPORT_I_SCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_i3c6[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_I3C6_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_I3C6_IPCLKPORT_I_SCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_i3c7[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_I3C7_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_PERIC1_UID_I3C7_IPCLKPORT_I_SCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_peric1[] = {
	V620_EVT0_CLK_BLK_PERIC1_UID_SFMPU_P_PERIC1_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_s2d_cmu_s2d[] = {
	V620_EVT0_CLK_BLK_S2D_UID_S2D_CMU_S2D_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bis_s2d[] = {
	V620_EVT0_GOUT_BLK_S2D_UID_BIS_S2D_IPCLKPORT_CLK,
	V620_EVT0_CLK_BLK_S2D_UID_BIS_S2D_IPCLKPORT_SCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_g_scan2dram_alive_mif[] = {
	V620_EVT0_GOUT_BLK_S2D_UID_SLH_AXI_MI_G_SCAN2DRAM_ALIVE_MIF_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sdma_cmu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_SDMA_CMU_SDMA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_SYSREG_SDMA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp1_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_SI_LD_SRAM_RDRSP1_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp0_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_SI_LD_SRAM_RDRSP0_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld0_mmu_sdma_dnc[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD0_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld1_mmu_sdma_dnc[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD1_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld2_mmu_sdma_dnc[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD2_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld3_mmu_sdma_dnc[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD3_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_sram_sdma_dsp[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD_SRAM_SDMA_DSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp3_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_SI_LD_SRAM_RDRSP3_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp2_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_SI_LD_SRAM_RDRSP2_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_D_TZPC_SDMA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_dnc_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AXI_MI_LD_CTRL_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_lp_dnc_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_SLH_AXI_MI_LP_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_cstfifo_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_SI_LD_SRAM_CSTFIFO_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ip_sdma_wrap[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_IP_SDMA_WRAP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq0_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_RDREQ0_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq1_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_RDREQ1_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq2_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_RDREQ2_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq3_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_RDREQ3_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq4_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_RDREQ4_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq5_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_RDREQ5_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_wr0_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_WR0_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_wr1_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_WR1_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_wr2_gnpu_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_WR2_GNPU_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp4_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_SI_LD_SRAM_RDRSP4_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp5_sdma_gnpu[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_SI_LD_SRAM_RDRSP5_SDMA_GNPU_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_sram_dnc_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AXI_MI_LD_SRAM_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_dsp_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_LH_AST_MI_LD_SRAM_DSP_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_SFMPU_P_SDMA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bic_apb_s_sdma[] = {
	V620_EVT0_CLK_BLK_SDMA_UID_BIC_APB_S_SDMA_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi19_usi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_USI19_USI_IPCLKPORT_IPCLK,
	V620_EVT0_GOUT_BLK_SFI_UID_USI19_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon0[] = {
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_REF_CLK0,
	V620_EVT0_GOUT_BLK_SFI_UID_CLKMON0_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_MON_CLK10,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_MON_CLK00,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi18_usi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_USI18_USI_IPCLKPORT_IPCLK,
	V620_EVT0_GOUT_BLK_SFI_UID_USI18_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi20_usi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_USI20_USI_IPCLKPORT_IPCLK,
	V620_EVT0_GOUT_BLK_SFI_UID_USI20_USI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sc_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_SC_SFI_IPCLKPORT_I_ACLK,
	V620_EVT0_GOUT_BLK_SFI_UID_SC_SFI_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_D_TZPC_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_gpio_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_GPIO_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap4[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_MAILBOX_AP4_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mct0_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_MCT0_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_rom_crc32_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_ROM_CRC32_SFI_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_SFI_UID_ROM_CRC32_SFI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_SYSREG_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_volmon[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_VOLMON_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wdt0_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_WDT0_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_usi21_usi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_USI21_USI_IPCLKPORT_PCLK,
	V620_EVT0_GOUT_BLK_SFI_UID_USI21_USI_IPCLKPORT_IPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_sfpc_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_D_SFPC_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_intmem_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_INTMEM_SFI_IPCLKPORT_I_ACLK,
	V620_EVT0_CLK_BLK_SFI_UID_INTMEM_SFI_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_asyncapb_intmem[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_ASYNCAPB_INTMEM_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_SLH_AXI_MI_P_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_LH_AXI_SI_D_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_dp_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_XIU_DP_SFI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xspi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_XSPI_IPCLKPORT_FLASH_CLK,
	V620_EVT0_CLK_BLK_SFI_UID_XSPI_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_SFI_UID_XSPI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_lt0_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_LH_ATB_SI_LT0_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_lt1_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_LH_ATB_SI_LT1_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_lt2_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_LH_ATB_SI_LT2_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_atb_si_lt3_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_LH_ATB_SI_LT3_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wdt1_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_WDT1_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_cluster_sfi[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_CLUSTER_SFI_IPCLKPORT_CLKIN,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon1[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_CLKMON1_IPCLKPORT_PCLK,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON1_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON1_IPCLKPORT_MON_CLK00,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON1_IPCLKPORT_MON_CLK10,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON1_IPCLKPORT_REF_CLK1,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_clkmon2[] = {
	V620_EVT0_GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_PCLK,
	V620_EVT0_GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_REF_CLK0,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON2_IPCLKPORT_REF_CLK1,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON2_IPCLKPORT_MON_CLK10,
	V620_EVT0_CLK_BLK_SFI_UID_CLKMON2_IPCLKPORT_MON_CLK00,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ip1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IP1_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IP1_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_idp0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IDP0_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_idp1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IDP1_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ip0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IP0_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_idp0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IDP0_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_idp1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IDP1_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IP0_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap0[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP0_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap1[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP1_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap7[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP7_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap2[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP2_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap3[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP3_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap6[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP6_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_dnc[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_DNC_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_ap5[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP5_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SFMPU_P0_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SFMPU_P1_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p2_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SFMPU_P2_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p3_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SFMPU_P3_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_d_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_VGEN_LITE_D_SFI_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SYSMMU_S0_SFI_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SYSMMU_S0_PMMU0_SFI_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_XIU_D0_SFI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p4_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SFMPU_P4_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_can_fd0[] = {
	V620_EVT0_CLK_BLK_SFI_UID_CAN_FD0_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_SFI_UID_CAN_FD0_IPCLKPORT_I_CCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_can_fd1[] = {
	V620_EVT0_CLK_BLK_SFI_UID_CAN_FD1_IPCLKPORT_I_PCLK,
	V620_EVT0_CLK_BLK_SFI_UID_CAN_FD1_IPCLKPORT_I_CCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mct1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MCT1_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_baaw_p_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BAAW_P_SFI_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_asyncapb_cluster_sfi_debug[] = {
	V620_EVT0_CLK_BLK_SFI_UID_ASYNCAPB_CLUSTER_SFI_DEBUG_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_PPMU_D_SFI_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SFI_UID_PPMU_D_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dma_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_DMA_SFI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_abox[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_ABOX_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_mailbox_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_MAILBOX_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_dapapbap_mux_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_DAPAPBAP_MUX_SFI_IPCLKPORT_DAPCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_adm_dap_g_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_ADM_DAP_G_SFI_IPCLKPORT_DAPCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_tmu_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_TMU_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ip2_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IP2_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip2_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IP2_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_asyncapb_fmu[] = {
	V620_EVT0_CLK_BLK_SFI_UID_ASYNCAPB_FMU_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_fmu[] = {
	V620_EVT0_CLK_BLK_SFI_UID_FMU_IPCLKPORT_PCLK_FMU,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_baaw_d_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BAAW_D_SFI_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p5_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SFMPU_P5_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d3_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_XIU_D3_SFI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BICM_AXI_D0_SFI_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SFI_UID_BICM_AXI_D0_SFI_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BICM_AXI_D1_SFI_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SFI_UID_BICM_AXI_D1_SFI_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_axi_d0_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D0_SFI_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D0_SFI_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_axi_d1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D1_SFI_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D1_SFI_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_axi_d2_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D2_SFI_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D2_SFI_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_axi_d3_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D3_SFI_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D3_SFI_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_axi_d4_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D4_SFI_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SFI_UID_BICS_AXI_D4_SFI_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_BICS_APB_P_SFI_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfi_cmu_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SFI_CMU_SFI_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_XIU_D1_SFI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d2_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_XIU_D2_SFI_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_ip3_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IP3_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip3_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IP3_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sbisttbox_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SBISTTBOX_SFI_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xhb_p_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_XHB_P_SFI_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_si_ip4_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SLH_AXI_SI_IP4_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_ip4_sfi[] = {
	V620_EVT0_CLK_BLK_SFI_UID_SLH_AXI_MI_IP4_SFI_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_snw_cmu_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SNW_CMU_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_D_TZPC_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SYSREG_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d0_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_VGEN_D0_SNW_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d2_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_VGEN_D2_SNW_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d0_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_PPMU_D0_SNW_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SNW_UID_PPMU_D0_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d1_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_PPMU_D1_SNW_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SNW_UID_PPMU_D1_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d0_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_LH_AXI_SI_D0_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d1_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_LH_AXI_SI_D1_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SLH_AXI_MI_P_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_l0_isp_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_LH_AST_MI_L0_ISP_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_acc_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_LH_AST_MI_L_ACC_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_snw_snf0_0[] = {
	V620_EVT0_CLK_BLK_SNW_UID_AS_APB_SNW_SNF0_0_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d0_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_QE_D0_SNW_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SNW_UID_QE_D0_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d2_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_QE_D2_SNW_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SNW_UID_QE_D2_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d3_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_QE_D3_SNW_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SNW_UID_QE_D3_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d2_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_PPMU_D2_SNW_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SNW_UID_PPMU_D2_SNW_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d2_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_LH_AXI_SI_D2_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SYSMMU_S0_PMMU0_SNW_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SYSMMU_S0_PMMU1_SNW_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SYSMMU_S0_SNW_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu2_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SYSMMU_S0_PMMU2_SNW_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_snf0[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SNF0_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_wrp[] = {
	V620_EVT0_CLK_BLK_SNW_UID_WRP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_XIU_D1_SNW_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d3_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_VGEN_D3_SNW_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_BICS_APB_P_SNW_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SFMPU_P_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_ast_mi_l1_isp_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_SLH_AST_MI_L1_ISP_SNW_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d3_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_XIU_D3_SNW_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d4_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_XIU_D4_SNW_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d5_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_XIU_D5_SNW_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d0_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_BICM_AXI_D0_SNW_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SNW_UID_BICM_AXI_D0_SNW_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d2_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_BICM_AXI_D2_SNW_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SNW_UID_BICM_AXI_D2_SNW_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bicm_axi_d3_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_BICM_AXI_D3_SNW_IPCLKPORT_I_APBCLK,
	V620_EVT0_CLK_BLK_SNW_UID_BICM_AXI_D3_SNW_IPCLKPORT_I_AXICLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_us_128_256_d0_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_US_128_256_D0_SNW_IPCLKPORT_MAINCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_us_128_256_d1_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_US_128_256_D1_SNW_IPCLKPORT_MAINCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_us_128_256_d3_snw[] = {
	V620_EVT0_CLK_BLK_SNW_UID_US_128_256_D3_SNW_IPCLKPORT_MAINCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ssp_cmu_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_SSP_CMU_SSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_SLH_AXI_MI_P_SSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_D_TZPC_SSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_SYSREG_SSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_securitycontroller[] = {
	V620_EVT0_CLK_BLK_SSP_UID_SECURITYCONTROLLER_IPCLKPORT_I_ACLK,
	V620_EVT0_CLK_BLK_SSP_UID_SECURITYCONTROLLER_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_SYSMMU_S0_PMMU0_SSP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_baaw_d_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_BAAW_D_SSP_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_XIU_D0_SSP_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_acel_si_d_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_LH_ACEL_SI_D_SSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_PPMU_D_SSP_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SSP_UID_PPMU_D_SSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d0_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_QE_D0_SSP_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SSP_UID_QE_D0_SSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d1_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_QE_D1_SSP_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SSP_UID_QE_D1_SSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_asyncapb_sysmmu[] = {
	V620_EVT0_CLK_BLK_SSP_UID_ASYNCAPB_SYSMMU_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_SYSMMU_S0_SSP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d1_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_XIU_D1_SSP_IPCLKPORT_ACLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_strong[] = {
	V620_EVT0_CLK_BLK_SSP_UID_LH_AXI_MI_ID_STRONG_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_rtic[] = {
	V620_EVT0_CLK_BLK_SSP_UID_RTIC_IPCLKPORT_I_ACLK,
	V620_EVT0_CLK_BLK_SSP_UID_RTIC_IPCLKPORT_I_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d2_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_QE_D2_SSP_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_SSP_UID_QE_D2_SSP_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_lite_d_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_VGEN_LITE_D_SSP_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_BICS_APB_P_SSP_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_SFMPU_P_SSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_otp_adk_deserial_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_OTP_ADK_DESERIAL_SSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_otp_puf_deserial_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_OTP_PUF_DESERIAL_SSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_otp_rek_deserial_ssp[] = {
	V620_EVT0_CLK_BLK_SSP_UID_OTP_REK_DESERIAL_SSP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_strong_cmu_strong[] = {
	V620_EVT0_CLK_BLK_STRONG_UID_STRONG_CMU_STRONG_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_axi_si_d_taa[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_LH_AXI_SI_D_TAA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_taa[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_SLH_AXI_MI_P_TAA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysreg_taa[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_SYSREG_TAA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_taa_cmu_taa[] = {
	V620_EVT0_CLK_BLK_TAA_UID_TAA_CMU_TAA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_acc_taa[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_LH_AST_MI_L_ACC_TAA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_lh_ast_si_l0_taa_isp[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_LH_AST_SI_L0_TAA_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_d_tzpc_taa[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_D_TZPC_TAA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_as_apb_taa0[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_AS_APB_TAA0_IPCLKPORT_PCLKM,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_ppmu_d_taa[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_PPMU_D_TAA_IPCLKPORT_ACLK,
	V620_EVT0_GOUT_BLK_TAA_UID_PPMU_D_TAA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_taa[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_SYSMMU_S0_PMMU0_TAA_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_taa[] = {
	V620_EVT0_GOUT_BLK_TAA_UID_TAA_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_qe_d0_taa[] = {
	V620_EVT0_CLK_BLK_TAA_UID_QE_D0_TAA_IPCLKPORT_ACLK,
	V620_EVT0_CLK_BLK_TAA_UID_QE_D0_TAA_IPCLKPORT_PCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_vgen_d_taa[] = {
	V620_EVT0_CLK_BLK_TAA_UID_VGEN_D_TAA_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sysmmu_s0_taa[] = {
	V620_EVT0_CLK_BLK_TAA_UID_SYSMMU_S0_TAA_IPCLKPORT_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_bics_apb_p_taa[] = {
	V620_EVT0_CLK_BLK_TAA_UID_BICS_APB_P_TAA_IPCLKPORT_I_APBCLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_sfmpu_p_taa[] = {
	V620_EVT0_CLK_BLK_TAA_UID_SFMPU_P_TAA_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_slh_ast_si_l1_taa_isp[] = {
	V620_EVT0_CLK_BLK_TAA_UID_SLH_AST_SI_L1_TAA_ISP_IPCLKPORT_I_CLK,
};
enum v620_evt0_clk_id v620_evt0_cmucal_vclk_ip_xiu_d0_taa[] = {
	V620_EVT0_CLK_BLK_TAA_UID_XIU_D0_TAA_IPCLKPORT_ACLK,
};

/* DVFS VCLK -> LUT List */
struct vclk_lut cmucal_vclk_v620_evt0_vdd_int_lut[] = {
	{200000, v620_evt0_vdd_int_od_lut_params},
	{100000, v620_evt0_vdd_int_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_vdd_mif_lut[] = {
	{7500000, v620_evt0_vdd_mif_od_lut_params},
	{7500000, v620_evt0_vdd_mif_sod_lut_params},
	{6400000, v620_evt0_vdd_mif_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_vdd_g3d_lut[] = {
	{1300000, v620_evt0_vdd_g3d_od_lut_params},
	{1000000, v620_evt0_vdd_g3d_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_vdd_cpucl0_lut[] = {
	{2400000, v620_evt0_vdd_cpucl0_sod_lut_params},
	{2100000, v620_evt0_vdd_cpucl0_od_lut_params},
	{1600000, v620_evt0_vdd_cpucl0_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_vdd_sfi_lut[] = {
	{1200000, v620_evt0_vdd_sfi_sod_lut_params},
	{800000, v620_evt0_vdd_sfi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_vdd_aud_lut[] = {
	{1033000, v620_evt0_vdd_aud_sod_lut_params},
	{811000, v620_evt0_vdd_aud_od_lut_params},
	{590000, v620_evt0_vdd_aud_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_vdd_int_cmu_lut[] = {
	{1800000, v620_evt0_vdd_int_cmu_sod_lut_params},
	{1600000, v620_evt0_vdd_int_cmu_od_lut_params},
	{1100000, v620_evt0_vdd_int_cmu_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_vdd_cpucl2_lut[] = {
	{2400000, v620_evt0_vdd_cpucl2_sod_lut_params},
	{2100000, v620_evt0_vdd_cpucl2_od_lut_params},
	{1600000, v620_evt0_vdd_cpucl2_nm_lut_params},
};

/* SPECIAL VCLK -> LUT List */
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_apm_timer_lut[] = {
	{38400, v620_evt0_mux_clk_apm_timer_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_aud_uaif3_lut[] = {
	{811000, v620_evt0_mux_clk_aud_uaif3_od_lut_params},
	{590000, v620_evt0_mux_clk_aud_uaif3_nm_lut_params},
	{533333, v620_evt0_mux_clk_aud_uaif3_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_cmu_cmuref_lut[] = {
	{393667, v620_evt0_mux_cmu_cmuref_ud_lut_params},
	{266667, v620_evt0_mux_cmu_cmuref_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_cpucl0_cmuref_lut[] = {
	{266667, v620_evt0_mux_cpucl0_cmuref_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_cpucl2_cmuref_lut[] = {
	{266667, v620_evt0_mux_cpucl2_cmuref_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_misc_gic_lut[] = {
	{266667, v620_evt0_mux_clk_misc_gic_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_misc_noc_lut[] = {
	{984500, v620_evt0_clkcmu_misc_noc_ud_lut_params},
	{533333, v620_evt0_clkcmu_misc_noc_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_nocl0_cmuref_lut[] = {
	{266667, v620_evt0_mux_nocl0_cmuref_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_dptx_dpgtc_lut[] = {
	{147625, v620_evt0_clkcmu_dptx_dpgtc_ud_lut_params},
	{133333, v620_evt0_clkcmu_dptx_dpgtc_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clkcmu_dpub_dsim_lut[] = {
	{787333, v620_evt0_mux_clkcmu_dpub_dsim_ud_lut_params},
	{400000, v620_evt0_mux_clkcmu_dpub_dsim_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_gnpu_xmaa_lut[] = {
	{1066000, v620_evt0_clkcmu_gnpu_xmaa_ud_lut_params},
	{866000, v620_evt0_clkcmu_gnpu_xmaa_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_apm_spmi_lut[] = {
	{19200, v620_evt0_div_clk_apm_spmi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_apm_noc_lut[] = {
	{590500, v620_evt0_clkcmu_apm_noc_ud_lut_params},
	{400000, v620_evt0_clkcmu_apm_noc_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_aud_mclk_lut[] = {
	{73786, v620_evt0_div_clk_aud_mclk_sod_lut_params},
	{73750, v620_evt0_div_clk_aud_mclk_nm_lut_params},
	{73727, v620_evt0_div_clk_aud_mclk_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clk_aud_avb_eth_lut[] = {
	{8, v620_evt0_clk_aud_avb_eth_sod_lut_params},
	{8, v620_evt0_clk_aud_avb_eth_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clk_cmu_pllclkout_lut[] = {
	{213200, v620_evt0_clk_cmu_pllclkout_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clk_add_ch_clk_lut[] = {
	{2133, v620_evt0_clk_add_ch_clk_sod_lut_params},
	{1536, v620_evt0_clk_add_ch_clk_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_cmu_nocp_lut[] = {
	{710667, v620_evt0_div_clk_cmu_nocp_ud_lut_params},
	{355333, v620_evt0_div_clk_cmu_nocp_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_cis_mclk3_lut[] = {
	{100000, v620_evt0_clkcmu_cis_mclk3_sod_lut_params},
	{38400, v620_evt0_clkcmu_cis_mclk3_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_cis_mclk0_lut[] = {
	{100000, v620_evt0_clkcmu_cis_mclk0_sod_lut_params},
	{38400, v620_evt0_clkcmu_cis_mclk0_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_cis_mclk1_lut[] = {
	{100000, v620_evt0_clkcmu_cis_mclk1_sod_lut_params},
	{38400, v620_evt0_clkcmu_cis_mclk1_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_cis_mclk2_lut[] = {
	{100000, v620_evt0_clkcmu_cis_mclk2_sod_lut_params},
	{38400, v620_evt0_clkcmu_cis_mclk2_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_cpucl0_shortstop_lut[] = {
	{550000, v620_evt0_div_clk_cpucl0_shortstop_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_cpucl0_cluster_lut[] = {
	{1800000, v620_evt0_clkcmu_cpucl0_cluster_sod_lut_params},
	{1600000, v620_evt0_clkcmu_cpucl0_cluster_od_lut_params},
	{1100000, v620_evt0_clkcmu_cpucl0_cluster_nm_lut_params},
	{590500, v620_evt0_clkcmu_cpucl0_cluster_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_cpucl0_switch_lut[] = {
	{1066000, v620_evt0_clkcmu_cpucl0_switch_sod_lut_params},
	{533000, v620_evt0_clkcmu_cpucl0_switch_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_cpucl2_shortstop_lut[] = {
	{550000, v620_evt0_div_clk_cpucl2_shortstop_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_cpucl2_cluster_lut[] = {
	{2362000, v620_evt0_clkcmu_cpucl2_cluster_ud_lut_params},
	{1800000, v620_evt0_clkcmu_cpucl2_cluster_sod_lut_params},
	{1600000, v620_evt0_clkcmu_cpucl2_cluster_od_lut_params},
	{1100000, v620_evt0_clkcmu_cpucl2_cluster_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_cpucl2_switch_lut[] = {
	{1066000, v620_evt0_clkcmu_cpucl2_switch_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_dpuf1_nocp_lut[] = {
	{333250, v620_evt0_div_clk_dpuf1_nocp_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_dpuf1_noc_lut[] = {
	{984500, v620_evt0_clkcmu_dpuf1_noc_ud_lut_params},
	{667000, v620_evt0_clkcmu_dpuf1_noc_od_lut_params},
	{666500, v620_evt0_clkcmu_dpuf1_noc_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_nocl0_sci_div2_lut[] = {
	{433000, v620_evt0_div_clk_nocl0_sci_div2_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_nocl0_noc_lut[] = {
	{918667, v620_evt0_clkcmu_nocl0_noc_ud_lut_params},
	{866000, v620_evt0_clkcmu_nocl0_noc_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric0_usi00_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric0_usi00_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_peric0_ip_lut[] = {
	{590500, v620_evt0_clkcmu_peric0_ip_ud_lut_params},
	{400000, v620_evt0_clkcmu_peric0_ip_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric0_usi01_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric0_usi01_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric0_usi02_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric0_usi02_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric0_usi07_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric0_usi07_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric0_usi03_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric0_usi03_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric1_usi09_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric1_usi09_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_clkcmu_peric1_ip_lut[] = {
	{590500, v620_evt0_clkcmu_peric1_ip_ud_lut_params},
	{400000, v620_evt0_clkcmu_peric1_ip_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric1_usi11_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric1_usi11_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric1_usi12_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric1_usi12_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric1_usi13_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric1_usi13_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric1_usi14_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric1_usi14_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric1_usi15_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric1_usi15_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric1_usi16_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric1_usi16_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_peric1_usi17_usi_lut[] = {
	{400000, v620_evt0_div_clk_peric1_usi17_usi_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_sfi_pllclkout_lut[] = {
	{1200000, v620_evt0_mux_clk_sfi_pllclkout_sod_lut_params},
	{800000, v620_evt0_mux_clk_sfi_pllclkout_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_sfi_usi20_lut[] = {
	{1200000, v620_evt0_mux_clk_sfi_usi20_sod_lut_params},
	{800000, v620_evt0_mux_clk_sfi_usi20_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_sfi_usi19_lut[] = {
	{1200000, v620_evt0_mux_clk_sfi_usi19_sod_lut_params},
	{800000, v620_evt0_mux_clk_sfi_usi19_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_div_clk_sfi_cpu_cntclk_lut[] = {
	{300000, v620_evt0_div_clk_sfi_cpu_cntclk_sod_lut_params},
	{200000, v620_evt0_div_clk_sfi_cpu_cntclk_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_sfi_usi21_lut[] = {
	{1200000, v620_evt0_mux_clk_sfi_usi21_sod_lut_params},
	{800000, v620_evt0_mux_clk_sfi_usi21_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_sfi_can0_lut[] = {
	{1200000, v620_evt0_mux_clk_sfi_can0_sod_lut_params},
	{800000, v620_evt0_mux_clk_sfi_can0_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_sfi_can1_lut[] = {
	{1200000, v620_evt0_mux_clk_sfi_can1_sod_lut_params},
	{800000, v620_evt0_mux_clk_sfi_can1_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_cpucl0_core_lut[] = {
	{2400000, v620_evt0_mux_clk_cpucl0_core_sod_lut_params},
	{2100000, v620_evt0_mux_clk_cpucl0_core_od_lut_params},
	{1600000, v620_evt0_mux_clk_cpucl0_core_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_mux_clk_cpucl2_core_lut[] = {
	{2400000, v620_evt0_mux_clk_cpucl2_core_sod_lut_params},
	{2100000, v620_evt0_mux_clk_cpucl2_core_od_lut_params},
	{1600000, v620_evt0_mux_clk_cpucl2_core_nm_lut_params},
};

/* COMMON VCLK -> LUT List */
struct vclk_lut cmucal_vclk_v620_evt0_blk_aud_lut[] = {
	{516500, v620_evt0_blk_aud_sod_lut_params},
	{405500, v620_evt0_blk_aud_od_lut_params},
	{295000, v620_evt0_blk_aud_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_cmu_lut[] = {
	{2132000, v620_evt0_blk_cmu_od_lut_params},
	{2132000, v620_evt0_blk_cmu_sod_lut_params},
	{2132000, v620_evt0_blk_cmu_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_hsi2_lut[] = {
	{375000, v620_evt0_blk_hsi2_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_s2d_lut[] = {
	{100000, v620_evt0_blk_s2d_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_apm_lut[] = {
	{400000, v620_evt0_blk_apm_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_cpucl0_lut[] = {
	{550000, v620_evt0_blk_cpucl0_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_cpucl2_lut[] = {
	{550000, v620_evt0_blk_cpucl2_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_g3d_lut[] = {
	{1500000, v620_evt0_blk_g3d_sod_lut_params},
	{1300000, v620_evt0_blk_g3d_od_lut_params},
	{1000000, v620_evt0_blk_g3d_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_hsi1_lut[] = {
	{40000, v620_evt0_blk_hsi1_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_peric0_lut[] = {
	{200000, v620_evt0_blk_peric0_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_peric1_lut[] = {
	{200000, v620_evt0_blk_peric1_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_sfi_lut[] = {
	{1200000, v620_evt0_blk_sfi_sod_lut_params},
	{800000, v620_evt0_blk_sfi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_acc_lut[] = {
	{266667, v620_evt0_blk_acc_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_dnc_lut[] = {
	{166625, v620_evt0_blk_dnc_od_lut_params},
	{38400, v620_evt0_blk_dnc_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_dpub_lut[] = {
	{333250, v620_evt0_blk_dpub_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_dpuf_lut[] = {
	{333250, v620_evt0_blk_dpuf_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_dsp_lut[] = {
	{216500, v620_evt0_blk_dsp_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_gnpu_lut[] = {
	{216500, v620_evt0_blk_gnpu_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_hsi0_lut[] = {
	{106667, v620_evt0_blk_hsi0_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_isp_lut[] = {
	{266667, v620_evt0_blk_isp_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_m2m_lut[] = {
	{355333, v620_evt0_blk_m2m_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_mfc_lut[] = {
	{333250, v620_evt0_blk_mfc_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_misc_lut[] = {
	{266667, v620_evt0_blk_misc_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_nocl0_lut[] = {
	{288667, v620_evt0_blk_nocl0_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_nocl1_lut[] = {
	{207333, v620_evt0_blk_nocl1_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_nocl2_lut[] = {
	{177778, v620_evt0_blk_nocl2_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_sdma_lut[] = {
	{166625, v620_evt0_blk_sdma_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_snw_lut[] = {
	{266667, v620_evt0_blk_snw_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_ssp_lut[] = {
	{266667, v620_evt0_blk_ssp_od_lut_params},
};
struct vclk_lut cmucal_vclk_v620_evt0_blk_taa_lut[] = {
	{266667, v620_evt0_blk_taa_sod_lut_params},
};

/* Switch VCLK -> LUT Parameter List */
struct switch_lut v620_evt0_mux_clk_aud_cpu_lut[] = {
	{933000, 1, 0},
	{710667, 3, 0},
	{533333, 6, 0},
	{533000, 0, 1},
};
struct switch_lut v620_evt0_mux_clk_aud_noc_lut[] = {
	{1181000, 0, 0},
	{800000, 0, 0},
	{533333, 3, 0},
};
struct switch_lut v620_evt0_mux_clk_hsi2_ethernet_lut[] = {
	{393667, 1, 2},
	{355333, 2, 1},
};
/*================================ SWPLL List =================================*/
struct vclk_switch v620_evt0_switch_vdd_aud[] = {
	{V620_EVT0_MUX_CLK_AUD_CPU, V620_EVT0_MUX_CLKCMU_AUD_CPU, V620_EVT0_CLKCMU_AUD_CPU,
	V620_EVT0_GATE_CLKCMU_AUD_CPU, V620_EVT0_MUX_CLKCMU_AUD_CPU_USER,
	v620_evt0_mux_clk_aud_cpu_lut, 4},
	{V620_EVT0_MUX_CLK_AUD_NOC, V620_EVT0_MUX_CLKCMU_AUD_NOC, V620_EVT0_CLKCMU_AUD_NOC,
	V620_EVT0_GATE_CLKCMU_AUD_NOC, V620_EVT0_MUX_CLKCMU_AUD_NOC_USER,
	v620_evt0_mux_clk_aud_noc_lut, 3},
};
struct vclk_switch v620_evt0_switch_blk_hsi2[] = {
	{V620_EVT0_MUX_CLK_HSI2_ETHERNET, V620_EVT0_MUX_CLKCMU_HSI2_ETHERNET,
	V620_EVT0_CLKCMU_HSI2_ETHERNET, V620_EVT0_GATE_CLKCMU_HSI2_ETHERNET,
	V620_EVT0_MUX_CLKCMU_HSI2_ETHERNET_USER, v620_evt0_mux_clk_hsi2_ethernet_lut, 2},
};

/*================================ VCLK List =================================*/
unsigned int v620_evt0_cmucal_vclk_size = 1068;
struct vclk v620_evt0_cmucal_vclk_list[] = {

/* DVFS VCLK*/
	CMUCAL_VCLK(V620_EVT0_VCLK_V620_EVT0_VDD_INT, cmucal_vclk_v620_evt0_vdd_int_lut,
	v620_evt0_cmucal_vclk_vdd_int, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_VDD_MIF, cmucal_vclk_v620_evt0_vdd_mif_lut,
	v620_evt0_cmucal_vclk_vdd_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_VDD_G3D, cmucal_vclk_v620_evt0_vdd_g3d_lut,
	v620_evt0_cmucal_vclk_vdd_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_VDD_CPUCL0, cmucal_vclk_v620_evt0_vdd_cpucl0_lut,
	v620_evt0_cmucal_vclk_vdd_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_VDD_SFI, cmucal_vclk_v620_evt0_vdd_sfi_lut,
	v620_evt0_cmucal_vclk_vdd_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_VDD_AUD, cmucal_vclk_v620_evt0_vdd_aud_lut,
	v620_evt0_cmucal_vclk_vdd_aud, NULL, v620_evt0_switch_vdd_aud),
	CMUCAL_VCLK(VCLK_V620_EVT0_VDD_INT_CMU, cmucal_vclk_v620_evt0_vdd_int_cmu_lut,
	v620_evt0_cmucal_vclk_vdd_int_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_VDD_CPUCL2, cmucal_vclk_v620_evt0_vdd_cpucl2_lut,
	v620_evt0_cmucal_vclk_vdd_cpucl2, NULL, NULL),

/* SPECIAL VCLK*/
	CMUCAL_VCLK(V620_EVT0_VCLK_MUX_CLK_APM_TIMER, cmucal_vclk_v620_evt0_mux_clk_apm_timer_lut,
	v620_evt0_cmucal_vclk_mux_clk_apm_timer, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_AUD_UAIF3, cmucal_vclk_v620_evt0_mux_clk_aud_uaif3_lut,
	v620_evt0_cmucal_vclk_mux_clk_aud_uaif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CMU_CMUREF, cmucal_vclk_v620_evt0_mux_cmu_cmuref_lut,
	v620_evt0_cmucal_vclk_mux_cmu_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CPUCL0_CMUREF, cmucal_vclk_v620_evt0_mux_cpucl0_cmuref_lut,
	v620_evt0_cmucal_vclk_mux_cpucl0_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CPUCL2_CMUREF, cmucal_vclk_v620_evt0_mux_cpucl2_cmuref_lut,
	v620_evt0_cmucal_vclk_mux_cpucl2_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_MISC_GIC, cmucal_vclk_v620_evt0_mux_clk_misc_gic_lut,
	v620_evt0_cmucal_vclk_mux_clk_misc_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_MISC_NOC, cmucal_vclk_v620_evt0_clkcmu_misc_noc_lut,
	v620_evt0_cmucal_vclk_clkcmu_misc_noc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_NOCL0_CMUREF, cmucal_vclk_v620_evt0_mux_nocl0_cmuref_lut,
	v620_evt0_cmucal_vclk_mux_nocl0_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_DPTX_DPGTC, cmucal_vclk_v620_evt0_clkcmu_dptx_dpgtc_lut,
	v620_evt0_cmucal_vclk_clkcmu_dptx_dpgtc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLKCMU_DPUB_DSIM,
	cmucal_vclk_v620_evt0_mux_clkcmu_dpub_dsim_lut, v620_evt0_cmucal_vclk_mux_clkcmu_dpub_dsim,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_GNPU_XMAA, cmucal_vclk_v620_evt0_clkcmu_gnpu_xmaa_lut,
	v620_evt0_cmucal_vclk_clkcmu_gnpu_xmaa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_APM_SPMI, cmucal_vclk_v620_evt0_div_clk_apm_spmi_lut,
	v620_evt0_cmucal_vclk_div_clk_apm_spmi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_APM_NOC, cmucal_vclk_v620_evt0_clkcmu_apm_noc_lut,
	v620_evt0_cmucal_vclk_clkcmu_apm_noc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_AUD_MCLK, cmucal_vclk_v620_evt0_div_clk_aud_mclk_lut,
	v620_evt0_cmucal_vclk_div_clk_aud_mclk, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLK_AUD_AVB_ETH, cmucal_vclk_v620_evt0_clk_aud_avb_eth_lut,
	v620_evt0_cmucal_vclk_clk_aud_avb_eth, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLK_CMU_PLLCLKOUT, cmucal_vclk_v620_evt0_clk_cmu_pllclkout_lut,
	v620_evt0_cmucal_vclk_clk_cmu_pllclkout, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLK_ADD_CH_CLK, cmucal_vclk_v620_evt0_clk_add_ch_clk_lut,
	v620_evt0_cmucal_vclk_clk_add_ch_clk, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_CMU_NOCP, cmucal_vclk_v620_evt0_div_clk_cmu_nocp_lut,
	v620_evt0_cmucal_vclk_div_clk_cmu_nocp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_CIS_MCLK3, cmucal_vclk_v620_evt0_clkcmu_cis_mclk3_lut,
	v620_evt0_cmucal_vclk_clkcmu_cis_mclk3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_CIS_MCLK0, cmucal_vclk_v620_evt0_clkcmu_cis_mclk0_lut,
	v620_evt0_cmucal_vclk_clkcmu_cis_mclk0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_CIS_MCLK1, cmucal_vclk_v620_evt0_clkcmu_cis_mclk1_lut,
	v620_evt0_cmucal_vclk_clkcmu_cis_mclk1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_CIS_MCLK2, cmucal_vclk_v620_evt0_clkcmu_cis_mclk2_lut,
	v620_evt0_cmucal_vclk_clkcmu_cis_mclk2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_CPUCL0_SHORTSTOP,
	cmucal_vclk_v620_evt0_div_clk_cpucl0_shortstop_lut,
	v620_evt0_cmucal_vclk_div_clk_cpucl0_shortstop, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_CPUCL0_CLUSTER,
	cmucal_vclk_v620_evt0_clkcmu_cpucl0_cluster_lut,
	v620_evt0_cmucal_vclk_clkcmu_cpucl0_cluster, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_CPUCL0_SWITCH,
	cmucal_vclk_v620_evt0_clkcmu_cpucl0_switch_lut, v620_evt0_cmucal_vclk_clkcmu_cpucl0_switch,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_CPUCL2_SHORTSTOP,
	cmucal_vclk_v620_evt0_div_clk_cpucl2_shortstop_lut,
	v620_evt0_cmucal_vclk_div_clk_cpucl2_shortstop, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_CPUCL2_CLUSTER,
	cmucal_vclk_v620_evt0_clkcmu_cpucl2_cluster_lut,
	v620_evt0_cmucal_vclk_clkcmu_cpucl2_cluster, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_CPUCL2_SWITCH,
	cmucal_vclk_v620_evt0_clkcmu_cpucl2_switch_lut, v620_evt0_cmucal_vclk_clkcmu_cpucl2_switch,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_DPUF1_NOCP,
	cmucal_vclk_v620_evt0_div_clk_dpuf1_nocp_lut, v620_evt0_cmucal_vclk_div_clk_dpuf1_nocp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_DPUF1_NOC, cmucal_vclk_v620_evt0_clkcmu_dpuf1_noc_lut,
	v620_evt0_cmucal_vclk_clkcmu_dpuf1_noc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_NOCL0_SCI_DIV2,
	cmucal_vclk_v620_evt0_div_clk_nocl0_sci_div2_lut,
	v620_evt0_cmucal_vclk_div_clk_nocl0_sci_div2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_NOCL0_NOC, cmucal_vclk_v620_evt0_clkcmu_nocl0_noc_lut,
	v620_evt0_cmucal_vclk_clkcmu_nocl0_noc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC0_USI00_USI,
	cmucal_vclk_v620_evt0_div_clk_peric0_usi00_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric0_usi00_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_PERIC0_IP, cmucal_vclk_v620_evt0_clkcmu_peric0_ip_lut,
	v620_evt0_cmucal_vclk_clkcmu_peric0_ip, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC0_USI01_USI,
	cmucal_vclk_v620_evt0_div_clk_peric0_usi01_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric0_usi01_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC0_USI02_USI,
	cmucal_vclk_v620_evt0_div_clk_peric0_usi02_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric0_usi02_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC0_USI07_USI,
	cmucal_vclk_v620_evt0_div_clk_peric0_usi07_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric0_usi07_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC0_USI03_USI,
	cmucal_vclk_v620_evt0_div_clk_peric0_usi03_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric0_usi03_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC1_USI09_USI,
	cmucal_vclk_v620_evt0_div_clk_peric1_usi09_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric1_usi09_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_CLKCMU_PERIC1_IP, cmucal_vclk_v620_evt0_clkcmu_peric1_ip_lut,
	v620_evt0_cmucal_vclk_clkcmu_peric1_ip, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC1_USI11_USI,
	cmucal_vclk_v620_evt0_div_clk_peric1_usi11_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric1_usi11_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC1_USI12_USI,
	cmucal_vclk_v620_evt0_div_clk_peric1_usi12_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric1_usi12_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC1_USI13_USI,
	cmucal_vclk_v620_evt0_div_clk_peric1_usi13_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric1_usi13_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC1_USI14_USI,
	cmucal_vclk_v620_evt0_div_clk_peric1_usi14_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric1_usi14_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC1_USI15_USI,
	cmucal_vclk_v620_evt0_div_clk_peric1_usi15_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric1_usi15_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC1_USI16_USI,
	cmucal_vclk_v620_evt0_div_clk_peric1_usi16_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric1_usi16_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_PERIC1_USI17_USI,
	cmucal_vclk_v620_evt0_div_clk_peric1_usi17_usi_lut,
	v620_evt0_cmucal_vclk_div_clk_peric1_usi17_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_SFI_PLLCLKOUT,
	cmucal_vclk_v620_evt0_mux_clk_sfi_pllclkout_lut,
	v620_evt0_cmucal_vclk_mux_clk_sfi_pllclkout, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_SFI_USI20, cmucal_vclk_v620_evt0_mux_clk_sfi_usi20_lut,
	v620_evt0_cmucal_vclk_mux_clk_sfi_usi20, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_SFI_USI19, cmucal_vclk_v620_evt0_mux_clk_sfi_usi19_lut,
	v620_evt0_cmucal_vclk_mux_clk_sfi_usi19, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_DIV_CLK_SFI_CPU_CNTCLK,
	cmucal_vclk_v620_evt0_div_clk_sfi_cpu_cntclk_lut,
	v620_evt0_cmucal_vclk_div_clk_sfi_cpu_cntclk, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_SFI_USI21, cmucal_vclk_v620_evt0_mux_clk_sfi_usi21_lut,
	v620_evt0_cmucal_vclk_mux_clk_sfi_usi21, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_SFI_CAN0, cmucal_vclk_v620_evt0_mux_clk_sfi_can0_lut,
	v620_evt0_cmucal_vclk_mux_clk_sfi_can0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_SFI_CAN1, cmucal_vclk_v620_evt0_mux_clk_sfi_can1_lut,
	v620_evt0_cmucal_vclk_mux_clk_sfi_can1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_CPUCL0_CORE,
	cmucal_vclk_v620_evt0_mux_clk_cpucl0_core_lut, v620_evt0_cmucal_vclk_mux_clk_cpucl0_core,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_MUX_CLK_CPUCL2_CORE,
	cmucal_vclk_v620_evt0_mux_clk_cpucl2_core_lut, v620_evt0_cmucal_vclk_mux_clk_cpucl2_core,
	NULL, NULL),

/* COMMON VCLK*/
	CMUCAL_VCLK(V620_EVT0_VCLK_BLK_AUD, cmucal_vclk_v620_evt0_blk_aud_lut,
	v620_evt0_cmucal_vclk_blk_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_CMU, cmucal_vclk_v620_evt0_blk_cmu_lut,
	v620_evt0_cmucal_vclk_blk_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_HSI2, cmucal_vclk_v620_evt0_blk_hsi2_lut,
	v620_evt0_cmucal_vclk_blk_hsi2, NULL, v620_evt0_switch_blk_hsi2),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_S2D, cmucal_vclk_v620_evt0_blk_s2d_lut,
	v620_evt0_cmucal_vclk_blk_s2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_APM, cmucal_vclk_v620_evt0_blk_apm_lut,
	v620_evt0_cmucal_vclk_blk_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_CPUCL0, cmucal_vclk_v620_evt0_blk_cpucl0_lut,
	v620_evt0_cmucal_vclk_blk_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_CPUCL2, cmucal_vclk_v620_evt0_blk_cpucl2_lut,
	v620_evt0_cmucal_vclk_blk_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_G3D, cmucal_vclk_v620_evt0_blk_g3d_lut,
	v620_evt0_cmucal_vclk_blk_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_HSI1, cmucal_vclk_v620_evt0_blk_hsi1_lut,
	v620_evt0_cmucal_vclk_blk_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_PERIC0, cmucal_vclk_v620_evt0_blk_peric0_lut,
	v620_evt0_cmucal_vclk_blk_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_PERIC1, cmucal_vclk_v620_evt0_blk_peric1_lut,
	v620_evt0_cmucal_vclk_blk_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_SFI, cmucal_vclk_v620_evt0_blk_sfi_lut,
	v620_evt0_cmucal_vclk_blk_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_ACC, cmucal_vclk_v620_evt0_blk_acc_lut,
	v620_evt0_cmucal_vclk_blk_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_DNC, cmucal_vclk_v620_evt0_blk_dnc_lut,
	v620_evt0_cmucal_vclk_blk_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_DPUB, cmucal_vclk_v620_evt0_blk_dpub_lut,
	v620_evt0_cmucal_vclk_blk_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_DPUF, cmucal_vclk_v620_evt0_blk_dpuf_lut,
	v620_evt0_cmucal_vclk_blk_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_DSP, cmucal_vclk_v620_evt0_blk_dsp_lut,
	v620_evt0_cmucal_vclk_blk_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_GNPU, cmucal_vclk_v620_evt0_blk_gnpu_lut,
	v620_evt0_cmucal_vclk_blk_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_HSI0, cmucal_vclk_v620_evt0_blk_hsi0_lut,
	v620_evt0_cmucal_vclk_blk_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_ISP, cmucal_vclk_v620_evt0_blk_isp_lut,
	v620_evt0_cmucal_vclk_blk_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_M2M, cmucal_vclk_v620_evt0_blk_m2m_lut,
	v620_evt0_cmucal_vclk_blk_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_MFC, cmucal_vclk_v620_evt0_blk_mfc_lut,
	v620_evt0_cmucal_vclk_blk_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_MISC, cmucal_vclk_v620_evt0_blk_misc_lut,
	v620_evt0_cmucal_vclk_blk_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_NOCL0, cmucal_vclk_v620_evt0_blk_nocl0_lut,
	v620_evt0_cmucal_vclk_blk_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_NOCL1, cmucal_vclk_v620_evt0_blk_nocl1_lut,
	v620_evt0_cmucal_vclk_blk_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_NOCL2, cmucal_vclk_v620_evt0_blk_nocl2_lut,
	v620_evt0_cmucal_vclk_blk_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_SDMA, cmucal_vclk_v620_evt0_blk_sdma_lut,
	v620_evt0_cmucal_vclk_blk_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_SNW, cmucal_vclk_v620_evt0_blk_snw_lut,
	v620_evt0_cmucal_vclk_blk_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_SSP, cmucal_vclk_v620_evt0_blk_ssp_lut,
	v620_evt0_cmucal_vclk_blk_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_BLK_TAA, cmucal_vclk_v620_evt0_blk_taa_lut,
	v620_evt0_cmucal_vclk_blk_taa, NULL, NULL),

/* GATE VCLK*/
	CMUCAL_VCLK(V620_EVT0_VCLK_IP_SLH_AXI_MI_P_ACC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D_ACC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_ACC, NULL, v620_evt0_cmucal_vclk_ip_sysreg_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_L_ACC_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_l_acc_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_ACC, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_L_ACC_TAA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_l_acc_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D_ACC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_ORBMCH, NULL, v620_evt0_cmucal_vclk_ip_qe_d_orbmch,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_ACC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APB_ASYNC_ISPPRE0, NULL,
	v620_evt0_cmucal_vclk_ip_apb_async_isppre0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_ISPPRE, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_isppre,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ACC_CMU_ACC, NULL, v620_evt0_cmucal_vclk_ip_acc_cmu_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_D_ORBMCH, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_lite_d_orbmch, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CSIS_LINK_MUX2X4, NULL,
	v620_evt0_cmucal_vclk_ip_csis_link_mux2x4, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_ISPPRE, NULL, v620_evt0_cmucal_vclk_ip_qe_d_isppre,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ISPPRE, NULL, v620_evt0_cmucal_vclk_ip_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_ACC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_ACC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_ACC, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_US_128_256_D0_ACC, NULL,
	v620_evt0_cmucal_vclk_ip_us_128_256_d0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_ACC, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_ACC, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D2_ACC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d2_acc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D0_ACC, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ORBMCH, NULL, v620_evt0_cmucal_vclk_ip_orbmch, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WDT_APM, NULL, v620_evt0_cmucal_vclk_ip_wdt_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_APM, NULL, v620_evt0_cmucal_vclk_ip_sysreg_apm,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_APM_AP, NULL,
	v620_evt0_cmucal_vclk_ip_mailbox_apm_ap, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_PMU_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_apbif_pmu_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_INTMEM_ALIVE, NULL, v620_evt0_cmucal_vclk_ip_intmem_alive,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PMU_INTR_GEN, NULL, v620_evt0_cmucal_vclk_ip_pmu_intr_gen,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_DP_ALIVE, NULL, v620_evt0_cmucal_vclk_ip_xiu_dp_alive,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APM_CMU_APM, NULL, v620_evt0_cmucal_vclk_ip_apm_cmu_apm,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GREBE_APM, NULL, v620_evt0_cmucal_vclk_ip_grebe_apm,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_GPIO_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_apbif_gpio_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_TOP_RTC, NULL, v620_evt0_cmucal_vclk_ip_apbif_top_rtc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_APM, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_apm,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP_DBGCORE, NULL,
	v620_evt0_cmucal_vclk_ip_mailbox_ap_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_RTC, NULL, v620_evt0_cmucal_vclk_ip_apbif_rtc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ROM_CRC32_HOST, NULL,
	v620_evt0_cmucal_vclk_ip_rom_crc32_host, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_APM_SFI0, NULL,
	v620_evt0_cmucal_vclk_ip_mailbox_apm_sfi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_APM_SFI1, NULL,
	v620_evt0_cmucal_vclk_ip_mailbox_apm_sfi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SPMI_MASTER_PMIC, NULL,
	v620_evt0_cmucal_vclk_ip_spmi_master_pmic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ROM_CRC32_HCU, NULL, v620_evt0_cmucal_vclk_ip_rom_crc32_hcu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_HW_SCANDUMP_CLKSTOP_CTRL, NULL,
	v620_evt0_cmucal_vclk_ip_hw_scandump_clkstop_ctrl, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APM_DTA, NULL, v620_evt0_cmucal_vclk_ip_apm_dta, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_APM, NULL, v620_evt0_cmucal_vclk_ip_vgen_lite_apm,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_ALIVE, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_alive,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SPC_APM, NULL, v620_evt0_cmucal_vclk_ip_spc_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_INTCOMB_VGPIO2PMU, NULL,
	v620_evt0_cmucal_vclk_ip_apbif_intcomb_vgpio2pmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_INTCOMB_VGPIO2APM, NULL,
	v620_evt0_cmucal_vclk_ip_apbif_intcomb_vgpio2apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_INTCOMB_VGPIO2AP, NULL,
	v620_evt0_cmucal_vclk_ip_apbif_intcomb_vgpio2ap, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D_ALIVE, NULL, v620_evt0_cmucal_vclk_ip_xiu_d_alive,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_INTMEM, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_intmem,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_ALIVE, NULL, v620_evt0_cmucal_vclk_ip_bic_apb_alive,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IP_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ip_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_ID_DBGCORE_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_dbgcore_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PMU, NULL, v620_evt0_cmucal_vclk_ip_pmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ASYNCAHB_MI_APM, NULL,
	v620_evt0_cmucal_vclk_ip_asyncahb_mi_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AUD_CMU_AUD, NULL, v620_evt0_cmucal_vclk_ip_aud_cmu_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D_AUD, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_AUD, NULL, v620_evt0_cmucal_vclk_ip_sysreg_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ABOX, NULL, v620_evt0_cmucal_vclk_ip_abox, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPIO_AUD, NULL, v620_evt0_cmucal_vclk_ip_gpio_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AXI_US_32TO128, NULL,
	v620_evt0_cmucal_vclk_ip_axi_us_32to128, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WDT_AUD0, NULL, v620_evt0_cmucal_vclk_ip_wdt_aud0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DFTMUX_AUD, NULL, v620_evt0_cmucal_vclk_ip_dftmux_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AD_APB_SMMU_AUD_S1_NS, NULL,
	v620_evt0_cmucal_vclk_ip_ad_apb_smmu_aud_s1_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WDT_AUD1, NULL, v620_evt0_cmucal_vclk_ip_wdt_aud1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_AUD, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IP_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IP_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ip_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D_AUD, NULL, v620_evt0_cmucal_vclk_ip_xiu_d_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_AUD, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_AUD0, NULL, v620_evt0_cmucal_vclk_ip_vgen_aud0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_AUD1, NULL, v620_evt0_cmucal_vclk_ip_vgen_aud1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_AUD, NULL, v620_evt0_cmucal_vclk_ip_vgen_lite_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON_PLL_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_clkmon_pll_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON_PLL_AVB, NULL,
	v620_evt0_cmucal_vclk_ip_clkmon_pll_avb, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P0_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p0_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P1_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p1_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_AUD, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_aud,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_AUD2, NULL, v620_evt0_cmucal_vclk_ip_vgen_aud2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ADD_CH_CLK, NULL, v620_evt0_cmucal_vclk_ip_add_ch_clk,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_CMU, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_CMU, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_cmu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFR_APBIF_CMU, NULL, v620_evt0_cmucal_vclk_ip_sfr_apbif_cmu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON0_PLL_SHARED, NULL,
	v620_evt0_cmucal_vclk_ip_clkmon0_pll_shared, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON1_PLL_SHARED, NULL,
	v620_evt0_cmucal_vclk_ip_clkmon1_pll_shared, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_CPUCL0, NULL, v620_evt0_cmucal_vclk_ip_sysreg_cpucl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CSSYS, NULL, v620_evt0_cmucal_vclk_ip_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_T_BDU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_t_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_IT0_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_it0_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_IT1_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_it1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_IT2_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_it2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_CHI_SI_D_CLUSTER0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_chi_si_d_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_IT0_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_it0_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_IT1_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_it1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_IT2_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_it2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_IT3_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_it3_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CPUCL0_CMU_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_cpucl0_cmu_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLUSTER0, NULL, v620_evt0_cmucal_vclk_ip_cluster0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_CPUCL0, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_cpucl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IG_CSSYS, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ig_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IG_CSSYS, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ig_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_P_CPUCL0, NULL, v620_evt0_cmucal_vclk_ip_xiu_p_cpucl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_DP_CSSYS, NULL, v620_evt0_cmucal_vclk_ip_xiu_dp_cssys,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_G_CSSYS, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_g_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APB_ASYNC_P_CSSYS_0, NULL,
	v620_evt0_cmucal_vclk_ip_apb_async_p_cssys_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IG_ETR, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ig_etr, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IG_ETR, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ig_etr, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BPS_CPUCL0, NULL, v620_evt0_cmucal_vclk_ip_bps_cpucl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_LT0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_LT1_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_LT2_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_LT3_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ADM_APB_G_CLUSTER0, NULL,
	v620_evt0_cmucal_vclk_ip_adm_apb_g_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IG_STM, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ig_stm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_CPUCL0, NULL, v620_evt0_cmucal_vclk_ip_ppmu_cpucl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_LT1_CPUCL2_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt1_cpucl2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_LT0_CPUCL2_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_lt0_cpucl2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CPUCL0_CON, NULL, v620_evt0_cmucal_vclk_ip_cpucl0_con,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_L_IRI_GIC_CLUSTER0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_iri_gic_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_L_ICC_CLUSTER0_GIC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_l_icc_cluster0_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IG_STM, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ig_stm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_G_DBGCORE_ALIVE_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_g_dbgcore_alive_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_CLUSTER0_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cluster0_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_IT3_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_it3_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON_PLL_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_clkmon_pll_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SECJTAG, NULL, v620_evt0_cmucal_vclk_ip_secjtag, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QOS_OVERR_D_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_qos_overr_d_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_sfmpu_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_P_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_P1_CPUCL0, NULL, v620_evt0_cmucal_vclk_ip_xiu_p1_cpucl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLUSTER2, NULL, v620_evt0_cmucal_vclk_ip_cluster2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CPUCL2_CMU_CPUCL2, NULL,
	v620_evt0_cmucal_vclk_ip_cpucl2_cmu_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ADM_APB_G_CLUSTER2, NULL,
	v620_evt0_cmucal_vclk_ip_adm_apb_g_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_CPUCL2, NULL, v620_evt0_cmucal_vclk_ip_ppmu_cpucl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_CPUCL2, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CPUCL2_CON, NULL, v620_evt0_cmucal_vclk_ip_cpucl2_con,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_L_IRI_GIC_CLUSTER2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_iri_gic_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_L_ICC_CLUSTER2_GIC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_l_icc_cluster2_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_CLUSTER2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cluster2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_CHI_SI_D_CLUSTER2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_chi_si_d_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_LT0_CPUCL2_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_lt0_cpucl2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_LT1_CPUCL2_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_lt1_cpucl2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_CPUCL2, NULL, v620_evt0_cmucal_vclk_ip_sysreg_cpucl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_CPUCL2, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_cpucl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON_PLL_CPUCL2, NULL,
	v620_evt0_cmucal_vclk_ip_clkmon_pll_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QOS_OVERR_D_CPUCL2, NULL,
	v620_evt0_cmucal_vclk_ip_qos_overr_d_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_CPUCL2, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_CPUCL2, NULL,
	v620_evt0_cmucal_vclk_ip_sfmpu_p_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_P_CPUCL2, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_p_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_P_CPUCL2, NULL, v620_evt0_cmucal_vclk_ip_xiu_p_cpucl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DBGCORE_CMU_DBGCORE, NULL,
	v620_evt0_cmucal_vclk_ip_dbgcore_cmu_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GREBEINTEGRATION_DBGCORE, NULL,
	v620_evt0_cmucal_vclk_ip_grebeintegration_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_ID_DBGCORE_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_id_dbgcore_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IP_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_G_SCAN2DRAM_ALIVE_MIF0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_g_scan2dram_alive_mif0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_DBGCORE, NULL,
	v620_evt0_cmucal_vclk_ip_sysreg_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_G_DBGCORE_ALIVE_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_g_dbgcore_alive_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WDT_DBGCORE, NULL, v620_evt0_cmucal_vclk_ip_wdt_dbgcore,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AHB_BUSMATRIX_DBGCORE, NULL,
	v620_evt0_cmucal_vclk_ip_ahb_busmatrix_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XHB_DBGCORE, NULL, v620_evt0_cmucal_vclk_ip_xhb_dbgcore,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_CSSYS_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_apbif_cssys_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_DBGCORE_CORE, NULL,
	v620_evt0_cmucal_vclk_ip_sysreg_dbgcore_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_DBGCORE, NULL,
	v620_evt0_cmucal_vclk_ip_d_tzpc_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_APBIF_S2D_DBGCORE, NULL,
	v620_evt0_cmucal_vclk_ip_apbif_s2d_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DNC_CMU_DNC, NULL, v620_evt0_cmucal_vclk_ip_dnc_cmu_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_DNC, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_CTRL_GNPU_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_gnpu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD3_MMU_SDMA_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld3_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_CTRL_DNC_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_dnc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_SRAM_DNC_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_sram_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_ID_IPDNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_ipdnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD2_MMU_SDMA_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld2_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_DNC, NULL, v620_evt0_cmucal_vclk_ip_sysreg_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TREX_D_DNC, NULL, v620_evt0_cmucal_vclk_ip_trex_d_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_DRAM_DSP_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_dram_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD1_MMU_SDMA_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld1_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_DRAM_GNPU_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_dram_gnpu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_DNC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_DNC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D2_DNC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d2_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D3_DNC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d3_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D4_DNC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d4_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_ID_IPDNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_id_ipdnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_CTRL_DNC_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_CTRL_DNC_DSP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_IP_DNC, NULL, v620_evt0_cmucal_vclk_ip_ip_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_VGEN_LITE_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_as_apb_vgen_lite_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_DNC, NULL, v620_evt0_cmucal_vclk_ip_vgen_lite_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_LP_DNC_DSP, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_lp_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_LP_DNC_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_lp_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ADM_DAP_DNC, NULL, v620_evt0_cmucal_vclk_ip_adm_dap_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU1_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU2_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu2_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU3_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu3_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S1_PMMU0_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s1_pmmu0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD0_MMU_SDMA_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld0_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_CTRL_DSP_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_LP_DNC_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_lp_dnc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D0_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D1_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D2_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d2_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D3_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d3_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_DNC, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S1_DNC, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s1_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D_DNC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LG_DNC_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_lg_dnc_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P0_DNC, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p0_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P1_DNC, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p1_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_S0_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_bic_apb_s0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_S1_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_bic_apb_s1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BAAW_P_DNC, NULL, v620_evt0_cmucal_vclk_ip_baaw_p_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_DPTX, NULL, v620_evt0_cmucal_vclk_ip_sysreg_dptx,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_DPTX, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_DPTX, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_dptx,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DPTX_CMU_DPTX, NULL, v620_evt0_cmucal_vclk_ip_dptx_cmu_dptx,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DP_LINK0, NULL, v620_evt0_cmucal_vclk_ip_dp_link0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST0, NULL,
	v620_evt0_cmucal_vclk_ip_dwc_usbc31dptxphy_upcs_x4_ns_x1_x4_pipe_wrap_inst0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_DPTX, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_dptx,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_DPTX, NULL, v620_evt0_cmucal_vclk_ip_bic_dptx,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DPUB_CMU_DPUB, NULL, v620_evt0_cmucal_vclk_ip_dpub_cmu_dpub,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_DPUB, NULL, v620_evt0_cmucal_vclk_ip_sysreg_dpub,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_DPUB, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_DPUB, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_dpub,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AD_APB_DECON_MAIN_DPUB0, NULL,
	v620_evt0_cmucal_vclk_ip_ad_apb_decon_main_dpub0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DPUB, NULL, v620_evt0_cmucal_vclk_ip_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_DPUB, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_dpub,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P0_DPUB, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p0_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DPUF_CMU_DPUF, NULL, v620_evt0_cmucal_vclk_ip_dpuf_cmu_dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_DPUF, NULL, v620_evt0_cmucal_vclk_ip_sysreg_dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU3_DPUF, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu3_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU2_DPUF, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu2_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU1_DPUF, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_DPUF, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_DPUF1, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_dpuf1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_DPUF0, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_dpuf0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_DPUF1, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_dpuf1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_DPUF0, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_dpuf0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_DPUF, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D0_DPUF, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d0_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D1_DPUF, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d1_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_DPUF, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DPUF, NULL, v620_evt0_cmucal_vclk_ip_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_DPUF, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_DPUF, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AD_APB_DPUF0_DMA, NULL,
	v620_evt0_cmucal_vclk_ip_ad_apb_dpuf0_dma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_DPUF, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_DPUF, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_DPUF, NULL, v620_evt0_cmucal_vclk_ip_bic_dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN0_0DPUF, NULL, v620_evt0_cmucal_vclk_ip_vgen0_0dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN0_1DPUF, NULL, v620_evt0_cmucal_vclk_ip_vgen0_1dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN1_0DPUF, NULL, v620_evt0_cmucal_vclk_ip_vgen1_0dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN1_1DPUF, NULL, v620_evt0_cmucal_vclk_ip_vgen1_1dpuf,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DPUF1_CMU_DPUF1, NULL,
	v620_evt0_cmucal_vclk_ip_dpuf1_cmu_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DSP_CMU_DSP, NULL, v620_evt0_cmucal_vclk_ip_dsp_cmu_dsp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_DSP, NULL, v620_evt0_cmucal_vclk_ip_sysreg_dsp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_CTRL_DNC_DSP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_SRAM_SDMA_DSP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_sram_sdma_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_LP_DNC_DSP, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_lp_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_DSP_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_dsp_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_CTRL_DSP_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_DRAM_DSP_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_dram_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_DSP, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_dsp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_IP_DSP, NULL, v620_evt0_cmucal_vclk_ip_ip_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_DSP, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_dsp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_S_DSP, NULL, v620_evt0_cmucal_vclk_ip_bic_apb_s_dsp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_G3D_CMU_G3D, NULL, v620_evt0_cmucal_vclk_ip_g3d_cmu_g3d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_G3D, NULL, v620_evt0_cmucal_vclk_ip_sysreg_g3d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IP_G3D, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ip_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_G3D, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_G3D, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_g3d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BG3D_PWRCTL, NULL, v620_evt0_cmucal_vclk_ip_bg3d_pwrctl,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ADM_DAP_G_G3D, NULL, v620_evt0_cmucal_vclk_ip_adm_dap_g_g3d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPU, NULL, v620_evt0_cmucal_vclk_ip_gpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ASB_G3D, NULL, v620_evt0_cmucal_vclk_ip_asb_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GNPU_CMU_GNPU, NULL, v620_evt0_cmucal_vclk_ip_gnpu_cmu_gnpu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_LP_DNC_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_lp_dnc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_GNPU, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_gnpu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDRSP0_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp0_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_CTRL_DNC_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_dnc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_DRAM_GNPU_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_dram_gnpu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_GNPU, NULL, v620_evt0_cmucal_vclk_ip_sysreg_gnpu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_CTRL_GNPU_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_ctrl_gnpu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDRSP1_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp1_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_IP_NPUCORE, NULL, v620_evt0_cmucal_vclk_ip_ip_npucore,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDRSP2_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp2_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDRSP3_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp3_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDRSP4_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp4_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDRSP5_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdrsp5_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_CSTFIFO_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_cstfifo_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_WR0_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_wr0_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_WR1_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_wr1_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_WR2_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_wr2_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDREQ0_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq0_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDREQ1_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq1_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDREQ2_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq2_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDREQ3_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq3_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDREQ5_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq5_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDREQ4_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdreq4_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_GNPU, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_gnpu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_S_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_bic_apb_s_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_HSI0_CMU_HSI0, NULL, v620_evt0_cmucal_vclk_ip_hsi0_cmu_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_SI_D0_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_si_d0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPIO_HSI0, NULL, v620_evt0_cmucal_vclk_ip_gpio_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_HSI0, NULL, v620_evt0_cmucal_vclk_ip_sysreg_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_HSI0, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_HSI0, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_P0_HSI0, NULL, v620_evt0_cmucal_vclk_ip_xiu_p0_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PCIE_GEN5_2L, NULL, v620_evt0_cmucal_vclk_ip_pcie_gen5_2l,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PCIE_IA_GEN5A_2L, NULL,
	v620_evt0_cmucal_vclk_ip_pcie_ia_gen5a_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PCIE_IA_GEN5B_2L, NULL,
	v620_evt0_cmucal_vclk_ip_pcie_ia_gen5b_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_HSI0_0, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_hsi0_0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_HSI0, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_HSI0, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PCIE_IA_GEN5A_4L, NULL,
	v620_evt0_cmucal_vclk_ip_pcie_ia_gen5a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PCIE_IA_GEN5B_4L, NULL,
	v620_evt0_cmucal_vclk_ip_pcie_ia_gen5b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_SI_D1_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_si_d1_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PCIE_GEN5_4L, NULL, v620_evt0_cmucal_vclk_ip_pcie_gen5_4l,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_HSI0_1, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_hsi0_1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_PCIE_GEN5A_2L, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_pcie_gen5a_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_PCIE_GEN5A_4L, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_pcie_gen5a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_PCIE_GEN5B_2L, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_pcie_gen5b_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_PCIE_GEN5B_4L, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_pcie_gen5b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_PCIE_GEN5A_2L, NULL,
	v620_evt0_cmucal_vclk_ip_qe_pcie_gen5a_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_PCIE_GEN5A_4L, NULL,
	v620_evt0_cmucal_vclk_ip_qe_pcie_gen5a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_PCIE_GEN5B_2L, NULL,
	v620_evt0_cmucal_vclk_ip_qe_pcie_gen5b_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_PCIE_GEN5B_4L, NULL,
	v620_evt0_cmucal_vclk_ip_qe_pcie_gen5b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU1_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D2_HSI0, NULL, v620_evt0_cmucal_vclk_ip_xiu_d2_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ASYNC_APB_PCIE_GEN5_4L_SUB_CTRL, NULL,
	v620_evt0_cmucal_vclk_ip_async_apb_pcie_gen5_4l_sub_ctrl, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_HSI1_CMU_HSI1, NULL, v620_evt0_cmucal_vclk_ip_hsi1_cmu_hsi1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MMC_CARD, NULL, v620_evt0_cmucal_vclk_ip_mmc_card,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_HSI1, NULL, v620_evt0_cmucal_vclk_ip_sysreg_hsi1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPIO_HSI1, NULL, v620_evt0_cmucal_vclk_ip_gpio_hsi1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_SI_D_HSI1, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_si_d_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_HSI1, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_HSI1, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_hsi1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_P_HSI1, NULL, v620_evt0_cmucal_vclk_ip_xiu_p_hsi1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_HSI1, NULL, v620_evt0_cmucal_vclk_ip_ppmu_hsi1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_HSI1, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_lite_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_HSI1, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_hsi1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USB20DRD_0, NULL, v620_evt0_cmucal_vclk_ip_usb20drd_0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USB20DRD_1, NULL, v620_evt0_cmucal_vclk_ip_usb20drd_1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USB30DRD_0, NULL, v620_evt0_cmucal_vclk_ip_usb30drd_0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_HSI1, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_HSI1, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_hsi1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_HSI1, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ETHERNET0, NULL, v620_evt0_cmucal_vclk_ip_ethernet0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ETHERNET1, NULL, v620_evt0_cmucal_vclk_ip_ethernet1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_SI_D0_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_si_d0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_UFS_EMBD0, NULL, v620_evt0_cmucal_vclk_ip_qe_ufs_embd0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_UFS_EMBD0, NULL, v620_evt0_cmucal_vclk_ip_ufs_embd0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_UFS_EMBD0, NULL,
	v620_evt0_cmucal_vclk_ip_ppmu_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D2_HSI2, NULL, v620_evt0_cmucal_vclk_ip_xiu_d2_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_UFS_EMBD0, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D3_HSI2, NULL, v620_evt0_cmucal_vclk_ip_xiu_d3_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_SI_D1_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_si_d1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_ETHERNET0, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_ETHERNET1, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_ETHERNET0, NULL,
	v620_evt0_cmucal_vclk_ip_ppmu_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_ETHERNET1, NULL,
	v620_evt0_cmucal_vclk_ip_ppmu_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_ETHERNET0, NULL, v620_evt0_cmucal_vclk_ip_qe_ethernet0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_ETHERNET1, NULL, v620_evt0_cmucal_vclk_ip_qe_ethernet1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S1_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S1_PMMU0_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s1_pmmu0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_HSI2, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ASYNCAPB_SYSMMU_S0_HSI2_NS, NULL,
	v620_evt0_cmucal_vclk_ip_asyncapb_sysmmu_s0_hsi2_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_HSI2, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPIO_HSI2, NULL, v620_evt0_cmucal_vclk_ip_gpio_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_HSI2_CMU_HSI2, NULL, v620_evt0_cmucal_vclk_ip_hsi2_cmu_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_HSI2, NULL, v620_evt0_cmucal_vclk_ip_sysreg_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON_PLL_ETH, NULL,
	v620_evt0_cmucal_vclk_ip_clkmon_pll_eth, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPIO_HSI2UFS, NULL, v620_evt0_cmucal_vclk_ip_gpio_hsi2ufs,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D4_HSI2, NULL, v620_evt0_cmucal_vclk_ip_xiu_d4_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D5_HSI2, NULL, v620_evt0_cmucal_vclk_ip_xiu_d5_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_P_HSI2, NULL, v620_evt0_cmucal_vclk_ip_xiu_p_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D0_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D1_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P0_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P1_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_HSI2, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SPC_HSI2, NULL, v620_evt0_cmucal_vclk_ip_spc_hsi2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_ISP, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_ISP, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_ISP, NULL, v620_evt0_cmucal_vclk_ip_sysreg_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D_ISP, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D0_ISP, NULL, v620_evt0_cmucal_vclk_ip_qe_d0_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ISP, NULL, v620_evt0_cmucal_vclk_ip_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_ISP0, NULL, v620_evt0_cmucal_vclk_ip_as_apb_isp0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_L0_TAA_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_l0_taa_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_L0_ISP_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_l0_isp_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_ISP, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ISP_CMU_ISP, NULL, v620_evt0_cmucal_vclk_ip_isp_cmu_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_ISP, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_ISP, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AST_MI_L1_TAA_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_slh_ast_mi_l1_taa_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AST_SI_L1_ISP_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_slh_ast_si_l1_isp_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D0_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d0_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_ISP, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_isp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_M2M_CMU_M2M, NULL, v620_evt0_cmucal_vclk_ip_m2m_cmu_m2m,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_M2M, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_m2m,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_M2M, NULL, v620_evt0_cmucal_vclk_ip_sysreg_m2m,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_M2M, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_m2m,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_M2M, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_M2M, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_m2m,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_M2M, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_M2M0, NULL, v620_evt0_cmucal_vclk_ip_as_apb_m2m0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_M2M, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_m2m,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_M2M, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_m2m,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_JPEG, NULL, v620_evt0_cmucal_vclk_ip_qe_d_jpeg,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_M2M, NULL, v620_evt0_cmucal_vclk_ip_qe_d_m2m,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_D_M2M, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_lite_d_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_M2M_D0, NULL, v620_evt0_cmucal_vclk_ip_m2m_d0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_JPEG, NULL, v620_evt0_cmucal_vclk_ip_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IP_JPEG, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IP_JPEG, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ip_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_SI_D0_M2M, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_si_d0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_M2M_JPEG, NULL,
	v620_evt0_cmucal_vclk_ip_as_apb_m2m_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MFC_CMU_MFC, NULL, v620_evt0_cmucal_vclk_ip_mfc_cmu_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_MFC, NULL, v620_evt0_cmucal_vclk_ip_sysreg_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D1_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU1_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_MFC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_MFC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D2_WFD, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d2_wfd,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_MFC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_WFD_NS, NULL, v620_evt0_cmucal_vclk_ip_as_apb_wfd_ns,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MFC, NULL, v620_evt0_cmucal_vclk_ip_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WFD, NULL, v620_evt0_cmucal_vclk_ip_wfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_MFC, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_MI_IT_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_mi_it_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_MFC, NULL, v620_evt0_cmucal_vclk_ip_as_apb_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_D_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_lite_d_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_ID_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_MFC, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_MFC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_IT_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_it_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D0_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_ID_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_id_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_FG, NULL, v620_evt0_cmucal_vclk_ip_fg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D3_MFC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d3_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D4_MFC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d4_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_D_FG_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_lite_d_fg_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D2_MFC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d2_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D3_MFC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d3_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S1_PMMU0_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s1_pmmu0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S1_MFC, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s1_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D2_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d2_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_FG_MFC, NULL, v620_evt0_cmucal_vclk_ip_as_apb_fg_mfc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MIF_CMU_MIF, NULL, v620_evt0_cmucal_vclk_ip_mif_cmu_mif,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DDRPHY0, NULL, v620_evt0_cmucal_vclk_ip_ddrphy0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_MIF, NULL, v620_evt0_cmucal_vclk_ip_sysreg_mif,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_MIF, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPC_DEBUG0, NULL, v620_evt0_cmucal_vclk_ip_ppc_debug0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SMC0, NULL, v620_evt0_cmucal_vclk_ip_smc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QCH_ADAPTER_PPC_DEBUG0, NULL,
	v620_evt0_cmucal_vclk_ip_qch_adapter_ppc_debug0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_MIF, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_mif,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SMC1, NULL, v620_evt0_cmucal_vclk_ip_smc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DDRPHY1, NULL, v620_evt0_cmucal_vclk_ip_ddrphy1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPC_DEBUG1, NULL, v620_evt0_cmucal_vclk_ip_ppc_debug1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QCH_ADAPTER_PPC_DEBUG1, NULL,
	v620_evt0_cmucal_vclk_ip_qch_adapter_ppc_debug1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_MIF, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_mif,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SPC_MIF, NULL, v620_evt0_cmucal_vclk_ip_spc_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SPMPU_P_MIF, NULL, v620_evt0_cmucal_vclk_ip_spmpu_p_mif,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QCH_ADAPTER_SMC0, NULL,
	v620_evt0_cmucal_vclk_ip_qch_adapter_smc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QCH_ADAPTER_SMC1, NULL,
	v620_evt0_cmucal_vclk_ip_qch_adapter_smc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QCH_ADAPTER_DDRPHY0, NULL,
	v620_evt0_cmucal_vclk_ip_qch_adapter_ddrphy0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QCH_ADAPTER_DDRPHY1, NULL,
	v620_evt0_cmucal_vclk_ip_qch_adapter_ddrphy1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON_PLL_MIF, NULL,
	v620_evt0_cmucal_vclk_ip_clkmon_pll_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_S_MIF, NULL, v620_evt0_cmucal_vclk_ip_bic_apb_s_mif,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MISC_CMU_MISC, NULL, v620_evt0_cmucal_vclk_ip_misc_cmu_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_SI_D_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_si_d_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PDMA0, NULL, v620_evt0_cmucal_vclk_ip_pdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PDMA1, NULL, v620_evt0_cmucal_vclk_ip_pdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PDMA2, NULL, v620_evt0_cmucal_vclk_ip_pdma2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PDMA3, NULL, v620_evt0_cmucal_vclk_ip_pdma3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PDMA4, NULL, v620_evt0_cmucal_vclk_ip_pdma4, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_MISC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_PDMA0, NULL, v620_evt0_cmucal_vclk_ip_qe_d_pdma0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_PDMA1, NULL, v620_evt0_cmucal_vclk_ip_qe_d_pdma1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_PDMA2, NULL, v620_evt0_cmucal_vclk_ip_qe_d_pdma2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_PDMA3, NULL, v620_evt0_cmucal_vclk_ip_qe_d_pdma3,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_PDMA4, NULL, v620_evt0_cmucal_vclk_ip_qe_d_pdma4,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_SPDMA0, NULL, v620_evt0_cmucal_vclk_ip_qe_d_spdma0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_SPDMA1, NULL, v620_evt0_cmucal_vclk_ip_qe_d_spdma1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SPDMA0, NULL, v620_evt0_cmucal_vclk_ip_spdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SPDMA1, NULL, v620_evt0_cmucal_vclk_ip_spdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_MISC, NULL, v620_evt0_cmucal_vclk_ip_sysreg_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_PDMA0, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_pdma0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_PDMA1, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_pdma1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_PDMA2, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_pdma2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_PDMA3, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_pdma3,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_PDMA4, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_pdma4,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_SPDMA0, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_spdma0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_SPDMA1, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_spdma1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_MISC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_MISC, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AD_APB_SYSMMU_S0_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_ad_apb_sysmmu_s0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_MISC_1, NULL, v620_evt0_cmucal_vclk_ip_sysreg_misc_1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_MISC_2, NULL, v620_evt0_cmucal_vclk_ip_sysreg_misc_2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_MISC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MCT0_MISC, NULL, v620_evt0_cmucal_vclk_ip_mct0_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_OTP_CON_BIRA, NULL, v620_evt0_cmucal_vclk_ip_otp_con_bira,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_OTP_CON_BISR, NULL, v620_evt0_cmucal_vclk_ip_otp_con_bisr,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_OTP_CON_TOP, NULL, v620_evt0_cmucal_vclk_ip_otp_con_top,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WDT_CLUSTER0, NULL, v620_evt0_cmucal_vclk_ip_wdt_cluster0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WDT_CLUSTER1, NULL, v620_evt0_cmucal_vclk_ip_wdt_cluster1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_L_ICC_CLUSTER0_GIC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_icc_cluster0_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_L_ICC_CLUSTER2_GIC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_icc_cluster2_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_L_IRI_GIC_CLUSTER0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_l_iri_gic_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_L_IRI_GIC_CLUSTER2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_l_iri_gic_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_MISC_GIC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_misc_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D_GIC, NULL, v620_evt0_cmucal_vclk_ip_qe_d_gic,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_GIC, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_gic,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TMU_1, NULL, v620_evt0_cmucal_vclk_ip_tmu_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TMU_0, NULL, v620_evt0_cmucal_vclk_ip_tmu_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VOLMON_INT, NULL, v620_evt0_cmucal_vclk_ip_volmon_int,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_ID_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_id_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_ID_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ADC_MISC, NULL, v620_evt0_cmucal_vclk_ip_adc_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_MISC, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S1_PMMU0_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s1_pmmu0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S1_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s1_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D2_MISC, NULL, v620_evt0_cmucal_vclk_ip_xiu_d2_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_MISC, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GIC, NULL, v620_evt0_cmucal_vclk_ip_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MCT1_MISC, NULL, v620_evt0_cmucal_vclk_ip_mct1_misc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AXI_US_64TO128_D_GIC, NULL,
	v620_evt0_cmucal_vclk_ip_axi_us_64to128_d_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_NOCL0_CMU_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_nocl0_cmu_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_NOCL0, NULL, v620_evt0_cmucal_vclk_ip_sysreg_nocl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MPACE2AXI_DP0_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_mpace2axi_dp0_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MPACE2AXI_DP1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_mpace2axi_dp1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPC_G_SCI, NULL, v620_evt0_cmucal_vclk_ip_ppc_g_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TREX_P_NOCL0, NULL, v620_evt0_cmucal_vclk_ip_trex_p_nocl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_T_BDU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_t_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BDU, NULL, v620_evt0_cmucal_vclk_ip_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_G3D, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_CPUCL0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D0_G3D, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d0_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D1_G3D, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d1_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D2_G3D, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d2_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D3_G3D, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d3_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TREX_D_NOCL0, NULL, v620_evt0_cmucal_vclk_ip_trex_d_nocl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_NOCL0, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_nocl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D_CPUCL2, NULL, v620_evt0_cmucal_vclk_ip_wow_d_cpucl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D2_G3D, NULL, v620_evt0_cmucal_vclk_ip_wow_d2_g3d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D3_G3D, NULL, v620_evt0_cmucal_vclk_ip_wow_d3_g3d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D0_TREX, NULL, v620_evt0_cmucal_vclk_ip_wow_d0_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D1_TREX, NULL, v620_evt0_cmucal_vclk_ip_wow_d1_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_G_NOCL1, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_g_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MPACE_ASB_D0_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_mpace_asb_d0_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MPACE_ASB_D1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_mpace_asb_d1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MPACE_ASB_D2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_mpace_asb_d2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MPACE_ASB_D3_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_mpace_asb_d3_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_CHI_MI_D_CLUSTER0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_chi_mi_d_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_MIF0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_mif0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D0_G3D, NULL, v620_evt0_cmucal_vclk_ip_wow_d0_g3d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D1_G3D, NULL, v620_evt0_cmucal_vclk_ip_wow_d1_g3d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D2_TREX, NULL, v620_evt0_cmucal_vclk_ip_wow_d2_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D3_TREX, NULL, v620_evt0_cmucal_vclk_ip_wow_d3_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_CPUCL2, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_G_NOCL2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_g_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D_CPUCL0, NULL, v620_evt0_cmucal_vclk_ip_wow_d_cpucl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_CHI_MI_D_CLUSTER2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_chi_mi_d_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_MIF1, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_mif1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_PERIC0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_PERIC1, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_MISC_GIC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_misc_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_SCI, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_SCI, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D2_SCI, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d2_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D3_SCI, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d3_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D0_SCI, NULL, v620_evt0_cmucal_vclk_ip_wow_d0_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D1_SCI, NULL, v620_evt0_cmucal_vclk_ip_wow_d1_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D2_SCI, NULL, v620_evt0_cmucal_vclk_ip_wow_d2_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D3_SCI, NULL, v620_evt0_cmucal_vclk_ip_wow_d3_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_CLUSTER0_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cluster0_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_CLUSTER2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_cluster2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D0_NOCL1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d0_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D1_NOCL1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d1_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D2_NOCL1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d2_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D3_NOCL1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d3_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_P_NOCL0_NOCL1, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_p_nocl0_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_P_NOCL0_NOCL2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_p_nocl0_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_TREX, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_TREX, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D2_TREX, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d2_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D3_TREX, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d3_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_DP0_SCI, NULL, v620_evt0_cmucal_vclk_ip_ppmu_dp0_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_DP1_SCI, NULL, v620_evt0_cmucal_vclk_ip_ppmu_dp1_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_P_CPUCL0, NULL, v620_evt0_cmucal_vclk_ip_ppmu_p_cpucl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_P_CPUCL2, NULL, v620_evt0_cmucal_vclk_ip_ppmu_p_cpucl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WOW_D_TREX_QURGENT, NULL,
	v620_evt0_cmucal_vclk_ip_wow_d_trex_qurgent, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_DP2_SCI, NULL, v620_evt0_cmucal_vclk_ip_ppmu_dp2_sci,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_CMU, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LG_DNC_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_lg_dnc_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_DP_TREX, NULL, v620_evt0_cmucal_vclk_ip_ppmu_dp_trex,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D0_NOCL2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d0_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D1_NOCL2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d1_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D2_NOCL2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d2_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D3_NOCL2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d3_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ADM_APB_G_BDU, NULL, v620_evt0_cmucal_vclk_ip_adm_apb_g_bdu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_P_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_bic_apb_p_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_NOCL0, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_nocl0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SCI, NULL, v620_evt0_cmucal_vclk_ip_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_NOCL1_CMU_NOCL1, NULL,
	v620_evt0_cmucal_vclk_ip_nocl1_cmu_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_sysreg_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TREX_D_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_trex_d_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D0_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D1_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d1_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D_ALIVE, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TREX_P_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_trex_p_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_G_NOCL1, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_g_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_G_CSSYS, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_g_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_AUD, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_HSI0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D_G3D_PTW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_g3d_ptw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CACHEAID_NOCL1, NULL,
	v620_evt0_cmucal_vclk_ip_cacheaid_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D0_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D1_HSI2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D0_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D1_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D2_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d2_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_D3_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_d3_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_P_NOCL0_NOCL1, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_p_nocl0_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D0_NOCL1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d0_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D2_NOCL1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d2_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D1_NOCL1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d1_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D3_NOCL1_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d3_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D_ALIVE, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d_alive,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_G_CSSYS, NULL, v620_evt0_cmucal_vclk_ip_ppmu_g_cssys,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D_G3DMMU, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d_g3dmmu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D2_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d2_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D3_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d3_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_NOCL1, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_nocl1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_P_NOCL1, NULL,
	v620_evt0_cmucal_vclk_ip_bic_apb_p_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D_MISC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_P_HSI0, NULL, v620_evt0_cmucal_vclk_ip_ppmu_p_hsi0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TREX_D_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_trex_d_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_sysreg_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D_HSI1, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D0_DPUF0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d0_dpuf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D0_DPUF1, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d0_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D1_DPUF0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d1_dpuf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D1_DPUF1, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d1_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D_ACC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D0_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D_TAA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_G_NOCL2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_g_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TREX_P_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_trex_p_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_ACC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_DPUF1, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_DPTX, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_DPUB, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_DPUF0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_dpuf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_HSI1, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_TAA, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_NOCL2_CMU_NOCL2, NULL,
	v620_evt0_cmucal_vclk_ip_nocl2_cmu_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D1_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D2_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_MI_D0_M2M, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_mi_d0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_M2M, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D2_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d2_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D3_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d3_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D0_NOCL2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d0_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D1_NOCL2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d1_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D2_NOCL2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d2_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_SI_D3_NOCL2_NOCL0, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_si_d3_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_TAXI_MI_P_NOCL0_NOCL2, NULL,
	v620_evt0_cmucal_vclk_ip_lh_taxi_mi_p_nocl0_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CACHEAID_NOCL2, NULL,
	v620_evt0_cmucal_vclk_ip_cacheaid_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_NOCL2, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_nocl2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_P_NOCL2, NULL,
	v620_evt0_cmucal_vclk_ip_bic_apb_p_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_P_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_p_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D0_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D1_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_D2_MFC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_d2_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPIO_PERIC0, NULL, v620_evt0_cmucal_vclk_ip_gpio_peric0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_PERIC0, NULL, v620_evt0_cmucal_vclk_ip_sysreg_peric0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PERIC0_CMU_PERIC0, NULL,
	v620_evt0_cmucal_vclk_ip_peric0_cmu_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_PERIC0, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_PERIC0, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_peric0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI00_USI, NULL, v620_evt0_cmucal_vclk_ip_usi00_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI00_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi00_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI01_USI, NULL, v620_evt0_cmucal_vclk_ip_usi01_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI01_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi01_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI02_USI, NULL, v620_evt0_cmucal_vclk_ip_usi02_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI02_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi02_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PWM, NULL, v620_evt0_cmucal_vclk_ip_pwm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_PERIC0, NULL,
	v620_evt0_cmucal_vclk_ip_sfmpu_p_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI03_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi03_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI07_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi07_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI03_USI, NULL, v620_evt0_cmucal_vclk_ip_usi03_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI07_USI, NULL, v620_evt0_cmucal_vclk_ip_usi07_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_I3C2, NULL, v620_evt0_cmucal_vclk_ip_i3c2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPIO_PERIC1, NULL, v620_evt0_cmucal_vclk_ip_gpio_peric1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_PERIC1, NULL, v620_evt0_cmucal_vclk_ip_sysreg_peric1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PERIC1_CMU_PERIC1, NULL,
	v620_evt0_cmucal_vclk_ip_peric1_cmu_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_PERIC1, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_PERIC1, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_peric1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI09_USI, NULL, v620_evt0_cmucal_vclk_ip_usi09_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI09_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi09_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI11_USI, NULL, v620_evt0_cmucal_vclk_ip_usi11_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI11_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi11_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI12_USI, NULL, v620_evt0_cmucal_vclk_ip_usi12_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI12_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi12_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI13_USI, NULL, v620_evt0_cmucal_vclk_ip_usi13_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI13_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi13_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI14_USI, NULL, v620_evt0_cmucal_vclk_ip_usi14_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI15_USI, NULL, v620_evt0_cmucal_vclk_ip_usi15_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI14_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi14_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI15_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi15_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI16_USI, NULL, v620_evt0_cmucal_vclk_ip_usi16_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI16_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi16_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI17_USI, NULL, v620_evt0_cmucal_vclk_ip_usi17_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI17_I2C, NULL, v620_evt0_cmucal_vclk_ip_usi17_i2c,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_I3C4, NULL, v620_evt0_cmucal_vclk_ip_i3c4, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_I3C5, NULL, v620_evt0_cmucal_vclk_ip_i3c5, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_I3C6, NULL, v620_evt0_cmucal_vclk_ip_i3c6, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_I3C7, NULL, v620_evt0_cmucal_vclk_ip_i3c7, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_PERIC1, NULL,
	v620_evt0_cmucal_vclk_ip_sfmpu_p_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_S2D_CMU_S2D, NULL, v620_evt0_cmucal_vclk_ip_s2d_cmu_s2d,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIS_S2D, NULL, v620_evt0_cmucal_vclk_ip_bis_s2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_G_SCAN2DRAM_ALIVE_MIF, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_g_scan2dram_alive_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SDMA_CMU_SDMA, NULL, v620_evt0_cmucal_vclk_ip_sdma_cmu_sdma,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_SDMA, NULL, v620_evt0_cmucal_vclk_ip_sysreg_sdma,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDRSP1_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp1_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDRSP0_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp0_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD0_MMU_SDMA_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld0_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD1_MMU_SDMA_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld1_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD2_MMU_SDMA_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld2_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD3_MMU_SDMA_DNC, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld3_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_LD_SRAM_SDMA_DSP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ld_sram_sdma_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDRSP3_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp3_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDRSP2_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp2_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_SDMA, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_sdma,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_CTRL_DNC_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_ctrl_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_LP_DNC_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_lp_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_CSTFIFO_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_cstfifo_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_IP_SDMA_WRAP, NULL, v620_evt0_cmucal_vclk_ip_ip_sdma_wrap,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDREQ0_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq0_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDREQ1_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq1_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDREQ2_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq2_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDREQ3_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq3_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDREQ4_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq4_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_RDREQ5_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_rdreq5_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_WR0_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_wr0_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_WR1_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_wr1_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_WR2_GNPU_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_wr2_gnpu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDRSP4_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp4_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_LD_SRAM_RDRSP5_SDMA_GNPU, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_ld_sram_rdrsp5_sdma_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_LD_SRAM_DNC_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ld_sram_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_LD_SRAM_DSP_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_ld_sram_dsp_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_SDMA, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_sdma,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BIC_APB_S_SDMA, NULL,
	v620_evt0_cmucal_vclk_ip_bic_apb_s_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI19_USI, NULL, v620_evt0_cmucal_vclk_ip_usi19_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON0, NULL, v620_evt0_cmucal_vclk_ip_clkmon0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI18_USI, NULL, v620_evt0_cmucal_vclk_ip_usi18_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI20_USI, NULL, v620_evt0_cmucal_vclk_ip_usi20_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SC_SFI, NULL, v620_evt0_cmucal_vclk_ip_sc_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_SFI, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_GPIO_SFI, NULL, v620_evt0_cmucal_vclk_ip_gpio_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP4, NULL, v620_evt0_cmucal_vclk_ip_mailbox_ap4,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MCT0_SFI, NULL, v620_evt0_cmucal_vclk_ip_mct0_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ROM_CRC32_SFI, NULL, v620_evt0_cmucal_vclk_ip_rom_crc32_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_SFI, NULL, v620_evt0_cmucal_vclk_ip_sysreg_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VOLMON, NULL, v620_evt0_cmucal_vclk_ip_volmon, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WDT0_SFI, NULL, v620_evt0_cmucal_vclk_ip_wdt0_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_USI21_USI, NULL, v620_evt0_cmucal_vclk_ip_usi21_usi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_SFPC_SFI, NULL, v620_evt0_cmucal_vclk_ip_d_sfpc_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_INTMEM_SFI, NULL, v620_evt0_cmucal_vclk_ip_intmem_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ASYNCAPB_INTMEM, NULL,
	v620_evt0_cmucal_vclk_ip_asyncapb_intmem, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_DP_SFI, NULL, v620_evt0_cmucal_vclk_ip_xiu_dp_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XSPI, NULL, v620_evt0_cmucal_vclk_ip_xspi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_LT0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_lt0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_LT1_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_lt1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_LT2_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_lt2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ATB_SI_LT3_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_atb_si_lt3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WDT1_SFI, NULL, v620_evt0_cmucal_vclk_ip_wdt1_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLUSTER_SFI, NULL, v620_evt0_cmucal_vclk_ip_cluster_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON1, NULL, v620_evt0_cmucal_vclk_ip_clkmon1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CLKMON2, NULL, v620_evt0_cmucal_vclk_ip_clkmon2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IP1_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ip1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IP1_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IDP0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_idp0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IDP1_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_idp1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IP0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ip0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IDP0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_idp0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IDP1_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_idp1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IP0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP0, NULL, v620_evt0_cmucal_vclk_ip_mailbox_ap0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP1, NULL, v620_evt0_cmucal_vclk_ip_mailbox_ap1,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP7, NULL, v620_evt0_cmucal_vclk_ip_mailbox_ap7,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP2, NULL, v620_evt0_cmucal_vclk_ip_mailbox_ap2,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP3, NULL, v620_evt0_cmucal_vclk_ip_mailbox_ap3,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP6, NULL, v620_evt0_cmucal_vclk_ip_mailbox_ap6,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_DNC, NULL, v620_evt0_cmucal_vclk_ip_mailbox_dnc,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_AP5, NULL, v620_evt0_cmucal_vclk_ip_mailbox_ap5,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P0_SFI, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p0_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P1_SFI, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p1_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P2_SFI, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p2_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P3_SFI, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p3_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_D_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_lite_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_SFI, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_SFI, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P4_SFI, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p4_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CAN_FD0, NULL, v620_evt0_cmucal_vclk_ip_can_fd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_CAN_FD1, NULL, v620_evt0_cmucal_vclk_ip_can_fd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MCT1_SFI, NULL, v620_evt0_cmucal_vclk_ip_mct1_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BAAW_P_SFI, NULL, v620_evt0_cmucal_vclk_ip_baaw_p_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ASYNCAPB_CLUSTER_SFI_DEBUG, NULL,
	v620_evt0_cmucal_vclk_ip_asyncapb_cluster_sfi_debug, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D_SFI, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DMA_SFI, NULL, v620_evt0_cmucal_vclk_ip_dma_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_ABOX, NULL, v620_evt0_cmucal_vclk_ip_mailbox_abox,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_MAILBOX_SFI, NULL, v620_evt0_cmucal_vclk_ip_mailbox_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_DAPAPBAP_MUX_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_dapapbap_mux_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ADM_DAP_G_SFI, NULL, v620_evt0_cmucal_vclk_ip_adm_dap_g_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TMU_SFI, NULL, v620_evt0_cmucal_vclk_ip_tmu_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IP2_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ip2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IP2_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ASYNCAPB_FMU, NULL, v620_evt0_cmucal_vclk_ip_asyncapb_fmu,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_FMU, NULL, v620_evt0_cmucal_vclk_ip_fmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BAAW_D_SFI, NULL, v620_evt0_cmucal_vclk_ip_baaw_d_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P5_SFI, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p5_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D3_SFI, NULL, v620_evt0_cmucal_vclk_ip_xiu_d3_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D1_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_AXI_D0_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_bics_axi_d0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_AXI_D1_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_bics_axi_d1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_AXI_D2_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_bics_axi_d2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_AXI_D3_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_bics_axi_d3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_AXI_D4_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_bics_axi_d4_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFI_CMU_SFI, NULL, v620_evt0_cmucal_vclk_ip_sfi_cmu_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_SFI, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D2_SFI, NULL, v620_evt0_cmucal_vclk_ip_xiu_d2_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_IP3_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_ip3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_IP3_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_ip3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SBISTTBOX_SFI, NULL, v620_evt0_cmucal_vclk_ip_sbisttbox_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XHB_P_SFI, NULL, v620_evt0_cmucal_vclk_ip_xhb_p_sfi,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_SI_IP4_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_si_ip4_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_IP4_SFI, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_ip4_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SNW_CMU_SNW, NULL, v620_evt0_cmucal_vclk_ip_snw_cmu_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_SNW, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_SNW, NULL, v620_evt0_cmucal_vclk_ip_sysreg_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D0_SNW, NULL, v620_evt0_cmucal_vclk_ip_vgen_d0_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D2_SNW, NULL, v620_evt0_cmucal_vclk_ip_vgen_d2_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D0_SNW, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d0_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D1_SNW, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d1_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D0_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D1_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_L0_ISP_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_l0_isp_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_L_ACC_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_acc_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_SNW_SNF0_0, NULL,
	v620_evt0_cmucal_vclk_ip_as_apb_snw_snf0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D0_SNW, NULL, v620_evt0_cmucal_vclk_ip_qe_d0_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D2_SNW, NULL, v620_evt0_cmucal_vclk_ip_qe_d2_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D3_SNW, NULL, v620_evt0_cmucal_vclk_ip_qe_d3_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D2_SNW, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d2_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D2_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU1_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_SNW, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU2_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SNF0, NULL, v620_evt0_cmucal_vclk_ip_snf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_WRP, NULL, v620_evt0_cmucal_vclk_ip_wrp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_SNW, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D3_SNW, NULL, v620_evt0_cmucal_vclk_ip_vgen_d3_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_SNW, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AST_MI_L1_ISP_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_slh_ast_mi_l1_isp_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D3_SNW, NULL, v620_evt0_cmucal_vclk_ip_xiu_d3_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D4_SNW, NULL, v620_evt0_cmucal_vclk_ip_xiu_d4_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D5_SNW, NULL, v620_evt0_cmucal_vclk_ip_xiu_d5_snw,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D0_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D2_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICM_AXI_D3_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_bicm_axi_d3_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_US_128_256_D0_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_us_128_256_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_US_128_256_D1_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_us_128_256_d1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_US_128_256_D3_SNW, NULL,
	v620_evt0_cmucal_vclk_ip_us_128_256_d3_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SSP_CMU_SSP, NULL, v620_evt0_cmucal_vclk_ip_ssp_cmu_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_SSP, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_SSP, NULL, v620_evt0_cmucal_vclk_ip_sysreg_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SECURITYCONTROLLER, NULL,
	v620_evt0_cmucal_vclk_ip_securitycontroller, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BAAW_D_SSP, NULL, v620_evt0_cmucal_vclk_ip_baaw_d_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_SSP, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_ACEL_SI_D_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_acel_si_d_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D_SSP, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D0_SSP, NULL, v620_evt0_cmucal_vclk_ip_qe_d0_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D1_SSP, NULL, v620_evt0_cmucal_vclk_ip_qe_d1_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_ASYNCAPB_SYSMMU, NULL,
	v620_evt0_cmucal_vclk_ip_asyncapb_sysmmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_SSP, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D1_SSP, NULL, v620_evt0_cmucal_vclk_ip_xiu_d1_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_MI_ID_STRONG, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_mi_id_strong, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_RTIC, NULL, v620_evt0_cmucal_vclk_ip_rtic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D2_SSP, NULL, v620_evt0_cmucal_vclk_ip_qe_d2_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_LITE_D_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_vgen_lite_d_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_SSP, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_ssp,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_OTP_ADK_DESERIAL_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_otp_adk_deserial_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_OTP_PUF_DESERIAL_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_otp_puf_deserial_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_OTP_REK_DESERIAL_SSP, NULL,
	v620_evt0_cmucal_vclk_ip_otp_rek_deserial_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_STRONG_CMU_STRONG, NULL,
	v620_evt0_cmucal_vclk_ip_strong_cmu_strong, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AXI_SI_D_TAA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_axi_si_d_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AXI_MI_P_TAA, NULL,
	v620_evt0_cmucal_vclk_ip_slh_axi_mi_p_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSREG_TAA, NULL, v620_evt0_cmucal_vclk_ip_sysreg_taa,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TAA_CMU_TAA, NULL, v620_evt0_cmucal_vclk_ip_taa_cmu_taa,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_MI_L_ACC_TAA, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_mi_l_acc_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_LH_AST_SI_L0_TAA_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_lh_ast_si_l0_taa_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_D_TZPC_TAA, NULL, v620_evt0_cmucal_vclk_ip_d_tzpc_taa,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_AS_APB_TAA0, NULL, v620_evt0_cmucal_vclk_ip_as_apb_taa0,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_PPMU_D_TAA, NULL, v620_evt0_cmucal_vclk_ip_ppmu_d_taa,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_PMMU0_TAA, NULL,
	v620_evt0_cmucal_vclk_ip_sysmmu_s0_pmmu0_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_TAA, NULL, v620_evt0_cmucal_vclk_ip_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_QE_D0_TAA, NULL, v620_evt0_cmucal_vclk_ip_qe_d0_taa,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_VGEN_D_TAA, NULL, v620_evt0_cmucal_vclk_ip_vgen_d_taa,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SYSMMU_S0_TAA, NULL, v620_evt0_cmucal_vclk_ip_sysmmu_s0_taa,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_BICS_APB_P_TAA, NULL,
	v620_evt0_cmucal_vclk_ip_bics_apb_p_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SFMPU_P_TAA, NULL, v620_evt0_cmucal_vclk_ip_sfmpu_p_taa,
	NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_SLH_AST_SI_L1_TAA_ISP, NULL,
	v620_evt0_cmucal_vclk_ip_slh_ast_si_l1_taa_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V620_EVT0_IP_XIU_D0_TAA, NULL, v620_evt0_cmucal_vclk_ip_xiu_d0_taa,
	NULL, NULL),
};

