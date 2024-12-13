// SPDX-License-Identifier: GPL-2.0
/*
 * Common Clock Framework support for exynosautov920 SoC.
 *
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include "../../../cmucal.h"
#include "cmucal-vclklut.h"


/* DVFS VCLK -> LUT Parameter List */
unsigned int v920_evt0_vdd_mif_od_lut_params[] = {
	7500000, 400000,
};
unsigned int v920_evt0_vdd_mif_nm_lut_params[] = {
	4266000, 400000,
};
unsigned int v920_evt0_vdd_cpucl0_sod_lut_params[] = {
	2300000, 2, 2, 2,
};
unsigned int v920_evt0_vdd_cpucl0_od_lut_params[] = {
	2000000, 1, 1, 7,
};
unsigned int v920_evt0_vdd_cpucl0_nm_lut_params[] = {
	1550000, 2, 2, 3,
};
unsigned int v920_evt0_vdd_cpucl1_sod_lut_params[] = {
	2, 2, 2, 2300000,
};
unsigned int v920_evt0_vdd_cpucl1_od_lut_params[] = {
	1, 1, 7, 2000000,
};
unsigned int v920_evt0_vdd_cpucl1_nm_lut_params[] = {
	2, 2, 3, 1550000,
};
unsigned int v920_evt0_vdd_sfi_od_lut_params[] = {
	5, 1200000, 2, 2, 2, 2, 3, 2, 2, 14, 14,
};
unsigned int v920_evt0_vdd_sfi_nm_lut_params[] = {
	3, 800000, 1, 1, 1, 1, 2, 1, 1, 9, 9,
};
unsigned int v920_evt0_vdd_aud_sod_lut_params[] = {
	740000, 14,
};
unsigned int v920_evt0_vdd_aud_od_lut_params[] = {
	738000, 9,
};
unsigned int v920_evt0_vdd_aud_nm_lut_params[] = {
	590000, 11,
};
unsigned int v920_evt0_vdd_int_cmu_od_lut_params[] = {
	0, 0, 0, 0, 5, 5, 0, 2, 0, 1, 0, 0, 3, 2, 2, 2, 0, 3, 0, 0, 0, 0, 0, 800000, 0, 0, 1, 0, 2, 6, 2, 1, 3, 2, 4, 1, 2, 1, 2, 1, 1, 2, 2,
};
unsigned int v920_evt0_vdd_int_cmu_nm_lut_params[] = {
	1, 2, 1, 4, 0, 0, 1, 3, 1, 2, 1, 4, 2, 3, 0, 3, 1, 0, 4, 1, 2, 2, 1, 618000, 9, 1, 1, 0, 2, 6, 2, 1, 3, 2, 4, 1, 2, 1, 2, 1, 1, 2, 2,
};
unsigned int v920_evt0_vdd_int_cmu_sod_lut_params[] = {
	0, 0, 0, 0, 5, 5, 1, 2, 0, 0, 0, 3, 2, 0, 2, 0, 0, 0, 5, 1, 0, 0, 0, 618000, 0, 0, 0, 4, 0, 0, 0, 5, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 5,
};
unsigned int v920_evt0_vdd_cpucl2_sod_lut_params[] = {
	4, 2300000, 2, 2,
};
unsigned int v920_evt0_vdd_cpucl2_od_lut_params[] = {
	7, 2000000, 1, 1,
};
unsigned int v920_evt0_vdd_cpucl2_nm_lut_params[] = {
	5, 1550000, 2, 2,
};

/* SPECIAL VCLK -> LUT Parameter List */
unsigned int v920_evt0_mux_clk_apm_timer_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_aud_uaif3_sod_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int v920_evt0_mux_clk_aud_uaif3_od_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int v920_evt0_mux_clk_aud_uaif3_nm_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int v920_evt0_mux_cmu_cmuref_sod_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_cmu_cmuref_ud_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_cpucl0_cmuref_sod_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_cpucl1_cmuref_sod_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_cpucl2_cmuref_sod_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_mif_cmuref_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_mif1_cmuref_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_mif2_cmuref_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_mif3_cmuref_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_nocl0_cmuref_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_nocl1_cmuref_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_nocl2_cmuref_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_clkcmu_dpub_dsim_sod_lut_params[] = {
	6, 0,
};
unsigned int v920_evt0_clkcmu_dpub_dsim_ud_lut_params[] = {
	0, 0,
};
unsigned int v920_evt0_div_clk_apm_spmi_nm_lut_params[] = {
	7, 1,
};
unsigned int v920_evt0_div_clk_aud_mclk_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_div_clk_aud_mclk_sod_lut_params[] = {
	0,
};
unsigned int v920_evt0_div_clk_aud_mclk_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_clk_aud_avb_eth_sod_lut_params[] = {
	9215, 1,
};
unsigned int v920_evt0_clk_cmu_pllclkout_ud_lut_params[] = {
	4, 1,
};
unsigned int v920_evt0_clk_add_ch_clk_sod_lut_params[] = {
	17,
};
unsigned int v920_evt0_clk_add_ch_clk_ud_lut_params[] = {
	24,
};
unsigned int v920_evt0_div_clk_cmu_nocp_ud_lut_params[] = {
	0, 0,
};
unsigned int v920_evt0_div_clk_cmu_nocp_sod_lut_params[] = {
	1, 0,
};
unsigned int v920_evt0_clkcmu_cis_mclk3_sod_lut_params[] = {
	3, 1,
};
unsigned int v920_evt0_clkcmu_cis_mclk3_ud_lut_params[] = {
	0, 0,
};
unsigned int v920_evt0_clkcmu_cis_mclk0_sod_lut_params[] = {
	3, 1,
};
unsigned int v920_evt0_clkcmu_cis_mclk0_ud_lut_params[] = {
	0, 0,
};
unsigned int v920_evt0_clkcmu_cis_mclk1_sod_lut_params[] = {
	3, 1,
};
unsigned int v920_evt0_clkcmu_cis_mclk1_ud_lut_params[] = {
	0, 0,
};
unsigned int v920_evt0_clkcmu_cis_mclk2_sod_lut_params[] = {
	3, 1,
};
unsigned int v920_evt0_clkcmu_cis_mclk2_ud_lut_params[] = {
	0, 0,
};
unsigned int v920_evt0_div_clk_cpucl0_shortstop_sod_lut_params[] = {
	1,
};
unsigned int v920_evt0_clkcmu_cpucl0_cluster_sod_lut_params[] = {
	0, 7,
};
unsigned int v920_evt0_clkcmu_cpucl0_cluster_ud_lut_params[] = {
	1, 4,
};
unsigned int v920_evt0_clkcmu_cpucl0_switch_ud_lut_params[] = {
	1, 0,
};
unsigned int v920_evt0_div_clk_cpucl1_shortstop_sod_lut_params[] = {
	1,
};
unsigned int v920_evt0_clkcmu_cpucl1_cluster_sod_lut_params[] = {
	0, 7,
};
unsigned int v920_evt0_clkcmu_cpucl1_cluster_ud_lut_params[] = {
	1, 4,
};
unsigned int v920_evt0_clkcmu_cpucl1_switch_ud_lut_params[] = {
	1, 0,
};
unsigned int v920_evt0_div_clk_cpucl2_shortstop_sod_lut_params[] = {
	1,
};
unsigned int v920_evt0_clkcmu_cpucl2_cluster_sod_lut_params[] = {
	0, 4,
};
unsigned int v920_evt0_clkcmu_cpucl2_cluster_ud_lut_params[] = {
	0, 0,
};
unsigned int v920_evt0_clkcmu_cpucl2_switch_ud_lut_params[] = {
	0, 0,
};
unsigned int v920_evt0_clkcmu_cpucl2_switch_sod_lut_params[] = {
	0, 2,
};
unsigned int v920_evt0_div_clk_dpuf1_nocp_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_div_clk_dpuf1_nocp_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_clkcmu_dpuf1_noc_sod_lut_params[] = {
	0,
};
unsigned int v920_evt0_clkcmu_dpuf1_noc_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_clkcmu_dpuf1_noc_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_clkcmu_dpuf1_noc_ud_lut_params[] = {
	0,
};
unsigned int v920_evt0_div_clk_dpuf2_nocp_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_div_clk_dpuf2_nocp_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_div_clk_gnpu1_nocp_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_div_clk_peric0_usi00_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_clkcmu_peric0_ip_sod_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_clkcmu_peric0_ip_ud_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric0_usi01_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric0_usi02_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric0_usi03_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric0_usi04_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric0_usi05_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric0_usi06_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric0_usi07_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric0_usi08_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi09_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_clkcmu_peric1_ip_sod_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_clkcmu_peric1_ip_ud_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi10_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi11_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi12_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi13_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi14_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi15_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi16_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_div_clk_peric1_usi17_usi_nm_lut_params[] = {
	0, 1,
};
unsigned int v920_evt0_mux_clk_sfi_pllclkout_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_pllclkout_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_usi20_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_usi20_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_usi19_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_usi19_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_div_clk_sfi_cpu_cntclk_od_lut_params[] = {
	3,
};
unsigned int v920_evt0_div_clk_sfi_cpu_cntclk_nm_lut_params[] = {
	3,
};
unsigned int v920_evt0_mux_clk_sfi_usi21_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_usi21_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_can0_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_can0_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_can1_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_sfi_can1_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_mux_clk_cpucl0_core_sod_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_clk_cpucl0_core_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_clk_cpucl0_core_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_clk_cpucl1_core_sod_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_clk_cpucl1_core_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_clk_cpucl1_core_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_clk_cpucl2_core_sod_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_clk_cpucl2_core_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_mux_clk_cpucl2_core_nm_lut_params[] = {
	0,
};

/* COMMON VCLK -> LUT Parameter List */
unsigned int v920_evt0_blk_cmu_od_lut_params[] = {
	1866000, 1334000, 480000, 1600000, 2132000, 860000, 0, 0, 2, 0, 0, 0, 0, 3, 0, 1, 3, 1, 0, 2, 0, 2, 7, 0, 1, 0, 7, 1, 1, 1, 9, 1, 1, 1, 1, 3, 1, 1, 0, 0, 0, 0, 1, 0,
};
unsigned int v920_evt0_blk_cmu_sod_lut_params[] = {
	1866000, 1333000, 480000, 1600000, 2132000, 860000, 0, 0, 2, 0, 0, 0, 0, 3, 0, 1, 3, 1, 0, 2, 0, 2, 7, 0, 1, 0, 7, 1, 1, 1, 9, 1, 1, 1, 1, 3, 1, 1, 0, 0, 0, 0, 1, 0,
};
unsigned int v920_evt0_blk_cmu_ud_lut_params[] = {
	39000, 39000, 1600000, 39000, 2132000, 38400, 0, 3, 2, 0, 0, 3, 0, 1, 1, 2, 0, 0, 0, 0, 0, 0, 7, 1, 1, 0, 7, 0, 0, 0, 9, 1, 1, 2, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int v920_evt0_blk_g3d_sod_lut_params[] = {
	1500000, 0,
};
unsigned int v920_evt0_blk_g3d_od_lut_params[] = {
	486000, 0,
};
unsigned int v920_evt0_blk_hsi2_nm_lut_params[] = {
	39000, 1, 5,
};
unsigned int v920_evt0_blk_mif1_od_lut_params[] = {
	26000,
};
unsigned int v920_evt0_blk_mif2_od_lut_params[] = {
	26000,
};
unsigned int v920_evt0_blk_mif3_od_lut_params[] = {
	26000,
};
unsigned int v920_evt0_blk_apm_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_aud_sod_lut_params[] = {
	0, 0, 0, 1, 1,
};
unsigned int v920_evt0_blk_cpucl0_sod_lut_params[] = {
	0, 1, 3, 1, 1, 1,
};
unsigned int v920_evt0_blk_cpucl1_sod_lut_params[] = {
	0, 1, 1, 1,
};
unsigned int v920_evt0_blk_cpucl2_sod_lut_params[] = {
	0, 1, 1, 1,
};
unsigned int v920_evt0_blk_hsi1_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_peric0_nm_lut_params[] = {
	1, 1, 1, 1,
};
unsigned int v920_evt0_blk_peric1_nm_lut_params[] = {
	1, 1, 1, 1,
};
unsigned int v920_evt0_blk_s2d_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_sfi_od_lut_params[] = {
	1, 1, 0, 0, 1,
};
unsigned int v920_evt0_blk_sfi_nm_lut_params[] = {
	1, 1, 0, 0, 1,
};
unsigned int v920_evt0_blk_acc_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_dnc_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_dnc_sod_lut_params[] = {
	0,
};
unsigned int v920_evt0_blk_dpub_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_blk_dpuf_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_dpuf_od_lut_params[] = {
	3,
};
unsigned int v920_evt0_blk_dsp_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_gnpu_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_hsi0_nm_lut_params[] = {
	3,
};
unsigned int v920_evt0_blk_isp_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_m2m_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_blk_m2m_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_blk_mfc_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_blk_mfd_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_blk_misc_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_nocl0_od_lut_params[] = {
	0,
};
unsigned int v920_evt0_blk_nocl1_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_nocl2_nm_lut_params[] = {
	0,
};
unsigned int v920_evt0_blk_sdma_od_lut_params[] = {
	3,
};
unsigned int v920_evt0_blk_snw_od_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_ssp_nm_lut_params[] = {
	1,
};
unsigned int v920_evt0_blk_taa_od_lut_params[] = {
	1,
};
