// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - IPs Traffic Monitor(ITMON) support
 */

#include "exynos-itmon-platform.h"
#include "exynos-itmon-local-v1.h"

const static struct itmon_v1_rpathinfo rpathinfo[] = {
	/* 0x8000_0000 - 0xf_ffff_ffff */
	{0,	"DPUM0",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{1,	"DPUM1",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{2,	"DPUM2",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{3,	"DPUM3",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{4,	"G2D0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{5,	"G2D1",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{6,	"G2D2",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{7,	"TAA0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{8,	"TAA1",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{9,	"ISPB0",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{10,	"ISPB1",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{11,	"MFC0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{12,	"MFC1",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{13,	"DPUS00",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{14,	"DPUS01",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{15,	"DPUS02",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{16,	"DPUS03",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{17,	"DPUS10",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{18,	"DPUS11",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{19,	"DPUS12",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{20,	"DPUS13",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{21,	"ACC0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{22,	"ACC1",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{23,	"FSYS1",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{24,	"SIREX",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{25,	"SECU",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{26,	"FSYS2",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{31,	"DNC0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{32,	"DNCC",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{33,	"FSYS00",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{34,	"FSYS01",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{35,	"SFI",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{36,	"AUD",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{37,	"ALVPS",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{38,	"MTCACHE",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{47,	"DPUM0",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{48,	"DPUM1",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{49,	"DPUM2",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{50,	"DPUM3",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{51,	"G2D0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{52,	"G2D1",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{53,	"G2D2",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{54,	"TAA0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{55,	"TAA1",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{56,	"ISPB0",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{57,	"ISPB1",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{58,	"MFC0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{59,	"MFC1",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{60,	"DPUS00",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{61,	"DPUS01",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{62,	"DPUS02",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{63,	"DPUS03",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{64,	"DPUS10",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{65,	"DPUS11",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{66,	"DPUS12",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{67,	"DPUS13",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{68,	"ACC0",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{69,	"ACC1",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{70,	"FSYS1",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{71,	"SIREX",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{72,	"SECU",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{73,	"FSYS2",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{78,	"DNC4",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{79,	"DNCC",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{80,	"FSYS00",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{81,	"FSYS01",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{82,	"SFI",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{83,	"AUD",		"DREX_IRPS",	GENMASK(6, 0), 0},
	{84,	"ALVPS",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{85,	"MTCACHE",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{94,	"CORESIGHT",	"DREX_IRPS",	GENMASK(6, 0), 0},
	{95,	"MMU_G3D",	"DREX_IRPS",	GENMASK(6, 0), 0},

	/* 0x0000_0000 - 0x7fff_ffff */

	{0,	"DPUM0",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{1,	"DPUM1",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{2,	"DPUM2",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{3,	"DPUM3",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{4,	"G2D0",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{5,	"G2D1",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{6,	"G2D2",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{7,	"TAA0",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{8,	"TAA1",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{9,	"ISPB0",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{10,	"ISPB1",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{11,	"MFC0",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{12,	"MFC1",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{13,	"DPUS00",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{14,	"DPUS01",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{15,	"DPUS02",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{16,	"DPUS03",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{17,	"DPUS10",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{18,	"DPUS11",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{19,	"DPUS12",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{20,	"DPUS13",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{21,	"ACC0",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{22,	"ACC1",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{23,	"TAA2",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{24,	"FSYS1",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{25,	"SIREX",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{26,	"SECU",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{27,	"FSYS2",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{31,	"DNCC",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{32,	"FSYS00",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{33,	"FSYS01",	"BUSMC_DP",	GENMASK(5, 0), 0},
	{34,	"SFI",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{35,	"AUD",		"BUSMC_DP",	GENMASK(5, 0), 0},
	{36,	"ALVPS",	"BUSMC_DP",	GENMASK(5, 0), 0},

	/* 0x0000_0000 - 0x7fff_ffff */

	{0,	"CORESIGHT",	"CORE_DP",	BIT(0), 0},
	{1,	"MMU_G3D",	"CORE_DP",	BIT(0), 0},
};

const static struct itmon_v1_masterinfo masterinfo[] = {
	{"DPUM0",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUM0",	BIT(1),	/* XXXX10 */	"SYSMMU_D0_DPUM",	GENMASK(1, 0)},
	{"DPUM0",	BIT(0),	/* XXXXX1 */	"SYSMMU_D0_DPUM_S2MPU",	BIT(0)},
	{"DPUM1",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUM1",	BIT(1),	/* XXXX10 */	"SYSMMU_D1_DPUM",	GENMASK(1, 0)},
	{"DPUM1",	BIT(0),	/* XXXXX1 */	"SYSMMU_D1_DPUM_S2MPU",	BIT(0)},
	{"DPUM2",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUM2",	BIT(1),	/* XXXX10 */	"SYSMMU_D2_DPUM",	GENMASK(1, 0)},
	{"DPUM2",	BIT(0),	/* XXXXX1 */	"SYSMMU_D2_DPUM_S2MPU",	BIT(0)},
	{"DPUM3",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUM3",	BIT(1),	/* XXXX10 */	"SYSMMU_D3_DPUM",	GENMASK(1, 0)},
	{"DPUM3",	BIT(0),	/* XXXXX1 */	"SYSMMU_D3_DPUM_S2MPU",	BIT(0)},

	{"DPUS00",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUS00",	BIT(1),	/* XXXX10 */	"SYSMMU_D0_DPUS",	GENMASK(1, 0)},
	{"DPUS00",	BIT(0),	/* XXXXX1 */	"SYSMMU_D0_DPUS_S2MPU",	BIT(0)},
	{"DPUS01",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUS01",	BIT(1),	/* XXXX10 */	"SYSMMU_D1_DPUS",	GENMASK(1, 0)},
	{"DPUS01",	BIT(0),	/* XXXXX1 */	"SYSMMU_D1_DPUS_S2MPU",	BIT(0)},
	{"DPUS02",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUS02",	BIT(1),	/* XXXX10 */	"SYSMMU_D2_DPUS",	GENMASK(1, 0)},
	{"DPUS02",	BIT(0),	/* XXXXX1 */	"SYSMMU_D2_DPUS_S2MPU",	BIT(0)},
	{"DPUS03",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUS03",	BIT(1),	/* XXXX10 */	"SYSMMU_D3_DPUS",	GENMASK(1, 0)},
	{"DPUS03",	BIT(0),	/* XXXXX1 */	"SYSMMU_D3_DPUS_S2MPU",	BIT(0)},

	{"DPUS10",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUS10",	BIT(1),	/* XXXX10 */	"SYSMMU_D0_DPUS",	GENMASK(1, 0)},
	{"DPUS10",	BIT(0),	/* XXXXX1 */	"SYSMMU_D0_DPUS_S2MPU",	BIT(0)},
	{"DPUS11",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUS11",	BIT(1),	/* XXXX10 */	"SYSMMU_D1_DPUS",	GENMASK(1, 0)},
	{"DPUS11",	BIT(0),	/* XXXXX1 */	"SYSMMU_D1_DPUS_S2MPU",	BIT(0)},
	{"DPUS12",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUS12",	BIT(1),	/* XXXX10 */	"SYSMMU_D2_DPUS",	GENMASK(1, 0)},
	{"DPUS12",	BIT(0),	/* XXXXX1 */	"SYSMMU_D2_DPUS_S2MPU",	BIT(0)},
	{"DPUS13",	0,	/* XXXX00 */	"DPP",			GENMASK(1, 0)},
	{"DPUS13",	BIT(1),	/* XXXX10 */	"SYSMMU_D3_DPUS",	GENMASK(1, 0)},
	{"DPUS13",	BIT(0),	/* XXXXX1 */	"SYSMMU_D3_DPUS_S2MPU",	BIT(0)},

	{"TAA0",	0,	/* XXXX00 */	"TAA",			GENMASK(1, 0)},
	{"TAA0",	BIT(1),	/* XXXX10 */	"SYSMMU_TAA",		GENMASK(1, 0)},
	{"TAA0",	BIT(0),	/* XXXXX1 */	"SYSMMU_TAA_S2MPU",	BIT(0)},
	{"TAA1",	0,	/* XXXX00 */	"TAA",			GENMASK(1, 0)},
	{"TAA1",	BIT(1),	/* XXXX10 */	"SYSMMU_TAA",		GENMASK(1, 0)},
	{"TAA1",	BIT(0),	/* XXXXX1 */	"SYSMMU_TAA_S2MPU",	BIT(0)},

	{"ISPB0",	0,	/* XXXX00 */	"ISPB",			GENMASK(1, 0)},
	{"ISPB0",	BIT(1),	/* XXXX10 */	"SYSMMU_ISPB",		GENMASK(1, 0)},
	{"ISPB0",	BIT(0),	/* XXXXX1 */	"SYSMMU_ISPB_S2MPU",	BIT(0)},
	{"ISPB1",	0,	/* XXXX00 */	"ISPB",			GENMASK(1, 0)},
	{"ISPB1",	BIT(1),	/* XXXX10 */	"SYSMMU_ISPB",		GENMASK(1, 0)},
	{"ISPB1",	BIT(0),	/* XXXXX1 */	"SYSMMU_ISPB_S2MPU",	BIT(0)},

	{"MFC0",	0,	/* XXXX00 */	"MFC",			GENMASK(1, 0)},
	{"MFC0",	BIT(1),	/* XXXX10 */	"SYSMMU_MFC0",		GENMASK(1, 0)},
	{"MFC0",	BIT(0),	/* XXXXX1 */	"SYSMMU_MFC0_S2MPU",	BIT(0)},

	{"MFC1",	0,	/* XXX000 */	"MFC",			GENMASK(2, 0)},
	{"MFC1",	BIT(2),	/* XXX100 */	"WFD",			GENMASK(2, 0)},
	{"MFC1",	BIT(1),	/* XXXX10 */	"SYSMMU_MFC1",		GENMASK(1, 0)},
	{"MFC1",	BIT(0),	/* XXXXX1 */	"SYSMMU_MFC1_S2MPU",	BIT(0)},

	{"SECU",	0,	/* XXXX00 */	"SSS",			GENMASK(1, 0)},
	{"SECU",	BIT(1),	/* XXXX10 */	"RTIC",			GENMASK(1, 0)},
	{"SECU",	BIT(0),	/* XXXXX1 */	"SYSMMU_BUSC",		BIT(0)},

	{"ACC0",	0,	/* XXXX00 */	"ISPPRE",		GENMASK(1, 0)},
	{"ACC0",	BIT(1),	/* XXXX10 */	"SYSMMU_ISPPRE",	GENMASK(1, 0)},
	{"ACC0",	BIT(0),	/* XXXXX1 */	"SYSMMU_ISPPRE_S2MPU",	BIT(0)},

	{"ACC1",	0,	/* XX0000 */	"GDC0",			GENMASK(3, 0)},
	{"ACC1",	BIT(2),	/* XX0100 */	"GDC1",			GENMASK(3, 0)},
	{"ACC1",	BIT(3),	/* XX1000 */	"VRA",			GENMASK(3, 0)},
	{"ACC1",	BIT(1),	/* XXXX10 */	"SYSMMU_ACC",		GENMASK(1, 0)},
	{"ACC1",	BIT(0),	/* XXXXX1 */	"SYSMMU_ACC_S2MPU",	BIT(0)},

	{"FSYS00",	0,	/* XX0000 */	"PCIE_GEN3A_4L",	GENMASK(3, 0)},
	{"FSYS00",	BIT(1),	/* XX0010 */	"PCIE_GEN3B_4L",	GENMASK(3, 0)},
	{"FSYS00",	BIT(0),	/* XXXXX1 */	"S2MPU_D0_FSYS0",	BIT(0)},

	{"FSYS01",	BIT(2),			/* XX0100 */
		"PCIE_GEN3A_2L0",	GENMASK(3, 0)},
	{"FSYS01",	BIT(2) | BIT(1),	/* XX0110 */
		"PCIE_GEN3B_2L0",	GENMASK(3, 0)},
	{"FSYS01",	BIT(3),			/* XX1000 */
		"PCIE_GEN3A_2L1",	GENMASK(3, 0)},
	{"FSYS01",	BIT(3) | BIT(1),	/* XX1010 */
		"PCIE_GEN3B_2L1",	GENMASK(3, 0)},
	{"FSYS01",	BIT(0),			/* XXXXX1 */
		"S2MPU_D1_FSYS0",	BIT(0)},

	{"FSYS1",	0,			/* XX0000 */
		"USB30DRD_0",		GENMASK(3, 0)},
	{"FSYS1",	BIT(1),			/* XX0010 */
		"USB30DRD_1",		GENMASK(3, 0)},
	{"FSYS1",	BIT(2),			/* XX0100 */
		"USB20DRD_0",		GENMASK(3, 0)},
	{"FSYS1",	GENMASK(2, 1),		/* XX0110 */
		"USB20DRD_1",		GENMASK(3, 0)},
	{"FSYS1",	BIT(3),			/* XX1000 */
		"MMC_CARD",		GENMASK(3, 0)},
	{"FSYS1",	BIT(0),			/* XXXXX1 */
		"S2MPU_D_FSYS1",	BIT(0)},

	{"FSYS2",	0,			/* XXX000 */
		"UFS_EMBD0",		GENMASK(2, 0)},
	{"FSYS2",	BIT(1),			/* XXX010 */
		"UFS_EMBD1",		GENMASK(2, 0)},
	{"FSYS2",	BIT(2),			/* X00100 */
		"ETHERNET0",		GENMASK(4, 0)},
	{"FSYS2",	BIT(4) | BIT(2),	/* X10100 */
		"ETHERNET1",		GENMASK(4, 0)},
	{"FSYS2",	GENMASK(3, 2),		/* XX1100 */
		"SYSMMU_ETHERNET",	GENMASK(3, 0)},
	{"FSYS2",	BIT(0),			/* XXXXX1 */
		"S2MPU_D_FSYS2",	BIT(0)},

	{"SIREX",	0,			/* 00XXXX */
		"SIREX",		GENMASK(5, 4)},

	{"G2D0",	0,			/* XXXX00 */
		"G2D",			GENMASK(1, 0)},
	{"G2D0",	BIT(1),			/* XXXX10 */
		"SYSMMU_G2D0",		GENMASK(1, 0)},
	{"G2D0",	BIT(0),			/* XXXXX1 */
		"SYSMMU_G2D0_S2MPU",	BIT(0)},

	{"G2D1",	0,			/* XXXX00 */
		"G2D",			GENMASK(1, 0)},
	{"G2D1",	BIT(1),			/* XXXX10 */
		"SYSMMU_G2D1",		GENMASK(1, 0)},
	{"G2D1",	BIT(0),			/* XXXXX1 */
		"SYSMMU_G2D1_S2MPU",	BIT(0)},

	{"G2D2",	0,			/* XXX000 */
		"JPEG",			GENMASK(2, 0)},
	{"G2D2",	BIT(2),			/* XXX100 */
		"MSCL",			GENMASK(2, 0)},
	{"G2D2",	BIT(1),			/* XXXX10 */
		"SYSMMU_G2D2",		GENMASK(1, 0)},
	{"G2D2",	BIT(0),			/* XXXXX1 */
		"SYSMMU_G2D2_S2MPU",	BIT(0)},

	{"DNC0",	0,				/* X00000 */
		"DMA0",				GENMASK(4, 0)},
	{"DNC0",	BIT(4),				/* X10000 */
		"SYSMMU_D0_DNCDMA",		GENMASK(4, 0)},
	{"DNC0",	BIT(3),				/* XX1000 */
		"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC0",	BIT(0),				/* X00001 */
		"DMA1",				GENMASK(4, 0)},
	{"DNC0",	BIT(4) | BIT(0),		/* X10001 */
		"SYSMMU_D0_DNCFLC",		GENMASK(4, 0)},
	{"DNC0",	BIT(3) | BIT(0),		/* XX1001 */
		"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC0",	BIT(2),				/* X00010 */
		"DMA2",				GENMASK(4, 0)},
	{"DNC0",	BIT(4) | BIT(1),		/* X10010 */
		"SYSMMU_D1_DNCDMA",		GENMASK(4, 0)},
	{"DNC0",	BIT(3) | BIT(1),		/* XX1010 */
		"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC0",	GENMASK(1, 0),			/* X00011 */
		"DMA3",				GENMASK(4, 0)},
	{"DNC0",	BIT(4) | GENMASK(1, 0),		/* X10011 */
		"SYSMMU_D1_DNCFLC",		GENMASK(4, 0)},
	{"DNC0",	BIT(3) | GENMASK(1, 0),		/* XX1011 */
		"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC0",	BIT(2),				/* X00100 */
		"DMA4",				GENMASK(4, 0)},
	{"DNC0",	BIT(4) | BIT(2),		/* X10100 */
		"SYSMMU_D2_DNCDMA",		GENMASK(4, 0)},
	{"DNC0",	BIT(3) | BIT(2),		/* XX1100 */
		"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC0",	BIT(2) | BIT(0),		/* X00101 */
		"DMA5",				GENMASK(4, 0)},
	{"DNC0",	BIT(4) | BIT(2) | BIT(0),	/* X10101 */
		"SYSMMU_D2_DNCFLC",		GENMASK(4, 0)},
	{"DNC0",	BIT(3) | BIT(2) | BIT(0),	/* XX1101 */
		"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(3, 0)},

	{"DNC1",	0,				/* X00000 */
		"DMA0",				GENMASK(4, 0)},
	{"DNC1",	BIT(4),				/* X10000 */
		"SYSMMU_D0_DNCDMA",		GENMASK(4, 0)},
	{"DNC1",	BIT(3),				/* XX1000 */
		"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC1",	BIT(0),				/* X00001 */
		"DMA1",				GENMASK(4, 0)},
	{"DNC1",	BIT(4) | BIT(0),		/* X10001 */
		"SYSMMU_D0_DNCFLC",		GENMASK(4, 0)},
	{"DNC1",	BIT(3) | BIT(0),		/* XX1001 */
		"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC1",	BIT(2),				/* X00010 */
		"DMA2",				GENMASK(4, 0)},
	{"DNC1",	BIT(4) | BIT(1),		/* X10010 */
		"SYSMMU_D1_DNCDMA",		GENMASK(4, 0)},
	{"DNC1",	BIT(3) | BIT(1),		/* XX1010 */
		"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC1",	GENMASK(1, 0),			/* X00011 */
		"DMA3",				GENMASK(4, 0)},
	{"DNC1",	BIT(4) | GENMASK(1, 0),		/* X10011 */
		"SYSMMU_D1_DNCFLC",		GENMASK(4, 0)},
	{"DNC1",	BIT(3) | GENMASK(1, 0),		/* XX1011 */
		"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC1",	BIT(2),				/* X00100 */
		"DMA4",				GENMASK(4, 0)},
	{"DNC1",	BIT(4) | BIT(2),		/* X10100 */
		"SYSMMU_D2_DNCDMA",		GENMASK(4, 0)},
	{"DNC1",	BIT(3) | BIT(2),		/* XX1100 */
		"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC1",	BIT(2) | BIT(0),		/* X00101 */
		"DMA5",				GENMASK(4, 0)},
	{"DNC1",	BIT(4) | BIT(2) | BIT(0),	/* X10101 */
		"SYSMMU_D2_DNCFLC",		GENMASK(4, 0)},
	{"DNC1",	BIT(3) | BIT(2) | BIT(0),	/* XX1101 */
		"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(3, 0)},

	{"DNC2",	0,				/* X00000 */
		"DMA0",				GENMASK(4, 0)},
	{"DNC2",	BIT(4),				/* X10000 */
		"SYSMMU_D0_DNCDMA",		GENMASK(4, 0)},
	{"DNC2",	BIT(3),				/* XX1000 */
		"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC2",	BIT(0),				/* X00001 */
		"DMA1",				GENMASK(4, 0)},
	{"DNC2",	BIT(4) | BIT(0),		/* X10001 */
		"SYSMMU_D0_DNCFLC",		GENMASK(4, 0)},
	{"DNC2",	BIT(3) | BIT(0),		/* XX1001 */
		"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC2",	BIT(2),				/* X00010 */
		"DMA2",				GENMASK(4, 0)},
	{"DNC2",	BIT(4) | BIT(1),		/* X10010 */
		"SYSMMU_D1_DNCDMA",		GENMASK(4, 0)},
	{"DNC2",	BIT(3) | BIT(1),		/* XX1010 */
		"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC2",	GENMASK(1, 0),			/* X00011 */
		"DMA3",				GENMASK(4, 0)},
	{"DNC2",	BIT(4) | GENMASK(1, 0),		/* X10011 */
		"SYSMMU_D1_DNCFLC",		GENMASK(4, 0)},
	{"DNC2",	BIT(3) | GENMASK(1, 0),		/* XX1011 */
		"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC2",	BIT(2),				/* X00100 */
		"DMA4",				GENMASK(4, 0)},
	{"DNC2",	BIT(4) | BIT(2),		/* X10100 */
		"SYSMMU_D2_DNCDMA",		GENMASK(4, 0)},
	{"DNC2",	BIT(3) | BIT(2),		/* XX1100 */
		"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC2",	BIT(2) | BIT(0),		/* X00101 */
		"DMA5",				GENMASK(4, 0)},
	{"DNC2",	BIT(4) | BIT(2) | BIT(0),	/* X10101 */
		"SYSMMU_D2_DNCFLC",		GENMASK(4, 0)},
	{"DNC2",	BIT(3) | BIT(2) | BIT(0),	/* XX1101 */
		"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(3, 0)},

	{"DNC3",	0,				/* X00000 */
		"DMA0",				GENMASK(4, 0)},
	{"DNC3",	BIT(4),				/* X10000 */
		"SYSMMU_D0_DNCDMA",		GENMASK(4, 0)},
	{"DNC3",	BIT(3),				/* XX1000 */
		"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC3",	BIT(0),				/* X00001 */
		"DMA1",				GENMASK(4, 0)},
	{"DNC3",	BIT(4) | BIT(0),		/* X10001 */
		"SYSMMU_D0_DNCFLC",		GENMASK(4, 0)},
	{"DNC3",	BIT(3) | BIT(0),		/* XX1001 */
		"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC3",	BIT(2),				/* X00010 */
		"DMA2",				GENMASK(4, 0)},
	{"DNC3",	BIT(4) | BIT(1),		/* X10010 */
		"SYSMMU_D1_DNCDMA",		GENMASK(4, 0)},
	{"DNC3",	BIT(3) | BIT(1),		/* XX1010 */
		"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC3",	GENMASK(1, 0),			/* X00011 */
		"DMA3",				GENMASK(4, 0)},
	{"DNC3",	BIT(4) | GENMASK(1, 0),		/* X10011 */
		"SYSMMU_D1_DNCFLC",		GENMASK(4, 0)},
	{"DNC3",	BIT(3) | GENMASK(1, 0),		/* XX1011 */
		"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC3",	BIT(2),				/* X00100 */
		"DMA4",				GENMASK(4, 0)},
	{"DNC3",	BIT(4) | BIT(2),		/* X10100 */
		"SYSMMU_D2_DNCDMA",		GENMASK(4, 0)},
	{"DNC3",	BIT(3) | BIT(2),		/* XX1100 */
		"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC3",	BIT(2) | BIT(0),		/* X00101 */
		"DMA5",				GENMASK(4, 0)},
	{"DNC3",	BIT(4) | BIT(2) | BIT(0),	/* X10101 */
		"SYSMMU_D2_DNCFLC",		GENMASK(4, 0)},
	{"DNC3",	BIT(3) | BIT(2) | BIT(0),	/* XX1101 */
		"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(3, 0)},

	{"DNC4",	0,				/* X00000 */
		"DMA0",				GENMASK(4, 0)},
	{"DNC4",	BIT(4),				/* X10000 */
		"SYSMMU_D0_DNCDMA",		GENMASK(4, 0)},
	{"DNC4",	BIT(3),				/* XX1000 */
		"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC4",	BIT(0),				/* X00001 */
		"DMA1",				GENMASK(4, 0)},
	{"DNC4",	BIT(4) | BIT(0),		/* X10001 */
		"SYSMMU_D0_DNCFLC",		GENMASK(4, 0)},
	{"DNC4",	BIT(3) | BIT(0),		/* XX1001 */
		"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC4",	BIT(2),				/* X00010 */
		"DMA2",				GENMASK(4, 0)},
	{"DNC4",	BIT(4) | BIT(1),		/* X10010 */
		"SYSMMU_D1_DNCDMA",		GENMASK(4, 0)},
	{"DNC4",	BIT(3) | BIT(1),		/* XX1010 */
		"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC4",	GENMASK(1, 0),			/* X00011 */
		"DMA3",				GENMASK(4, 0)},
	{"DNC4",	BIT(4) | GENMASK(1, 0),		/* X10011 */
		"SYSMMU_D1_DNCFLC",		GENMASK(4, 0)},
	{"DNC4",	BIT(3) | GENMASK(1, 0),		/* XX1011 */
		"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC4",	BIT(2),				/* X00100 */
		"DMA4",				GENMASK(4, 0)},
	{"DNC4",	BIT(4) | BIT(2),		/* X10100 */
		"SYSMMU_D2_DNCDMA",		GENMASK(4, 0)},
	{"DNC4",	BIT(3) | BIT(2),		/* XX1100 */
		"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC4",	BIT(2) | BIT(0),		/* X00101 */
		"DMA5",				GENMASK(4, 0)},
	{"DNC4",	BIT(4) | BIT(2) | BIT(0),	/* X10101 */
		"SYSMMU_D2_DNCFLC",		GENMASK(4, 0)},
	{"DNC4",	BIT(3) | BIT(2) | BIT(0),	/* XX1101 */
		"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(3, 0)},

	{"DNC5",	0,				/* X00000 */
		"DMA0",				GENMASK(4, 0)},
	{"DNC5",	BIT(4),				/* X10000 */
		"SYSMMU_D0_DNCDMA",		GENMASK(4, 0)},
	{"DNC5",	BIT(3),				/* XX1000 */
		"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC5",	BIT(0),				/* X00001 */
		"DMA1",				GENMASK(4, 0)},
	{"DNC5",	BIT(4) | BIT(0),		/* X10001 */
		"SYSMMU_D0_DNCFLC",		GENMASK(4, 0)},
	{"DNC5",	BIT(3) | BIT(0),		/* XX1001 */
		"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC5",	BIT(2),				/* X00010 */
		"DMA2",				GENMASK(4, 0)},
	{"DNC5",	BIT(4) | BIT(1),		/* X10010 */
		"SYSMMU_D1_DNCDMA",		GENMASK(4, 0)},
	{"DNC5",	BIT(3) | BIT(1),		/* XX1010 */
		"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC5",	GENMASK(1, 0),			/* X00011 */
		"DMA3",				GENMASK(4, 0)},
	{"DNC5",	BIT(4) | GENMASK(1, 0),		/* X10011 */
		"SYSMMU_D1_DNCFLC",		GENMASK(4, 0)},
	{"DNC5",	BIT(3) | GENMASK(1, 0),		/* XX1011 */
		"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC5",	BIT(2),				/* X00100 */
		"DMA4",				GENMASK(4, 0)},
	{"DNC5",	BIT(4) | BIT(2),		/* X10100 */
		"SYSMMU_D2_DNCDMA",		GENMASK(4, 0)},
	{"DNC5",	BIT(3) | BIT(2),		/* XX1100 */
		"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC5",	BIT(2) | BIT(0),		/* X00101 */
		"DMA5",				GENMASK(4, 0)},
	{"DNC5",	BIT(4) | BIT(2) | BIT(0),	/* X10101 */
		"SYSMMU_D2_DNCFLC",		GENMASK(4, 0)},
	{"DNC5",	BIT(3) | BIT(2) | BIT(0),	/* XX1101 */
		"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(3, 0)},

	{"DNC6",	0,				/* X00000 */
		"DMA0",				GENMASK(4, 0)},
	{"DNC6",	BIT(4),				/* X10000 */
		"SYSMMU_D0_DNCDMA",		GENMASK(4, 0)},
	{"DNC6",	BIT(3),				/* XX1000 */
		"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC6",	BIT(0),				/* X00001 */
		"DMA1",				GENMASK(4, 0)},
	{"DNC6",	BIT(4) | BIT(0),		/* X10001 */
		"SYSMMU_D0_DNCFLC",		GENMASK(4, 0)},
	{"DNC6",	BIT(3) | BIT(0),		/* XX1001 */
		"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC6",	BIT(2),				/* X00010 */
		"DMA2",				GENMASK(4, 0)},
	{"DNC6",	BIT(4) | BIT(1),		/* X10010 */
		"SYSMMU_D1_DNCDMA",		GENMASK(4, 0)},
	{"DNC6",	BIT(3) | BIT(1),		/* XX1010 */
		"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC6",	GENMASK(1, 0),			/* X00011 */
		"DMA3",				GENMASK(4, 0)},
	{"DNC6",	BIT(4) | GENMASK(1, 0),		/* X10011 */
		"SYSMMU_D1_DNCFLC",		GENMASK(4, 0)},
	{"DNC6",	BIT(3) | GENMASK(1, 0),		/* XX1011 */
		"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC6",	BIT(2),				/* X00100 */
		"DMA4",				GENMASK(4, 0)},
	{"DNC6",	BIT(4) | BIT(2),		/* X10100 */
		"SYSMMU_D2_DNCDMA",		GENMASK(4, 0)},
	{"DNC6",	BIT(3) | BIT(2),		/* XX1100 */
		"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC6",	BIT(2) | BIT(0),		/* X00101 */
		"DMA5",				GENMASK(4, 0)},
	{"DNC6",	BIT(4) | BIT(2) | BIT(0),	/* X10101 */
		"SYSMMU_D2_DNCFLC",		GENMASK(4, 0)},
	{"DNC6",	BIT(3) | BIT(2) | BIT(0),	/* XX1101 */
		"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(3, 0)},

	{"DNC7",	0,				/* X00000 */
		"DMA0",				GENMASK(4, 0)},
	{"DNC7",	BIT(4),				/* X10000 */
		"SYSMMU_D0_DNCDMA",		GENMASK(4, 0)},
	{"DNC7",	BIT(3),				/* XX1000 */
		"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC7",	BIT(0),				/* X00001 */
		"DMA1",				GENMASK(4, 0)},
	{"DNC7",	BIT(4) | BIT(0),		/* X10001 */
		"SYSMMU_D0_DNCFLC",		GENMASK(4, 0)},
	{"DNC7",	BIT(3) | BIT(0),		/* XX1001 */
		"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC7",	BIT(2),				/* X00010 */
		"DMA2",				GENMASK(4, 0)},
	{"DNC7",	BIT(4) | BIT(1),		/* X10010 */
		"SYSMMU_D1_DNCDMA",		GENMASK(4, 0)},
	{"DNC7",	BIT(3) | BIT(1),		/* XX1010 */
		"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC7",	GENMASK(1, 0),			/* X00011 */
		"DMA3",				GENMASK(4, 0)},
	{"DNC7",	BIT(4) | GENMASK(1, 0),		/* X10011 */
		"SYSMMU_D1_DNCFLC",		GENMASK(4, 0)},
	{"DNC7",	BIT(3) | GENMASK(1, 0),		/* XX1011 */
		"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(3, 0)},
	{"DNC7",	BIT(2),				/* X00100 */
		"DMA4",				GENMASK(4, 0)},
	{"DNC7",	BIT(4) | BIT(2),		/* X10100 */
		"SYSMMU_D2_DNCDMA",		GENMASK(4, 0)},
	{"DNC7",	BIT(3) | BIT(2),		/* XX1100 */
		"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(3, 0)},
	{"DNC7",	BIT(2) | BIT(0),		/* X00101 */
		"DMA5",				GENMASK(4, 0)},
	{"DNC7",	BIT(4) | BIT(2) | BIT(0),	/* X10101 */
		"SYSMMU_D2_DNCFLC",		GENMASK(4, 0)},
	{"DNC7",	BIT(3) | BIT(2) | BIT(0),	/* XX1101 */
		"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(3, 0)},

	{"DNCC",	0,
		/* XXXXX0 */	"CPU",				BIT(0)},
	{"DNCC",	BIT(0),
		/* 000001 */	"DMA0",				GENMASK(5, 0)},
	{"DNCC",	BIT(5) | BIT(0),
		/* 100001 */	"SYSMMU_D0_DNCDMA",		GENMASK(5, 0)},
	{"DNCC",	BIT(4) | BIT(0),
		/* X10001 */	"SYSMMU_D0_DNCDMA_S2MPU",	GENMASK(4, 0)},
	{"DNCC",	GENMASK(1, 0),
		/* 000011 */	"DMA1",				GENMASK(5, 0)},
	{"DNCC",	BIT(5) | GENMASK(1, 0),
		/* 100011 */	"SYSMMU_D0_DNCFLC",		GENMASK(5, 0)},
	{"DNCC",	BIT(4) | GENMASK(1, 0),
		/* X10011 */	"SYSMMU_D0_DNCFLC_S2MPU",	GENMASK(4, 0)},
	{"DNCC",	BIT(2) | BIT(0),
		/* 000101 */	"DMA2",				GENMASK(5, 0)},
	{"DNCC",	BIT(5) | BIT(2) | BIT(0),
		/* 100101 */	"SYSMMU_D1_DNCDMA",		GENMASK(5, 0)},
	{"DNCC",	BIT(4) | BIT(2) | BIT(0),
		/* X10101 */	"SYSMMU_D1_DNCDMA_S2MPU",	GENMASK(4, 0)},
	{"DNCC",	GENMASK(2, 0),
		/* 000111 */	"DMA3",				GENMASK(5, 0)},
	{"DNCC",	BIT(5) | GENMASK(2, 0),
		/* 100111 */	"SYSMMU_D1_DNCFLC",		GENMASK(5, 0)},
	{"DNCC",	BIT(4) | GENMASK(2, 0),
		/* X10111 */	"SYSMMU_D1_DNCFLC_S2MPU",	GENMASK(4, 0)},
	{"DNCC",	BIT(3) | BIT(0),
		/* 001001 */	"DMA4",				GENMASK(5, 0)},
	{"DNCC",	BIT(5) | BIT(3) | BIT(0),
		/* 101001 */	"SYSMMU_D2_DNCDMA",		GENMASK(5, 0)},
	{"DNCC",	BIT(4) | BIT(3) | BIT(0),
		/* X11001 */	"SYSMMU_D2_DNCDMA_S2MPU",	GENMASK(4, 0)},
	{"DNCC",	BIT(3) | GENMASK(1, 0),
		/* 001011 */	"DMA5",				GENMASK(5, 0)},
	{"DNCC",	BIT(5) | BIT(3) | GENMASK(1, 0),
		/* 101011 */	"SYSMMU_D2_DNCFLC",		GENMASK(5, 0)},
	{"DNCC",	BIT(4) | BIT(3) | GENMASK(1, 0),
		/* X11011 */	"SYSMMU_D2_DNCFLC_S2MPU",	GENMASK(4, 0)},

	{"ALVPS",	0,			/* XXX000 */
		"GREBE",		GENMASK(2, 0)},
	{"ALVPS",	BIT(1),			/* XXX010 */
		"SPDMA",		GENMASK(2, 0)},
	{"ALVPS",	BIT(2),			/* XXX100 */
		"PDMA",			GENMASK(2, 0)},
	{"ALVPS",	BIT(0),			/* XXXXX1 */
		"SYSMMU_BUSMC",		BIT(0)},

	{"SFI",		0,			/* XX0000 */
		"CR52_0",		GENMASK(3, 0)},
	{"SFI",		BIT(1),			/* XX0010 */
		"CR52_1",		GENMASK(3, 0)},
	{"SFI",		BIT(3),			/* XX1000 */
		"SSS",			GENMASK(3, 0)},
	{"SFI",		BIT(0),			/* XXXXX1 */
		"SYSMMU_SFI",		BIT(0)},

	{"AUD",		0,			/* X00000 */
		"SPUS/M",		GENMASK(4, 0)},
	{"AUD",		BIT(2),			/* X00100 */
		"SFT_SPU",		GENMASK(4, 0)},
	{"AUD",		GENMASK(3, 2),		/* X01100 */
		"HIFI4_a",		GENMASK(4, 0)},
	{"AUD",		BIT(4),			/* X10000 */
		"HIFI4_b",		GENMASK(4, 0)},
	{"AUD",		BIT(4) | BIT(2),	/* X10100 */
		"HIFI4_c",		GENMASK(4, 0)},
	{"AUD",		GENMASK(4, 3),		/* X11000 */
		"HIFI4_d",		GENMASK(4, 0)},
	{"AUD",		BIT(1),			/* XXXX10 */
		"SYSMMU_AUD",		GENMASK(1, 0)},
	{"AUD",		BIT(0),			/* XXXXX1 */
		"SYSMMU_AUD_S2MPU",	BIT(0)},

	{"CORESIGHT",	0,			/* 000000 */
		"CSSYS",		GENMASK(5, 0)},
	{"CORESIGHT",	BIT(0),			/* 000001 */
		"ETR",			GENMASK(5, 0)},
	{"CORESIGHT",	BIT(1),			/* 000010 */
		"DBGCORE",		GENMASK(5, 0)},

	{"MMU_G3D",	0,			/* 0XX000 */
		"SYSMMU_G3D1_0",	BIT(5) | GENMASK(2, 0)},
	{"MMU_G3D",	BIT(0),			/* 0XX001 */
		"SYSMMU_G3D1_1",	BIT(5) | GENMASK(2, 0)},
	{"MMU_G3D",	BIT(1),			/* 0XX010 */
		"SYSMMU_G3D1_2",	BIT(5) | GENMASK(2, 0)},
	{"MMU_G3D",	GENMASK(1, 0),		/* 0XX011 */
		"SYSMMU_G3D1_3",	BIT(5) | GENMASK(2, 0)},
	{"MMU_G3D",	BIT(2),			/* 0XX100 */
		"SYSMMU_G3D00",		BIT(5) | GENMASK(2, 0)},
	{"MMU_G3D",	BIT(2) | BIT(0),	/* 0XX101 */
		"SYSMMU_G3D01",		BIT(5) | GENMASK(2, 0)},
};

/* Data Path */
static struct itmon_v1_nodeinfo data_bus_c[] = {
	{M_NODE,	"ACC0",		0x1C133000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"ACC1",		0x1C143000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUM0",	0x1C083000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUM1",	0x1C093000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUM2",	0x1C0A3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUM3",	0x1C0B3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUS00",	0x1C003000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUS01",	0x1C013000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUS02",	0x1C023000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUS03",	0x1C033000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUS10",	0x1C043000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUS11",	0x1C053000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUS12",	0x1C063000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DPUS13",	0x1C073000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"FSYS1",	0x1C173000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"FSYS2",	0x1C183000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"G2D0",		0x1C0C3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"G2D1",		0x1C0D3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"G2D2",		0x1C0E3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"ISPB0",	0x1C113000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"ISPB1",	0x1C123000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"MFC0",		0x1C153000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"MFC1",		0x1C163000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"SECU",		0x1C193000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"SIREX",	0x1C1A3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"TAA0",		0x1C0F3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"TAA1",		0x1C103000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSC_DP",	0x1C233000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSC_S0",	0x1C1B3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSC_S1",	0x1C1C3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSC_S2",	0x1C1D3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSC_S3",	0x1C1E3000, NULL, 0,
		false, false, true, true, false, NULL},
	/* extend */
	{T_S_NODE,	"BUSC_S4",	0x1C1F3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSC_S5",	0x1C203000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSC_S6",	0x1C213000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSC_S7",	0x1C223000, NULL, 0,
		false, false, true, true, false, NULL},
};

static struct itmon_v1_nodeinfo data0_bus_mc[] = {
	{M_NODE,	"ALVPS",	0x1CD43000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"AUD",		0x1CD63000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_DP",	0x1CC83000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_M0",	0x1CC03000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_M1",	0x1CC13000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_M2",	0x1CC23000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_M3",	0x1CC33000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_M4",	0x1CC43000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_M5",	0x1CC53000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_M6",	0x1CC63000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSC_M7",	0x1CC73000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNC0",		0x1CC93000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNC1",		0x1CCA3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNC2",		0x1CCB3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNC3",		0x1CCC3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNC4",		0x1CCD3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNC5",		0x1CCE3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNC6",		0x1CCF3000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNC7",		0x1CD03000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"DNCC",		0x1CD13000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"FSYS00",	0x1CD23000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"FSYS01",	0x1CD33000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"SFI",		0x1CD53000, NULL, 0,
		false, false, true, true, false, NULL},
	{S_NODE,	"BUSMC_DP",	0x1CDF3000, NULL, V1_TMOUT,
		true, false, true, true, false, NULL},
	{T_S_NODE,	"BUSMC_S0",	0x1CD73000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSMC_S1",	0x1CD83000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSMC_S2",	0x1CD93000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSMC_S3",	0x1CDA3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSMC_S4",	0x1CDB3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSMC_S5",	0x1CDC3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSMC_S6",	0x1CDD3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"BUSMC_S7",	0x1CDE3000, NULL, 0,
		false, false, true, true, false, NULL},
};

static struct itmon_v1_nodeinfo data1_bus_mc[] = {
	{T_M_NODE,	"BUSMC_M0",	0x1F003000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSMC_M1",	0x1F013000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSMC_M2",	0x1F023000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSMC_M3",	0x1F033000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSMC_M4",	0x1F043000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSMC_M5",	0x1F053000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSMC_M6",	0x1F063000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"BUSMC_M7",	0x1F073000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"CORE_M0",	0x1F083000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"CORE_M1",	0x1F093000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"CORE_M2",	0x1F0A3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"CORE_M3",	0x1F0B3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"CORE_M4",	0x1F0C3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"CORE_M5",	0x1F0D3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"CORE_M6",	0x1F0E3000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_S_NODE,	"CORE_M7",	0x1F0F3000, NULL, 0,
		false, false, true, true, false, NULL},
};

static struct itmon_v1_nodeinfo data_core[] = {
	{M_NODE,	"CORESIGHT",	0x1B883000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"CORE_M0",	0x1B803000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"CORE_M1",	0x1B813000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"CORE_M2",	0x1B823000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"CORE_M3",	0x1B833000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"CORE_M4",	0x1B843000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"CORE_M5",	0x1B853000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"CORE_M6",	0x1B863000, NULL, 0,
		false, false, true, true, false, NULL},
	{T_M_NODE,	"CORE_M7",	0x1B873000, NULL, 0,
		false, false, true, true, false, NULL},
	{M_NODE,	"MMU_G3D",	0x1B893000, NULL, 0,
		false, false, true, true, false, NULL},
	{S_NODE,	"CORE_DP",	0x1B8E3000, NULL, V1_TMOUT,
		true, false, true, true, false, NULL},
	{S_NODE,	"DREX_IRPS0",	0x1B8A3000, NULL, 0,
		false, false, true, true, false, NULL},
	{S_NODE,	"DREX_IRPS1",	0x1B8B3000, NULL, 0,
		false, false, true, true, false, NULL},
	{S_NODE,	"DREX_IRPS2",	0x1B8C3000, NULL, 0,
		false, false, true, true, false, NULL},
	{S_NODE,	"DREX_IRPS3",	0x1B8D3000, NULL, 0,
		false, false, true, true, false, NULL},
};

static struct itmon_v1_nodeinfo peri0_core[] = {
	{M_NODE,	"DCORE_P0CORE",		0x1BA53000, NULL,
		0, false, false, true, true, false, NULL},
	{M_NODE,	"SCI_CCM0",		0x1BA23000, NULL,
		0, false, false, true, true, false, NULL},
	{M_NODE,	"SCI_CCM1",		0x1BA33000, NULL,
		0, false, false, true, true, false, NULL},
	{M_NODE,	"SCI_IRPM",		0x1BA43000, NULL,
		0, false, false, true, true, false, NULL},
	{T_S_NODE,	"P0CORE_P1CORE",	0x1BA03000, NULL,
		0, false, false, true, true, false, NULL},
	{T_S_NODE,	"P0CORE_PBUSMC",	0x1BA13000, NULL,
		0, false, false, true, true, false, NULL},
};

static struct itmon_v1_nodeinfo peri1_core[] = {
	{T_M_NODE,	"P0CORE_P1CORE",	0x1BC03000, NULL,
		0,	false, false, true, true, false, NULL},
	{T_M_NODE,	"PBUSMC_P1CORE",	0x1BC13000, NULL,
		0,	false, false, true, true, false, NULL},
	{S_NODE,	"ALIVE",		0x1BC93000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"CPUCL0",		0x1BC43000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"CPUCL1",		0x1BC53000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"G3D00",		0x1BC63000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"G3D01",		0x1BC73000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"G3D1",			0x1BC83000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"SFR_CORE",		0x1BC33000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"SFR_TREX_CORE",	0x1BC23000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
};

static struct itmon_v1_nodeinfo peri_bus_mc[] = {
	{M_NODE,	"DBUSMC_PBUSMC",	0x1CA53000, NULL,
		0,	false, false, true, true, false, NULL},
	{T_M_NODE,	"P0CORE_PBUSMC",	0x1CA63000, NULL,
		0,	false, false, true, true, false, NULL},
	{S_NODE,	"ACC",			0x1C943000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"AUD",			0x1C9C3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"BUSC_SFR0",		0x1C813000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"BUSC_SFR1",		0x1C823000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"BUSMC",		0x1C833000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"DNC",			0x1C953000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"DNCSRAM",		0x1CA43000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"DPTX",			0x1C8F3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"DPUM",			0x1C8C3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"DPUS0",		0x1C8D3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"DPUS1",		0x1C8E3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"FSYS0",		0x1C9D3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"FSYS1",		0x1C9E3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"FSYS2",		0x1C9F3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"G2D",			0x1C9B3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"ISPB0",		0x1C903000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"ISPB1",		0x1C913000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"MFC",			0x1C9A3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"MIF0",			0x1C853000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"MIF1",			0x1C863000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"MIF2",			0x1C873000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"MIF3",			0x1C883000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"NPU00",		0x1C963000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"NPU01",		0x1C973000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"NPU10",		0x1C983000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"NPU11",		0x1C993000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{T_S_NODE,	"PBUSMC_P1CORE",	0x1C843000, NULL,
		0,	false, false, true, true, false, NULL},
	{S_NODE,	"PCIE_0",		0x1CA23000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"PCIE_1",		0x1CA33000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"PERIC0",		0x1C8A3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	/* extend */
	{S_NODE,	"PERIC1",		0x1C8B3000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"PERIS",		0x1C893000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"SFI",			0x1CA03000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"SIREX",		0x1CA13000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"TAA0",			0x1C923000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"TAA1",			0x1C933000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
	{S_NODE,	"TREX_BUSC_SFR",	0x1C803000, NULL,
		V1_TMOUT, true, false, true, true, false, NULL},
};

static struct itmon_v1_nodegroup nodegroup[] = {
	/* Data Path */
	{0, "DATA_BUS_C",	0x1C253000, true,  NULL,
		data_bus_c,	ARRAY_SIZE(data_bus_c),		V1_BUS_DATA},
	{0, "DATA0_BUS_MC",	0x1CE13000, false, NULL,
		data0_bus_mc,	ARRAY_SIZE(data0_bus_mc),	V1_BUS_DATA},
	{0, "DATA1_BUS_MC",	0x1F113000, false, NULL,
		data1_bus_mc,	ARRAY_SIZE(data1_bus_mc),	V1_BUS_DATA},
	{0, "DATA_CORE",	0x1B903000, false, NULL,
		data_core,	ARRAY_SIZE(data_core),		V1_BUS_DATA},
	/* Peri Path */
	{0, "PERI0_CORE",	0x1BA83000, false, NULL,
		peri0_core,	ARRAY_SIZE(peri0_core),		V1_BUS_PERI},
	{0, "PERI1_CORE",	0x1BCB3000, false, NULL,
		peri1_core,	ARRAY_SIZE(peri1_core),		V1_BUS_PERI},
	{0, "PERI_BUS_MC",	0x1CA93000, true,  NULL,
		peri_bus_mc,	ARRAY_SIZE(peri_bus_mc),	V1_BUS_PERI},
};

struct itmon_v1_info_table exynosautov9_itmon_v1_info = {
	.masterinfo = masterinfo,
	.num_masterinfo = (int)ARRAY_SIZE(masterinfo),

	.rpathinfo = rpathinfo,
	.num_rpathinfo = (int)ARRAY_SIZE(rpathinfo),

	.nodegroup = nodegroup,
	.num_nodegroup = (int)ARRAY_SIZE(nodegroup),
};

struct itmon_info_table exynosautov9_itmon_info = {
	.ops = &itmon_v1_ops,
	.info_table = &exynosautov9_itmon_v1_info,
};
