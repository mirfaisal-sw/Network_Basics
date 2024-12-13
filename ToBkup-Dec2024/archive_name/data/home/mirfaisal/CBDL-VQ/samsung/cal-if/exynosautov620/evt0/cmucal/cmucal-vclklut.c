// SPDX-License-Identifier: GPL-2.0
/*
 * Common Clock Framework support for exynosautov620 SoC.
 *
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */
#include "../../../cmucal.h"
#include "cmucal-vclklut.h"


/* DVFS VCLK -> LUT Parameter List */
unsigned int v620_evt0_vdd_int_od_lut_params[] = {
	1, 0, 0, 1, 1,
};
unsigned int v620_evt0_vdd_int_nm_lut_params[] = {
	0, 1, 1, 0, 0,
};
unsigned int v620_evt0_vdd_mif_od_lut_params[] = {
	7500000, 1,
};
unsigned int v620_evt0_vdd_mif_sod_lut_params[] = {
	7500000, 0,
};
unsigned int v620_evt0_vdd_mif_nm_lut_params[] = {
	6400000, 1,
};
unsigned int v620_evt0_vdd_g3d_od_lut_params[] = {
	1300000,
};
unsigned int v620_evt0_vdd_g3d_nm_lut_params[] = {
	1000000,
};
unsigned int v620_evt0_vdd_cpucl0_sod_lut_params[] = {
	2400000, 1,
};
unsigned int v620_evt0_vdd_cpucl0_od_lut_params[] = {
	2100000, 1,
};
unsigned int v620_evt0_vdd_cpucl0_nm_lut_params[] = {
	1600000, 0,
};
unsigned int v620_evt0_vdd_sfi_sod_lut_params[] = {
	5, 1200000, 2, 2, 2, 2, 2, 2, 14, 14,
};
unsigned int v620_evt0_vdd_sfi_nm_lut_params[] = {
	3, 800000, 1, 1, 1, 1, 1, 1, 9, 9,
};
unsigned int v620_evt0_vdd_aud_sod_lut_params[] = {
	1033000, 13, 1, 2,
};
unsigned int v620_evt0_vdd_aud_od_lut_params[] = {
	811000, 10, 1, 1,
};
unsigned int v620_evt0_vdd_aud_nm_lut_params[] = {
	590000, 7, 0, 1,
};
unsigned int v620_evt0_vdd_int_cmu_sod_lut_params[] = {
	1800000, 4, 0, 1, 0, 3, 0, 2, 0, 4, 0, 0, 0, 0, 0, 5, 1,
};
unsigned int v620_evt0_vdd_int_cmu_od_lut_params[] = {
	1600000, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 2, 0,
};
unsigned int v620_evt0_vdd_int_cmu_nm_lut_params[] = {
	1100000, 4, 2, 6, 2, 3, 3, 6, 3, 4, 9, 1, 6, 1, 1, 2, 0,
};
unsigned int v620_evt0_vdd_cpucl2_sod_lut_params[] = {
	2400000, 1,
};
unsigned int v620_evt0_vdd_cpucl2_od_lut_params[] = {
	2100000, 1,
};
unsigned int v620_evt0_vdd_cpucl2_nm_lut_params[] = {
	1600000, 0,
};

/* SPECIAL VCLK -> LUT Parameter List */
unsigned int v620_evt0_mux_clk_apm_timer_nm_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_aud_uaif3_od_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int v620_evt0_mux_clk_aud_uaif3_nm_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int v620_evt0_mux_clk_aud_uaif3_sod_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int v620_evt0_mux_cmu_cmuref_ud_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_cmu_cmuref_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_cpucl0_cmuref_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_cpucl2_cmuref_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_misc_gic_od_lut_params[] = {
	0,
};
unsigned int v620_evt0_clkcmu_misc_noc_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_clkcmu_misc_noc_sod_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_mux_nocl0_cmuref_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_clkcmu_dptx_dpgtc_ud_lut_params[] = {
	3, 2,
};
unsigned int v620_evt0_clkcmu_dptx_dpgtc_sod_lut_params[] = {
	3, 1,
};
unsigned int v620_evt0_mux_clkcmu_dpub_dsim_ud_lut_params[] = {
	0,
};
unsigned int v620_evt0_mux_clkcmu_dpub_dsim_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_clkcmu_gnpu_xmaa_ud_lut_params[] = {
	0,
};
unsigned int v620_evt0_clkcmu_gnpu_xmaa_nm_lut_params[] = {
	0,
};
unsigned int v620_evt0_div_clk_apm_spmi_nm_lut_params[] = {
	1, 1,
};
unsigned int v620_evt0_clkcmu_apm_noc_ud_lut_params[] = {
	0, 2,
};
unsigned int v620_evt0_clkcmu_apm_noc_sod_lut_params[] = {
	0, 2,
};
unsigned int v620_evt0_div_clk_aud_mclk_sod_lut_params[] = {
	0,
};
unsigned int v620_evt0_div_clk_aud_mclk_nm_lut_params[] = {
	0,
};
unsigned int v620_evt0_div_clk_aud_mclk_od_lut_params[] = {
	0,
};
unsigned int v620_evt0_clk_aud_avb_eth_sod_lut_params[] = {
	9215,
};
unsigned int v620_evt0_clk_aud_avb_eth_nm_lut_params[] = {
	9215,
};
unsigned int v620_evt0_clk_cmu_pllclkout_ud_lut_params[] = {
	4, 1,
};
unsigned int v620_evt0_clk_add_ch_clk_sod_lut_params[] = {
	17,
};
unsigned int v620_evt0_clk_add_ch_clk_ud_lut_params[] = {
	24,
};
unsigned int v620_evt0_div_clk_cmu_nocp_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_div_clk_cmu_nocp_sod_lut_params[] = {
	1, 0,
};
unsigned int v620_evt0_clkcmu_cis_mclk3_sod_lut_params[] = {
	3, 1,
};
unsigned int v620_evt0_clkcmu_cis_mclk3_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_clkcmu_cis_mclk0_sod_lut_params[] = {
	3, 1,
};
unsigned int v620_evt0_clkcmu_cis_mclk0_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_clkcmu_cis_mclk1_sod_lut_params[] = {
	3, 1,
};
unsigned int v620_evt0_clkcmu_cis_mclk1_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_clkcmu_cis_mclk2_sod_lut_params[] = {
	3, 1,
};
unsigned int v620_evt0_clkcmu_cis_mclk2_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_div_clk_cpucl0_shortstop_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_clkcmu_cpucl0_cluster_sod_lut_params[] = {
	0, 7,
};
unsigned int v620_evt0_clkcmu_cpucl0_cluster_od_lut_params[] = {
	0, 7,
};
unsigned int v620_evt0_clkcmu_cpucl0_cluster_nm_lut_params[] = {
	0, 7,
};
unsigned int v620_evt0_clkcmu_cpucl0_cluster_ud_lut_params[] = {
	1, 4,
};
unsigned int v620_evt0_clkcmu_cpucl0_switch_sod_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_clkcmu_cpucl0_switch_ud_lut_params[] = {
	1, 0,
};
unsigned int v620_evt0_div_clk_cpucl2_shortstop_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_clkcmu_cpucl2_cluster_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_clkcmu_cpucl2_cluster_sod_lut_params[] = {
	0, 7,
};
unsigned int v620_evt0_clkcmu_cpucl2_cluster_od_lut_params[] = {
	0, 7,
};
unsigned int v620_evt0_clkcmu_cpucl2_cluster_nm_lut_params[] = {
	0, 7,
};
unsigned int v620_evt0_clkcmu_cpucl2_switch_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_div_clk_dpuf1_nocp_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_clkcmu_dpuf1_noc_ud_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_clkcmu_dpuf1_noc_od_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_clkcmu_dpuf1_noc_sod_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_div_clk_nocl0_sci_div2_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_clkcmu_nocl0_noc_ud_lut_params[] = {
	0, 5,
};
unsigned int v620_evt0_clkcmu_nocl0_noc_sod_lut_params[] = {
	0, 0,
};
unsigned int v620_evt0_div_clk_peric0_usi00_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_clkcmu_peric0_ip_ud_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_clkcmu_peric0_ip_sod_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric0_usi01_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric0_usi02_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric0_usi07_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric0_usi03_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric1_usi09_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_clkcmu_peric1_ip_ud_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_clkcmu_peric1_ip_sod_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric1_usi11_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric1_usi12_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric1_usi13_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric1_usi14_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric1_usi15_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric1_usi16_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_div_clk_peric1_usi17_usi_od_lut_params[] = {
	0, 1,
};
unsigned int v620_evt0_mux_clk_sfi_pllclkout_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_pllclkout_nm_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_usi20_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_usi20_nm_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_usi19_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_usi19_nm_lut_params[] = {
	1,
};
unsigned int v620_evt0_div_clk_sfi_cpu_cntclk_sod_lut_params[] = {
	3,
};
unsigned int v620_evt0_div_clk_sfi_cpu_cntclk_nm_lut_params[] = {
	3,
};
unsigned int v620_evt0_mux_clk_sfi_usi21_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_usi21_nm_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_can0_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_can0_nm_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_can1_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_sfi_can1_nm_lut_params[] = {
	1,
};
unsigned int v620_evt0_mux_clk_cpucl0_core_sod_lut_params[] = {
	0,
};
unsigned int v620_evt0_mux_clk_cpucl0_core_od_lut_params[] = {
	0,
};
unsigned int v620_evt0_mux_clk_cpucl0_core_nm_lut_params[] = {
	0,
};
unsigned int v620_evt0_mux_clk_cpucl2_core_sod_lut_params[] = {
	0,
};
unsigned int v620_evt0_mux_clk_cpucl2_core_od_lut_params[] = {
	0,
};
unsigned int v620_evt0_mux_clk_cpucl2_core_nm_lut_params[] = {
	0,
};

/* COMMON VCLK -> LUT Parameter List */
unsigned int v620_evt0_blk_aud_sod_lut_params[] = {
	74000, 0, 0, 1,
};
unsigned int v620_evt0_blk_aud_od_lut_params[] = {
	74000, 0, 0, 1,
};
unsigned int v620_evt0_blk_aud_nm_lut_params[] = {
	74000, 0, 0, 1,
};
unsigned int v620_evt0_blk_cmu_od_lut_params[] = {
	1866000, 1334000, 1600000, 2132000, 866000, 800000, 0, 0, 4, 4, 4, 1, 2, 0, 0, 0, 0,
	0, 0, 3, 0, 2, 3, 4, 0, 0, 2, 0, 2, 7, 0, 0, 0, 1, 0, 7, 0, 0, 0, 0, 0, 9, 1, 1, 1, 0,
	0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
};
unsigned int v620_evt0_blk_cmu_sod_lut_params[] = {
	1866000, 1333000, 1600000, 2132000, 866000, 800000, 0, 0, 4, 4, 4, 1, 2, 0, 0, 0, 0,
	0, 0, 3, 0, 2, 3, 4, 0, 0, 2, 0, 2, 7, 0, 0, 0, 1, 0, 7, 0, 0, 0, 0, 0, 9, 1, 1, 1, 0,
	0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
};
unsigned int v620_evt0_blk_cmu_ud_lut_params[] = {
	2756000, 1969000, 2362000, 2132000, 38400, 38400, 3, 0, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0,
	0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 2, 1, 1, 0, 7, 0, 0, 0, 0, 0, 9, 1, 1, 2, 3, 1,
	1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int v620_evt0_blk_hsi2_od_lut_params[] = {
	750000, 1, 5,
};
unsigned int v620_evt0_blk_s2d_nm_lut_params[] = {
	400000, 1,
};
unsigned int v620_evt0_blk_apm_nm_lut_params[] = {
	1, 1, 0,
};
unsigned int v620_evt0_blk_cpucl0_sod_lut_params[] = {
	0, 1, 1, 3, 7, 1, 1, 1,
};
unsigned int v620_evt0_blk_cpucl2_sod_lut_params[] = {
	0, 1, 7, 1, 1, 1,
};
unsigned int v620_evt0_blk_g3d_sod_lut_params[] = {
	0,
};
unsigned int v620_evt0_blk_g3d_od_lut_params[] = {
	0,
};
unsigned int v620_evt0_blk_g3d_nm_lut_params[] = {
	0,
};
unsigned int v620_evt0_blk_hsi1_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_peric0_od_lut_params[] = {
	1, 1,
};
unsigned int v620_evt0_blk_peric1_od_lut_params[] = {
	1, 1, 1, 1,
};
unsigned int v620_evt0_blk_sfi_sod_lut_params[] = {
	1, 1, 1, 0, 0, 0,
};
unsigned int v620_evt0_blk_sfi_nm_lut_params[] = {
	1, 1, 1, 0, 0, 0,
};
unsigned int v620_evt0_blk_acc_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_dnc_od_lut_params[] = {
	3,
};
unsigned int v620_evt0_blk_dnc_sod_lut_params[] = {
	0,
};
unsigned int v620_evt0_blk_dpub_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_dpuf_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_dsp_od_lut_params[] = {
	3,
};
unsigned int v620_evt0_blk_gnpu_od_lut_params[] = {
	3,
};
unsigned int v620_evt0_blk_hsi0_od_lut_params[] = {
	4,
};
unsigned int v620_evt0_blk_isp_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_m2m_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_mfc_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_misc_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_nocl0_od_lut_params[] = {
	2,
};
unsigned int v620_evt0_blk_nocl1_od_lut_params[] = {
	2,
};
unsigned int v620_evt0_blk_nocl2_od_lut_params[] = {
	2,
};
unsigned int v620_evt0_blk_sdma_od_lut_params[] = {
	3,
};
unsigned int v620_evt0_blk_snw_sod_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_ssp_od_lut_params[] = {
	1,
};
unsigned int v620_evt0_blk_taa_sod_lut_params[] = {
	1,
};
