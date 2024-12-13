/* SPDX-License-Identifier: GPL-2.0 */
/**
 * pcie-exynos-rc-auto.h - Exynos PCIe RC driver
 *
 * Copyright (c) 2013-2020 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Authors: Sanghoon Bae <sh86.bae@samsung.com>
 *	    Jiheon Oh <jiheon.oh@samsung.com>
 *
 */

#ifndef __PCIE_EXYNOS_HOST_V1_H
#define __PCIE_EXYNOS_HOST_V1_H

#define FW_VER_205 0x2005
#define FW_VER_207 0x2007
#define FW_VER_240 0x2040

#define EVT20 0x20
#define EVT21 0x21

#include <linux/dma-buf.h>
#include <linux/iommu.h>
#include <linux/dma-heap.h>
#include <soc/samsung/pcie-exynos-v920-rc-auto-external.h>

#define MAX_TIMEOUT		50000
#define ID_MASK			0xffff
#define MAX_RC_NUM		4
#define MAX_VF_NUM		4
#define MAX_MSI_SET		1
#define MSIX_MIN_VEC		1
#define MSIX_MAX_VEC		32
#define PHY_TIMEOUT		2000
#define MAX_PHY_INTI_TIMEOUT	15000

#define PCIE_VF_MSI_VECTOR_NUMBER	1
#define PCIE_VF_OB_SIZE		0x1000000

#define GIC_TRANSLATER_ADDR	0x10450000

#define PCI_DEVICE_ID_EXYNOS	0xecec

#define PCIE_CH0		0
#define PCIE_CH1		1
#define PCIE_CH2		2
#define PCIE_CH3		3

#define PERST_HIGH		1
#define PERST_LOW		0

#define PCIE_RC_RETRY		10
#define PCIE_RC_LINKRESTORE	0 /* Use only for test mode */

#define CH_SHARE_PHY		2

#define to_exynos_pcie(x)	dev_get_drvdata((x)->dev)

#define PCIE_BUS_PRIV_DATA(pdev) \
	((struct pcie_port *)pdev->bus->sysdata)

#define MAX_PCIE_PIN_STATE	2
#define PCIE_PIN_DEFAULT	0
#define PCIE_PIN_IDLE		1

struct regmap;

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

struct exynos_pcie_clks {
	struct clk	*dbi_hwacg;
	struct clk	*pcie_clks[10];
	struct clk	*phy_clks[3];
};

enum exynos_pcie_state {
	STATE_LINK_DOWN = 0,
	STATE_LINK_UP_TRY,
	STATE_LINK_DOWN_TRY,
	STATE_LINK_UP,
};

enum exynos_pcie_sriov {
	PCIE_PF = 0,
	PCIE_VF1,
	PCIE_VF2,
	PCIE_VF3,
	PCIE_VF4,
	PCIE_VF_MAX
};

struct pcie_phyops {
	void (*phy_check_rx_elecidle)(struct pcie_port *pp,
		int val);
	void (*phy_all_pwrdn)(struct pcie_port *pp);
	void (*phy_all_pwrdn_clear)(struct pcie_port *pp);
	int (*phy_config)(struct pcie_port *pp, bool fw_update);
	void (*phy_config_regmap)(struct pcie_port *pp);
};

struct exynos_pcie {
	struct dw_pcie		*pci;
	void __iomem		*elbi_base;
	void __iomem		*gen_subsys_base;
	void __iomem		*elbi_base_other;
	void __iomem		*e32_phy_base;
	void __iomem		*atu_base;
	void __iomem		*phy_base;
	void __iomem            *soc_ctrl_base;
	void __iomem            *pmu_base;
	void __iomem		*sysreg_base;
	void __iomem		*rc_dbi_base;
	void __iomem		*shadow_dbi_base;
	void __iomem		*cmu_base;
	void __iomem		*ia_base;
	void __iomem		*elbi_cmn_base;
	void __iomem		*sram_base;

	struct regmap		*pmureg;
	struct regmap		*sysreg;
	resource_size_t		mem_base;
	resource_size_t		mem_bus_addr;
	u32			mem_size;
	int			perst_gpio;
	int			ch_num;
	int			pcie_clk_num;
	int			phy_clk_num;
	enum exynos_pcie_state	state;
	int			probe_ok;
	int			l1ss_enable;
	int			linkdown_cnt;
	int			idle_ip_index;
	bool			use_msi;
	bool			use_cache_coherency;
	bool			use_sicd;
	bool			atu_ok;
	bool			use_sysmmu;
	bool			use_sharability;
	bool			use_ia;
	bool			use_bifurcation;
	bool			use_sris;
	spinlock_t		conf_lock;
	spinlock_t		reg_lock;
	struct workqueue_struct	*pcie_wq;
	struct exynos_pcie_clks	clks;
	struct pci_dev		*pci_dev;
	struct pci_saved_state	*pci_saved_configs;
	struct notifier_block	power_mode_nb;
	struct notifier_block   ss_dma_mon_nb;
	struct delayed_work	dislink_work;
	struct delayed_work	hardaccess_work;
	struct exynos_pcie_register_event *event_reg;
#ifdef CONFIG_PM_DEVFREQ
	unsigned int            int_min_lock;
#endif
	u32			ip_ver;
	u32			num_vfs;
	u32			pmu_phy_offset;
	struct pcie_phyops	phy_ops;
	int			l1ss_ctrl_id_state;
	struct workqueue_struct *pcie_wq_l1ss;
	struct delayed_work     l1ss_boot_delay_work;
	int			boot_cnt;
	int			work_l1ss_cnt;
	int			ep_device_type;
	int			max_link_speed;
	bool			s2r_if_control;

	struct pinctrl		*pcie_pinctrl;
	struct pinctrl_state	*pin_state[MAX_PCIE_PIN_STATE];
	int			msi_irq[PCIE_VF_MAX];
#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	struct pci_dev		*ep_pci_dev[PCIE_VF_MAX];
	int			sriov_msi_enable[PCIE_VF_MAX];
	int			sriov_supported;
#endif
	unsigned int		s2mpu_base;
	unsigned int		vgen_base;
	int			soc_variant;
	u32			phy_fw_ver;
	u32			evt2_minor_ver;

	bool is_probe_done;
	bool			fw_update;
	void			*pcie_fw_buf;
	dma_addr_t		fw_paddr;
	u64			phy_fw_size;

	u32			linkflushtime;
	bool			use_its;
};

//To check KITT2 Revision
#define EXYNOSAUTOV920_EVT01	0
#define EXYNOSAUTOV920_EVT2	2

static const int exynosautov920_evt01 = EXYNOSAUTOV920_EVT01;
static const int exynosautov920_evt2 = EXYNOSAUTOV920_EVT2;

/* PCIe ELBI registers */
#define PCIE_DEV_TYPE_RC			(0x1 << 2)
#define PCIE_DEV_TYPE_OVR			(0x1 << 4)
#define PCIE_SRIS_MODE				(0x1 << 8)
#define PCIE_GEN_CTRL_3				0x8
#define PCIE_LTSSM_DISABLE			0x0
#define PCIE_LTSSM_ENABLE			0x1
#define PCIE_GEN_CTRL4				0xC
#define PCIE_LINK_FLUSH_TIME_M(x)		((x << 11) & 0xF800)
#define PCIE_LINK_FLUSH_TIME_NONE		0x0
#define PCIE_LINK_FLUSH_TIME_1MS_VALUE		0xA
#define PCIE_LFT_SLEEP_CAL(x) ((1 << (x - PCIE_LINK_FLUSH_TIME_1MS_VALUE)) + 50)
#define PCIE_APP_REQ_EXIT_L1			0x30
#define PCIE_EXIT_ASPM_L1_ENABLE		((0x1)<<18)
#define PCIE_EXIT_ASPM_L1_BIT			18
#define PCIE_INT_STS				0x05C
#define PCIE_INT_EN				0xF
#define PCIE_RX_MSG_INT_STS			0x80
#define PCIE_PM_TO_ACK				(0x1 << 11)
#define PCIE_ERR_INT_STS			0x84
#define PCIE_USER_INT_STS			0x8C
#define PCIE_IRQ_CPL_TIMEOUT			0x1
#define PCIE_RX_MSG_INT_EN			0x90
#define EVT2_PCIE_PME_ACK_INT_EN		(0x1 << 11)
#define EVT01_PCIE_PME_ACK_INT_EN		((0x1 << 11) | (0x1 << 0))
#define PCIE_ERR_INT_CTRL			0x94
#define PCIE_IRQ_LINK_DOWN			(0x1 << 30)
#define PCIE_USER_INT_EN			0x9C
#define PCIE_IRQ_CPL_TIMEOUT_EN			0x1
#define PCIE_TX_MSG_REQ				0x1C0
#define PCIE_PME_TURNOFF_REQ			((0x1)<<19)
#define PCIE_PME_TURNOFF_REQ_RST		((0x0)<<19)
#define PCIE_LINK_DBG_2				0x304
#define PCIE_LINK_LTSSM_MASK			0x1f
#define PCIE_LINK_CDM_IN_RESET_MASK		24

/* SUB Controller General */
#define GENERAL_SS_RST_CTRL_1			0x48
#define GENERAL_RST_PE0_SOFT_COLD_RESET		0x1
#define GENERAL_RST_PE0_SOFT_WARM_PHY_RESET	(0x3<<1)
#define GENERAL_RST_PE1_SOFT_COLD_RESET		(0x1<<8)
#define GENERAL_RST_PE1_SOFT_WARM_PHY_RESET	(0x3<<9)
#define GENERAL_RST_PE0_1_PHY_EN		0x808

/* PHY registers */
#define PCIE_PHY_RTUNE_REQ			0x10000001
#define PCIE_PHY0_GEN_CTRL_1			0x10
#define PCIE_PHY0_ALL_BYPASS			(0x7 << 10)
#define PCIE_PHY0_SRAM_BL_BYPASS		(0x1 << 12)
#define PCIE_PHY0_REFA_CLK_SEL_MASK		(0x3 << 16)
#define PCIE_PHY0_REFA_CLK_SEL_PAD		(0x1 << 16)
#define PCIE_PHY0_REFA_CLK_SEL_ALT1		(0x2 << 16)
#define PCIE_PHY0_REFB_CLK_SEL_MASK		(0x3 << 18)
#define PCIE_PHY0_REFB_CLK_SEL_PAD		(0x1 << 18)
#define PCIE_PHY0_REFB_CLK_SEL_ALT1		(0x2 << 18)
#define PCIE_PHY0_PHY0_SRAM_BYPASS		(0x1 << 10)
#define PCIE_PHY0_PHY0_SRAM_EXT_LD_DONE		(0x1 << 11)
#define PCIE_PHY0_REFA_B_ALT1			0x061A0060
#define PCIE_PHY0_REFA_B_ALT0			0x06100060
#define PCIE_PHY0_REFA_B_PAD			0x06150060
#define PCIE_PHY0_SRAM_INIT_DONE_BIT		31
#define PCIE_PHY_FW_VERSION_0			0x660

#define PCIE_PHY_EXT_TX_ROPLL_POSTDIV_CTRL		0x1A8
#define PCIE_PHY_ROPLL_POSTDIV_VAL			0x1249
#define PCIE_PHY_EXT_TX_ROPLL_POSTDIV_OVRD_EN_CTRL	0x1C4
#define PCIE_PHY_ROPLL_POSTDIV_OVRD_EN_VAL		0xF
#define PCIE_PIPE_LANEX_LANEPLL_BYPASS_MODE_CTRL_1	0x384
#define PCIE_PIPE_BYPASS_MODE_CTRL_VAL1			0x00
#define PCIE_PIPE_BYPASS_MODE_CTRL_VAL2			0x000

/* PCIe PMU registers */
#define PCIE_PHY_4L_CONFIGURATION	0x700
#define PCIE_PHY_2L_CONFIGURATION	0x704
#define PCIE_PHY_CFG_EN_PHY		0x1

/* PCIe DBI registers */
#define HDR_ID_DID_BIT			16
#define PM_CAP_OFFSET			0x40
#define MSI_CAP_OFFSET			0x50

#define PM_CAP_CON_STATUS_OFFSET	0x44
#define PM_CAP_POWER_STATE_MASK		0x3
#define PCI_COMMAND_DBI_ACCESS_CMD	0x140
#define PCI_COMMAND_DBI_OTH_ACCESS_CMD	0x146

#define MSI_MULTIPLE_MSG_CAP_MASK	(0x7 << 17)
#define MSI_MULTIPLE_MSG_CAP_32		(0x5 << 17)
#define PCIE_CAP_OFFSET			0x70
#define PCIE_LINK_CTRL_STAT		0x80
#define PCI_EXP_LNKCAP_MLW_X1		(0x1 << 4)
#define PCI_EXP_LNKCAP_L1EL_64USEC	(0x7 << 15)
#define PCI_EXP_LNKCTL2_TLS_2_5GB	0x1
#define PCI_EXP_LNKCTL2_TLS_5_0GB	0x2
#define PCI_EXP_LNKCTL2_TLS_8_0GB	0x3
#define PCI_EXP_LNKCTL2_TLS_16_0GB      0x4
#define PCIE_LINK_L1SS_CONTROL		0x158
#define PORT_LINK_TCOMMON_32US		(0x20 << 8)
#define PCIE_LINK_L1SS_CONTROL2		0x15C
#define PORT_LINK_L1SS_ENABLE		(0xf << 0)
#define PORT_LINK_TPOWERON_130US	(0x69 << 0)
#define PORT_LINK_TPOWERON_3100US	(0xfa << 0)
#define PORT_LINK_L1SS_T_PCLKACK	(0x3 << 6)
#define PORT_LINK_L1SS_T_L1_2		(0x4 << 2)
#define PORT_LINK_L1SS_T_POWER_OFF	(0x2 << 0)
#define PCIE_ACK_F_ASPM_CONTROL		0x70C

#define PCIE_MISC_CONTROL		0x8BC
#define PCIE_MISC_DBI_RO_WR_EN		0x1

#define PCIE_HEADERTYPE_L_C		0xC
#define PCIE_HEADER_1			(0x1 << 16)

#define PCIE_TIMER_CTRL			0x718
#define PCIE_TIMER_FAST_LINK_MASK	(0x3 << 29)
#define PCIE_TIMER_FAST_LISSM_EN	(0x1 << 29)

#define PCIE_PORT_LINK_CTRL		0x710
#define PCIE_PORT_LINK_CAPABLE_MASK	(0xF << 16)
#define PCIE_PORT_LINK_CAPABLE_4L	(0x7 << 16)
#define PCIE_PORT_LINK_CAPABLE_2L	(0x3 << 16)
#define PCIE_PORT_LINK_CAPABLE_1L	(0x1 << 16)
#define PCIE_PORT_FAST_LINK_MODE	(0x1 << 7)


#define PCIE_AUX_CLK_FREQ_OFF		0xB40
#define PCIE_AUX_CLK_FREQ_24MHZ		0x18
#define PCIE_AUX_CLK_FREQ_26MHZ		0x1A
#define PCIE_L1_SUBSTATES_OFF		0xB44
#define PCIE_L1_SUB_VAL			0xEA

#define PCIE_MSI_CTRL_INTR0_ENABLE	0x2D00
#define PCIE_MSI_CTRL_INTR0_MASK	0x2D04
#define PCIE_MSI_CTRL_INTR0_STATUS	0x2D08
#define PCIE_MSI_CTRL_INT_OFFSET	0x2000


#define PCIE_GEN3_RELATED_OFF		0x890
#define PCIE_GEN3_EQUALIZATION_DISABLE	(0x1 << 16)

#define DBI_SHADOW_BASE_ADDR		0x200000

/* PCIe SYSREG registers */
#define HSI0_PLL_REG0			0x600
#define HSI0_PLL_FOUTEN			(0x1 << 8)
#define HSI0_PLL_REG1			0x604
#define HSI0_PLL_FOUTPOSTDIVEN		(0x1)
#define HSI0_PLL_REG2			0x608
#define HSI0_PLL_PLLEN			(0x1 << 24)
#define HSI0_CLKBUF0_REG0		0x620
#define HSI0_CLKBUF1_REG0		0x630
#define HSI0_CLKBUF2_REG0		0x640
#define HSI0_CLKBUF3_REG0		0x650
#define HSI0_CLKBUF_IMP_CTRL		(0x1 << 0)

#define PCIE_SYSREG_SHARABILITY_CTRL	0x700
#define PCIE_SYSREG_SHARABLE_OFFSET	8
#define PCIE_SYSREG_SHARABLE_ENABLE	0x3
#define PCIE_SYSREG_SHARABLE_DISABLE	0x0
#define PCIE_SYSREG_SHARABLE_RW_ENABLE	0xC
#define PCIE_SYSREG_SHARABLE_RW_OFFSET	0x10
#define PCIE_SYSREG_SHARABLE_CH_OFFSET	0x4

#define HSI0_PCIE_GEN5_PHY_PWRDWN_4L	0x670
#define HSI0_PCIE_GEN5_PHY_PWRDWN_2L	0x4
#define HSI0_PCIE_PHY_TEST_PWRDWN_MSK	0x1
#define HSI0_PCIE_PHY_TEST_PWRDWN_EN	0x1

#define HSI0_PCIE_GEN5_4LA_PHY_CTRL	0x828
#define HSI0_PCIE_GEN5_4LB_PHY_CTRL	0x848
#define HSI0_PCIE_GEN5_2LA_PHY_CTRL	0x868
#define HSI0_PCIE_GEN5_2LB_PHY_CTRL	0x888
#define HSI0_PCIE_IP_CTRL_DEV_TYPE_RC_A	(0x4 << 24)
#define HSI0_PCIE_IP_CTRL_DEV_TYPE_RC_B	(0x4 << 25)
#define HSI0_PCIE_IP_CTRL_SS_MODE	(0x1 << 28)
#define HSI0_PCIE_IP_CTRL_PERST_MASK	0xf0
#define HSI0_PCIE_IP_CTRL_PERST_INIT	(0x1 << 4)
#define HSI0_PCIE_IP_CTRL_PERST_IN	(0x3 << 4)

//CMU
#define CMU_QCH_DOWN			0x4
#define CMU_QCH_UP			0x6

/*	SOC Control	*/
#define PCIE_REFCLK_CTRL_SOC_OPTION_0		0xA200
#define PCIE_REFCLK_OPTION0_RC			0x103F5
#define PCIE_HISTORY_REG(x)			(0x9200 + ((x) * 0x4))
#define PCIE_HISTORY_LTSSM_STATE(x)		(((x) >> 24) & 0xff)
#define PCIE_HISTORY_PHY_LOCK(x)		(((x) >> 16) & 0xF)
#define PCIE_HISTORY_L1SUB_STATE(x)		(((x) >> 20) & 0xF)

#define PCIE_DEBUG_MODE				0x9000
#define PCIE_DEBUG_SIGNAL_MASK			0x9004
#define PCIE_DEBUG_INTERRUPT_DIRECT_GEN_COND	0x9020
#define PCIE_DEBUG_REG_0			0x9200

enum debug_mode {
	DEBUG_MODE_0  = 0,
	DEBUG_MODE_11 = 11,
	DEBUG_STM_MODEM_EN = 12,
	DEBUG_BUFFER_MODE_EN = 13,
	DEBUG_STORE_START = 15,
	DEBUG_BUFFER_CLEAR = 24,
	DEBUG_STORE_MODE = 25,
	HSI_DEBUG_EN = 31,
};

enum debug_siganl_mask {
	PHY_TRACE_DYNAMIC_0 = 0,
	PHY_TRACE_DYNAMIC_3 = 3,
	PHY_TRACE_FIXED_0 = 4,
	PHY_TRACE_FIXED_3 = 7,
	PHY_TRACE_ESSENTIAL_0 = 8,
	PHY_TRACE_ESSENTIAL_3 = 11,
	PHY_DTB_0 = 12,
	PHY_DTB_3 = 15,
	PHY_LOCK_STATE_0 = 16,
	PHY_LOCK_STATE_3 = 19,
	L1_SUB_STATE_0 = 20,
	L1_SUB_STATE_3 = 23,
	LTSSM_STATE_0 = 24,
	LTSSM_STATE_7 = 31,
};

#define PCIE_REFCLK_CTRL_SOC_OPTION_1		0xA204
#define PCIE_REFCLK_OPTION1_RC			0x30C00

/* ETC definitions */
#define	PCIE_DISABLE_CLOCK		0
#define	PCIE_ENABLE_CLOCK		1
#define PCIE_IS_IDLE			1
#define PCIE_IS_ACTIVE			0

/* SR-IOV Default */
#define VF_OFFSET			0x8
#define VF_STRIDE			0x1

/* PCIe PHY definitions */
#define LN0_TX				0x8010
#define LN1_TX				0x8810
#define LN2_TX				0x9010
#define LN3_TX				0x9810

#define LN0_RX				0x8064
#define LN1_RX				0x8864
#define LN2_RX				0x9064
#define LN3_RX				0x9864

#define PCIE_ATU_CR1_OUTBOUND0		0x00000
#define PCIE_ATU_CR2_OUTBOUND0		0x00004
#define PCIE_ATU_LOWER_BASE_OUTBOUND0	0x00008
#define PCIE_ATU_UPPER_BASE_OUTBOUND0	0x0000C
#define PCIE_ATU_LIMIT_OUTBOUND0	0x00010
#define PCIE_ATU_LOWER_TARGET_OUTBOUND0	0x00014
#define PCIE_ATU_UPPER_TARGET_OUTBOUND0	0x00018

#define PCIE_ATU_CR1_OUTBOUND1		0x00200
#define PCIE_ATU_CR2_OUTBOUND1		0x00204
#define PCIE_ATU_LOWER_BASE_OUTBOUND1	0x00208
#define PCIE_ATU_UPPER_BASE_OUTBOUND1	0x0020C
#define PCIE_ATU_LIMIT_OUTBOUND1	0x00210
#define PCIE_ATU_LOWER_TARGET_OUTBOUND1	0x00214
#define PCIE_ATU_UPPER_TARGET_OUTBOUND1	0x00218

#define PCIE_ATU_OUTBOUND_OFFSET	0x200

//Maximum Payload supported
#define EXYNOS_PCIE_EVT01_MAX_PAYLOAD	256
#define EXYNOS_PCIE_EVT2_MAX_PAYLOAD	1024

#define HARD_ACCESS_TOTAL		1000000
#define HARD_ACCESS_PRINT_INTERVAL	5000
void exynos_v920_pcie_phy_init(struct pcie_port *pp);
void exynos_v920_pcie_poweroff(int ch_num);
int exynos_v920_pcie_poweron(int ch_num);

#ifdef CONFIG_EXYNOS_PCIE_IOMMU
extern void pcie_sysmmu_enable(int ch_num);
extern void pcie_sysmmu_disable(int ch_num);
extern int pcie_iommu_map(int ch_num, unsigned long iova, phys_addr_t paddr,
		size_t size, int prot);
extern size_t pcie_iommu_unmap(int ch_num, unsigned long iova, size_t size);

extern const struct dma_map_ops exynos_pcie_dma_ops;
#else
static void __maybe_unused pcie_sysmmu_enable(int ch_num)
{
	pr_err("PCIe SysMMU is NOT Enabled!!!\n");
}
static void __maybe_unused pcie_sysmmu_disable(int ch_num)
{
	pr_err("PCIe SysMMU is NOT Enabled!!!\n");
}
static int __maybe_unused pcie_iommu_map(int ch_num, unsigned long iova,
		phys_addr_t paddr, size_t size, int prot)
{
	pr_err("PCIe SysMMU is NOT Enabled!!!\n");

	return -ENODEV;
}
static size_t __maybe_unused pcie_iommu_unmap(int ch_num, unsigned long iova,
						size_t size)
{
	pr_err("PCIe SysMMU is NOT Enabled!!!\n");

	return -ENODEV;
}
#endif

#endif
