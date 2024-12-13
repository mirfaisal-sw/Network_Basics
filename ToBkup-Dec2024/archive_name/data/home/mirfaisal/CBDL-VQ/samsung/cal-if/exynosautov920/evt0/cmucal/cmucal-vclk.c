// SPDX-License-Identifier: GPL-2.0
/*
 * Common Clock Framework support for exynosautov920 SoC.
 *
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include "../../../cmucal.h"
#include "cmucal-node.h"
#include "cmucal-vclk.h"
#include "cmucal-vclklut.h"

/* DVFS VCLK -> Clock Node List */
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_vdd_mif[] = {
	V920_EVT0_PLL_MIF_MAIN,
	V920_EVT0_PLL_MIF_S2D,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_vdd_cpucl0[] = {
	V920_EVT0_DIV_CLK_CLUSTER0_ACLK,
	V920_EVT0_DIV_CLK_CLUSTER0_ATCLK,
	V920_EVT0_DIV_CLK_CPUCL0_NOCP,
	V920_EVT0_PLL_CPUCL0,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_vdd_cpucl1[] = {
	V920_EVT0_DIV_CLK_CLUSTER1_ACLK,
	V920_EVT0_DIV_CLK_CLUSTER1_ATCLK,
	V920_EVT0_DIV_CLK_CPUCL1_NOCP,
	V920_EVT0_PLL_CPUCL1,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_vdd_sfi[] = {
	V920_EVT0_CLK_SFI_PLLCLKOUT,
	V920_EVT0_DIV_CLK_SFI_USI18,
	V920_EVT0_DIV_CLK_SFI_USI21,
	V920_EVT0_DIV_CLK_SFI_XSPI,
	V920_EVT0_DIV_CLK_SFI_USI19,
	V920_EVT0_DIV_CLK_SFI_USI20,
	V920_EVT0_DIV_CLK_SFI_CPU_ATCLK,
	V920_EVT0_DIV_CLK_SFI_NOCD,
	V920_EVT0_DIV_CLK_SFI_CAN0,
	V920_EVT0_DIV_CLK_SFI_CAN1,
	V920_EVT0_PLL_SFI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_vdd_aud[] = {
	V920_EVT0_DIV_CLK_AUD_AUDIF,
	V920_EVT0_PLL_AUD,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_vdd_int_cmu[] = {
	V920_EVT0_CLKCMU_MFC_MFC,
	V920_EVT0_MUX_CLKCMU_MFC_MFC,
	V920_EVT0_MUX_CLKCMU_ACC_NOC,
	V920_EVT0_MUX_CLKCMU_ISP_NOC,
	V920_EVT0_CLKCMU_CPUCL0_DBG,
	V920_EVT0_CLKCMU_APM_NOC,
	V920_EVT0_MUX_CLKCMU_APM_NOC,
	V920_EVT0_MUX_CLKCMU_HSI1_NOC,
	V920_EVT0_MUX_CLKCMU_DPUF1_NOC,
	V920_EVT0_CLKCMU_NOCL1_NOC,
	V920_EVT0_MUX_CLKCMU_NOCL1_NOC,
	V920_EVT0_CLKCMU_HSI2_NOC_UFS,
	V920_EVT0_MUX_CLKCMU_HSI2_NOC_UFS,
	V920_EVT0_CLKCMU_DSP_NOC,
	V920_EVT0_MUX_CLKCMU_DSP_NOC,
	V920_EVT0_MUX_CLKCMU_SDMA_NOC,
	V920_EVT0_CLKCMU_DPUF2_NOC,
	V920_EVT0_MUX_CLKCMU_DPUF2_NOC,
	V920_EVT0_CLKCMU_MFD_NOC,
	V920_EVT0_MUX_CLKCMU_MFD_NOC,
	V920_EVT0_CLKCMU_MISC_NOC,
	V920_EVT0_CLKCMU_M2M_NOC,
	V920_EVT0_MUX_CLKCMU_M2M_NOC,
	V920_EVT0_PLL_MMC,
	V920_EVT0_CLKCMU_DPTX_DPOSC,
	V920_EVT0_MUX_CLKCMU_DPTX_DPOSC,
	V920_EVT0_CLKCMU_NOCL2_NOC,
	V920_EVT0_MUX_CLKCMU_NOCL2_NOC,
	V920_EVT0_MUX_CLKCMU_DPUF0_NOC,
	V920_EVT0_CLKCMU_NOCL0_NOC,
	V920_EVT0_MUX_CLKCMU_NOCL0_NOC,
	V920_EVT0_MUX_CLKCMU_TAA_NOC,
	V920_EVT0_MUX_CLKCMU_HSI0_NOC,
	V920_EVT0_MUX_CLKCMU_MIF_NOCP,
	V920_EVT0_CLKCMU_GNPU_NOC,
	V920_EVT0_MUX_CLKCMU_GNPU_NOC,
	V920_EVT0_CLKCMU_DPUB_NOC,
	V920_EVT0_MUX_CLKCMU_DPUB_NOC,
	V920_EVT0_CLKCMU_G3D_SWITCH,
	V920_EVT0_MUX_CLKCMU_G3D_SWITCH,
	V920_EVT0_CLKCMU_DNC_NOC,
	V920_EVT0_MUX_CLKCMU_DNC_NOC,
	V920_EVT0_MUX_CLKCMU_SNW_NOC,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_vdd_cpucl2[] = {
	V920_EVT0_DIV_CLK_CLUSTER2_ACLK,
	V920_EVT0_DIV_CLK_CLUSTER2_ATCLK,
	V920_EVT0_DIV_CLK_CPUCL2_NOCP,
	V920_EVT0_PLL_CPUCL2,
};
/* SPECIAL VCLK -> Clock Node List */
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_apm_timer[] = {
	V920_EVT0_MUX_CLK_APM_TIMER,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_aud_uaif3[] = {
	V920_EVT0_MUX_CLK_AUD_UAIF3,
	V920_EVT0_DIV_CLK_AUD_UAIF3,
	V920_EVT0_MUX_CLK_AUD_UAIF2,
	V920_EVT0_DIV_CLK_AUD_UAIF2,
	V920_EVT0_MUX_CLK_AUD_UAIF1,
	V920_EVT0_DIV_CLK_AUD_UAIF1,
	V920_EVT0_MUX_CLK_AUD_UAIF0,
	V920_EVT0_DIV_CLK_AUD_UAIF0,
	V920_EVT0_MUX_HCHGEN_CLK_AUD_CPU,
	V920_EVT0_DIV_CLK_AUD_CPU,
	V920_EVT0_DIV_HCHGEN_CLK_AUD_CPU,
	V920_EVT0_MUX_CLK_AUD_UAIF4,
	V920_EVT0_DIV_CLK_AUD_UAIF4,
	V920_EVT0_MUX_CLK_AUD_UAIF5,
	V920_EVT0_DIV_CLK_AUD_UAIF5,
	V920_EVT0_MUX_CLK_AUD_UAIF6,
	V920_EVT0_DIV_CLK_AUD_UAIF6,
	V920_EVT0_MUX_CLK_AUD_UAIF7,
	V920_EVT0_DIV_CLK_AUD_UAIF7,
	V920_EVT0_MUX_CLK_AUD_UAIF8,
	V920_EVT0_DIV_CLK_AUD_UAIF8,
	V920_EVT0_MUX_CLK_AUD_UAIF9,
	V920_EVT0_DIV_CLK_AUD_UAIF9,
	V920_EVT0_MUX_CLK_AUD_UAIFS,
	V920_EVT0_DIV_CLK_AUD_UAIFS,
	V920_EVT0_DIV_CLK_AUD_CNT,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_cmu_cmuref[] = {
	V920_EVT0_MUX_CMU_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_cpucl0_cmuref[] = {
	V920_EVT0_MUX_CPUCL0_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_cpucl1_cmuref[] = {
	V920_EVT0_MUX_CPUCL1_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_cpucl2_cmuref[] = {
	V920_EVT0_MUX_CPUCL2_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_mif_cmuref[] = {
	V920_EVT0_MUX_MIF_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_mif1_cmuref[] = {
	V920_EVT0_MUX_MIF1_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_mif2_cmuref[] = {
	V920_EVT0_MUX_MIF2_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_mif3_cmuref[] = {
	V920_EVT0_MUX_MIF3_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_nocl0_cmuref[] = {
	V920_EVT0_MUX_NOCL0_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_nocl1_cmuref[] = {
	V920_EVT0_MUX_NOCL1_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_nocl2_cmuref[] = {
	V920_EVT0_MUX_NOCL2_CMUREF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_dpub_dsim[] = {
	V920_EVT0_CLKCMU_DPUB_DSIM,
	V920_EVT0_MUX_CLKCMU_DPUB_DSIM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_apm_spmi[] = {
	V920_EVT0_DIV_CLK_APM_SPMI,
	V920_EVT0_MUX_CLK_APM_SPMI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_aud_mclk[] = {
	V920_EVT0_DIV_CLK_AUD_MCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clk_aud_avb_eth[] = {
	V920_EVT0_CLK_AUD_AVB_ETH,
	V920_EVT0_MUX_CLK_AUD_AVB_ETH,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clk_cmu_pllclkout[] = {
	V920_EVT0_CLK_CMU_PLLCLKOUT,
	V920_EVT0_MUX_CLK_CMU_PLLCLKOUT,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clk_add_ch_clk[] = {
	V920_EVT0_CLK_ADD_CH_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_cmu_nocp[] = {
	V920_EVT0_DIV_CLK_CMU_NOCP,
	V920_EVT0_MUX_CLK_CMU_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cis_mclk3[] = {
	V920_EVT0_CLKCMU_CIS_MCLK3,
	V920_EVT0_MUX_CLKCMU_CIS_MCLK3,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cis_mclk0[] = {
	V920_EVT0_CLKCMU_CIS_MCLK0,
	V920_EVT0_MUX_CLKCMU_CIS_MCLK0,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cis_mclk1[] = {
	V920_EVT0_CLKCMU_CIS_MCLK1,
	V920_EVT0_MUX_CLKCMU_CIS_MCLK1,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cis_mclk2[] = {
	V920_EVT0_CLKCMU_CIS_MCLK2,
	V920_EVT0_MUX_CLKCMU_CIS_MCLK2,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_cpucl0_shortstop[] = {
	V920_EVT0_DIV_CLK_CPUCL0_SHORTSTOP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cpucl0_cluster[] = {
	V920_EVT0_CLKCMU_CPUCL0_CLUSTER,
	V920_EVT0_MUX_CLKCMU_CPUCL0_CLUSTER,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cpucl0_switch[] = {
	V920_EVT0_CLKCMU_CPUCL0_SWITCH,
	V920_EVT0_MUX_CLKCMU_CPUCL0_SWITCH,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_cpucl1_shortstop[] = {
	V920_EVT0_DIV_CLK_CPUCL1_SHORTSTOP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cpucl1_cluster[] = {
	V920_EVT0_CLKCMU_CPUCL1_CLUSTER,
	V920_EVT0_MUX_CLKCMU_CPUCL1_CLUSTER,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cpucl1_switch[] = {
	V920_EVT0_CLKCMU_CPUCL1_SWITCH,
	V920_EVT0_MUX_CLKCMU_CPUCL1_SWITCH,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_cpucl2_shortstop[] = {
	V920_EVT0_DIV_CLK_CPUCL2_SHORTSTOP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cpucl2_cluster[] = {
	V920_EVT0_CLKCMU_CPUCL2_CLUSTER,
	V920_EVT0_MUX_CLKCMU_CPUCL2_CLUSTER,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_cpucl2_switch[] = {
	V920_EVT0_CLKCMU_CPUCL2_SWITCH,
	V920_EVT0_MUX_CLKCMU_CPUCL2_SWITCH,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_dpuf1_nocp[] = {
	V920_EVT0_DIV_CLK_DPUF1_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_dpuf1_noc[] = {
	V920_EVT0_CLKCMU_DPUF1_NOC,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_dpuf2_nocp[] = {
	V920_EVT0_DIV_CLK_DPUF2_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_gnpu1_nocp[] = {
	V920_EVT0_DIV_CLK_GNPU1_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi00_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI00_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI00_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_peric0_ip[] = {
	V920_EVT0_CLKCMU_PERIC0_IP,
	V920_EVT0_MUX_CLKCMU_PERIC0_IP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi01_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI01_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI01_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi02_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI02_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI02_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi03_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI03_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI03_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi04_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI04_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI04_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi05_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI05_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI05_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi06_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI06_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI06_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi07_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI07_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI07_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric0_usi08_usi[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI08_USI,
	V920_EVT0_MUX_CLK_PERIC0_USI08_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi09_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI09_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI09_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_clkcmu_peric1_ip[] = {
	V920_EVT0_CLKCMU_PERIC1_IP,
	V920_EVT0_MUX_CLKCMU_PERIC1_IP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi10_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI10_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI10_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi11_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI11_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI11_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi12_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI12_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI12_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi13_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI13_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI13_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi14_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI14_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI14_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi15_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI15_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI15_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi16_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI16_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI16_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_peric1_usi17_usi[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI17_USI,
	V920_EVT0_MUX_CLK_PERIC1_USI17_USI,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_sfi_pllclkout[] = {
	V920_EVT0_MUX_CLK_SFI_PLLCLKOUT,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_sfi_usi20[] = {
	V920_EVT0_MUX_CLK_SFI_USI20,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_sfi_usi19[] = {
	V920_EVT0_MUX_CLK_SFI_USI19,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_div_clk_sfi_cpu_cntclk[] = {
	V920_EVT0_DIV_CLK_SFI_CPU_CNTCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_sfi_usi21[] = {
	V920_EVT0_MUX_CLK_SFI_USI21,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_sfi_can0[] = {
	V920_EVT0_MUX_CLK_SFI_CAN0,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_sfi_can1[] = {
	V920_EVT0_MUX_CLK_SFI_CAN1,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_cpucl0_core[] = {
	V920_EVT0_MUX_CLK_CPUCL0_CORE,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_cpucl1_core[] = {
	V920_EVT0_MUX_CLK_CPUCL1_CORE,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_mux_clk_cpucl2_core[] = {
	V920_EVT0_MUX_CLK_CPUCL2_CORE,
};
/* COMMON VCLK -> Clock Node List */
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_cmu[] = {
	V920_EVT0_MUX_CLKCMU_MIF_SWITCH,
	V920_EVT0_CLKCMU_SDMA_NOC,
	V920_EVT0_CLKCMU_DPUF0_NOC,
	V920_EVT0_CLKCMU_ACC_NOC,
	V920_EVT0_CLKCMU_ISP_NOC,
	V920_EVT0_CLKCMU_SNW_NOC,
	V920_EVT0_CLKCMU_TAA_NOC,
	V920_EVT0_CLKCMU_MIF_NOCP,
	V920_EVT0_DIV_CLKCMU_CMU_BOOST,
	V920_EVT0_MUX_CLKCMU_CMU_BOOST,
	V920_EVT0_CLKCMU_HSI1_NOC,
	V920_EVT0_CLKCMU_HSI0_NOC,
	V920_EVT0_CLKCMU_DPTX_NOC,
	V920_EVT0_MUX_CLKCMU_DPTX_NOC,
	V920_EVT0_CLKCMU_MFC_WFD,
	V920_EVT0_MUX_CLKCMU_MFC_WFD,
	V920_EVT0_MUX_CLKCMU_MISC_NOC,
	V920_EVT0_CLKCMU_SSP_NOC,
	V920_EVT0_MUX_CLKCMU_SSP_NOC,
	V920_EVT0_CLKCMU_M2M_JPEG,
	V920_EVT0_MUX_CLKCMU_M2M_JPEG,
	V920_EVT0_CLKCMU_HSI2_NOC,
	V920_EVT0_MUX_CLKCMU_HSI2_NOC,
	V920_EVT0_CLKCMU_G3D_NOCP,
	V920_EVT0_MUX_CLKCMU_G3D_NOCP,
	V920_EVT0_PLL_SHARED1,
	V920_EVT0_MUX_CLKCMU_CPUCL0_DBG,
	V920_EVT0_CLKCMU_HSI1_MMC_CARD,
	V920_EVT0_MUX_CLKCMU_HSI1_MMC_CARD,
	V920_EVT0_CLKCMU_DPTX_DPGTC,
	V920_EVT0_MUX_CLKCMU_DPTX_DPGTC,
	V920_EVT0_CLKCMU_HSI1_USBDRD,
	V920_EVT0_MUX_CLKCMU_HSI1_USBDRD,
	V920_EVT0_CLKCMU_HSI2_UFS_EMBD,
	V920_EVT0_MUX_CLKCMU_HSI2_UFS_EMBD,
	V920_EVT0_PLL_SHARED4,
	V920_EVT0_PLL_SHARED3,
	V920_EVT0_CLKCMU_PERIC0_NOC,
	V920_EVT0_MUX_CLKCMU_PERIC0_NOC,
	V920_EVT0_CLKCMU_PERIC1_NOC,
	V920_EVT0_MUX_CLKCMU_PERIC1_NOC,
	V920_EVT0_PLL_SHARED2,
	V920_EVT0_PLL_SHARED0,
	V920_EVT0_PLL_SHARED5,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_g3d[] = {
	V920_EVT0_MUX_CLK_G3D_NOC,
	V920_EVT0_PLL_G3D,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_hsi2[] = {
	V920_EVT0_DIV_CLK_HSI2_ETHERNET,
	V920_EVT0_DIV_CLK_HSI2_ETHERNET_PTP,
	V920_EVT0_PLL_ETH,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_mif1[] = {
	V920_EVT0_PLL_MIF1,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_mif2[] = {
	V920_EVT0_PLL_MIF2,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_mif3[] = {
	V920_EVT0_PLL_MIF3,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_apm[] = {
	V920_EVT0_MUX_CLK_APM_NOC,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_aud[] = {
	V920_EVT0_MUX_CLK_AUD_AUDIF,
	V920_EVT0_MUX_CLK_AUD_AVB,
	V920_EVT0_MUX_PLL_AVB_FIN,
	V920_EVT0_DIV_CLK_AUD_NOCP,
	V920_EVT0_DIV_CLK_AUD_NOC,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_cpucl0[] = {
	V920_EVT0_DIV_CLK_CLUSTER0_PERIPHCLK,
	V920_EVT0_DIV_CLK_CLUSTER0_MPCLK,
	V920_EVT0_DIV_CLK_CLUSTER0_PCLK,
	V920_EVT0_MUX_CLK_CPUCL0_CLUSTER,
	V920_EVT0_DIV_CLK_CPUCL0_DBG_PCLKDBG,
	V920_EVT0_DIV_CLK_CPUCL0_DBG_NOC,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_cpucl1[] = {
	V920_EVT0_DIV_CLK_CLUSTER1_PERIPHCLK,
	V920_EVT0_DIV_CLK_CLUSTER1_MPCLK,
	V920_EVT0_DIV_CLK_CLUSTER1_PCLK,
	V920_EVT0_MUX_CLK_CPUCL1_CLUSTER,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_cpucl2[] = {
	V920_EVT0_DIV_CLK_CLUSTER2_PCLK,
	V920_EVT0_DIV_CLK_CLUSTER2_PERIPHCLK,
	V920_EVT0_DIV_CLK_CLUSTER2_MPCLK,
	V920_EVT0_MUX_CLK_CPUCL2_CLUSTER,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_hsi1[] = {
	V920_EVT0_MUX_CLK_HSI1_USBDRD,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_peric0[] = {
	V920_EVT0_DIV_CLK_PERIC0_USI_I2C,
	V920_EVT0_MUX_CLK_PERIC0_USI_I2C,
	V920_EVT0_DIV_CLK_PERIC0_I3C,
	V920_EVT0_MUX_CLK_PERIC0_I3C,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_peric1[] = {
	V920_EVT0_DIV_CLK_PERIC1_USI_I2C,
	V920_EVT0_MUX_CLK_PERIC1_USI_I2C,
	V920_EVT0_DIV_CLK_PERIC1_I3C,
	V920_EVT0_MUX_CLK_PERIC1_I3C,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_s2d[] = {
	V920_EVT0_MUX_CLK_S2D_CORE,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_sfi[] = {
	V920_EVT0_MUX_CLK_SFI_USI18,
	V920_EVT0_MUX_CLK_SFI_XSPI,
	V920_EVT0_DIV_CLK_SFI_CPU_ACLK,
	V920_EVT0_DIV_CLK_SFI_CPU_PCLKDBG,
	V920_EVT0_DIV_CLK_SFI_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_acc[] = {
	V920_EVT0_DIV_CLK_ACC_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_dnc[] = {
	V920_EVT0_DIV_CLK_DNC_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_dpub[] = {
	V920_EVT0_DIV_CLK_DPUB_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_dpuf[] = {
	V920_EVT0_DIV_CLK_DPUF_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_dsp[] = {
	V920_EVT0_DIV_CLK_DSP_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_gnpu[] = {
	V920_EVT0_DIV_CLK_GNPU_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_hsi0[] = {
	V920_EVT0_DIV_CLK_HSI0_PCIE_APB,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_isp[] = {
	V920_EVT0_DIV_CLK_ISP_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_m2m[] = {
	V920_EVT0_DIV_CLK_M2M_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_mfc[] = {
	V920_EVT0_DIV_CLK_MFC_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_mfd[] = {
	V920_EVT0_DIV_CLK_MFD_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_misc[] = {
	V920_EVT0_DIV_CLK_MISC_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_nocl0[] = {
	V920_EVT0_DIV_CLK_NOCL0_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_nocl1[] = {
	V920_EVT0_DIV_CLK_NOCL1_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_nocl2[] = {
	V920_EVT0_DIV_CLK_NOCL2_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_sdma[] = {
	V920_EVT0_DIV_CLK_SDMA_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_snw[] = {
	V920_EVT0_DIV_CLK_SNW_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_ssp[] = {
	V920_EVT0_DIV_CLK_SSP_NOCP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_blk_taa[] = {
	V920_EVT0_DIV_CLK_TAA_NOCP,
};
/* GATE VCLK -> Clock Node List */
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_acc[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_SLH_AXI_MI_P_ACC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_acc[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_LH_AXI_SI_D0_ACC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_acc[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_SYSREG_ACC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_acc_snw[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_LH_AST_SI_L_ACC_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_acc[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_D_TZPC_ACC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_acc_taa[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_LH_AST_SI_L_ACC_TAA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_acc[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_PPMU_D0_ACC_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_ACC_UID_PPMU_D0_ACC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d_orbmch[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_QE_D_ORBMCH_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_ACC_UID_QE_D_ORBMCH_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf0_isppre[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_QE_VOTF0_ISPPRE_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_ACC_UID_QE_VOTF0_ISPPRE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_acc[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_SYSMMU_S0_PMMU0_ACC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apb_async_isppre0[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_APB_ASYNC_ISPPRE0_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_orbmch[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_ORBMCH_IPCLKPORT_CLK,
	V920_EVT0_CLK_BLK_ACC_UID_ORBMCH_IPCLKPORT_C2CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_d_isppre[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_VGEN_D_ISPPRE_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_acc_cmu_acc[] = {
	V920_EVT0_CLK_BLK_ACC_UID_ACC_CMU_ACC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_acc[] = {
	V920_EVT0_GOUT_BLK_ACC_UID_LH_AXI_SI_D1_ACC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_votf_isppre[] = {
	V920_EVT0_CLK_BLK_ACC_UID_VGEN_LITE_VOTF_ISPPRE_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_d_orbmch[] = {
	V920_EVT0_CLK_BLK_ACC_UID_VGEN_LITE_D_ORBMCH_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_acc[] = {
	V920_EVT0_CLK_BLK_ACC_UID_PPMU_D1_ACC_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_PPMU_D1_ACC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_csis_link_mux3x6[] = {
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS0_I_PCLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS1_I_PCLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS3_I_PCLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS4_I_PCLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS2_I_PCLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS5_I_PCLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS0_I_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS1_I_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS2_I_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS3_I_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS4_I_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS5_I_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_acc[] = {
	V920_EVT0_CLK_BLK_ACC_UID_SYSMMU_S0_PMMU1_ACC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d_isppre[] = {
	V920_EVT0_CLK_BLK_ACC_UID_QE_D_ISPPRE_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_QE_D_ISPPRE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_isppre[] = {
	V920_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_CLK,
	V920_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_C2CLK,
	V920_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_CLK_CSIS,
	V920_EVT0_CLK_BLK_ACC_UID_ISPPRE_IPCLKPORT_CLK_CSIS,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf1_isppre[] = {
	V920_EVT0_CLK_BLK_ACC_UID_QE_VOTF1_ISPPRE_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_QE_VOTF1_ISPPRE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf2_isppre[] = {
	V920_EVT0_CLK_BLK_ACC_UID_QE_VOTF2_ISPPRE_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_QE_VOTF2_ISPPRE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf3_isppre[] = {
	V920_EVT0_CLK_BLK_ACC_UID_QE_VOTF3_ISPPRE_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ACC_UID_QE_VOTF3_ISPPRE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_acc[] = {
	V920_EVT0_CLK_BLK_ACC_UID_XIU_D0_ACC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_acc[] = {
	V920_EVT0_CLK_BLK_ACC_UID_XIU_D1_ACC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_acc[] = {
	V920_EVT0_CLK_BLK_ACC_UID_SYSMMU_S0_ACC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sensor_vsync[] = {
	V920_EVT0_CLK_BLK_ACC_UID_SENSOR_VSYNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d_alive[] = {
	V920_EVT0_GOUT_BLK_APM_UID_LH_AXI_SI_D_ALIVE_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_alive[] = {
	V920_EVT0_GOUT_BLK_APM_UID_SLH_AXI_MI_P_ALIVE_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt_apm[] = {
	V920_EVT0_GOUT_BLK_APM_UID_WDT_APM_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_apm[] = {
	V920_EVT0_GOUT_BLK_APM_UID_SYSREG_APM_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_apm_ap[] = {
	V920_EVT0_GOUT_BLK_APM_UID_MAILBOX_APM_AP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbif_pmu_alive[] = {
	V920_EVT0_GOUT_BLK_APM_UID_APBIF_PMU_ALIVE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_intmem_alive[] = {
	V920_EVT0_GOUT_BLK_APM_UID_INTMEM_ALIVE_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_APM_UID_INTMEM_ALIVE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_lg_scan2dram[] = {
	V920_EVT0_GOUT_BLK_APM_UID_SLH_AXI_SI_LG_SCAN2DRAM_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pmu_intr_gen[] = {
	V920_EVT0_GOUT_BLK_APM_UID_PMU_INTR_GEN_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_dp_alive[] = {
	V920_EVT0_GOUT_BLK_APM_UID_XIU_DP_ALIVE_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apm_cmu_apm[] = {
	V920_EVT0_CLK_BLK_APM_UID_APM_CMU_APM_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_grebe_apm[] = {
	V920_EVT0_GOUT_BLK_APM_UID_GREBE_APM_IPCLKPORT_HCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbif_gpio_alive[] = {
	V920_EVT0_GOUT_BLK_APM_UID_APBIF_GPIO_ALIVE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbif_top_rtc[] = {
	V920_EVT0_GOUT_BLK_APM_UID_APBIF_TOP_RTC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_grebe_dbgcore[] = {
	V920_EVT0_GOUT_BLK_APM_UID_GREBE_DBGCORE_IPCLKPORT_HCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_apm[] = {
	V920_EVT0_GOUT_BLK_APM_UID_D_TZPC_APM_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap_dbgcore[] = {
	V920_EVT0_GOUT_BLK_APM_UID_MAILBOX_AP_DBGCORE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_g_dbgcore[] = {
	V920_EVT0_GOUT_BLK_APM_UID_LH_AXI_SI_G_DBGCORE_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbif_rtc[] = {
	V920_EVT0_GOUT_BLK_APM_UID_APBIF_RTC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_rom_crc32_host[] = {
	V920_EVT0_GOUT_BLK_APM_UID_ROM_CRC32_HOST_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_APM_UID_ROM_CRC32_HOST_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_apm_sfi0[] = {
	V920_EVT0_GOUT_BLK_APM_UID_MAILBOX_APM_SFI0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_apm_sfi1[] = {
	V920_EVT0_GOUT_BLK_APM_UID_MAILBOX_APM_SFI1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt_dbgcore[] = {
	V920_EVT0_CLK_BLK_APM_UID_WDT_DBGCORE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_spmi_master_pmic[] = {
	V920_EVT0_CLK_BLK_APM_UID_SPMI_MASTER_PMIC_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_APM_UID_SPMI_MASTER_PMIC_IPCLKPORT_I_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_rom_crc32_hcu[] = {
	V920_EVT0_CLK_BLK_APM_UID_ROM_CRC32_HCU_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_hw_scandump_clkstop_ctrl[] = {
	V920_EVT0_CLK_BLK_APM_UID_HW_SCANDUMP_CLKSTOP_CTRL_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apm_dta[] = {
	V920_EVT0_CLK_BLK_APM_UID_APM_DTA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_alive[] = {
	V920_EVT0_CLK_BLK_APM_UID_SYSMMU_S0_PMMU0_ALIVE_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_apm[] = {
	V920_EVT0_CLK_BLK_APM_UID_VGEN_LITE_APM_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_alive[] = {
	V920_EVT0_CLK_BLK_APM_UID_SYSMMU_S0_ALIVE_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_alive[] = {
	V920_EVT0_CLK_BLK_APM_UID_SFMPU_ALIVE_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_spc_apm[] = {
	V920_EVT0_CLK_BLK_APM_UID_SPC_APM_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbif_intcomb_vgpio2pmu[] = {
	V920_EVT0_CLK_BLK_APM_UID_APBIF_INTCOMB_VGPIO2PMU_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbif_intcomb_vgpio2apm[] = {
	V920_EVT0_CLK_BLK_APM_UID_APBIF_INTCOMB_VGPIO2APM_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbif_intcomb_vgpio2ap[] = {
	V920_EVT0_CLK_BLK_APM_UID_APBIF_INTCOMB_VGPIO2AP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d_alive[] = {
	V920_EVT0_CLK_BLK_APM_UID_XIU_D_ALIVE_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_aud_cmu_aud[] = {
	V920_EVT0_CLK_BLK_AUD_UID_AUD_CMU_AUD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d_aud[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_LH_AXI_SI_D_AUD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d_aud[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_PPMU_D_AUD_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_AUD_UID_PPMU_D_AUD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_aud[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_SYSREG_AUD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_abox[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF0,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF1,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF3,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF2,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK_IRQ,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK_IRQ,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_CNT,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF4,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF5,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF6,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI5,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI5_B,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI5_B,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_DAP,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP0,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP1,
	V920_EVT0_GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP2,
	V920_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF7,
	V920_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF8,
	V920_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF9,
	V920_EVT0_CLK_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIFS,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_aud0[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_GPIO_AUD0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_axi_us_32to128[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_AXI_US_32TO128_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_aud[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_SLH_AXI_MI_P_AUD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt_aud0[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_WDT_AUD0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dftmux_aud0[] = {
	V920_EVT0_CLK_BLK_AUD_UID_DFTMUX_AUD0_IPCLKPORT_AUD_CODEC_MCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_aud[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_SYSMMU_S0_PMMU0_AUD_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ad_apb_smmu_aud_s1_ns[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_AD_APB_SMMU_AUD_S1_NS_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt_aud1[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_WDT_AUD1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt_aud2[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_WDT_AUD2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_aud[] = {
	V920_EVT0_GOUT_BLK_AUD_UID_D_TZPC_AUD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip_aud[] = {
	V920_EVT0_CLK_BLK_AUD_UID_LH_AXI_MI_IP_AUD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ip_aud[] = {
	V920_EVT0_CLK_BLK_AUD_UID_LH_AXI_SI_IP_AUD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d_aud[] = {
	V920_EVT0_CLK_BLK_AUD_UID_XIU_D_AUD_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_aud[] = {
	V920_EVT0_CLK_BLK_AUD_UID_SYSMMU_S0_AUD_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_aud1[] = {
	V920_EVT0_CLK_BLK_AUD_UID_GPIO_AUD1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_aud0[] = {
	V920_EVT0_CLK_BLK_AUD_UID_VGEN_AUD0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_aud1[] = {
	V920_EVT0_CLK_BLK_AUD_UID_VGEN_AUD1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_aud[] = {
	V920_EVT0_CLK_BLK_AUD_UID_VGEN_LITE_AUD_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon_pll_aud[] = {
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_MON_CLK10,
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AUD_IPCLKPORT_MON_CLK00,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon_pll_avb[] = {
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_AUD_UID_CLKMON_PLL_AVB_IPCLKPORT_MON_CLK10,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_add_ch_clk[] = {
	V920_EVT0_CLK_BLK_CMU_UID_ADD_CH_CLK_IPCLKPORT_ADD_CH_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cmu[] = {
	V920_EVT0_CLK_BLK_CMU_UID_SLH_AXI_MI_P_CMU_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_cmu[] = {
	V920_EVT0_CLK_BLK_CMU_UID_D_TZPC_CMU_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfr_apbif_cmu[] = {
	V920_EVT0_CLK_BLK_CMU_UID_SFR_APBIF_CMU_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon0_pll_shared[] = {
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK10,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK01,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK11,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK02,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK12,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK03,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON0_PLL_SHARED_IPCLKPORT_MON_CLK13,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon1_pll_shared[] = {
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK10,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK01,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK11,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK02,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK12,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK03,
	V920_EVT0_CLK_BLK_CMU_UID_CLKMON1_PLL_SHARED_IPCLKPORT_MON_CLK13,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_SYSREG_CPUCL0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cssys[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_CSSYS_IPCLKPORT_PCLKDBG,
	V920_EVT0_GOUT_BLK_CPUCL0_UID_CSSYS_IPCLKPORT_ATCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_t_bdu[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_T_BDU_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_it0_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_IT0_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt0_cpucl1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT0_CPUCL1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_it1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_IT1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt1_cpucl1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT1_CPUCL1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_it2_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_IT2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_SLH_AXI_MI_P_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_chi_si_d_cluster0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_CHI_SI_D_CLUSTER0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_it0_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_SI_IT0_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_it1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_SI_IT1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_it2_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_SI_IT2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_it3_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_SI_IT3_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cpucl0_cmu_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_CPUCL0_CMU_CPUCL0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cluster0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_PERIPHCLK,
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_SCLK,
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_ATCLK,
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_MPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_D_TZPC_CPUCL0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ig_cssys[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_SI_IG_CSSYS_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ig_cssys[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_MI_IG_CSSYS_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_p_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_XIU_P_CPUCL0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_dp_cssys[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_XIU_DP_CSSYS_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_g_cssys[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_SI_G_CSSYS_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apb_async_p_cssys_0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_APB_ASYNC_P_CSSYS_0_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ig_etr[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_SI_IG_ETR_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ig_etr[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_MI_IG_ETR_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_bps_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_BPS_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt2_cpucl1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT2_CPUCL1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt3_cpucl1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT3_CPUCL1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt0_sfi[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT0_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt1_sfi[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT1_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt2_sfi[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT2_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt3_sfi[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_ATB_MI_LT3_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adm_apb_g_cluster0[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_ADM_APB_G_CLUSTER0_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ig_stm[] = {
	V920_EVT0_GOUT_BLK_CPUCL0_UID_LH_AXI_MI_IG_STM_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_PPMU_CPUCL0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_CPUCL0_UID_PPMU_CPUCL0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt1_cpucl2_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_LH_ATB_MI_LT1_CPUCL2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt0_cpucl2_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_LH_ATB_MI_LT0_CPUCL2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cpucl0_con[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_CPUCL0_CON_IPCLKPORT_I_PERIPHCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_iri_gic_cluster0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_LH_AST_MI_L_IRI_GIC_CLUSTER0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_icc_cluster0_gic[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_LH_AST_SI_L_ICC_CLUSTER0_GIC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ig_stm[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_LH_AXI_SI_IG_STM_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_g_dbgcore[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_LH_AXI_MI_G_DBGCORE_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cluster0_nocl0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_SLH_AXI_SI_P_CLUSTER0_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_it3_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_LH_ATB_MI_IT3_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon_pll_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_CPUCL0_UID_CLKMON_PLL_CPUCL0_IPCLKPORT_MON_CLK10,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_secjtag[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_SECJTAG_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qos_overr_d_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL0_UID_QOS_OVERR_D_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cluster1[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_ATCLK,
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_PERIPHCLK,
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_SCLK,
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cpucl1_cmu_cpucl1[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_CPUCL1_CMU_CPUCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_cpucl1[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_SYSREG_CPUCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt3_cpucl1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_LH_ATB_SI_LT3_CPUCL1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt2_cpucl1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_LH_ATB_SI_LT2_CPUCL1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt1_cpucl1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_LH_ATB_SI_LT1_CPUCL1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt0_cpucl1_cpucl0[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_LH_ATB_SI_LT0_CPUCL1_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_chi_si_d_cluster1[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_LH_CHI_SI_D_CLUSTER1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cpucl1[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_SLH_AXI_MI_P_CPUCL1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_cpucl1[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_D_TZPC_CPUCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adm_apb_g_cluster1[] = {
	V920_EVT0_GOUT_BLK_CPUCL1_UID_ADM_APB_G_CLUSTER1_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_cpucl1[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_PPMU_CPUCL1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_CPUCL1_UID_PPMU_CPUCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cpucl1_con[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_CPUCL1_CON_IPCLKPORT_I_PERIPHCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_iri_gic_cluster1[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_LH_AST_MI_L_IRI_GIC_CLUSTER1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_icc_cluster1_gic[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_LH_AST_SI_L_ICC_CLUSTER1_GIC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cluster1_nocl0[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_SLH_AXI_SI_P_CLUSTER1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon_pll_cpucl1[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLKMON_PLL_CPUCL1_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLKMON_PLL_CPUCL1_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLKMON_PLL_CPUCL1_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLKMON_PLL_CPUCL1_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_CPUCL1_UID_CLKMON_PLL_CPUCL1_IPCLKPORT_MON_CLK10,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qos_overr_d_cpucl1[] = {
	V920_EVT0_CLK_BLK_CPUCL1_UID_QOS_OVERR_D_CPUCL1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cluster2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLUSTER2_IPCLKPORT_SCLK,
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLUSTER2_IPCLKPORT_ATCLK,
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLUSTER2_IPCLKPORT_PERIPHCLK,
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLUSTER2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cpucl2_cmu_cpucl2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_CPUCL2_CMU_CPUCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adm_apb_g_cluster2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_ADM_APB_G_CLUSTER2_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_cpucl2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_PPMU_CPUCL2_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_CPUCL2_UID_PPMU_CPUCL2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cpucl2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_SLH_AXI_MI_P_CPUCL2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cpucl2_con[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_CPUCL2_CON_IPCLKPORT_I_PERIPHCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_iri_gic_cluster2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_LH_AST_MI_L_IRI_GIC_CLUSTER2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_icc_cluster2_gic[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_LH_AST_SI_L_ICC_CLUSTER2_GIC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cluster2_nocl0[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_SLH_AXI_SI_P_CLUSTER2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_chi_si_d_cluster2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_LH_CHI_SI_D_CLUSTER2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt0_cpucl2_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_LH_ATB_SI_LT0_CPUCL2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt1_cpucl2_cpucl0[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_LH_ATB_SI_LT1_CPUCL2_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_cpucl2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_SYSREG_CPUCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_cpucl2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_D_TZPC_CPUCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon_pll_cpucl2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_CPUCL2_UID_CLKMON_PLL_CPUCL2_IPCLKPORT_MON_CLK10,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qos_overr_d_cpucl2[] = {
	V920_EVT0_CLK_BLK_CPUCL2_UID_QOS_OVERR_D_CPUCL2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dnc_cmu_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_DNC_CMU_DNC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_D_TZPC_DNC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_rq_gnpu0_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD_RQ_GNPU0_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld6_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD6_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld3_mmu_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD3_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld7_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD7_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_ctrl_dnc_gnpu0[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_LD_CTRL_DNC_GNPU0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_ctrl_dnc_gnpu1[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_LD_CTRL_DNC_GNPU1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld0_dnc_gnpu0[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_LD0_DNC_GNPU0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld1_dnc_gnpu1[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_LD1_DNC_GNPU1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld1_dnc_gnpu0[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_LD1_DNC_GNPU0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_IP_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld2_mmu_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD2_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_SYSREG_DNC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_trex_d_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_TREX_D_DNC_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_DNC_UID_TREX_D_DNC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_ld_cache_dsp_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_SLH_AXI_MI_LD_CACHE_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld0_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD0_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld1_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD1_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld2_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD2_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld3_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD3_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld4_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD4_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld5_sdma_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD5_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_sfr_dsp_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_MI_LD_SFR_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_sdma[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_PPMU_D0_SDMA_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_DNC_UID_PPMU_D0_SDMA_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_sdma[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_PPMU_D1_SDMA_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_DNC_UID_PPMU_D1_SDMA_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d2_sdma[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_PPMU_D2_SDMA_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_DNC_UID_PPMU_D2_SDMA_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d3_sdma[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_PPMU_D3_SDMA_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_DNC_UID_PPMU_D3_SDMA_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d4_ipdnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_PPMU_D4_IPDNC_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_DNC_UID_PPMU_D4_IPDNC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ip_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_LH_AXI_SI_IP_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_dnc[] = {
	V920_EVT0_GOUT_BLK_DNC_UID_SLH_AXI_MI_P_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_ctrl_dnc_sdma[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_SI_LD_CTRL_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_sfr_dnc_dsp[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_SI_LD_SFR_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_dma_dnc_dsp[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_SI_LD_DMA_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_rq_gnpu1_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_MI_LD_RQ_GNPU1_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_ld_cmdq_gnpu0_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SLH_AXI_MI_LD_CMDQ_GNPU0_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ip_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_IP_DNC_IPCLKPORT_CLK,
	V920_EVT0_CLK_BLK_DNC_UID_IP_DNC_IPCLKPORT_DAPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_sysmmu_s0_dnc_s1_ns[] = {
	V920_EVT0_CLK_BLK_DNC_UID_AS_APB_SYSMMU_S0_DNC_S1_NS_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_shmem_dsp_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_MI_LD_SHMEM_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_shmem_gnpu0_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_MI_LD_SHMEM_GNPU0_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_shmem_gnpu1_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_MI_LD_SHMEM_GNPU1_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_VGEN_LITE_DNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_lp_dnc_dsp[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SLH_AXI_SI_LP_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_lp_dnc_sdma[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SLH_AXI_SI_LP_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_lp_dnc_gnpu1[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SLH_AXI_SI_LP_DNC_GNPU1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adm_dap_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_ADM_DAP_DNC_IPCLKPORT_DAPCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_PMMU0_DNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_PMMU1_DNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu2_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_PMMU2_DNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu3_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_PMMU3_DNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SYSMMU_S1_PMMU0_DNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_ld_cmdq_gnpu1_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SLH_AXI_MI_LD_CMDQ_GNPU1_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld0_dnc_gnpu1[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_SI_LD0_DNC_GNPU1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld1_mmu_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_MI_LD1_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld0_mmu_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AXI_MI_LD0_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_lp_dnc_gnpu0[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SLH_AXI_SI_LP_DNC_GNPU0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d0_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_TAXI_SI_D0_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d1_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_TAXI_SI_D1_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d2_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_TAXI_SI_D2_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d3_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_TAXI_SI_D3_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SYSMMU_S0_DNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_SYSMMU_S1_DNC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_XIU_D_DNC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_baaw_p_dnc[] = {
	V920_EVT0_CLK_BLK_DNC_UID_BAAW_P_DNC_IPCLKPORT_I_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_lg_dnc_nocl0[] = {
	V920_EVT0_CLK_BLK_DNC_UID_LH_AST_SI_LG_DNC_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_dptx[] = {
	V920_EVT0_GOUT_BLK_DPTX_UID_SYSREG_DPTX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_dptx[] = {
	V920_EVT0_GOUT_BLK_DPTX_UID_SLH_AXI_MI_P_DPTX_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_dptx[] = {
	V920_EVT0_GOUT_BLK_DPTX_UID_D_TZPC_DPTX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dptx_cmu_dptx[] = {
	V920_EVT0_CLK_BLK_DPTX_UID_DPTX_CMU_DPTX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dp_link0[] = {
	V920_EVT0_CLK_BLK_DPTX_UID_DP_LINK0_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_DPTX_UID_DP_LINK0_IPCLKPORT_I_DP_GTC_CLK,
	V920_EVT0_CLK_BLK_DPTX_UID_DP_LINK0_IPCLKPORT_I_DP_OSC_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dp_link1[] = {
	V920_EVT0_CLK_BLK_DPTX_UID_DP_LINK1_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_DPTX_UID_DP_LINK1_IPCLKPORT_I_DP_GTC_CLK,
	V920_EVT0_CLK_BLK_DPTX_UID_DP_LINK1_IPCLKPORT_I_DP_OSC_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dwc_usbc31dptxphy_upcs_x4_ns_x1_x4_pipe_wrap_inst0[] = {
	V920_EVT0_CLK_BLK_DPTX_UID_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST0_IPCLKPORT_USBC31_DP_CTRL_PCLK,
	V920_EVT0_CLK_BLK_DPTX_UID_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST0_IPCLKPORT_TCA_APB_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dwc_usbc31dptxphy_upcs_x4_ns_x1_x4_pipe_wrap_inst1[] = {
	V920_EVT0_CLK_BLK_DPTX_UID_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST1_IPCLKPORT_USBC31_DP_CTRL_PCLK,
	V920_EVT0_CLK_BLK_DPTX_UID_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST1_IPCLKPORT_TCA_APB_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dpub_cmu_dpub[] = {
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_CMU_DPUB_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_dpub[] = {
	V920_EVT0_GOUT_BLK_DPUB_UID_SYSREG_DPUB_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_dpub[] = {
	V920_EVT0_GOUT_BLK_DPUB_UID_SLH_AXI_MI_P_DPUB_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_dpub[] = {
	V920_EVT0_GOUT_BLK_DPUB_UID_D_TZPC_DPUB_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ad_apb_decon_main_dpub0[] = {
	V920_EVT0_CLK_BLK_DPUB_UID_AD_APB_DECON_MAIN_DPUB0_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dpub[] = {
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ACLK_DECON_DPUB0,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_OSCCLK_DSIM0_DPUB0,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_OSCCLK_DSIM1_DPUB0,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_OSCCLK_DSIM0_DPUB1,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_OSCCLK_DSIM1_DPUB1,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ALVCLK_DSIM0_DPUB0,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ALVCLK_DSIM1_DPUB0,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ALVCLK_DSIM0_DPUB1,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ALVCLK_DSIM1_DPUB1,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ACLK_DECON_DPUB1,
	V920_EVT0_CLK_BLK_DPUB_UID_DPUB_IPCLKPORT_ACLK_DECON_DPUB2,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_dpub[] = {
	V920_EVT0_CLK_BLK_DPUB_UID_SFMPU_DPUB_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dpuf_cmu_dpuf[] = {
	V920_EVT0_CLK_BLK_DPUF_UID_DPUF_CMU_DPUF_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_SYSREG_DPUF_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu1_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_SYSMMU_S1_PMMU1_DPUF_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_SYSMMU_S1_PMMU0_DPUF_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_SYSMMU_S0_PMMU1_DPUF_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_SYSMMU_S0_PMMU0_DPUF_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_dpuf1[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_PPMU_D0_DPUF1_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_DPUF_UID_PPMU_D0_DPUF1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_dpuf0[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_PPMU_D1_DPUF0_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_DPUF_UID_PPMU_D1_DPUF0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_dpuf1[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_PPMU_D1_DPUF1_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_DPUF_UID_PPMU_D1_DPUF1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_dpuf0[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_PPMU_D0_DPUF0_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_DPUF_UID_PPMU_D0_DPUF0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_SLH_AXI_MI_P_DPUF_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_LH_AXI_SI_D0_DPUF_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_LH_AXI_SI_D1_DPUF_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_D_TZPC_DPUF_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dpuf[] = {
	V920_EVT0_GOUT_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_DPUF0,
	V920_EVT0_GOUT_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_DPUF1,
	V920_EVT0_GOUT_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_VOTF0,
	V920_EVT0_CLK_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_SRAMC,
	V920_EVT0_CLK_BLK_DPUF_UID_DPUF_IPCLKPORT_ACLK_VOTF1,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_dpuf[] = {
	V920_EVT0_CLK_BLK_DPUF_UID_XIU_D0_DPUF_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_dpuf[] = {
	V920_EVT0_CLK_BLK_DPUF_UID_XIU_D1_DPUF_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ad_apb_dpuf0_dma[] = {
	V920_EVT0_CLK_BLK_DPUF_UID_AD_APB_DPUF0_DMA_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_dpuf[] = {
	V920_EVT0_CLK_BLK_DPUF_UID_SYSMMU_S0_DPUF_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_dpuf[] = {
	V920_EVT0_CLK_BLK_DPUF_UID_SYSMMU_S1_DPUF_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_dpuf[] = {
	V920_EVT0_CLK_BLK_DPUF_UID_SFMPU_DPUF_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dpuf1_cmu_dpuf1[] = {
	V920_EVT0_CLK_BLK_DPUF1_UID_DPUF1_CMU_DPUF1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dpuf2_cmu_dpuf2[] = {
	V920_EVT0_CLK_BLK_DPUF2_UID_DPUF2_CMU_DPUF2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dsp_cmu_dsp[] = {
	V920_EVT0_CLK_BLK_DSP_UID_DSP_CMU_DSP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_dsp[] = {
	V920_EVT0_CLK_BLK_DSP_UID_SYSREG_DSP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_sfr_dnc_dsp[] = {
	V920_EVT0_CLK_BLK_DSP_UID_LH_AXI_MI_LD_SFR_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_dma_dnc_dsp[] = {
	V920_EVT0_CLK_BLK_DSP_UID_LH_AXI_MI_LD_DMA_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_ld_sdma_dsp[] = {
	V920_EVT0_CLK_BLK_DSP_UID_LH_AST_MI_LD_SDMA_DSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_lp_dnc_dsp[] = {
	V920_EVT0_CLK_BLK_DSP_UID_SLH_AXI_MI_LP_DNC_DSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_ld_cache_dsp_dnc[] = {
	V920_EVT0_CLK_BLK_DSP_UID_SLH_AXI_SI_LD_CACHE_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_sfr_dsp_dnc[] = {
	V920_EVT0_CLK_BLK_DSP_UID_LH_AXI_SI_LD_SFR_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_shmem_dsp_dnc[] = {
	V920_EVT0_CLK_BLK_DSP_UID_LH_AXI_SI_LD_SHMEM_DSP_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_dsp[] = {
	V920_EVT0_CLK_BLK_DSP_UID_D_TZPC_DSP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ip_dsp[] = {
	V920_EVT0_CLK_BLK_DSP_UID_IP_DSP_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_g3d_cmu_g3d[] = {
	V920_EVT0_CLK_BLK_G3D_UID_G3D_CMU_G3D_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_g3d[] = {
	V920_EVT0_GOUT_BLK_G3D_UID_SYSREG_G3D_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ip_g3d[] = {
	V920_EVT0_GOUT_BLK_G3D_UID_LH_AXI_SI_IP_G3D_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_g3d[] = {
	V920_EVT0_GOUT_BLK_G3D_UID_SLH_AXI_MI_P_G3D_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_g3d[] = {
	V920_EVT0_GOUT_BLK_G3D_UID_D_TZPC_G3D_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_bg3d_pwrctl[] = {
	V920_EVT0_CLK_BLK_G3D_UID_BG3D_PWRCTL_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adm_dap_g_g3d[] = {
	V920_EVT0_CLK_BLK_G3D_UID_ADM_DAP_G_G3D_IPCLKPORT_DAPCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpu[] = {
	V920_EVT0_CLK_BLK_G3D_UID_GPU_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_G3D_UID_GPU_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_asb_g3d[] = {
	V920_EVT0_CLK_BLK_G3D_UID_ASB_G3D_IPCLKPORT_PCLK_PPMU_G3D0,
	V920_EVT0_CLK_BLK_G3D_UID_ASB_G3D_IPCLKPORT_PCLK_PPMU_G3D1,
	V920_EVT0_CLK_BLK_G3D_UID_ASB_G3D_IPCLKPORT_PCLK_PPMU_G3D2,
	V920_EVT0_CLK_BLK_G3D_UID_ASB_G3D_IPCLKPORT_PCLK_PPMU_G3D3,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gnpu_cmu_gnpu[] = {
	V920_EVT0_CLK_BLK_GNPU_UID_GNPU_CMU_GNPU_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_lp_dnc_gnpu[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_SLH_AXI_MI_LP_DNC_GNPU_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_gnpu[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_D_TZPC_GNPU_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld0_dnc_gnpu[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_LH_AXI_MI_LD0_DNC_GNPU_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_ctrl_dnc_gnpu[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_LH_AXI_MI_LD_CTRL_DNC_GNPU_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_ld_cmdq_gnpu_dnc[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_SLH_AXI_SI_LD_CMDQ_GNPU_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_shmem_gnpu_dnc[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_LH_AXI_SI_LD_SHMEM_GNPU_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_gnpu[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_SYSREG_GNPU_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_rq_gnpu_dnc[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_LH_AXI_SI_LD_RQ_GNPU_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ip_npucore[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_IP_NPUCORE_IPCLKPORT_CLK_SRAM,
	V920_EVT0_CLK_BLK_GNPU_UID_IP_NPUCORE_IPCLKPORT_CLK_CORE,
	V920_EVT0_CLK_BLK_GNPU_UID_IP_NPUCORE_IPCLKPORT_CLK_XMAA,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld1_dnc_gnpu[] = {
	V920_EVT0_GOUT_BLK_GNPU_UID_LH_AXI_MI_LD1_DNC_GNPU_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_gnpu[] = {
	V920_EVT0_CLK_BLK_GNPU_UID_XIU_D0_GNPU_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_gnpu[] = {
	V920_EVT0_CLK_BLK_GNPU_UID_XIU_D1_GNPU_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gnpu1_cmu_gnpu1[] = {
	V920_EVT0_CLK_BLK_GNPU1_UID_GNPU1_CMU_GNPU1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_hsi0_cmu_hsi0[] = {
	V920_EVT0_CLK_BLK_HSI0_UID_HSI0_CMU_HSI0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_si_d0_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_LH_ACEL_SI_D0_HSI0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_SLH_AXI_MI_P_HSI0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_GPIO_HSI0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_SYSREG_HSI0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_XIU_D0_HSI0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_PPMU_D0_HSI0_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_HSI0_UID_PPMU_D0_HSI0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_p0_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_XIU_P0_HSI0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pcie_gen5_2l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE1_DBI_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE1_MSTR_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE1_SLV_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE0_DBI_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE0_MSTR_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE0_SLV_ACLK_UG,
	V920_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_APB_PCLK,
	V920_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE0_DOORBELL_IPCLK,
	V920_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_2L_IPCLKPORT_PCIE1_DOORBELL_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pcie_ia_gen5a_2l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_IA_GEN5A_2L_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pcie_ia_gen5b_2l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_IA_GEN5B_2L_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_hsi0_0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_D_TZPC_HSI0_0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_XIU_D1_HSI0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_PPMU_D1_HSI0_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_HSI0_UID_PPMU_D1_HSI0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pcie_ia_gen5a_4l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_IA_GEN5A_4L_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pcie_ia_gen5b_4l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_IA_GEN5B_4L_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_si_d1_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_LH_ACEL_SI_D1_HSI0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pcie_gen5_4l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_DBI_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE1_MSTR_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_SLV_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE1_DBI_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_MSTR_ACLK_UG,
	V920_EVT0_GOUT_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE1_SLV_ACLK_UG,
	V920_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_APB_PCLK,
	V920_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_CXS_CLK,
	V920_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE0_DOORBELL_IPCLK,
	V920_EVT0_CLK_BLK_HSI0_UID_PCIE_GEN5_4L_IPCLKPORT_PCIE1_DOORBELL_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_hsi0_1[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_D_TZPC_HSI0_1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pcie_gen5a_2l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_VGEN_PCIE_GEN5A_2L_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pcie_gen5a_4l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_VGEN_PCIE_GEN5A_4L_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pcie_gen5b_2l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_VGEN_PCIE_GEN5B_2L_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pcie_gen5b_4l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_VGEN_PCIE_GEN5B_4L_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pcie_gen5a_2l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_QE_PCIE_GEN5A_2L_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI0_UID_QE_PCIE_GEN5A_2L_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pcie_gen5a_4l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_QE_PCIE_GEN5A_4L_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI0_UID_QE_PCIE_GEN5A_4L_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pcie_gen5b_2l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_QE_PCIE_GEN5B_2L_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI0_UID_QE_PCIE_GEN5B_2L_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pcie_gen5b_4l[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_QE_PCIE_GEN5B_4L_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI0_UID_QE_PCIE_GEN5B_4L_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_SYSMMU_S0_PMMU0_HSI0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_hsi0[] = {
	V920_EVT0_GOUT_BLK_HSI0_UID_SYSMMU_S0_PMMU1_HSI0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d2_hsi0[] = {
	V920_EVT0_CLK_BLK_HSI0_UID_XIU_D2_HSI0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_hsi0[] = {
	V920_EVT0_CLK_BLK_HSI0_UID_SYSMMU_S0_HSI0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_async_apb_pcie_gen5_4l_sub_ctrl[] = {
	V920_EVT0_CLK_BLK_HSI0_UID_ASYNC_APB_PCIE_GEN5_4L_SUB_CTRL_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_hsi1_cmu_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_HSI1_CMU_HSI1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mmc_card[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_MMC_CARD_IPCLKPORT_SDCLKIN,
	V920_EVT0_GOUT_BLK_HSI1_UID_MMC_CARD_IPCLKPORT_I_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_SYSREG_HSI1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_GPIO_HSI1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_si_d_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_LH_ACEL_SI_D_HSI1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_SLH_AXI_MI_P_HSI1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_XIU_D0_HSI1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_p_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_XIU_P_HSI1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_PPMU_HSI1_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_HSI1_UID_PPMU_HSI1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_VGEN_LITE_HSI1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_D_TZPC_HSI1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usb20drd_0[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_0_IPCLKPORT_BUS_CLK_EARLY,
	V920_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_0_IPCLKPORT_ACLK_PHYCTRL,
	V920_EVT0_CLK_BLK_HSI1_UID_USB20DRD_0_IPCLKPORT_I_USB20DRD_ALIVE_CLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB20DRD_0_IPCLKPORT_I_USB20DRD_REF_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usb20drd_1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_1_IPCLKPORT_BUS_CLK_EARLY,
	V920_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_1_IPCLKPORT_ACLK_PHYCTRL,
	V920_EVT0_CLK_BLK_HSI1_UID_USB20DRD_1_IPCLKPORT_I_USB20DRD_ALIVE_CLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB20DRD_1_IPCLKPORT_I_USB20DRD_REF_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usb30drd_0[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USB31PHY_USB31_CTRL_PCLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBSUBCTL_I_APB_PCLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBLINK_ACLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBSUBCTL_ALIVE_CLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBLINK_SUSPEND_CLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBSUBCTL_U3REWA_ALV_CLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB30DRD_0_IPCLKPORT_I_USBLINK_REF_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usb20drd_2[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_2_IPCLKPORT_BUS_CLK_EARLY,
	V920_EVT0_GOUT_BLK_HSI1_UID_USB20DRD_2_IPCLKPORT_ACLK_PHYCTRL,
	V920_EVT0_CLK_BLK_HSI1_UID_USB20DRD_2_IPCLKPORT_I_USB20DRD_ALIVE_CLK,
	V920_EVT0_CLK_BLK_HSI1_UID_USB20DRD_2_IPCLKPORT_I_USB20DRD_REF_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_hsi1[] = {
	V920_EVT0_GOUT_BLK_HSI1_UID_SYSMMU_S0_PMMU0_HSI1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_hsi1[] = {
	V920_EVT0_CLK_BLK_HSI1_UID_XIU_D1_HSI1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_hsi1[] = {
	V920_EVT0_CLK_BLK_HSI1_UID_SYSMMU_S0_HSI1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ethernet0[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_SYS_CLK,
	V920_EVT0_CLK_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_PCLK_S0,
	V920_EVT0_CLK_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_PCLK_S1,
	V920_EVT0_CLK_BLK_HSI2_UID_ETHERNET0_IPCLKPORT_CLK_PTP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ethernet1[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_SYS_CLK,
	V920_EVT0_CLK_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_PCLK_S0,
	V920_EVT0_CLK_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_PCLK_S1,
	V920_EVT0_CLK_BLK_HSI2_UID_ETHERNET1_IPCLKPORT_CLK_PTP,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_si_d0_hsi2[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_LH_ACEL_SI_D0_HSI2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_ufs_embd0[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_QE_UFS_EMBD0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_QE_UFS_EMBD0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_ufs_embd1[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_QE_UFS_EMBD1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_QE_UFS_EMBD1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_hsi2[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_SYSMMU_S0_PMMU0_HSI2_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ufs_embd0[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD0_IPCLKPORT_I_ACLK,
	V920_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD0_IPCLKPORT_I_CLK_UNIPRO,
	V920_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD0_IPCLKPORT_I_FMP_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ufs_embd1[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD1_IPCLKPORT_I_ACLK,
	V920_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD1_IPCLKPORT_I_CLK_UNIPRO,
	V920_EVT0_GOUT_BLK_HSI2_UID_UFS_EMBD1_IPCLKPORT_I_FMP_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_hsi2[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_XIU_D0_HSI2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_ufs_embd0[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_PPMU_UFS_EMBD0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_PPMU_UFS_EMBD0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_ufs_embd1[] = {
	V920_EVT0_GOUT_BLK_HSI2_UID_PPMU_UFS_EMBD1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_PPMU_UFS_EMBD1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_SYSMMU_S0_HSI2_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d2_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_XIU_D2_HSI2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_ufs_embd0[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_VGEN_UFS_EMBD0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_ufs_embd1[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_VGEN_UFS_EMBD1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d3_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_XIU_D3_HSI2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_si_d1_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_LH_ACEL_SI_D1_HSI2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_ethernet0[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_VGEN_ETHERNET0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_ethernet1[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_VGEN_ETHERNET1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_ethernet0[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_PPMU_ETHERNET0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_PPMU_ETHERNET0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_ethernet1[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_PPMU_ETHERNET1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_PPMU_ETHERNET1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_ethernet0[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_QE_ETHERNET0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_QE_ETHERNET0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_ethernet1[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_QE_ETHERNET1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_HSI2_UID_QE_ETHERNET1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_SYSMMU_S1_HSI2_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_SYSMMU_S1_PMMU0_HSI2_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_XIU_D1_HSI2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_asyncapb_sysmmu_s0_hsi2_ns[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_ASYNCAPB_SYSMMU_S0_HSI2_NS_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_D_TZPC_HSI2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_GPIO_HSI2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_hsi2_cmu_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_HSI2_CMU_HSI2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_SLH_AXI_MI_P_HSI2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_hsi2[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_SYSREG_HSI2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon_pll_eth[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_HSI2_UID_CLKMON_PLL_ETH_IPCLKPORT_MON_CLK10,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_hsi2ufs[] = {
	V920_EVT0_CLK_BLK_HSI2_UID_GPIO_HSI2UFS_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_d_isp[] = {
	V920_EVT0_GOUT_BLK_ISP_UID_VGEN_LITE_D_ISP_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_D_TZPC_ISP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_SYSREG_ISP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_SYSMMU_S0_PMMU0_ISP_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_PPMU_ISP_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ISP_UID_PPMU_ISP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d0_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_QE_D0_ISP_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ISP_UID_QE_D0_ISP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d1_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_QE_D1_ISP_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ISP_UID_QE_D1_ISP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d2_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_QE_D2_ISP_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ISP_UID_QE_D2_ISP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d3_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_QE_D3_ISP_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_ISP_UID_QE_D3_ISP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_ISP_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_LH_AXI_SI_D_ISP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_SLH_AXI_MI_P_ISP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_isp0[] = {
	V920_EVT0_CLK_BLK_ISP_UID_AS_APB_ISP0_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_taa_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_LH_AST_MI_L_TAA_ISP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_isp_snw[] = {
	V920_EVT0_CLK_BLK_ISP_UID_LH_AST_SI_L_ISP_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_SYSMMU_S0_ISP_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_XIU_D1_ISP_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_isp_cmu_isp[] = {
	V920_EVT0_CLK_BLK_ISP_UID_ISP_CMU_ISP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_m2m_cmu_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_M2M_CMU_M2M_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_D_TZPC_M2M_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_SYSREG_M2M_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_PPMU_D0_M2M_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_M2M_UID_PPMU_D0_M2M_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_PPMU_D1_M2M_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_M2M_UID_PPMU_D1_M2M_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_SYSMMU_S0_PMMU0_M2M_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_SYSMMU_S0_PMMU1_M2M_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_SYSMMU_S0_M2M_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_SLH_AXI_MI_P_M2M_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_m2m0[] = {
	V920_EVT0_CLK_BLK_M2M_UID_AS_APB_M2M0_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_XIU_D0_M2M_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_XIU_D1_M2M_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_jpeg[] = {
	V920_EVT0_CLK_BLK_M2M_UID_QE_JPEG_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_M2M_UID_QE_JPEG_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_m2m_d0[] = {
	V920_EVT0_CLK_BLK_M2M_UID_QE_M2M_D0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_M2M_UID_QE_M2M_D0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_d_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_VGEN_LITE_D_M2M_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_m2m_d0[] = {
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK_2X1,
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK_2X1,
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK_VOTF,
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D0_IPCLKPORT_ACLK_VOTF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_m2m_d1[] = {
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D1_IPCLKPORT_ACLK_2X1,
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D1_IPCLKPORT_ACLK_2X1,
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D1_IPCLKPORT_ACLK_VOTF,
	V920_EVT0_CLK_BLK_M2M_UID_M2M_D1_IPCLKPORT_ACLK_VOTF,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_jpeg[] = {
	V920_EVT0_CLK_BLK_M2M_UID_JPEG_IPCLKPORT_I_SMFC_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip_jpeg[] = {
	V920_EVT0_CLK_BLK_M2M_UID_LH_AXI_MI_IP_JPEG_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ip_jpeg[] = {
	V920_EVT0_CLK_BLK_M2M_UID_LH_AXI_SI_IP_JPEG_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_si_d0_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_LH_ACEL_SI_D0_M2M_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_si_d1_m2m[] = {
	V920_EVT0_CLK_BLK_M2M_UID_LH_ACEL_SI_D1_M2M_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_m2m_jpeg[] = {
	V920_EVT0_CLK_BLK_M2M_UID_AS_APB_M2M_JPEG_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mfc_cmu_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_MFC_CMU_MFC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_SYSREG_MFC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_LH_AXI_SI_D1_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_SLH_AXI_MI_P_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_SYSMMU_S0_PMMU0_MFC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_SYSMMU_S0_PMMU1_MFC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_PPMU_D0_MFC_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_MFC_UID_PPMU_D0_MFC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_PPMU_D1_MFC_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_MFC_UID_PPMU_D1_MFC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d2_wfd[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_PPMU_D2_WFD_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MFC_UID_PPMU_D2_WFD_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_XIU_D_MFC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_wfd_ns[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_AS_APB_WFD_NS_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_MFC_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MFC_UID_MFC_IPCLKPORT_C2CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wfd[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_WFD_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_D_TZPC_MFC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_mi_it_mfc[] = {
	V920_EVT0_GOUT_BLK_MFC_UID_LH_ATB_MI_IT_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_AS_APB_MFC_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_VGEN_LITE_MFC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf1_mfd_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AST_MI_OTF1_MFD_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf3_mfd_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AST_MI_OTF3_MFD_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_otf0_mfc_mfd[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AST_SI_OTF0_MFC_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_otf1_mfc_mfd[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AST_SI_OTF1_MFC_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_otf2_mfc_mfd[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AST_SI_OTF2_MFC_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_otf3_mfc_mfd[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AST_SI_OTF3_MFC_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_id_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AXI_MI_ID_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf2_mfd_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AST_MI_OTF2_MFD_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf0_mfd_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AST_MI_OTF0_MFD_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_SYSMMU_S0_MFC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_XIU_D0_MFC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_it_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_ATB_SI_IT_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AXI_SI_D0_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_id_mfc[] = {
	V920_EVT0_CLK_BLK_MFC_UID_LH_AXI_SI_ID_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mfd_cmu_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_MFD_CMU_MFD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_SLH_AXI_MI_P_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_AS_APB_MFD_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_D_TZPC_MFD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AXI_SI_D0_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AXI_SI_D1_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_MFD_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_PPMU_D0_MFD_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MFD_UID_PPMU_D0_MFD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_PPMU_D1_MFD_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MFD_UID_PPMU_D1_MFD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_SYSMMU_S0_PMMU0_MFD_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_SYSMMU_S0_PMMU1_MFD_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_VGEN_LITE_MFD_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_SYSREG_MFD_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_otf0_mfd_mfc[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AST_SI_OTF0_MFD_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_otf2_mfd_mfc[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AST_SI_OTF2_MFD_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_otf1_mfd_mfc[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AST_SI_OTF1_MFD_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_otf3_mfd_mfc[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AST_SI_OTF3_MFD_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf0_mfc_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AST_MI_OTF0_MFC_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf1_mfc_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AST_MI_OTF1_MFC_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf2_mfc_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AST_MI_OTF2_MFC_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf3_mfc_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_LH_AST_MI_OTF3_MFC_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adm_apb_mfcmfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_ADM_APB_MFCMFD_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_SYSMMU_S0_MFD_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d_mfd[] = {
	V920_EVT0_CLK_BLK_MFD_UID_XIU_D_MFD_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mif_cmu_mif[] = {
	V920_EVT0_CLK_BLK_MIF_UID_MIF_CMU_MIF_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ddrphy0[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_DDRPHY0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_mif[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_SYSREG_MIF_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_mif[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_SLH_AXI_MI_P_MIF_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppc_debug0[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_PPC_DEBUG0_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MIF_UID_PPC_DEBUG0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_smc0[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_SMC0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ppc_debug0[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_QCH_ADAPTER_PPC_DEBUG0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_mif[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_D_TZPC_MIF_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_smc1[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_SMC1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ddrphy1[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_DDRPHY1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppc_debug1[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_PPC_DEBUG1_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MIF_UID_PPC_DEBUG1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ppc_debug1[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_QCH_ADAPTER_PPC_DEBUG1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_mif[] = {
	V920_EVT0_GOUT_BLK_MIF_UID_SFMPU_MIF_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_spc_mif[] = {
	V920_EVT0_CLK_BLK_MIF_UID_SPC_MIF_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_spmpu_mif[] = {
	V920_EVT0_CLK_BLK_MIF_UID_SPMPU_MIF_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_smc0[] = {
	V920_EVT0_CLK_BLK_MIF_UID_QCH_ADAPTER_SMC0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_smc1[] = {
	V920_EVT0_CLK_BLK_MIF_UID_QCH_ADAPTER_SMC1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ddrphy0[] = {
	V920_EVT0_CLK_BLK_MIF_UID_QCH_ADAPTER_DDRPHY0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ddrphy1[] = {
	V920_EVT0_CLK_BLK_MIF_UID_QCH_ADAPTER_DDRPHY1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mif1_cmu_mif1[] = {
	V920_EVT0_CLK_BLK_MIF1_UID_MIF1_CMU_MIF1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_ddrphy_1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_APBBR_DDRPHY_1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_dmc_1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_APBBR_DMC_1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_dmctz_1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_APBBR_DMCTZ_1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_axi2apb_mif1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_AXI2APB_MIF1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ddrphy_1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_DDRPHY_1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dmc_1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_DMC_1_IPCLKPORT_PCLK1,
	V920_EVT0_GOUT_BLK_MIF1_UID_DMC_1_IPCLKPORT_PCLK2,
	V920_EVT0_CLK_BLK_MIF1_UID_DMC_1_IPCLKPORT_SOC_CLK,
	V920_EVT0_CLK_BLK_MIF1_UID_DMC_1_IPCLKPORT_SOC_MPACE_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lhm_axi_p_mif1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_LHM_AXI_P_MIF1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmuppc_debug1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_PPMUPPC_DEBUG1_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MIF1_UID_PPMUPPC_DEBUG1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmuppc_dvfs1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_PPMUPPC_DVFS1_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MIF1_UID_PPMUPPC_DVFS1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_mif1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_SYSREG_MIF1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_debug1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_QCH_ADAPTER_PPMUPPC_DEBUG1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_dvfs1[] = {
	V920_EVT0_GOUT_BLK_MIF1_UID_QCH_ADAPTER_PPMUPPC_DVFS1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_ddrphy2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_APBBR_DDRPHY2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_dmc2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_APBBR_DMC2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_dmctz2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_APBBR_DMCTZ2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_axi2apb_mif2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_AXI2APB_MIF2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ddrphy2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_DDRPHY2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dmc2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_DMC2_IPCLKPORT_PCLK1,
	V920_EVT0_GOUT_BLK_MIF2_UID_DMC2_IPCLKPORT_PCLK2,
	V920_EVT0_CLK_BLK_MIF2_UID_DMC2_IPCLKPORT_SOC_CLK,
	V920_EVT0_CLK_BLK_MIF2_UID_DMC2_IPCLKPORT_SOC_MPACE_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lhm_axi_p_mif2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_LHM_AXI_P_MIF2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmuppc_debug2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_PPMUPPC_DEBUG2_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MIF2_UID_PPMUPPC_DEBUG2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmuppc_dvfs2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_PPMUPPC_DVFS2_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MIF2_UID_PPMUPPC_DVFS2_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_mif2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_SYSREG_MIF2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_debug2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_QCH_ADAPTER_PPMUPPC_DEBUG2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_dvfs2[] = {
	V920_EVT0_GOUT_BLK_MIF2_UID_QCH_ADAPTER_PPMUPPC_DVFS2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mif2_cmu_mif2[] = {
	V920_EVT0_CLK_BLK_MIF2_UID_MIF2_CMU_MIF2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_ddrphy3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_APBBR_DDRPHY3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_dmc3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_APBBR_DMC3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_apbbr_dmctz3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_APBBR_DMCTZ3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_axi2apb_mif3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_AXI2APB_MIF3_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ddrphy3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_DDRPHY3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dmc3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_DMC3_IPCLKPORT_PCLK1,
	V920_EVT0_GOUT_BLK_MIF3_UID_DMC3_IPCLKPORT_PCLK2,
	V920_EVT0_CLK_BLK_MIF3_UID_DMC3_IPCLKPORT_SOC_CLK,
	V920_EVT0_CLK_BLK_MIF3_UID_DMC3_IPCLKPORT_SOC_MPACE_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lhm_axi_p_mif3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_LHM_AXI_P_MIF3_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmuppc_debug3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_PPMUPPC_DEBUG3_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MIF3_UID_PPMUPPC_DEBUG3_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmuppc_dvfs3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_PPMUPPC_DVFS3_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MIF3_UID_PPMUPPC_DVFS3_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_mif3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_SYSREG_MIF3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mif3_cmu_mif3[] = {
	V920_EVT0_CLK_BLK_MIF3_UID_MIF3_CMU_MIF3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_debug3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_QCH_ADAPTER_PPMUPPC_DEBUG3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_dvfs3[] = {
	V920_EVT0_GOUT_BLK_MIF3_UID_QCH_ADAPTER_PPMUPPC_DVFS3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_misc_cmu_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_MISC_CMU_MISC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AXI_SI_D_MISC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pdma0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_PDMA0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pdma1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_PDMA1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pdma2[] = {
	V920_EVT0_CLK_BLK_MISC_UID_PDMA2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pdma3[] = {
	V920_EVT0_CLK_BLK_MISC_UID_PDMA3_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pdma4[] = {
	V920_EVT0_CLK_BLK_MISC_UID_PDMA4_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_PPMU_D0_MISC_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_PPMU_D0_MISC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pdma0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pdma1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pdma2[] = {
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA2_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pdma3[] = {
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA3_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_pdma4[] = {
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA4_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_QE_PDMA4_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_spdma0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_QE_SPDMA0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_QE_SPDMA0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_spdma1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_QE_SPDMA1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_QE_SPDMA1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SLH_AXI_MI_P_MISC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_spdma0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SPDMA0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_spdma1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SPDMA1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SYSMMU_S0_PMMU0_MISC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SYSREG_MISC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pdma0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VGEN_PDMA0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pdma1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VGEN_PDMA1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pdma2[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VGEN_PDMA2_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pdma3[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VGEN_PDMA3_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_pdma4[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VGEN_PDMA4_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_spdma0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VGEN_SPDMA0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_spdma1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VGEN_SPDMA1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_XIU_D0_MISC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_D_TZPC_MISC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ad_apb_sysmmu_s0_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_AD_APB_SYSMMU_S0_MISC_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SYSMMU_S0_MISC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_misc_1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SYSREG_MISC_1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_misc_2[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SYSREG_MISC_2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_XIU_D1_MISC_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mct0_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_MCT0_MISC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_otp_con_bira[] = {
	V920_EVT0_CLK_BLK_MISC_UID_OTP_CON_BIRA_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MISC_UID_OTP_CON_BIRA_IPCLKPORT_I_OSCCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_otp_con_bisr[] = {
	V920_EVT0_CLK_BLK_MISC_UID_OTP_CON_BISR_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MISC_UID_OTP_CON_BISR_IPCLKPORT_I_OSCCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_otp_con_top[] = {
	V920_EVT0_CLK_BLK_MISC_UID_OTP_CON_TOP_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_MISC_UID_OTP_CON_TOP_IPCLKPORT_I_OSCCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt_cluster0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_WDT_CLUSTER0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt_cluster1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_WDT_CLUSTER1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_icc_cluster0_gic[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AST_MI_L_ICC_CLUSTER0_GIC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_icc_cluster1_gic[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AST_MI_L_ICC_CLUSTER1_GIC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_icc_cluster2_gic[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AST_MI_L_ICC_CLUSTER2_GIC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_iri_gic_cluster0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AST_SI_L_IRI_GIC_CLUSTER0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_iri_gic_cluster1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AST_SI_L_IRI_GIC_CLUSTER1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_iri_gic_cluster2[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AST_SI_L_IRI_GIC_CLUSTER2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gic[] = {
	V920_EVT0_CLK_BLK_MISC_UID_GIC_IPCLKPORT_GCLK,
	V920_EVT0_CLK_BLK_MISC_UID_GIC_IPCLKPORT_GCLK_FDC,
	V920_EVT0_CLK_BLK_MISC_UID_GIC_IPCLKPORT_GCLK_FDC,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_misc_gic[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SLH_AXI_MI_P_MISC_GIC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_gic[] = {
	V920_EVT0_CLK_BLK_MISC_UID_QE_GIC_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_QE_GIC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_gic[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VGEN_GIC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_tmu_sub0[] = {
	V920_EVT0_CLK_BLK_MISC_UID_TMU_SUB0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_tmu_top[] = {
	V920_EVT0_CLK_BLK_MISC_UID_TMU_TOP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_volmon_int[] = {
	V920_EVT0_CLK_BLK_MISC_UID_VOLMON_INT_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_id_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AXI_SI_ID_MISC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_id_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_LH_AXI_MI_ID_MISC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_tmu_sub1[] = {
	V920_EVT0_CLK_BLK_MISC_UID_TMU_SUB1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adc_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_ADC_MISC_IPCLKPORT_PCLK_S1,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_PPMU_D1_MISC_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_MISC_UID_PPMU_D1_MISC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SYSMMU_S1_PMMU0_MISC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_SYSMMU_S1_MISC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mct1_misc[] = {
	V920_EVT0_CLK_BLK_MISC_UID_MCT1_MISC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_nocl0_cmu_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_NOCL0_CMU_NOCL0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_SYSREG_NOCL0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace2axi_dp0_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE2AXI_DP0_NOCL0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace2axi_dp1_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE2AXI_DP1_NOCL0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppc_g_sci[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_PPC_G_SCI_IPCLKPORT_CLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPC_G_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_trex_p_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_TREX_P_NOCL0_IPCLKPORT_ACLK_NOCL0,
	V920_EVT0_GOUT_BLK_NOCL0_UID_TREX_P_NOCL0_IPCLKPORT_PCLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_TREX_P_NOCL0_IPCLKPORT_PCLK_NOCL0,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_t_bdu[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_ATB_SI_T_BDU_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_bdu[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_BDU_IPCLKPORT_I_CLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_BDU_IPCLKPORT_I_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_G3D_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cpucl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_CPUCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cpucl1[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_CPUCL1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d0_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_ACEL_MI_D0_G3D_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d1_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_ACEL_MI_D1_G3D_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d2_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_ACEL_MI_D2_G3D_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d3_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_ACEL_MI_D3_G3D_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_trex_d_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_TREX_D_NOCL0_IPCLKPORT_PCLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_TREX_D_NOCL0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_alive[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_ALIVE_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_D_TZPC_NOCL0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d_cpucl2[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL2_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d2_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D2_G3D_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D2_G3D_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d3_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D3_G3D_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D3_G3D_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d0_trex[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D0_TREX_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D0_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d1_trex[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D1_TREX_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D1_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_g_nocl1[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_AST_MI_G_NOCL1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace_asb_d0_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D0_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace_asb_d1_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace_asb_d2_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace_asb_d3_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D3_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SCI_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_SCI_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_chi_mi_d_cluster0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_CHI_MI_D_CLUSTER0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mif0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_MIF0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace_asb_d4_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D4_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace_asb_d5_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D5_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace_asb_d6_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D6_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mpace_asb_d7_nocl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_MPACE_ASB_D7_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d0_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D0_G3D_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D0_G3D_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d1_g3d[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D1_G3D_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D1_G3D_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d2_trex[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D2_TREX_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D2_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d3_trex[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D3_TREX_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D3_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cpucl2[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_SLH_AXI_SI_P_CPUCL2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_g_nocl2[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_AST_MI_G_NOCL2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d_cpucl0[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL0_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d_cpucl1[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL1_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL0_UID_WOW_D_CPUCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_chi_mi_d_cluster1[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_CHI_MI_D_CLUSTER1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_chi_mi_d_cluster2[] = {
	V920_EVT0_GOUT_BLK_NOCL0_UID_LH_CHI_MI_D_CLUSTER2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mif1[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_MIF1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mif2[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_MIF2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mif3[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_MIF3_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_peric0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_PERIC0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_peric1[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_PERIC1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_misc_gic[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_MISC_GIC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D0_SCI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D1_SCI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d2_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D2_SCI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d3_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D3_SCI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d4_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D4_SCI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d5_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D5_SCI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d6_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D6_SCI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d7_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D7_SCI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d0_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D0_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D0_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d1_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D1_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D1_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d2_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D2_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D2_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d3_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D3_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D3_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d4_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D4_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D4_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d5_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D5_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D5_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d6_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D6_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D6_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d7_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D7_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D7_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cluster0_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_MI_P_CLUSTER0_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cluster1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_MI_P_CLUSTER1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cluster2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_MI_P_CLUSTER2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_misc[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_MISC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d0_nocl1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D0_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d1_nocl1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D1_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d2_nocl1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D2_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d3_nocl1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D3_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_p_nocl0_nocl1[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_SI_P_NOCL0_NOCL1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_p_nocl0_nocl2[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_SI_P_NOCL0_NOCL2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_trex[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D0_TREX_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D0_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_trex[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D1_TREX_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D1_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d2_trex[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D2_TREX_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D2_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d3_trex[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D3_TREX_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_D3_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_dp0_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP0_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP0_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_dp1_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP1_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP1_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_p_cpucl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL0_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL0_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_p_cpucl1[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL1_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_p_cpucl2[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL2_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_P_CPUCL2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wow_d_trex_qurgent[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D_TREX_QURGENT_IPCLKPORT_CLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_WOW_D_TREX_QURGENT_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_dp2_sci[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP2_SCI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP2_SCI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cmu[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_SLH_AXI_SI_P_CMU_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_lg_dnc_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_AST_MI_LG_DNC_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_dp_trex[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP_TREX_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL0_UID_PPMU_DP_TREX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d0_nocl2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D0_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d1_nocl2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D1_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d2_nocl2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D2_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d3_nocl2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_LH_TAXI_MI_D3_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adm_apb_g_bdu[] = {
	V920_EVT0_CLK_BLK_NOCL0_UID_ADM_APB_G_BDU_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_nocl1_cmu_nocl1[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_NOCL1_CMU_NOCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_nocl1[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_SYSREG_NOCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_trex_d_nocl1[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_TREX_D_NOCL1_IPCLKPORT_PCLK,
	V920_EVT0_GOUT_BLK_NOCL1_UID_TREX_D_NOCL1_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d0_hsi0[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_LH_ACEL_MI_D0_HSI0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d1_hsi0[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_LH_ACEL_MI_D1_HSI0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_alive[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_LH_AXI_MI_D_ALIVE_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_trex_p_nocl1[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_TREX_P_NOCL1_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_TREX_P_NOCL1_IPCLKPORT_PCLK_NOCL1,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_nocl1[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_D_TZPC_NOCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_g_nocl1[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_LH_AST_SI_G_NOCL1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_sfi[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_LH_AXI_MI_D_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_aud[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_LH_AXI_MI_D_AUD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_hsi2[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_SLH_AXI_SI_P_HSI2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_g_cssys[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_LH_AXI_MI_G_CSSYS_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d_ssp[] = {
	V920_EVT0_GOUT_BLK_NOCL1_UID_LH_ACEL_MI_D_SSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_aud[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_AUD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dnc[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_ssp[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_SSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_hsi0[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_HSI0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_g3d_ptw[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_AXI_MI_D_G3D_PTW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cacheaid_nocl1[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_CACHEAID_NOCL1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_CACHEAID_NOCL1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_sfi[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_SLH_AXI_SI_P_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d0_hsi2[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_ACEL_MI_D0_HSI2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d1_hsi2[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_ACEL_MI_D1_HSI2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d0_dnc[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_D0_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d1_dnc[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_D1_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d2_dnc[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_D2_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d3_dnc[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_D3_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_p_nocl0_nocl1[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_MI_P_NOCL0_NOCL1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d0_nocl1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_SI_D0_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d2_nocl1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_SI_D2_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d1_nocl1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_SI_D1_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d3_nocl1_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_LH_TAXI_SI_D3_NOCL1_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_alive[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_ALIVE_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_ALIVE_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_cssys[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_CSSYS_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_CSSYS_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_g3dmmu[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_G3DMMU_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_G3DMMU_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_nocl1_s0[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_NOCL1_S0_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_NOCL1_S0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_nocl1_s1[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_NOCL1_S1_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_NOCL1_S1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_nocl1_s2[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_NOCL1_S2_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_NOCL1_S2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_nocl1_s3[] = {
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_NOCL1_S3_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL1_UID_PPMU_NOCL1_S3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_trex_d_nocl2[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_TREX_D_NOCL2_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_NOCL2_UID_TREX_D_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_nocl2[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SYSREG_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_nocl2[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_D_TZPC_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d_hsi1[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_ACEL_MI_D_HSI1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_dpuf2[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_DPUF2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_dpuf0[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_DPUF0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_dpuf1[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_DPUF1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_acc[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D1_ACC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_mfc[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_dpuf2[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D1_DPUF2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_dpuf0[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D1_DPUF0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_dpuf1[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D1_DPUF1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_acc[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_ACC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_mfc[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D1_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_mfd[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D1_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_snw[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_mfd[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D0_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_isp[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D_ISP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_misc[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D_MISC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_taa[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AXI_MI_D_TAA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_g_nocl2[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_LH_AST_SI_G_NOCL2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_trex_p_nocl2[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_TREX_P_NOCL2_IPCLKPORT_PCLK_NOCL2,
	V920_EVT0_GOUT_BLK_NOCL2_UID_TREX_P_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_acc[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_ACC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dpuf1[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPUF1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dptx[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPTX_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dpub[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPUB_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dpuf0[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPUF0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dpuf2[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_DPUF2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_hsi1[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_HSI1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mfd[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_MFD_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_isp[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_ISP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mfc[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_MFC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_taa[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_TAA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_nocl2_cmu_nocl2[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_NOCL2_CMU_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_snw[] = {
	V920_EVT0_GOUT_BLK_NOCL2_UID_SLH_AXI_SI_P_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_snw[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_AXI_MI_D1_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_d2_snw[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_AXI_MI_D2_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d0_m2m[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_ACEL_MI_D0_M2M_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_mi_d1_m2m[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_ACEL_MI_D1_M2M_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_si_p_m2m[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_SLH_AXI_SI_P_M2M_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_nocl2[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_PPMU_D0_NOCL2_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_NOCL2_UID_PPMU_D0_NOCL2_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_nocl2[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_PPMU_D1_NOCL2_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL2_UID_PPMU_D1_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d2_nocl2[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_PPMU_D2_NOCL2_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL2_UID_PPMU_D2_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d3_nocl2[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_PPMU_D3_NOCL2_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL2_UID_PPMU_D3_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d0_nocl2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_SI_D0_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d1_nocl2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_SI_D1_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d2_nocl2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_SI_D2_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_si_d3_nocl2_nocl0[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_SI_D3_NOCL2_NOCL0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_taxi_mi_p_nocl0_nocl2[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_LH_TAXI_MI_P_NOCL0_NOCL2_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cacheaid_nocl2[] = {
	V920_EVT0_CLK_BLK_NOCL2_UID_CACHEAID_NOCL2_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_NOCL2_UID_CACHEAID_NOCL2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_peric0[] = {
	V920_EVT0_GOUT_BLK_PERIC0_UID_GPIO_PERIC0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_peric0[] = {
	V920_EVT0_GOUT_BLK_PERIC0_UID_SYSREG_PERIC0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_peric0_cmu_peric0[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_PERIC0_CMU_PERIC0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_peric0[] = {
	V920_EVT0_GOUT_BLK_PERIC0_UID_SLH_AXI_MI_P_PERIC0_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_peric0[] = {
	V920_EVT0_GOUT_BLK_PERIC0_UID_D_TZPC_PERIC0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi00_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI00_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI00_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi00_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI00_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI00_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi01_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI01_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI01_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi01_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI01_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI01_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi02_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI02_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI02_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi02_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI02_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI02_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi03_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI03_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI03_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi03_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI03_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI03_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi04_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI04_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI04_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi04_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI04_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI04_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi05_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI05_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI05_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi05_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI05_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI05_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi06_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI06_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI06_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi06_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI06_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI06_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi07_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI07_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI07_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi07_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI07_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI07_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi08_usi[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI08_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI08_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi08_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_USI08_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_USI08_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_i3c0[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_I3C0_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_I3C0_IPCLKPORT_I_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_i3c1[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_I3C1_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_I3C1_IPCLKPORT_I_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_i3c2[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_I3C2_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_I3C2_IPCLKPORT_I_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_i3c3[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_I3C3_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_PERIC0_UID_I3C3_IPCLKPORT_I_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pwm[] = {
	V920_EVT0_CLK_BLK_PERIC0_UID_PWM_IPCLKPORT_I_PCLK_S0,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_peric1[] = {
	V920_EVT0_GOUT_BLK_PERIC1_UID_GPIO_PERIC1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_peric1[] = {
	V920_EVT0_GOUT_BLK_PERIC1_UID_SYSREG_PERIC1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_peric1_cmu_peric1[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_PERIC1_CMU_PERIC1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_peric1[] = {
	V920_EVT0_GOUT_BLK_PERIC1_UID_SLH_AXI_MI_P_PERIC1_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_peric1[] = {
	V920_EVT0_GOUT_BLK_PERIC1_UID_D_TZPC_PERIC1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi09_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI09_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI09_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi09_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI09_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI09_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi10_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI10_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI10_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi10_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI10_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI10_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi11_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI11_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI11_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi11_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI11_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI11_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi12_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI12_USI_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI12_USI_IPCLKPORT_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi12_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI12_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI12_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi13_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI13_USI_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI13_USI_IPCLKPORT_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi13_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI13_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI13_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi14_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI14_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI14_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi15_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI15_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI15_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi14_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI14_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI14_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi15_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI15_I2C_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI15_I2C_IPCLKPORT_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi16_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI16_USI_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI16_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi16_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI16_I2C_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI16_I2C_IPCLKPORT_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi17_usi[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI17_USI_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI17_USI_IPCLKPORT_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi17_i2c[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_USI17_I2C_IPCLKPORT_IPCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_USI17_I2C_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_i3c4[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_I3C4_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_I3C4_IPCLKPORT_I_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_i3c5[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_I3C5_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_I3C5_IPCLKPORT_I_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_i3c6[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_I3C6_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_I3C6_IPCLKPORT_I_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_i3c7[] = {
	V920_EVT0_CLK_BLK_PERIC1_UID_I3C7_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_PERIC1_UID_I3C7_IPCLKPORT_I_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_s2d_cmu_s2d[] = {
	V920_EVT0_CLK_BLK_S2D_UID_S2D_CMU_S2D_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_bis_s2d[] = {
	V920_EVT0_GOUT_BLK_S2D_UID_BIS_S2D_IPCLKPORT_CLK,
	V920_EVT0_CLK_BLK_S2D_UID_BIS_S2D_IPCLKPORT_SCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_lg_scan2dram[] = {
	V920_EVT0_GOUT_BLK_S2D_UID_SLH_AXI_MI_LG_SCAN2DRAM_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sdma_cmu_sdma[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_SDMA_CMU_SDMA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_sdma[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_SYSREG_SDMA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld0_mmu_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD0_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld1_mmu_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD1_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld2_mmu_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD2_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld3_mmu_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD3_MMU_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld0_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD0_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld1_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD1_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld2_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD2_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld3_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD3_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld4_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD4_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld5_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD5_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld6_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD6_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ld7_sdma_dnc[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_SI_LD7_SDMA_DNC_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_sdma[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_D_TZPC_SDMA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_ctrl_dnc_sdma[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AXI_MI_LD_CTRL_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_lp_dnc_sdma[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_SLH_AXI_MI_LP_DNC_SDMA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_ld_sdma_dsp[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_LH_AST_SI_LD_SDMA_DSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ip_sdma[] = {
	V920_EVT0_CLK_BLK_SDMA_UID_IP_SDMA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi19_usi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_USI19_USI_IPCLKPORT_IPCLK,
	V920_EVT0_GOUT_BLK_SFI_UID_USI19_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon0[] = {
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_REF_CLK0,
	V920_EVT0_GOUT_BLK_SFI_UID_CLKMON0_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_MON_CLK10,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi18_usi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_USI18_USI_IPCLKPORT_IPCLK,
	V920_EVT0_GOUT_BLK_SFI_UID_USI18_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi20_usi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_USI20_USI_IPCLKPORT_IPCLK,
	V920_EVT0_GOUT_BLK_SFI_UID_USI20_USI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sc_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_SC_SFI_IPCLKPORT_I_ACLK,
	V920_EVT0_GOUT_BLK_SFI_UID_SC_SFI_IPCLKPORT_I_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_D_TZPC_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_sfi0[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_GPIO_SFI0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap4[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_MAILBOX_AP4_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mct0_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_MCT0_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_rom_crc32_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_ROM_CRC32_SFI_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_SFI_UID_ROM_CRC32_SFI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_SYSREG_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_volmon[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_VOLMON_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt0_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_WDT0_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_usi21_usi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_USI21_USI_IPCLKPORT_PCLK,
	V920_EVT0_GOUT_BLK_SFI_UID_USI21_USI_IPCLKPORT_IPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_sfpc_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_D_SFPC_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_intmem_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_INTMEM_SFI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SFI_UID_INTMEM_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_asyncapb_intmem[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_ASYNCAPB_INTMEM_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_SLH_AXI_MI_P_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_LH_AXI_SI_D_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_dp_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_XIU_DP_SFI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xspi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_XSPI_IPCLKPORT_FLASH_CLK,
	V920_EVT0_CLK_BLK_SFI_UID_XSPI_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_SFI_UID_XSPI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt0_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_LH_ATB_SI_LT0_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt1_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_LH_ATB_SI_LT1_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt2_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_LH_ATB_SI_LT2_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_atb_si_lt3_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_LH_ATB_SI_LT3_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_wdt1_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_WDT1_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_busif_cmu_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_BUSIF_CMU_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pmu_handshake_bus[] = {
	V920_EVT0_CLK_BLK_SFI_UID_PMU_HANDSHAKE_BUS_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_cluster_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_CLUSTER_SFI_IPCLKPORT_CLKIN,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_p1_sfi[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_XIU_P1_SFI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sbist0[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_SBIST0_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sbist1[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_SBIST1_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_pmc[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_PMC_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon1[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_CLKMON1_IPCLKPORT_PCLK,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON1_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON1_IPCLKPORT_MON_CLK00,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON1_IPCLKPORT_MON_CLK10,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON1_IPCLKPORT_REF_CLK1,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_clkmon2[] = {
	V920_EVT0_GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_PCLK,
	V920_EVT0_GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_REF_CLK0,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON2_IPCLKPORT_REF_CLK1,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON2_IPCLKPORT_MON_CLK10,
	V920_EVT0_CLK_BLK_SFI_UID_CLKMON2_IPCLKPORT_MON_CLK00,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ip1_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IP1_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip1_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IP1_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_idp0_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IDP0_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_idp1_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IDP1_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ip0_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IP0_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_idp0_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IDP0_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_idp1_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IDP1_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip0_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IP0_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap0[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP0_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap1[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap7[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP7_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap2[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP2_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap3[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP3_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap6[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP6_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_apm[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_APM_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_ap5[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_AP5_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_p0_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_SFMPU_P0_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_p1_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_SFMPU_P1_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_p2_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_SFMPU_P2_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_p3_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_SFMPU_P3_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_d_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_VGEN_LITE_D_SFI_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_SYSMMU_S0_SFI_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_SYSMMU_S0_PMMU_SFI_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_XIU_D_SFI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sfmpu_p4_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_SFMPU_P4_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_can_fd0[] = {
	V920_EVT0_CLK_BLK_SFI_UID_CAN_FD0_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_SFI_UID_CAN_FD0_IPCLKPORT_I_CCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_can_fd1[] = {
	V920_EVT0_CLK_BLK_SFI_UID_CAN_FD1_IPCLKPORT_I_PCLK,
	V920_EVT0_CLK_BLK_SFI_UID_CAN_FD1_IPCLKPORT_I_CCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mct1_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MCT1_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_baaw_p_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_BAAW_P_SFI_IPCLKPORT_I_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_asyncapb_cluster_sfi_debug[] = {
	V920_EVT0_CLK_BLK_SFI_UID_ASYNCAPB_CLUSTER_SFI_DEBUG_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_PPMU_D_SFI_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SFI_UID_PPMU_D_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dma_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_DMA_SFI_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_abox[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_ABOX_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_mailbox_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_MAILBOX_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_dapapbap_mux_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_DAPAPBAP_MUX_SFI_IPCLKPORT_DAPCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_adm_dap_g_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_ADM_DAP_G_SFI_IPCLKPORT_DAPCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_tmu_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_TMU_SFI_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_gpio_sfi1[] = {
	V920_EVT0_CLK_BLK_SFI_UID_GPIO_SFI1_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_ip2_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_SI_IP2_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip2_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_LH_AXI_MI_IP2_SFI_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_asyncapb_fmu[] = {
	V920_EVT0_CLK_BLK_SFI_UID_ASYNCAPB_FMU_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_fmu[] = {
	V920_EVT0_CLK_BLK_SFI_UID_FMU_IPCLKPORT_PCLK_FMU,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_baaw_d_sfi[] = {
	V920_EVT0_CLK_BLK_SFI_UID_BAAW_D_SFI_IPCLKPORT_I_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_snw_cmu_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SNW_CMU_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_D_TZPC_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SYSREG_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_d0_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_VGEN_LITE_D0_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_d1_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_VGEN_LITE_D1_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_d2_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_VGEN_LITE_D2_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d0_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_PPMU_D0_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_PPMU_D0_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d1_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_PPMU_D1_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_PPMU_D1_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_LH_AXI_SI_D0_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_LH_AXI_SI_D1_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SLH_AXI_MI_P_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_isp_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_LH_AST_MI_L_ISP_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_acc_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_LH_AST_MI_L_ACC_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_snw_snw0[] = {
	V920_EVT0_CLK_BLK_SNW_UID_AS_APB_SNW_SNW0_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d0_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D0_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D0_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d1_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D1_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D1_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d2_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D2_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D2_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d3_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D3_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D3_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf0_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF0_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF0_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d4_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D4_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D4_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d5_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D5_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D5_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d6_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D6_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D6_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d7_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D7_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D7_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf1_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF1_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF1_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_d2_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_PPMU_D2_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_PPMU_D2_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d2_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_LH_AXI_SI_D2_SNW_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d8_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D8_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D8_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d9_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D9_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D9_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d10_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D10_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D10_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d11_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_D11_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_D11_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf2_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF2_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF2_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf4_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF4_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF4_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf3_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF3_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF3_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf5_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF5_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF5_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf6_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF6_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF6_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_votf7_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF7_SNW_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SNW_UID_QE_VOTF7_SNW_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d3_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_XIU_D3_SNW_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SYSMMU_S0_PMMU0_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SYSMMU_S0_PMMU1_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SYSMMU_S0_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SYSMMU_S1_PMMU0_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d4_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_XIU_D4_SNW_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s1_snw[] = {
	V920_EVT0_CLK_BLK_SNW_UID_SYSMMU_S1_SNW_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ssp_cmu_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_SSP_CMU_SSP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_SLH_AXI_MI_P_SSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_D_TZPC_SSP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_SYSREG_SSP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_otp_adk_deserial_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_OTP_ADK_DESERIAL_SSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_securitycontroller[] = {
	V920_EVT0_CLK_BLK_SSP_UID_SECURITYCONTROLLER_IPCLKPORT_I_ACLK,
	V920_EVT0_CLK_BLK_SSP_UID_SECURITYCONTROLLER_IPCLKPORT_I_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_SYSMMU_S0_PMMU0_SSP_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_baaw_sc[] = {
	V920_EVT0_CLK_BLK_SSP_UID_BAAW_SC_IPCLKPORT_I_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d0_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_XIU_D0_SSP_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_acel_si_d_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_LH_ACEL_SI_D_SSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_PPMU_SSP_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SSP_UID_PPMU_SSP_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_strong[] = {
	V920_EVT0_CLK_BLK_SSP_UID_QE_STRONG_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SSP_UID_QE_STRONG_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_sc[] = {
	V920_EVT0_CLK_BLK_SSP_UID_QE_SC_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SSP_UID_QE_SC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_asyncapb_sysmmu[] = {
	V920_EVT0_CLK_BLK_SSP_UID_ASYNCAPB_SYSMMU_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_SYSMMU_S0_SSP_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_XIU_D1_SSP_IPCLKPORT_ACLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_mi_id_strong[] = {
	V920_EVT0_CLK_BLK_SSP_UID_LH_AXI_MI_ID_STRONG_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_rtic[] = {
	V920_EVT0_CLK_BLK_SSP_UID_RTIC_IPCLKPORT_I_ACLK,
	V920_EVT0_CLK_BLK_SSP_UID_RTIC_IPCLKPORT_I_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_rtic[] = {
	V920_EVT0_CLK_BLK_SSP_UID_QE_RTIC_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_SSP_UID_QE_RTIC_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_VGEN_LITE_SSP_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_otp_puf_deserial_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_OTP_PUF_DESERIAL_SSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_otp_rek_deserial_ssp[] = {
	V920_EVT0_CLK_BLK_SSP_UID_OTP_REK_DESERIAL_SSP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_strong_cmu_strong[] = {
	V920_EVT0_CLK_BLK_STRONG_UID_STRONG_CMU_STRONG_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_axi_si_d_taa[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_LH_AXI_SI_D_TAA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_taa[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_SLH_AXI_MI_P_TAA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysreg_taa[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_SYSREG_TAA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_taa_cmu_taa[] = {
	V920_EVT0_CLK_BLK_TAA_UID_TAA_CMU_TAA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_acc_taa[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_LH_AST_MI_L_ACC_TAA_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_lh_ast_si_l_taa_isp[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_LH_AST_SI_L_TAA_ISP_IPCLKPORT_I_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_d_tzpc_taa[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_D_TZPC_TAA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_as_apb_taa0[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_AS_APB_TAA0_IPCLKPORT_PCLKM,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_ppmu_taa[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_PPMU_TAA_IPCLKPORT_ACLK,
	V920_EVT0_GOUT_BLK_TAA_UID_PPMU_TAA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_taa[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_SYSMMU_S0_PMMU0_TAA_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_taa[] = {
	V920_EVT0_GOUT_BLK_TAA_UID_TAA_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d0_taa[] = {
	V920_EVT0_CLK_BLK_TAA_UID_QE_D0_TAA_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_TAA_UID_QE_D0_TAA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d1_taa[] = {
	V920_EVT0_CLK_BLK_TAA_UID_QE_D1_TAA_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_TAA_UID_QE_D1_TAA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d2_taa[] = {
	V920_EVT0_CLK_BLK_TAA_UID_QE_D2_TAA_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_TAA_UID_QE_D2_TAA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_qe_d3_taa[] = {
	V920_EVT0_CLK_BLK_TAA_UID_QE_D3_TAA_IPCLKPORT_ACLK,
	V920_EVT0_CLK_BLK_TAA_UID_QE_D3_TAA_IPCLKPORT_PCLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_vgen_lite_d_taa[] = {
	V920_EVT0_CLK_BLK_TAA_UID_VGEN_LITE_D_TAA_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_sysmmu_s0_taa[] = {
	V920_EVT0_CLK_BLK_TAA_UID_SYSMMU_S0_TAA_IPCLKPORT_CLK,
};
enum v920_evt0_clk_id cmucal_vclk_v920_evt0_ip_xiu_d1_taa[] = {
	V920_EVT0_CLK_BLK_TAA_UID_XIU_D1_TAA_IPCLKPORT_ACLK,
};

/* DVFS VCLK -> LUT List */
struct vclk_lut cmucal_vclk_v920_evt0_vdd_mif_lut[] = {
	{7500000, v920_evt0_vdd_mif_od_lut_params},
	{4266000, v920_evt0_vdd_mif_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_vdd_cpucl0_lut[] = {
	{2300000, v920_evt0_vdd_cpucl0_sod_lut_params},
	{2000000, v920_evt0_vdd_cpucl0_od_lut_params},
	{1550000, v920_evt0_vdd_cpucl0_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_vdd_cpucl1_lut[] = {
	{2300000, v920_evt0_vdd_cpucl1_sod_lut_params},
	{2000000, v920_evt0_vdd_cpucl1_od_lut_params},
	{1550000, v920_evt0_vdd_cpucl1_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_vdd_sfi_lut[] = {
	{1200000, v920_evt0_vdd_sfi_od_lut_params},
	{800000, v920_evt0_vdd_sfi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_vdd_aud_lut[] = {
	{740000, v920_evt0_vdd_aud_sod_lut_params},
	{738000, v920_evt0_vdd_aud_od_lut_params},
	{590000, v920_evt0_vdd_aud_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_vdd_int_cmu_lut[] = {
	{800000, v920_evt0_vdd_int_cmu_od_lut_params},
	{618000, v920_evt0_vdd_int_cmu_nm_lut_params},
	{618000, v920_evt0_vdd_int_cmu_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_vdd_cpucl2_lut[] = {
	{2300000, v920_evt0_vdd_cpucl2_sod_lut_params},
	{2000000, v920_evt0_vdd_cpucl2_od_lut_params},
	{1550000, v920_evt0_vdd_cpucl2_nm_lut_params},
};

/* SPECIAL VCLK -> LUT List */
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_apm_timer_lut[] = {
	{38400, v920_evt0_mux_clk_apm_timer_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_aud_uaif3_lut[] = {
	{740000, v920_evt0_mux_clk_aud_uaif3_sod_lut_params},
	{738000, v920_evt0_mux_clk_aud_uaif3_od_lut_params},
	{590000, v920_evt0_mux_clk_aud_uaif3_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_cmu_cmuref_lut[] = {
	{266667, v920_evt0_mux_cmu_cmuref_sod_lut_params},
	{6500, v920_evt0_mux_cmu_cmuref_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_cpucl0_cmuref_lut[] = {
	{266667, v920_evt0_mux_cpucl0_cmuref_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_cpucl1_cmuref_lut[] = {
	{266667, v920_evt0_mux_cpucl1_cmuref_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_cpucl2_cmuref_lut[] = {
	{266667, v920_evt0_mux_cpucl2_cmuref_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_mif_cmuref_lut[] = {
	{266667, v920_evt0_mux_mif_cmuref_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_mif1_cmuref_lut[] = {
	{26000, v920_evt0_mux_mif1_cmuref_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_mif2_cmuref_lut[] = {
	{26000, v920_evt0_mux_mif2_cmuref_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_mif3_cmuref_lut[] = {
	{26000, v920_evt0_mux_mif3_cmuref_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_nocl0_cmuref_lut[] = {
	{266667, v920_evt0_mux_nocl0_cmuref_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_nocl1_cmuref_lut[] = {
	{266667, v920_evt0_mux_nocl1_cmuref_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_nocl2_cmuref_lut[] = {
	{266667, v920_evt0_mux_nocl2_cmuref_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_dpub_dsim_lut[] = {
	{76190, v920_evt0_clkcmu_dpub_dsim_sod_lut_params},
	{13000, v920_evt0_clkcmu_dpub_dsim_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_apm_spmi_lut[] = {
	{20828, v920_evt0_div_clk_apm_spmi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_aud_mclk_lut[] = {
	{73800, v920_evt0_div_clk_aud_mclk_od_lut_params},
	{49333, v920_evt0_div_clk_aud_mclk_sod_lut_params},
	{49167, v920_evt0_div_clk_aud_mclk_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clk_aud_avb_eth_lut[] = {
	{8, v920_evt0_clk_aud_avb_eth_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clk_cmu_pllclkout_lut[] = {
	{213200, v920_evt0_clk_cmu_pllclkout_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clk_add_ch_clk_lut[] = {
	{2133, v920_evt0_clk_add_ch_clk_sod_lut_params},
	{1536, v920_evt0_clk_add_ch_clk_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_cmu_nocp_lut[] = {
	{710667, v920_evt0_div_clk_cmu_nocp_ud_lut_params},
	{355333, v920_evt0_div_clk_cmu_nocp_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cis_mclk3_lut[] = {
	{100000, v920_evt0_clkcmu_cis_mclk3_sod_lut_params},
	{38400, v920_evt0_clkcmu_cis_mclk3_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cis_mclk0_lut[] = {
	{100000, v920_evt0_clkcmu_cis_mclk0_sod_lut_params},
	{38400, v920_evt0_clkcmu_cis_mclk0_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cis_mclk1_lut[] = {
	{100000, v920_evt0_clkcmu_cis_mclk1_sod_lut_params},
	{38400, v920_evt0_clkcmu_cis_mclk1_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cis_mclk2_lut[] = {
	{100000, v920_evt0_clkcmu_cis_mclk2_sod_lut_params},
	{38400, v920_evt0_clkcmu_cis_mclk2_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_cpucl0_shortstop_lut[] = {
	{240000, v920_evt0_div_clk_cpucl0_shortstop_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cpucl0_cluster_lut[] = {
	{480000, v920_evt0_clkcmu_cpucl0_cluster_sod_lut_params},
	{9750, v920_evt0_clkcmu_cpucl0_cluster_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cpucl0_switch_lut[] = {
	{533000, v920_evt0_clkcmu_cpucl0_switch_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_cpucl1_shortstop_lut[] = {
	{240000, v920_evt0_div_clk_cpucl1_shortstop_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cpucl1_cluster_lut[] = {
	{480000, v920_evt0_clkcmu_cpucl1_cluster_sod_lut_params},
	{9750, v920_evt0_clkcmu_cpucl1_cluster_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cpucl1_switch_lut[] = {
	{533000, v920_evt0_clkcmu_cpucl1_switch_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_cpucl2_shortstop_lut[] = {
	{400000, v920_evt0_div_clk_cpucl2_shortstop_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cpucl2_cluster_lut[] = {
	{800000, v920_evt0_clkcmu_cpucl2_cluster_sod_lut_params},
	{39000, v920_evt0_clkcmu_cpucl2_cluster_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_cpucl2_switch_lut[] = {
	{1066000, v920_evt0_clkcmu_cpucl2_switch_ud_lut_params},
	{800000, v920_evt0_clkcmu_cpucl2_switch_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_dpuf1_nocp_lut[] = {
	{266667, v920_evt0_div_clk_dpuf1_nocp_nm_lut_params},
	{26000, v920_evt0_div_clk_dpuf1_nocp_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_dpuf1_noc_lut[] = {
	{666500, v920_evt0_clkcmu_dpuf1_noc_sod_lut_params},
	{622000, v920_evt0_clkcmu_dpuf1_noc_od_lut_params},
	{533333, v920_evt0_clkcmu_dpuf1_noc_nm_lut_params},
	{19500, v920_evt0_clkcmu_dpuf1_noc_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_dpuf2_nocp_lut[] = {
	{166625, v920_evt0_div_clk_dpuf2_nocp_nm_lut_params},
	{26000, v920_evt0_div_clk_dpuf2_nocp_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_gnpu1_nocp_lut[] = {
	{100000, v920_evt0_div_clk_gnpu1_nocp_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi00_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi00_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_peric0_ip_lut[] = {
	{400000, v920_evt0_clkcmu_peric0_ip_sod_lut_params},
	{9750, v920_evt0_clkcmu_peric0_ip_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi01_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi01_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi02_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi02_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi03_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi03_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi04_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi04_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi05_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi05_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi06_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi06_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi07_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi07_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric0_usi08_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric0_usi08_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi09_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi09_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_clkcmu_peric1_ip_lut[] = {
	{400000, v920_evt0_clkcmu_peric1_ip_sod_lut_params},
	{9750, v920_evt0_clkcmu_peric1_ip_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi10_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi10_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi11_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi11_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi12_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi12_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi13_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi13_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi14_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi14_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi15_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi15_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi16_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi16_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_peric1_usi17_usi_lut[] = {
	{400000, v920_evt0_div_clk_peric1_usi17_usi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_sfi_pllclkout_lut[] = {
	{1200000, v920_evt0_mux_clk_sfi_pllclkout_od_lut_params},
	{800000, v920_evt0_mux_clk_sfi_pllclkout_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_sfi_usi20_lut[] = {
	{1200000, v920_evt0_mux_clk_sfi_usi20_od_lut_params},
	{800000, v920_evt0_mux_clk_sfi_usi20_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_sfi_usi19_lut[] = {
	{1200000, v920_evt0_mux_clk_sfi_usi19_od_lut_params},
	{800000, v920_evt0_mux_clk_sfi_usi19_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_div_clk_sfi_cpu_cntclk_lut[] = {
	{300000, v920_evt0_div_clk_sfi_cpu_cntclk_od_lut_params},
	{200000, v920_evt0_div_clk_sfi_cpu_cntclk_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_sfi_usi21_lut[] = {
	{1200000, v920_evt0_mux_clk_sfi_usi21_od_lut_params},
	{800000, v920_evt0_mux_clk_sfi_usi21_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_sfi_can0_lut[] = {
	{1200000, v920_evt0_mux_clk_sfi_can0_od_lut_params},
	{800000, v920_evt0_mux_clk_sfi_can0_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_sfi_can1_lut[] = {
	{1200000, v920_evt0_mux_clk_sfi_can1_od_lut_params},
	{800000, v920_evt0_mux_clk_sfi_can1_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_cpucl0_core_lut[] = {
	{2300000, v920_evt0_mux_clk_cpucl0_core_sod_lut_params},
	{2000000, v920_evt0_mux_clk_cpucl0_core_od_lut_params},
	{1550000, v920_evt0_mux_clk_cpucl0_core_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_cpucl1_core_lut[] = {
	{2300000, v920_evt0_mux_clk_cpucl1_core_sod_lut_params},
	{2000000, v920_evt0_mux_clk_cpucl1_core_od_lut_params},
	{1550000, v920_evt0_mux_clk_cpucl1_core_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_mux_clk_cpucl2_core_lut[] = {
	{2300000, v920_evt0_mux_clk_cpucl2_core_sod_lut_params},
	{2000000, v920_evt0_mux_clk_cpucl2_core_od_lut_params},
	{1550000, v920_evt0_mux_clk_cpucl2_core_nm_lut_params},
};

/* COMMON VCLK -> LUT List */
struct vclk_lut cmucal_vclk_v920_evt0_blk_cmu_lut[] = {
	{2132000, v920_evt0_blk_cmu_od_lut_params},
	{2132000, v920_evt0_blk_cmu_sod_lut_params},
	{2132000, v920_evt0_blk_cmu_ud_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_g3d_lut[] = {
	{1500000, v920_evt0_blk_g3d_sod_lut_params},
	{486000, v920_evt0_blk_g3d_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_hsi2_lut[] = {
	{19500, v920_evt0_blk_hsi2_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_mif1_lut[] = {
	{26000, v920_evt0_blk_mif1_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_mif2_lut[] = {
	{26000, v920_evt0_blk_mif2_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_mif3_lut[] = {
	{26000, v920_evt0_blk_mif3_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_apm_lut[] = {
	{166625, v920_evt0_blk_apm_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_aud_lut[] = {
	{200000, v920_evt0_blk_aud_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_cpucl0_lut[] = {
	{240000, v920_evt0_blk_cpucl0_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_cpucl1_lut[] = {
	{240000, v920_evt0_blk_cpucl1_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_cpucl2_lut[] = {
	{400000, v920_evt0_blk_cpucl2_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_hsi1_lut[] = {
	{40000, v920_evt0_blk_hsi1_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_peric0_lut[] = {
	{200000, v920_evt0_blk_peric0_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_peric1_lut[] = {
	{200000, v920_evt0_blk_peric1_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_s2d_lut[] = {
	{100000, v920_evt0_blk_s2d_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_sfi_lut[] = {
	{1200000, v920_evt0_blk_sfi_od_lut_params},
	{800000, v920_evt0_blk_sfi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_acc_lut[] = {
	{200000, v920_evt0_blk_acc_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_dnc_lut[] = {
	{177667, v920_evt0_blk_dnc_od_lut_params},
	{38400, v920_evt0_blk_dnc_sod_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_dpub_lut[] = {
	{266667, v920_evt0_blk_dpub_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_dpuf_lut[] = {
	{266667, v920_evt0_blk_dpuf_nm_lut_params},
	{133333, v920_evt0_blk_dpuf_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_dsp_lut[] = {
	{133250, v920_evt0_blk_dsp_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_gnpu_lut[] = {
	{166625, v920_evt0_blk_gnpu_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_hsi0_lut[] = {
	{100000, v920_evt0_blk_hsi0_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_isp_lut[] = {
	{200000, v920_evt0_blk_isp_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_m2m_lut[] = {
	{266667, v920_evt0_blk_m2m_nm_lut_params},
	{38400, v920_evt0_blk_m2m_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_mfc_lut[] = {
	{266667, v920_evt0_blk_mfc_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_mfd_lut[] = {
	{266667, v920_evt0_blk_mfd_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_misc_lut[] = {
	{88889, v920_evt0_blk_misc_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_nocl0_lut[] = {
	{114286, v920_evt0_blk_nocl0_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_nocl1_lut[] = {
	{133333, v920_evt0_blk_nocl1_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_nocl2_lut[] = {
	{160000, v920_evt0_blk_nocl2_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_sdma_lut[] = {
	{166625, v920_evt0_blk_sdma_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_snw_lut[] = {
	{166625, v920_evt0_blk_snw_od_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_ssp_lut[] = {
	{266667, v920_evt0_blk_ssp_nm_lut_params},
};
struct vclk_lut cmucal_vclk_v920_evt0_blk_taa_lut[] = {
	{177667, v920_evt0_blk_taa_od_lut_params},
};

/* Switch VCLK -> LUT Parameter List */
struct switch_lut v920_evt0_mux_clk_aud_cpu_lut[] = {
	{710667, 3, 0},
	{533333, 6, 0},
	{533000, 0, 1},
};
struct switch_lut v920_evt0_mux_clk_aud_noc_lut[] = {
	{533333, 3, 0},
	{400000, 0, 1},
	{19500, 0, 0},
};
struct switch_lut v920_evt0_mux_clk_hsi2_ethernet_lut[] = {
	{355333, 2, 1},
	{6500, 1, 2},
};
/*================================ SWPLL List =================================*/
struct vclk_switch v920_evt0_switch_vdd_aud[] = {
	{V920_EVT0_MUX_CLK_AUD_CPU, V920_EVT0_MUX_CLKCMU_AUD_CPU, V920_EVT0_CLKCMU_AUD_CPU, V920_EVT0_GATE_CLKCMU_AUD_CPU, V920_EVT0_MUX_CLKCMU_AUD_CPU_USER, v920_evt0_mux_clk_aud_cpu_lut, 3},
	{V920_EVT0_MUX_CLK_AUD_NOC, V920_EVT0_MUX_CLKCMU_AUD_NOC, V920_EVT0_CLKCMU_AUD_NOC, V920_EVT0_GATE_CLKCMU_AUD_NOC, V920_EVT0_MUX_CLKCMU_AUD_NOC_USER, v920_evt0_mux_clk_aud_noc_lut, 3},
};
struct vclk_switch v920_evt0_switch_blk_hsi2[] = {
	{V920_EVT0_MUX_CLK_HSI2_ETHERNET, V920_EVT0_MUX_CLKCMU_HSI2_ETHERNET, V920_EVT0_CLKCMU_HSI2_ETHERNET, V920_EVT0_GATE_CLKCMU_HSI2_ETHERNET, V920_EVT0_MUX_CLKCMU_HSI2_ETHERNET_USER, v920_evt0_mux_clk_hsi2_ethernet_lut, 2},
};

/*================================ VCLK List =================================*/
unsigned int v920_evt0_cmucal_vclk_size = 1146;
struct vclk v920_evt0_cmucal_vclk_list[] = {

/* DVFS VCLK*/
	CMUCAL_VCLK(VCLK_V920_EVT0_VDD_MIF, cmucal_vclk_v920_evt0_vdd_mif_lut, cmucal_vclk_v920_evt0_vdd_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_VDD_CPUCL0, cmucal_vclk_v920_evt0_vdd_cpucl0_lut, cmucal_vclk_v920_evt0_vdd_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_VDD_CPUCL1, cmucal_vclk_v920_evt0_vdd_cpucl1_lut, cmucal_vclk_v920_evt0_vdd_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_VDD_SFI, cmucal_vclk_v920_evt0_vdd_sfi_lut, cmucal_vclk_v920_evt0_vdd_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_VDD_AUD, cmucal_vclk_v920_evt0_vdd_aud_lut, cmucal_vclk_v920_evt0_vdd_aud, NULL, v920_evt0_switch_vdd_aud),
	CMUCAL_VCLK(VCLK_V920_EVT0_VDD_INT_CMU, cmucal_vclk_v920_evt0_vdd_int_cmu_lut, cmucal_vclk_v920_evt0_vdd_int_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_VDD_CPUCL2, cmucal_vclk_v920_evt0_vdd_cpucl2_lut, cmucal_vclk_v920_evt0_vdd_cpucl2, NULL, NULL),

/* SPECIAL VCLK*/
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_APM_TIMER, cmucal_vclk_v920_evt0_mux_clk_apm_timer_lut, cmucal_vclk_v920_evt0_mux_clk_apm_timer, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_AUD_UAIF3, cmucal_vclk_v920_evt0_mux_clk_aud_uaif3_lut, cmucal_vclk_v920_evt0_mux_clk_aud_uaif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CMU_CMUREF, cmucal_vclk_v920_evt0_mux_cmu_cmuref_lut, cmucal_vclk_v920_evt0_mux_cmu_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CPUCL0_CMUREF, cmucal_vclk_v920_evt0_mux_cpucl0_cmuref_lut, cmucal_vclk_v920_evt0_mux_cpucl0_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CPUCL1_CMUREF, cmucal_vclk_v920_evt0_mux_cpucl1_cmuref_lut, cmucal_vclk_v920_evt0_mux_cpucl1_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CPUCL2_CMUREF, cmucal_vclk_v920_evt0_mux_cpucl2_cmuref_lut, cmucal_vclk_v920_evt0_mux_cpucl2_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_MIF_CMUREF, cmucal_vclk_v920_evt0_mux_mif_cmuref_lut, cmucal_vclk_v920_evt0_mux_mif_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_MIF1_CMUREF, cmucal_vclk_v920_evt0_mux_mif1_cmuref_lut, cmucal_vclk_v920_evt0_mux_mif1_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_MIF2_CMUREF, cmucal_vclk_v920_evt0_mux_mif2_cmuref_lut, cmucal_vclk_v920_evt0_mux_mif2_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_MIF3_CMUREF, cmucal_vclk_v920_evt0_mux_mif3_cmuref_lut, cmucal_vclk_v920_evt0_mux_mif3_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_NOCL0_CMUREF, cmucal_vclk_v920_evt0_mux_nocl0_cmuref_lut, cmucal_vclk_v920_evt0_mux_nocl0_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_NOCL1_CMUREF, cmucal_vclk_v920_evt0_mux_nocl1_cmuref_lut, cmucal_vclk_v920_evt0_mux_nocl1_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_NOCL2_CMUREF, cmucal_vclk_v920_evt0_mux_nocl2_cmuref_lut, cmucal_vclk_v920_evt0_mux_nocl2_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_DPUB_DSIM, cmucal_vclk_v920_evt0_clkcmu_dpub_dsim_lut, cmucal_vclk_v920_evt0_clkcmu_dpub_dsim, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_APM_SPMI, cmucal_vclk_v920_evt0_div_clk_apm_spmi_lut, cmucal_vclk_v920_evt0_div_clk_apm_spmi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_AUD_MCLK, cmucal_vclk_v920_evt0_div_clk_aud_mclk_lut, cmucal_vclk_v920_evt0_div_clk_aud_mclk, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLK_AUD_AVB_ETH, cmucal_vclk_v920_evt0_clk_aud_avb_eth_lut, cmucal_vclk_v920_evt0_clk_aud_avb_eth, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLK_CMU_PLLCLKOUT, cmucal_vclk_v920_evt0_clk_cmu_pllclkout_lut, cmucal_vclk_v920_evt0_clk_cmu_pllclkout, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLK_ADD_CH_CLK, cmucal_vclk_v920_evt0_clk_add_ch_clk_lut, cmucal_vclk_v920_evt0_clk_add_ch_clk, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_CMU_NOCP, cmucal_vclk_v920_evt0_div_clk_cmu_nocp_lut, cmucal_vclk_v920_evt0_div_clk_cmu_nocp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CIS_MCLK3, cmucal_vclk_v920_evt0_clkcmu_cis_mclk3_lut, cmucal_vclk_v920_evt0_clkcmu_cis_mclk3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CIS_MCLK0, cmucal_vclk_v920_evt0_clkcmu_cis_mclk0_lut, cmucal_vclk_v920_evt0_clkcmu_cis_mclk0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CIS_MCLK1, cmucal_vclk_v920_evt0_clkcmu_cis_mclk1_lut, cmucal_vclk_v920_evt0_clkcmu_cis_mclk1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CIS_MCLK2, cmucal_vclk_v920_evt0_clkcmu_cis_mclk2_lut, cmucal_vclk_v920_evt0_clkcmu_cis_mclk2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_CPUCL0_SHORTSTOP, cmucal_vclk_v920_evt0_div_clk_cpucl0_shortstop_lut, cmucal_vclk_v920_evt0_div_clk_cpucl0_shortstop, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CPUCL0_CLUSTER, cmucal_vclk_v920_evt0_clkcmu_cpucl0_cluster_lut, cmucal_vclk_v920_evt0_clkcmu_cpucl0_cluster, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CPUCL0_SWITCH, cmucal_vclk_v920_evt0_clkcmu_cpucl0_switch_lut, cmucal_vclk_v920_evt0_clkcmu_cpucl0_switch, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_CPUCL1_SHORTSTOP, cmucal_vclk_v920_evt0_div_clk_cpucl1_shortstop_lut, cmucal_vclk_v920_evt0_div_clk_cpucl1_shortstop, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CPUCL1_CLUSTER, cmucal_vclk_v920_evt0_clkcmu_cpucl1_cluster_lut, cmucal_vclk_v920_evt0_clkcmu_cpucl1_cluster, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CPUCL1_SWITCH, cmucal_vclk_v920_evt0_clkcmu_cpucl1_switch_lut, cmucal_vclk_v920_evt0_clkcmu_cpucl1_switch, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_CPUCL2_SHORTSTOP, cmucal_vclk_v920_evt0_div_clk_cpucl2_shortstop_lut, cmucal_vclk_v920_evt0_div_clk_cpucl2_shortstop, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CPUCL2_CLUSTER, cmucal_vclk_v920_evt0_clkcmu_cpucl2_cluster_lut, cmucal_vclk_v920_evt0_clkcmu_cpucl2_cluster, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_CPUCL2_SWITCH, cmucal_vclk_v920_evt0_clkcmu_cpucl2_switch_lut, cmucal_vclk_v920_evt0_clkcmu_cpucl2_switch, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_DPUF1_NOCP, cmucal_vclk_v920_evt0_div_clk_dpuf1_nocp_lut, cmucal_vclk_v920_evt0_div_clk_dpuf1_nocp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_DPUF1_NOC, cmucal_vclk_v920_evt0_clkcmu_dpuf1_noc_lut, cmucal_vclk_v920_evt0_clkcmu_dpuf1_noc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_DPUF2_NOCP, cmucal_vclk_v920_evt0_div_clk_dpuf2_nocp_lut, cmucal_vclk_v920_evt0_div_clk_dpuf2_nocp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_GNPU1_NOCP, cmucal_vclk_v920_evt0_div_clk_gnpu1_nocp_lut, cmucal_vclk_v920_evt0_div_clk_gnpu1_nocp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI00_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi00_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi00_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_PERIC0_IP, cmucal_vclk_v920_evt0_clkcmu_peric0_ip_lut, cmucal_vclk_v920_evt0_clkcmu_peric0_ip, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI01_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi01_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi01_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI02_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi02_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi02_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI03_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi03_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi03_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI04_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi04_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi04_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI05_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi05_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi05_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI06_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi06_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi06_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI07_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi07_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi07_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC0_USI08_USI, cmucal_vclk_v920_evt0_div_clk_peric0_usi08_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric0_usi08_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI09_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi09_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi09_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_CLKCMU_PERIC1_IP, cmucal_vclk_v920_evt0_clkcmu_peric1_ip_lut, cmucal_vclk_v920_evt0_clkcmu_peric1_ip, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI10_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi10_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi10_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI11_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi11_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi11_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI12_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi12_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi12_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI13_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi13_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi13_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI14_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi14_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi14_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI15_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi15_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi15_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI16_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi16_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi16_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_PERIC1_USI17_USI, cmucal_vclk_v920_evt0_div_clk_peric1_usi17_usi_lut, cmucal_vclk_v920_evt0_div_clk_peric1_usi17_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_SFI_PLLCLKOUT, cmucal_vclk_v920_evt0_mux_clk_sfi_pllclkout_lut, cmucal_vclk_v920_evt0_mux_clk_sfi_pllclkout, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_SFI_USI20, cmucal_vclk_v920_evt0_mux_clk_sfi_usi20_lut, cmucal_vclk_v920_evt0_mux_clk_sfi_usi20, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_SFI_USI19, cmucal_vclk_v920_evt0_mux_clk_sfi_usi19_lut, cmucal_vclk_v920_evt0_mux_clk_sfi_usi19, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_DIV_CLK_SFI_CPU_CNTCLK, cmucal_vclk_v920_evt0_div_clk_sfi_cpu_cntclk_lut, cmucal_vclk_v920_evt0_div_clk_sfi_cpu_cntclk, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_SFI_USI21, cmucal_vclk_v920_evt0_mux_clk_sfi_usi21_lut, cmucal_vclk_v920_evt0_mux_clk_sfi_usi21, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_SFI_CAN0, cmucal_vclk_v920_evt0_mux_clk_sfi_can0_lut, cmucal_vclk_v920_evt0_mux_clk_sfi_can0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_SFI_CAN1, cmucal_vclk_v920_evt0_mux_clk_sfi_can1_lut, cmucal_vclk_v920_evt0_mux_clk_sfi_can1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_CPUCL0_CORE, cmucal_vclk_v920_evt0_mux_clk_cpucl0_core_lut, cmucal_vclk_v920_evt0_mux_clk_cpucl0_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_CPUCL1_CORE, cmucal_vclk_v920_evt0_mux_clk_cpucl1_core_lut, cmucal_vclk_v920_evt0_mux_clk_cpucl1_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_MUX_CLK_CPUCL2_CORE, cmucal_vclk_v920_evt0_mux_clk_cpucl2_core_lut, cmucal_vclk_v920_evt0_mux_clk_cpucl2_core, NULL, NULL),

/* COMMON VCLK*/
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_CMU, cmucal_vclk_v920_evt0_blk_cmu_lut, cmucal_vclk_v920_evt0_blk_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_G3D, cmucal_vclk_v920_evt0_blk_g3d_lut, cmucal_vclk_v920_evt0_blk_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_HSI2, cmucal_vclk_v920_evt0_blk_hsi2_lut, cmucal_vclk_v920_evt0_blk_hsi2, NULL, v920_evt0_switch_blk_hsi2),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_MIF1, cmucal_vclk_v920_evt0_blk_mif1_lut, cmucal_vclk_v920_evt0_blk_mif1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_MIF2, cmucal_vclk_v920_evt0_blk_mif2_lut, cmucal_vclk_v920_evt0_blk_mif2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_MIF3, cmucal_vclk_v920_evt0_blk_mif3_lut, cmucal_vclk_v920_evt0_blk_mif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_APM, cmucal_vclk_v920_evt0_blk_apm_lut, cmucal_vclk_v920_evt0_blk_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_AUD, cmucal_vclk_v920_evt0_blk_aud_lut, cmucal_vclk_v920_evt0_blk_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_CPUCL0, cmucal_vclk_v920_evt0_blk_cpucl0_lut, cmucal_vclk_v920_evt0_blk_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_CPUCL1, cmucal_vclk_v920_evt0_blk_cpucl1_lut, cmucal_vclk_v920_evt0_blk_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_CPUCL2, cmucal_vclk_v920_evt0_blk_cpucl2_lut, cmucal_vclk_v920_evt0_blk_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_HSI1, cmucal_vclk_v920_evt0_blk_hsi1_lut, cmucal_vclk_v920_evt0_blk_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_PERIC0, cmucal_vclk_v920_evt0_blk_peric0_lut, cmucal_vclk_v920_evt0_blk_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_PERIC1, cmucal_vclk_v920_evt0_blk_peric1_lut, cmucal_vclk_v920_evt0_blk_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_S2D, cmucal_vclk_v920_evt0_blk_s2d_lut, cmucal_vclk_v920_evt0_blk_s2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_SFI, cmucal_vclk_v920_evt0_blk_sfi_lut, cmucal_vclk_v920_evt0_blk_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_ACC, cmucal_vclk_v920_evt0_blk_acc_lut, cmucal_vclk_v920_evt0_blk_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_DNC, cmucal_vclk_v920_evt0_blk_dnc_lut, cmucal_vclk_v920_evt0_blk_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_DPUB, cmucal_vclk_v920_evt0_blk_dpub_lut, cmucal_vclk_v920_evt0_blk_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_DPUF, cmucal_vclk_v920_evt0_blk_dpuf_lut, cmucal_vclk_v920_evt0_blk_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_DSP, cmucal_vclk_v920_evt0_blk_dsp_lut, cmucal_vclk_v920_evt0_blk_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_GNPU, cmucal_vclk_v920_evt0_blk_gnpu_lut, cmucal_vclk_v920_evt0_blk_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_HSI0, cmucal_vclk_v920_evt0_blk_hsi0_lut, cmucal_vclk_v920_evt0_blk_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_ISP, cmucal_vclk_v920_evt0_blk_isp_lut, cmucal_vclk_v920_evt0_blk_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_M2M, cmucal_vclk_v920_evt0_blk_m2m_lut, cmucal_vclk_v920_evt0_blk_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_MFC, cmucal_vclk_v920_evt0_blk_mfc_lut, cmucal_vclk_v920_evt0_blk_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_MFD, cmucal_vclk_v920_evt0_blk_mfd_lut, cmucal_vclk_v920_evt0_blk_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_MISC, cmucal_vclk_v920_evt0_blk_misc_lut, cmucal_vclk_v920_evt0_blk_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_NOCL0, cmucal_vclk_v920_evt0_blk_nocl0_lut, cmucal_vclk_v920_evt0_blk_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_NOCL1, cmucal_vclk_v920_evt0_blk_nocl1_lut, cmucal_vclk_v920_evt0_blk_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_NOCL2, cmucal_vclk_v920_evt0_blk_nocl2_lut, cmucal_vclk_v920_evt0_blk_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_SDMA, cmucal_vclk_v920_evt0_blk_sdma_lut, cmucal_vclk_v920_evt0_blk_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_SNW, cmucal_vclk_v920_evt0_blk_snw_lut, cmucal_vclk_v920_evt0_blk_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_SSP, cmucal_vclk_v920_evt0_blk_ssp_lut, cmucal_vclk_v920_evt0_blk_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_BLK_TAA, cmucal_vclk_v920_evt0_blk_taa_lut, cmucal_vclk_v920_evt0_blk_taa, NULL, NULL),

/* GATE VCLK*/
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_ACC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D0_ACC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_ACC, NULL, cmucal_vclk_v920_evt0_ip_sysreg_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_ACC_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_acc_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_ACC, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_ACC_TAA, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_acc_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_ACC, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D_ORBMCH, NULL, cmucal_vclk_v920_evt0_ip_qe_d_orbmch, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF0_ISPPRE, NULL, cmucal_vclk_v920_evt0_ip_qe_votf0_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_ACC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APB_ASYNC_ISPPRE0, NULL, cmucal_vclk_v920_evt0_ip_apb_async_isppre0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ORBMCH, NULL, cmucal_vclk_v920_evt0_ip_orbmch, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_D_ISPPRE, NULL, cmucal_vclk_v920_evt0_ip_vgen_d_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ACC_CMU_ACC, NULL, cmucal_vclk_v920_evt0_ip_acc_cmu_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D1_ACC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_VOTF_ISPPRE, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_votf_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_D_ORBMCH, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_d_orbmch, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_ACC, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CSIS_LINK_MUX3X6, NULL, cmucal_vclk_v920_evt0_ip_csis_link_mux3x6, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU1_ACC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D_ISPPRE, NULL, cmucal_vclk_v920_evt0_ip_qe_d_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ISPPRE, NULL, cmucal_vclk_v920_evt0_ip_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF1_ISPPRE, NULL, cmucal_vclk_v920_evt0_ip_qe_votf1_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF2_ISPPRE, NULL, cmucal_vclk_v920_evt0_ip_qe_votf2_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF3_ISPPRE, NULL, cmucal_vclk_v920_evt0_ip_qe_votf3_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_ACC, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_ACC, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_ACC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SENSOR_VSYNC, NULL, cmucal_vclk_v920_evt0_ip_sensor_vsync, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT_APM, NULL, cmucal_vclk_v920_evt0_ip_wdt_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_APM, NULL, cmucal_vclk_v920_evt0_ip_sysreg_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_APM_AP, NULL, cmucal_vclk_v920_evt0_ip_mailbox_apm_ap, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBIF_PMU_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_apbif_pmu_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_INTMEM_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_intmem_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_LG_SCAN2DRAM, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_lg_scan2dram, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PMU_INTR_GEN, NULL, cmucal_vclk_v920_evt0_ip_pmu_intr_gen, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_DP_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_xiu_dp_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APM_CMU_APM, NULL, cmucal_vclk_v920_evt0_ip_apm_cmu_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GREBE_APM, NULL, cmucal_vclk_v920_evt0_ip_grebe_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBIF_GPIO_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_apbif_gpio_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBIF_TOP_RTC, NULL, cmucal_vclk_v920_evt0_ip_apbif_top_rtc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GREBE_DBGCORE, NULL, cmucal_vclk_v920_evt0_ip_grebe_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_APM, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP_DBGCORE, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_G_DBGCORE, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_g_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBIF_RTC, NULL, cmucal_vclk_v920_evt0_ip_apbif_rtc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ROM_CRC32_HOST, NULL, cmucal_vclk_v920_evt0_ip_rom_crc32_host, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_APM_SFI0, NULL, cmucal_vclk_v920_evt0_ip_mailbox_apm_sfi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_APM_SFI1, NULL, cmucal_vclk_v920_evt0_ip_mailbox_apm_sfi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT_DBGCORE, NULL, cmucal_vclk_v920_evt0_ip_wdt_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SPMI_MASTER_PMIC, NULL, cmucal_vclk_v920_evt0_ip_spmi_master_pmic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ROM_CRC32_HCU, NULL, cmucal_vclk_v920_evt0_ip_rom_crc32_hcu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_HW_SCANDUMP_CLKSTOP_CTRL, NULL, cmucal_vclk_v920_evt0_ip_hw_scandump_clkstop_ctrl, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APM_DTA, NULL, cmucal_vclk_v920_evt0_ip_apm_dta, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_APM, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SPC_APM, NULL, cmucal_vclk_v920_evt0_ip_spc_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBIF_INTCOMB_VGPIO2PMU, NULL, cmucal_vclk_v920_evt0_ip_apbif_intcomb_vgpio2pmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBIF_INTCOMB_VGPIO2APM, NULL, cmucal_vclk_v920_evt0_ip_apbif_intcomb_vgpio2apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBIF_INTCOMB_VGPIO2AP, NULL, cmucal_vclk_v920_evt0_ip_apbif_intcomb_vgpio2ap, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_xiu_d_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AUD_CMU_AUD, NULL, cmucal_vclk_v920_evt0_ip_aud_cmu_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D_AUD, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D_AUD, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_AUD, NULL, cmucal_vclk_v920_evt0_ip_sysreg_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ABOX, NULL, cmucal_vclk_v920_evt0_ip_abox, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_AUD0, NULL, cmucal_vclk_v920_evt0_ip_gpio_aud0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AXI_US_32TO128, NULL, cmucal_vclk_v920_evt0_ip_axi_us_32to128, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_AUD, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT_AUD0, NULL, cmucal_vclk_v920_evt0_ip_wdt_aud0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DFTMUX_AUD0, NULL, cmucal_vclk_v920_evt0_ip_dftmux_aud0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_AUD, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AD_APB_SMMU_AUD_S1_NS, NULL, cmucal_vclk_v920_evt0_ip_ad_apb_smmu_aud_s1_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT_AUD1, NULL, cmucal_vclk_v920_evt0_ip_wdt_aud1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT_AUD2, NULL, cmucal_vclk_v920_evt0_ip_wdt_aud2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_AUD, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IP_AUD, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IP_AUD, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ip_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D_AUD, NULL, cmucal_vclk_v920_evt0_ip_xiu_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_AUD, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_AUD1, NULL, cmucal_vclk_v920_evt0_ip_gpio_aud1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_AUD0, NULL, cmucal_vclk_v920_evt0_ip_vgen_aud0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_AUD1, NULL, cmucal_vclk_v920_evt0_ip_vgen_aud1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_AUD, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON_PLL_AUD, NULL, cmucal_vclk_v920_evt0_ip_clkmon_pll_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON_PLL_AVB, NULL, cmucal_vclk_v920_evt0_ip_clkmon_pll_avb, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADD_CH_CLK, NULL, cmucal_vclk_v920_evt0_ip_add_ch_clk, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_CMU, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_CMU, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFR_APBIF_CMU, NULL, cmucal_vclk_v920_evt0_ip_sfr_apbif_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON0_PLL_SHARED, NULL, cmucal_vclk_v920_evt0_ip_clkmon0_pll_shared, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON1_PLL_SHARED, NULL, cmucal_vclk_v920_evt0_ip_clkmon1_pll_shared, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_sysreg_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CSSYS, NULL, cmucal_vclk_v920_evt0_ip_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_T_BDU, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_t_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_IT0_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_it0_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT0_CPUCL1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt0_cpucl1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_IT1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_it1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT1_CPUCL1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt1_cpucl1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_IT2_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_it2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_CHI_SI_D_CLUSTER0, NULL, cmucal_vclk_v920_evt0_ip_lh_chi_si_d_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_IT0_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_it0_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_IT1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_it1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_IT2_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_it2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_IT3_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_it3_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CPUCL0_CMU_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_cpucl0_cmu_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLUSTER0, NULL, cmucal_vclk_v920_evt0_ip_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IG_CSSYS, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ig_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IG_CSSYS, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ig_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_P_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_xiu_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_DP_CSSYS, NULL, cmucal_vclk_v920_evt0_ip_xiu_dp_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_G_CSSYS, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_g_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APB_ASYNC_P_CSSYS_0, NULL, cmucal_vclk_v920_evt0_ip_apb_async_p_cssys_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IG_ETR, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ig_etr, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IG_ETR, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ig_etr, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BPS_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_bps_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT2_CPUCL1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt2_cpucl1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT3_CPUCL1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt3_cpucl1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT0_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT1_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT2_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT3_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADM_APB_G_CLUSTER0, NULL, cmucal_vclk_v920_evt0_ip_adm_apb_g_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IG_STM, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ig_stm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT1_CPUCL2_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt1_cpucl2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_LT0_CPUCL2_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_lt0_cpucl2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CPUCL0_CON, NULL, cmucal_vclk_v920_evt0_ip_cpucl0_con, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_IRI_GIC_CLUSTER0, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_iri_gic_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_ICC_CLUSTER0_GIC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_icc_cluster0_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IG_STM, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ig_stm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_G_DBGCORE, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_g_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_CLUSTER0_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cluster0_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_IT3_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_it3_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON_PLL_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_clkmon_pll_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SECJTAG, NULL, cmucal_vclk_v920_evt0_ip_secjtag, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QOS_OVERR_D_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_qos_overr_d_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLUSTER1, NULL, cmucal_vclk_v920_evt0_ip_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CPUCL1_CMU_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_cpucl1_cmu_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_sysreg_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT3_CPUCL1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt3_cpucl1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT2_CPUCL1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt2_cpucl1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT1_CPUCL1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt1_cpucl1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT0_CPUCL1_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt0_cpucl1_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_CHI_SI_D_CLUSTER1, NULL, cmucal_vclk_v920_evt0_ip_lh_chi_si_d_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADM_APB_G_CLUSTER1, NULL, cmucal_vclk_v920_evt0_ip_adm_apb_g_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_ppmu_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CPUCL1_CON, NULL, cmucal_vclk_v920_evt0_ip_cpucl1_con, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_IRI_GIC_CLUSTER1, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_iri_gic_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_ICC_CLUSTER1_GIC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_icc_cluster1_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_CLUSTER1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cluster1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON_PLL_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_clkmon_pll_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QOS_OVERR_D_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_qos_overr_d_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLUSTER2, NULL, cmucal_vclk_v920_evt0_ip_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CPUCL2_CMU_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_cpucl2_cmu_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADM_APB_G_CLUSTER2, NULL, cmucal_vclk_v920_evt0_ip_adm_apb_g_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_ppmu_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CPUCL2_CON, NULL, cmucal_vclk_v920_evt0_ip_cpucl2_con, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_IRI_GIC_CLUSTER2, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_iri_gic_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_ICC_CLUSTER2_GIC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_icc_cluster2_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_CLUSTER2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cluster2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_CHI_SI_D_CLUSTER2, NULL, cmucal_vclk_v920_evt0_ip_lh_chi_si_d_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT0_CPUCL2_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt0_cpucl2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT1_CPUCL2_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt1_cpucl2_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_sysreg_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON_PLL_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_clkmon_pll_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QOS_OVERR_D_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_qos_overr_d_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DNC_CMU_DNC, NULL, cmucal_vclk_v920_evt0_ip_dnc_cmu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_DNC, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_RQ_GNPU0_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_rq_gnpu0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD6_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld6_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD3_MMU_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld3_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD7_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld7_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_CTRL_DNC_GNPU0, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_ctrl_dnc_gnpu0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_CTRL_DNC_GNPU1, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_ctrl_dnc_gnpu1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD0_DNC_GNPU0, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld0_dnc_gnpu0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD1_DNC_GNPU1, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld1_dnc_gnpu1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD1_DNC_GNPU0, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld1_dnc_gnpu0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IP_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD2_MMU_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld2_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_DNC, NULL, cmucal_vclk_v920_evt0_ip_sysreg_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TREX_D_DNC, NULL, cmucal_vclk_v920_evt0_ip_trex_d_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_LD_CACHE_DSP_DNC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_ld_cache_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD0_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld0_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD1_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld1_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD2_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld2_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD3_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld3_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD4_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld4_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD5_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld5_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_SFR_DSP_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_sfr_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_SDMA, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_SDMA, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D2_SDMA, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d2_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D3_SDMA, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d3_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D4_IPDNC, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d4_ipdnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IP_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ip_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_DNC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_CTRL_DNC_SDMA, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_ctrl_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_SFR_DNC_DSP, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_sfr_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_DMA_DNC_DSP, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_dma_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_RQ_GNPU1_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_rq_gnpu1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_LD_CMDQ_GNPU0_DNC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_ld_cmdq_gnpu0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_IP_DNC, NULL, cmucal_vclk_v920_evt0_ip_ip_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_SYSMMU_S0_DNC_S1_NS, NULL, cmucal_vclk_v920_evt0_ip_as_apb_sysmmu_s0_dnc_s1_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_SHMEM_DSP_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_shmem_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_SHMEM_GNPU0_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_shmem_gnpu0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_SHMEM_GNPU1_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_shmem_gnpu1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_DNC, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_LP_DNC_DSP, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_lp_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_LP_DNC_SDMA, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_lp_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_LP_DNC_GNPU1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_lp_dnc_gnpu1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADM_DAP_DNC, NULL, cmucal_vclk_v920_evt0_ip_adm_dap_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_DNC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU1_DNC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU2_DNC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu2_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU3_DNC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu3_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_PMMU0_DNC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_LD_CMDQ_GNPU1_DNC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_ld_cmdq_gnpu1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD0_DNC_GNPU1, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld0_dnc_gnpu1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD1_MMU_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld1_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD0_MMU_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld0_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_LP_DNC_GNPU0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_lp_dnc_gnpu0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D0_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D1_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D2_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d2_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D3_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d3_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_DNC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_DNC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D_DNC, NULL, cmucal_vclk_v920_evt0_ip_xiu_d_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BAAW_P_DNC, NULL, cmucal_vclk_v920_evt0_ip_baaw_p_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_LG_DNC_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_lg_dnc_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_DPTX, NULL, cmucal_vclk_v920_evt0_ip_sysreg_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_DPTX, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_DPTX, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DPTX_CMU_DPTX, NULL, cmucal_vclk_v920_evt0_ip_dptx_cmu_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DP_LINK0, NULL, cmucal_vclk_v920_evt0_ip_dp_link0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DP_LINK1, NULL, cmucal_vclk_v920_evt0_ip_dp_link1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST0, NULL, cmucal_vclk_v920_evt0_ip_dwc_usbc31dptxphy_upcs_x4_ns_x1_x4_pipe_wrap_inst0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DWC_USBC31DPTXPHY_UPCS_X4_NS_X1_X4_PIPE_WRAP_INST1, NULL, cmucal_vclk_v920_evt0_ip_dwc_usbc31dptxphy_upcs_x4_ns_x1_x4_pipe_wrap_inst1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DPUB_CMU_DPUB, NULL, cmucal_vclk_v920_evt0_ip_dpub_cmu_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_DPUB, NULL, cmucal_vclk_v920_evt0_ip_sysreg_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_DPUB, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_DPUB, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AD_APB_DECON_MAIN_DPUB0, NULL, cmucal_vclk_v920_evt0_ip_ad_apb_decon_main_dpub0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DPUB, NULL, cmucal_vclk_v920_evt0_ip_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_DPUB, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DPUF_CMU_DPUF, NULL, cmucal_vclk_v920_evt0_ip_dpuf_cmu_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_DPUF, NULL, cmucal_vclk_v920_evt0_ip_sysreg_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_PMMU1_DPUF, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu1_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_PMMU0_DPUF, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU1_DPUF, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_DPUF, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_DPUF1, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_DPUF0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_dpuf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_DPUF1, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_DPUF0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_dpuf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_DPUF, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D0_DPUF, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D1_DPUF, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_DPUF, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DPUF, NULL, cmucal_vclk_v920_evt0_ip_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_DPUF, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_DPUF, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AD_APB_DPUF0_DMA, NULL, cmucal_vclk_v920_evt0_ip_ad_apb_dpuf0_dma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_DPUF, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_DPUF, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_DPUF, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_dpuf, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DPUF1_CMU_DPUF1, NULL, cmucal_vclk_v920_evt0_ip_dpuf1_cmu_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DPUF2_CMU_DPUF2, NULL, cmucal_vclk_v920_evt0_ip_dpuf2_cmu_dpuf2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DSP_CMU_DSP, NULL, cmucal_vclk_v920_evt0_ip_dsp_cmu_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_DSP, NULL, cmucal_vclk_v920_evt0_ip_sysreg_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_SFR_DNC_DSP, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_sfr_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_DMA_DNC_DSP, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_dma_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_LD_SDMA_DSP, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_ld_sdma_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_LP_DNC_DSP, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_lp_dnc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_LD_CACHE_DSP_DNC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_ld_cache_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_SFR_DSP_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_sfr_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_SHMEM_DSP_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_shmem_dsp_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_DSP, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_IP_DSP, NULL, cmucal_vclk_v920_evt0_ip_ip_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_G3D_CMU_G3D, NULL, cmucal_vclk_v920_evt0_ip_g3d_cmu_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_G3D, NULL, cmucal_vclk_v920_evt0_ip_sysreg_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IP_G3D, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ip_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_G3D, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_G3D, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BG3D_PWRCTL, NULL, cmucal_vclk_v920_evt0_ip_bg3d_pwrctl, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADM_DAP_G_G3D, NULL, cmucal_vclk_v920_evt0_ip_adm_dap_g_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPU, NULL, cmucal_vclk_v920_evt0_ip_gpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ASB_G3D, NULL, cmucal_vclk_v920_evt0_ip_asb_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GNPU_CMU_GNPU, NULL, cmucal_vclk_v920_evt0_ip_gnpu_cmu_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_LP_DNC_GNPU, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_lp_dnc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_GNPU, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD0_DNC_GNPU, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld0_dnc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_CTRL_DNC_GNPU, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_ctrl_dnc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_LD_CMDQ_GNPU_DNC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_ld_cmdq_gnpu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_SHMEM_GNPU_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_shmem_gnpu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_GNPU, NULL, cmucal_vclk_v920_evt0_ip_sysreg_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD_RQ_GNPU_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld_rq_gnpu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_IP_NPUCORE, NULL, cmucal_vclk_v920_evt0_ip_ip_npucore, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD1_DNC_GNPU, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld1_dnc_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_GNPU, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_GNPU, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_gnpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GNPU1_CMU_GNPU1, NULL, cmucal_vclk_v920_evt0_ip_gnpu1_cmu_gnpu1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_HSI0_CMU_HSI0, NULL, cmucal_vclk_v920_evt0_ip_hsi0_cmu_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_SI_D0_HSI0, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_si_d0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_HSI0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_HSI0, NULL, cmucal_vclk_v920_evt0_ip_gpio_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_HSI0, NULL, cmucal_vclk_v920_evt0_ip_sysreg_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_HSI0, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_HSI0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_P0_HSI0, NULL, cmucal_vclk_v920_evt0_ip_xiu_p0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PCIE_GEN5_2L, NULL, cmucal_vclk_v920_evt0_ip_pcie_gen5_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PCIE_IA_GEN5A_2L, NULL, cmucal_vclk_v920_evt0_ip_pcie_ia_gen5a_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PCIE_IA_GEN5B_2L, NULL, cmucal_vclk_v920_evt0_ip_pcie_ia_gen5b_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_HSI0_0, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_hsi0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_HSI0, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_HSI0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PCIE_IA_GEN5A_4L, NULL, cmucal_vclk_v920_evt0_ip_pcie_ia_gen5a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PCIE_IA_GEN5B_4L, NULL, cmucal_vclk_v920_evt0_ip_pcie_ia_gen5b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_SI_D1_HSI0, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_si_d1_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PCIE_GEN5_4L, NULL, cmucal_vclk_v920_evt0_ip_pcie_gen5_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_HSI0_1, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_hsi0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PCIE_GEN5A_2L, NULL, cmucal_vclk_v920_evt0_ip_vgen_pcie_gen5a_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PCIE_GEN5A_4L, NULL, cmucal_vclk_v920_evt0_ip_vgen_pcie_gen5a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PCIE_GEN5B_2L, NULL, cmucal_vclk_v920_evt0_ip_vgen_pcie_gen5b_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PCIE_GEN5B_4L, NULL, cmucal_vclk_v920_evt0_ip_vgen_pcie_gen5b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PCIE_GEN5A_2L, NULL, cmucal_vclk_v920_evt0_ip_qe_pcie_gen5a_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PCIE_GEN5A_4L, NULL, cmucal_vclk_v920_evt0_ip_qe_pcie_gen5a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PCIE_GEN5B_2L, NULL, cmucal_vclk_v920_evt0_ip_qe_pcie_gen5b_2l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PCIE_GEN5B_4L, NULL, cmucal_vclk_v920_evt0_ip_qe_pcie_gen5b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_HSI0, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU1_HSI0, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D2_HSI0, NULL, cmucal_vclk_v920_evt0_ip_xiu_d2_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_HSI0, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ASYNC_APB_PCIE_GEN5_4L_SUB_CTRL, NULL, cmucal_vclk_v920_evt0_ip_async_apb_pcie_gen5_4l_sub_ctrl, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_HSI1_CMU_HSI1, NULL, cmucal_vclk_v920_evt0_ip_hsi1_cmu_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MMC_CARD, NULL, cmucal_vclk_v920_evt0_ip_mmc_card, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_HSI1, NULL, cmucal_vclk_v920_evt0_ip_sysreg_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_HSI1, NULL, cmucal_vclk_v920_evt0_ip_gpio_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_SI_D_HSI1, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_si_d_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_HSI1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_HSI1, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_P_HSI1, NULL, cmucal_vclk_v920_evt0_ip_xiu_p_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_HSI1, NULL, cmucal_vclk_v920_evt0_ip_ppmu_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_HSI1, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_HSI1, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USB20DRD_0, NULL, cmucal_vclk_v920_evt0_ip_usb20drd_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USB20DRD_1, NULL, cmucal_vclk_v920_evt0_ip_usb20drd_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USB30DRD_0, NULL, cmucal_vclk_v920_evt0_ip_usb30drd_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USB20DRD_2, NULL, cmucal_vclk_v920_evt0_ip_usb20drd_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_HSI1, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_HSI1, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_HSI1, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ETHERNET0, NULL, cmucal_vclk_v920_evt0_ip_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ETHERNET1, NULL, cmucal_vclk_v920_evt0_ip_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_SI_D0_HSI2, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_si_d0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_UFS_EMBD0, NULL, cmucal_vclk_v920_evt0_ip_qe_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_UFS_EMBD1, NULL, cmucal_vclk_v920_evt0_ip_qe_ufs_embd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_HSI2, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_UFS_EMBD0, NULL, cmucal_vclk_v920_evt0_ip_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_UFS_EMBD1, NULL, cmucal_vclk_v920_evt0_ip_ufs_embd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_HSI2, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_UFS_EMBD0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_UFS_EMBD1, NULL, cmucal_vclk_v920_evt0_ip_ppmu_ufs_embd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_HSI2, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D2_HSI2, NULL, cmucal_vclk_v920_evt0_ip_xiu_d2_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_UFS_EMBD0, NULL, cmucal_vclk_v920_evt0_ip_vgen_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_UFS_EMBD1, NULL, cmucal_vclk_v920_evt0_ip_vgen_ufs_embd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D3_HSI2, NULL, cmucal_vclk_v920_evt0_ip_xiu_d3_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_SI_D1_HSI2, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_si_d1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_ETHERNET0, NULL, cmucal_vclk_v920_evt0_ip_vgen_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_ETHERNET1, NULL, cmucal_vclk_v920_evt0_ip_vgen_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_ETHERNET0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_ETHERNET1, NULL, cmucal_vclk_v920_evt0_ip_ppmu_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_ETHERNET0, NULL, cmucal_vclk_v920_evt0_ip_qe_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_ETHERNET1, NULL, cmucal_vclk_v920_evt0_ip_qe_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_HSI2, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_PMMU0_HSI2, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_HSI2, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ASYNCAPB_SYSMMU_S0_HSI2_NS, NULL, cmucal_vclk_v920_evt0_ip_asyncapb_sysmmu_s0_hsi2_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_HSI2, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_HSI2, NULL, cmucal_vclk_v920_evt0_ip_gpio_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_HSI2_CMU_HSI2, NULL, cmucal_vclk_v920_evt0_ip_hsi2_cmu_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_HSI2, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_HSI2, NULL, cmucal_vclk_v920_evt0_ip_sysreg_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON_PLL_ETH, NULL, cmucal_vclk_v920_evt0_ip_clkmon_pll_eth, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_HSI2UFS, NULL, cmucal_vclk_v920_evt0_ip_gpio_hsi2ufs, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_D_ISP, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_d_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_ISP, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_ISP, NULL, cmucal_vclk_v920_evt0_ip_sysreg_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_ISP, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_ISP, NULL, cmucal_vclk_v920_evt0_ip_ppmu_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D0_ISP, NULL, cmucal_vclk_v920_evt0_ip_qe_d0_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D1_ISP, NULL, cmucal_vclk_v920_evt0_ip_qe_d1_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D2_ISP, NULL, cmucal_vclk_v920_evt0_ip_qe_d2_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D3_ISP, NULL, cmucal_vclk_v920_evt0_ip_qe_d3_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ISP, NULL, cmucal_vclk_v920_evt0_ip_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D_ISP, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_ISP, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_ISP0, NULL, cmucal_vclk_v920_evt0_ip_as_apb_isp0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_TAA_ISP, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_taa_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_ISP_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_isp_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_ISP, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_ISP, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ISP_CMU_ISP, NULL, cmucal_vclk_v920_evt0_ip_isp_cmu_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_M2M_CMU_M2M, NULL, cmucal_vclk_v920_evt0_ip_m2m_cmu_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_M2M, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_M2M, NULL, cmucal_vclk_v920_evt0_ip_sysreg_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_M2M, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_M2M, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_M2M, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU1_M2M, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_M2M, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_M2M, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_M2M0, NULL, cmucal_vclk_v920_evt0_ip_as_apb_m2m0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_M2M, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_M2M, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_JPEG, NULL, cmucal_vclk_v920_evt0_ip_qe_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_M2M_D0, NULL, cmucal_vclk_v920_evt0_ip_qe_m2m_d0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_D_M2M, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_d_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_M2M_D0, NULL, cmucal_vclk_v920_evt0_ip_m2m_d0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_M2M_D1, NULL, cmucal_vclk_v920_evt0_ip_m2m_d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_JPEG, NULL, cmucal_vclk_v920_evt0_ip_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IP_JPEG, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IP_JPEG, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ip_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_SI_D0_M2M, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_si_d0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_SI_D1_M2M, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_si_d1_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_M2M_JPEG, NULL, cmucal_vclk_v920_evt0_ip_as_apb_m2m_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MFC_CMU_MFC, NULL, cmucal_vclk_v920_evt0_ip_mfc_cmu_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MFC, NULL, cmucal_vclk_v920_evt0_ip_sysreg_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D1_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_MFC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_MFC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU1_MFC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_MFC, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_MFC, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D2_WFD, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d2_wfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D_MFC, NULL, cmucal_vclk_v920_evt0_ip_xiu_d_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_WFD_NS, NULL, cmucal_vclk_v920_evt0_ip_as_apb_wfd_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MFC, NULL, cmucal_vclk_v920_evt0_ip_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WFD, NULL, cmucal_vclk_v920_evt0_ip_wfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_MFC, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_MI_IT_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_mi_it_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_MFC, NULL, cmucal_vclk_v920_evt0_ip_as_apb_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_MFC, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_OTF1_MFD_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf1_mfd_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_OTF3_MFD_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf3_mfd_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_OTF0_MFC_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_otf0_mfc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_OTF1_MFC_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_otf1_mfc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_OTF2_MFC_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_otf2_mfc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_OTF3_MFC_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_otf3_mfc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_ID_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_id_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_OTF2_MFD_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf2_mfd_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_OTF0_MFD_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf0_mfd_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_MFC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_MFC, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_IT_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_it_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D0_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_ID_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_id_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MFD_CMU_MFD, NULL, cmucal_vclk_v920_evt0_ip_mfd_cmu_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_MFD, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_MFD, NULL, cmucal_vclk_v920_evt0_ip_as_apb_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_MFD, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D0_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D1_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MFD, NULL, cmucal_vclk_v920_evt0_ip_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_MFD, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_MFD, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_MFD, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU1_MFD, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_MFD, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MFD, NULL, cmucal_vclk_v920_evt0_ip_sysreg_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_OTF0_MFD_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_otf0_mfd_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_OTF2_MFD_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_otf2_mfd_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_OTF1_MFD_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_otf1_mfd_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_OTF3_MFD_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_otf3_mfd_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_OTF0_MFC_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf0_mfc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_OTF1_MFC_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf1_mfc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_OTF2_MFC_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf2_mfc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_OTF3_MFC_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_otf3_mfc_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADM_APB_MFCMFD, NULL, cmucal_vclk_v920_evt0_ip_adm_apb_mfcmfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_MFD, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D_MFD, NULL, cmucal_vclk_v920_evt0_ip_xiu_d_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MIF_CMU_MIF, NULL, cmucal_vclk_v920_evt0_ip_mif_cmu_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DDRPHY0, NULL, cmucal_vclk_v920_evt0_ip_ddrphy0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MIF, NULL, cmucal_vclk_v920_evt0_ip_sysreg_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_MIF, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPC_DEBUG0, NULL, cmucal_vclk_v920_evt0_ip_ppc_debug0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SMC0, NULL, cmucal_vclk_v920_evt0_ip_smc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_PPC_DEBUG0, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ppc_debug0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_MIF, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SMC1, NULL, cmucal_vclk_v920_evt0_ip_smc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DDRPHY1, NULL, cmucal_vclk_v920_evt0_ip_ddrphy1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPC_DEBUG1, NULL, cmucal_vclk_v920_evt0_ip_ppc_debug1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_PPC_DEBUG1, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ppc_debug1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_MIF, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SPC_MIF, NULL, cmucal_vclk_v920_evt0_ip_spc_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SPMPU_MIF, NULL, cmucal_vclk_v920_evt0_ip_spmpu_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_SMC0, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_smc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_SMC1, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_smc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_DDRPHY0, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ddrphy0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_DDRPHY1, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ddrphy1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MIF1_CMU_MIF1, NULL, cmucal_vclk_v920_evt0_ip_mif1_cmu_mif1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DDRPHY_1, NULL, cmucal_vclk_v920_evt0_ip_apbbr_ddrphy_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DMC_1, NULL, cmucal_vclk_v920_evt0_ip_apbbr_dmc_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DMCTZ_1, NULL, cmucal_vclk_v920_evt0_ip_apbbr_dmctz_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AXI2APB_MIF1, NULL, cmucal_vclk_v920_evt0_ip_axi2apb_mif1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DDRPHY_1, NULL, cmucal_vclk_v920_evt0_ip_ddrphy_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DMC_1, NULL, cmucal_vclk_v920_evt0_ip_dmc_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LHM_AXI_P_MIF1, NULL, cmucal_vclk_v920_evt0_ip_lhm_axi_p_mif1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMUPPC_DEBUG1, NULL, cmucal_vclk_v920_evt0_ip_ppmuppc_debug1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMUPPC_DVFS1, NULL, cmucal_vclk_v920_evt0_ip_ppmuppc_dvfs1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MIF1, NULL, cmucal_vclk_v920_evt0_ip_sysreg_mif1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_PPMUPPC_DEBUG1, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_debug1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_PPMUPPC_DVFS1, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_dvfs1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DDRPHY2, NULL, cmucal_vclk_v920_evt0_ip_apbbr_ddrphy2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DMC2, NULL, cmucal_vclk_v920_evt0_ip_apbbr_dmc2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DMCTZ2, NULL, cmucal_vclk_v920_evt0_ip_apbbr_dmctz2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AXI2APB_MIF2, NULL, cmucal_vclk_v920_evt0_ip_axi2apb_mif2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DDRPHY2, NULL, cmucal_vclk_v920_evt0_ip_ddrphy2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DMC2, NULL, cmucal_vclk_v920_evt0_ip_dmc2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LHM_AXI_P_MIF2, NULL, cmucal_vclk_v920_evt0_ip_lhm_axi_p_mif2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMUPPC_DEBUG2, NULL, cmucal_vclk_v920_evt0_ip_ppmuppc_debug2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMUPPC_DVFS2, NULL, cmucal_vclk_v920_evt0_ip_ppmuppc_dvfs2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MIF2, NULL, cmucal_vclk_v920_evt0_ip_sysreg_mif2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_PPMUPPC_DEBUG2, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_debug2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_PPMUPPC_DVFS2, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_dvfs2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MIF2_CMU_MIF2, NULL, cmucal_vclk_v920_evt0_ip_mif2_cmu_mif2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DDRPHY3, NULL, cmucal_vclk_v920_evt0_ip_apbbr_ddrphy3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DMC3, NULL, cmucal_vclk_v920_evt0_ip_apbbr_dmc3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_APBBR_DMCTZ3, NULL, cmucal_vclk_v920_evt0_ip_apbbr_dmctz3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AXI2APB_MIF3, NULL, cmucal_vclk_v920_evt0_ip_axi2apb_mif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DDRPHY3, NULL, cmucal_vclk_v920_evt0_ip_ddrphy3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DMC3, NULL, cmucal_vclk_v920_evt0_ip_dmc3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LHM_AXI_P_MIF3, NULL, cmucal_vclk_v920_evt0_ip_lhm_axi_p_mif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMUPPC_DEBUG3, NULL, cmucal_vclk_v920_evt0_ip_ppmuppc_debug3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMUPPC_DVFS3, NULL, cmucal_vclk_v920_evt0_ip_ppmuppc_dvfs3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MIF3, NULL, cmucal_vclk_v920_evt0_ip_sysreg_mif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MIF3_CMU_MIF3, NULL, cmucal_vclk_v920_evt0_ip_mif3_cmu_mif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_PPMUPPC_DEBUG3, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_debug3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QCH_ADAPTER_PPMUPPC_DVFS3, NULL, cmucal_vclk_v920_evt0_ip_qch_adapter_ppmuppc_dvfs3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MISC_CMU_MISC, NULL, cmucal_vclk_v920_evt0_ip_misc_cmu_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D_MISC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PDMA0, NULL, cmucal_vclk_v920_evt0_ip_pdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PDMA1, NULL, cmucal_vclk_v920_evt0_ip_pdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PDMA2, NULL, cmucal_vclk_v920_evt0_ip_pdma2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PDMA3, NULL, cmucal_vclk_v920_evt0_ip_pdma3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PDMA4, NULL, cmucal_vclk_v920_evt0_ip_pdma4, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_MISC, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PDMA0, NULL, cmucal_vclk_v920_evt0_ip_qe_pdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PDMA1, NULL, cmucal_vclk_v920_evt0_ip_qe_pdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PDMA2, NULL, cmucal_vclk_v920_evt0_ip_qe_pdma2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PDMA3, NULL, cmucal_vclk_v920_evt0_ip_qe_pdma3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_PDMA4, NULL, cmucal_vclk_v920_evt0_ip_qe_pdma4, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_SPDMA0, NULL, cmucal_vclk_v920_evt0_ip_qe_spdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_SPDMA1, NULL, cmucal_vclk_v920_evt0_ip_qe_spdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_MISC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SPDMA0, NULL, cmucal_vclk_v920_evt0_ip_spdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SPDMA1, NULL, cmucal_vclk_v920_evt0_ip_spdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_MISC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MISC, NULL, cmucal_vclk_v920_evt0_ip_sysreg_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PDMA0, NULL, cmucal_vclk_v920_evt0_ip_vgen_pdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PDMA1, NULL, cmucal_vclk_v920_evt0_ip_vgen_pdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PDMA2, NULL, cmucal_vclk_v920_evt0_ip_vgen_pdma2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PDMA3, NULL, cmucal_vclk_v920_evt0_ip_vgen_pdma3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_PDMA4, NULL, cmucal_vclk_v920_evt0_ip_vgen_pdma4, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_SPDMA0, NULL, cmucal_vclk_v920_evt0_ip_vgen_spdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_SPDMA1, NULL, cmucal_vclk_v920_evt0_ip_vgen_spdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_MISC, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_MISC, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AD_APB_SYSMMU_S0_MISC, NULL, cmucal_vclk_v920_evt0_ip_ad_apb_sysmmu_s0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_MISC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MISC_1, NULL, cmucal_vclk_v920_evt0_ip_sysreg_misc_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_MISC_2, NULL, cmucal_vclk_v920_evt0_ip_sysreg_misc_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_MISC, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MCT0_MISC, NULL, cmucal_vclk_v920_evt0_ip_mct0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_OTP_CON_BIRA, NULL, cmucal_vclk_v920_evt0_ip_otp_con_bira, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_OTP_CON_BISR, NULL, cmucal_vclk_v920_evt0_ip_otp_con_bisr, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_OTP_CON_TOP, NULL, cmucal_vclk_v920_evt0_ip_otp_con_top, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT_CLUSTER0, NULL, cmucal_vclk_v920_evt0_ip_wdt_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT_CLUSTER1, NULL, cmucal_vclk_v920_evt0_ip_wdt_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_ICC_CLUSTER0_GIC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_icc_cluster0_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_ICC_CLUSTER1_GIC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_icc_cluster1_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_ICC_CLUSTER2_GIC, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_icc_cluster2_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_IRI_GIC_CLUSTER0, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_iri_gic_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_IRI_GIC_CLUSTER1, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_iri_gic_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_IRI_GIC_CLUSTER2, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_iri_gic_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GIC, NULL, cmucal_vclk_v920_evt0_ip_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_MISC_GIC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_misc_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_GIC, NULL, cmucal_vclk_v920_evt0_ip_qe_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_GIC, NULL, cmucal_vclk_v920_evt0_ip_vgen_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TMU_SUB0, NULL, cmucal_vclk_v920_evt0_ip_tmu_sub0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TMU_TOP, NULL, cmucal_vclk_v920_evt0_ip_tmu_top, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VOLMON_INT, NULL, cmucal_vclk_v920_evt0_ip_volmon_int, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_ID_MISC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_id_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_ID_MISC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_id_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TMU_SUB1, NULL, cmucal_vclk_v920_evt0_ip_tmu_sub1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADC_MISC, NULL, cmucal_vclk_v920_evt0_ip_adc_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_MISC, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_PMMU0_MISC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_MISC, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MCT1_MISC, NULL, cmucal_vclk_v920_evt0_ip_mct1_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_NOCL0_CMU_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_nocl0_cmu_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_sysreg_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE2AXI_DP0_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace2axi_dp0_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE2AXI_DP1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace2axi_dp1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPC_G_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppc_g_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TREX_P_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_trex_p_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_T_BDU, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_t_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BDU, NULL, cmucal_vclk_v920_evt0_ip_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_G3D, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D0_G3D, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d0_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D1_G3D, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d1_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D2_G3D, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d2_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D3_G3D, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d3_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TREX_D_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_trex_d_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_wow_d_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D2_G3D, NULL, cmucal_vclk_v920_evt0_ip_wow_d2_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D3_G3D, NULL, cmucal_vclk_v920_evt0_ip_wow_d3_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D0_TREX, NULL, cmucal_vclk_v920_evt0_ip_wow_d0_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D1_TREX, NULL, cmucal_vclk_v920_evt0_ip_wow_d1_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_G_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_g_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE_ASB_D0_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace_asb_d0_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE_ASB_D1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace_asb_d1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE_ASB_D2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace_asb_d2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE_ASB_D3_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace_asb_d3_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SCI, NULL, cmucal_vclk_v920_evt0_ip_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_CHI_MI_D_CLUSTER0, NULL, cmucal_vclk_v920_evt0_ip_lh_chi_mi_d_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_MIF0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mif0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE_ASB_D4_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace_asb_d4_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE_ASB_D5_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace_asb_d5_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE_ASB_D6_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace_asb_d6_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MPACE_ASB_D7_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_mpace_asb_d7_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D0_G3D, NULL, cmucal_vclk_v920_evt0_ip_wow_d0_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D1_G3D, NULL, cmucal_vclk_v920_evt0_ip_wow_d1_g3d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D2_TREX, NULL, cmucal_vclk_v920_evt0_ip_wow_d2_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D3_TREX, NULL, cmucal_vclk_v920_evt0_ip_wow_d3_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_G_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_g_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_wow_d_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_wow_d_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_CHI_MI_D_CLUSTER1, NULL, cmucal_vclk_v920_evt0_ip_lh_chi_mi_d_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_CHI_MI_D_CLUSTER2, NULL, cmucal_vclk_v920_evt0_ip_lh_chi_mi_d_cluster2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_MIF1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mif1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_MIF2, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mif2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_MIF3, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_PERIC0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_PERIC1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_MISC_GIC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_misc_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D2_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d2_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D3_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d3_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D4_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d4_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D5_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d5_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D6_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d6_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D7_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d7_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D0_SCI, NULL, cmucal_vclk_v920_evt0_ip_wow_d0_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D1_SCI, NULL, cmucal_vclk_v920_evt0_ip_wow_d1_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D2_SCI, NULL, cmucal_vclk_v920_evt0_ip_wow_d2_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D3_SCI, NULL, cmucal_vclk_v920_evt0_ip_wow_d3_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D4_SCI, NULL, cmucal_vclk_v920_evt0_ip_wow_d4_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D5_SCI, NULL, cmucal_vclk_v920_evt0_ip_wow_d5_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D6_SCI, NULL, cmucal_vclk_v920_evt0_ip_wow_d6_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D7_SCI, NULL, cmucal_vclk_v920_evt0_ip_wow_d7_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_CLUSTER0_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cluster0_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_CLUSTER1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cluster1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_CLUSTER2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_cluster2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_MISC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D0_NOCL1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d0_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D1_NOCL1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d1_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D2_NOCL1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d2_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D3_NOCL1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d3_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_P_NOCL0_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_p_nocl0_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_P_NOCL0_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_p_nocl0_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_TREX, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_TREX, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D2_TREX, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d2_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D3_TREX, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d3_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_DP0_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_dp0_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_DP1_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_dp1_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_P_CPUCL0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_P_CPUCL1, NULL, cmucal_vclk_v920_evt0_ip_ppmu_p_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_P_CPUCL2, NULL, cmucal_vclk_v920_evt0_ip_ppmu_p_cpucl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WOW_D_TREX_QURGENT, NULL, cmucal_vclk_v920_evt0_ip_wow_d_trex_qurgent, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_DP2_SCI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_dp2_sci, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_CMU, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_LG_DNC_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_lg_dnc_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_DP_TREX, NULL, cmucal_vclk_v920_evt0_ip_ppmu_dp_trex, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D0_NOCL2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d0_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D1_NOCL2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d1_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D2_NOCL2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d2_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D3_NOCL2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d3_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADM_APB_G_BDU, NULL, cmucal_vclk_v920_evt0_ip_adm_apb_g_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_NOCL1_CMU_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_nocl1_cmu_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_sysreg_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TREX_D_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_trex_d_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D0_HSI0, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d0_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D1_HSI0, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d1_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TREX_P_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_trex_p_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_G_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_g_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D_AUD, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_HSI2, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_G_CSSYS, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_g_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D_SSP, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_AUD, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_DNC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_SSP, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_HSI0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_hsi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D_G3D_PTW, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_g3d_ptw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CACHEAID_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_cacheaid_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_SFI, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D0_HSI2, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d0_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D1_HSI2, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d1_hsi2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D0_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D1_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D2_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d2_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_D3_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_d3_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_P_NOCL0_NOCL1, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_p_nocl0_nocl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D0_NOCL1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d0_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D2_NOCL1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d2_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D1_NOCL1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d1_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D3_NOCL1_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d3_nocl1_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_ALIVE, NULL, cmucal_vclk_v920_evt0_ip_ppmu_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_CSSYS, NULL, cmucal_vclk_v920_evt0_ip_ppmu_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_G3DMMU, NULL, cmucal_vclk_v920_evt0_ip_ppmu_g3dmmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_NOCL1_S0, NULL, cmucal_vclk_v920_evt0_ip_ppmu_nocl1_s0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_NOCL1_S1, NULL, cmucal_vclk_v920_evt0_ip_ppmu_nocl1_s1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_NOCL1_S2, NULL, cmucal_vclk_v920_evt0_ip_ppmu_nocl1_s2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_NOCL1_S3, NULL, cmucal_vclk_v920_evt0_ip_ppmu_nocl1_s3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TREX_D_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_trex_d_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_sysreg_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D_HSI1, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D0_DPUF2, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_dpuf2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D0_DPUF0, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_dpuf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D0_DPUF1, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D1_ACC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D0_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D1_DPUF2, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_dpuf2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D1_DPUF0, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_dpuf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D1_DPUF1, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D0_ACC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D1_MFC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D1_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D0_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D0_MFD, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d0_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D_ISP, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D_MISC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_misc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D_TAA, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_G_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_g_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TREX_P_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_trex_p_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_ACC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_DPUF1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dpuf1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_DPTX, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_DPUB, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dpub, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_DPUF0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dpuf0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_DPUF2, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_dpuf2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_HSI1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_hsi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_MFD, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_ISP, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_MFC, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_TAA, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_NOCL2_CMU_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_nocl2_cmu_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_SNW, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D1_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_D2_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_d2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D0_M2M, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d0_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_MI_D1_M2M, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_mi_d1_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_SI_P_M2M, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_si_p_m2m, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D2_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d2_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D3_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d3_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D0_NOCL2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d0_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D1_NOCL2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d1_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D2_NOCL2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d2_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_SI_D3_NOCL2_NOCL0, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_si_d3_nocl2_nocl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_TAXI_MI_P_NOCL0_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_lh_taxi_mi_p_nocl0_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CACHEAID_NOCL2, NULL, cmucal_vclk_v920_evt0_ip_cacheaid_nocl2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_PERIC0, NULL, cmucal_vclk_v920_evt0_ip_gpio_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_PERIC0, NULL, cmucal_vclk_v920_evt0_ip_sysreg_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PERIC0_CMU_PERIC0, NULL, cmucal_vclk_v920_evt0_ip_peric0_cmu_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_PERIC0, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_PERIC0, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI00_USI, NULL, cmucal_vclk_v920_evt0_ip_usi00_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI00_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi00_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI01_USI, NULL, cmucal_vclk_v920_evt0_ip_usi01_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI01_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi01_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI02_USI, NULL, cmucal_vclk_v920_evt0_ip_usi02_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI02_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi02_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI03_USI, NULL, cmucal_vclk_v920_evt0_ip_usi03_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI03_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi03_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI04_USI, NULL, cmucal_vclk_v920_evt0_ip_usi04_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI04_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi04_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI05_USI, NULL, cmucal_vclk_v920_evt0_ip_usi05_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI05_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi05_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI06_USI, NULL, cmucal_vclk_v920_evt0_ip_usi06_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI06_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi06_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI07_USI, NULL, cmucal_vclk_v920_evt0_ip_usi07_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI07_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi07_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI08_USI, NULL, cmucal_vclk_v920_evt0_ip_usi08_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI08_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi08_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_I3C0, NULL, cmucal_vclk_v920_evt0_ip_i3c0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_I3C1, NULL, cmucal_vclk_v920_evt0_ip_i3c1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_I3C2, NULL, cmucal_vclk_v920_evt0_ip_i3c2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_I3C3, NULL, cmucal_vclk_v920_evt0_ip_i3c3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PWM, NULL, cmucal_vclk_v920_evt0_ip_pwm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_PERIC1, NULL, cmucal_vclk_v920_evt0_ip_gpio_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_PERIC1, NULL, cmucal_vclk_v920_evt0_ip_sysreg_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PERIC1_CMU_PERIC1, NULL, cmucal_vclk_v920_evt0_ip_peric1_cmu_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_PERIC1, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_PERIC1, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI09_USI, NULL, cmucal_vclk_v920_evt0_ip_usi09_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI09_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi09_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI10_USI, NULL, cmucal_vclk_v920_evt0_ip_usi10_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI10_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi10_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI11_USI, NULL, cmucal_vclk_v920_evt0_ip_usi11_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI11_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi11_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI12_USI, NULL, cmucal_vclk_v920_evt0_ip_usi12_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI12_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi12_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI13_USI, NULL, cmucal_vclk_v920_evt0_ip_usi13_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI13_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi13_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI14_USI, NULL, cmucal_vclk_v920_evt0_ip_usi14_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI15_USI, NULL, cmucal_vclk_v920_evt0_ip_usi15_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI14_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi14_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI15_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi15_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI16_USI, NULL, cmucal_vclk_v920_evt0_ip_usi16_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI16_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi16_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI17_USI, NULL, cmucal_vclk_v920_evt0_ip_usi17_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI17_I2C, NULL, cmucal_vclk_v920_evt0_ip_usi17_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_I3C4, NULL, cmucal_vclk_v920_evt0_ip_i3c4, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_I3C5, NULL, cmucal_vclk_v920_evt0_ip_i3c5, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_I3C6, NULL, cmucal_vclk_v920_evt0_ip_i3c6, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_I3C7, NULL, cmucal_vclk_v920_evt0_ip_i3c7, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_S2D_CMU_S2D, NULL, cmucal_vclk_v920_evt0_ip_s2d_cmu_s2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BIS_S2D, NULL, cmucal_vclk_v920_evt0_ip_bis_s2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_LG_SCAN2DRAM, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_lg_scan2dram, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SDMA_CMU_SDMA, NULL, cmucal_vclk_v920_evt0_ip_sdma_cmu_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_SDMA, NULL, cmucal_vclk_v920_evt0_ip_sysreg_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD0_MMU_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld0_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD1_MMU_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld1_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD2_MMU_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld2_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD3_MMU_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld3_mmu_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD0_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld0_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD1_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld1_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD2_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld2_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD3_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld3_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD4_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld4_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD5_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld5_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD6_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld6_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_LD7_SDMA_DNC, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ld7_sdma_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_SDMA, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_LD_CTRL_DNC_SDMA, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ld_ctrl_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_LP_DNC_SDMA, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_lp_dnc_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_LD_SDMA_DSP, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_ld_sdma_dsp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_IP_SDMA, NULL, cmucal_vclk_v920_evt0_ip_ip_sdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI19_USI, NULL, cmucal_vclk_v920_evt0_ip_usi19_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON0, NULL, cmucal_vclk_v920_evt0_ip_clkmon0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI18_USI, NULL, cmucal_vclk_v920_evt0_ip_usi18_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI20_USI, NULL, cmucal_vclk_v920_evt0_ip_usi20_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SC_SFI, NULL, cmucal_vclk_v920_evt0_ip_sc_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_SFI, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_SFI0, NULL, cmucal_vclk_v920_evt0_ip_gpio_sfi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP4, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap4, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MCT0_SFI, NULL, cmucal_vclk_v920_evt0_ip_mct0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ROM_CRC32_SFI, NULL, cmucal_vclk_v920_evt0_ip_rom_crc32_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_SFI, NULL, cmucal_vclk_v920_evt0_ip_sysreg_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VOLMON, NULL, cmucal_vclk_v920_evt0_ip_volmon, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT0_SFI, NULL, cmucal_vclk_v920_evt0_ip_wdt0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_USI21_USI, NULL, cmucal_vclk_v920_evt0_ip_usi21_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_SFPC_SFI, NULL, cmucal_vclk_v920_evt0_ip_d_sfpc_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_INTMEM_SFI, NULL, cmucal_vclk_v920_evt0_ip_intmem_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ASYNCAPB_INTMEM, NULL, cmucal_vclk_v920_evt0_ip_asyncapb_intmem, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_SFI, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_DP_SFI, NULL, cmucal_vclk_v920_evt0_ip_xiu_dp_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XSPI, NULL, cmucal_vclk_v920_evt0_ip_xspi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT0_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT1_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT2_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ATB_SI_LT3_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_atb_si_lt3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_WDT1_SFI, NULL, cmucal_vclk_v920_evt0_ip_wdt1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BUSIF_CMU_SFI, NULL, cmucal_vclk_v920_evt0_ip_busif_cmu_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PMU_HANDSHAKE_BUS, NULL, cmucal_vclk_v920_evt0_ip_pmu_handshake_bus, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLUSTER_SFI, NULL, cmucal_vclk_v920_evt0_ip_cluster_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_P1_SFI, NULL, cmucal_vclk_v920_evt0_ip_xiu_p1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SBIST0, NULL, cmucal_vclk_v920_evt0_ip_sbist0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SBIST1, NULL, cmucal_vclk_v920_evt0_ip_sbist1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PMC, NULL, cmucal_vclk_v920_evt0_ip_pmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON1, NULL, cmucal_vclk_v920_evt0_ip_clkmon1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CLKMON2, NULL, cmucal_vclk_v920_evt0_ip_clkmon2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IP1_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ip1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IP1_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IDP0_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_idp0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IDP1_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_idp1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IP0_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ip0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IDP0_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_idp0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IDP1_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_idp1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IP0_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP0, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP1, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP7, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap7, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP2, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap2, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP3, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap3, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP6, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap6, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_APM, NULL, cmucal_vclk_v920_evt0_ip_mailbox_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_AP5, NULL, cmucal_vclk_v920_evt0_ip_mailbox_ap5, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_P0_SFI, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_p0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_P1_SFI, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_p1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_P2_SFI, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_p2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_P3_SFI, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_p3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_D_SFI, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_SFI, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU_SFI, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D_SFI, NULL, cmucal_vclk_v920_evt0_ip_xiu_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SFMPU_P4_SFI, NULL, cmucal_vclk_v920_evt0_ip_sfmpu_p4_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CAN_FD0, NULL, cmucal_vclk_v920_evt0_ip_can_fd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_CAN_FD1, NULL, cmucal_vclk_v920_evt0_ip_can_fd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MCT1_SFI, NULL, cmucal_vclk_v920_evt0_ip_mct1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BAAW_P_SFI, NULL, cmucal_vclk_v920_evt0_ip_baaw_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ASYNCAPB_CLUSTER_SFI_DEBUG, NULL, cmucal_vclk_v920_evt0_ip_asyncapb_cluster_sfi_debug, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D_SFI, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DMA_SFI, NULL, cmucal_vclk_v920_evt0_ip_dma_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_ABOX, NULL, cmucal_vclk_v920_evt0_ip_mailbox_abox, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_MAILBOX_SFI, NULL, cmucal_vclk_v920_evt0_ip_mailbox_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_DAPAPBAP_MUX_SFI, NULL, cmucal_vclk_v920_evt0_ip_dapapbap_mux_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ADM_DAP_G_SFI, NULL, cmucal_vclk_v920_evt0_ip_adm_dap_g_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TMU_SFI, NULL, cmucal_vclk_v920_evt0_ip_tmu_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_GPIO_SFI1, NULL, cmucal_vclk_v920_evt0_ip_gpio_sfi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_IP2_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_ip2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_IP2_SFI, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_ip2_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ASYNCAPB_FMU, NULL, cmucal_vclk_v920_evt0_ip_asyncapb_fmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_FMU, NULL, cmucal_vclk_v920_evt0_ip_fmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BAAW_D_SFI, NULL, cmucal_vclk_v920_evt0_ip_baaw_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SNW_CMU_SNW, NULL, cmucal_vclk_v920_evt0_ip_snw_cmu_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_SNW, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_SNW, NULL, cmucal_vclk_v920_evt0_ip_sysreg_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_D0_SNW, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_D1_SNW, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_d1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_D2_SNW, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_d2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D0_SNW, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D1_SNW, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SNW, NULL, cmucal_vclk_v920_evt0_ip_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D0_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D1_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_SNW, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_ISP_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_isp_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_ACC_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_acc_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_SNW_SNW0, NULL, cmucal_vclk_v920_evt0_ip_as_apb_snw_snw0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D0_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D1_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D2_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D3_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d3_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF0_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_votf0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D4_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d4_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D5_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d5_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D6_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d6_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D7_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d7_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF1_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_votf1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_D2_SNW, NULL, cmucal_vclk_v920_evt0_ip_ppmu_d2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D2_SNW, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D8_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d8_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D9_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d9_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D10_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d10_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D11_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_d11_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF2_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_votf2_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF4_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_votf4_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF3_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_votf3_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF5_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_votf5_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF6_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_votf6_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_VOTF7_SNW, NULL, cmucal_vclk_v920_evt0_ip_qe_votf7_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D3_SNW, NULL, cmucal_vclk_v920_evt0_ip_xiu_d3_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_SNW, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU1_SNW, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_SNW, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_PMMU0_SNW, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_pmmu0_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D4_SNW, NULL, cmucal_vclk_v920_evt0_ip_xiu_d4_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S1_SNW, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s1_snw, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SSP_CMU_SSP, NULL, cmucal_vclk_v920_evt0_ip_ssp_cmu_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_SSP, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_SSP, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_SSP, NULL, cmucal_vclk_v920_evt0_ip_sysreg_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_OTP_ADK_DESERIAL_SSP, NULL, cmucal_vclk_v920_evt0_ip_otp_adk_deserial_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SECURITYCONTROLLER, NULL, cmucal_vclk_v920_evt0_ip_securitycontroller, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_SSP, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_BAAW_SC, NULL, cmucal_vclk_v920_evt0_ip_baaw_sc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D0_SSP, NULL, cmucal_vclk_v920_evt0_ip_xiu_d0_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_ACEL_SI_D_SSP, NULL, cmucal_vclk_v920_evt0_ip_lh_acel_si_d_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_SSP, NULL, cmucal_vclk_v920_evt0_ip_ppmu_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_STRONG, NULL, cmucal_vclk_v920_evt0_ip_qe_strong, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_SC, NULL, cmucal_vclk_v920_evt0_ip_qe_sc, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_ASYNCAPB_SYSMMU, NULL, cmucal_vclk_v920_evt0_ip_asyncapb_sysmmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_SSP, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_SSP, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_MI_ID_STRONG, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_mi_id_strong, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_RTIC, NULL, cmucal_vclk_v920_evt0_ip_rtic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_RTIC, NULL, cmucal_vclk_v920_evt0_ip_qe_rtic, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_SSP, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_OTP_PUF_DESERIAL_SSP, NULL, cmucal_vclk_v920_evt0_ip_otp_puf_deserial_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_OTP_REK_DESERIAL_SSP, NULL, cmucal_vclk_v920_evt0_ip_otp_rek_deserial_ssp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_STRONG_CMU_STRONG, NULL, cmucal_vclk_v920_evt0_ip_strong_cmu_strong, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AXI_SI_D_TAA, NULL, cmucal_vclk_v920_evt0_ip_lh_axi_si_d_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SLH_AXI_MI_P_TAA, NULL, cmucal_vclk_v920_evt0_ip_slh_axi_mi_p_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSREG_TAA, NULL, cmucal_vclk_v920_evt0_ip_sysreg_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TAA_CMU_TAA, NULL, cmucal_vclk_v920_evt0_ip_taa_cmu_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_MI_L_ACC_TAA, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_mi_l_acc_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_LH_AST_SI_L_TAA_ISP, NULL, cmucal_vclk_v920_evt0_ip_lh_ast_si_l_taa_isp, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_D_TZPC_TAA, NULL, cmucal_vclk_v920_evt0_ip_d_tzpc_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_AS_APB_TAA0, NULL, cmucal_vclk_v920_evt0_ip_as_apb_taa0, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_PPMU_TAA, NULL, cmucal_vclk_v920_evt0_ip_ppmu_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_PMMU0_TAA, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_pmmu0_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_TAA, NULL, cmucal_vclk_v920_evt0_ip_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D0_TAA, NULL, cmucal_vclk_v920_evt0_ip_qe_d0_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D1_TAA, NULL, cmucal_vclk_v920_evt0_ip_qe_d1_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D2_TAA, NULL, cmucal_vclk_v920_evt0_ip_qe_d2_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_QE_D3_TAA, NULL, cmucal_vclk_v920_evt0_ip_qe_d3_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_VGEN_LITE_D_TAA, NULL, cmucal_vclk_v920_evt0_ip_vgen_lite_d_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_SYSMMU_S0_TAA, NULL, cmucal_vclk_v920_evt0_ip_sysmmu_s0_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_V920_EVT0_IP_XIU_D1_TAA, NULL, cmucal_vclk_v920_evt0_ip_xiu_d1_taa, NULL, NULL),
};

