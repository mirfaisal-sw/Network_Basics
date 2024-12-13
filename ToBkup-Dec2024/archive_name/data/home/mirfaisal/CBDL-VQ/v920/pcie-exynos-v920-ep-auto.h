/* SPDX-License-Identifier: GPL-2.0 */
/**
 * pcie-exynos-ep-auto.h - Exynos PCIe EP driver
 *
 * Copyright (c) 2013-2020 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Authors: Sanghoon Bae <sh86.bae@samsung.com>
 *	    Jiheon Oh <jiheon.oh@samsung.com>
 *
 */

#include <linux/pci-epf.h>
#include <linux/dma-buf.h>
#include <linux/iommu.h>
#include <linux/dma-heap.h>
#include <soc/samsung/exynos-scd-external-def.h>
#include <soc/samsung/pcie-exynos-v920-ep-auto-external.h>

#define EXYNOS_PCIE_EP_DBG_MSI	0

#define DMA_CONT_MODE_EP	0
#define	DMA_LOG_PRINT_EP	0

#define to_exynos_pcie_ep(x)	dev_get_drvdata((x)->dev)

/* Exynos920 Auto */
#define MAX_EP_NUM		(4)
#define CLASS_ID		(PCI_CLASS_SYSTEM_OTHER)
#define REVISION_ID		(0x01)
#define MAX_VF_NUM		4
#define CH_SHARE_PHY		2
#define PHY_TIMEOUT		2000
#define EP_LINKUP_WAIT		50000
#define EXP_CNT			10000
#define LTSSM_EN		1
#define LTSSM_DIS		0

#define MSIX_ADDR		0x10450040
/* Register address builder */
#define PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(region) \
		((region) << 9)

#define MAX_TEST_WR_SIZE	0x1000000
#define WR_DMA_TEST_ADDR	0xDD000000
#define RD_DMA_TEST_ADDR	0xDE000000

/* BAR */
#define BAR0			0x10
#define BAR1			0x14
#define BAR2			0x18
#define BAR3			0x1C
#define BAR4			0x20
#define BAR5			0x24

/* SUB Controller */
#define PCIE_GEN_CTRL_3		0x8
#define PCIE_LTSSM_EN		0x1
#define PCIE_LINK_DBG_2		0x304
#define PCIE_LINK_LTSSM_MASK	0x1f
#define PCIE_LINK_CDM_IN_RESET_MASK	24
#define PCIE_SRIS_MODE		(0x1 << 8)
#define PCIE_PM_STS		0x034
#define PCIE_INT_STS		0x05C
#define PCIE_RX_MSG_INT_STS	0x080
#define PCIE_RX_MSG_INT_EN	0x090
#define EVT2_PCIE_MISC_INT_STS	0x08C
#define EVT2_PCIE_MISC_INT_EN	0x09C
#define EVT01_PCIE_MISC_INT_STS	0x088
#define EVT01_PCIE_MISC_INT_EN	0x098

/* SUB Controller General */
#define GENERAL_SS_RST_CTRL_1			0x48
#define GENERAL_RST_PE0_SOFT_COLD_RESET		0x1
#define GENERAL_RST_PE0_SOFT_WARM_PHY_RESET	(0x3<<1)
#define GENERAL_RST_PE1_SOFT_COLD_RESET		(0x1<<8)
#define GENERAL_RST_PE1_SOFT_WARM_PHY_RESET	(0x3<<9)

/* phy */
#define PCIE_PHY_RTUNE_REQ		0x10000001
#define PCIE_PHY0_GEN_CTRL_1		0x10
#define PCIE_PHY0_REFA_CLK_SEL_MASK	(0x3<<16)
#define PCIE_PHY0_REFA_CLK_SEL_PAD	(0x1<<16)
#define PCIE_PHY0_REFB_CLK_SEL_MASK	(0x3<<18)
#define PCIE_PHY0_REFB_CLK_SEL_PAD	(0x1<<18)
#define PCIE_PHY0_PHY0_SRAM_BYPASS	(0x1<<10)
#define PCIE_PHY0_PHY0_SRAM_EXT_LD_DONE	(0x1<<11)
#define PCIE_PHY0_REFA_B_ALT1		0x061A0060
#define PCIE_PHY0_REFA_B_ALT0		0x06100060
#define PCIE_PHY0_REFA_B_PAD		0x06150060
#define PCIE_PHY0_SRAM_INIT_DONE_BIT	31

#define PCIE_PHY_EXT_TX_ROPLL_POSTDIV_CTRL		0x1A8
#define PCIE_PHY_ROPLL_POSTDIV_VAL			0x1249
#define PCIE_PHY_EXT_TX_ROPLL_POSTDIV_OVRD_EN_CTRL	0x1C4
#define PCIE_PHY_ROPLL_POSTDIV_OVRD_EN_VAL		0xF
#define PCIE_PIPE_LANEX_LANEPLL_BYPASS_MODE_CTRL_1	0x384
#define PCIE_PIPE_BYPASS_MODE_CTRL_VAL1			0x00
#define PCIE_PIPE_BYPASS_MODE_CTRL_VAL2			0x000


/* channel define */
#define CH0_4L			0
#define CH1_4L			1
#define CH2_2L			2
#define CH3_2L			3

/* pmu */
#define PCIE_PHY_4L_CONFIGURATION			0x700
#define PCIE_PHY_2L_CONFIGURATION			0x704
#define PCIE_PHY_CFG_EN_PHY				0x1

/* sysreg */
#define HSI0_PLL_REG0					0x600
#define HSI0_PLL_FOUTEN					(0x1 << 8)
#define HSI0_PLL_REG1					0x604
#define HSI0_PLL_FOUTPOSTDIVEN				(0x1)
#define HSI0_PLL_REG2					0x608
#define HSI0_PLL_PLLEN					(0x1 << 24)
#define HSI0_CLKBUF0_REG0				0x620
#define HSI0_CLKBUF1_REG0				0x630
#define HSI0_CLKBUF2_REG0				0x640
#define HSI0_CLKBUF3_REG0				0x650
#define HSI0_CLKBUF_IMP_CTRL				(0x1 << 0)

#define HSI0_PCIE_GEN5_PHY_PWRDWN_4L			0x670
#define HSI0_PCIE_GEN5_PHY_PWRDWN_2L			0x4
#define HSI0_PCIE_PHY_TEST_PWRDWN_MSK			0x1
#define HSI0_PCIE_PHY_TEST_PWRDWN_EN			0x1

#define HSI0_PCIE_GEN5_4LA_PHY_CTRL			0x828
#define HSI0_PCIE_GEN5_4LB_PHY_CTRL			0x848
#define HSI0_PCIE_GEN5_2LA_PHY_CTRL			0x868
#define HSI0_PCIE_GEN5_2LB_PHY_CTRL			0x888

#define HSI0_PCIE_IP_CTRL_PERST_PAD			(0x3 << 4)
#define HSI0_PCIE_IP_CTRL_PERST_OVERRIDE		(0x1 << 4)
#define HSI0_PCIE_IP_CTRL_SS_MODE			(0x1 << 28)
#define HSI0_PCIE_IP_CTRL_PERST_MASK			0xf0
#define HSI0_PCIE_IP_CTRL_PERST_INIT			(0x1 << 4)
#define HSI0_PCIE_IP_CTRL_PERST_IN			(0x3 << 4)

#define PCIE_EP_SYSREG_SHARABILITY_CTRL			0x700
#define PCIE_EP_SYSREG_SHARABLE_RW_OFFSET		0x10
#define PCIE_EP_SYSREG_SHARABLE_CH_OFFSET		0x4
#define PCIE_EP_SYSREG_SHARABLE_RW_ENABLE		0xC

/*	SOC Control	*/
#define PCIE_REFCLK_CTRL_SOC_OPTION_0			0x200
#define PCIE_REFCLK_OPTION0_DEFAULT			0x3F0
#define PCIE_REFCLK_OPTION0_PLLEN			0x1
#define PCIE_REFCLK_OPTION0_PLLEN_PLL_LOCK		0x5
#define PCIE_REFCLK_OPTION0_DEVICETYPE_EP		0x10000

#define PCIE_REFCLK_CTRL_SOC_OPTION_1			0x204
#define PCIE_REFCLK_OPTION1_DEFAULT			0x30000
#define PCIE_REFCLK_OPTION1_CG_PHY			0xC00

/* irq 0 RX_MSG_INT_STS */
#define PM_TURNOFF					(0x1 << 19)

/* irq 1 PCIE_MISC_INT_STS */
#define EVT2_PERST_N_1					(0x1 << 2)
#define EVT2_LINK_DOWN					(0x1 << 3)
#define EVT2_PERST_N_0					(0x1 << 8)
#define EVT2_PM_IN_L2_RISE				(0x1 << 10)

#define EVT01_PERST_N_0					(0x1 << 7)
#define EVT01_LINK_DOWN					(0x1 << 10)
#define EVT01_PERST_N_1					(0x1 << 11)
#define EVT01_PM_IN_L2_RISE				(0x1 << 13)

/* DMA_INT */
#define WR_CH0		(0x1 << 16)
#define WR_CH1		(0x1 << 17)
#define WR_CH2		(0x1 << 18)
#define WR_CH3		(0x1 << 19)
#define WR_CH4		(0x1 << 20)
#define WR_CH5		(0x1 << 21)
#define WR_CH6		(0x1 << 22)
#define WR_CH7		(0x1 << 23)
#define RD_CH0		(0x1 << 24)
#define RD_CH1		(0x1 << 25)
#define RD_CH2		(0x1 << 26)
#define RD_CH3		(0x1 << 27)
#define RD_CH4		(0x1 << 28)
#define RD_CH5		(0x1 << 29)
#define RD_CH6		(0x1 << 30)
#define RD_CH7		(0x1 << 31)

#define DMA_WR_CH0		0
#define DMA_WR_CH1		1
#define DMA_WR_CH2		2
#define DMA_WR_CH3		3
#define DMA_WR_CH4		4
#define DMA_WR_CH5		5
#define DMA_WR_CH6		6
#define DMA_WR_CH7		7
#define DMA_RD_CH0		0
#define DMA_RD_CH1		1
#define DMA_RD_CH2		2
#define DMA_RD_CH3		3
#define DMA_RD_CH4		4
#define DMA_RD_CH5		5
#define DMA_RD_CH6		6
#define DMA_RD_CH7		7

// PCIE Capability Register fields
#define CAP_REGSHIFT_VERSION		0
#define CAP_REGMASK_VERSION		(0xF << CAP_REGSHIFT_VERSION)
#define CAP_REGSHIFT_PORTTYPE		4
#define CAP_REGMASK_PORTTYPE		(0xF << CAP_REGSHIFT_PORTTYPE)
#define CAP_REGSHIFT_SLOTIMP		8
#define CAP_REGMASK_SLOTIMP		(1 << CAP_REGSHIFT_SLOTIMP)
#define CAP_REGSHIFT_INTRNUM		9
#define CAP_REGMASK_INTRNUM		(0xF << CAP_REGSHIFT_INTRNUM)
#define CAP_MC_REGSHIFT_64BIT		7
#define CAP_MC_REGMASK_64BIT		(1 << CAP_MC_REGSHIFT_64BIT)
#define CAP_MC_REGSHIFT_MSG		7

#define RC_REGSHIFT_VENDOR_ID		0
#define RC_REGMASK_VENDOR_ID		(0xFFFF << RC_REGSHIFT_VENDOR_ID)
#define RC_REGSHIFT_DEVICE_ID		16
#define RC_REGMASK_DEVICE_ID		(0xFFFF << RC_REGSHIFT_DEVICE_ID)
#define RC_REGSHIFT_SCRAMBLE		1
#define RC_REGMASK_SCRAMBLE		(1 << RC_REGSHIFT_SCRAMBLE)
#define RC_REGSHIFT_LINK_MODE		16
#define RC_REGMASK_LINK_MODE		(0x3F << RC_REGSHIFT_LINK_MODE)
#define RC_REGSHIFT_LINK_WIDTH		8
#define RC_REGMASK_LINK_WIDTH		(0x1FF << RC_REGSHIFT_LINK_WIDTH)
#define RC_REGSHIFT_DBI_ACCESS		21
#define RC_REGSHIFT_TLP_TYPE		0
#define RC_REGSHIFT_LINK_NUM		0
#define RC_REGMASK_LINK_NUM		(0xF << RC_REGSHIFT_LINK_NUM)
#define RC_REGSHIFT_LINK_COMMAND	16
#define RC_REGMASK_LINK_COMMAND		(0x1F << RC_REGSHIFT_LINK_COMMAND)
#define RC_REGSHIFT_FORCE_LINK		15
#define RC_REGMASK_FORCE_LINK		(0x1 << RC_REGSHIFT_LINK_COMMAND)
#define RC_REGSHIFT_IO_EN		0
#define RC_REGMASK_IO_EN		(0x1 << RC_REGSHIFT_IO_EN)
#define RC_REGSHIFT_MEM_EN		1
#define RC_REGMASK_MEM_EN		(0x1 << RC_REGSHIFT_MEM_EN)
#define RC_REGSHIFT_BUS_MASTER_EN	2
#define RC_REGMASK_BUS_MASTER_EN	(0x1 << RC_REGSHIFT_BUS_MASTER_EN)
#define RC_REGSHIFT_SERR_EN		8
#define RC_REGMASK_SERR_EN		(0x1 << RC_REGSHIFT_SERR_EN)
#define RC_REGSHIFT_PARR_ERR_EN		6
#define RC_REGMASK_PARR_ERR_EN		(0x1 << RC_REGSHIFT_PARR_ERR_EN)
#define RC_REGSHIFT_DIRECTION		31
#define RC_REGSHIFT_SPEED_CHANGE	17
#define RC_REGMASK_SPEED_CHANGE		(0x1 << RC_REGSHIFT_SPEED_CHANGE)
#define RC_REGSHIFT_CORRECTABLE_ERR_EN	0
#define RC_REGMASK_CORRECTABLE_ERR_EN	(0x1 << RC_REGSHIFT_CORRECTABLE_ERR_EN)
#define RC_REGSHIFT_NONFATAL_ERR_EN	1
#define RC_REGMASK_NONFATAL_ERR_EN	(0x1 << RC_REGSHIFT_NONFATAL_ERR_EN)
#define RC_REGSHIFT_FATAL_ERR_EN	2
#define RC_REGMASK_FATAL_ERR_EN		(0x1 << RC_REGSHIFT_FATAL_ERR_EN)
#define RC_REGSHIFT_ERR_COR_RCV		0
#define RC_REGMASK_ERR_COR_RCV		(0x1 << RC_REGSHIFT_ERR_COR_RCV)
#define RC_REGSHIFT_ERR_MCOR_RCV	1
#define RC_REGMASK_ERR_MCOR_RCV		(0x1 << RC_REGSHIFT_ERR_MCOR_RCV)
#define RC_REGSHIFT_ERR_FAT_RCV		2
#define RC_REGMASK_ERR_FAT_RCV		(0x1 << RC_REGSHIFT_ERR_FAT_RCV)
#define RC_REGSHIFT_ERR_MFAT_RCV	3
#define RC_REGMASK_ERR_MFAT_RCV		(0x1 << RC_REGSHIFT_ERR_MFAT_RCV)
#define RC_REGSHIFT_FIRST_FAT_RCV	4
#define RC_REGMASK_FIRST_FAT_RCV	(0x1 << RC_REGSHIFT_FIRST_FAT_RCV)
#define RC_REGSHIFT_NON_FAT_RCV		5
#define RC_REGMASK_NON_FAT_RCV		(0x1 << RC_REGSHIFT_NON_FAT_RCV)
#define RC_REGSHIFT_MSG_FAT_RCV		6
#define RC_REGMASK_MSG_FAT_RCV		(0x1 << RC_REGSHIFT_MSG_FAT_RCV)
#define RC_REGSHIFT_RCV_ERR_STATUS	0
#define RC_REGMASK_RCV_ERR_STATUS	(0x1 << RC_REGSHIFT_RCV_ERR_STATUS)
#define RC_REGSHIFT_BAD_TLP		6
#define RC_REGMASK_BAD_TLP		(0x1 << RC_REGSHIFT_BAD_TLP)
#define RC_REGSHIFT_BAD_DLLP		7
#define RC_REGMASK_BAD_DLLP		(0x1 << RC_REGSHIFT_BAD_DLLP)
#define RC_REGSHIFT_RN_ROLLOVER		8
#define RC_REGMASK_RN_ROLLOVER		(0x1 << RC_REGSHIFT_RN_ROLLOVER)
#define RC_REGSHIFT_TIMEOUT		12
#define RC_REGMASK_TIMEOUT		(0x1 << RC_REGSHIFT_TIMEOUT)
#define RC_REGSHIFT_NONFATAL		13
#define RC_REGMASK_NONFATAL		(0x1 << RC_REGSHIFT_NONFATAL)
#define RC_REGSHIFT_CORINT		14
#define RC_REGMASK_CORINT		(0x1 << RC_REGSHIFT_CORINT)
#define RC_REGSHIFT_LINKERR		4
#define RC_REGMASK_LINKERR		(0x1 << RC_REGSHIFT_LINKERR)
#define RC_REGSHIFT_POISONED		12
#define RC_REGMASK_POISONED		(0x1 << RC_REGSHIFT_POISONED)
#define RC_REGSHIFT_FLOWCONTROL		13
#define RC_REGMASK_FLOWCONTROL		(0x1 << RC_REGSHIFT_FLOWCONTROL)
#define RC_REGSHIFT_CTIMEOUT		14
#define RC_REGMASK_CTIMEOUT		(0x1 << RC_REGSHIFT_CTIMEOUT)
#define RC_REGSHIFT_CABORT		15
#define RC_REGMASK_CABORT		(0x1 << RC_REGSHIFT_CABORT)
#define RC_REGSHIFT_UNEXPECTEDC		16
#define RC_REGMASK_UNEXPECTEDC		(0x1 << RC_REGSHIFT_UNEXPECTEDC)
#define RC_REGSHIFT_ROVERFLOW		17
#define RC_REGMASK_ROVERFLOW		(0x1 << RC_REGSHIFT_ROVERFLOW)
#define RC_REGSHIFT_MALFORMEDTLP	18
#define RC_REGMASK_MALFORMEDTLP		(0x1 << RC_REGSHIFT_MALFORMEDTLP)
#define RC_REGSHIFT_ECRC_ERR		19
#define RC_REGMASK_ECRC_ERR		(0x1 << RC_REGSHIFT_ECRC_ERR)
#define RC_REGSHIFT_UR_ERR		20
#define RC_REGMASK_UR_ERR		(0x1 << RC_REGSHIFT_UR_ERR)
#define RC_REGSHIFT_UCI_ERR		22
#define RC_REGMASK_UCI_ERR		(0x1 << RC_REGSHIFT_UCI_ERR)
#define RC_REGSHIFT_ATOMIC_BLOCKED	24
#define RC_REGMASK_ATOMIC_BLOCKED	(0x1 << RC_REGSHIFT_ATOMIC_BLOCKED)
#define RC_REGSHIFT_TLPBLOCKED_ERR	25
#define RC_REGMASK_TLPBLOCKED_ERR	(0x1 << RC_REGSHIFT_TLPBLOCKED_ERR)
#define RC_REGSHIFT_CAP_LIST_EXIST	20
#define RC_REGMASK_CAP_LIST_EXIST	(0x1 << RC_REGSHIFT_CAP_LIST_EXIST)
#define RC_REGSHIFT_DMA_H_ERR_UR	0
#define RC_REGMASK_DMA_H_ERR_UR		(0xFF << RC_REGSHIFT_DMA_H_ERR_UR)
#define RC_REGSHIFT_DMA_H_ERR_CA	8
#define RC_REGMASK_DMA_H_ERR_CA		(0xFF << RC_REGSHIFT_DMA_H_ERR_CA)
#define RC_REGSHIFT_DMA_H_ERR_CTO	16
#define RC_REGMASK_DMA_H_ERR_CTO	 (0xFF << RC_REGSHIFT_DMA_H_ERR_CTO)
#define RC_REGSHIFT_DMA_H_ERR_DP	24
#define RC_REGMASK_DMA_H_ERR_DP		(0xFF << RC_REGSHIFT_DMA_H_ERR_DP)
#define RC_REGSHIFT_DMA_L_ERR_AWED	0
#define RC_REGMASK_DMA_L_ERR_AWED	(0xFF << RC_REGSHIFT_DMA_L_ERR_AWED)
#define RC_REGSHIFT_DMA_L_ERR_LLEFED	16
#define RC_REGMASK_DMA_L_ERR_LLEFED	(0xFF << RC_REGSHIFT_DMA_L_ERR_LLEFED)
#define RC_REGSHIFT_DMA_W_ERR_AWED	0
#define RC_REGMASK_DMA_W_ERR_AWED	(0xFF << RC_REGSHIFT_DMA_W_ERR_AWED)
#define RC_REGSHIFT_DMA_W_ERR_LLEFED	16
#define RC_REGMASK_DMA_W_ERR_LLEFED	(0xFF << RC_REGSHIFT_DMA_W_ERR_LLEFED)
#define RC_REGSHIFT_TARGET_LINK_SPEED	0
#define RC_REGMASK_TARGET_LINK_SPEED	(0xF << RC_REGSHIFT_TARGET_LINK_SPEED)
#define RC_REGSHIFT_TRANSMIT_MARGIN	0
#define RC_REGMASK_TRANSMIT_MARGIN	(0x7 << RC_REGSHIFT_TRANSMIT_MARGIN)
#define RC_REGSHIFT_COMPLIANCE		4
#define RC_REGMASK_COMPLIANCE		(0x1 << RC_REGSHIFT_COMPLIANCE)
#define RC_REGSHIFT_ASPM		0
#define RC_REGMASK_ASPM			(0x3 << RC_REGSHIFT_ASPM)
#define RC_REGSHIFT_ASPM_ENTER_L1	30
#define RC_REGMASK_ASPM_ENTER_L1	(0x1 << RC_REGSHIFT_ASPM_ENTER_L1)
#define RC_REGSHIFT_RETRAIN		5
#define RC_REGMASK_RETRAIN		(0x1 << RC_REGSHIFT_RETRAIN)
#define RC_REGSHIFT_MAX_PAYLOAD		5
#define RC_REGMASK_MAX_PAYLOAD		(0x7 << RC_REGSHIFT_MAX_PAYLOAD)
#define RC_REGSHIFT_MAX_READREQ		12
#define RC_REGMASK_MAX_READREQ		(0x7 << RC_REGSHIFT_MAX_READREQ)
#define RC_REGSHIFT_SECONDARY_BUS_RESET	22
#define RC_REGMASK_SECONDARY_BUS_RESET	(1 << RC_REGSHIFT_SECONDARY_BUS_RESET)
#define RC_REGSHIFT_CB			0
#define RC_REGMASK_CB			(1 << RC_REGSHIFT_CB)
#define RC_REGSHIFT_TCB			1
#define RC_REGMASK_TCB			(1 << RC_REGSHIFT_TCB)
#define RC_REGSHIFT_LLP			2
#define RC_REGMASK_LLP			(1 << RC_REGSHIFT_LLP)
#define RC_REGSHIFT_LIE			3
#define RC_REGMASK_LIE			(1 << RC_REGSHIFT_LIE)
#define RC_REGSHIFT_RIE			4
#define RC_REGMASK_RIE			(1 << RC_REGSHIFT_RIE)
#define RC_REGSHIFT_CS			5
#define RC_REGMASK_CS			(3 << RC_REGSHIFT_CS)
#define RC_REGSHIFT_DMA_STOP		31
#define RC_REGMASK_DMA_STOP		(1 << RC_REGSHIFT_DMA_STOP)
#define RC_REGSHIFT_DBI_RO_WR_EN	0
#define RC_REGMASK_DBI_RO_WR_EN		(1 << RC_REGSHIFT_DBI_RO_WR_EN)

#define XMU_PCIE_POWER_DOMAIN_NO	6
#define CSR_REGSHIFT_CORE_RST_STATUS	1
#define CSR_REGMASK_CORE_RST_STATUS	(1 << CSR_REGSHIFT_CORE_RST_STATUS)

#define CAP_REGSHIFT_D1_SUPPORT		9
#define CAP_REGMASK_D1_SUPPORT		(1 << CAP_REGSHIFT_D1_SUPPORT)
#define CAP_REGSHIFT_POWERSTATE		0
#define CAP_REGMASK_POWERSTATE		(3 << CAP_REGSHIFT_POWERSTATE)
// PCIE SPACE REGISTERS
///////////////////////////////////////////////////////////////////////////////
//  PCIe Basic Design Maps
///////////////////////////////////////////////////////////////////////////////
#define CFG_TYPE1_HDR			0x00	//`defineCFG_TYPE1_HDR 8'h00
#define CFG_PM_CAP			0x40	//`define CFG_PM_CAP 8'h40
#define CFG_MSI_CAP			0x50	//`define CFG_PM_CAP 8'h40
#define MULTIPLE_MSG_CAP_MASK		((0x7)<<17)
#define MULTIPLE_MSG_CAP_32		((0x5)<<17)
#define CFG_PCIE_CAP			0x70	//`define CFG_PCIE_CAP 8'h70
#define CFG_AER_CAP			0x100	//`define CFG_AER_CAP 8'h100
#define CFG_L1SUB_CAP			0x150	//`define CFG_L1SUB_CAP 8'h150
#define	PORTLOG_OFFSET			0x0700
#define	BARMASK_OFFSET			0x1000
#define	ATU_OFFSET			0x4000
#define PORTLOG_OFFSET_GEN3		0x600000 //Gen3 @KITT
//---------------------------------------------------------------------------
// PCI_CONFIG_HEADER
//---------------------------------------------------------------------------
#define	DBI_DEV_VENDOR_ID		(CFG_TYPE1_HDR+0x00)
#define DBI_VENDOR_ID_MASK		0x0000FFFF
#define	DBI_STATUS_COMMAND		(CFG_TYPE1_HDR+0x04)
#define	DBI_COMMAND_MASK		0xffff0000
#define	DBI_CLASS_CODE_REV_ID		(CFG_TYPE1_HDR+0x08)
#define	DBI_BIST_HDRTYPE_LAT_CACHELINESIZE	(CFG_TYPE1_HDR+0x0C)
#define	DBI_BAR0	                (CFG_TYPE1_HDR+0x10)
#define	DBI_BAR1	                (CFG_TYPE1_HDR+0x14)
#define	DBI_PRIM_SEC_BUS		(CFG_TYPE1_HDR+0x18)
#define	DBI_SEC_LAT_TIMER_MASK		0xff000000
#define	DBI_SUB_BUS_FF_SEC_BUS_1	0x00ff0100
#define	DBI_BAR_IO_LIMIT_BASE		(CFG_TYPE1_HDR+0x1C)
#define	DBI_BAR_MEM_LIMIT_BASE		(CFG_TYPE1_HDR+0x20)
#define	DBI_BAR_PREF_MEM_LIMIT_BASE	(CFG_TYPE1_HDR+0x24)
#define	DBI_BAR_PREF_BASE_UPPER		(CFG_TYPE1_HDR+0x28)
#define	DBI_BAR_PREF_LIMIT_UPPER	(CFG_TYPE1_HDR+0x2C)
#define	DBI_BAR_IO_LIMIT_BASE_UPPER	(CFG_TYPE1_HDR+0x30)
#define DBI_CAP_PTR			(CFG_TYPE1_HDR+0x34)
#define DBI_EXP_ROM_BASE		(CFG_TYPE1_HDR+0x38)
#define DBI_BRIDGE_CTRL_INT_PIN		(CFG_TYPE1_HDR+0x3C)
#define DBI_INT_PIN_MASK		0xffff00ff
#define DBI_INT_PIN_1			0x00000100
//---------------------------------------------------------------------------
// PCIE CAPABILITY STRUCTURE
//---------------------------------------------------------------------------
#define	PCIE_CAP_ID			(CFG_PCIE_CAP+0x0)
#define	DEV_CAP				(CFG_PCIE_CAP+0x4)
#define	DEV_STS_CTRL			(CFG_PCIE_CAP+0x8)
#define	DEV_STS_PAYLOAD_256		((1<<5) | (1<<12) | (1<<14))
#define	DEV_STS				(CFG_PCIE_CAP+0xA)
#define	LNK_CAP				(CFG_PCIE_CAP+0xC)
#define LNK_CAP_SPD_WIDTH_MASK		0xfffffc00
#define	LNK_STS_CTRL			(CFG_PCIE_CAP+0x10)
#define	ROOT_CTRL_CAP			(CFG_PCIE_CAP+0x1C)
#define	ROOT_STATUS			(CFG_PCIE_CAP+0x20)
#define	DEV_CAP2			(CFG_PCIE_CAP+0x24)
#define	DEV_STS_CTRL2			(CFG_PCIE_CAP+0x28)
#define	LINK_CAP2			(CFG_PCIE_CAP+0x2C)
#define	LNK_STS_CTRL2			(CFG_PCIE_CAP+0x30)
//----------------------------------------------------------------------------
//	Advanced Error Reporting CAPABILITY STRUCTURE
//----------------------------------------------------------------------------
#define	AER_PCIE_EN_CAP			(CFG_AER_CAP+0x0)
#define	AER_UN_ERR_ST			(CFG_AER_CAP+0x4)
#define	AER_UN_ERR_MS			(CFG_AER_CAP+0x8)
#define	AER_UN_ERR_SV			(CFG_AER_CAP+0xC)
#define	AER_CO_ERR_ST			(CFG_AER_CAP+0x10)
#define	AER_CO_ERR_MS			(CFG_AER_CAP+0x14)
#define	AER_CO_ERR_RPL_TIMER_TIMEOUT_MASK	(0x1 << 12)
#define	AER_CAP_CR			(CFG_AER_CAP+0x18)
#define	AER_HD_L_0			(CFG_AER_CAP+0x1C)
#define	AER_HD_L_4			(CFG_AER_CAP+0x20)
#define	AER_HD_L_8			(CFG_AER_CAP+0x24)
#define	AER_HD_L_12			(CFG_AER_CAP+0x28)
#define	AER_RT_ERR_CMD			(CFG_AER_CAP+0x2C)
#define	AER_RT_ERR_STS			(CFG_AER_CAP+0x30)
#define	AER_ERR_SRC_ID			(CFG_AER_CAP+0x34)
#define AER_TLP_PRE_LOG			(CFG_AER_CAP+0x38)
//---------------------------------------------------------------------------
//PORT_LOGIC
//---------------------------------------------------------------------------
#define	PL_LAT_REL_TIM			(PORTLOG_OFFSET+0x0)
#define	PL_OT_MSG_R			(PORTLOG_OFFSET+0x4)
#define	PL_PT_LNK_R			(PORTLOG_OFFSET+0x8)
#define	PL_ACK_FREQ_R			(PORTLOG_OFFSET+0xC)
#define	PL_ACK_N_FTS_MSK		0xffff00ff
#define	PL_ACK_N_FTS_255		(0xff << 8)
#define	PL_PT_LNK_CTRL_R		(PORTLOG_OFFSET+0x10)
#define PL_PT_LNK_CTRL_LANE_MASK	0xfff0ffff
#define PL_PT_LANE_4			4
#define PL_PT_LNK_CTRL_4LANE		(0x7 << 16)
#define PL_PT_LANE_2			2
#define PL_PT_LNK_CTRL_2LANE		(0x3 << 16)
#define PL_PT_LANE_1			1
#define PL_PT_LNK_CTRL_1LANE		(0x1 << 16)
#define	PL_LN_SKW_R			(PORTLOG_OFFSET+0x14)
#define	PL_SYMB_N_R			(PORTLOG_OFFSET+0x18)
#define	PL_SYMB_T_R			(PORTLOG_OFFSET+0x1C)
#define	PL_FL_MSK_R2			(PORTLOG_OFFSET+0x20)
#define	PL_DB_R0			(PORTLOG_OFFSET+0x28)
#define	PL_DB_R1			(PORTLOG_OFFSET+0x2C)
#define	PL_TR_P_STS_R			(PORTLOG_OFFSET+0x30)
#define	PL_TR_NP_STS_R			(PORTLOG_OFFSET+0x34)
#define	PL_TR_C_STS_R			(PORTLOG_OFFSET+0x38)
#define	PL_Q_STS_R			(PORTLOG_OFFSET+0x3C)
#define	PL_VC_TR_A_R1			(PORTLOG_OFFSET+0x40)
#define	PL_VC_TR_A_R2			(PORTLOG_OFFSET+0x44)
#define	PL_VC0_PR_Q_C			(PORTLOG_OFFSET+0x48)
#define	PL_VC0_NPR_Q_C			(PORTLOG_OFFSET+0x4C)
#define	PL_VC0_CR_Q_C			(PORTLOG_OFFSET+0x50)
#define	PL_GEN2_CTRL			(PORTLOG_OFFSET+0x10C)
#define PL_GEN2_CTRL_LANE_MASK		(0x1FF << 8)
#define	PL_PHY_STS_R			(PORTLOG_OFFSET+0x110)
#define	PL_PHY_CTRL_R			(PORTLOG_OFFSET+0x114)
#define	PL_MSI_CNT_ADDR			(PORTLOG_OFFSET+0x120)
#define	PL_MSI_CNT_ADDR_UPPER		(PORTLOG_OFFSET+0x124)
#define	PL_MSI_CNT_INT_EN(num)		(PORTLOG_OFFSET+0x128+(num*0xc))
#define	PL_MSI_CNT_INT_MSK(num)		(PORTLOG_OFFSET+0x12C+(num*0xc))
#define	PL_MSI_CNT_INT_STS(num)		(PORTLOG_OFFSET+0x130+(num*0xc))
#define	PL_MSI_CNT_GPIO			(PORTLOG_OFFSET+0x188)
#define	PL_MSI_CNT_GPIO			(PORTLOG_OFFSET+0x188)
#define	PL_LOGIC_GEN3			(PORTLOG_OFFSET+0x190)
#define PL_LOGIC_GEN3_EQ_OFF		0x00012001
#define	PL_PIPE_LOOPBACK_CTRL		(PORTLOG_OFFSET+0x1B8)
#define	PL_MISC_CTRL			0x8BC
#define	PL_AMBA_ERR_RESP		(PORTLOG_OFFSET+0x1D0)
#define	PL_AMBA_LINK_TOUT		(PORTLOG_OFFSET+0x1D4)
#define	PL_AMBA_ORDRMGR			(PORTLOG_OFFSET+0x1DC)
#define	PL_COHERENCY_CTRL1		(PORTLOG_OFFSET+0x1E0)
#define	PL_COHERENCY_CTRL2		(PORTLOG_OFFSET+0x1E4)
#define	PL_COHERENCY_CTRL3		(PORTLOG_OFFSET+0x1E8)
#define PL_COHERENCY_DMA_CACHE		0x10101010
#define	PL_LTR_LATENCY			(PORTLOG_OFFSET+0x430)
#define	PL_AUX_CLK_FREQ			(PORTLOG_OFFSET+0x440)
#define	PL_L1_SUBSTATES			(PORTLOG_OFFSET+0x444)

// ATU
#define PCIE_ATU_BASE			0x600000
// for outbound
#define PCIE_ATU_CR1_OUT0		(PCIE_ATU_BASE + 0x0)
#define PCIE_ATU_CR2_OUT0		(PCIE_ATU_BASE + 0x4)
#define PCIE_ATU_LOWER_BASE_OUT0	(PCIE_ATU_BASE + 0x8)
#define PCIE_ATU_UPPER_BASE_OUT0	(PCIE_ATU_BASE + 0xC)
#define PCIE_ATU_LIMIT_OUT0		(PCIE_ATU_BASE + 0x10)
#define PCIE_ATU_LOWER_TARGET_OUT0	(PCIE_ATU_BASE + 0x14)
#define PCIE_ATU_UPPER_TARGET_OUT0	(PCIE_ATU_BASE + 0x18)
#define PCIE_ATU_CR3_OUT0		0x1C

// for inbound
#define PCIE_ATU_CR1_IN0		(PCIE_ATU_BASE + 0x100)
#define PCIE_ATU_CR2_IN0		(PCIE_ATU_BASE + 0x104)
#define PCIE_ATU_LOWER_BASE_IN0		(PCIE_ATU_BASE + 0x108)
#define PCIE_ATU_UPPER_BASE_IN0		(PCIE_ATU_BASE + 0x10C)
#define PCIE_ATU_LIMIT_IN0		(PCIE_ATU_BASE + 0x110)
#define PCIE_ATU_LOWER_TARGET_IN0	(PCIE_ATU_BASE + 0x114)
#define PCIE_ATU_UPPER_TARGET_IN0	(PCIE_ATU_BASE + 0x118)
//DMA
#define PCIE_DMA_BASE			(0x0)

#define PCIE_DMA_DBOFF_STOP		(0x80000000)

#define PCIE_DMA_WR_INT_STS		(PCIE_DMA_BASE + 0x2000)
#define PCIE_DMA_WR_INT_MASK		(PCIE_DMA_BASE + 0x2004)
#define PCIE_DMA_WR_INT_CLEAR		(PCIE_DMA_BASE + 0x2008)
#define PCIE_DMA_WR_ERR_STS		(PCIE_DMA_BASE + 0x200C)
#define PCIE_DMA_RD_INT_STS		(PCIE_DMA_BASE + 0x2010)
#define PCIE_DMA_RD_INT_MASK		(PCIE_DMA_BASE + 0x2014)
#define PCIE_DMA_RD_INT_CLEAR		(PCIE_DMA_BASE + 0x2018)

#define PCIE_DMA_CH_CTRL1		(PCIE_DMA_BASE + 0x3000)
#define PCIE_DMA_CH_CTRL2		(PCIE_DMA_BASE + 0x3100)

#define PCIE_DMA_CH_OFFSET		(0x1000)

#define PCIE_DMA_STS_2CH_MASK		(0x3)

#define PCIE_DMA_STS_IRQ0_MASK		(0x30003)
#define PCIE_DMA_STS_IRQ1_MASK		(0xC000C)
#define PCIE_DMA_STS_IRQ2_MASK		(0x300030)
#define PCIE_DMA_STS_IRQ3_MASK		(0xC000C0)

#define PCIE_DMA_CAP_CTRL		(PCIE_DMA_BASE + 0x8)

#define PCIE_DMA_WR_EN			(PCIE_DMA_BASE + 0xC)
#define PCIE_DMA_WR_DBOFF		(PCIE_DMA_BASE + 0x10)
#define PCIE_DMA_WR_PWR_EN		(PCIE_DMA_BASE + 0x128)

#define PCIE_DMA_RD_EN			(PCIE_DMA_BASE + 0x2C)
#define PCIE_DMA_RD_DBOFF		(PCIE_DMA_BASE + 0x30)
#define PCIE_DMA_RD_PWR_EN		(PCIE_DMA_BASE + 0x168)

#define PWR_EN_OFFSET(dma_num)		(dma_num * 0x4)

#define DBOFF_WR_OFF			(0x1 << 31)
#define DBOFF_DMA_CH_MASK		(0x3 << 0)

#define CH_CTRL1_CB			(0x1 << 0)
#define CH_CTRL1_TCB			(0x1 << 1)
#define CH_CTRL1_LLP			(0x1 << 2)
#define CH_CTRL1_LIE			(0x1 << 3)
#define CH_CTRL1_RIE			(0x1 << 4)
#define CH_CTRL1_CS			(0x3 << 5)
#define CH_CTRL1_DMA_RESERVED0		(0x1 << 7)
#define CH_CTRL1_CCS			(0x1 << 8)
#define CH_CTRL1_LLE			(0x1 << 9)
#define CH_CTRL1_DMA_RESERVED1		(0x3 << 10)
#define CH_CTRL1_DMA_FUNC_NUM		(0x1F << 12)
#define CH_CTRL1_DMA_RESERVED2		(0x3F << 17)
#define CH_CTRL1_DMA_NS_DST		(0x1 << 23)
#define CH_CTRL1_DMA_NS_SRC		(0x1 << 24)
#define CH_CTRL1_DMA_RO			(0x1 << 25)
#define CH_CTRL1_DMA_RESERVED5		(0x1 << 26)
#define CH_CTRL1_DMA_TC			(0x7 << 27)
#define CH_CTRL1_DMA_AT			(0x3 << 30)

#define PCIE_DMA_WR_XFER_SIZE		(PCIE_DMA_CH_CTRL1 + 0x08)
#define PCIE_DMA_WR_SARL		(PCIE_DMA_CH_CTRL1 + 0x0C)
#define PCIE_DMA_WR_SARH		(PCIE_DMA_CH_CTRL1 + 0x10)
#define PCIE_DMA_WR_DARL		(PCIE_DMA_CH_CTRL1 + 0x14)
#define PCIE_DMA_WR_DARH		(PCIE_DMA_CH_CTRL1 + 0x18)

#define CH_CTRL2_CB			(0x1 << 0)
#define CH_CTRL2_TCB			(0x1 << 1)
#define CH_CTRL2_LLP			(0x1 << 2)
#define CH_CTRL2_LIE			(0x1 << 3)
#define CH_CTRL2_RIE			(0x1 << 4)
#define CH_CTRL2_CS			(0x3 << 5)
#define CH_CTRL2_DMA_RESERVED0		(0x1 << 7)
#define CH_CTRL2_CCS			(0x1 << 8)
#define CH_CTRL2_LLE			(0x1 << 9)
#define CH_CTRL2_DMA_RESERVED1		(0x3 << 10)
#define CH_CTRL2_DMA_FUNC_NUM		(0x1F << 12)
#define CH_CTRL2_DMA_RESERVED2		(0x3F << 17)
#define CH_CTRL2_DMA_NS_DST		(0x1 << 23)
#define CH_CTRL2_DMA_NS_SRC		(0x1 << 24)
#define CH_CTRL2_DMA_RO			(0x1 << 25)
#define CH_CTRL2_DMA_RESERVED5		(0x1 << 26)
#define CH_CTRL2_DMA_TC			(0x7 << 27)
#define CH_CTRL2_DMA_AT			(0x3 << 30)

#define PCIE_DMA_RD_XFER_SIZE		(PCIE_DMA_CH_CTRL2 + 0x08)
#define PCIE_DMA_RD_SARL		(PCIE_DMA_CH_CTRL2 + 0x0C)
#define PCIE_DMA_RD_SARH		(PCIE_DMA_CH_CTRL2 + 0x10)
#define PCIE_DMA_RD_DARL		(PCIE_DMA_CH_CTRL2 + 0x14)
#define PCIE_DMA_RD_DARH		(PCIE_DMA_CH_CTRL2 + 0x18)

//CMU
#define CMU_QCH_DOWN			0x4
#define CMU_QCH_UP			0x6

//MSI-X
#define MSIX_BAR5_BASE_ADDRESS		0x24000000

//Doorbell
#define PCIE_0_DOORBELL_BASE_ADDRESS	(0x16360000)
#define PCIE_1_DOORBELL_BASE_ADDRESS	(0x16370000)
#define PCIE_2_DOORBELL_BASE_ADDRESS	(0x16160000)
#define PCIE_3_DOORBELL_BASE_ADDRESS	(0x16170000)

#define PCIE_DOORBELL0_STATUS		(0x0)
#define PCIE_DOORBELL0_ENABLE		(0x4)
#define PCIE_DOORBELL0_CLEAR		(0x8)

#define PCIE_DOORBELL_OFFSET		(0x1000)

#define PCIE_EP_INTADMASK		0xF
#define PCIE_EP_IRQMASK			0xFFFFFFFF
#define PCIE_EP_IRQMASK1		PCIE_EP_IRQMASK
#define PCIE_EP_IRQMASK2		(PCIE_EP_IRQMASK & \
						(~(RTLH_RFC_UPD_0_1 | \
							RTLH_RFC_UPD_0_0 | \
							Q_NOT_EMPTY) \
						) \
					)
#define PCIE_EP_IRQMASK3		0xFFFF0000

//SR-IOV
#define PCIE_SRIOV_CAP_POS		(0x200)
#define VF_OFFSET			0x8
#define VF_STRIDE			0x1
#define INITIAL_VF			0x4
#define TOTAL_VF			0x4
#define VF_DID_INTERVAL			0x10

#define PCIE_SHADOW_REG_POS		(0x200000)

//Speed
#define	EXYNOS_PCIE_EVT01_SPEED_GEN3	3
#define	EXYNOS_PCIE_EVT2_SPEED_GEN4	4

//Status
#define EXYNOS_PCIE_EVT01_LNK_STS	0x3
#define EXYNOS_PCIE_EVT2_LNK_STS	0x4

//To check KITT2 Version
#define EXYNOSAUTOV920_EVT01		0
#define EXYNOSAUTOV920_EVT2		2

#define LANE_STAT_MAX			3

static const int exynosautov920_evt01 = EXYNOSAUTOV920_EVT01;
static const int exynosautov920_evt2 = EXYNOSAUTOV920_EVT2;

enum exynos_pcie_ep_state {
	STATE_LINK_DOWN = 0,
	STATE_LINK_UP_TRY,
	STATE_LINK_DOWN_TRY,
	STATE_LINK_UP,
	STATE_LINK_DOWN_ABNORMAL,
};

enum exynos_pcie_ep_lane_state {
	STATE_LANE_INIT = -1,
	STATE_LANE_DOWN,
	STATE_LANE_UP,
};

enum exynos_pcie_ep_work_state {
	STATE_DMA_IDLE = 0,
	STATE_DMA_BUSY,
};

enum atu_direction {
	ATU_OB = 0,
	ATU_IB,
	ATU_VF_OB,
	ATU_VF_IB,
};

enum __ltssm_states {
	S_DETECT_QUIET		= 0x00,
	S_DETECT_ACT		= 0x01,
	S_POLL_ACTIVE		= 0x02,
	S_POLL_COMPLIANCE	= 0x03,
	S_POLL_CONFIG		= 0x04,
	S_PRE_DETECT_QUIET	= 0x05,
	S_DETECT_WAIT		= 0x06,
	S_CFG_LINKWD_START	= 0x07,
	S_CFG_LINKWD_ACEPT	= 0x08,
	S_CFG_LANENUM_WAIT	= 0x09,
	S_CFG_LANENUM_ACEPT	= 0x0A,
	S_CFG_COMPLETE		= 0x0B,
	S_CFG_IDLE		= 0x0C,
	S_RCVRY_LOCK		= 0x0D,
	S_RCVRY_SPEED		= 0x0E,
	S_RCVRY_RCVRCFG		= 0x0F,
	S_RCVRY_IDLE		= 0x10,
	S_L0			= 0x11,
	S_L0S			= 0x12,
	S_L123_SEND_EIDLE	= 0x13,
	S_L1_IDLE		= 0x14,
	S_L2_IDLE		= 0x15,
	S_L2_WAKE		= 0x16,
	S_DISABLED_ENTRY	= 0x17,
	S_DISABLED_IDLE		= 0x18,
	S_DISABLED		= 0x19,
	S_LPBK_ENTRY		= 0x1A,
	S_LPBK_ACTIVE		= 0x1B,
	S_LPBK_EXIT		= 0x1C,
	S_LPBK_EXIT_TIMEOUT	= 0x1D,
	S_HOT_RESET_ENTRY	= 0x1E,
	S_HOT_RESET		= 0x1F,
	GEN3_LINKUP		= 0x91,
};

enum exynos_pcie_ep_work_cmd {
	WORK_MEM_WR = 0,
	WORK_MEM_RD,
	WORK_MSI,
	WORK_DOORBELL,
	WORK_MEM_WR_RD,
	WORK_MSI_X,
};

enum exynos_pcie_ep_work_exception {
	WORK_DEFAULT = 0,
	WORK_LINK_RESET,
};

struct exynos_ep_pcie_phyinfo {
	void __iomem		*phy_base;
	void __iomem		*sysreg_base;
	void __iomem		*elbi_base;
	void __iomem		*gen_subsys_base;
	void __iomem		*e32_phy_base;
	void __iomem		*soc_ctrl_base;
	void __iomem		*pmu_base;
	void __iomem		*elbi_base_other;
	void __iomem		*dbi_base;
	void __iomem		*elbi_cmn_base;
	void __iomem		*cmu_base;
	void __iomem		*doorbell_base;
	int			ch_num;
	/* 0: bifurcation, 1:aggregation */
	bool			use_bifurcation;
	int			lanes_num;
	bool			use_sris;
	bool			cold_rst;

	u32			linkA_status;
	u32			linkB_status;

	int			soc_variant;
};

struct pcie_ep_phyops {
	void (*phy_all_pwrdn)(struct exynos_ep_pcie_phyinfo *phyinfo);
	void (*phy_all_pwrdn_clear)(struct exynos_ep_pcie_phyinfo *phyinfo);
	int (*phy_config)(struct exynos_ep_pcie_phyinfo *phyinfo);
	void (*phy_other_set)(struct exynos_ep_pcie_phyinfo *phyinfo);
};

struct exynos_pcie_ep_clks {
	struct clk		*dbi_hwacg;
	struct clk		*pcie_clks[10];
	struct clk		*phy_clks[3];
};

struct exynos_pcie_ep_bar {
	u32			type;
	size_t			size;
	u32			mem_type;
	bool			is_mapped;
	struct pci_epf_bar	epf_bar;
};

struct exynos_ep_ext_node {
	int				ch_num;
	u16				vf_num;
	bool				is_activated;
	struct exynos_ep_ext_info	*ext;
	struct exynos_ep_ext_dd_ops	*ext_ops;
};

struct exynos_ep_msi {
	void __iomem		*msi_va;
	phys_addr_t		msi_pa;
	u64			msi_pciaddr;
	u64			umsi_pci_addr;
	u64			lmsi_pci_addr;
	size_t			msi_size;
};

struct exynos_ep_pcie {
	struct dw_pcie			*pci;
	struct exynos_ep_pcie_phyinfo	*ep_phyinfo;
	void __iomem			*elbi_base;
	void __iomem			*gen_subsys_base;
	void __iomem			*e32_phy_base;
	void __iomem			*soc_ctrl_base;
	void __iomem			*pmu_base;
	void __iomem			*elbi_base_other;
	void __iomem			*phy_base;
	void __iomem			*sysreg_base;
	void __iomem			*dbi_base;
	void __iomem			*ia_base;
	void __iomem			*atu_base;
	void __iomem			*elbi_cmn_base;
	void __iomem			*sysreg_ctrl_base;
	void __iomem			*doorbell_base;
	void __iomem			*cmu_base;
	void __iomem			*dma_base;

	struct pcie_ep_phyops		phy_ops;
	struct platform_device		*pdev;
	struct workqueue_struct		*pcie_ep_wq;
	struct workqueue_struct		*pcie_ep_wq_exception;
	struct workqueue_struct		*pcie_ep_wq_dislink;
	struct exynos_pcie_ep_clks	clks;

	struct delayed_work		irq1_evt_work;
	struct delayed_work		test_work;
	struct delayed_work		exception_work;
	struct delayed_work		dislink_work;
	enum exynos_pcie_ep_work_cmd	work_cmd;
	enum exynos_pcie_ep_work_exception	work_exception;

	enum exynos_pcie_ep_state	state;
	enum exynos_pcie_ep_work_state	work_state;

	spinlock_t			dma_lock;
	int				dma_ch_num;
	void				*dma_wr_buf;
	void				*dma_rd_buf;
	dma_addr_t			rd_dma;
	dma_addr_t			wr_dma;
	u32				checksum;
	unsigned int			size;
	int				cnt;

	int				dma_num;
	int				msi_data;
	int				vf_num;

	int				sysreg_offset;

	struct completion		wdma0;
	struct completion		rdma0;

	struct completion		wdma1;
	struct completion		wdma2;
	struct completion		wdma3;

	struct completion		rdma1;
	struct completion		rdma2;
	struct completion		rdma3;

	u32				irq0_status;
	u32				irq1_status;
	u32				doorbell;

	int				irq;
	int				db_irq[MAX_VF_NUM];
	int				dma_irq[MAX_VF_NUM];

	u32				pmu_phy_offset;
	/* 1: bifurcation, 0:aggregation */
	bool				use_bifurcation;
	bool				use_sris;
	bool				use_sysmmu;
	bool				use_sharability;
	int				lanes_num;

	int				ch_num;
	int				pcie_clk_num;
	int				phy_clk_num;
	int				max_link_speed;
	bool				linkup_ready;
	struct regmap			*pmureg;
	struct regmap			*sysreg;

	struct pinctrl			*pcie_ep_pinctrl;
	struct pinctrl_state		*pin_perst;
	struct pinctrl_state		*pin_clkreq;

	int				perst_gpio;

	int				lane_status;

	struct exynos_pcie_ep_bar bar[6];
	struct exynos_ep_ext_node ext_node[MAX_VF_NUM + 1];

	unsigned int			s2mpu_base;
	unsigned int			vgen_base;
	int				soc_variant;

	struct exynos_ep_msi		msi_region;
	int				wr_ll_start;
	int				wr_ll_cnt;
	int				rd_ll_start;
	int				rd_ll_cnt;
};