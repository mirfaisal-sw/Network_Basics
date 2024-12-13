#include "../cmucal.h"
#include "cmucal-node.h"
#include "cmucal-vclk.h"
#include "cmucal-vclklut.h"

/* DVFS VCLK -> Clock Node List */
enum clk_id cmucal_vclk_vdd_mif[] = {
	MUX_CORE_CMUREF,
	PLL_MIF_MAIN,
	PLL_MIF_SUB,
};
enum clk_id cmucal_vclk_vdd_g3d1[] = {
	PLL_G3D1,
};
enum clk_id cmucal_vclk_vdd_cpucl0[] = {
	DIV_CLK_CLUSTER0_ACLK,
	PLL_CPUCL0,
	MUX_CPUCL0_CMUREF,
};
enum clk_id cmucal_vclk_vdd_cpucl1[] = {
	DIV_CLK_CLUSTER1_ACLK,
	PLL_CPUCL1,
};
enum clk_id cmucal_vclk_vdd_g3d00[] = {
	PLL_G3D0,
};
enum clk_id cmucal_vclk_vdd_g3d01[] = {
	PLL_G3D01,
};
enum clk_id cmucal_vclk_vdd_aud[] = {
	DIV_CLK_AUD_AUDIF,
	PLL_AUD,
};
enum clk_id cmucal_vclk_vdd_int_cmu[] = {
	MUX_CLKCMU_CORE_BUS,
	CLKCMU_MIF_BUSP,
	MUX_CLKCMU_MIF_BUSP,
	CLKCMU_NPU_BUS,
	MUX_CLKCMU_NPU_BUS,
	CLKCMU_CPUCL0_CLUSTER,
	MUX_CLKCMU_CPUCL0_CLUSTER,
	CLKCMU_G3D01_SWITCH,
	MUX_CLKCMU_G3D01_SWITCH,
	MUX_CLKCMU_DPTX_DPGTC,
	CLKCMU_CPUCL1_CLUSTER,
	MUX_CLKCMU_CPUCL1_CLUSTER,
	CLKCMU_FSYS2_UFS_EMBD,
	MUX_CLKCMU_FSYS2_UFS_EMBD,
	CLKCMU_FSYS2_BUS,
	CLKCMU_FSYS2_ETHERNET,
	CLKCMU_DNC_CPU,
	MUX_CLKCMU_DNC_CPU,
	CLKCMU_DNC_BUS,
	MUX_CLKCMU_DNC_BUS,
	MUX_CLKCMU_G2D_G2D,
	MUX_CLKCMU_APM_BUS,
	CLKCMU_PERIC0_IP,
	MUX_CLKCMU_PERIC0_IP,
	CLKCMU_PERIC1_IP,
	MUX_CLKCMU_PERIC1_IP,
	CLKCMU_DPUM_BUS,
	CLKCMU_MFC_MFC,
	MUX_CLKCMU_BUSC_BUS,
	MUX_CLKCMU_DPUS0_BUS,
	MUX_CLKCMU_DPUS1_BUS,
	CLKCMU_CPUCL0_SWITCH,
	MUX_CLKCMU_TAA_BUS,
	MUX_CLKCMU_ACC_BUS,
	MUX_CLKCMU_ISPB_BUS,
	MUX_CLKCMU_G2D_MSCL,
	MUX_CLKCMU_CPUCL0_DBG,
	MUX_CLKCMU_FSYS0_BUS,
	MUX_CLKCMU_MFC_WFD,
	MUX_CLKCMU_DPTX_BUS,
	CLKCMU_CPUCL1_SWITCH,
};
/* SPECIAL VCLK -> Clock Node List */
enum clk_id cmucal_vclk_mux_clk_aud_uaif3[] = {
	MUX_CLK_AUD_UAIF3,
	DIV_CLK_AUD_UAIF3,
	MUX_CLK_AUD_UAIF2,
	DIV_CLK_AUD_UAIF2,
	MUX_CLK_AUD_UAIF1,
	DIV_CLK_AUD_UAIF1,
	MUX_CLK_AUD_UAIF0,
	DIV_CLK_AUD_UAIF0,
	MUX_HCHGEN_CLK_AUD_CPU,
	DIV_CLK_AUD_CPU,
	DIV_HCHGEN_CLK_AUD_CPU,
	MUX_CLK_AUD_UAIF4,
	DIV_CLK_AUD_UAIF4,
	MUX_CLK_AUD_UAIF5,
	DIV_CLK_AUD_UAIF5,
	MUX_CLK_AUD_UAIF6,
	DIV_CLK_AUD_UAIF6,
	DIV_CLK_AUD_CNT,
};
enum clk_id cmucal_vclk_mux_busc_cmuref[] = {
	MUX_BUSC_CMUREF,
};
enum clk_id cmucal_vclk_mux_busmc_cmuref[] = {
	MUX_BUSMC_CMUREF,
};
enum clk_id cmucal_vclk_mux_cmu_cmuref[] = {
	MUX_CMU_CMUREF,
};
enum clk_id cmucal_vclk_mux_cpucl1_cmuref[] = {
	MUX_CPUCL1_CMUREF,
};
enum clk_id cmucal_vclk_mux_mif_cmuref[] = {
	MUX_MIF_CMUREF,
};
enum clk_id cmucal_vclk_div_clk_aud_mclk[] = {
	DIV_CLK_AUD_MCLK,
};
enum clk_id cmucal_vclk_clk_cmu_pllclkout[] = {
	CLK_CMU_PLLCLKOUT,
	MUX_CLK_CMU_PLLCLKOUT,
};
enum clk_id cmucal_vclk_clkcmu_hpm[] = {
	CLKCMU_HPM,
	MUX_CLKCMU_HPM,
};
enum clk_id cmucal_vclk_div_clk_cpucl0_shortstop[] = {
	DIV_CLK_CPUCL0_SHORTSTOP,
};
enum clk_id cmucal_vclk_mux_clkcmu_cpucl0_switch[] = {
	MUX_CLKCMU_CPUCL0_SWITCH,
};
enum clk_id cmucal_vclk_div_clk_cluster0_periphclk[] = {
	DIV_CLK_CLUSTER0_PERIPHCLK,
};
enum clk_id cmucal_vclk_div_clk_cpucl1_shortstop[] = {
	DIV_CLK_CPUCL1_SHORTSTOP,
};
enum clk_id cmucal_vclk_mux_clkcmu_cpucl1_switch[] = {
	MUX_CLKCMU_CPUCL1_SWITCH,
};
enum clk_id cmucal_vclk_div_clk_cluster1_periphclk[] = {
	DIV_CLK_CLUSTER1_PERIPHCLK,
	MUX_CLK_CPUCL1_CORE,
};
enum clk_id cmucal_vclk_div_clk_fsys1_mmc_card[] = {
	DIV_CLK_FSYS1_MMC_CARD,
	PLL_MMC,
};
enum clk_id cmucal_vclk_div_clk_peric0_usi00_usi[] = {
	DIV_CLK_PERIC0_USI00_USI,
	MUX_CLK_PERIC0_USI00_USI,
	DIV_CLK_PERIC0_USI01_USI,
	MUX_CLK_PERIC0_USI01_USI,
	DIV_CLK_PERIC0_USI02_USI,
	MUX_CLK_PERIC0_USI02_USI,
	DIV_CLK_PERIC0_USI03_USI,
	MUX_CLK_PERIC0_USI03_USI,
	DIV_CLK_PERIC0_USI04_USI,
	MUX_CLK_PERIC0_USI04_USI,
	DIV_CLK_PERIC0_USI05_USI,
	MUX_CLK_PERIC0_USI05_USI,
	DIV_CLK_PERIC0_USI_I2C,
	MUX_CLK_PERIC0_USI_I2C,
};
enum clk_id cmucal_vclk_div_clk_peric1_usi_i2c[] = {
	DIV_CLK_PERIC1_USI_I2C,
	MUX_CLK_PERIC1_USI_I2C,
	DIV_CLK_PERIC1_USI06_USI,
	MUX_CLK_PERIC1_USI06_USI,
	DIV_CLK_PERIC1_USI07_USI,
	MUX_CLK_PERIC1_USI07_USI,
	DIV_CLK_PERIC1_USI08_USI,
	MUX_CLK_PERIC1_USI08_USI,
	DIV_CLK_PERIC1_USI09_USI,
	MUX_CLK_PERIC1_USI09_USI,
	DIV_CLK_PERIC1_USI10_USI,
	MUX_CLK_PERIC1_USI10_USI,
	DIV_CLK_PERIC1_USI11_USI,
	MUX_CLK_PERIC1_USI11_USI,
};
enum clk_id cmucal_vclk_clk_sfi_pllclkout[] = {
	CLK_SFI_PLLCLKOUT,
	MUX_CLK_SFI_PLLCLKOUT,
};
enum clk_id cmucal_vclk_div_clk_sfi_usi14[] = {
	DIV_CLK_SFI_USI14,
	MUX_CLK_SFI_USI14,
};
enum clk_id cmucal_vclk_div_clk_sfi_usi13[] = {
	DIV_CLK_SFI_USI13,
	MUX_CLK_SFI_USI13,
};
enum clk_id cmucal_vclk_div_clk_sfi_cpu_pclkdbg[] = {
	DIV_CLK_SFI_CPU_PCLKDBG,
	DIV_CLK_SFI_CPU_CNTCLK,
};
enum clk_id cmucal_vclk_div_clk_sfi_can0[] = {
	DIV_CLK_SFI_CAN0,
	MUX_CLK_SFI_CAN0,
};
enum clk_id cmucal_vclk_div_clk_sfi_can1[] = {
	DIV_CLK_SFI_CAN1,
	MUX_CLK_SFI_CAN1,
};
enum clk_id cmucal_vclk_div_clk_sfi_usi15[] = {
	DIV_CLK_SFI_USI15,
	MUX_CLK_SFI_USI15,
};
/* COMMON VCLK -> Clock Node List */
enum clk_id cmucal_vclk_blk_cmu[] = {
	MUX_CLKCMU_MIF_SWITCH,
	CLKCMU_ACC_BUS,
	CLKCMU_ISPB_BUS,
	CLKCMU_TAA_BUS,
	CLKCMU_CORE_BUS,
	CLKCMU_G2D_G2D,
	MUX_CLKCMU_DPUM_BUS,
	CLKCMU_DPUS0_BUS,
	CLKCMU_DPUS1_BUS,
	MUX_CLKCMU_DPUS1_BUS,
	MUX_CLKCMU_FSYS2_ETHERNET,
	PLL_SHARED1_DIV3,
	MUX_CLKCMU_MFC_MFC,
	CLKCMU_G2D_MSCL,
	CLKCMU_MFC_WFD,
	CLKCMU_BUSC_BUS,
	DIV_CLKCMU_CMU_BOOST,
	MUX_CLKCMU_CMU_BOOST,
	CLKCMU_FSYS1_BUS,
	MUX_CLKCMU_FSYS1_BUS,
	CLKCMU_FSYS0_BUS,
	CLKCMU_DPTX_BUS,
	CLKCMU_BUSMC_BUS,
	MUX_CLKCMU_BUSMC_BUS,
	CLKCMU_APM_BUS,
	CLKCMU_BUSC_SSS,
	MUX_CLKCMU_BUSC_SSS,
	MUX_CLKCMU_FSYS2_BUS,
	PLL_SHARED1_DIV4,
	PLL_SHARED1_DIV2,
	PLL_SHARED1,
	CLKCMU_CPUCL0_DBG,
	CLKCMU_DPTX_DPGTC,
	CLKCMU_FSYS1_USBDRD,
	MUX_CLKCMU_FSYS1_USBDRD,
	PLL_SHARED4_DIV4,
	PLL_SHARED4_DIV2,
	PLL_SHARED4,
	PLL_SHARED3,
	CLKCMU_PERIC1_BUS,
	MUX_CLKCMU_PERIC1_BUS,
	CLKCMU_PERIC0_BUS,
	MUX_CLKCMU_PERIC0_BUS,
	CLKCMU_PERIS_BUS,
	MUX_CLKCMU_PERIS_BUS,
	MUX_CLKCMU_FSYS0_PCIE,
	PLL_SHARED2_DIV4,
	PLL_SHARED2_DIV2,
	PLL_SHARED2_DIV3,
	PLL_SHARED2,
	PLL_SHARED0_DIV2,
	PLL_SHARED0_DIV3,
	PLL_SHARED0,
};
enum clk_id cmucal_vclk_blk_s2d[] = {
	MUX_CLK_S2D_CORE,
	PLL_MIF_S2D,
};
enum clk_id cmucal_vclk_blk_sfi[] = {
	DIV_CLK_SFI_USI12,
	MUX_CLK_SFI_USI12,
	DIV_CLK_SFI_CPU_ACLK,
	DIV_CLK_SFI_CPU_ATCLK,
	MUX_CLK_SFI_CPU,
	DIV_CLK_SFI_BUSP,
	DIV_CLK_SFI_BUSD,
	MUX_CLK_SFI_BUS,
	DIV_CLK_SFI_SERIALFLASH,
	MUX_CLK_SFI_SERIALFLASH,
	DIV_CLK_SFI_HYPERBUS,
	MUX_CLK_SFI_HYPERBUS,
	CLKSFI_FSYS0_BUS,
	MUX_CLKSFI_FSYS0_BUS,
	CLKSFI_FSYS2_BUS,
	MUX_CLKSFI_FSYS2_BUS,
	CLKSFI_FSYS0_PCIE,
	MUX_CLKSFI_FSYS0_PCIE,
	CLKSFI_FSYS2_UFS_EMBD,
	MUX_CLKSFI_FSYS2_UFS_EMBD,
	CLKSFI_FSYS2_ETHERNET,
	MUX_CLKSFI_FSYS2_ETHERNET,
	PLL_SFI,
	PLL_ETH,
};
enum clk_id cmucal_vclk_blk_apm[] = {
	MUX_CLK_APM_BUS,
};
enum clk_id cmucal_vclk_blk_cpucl0[] = {
	MUX_CLK_CPUCL0_CORE,
	DIV_CLK_CLUSTER0_ATCLK,
	DIV_CLK_CLUSTER0_PCLK,
	DIV_CLK_CPUCL0_BUSP,
	MUX_CLK_CPUCL0_CLUSTER,
	DIV_CLK_CPUCL0_DBG_PCLKDBG,
	DIV_CLK_CPUCL0_DBG_BUS,
};
enum clk_id cmucal_vclk_blk_cpucl1[] = {
	DIV_CLK_CLUSTER1_ATCLK,
	DIV_CLK_CLUSTER1_PCLK,
	DIV_CLK_CPUCL1_BUSP,
	MUX_CLK_CPUCL1_CLUSTER,
};
enum clk_id cmucal_vclk_blk_fsys0[] = {
	MUX_CLK_FSYS0_BUS,
	MUX_CLK_FSYS0_PCIE,
};
enum clk_id cmucal_vclk_blk_fsys2[] = {
	MUX_CLK_FSYS2_UFS_EMBD,
	MUX_CLK_FSYS2_ETHERNET,
	MUX_CLK_FSYS2_BUS,
};
enum clk_id cmucal_vclk_blk_acc[] = {
	DIV_CLK_ACC_BUSP,
};
enum clk_id cmucal_vclk_blk_aud[] = {
	DIV_CLK_AUD_BUSP,
	DIV_CLK_AUD_BUS,
	DIV_CLK_AUD_DMIC,
};
enum clk_id cmucal_vclk_blk_busc[] = {
	DIV_CLK_BUSC_BUSP,
};
enum clk_id cmucal_vclk_blk_busmc[] = {
	DIV_CLK_BUSMC_BUSP,
};
enum clk_id cmucal_vclk_blk_core[] = {
	DIV_CLK_CORE_BUSP,
};
enum clk_id cmucal_vclk_blk_dnc[] = {
	DIV_CLK_DNC_BUSP,
};
enum clk_id cmucal_vclk_blk_dpum[] = {
	DIV_CLK_DPUM_BUSP,
};
enum clk_id cmucal_vclk_blk_dpus[] = {
	DIV_CLK_DPUS_BUSP,
};
enum clk_id cmucal_vclk_blk_dpus1[] = {
	DIV_CLK_DPUS1_BUSP,
};
enum clk_id cmucal_vclk_blk_g2d[] = {
	DIV_CLK_G2D_BUSP,
};
enum clk_id cmucal_vclk_blk_g3d00[] = {
	DIV_CLK_G3D00_BUSP,
};
enum clk_id cmucal_vclk_blk_g3d01[] = {
	DIV_CLK_G3D01_BUSP,
};
enum clk_id cmucal_vclk_blk_g3d1[] = {
	DIV_CLK_G3D1_BUSP,
};
enum clk_id cmucal_vclk_blk_ispb[] = {
	DIV_CLK_ISPB_BUSP,
};
enum clk_id cmucal_vclk_blk_mfc[] = {
	DIV_CLK_MFC_BUSP,
};
enum clk_id cmucal_vclk_blk_npu[] = {
	DIV_CLK_NPU_BUSP,
};
enum clk_id cmucal_vclk_blk_taa[] = {
	DIV_CLK_TAA_BUSP,
};
/* GATE VCLK -> Clock Node List */
enum clk_id cmucal_vclk_ip_lhm_axi_p_acc[] = {
	GOUT_BLK_ACC_UID_LHM_AXI_P_ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d0_acc[] = {
	GOUT_BLK_ACC_UID_LHS_AXI_D0_ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_btm_acc[] = {
	GOUT_BLK_ACC_UID_BTM_ACC_IPCLKPORT_I_ACLK,
	GOUT_BLK_ACC_UID_BTM_ACC_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_acc[] = {
	GOUT_BLK_ACC_UID_SYSREG_ACC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_c2c_acctaa0[] = {
	GOUT_BLK_ACC_UID_LHS_AST_C2C_ACCTAA0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_otf_acctaa1[] = {
	GOUT_BLK_ACC_UID_LHS_AST_OTF_ACCTAA1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_acc[] = {
	GOUT_BLK_ACC_UID_D_TZPC_ACC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_otf_acctaa0[] = {
	GOUT_BLK_ACC_UID_LHS_AST_OTF_ACCTAA0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_acc_kitt[] = {
	GOUT_BLK_ACC_UID_XIU_D_ACC_KITT_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_acc[] = {
	GOUT_BLK_ACC_UID_VGEN_LITE_ACC_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_acc[] = {
	GOUT_BLK_ACC_UID_PPMU_ACC_IPCLKPORT_ACLK,
	GOUT_BLK_ACC_UID_PPMU_ACC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_gdc0[] = {
	GOUT_BLK_ACC_UID_QE_GDC0_IPCLKPORT_ACLK,
	GOUT_BLK_ACC_UID_QE_GDC0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_gdc1[] = {
	GOUT_BLK_ACC_UID_QE_GDC1_IPCLKPORT_ACLK,
	GOUT_BLK_ACC_UID_QE_GDC1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_isppre[] = {
	GOUT_BLK_ACC_UID_QE_ISPPRE_IPCLKPORT_ACLK,
	GOUT_BLK_ACC_UID_QE_ISPPRE_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_vra[] = {
	GOUT_BLK_ACC_UID_QE_VRA_IPCLKPORT_ACLK,
	GOUT_BLK_ACC_UID_QE_VRA_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_acc[] = {
	GOUT_BLK_ACC_UID_SYSMMU_ACC_IPCLKPORT_CLK_S1,
	GOUT_BLK_ACC_UID_SYSMMU_ACC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_apb_async_csis0[] = {
	GOUT_BLK_ACC_UID_APB_ASYNC_CSIS0_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_hub[] = {
	GOUT_BLK_ACC_UID_HUB_IPCLKPORT_ISPPRE_I_CLK,
	GOUT_BLK_ACC_UID_HUB_IPCLKPORT_GDC0_CLK,
	GOUT_BLK_ACC_UID_HUB_IPCLKPORT_GDC1_CLK,
	GOUT_BLK_ACC_UID_HUB_IPCLKPORT_VRA_CLK,
	GOUT_BLK_ACC_UID_HUB_IPCLKPORT_C2COMX_C2CLK,
	GOUT_BLK_ACC_UID_HUB_IPCLKPORT_ISPPRE_I_C2CLK,
	GOUT_BLK_ACC_UID_HUB_IPCLKPORT_GDC0_C2CLK,
	GOUT_BLK_ACC_UID_HUB_IPCLKPORT_GDC1_C2CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_c2c_acctaa1[] = {
	GOUT_BLK_ACC_UID_LHS_AST_C2C_ACCTAA1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_c2c_ispb0acc[] = {
	GOUT_BLK_ACC_UID_LHM_AST_C2C_ISPB0ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_c2c_ispb1acc[] = {
	GOUT_BLK_ACC_UID_LHM_AST_C2C_ISPB1ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d0_accdpum[] = {
	GOUT_BLK_ACC_UID_LHS_AST_D0_ACCDPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d1_accdpum[] = {
	GOUT_BLK_ACC_UID_LHS_AST_D1_ACCDPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_otf_ispb0acc[] = {
	GOUT_BLK_ACC_UID_LHM_AST_OTF_ISPB0ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_otf_ispb1acc[] = {
	GOUT_BLK_ACC_UID_LHM_AST_OTF_ISPB1ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_csis_link_mux3x6[] = {
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS0_I_ACLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS1_I_ACLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS2_I_ACLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS0P1_I_ACLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS1P1_I_ACLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS2P1_I_ACLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS0_I_PCLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS0P1_I_PCLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS1_I_PCLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS1P1_I_PCLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS2_I_PCLK,
	GOUT_BLK_ACC_UID_CSIS_LINK_MUX3X6_IPCLKPORT_CSIS2P1_I_PCLK,
};
enum clk_id cmucal_vclk_ip_vgen_isppre[] = {
	GOUT_BLK_ACC_UID_VGEN_ISPPRE_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_isppre[] = {
	GOUT_BLK_ACC_UID_SYSMMU_ISPPRE_IPCLKPORT_CLK_S1,
	GOUT_BLK_ACC_UID_SYSMMU_ISPPRE_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_ppmu_isppre[] = {
	GOUT_BLK_ACC_UID_PPMU_ISPPRE_IPCLKPORT_ACLK,
	GOUT_BLK_ACC_UID_PPMU_ISPPRE_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_acc_cmu_acc[] = {
	CLK_BLK_ACC_UID_ACC_CMU_ACC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d1_acc[] = {
	GOUT_BLK_ACC_UID_LHS_AXI_D1_ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_btm_isppre[] = {
	GOUT_BLK_ACC_UID_BTM_ISPPRE_IPCLKPORT_I_ACLK,
	GOUT_BLK_ACC_UID_BTM_ISPPRE_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_apm[] = {
	GOUT_BLK_APM_UID_LHS_AXI_D_APM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_apm[] = {
	GOUT_BLK_APM_UID_LHM_AXI_P_APM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_wdt_apm[] = {
	GOUT_BLK_APM_UID_WDT_APM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_apm[] = {
	GOUT_BLK_APM_UID_SYSREG_APM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_apm_ap[] = {
	GOUT_BLK_APM_UID_MAILBOX_APM_AP_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_apbif_pmu_alive[] = {
	GOUT_BLK_APM_UID_APBIF_PMU_ALIVE_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_intmem[] = {
	GOUT_BLK_APM_UID_INTMEM_IPCLKPORT_ACLK,
	GOUT_BLK_APM_UID_INTMEM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_g_scan2dram[] = {
	GOUT_BLK_APM_UID_LHS_AXI_G_SCAN2DRAM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_pmu_intr_gen[] = {
	GOUT_BLK_APM_UID_PMU_INTR_GEN_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_speedy_apm[] = {
	GOUT_BLK_APM_UID_SPEEDY_APM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_xiu_dp_apm[] = {
	GOUT_BLK_APM_UID_XIU_DP_APM_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_apm_cmu_apm[] = {
	CLK_BLK_APM_UID_APM_CMU_APM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_grebeintegration[] = {
	GOUT_BLK_APM_UID_GREBEINTEGRATION_IPCLKPORT_HCLK,
};
enum clk_id cmucal_vclk_ip_apbif_gpio_alive[] = {
	GOUT_BLK_APM_UID_APBIF_GPIO_ALIVE_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_apbif_top_rtc[] = {
	GOUT_BLK_APM_UID_APBIF_TOP_RTC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ss_dbgcore[] = {
	GOUT_BLK_APM_UID_SS_DBGCORE_IPCLKPORT_SS_DBGCORE_IPCLKPORT_HCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_apm[] = {
	GOUT_BLK_APM_UID_D_TZPC_APM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_ap_dbgcore[] = {
	GOUT_BLK_APM_UID_MAILBOX_AP_DBGCORE_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_g_dbgcore[] = {
	GOUT_BLK_APM_UID_LHS_AXI_G_DBGCORE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_apbif_rtc[] = {
	GOUT_BLK_APM_UID_APBIF_RTC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_rom_crc32_hcu[] = {
	GOUT_BLK_APM_UID_ROM_CRC32_HCU_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_rom_crc32_host[] = {
	GOUT_BLK_APM_UID_ROM_CRC32_HOST_IPCLKPORT_ACLK,
	GOUT_BLK_APM_UID_ROM_CRC32_HOST_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_apm_sfi0[] = {
	GOUT_BLK_APM_UID_MAILBOX_APM_SFI0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_sfiapm[] = {
	GOUT_BLK_APM_UID_LHM_AXI_P_SFIAPM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_speedy_sub0_apm[] = {
	GOUT_BLK_APM_UID_SPEEDY_SUB0_APM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_apm_sfi1[] = {
	GOUT_BLK_APM_UID_MAILBOX_APM_SFI1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_aud_cmu_aud[] = {
	CLK_BLK_AUD_UID_AUD_CMU_AUD_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_aud[] = {
	GOUT_BLK_AUD_UID_LHS_AXI_D_AUD_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d_aud[] = {
	GOUT_BLK_AUD_UID_PPMU_D_AUD_IPCLKPORT_ACLK,
	GOUT_BLK_AUD_UID_PPMU_D_AUD_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_aud[] = {
	GOUT_BLK_AUD_UID_SYSREG_AUD_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_abox[] = {
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF0,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF1,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF3,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF2,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK_IRQ,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_ACLK_IRQ,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_CNT,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF4,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF5,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_BCLK_UAIF6,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI4,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI4_B,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_HIFI4_B,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_DAP,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP0,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP1,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP2,
	GOUT_BLK_AUD_UID_ABOX_IPCLKPORT_CCLK_SWP3,
};
enum clk_id cmucal_vclk_ip_gpio_aud[] = {
	GOUT_BLK_AUD_UID_GPIO_AUD_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_axi_us_32to128[] = {
	GOUT_BLK_AUD_UID_AXI_US_32TO128_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_btm_d_aud[] = {
	GOUT_BLK_AUD_UID_BTM_D_AUD_IPCLKPORT_I_ACLK,
	GOUT_BLK_AUD_UID_BTM_D_AUD_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_peri_axi_asb[] = {
	GOUT_BLK_AUD_UID_PERI_AXI_ASB_IPCLKPORT_ACLKM,
	GOUT_BLK_AUD_UID_PERI_AXI_ASB_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_aud[] = {
	GOUT_BLK_AUD_UID_LHM_AXI_P_AUD_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_wdt_aud0[] = {
	GOUT_BLK_AUD_UID_WDT_AUD0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_dftmux_aud[] = {
	CLK_BLK_AUD_UID_DFTMUX_AUD_IPCLKPORT_AUD_CODEC_MCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_axi_d_aud[] = {
	GOUT_BLK_AUD_UID_SYSMMU_AXI_D_AUD_IPCLKPORT_CLK_S1,
	GOUT_BLK_AUD_UID_SYSMMU_AXI_D_AUD_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_ad_apb_smmu_aud_s1_ns[] = {
	GOUT_BLK_AUD_UID_AD_APB_SMMU_AUD_S1_NS_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_ad_apb_smmu_aud_s1_s[] = {
	GOUT_BLK_AUD_UID_AD_APB_SMMU_AUD_S1_S_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_ad_apb_smmu_aud_s2[] = {
	GOUT_BLK_AUD_UID_AD_APB_SMMU_AUD_S2_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_wdt_aud1[] = {
	GOUT_BLK_AUD_UID_WDT_AUD1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_wdt_aud2[] = {
	GOUT_BLK_AUD_UID_WDT_AUD2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_wdt_aud3[] = {
	GOUT_BLK_AUD_UID_WDT_AUD3_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_vgen_aud0[] = {
	GOUT_BLK_AUD_UID_AD_APB_VGEN_AUD0_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_vgen_aud0[] = {
	GOUT_BLK_AUD_UID_VGEN_AUD0_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_aud1[] = {
	GOUT_BLK_AUD_UID_VGEN_AUD1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_aud[] = {
	GOUT_BLK_AUD_UID_VGEN_LITE_AUD_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_vgen_aud1[] = {
	GOUT_BLK_AUD_UID_AD_APB_VGEN_AUD1_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_d_tzpc_aud[] = {
	GOUT_BLK_AUD_UID_D_TZPC_AUD_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_trex_d_busc[] = {
	GOUT_BLK_BUSC_UID_TREX_D_BUSC_IPCLKPORT_ACLK,
	GOUT_BLK_BUSC_UID_TREX_D_BUSC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_busc[] = {
	GOUT_BLK_BUSC_UID_SYSREG_BUSC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_busc[] = {
	GOUT_BLK_BUSC_UID_D_TZPC_BUSC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d_fsys1[] = {
	GOUT_BLK_BUSC_UID_LHM_ACEL_D_FSYS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d0_dpum[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D0_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d0_dpus0[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D0_DPUS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d0_dpus1[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D0_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d1_g2d[] = {
	GOUT_BLK_BUSC_UID_LHM_ACEL_D1_G2D_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d0_mfc[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D0_MFC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d1_dpum[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D1_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d1_dpus0[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D1_DPUS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d1_dpus1[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D1_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d0_g2d[] = {
	GOUT_BLK_BUSC_UID_LHM_ACEL_D0_G2D_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d1_mfc[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D1_MFC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d2_dpum[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D2_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d2_dpus0[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D2_DPUS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d2_dpus1[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D2_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d2_g2d[] = {
	GOUT_BLK_BUSC_UID_LHM_ACEL_D2_G2D_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d3_dpum[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D3_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d3_dpus0[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D3_DPUS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d3_dpus1[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D3_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d0_acc[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D0_ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_ispb0[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D_ISPB0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_ispb1[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D_ISPB1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_taa0[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D_TAA0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_taa1[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D_TAA1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_dbg_g_busc[] = {
	GOUT_BLK_BUSC_UID_LHS_DBG_G_BUSC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d1_acc[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D1_ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_trex_p_busmc[] = {
	GOUT_BLK_BUSC_UID_TREX_P_BUSMC_IPCLKPORT_ACLK_BUSMC,
	GOUT_BLK_BUSC_UID_TREX_P_BUSMC_IPCLKPORT_PCLK_BUSMC,
	GOUT_BLK_BUSC_UID_TREX_P_BUSMC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_adm_ahb_sss[] = {
	GOUT_BLK_BUSC_UID_ADM_AHB_SSS_IPCLKPORT_HCLKM,
};
enum clk_id cmucal_vclk_ip_ad_apb_puf[] = {
	GOUT_BLK_BUSC_UID_AD_APB_PUF_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_baaw_d_sss[] = {
	GOUT_BLK_BUSC_UID_BAAW_D_SSS_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_baaw_p_sfi_busc[] = {
	GOUT_BLK_BUSC_UID_BAAW_P_SFI_BUSC_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_busmc_dp[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D_BUSMC_DP_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_sss[] = {
	GOUT_BLK_BUSC_UID_LHM_AXI_D_SSS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_sss[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_D_SSS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d0_pcie[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_D0_PCIE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d1_pcie[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_D1_PCIE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_acc[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_ACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_aud[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_AUD_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_busmc[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_BUSMC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_dptx[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_DPTX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_dpum[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_dpus0[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_DPUS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_dpus1[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_fsys2[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_FSYS2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_fsys0[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_FSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_fsys1[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_FSYS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_g2d[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_G2D_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_ispb0[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_ISPB0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_ispb1[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_ISPB1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_mfc[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_MFC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_mif0[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_MIF0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_mif1[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_MIF1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_mif2[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_MIF2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_mif3[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_MIF3_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_peric0[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_PERIC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_peric1[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_PERIC1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_peris[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_PERIS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_taa0[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_TAA0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_taa1[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_TAA1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_puf[] = {
	GOUT_BLK_BUSC_UID_PUF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_busc[] = {
	GOUT_BLK_BUSC_UID_XIU_D_BUSC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_busc[] = {
	GOUT_BLK_BUSC_UID_VGEN_LITE_BUSC_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_sss[] = {
	GOUT_BLK_BUSC_UID_SSS_IPCLKPORT_I_ACLK,
	GOUT_BLK_BUSC_UID_SSS_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_sirex[] = {
	GOUT_BLK_BUSC_UID_SIREX_IPCLKPORT_I_ACLK,
	GOUT_BLK_BUSC_UID_SIREX_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_rtic[] = {
	GOUT_BLK_BUSC_UID_RTIC_IPCLKPORT_I_ACLK,
	GOUT_BLK_BUSC_UID_RTIC_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_sss[] = {
	GOUT_BLK_BUSC_UID_QE_SSS_IPCLKPORT_ACLK,
	GOUT_BLK_BUSC_UID_QE_SSS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_rtic[] = {
	GOUT_BLK_BUSC_UID_QE_RTIC_IPCLKPORT_ACLK,
	GOUT_BLK_BUSC_UID_QE_RTIC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_busc_cmu_busc[] = {
	CLK_BLK_BUSC_UID_BUSC_CMU_BUSC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_dnc[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_npu00[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_NPU00_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_npu01[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_NPU01_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_npu10[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_NPU10_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_npu11[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_NPU11_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d_fsys2[] = {
	GOUT_BLK_BUSC_UID_LHM_ACEL_D_FSYS2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_busc[] = {
	GOUT_BLK_BUSC_UID_SYSMMU_BUSC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_sfi[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_P_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_dncsram[] = {
	GOUT_BLK_BUSC_UID_LHS_AXI_D_DNCSRAM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_sysmmu_busc[] = {
	GOUT_BLK_BUSC_UID_AD_APB_SYSMMU_BUSC_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_busmc_cmu_busmc[] = {
	CLK_BLK_BUSMC_UID_BUSMC_CMU_BUSMC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_busmc[] = {
	GOUT_BLK_BUSMC_UID_SYSREG_BUSMC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_busif_cmutopc[] = {
	GOUT_BLK_BUSMC_UID_BUSIF_CMUTOPC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_trex_d0_busmc[] = {
	GOUT_BLK_BUSMC_UID_TREX_D0_BUSMC_IPCLKPORT_PCLK,
	GOUT_BLK_BUSMC_UID_TREX_D0_BUSMC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d0_fsys0[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D0_FSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d7_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D7_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d1_fsys0[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D1_FSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_apm[] = {
	GOUT_BLK_BUSMC_UID_LHM_AXI_D_APM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_trex_rb_busmc[] = {
	GOUT_BLK_BUSMC_UID_TREX_RB_BUSMC_IPCLKPORT_CLK,
	GOUT_BLK_BUSMC_UID_TREX_RB_BUSMC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_wrap2_conv_busmc[] = {
	GOUT_BLK_BUSMC_UID_WRAP2_CONV_BUSMC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_pdma0[] = {
	GOUT_BLK_BUSMC_UID_VGEN_PDMA0_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_pdma0[] = {
	GOUT_BLK_BUSMC_UID_PDMA0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_spdma[] = {
	GOUT_BLK_BUSMC_UID_SPDMA_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_busmc[] = {
	GOUT_BLK_BUSMC_UID_D_TZPC_BUSMC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mmcache0[] = {
	GOUT_BLK_BUSMC_UID_MMCACHE0_IPCLKPORT_ACLK,
	GOUT_BLK_BUSMC_UID_MMCACHE0_IPCLKPORT_PCLK,
	GOUT_BLK_BUSMC_UID_MMCACHE0_IPCLKPORT_PCLK_SECURE,
	GOUT_BLK_BUSMC_UID_MMCACHE0_IPCLKPORT_PCLK_SECURE,
};
enum clk_id cmucal_vclk_ip_trex_d1_busmc[] = {
	GOUT_BLK_BUSMC_UID_TREX_D1_BUSMC_IPCLKPORT_ACLK,
	GOUT_BLK_BUSMC_UID_TREX_D1_BUSMC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_dbg_g_busmc[] = {
	GOUT_BLK_BUSMC_UID_LHS_DBG_G_BUSMC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_qe_spdma[] = {
	GOUT_BLK_BUSMC_UID_QE_SPDMA_IPCLKPORT_ACLK,
	GOUT_BLK_BUSMC_UID_QE_SPDMA_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_pdma0[] = {
	GOUT_BLK_BUSMC_UID_QE_PDMA0_IPCLKPORT_ACLK,
	GOUT_BLK_BUSMC_UID_QE_PDMA0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_xiu_d0_busmc[] = {
	GOUT_BLK_BUSMC_UID_XIU_D0_BUSMC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_mmcache0_ns[] = {
	GOUT_BLK_BUSMC_UID_AD_APB_MMCACHE0_NS_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_lhm_acel_dc_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_DC_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_sfi[] = {
	GOUT_BLK_BUSMC_UID_LHM_AXI_D_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_aud[] = {
	GOUT_BLK_BUSMC_UID_LHM_AXI_D_AUD_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mmcache1[] = {
	GOUT_BLK_BUSMC_UID_MMCACHE1_IPCLKPORT_ACLK,
	GOUT_BLK_BUSMC_UID_MMCACHE1_IPCLKPORT_PCLK,
	GOUT_BLK_BUSMC_UID_MMCACHE1_IPCLKPORT_PCLK_SECURE,
	GOUT_BLK_BUSMC_UID_MMCACHE1_IPCLKPORT_PCLK_SECURE,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_busmc_dp[] = {
	GOUT_BLK_BUSMC_UID_LHS_AXI_D_BUSMC_DP_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_busmc[] = {
	GOUT_BLK_BUSMC_UID_LHM_AXI_P_BUSMC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d0_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D0_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d1_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D1_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d2_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D2_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d3_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D3_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d4_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D4_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d5_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D5_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_acel_d6_dnc[] = {
	GOUT_BLK_BUSMC_UID_LHM_ACEL_D6_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_busmc[] = {
	GOUT_BLK_BUSMC_UID_SYSMMU_BUSMC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_vgen_lite_apm[] = {
	GOUT_BLK_BUSMC_UID_VGEN_LITE_APM_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_sfi[] = {
	GOUT_BLK_BUSMC_UID_VGEN_SFI_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_sfi[] = {
	GOUT_BLK_BUSMC_UID_SYSMMU_SFI_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sfmpu_busmc[] = {
	GOUT_BLK_BUSMC_UID_SFMPU_BUSMC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_core_cmu_core[] = {
	CLK_BLK_CORE_UID_CORE_CMU_CORE_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_core[] = {
	GOUT_BLK_CORE_UID_SYSREG_CORE_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mpace2axi_0[] = {
	GOUT_BLK_CORE_UID_MPACE2AXI_0_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_mpace2axi_1[] = {
	GOUT_BLK_CORE_UID_MPACE2AXI_1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_ppc_debug_cci[] = {
	GOUT_BLK_CORE_UID_PPC_DEBUG_CCI_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_trex_p0_core[] = {
	GOUT_BLK_CORE_UID_TREX_P0_CORE_IPCLKPORT_ACLK_CORE,
	GOUT_BLK_CORE_UID_TREX_P0_CORE_IPCLKPORT_PCLK,
	GOUT_BLK_CORE_UID_TREX_P0_CORE_IPCLKPORT_PCLK_CORE,
};
enum clk_id cmucal_vclk_ip_ppmu_cpucl1_0[] = {
	GOUT_BLK_CORE_UID_PPMU_CPUCL1_0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPMU_CPUCL1_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g0_dmc0[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G0_DMC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g1_dmc0[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G1_DMC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g0_dmc1[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G0_DMC1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g1_dmc1[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G1_DMC1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t_bdu[] = {
	GOUT_BLK_CORE_UID_LHS_ATB_T_BDU_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_adm_apb_g_bdu[] = {
	GOUT_BLK_CORE_UID_ADM_APB_G_BDU_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_bdu[] = {
	GOUT_BLK_CORE_UID_BDU_IPCLKPORT_I_CLK,
	GOUT_BLK_CORE_UID_BDU_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_trex_p1_core[] = {
	GOUT_BLK_CORE_UID_TREX_P1_CORE_IPCLKPORT_PCLK,
	GOUT_BLK_CORE_UID_TREX_P1_CORE_IPCLKPORT_PCLK_CORE,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_g3d00[] = {
	GOUT_BLK_CORE_UID_LHS_AXI_P_G3D00_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_cpucl0[] = {
	GOUT_BLK_CORE_UID_LHS_AXI_P_CPUCL0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_cpucl1[] = {
	GOUT_BLK_CORE_UID_LHS_AXI_P_CPUCL1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d0_g3d1[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D0_G3D1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d1_g3d1[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D1_G3D1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d2_g3d1[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D2_G3D1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d3_g3d1[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D3_G3D1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_trex_d_core[] = {
	GOUT_BLK_CORE_UID_TREX_D_CORE_IPCLKPORT_PCLK,
	GOUT_BLK_CORE_UID_TREX_D_CORE_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppcfw_g3d0_0[] = {
	GOUT_BLK_CORE_UID_PPCFW_G3D0_0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPCFW_G3D0_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_apm[] = {
	GOUT_BLK_CORE_UID_LHS_AXI_P_APM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_cpucl1_1[] = {
	GOUT_BLK_CORE_UID_PPMU_CPUCL1_1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPMU_CPUCL1_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_core[] = {
	GOUT_BLK_CORE_UID_D_TZPC_CORE_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_cpucl1_0[] = {
	GOUT_BLK_CORE_UID_PPC_CPUCL1_0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_CPUCL1_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_cpucl1_1[] = {
	GOUT_BLK_CORE_UID_PPC_CPUCL1_1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_CPUCL1_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_g3d1_0[] = {
	GOUT_BLK_CORE_UID_PPC_G3D1_0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_G3D1_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_g3d1_1[] = {
	GOUT_BLK_CORE_UID_PPC_G3D1_1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_G3D1_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_g3d1_2[] = {
	GOUT_BLK_CORE_UID_PPC_G3D1_2_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_G3D1_2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_g3d1_3[] = {
	GOUT_BLK_CORE_UID_PPC_G3D1_3_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_G3D1_3_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_irps0[] = {
	GOUT_BLK_CORE_UID_PPC_IRPS0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_IRPS0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_irps1[] = {
	GOUT_BLK_CORE_UID_PPC_IRPS1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_IRPS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g_busc[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G_BUSC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mpace_asb_d0_mif[] = {
	GOUT_BLK_CORE_UID_MPACE_ASB_D0_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mpace_asb_d1_mif[] = {
	GOUT_BLK_CORE_UID_MPACE_ASB_D1_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mpace_asb_d2_mif[] = {
	GOUT_BLK_CORE_UID_MPACE_ASB_D2_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mpace_asb_d3_mif[] = {
	GOUT_BLK_CORE_UID_MPACE_ASB_D3_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_g_cssys[] = {
	GOUT_BLK_CORE_UID_LHM_AXI_G_CSSYS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_axi_asb_cssys[] = {
	GOUT_BLK_CORE_UID_AXI_ASB_CSSYS_IPCLKPORT_ACLKM,
};
enum clk_id cmucal_vclk_ip_cci[] = {
	CLK_BLK_CORE_UID_CCI_IPCLKPORT_PCLK,
	CLK_BLK_CORE_UID_CCI_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d0_cluster1[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D0_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d_g3d00[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D_G3D00_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d1_cluster1[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D1_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d_g3d01[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D_G3D01_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g0_dmc2[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G0_DMC2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g1_dmc2[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G1_DMC2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g0_dmc3[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G0_DMC3_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g1_dmc3[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G1_DMC3_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_g3d1[] = {
	GOUT_BLK_CORE_UID_LHS_AXI_P_G3D1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mpace_asb_d4_mif[] = {
	GOUT_BLK_CORE_UID_MPACE_ASB_D4_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mpace_asb_d5_mif[] = {
	GOUT_BLK_CORE_UID_MPACE_ASB_D5_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mpace_asb_d6_mif[] = {
	GOUT_BLK_CORE_UID_MPACE_ASB_D6_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mpace_asb_d7_mif[] = {
	GOUT_BLK_CORE_UID_MPACE_ASB_D7_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ppcfw_g3d1[] = {
	GOUT_BLK_CORE_UID_PPCFW_G3D1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPCFW_G3D1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_g3d0_0[] = {
	GOUT_BLK_CORE_UID_PPC_G3D0_0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_G3D0_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_g3d0_1[] = {
	GOUT_BLK_CORE_UID_PPC_G3D0_1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_G3D0_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_irps2[] = {
	GOUT_BLK_CORE_UID_PPC_IRPS2_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_IRPS2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_irps3[] = {
	GOUT_BLK_CORE_UID_PPC_IRPS3_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_IRPS3_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppcfw_g3d0_1[] = {
	GOUT_BLK_CORE_UID_PPCFW_G3D0_1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPCFW_G3D0_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_g3d01[] = {
	GOUT_BLK_CORE_UID_LHS_AXI_P_G3D01_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_dbg_g_busmc[] = {
	GOUT_BLK_CORE_UID_LHM_DBG_G_BUSMC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_cpucl0_0[] = {
	GOUT_BLK_CORE_UID_PPMU_CPUCL0_0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPMU_CPUCL0_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_cpucl0_1[] = {
	GOUT_BLK_CORE_UID_PPMU_CPUCL0_1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPMU_CPUCL0_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_cpucl0_0[] = {
	GOUT_BLK_CORE_UID_PPC_CPUCL0_0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_CPUCL0_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_cpucl0_1[] = {
	GOUT_BLK_CORE_UID_PPC_CPUCL0_1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPC_CPUCL0_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d0_cluster0[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D0_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ace_d1_cluster0[] = {
	GOUT_BLK_CORE_UID_LHM_ACE_D1_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d1_0[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D1_0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d1_1[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D1_1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d1_2[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D1_2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d1_3[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D1_3_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_g3d1_0[] = {
	GOUT_BLK_CORE_UID_PPMU_G3D1_0_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPMU_G3D1_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_g3d1_1[] = {
	GOUT_BLK_CORE_UID_PPMU_G3D1_1_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPMU_G3D1_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_g3d1_2[] = {
	GOUT_BLK_CORE_UID_PPMU_G3D1_2_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPMU_G3D1_2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_g3d1_3[] = {
	GOUT_BLK_CORE_UID_PPMU_G3D1_3_IPCLKPORT_ACLK,
	GOUT_BLK_CORE_UID_PPMU_G3D1_3_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_g3d0_0[] = {
	GOUT_BLK_CORE_UID_SYSMMU_G3D0_0_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_g3d0_1[] = {
	GOUT_BLK_CORE_UID_SYSMMU_G3D0_1_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_g3d1_0[] = {
	GOUT_BLK_CORE_UID_SYSMMU_G3D1_0_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_g3d1_1[] = {
	GOUT_BLK_CORE_UID_SYSMMU_G3D1_1_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_g3d1_2[] = {
	GOUT_BLK_CORE_UID_SYSMMU_G3D1_2_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_g3d1_3[] = {
	GOUT_BLK_CORE_UID_SYSMMU_G3D1_3_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_xiu_d_core[] = {
	GOUT_BLK_CORE_UID_XIU_D_CORE_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_apb_async_sysmmu_g3d0_0[] = {
	GOUT_BLK_CORE_UID_APB_ASYNC_SYSMMU_G3D0_0_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_apb_async_sysmmu_g3d0_1[] = {
	GOUT_BLK_CORE_UID_APB_ASYNC_SYSMMU_G3D0_1_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_apb_async_sysmmu_g3d1_0[] = {
	GOUT_BLK_CORE_UID_APB_ASYNC_SYSMMU_G3D1_0_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_apb_async_sysmmu_g3d1_1[] = {
	GOUT_BLK_CORE_UID_APB_ASYNC_SYSMMU_G3D1_1_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_apb_async_sysmmu_g3d1_2[] = {
	GOUT_BLK_CORE_UID_APB_ASYNC_SYSMMU_G3D1_2_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_apb_async_sysmmu_g3d1_3[] = {
	GOUT_BLK_CORE_UID_APB_ASYNC_SYSMMU_G3D1_3_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d0_0[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D0_0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d0_1[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D0_1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d1_1_0[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D1_1_0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d1_1_1[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D1_1_1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d1_1_2[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D1_1_2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ace_slice_g3d1_1_3[] = {
	GOUT_BLK_CORE_UID_ACE_SLICE_G3D1_1_3_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_g3d0_0_qos[] = {
	GOUT_BLK_CORE_UID_G3D0_0_QOS_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_little_qos_0[] = {
	GOUT_BLK_CORE_UID_LITTLE_QOS_0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_little_qos_1[] = {
	GOUT_BLK_CORE_UID_LITTLE_QOS_1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_big_qos_0[] = {
	GOUT_BLK_CORE_UID_BIG_QOS_0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_big_qos_1[] = {
	GOUT_BLK_CORE_UID_BIG_QOS_1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_g3d0_1_qos[] = {
	GOUT_BLK_CORE_UID_G3D0_1_QOS_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_g3d1_0_qos[] = {
	GOUT_BLK_CORE_UID_G3D1_0_QOS_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_g3d1_1_qos[] = {
	GOUT_BLK_CORE_UID_G3D1_1_QOS_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_g3d1_2_qos[] = {
	GOUT_BLK_CORE_UID_G3D1_2_QOS_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_g3d1_3_qos[] = {
	GOUT_BLK_CORE_UID_G3D1_3_QOS_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_sysreg_cpucl0[] = {
	GOUT_BLK_CPUCL0_UID_SYSREG_CPUCL0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_cssys[] = {
	GOUT_BLK_CPUCL0_UID_CSSYS_IPCLKPORT_PCLKDBG,
	GOUT_BLK_CPUCL0_UID_CSSYS_IPCLKPORT_ATCLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t_bdu[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T_BDU_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t0_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T0_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t0_cluster1[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T0_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t1_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T1_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t1_cluster1[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T1_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t2_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T2_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t3_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T3_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_secjtag[] = {
	GOUT_BLK_CPUCL0_UID_SECJTAG_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_cpucl0[] = {
	GOUT_BLK_CPUCL0_UID_LHM_AXI_P_CPUCL0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ace_d0_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHS_ACE_D0_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t0_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHS_ATB_T0_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t1_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHS_ATB_T1_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t2_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHS_ATB_T2_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t3_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHS_ATB_T3_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_cpucl0_cmu_cpucl0[] = {
	CLK_BLK_CPUCL0_UID_CPUCL0_CMU_CPUCL0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_cluster0[] = {
	CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_PERIPHCLK,
	CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_SCLK,
	CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_ATCLK,
	CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_ACLK,
	CLK_BLK_CPUCL0_UID_CLUSTER0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_cpucl0[] = {
	GOUT_BLK_CPUCL0_UID_D_TZPC_CPUCL0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_g_int_cssys[] = {
	GOUT_BLK_CPUCL0_UID_LHS_AXI_G_INT_CSSYS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_g_int_cssys[] = {
	GOUT_BLK_CPUCL0_UID_LHM_AXI_G_INT_CSSYS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_p_cpucl0[] = {
	GOUT_BLK_CPUCL0_UID_XIU_P_CPUCL0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_xiu_dp_cssys[] = {
	GOUT_BLK_CPUCL0_UID_XIU_DP_CSSYS_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_trex_cpucl0[] = {
	GOUT_BLK_CPUCL0_UID_TREX_CPUCL0_IPCLKPORT_CLK,
	GOUT_BLK_CPUCL0_UID_TREX_CPUCL0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_axi_us_32to64_g_dbgcore[] = {
	GOUT_BLK_CPUCL0_UID_AXI_US_32TO64_G_DBGCORE_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_g_cssys[] = {
	GOUT_BLK_CPUCL0_UID_LHS_AXI_G_CSSYS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_apb_async_p_cssys_0[] = {
	GOUT_BLK_CPUCL0_UID_APB_ASYNC_P_CSSYS_0_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_lhs_axi_g_int_etr[] = {
	GOUT_BLK_CPUCL0_UID_LHS_AXI_G_INT_ETR_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_g_int_etr[] = {
	GOUT_BLK_CPUCL0_UID_LHM_AXI_G_INT_ETR_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_bps_cpucl0[] = {
	GOUT_BLK_CPUCL0_UID_BPS_CPUCL0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ace_d1_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_LHS_ACE_D1_CLUSTER0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t2_cluster1[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T2_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_t3_cluster1[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_T3_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_dt0_sfi[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_DT0_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_dt1_sfi[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_DT1_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_it0_sfi[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_IT0_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_atb_it1_sfi[] = {
	GOUT_BLK_CPUCL0_UID_LHM_ATB_IT1_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_adm_apb_g_cluster0[] = {
	GOUT_BLK_CPUCL0_UID_ADM_APB_G_CLUSTER0_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_xiu_dp_dbgcore[] = {
	GOUT_BLK_CPUCL0_UID_XIU_DP_DBGCORE_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_g_int_stm[] = {
	GOUT_BLK_CPUCL0_UID_LHM_AXI_G_INT_STM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_g_int_stm[] = {
	GOUT_BLK_CPUCL0_UID_LHS_AXI_G_INT_STM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_g_int_dbgcore[] = {
	GOUT_BLK_CPUCL0_UID_LHM_AXI_G_INT_DBGCORE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_g_dbgcore[] = {
	GOUT_BLK_CPUCL0_UID_LHM_AXI_G_DBGCORE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_g_int_dbgcore[] = {
	GOUT_BLK_CPUCL0_UID_LHS_AXI_G_INT_DBGCORE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_hpm_apbif_cpucl0[] = {
	GOUT_BLK_CPUCL0_UID_HPM_APBIF_CPUCL0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_hpm_cpucl0[] = {
	CLK_BLK_CPUCL0_UID_HPM_CPUCL0_IPCLKPORT_HPM_TARGETCLK_C,
};
enum clk_id cmucal_vclk_ip_cluster1[] = {
	CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_ATCLK,
	CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_PERIPHCLK,
	CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_SCLK,
	CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_ACLK,
	CLK_BLK_CPUCL1_UID_CLUSTER1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_cpucl1_cmu_cpucl1[] = {
	CLK_BLK_CPUCL1_UID_CPUCL1_CMU_CPUCL1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_cpucl1[] = {
	GOUT_BLK_CPUCL1_UID_SYSREG_CPUCL1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t3_cluster1[] = {
	GOUT_BLK_CPUCL1_UID_LHS_ATB_T3_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t2_cluster1[] = {
	GOUT_BLK_CPUCL1_UID_LHS_ATB_T2_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t1_cluster1[] = {
	GOUT_BLK_CPUCL1_UID_LHS_ATB_T1_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_t0_cluster1[] = {
	GOUT_BLK_CPUCL1_UID_LHS_ATB_T0_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ace_d0_cluster1[] = {
	GOUT_BLK_CPUCL1_UID_LHS_ACE_D0_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_cpucl1[] = {
	GOUT_BLK_CPUCL1_UID_LHM_AXI_P_CPUCL1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_cpucl1[] = {
	GOUT_BLK_CPUCL1_UID_D_TZPC_CPUCL1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_ace_d1_cluster1[] = {
	GOUT_BLK_CPUCL1_UID_LHS_ACE_D1_CLUSTER1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_adm_apb_g_cluster1[] = {
	GOUT_BLK_CPUCL1_UID_ADM_APB_G_CLUSTER1_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_hpm_cpucl1[] = {
	CLK_BLK_CPUCL1_UID_HPM_CPUCL1_IPCLKPORT_HPM_TARGETCLK_C,
};
enum clk_id cmucal_vclk_ip_hpm_apbif_cpucl1[] = {
	GOUT_BLK_CPUCL1_UID_HPM_APBIF_CPUCL1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_npu00[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_D_NPU00_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_dnc_cmu_dnc[] = {
	CLK_BLK_DNC_UID_DNC_CMU_DNC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_dnc[] = {
	GOUT_BLK_DNC_UID_D_TZPC_DNC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d_npu0_unit1_done[] = {
	GOUT_BLK_DNC_UID_LHM_AST_D_NPU0_UNIT1_DONE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d_npu1_unit0_done[] = {
	GOUT_BLK_DNC_UID_LHM_AST_D_NPU1_UNIT0_DONE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d_npu1_unit1_done[] = {
	GOUT_BLK_DNC_UID_LHM_AST_D_NPU1_UNIT1_DONE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d_npu0_unit0_done[] = {
	GOUT_BLK_DNC_UID_LHM_AST_D_NPU0_UNIT0_DONE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d_npu0_unit1_setreg[] = {
	GOUT_BLK_DNC_UID_LHS_AST_D_NPU0_UNIT1_SETREG_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d_npu1_unit0_setreg[] = {
	GOUT_BLK_DNC_UID_LHS_AST_D_NPU1_UNIT0_SETREG_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d_npu1_unit1_setreg[] = {
	GOUT_BLK_DNC_UID_LHS_AST_D_NPU1_UNIT1_SETREG_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_npu01[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_D_NPU01_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_npu10[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_D_NPU10_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_npu11[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_D_NPU11_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d_npu0_unit0_setreg[] = {
	GOUT_BLK_DNC_UID_LHS_AST_D_NPU0_UNIT0_SETREG_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ip_npuc[] = {
	GOUT_BLK_DNC_UID_IP_NPUC_IPCLKPORT_I_PCLK,
	GOUT_BLK_DNC_UID_IP_NPUC_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_sysreg_dnc[] = {
	GOUT_BLK_DNC_UID_SYSREG_DNC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_dnc[] = {
	GOUT_BLK_DNC_UID_VGEN_LITE_DNC_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_trex_d_dnc[] = {
	GOUT_BLK_DNC_UID_TREX_D_DNC_IPCLKPORT_PCLK,
	GOUT_BLK_DNC_UID_TREX_D_DNC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_dspc_npuc0[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_DSPC_NPUC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_npuc_dspc0[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_NPUC_DSPC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ip_dspc[] = {
	GOUT_BLK_DNC_UID_IP_DSPC_IPCLKPORT_I_CLK_HIGH,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_d0_sdma0[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_D0_SDMA0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_d0_sdma1[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_D0_SDMA1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_d0_sdma2[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_D0_SDMA2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_d1_sdma0[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_D1_SDMA0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_d1_sdma1[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_D1_SDMA1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_d1_sdma2[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_D1_SDMA2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_d_sramdma[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_D_SRAMDMA_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d0[] = {
	GOUT_BLK_DNC_UID_PPMU_D0_IPCLKPORT_PCLK,
	GOUT_BLK_DNC_UID_PPMU_D0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d1[] = {
	GOUT_BLK_DNC_UID_PPMU_D1_IPCLKPORT_PCLK,
	GOUT_BLK_DNC_UID_PPMU_D1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d2[] = {
	GOUT_BLK_DNC_UID_PPMU_D2_IPCLKPORT_PCLK,
	GOUT_BLK_DNC_UID_PPMU_D2_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d3[] = {
	GOUT_BLK_DNC_UID_PPMU_D3_IPCLKPORT_PCLK,
	GOUT_BLK_DNC_UID_PPMU_D3_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d4[] = {
	GOUT_BLK_DNC_UID_PPMU_D4_IPCLKPORT_PCLK,
	GOUT_BLK_DNC_UID_PPMU_D4_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d5[] = {
	GOUT_BLK_DNC_UID_PPMU_D5_IPCLKPORT_PCLK,
	GOUT_BLK_DNC_UID_PPMU_D5_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_dc[] = {
	GOUT_BLK_DNC_UID_PPMU_DC_IPCLKPORT_PCLK,
	GOUT_BLK_DNC_UID_PPMU_DC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_d0_dncdma[] = {
	GOUT_BLK_DNC_UID_SYSMMU_D0_DNCDMA_IPCLKPORT_CLK_S1,
	GOUT_BLK_DNC_UID_SYSMMU_D0_DNCDMA_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d0_dncflc[] = {
	GOUT_BLK_DNC_UID_SYSMMU_D0_DNCFLC_IPCLKPORT_CLK_S1,
	GOUT_BLK_DNC_UID_SYSMMU_D0_DNCFLC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d1_dncdma[] = {
	GOUT_BLK_DNC_UID_SYSMMU_D1_DNCDMA_IPCLKPORT_CLK_S1,
	GOUT_BLK_DNC_UID_SYSMMU_D1_DNCDMA_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d1_dncflc[] = {
	GOUT_BLK_DNC_UID_SYSMMU_D1_DNCFLC_IPCLKPORT_CLK_S1,
	GOUT_BLK_DNC_UID_SYSMMU_D1_DNCFLC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d2_dncdma[] = {
	GOUT_BLK_DNC_UID_SYSMMU_D2_DNCDMA_IPCLKPORT_CLK_S1,
	GOUT_BLK_DNC_UID_SYSMMU_D2_DNCDMA_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d2_dncflc[] = {
	GOUT_BLK_DNC_UID_SYSMMU_D2_DNCFLC_IPCLKPORT_CLK_S1,
	GOUT_BLK_DNC_UID_SYSMMU_D2_DNCFLC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_dc_dnc[] = {
	GOUT_BLK_DNC_UID_SYSMMU_DC_DNC_IPCLKPORT_CLK_S1,
	GOUT_BLK_DNC_UID_SYSMMU_DC_DNC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_d0_btm_dnc[] = {
	GOUT_BLK_DNC_UID_D0_BTM_DNC_IPCLKPORT_I_PCLK,
	GOUT_BLK_DNC_UID_D0_BTM_DNC_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_dc_btm_dnc[] = {
	GOUT_BLK_DNC_UID_DC_BTM_DNC_IPCLKPORT_I_ACLK,
	GOUT_BLK_DNC_UID_DC_BTM_DNC_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_d1_btm_dnc[] = {
	GOUT_BLK_DNC_UID_D1_BTM_DNC_IPCLKPORT_I_PCLK,
	GOUT_BLK_DNC_UID_D1_BTM_DNC_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_d2_btm_dnc[] = {
	GOUT_BLK_DNC_UID_D2_BTM_DNC_IPCLKPORT_I_PCLK,
	GOUT_BLK_DNC_UID_D2_BTM_DNC_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_d3_btm_dnc[] = {
	GOUT_BLK_DNC_UID_D3_BTM_DNC_IPCLKPORT_I_PCLK,
	GOUT_BLK_DNC_UID_D3_BTM_DNC_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_d4_btm_dnc[] = {
	GOUT_BLK_DNC_UID_D4_BTM_DNC_IPCLKPORT_I_PCLK,
	GOUT_BLK_DNC_UID_D4_BTM_DNC_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_d5_btm_dnc[] = {
	GOUT_BLK_DNC_UID_D5_BTM_DNC_IPCLKPORT_I_PCLK,
	GOUT_BLK_DNC_UID_D5_BTM_DNC_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_npuc_dspc1[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_NPUC_DSPC1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_d1_dspc_wrapper[] = {
	GOUT_BLK_DNC_UID_XIU_D1_DSPC_WRAPPER_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_dncsram[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_D_DNCSRAM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d0_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_D0_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d1_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_D1_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d2_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_D2_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d3_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_D3_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d4_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_D4_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d5_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_D5_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d6_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_D6_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_dc_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_DC_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d7_dnc[] = {
	GOUT_BLK_DNC_UID_LHS_ACEL_D7_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_dspc0[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_DSPC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_npuc_dspc0[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_NPUC_DSPC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_npuc_dspc1[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_NPUC_DSPC1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_dspc0[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_DSPC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_d0_sdma0[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_D0_SDMA0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_d0_sdma1[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_D0_SDMA1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_d0_sdma2[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_D0_SDMA2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_d1_sdma0[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_D1_SDMA0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_d1_sdma1[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_D1_SDMA1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_d1_sdma2[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_D1_SDMA2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_d_sramdma[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_D_SRAMDMA_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_dap_async[] = {
	GOUT_BLK_DNC_UID_DAP_ASYNC_IPCLKPORT_DAPCLKM,
};
enum clk_id cmucal_vclk_ip_lhm_axi_int_dspc_npuc1[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_INT_DSPC_NPUC1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_dspc_npuc0[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_DSPC_NPUC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_int_dspc_npuc1[] = {
	GOUT_BLK_DNC_UID_LHS_AXI_INT_DSPC_NPUC1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_d0_dnc[] = {
	GOUT_BLK_DNC_UID_XIU_D0_DNC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_apb_async_smmu_s1_ns_dc[] = {
	GOUT_BLK_DNC_UID_APB_ASYNC_SMMU_S1_NS_DC_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_dnc[] = {
	GOUT_BLK_DNC_UID_LHM_AXI_P_DNC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_rs_d0_dncdma[] = {
	GOUT_BLK_DNC_UID_RS_D0_DNCDMA_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_rs_d0_dncflc[] = {
	GOUT_BLK_DNC_UID_RS_D0_DNCFLC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_rs_d1_dncdma[] = {
	GOUT_BLK_DNC_UID_RS_D1_DNCDMA_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_rs_d1_dncflc[] = {
	GOUT_BLK_DNC_UID_RS_D1_DNCFLC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_rs_d2_dncdma[] = {
	GOUT_BLK_DNC_UID_RS_D2_DNCDMA_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_rs_d2_dncflc[] = {
	GOUT_BLK_DNC_UID_RS_D2_DNCFLC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_rs_dc_dnc[] = {
	GOUT_BLK_DNC_UID_RS_DC_DNC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_sysreg_dptx[] = {
	GOUT_BLK_DPTX_UID_SYSREG_DPTX_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_dptx[] = {
	GOUT_BLK_DPTX_UID_LHM_AXI_P_DPTX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_dptx_top0[] = {
	GOUT_BLK_DPTX_UID_DPTX_TOP0_IPCLKPORT_I_PCLK,
	GOUT_BLK_DPTX_UID_DPTX_TOP0_IPCLKPORT_I_DP_GTC_CLK,
	GOUT_BLK_DPTX_UID_DPTX_TOP0_IPCLKPORT_I_APB_PCLK,
};
enum clk_id cmucal_vclk_ip_dptx_top1[] = {
	GOUT_BLK_DPTX_UID_DPTX_TOP1_IPCLKPORT_I_PCLK,
	GOUT_BLK_DPTX_UID_DPTX_TOP1_IPCLKPORT_I_DP_GTC_CLK,
	GOUT_BLK_DPTX_UID_DPTX_TOP1_IPCLKPORT_I_APB_PCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_dptx[] = {
	GOUT_BLK_DPTX_UID_D_TZPC_DPTX_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_dptx_cmu_dptx[] = {
	CLK_BLK_DPTX_UID_DPTX_CMU_DPTX_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_dpum_cmu_dpum[] = {
	CLK_BLK_DPUM_UID_DPUM_CMU_DPUM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d0_dpum[] = {
	GOUT_BLK_DPUM_UID_BTM_D0_DPUM_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUM_UID_BTM_D0_DPUM_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d1_dpum[] = {
	GOUT_BLK_DPUM_UID_BTM_D1_DPUM_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUM_UID_BTM_D1_DPUM_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_dpum[] = {
	GOUT_BLK_DPUM_UID_SYSREG_DPUM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_d0_dpum[] = {
	GOUT_BLK_DPUM_UID_SYSMMU_D0_DPUM_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUM_UID_SYSMMU_D0_DPUM_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_dpum[] = {
	GOUT_BLK_DPUM_UID_LHM_AXI_P_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d1_dpum[] = {
	GOUT_BLK_DPUM_UID_LHS_AXI_D1_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_dpp_dpum[] = {
	GOUT_BLK_DPUM_UID_AD_APB_DPP_DPUM_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d2_dpum[] = {
	GOUT_BLK_DPUM_UID_LHS_AXI_D2_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_btm_d2_dpum[] = {
	GOUT_BLK_DPUM_UID_BTM_D2_DPUM_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUM_UID_BTM_D2_DPUM_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_d2_dpum[] = {
	GOUT_BLK_DPUM_UID_SYSMMU_D2_DPUM_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUM_UID_SYSMMU_D2_DPUM_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d1_dpum[] = {
	GOUT_BLK_DPUM_UID_SYSMMU_D1_DPUM_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUM_UID_SYSMMU_D1_DPUM_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_ppmu_d0_dpum[] = {
	GOUT_BLK_DPUM_UID_PPMU_D0_DPUM_IPCLKPORT_ACLK,
	GOUT_BLK_DPUM_UID_PPMU_D0_DPUM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d1_dpum[] = {
	GOUT_BLK_DPUM_UID_PPMU_D1_DPUM_IPCLKPORT_ACLK,
	GOUT_BLK_DPUM_UID_PPMU_D1_DPUM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d2_dpum[] = {
	GOUT_BLK_DPUM_UID_PPMU_D2_DPUM_IPCLKPORT_ACLK,
	GOUT_BLK_DPUM_UID_PPMU_D2_DPUM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_wrapper_for_s5i6287_hsi_dphy_combo_top[] = {
	GOUT_BLK_DPUM_UID_WRAPPER_FOR_S5I6287_HSI_DPHY_COMBO_TOP_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d0_dpum[] = {
	GOUT_BLK_DPUM_UID_LHS_AXI_D0_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_dpum[] = {
	GOUT_BLK_DPUM_UID_DPUM_IPCLKPORT_ACLK_DECON,
	GOUT_BLK_DPUM_UID_DPUM_IPCLKPORT_ACLK_DMA,
	GOUT_BLK_DPUM_UID_DPUM_IPCLKPORT_ACLK_DPP,
};
enum clk_id cmucal_vclk_ip_d_tzpc_dpum[] = {
	GOUT_BLK_DPUM_UID_D_TZPC_DPUM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d3_dpum[] = {
	GOUT_BLK_DPUM_UID_BTM_D3_DPUM_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUM_UID_BTM_D3_DPUM_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d3_dpum[] = {
	GOUT_BLK_DPUM_UID_LHS_AXI_D3_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_d3_dpum[] = {
	GOUT_BLK_DPUM_UID_SYSMMU_D3_DPUM_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUM_UID_SYSMMU_D3_DPUM_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_ppmu_d3_dpum[] = {
	GOUT_BLK_DPUM_UID_PPMU_D3_DPUM_IPCLKPORT_ACLK,
	GOUT_BLK_DPUM_UID_PPMU_D3_DPUM_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sfmpu_dpum[] = {
	GOUT_BLK_DPUM_UID_SFMPU_DPUM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_dpus_cmu_dpus[] = {
	CLK_BLK_DPUS_UID_DPUS_CMU_DPUS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_dpus[] = {
	GOUT_BLK_DPUS_UID_SYSREG_DPUS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_d3_dpus[] = {
	GOUT_BLK_DPUS_UID_SYSMMU_D3_DPUS_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUS_UID_SYSMMU_D3_DPUS_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d2_dpus[] = {
	GOUT_BLK_DPUS_UID_SYSMMU_D2_DPUS_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUS_UID_SYSMMU_D2_DPUS_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d1_dpus[] = {
	GOUT_BLK_DPUS_UID_SYSMMU_D1_DPUS_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUS_UID_SYSMMU_D1_DPUS_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d0_dpus[] = {
	GOUT_BLK_DPUS_UID_SYSMMU_D0_DPUS_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUS_UID_SYSMMU_D0_DPUS_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_ppmu_d3_dpus[] = {
	GOUT_BLK_DPUS_UID_PPMU_D3_DPUS_IPCLKPORT_ACLK,
	GOUT_BLK_DPUS_UID_PPMU_D3_DPUS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d2_dpus[] = {
	GOUT_BLK_DPUS_UID_PPMU_D2_DPUS_IPCLKPORT_ACLK,
	GOUT_BLK_DPUS_UID_PPMU_D2_DPUS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d1_dpus[] = {
	GOUT_BLK_DPUS_UID_PPMU_D1_DPUS_IPCLKPORT_ACLK,
	GOUT_BLK_DPUS_UID_PPMU_D1_DPUS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d0_dpus[] = {
	GOUT_BLK_DPUS_UID_PPMU_D0_DPUS_IPCLKPORT_ACLK,
	GOUT_BLK_DPUS_UID_PPMU_D0_DPUS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_dpus[] = {
	GOUT_BLK_DPUS_UID_LHM_AXI_P_DPUS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d0_dpus[] = {
	GOUT_BLK_DPUS_UID_LHS_AXI_D0_DPUS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d1_dpus[] = {
	GOUT_BLK_DPUS_UID_LHS_AXI_D1_DPUS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d2_dpus[] = {
	GOUT_BLK_DPUS_UID_LHS_AXI_D2_DPUS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d3_dpus[] = {
	GOUT_BLK_DPUS_UID_LHS_AXI_D3_DPUS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_dpus[] = {
	GOUT_BLK_DPUS_UID_D_TZPC_DPUS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_dpus[] = {
	GOUT_BLK_DPUS_UID_DPUS_IPCLKPORT_ACLK_DECON,
	GOUT_BLK_DPUS_UID_DPUS_IPCLKPORT_ACLK_DMA,
	GOUT_BLK_DPUS_UID_DPUS_IPCLKPORT_ACLK_DPP,
};
enum clk_id cmucal_vclk_ip_btm_d0_dpus[] = {
	GOUT_BLK_DPUS_UID_BTM_D0_DPUS_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUS_UID_BTM_D0_DPUS_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d1_dpus[] = {
	GOUT_BLK_DPUS_UID_BTM_D1_DPUS_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUS_UID_BTM_D1_DPUS_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d2_dpus[] = {
	GOUT_BLK_DPUS_UID_BTM_D2_DPUS_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUS_UID_BTM_D2_DPUS_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d3_dpus[] = {
	GOUT_BLK_DPUS_UID_BTM_D3_DPUS_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUS_UID_BTM_D3_DPUS_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_dpp_dpus[] = {
	GOUT_BLK_DPUS_UID_AD_APB_DPP_DPUS_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_sfmpu_dpus[] = {
	GOUT_BLK_DPUS_UID_SFMPU_DPUS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_dpus1_cmu_dpus1[] = {
	CLK_BLK_DPUS1_UID_DPUS1_CMU_DPUS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_dpus1[] = {
	GOUT_BLK_DPUS1_UID_SYSREG_DPUS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_d3_dpus1[] = {
	GOUT_BLK_DPUS1_UID_SYSMMU_D3_DPUS1_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUS1_UID_SYSMMU_D3_DPUS1_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d2_dpus1[] = {
	GOUT_BLK_DPUS1_UID_SYSMMU_D2_DPUS1_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUS1_UID_SYSMMU_D2_DPUS1_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d1_dpus1[] = {
	GOUT_BLK_DPUS1_UID_SYSMMU_D1_DPUS1_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUS1_UID_SYSMMU_D1_DPUS1_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d0_dpus1[] = {
	GOUT_BLK_DPUS1_UID_SYSMMU_D0_DPUS1_IPCLKPORT_CLK_S1,
	GOUT_BLK_DPUS1_UID_SYSMMU_D0_DPUS1_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_ppmu_d3_dpus1[] = {
	GOUT_BLK_DPUS1_UID_PPMU_D3_DPUS1_IPCLKPORT_ACLK,
	GOUT_BLK_DPUS1_UID_PPMU_D3_DPUS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d2_dpus1[] = {
	GOUT_BLK_DPUS1_UID_PPMU_D2_DPUS1_IPCLKPORT_ACLK,
	GOUT_BLK_DPUS1_UID_PPMU_D2_DPUS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d1_dpus1[] = {
	GOUT_BLK_DPUS1_UID_PPMU_D1_DPUS1_IPCLKPORT_ACLK,
	GOUT_BLK_DPUS1_UID_PPMU_D1_DPUS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d0_dpus1[] = {
	GOUT_BLK_DPUS1_UID_PPMU_D0_DPUS1_IPCLKPORT_ACLK,
	GOUT_BLK_DPUS1_UID_PPMU_D0_DPUS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_dpus1[] = {
	GOUT_BLK_DPUS1_UID_LHM_AXI_P_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d0_dpus1[] = {
	GOUT_BLK_DPUS1_UID_LHS_AXI_D0_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d1_dpus1[] = {
	GOUT_BLK_DPUS1_UID_LHS_AXI_D1_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d2_dpus1[] = {
	GOUT_BLK_DPUS1_UID_LHS_AXI_D2_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d3_dpus1[] = {
	GOUT_BLK_DPUS1_UID_LHS_AXI_D3_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_dpus1[] = {
	GOUT_BLK_DPUS1_UID_D_TZPC_DPUS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_dpus1[] = {
	GOUT_BLK_DPUS1_UID_DPUS1_IPCLKPORT_ACLK_DECON,
	GOUT_BLK_DPUS1_UID_DPUS1_IPCLKPORT_ACLK_DMA,
	GOUT_BLK_DPUS1_UID_DPUS1_IPCLKPORT_ACLK_DPP,
};
enum clk_id cmucal_vclk_ip_btm_d0_dpus1[] = {
	GOUT_BLK_DPUS1_UID_BTM_D0_DPUS1_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUS1_UID_BTM_D0_DPUS1_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d1_dpus1[] = {
	GOUT_BLK_DPUS1_UID_BTM_D1_DPUS1_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUS1_UID_BTM_D1_DPUS1_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d2_dpus1[] = {
	GOUT_BLK_DPUS1_UID_BTM_D2_DPUS1_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUS1_UID_BTM_D2_DPUS1_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d3_dpus1[] = {
	GOUT_BLK_DPUS1_UID_BTM_D3_DPUS1_IPCLKPORT_I_ACLK,
	GOUT_BLK_DPUS1_UID_BTM_D3_DPUS1_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_dpp_dpus1[] = {
	GOUT_BLK_DPUS1_UID_AD_APB_DPP_DPUS1_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_sfmpu_dpus1[] = {
	GOUT_BLK_DPUS1_UID_SFMPU_DPUS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_fsys0_cmu_fsys0[] = {
	CLK_BLK_FSYS0_UID_FSYS0_CMU_FSYS0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d0_fsys0[] = {
	GOUT_BLK_FSYS0_UID_LHS_ACEL_D0_FSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_fsys0[] = {
	GOUT_BLK_FSYS0_UID_LHM_AXI_P_FSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_gpio_fsys0[] = {
	GOUT_BLK_FSYS0_UID_GPIO_FSYS0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_fsys0[] = {
	GOUT_BLK_FSYS0_UID_SYSREG_FSYS0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_fsys0[] = {
	GOUT_BLK_FSYS0_UID_XIU_D_FSYS0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_btm_d0_fsys0[] = {
	GOUT_BLK_FSYS0_UID_BTM_D0_FSYS0_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d0_pcie[] = {
	GOUT_BLK_FSYS0_UID_LHM_AXI_D0_PCIE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d0_fsys0[] = {
	GOUT_BLK_FSYS0_UID_PPMU_D0_FSYS0_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS0_UID_PPMU_D0_FSYS0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_xiu_p_fsys0[] = {
	GOUT_BLK_FSYS0_UID_XIU_P_FSYS0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_pcie_gen3_2l0[] = {
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_PIPE_PAL_PCIE_G3X2_INST_0_I_APB_PCLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_DBI_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_MSTR_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X1_PCIE_SUB_CTRL_INST_0_I_DRIVER_APB_CLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_SLV_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X1_DWC_PCIE_DM_INST_0_DBI_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X1_DWC_PCIE_DM_INST_0_MSTR_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X2_PCIE_SUB_CTRL_INST_0_I_DRIVER_APB_CLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X1_DWC_PCIE_DM_INST_0_SLV_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_QCHANNEL_WRAPPER_FOR_PCIE_PHY_S2121X2_INST_0_I_APB_PCLK,
	CLK_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X1_PCIE_SUB_CTRL_INST_0_PHY_REFCLK_IN,
	CLK_BLK_FSYS0_UID_PCIE_GEN3_2L0_IPCLKPORT_KITT_G3X2_PCIE_SUB_CTRL_INST_0_PHY_REFCLK_IN,
};
enum clk_id cmucal_vclk_ip_pcie_ia_gen3a_2l0[] = {
	GOUT_BLK_FSYS0_UID_PCIE_IA_GEN3A_2L0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_pcie_ia_gen3b_2l0[] = {
	GOUT_BLK_FSYS0_UID_PCIE_IA_GEN3B_2L0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_fsys0_0[] = {
	GOUT_BLK_FSYS0_UID_D_TZPC_FSYS0_0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_fsys0_pcie_slv[] = {
	GOUT_BLK_FSYS0_UID_XIU_D_FSYS0_PCIE_SLV_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d1_fsys0[] = {
	GOUT_BLK_FSYS0_UID_PPMU_D1_FSYS0_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS0_UID_PPMU_D1_FSYS0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_pcie_ia_gen3a_2l1[] = {
	GOUT_BLK_FSYS0_UID_PCIE_IA_GEN3A_2L1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_pcie_ia_gen3b_2l1[] = {
	GOUT_BLK_FSYS0_UID_PCIE_IA_GEN3B_2L1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_pcie_ia_gen3a_4l[] = {
	GOUT_BLK_FSYS0_UID_PCIE_IA_GEN3A_4L_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_pcie_ia_gen3b_4l[] = {
	GOUT_BLK_FSYS0_UID_PCIE_IA_GEN3B_4L_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_btm_d1_fsys0[] = {
	GOUT_BLK_FSYS0_UID_BTM_D1_FSYS0_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d1_pcie[] = {
	GOUT_BLK_FSYS0_UID_LHM_AXI_D1_PCIE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d1_fsys0[] = {
	GOUT_BLK_FSYS0_UID_LHS_ACEL_D1_FSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_pcie_gen3_4l[] = {
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_QCHANNEL_WRAPPER_FOR_PCIE_PHY_S2120X4_INST_0_I_APB_PCLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_PIPE_PAL_PCIE_G3X4_INST_0_I_APB_PCLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X4_PCIE_SUB_CTRL_INST_0_I_DRIVER_APB_CLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X4_DWC_PCIE_DM_INST_0_DBI_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X4_DWC_PCIE_DM_INST_0_MSTR_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X4_DWC_PCIE_DM_INST_0_SLV_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X2_PCIE_SUB_CTRL_INST_0_I_DRIVER_APB_CLK,
	CLK_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X4_PCIE_SUB_CTRL_INST_0_PHY_REFCLK_IN,
	CLK_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X2_PCIE_SUB_CTRL_INST_0_PHY_REFCLK_IN,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_DBI_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_MSTR_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_4L_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_SLV_ACLK_UG,
};
enum clk_id cmucal_vclk_ip_pcie_gen3_2l1[] = {
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_QCHANNEL_WRAPPER_FOR_PCIE_PHY_S2121X2_INST_0_I_APB_PCLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_PIPE_PAL_PCIE_G3X2_INST_0_I_APB_PCLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X2_PCIE_SUB_CTRL_INST_0_I_DRIVER_APB_CLK,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_DBI_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_MSTR_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X2_DWC_PCIE_DM_INST_0_SLV_ACLK_UG,
	CLK_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X2_PCIE_SUB_CTRL_INST_0_PHY_REFCLK_IN,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X1_PCIE_SUB_CTRL_INST_0_I_DRIVER_APB_CLK,
	CLK_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X1_PCIE_SUB_CTRL_INST_0_PHY_REFCLK_IN,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X1_DWC_PCIE_DM_INST_0_DBI_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X1_DWC_PCIE_DM_INST_0_MSTR_ACLK_UG,
	GOUT_BLK_FSYS0_UID_PCIE_GEN3_2L1_IPCLKPORT_KITT_G3X1_DWC_PCIE_DM_INST_0_SLV_ACLK_UG,
};
enum clk_id cmucal_vclk_ip_btm_d_fsys0sfi[] = {
	GOUT_BLK_FSYS0_UID_BTM_D_FSYS0SFI_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_sfifsys0[] = {
	GOUT_BLK_FSYS0_UID_LHM_AXI_P_SFIFSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_fsys0sfi[] = {
	GOUT_BLK_FSYS0_UID_LHS_AXI_D_FSYS0SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_fsys0_1[] = {
	GOUT_BLK_FSYS0_UID_D_TZPC_FSYS0_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_vgen_pcie_gen3a_2l0[] = {
	GOUT_BLK_FSYS0_UID_VGEN_PCIE_GEN3A_2L0_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_pcie_gen3a_2l1[] = {
	GOUT_BLK_FSYS0_UID_VGEN_PCIE_GEN3A_2L1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_pcie_gen3a_4l[] = {
	GOUT_BLK_FSYS0_UID_VGEN_PCIE_GEN3A_4L_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_pcie_gen3b_2l0[] = {
	GOUT_BLK_FSYS0_UID_VGEN_PCIE_GEN3B_2L0_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_pcie_gen3b_2l1[] = {
	GOUT_BLK_FSYS0_UID_VGEN_PCIE_GEN3B_2L1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_pcie_gen3b_4l[] = {
	GOUT_BLK_FSYS0_UID_VGEN_PCIE_GEN3B_4L_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d_fsys0sfi[] = {
	GOUT_BLK_FSYS0_UID_PPMU_D_FSYS0SFI_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS0_UID_PPMU_D_FSYS0SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_pcie_gen3a_2l0[] = {
	GOUT_BLK_FSYS0_UID_QE_PCIE_GEN3A_2L0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_qe_pcie_gen3a_2l1[] = {
	GOUT_BLK_FSYS0_UID_QE_PCIE_GEN3A_2L1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_qe_pcie_gen3a_4l[] = {
	GOUT_BLK_FSYS0_UID_QE_PCIE_GEN3A_4L_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_qe_pcie_gen3b_2l0[] = {
	GOUT_BLK_FSYS0_UID_QE_PCIE_GEN3B_2L0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_qe_pcie_gen3b_2l1[] = {
	GOUT_BLK_FSYS0_UID_QE_PCIE_GEN3B_2L1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_qe_pcie_gen3b_4l[] = {
	GOUT_BLK_FSYS0_UID_QE_PCIE_GEN3B_4L_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_fsys2fsys0[] = {
	GOUT_BLK_FSYS0_UID_LHM_AXI_D_FSYS2FSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_fsys0fsys2[] = {
	GOUT_BLK_FSYS0_UID_LHS_AXI_P_FSYS0FSYS2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_s2mpu_d0_fsys0[] = {
	GOUT_BLK_FSYS0_UID_S2MPU_D0_FSYS0_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_s2mpu_d1_fsys0[] = {
	GOUT_BLK_FSYS0_UID_S2MPU_D1_FSYS0_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_fsys1_cmu_fsys1[] = {
	GOUT_BLK_FSYS1_UID_FSYS1_CMU_FSYS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mmc_card[] = {
	GOUT_BLK_FSYS1_UID_MMC_CARD_IPCLKPORT_SDCLKIN,
	GOUT_BLK_FSYS1_UID_MMC_CARD_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_sysreg_fsys1[] = {
	GOUT_BLK_FSYS1_UID_SYSREG_FSYS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_gpio_fsys1[] = {
	GOUT_BLK_FSYS1_UID_GPIO_FSYS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d_fsys1[] = {
	GOUT_BLK_FSYS1_UID_LHS_ACEL_D_FSYS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_fsys1[] = {
	GOUT_BLK_FSYS1_UID_LHM_AXI_P_FSYS1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_fsys1[] = {
	GOUT_BLK_FSYS1_UID_XIU_D_FSYS1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_xiu_p_fsys1[] = {
	GOUT_BLK_FSYS1_UID_XIU_P_FSYS1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_fsys1[] = {
	GOUT_BLK_FSYS1_UID_PPMU_FSYS1_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS1_UID_PPMU_FSYS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_fsys1[] = {
	GOUT_BLK_FSYS1_UID_BTM_FSYS1_IPCLKPORT_I_ACLK,
	GOUT_BLK_FSYS1_UID_BTM_FSYS1_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_fsys1[] = {
	GOUT_BLK_FSYS1_UID_VGEN_LITE_FSYS1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_fsys1[] = {
	GOUT_BLK_FSYS1_UID_D_TZPC_FSYS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_usb20drd_0[] = {
	GOUT_BLK_FSYS1_UID_USB20DRD_0_IPCLKPORT_I_USB20DRD_REF_CLK_40,
	GOUT_BLK_FSYS1_UID_USB20DRD_0_IPCLKPORT_BUS_CLK_EARLY,
	GOUT_BLK_FSYS1_UID_USB20DRD_0_IPCLKPORT_ACLK_PHYCTRL_20,
};
enum clk_id cmucal_vclk_ip_usb20drd_1[] = {
	GOUT_BLK_FSYS1_UID_USB20DRD_1_IPCLKPORT_I_USB20DRD_REF_CLK_40,
	GOUT_BLK_FSYS1_UID_USB20DRD_1_IPCLKPORT_BUS_CLK_EARLY,
	GOUT_BLK_FSYS1_UID_USB20DRD_1_IPCLKPORT_ACLK_PHYCTRL_20,
};
enum clk_id cmucal_vclk_ip_usb30drd_0[] = {
	GOUT_BLK_FSYS1_UID_USB30DRD_0_IPCLKPORT_I_USB30DRD_REF_CLK_40,
	GOUT_BLK_FSYS1_UID_USB30DRD_0_IPCLKPORT_BUS_CLK_EARLY,
	GOUT_BLK_FSYS1_UID_USB30DRD_0_IPCLKPORT_ACLK_PHYCTRL,
	CLK_BLK_FSYS1_UID_USB30DRD_0_IPCLKPORT_I_USB30DRD_SUSPEND_CLK_26,
};
enum clk_id cmucal_vclk_ip_usb30drd_1[] = {
	GOUT_BLK_FSYS1_UID_USB30DRD_1_IPCLKPORT_I_USB30DRD_REF_CLK_40,
	GOUT_BLK_FSYS1_UID_USB30DRD_1_IPCLKPORT_BUS_CLK_EARLY,
	GOUT_BLK_FSYS1_UID_USB30DRD_1_IPCLKPORT_ACLK_PHYCTRL,
	CLK_BLK_FSYS1_UID_USB30DRD_1_IPCLKPORT_I_USB30DRD_SUSPEND_CLK_26,
};
enum clk_id cmucal_vclk_ip_us_d_usb3_0[] = {
	GOUT_BLK_FSYS1_UID_US_D_USB3_0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_us_d_usb3_1[] = {
	GOUT_BLK_FSYS1_UID_US_D_USB3_1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_us_d_usb2_0[] = {
	GOUT_BLK_FSYS1_UID_US_D_USB2_0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_us_d_usb2_1[] = {
	GOUT_BLK_FSYS1_UID_US_D_USB2_1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_s2mpu_d_fsys1[] = {
	GOUT_BLK_FSYS1_UID_S2MPU_D_FSYS1_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_fsys2_cmu_fsys2[] = {
	CLK_BLK_FSYS2_UID_FSYS2_CMU_FSYS2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_fsys0fsys2[] = {
	GOUT_BLK_FSYS2_UID_LHM_AXI_P_FSYS0FSYS2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_btm_d_fsys2fsys0[] = {
	GOUT_BLK_FSYS2_UID_BTM_D_FSYS2FSYS0_IPCLKPORT_I_ACLK,
	GOUT_BLK_FSYS2_UID_BTM_D_FSYS2FSYS0_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d_fsys2[] = {
	GOUT_BLK_FSYS2_UID_BTM_D_FSYS2_IPCLKPORT_I_ACLK,
	GOUT_BLK_FSYS2_UID_BTM_D_FSYS2_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_fsys2[] = {
	GOUT_BLK_FSYS2_UID_D_TZPC_FSYS2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ethernet0[] = {
	GOUT_BLK_FSYS2_UID_ETHERNET0_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_ETHERNET0_IPCLKPORT_PCLK_S0,
	GOUT_BLK_FSYS2_UID_ETHERNET0_IPCLKPORT_SYS_CLK,
	GOUT_BLK_FSYS2_UID_ETHERNET0_IPCLKPORT_PCLK_S1,
};
enum clk_id cmucal_vclk_ip_ethernet1[] = {
	GOUT_BLK_FSYS2_UID_ETHERNET1_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_ETHERNET1_IPCLKPORT_PCLK_S0,
	GOUT_BLK_FSYS2_UID_ETHERNET1_IPCLKPORT_SYS_CLK,
	GOUT_BLK_FSYS2_UID_ETHERNET1_IPCLKPORT_PCLK_S1,
};
enum clk_id cmucal_vclk_ip_gpio_fsys2[] = {
	GOUT_BLK_FSYS2_UID_GPIO_FSYS2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_fsys2fsys0[] = {
	GOUT_BLK_FSYS2_UID_LHS_AXI_D_FSYS2FSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d_fsys2[] = {
	GOUT_BLK_FSYS2_UID_LHS_ACEL_D_FSYS2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_qe_ethernet0[] = {
	GOUT_BLK_FSYS2_UID_QE_ETHERNET0_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_QE_ETHERNET0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_ethernet1[] = {
	GOUT_BLK_FSYS2_UID_QE_ETHERNET1_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_QE_ETHERNET1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_ufs_embd0[] = {
	GOUT_BLK_FSYS2_UID_QE_UFS_EMBD0_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_QE_UFS_EMBD0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_ufs_embd1[] = {
	GOUT_BLK_FSYS2_UID_QE_UFS_EMBD1_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_QE_UFS_EMBD1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_ethernet[] = {
	GOUT_BLK_FSYS2_UID_SYSMMU_ETHERNET_IPCLKPORT_CLK_S1,
};
enum clk_id cmucal_vclk_ip_s2mpu_d_fsys2[] = {
	GOUT_BLK_FSYS2_UID_S2MPU_D_FSYS2_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysreg_fsys2[] = {
	GOUT_BLK_FSYS2_UID_SYSREG_FSYS2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ufs_embd0[] = {
	GOUT_BLK_FSYS2_UID_UFS_EMBD0_IPCLKPORT_I_ACLK,
	GOUT_BLK_FSYS2_UID_UFS_EMBD0_IPCLKPORT_I_CLK_UNIPRO,
	GOUT_BLK_FSYS2_UID_UFS_EMBD0_IPCLKPORT_I_FMP_CLK,
};
enum clk_id cmucal_vclk_ip_ufs_embd1[] = {
	GOUT_BLK_FSYS2_UID_UFS_EMBD1_IPCLKPORT_I_ACLK,
	GOUT_BLK_FSYS2_UID_UFS_EMBD1_IPCLKPORT_I_CLK_UNIPRO,
	GOUT_BLK_FSYS2_UID_UFS_EMBD1_IPCLKPORT_I_FMP_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_ethernet0[] = {
	GOUT_BLK_FSYS2_UID_VGEN_ETHERNET0_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_fsys2[] = {
	GOUT_BLK_FSYS2_UID_XIU_D_FSYS2_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_fsys2_eth[] = {
	GOUT_BLK_FSYS2_UID_XIU_D_FSYS2_ETH_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_fsys2[] = {
	GOUT_BLK_FSYS2_UID_LHM_AXI_P_FSYS2_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_ethernet1[] = {
	GOUT_BLK_FSYS2_UID_VGEN_ETHERNET1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_ethernet[] = {
	GOUT_BLK_FSYS2_UID_PPMU_ETHERNET_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_PPMU_ETHERNET_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_xiu_p_fsys2[] = {
	GOUT_BLK_FSYS2_UID_XIU_P_FSYS2_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppmu_ufs_embd0[] = {
	GOUT_BLK_FSYS2_UID_PPMU_UFS_EMBD0_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_PPMU_UFS_EMBD0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_ufs_embd1[] = {
	GOUT_BLK_FSYS2_UID_PPMU_UFS_EMBD1_IPCLKPORT_ACLK,
	GOUT_BLK_FSYS2_UID_PPMU_UFS_EMBD1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_g2d_cmu_g2d[] = {
	CLK_BLK_G2D_UID_G2D_CMU_G2D_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_g2dd0[] = {
	GOUT_BLK_G2D_UID_PPMU_G2DD0_IPCLKPORT_ACLK,
	GOUT_BLK_G2D_UID_PPMU_G2DD0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_g2dd1[] = {
	GOUT_BLK_G2D_UID_PPMU_G2DD1_IPCLKPORT_ACLK,
	GOUT_BLK_G2D_UID_PPMU_G2DD1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_g2dd0[] = {
	GOUT_BLK_G2D_UID_SYSMMU_G2DD0_IPCLKPORT_CLK_S1,
	GOUT_BLK_G2D_UID_SYSMMU_G2DD0_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysreg_g2d[] = {
	GOUT_BLK_G2D_UID_SYSREG_G2D_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d0_g2d[] = {
	GOUT_BLK_G2D_UID_LHS_ACEL_D0_G2D_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d1_g2d[] = {
	GOUT_BLK_G2D_UID_LHS_ACEL_D1_G2D_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_g2d[] = {
	GOUT_BLK_G2D_UID_LHM_AXI_P_G2D_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_as_p_g2d[] = {
	GOUT_BLK_G2D_UID_AS_P_G2D_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_btm_g2dd0[] = {
	GOUT_BLK_G2D_UID_BTM_G2DD0_IPCLKPORT_I_ACLK,
	GOUT_BLK_G2D_UID_BTM_G2DD0_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_g2dd1[] = {
	GOUT_BLK_G2D_UID_BTM_G2DD1_IPCLKPORT_I_ACLK,
	GOUT_BLK_G2D_UID_BTM_G2DD1_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_g2dd2[] = {
	GOUT_BLK_G2D_UID_BTM_G2DD2_IPCLKPORT_I_ACLK,
	GOUT_BLK_G2D_UID_BTM_G2DD2_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_jpeg[] = {
	GOUT_BLK_G2D_UID_QE_JPEG_IPCLKPORT_ACLK,
	GOUT_BLK_G2D_UID_QE_JPEG_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qe_mscl[] = {
	GOUT_BLK_G2D_UID_QE_MSCL_IPCLKPORT_ACLK,
	GOUT_BLK_G2D_UID_QE_MSCL_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_g2dd2[] = {
	GOUT_BLK_G2D_UID_SYSMMU_G2DD2_IPCLKPORT_CLK_S1,
	GOUT_BLK_G2D_UID_SYSMMU_G2DD2_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_ppmu_g2dd2[] = {
	GOUT_BLK_G2D_UID_PPMU_G2DD2_IPCLKPORT_ACLK,
	GOUT_BLK_G2D_UID_PPMU_G2DD2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_acel_d2_g2d[] = {
	GOUT_BLK_G2D_UID_LHS_ACEL_D2_G2D_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_g2d[] = {
	GOUT_BLK_G2D_UID_XIU_D_G2D_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_g2d[] = {
	GOUT_BLK_G2D_UID_VGEN_LITE_G2D_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_g2d[] = {
	GOUT_BLK_G2D_UID_G2D_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_g2dd1[] = {
	GOUT_BLK_G2D_UID_SYSMMU_G2DD1_IPCLKPORT_CLK_S1,
	GOUT_BLK_G2D_UID_SYSMMU_G2DD1_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_jpeg[] = {
	GOUT_BLK_G2D_UID_JPEG_IPCLKPORT_I_SMFC_CLK,
};
enum clk_id cmucal_vclk_ip_mscl[] = {
	GOUT_BLK_G2D_UID_MSCL_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_g2d[] = {
	GOUT_BLK_G2D_UID_D_TZPC_G2D_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_as_p_mscl[] = {
	GOUT_BLK_G2D_UID_AS_P_MSCL_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_g3d00_cmu_g3d00[] = {
	CLK_BLK_G3D00_UID_G3D00_CMU_G3D00_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_g3d00[] = {
	GOUT_BLK_G3D00_UID_VGEN_LITE_G3D00_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_sysreg_g3d00[] = {
	GOUT_BLK_G3D00_UID_SYSREG_G3D00_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_int_g3d00[] = {
	GOUT_BLK_G3D00_UID_LHS_AXI_P_INT_G3D00_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ace_d_g3d00[] = {
	GOUT_BLK_G3D00_UID_LHS_ACE_D_G3D00_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_g3d00[] = {
	GOUT_BLK_G3D00_UID_LHM_AXI_P_G3D00_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_int_g3d00[] = {
	GOUT_BLK_G3D00_UID_LHM_AXI_P_INT_G3D00_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_gray2bin_g3d00[] = {
	GOUT_BLK_G3D00_UID_GRAY2BIN_G3D00_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_gpu00[] = {
	CLK_BLK_G3D00_UID_GPU00_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_g3d00[] = {
	GOUT_BLK_G3D00_UID_D_TZPC_G3D00_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_g3d00[] = {
	GOUT_BLK_G3D00_UID_PPMU_G3D00_IPCLKPORT_PCLK,
	GOUT_BLK_G3D00_UID_PPMU_G3D00_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_g3d01_cmu_g3d01[] = {
	CLK_BLK_G3D01_UID_G3D01_CMU_G3D01_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_g3d01[] = {
	GOUT_BLK_G3D01_UID_VGEN_LITE_G3D01_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_sysreg_g3d01[] = {
	GOUT_BLK_G3D01_UID_SYSREG_G3D01_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_int_g3d01[] = {
	GOUT_BLK_G3D01_UID_LHS_AXI_P_INT_G3D01_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ace_d_g3d01[] = {
	GOUT_BLK_G3D01_UID_LHS_ACE_D_G3D01_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_g3d01[] = {
	GOUT_BLK_G3D01_UID_LHM_AXI_P_G3D01_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_int_g3d01[] = {
	GOUT_BLK_G3D01_UID_LHM_AXI_P_INT_G3D01_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_gray2bin_g3d01[] = {
	GOUT_BLK_G3D01_UID_GRAY2BIN_G3D01_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_gpu01[] = {
	CLK_BLK_G3D01_UID_GPU01_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_g3d01[] = {
	GOUT_BLK_G3D01_UID_D_TZPC_G3D01_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_g3d01[] = {
	GOUT_BLK_G3D01_UID_PPMU_G3D01_IPCLKPORT_PCLK,
	GOUT_BLK_G3D01_UID_PPMU_G3D01_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_g3d1[] = {
	GOUT_BLK_G3D1_UID_LHM_AXI_P_G3D1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sysreg_g3d1[] = {
	GOUT_BLK_G3D1_UID_SYSREG_G3D1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_g3d1_cmu_g3d1[] = {
	CLK_BLK_G3D1_UID_G3D1_CMU_G3D1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_int_g3d1[] = {
	GOUT_BLK_G3D1_UID_LHS_AXI_P_INT_G3D1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_vgen_lite_g3d1[] = {
	GOUT_BLK_G3D1_UID_VGEN_LITE_G3D1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_gpu1[] = {
	CLK_BLK_G3D1_UID_GPU1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_int_g3d1[] = {
	GOUT_BLK_G3D1_UID_LHM_AXI_P_INT_G3D1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_gray2bin_g3d1[] = {
	GOUT_BLK_G3D1_UID_GRAY2BIN_G3D1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_g3d1[] = {
	GOUT_BLK_G3D1_UID_D_TZPC_G3D1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_ispb[] = {
	GOUT_BLK_ISPB_UID_LHM_AXI_P_ISPB_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_ispb[] = {
	GOUT_BLK_ISPB_UID_LHS_AXI_D_ISPB_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ispb[] = {
	GOUT_BLK_ISPB_UID_ISPB_IPCLKPORT_CLK,
	GOUT_BLK_ISPB_UID_ISPB_IPCLKPORT_C2CLK,
};
enum clk_id cmucal_vclk_ip_sysreg_ispb[] = {
	GOUT_BLK_ISPB_UID_SYSREG_ISPB_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ispb_cmu_ispb[] = {
	CLK_BLK_ISPB_UID_ISPB_CMU_ISPB_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_c2c_taaispb[] = {
	GOUT_BLK_ISPB_UID_LHM_AST_C2C_TAAISPB_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_c2c_ispbacc[] = {
	GOUT_BLK_ISPB_UID_LHS_AST_C2C_ISPBACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_btm_ispb[] = {
	GOUT_BLK_ISPB_UID_BTM_ISPB_IPCLKPORT_I_ACLK,
	GOUT_BLK_ISPB_UID_BTM_ISPB_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_otf_taaispb[] = {
	GOUT_BLK_ISPB_UID_LHM_AST_OTF_TAAISPB_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_ispb[] = {
	GOUT_BLK_ISPB_UID_D_TZPC_ISPB_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_otf_ispbacc[] = {
	GOUT_BLK_ISPB_UID_LHS_AST_OTF_ISPBACC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_ispb[] = {
	GOUT_BLK_ISPB_UID_SYSMMU_ISPB_IPCLKPORT_CLK_S1,
	GOUT_BLK_ISPB_UID_SYSMMU_ISPB_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_vgen_lite_ispb[] = {
	GOUT_BLK_ISPB_UID_VGEN_LITE_ISPB_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_ispb[] = {
	GOUT_BLK_ISPB_UID_PPMU_ISPB_IPCLKPORT_ACLK,
	GOUT_BLK_ISPB_UID_PPMU_ISPB_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_as_apb_ispb_ispb0[] = {
	GOUT_BLK_ISPB_UID_AS_APB_ISPB_ISPB0_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_ispb_sysmmu_s1_ns[] = {
	GOUT_BLK_ISPB_UID_AS_APB_ISPB_SYSMMU_S1_NS_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_ispb_sysmmu_s1_s[] = {
	GOUT_BLK_ISPB_UID_AS_APB_ISPB_SYSMMU_S1_S_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_ispb_ispb1[] = {
	GOUT_BLK_ISPB_UID_AS_APB_ISPB_ISPB1_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_ispb_ispb2[] = {
	GOUT_BLK_ISPB_UID_AS_APB_ISPB_ISPB2_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_ispb_sysmmu_s2[] = {
	GOUT_BLK_ISPB_UID_AS_APB_ISPB_SYSMMU_S2_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_ispb_ispb3[] = {
	GOUT_BLK_ISPB_UID_AS_APB_ISPB_ISPB3_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_sysmmu_d0_mfc_s2[] = {
	GOUT_BLK_MFC_UID_AS_APB_SYSMMU_D0_MFC_S2_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_sysmmu_d1_mfc_s2[] = {
	GOUT_BLK_MFC_UID_AS_APB_SYSMMU_D1_MFC_S2_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_mfc_cmu_mfc[] = {
	CLK_BLK_MFC_UID_MFC_CMU_MFC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_as_apb_mfc[] = {
	GOUT_BLK_MFC_UID_AS_APB_MFC_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_sysreg_mfc[] = {
	GOUT_BLK_MFC_UID_SYSREG_MFC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d0_mfc[] = {
	GOUT_BLK_MFC_UID_LHS_AXI_D0_MFC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d1_mfc[] = {
	GOUT_BLK_MFC_UID_LHS_AXI_D1_MFC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_mfc[] = {
	GOUT_BLK_MFC_UID_LHM_AXI_P_MFC_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_d0_mfc[] = {
	GOUT_BLK_MFC_UID_SYSMMU_D0_MFC_IPCLKPORT_CLK_S1,
	GOUT_BLK_MFC_UID_SYSMMU_D0_MFC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_sysmmu_d1_mfc[] = {
	GOUT_BLK_MFC_UID_SYSMMU_D1_MFC_IPCLKPORT_CLK_S1,
	GOUT_BLK_MFC_UID_SYSMMU_D1_MFC_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_ppmu_d0_mfc[] = {
	GOUT_BLK_MFC_UID_PPMU_D0_MFC_IPCLKPORT_ACLK,
	GOUT_BLK_MFC_UID_PPMU_D0_MFC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_d1_mfc[] = {
	GOUT_BLK_MFC_UID_PPMU_D1_MFC_IPCLKPORT_ACLK,
	GOUT_BLK_MFC_UID_PPMU_D1_MFC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d0_mfc[] = {
	GOUT_BLK_MFC_UID_BTM_D0_MFC_IPCLKPORT_I_ACLK,
	GOUT_BLK_MFC_UID_BTM_D0_MFC_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_btm_d1_mfc[] = {
	GOUT_BLK_MFC_UID_BTM_D1_MFC_IPCLKPORT_I_ACLK,
	GOUT_BLK_MFC_UID_BTM_D1_MFC_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_as_apb_sysmmu_d0_mfc_n[] = {
	GOUT_BLK_MFC_UID_AS_APB_SYSMMU_D0_MFC_N_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_sysmmu_d1_mfc_n[] = {
	GOUT_BLK_MFC_UID_AS_APB_SYSMMU_D1_MFC_N_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_sysmmu_d0_mfc_s[] = {
	GOUT_BLK_MFC_UID_AS_APB_SYSMMU_D0_MFC_S_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_sysmmu_d1_mfc_s[] = {
	GOUT_BLK_MFC_UID_AS_APB_SYSMMU_D1_MFC_S_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_axi_wfd[] = {
	GOUT_BLK_MFC_UID_AS_AXI_WFD_IPCLKPORT_ACLKS,
	GOUT_BLK_MFC_UID_AS_AXI_WFD_IPCLKPORT_ACLKM,
};
enum clk_id cmucal_vclk_ip_ppmu_d2_mfc[] = {
	GOUT_BLK_MFC_UID_PPMU_D2_MFC_IPCLKPORT_PCLK,
	GOUT_BLK_MFC_UID_PPMU_D2_MFC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_xiu_d_mfc[] = {
	GOUT_BLK_MFC_UID_XIU_D_MFC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_as_apb_wfd_ns[] = {
	GOUT_BLK_MFC_UID_AS_APB_WFD_NS_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_as_apb_wfd_s[] = {
	GOUT_BLK_MFC_UID_AS_APB_WFD_S_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_vgen_lite_mfc[] = {
	GOUT_BLK_MFC_UID_VGEN_LITE_MFC_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_mfc[] = {
	GOUT_BLK_MFC_UID_MFC_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_wfd[] = {
	GOUT_BLK_MFC_UID_WFD_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_lh_atb_mfc[] = {
	GOUT_BLK_MFC_UID_LH_ATB_MFC_IPCLKPORT_I_CLK_SI,
	GOUT_BLK_MFC_UID_LH_ATB_MFC_IPCLKPORT_I_CLK_MI,
};
enum clk_id cmucal_vclk_ip_d_tzpc_mfc[] = {
	GOUT_BLK_MFC_UID_D_TZPC_MFC_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mif_cmu_mif[] = {
	CLK_BLK_MIF_UID_MIF_CMU_MIF_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ddrphy0[] = {
	GOUT_BLK_MIF_UID_DDRPHY0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_mif[] = {
	GOUT_BLK_MIF_UID_SYSREG_MIF_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_mif[] = {
	GOUT_BLK_MIF_UID_LHM_AXI_P_MIF_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_axi2apb_mif[] = {
	GOUT_BLK_MIF_UID_AXI2APB_MIF_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppc_dvfs0[] = {
	GOUT_BLK_MIF_UID_PPC_DVFS0_IPCLKPORT_PCLK,
	CLK_BLK_MIF_UID_PPC_DVFS0_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_ppc_debug0[] = {
	GOUT_BLK_MIF_UID_PPC_DEBUG0_IPCLKPORT_PCLK,
	CLK_BLK_MIF_UID_PPC_DEBUG0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_apbbr_ddrphy0[] = {
	GOUT_BLK_MIF_UID_APBBR_DDRPHY0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_apbbr_dmc0[] = {
	GOUT_BLK_MIF_UID_APBBR_DMC0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_apbbr_dmctz0[] = {
	GOUT_BLK_MIF_UID_APBBR_DMCTZ0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_dmc0[] = {
	GOUT_BLK_MIF_UID_DMC0_IPCLKPORT_PCLK1,
	GOUT_BLK_MIF_UID_DMC0_IPCLKPORT_PCLK2,
};
enum clk_id cmucal_vclk_ip_qch_adapter_ppc_debug0[] = {
	GOUT_BLK_MIF_UID_QCH_ADAPTER_PPC_DEBUG0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qch_adapter_ppc_dvfs0[] = {
	GOUT_BLK_MIF_UID_QCH_ADAPTER_PPC_DVFS0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_mif[] = {
	GOUT_BLK_MIF_UID_D_TZPC_MIF_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_dmc1[] = {
	GOUT_BLK_MIF_UID_DMC1_IPCLKPORT_PCLK1,
	GOUT_BLK_MIF_UID_DMC1_IPCLKPORT_PCLK2,
};
enum clk_id cmucal_vclk_ip_apbbr_ddrphy1[] = {
	GOUT_BLK_MIF_UID_APBBR_DDRPHY1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_apbbr_dmc1[] = {
	GOUT_BLK_MIF_UID_APBBR_DMC1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_apbbr_dmctz1[] = {
	GOUT_BLK_MIF_UID_APBBR_DMCTZ1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ddrphy1[] = {
	GOUT_BLK_MIF_UID_DDRPHY1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppc_debug1[] = {
	GOUT_BLK_MIF_UID_PPC_DEBUG1_IPCLKPORT_PCLK,
	CLK_BLK_MIF_UID_PPC_DEBUG1_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ppc_dvfs1[] = {
	GOUT_BLK_MIF_UID_PPC_DVFS1_IPCLKPORT_PCLK,
	CLK_BLK_MIF_UID_PPC_DVFS1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_qch_adapter_ppc_debug1[] = {
	GOUT_BLK_MIF_UID_QCH_ADAPTER_PPC_DEBUG1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_qch_adapter_ppc_dvfs1[] = {
	GOUT_BLK_MIF_UID_QCH_ADAPTER_PPC_DVFS1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sfmpu[] = {
	GOUT_BLK_MIF_UID_SFMPU_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_npu_cmu_npu[] = {
	CLK_BLK_NPU_UID_NPU_CMU_NPU_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d1_npuh[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D1_NPUH_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_npu[] = {
	GOUT_BLK_NPU_UID_LHM_AXI_P_NPU_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_axi2apb_npu[] = {
	GOUT_BLK_NPU_UID_AXI2APB_NPU_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_npu[] = {
	GOUT_BLK_NPU_UID_D_TZPC_NPU_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d2_npuh[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D2_NPUH_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d3_npuh[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D3_NPUH_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d0_npuv[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D0_NPUV_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d1_npuv[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D1_NPUV_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d2_npuv[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D2_NPUV_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d3_npuv[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D3_NPUV_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d0_npux[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D0_NPUX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d0_npuh[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D0_NPUH_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d1_npux[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D1_NPUX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d2_npux[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D2_NPUX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d0_npuv[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D0_NPUV_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d0_npux[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D0_NPUX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d1_npuh[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D1_NPUH_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d1_npuv[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D1_NPUV_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d1_npux[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D1_NPUX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d2_npuh[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D2_NPUH_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d2_npuv[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D2_NPUV_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sysreg_npu[] = {
	GOUT_BLK_NPU_UID_SYSREG_NPU_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d0_npuh[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D0_NPUH_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_npud_unit[] = {
	GOUT_BLK_NPU_UID_NPUD_UNIT_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ppmu_npu[] = {
	GOUT_BLK_NPU_UID_PPMU_NPU_IPCLKPORT_ACLK,
	GOUT_BLK_NPU_UID_PPMU_NPU_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_npu_ppc_wrapper[] = {
	GOUT_BLK_NPU_UID_NPU_PPC_WRAPPER_IPCLKPORT_ACLK,
	GOUT_BLK_NPU_UID_NPU_PPC_WRAPPER_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d3_npux[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D3_NPUX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d2_npux[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D2_NPUX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d3_npuh[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D3_NPUH_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d3_npuv[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D3_NPUV_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d3_npux[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D3_NPUX_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_d_npu_unit_setreg[] = {
	GOUT_BLK_NPU_UID_LHM_AST_D_NPU_UNIT_SETREG_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_npu[] = {
	GOUT_BLK_NPU_UID_LHM_AXI_D_NPU_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_d_npu_unit_done[] = {
	GOUT_BLK_NPU_UID_LHS_AST_D_NPU_UNIT_DONE_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_gpio_peric0[] = {
	GOUT_BLK_PERIC0_UID_GPIO_PERIC0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_peric0[] = {
	GOUT_BLK_PERIC0_UID_SYSREG_PERIC0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_peric0_cmu_peric0[] = {
	CLK_BLK_PERIC0_UID_PERIC0_CMU_PERIC0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_peric0[] = {
	GOUT_BLK_PERIC0_UID_LHM_AXI_P_PERIC0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_peric0[] = {
	GOUT_BLK_PERIC0_UID_D_TZPC_PERIC0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_peric0_top0[] = {
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_0,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_1,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_2,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_3,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_4,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_5,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_6,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_7,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_8,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_9,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_10,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_10,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_11,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_IPCLK_11,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_0,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_1,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_2,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_3,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_4,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_7,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_5,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_6,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_8,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_9,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_10,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_10,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_11,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_11,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_15,
	GOUT_BLK_PERIC0_UID_PERIC0_TOP0_IPCLKPORT_PCLK_15,
};
enum clk_id cmucal_vclk_ip_gpio_peric1[] = {
	GOUT_BLK_PERIC1_UID_GPIO_PERIC1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_peric1[] = {
	GOUT_BLK_PERIC1_UID_SYSREG_PERIC1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_peric1_cmu_peric1[] = {
	CLK_BLK_PERIC1_UID_PERIC1_CMU_PERIC1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_peric1[] = {
	GOUT_BLK_PERIC1_UID_LHM_AXI_P_PERIC1_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_peric1[] = {
	GOUT_BLK_PERIC1_UID_D_TZPC_PERIC1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_peric1_top0[] = {
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_0,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_2,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_3,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_4,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_5,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_6,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_7,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_8,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_9,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_10,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_10,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_11,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_11,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_0,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_1,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_2,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_3,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_4,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_5,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_6,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_7,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_8,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_9,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_10,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_10,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_11,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_PCLK_11,
	GOUT_BLK_PERIC1_UID_PERIC1_TOP0_IPCLKPORT_IPCLK_1,
};
enum clk_id cmucal_vclk_ip_sysreg_peris[] = {
	GOUT_BLK_PERIS_UID_SYSREG_PERIS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_wdt_cluster1[] = {
	GOUT_BLK_PERIS_UID_WDT_CLUSTER1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_wdt_cluster0[] = {
	GOUT_BLK_PERIS_UID_WDT_CLUSTER0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_peris_cmu_peris[] = {
	CLK_BLK_PERIS_UID_PERIS_CMU_PERIS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ad_axi_p_peris[] = {
	GOUT_BLK_PERIS_UID_AD_AXI_P_PERIS_IPCLKPORT_ACLKM,
};
enum clk_id cmucal_vclk_ip_otp_con_bira[] = {
	GOUT_BLK_PERIS_UID_OTP_CON_BIRA_IPCLKPORT_PCLK,
	CLK_BLK_PERIS_UID_OTP_CON_BIRA_IPCLKPORT_I_OSCCLK,
};
enum clk_id cmucal_vclk_ip_gic[] = {
	GOUT_BLK_PERIS_UID_GIC_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_peris[] = {
	GOUT_BLK_PERIS_UID_LHM_AXI_P_PERIS_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_mct[] = {
	GOUT_BLK_PERIS_UID_MCT_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_otp_con_top[] = {
	GOUT_BLK_PERIS_UID_OTP_CON_TOP_IPCLKPORT_PCLK,
	CLK_BLK_PERIS_UID_OTP_CON_TOP_IPCLKPORT_I_OSCCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_peris[] = {
	GOUT_BLK_PERIS_UID_D_TZPC_PERIS_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_otp_con_bisr[] = {
	CLK_BLK_PERIS_UID_OTP_CON_BISR_IPCLKPORT_I_OSCCLK,
	GOUT_BLK_PERIS_UID_OTP_CON_BISR_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_busif_tmu[] = {
	GOUT_BLK_PERIS_UID_BUSIF_TMU_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_peris_1[] = {
	GOUT_BLK_PERIS_UID_SYSREG_PERIS_1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_abox_cr52_c0[] = {
	GOUT_BLK_PERIS_UID_MAILBOX_ABOX_CR52_C0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_abox_cr52_c1[] = {
	GOUT_BLK_PERIS_UID_MAILBOX_ABOX_CR52_C1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_ap_cr52_c0[] = {
	GOUT_BLK_PERIS_UID_MAILBOX_AP_CR52_C0_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_ap_cr52_c1[] = {
	GOUT_BLK_PERIS_UID_MAILBOX_AP_CR52_C1_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysreg_peris_2[] = {
	GOUT_BLK_PERIS_UID_SYSREG_PERIS_2_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_s2d_cmu_s2d[] = {
	CLK_BLK_S2D_UID_S2D_CMU_S2D_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_bis_s2d[] = {
	GOUT_BLK_S2D_UID_BIS_S2D_IPCLKPORT_CLK,
	CLK_BLK_S2D_UID_BIS_S2D_IPCLKPORT_SCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_g_scan2dram[] = {
	GOUT_BLK_S2D_UID_LHM_AXI_G_SCAN2DRAM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_usi13_usi[] = {
	GOUT_BLK_SFI_UID_USI13_USI_IPCLKPORT_IPCLK,
	GOUT_BLK_SFI_UID_USI13_USI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_adm_apb_g_sfi[] = {
	GOUT_BLK_SFI_UID_ADM_APB_G_SFI_IPCLKPORT_PCLKM,
	GOUT_BLK_SFI_UID_ADM_APB_G_SFI_IPCLKPORT_PCLKS,
};
enum clk_id cmucal_vclk_ip_clkmon0[] = {
	CLK_BLK_SFI_UID_CLKMON0_IPCLKPORT_REF_CLK0,
	GOUT_BLK_SFI_UID_CLKMON0_IPCLKPORT_REF_CLK1,
	GOUT_BLK_SFI_UID_CLKMON0_IPCLKPORT_PCLK,
	GOUT_BLK_SFI_UID_CLKMON0_IPCLKPORT_MON_CLK00,
	GOUT_BLK_SFI_UID_CLKMON0_IPCLKPORT_MON_CLK10,
};
enum clk_id cmucal_vclk_ip_usi12_usi[] = {
	GOUT_BLK_SFI_UID_USI12_USI_IPCLKPORT_IPCLK,
	GOUT_BLK_SFI_UID_USI12_USI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_usi14_usi[] = {
	GOUT_BLK_SFI_UID_USI14_USI_IPCLKPORT_IPCLK,
	GOUT_BLK_SFI_UID_USI14_USI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sss_sfi[] = {
	GOUT_BLK_SFI_UID_SSS_SFI_IPCLKPORT_I_ACLK,
	GOUT_BLK_SFI_UID_SSS_SFI_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_dap_sfi[] = {
	GOUT_BLK_SFI_UID_DAP_SFI_IPCLKPORT_DAPCLK,
};
enum clk_id cmucal_vclk_ip_add_gpio_lo[] = {
	CLK_BLK_SFI_UID_ADD_GPIO_LO_IPCLKPORT_CH_CLK,
};
enum clk_id cmucal_vclk_ip_add_gpio_up[] = {
	CLK_BLK_SFI_UID_ADD_GPIO_UP_IPCLKPORT_CH_CLK,
};
enum clk_id cmucal_vclk_ip_add_sfi_lo[] = {
	CLK_BLK_SFI_UID_ADD_SFI_LO_IPCLKPORT_CH_CLK,
};
enum clk_id cmucal_vclk_ip_add_sfi_up[] = {
	CLK_BLK_SFI_UID_ADD_SFI_UP_IPCLKPORT_CH_CLK,
};
enum clk_id cmucal_vclk_ip_fmu[] = {
	GOUT_BLK_SFI_UID_FMU_IPCLKPORT_PCLK_FMU,
};
enum clk_id cmucal_vclk_ip_asyncapb_fmu0[] = {
	GOUT_BLK_SFI_UID_ASYNCAPB_FMU0_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_baaw_d_sfi[] = {
	GOUT_BLK_SFI_UID_BAAW_D_SFI_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_baaw_p_sfiapm_sfi[] = {
	GOUT_BLK_SFI_UID_BAAW_P_SFIAPM_SFI_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_baaw_p_sfifsys0_sfi[] = {
	GOUT_BLK_SFI_UID_BAAW_P_SFIFSYS0_SFI_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_sfi[] = {
	GOUT_BLK_SFI_UID_D_TZPC_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_gpio_sfi[] = {
	GOUT_BLK_SFI_UID_GPIO_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mailbox_cr52_sfi[] = {
	GOUT_BLK_SFI_UID_MAILBOX_CR52_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_mct_sfi[] = {
	GOUT_BLK_SFI_UID_MCT_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_ppmu_sfifsys0[] = {
	GOUT_BLK_SFI_UID_PPMU_SFIFSYS0_IPCLKPORT_PCLK,
	GOUT_BLK_SFI_UID_PPMU_SFIFSYS0_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_rom_crc32_sfi[] = {
	GOUT_BLK_SFI_UID_ROM_CRC32_SFI_IPCLKPORT_PCLK,
	CLK_BLK_SFI_UID_ROM_CRC32_SFI_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_sysreg_sfi[] = {
	GOUT_BLK_SFI_UID_SYSREG_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_volmon[] = {
	GOUT_BLK_SFI_UID_VOLMON_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_wdt0_sfi[] = {
	GOUT_BLK_SFI_UID_WDT0_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_usi15_usi[] = {
	GOUT_BLK_SFI_UID_USI15_USI_IPCLKPORT_PCLK,
	GOUT_BLK_SFI_UID_USI15_USI_IPCLKPORT_IPCLK,
};
enum clk_id cmucal_vclk_ip_can_fd0[] = {
	GOUT_BLK_SFI_UID_CAN_FD0_IPCLKPORT_I_PCLK,
	GOUT_BLK_SFI_UID_CAN_FD0_IPCLKPORT_I_CCLK,
};
enum clk_id cmucal_vclk_ip_can_fd1[] = {
	GOUT_BLK_SFI_UID_CAN_FD1_IPCLKPORT_I_PCLK,
	GOUT_BLK_SFI_UID_CAN_FD1_IPCLKPORT_I_CCLK,
};
enum clk_id cmucal_vclk_ip_d_sfpc_sfi[] = {
	GOUT_BLK_SFI_UID_D_SFPC_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_intmem_sfi[] = {
	GOUT_BLK_SFI_UID_INTMEM_SFI_IPCLKPORT_ACLK,
	GOUT_BLK_SFI_UID_INTMEM_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_asyncapb_intmem[] = {
	GOUT_BLK_SFI_UID_ASYNCAPB_INTMEM_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_lhm_axi_d_fsys0sfi[] = {
	GOUT_BLK_SFI_UID_LHM_AXI_D_FSYS0SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_sfi[] = {
	GOUT_BLK_SFI_UID_LHM_AXI_P_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_sfi[] = {
	GOUT_BLK_SFI_UID_LHS_AXI_D_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_sfifsys0[] = {
	GOUT_BLK_SFI_UID_LHS_AXI_P_SFIFSYS0_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_axi_p_sfiapm[] = {
	GOUT_BLK_SFI_UID_LHS_AXI_P_SFIAPM_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_xiu_dp_sfi[] = {
	GOUT_BLK_SFI_UID_XIU_DP_SFI_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_serialflash[] = {
	GOUT_BLK_SFI_UID_SERIALFLASH_IPCLKPORT_HCLK,
	GOUT_BLK_SFI_UID_SERIALFLASH_IPCLKPORT_SFCLK,
};
enum clk_id cmucal_vclk_ip_hyperbus[] = {
	GOUT_BLK_SFI_UID_HYPERBUS_IPCLKPORT_AXIR_ACLK,
	GOUT_BLK_SFI_UID_HYPERBUS_IPCLKPORT_AXIM_ACLK,
	GOUT_BLK_SFI_UID_HYPERBUS_IPCLKPORT_SYS_CLK,
};
enum clk_id cmucal_vclk_ip_baaw_d_fsys0sfi_sfi[] = {
	GOUT_BLK_SFI_UID_BAAW_D_FSYS0SFI_SFI_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_dt0_sfi[] = {
	GOUT_BLK_SFI_UID_LHS_ATB_DT0_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_dt1_sfi[] = {
	GOUT_BLK_SFI_UID_LHS_ATB_DT1_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_it0_sfi[] = {
	GOUT_BLK_SFI_UID_LHS_ATB_IT0_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_atb_it1_sfi[] = {
	GOUT_BLK_SFI_UID_LHS_ATB_IT1_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_asyncaxi_m0[] = {
	GOUT_BLK_SFI_UID_ASYNCAXI_M0_IPCLKPORT_ACLKS,
};
enum clk_id cmucal_vclk_ip_asyncaxi_m1[] = {
	GOUT_BLK_SFI_UID_ASYNCAXI_M1_IPCLKPORT_ACLKS,
};
enum clk_id cmucal_vclk_ip_asyncaxi_s0[] = {
	GOUT_BLK_SFI_UID_ASYNCAXI_S0_IPCLKPORT_ACLKM,
};
enum clk_id cmucal_vclk_ip_sf_axi2apb_bridge_sfi[] = {
	GOUT_BLK_SFI_UID_SF_AXI2APB_BRIDGE_SFI_IPCLKPORT_I_ACLK,
};
enum clk_id cmucal_vclk_ip_wdt1_sfi[] = {
	GOUT_BLK_SFI_UID_WDT1_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sfmpu_flsh0_sfi[] = {
	GOUT_BLK_SFI_UID_SFMPU_FLSH0_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sfmpu_flsh1_sfi[] = {
	GOUT_BLK_SFI_UID_SFMPU_FLSH1_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sfmpu_imem_sfi[] = {
	GOUT_BLK_SFI_UID_SFMPU_IMEM_SFI_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_busif_cmu_sfi[] = {
	GOUT_BLK_SFI_UID_BUSIF_CMU_SFI_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_pmu_handshake_bus[] = {
	CLK_BLK_SFI_UID_PMU_HANDSHAKE_BUS_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_baaw_flsh_sfi[] = {
	GOUT_BLK_SFI_UID_BAAW_FLSH_SFI_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_cluster_sfi[] = {
	GOUT_BLK_SFI_UID_CLUSTER_SFI_IPCLKPORT_CLKIN,
};
enum clk_id cmucal_vclk_ip_xiu_p3_sfi[] = {
	GOUT_BLK_SFI_UID_XIU_P3_SFI_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_sbist0[] = {
	GOUT_BLK_SFI_UID_SBIST0_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_sbist1[] = {
	GOUT_BLK_SFI_UID_SBIST1_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_baaw_p_sfi[] = {
	GOUT_BLK_SFI_UID_BAAW_P_SFI_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_asyncapb_sfmpu_flsh0_sfi[] = {
	GOUT_BLK_SFI_UID_ASYNCAPB_SFMPU_FLSH0_SFI_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_asyncapb_sfmpu_flsh1_sfi[] = {
	GOUT_BLK_SFI_UID_ASYNCAPB_SFMPU_FLSH1_SFI_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_asyncapb_sfmpu_intmem_sfi[] = {
	GOUT_BLK_SFI_UID_ASYNCAPB_SFMPU_INTMEM_SFI_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_pmc[] = {
	GOUT_BLK_SFI_UID_PMC_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_clkmon1[] = {
	GOUT_BLK_SFI_UID_CLKMON1_IPCLKPORT_PCLK,
	GOUT_BLK_SFI_UID_CLKMON1_IPCLKPORT_REF_CLK0,
	GOUT_BLK_SFI_UID_CLKMON1_IPCLKPORT_REF_CLK1,
	GOUT_BLK_SFI_UID_CLKMON1_IPCLKPORT_MON_CLK00,
	GOUT_BLK_SFI_UID_CLKMON1_IPCLKPORT_MON_CLK10,
};
enum clk_id cmucal_vclk_ip_clkmon2[] = {
	GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_PCLK,
	GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_REF_CLK0,
	GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_REF_CLK1,
	GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_MON_CLK10,
	GOUT_BLK_SFI_UID_CLKMON2_IPCLKPORT_MON_CLK00,
};
enum clk_id cmucal_vclk_ip_clkmon3[] = {
	GOUT_BLK_SFI_UID_CLKMON3_IPCLKPORT_PCLK,
	GOUT_BLK_SFI_UID_CLKMON3_IPCLKPORT_REF_CLK0,
	GOUT_BLK_SFI_UID_CLKMON3_IPCLKPORT_REF_CLK1,
	GOUT_BLK_SFI_UID_CLKMON3_IPCLKPORT_MON_CLK00,
	GOUT_BLK_SFI_UID_CLKMON3_IPCLKPORT_MON_CLK10,
};
enum clk_id cmucal_vclk_ip_lhs_axi_d_taa[] = {
	GOUT_BLK_TAA_UID_LHS_AXI_D_TAA_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_btm_taa[] = {
	GOUT_BLK_TAA_UID_BTM_TAA_IPCLKPORT_I_ACLK,
	GOUT_BLK_TAA_UID_BTM_TAA_IPCLKPORT_I_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_axi_p_taa[] = {
	GOUT_BLK_TAA_UID_LHM_AXI_P_TAA_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_sysreg_taa[] = {
	GOUT_BLK_TAA_UID_SYSREG_TAA_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_taa_cmu_taa[] = {
	CLK_BLK_TAA_UID_TAA_CMU_TAA_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_c2c_taaispb[] = {
	GOUT_BLK_TAA_UID_LHS_AST_C2C_TAAISPB_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhs_ast_otf_taaispb[] = {
	GOUT_BLK_TAA_UID_LHS_AST_OTF_TAAISPB_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_taa[] = {
	GOUT_BLK_TAA_UID_TAA_IPCLKPORT_CLK,
	GOUT_BLK_TAA_UID_TAA_IPCLKPORT_C2CLK,
};
enum clk_id cmucal_vclk_ip_d_tzpc_taa[] = {
	GOUT_BLK_TAA_UID_D_TZPC_TAA_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_otf_acctaa[] = {
	GOUT_BLK_TAA_UID_LHM_AST_OTF_ACCTAA_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_lhm_ast_c2c_acctaa[] = {
	GOUT_BLK_TAA_UID_LHM_AST_C2C_ACCTAA_IPCLKPORT_I_CLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_taa_taa0[] = {
	GOUT_BLK_TAA_UID_AD_APB_TAA_TAA0_IPCLKPORT_PCLKM,
	GOUT_BLK_TAA_UID_AD_APB_TAA_TAA0_IPCLKPORT_PCLKS,
};
enum clk_id cmucal_vclk_ip_ppmu_taa[] = {
	GOUT_BLK_TAA_UID_PPMU_TAA_IPCLKPORT_ACLK,
	GOUT_BLK_TAA_UID_PPMU_TAA_IPCLKPORT_PCLK,
};
enum clk_id cmucal_vclk_ip_sysmmu_taa[] = {
	GOUT_BLK_TAA_UID_SYSMMU_TAA_IPCLKPORT_CLK_S1,
	GOUT_BLK_TAA_UID_SYSMMU_TAA_IPCLKPORT_CLK_S2,
};
enum clk_id cmucal_vclk_ip_vgen_lite_taa[] = {
	GOUT_BLK_TAA_UID_VGEN_LITE_TAA_IPCLKPORT_CLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_taa_sysmmu_s1_s[] = {
	GOUT_BLK_TAA_UID_AD_APB_TAA_SYSMMU_S1_S_IPCLKPORT_PCLKS,
	GOUT_BLK_TAA_UID_AD_APB_TAA_SYSMMU_S1_S_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_ad_apb_taa_sysmmu_s1_ns[] = {
	GOUT_BLK_TAA_UID_AD_APB_TAA_SYSMMU_S1_NS_IPCLKPORT_PCLKS,
	GOUT_BLK_TAA_UID_AD_APB_TAA_SYSMMU_S1_NS_IPCLKPORT_PCLKM,
};
enum clk_id cmucal_vclk_ip_axi2apb_taa[] = {
	GOUT_BLK_TAA_UID_AXI2APB_TAA_IPCLKPORT_ACLK,
};
enum clk_id cmucal_vclk_ip_ad_apb_taa_taa1[] = {
	GOUT_BLK_TAA_UID_AD_APB_TAA_TAA1_IPCLKPORT_PCLKM,
	GOUT_BLK_TAA_UID_AD_APB_TAA_TAA1_IPCLKPORT_PCLKS,
};
enum clk_id cmucal_vclk_ip_ad_apb_taa_taa2[] = {
	GOUT_BLK_TAA_UID_AD_APB_TAA_TAA2_IPCLKPORT_PCLKM,
	GOUT_BLK_TAA_UID_AD_APB_TAA_TAA2_IPCLKPORT_PCLKS,
};
enum clk_id cmucal_vclk_ip_ad_apb_taa_taa3[] = {
	GOUT_BLK_TAA_UID_AD_APB_TAA_TAA3_IPCLKPORT_PCLKM,
	GOUT_BLK_TAA_UID_AD_APB_TAA_TAA3_IPCLKPORT_PCLKS,
};
enum clk_id cmucal_vclk_ip_ad_apb_taa_sysmmu_s2[] = {
	GOUT_BLK_TAA_UID_AD_APB_TAA_SYSMMU_S2_IPCLKPORT_PCLKM,
	GOUT_BLK_TAA_UID_AD_APB_TAA_SYSMMU_S2_IPCLKPORT_PCLKS,
};

/* DVFS VCLK -> LUT List */
struct vclk_lut cmucal_vclk_vdd_mif_lut[] = {
	{6400000, vdd_mif_od_lut_params},
	{3733000, vdd_mif_nm_lut_params},
};
struct vclk_lut cmucal_vclk_vdd_g3d1_lut[] = {
	{1070000, vdd_g3d1_sod_lut_params},
	{860000, vdd_g3d1_od_lut_params},
	{650000, vdd_g3d1_nm_lut_params},
};
struct vclk_lut cmucal_vclk_vdd_cpucl0_lut[] = {
	{2100000, vdd_cpucl0_sod_lut_params},
	{1700000, vdd_cpucl0_od_lut_params},
	{1100000, vdd_cpucl0_nm_lut_params},
	{550000, vdd_cpucl0_ud_lut_params},
};
struct vclk_lut cmucal_vclk_vdd_cpucl1_lut[] = {
	{2100000, vdd_cpucl1_sod_lut_params},
	{1700000, vdd_cpucl1_od_lut_params},
	{1100000, vdd_cpucl1_nm_lut_params},
	{550000, vdd_cpucl1_ud_lut_params},
};
struct vclk_lut cmucal_vclk_vdd_g3d00_lut[] = {
	{1070000, vdd_g3d00_sod_lut_params},
	{860000, vdd_g3d00_od_lut_params},
	{650000, vdd_g3d00_nm_lut_params},
};
struct vclk_lut cmucal_vclk_vdd_g3d01_lut[] = {
	{1070000, vdd_g3d01_sod_lut_params},
	{860000, vdd_g3d01_od_lut_params},
	{650000, vdd_g3d01_nm_lut_params},
};
struct vclk_lut cmucal_vclk_vdd_aud_lut[] = {
	{983000, vdd_aud_sod_lut_params},
	{836000, vdd_aud_od_lut_params},
	{590000, vdd_aud_nm_lut_params},
};
struct vclk_lut cmucal_vclk_vdd_int_cmu_lut[] = {
	{400000, vdd_int_cmu_ud_lut_params},
	{300000, vdd_int_cmu_sod_lut_params},
	{200000, vdd_int_cmu_od_lut_params},
	{100000, vdd_int_cmu_nm_lut_params},
};

/* SPECIAL VCLK -> LUT List */
struct vclk_lut cmucal_vclk_mux_clk_aud_uaif3_lut[] = {
	{983000, mux_clk_aud_uaif3_sod_lut_params},
	{836000, mux_clk_aud_uaif3_od_lut_params},
	{666500, mux_clk_aud_uaif3_nm_lut_params},
	{600000, mux_clk_aud_uaif3_ud_lut_params},
};
struct vclk_lut cmucal_vclk_mux_busc_cmuref_lut[] = {
	{266667, mux_busc_cmuref_nm_lut_params},
};
struct vclk_lut cmucal_vclk_mux_busmc_cmuref_lut[] = {
	{266667, mux_busmc_cmuref_nm_lut_params},
	{26000, mux_busmc_cmuref_ud_lut_params},
};
struct vclk_lut cmucal_vclk_mux_cmu_cmuref_lut[] = {
	{266667, mux_cmu_cmuref_ud_lut_params},
};
struct vclk_lut cmucal_vclk_mux_cpucl1_cmuref_lut[] = {
	{266667, mux_cpucl1_cmuref_ud_lut_params},
};
struct vclk_lut cmucal_vclk_mux_mif_cmuref_lut[] = {
	{266667, mux_mif_cmuref_od_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_aud_mclk_lut[] = {
	{104500, div_clk_aud_mclk_od_lut_params},
	{98333, div_clk_aud_mclk_nm_lut_params},
	{98300, div_clk_aud_mclk_sod_lut_params},
	{50000, div_clk_aud_mclk_ud_lut_params},
};
struct vclk_lut cmucal_vclk_clk_cmu_pllclkout_lut[] = {
	{213200, clk_cmu_pllclkout_ud_lut_params},
};
struct vclk_lut cmucal_vclk_clkcmu_hpm_lut[] = {
	{400000, clkcmu_hpm_ud_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_cpucl0_shortstop_lut[] = {
	{400000, div_clk_cpucl0_shortstop_ud_lut_params},
};
struct vclk_lut cmucal_vclk_mux_clkcmu_cpucl0_switch_lut[] = {
	{1066000, mux_clkcmu_cpucl0_switch_ud_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_cluster0_periphclk_lut[] = {
	{200000, div_clk_cluster0_periphclk_ud_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_cpucl1_shortstop_lut[] = {
	{400000, div_clk_cpucl1_shortstop_ud_lut_params},
};
struct vclk_lut cmucal_vclk_mux_clkcmu_cpucl1_switch_lut[] = {
	{1066000, mux_clkcmu_cpucl1_switch_ud_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_cluster1_periphclk_lut[] = {
	{2100000, div_clk_cluster1_periphclk_sod_lut_params},
	{1700000, div_clk_cluster1_periphclk_od_lut_params},
	{1100000, div_clk_cluster1_periphclk_nm_lut_params},
	{550000, div_clk_cluster1_periphclk_ud_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_fsys1_mmc_card_lut[] = {
	{808000, div_clk_fsys1_mmc_card_nm_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_peric0_usi00_usi_lut[] = {
	{400000, div_clk_peric0_usi00_usi_nm_lut_params},
	{26000, div_clk_peric0_usi00_usi_ud_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_peric1_usi_i2c_lut[] = {
	{400000, div_clk_peric1_usi_i2c_nm_lut_params},
	{26000, div_clk_peric1_usi_i2c_ud_lut_params},
};
struct vclk_lut cmucal_vclk_clk_sfi_pllclkout_lut[] = {
	{200000, clk_sfi_pllclkout_nm_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_sfi_usi14_lut[] = {
	{400000, div_clk_sfi_usi14_nm_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_sfi_usi13_lut[] = {
	{400000, div_clk_sfi_usi13_nm_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_sfi_cpu_pclkdbg_lut[] = {
	{800000, div_clk_sfi_cpu_pclkdbg_nm_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_sfi_can0_lut[] = {
	{80000, div_clk_sfi_can0_nm_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_sfi_can1_lut[] = {
	{80000, div_clk_sfi_can1_nm_lut_params},
};
struct vclk_lut cmucal_vclk_div_clk_sfi_usi15_lut[] = {
	{400000, div_clk_sfi_usi15_nm_lut_params},
};

/* COMMON VCLK -> LUT List */
struct vclk_lut cmucal_vclk_blk_cmu_lut[] = {
	{2132000, blk_cmu_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_s2d_lut[] = {
	{400000, blk_s2d_nm_lut_params},
	{6500, blk_s2d_od_lut_params},
};
struct vclk_lut cmucal_vclk_blk_sfi_lut[] = {
	{800000, blk_sfi_nm_lut_params},
};
struct vclk_lut cmucal_vclk_blk_apm_lut[] = {
	{400000, blk_apm_nm_lut_params},
};
struct vclk_lut cmucal_vclk_blk_cpucl0_lut[] = {
	{2100000, blk_cpucl0_sod_lut_params},
	{1700000, blk_cpucl0_od_lut_params},
	{1100000, blk_cpucl0_nm_lut_params},
	{550000, blk_cpucl0_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_cpucl1_lut[] = {
	{200000, blk_cpucl1_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_fsys0_lut[] = {
	{333250, blk_fsys0_nm_lut_params},
};
struct vclk_lut cmucal_vclk_blk_fsys2_lut[] = {
	{333250, blk_fsys2_nm_lut_params},
};
struct vclk_lut cmucal_vclk_blk_acc_lut[] = {
	{311000, blk_acc_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_aud_lut[] = {
	{266667, blk_aud_sod_lut_params},
	{100000, blk_aud_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_busc_lut[] = {
	{266667, blk_busc_nm_lut_params},
};
struct vclk_lut cmucal_vclk_blk_busmc_lut[] = {
	{266667, blk_busmc_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_core_lut[] = {
	{311000, blk_core_od_lut_params},
};
struct vclk_lut cmucal_vclk_blk_dnc_lut[] = {
	{133333, blk_dnc_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_dpum_lut[] = {
	{166625, blk_dpum_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_dpus_lut[] = {
	{166625, blk_dpus_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_dpus1_lut[] = {
	{166625, blk_dpus1_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_g2d_lut[] = {
	{266667, blk_g2d_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_g3d00_lut[] = {
	{267500, blk_g3d00_sod_lut_params},
	{215000, blk_g3d00_od_lut_params},
	{162500, blk_g3d00_nm_lut_params},
};
struct vclk_lut cmucal_vclk_blk_g3d01_lut[] = {
	{267500, blk_g3d01_sod_lut_params},
	{215000, blk_g3d01_od_lut_params},
	{162500, blk_g3d01_nm_lut_params},
};
struct vclk_lut cmucal_vclk_blk_g3d1_lut[] = {
	{267500, blk_g3d1_sod_lut_params},
	{215000, blk_g3d1_od_lut_params},
	{162500, blk_g3d1_nm_lut_params},
	{112500, blk_g3d1_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_ispb_lut[] = {
	{311000, blk_ispb_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_mfc_lut[] = {
	{166625, blk_mfc_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_npu_lut[] = {
	{133333, blk_npu_ud_lut_params},
};
struct vclk_lut cmucal_vclk_blk_taa_lut[] = {
	{311000, blk_taa_ud_lut_params},
};

/* Switch VCLK -> LUT Parameter List */
struct switch_lut mux_clk_aud_cpu_lut[] = {
	{933000, 1, 0},
	{800000, 2, 0},
	{666500, 4, 0},
	{533000, 0, 1},
};
struct switch_lut mux_clk_aud_bus_lut[] = {
	{666500, 0, 0},
	{622000, 1, 0},
	{533333, 2, 0},
};
struct switch_lut mux_clk_fsys1_mmc_card_lut[] = {
	{800000, 1, -1},
};
struct switch_lut mux_clk_g3d00_bus_lut[] = {
	{1066000, 0, 0},
	{800000, 2, 0},
	{666500, 3, 0},
	{400000, 2, 1},
};
struct switch_lut mux_clk_g3d01_bus_lut[] = {
	{1066000, 0, 0},
	{800000, 2, 0},
	{666500, 3, 0},
	{400000, 2, 1},
};
struct switch_lut mux_clk_g3d1_bus_lut[] = {
	{800000, 0, 0},
	{666500, 1, 0},
	{400000, 0, 1},
};
/*================================ SWPLL List =================================*/
struct vclk_switch switch_vdd_g3d1[] = {
	{MUX_CLK_G3D1_BUS, MUX_CLKCMU_G3D1_SWITCH, CLKCMU_G3D1_SWITCH, GATE_CLKCMU_G3D1_SWITCH, MUX_CLKCMU_G3D1_SWITCH_USER, mux_clk_g3d1_bus_lut, 3},
};
struct vclk_switch switch_vdd_g3d00[] = {
	{MUX_CLK_G3D00_BUS, MUX_CLKCMU_G3D00_SWITCH, CLKCMU_G3D00_SWITCH, GATE_CLKCMU_G3D00_SWITCH, MUX_CLKCMU_G3D00_SWITCH_USER, mux_clk_g3d00_bus_lut, 4},
};
struct vclk_switch switch_vdd_g3d01[] = {
	{MUX_CLK_G3D01_BUS, MUX_CLKCMU_G3D01_SWITCH, CLKCMU_G3D01_SWITCH, GATE_CLKCMU_G3D01_SWITCH, MUX_CLKCMU_G3D01_SWITCH_USER, mux_clk_g3d01_bus_lut, 4},
};
struct vclk_switch switch_vdd_aud[] = {
	{MUX_CLK_AUD_CPU, MUX_CLKCMU_AUD_CPU, CLKCMU_AUD_CPU, GATE_CLKCMU_AUD_CPU, MUX_CLKCMU_AUD_CPU_USER, mux_clk_aud_cpu_lut, 4},
	{MUX_CLK_AUD_BUS, MUX_CLKCMU_AUD_BUS, CLKCMU_AUD_BUS, GATE_CLKCMU_AUD_BUS, MUX_CLKCMU_AUD_BUS_USER, mux_clk_aud_bus_lut, 3},
};
struct vclk_switch switch_div_clk_fsys1_mmc_card[] = {
	{MUX_CLK_FSYS1_MMC_CARD, MUX_CLKCMU_FSYS1_MMC_CARD, EMPTY_CAL_ID, CLKCMU_FSYS1_MMC_CARD, MUX_CLKCMU_FSYS1_MMC_CARD_USER, mux_clk_fsys1_mmc_card_lut, 1},
};

/*================================ VCLK List =================================*/
unsigned int cmucal_vclk_size = 971;
struct vclk cmucal_vclk_list[] = {

/* DVFS VCLK*/
	CMUCAL_VCLK(VCLK_VDD_MIF, cmucal_vclk_vdd_mif_lut, cmucal_vclk_vdd_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_VDD_G3D1, cmucal_vclk_vdd_g3d1_lut, cmucal_vclk_vdd_g3d1, NULL, switch_vdd_g3d1),
	CMUCAL_VCLK(VCLK_VDD_CPUCL0, cmucal_vclk_vdd_cpucl0_lut, cmucal_vclk_vdd_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_VDD_CPUCL1, cmucal_vclk_vdd_cpucl1_lut, cmucal_vclk_vdd_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_VDD_G3D00, cmucal_vclk_vdd_g3d00_lut, cmucal_vclk_vdd_g3d00, NULL, switch_vdd_g3d00),
	CMUCAL_VCLK(VCLK_VDD_G3D01, cmucal_vclk_vdd_g3d01_lut, cmucal_vclk_vdd_g3d01, NULL, switch_vdd_g3d01),
	CMUCAL_VCLK(VCLK_VDD_AUD, cmucal_vclk_vdd_aud_lut, cmucal_vclk_vdd_aud, NULL, switch_vdd_aud),
	CMUCAL_VCLK(VCLK_VDD_INT_CMU, cmucal_vclk_vdd_int_cmu_lut, cmucal_vclk_vdd_int_cmu, NULL, NULL),

/* SPECIAL VCLK*/
	CMUCAL_VCLK(VCLK_MUX_CLK_AUD_UAIF3, cmucal_vclk_mux_clk_aud_uaif3_lut, cmucal_vclk_mux_clk_aud_uaif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_MUX_BUSC_CMUREF, cmucal_vclk_mux_busc_cmuref_lut, cmucal_vclk_mux_busc_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_MUX_BUSMC_CMUREF, cmucal_vclk_mux_busmc_cmuref_lut, cmucal_vclk_mux_busmc_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_MUX_CMU_CMUREF, cmucal_vclk_mux_cmu_cmuref_lut, cmucal_vclk_mux_cmu_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_MUX_CPUCL1_CMUREF, cmucal_vclk_mux_cpucl1_cmuref_lut, cmucal_vclk_mux_cpucl1_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_MUX_MIF_CMUREF, cmucal_vclk_mux_mif_cmuref_lut, cmucal_vclk_mux_mif_cmuref, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_AUD_MCLK, cmucal_vclk_div_clk_aud_mclk_lut, cmucal_vclk_div_clk_aud_mclk, NULL, NULL),
	CMUCAL_VCLK(VCLK_CLK_CMU_PLLCLKOUT, cmucal_vclk_clk_cmu_pllclkout_lut, cmucal_vclk_clk_cmu_pllclkout, NULL, NULL),
	CMUCAL_VCLK(VCLK_CLKCMU_HPM, cmucal_vclk_clkcmu_hpm_lut, cmucal_vclk_clkcmu_hpm, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_CPUCL0_SHORTSTOP, cmucal_vclk_div_clk_cpucl0_shortstop_lut, cmucal_vclk_div_clk_cpucl0_shortstop, NULL, NULL),
	CMUCAL_VCLK(VCLK_MUX_CLKCMU_CPUCL0_SWITCH, cmucal_vclk_mux_clkcmu_cpucl0_switch_lut, cmucal_vclk_mux_clkcmu_cpucl0_switch, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_CLUSTER0_PERIPHCLK, cmucal_vclk_div_clk_cluster0_periphclk_lut, cmucal_vclk_div_clk_cluster0_periphclk, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_CPUCL1_SHORTSTOP, cmucal_vclk_div_clk_cpucl1_shortstop_lut, cmucal_vclk_div_clk_cpucl1_shortstop, NULL, NULL),
	CMUCAL_VCLK(VCLK_MUX_CLKCMU_CPUCL1_SWITCH, cmucal_vclk_mux_clkcmu_cpucl1_switch_lut, cmucal_vclk_mux_clkcmu_cpucl1_switch, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_CLUSTER1_PERIPHCLK, cmucal_vclk_div_clk_cluster1_periphclk_lut, cmucal_vclk_div_clk_cluster1_periphclk, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_FSYS1_MMC_CARD, cmucal_vclk_div_clk_fsys1_mmc_card_lut, cmucal_vclk_div_clk_fsys1_mmc_card, NULL, switch_div_clk_fsys1_mmc_card),
	CMUCAL_VCLK(VCLK_DIV_CLK_PERIC0_USI00_USI, cmucal_vclk_div_clk_peric0_usi00_usi_lut, cmucal_vclk_div_clk_peric0_usi00_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_PERIC1_USI_I2C, cmucal_vclk_div_clk_peric1_usi_i2c_lut, cmucal_vclk_div_clk_peric1_usi_i2c, NULL, NULL),
	CMUCAL_VCLK(VCLK_CLK_SFI_PLLCLKOUT, cmucal_vclk_clk_sfi_pllclkout_lut, cmucal_vclk_clk_sfi_pllclkout, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_SFI_USI14, cmucal_vclk_div_clk_sfi_usi14_lut, cmucal_vclk_div_clk_sfi_usi14, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_SFI_USI13, cmucal_vclk_div_clk_sfi_usi13_lut, cmucal_vclk_div_clk_sfi_usi13, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_SFI_CPU_PCLKDBG, cmucal_vclk_div_clk_sfi_cpu_pclkdbg_lut, cmucal_vclk_div_clk_sfi_cpu_pclkdbg, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_SFI_CAN0, cmucal_vclk_div_clk_sfi_can0_lut, cmucal_vclk_div_clk_sfi_can0, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_SFI_CAN1, cmucal_vclk_div_clk_sfi_can1_lut, cmucal_vclk_div_clk_sfi_can1, NULL, NULL),
	CMUCAL_VCLK(VCLK_DIV_CLK_SFI_USI15, cmucal_vclk_div_clk_sfi_usi15_lut, cmucal_vclk_div_clk_sfi_usi15, NULL, NULL),

/* COMMON VCLK*/
	CMUCAL_VCLK(VCLK_BLK_CMU, cmucal_vclk_blk_cmu_lut, cmucal_vclk_blk_cmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_S2D, cmucal_vclk_blk_s2d_lut, cmucal_vclk_blk_s2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_SFI, cmucal_vclk_blk_sfi_lut, cmucal_vclk_blk_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_APM, cmucal_vclk_blk_apm_lut, cmucal_vclk_blk_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_CPUCL0, cmucal_vclk_blk_cpucl0_lut, cmucal_vclk_blk_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_CPUCL1, cmucal_vclk_blk_cpucl1_lut, cmucal_vclk_blk_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_FSYS0, cmucal_vclk_blk_fsys0_lut, cmucal_vclk_blk_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_FSYS2, cmucal_vclk_blk_fsys2_lut, cmucal_vclk_blk_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_ACC, cmucal_vclk_blk_acc_lut, cmucal_vclk_blk_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_AUD, cmucal_vclk_blk_aud_lut, cmucal_vclk_blk_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_BUSC, cmucal_vclk_blk_busc_lut, cmucal_vclk_blk_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_BUSMC, cmucal_vclk_blk_busmc_lut, cmucal_vclk_blk_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_CORE, cmucal_vclk_blk_core_lut, cmucal_vclk_blk_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_DNC, cmucal_vclk_blk_dnc_lut, cmucal_vclk_blk_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_DPUM, cmucal_vclk_blk_dpum_lut, cmucal_vclk_blk_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_DPUS, cmucal_vclk_blk_dpus_lut, cmucal_vclk_blk_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_DPUS1, cmucal_vclk_blk_dpus1_lut, cmucal_vclk_blk_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_G2D, cmucal_vclk_blk_g2d_lut, cmucal_vclk_blk_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_G3D00, cmucal_vclk_blk_g3d00_lut, cmucal_vclk_blk_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_G3D01, cmucal_vclk_blk_g3d01_lut, cmucal_vclk_blk_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_G3D1, cmucal_vclk_blk_g3d1_lut, cmucal_vclk_blk_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_ISPB, cmucal_vclk_blk_ispb_lut, cmucal_vclk_blk_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_MFC, cmucal_vclk_blk_mfc_lut, cmucal_vclk_blk_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_NPU, cmucal_vclk_blk_npu_lut, cmucal_vclk_blk_npu, NULL, NULL),
	CMUCAL_VCLK(VCLK_BLK_TAA, cmucal_vclk_blk_taa_lut, cmucal_vclk_blk_taa, NULL, NULL),

/* GATE VCLK*/
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_ACC, NULL, cmucal_vclk_ip_lhm_axi_p_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D0_ACC, NULL, cmucal_vclk_ip_lhs_axi_d0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_ACC, NULL, cmucal_vclk_ip_btm_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_ACC, NULL, cmucal_vclk_ip_sysreg_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_C2C_ACCTAA0, NULL, cmucal_vclk_ip_lhs_ast_c2c_acctaa0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_OTF_ACCTAA1, NULL, cmucal_vclk_ip_lhs_ast_otf_acctaa1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_ACC, NULL, cmucal_vclk_ip_d_tzpc_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_OTF_ACCTAA0, NULL, cmucal_vclk_ip_lhs_ast_otf_acctaa0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_ACC_KITT, NULL, cmucal_vclk_ip_xiu_d_acc_kitt, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_ACC, NULL, cmucal_vclk_ip_vgen_lite_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_ACC, NULL, cmucal_vclk_ip_ppmu_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_GDC0, NULL, cmucal_vclk_ip_qe_gdc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_GDC1, NULL, cmucal_vclk_ip_qe_gdc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_ISPPRE, NULL, cmucal_vclk_ip_qe_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_VRA, NULL, cmucal_vclk_ip_qe_vra, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_ACC, NULL, cmucal_vclk_ip_sysmmu_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_CSIS0, NULL, cmucal_vclk_ip_apb_async_csis0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_HUB, NULL, cmucal_vclk_ip_hub, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_C2C_ACCTAA1, NULL, cmucal_vclk_ip_lhs_ast_c2c_acctaa1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_C2C_ISPB0ACC, NULL, cmucal_vclk_ip_lhm_ast_c2c_ispb0acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_C2C_ISPB1ACC, NULL, cmucal_vclk_ip_lhm_ast_c2c_ispb1acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D0_ACCDPUM, NULL, cmucal_vclk_ip_lhs_ast_d0_accdpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D1_ACCDPUM, NULL, cmucal_vclk_ip_lhs_ast_d1_accdpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_OTF_ISPB0ACC, NULL, cmucal_vclk_ip_lhm_ast_otf_ispb0acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_OTF_ISPB1ACC, NULL, cmucal_vclk_ip_lhm_ast_otf_ispb1acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CSIS_LINK_MUX3X6, NULL, cmucal_vclk_ip_csis_link_mux3x6, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_ISPPRE, NULL, cmucal_vclk_ip_vgen_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_ISPPRE, NULL, cmucal_vclk_ip_sysmmu_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_ISPPRE, NULL, cmucal_vclk_ip_ppmu_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACC_CMU_ACC, NULL, cmucal_vclk_ip_acc_cmu_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D1_ACC, NULL, cmucal_vclk_ip_lhs_axi_d1_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_ISPPRE, NULL, cmucal_vclk_ip_btm_isppre, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_APM, NULL, cmucal_vclk_ip_lhs_axi_d_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_APM, NULL, cmucal_vclk_ip_lhm_axi_p_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT_APM, NULL, cmucal_vclk_ip_wdt_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_APM, NULL, cmucal_vclk_ip_sysreg_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_APM_AP, NULL, cmucal_vclk_ip_mailbox_apm_ap, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBIF_PMU_ALIVE, NULL, cmucal_vclk_ip_apbif_pmu_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_INTMEM, NULL, cmucal_vclk_ip_intmem, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_G_SCAN2DRAM, NULL, cmucal_vclk_ip_lhs_axi_g_scan2dram, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PMU_INTR_GEN, NULL, cmucal_vclk_ip_pmu_intr_gen, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SPEEDY_APM, NULL, cmucal_vclk_ip_speedy_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_DP_APM, NULL, cmucal_vclk_ip_xiu_dp_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APM_CMU_APM, NULL, cmucal_vclk_ip_apm_cmu_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GREBEINTEGRATION, NULL, cmucal_vclk_ip_grebeintegration, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBIF_GPIO_ALIVE, NULL, cmucal_vclk_ip_apbif_gpio_alive, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBIF_TOP_RTC, NULL, cmucal_vclk_ip_apbif_top_rtc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SS_DBGCORE, NULL, cmucal_vclk_ip_ss_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_APM, NULL, cmucal_vclk_ip_d_tzpc_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_AP_DBGCORE, NULL, cmucal_vclk_ip_mailbox_ap_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_G_DBGCORE, NULL, cmucal_vclk_ip_lhs_axi_g_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBIF_RTC, NULL, cmucal_vclk_ip_apbif_rtc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ROM_CRC32_HCU, NULL, cmucal_vclk_ip_rom_crc32_hcu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ROM_CRC32_HOST, NULL, cmucal_vclk_ip_rom_crc32_host, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_APM_SFI0, NULL, cmucal_vclk_ip_mailbox_apm_sfi0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_SFIAPM, NULL, cmucal_vclk_ip_lhm_axi_p_sfiapm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SPEEDY_SUB0_APM, NULL, cmucal_vclk_ip_speedy_sub0_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_APM_SFI1, NULL, cmucal_vclk_ip_mailbox_apm_sfi1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AUD_CMU_AUD, NULL, cmucal_vclk_ip_aud_cmu_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_AUD, NULL, cmucal_vclk_ip_lhs_axi_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D_AUD, NULL, cmucal_vclk_ip_ppmu_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_AUD, NULL, cmucal_vclk_ip_sysreg_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ABOX, NULL, cmucal_vclk_ip_abox, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPIO_AUD, NULL, cmucal_vclk_ip_gpio_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AXI_US_32TO128, NULL, cmucal_vclk_ip_axi_us_32to128, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D_AUD, NULL, cmucal_vclk_ip_btm_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PERI_AXI_ASB, NULL, cmucal_vclk_ip_peri_axi_asb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_AUD, NULL, cmucal_vclk_ip_lhm_axi_p_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT_AUD0, NULL, cmucal_vclk_ip_wdt_aud0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DFTMUX_AUD, NULL, cmucal_vclk_ip_dftmux_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_AXI_D_AUD, NULL, cmucal_vclk_ip_sysmmu_axi_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_SMMU_AUD_S1_NS, NULL, cmucal_vclk_ip_ad_apb_smmu_aud_s1_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_SMMU_AUD_S1_S, NULL, cmucal_vclk_ip_ad_apb_smmu_aud_s1_s, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_SMMU_AUD_S2, NULL, cmucal_vclk_ip_ad_apb_smmu_aud_s2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT_AUD1, NULL, cmucal_vclk_ip_wdt_aud1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT_AUD2, NULL, cmucal_vclk_ip_wdt_aud2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT_AUD3, NULL, cmucal_vclk_ip_wdt_aud3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_VGEN_AUD0, NULL, cmucal_vclk_ip_ad_apb_vgen_aud0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_AUD0, NULL, cmucal_vclk_ip_vgen_aud0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_AUD1, NULL, cmucal_vclk_ip_vgen_aud1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_AUD, NULL, cmucal_vclk_ip_vgen_lite_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_VGEN_AUD1, NULL, cmucal_vclk_ip_ad_apb_vgen_aud1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_AUD, NULL, cmucal_vclk_ip_d_tzpc_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_D_BUSC, NULL, cmucal_vclk_ip_trex_d_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_BUSC, NULL, cmucal_vclk_ip_sysreg_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_BUSC, NULL, cmucal_vclk_ip_d_tzpc_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D_FSYS1, NULL, cmucal_vclk_ip_lhm_acel_d_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D0_DPUM, NULL, cmucal_vclk_ip_lhm_axi_d0_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D0_DPUS0, NULL, cmucal_vclk_ip_lhm_axi_d0_dpus0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D0_DPUS1, NULL, cmucal_vclk_ip_lhm_axi_d0_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D1_G2D, NULL, cmucal_vclk_ip_lhm_acel_d1_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D0_MFC, NULL, cmucal_vclk_ip_lhm_axi_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D1_DPUM, NULL, cmucal_vclk_ip_lhm_axi_d1_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D1_DPUS0, NULL, cmucal_vclk_ip_lhm_axi_d1_dpus0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D1_DPUS1, NULL, cmucal_vclk_ip_lhm_axi_d1_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D0_G2D, NULL, cmucal_vclk_ip_lhm_acel_d0_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D1_MFC, NULL, cmucal_vclk_ip_lhm_axi_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D2_DPUM, NULL, cmucal_vclk_ip_lhm_axi_d2_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D2_DPUS0, NULL, cmucal_vclk_ip_lhm_axi_d2_dpus0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D2_DPUS1, NULL, cmucal_vclk_ip_lhm_axi_d2_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D2_G2D, NULL, cmucal_vclk_ip_lhm_acel_d2_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D3_DPUM, NULL, cmucal_vclk_ip_lhm_axi_d3_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D3_DPUS0, NULL, cmucal_vclk_ip_lhm_axi_d3_dpus0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D3_DPUS1, NULL, cmucal_vclk_ip_lhm_axi_d3_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D0_ACC, NULL, cmucal_vclk_ip_lhm_axi_d0_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_ISPB0, NULL, cmucal_vclk_ip_lhm_axi_d_ispb0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_ISPB1, NULL, cmucal_vclk_ip_lhm_axi_d_ispb1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_TAA0, NULL, cmucal_vclk_ip_lhm_axi_d_taa0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_TAA1, NULL, cmucal_vclk_ip_lhm_axi_d_taa1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_DBG_G_BUSC, NULL, cmucal_vclk_ip_lhs_dbg_g_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D1_ACC, NULL, cmucal_vclk_ip_lhm_axi_d1_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_P_BUSMC, NULL, cmucal_vclk_ip_trex_p_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADM_AHB_SSS, NULL, cmucal_vclk_ip_adm_ahb_sss, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_PUF, NULL, cmucal_vclk_ip_ad_apb_puf, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BAAW_D_SSS, NULL, cmucal_vclk_ip_baaw_d_sss, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BAAW_P_SFI_BUSC, NULL, cmucal_vclk_ip_baaw_p_sfi_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_BUSMC_DP, NULL, cmucal_vclk_ip_lhm_axi_d_busmc_dp, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_SSS, NULL, cmucal_vclk_ip_lhm_axi_d_sss, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_SSS, NULL, cmucal_vclk_ip_lhs_axi_d_sss, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D0_PCIE, NULL, cmucal_vclk_ip_lhs_axi_d0_pcie, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D1_PCIE, NULL, cmucal_vclk_ip_lhs_axi_d1_pcie, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_ACC, NULL, cmucal_vclk_ip_lhs_axi_p_acc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_AUD, NULL, cmucal_vclk_ip_lhs_axi_p_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_BUSMC, NULL, cmucal_vclk_ip_lhs_axi_p_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_DPTX, NULL, cmucal_vclk_ip_lhs_axi_p_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_DPUM, NULL, cmucal_vclk_ip_lhs_axi_p_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_DPUS0, NULL, cmucal_vclk_ip_lhs_axi_p_dpus0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_DPUS1, NULL, cmucal_vclk_ip_lhs_axi_p_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_FSYS2, NULL, cmucal_vclk_ip_lhs_axi_p_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_FSYS0, NULL, cmucal_vclk_ip_lhs_axi_p_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_FSYS1, NULL, cmucal_vclk_ip_lhs_axi_p_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_G2D, NULL, cmucal_vclk_ip_lhs_axi_p_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_ISPB0, NULL, cmucal_vclk_ip_lhs_axi_p_ispb0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_ISPB1, NULL, cmucal_vclk_ip_lhs_axi_p_ispb1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_MFC, NULL, cmucal_vclk_ip_lhs_axi_p_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_MIF0, NULL, cmucal_vclk_ip_lhs_axi_p_mif0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_MIF1, NULL, cmucal_vclk_ip_lhs_axi_p_mif1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_MIF2, NULL, cmucal_vclk_ip_lhs_axi_p_mif2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_MIF3, NULL, cmucal_vclk_ip_lhs_axi_p_mif3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_PERIC0, NULL, cmucal_vclk_ip_lhs_axi_p_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_PERIC1, NULL, cmucal_vclk_ip_lhs_axi_p_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_PERIS, NULL, cmucal_vclk_ip_lhs_axi_p_peris, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_TAA0, NULL, cmucal_vclk_ip_lhs_axi_p_taa0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_TAA1, NULL, cmucal_vclk_ip_lhs_axi_p_taa1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PUF, NULL, cmucal_vclk_ip_puf, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_BUSC, NULL, cmucal_vclk_ip_xiu_d_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_BUSC, NULL, cmucal_vclk_ip_vgen_lite_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SSS, NULL, cmucal_vclk_ip_sss, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SIREX, NULL, cmucal_vclk_ip_sirex, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_RTIC, NULL, cmucal_vclk_ip_rtic, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_SSS, NULL, cmucal_vclk_ip_qe_sss, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_RTIC, NULL, cmucal_vclk_ip_qe_rtic, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BUSC_CMU_BUSC, NULL, cmucal_vclk_ip_busc_cmu_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_DNC, NULL, cmucal_vclk_ip_lhs_axi_p_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_NPU00, NULL, cmucal_vclk_ip_lhs_axi_p_npu00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_NPU01, NULL, cmucal_vclk_ip_lhs_axi_p_npu01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_NPU10, NULL, cmucal_vclk_ip_lhs_axi_p_npu10, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_NPU11, NULL, cmucal_vclk_ip_lhs_axi_p_npu11, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D_FSYS2, NULL, cmucal_vclk_ip_lhm_acel_d_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_BUSC, NULL, cmucal_vclk_ip_sysmmu_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_SFI, NULL, cmucal_vclk_ip_lhs_axi_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_DNCSRAM, NULL, cmucal_vclk_ip_lhs_axi_d_dncsram, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_SYSMMU_BUSC, NULL, cmucal_vclk_ip_ad_apb_sysmmu_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BUSMC_CMU_BUSMC, NULL, cmucal_vclk_ip_busmc_cmu_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_BUSMC, NULL, cmucal_vclk_ip_sysreg_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BUSIF_CMUTOPC, NULL, cmucal_vclk_ip_busif_cmutopc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_D0_BUSMC, NULL, cmucal_vclk_ip_trex_d0_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D0_FSYS0, NULL, cmucal_vclk_ip_lhm_acel_d0_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D7_DNC, NULL, cmucal_vclk_ip_lhm_acel_d7_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D1_FSYS0, NULL, cmucal_vclk_ip_lhm_acel_d1_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_APM, NULL, cmucal_vclk_ip_lhm_axi_d_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_RB_BUSMC, NULL, cmucal_vclk_ip_trex_rb_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WRAP2_CONV_BUSMC, NULL, cmucal_vclk_ip_wrap2_conv_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_PDMA0, NULL, cmucal_vclk_ip_vgen_pdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PDMA0, NULL, cmucal_vclk_ip_pdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SPDMA, NULL, cmucal_vclk_ip_spdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_BUSMC, NULL, cmucal_vclk_ip_d_tzpc_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MMCACHE0, NULL, cmucal_vclk_ip_mmcache0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_D1_BUSMC, NULL, cmucal_vclk_ip_trex_d1_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_DBG_G_BUSMC, NULL, cmucal_vclk_ip_lhs_dbg_g_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_SPDMA, NULL, cmucal_vclk_ip_qe_spdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_PDMA0, NULL, cmucal_vclk_ip_qe_pdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D0_BUSMC, NULL, cmucal_vclk_ip_xiu_d0_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_MMCACHE0_NS, NULL, cmucal_vclk_ip_ad_apb_mmcache0_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_DC_DNC, NULL, cmucal_vclk_ip_lhm_acel_dc_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_SFI, NULL, cmucal_vclk_ip_lhm_axi_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_AUD, NULL, cmucal_vclk_ip_lhm_axi_d_aud, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MMCACHE1, NULL, cmucal_vclk_ip_mmcache1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_BUSMC_DP, NULL, cmucal_vclk_ip_lhs_axi_d_busmc_dp, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_BUSMC, NULL, cmucal_vclk_ip_lhm_axi_p_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D0_DNC, NULL, cmucal_vclk_ip_lhm_acel_d0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D1_DNC, NULL, cmucal_vclk_ip_lhm_acel_d1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D2_DNC, NULL, cmucal_vclk_ip_lhm_acel_d2_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D3_DNC, NULL, cmucal_vclk_ip_lhm_acel_d3_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D4_DNC, NULL, cmucal_vclk_ip_lhm_acel_d4_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D5_DNC, NULL, cmucal_vclk_ip_lhm_acel_d5_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACEL_D6_DNC, NULL, cmucal_vclk_ip_lhm_acel_d6_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_BUSMC, NULL, cmucal_vclk_ip_sysmmu_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_APM, NULL, cmucal_vclk_ip_vgen_lite_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_SFI, NULL, cmucal_vclk_ip_vgen_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_SFI, NULL, cmucal_vclk_ip_sysmmu_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SFMPU_BUSMC, NULL, cmucal_vclk_ip_sfmpu_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CORE_CMU_CORE, NULL, cmucal_vclk_ip_core_cmu_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_CORE, NULL, cmucal_vclk_ip_sysreg_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE2AXI_0, NULL, cmucal_vclk_ip_mpace2axi_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE2AXI_1, NULL, cmucal_vclk_ip_mpace2axi_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_DEBUG_CCI, NULL, cmucal_vclk_ip_ppc_debug_cci, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_P0_CORE, NULL, cmucal_vclk_ip_trex_p0_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_CPUCL1_0, NULL, cmucal_vclk_ip_ppmu_cpucl1_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G0_DMC0, NULL, cmucal_vclk_ip_lhm_dbg_g0_dmc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G1_DMC0, NULL, cmucal_vclk_ip_lhm_dbg_g1_dmc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G0_DMC1, NULL, cmucal_vclk_ip_lhm_dbg_g0_dmc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G1_DMC1, NULL, cmucal_vclk_ip_lhm_dbg_g1_dmc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T_BDU, NULL, cmucal_vclk_ip_lhs_atb_t_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADM_APB_G_BDU, NULL, cmucal_vclk_ip_adm_apb_g_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BDU, NULL, cmucal_vclk_ip_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_P1_CORE, NULL, cmucal_vclk_ip_trex_p1_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_G3D00, NULL, cmucal_vclk_ip_lhs_axi_p_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_CPUCL0, NULL, cmucal_vclk_ip_lhs_axi_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_CPUCL1, NULL, cmucal_vclk_ip_lhs_axi_p_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D0_G3D1, NULL, cmucal_vclk_ip_lhm_ace_d0_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D1_G3D1, NULL, cmucal_vclk_ip_lhm_ace_d1_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D2_G3D1, NULL, cmucal_vclk_ip_lhm_ace_d2_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D3_G3D1, NULL, cmucal_vclk_ip_lhm_ace_d3_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_D_CORE, NULL, cmucal_vclk_ip_trex_d_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPCFW_G3D0_0, NULL, cmucal_vclk_ip_ppcfw_g3d0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_APM, NULL, cmucal_vclk_ip_lhs_axi_p_apm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_CPUCL1_1, NULL, cmucal_vclk_ip_ppmu_cpucl1_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_CORE, NULL, cmucal_vclk_ip_d_tzpc_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_CPUCL1_0, NULL, cmucal_vclk_ip_ppc_cpucl1_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_CPUCL1_1, NULL, cmucal_vclk_ip_ppc_cpucl1_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_G3D1_0, NULL, cmucal_vclk_ip_ppc_g3d1_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_G3D1_1, NULL, cmucal_vclk_ip_ppc_g3d1_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_G3D1_2, NULL, cmucal_vclk_ip_ppc_g3d1_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_G3D1_3, NULL, cmucal_vclk_ip_ppc_g3d1_3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_IRPS0, NULL, cmucal_vclk_ip_ppc_irps0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_IRPS1, NULL, cmucal_vclk_ip_ppc_irps1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G_BUSC, NULL, cmucal_vclk_ip_lhm_dbg_g_busc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE_ASB_D0_MIF, NULL, cmucal_vclk_ip_mpace_asb_d0_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE_ASB_D1_MIF, NULL, cmucal_vclk_ip_mpace_asb_d1_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE_ASB_D2_MIF, NULL, cmucal_vclk_ip_mpace_asb_d2_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE_ASB_D3_MIF, NULL, cmucal_vclk_ip_mpace_asb_d3_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_G_CSSYS, NULL, cmucal_vclk_ip_lhm_axi_g_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AXI_ASB_CSSYS, NULL, cmucal_vclk_ip_axi_asb_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CCI, NULL, cmucal_vclk_ip_cci, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D0_CLUSTER1, NULL, cmucal_vclk_ip_lhm_ace_d0_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D_G3D00, NULL, cmucal_vclk_ip_lhm_ace_d_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D1_CLUSTER1, NULL, cmucal_vclk_ip_lhm_ace_d1_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D_G3D01, NULL, cmucal_vclk_ip_lhm_ace_d_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G0_DMC2, NULL, cmucal_vclk_ip_lhm_dbg_g0_dmc2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G1_DMC2, NULL, cmucal_vclk_ip_lhm_dbg_g1_dmc2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G0_DMC3, NULL, cmucal_vclk_ip_lhm_dbg_g0_dmc3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G1_DMC3, NULL, cmucal_vclk_ip_lhm_dbg_g1_dmc3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_G3D1, NULL, cmucal_vclk_ip_lhs_axi_p_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE_ASB_D4_MIF, NULL, cmucal_vclk_ip_mpace_asb_d4_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE_ASB_D5_MIF, NULL, cmucal_vclk_ip_mpace_asb_d5_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE_ASB_D6_MIF, NULL, cmucal_vclk_ip_mpace_asb_d6_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MPACE_ASB_D7_MIF, NULL, cmucal_vclk_ip_mpace_asb_d7_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPCFW_G3D1, NULL, cmucal_vclk_ip_ppcfw_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_G3D0_0, NULL, cmucal_vclk_ip_ppc_g3d0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_G3D0_1, NULL, cmucal_vclk_ip_ppc_g3d0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_IRPS2, NULL, cmucal_vclk_ip_ppc_irps2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_IRPS3, NULL, cmucal_vclk_ip_ppc_irps3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPCFW_G3D0_1, NULL, cmucal_vclk_ip_ppcfw_g3d0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_G3D01, NULL, cmucal_vclk_ip_lhs_axi_p_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_DBG_G_BUSMC, NULL, cmucal_vclk_ip_lhm_dbg_g_busmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_CPUCL0_0, NULL, cmucal_vclk_ip_ppmu_cpucl0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_CPUCL0_1, NULL, cmucal_vclk_ip_ppmu_cpucl0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_CPUCL0_0, NULL, cmucal_vclk_ip_ppc_cpucl0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_CPUCL0_1, NULL, cmucal_vclk_ip_ppc_cpucl0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D0_CLUSTER0, NULL, cmucal_vclk_ip_lhm_ace_d0_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ACE_D1_CLUSTER0, NULL, cmucal_vclk_ip_lhm_ace_d1_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D1_0, NULL, cmucal_vclk_ip_ace_slice_g3d1_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D1_1, NULL, cmucal_vclk_ip_ace_slice_g3d1_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D1_2, NULL, cmucal_vclk_ip_ace_slice_g3d1_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D1_3, NULL, cmucal_vclk_ip_ace_slice_g3d1_3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G3D1_0, NULL, cmucal_vclk_ip_ppmu_g3d1_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G3D1_1, NULL, cmucal_vclk_ip_ppmu_g3d1_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G3D1_2, NULL, cmucal_vclk_ip_ppmu_g3d1_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G3D1_3, NULL, cmucal_vclk_ip_ppmu_g3d1_3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G3D0_0, NULL, cmucal_vclk_ip_sysmmu_g3d0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G3D0_1, NULL, cmucal_vclk_ip_sysmmu_g3d0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G3D1_0, NULL, cmucal_vclk_ip_sysmmu_g3d1_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G3D1_1, NULL, cmucal_vclk_ip_sysmmu_g3d1_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G3D1_2, NULL, cmucal_vclk_ip_sysmmu_g3d1_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G3D1_3, NULL, cmucal_vclk_ip_sysmmu_g3d1_3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_CORE, NULL, cmucal_vclk_ip_xiu_d_core, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_SYSMMU_G3D0_0, NULL, cmucal_vclk_ip_apb_async_sysmmu_g3d0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_SYSMMU_G3D0_1, NULL, cmucal_vclk_ip_apb_async_sysmmu_g3d0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_SYSMMU_G3D1_0, NULL, cmucal_vclk_ip_apb_async_sysmmu_g3d1_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_SYSMMU_G3D1_1, NULL, cmucal_vclk_ip_apb_async_sysmmu_g3d1_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_SYSMMU_G3D1_2, NULL, cmucal_vclk_ip_apb_async_sysmmu_g3d1_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_SYSMMU_G3D1_3, NULL, cmucal_vclk_ip_apb_async_sysmmu_g3d1_3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D0_0, NULL, cmucal_vclk_ip_ace_slice_g3d0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D0_1, NULL, cmucal_vclk_ip_ace_slice_g3d0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D1_1_0, NULL, cmucal_vclk_ip_ace_slice_g3d1_1_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D1_1_1, NULL, cmucal_vclk_ip_ace_slice_g3d1_1_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D1_1_2, NULL, cmucal_vclk_ip_ace_slice_g3d1_1_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ACE_SLICE_G3D1_1_3, NULL, cmucal_vclk_ip_ace_slice_g3d1_1_3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D0_0_QOS, NULL, cmucal_vclk_ip_g3d0_0_qos, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LITTLE_QOS_0, NULL, cmucal_vclk_ip_little_qos_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LITTLE_QOS_1, NULL, cmucal_vclk_ip_little_qos_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BIG_QOS_0, NULL, cmucal_vclk_ip_big_qos_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BIG_QOS_1, NULL, cmucal_vclk_ip_big_qos_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D0_1_QOS, NULL, cmucal_vclk_ip_g3d0_1_qos, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D1_0_QOS, NULL, cmucal_vclk_ip_g3d1_0_qos, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D1_1_QOS, NULL, cmucal_vclk_ip_g3d1_1_qos, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D1_2_QOS, NULL, cmucal_vclk_ip_g3d1_2_qos, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D1_3_QOS, NULL, cmucal_vclk_ip_g3d1_3_qos, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_CPUCL0, NULL, cmucal_vclk_ip_sysreg_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CSSYS, NULL, cmucal_vclk_ip_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T_BDU, NULL, cmucal_vclk_ip_lhm_atb_t_bdu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T0_CLUSTER0, NULL, cmucal_vclk_ip_lhm_atb_t0_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T0_CLUSTER1, NULL, cmucal_vclk_ip_lhm_atb_t0_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T1_CLUSTER0, NULL, cmucal_vclk_ip_lhm_atb_t1_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T1_CLUSTER1, NULL, cmucal_vclk_ip_lhm_atb_t1_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T2_CLUSTER0, NULL, cmucal_vclk_ip_lhm_atb_t2_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T3_CLUSTER0, NULL, cmucal_vclk_ip_lhm_atb_t3_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SECJTAG, NULL, cmucal_vclk_ip_secjtag, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_CPUCL0, NULL, cmucal_vclk_ip_lhm_axi_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACE_D0_CLUSTER0, NULL, cmucal_vclk_ip_lhs_ace_d0_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T0_CLUSTER0, NULL, cmucal_vclk_ip_lhs_atb_t0_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T1_CLUSTER0, NULL, cmucal_vclk_ip_lhs_atb_t1_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T2_CLUSTER0, NULL, cmucal_vclk_ip_lhs_atb_t2_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T3_CLUSTER0, NULL, cmucal_vclk_ip_lhs_atb_t3_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CPUCL0_CMU_CPUCL0, NULL, cmucal_vclk_ip_cpucl0_cmu_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CLUSTER0, NULL, cmucal_vclk_ip_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_CPUCL0, NULL, cmucal_vclk_ip_d_tzpc_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_G_INT_CSSYS, NULL, cmucal_vclk_ip_lhs_axi_g_int_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_G_INT_CSSYS, NULL, cmucal_vclk_ip_lhm_axi_g_int_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_P_CPUCL0, NULL, cmucal_vclk_ip_xiu_p_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_DP_CSSYS, NULL, cmucal_vclk_ip_xiu_dp_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_CPUCL0, NULL, cmucal_vclk_ip_trex_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AXI_US_32TO64_G_DBGCORE, NULL, cmucal_vclk_ip_axi_us_32to64_g_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_G_CSSYS, NULL, cmucal_vclk_ip_lhs_axi_g_cssys, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_P_CSSYS_0, NULL, cmucal_vclk_ip_apb_async_p_cssys_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_G_INT_ETR, NULL, cmucal_vclk_ip_lhs_axi_g_int_etr, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_G_INT_ETR, NULL, cmucal_vclk_ip_lhm_axi_g_int_etr, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BPS_CPUCL0, NULL, cmucal_vclk_ip_bps_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACE_D1_CLUSTER0, NULL, cmucal_vclk_ip_lhs_ace_d1_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T2_CLUSTER1, NULL, cmucal_vclk_ip_lhm_atb_t2_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_T3_CLUSTER1, NULL, cmucal_vclk_ip_lhm_atb_t3_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_DT0_SFI, NULL, cmucal_vclk_ip_lhm_atb_dt0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_DT1_SFI, NULL, cmucal_vclk_ip_lhm_atb_dt1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_IT0_SFI, NULL, cmucal_vclk_ip_lhm_atb_it0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_ATB_IT1_SFI, NULL, cmucal_vclk_ip_lhm_atb_it1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADM_APB_G_CLUSTER0, NULL, cmucal_vclk_ip_adm_apb_g_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_DP_DBGCORE, NULL, cmucal_vclk_ip_xiu_dp_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_G_INT_STM, NULL, cmucal_vclk_ip_lhm_axi_g_int_stm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_G_INT_STM, NULL, cmucal_vclk_ip_lhs_axi_g_int_stm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_G_INT_DBGCORE, NULL, cmucal_vclk_ip_lhm_axi_g_int_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_G_DBGCORE, NULL, cmucal_vclk_ip_lhm_axi_g_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_G_INT_DBGCORE, NULL, cmucal_vclk_ip_lhs_axi_g_int_dbgcore, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_HPM_APBIF_CPUCL0, NULL, cmucal_vclk_ip_hpm_apbif_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_HPM_CPUCL0, NULL, cmucal_vclk_ip_hpm_cpucl0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CLUSTER1, NULL, cmucal_vclk_ip_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CPUCL1_CMU_CPUCL1, NULL, cmucal_vclk_ip_cpucl1_cmu_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_CPUCL1, NULL, cmucal_vclk_ip_sysreg_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T3_CLUSTER1, NULL, cmucal_vclk_ip_lhs_atb_t3_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T2_CLUSTER1, NULL, cmucal_vclk_ip_lhs_atb_t2_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T1_CLUSTER1, NULL, cmucal_vclk_ip_lhs_atb_t1_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_T0_CLUSTER1, NULL, cmucal_vclk_ip_lhs_atb_t0_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACE_D0_CLUSTER1, NULL, cmucal_vclk_ip_lhs_ace_d0_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_CPUCL1, NULL, cmucal_vclk_ip_lhm_axi_p_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_CPUCL1, NULL, cmucal_vclk_ip_d_tzpc_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACE_D1_CLUSTER1, NULL, cmucal_vclk_ip_lhs_ace_d1_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADM_APB_G_CLUSTER1, NULL, cmucal_vclk_ip_adm_apb_g_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_HPM_CPUCL1, NULL, cmucal_vclk_ip_hpm_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_HPM_APBIF_CPUCL1, NULL, cmucal_vclk_ip_hpm_apbif_cpucl1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_NPU00, NULL, cmucal_vclk_ip_lhs_axi_d_npu00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DNC_CMU_DNC, NULL, cmucal_vclk_ip_dnc_cmu_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_DNC, NULL, cmucal_vclk_ip_d_tzpc_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D_NPU0_UNIT1_DONE, NULL, cmucal_vclk_ip_lhm_ast_d_npu0_unit1_done, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D_NPU1_UNIT0_DONE, NULL, cmucal_vclk_ip_lhm_ast_d_npu1_unit0_done, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D_NPU1_UNIT1_DONE, NULL, cmucal_vclk_ip_lhm_ast_d_npu1_unit1_done, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D_NPU0_UNIT0_DONE, NULL, cmucal_vclk_ip_lhm_ast_d_npu0_unit0_done, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D_NPU0_UNIT1_SETREG, NULL, cmucal_vclk_ip_lhs_ast_d_npu0_unit1_setreg, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D_NPU1_UNIT0_SETREG, NULL, cmucal_vclk_ip_lhs_ast_d_npu1_unit0_setreg, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D_NPU1_UNIT1_SETREG, NULL, cmucal_vclk_ip_lhs_ast_d_npu1_unit1_setreg, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_NPU01, NULL, cmucal_vclk_ip_lhs_axi_d_npu01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_NPU10, NULL, cmucal_vclk_ip_lhs_axi_d_npu10, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_NPU11, NULL, cmucal_vclk_ip_lhs_axi_d_npu11, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D_NPU0_UNIT0_SETREG, NULL, cmucal_vclk_ip_lhs_ast_d_npu0_unit0_setreg, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_IP_NPUC, NULL, cmucal_vclk_ip_ip_npuc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_DNC, NULL, cmucal_vclk_ip_sysreg_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_DNC, NULL, cmucal_vclk_ip_vgen_lite_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TREX_D_DNC, NULL, cmucal_vclk_ip_trex_d_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_DSPC_NPUC0, NULL, cmucal_vclk_ip_lhm_axi_int_dspc_npuc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_NPUC_DSPC0, NULL, cmucal_vclk_ip_lhs_axi_int_npuc_dspc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_IP_DSPC, NULL, cmucal_vclk_ip_ip_dspc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_D0_SDMA0, NULL, cmucal_vclk_ip_lhm_axi_int_d0_sdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_D0_SDMA1, NULL, cmucal_vclk_ip_lhm_axi_int_d0_sdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_D0_SDMA2, NULL, cmucal_vclk_ip_lhm_axi_int_d0_sdma2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_D1_SDMA0, NULL, cmucal_vclk_ip_lhm_axi_int_d1_sdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_D1_SDMA1, NULL, cmucal_vclk_ip_lhm_axi_int_d1_sdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_D1_SDMA2, NULL, cmucal_vclk_ip_lhm_axi_int_d1_sdma2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_D_SRAMDMA, NULL, cmucal_vclk_ip_lhm_axi_int_d_sramdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D0, NULL, cmucal_vclk_ip_ppmu_d0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D1, NULL, cmucal_vclk_ip_ppmu_d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D2, NULL, cmucal_vclk_ip_ppmu_d2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D3, NULL, cmucal_vclk_ip_ppmu_d3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D4, NULL, cmucal_vclk_ip_ppmu_d4, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D5, NULL, cmucal_vclk_ip_ppmu_d5, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_DC, NULL, cmucal_vclk_ip_ppmu_dc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D0_DNCDMA, NULL, cmucal_vclk_ip_sysmmu_d0_dncdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D0_DNCFLC, NULL, cmucal_vclk_ip_sysmmu_d0_dncflc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D1_DNCDMA, NULL, cmucal_vclk_ip_sysmmu_d1_dncdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D1_DNCFLC, NULL, cmucal_vclk_ip_sysmmu_d1_dncflc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D2_DNCDMA, NULL, cmucal_vclk_ip_sysmmu_d2_dncdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D2_DNCFLC, NULL, cmucal_vclk_ip_sysmmu_d2_dncflc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_DC_DNC, NULL, cmucal_vclk_ip_sysmmu_dc_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D0_BTM_DNC, NULL, cmucal_vclk_ip_d0_btm_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DC_BTM_DNC, NULL, cmucal_vclk_ip_dc_btm_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D1_BTM_DNC, NULL, cmucal_vclk_ip_d1_btm_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D2_BTM_DNC, NULL, cmucal_vclk_ip_d2_btm_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D3_BTM_DNC, NULL, cmucal_vclk_ip_d3_btm_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D4_BTM_DNC, NULL, cmucal_vclk_ip_d4_btm_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D5_BTM_DNC, NULL, cmucal_vclk_ip_d5_btm_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_NPUC_DSPC1, NULL, cmucal_vclk_ip_lhs_axi_int_npuc_dspc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D1_DSPC_WRAPPER, NULL, cmucal_vclk_ip_xiu_d1_dspc_wrapper, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_DNCSRAM, NULL, cmucal_vclk_ip_lhm_axi_d_dncsram, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D0_DNC, NULL, cmucal_vclk_ip_lhs_acel_d0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D1_DNC, NULL, cmucal_vclk_ip_lhs_acel_d1_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D2_DNC, NULL, cmucal_vclk_ip_lhs_acel_d2_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D3_DNC, NULL, cmucal_vclk_ip_lhs_acel_d3_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D4_DNC, NULL, cmucal_vclk_ip_lhs_acel_d4_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D5_DNC, NULL, cmucal_vclk_ip_lhs_acel_d5_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D6_DNC, NULL, cmucal_vclk_ip_lhs_acel_d6_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_DC_DNC, NULL, cmucal_vclk_ip_lhs_acel_dc_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D7_DNC, NULL, cmucal_vclk_ip_lhs_acel_d7_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_DSPC0, NULL, cmucal_vclk_ip_lhm_axi_int_dspc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_NPUC_DSPC0, NULL, cmucal_vclk_ip_lhm_axi_int_npuc_dspc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_NPUC_DSPC1, NULL, cmucal_vclk_ip_lhm_axi_int_npuc_dspc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_DSPC0, NULL, cmucal_vclk_ip_lhs_axi_int_dspc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_D0_SDMA0, NULL, cmucal_vclk_ip_lhs_axi_int_d0_sdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_D0_SDMA1, NULL, cmucal_vclk_ip_lhs_axi_int_d0_sdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_D0_SDMA2, NULL, cmucal_vclk_ip_lhs_axi_int_d0_sdma2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_D1_SDMA0, NULL, cmucal_vclk_ip_lhs_axi_int_d1_sdma0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_D1_SDMA1, NULL, cmucal_vclk_ip_lhs_axi_int_d1_sdma1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_D1_SDMA2, NULL, cmucal_vclk_ip_lhs_axi_int_d1_sdma2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_D_SRAMDMA, NULL, cmucal_vclk_ip_lhs_axi_int_d_sramdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DAP_ASYNC, NULL, cmucal_vclk_ip_dap_async, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_INT_DSPC_NPUC1, NULL, cmucal_vclk_ip_lhm_axi_int_dspc_npuc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_DSPC_NPUC0, NULL, cmucal_vclk_ip_lhs_axi_int_dspc_npuc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_INT_DSPC_NPUC1, NULL, cmucal_vclk_ip_lhs_axi_int_dspc_npuc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D0_DNC, NULL, cmucal_vclk_ip_xiu_d0_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APB_ASYNC_SMMU_S1_NS_DC, NULL, cmucal_vclk_ip_apb_async_smmu_s1_ns_dc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_DNC, NULL, cmucal_vclk_ip_lhm_axi_p_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_RS_D0_DNCDMA, NULL, cmucal_vclk_ip_rs_d0_dncdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_RS_D0_DNCFLC, NULL, cmucal_vclk_ip_rs_d0_dncflc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_RS_D1_DNCDMA, NULL, cmucal_vclk_ip_rs_d1_dncdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_RS_D1_DNCFLC, NULL, cmucal_vclk_ip_rs_d1_dncflc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_RS_D2_DNCDMA, NULL, cmucal_vclk_ip_rs_d2_dncdma, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_RS_D2_DNCFLC, NULL, cmucal_vclk_ip_rs_d2_dncflc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_RS_DC_DNC, NULL, cmucal_vclk_ip_rs_dc_dnc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_DPTX, NULL, cmucal_vclk_ip_sysreg_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_DPTX, NULL, cmucal_vclk_ip_lhm_axi_p_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPTX_TOP0, NULL, cmucal_vclk_ip_dptx_top0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPTX_TOP1, NULL, cmucal_vclk_ip_dptx_top1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_DPTX, NULL, cmucal_vclk_ip_d_tzpc_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPTX_CMU_DPTX, NULL, cmucal_vclk_ip_dptx_cmu_dptx, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPUM_CMU_DPUM, NULL, cmucal_vclk_ip_dpum_cmu_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D0_DPUM, NULL, cmucal_vclk_ip_btm_d0_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D1_DPUM, NULL, cmucal_vclk_ip_btm_d1_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_DPUM, NULL, cmucal_vclk_ip_sysreg_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D0_DPUM, NULL, cmucal_vclk_ip_sysmmu_d0_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_DPUM, NULL, cmucal_vclk_ip_lhm_axi_p_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D1_DPUM, NULL, cmucal_vclk_ip_lhs_axi_d1_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_DPP_DPUM, NULL, cmucal_vclk_ip_ad_apb_dpp_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D2_DPUM, NULL, cmucal_vclk_ip_lhs_axi_d2_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D2_DPUM, NULL, cmucal_vclk_ip_btm_d2_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D2_DPUM, NULL, cmucal_vclk_ip_sysmmu_d2_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D1_DPUM, NULL, cmucal_vclk_ip_sysmmu_d1_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D0_DPUM, NULL, cmucal_vclk_ip_ppmu_d0_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D1_DPUM, NULL, cmucal_vclk_ip_ppmu_d1_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D2_DPUM, NULL, cmucal_vclk_ip_ppmu_d2_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WRAPPER_FOR_S5I6287_HSI_DPHY_COMBO_TOP, NULL, cmucal_vclk_ip_wrapper_for_s5i6287_hsi_dphy_combo_top, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D0_DPUM, NULL, cmucal_vclk_ip_lhs_axi_d0_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPUM, NULL, cmucal_vclk_ip_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_DPUM, NULL, cmucal_vclk_ip_d_tzpc_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D3_DPUM, NULL, cmucal_vclk_ip_btm_d3_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D3_DPUM, NULL, cmucal_vclk_ip_lhs_axi_d3_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D3_DPUM, NULL, cmucal_vclk_ip_sysmmu_d3_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D3_DPUM, NULL, cmucal_vclk_ip_ppmu_d3_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SFMPU_DPUM, NULL, cmucal_vclk_ip_sfmpu_dpum, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPUS_CMU_DPUS, NULL, cmucal_vclk_ip_dpus_cmu_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_DPUS, NULL, cmucal_vclk_ip_sysreg_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D3_DPUS, NULL, cmucal_vclk_ip_sysmmu_d3_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D2_DPUS, NULL, cmucal_vclk_ip_sysmmu_d2_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D1_DPUS, NULL, cmucal_vclk_ip_sysmmu_d1_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D0_DPUS, NULL, cmucal_vclk_ip_sysmmu_d0_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D3_DPUS, NULL, cmucal_vclk_ip_ppmu_d3_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D2_DPUS, NULL, cmucal_vclk_ip_ppmu_d2_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D1_DPUS, NULL, cmucal_vclk_ip_ppmu_d1_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D0_DPUS, NULL, cmucal_vclk_ip_ppmu_d0_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_DPUS, NULL, cmucal_vclk_ip_lhm_axi_p_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D0_DPUS, NULL, cmucal_vclk_ip_lhs_axi_d0_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D1_DPUS, NULL, cmucal_vclk_ip_lhs_axi_d1_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D2_DPUS, NULL, cmucal_vclk_ip_lhs_axi_d2_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D3_DPUS, NULL, cmucal_vclk_ip_lhs_axi_d3_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_DPUS, NULL, cmucal_vclk_ip_d_tzpc_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPUS, NULL, cmucal_vclk_ip_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D0_DPUS, NULL, cmucal_vclk_ip_btm_d0_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D1_DPUS, NULL, cmucal_vclk_ip_btm_d1_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D2_DPUS, NULL, cmucal_vclk_ip_btm_d2_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D3_DPUS, NULL, cmucal_vclk_ip_btm_d3_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_DPP_DPUS, NULL, cmucal_vclk_ip_ad_apb_dpp_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SFMPU_DPUS, NULL, cmucal_vclk_ip_sfmpu_dpus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPUS1_CMU_DPUS1, NULL, cmucal_vclk_ip_dpus1_cmu_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_DPUS1, NULL, cmucal_vclk_ip_sysreg_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D3_DPUS1, NULL, cmucal_vclk_ip_sysmmu_d3_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D2_DPUS1, NULL, cmucal_vclk_ip_sysmmu_d2_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D1_DPUS1, NULL, cmucal_vclk_ip_sysmmu_d1_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D0_DPUS1, NULL, cmucal_vclk_ip_sysmmu_d0_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D3_DPUS1, NULL, cmucal_vclk_ip_ppmu_d3_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D2_DPUS1, NULL, cmucal_vclk_ip_ppmu_d2_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D1_DPUS1, NULL, cmucal_vclk_ip_ppmu_d1_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D0_DPUS1, NULL, cmucal_vclk_ip_ppmu_d0_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_DPUS1, NULL, cmucal_vclk_ip_lhm_axi_p_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D0_DPUS1, NULL, cmucal_vclk_ip_lhs_axi_d0_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D1_DPUS1, NULL, cmucal_vclk_ip_lhs_axi_d1_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D2_DPUS1, NULL, cmucal_vclk_ip_lhs_axi_d2_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D3_DPUS1, NULL, cmucal_vclk_ip_lhs_axi_d3_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_DPUS1, NULL, cmucal_vclk_ip_d_tzpc_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DPUS1, NULL, cmucal_vclk_ip_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D0_DPUS1, NULL, cmucal_vclk_ip_btm_d0_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D1_DPUS1, NULL, cmucal_vclk_ip_btm_d1_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D2_DPUS1, NULL, cmucal_vclk_ip_btm_d2_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D3_DPUS1, NULL, cmucal_vclk_ip_btm_d3_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_DPP_DPUS1, NULL, cmucal_vclk_ip_ad_apb_dpp_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SFMPU_DPUS1, NULL, cmucal_vclk_ip_sfmpu_dpus1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_FSYS0_CMU_FSYS0, NULL, cmucal_vclk_ip_fsys0_cmu_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D0_FSYS0, NULL, cmucal_vclk_ip_lhs_acel_d0_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_FSYS0, NULL, cmucal_vclk_ip_lhm_axi_p_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPIO_FSYS0, NULL, cmucal_vclk_ip_gpio_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_FSYS0, NULL, cmucal_vclk_ip_sysreg_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_FSYS0, NULL, cmucal_vclk_ip_xiu_d_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D0_FSYS0, NULL, cmucal_vclk_ip_btm_d0_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D0_PCIE, NULL, cmucal_vclk_ip_lhm_axi_d0_pcie, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D0_FSYS0, NULL, cmucal_vclk_ip_ppmu_d0_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_P_FSYS0, NULL, cmucal_vclk_ip_xiu_p_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_GEN3_2L0, NULL, cmucal_vclk_ip_pcie_gen3_2l0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_IA_GEN3A_2L0, NULL, cmucal_vclk_ip_pcie_ia_gen3a_2l0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_IA_GEN3B_2L0, NULL, cmucal_vclk_ip_pcie_ia_gen3b_2l0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_FSYS0_0, NULL, cmucal_vclk_ip_d_tzpc_fsys0_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_FSYS0_PCIE_SLV, NULL, cmucal_vclk_ip_xiu_d_fsys0_pcie_slv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D1_FSYS0, NULL, cmucal_vclk_ip_ppmu_d1_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_IA_GEN3A_2L1, NULL, cmucal_vclk_ip_pcie_ia_gen3a_2l1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_IA_GEN3B_2L1, NULL, cmucal_vclk_ip_pcie_ia_gen3b_2l1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_IA_GEN3A_4L, NULL, cmucal_vclk_ip_pcie_ia_gen3a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_IA_GEN3B_4L, NULL, cmucal_vclk_ip_pcie_ia_gen3b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D1_FSYS0, NULL, cmucal_vclk_ip_btm_d1_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D1_PCIE, NULL, cmucal_vclk_ip_lhm_axi_d1_pcie, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D1_FSYS0, NULL, cmucal_vclk_ip_lhs_acel_d1_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_GEN3_4L, NULL, cmucal_vclk_ip_pcie_gen3_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PCIE_GEN3_2L1, NULL, cmucal_vclk_ip_pcie_gen3_2l1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D_FSYS0SFI, NULL, cmucal_vclk_ip_btm_d_fsys0sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_SFIFSYS0, NULL, cmucal_vclk_ip_lhm_axi_p_sfifsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_FSYS0SFI, NULL, cmucal_vclk_ip_lhs_axi_d_fsys0sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_FSYS0_1, NULL, cmucal_vclk_ip_d_tzpc_fsys0_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_PCIE_GEN3A_2L0, NULL, cmucal_vclk_ip_vgen_pcie_gen3a_2l0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_PCIE_GEN3A_2L1, NULL, cmucal_vclk_ip_vgen_pcie_gen3a_2l1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_PCIE_GEN3A_4L, NULL, cmucal_vclk_ip_vgen_pcie_gen3a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_PCIE_GEN3B_2L0, NULL, cmucal_vclk_ip_vgen_pcie_gen3b_2l0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_PCIE_GEN3B_2L1, NULL, cmucal_vclk_ip_vgen_pcie_gen3b_2l1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_PCIE_GEN3B_4L, NULL, cmucal_vclk_ip_vgen_pcie_gen3b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D_FSYS0SFI, NULL, cmucal_vclk_ip_ppmu_d_fsys0sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_PCIE_GEN3A_2L0, NULL, cmucal_vclk_ip_qe_pcie_gen3a_2l0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_PCIE_GEN3A_2L1, NULL, cmucal_vclk_ip_qe_pcie_gen3a_2l1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_PCIE_GEN3A_4L, NULL, cmucal_vclk_ip_qe_pcie_gen3a_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_PCIE_GEN3B_2L0, NULL, cmucal_vclk_ip_qe_pcie_gen3b_2l0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_PCIE_GEN3B_2L1, NULL, cmucal_vclk_ip_qe_pcie_gen3b_2l1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_PCIE_GEN3B_4L, NULL, cmucal_vclk_ip_qe_pcie_gen3b_4l, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_FSYS2FSYS0, NULL, cmucal_vclk_ip_lhm_axi_d_fsys2fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_FSYS0FSYS2, NULL, cmucal_vclk_ip_lhs_axi_p_fsys0fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_S2MPU_D0_FSYS0, NULL, cmucal_vclk_ip_s2mpu_d0_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_S2MPU_D1_FSYS0, NULL, cmucal_vclk_ip_s2mpu_d1_fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_FSYS1_CMU_FSYS1, NULL, cmucal_vclk_ip_fsys1_cmu_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MMC_CARD, NULL, cmucal_vclk_ip_mmc_card, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_FSYS1, NULL, cmucal_vclk_ip_sysreg_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPIO_FSYS1, NULL, cmucal_vclk_ip_gpio_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D_FSYS1, NULL, cmucal_vclk_ip_lhs_acel_d_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_FSYS1, NULL, cmucal_vclk_ip_lhm_axi_p_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_FSYS1, NULL, cmucal_vclk_ip_xiu_d_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_P_FSYS1, NULL, cmucal_vclk_ip_xiu_p_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_FSYS1, NULL, cmucal_vclk_ip_ppmu_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_FSYS1, NULL, cmucal_vclk_ip_btm_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_FSYS1, NULL, cmucal_vclk_ip_vgen_lite_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_FSYS1, NULL, cmucal_vclk_ip_d_tzpc_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_USB20DRD_0, NULL, cmucal_vclk_ip_usb20drd_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_USB20DRD_1, NULL, cmucal_vclk_ip_usb20drd_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_USB30DRD_0, NULL, cmucal_vclk_ip_usb30drd_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_USB30DRD_1, NULL, cmucal_vclk_ip_usb30drd_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_US_D_USB3_0, NULL, cmucal_vclk_ip_us_d_usb3_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_US_D_USB3_1, NULL, cmucal_vclk_ip_us_d_usb3_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_US_D_USB2_0, NULL, cmucal_vclk_ip_us_d_usb2_0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_US_D_USB2_1, NULL, cmucal_vclk_ip_us_d_usb2_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_S2MPU_D_FSYS1, NULL, cmucal_vclk_ip_s2mpu_d_fsys1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_FSYS2_CMU_FSYS2, NULL, cmucal_vclk_ip_fsys2_cmu_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_FSYS0FSYS2, NULL, cmucal_vclk_ip_lhm_axi_p_fsys0fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D_FSYS2FSYS0, NULL, cmucal_vclk_ip_btm_d_fsys2fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D_FSYS2, NULL, cmucal_vclk_ip_btm_d_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_FSYS2, NULL, cmucal_vclk_ip_d_tzpc_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ETHERNET0, NULL, cmucal_vclk_ip_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ETHERNET1, NULL, cmucal_vclk_ip_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPIO_FSYS2, NULL, cmucal_vclk_ip_gpio_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_FSYS2FSYS0, NULL, cmucal_vclk_ip_lhs_axi_d_fsys2fsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D_FSYS2, NULL, cmucal_vclk_ip_lhs_acel_d_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_ETHERNET0, NULL, cmucal_vclk_ip_qe_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_ETHERNET1, NULL, cmucal_vclk_ip_qe_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_UFS_EMBD0, NULL, cmucal_vclk_ip_qe_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_UFS_EMBD1, NULL, cmucal_vclk_ip_qe_ufs_embd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_ETHERNET, NULL, cmucal_vclk_ip_sysmmu_ethernet, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_S2MPU_D_FSYS2, NULL, cmucal_vclk_ip_s2mpu_d_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_FSYS2, NULL, cmucal_vclk_ip_sysreg_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_UFS_EMBD0, NULL, cmucal_vclk_ip_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_UFS_EMBD1, NULL, cmucal_vclk_ip_ufs_embd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_ETHERNET0, NULL, cmucal_vclk_ip_vgen_ethernet0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_FSYS2, NULL, cmucal_vclk_ip_xiu_d_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_FSYS2_ETH, NULL, cmucal_vclk_ip_xiu_d_fsys2_eth, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_FSYS2, NULL, cmucal_vclk_ip_lhm_axi_p_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_ETHERNET1, NULL, cmucal_vclk_ip_vgen_ethernet1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_ETHERNET, NULL, cmucal_vclk_ip_ppmu_ethernet, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_P_FSYS2, NULL, cmucal_vclk_ip_xiu_p_fsys2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_UFS_EMBD0, NULL, cmucal_vclk_ip_ppmu_ufs_embd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_UFS_EMBD1, NULL, cmucal_vclk_ip_ppmu_ufs_embd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G2D_CMU_G2D, NULL, cmucal_vclk_ip_g2d_cmu_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G2DD0, NULL, cmucal_vclk_ip_ppmu_g2dd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G2DD1, NULL, cmucal_vclk_ip_ppmu_g2dd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G2DD0, NULL, cmucal_vclk_ip_sysmmu_g2dd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_G2D, NULL, cmucal_vclk_ip_sysreg_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D0_G2D, NULL, cmucal_vclk_ip_lhs_acel_d0_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D1_G2D, NULL, cmucal_vclk_ip_lhs_acel_d1_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_G2D, NULL, cmucal_vclk_ip_lhm_axi_p_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_P_G2D, NULL, cmucal_vclk_ip_as_p_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_G2DD0, NULL, cmucal_vclk_ip_btm_g2dd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_G2DD1, NULL, cmucal_vclk_ip_btm_g2dd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_G2DD2, NULL, cmucal_vclk_ip_btm_g2dd2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_JPEG, NULL, cmucal_vclk_ip_qe_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QE_MSCL, NULL, cmucal_vclk_ip_qe_mscl, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G2DD2, NULL, cmucal_vclk_ip_sysmmu_g2dd2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G2DD2, NULL, cmucal_vclk_ip_ppmu_g2dd2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACEL_D2_G2D, NULL, cmucal_vclk_ip_lhs_acel_d2_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_G2D, NULL, cmucal_vclk_ip_xiu_d_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_G2D, NULL, cmucal_vclk_ip_vgen_lite_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G2D, NULL, cmucal_vclk_ip_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_G2DD1, NULL, cmucal_vclk_ip_sysmmu_g2dd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_JPEG, NULL, cmucal_vclk_ip_jpeg, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MSCL, NULL, cmucal_vclk_ip_mscl, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_G2D, NULL, cmucal_vclk_ip_d_tzpc_g2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_P_MSCL, NULL, cmucal_vclk_ip_as_p_mscl, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D00_CMU_G3D00, NULL, cmucal_vclk_ip_g3d00_cmu_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_G3D00, NULL, cmucal_vclk_ip_vgen_lite_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_G3D00, NULL, cmucal_vclk_ip_sysreg_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_INT_G3D00, NULL, cmucal_vclk_ip_lhs_axi_p_int_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACE_D_G3D00, NULL, cmucal_vclk_ip_lhs_ace_d_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_G3D00, NULL, cmucal_vclk_ip_lhm_axi_p_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_INT_G3D00, NULL, cmucal_vclk_ip_lhm_axi_p_int_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GRAY2BIN_G3D00, NULL, cmucal_vclk_ip_gray2bin_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPU00, NULL, cmucal_vclk_ip_gpu00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_G3D00, NULL, cmucal_vclk_ip_d_tzpc_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G3D00, NULL, cmucal_vclk_ip_ppmu_g3d00, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D01_CMU_G3D01, NULL, cmucal_vclk_ip_g3d01_cmu_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_G3D01, NULL, cmucal_vclk_ip_vgen_lite_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_G3D01, NULL, cmucal_vclk_ip_sysreg_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_INT_G3D01, NULL, cmucal_vclk_ip_lhs_axi_p_int_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ACE_D_G3D01, NULL, cmucal_vclk_ip_lhs_ace_d_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_G3D01, NULL, cmucal_vclk_ip_lhm_axi_p_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_INT_G3D01, NULL, cmucal_vclk_ip_lhm_axi_p_int_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GRAY2BIN_G3D01, NULL, cmucal_vclk_ip_gray2bin_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPU01, NULL, cmucal_vclk_ip_gpu01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_G3D01, NULL, cmucal_vclk_ip_d_tzpc_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_G3D01, NULL, cmucal_vclk_ip_ppmu_g3d01, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_G3D1, NULL, cmucal_vclk_ip_lhm_axi_p_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_G3D1, NULL, cmucal_vclk_ip_sysreg_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_G3D1_CMU_G3D1, NULL, cmucal_vclk_ip_g3d1_cmu_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_INT_G3D1, NULL, cmucal_vclk_ip_lhs_axi_p_int_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_G3D1, NULL, cmucal_vclk_ip_vgen_lite_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPU1, NULL, cmucal_vclk_ip_gpu1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_INT_G3D1, NULL, cmucal_vclk_ip_lhm_axi_p_int_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GRAY2BIN_G3D1, NULL, cmucal_vclk_ip_gray2bin_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_G3D1, NULL, cmucal_vclk_ip_d_tzpc_g3d1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_ISPB, NULL, cmucal_vclk_ip_lhm_axi_p_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_ISPB, NULL, cmucal_vclk_ip_lhs_axi_d_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ISPB, NULL, cmucal_vclk_ip_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_ISPB, NULL, cmucal_vclk_ip_sysreg_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ISPB_CMU_ISPB, NULL, cmucal_vclk_ip_ispb_cmu_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_C2C_TAAISPB, NULL, cmucal_vclk_ip_lhm_ast_c2c_taaispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_C2C_ISPBACC, NULL, cmucal_vclk_ip_lhs_ast_c2c_ispbacc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_ISPB, NULL, cmucal_vclk_ip_btm_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_OTF_TAAISPB, NULL, cmucal_vclk_ip_lhm_ast_otf_taaispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_ISPB, NULL, cmucal_vclk_ip_d_tzpc_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_OTF_ISPBACC, NULL, cmucal_vclk_ip_lhs_ast_otf_ispbacc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_ISPB, NULL, cmucal_vclk_ip_sysmmu_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_ISPB, NULL, cmucal_vclk_ip_vgen_lite_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_ISPB, NULL, cmucal_vclk_ip_ppmu_ispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_ISPB_ISPB0, NULL, cmucal_vclk_ip_as_apb_ispb_ispb0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_ISPB_SYSMMU_S1_NS, NULL, cmucal_vclk_ip_as_apb_ispb_sysmmu_s1_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_ISPB_SYSMMU_S1_S, NULL, cmucal_vclk_ip_as_apb_ispb_sysmmu_s1_s, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_ISPB_ISPB1, NULL, cmucal_vclk_ip_as_apb_ispb_ispb1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_ISPB_ISPB2, NULL, cmucal_vclk_ip_as_apb_ispb_ispb2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_ISPB_SYSMMU_S2, NULL, cmucal_vclk_ip_as_apb_ispb_sysmmu_s2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_ISPB_ISPB3, NULL, cmucal_vclk_ip_as_apb_ispb_ispb3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_SYSMMU_D0_MFC_S2, NULL, cmucal_vclk_ip_as_apb_sysmmu_d0_mfc_s2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_SYSMMU_D1_MFC_S2, NULL, cmucal_vclk_ip_as_apb_sysmmu_d1_mfc_s2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MFC_CMU_MFC, NULL, cmucal_vclk_ip_mfc_cmu_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_MFC, NULL, cmucal_vclk_ip_as_apb_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_MFC, NULL, cmucal_vclk_ip_sysreg_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D0_MFC, NULL, cmucal_vclk_ip_lhs_axi_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D1_MFC, NULL, cmucal_vclk_ip_lhs_axi_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_MFC, NULL, cmucal_vclk_ip_lhm_axi_p_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D0_MFC, NULL, cmucal_vclk_ip_sysmmu_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_D1_MFC, NULL, cmucal_vclk_ip_sysmmu_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D0_MFC, NULL, cmucal_vclk_ip_ppmu_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D1_MFC, NULL, cmucal_vclk_ip_ppmu_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D0_MFC, NULL, cmucal_vclk_ip_btm_d0_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_D1_MFC, NULL, cmucal_vclk_ip_btm_d1_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_SYSMMU_D0_MFC_N, NULL, cmucal_vclk_ip_as_apb_sysmmu_d0_mfc_n, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_SYSMMU_D1_MFC_N, NULL, cmucal_vclk_ip_as_apb_sysmmu_d1_mfc_n, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_SYSMMU_D0_MFC_S, NULL, cmucal_vclk_ip_as_apb_sysmmu_d0_mfc_s, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_SYSMMU_D1_MFC_S, NULL, cmucal_vclk_ip_as_apb_sysmmu_d1_mfc_s, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_AXI_WFD, NULL, cmucal_vclk_ip_as_axi_wfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_D2_MFC, NULL, cmucal_vclk_ip_ppmu_d2_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_D_MFC, NULL, cmucal_vclk_ip_xiu_d_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_WFD_NS, NULL, cmucal_vclk_ip_as_apb_wfd_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AS_APB_WFD_S, NULL, cmucal_vclk_ip_as_apb_wfd_s, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_MFC, NULL, cmucal_vclk_ip_vgen_lite_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MFC, NULL, cmucal_vclk_ip_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WFD, NULL, cmucal_vclk_ip_wfd, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LH_ATB_MFC, NULL, cmucal_vclk_ip_lh_atb_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_MFC, NULL, cmucal_vclk_ip_d_tzpc_mfc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MIF_CMU_MIF, NULL, cmucal_vclk_ip_mif_cmu_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DDRPHY0, NULL, cmucal_vclk_ip_ddrphy0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_MIF, NULL, cmucal_vclk_ip_sysreg_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_MIF, NULL, cmucal_vclk_ip_lhm_axi_p_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AXI2APB_MIF, NULL, cmucal_vclk_ip_axi2apb_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_DVFS0, NULL, cmucal_vclk_ip_ppc_dvfs0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_DEBUG0, NULL, cmucal_vclk_ip_ppc_debug0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBBR_DDRPHY0, NULL, cmucal_vclk_ip_apbbr_ddrphy0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBBR_DMC0, NULL, cmucal_vclk_ip_apbbr_dmc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBBR_DMCTZ0, NULL, cmucal_vclk_ip_apbbr_dmctz0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DMC0, NULL, cmucal_vclk_ip_dmc0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QCH_ADAPTER_PPC_DEBUG0, NULL, cmucal_vclk_ip_qch_adapter_ppc_debug0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QCH_ADAPTER_PPC_DVFS0, NULL, cmucal_vclk_ip_qch_adapter_ppc_dvfs0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_MIF, NULL, cmucal_vclk_ip_d_tzpc_mif, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DMC1, NULL, cmucal_vclk_ip_dmc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBBR_DDRPHY1, NULL, cmucal_vclk_ip_apbbr_ddrphy1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBBR_DMC1, NULL, cmucal_vclk_ip_apbbr_dmc1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_APBBR_DMCTZ1, NULL, cmucal_vclk_ip_apbbr_dmctz1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DDRPHY1, NULL, cmucal_vclk_ip_ddrphy1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_DEBUG1, NULL, cmucal_vclk_ip_ppc_debug1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPC_DVFS1, NULL, cmucal_vclk_ip_ppc_dvfs1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QCH_ADAPTER_PPC_DEBUG1, NULL, cmucal_vclk_ip_qch_adapter_ppc_debug1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_QCH_ADAPTER_PPC_DVFS1, NULL, cmucal_vclk_ip_qch_adapter_ppc_dvfs1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SFMPU, NULL, cmucal_vclk_ip_sfmpu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_NPU_CMU_NPU, NULL, cmucal_vclk_ip_npu_cmu_npu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D1_NPUH, NULL, cmucal_vclk_ip_lhm_ast_d1_npuh, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_NPU, NULL, cmucal_vclk_ip_lhm_axi_p_npu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AXI2APB_NPU, NULL, cmucal_vclk_ip_axi2apb_npu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_NPU, NULL, cmucal_vclk_ip_d_tzpc_npu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D2_NPUH, NULL, cmucal_vclk_ip_lhm_ast_d2_npuh, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D3_NPUH, NULL, cmucal_vclk_ip_lhm_ast_d3_npuh, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D0_NPUV, NULL, cmucal_vclk_ip_lhm_ast_d0_npuv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D1_NPUV, NULL, cmucal_vclk_ip_lhm_ast_d1_npuv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D2_NPUV, NULL, cmucal_vclk_ip_lhm_ast_d2_npuv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D3_NPUV, NULL, cmucal_vclk_ip_lhm_ast_d3_npuv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D0_NPUX, NULL, cmucal_vclk_ip_lhm_ast_d0_npux, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D0_NPUH, NULL, cmucal_vclk_ip_lhm_ast_d0_npuh, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D1_NPUX, NULL, cmucal_vclk_ip_lhm_ast_d1_npux, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D2_NPUX, NULL, cmucal_vclk_ip_lhm_ast_d2_npux, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D0_NPUV, NULL, cmucal_vclk_ip_lhs_ast_d0_npuv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D0_NPUX, NULL, cmucal_vclk_ip_lhs_ast_d0_npux, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D1_NPUH, NULL, cmucal_vclk_ip_lhs_ast_d1_npuh, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D1_NPUV, NULL, cmucal_vclk_ip_lhs_ast_d1_npuv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D1_NPUX, NULL, cmucal_vclk_ip_lhs_ast_d1_npux, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D2_NPUH, NULL, cmucal_vclk_ip_lhs_ast_d2_npuh, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D2_NPUV, NULL, cmucal_vclk_ip_lhs_ast_d2_npuv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_NPU, NULL, cmucal_vclk_ip_sysreg_npu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D0_NPUH, NULL, cmucal_vclk_ip_lhs_ast_d0_npuh, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_NPUD_UNIT, NULL, cmucal_vclk_ip_npud_unit, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_NPU, NULL, cmucal_vclk_ip_ppmu_npu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_NPU_PPC_WRAPPER, NULL, cmucal_vclk_ip_npu_ppc_wrapper, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D3_NPUX, NULL, cmucal_vclk_ip_lhm_ast_d3_npux, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D2_NPUX, NULL, cmucal_vclk_ip_lhs_ast_d2_npux, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D3_NPUH, NULL, cmucal_vclk_ip_lhs_ast_d3_npuh, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D3_NPUV, NULL, cmucal_vclk_ip_lhs_ast_d3_npuv, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D3_NPUX, NULL, cmucal_vclk_ip_lhs_ast_d3_npux, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_D_NPU_UNIT_SETREG, NULL, cmucal_vclk_ip_lhm_ast_d_npu_unit_setreg, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_NPU, NULL, cmucal_vclk_ip_lhm_axi_d_npu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_D_NPU_UNIT_DONE, NULL, cmucal_vclk_ip_lhs_ast_d_npu_unit_done, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPIO_PERIC0, NULL, cmucal_vclk_ip_gpio_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_PERIC0, NULL, cmucal_vclk_ip_sysreg_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PERIC0_CMU_PERIC0, NULL, cmucal_vclk_ip_peric0_cmu_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_PERIC0, NULL, cmucal_vclk_ip_lhm_axi_p_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_PERIC0, NULL, cmucal_vclk_ip_d_tzpc_peric0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PERIC0_TOP0, NULL, cmucal_vclk_ip_peric0_top0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPIO_PERIC1, NULL, cmucal_vclk_ip_gpio_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_PERIC1, NULL, cmucal_vclk_ip_sysreg_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PERIC1_CMU_PERIC1, NULL, cmucal_vclk_ip_peric1_cmu_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_PERIC1, NULL, cmucal_vclk_ip_lhm_axi_p_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_PERIC1, NULL, cmucal_vclk_ip_d_tzpc_peric1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PERIC1_TOP0, NULL, cmucal_vclk_ip_peric1_top0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_PERIS, NULL, cmucal_vclk_ip_sysreg_peris, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT_CLUSTER1, NULL, cmucal_vclk_ip_wdt_cluster1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT_CLUSTER0, NULL, cmucal_vclk_ip_wdt_cluster0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PERIS_CMU_PERIS, NULL, cmucal_vclk_ip_peris_cmu_peris, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_AXI_P_PERIS, NULL, cmucal_vclk_ip_ad_axi_p_peris, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_OTP_CON_BIRA, NULL, cmucal_vclk_ip_otp_con_bira, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GIC, NULL, cmucal_vclk_ip_gic, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_PERIS, NULL, cmucal_vclk_ip_lhm_axi_p_peris, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MCT, NULL, cmucal_vclk_ip_mct, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_OTP_CON_TOP, NULL, cmucal_vclk_ip_otp_con_top, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_PERIS, NULL, cmucal_vclk_ip_d_tzpc_peris, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_OTP_CON_BISR, NULL, cmucal_vclk_ip_otp_con_bisr, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BUSIF_TMU, NULL, cmucal_vclk_ip_busif_tmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_PERIS_1, NULL, cmucal_vclk_ip_sysreg_peris_1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_ABOX_CR52_C0, NULL, cmucal_vclk_ip_mailbox_abox_cr52_c0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_ABOX_CR52_C1, NULL, cmucal_vclk_ip_mailbox_abox_cr52_c1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_AP_CR52_C0, NULL, cmucal_vclk_ip_mailbox_ap_cr52_c0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_AP_CR52_C1, NULL, cmucal_vclk_ip_mailbox_ap_cr52_c1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_PERIS_2, NULL, cmucal_vclk_ip_sysreg_peris_2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_S2D_CMU_S2D, NULL, cmucal_vclk_ip_s2d_cmu_s2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BIS_S2D, NULL, cmucal_vclk_ip_bis_s2d, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_G_SCAN2DRAM, NULL, cmucal_vclk_ip_lhm_axi_g_scan2dram, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_USI13_USI, NULL, cmucal_vclk_ip_usi13_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADM_APB_G_SFI, NULL, cmucal_vclk_ip_adm_apb_g_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CLKMON0, NULL, cmucal_vclk_ip_clkmon0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_USI12_USI, NULL, cmucal_vclk_ip_usi12_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_USI14_USI, NULL, cmucal_vclk_ip_usi14_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SSS_SFI, NULL, cmucal_vclk_ip_sss_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_DAP_SFI, NULL, cmucal_vclk_ip_dap_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADD_GPIO_LO, NULL, cmucal_vclk_ip_add_gpio_lo, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADD_GPIO_UP, NULL, cmucal_vclk_ip_add_gpio_up, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADD_SFI_LO, NULL, cmucal_vclk_ip_add_sfi_lo, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ADD_SFI_UP, NULL, cmucal_vclk_ip_add_sfi_up, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_FMU, NULL, cmucal_vclk_ip_fmu, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ASYNCAPB_FMU0, NULL, cmucal_vclk_ip_asyncapb_fmu0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BAAW_D_SFI, NULL, cmucal_vclk_ip_baaw_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BAAW_P_SFIAPM_SFI, NULL, cmucal_vclk_ip_baaw_p_sfiapm_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BAAW_P_SFIFSYS0_SFI, NULL, cmucal_vclk_ip_baaw_p_sfifsys0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_SFI, NULL, cmucal_vclk_ip_d_tzpc_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_GPIO_SFI, NULL, cmucal_vclk_ip_gpio_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MAILBOX_CR52_SFI, NULL, cmucal_vclk_ip_mailbox_cr52_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_MCT_SFI, NULL, cmucal_vclk_ip_mct_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_SFIFSYS0, NULL, cmucal_vclk_ip_ppmu_sfifsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ROM_CRC32_SFI, NULL, cmucal_vclk_ip_rom_crc32_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_SFI, NULL, cmucal_vclk_ip_sysreg_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VOLMON, NULL, cmucal_vclk_ip_volmon, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT0_SFI, NULL, cmucal_vclk_ip_wdt0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_USI15_USI, NULL, cmucal_vclk_ip_usi15_usi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CAN_FD0, NULL, cmucal_vclk_ip_can_fd0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CAN_FD1, NULL, cmucal_vclk_ip_can_fd1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_SFPC_SFI, NULL, cmucal_vclk_ip_d_sfpc_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_INTMEM_SFI, NULL, cmucal_vclk_ip_intmem_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ASYNCAPB_INTMEM, NULL, cmucal_vclk_ip_asyncapb_intmem, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_D_FSYS0SFI, NULL, cmucal_vclk_ip_lhm_axi_d_fsys0sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_SFI, NULL, cmucal_vclk_ip_lhm_axi_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_SFI, NULL, cmucal_vclk_ip_lhs_axi_d_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_SFIFSYS0, NULL, cmucal_vclk_ip_lhs_axi_p_sfifsys0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_P_SFIAPM, NULL, cmucal_vclk_ip_lhs_axi_p_sfiapm, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_DP_SFI, NULL, cmucal_vclk_ip_xiu_dp_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SERIALFLASH, NULL, cmucal_vclk_ip_serialflash, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_HYPERBUS, NULL, cmucal_vclk_ip_hyperbus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BAAW_D_FSYS0SFI_SFI, NULL, cmucal_vclk_ip_baaw_d_fsys0sfi_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_DT0_SFI, NULL, cmucal_vclk_ip_lhs_atb_dt0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_DT1_SFI, NULL, cmucal_vclk_ip_lhs_atb_dt1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_IT0_SFI, NULL, cmucal_vclk_ip_lhs_atb_it0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_ATB_IT1_SFI, NULL, cmucal_vclk_ip_lhs_atb_it1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ASYNCAXI_M0, NULL, cmucal_vclk_ip_asyncaxi_m0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ASYNCAXI_M1, NULL, cmucal_vclk_ip_asyncaxi_m1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ASYNCAXI_S0, NULL, cmucal_vclk_ip_asyncaxi_s0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SF_AXI2APB_BRIDGE_SFI, NULL, cmucal_vclk_ip_sf_axi2apb_bridge_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_WDT1_SFI, NULL, cmucal_vclk_ip_wdt1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SFMPU_FLSH0_SFI, NULL, cmucal_vclk_ip_sfmpu_flsh0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SFMPU_FLSH1_SFI, NULL, cmucal_vclk_ip_sfmpu_flsh1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SFMPU_IMEM_SFI, NULL, cmucal_vclk_ip_sfmpu_imem_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BUSIF_CMU_SFI, NULL, cmucal_vclk_ip_busif_cmu_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PMU_HANDSHAKE_BUS, NULL, cmucal_vclk_ip_pmu_handshake_bus, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BAAW_FLSH_SFI, NULL, cmucal_vclk_ip_baaw_flsh_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CLUSTER_SFI, NULL, cmucal_vclk_ip_cluster_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_XIU_P3_SFI, NULL, cmucal_vclk_ip_xiu_p3_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SBIST0, NULL, cmucal_vclk_ip_sbist0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SBIST1, NULL, cmucal_vclk_ip_sbist1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BAAW_P_SFI, NULL, cmucal_vclk_ip_baaw_p_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ASYNCAPB_SFMPU_FLSH0_SFI, NULL, cmucal_vclk_ip_asyncapb_sfmpu_flsh0_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ASYNCAPB_SFMPU_FLSH1_SFI, NULL, cmucal_vclk_ip_asyncapb_sfmpu_flsh1_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_ASYNCAPB_SFMPU_INTMEM_SFI, NULL, cmucal_vclk_ip_asyncapb_sfmpu_intmem_sfi, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PMC, NULL, cmucal_vclk_ip_pmc, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CLKMON1, NULL, cmucal_vclk_ip_clkmon1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CLKMON2, NULL, cmucal_vclk_ip_clkmon2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_CLKMON3, NULL, cmucal_vclk_ip_clkmon3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AXI_D_TAA, NULL, cmucal_vclk_ip_lhs_axi_d_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_BTM_TAA, NULL, cmucal_vclk_ip_btm_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AXI_P_TAA, NULL, cmucal_vclk_ip_lhm_axi_p_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSREG_TAA, NULL, cmucal_vclk_ip_sysreg_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TAA_CMU_TAA, NULL, cmucal_vclk_ip_taa_cmu_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_C2C_TAAISPB, NULL, cmucal_vclk_ip_lhs_ast_c2c_taaispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHS_AST_OTF_TAAISPB, NULL, cmucal_vclk_ip_lhs_ast_otf_taaispb, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_TAA, NULL, cmucal_vclk_ip_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_D_TZPC_TAA, NULL, cmucal_vclk_ip_d_tzpc_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_OTF_ACCTAA, NULL, cmucal_vclk_ip_lhm_ast_otf_acctaa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_LHM_AST_C2C_ACCTAA, NULL, cmucal_vclk_ip_lhm_ast_c2c_acctaa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_TAA_TAA0, NULL, cmucal_vclk_ip_ad_apb_taa_taa0, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_PPMU_TAA, NULL, cmucal_vclk_ip_ppmu_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_SYSMMU_TAA, NULL, cmucal_vclk_ip_sysmmu_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_VGEN_LITE_TAA, NULL, cmucal_vclk_ip_vgen_lite_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_TAA_SYSMMU_S1_S, NULL, cmucal_vclk_ip_ad_apb_taa_sysmmu_s1_s, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_TAA_SYSMMU_S1_NS, NULL, cmucal_vclk_ip_ad_apb_taa_sysmmu_s1_ns, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AXI2APB_TAA, NULL, cmucal_vclk_ip_axi2apb_taa, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_TAA_TAA1, NULL, cmucal_vclk_ip_ad_apb_taa_taa1, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_TAA_TAA2, NULL, cmucal_vclk_ip_ad_apb_taa_taa2, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_TAA_TAA3, NULL, cmucal_vclk_ip_ad_apb_taa_taa3, NULL, NULL),
	CMUCAL_VCLK(VCLK_IP_AD_APB_TAA_SYSMMU_S2, NULL, cmucal_vclk_ip_ad_apb_taa_sysmmu_s2, NULL, NULL),
};
