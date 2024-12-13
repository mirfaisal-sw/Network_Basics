// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - IPs Traffic Monitor(ITMON) support
 */

#include "exynos-itmon-platform.h"
#include "exynos-itmon-local-v2.h"

#define NA		(NOT_SUPPORT)

static struct itmon_v2_rpathinfo exynoautov920_rpathinfo[] = {
	/*0x8000_0000 ~ 0xffff_ffff*/
	{0,	"DNC_D0",	"IRPS",		0x3f},
	{1,	"DNC_D1",	"IRPS",		0x3f},
	{2,	"DNC_D2",	"IRPS",		0x3f},
	{3,	"DNC_D3",	"IRPS",		0x3f},
	{4,	"DNC_D4",	"IRPS",		0x3f},
	{5,	"MFC_D0",	"IRPS",		0x3f},
	{6,	"MFC_D1",	"IRPS",		0x3f},
	{7,	"HSI0_D1",	"IRPS",		0x3f},
	{8,	"ALIVE",	"IRPS",		0x3f},
	{9,	"AUD",		"IRPS",		0x3f},
	{10,	"SSP",		"IRPS",		0x3f},
	{11,	"SFI",		"IRPS",		0x3f},
	{12,	"MFD_D0",	"IRPS",		0x3f},
	{13,	"MFD_D1",	"IRPS",		0x3f},
	{14,	"CSSYS",	"IRPS",		0x3f},
	{15,	"HSI2_D0",	"IRPS",		0x3f},
	{16,	"HSI2_D1",	"IRPS",		0x3f},
	{17,	"G3DMMU",	"IRPS",		0x3f},
	{18,	"MISC",		"IRPS",		0x3f},
	{19,	"HSI0_D0",	"IRPS",		0x3f},
	{20,	"DPUF0_D0",	"IRPS",		0x3f},
	{21,	"DPUF0_D1",	"IRPS",		0x3f},
	{22,	"DPUF1_D0",	"IRPS",		0x3f},
	{23,	"DPUF1_D1",	"IRPS",		0x3f},
	{24,	"DPUF2_D0",	"IRPS",		0x3f},
	{25,	"DPUF2_D1",	"IRPS",		0x3f},
	{26,	"SNW_D0",	"IRPS",		0x3f},
	{27,	"SNW_D1",	"IRPS",		0x3f},
	{28,	"SNW_D2",	"IRPS",		0x3f},
	{29,	"TAA",		"IRPS",		0x3f},
	{30,	"ACC",		"IRPS",		0x3f},
	{31,	"HSI1",		"IRPS",		0x3f},
	{32,	"ISP",		"IRPS",		0x3f},
	{33,	"M2M_D0",	"IRPS",		0x3f},
	{34,	"M2M_D1",	"IRPS",		0x3f},

	/* 0x0000_0000 - 0x7fff_ffff */
	{0,	"DNC_D0",	"DP_NOCL0",		0x3f},
	{1,	"DNC_D1",	"DP_NOCL0",		0x3f},
	{2,	"DNC_D2",	"DP_NOCL0",		0x3f},
	{3,	"DNC_D3",	"DP_NOCL0",		0x3f},
	{4,	"DNC_D4",	"DP_NOCL0",		0x3f},
	{5,	"MFC_D0",	"DP_NOCL0",		0x3f},
	{6,	"MFC_D1",	"DP_NOCL0",		0x3f},
	{7,	"HSI0_D1",	"DP_NOCL0",		0x3f},
	{8,	"ALIVE",	"DP_NOCL0",		0x3f},
	{9,	"AUD",		"DP_NOCL0",		0x3f},
	{10,	"SSP",		"DP_NOCL0",		0x3f},
	{11,	"SFI",		"DP_NOCL0",		0x3f},
	{12,	"MFD_D0",	"DP_NOCL0",		0x3f},
	{13,	"MFD_D1",	"DP_NOCL0",		0x3f},
	{14,	"CSSYS",	"DP_NOCL0",		0x3f},
	{15,	"HSI2_D0",	"DP_NOCL0",		0x3f},
	{16,	"HSI2_D1",	"DP_NOCL0",		0x3f},
	{17,	"G3DMMU",	"DP_NOCL0",		0x3f},
	{18,	"MISC",		"DP_NOCL0",		0x3f},
	{19,	"HSI0_D0",	"DP_NOCL0",		0x3f},
	{20,	"DPUF0_D0",	"DP_NOCL0",		0x3f},
	{21,	"DPUF0_D1",	"DP_NOCL0",		0x3f},
	{22,	"DPUF1_D0",	"DP_NOCL0",		0x3f},
	{23,	"DPUF1_D1",	"DP_NOCL0",		0x3f},
	{24,	"DPUF2_D0",	"DP_NOCL0",		0x3f},
	{25,	"DPUF2_D1",	"DP_NOCL0",		0x3f},
	{26,	"SNW_D0",	"DP_NOCL0",		0x3f},
	{27,	"SNW_D1",	"DP_NOCL0",		0x3f},
	{28,	"SNW_D2",	"DP_NOCL0",		0x3f},
	{29,	"TAA",		"DP_NOCL0",		0x3f},
	{30,	"ACC",		"DP_NOCL0",		0x3f},
	{31,	"HSI1",		"DP_NOCL0",		0x3f},
	{32,	"ISP",		"DP_NOCL0",		0x3f},
	{33,	"M2M_D0",	"DP_NOCL0",		0x3f},
	{34,	"M2M_D1",	"DP_NOCL0",		0x3f},

	/* Configuration Interconnect */
	{0,	"SCI_CCM_0",	"CONFIGURATION",	0x7},
	{1,	"SCI_CCM_1",	"CONFIGURATION",	0x7},
	{2,	"SCI_IRPM",	"CONFIGURATION",	0x7},
	{3,	"NOCL0_DP",	"CONFIGURATION",	0x7},
	{4,	"CPU_M0",	"CONFIGURATION",	0x7},
	{5,	"CPU_M1",	"CONFIGURATION",	0x7},
	{6,	"CPU_M2",	"CONFIGURATION",	0x7},
};

static struct itmon_v2_masterinfo exynoautov920_masterinfo[] = {
	{"DNC_D0",	0b000000, "SDMA.BUSS0",		0x0},
	{"DNC_D1",	0b000000, "SDMA.BUSS1",		0},
	{"DNC_D2",	0b000000, "SDMA.BUSS2",		0},
	{"DNC_D3",	0b000000, "SDMA.BUSS3",		0},

	{"DNC_D4",	0b000000, "CA32",		GENMASK(4, 0)},
	{"DNC_D4",	0b000100, "DSP I-cache",	GENMASK(5, 0)},
	{"DNC_D4",	0b100100, "DSP CMDQ",		GENMASK(5, 0)},
	{"DNC_D4",	0b001000, "GNPU0 CMDQ",		GENMASK(4, 0)},
	{"DNC_D4",	0b001100, "GNPU1 CMDQ",		GENMASK(4, 0)},
	{"DNC_D4",	0b010000, "C2AGENT",		GENMASK(4, 0)},
	{"DNC_D4",	0b000001, "SYSMMU_S0_DNC",	GENMASK(1, 0)},
	{"DNC_D4",	0b000010, "SYSMMU_S1_DNC",	GENMASK(1, 0)},

	{"ACC",		0b000100, "ISPPRE",		GENMASK(2, 0)},
	{"ACC",		0b000000, "BICM_AXI_D0_ACC",	GENMASK(2, 0)},
	{"ACC",		0b000010, "ORBMCH",		GENMASK(1, 0)},
	{"ACC",		0b000001, "SYSMMU_S0_ACC",	BIT(0)},

	{"DPUF0_D0",	0b000000, "DPUF0.M0",		BIT(0)},
	{"DPUF0_D0",	0b000001, "DPUF0.M1",		BIT(0)},

	{"DPUF0_D1",	0b000000, "DPUF1.M0",		GENMASK(1, 0)},
	{"DPUF0_D1",	0b000001, "DPUF1.M1",		GENMASK(1, 0)},
	{"DPUF0_D1",	0b000010, "SYSMMU_S1_DPUF",	GENMASK(1, 0)},

	{"DPUF1_D0",	0b000000, "DPUF0.M0",		BIT(0)},
	{"DPUF1_D0",	0b000001, "DPUF0.M1",		BIT(0)},

	{"DPUF1_D1",	0b000000, "DPUF1.M0",		GENMASK(1, 0)},
	{"DPUF1_D1",	0b000001, "DPUF1.M1",		GENMASK(1, 0)},
	{"DPUF1_D1",	0b000010, "SYSMMU_S1_DPUF",	GENMASK(1, 0)},

	{"DPUF2_D0",	0b000000, "DPUF0.M0",		BIT(0)},
	{"DPUF2_D0",	0b000001, "DPUF0.M1",		BIT(0)},

	{"DPUF2_D1",	0b000000, "DPUF1.M0",		GENMASK(1, 0)},
	{"DPUF2_D1",	0b000001, "DPUF1.M1",		GENMASK(1, 0)},
	{"DPUF2_D1",	0b000010, "SYSMMU_S1_DPUF",	GENMASK(1, 0)},

	{"HSI1",	0b000000, "USBDRD30_0",		GENMASK(3, 0)},
	{"HSI1",	0b000010, "USBDRD20_0",		GENMASK(3, 0)},
	{"HSI1",	0b000100, "USBDRD20_1",		GENMASK(3, 0)},
	{"HSI1",	0b000110, "USBDRD20_2",		GENMASK(3, 0)},
	{"HSI1",	0b001000, "MMC_CARD",		GENMASK(3, 0)},
	{"HSI1",	0b000001, "SYSMMU_S0_HSI1",	BIT(0)},

	{"ISP",		0b000010, "ISP",		GENMASK(1, 0)},
	{"ISP",		0b000000, "BICM_AXI_D0_ISP",	GENMASK(1, 0)},
	{"ISP",		0b000001, "SYSMMU_S0_ISP",	BIT(0)},

	{"M2M_D0",	0b000000, "JPEG",		BIT(0)},
	{"M2M_D0",	0b000001, "M2M0",		BIT(0)},
	{"M2M_D1",	0b000000, "M2M1",		BIT(0)},
	{"M2M_D1",	0b000001, "SYSMMU_S0_M2M",	BIT(0)},

	{"SNW_D0",	0b000011, "WRP_M20",		GENMASK(1, 0)},
	{"SNW_D0",	0b000001, "BICM_AXI_D2_SNW",	GENMASK(1, 0)},
	{"SNW_D0",	0b000010, "SNF1_M1",		GENMASK(1, 0)},
	{"SNW_D0",	0b000000, "BICM_AXI_D1_SNW",	GENMASK(1, 0)},

	{"SNW_D1",	0b000010, "SNW0_M0",		GENMASK(1, 0)},
	{"SNW_D1",	0b000000, "BICM_AXI_D0_SNW",	GENMASK(1, 0)},
	{"SNW_D1",	0b000001, "SYSMMU_S0_SNW",	BIT(0)},

	{"SNW_D2",	0b000001, "WRP_M21",		BIT(0)},
	{"SNW_D2",	0b000000, "BICM_AXI_D3_SNW",	BIT(0)},

	{"TAA",		0b000000, "TAA.M0",		BIT(0)},
	{"TAA",		0b000001, "SYSMMU_S0_TAA",	BIT(0)},

	{"ALIVE",	0b000000, "GREBE_APM",		GENMASK(2, 0)},
	{"ALIVE",	0b000010, "GREBE_DBGCORE",	GENMASK(2, 0)},
	{"ALIVE",	0b000100, "APM_DTA",		GENMASK(2, 0)},
	{"ALIVE",	0b000110, "PMU",		GENMASK(2, 0)},
	{"ALIVE",	0b000001, "SYSMMU_S0_ALIVE",	BIT(0)},

	{"AUD",		0b001100, "SPUS1(SF)",		GENMASK(3, 0)},
	{"AUD",		0b001010, "HIFI5_C",		GENMASK(3, 0)},
	{"AUD",		0b001000, "HIFI5_B",		GENMASK(3, 0)},
	{"AUD",		0b000110, "HIFI5_A",		GENMASK(3, 0)},
	{"AUD",		0b000010, "SPUS0(SF)",		GENMASK(3, 0)},
	{"AUD",		0b000000, "SPUS/M",		GENMASK(3, 0)},
	{"AUD",		0b000001, "SYSMMU_S0_AUD",	BIT(0)},

	{"CSSYS",	0b000000, "CSSYS_ETR",		BIT(0)},
	{"CSSYS",	0b000001, "CSSYS_STM",		BIT(0)},

	{"G3DMMU",	0b000000, "SYSMMU_S0_G3D",	0},

	{"HSI0_D0",	0b000000, "PCIE_GEN5A_4L",	BIT(0)},
	{"HSI0_D0",	0b000001, "PCIE_GEN5B_4L",	BIT(0)},

	{"HSI0_D1",	0b000000, "PCIE_GEN5A_2L ",	GENMASK(1, 0)},
	{"HSI0_D1",	0b000010, "PCIE_GEN5B_2L ",	GENMASK(1, 0)},
	{"HSI0_D1",	0b000001, "SYSMMU_S0_HSI0",	BIT(0)},

	{"HSI2_D0",	0b000000, "UFS_LINK0",		GENMASK(1, 0)},
	{"HSI2_D0",	0b000010, "UFS_LINK1",		GENMASK(1, 0)},
	{"HSI2_D0",	0b000001, "SYSMMU_S0_HSI2",	BIT(0)},

	{"HSI2_D1",	0b000100, "ETHERNET0",		GENMASK(2, 0)},
	{"HSI2_D1",	0b000000, "BICM_AXI_D0_HSI2",	GENMASK(2, 0)},
	{"HSI2_D1",	0b000110, "ETHERNET1",		GENMASK(2, 0)},
	{"HSI2_D1",	0b000010, "BICM_AXI_D1_HSI2",	GENMASK(2, 0)},
	{"HSI2_D1",	0b000001, "SYSMMU_S1_HSI2",	BIT(0)},

	{"SFI",		0b010000, "CLUSTER_SFI.M0",	GENMASK(4, 0)},
	{"SFI",		0b000000, "BICM_AXI_D0_SFI",	GENMASK(4, 0)},
	{"SFI",		0b010010, "CLUSTER_SFI.M1",	GENMASK(4, 0)},
	{"SFI",		0b000010, "BICM_AXI_D1_SFI",	GENMASK(4, 0)},
	{"SFI",		0b000100, "DMA_SFI",		GENMASK(3, 0)},
	{"SFI",		0b000110, "SC_SFI",		GENMASK(3, 0)},
	{"SFI",		0b000001, "SYSMMU_S0_SFI",	BIT(0)},

	{"SSP",		0b000000, "STRONG",		GENMASK(2, 0)},
	{"SSP",		0b000010, "SC_SFI",		GENMASK(2, 0)},
	{"SSP",		0b000100, "RTIC",		GENMASK(2, 0)},
	{"SSP",		0b000001, "SYSMMU_S0_SSP",	BIT(0)},

	{"MFC_D0",	0b000000, "MFC.M0",		BIT(0)},
	{"MFC_D0",	0b000001, "SYSMMU_S0_MFC",	BIT(0)},

	{"MFC_D1",	0b000000, "MFC.M1",		GENMASK(1, 0)},
	{"MFC_D1",	0b000001, "MFC.M2",		GENMASK(1, 0)},
	{"MFC_D1",	0b000010, "WFD",		GENMASK(1, 0)},

	{"MFD_D0",	0b000000, "MFD.M0",		BIT(0)},
	{"MFD_D0",	0b000001, "SYSMMU_S0_MFD",	BIT(0)},

	{"MFD_D1",	0b000000, "MFD.M1",		0},

	{"MISC",	0b000000, "SPDMA0",		GENMASK(4, 0)},
	{"MISC",	0b000100, "SPDMA1",		GENMASK(4, 0)},
	{"MISC",	0b001000, "PDMA0",		GENMASK(4, 0)},
	{"MISC",	0b001100, "PDMA1",		GENMASK(4, 0)},
	{"MISC",	0b010000, "PDMA2",		GENMASK(4, 0)},
	{"MISC",	0b010100, "PDMA3",		GENMASK(4, 0)},
	{"MISC",	0b011000, "PDMA4",		GENMASK(4, 0)},
	{"MISC",	0b000001, "SYSMMU_S0_MISC",	GENMASK(1, 0)},
	{"MISC",	0b000110, "GIC",		GENMASK(2, 0)},
	{"MISC",	0b000010, "BICM_AXI_D_MISC",	GENMASK(2, 0)},
	{"MISC",	0b000011, "SYSMMU_S1_MISC",	GENMASK(1, 0)},
};

static struct itmon_v2_nodeinfo exynosauto_v920_d_dnc[] = {
	{"DNC_D0", M_NODE, 0, 1, 1, 0, 0, 0, 0, 0,   4, NA, 0, NULL},
	{"DNC_D1", M_NODE, 1, 1, 1, 0, 0, 0, 0, 0,   5, NA, 1, NULL},
	{"DNC_D2", M_NODE, 2, 1, 1, 0, 0, 0, 0, 0,   6, NA, 2, NULL},
	{"DNC_D3", M_NODE, 3, 1, 1, 0, 0, 0, 0, 0,   7, NA, 3, NULL},
	{"DNC_D4", M_NODE, 4, 1, 1, 0, 0, 0, 0, 0,   8, NA, 4, NULL},
	{"DNC_S0", S_NODE, 5, 1, 1, 0, 0, 0, 0, 0, 196, NA, 0, NULL},
	{"DNC_S1", S_NODE, 6, 1, 1, 0, 0, 0, 0, 0, 197, NA, 1, NULL},
	{"DNC_S2", S_NODE, 7, 1, 1, 0, 0, 0, 0, 0, 198, NA, 2, NULL},
	{"DNC_S3", S_NODE, 8, 1, 1, 0, 0, 0, 0, 0, 199, NA, 3, NULL},
};

static struct itmon_v2_nodeinfo exynosauto_v920_d_nocl0[] = {
	{"NOCL0_M0", M_NODE,  0, 1, 1, 0, 0, 0, 0, 0,  64, NA, 0, NULL},
	{"NOCL0_M1", M_NODE,  1, 1, 1, 0, 0, 0, 0, 0,  65, NA, 1, NULL},
	{"NOCL0_M2", M_NODE,  2, 1, 1, 0, 0, 0, 0, 0,  66, NA, 2, NULL},
	{"NOCL0_M3", M_NODE,  3, 1, 1, 0, 0, 0, 0, 0,  67, NA, 3, NULL},
	{"NOCL0_M4", M_NODE,  4, 1, 1, 0, 0, 0, 0, 0,  72, NA, 4, NULL},
	{"NOCL0_M5", M_NODE,  5, 1, 1, 0, 0, 0, 0, 0,  73, NA, 5, NULL},
	{"NOCL0_M6", M_NODE,  6, 1, 1, 0, 0, 0, 0, 0,  74, NA, 6, NULL},
	{"NOCL0_M7", M_NODE,  7, 1, 1, 0, 0, 0, 0, 0,  75, NA, 7, NULL},
	{"IRPS0",    S_NODE,  8, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 128,  0, 0, NULL},
	{"IRPS1",    S_NODE,  9, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 129,  1, 1, NULL},
	{"IRPS2",    S_NODE, 10, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 130,  2, 2, NULL},
	{"IRPS3",    S_NODE, 11, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 131,  3, 3, NULL},
	{"NOCL0_DP", S_NODE, 12, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 132,  4, 4, NULL},
};

static struct itmon_v2_nodeinfo exynosauto_v920_p_nocl0[] = {
	{"CPU_M0",   M_NODE,  0, 1, 1, 0, 0, 0, 0, 0,   0, NA,  0, NULL},
	{"CPU_M1",   M_NODE,  1, 1, 1, 0, 0, 0, 0, 0,   1, NA,  1, NULL},
	{"CPU_M2",   M_NODE,  2, 1, 1, 0, 0, 0, 0, 0,   2, NA,  2, NULL},
	{"SCI_CCM0", M_NODE,  3, 1, 1, 0, 0, 0, 0, 0,   3, NA,  3, NULL},
	{"SCI_CCM1", M_NODE,  4, 1, 1, 0, 0, 0, 0, 0,   4, NA,  4, NULL},
	{"SCI_IRPM", M_NODE,  5, 1, 1, 0, 0, 0, 0, 0,   5, NA,  5, NULL},
	{"NOCL0_DP", M_NODE,  6, 1, 1, 0, 0, 0, 0, 0,   6, NA,  6, NULL},
	{"ALIVE",    S_NODE,  0, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 129,  0,  0, NULL},
	{"CMU",      S_NODE,  1, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 130,  1,  1, NULL},
	{"CPU_S0",   S_NODE,  2, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 131,  2,  2, NULL},
	{"CPU_S1",   S_NODE,  3, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 132,  3,  3, NULL},
	{"CPU_S2",   S_NODE,  4, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 133,  4,  4, NULL},
	{"GIC",      S_NODE,  5, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 134,  5,  5, NULL},
	{"G3D",      S_NODE,  6, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 135,  6,  6, NULL},
	{"MIF0",     S_NODE,  7, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 136,  7,  7, NULL},
	{"MIF1",     S_NODE,  8, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 137,  8,  8, NULL},
	{"MIF2",     S_NODE,  9, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 138,  9,  9, NULL},
	{"MIF3",     S_NODE, 10, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 139, 10, 10, NULL},
	{"MISC",     S_NODE, 11, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 140, 11, 11, NULL},
	{"PERIC0",   S_NODE, 12, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 141, 12, 12, NULL},
	{"PERIC1",   S_NODE, 13, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 142, 13, 13, NULL},
	{"NOCL0",    S_NODE, 14, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 128, 14, 14, NULL},
	{"NOCL1_P",  S_NODE, 15, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 192, NA, 15, NULL},
	{"NOCL2_P",  S_NODE, 16, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 193, NA, 16, NULL},
};

static struct itmon_v2_nodeinfo exynosauto_v920_d_nocl1[] = {
	{"ALIVE",    M_NODE,  0, 1, 1, 0, 0, 0, 0, 0,   1, NA,  0, NULL},
	{"AUD",      M_NODE,  1, 1, 1, 0, 0, 0, 0, 0,   2, NA,  1, NULL},
	{"CSSYS",    M_NODE,  2, 1, 1, 0, 0, 0, 0, 0,   3, NA,  2, NULL},
	{"G3DMMU",   M_NODE,  3, 1, 1, 0, 0, 0, 0, 0,  15, NA,  3, NULL},
	{"DNC_M0",   M_NODE,  4, 1, 1, 0, 0, 0, 0, 0,  68, NA,  4, NULL},
	{"DNC_M1",   M_NODE,  5, 1, 1, 0, 0, 0, 0, 0,  69, NA,  5, NULL},
	{"DNC_M2",   M_NODE,  6, 1, 1, 0, 0, 0, 0, 0,  70, NA,  6, NULL},
	{"DNC_M3",   M_NODE,  7, 1, 1, 0, 0, 0, 0, 0,  71, NA,  7, NULL},
	{"HSI0_D0",  M_NODE,  8, 1, 1, 0, 0, 0, 0, 0,  16, NA,  8, NULL},
	{"HSI0_D1",  M_NODE,  9, 1, 1, 0, 0, 0, 0, 0,  17, NA,  9, NULL},
	{"HSI2_D0",  M_NODE, 10, 1, 1, 0, 0, 0, 0, 0,  19, NA, 10, NULL},
	{"HSI2_D1",  M_NODE, 11, 1, 1, 0, 0, 0, 0, 0,  20, NA, 11, NULL},
	{"SFI",      M_NODE, 12, 1, 1, 0, 0, 0, 0, 0,  29, NA, 12, NULL},
	{"SSP",      M_NODE, 13, 1, 1, 0, 0, 0, 0, 0,  30, NA, 13, NULL},
	{"MFC_D0",   M_NODE, 14, 1, 1, 0, 0, 0, 0, 0,  24, NA, 14, NULL},
	{"MFC_D1",   M_NODE, 15, 1, 1, 0, 0, 0, 0, 0,  25, NA, 15, NULL},
	{"MFD_D0",   M_NODE, 16, 1, 1, 0, 0, 0, 0, 0,  26, NA, 16, NULL},
	{"MFD_D1",   M_NODE, 17, 1, 1, 0, 0, 0, 0, 0,  27, NA, 17, NULL},
	{"MISC",     M_NODE, 18, 1, 1, 0, 0, 0, 0, 0,  28, NA, 18, NULL},
	{"NOCL1_S0", S_NODE, 19, 1, 1, 0, 0, 0, 0, 0, 192, NA,  0, NULL},
	{"NOCL1_S1", S_NODE, 20, 1, 1, 0, 0, 0, 0, 0, 193, NA,  1, NULL},
	{"NOCL1_S2", S_NODE, 21, 1, 1, 0, 0, 0, 0, 0, 194, NA,  2, NULL},
	{"NOCL1_S3", S_NODE, 22, 1, 1, 0, 0, 0, 0, 0, 195, NA,  3, NULL},
};

static struct itmon_v2_nodeinfo exynosauto_v920_p_nocl1[] = {
	{"NOCL1_M", M_NODE, 0, 1, 1, 0, 0, 0, 0, 0,  64, NA,  0, NULL},
	{"NOCL1",   S_NODE, 1, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 143,  0,  0, NULL},
	{"AUD",     S_NODE, 2, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 144,  1,  1, NULL},
	{"DNC",     S_NODE, 3, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 145,  2,  2, NULL},
	{"HSI0",    S_NODE, 4, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 146,  3,  3, NULL},
	{"HSI2",    S_NODE, 5, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 147,  4,  4, NULL},
	{"SFI",     S_NODE, 6, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 148,  5,  5, NULL},
	{"SSP",     S_NODE, 7, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 149,  6,  6, NULL},
	{"MFC",     S_NODE, 8, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 150,  7,  7, NULL},
	{"MFD",     S_NODE, 9, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 151,  8,  8, NULL},
};

static struct itmon_v2_nodeinfo exynosauto_v920_d_nocl2[] = {
	{"ACC",      M_NODE,  0, 1, 1, 0, 0, 0, 0, 0,   0, NA,  0, NULL},
	{"DPUF0_D0", M_NODE,  1, 1, 1, 0, 0, 0, 0, 0,   9, NA,  1, NULL},
	{"DPUF0_D1", M_NODE,  2, 1, 1, 0, 0, 0, 0, 0,  10, NA,  2, NULL},
	{"DPUF1_D0", M_NODE,  3, 1, 1, 0, 0, 0, 0, 0,  11, NA,  3, NULL},
	{"DPUF1_D1", M_NODE,  4, 1, 1, 0, 0, 0, 0, 0,  12, NA,  4, NULL},
	{"DPUF2_D0", M_NODE,  5, 1, 1, 0, 0, 0, 0, 0,  13, NA,  5, NULL},
	{"DPUF2_D1", M_NODE,  6, 1, 1, 0, 0, 0, 0, 0,  14, NA,  6, NULL},
	{"HSI1",     M_NODE,  7, 1, 1, 0, 0, 0, 0, 0,  18, NA,  7, NULL},
	{"ISP",      M_NODE,  8, 1, 1, 0, 0, 0, 0, 0,  21, NA,  8, NULL},
	{"M2M_D0",   M_NODE,  9, 1, 1, 0, 0, 0, 0, 0,  22, NA,  9, NULL},
	{"M2M_D1",   M_NODE, 10, 1, 1, 0, 0, 0, 0, 0,  23, NA, 10, NULL},
	{"SNW_D0",   M_NODE, 11, 1, 1, 0, 0, 0, 0, 0,  31, NA, 11, NULL},
	{"SNW_D1",   M_NODE, 12, 1, 1, 0, 0, 0, 0, 0,  32, NA, 12, NULL},
	{"SNW_D2",   M_NODE, 13, 1, 1, 0, 0, 0, 0, 0,  33, NA, 13, NULL},
	{"TAA",      M_NODE, 14, 1, 1, 0, 0, 0, 0, 0,  34, NA, 14, NULL},
	{"NOCL2_S0", S_NODE, 15, 1, 1, 0, 0, 0, 0, 0, 200, NA,  0, NULL},
	{"NOCL2_S1", S_NODE, 16, 1, 1, 0, 0, 0, 0, 0, 201, NA,  1, NULL},
	{"NOCL2_S2", S_NODE, 17, 1, 1, 0, 0, 0, 0, 0, 202, NA,  2, NULL},
	{"NOCL2_S3", S_NODE, 18, 1, 1, 0, 0, 0, 0, 0, 203, NA,  3, NULL},
};

static struct itmon_v2_nodeinfo exynosauto_v920_p_nocl2[] = {
	{"NOCL2_M", M_NODE,  0, 1, 1, 0, 0, 0, 0, 0,  65, NA,  0, NULL},
	{"NOCL2",   S_NODE,  1, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 152,  0,  0, NULL},
	{"ACC",     S_NODE,  2, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 153,  1,  1, NULL},
	{"DPTX",    S_NODE,  3, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 154,  2,  2, NULL},
	{"DPUB",    S_NODE,  4, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 163,  3,  3, NULL},
	{"DPUF0",   S_NODE,  5, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 155,  4,  4, NULL},
	{"DPUF1",   S_NODE,  6, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 156,  5,  5, NULL},
	{"DPUF2",   S_NODE,  7, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 157,  6,  6, NULL},
	{"HSI1",    S_NODE,  8, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 158,  7,  7, NULL},
	{"ISP",     S_NODE,  9, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 159,  8,  8, NULL},
	{"M2M",     S_NODE, 10, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 160,  9,  9, NULL},
	{"SNW",     S_NODE, 11, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 161, 10, 10, NULL},
	{"TAA",     S_NODE, 12, 1, 1, 0, 0, 0, 0, V2_TMOUT_VAL, 162, 11, 11, NULL},
};

static struct itmon_v2_nodegroup exynoautov920_nodegroup[] = {
	{
		.name = "D_DNC",
		.phy_regs = 0x1B6F0000,
		.nodeinfo = exynosauto_v920_d_dnc,
		.nodesize = ARRAY_SIZE(exynosauto_v920_d_dnc),
		.path_type = V2_DATA,
		.pd_name = "pd-dnc",
		.pd_support = true,
	},
	{
		.name = "D_NOCL0",
		.phy_regs = 0x1DC40000,
		.nodeinfo = exynosauto_v920_d_nocl0,
		.nodesize = ARRAY_SIZE(exynosauto_v920_d_nocl0),
		.path_type = V2_DATA,
		.frz_support = 1,
	},
	{
		.name = "P_NOCL0",
		.phy_regs = 0x1DC50000,
		.nodeinfo = exynosauto_v920_p_nocl0,
		.nodesize = ARRAY_SIZE(exynosauto_v920_p_nocl0),
		.path_type = V2_CONFIG,
		.frz_support = 1,
	},
	{
		.name = "D_NOCL1",
		.phy_regs = 0x1E050000,
		.nodeinfo = exynosauto_v920_d_nocl1,
		.nodesize = ARRAY_SIZE(exynosauto_v920_d_nocl1),
		.path_type = V2_DATA,
	},
	{
		.name = "P_NOCL1",
		.phy_regs = 0x1E060000,
		.nodeinfo = exynosauto_v920_p_nocl1,
		.nodesize = ARRAY_SIZE(exynosauto_v920_p_nocl1),
		.path_type = V2_CONFIG,
	},
	{
		.name = "D_NOCL2",
		.phy_regs = 0x1E270000,
		.nodeinfo = exynosauto_v920_d_nocl2,
		.nodesize = ARRAY_SIZE(exynosauto_v920_d_nocl2),
		.path_type = V2_DATA,
	},
	{
		.name = "P_NOCL2",
		.phy_regs = 0x1E280000,
		.nodeinfo = exynosauto_v920_p_nocl2,
		.nodesize = ARRAY_SIZE(exynosauto_v920_p_nocl2),
		.path_type = V2_CONFIG,
	},
};

struct itmon_v2_info_table exynosautov920_itmon_v2_info = {
	.rpathinfo = exynoautov920_rpathinfo,
	.num_rpathinfo = (int)ARRAY_SIZE(exynoautov920_rpathinfo),

	.masterinfo = exynoautov920_masterinfo,
	.num_masterinfo = (int)ARRAY_SIZE(exynoautov920_masterinfo),

	.nodegroup = exynoautov920_nodegroup,
	.num_nodegroup = (int)ARRAY_SIZE(exynoautov920_nodegroup),
};

struct itmon_info_table exynosautov920_itmon_info = {
	.ops = &itmon_v2_ops,
	.info_table = &exynosautov920_itmon_v2_info,
};