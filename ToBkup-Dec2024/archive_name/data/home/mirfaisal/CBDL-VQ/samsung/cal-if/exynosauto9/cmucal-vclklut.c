#include "../cmucal.h"
#include "cmucal-vclklut.h"


/* DVFS VCLK -> LUT Parameter List */
unsigned int vdd_mif_od_lut_params[] = {
	0, 6400000, 6400000,
};
unsigned int vdd_mif_nm_lut_params[] = {
	1, 3733000, 3733000,
};
unsigned int vdd_g3d1_sod_lut_params[] = {
	1070000,
};
unsigned int vdd_g3d1_od_lut_params[] = {
	860000,
};
unsigned int vdd_g3d1_nm_lut_params[] = {
	650000,
};
unsigned int vdd_cpucl0_sod_lut_params[] = {
	2100000, 1, 1,
};
unsigned int vdd_cpucl0_od_lut_params[] = {
	1700000, 1, 1,
};
unsigned int vdd_cpucl0_nm_lut_params[] = {
	1100000, 0, 1,
};
unsigned int vdd_cpucl0_ud_lut_params[] = {
	550000, 0, 0,
};
unsigned int vdd_cpucl1_sod_lut_params[] = {
	2100000, 1,
};
unsigned int vdd_cpucl1_od_lut_params[] = {
	1700000, 1,
};
unsigned int vdd_cpucl1_nm_lut_params[] = {
	1100000, 0,
};
unsigned int vdd_cpucl1_ud_lut_params[] = {
	550000, 0,
};
unsigned int vdd_g3d00_sod_lut_params[] = {
	1070000,
};
unsigned int vdd_g3d00_od_lut_params[] = {
	860000,
};
unsigned int vdd_g3d00_nm_lut_params[] = {
	650000,
};
unsigned int vdd_g3d01_sod_lut_params[] = {
	1070000,
};
unsigned int vdd_g3d01_od_lut_params[] = {
	860000,
};
unsigned int vdd_g3d01_nm_lut_params[] = {
	650000,
};
unsigned int vdd_aud_sod_lut_params[] = {
	983000, 9,
};
unsigned int vdd_aud_od_lut_params[] = {
	836000, 7,
};
unsigned int vdd_aud_nm_lut_params[] = {
	590000, 5,
};
unsigned int vdd_int_cmu_ud_lut_params[] = {
	5, 0, 1, 4, 2, 2, 4, 0, 0, 0, 2, 0, 3, 1, 5, 2, 0, 1, 1, 0, 2, 1, 2, 3, 1, 1, 2, 2, 2, 3, 1, 3, 2, 3, 1, 1, 1, 1, 1, 3,
};
unsigned int vdd_int_cmu_sod_lut_params[] = {
	0, 1, 1, 0, 0, 2, 0, 3, 0, 0, 2, 0, 0, 0, 0, 1, 7, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int vdd_int_cmu_od_lut_params[] = {
	0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 2, 1, 0, 0, 1, 2, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int vdd_int_cmu_nm_lut_params[] = {
	1, 1, 5, 4, 3, 1, 4, 3, 1, 1, 5, 0, 1, 5, 1, 2, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
};

/* SPECIAL VCLK -> LUT Parameter List */
unsigned int mux_clk_aud_uaif3_sod_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int mux_clk_aud_uaif3_od_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int mux_clk_aud_uaif3_nm_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int mux_clk_aud_uaif3_ud_lut_params[] = {
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
};
unsigned int mux_busc_cmuref_nm_lut_params[] = {
	1,
};
unsigned int mux_busmc_cmuref_nm_lut_params[] = {
	1,
};
unsigned int mux_busmc_cmuref_ud_lut_params[] = {
	0,
};
unsigned int mux_cmu_cmuref_ud_lut_params[] = {
	1,
};
unsigned int mux_cpucl1_cmuref_ud_lut_params[] = {
	1,
};
unsigned int mux_mif_cmuref_od_lut_params[] = {
	1,
};
unsigned int div_clk_aud_mclk_od_lut_params[] = {
	0,
};
unsigned int div_clk_aud_mclk_nm_lut_params[] = {
	0,
};
unsigned int div_clk_aud_mclk_sod_lut_params[] = {
	0,
};
unsigned int div_clk_aud_mclk_ud_lut_params[] = {
	0,
};
unsigned int clk_cmu_pllclkout_ud_lut_params[] = {
	4, 1,
};
unsigned int clkcmu_hpm_ud_lut_params[] = {
	0, 2,
};
unsigned int div_clk_cpucl0_shortstop_ud_lut_params[] = {
	1,
};
unsigned int mux_clkcmu_cpucl0_switch_ud_lut_params[] = {
	0,
};
unsigned int div_clk_cluster0_periphclk_ud_lut_params[] = {
	3,
};
unsigned int div_clk_cpucl1_shortstop_ud_lut_params[] = {
	1,
};
unsigned int mux_clkcmu_cpucl1_switch_ud_lut_params[] = {
	0,
};
unsigned int div_clk_cluster1_periphclk_sod_lut_params[] = {
	3, 0,
};
unsigned int div_clk_cluster1_periphclk_od_lut_params[] = {
	3, 0,
};
unsigned int div_clk_cluster1_periphclk_nm_lut_params[] = {
	3, 0,
};
unsigned int div_clk_cluster1_periphclk_ud_lut_params[] = {
	3, 0,
};
unsigned int div_clk_fsys1_mmc_card_nm_lut_params[] = {
	0, 808000,
};
unsigned int div_clk_peric0_usi00_usi_nm_lut_params[] = {
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1,
};
unsigned int div_clk_peric0_usi00_usi_ud_lut_params[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
};
unsigned int div_clk_peric1_usi_i2c_nm_lut_params[] = {
	1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
};
unsigned int div_clk_peric1_usi_i2c_ud_lut_params[] = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
unsigned int clk_sfi_pllclkout_nm_lut_params[] = {
	3, 1,
};
unsigned int div_clk_sfi_usi14_nm_lut_params[] = {
	1, 1,
};
unsigned int div_clk_sfi_usi13_nm_lut_params[] = {
	1, 1,
};
unsigned int div_clk_sfi_cpu_pclkdbg_nm_lut_params[] = {
	0, 3,
};
unsigned int div_clk_sfi_can0_nm_lut_params[] = {
	9, 1,
};
unsigned int div_clk_sfi_can1_nm_lut_params[] = {
	9, 1,
};
unsigned int div_clk_sfi_usi15_nm_lut_params[] = {
	1, 1,
};

/* COMMON VCLK -> LUT Parameter List */
unsigned int blk_cmu_ud_lut_params[] = {
	1866000, 1333000, 1600000, 1600000, 2132000, 1, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 1, 0, 1, 7, 7, 1, 1, 1, 1, 2, 0, 1, 0, 7, 0, 0, 0, 0, 0, 0, 0, 9, 1, 1, 1, 2, 2, 1, 0, 3, 1, 1, 0, 0,
};
unsigned int blk_s2d_nm_lut_params[] = {
	1600000, 1,
};
unsigned int blk_s2d_od_lut_params[] = {
	1600000, 1,
};
unsigned int blk_sfi_nm_lut_params[] = {
	800000, 750000, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 2, 7, 4, 1, 1, 0, 0, 5, 2, 1, 9, 1,
};
unsigned int blk_apm_nm_lut_params[] = {
	1,
};
unsigned int blk_cpucl0_sod_lut_params[] = {
	0, 0, 3, 3, 7, 1, 3,
};
unsigned int blk_cpucl0_od_lut_params[] = {
	0, 0, 3, 3, 7, 1, 3,
};
unsigned int blk_cpucl0_nm_lut_params[] = {
	0, 0, 3, 3, 7, 1, 3,
};
unsigned int blk_cpucl0_ud_lut_params[] = {
	0, 0, 3, 3, 7, 1, 3,
};
unsigned int blk_cpucl1_ud_lut_params[] = {
	0, 3, 7, 3,
};
unsigned int blk_fsys0_nm_lut_params[] = {
	0, 0,
};
unsigned int blk_fsys2_nm_lut_params[] = {
	0, 0, 0,
};
unsigned int blk_acc_ud_lut_params[] = {
	1,
};
unsigned int blk_aud_sod_lut_params[] = {
	1, 1, 0,
};
unsigned int blk_aud_ud_lut_params[] = {
	3, 2, 1,
};
unsigned int blk_busc_nm_lut_params[] = {
	1,
};
unsigned int blk_busmc_ud_lut_params[] = {
	1,
};
unsigned int blk_core_od_lut_params[] = {
	2,
};
unsigned int blk_dnc_ud_lut_params[] = {
	3,
};
unsigned int blk_dpum_ud_lut_params[] = {
	3,
};
unsigned int blk_dpus_ud_lut_params[] = {
	3,
};
unsigned int blk_dpus1_ud_lut_params[] = {
	3,
};
unsigned int blk_g2d_ud_lut_params[] = {
	1,
};
unsigned int blk_g3d00_sod_lut_params[] = {
	3,
};
unsigned int blk_g3d00_od_lut_params[] = {
	3,
};
unsigned int blk_g3d00_nm_lut_params[] = {
	3,
};
unsigned int blk_g3d01_sod_lut_params[] = {
	3,
};
unsigned int blk_g3d01_od_lut_params[] = {
	3,
};
unsigned int blk_g3d01_nm_lut_params[] = {
	3,
};
unsigned int blk_g3d1_sod_lut_params[] = {
	3,
};
unsigned int blk_g3d1_od_lut_params[] = {
	3,
};
unsigned int blk_g3d1_nm_lut_params[] = {
	3,
};
unsigned int blk_g3d1_ud_lut_params[] = {
	3,
};
unsigned int blk_ispb_ud_lut_params[] = {
	1,
};
unsigned int blk_mfc_ud_lut_params[] = {
	3,
};
unsigned int blk_npu_ud_lut_params[] = {
	3,
};
unsigned int blk_taa_ud_lut_params[] = {
	1,
};
