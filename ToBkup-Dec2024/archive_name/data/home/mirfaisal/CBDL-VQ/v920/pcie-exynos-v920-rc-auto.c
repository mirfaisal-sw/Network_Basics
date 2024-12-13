// SPDX-License-Identifier: GPL-2.0
/**
 * pcie-exynos-rc-auto.c - Exynos PCIe RC driver
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Authors: Sanghoon Bae <sh86.bae@samsung.com>
 *	    Jiheon Oh <jiheon.oh@samsung.com>
 *
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/of_address.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/resource.h>
#include <linux/signal.h>
#include <linux/types.h>
#include <linux/exynos-pci-noti.h>
#include <linux/pm_qos.h>
#include <dt-bindings/pci/pci.h>
#include "../pcie-designware.h"
#include "pcie-exynos-v920-rc-auto.h"
#include <linux/kthread.h>
#include <linux/random.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/iommu.h>
#include <linux/samsung_iommu.h>
#include "pcie-exynos-v920-fw.h"

#ifdef CONFIG_S2MPU
#include <vlx/s2mpu-protection.h>
#include <vlx/s2mpu-protection-v9.h>
#include <vlx/s2mpu-version.h>
#endif

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
#include "../../pci.h"
#endif

#ifdef MSI_TARGET_FROM_DT
#define MODIFY_MSI_ADDR
#endif

struct exynos_pcie g_pcie_v920[MAX_RC_NUM];

#define PCIE_EXYNOS_OP_READ(base)				\
static inline u32 exynos_##base##_read					\
	(struct exynos_pcie *pcie, u32 reg)				\
{									\
		u32 data = 0;						\
		data = readl(pcie->base##_base + reg);			\
		return (u32)data;					\
}

#define PCIE_EXYNOS_OP_WRITE(base, type)				\
static inline void exynos_##base##_write				\
	(struct exynos_pcie *pcie, type value, type reg)		\
{									\
		writel(value, pcie->base##_base + reg);			\
}

PCIE_EXYNOS_OP_READ(elbi);
PCIE_EXYNOS_OP_READ(phy);
PCIE_EXYNOS_OP_READ(sysreg);
PCIE_EXYNOS_OP_READ(ia);
PCIE_EXYNOS_OP_READ(atu);
PCIE_EXYNOS_OP_WRITE(elbi, u32);
PCIE_EXYNOS_OP_WRITE(phy, u32);
PCIE_EXYNOS_OP_WRITE(sysreg, u32);
PCIE_EXYNOS_OP_WRITE(ia, u32);
PCIE_EXYNOS_OP_WRITE(atu, u32);

static int exynos_v920_pcie_resumed_phydown(struct pcie_port *pp);
static int exynos_v920_pcie_assert_phy_reset(struct pcie_port *pp);
static void exynos_v920_pcie_send_pme_turn_off(struct exynos_pcie *exynos_pcie);
static int exynos_v920_pcie_wr_own_conf(struct pcie_port *pp, int where,
					int size, u32 val);
static int exynos_v920_pcie_rd_own_conf(struct pcie_port *pp, int where,
					int size, u32 *val);
static int exynos_v920_pcie_rd_other_conf(struct pcie_port *pp,
		struct pci_bus *bus, u32 devfn, int where, int size, u32 *val);
static int exynos_v920_pcie_wr_other_conf(struct pcie_port *pp,
		struct pci_bus *bus, u32 devfn, int where, int size, u32 val);
static void exynos_v920_pcie_prog_viewport_cfg(struct pcie_port *pp,
						u32 busdev, u32 type);
static void exynos_v920_pcie_prog_viewport_mem_outbound(struct pcie_port *pp);
static int exynos_v920_pcie_write_other(struct exynos_pcie *exynos_pcie,
					void __iomem *addr, int size, u32 val);
static int exynos_v920_pcie_read_other(struct exynos_pcie *exynos_pcie,
					void __iomem *addr, int size, u32 *val);
static int exynos_v920_pcie_link_up(struct dw_pcie *pci);
#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
static void exynos_v920_pcie_prog_viewport_mem_outbound_for_vf(
	struct pcie_port *pp, int vf_num, u64 base_addr, u64 target_addr,
	u32 size, u64 vf_bar_size);
#endif

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Dump register info"
 * @logic "Dump register info of RC driver"
 * @params
 * @param{in, ch_num, int, 0~5}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_register_dump(int ch_num)
{
	struct exynos_pcie *exynos_pcie = &g_pcie_v920[ch_num];
	struct dw_pcie *pci = exynos_pcie->pci;
	u32 i, j;

	/* Link Reg : 0x0 ~ 0x304 */
	dev_info(pci->dev, "Print ELBI region...\n");
	for (i = 0; i <= (PCIE_LINK_DBG_2 / 0x10); i++) {
		for (j = 0; j < 4; j++) {
			if (((i * 0x10) + (j * 4)) <= PCIE_LINK_DBG_2) {
				dev_info(pci->dev, "ELBI 0x%04x : 0x%08x\n",
					(i * 0x10) + (j * 4),
					exynos_elbi_read(exynos_pcie,
						(i * 0x10) + (j * 4)));
			}
		}
	}
	dev_info(pci->dev, "\n");

	/* PHY Reg : 0x0 ~ 0x384 */
	dev_info(pci->dev, "Print PHY region...\n");
	for (i = 0; i <= PCIE_PIPE_LANEX_LANEPLL_BYPASS_MODE_CTRL_1; i += 4) {
		dev_info(pci->dev, "PHY PMA 0x%04x : 0x%08x\n",
			i, readl(exynos_pcie->e32_phy_base + i));

	}
	dev_info(pci->dev, "\n");
}
EXPORT_SYMBOL(exynos_v920_pcie_register_dump);

static int chk_pcie_link_v920(struct exynos_pcie *exynos_pcie)
{
	struct dw_pcie *pci = exynos_pcie->pci;
	u32 val;
	int test_result = 0;

	exynos_v920_pcie_poweron(exynos_pcie->ch_num);
	val = exynos_elbi_read(exynos_pcie,
				PCIE_LINK_DBG_2) & PCIE_LINK_LTSSM_MASK;
	if (val >= S_RCVRY_LOCK && val <= S_L1_IDLE) {
		dev_info(pci->dev, "PCIe link test Success.\n");
	} else {
		dev_info(pci->dev, "PCIe Link test Fail...\n");
		test_result = -EPERM;
	}

	return test_result;
}

static int chk_dbi_access_v920(struct exynos_pcie *exynos_pcie)
{
	u32 val;
	int test_result = 0;
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;

	exynos_v920_pcie_wr_own_conf(pp, PCI_COMMAND, 4,
					PCI_COMMAND_DBI_ACCESS_CMD);
	exynos_v920_pcie_rd_own_conf(pp, PCI_COMMAND, 4, &val);
	if ((val & 0xfff) == PCI_COMMAND_DBI_ACCESS_CMD) {
		dev_info(pci->dev, "PCIe DBI access Success.\n");
	} else {
		dev_info(pci->dev, "PCIe DBI access Fail...\n");
		test_result = -EPERM;
	}

	return test_result;
}

static int chk_epconf_access_v920(struct exynos_pcie *exynos_pcie)
{
	u32 val = 0;
	int test_result = 0;
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;
	struct pci_bus *ep_pci_bus;

	ep_pci_bus = pci_find_bus(exynos_pcie->pci_dev->bus->domain_nr, 1);
	if (!ep_pci_bus) {
		dev_err(pci->dev, "ep_pci_bus is NULL\n");
		return -EINVAL;
	}

	exynos_v920_pcie_wr_other_conf(pp, ep_pci_bus,
					0, PCI_COMMAND, 4,
					PCI_COMMAND_DBI_OTH_ACCESS_CMD);
	exynos_v920_pcie_rd_other_conf(pp, ep_pci_bus,
					0, PCI_COMMAND, 4, &val);
	if ((val & 0xfff) == PCI_COMMAND_DBI_OTH_ACCESS_CMD) {
		dev_info(pci->dev, "PCIe DBI access Success.\n");
	} else {
		dev_info(pci->dev, "PCIe DBI access Fail...\n");
		test_result = -EPERM;
	}

	return test_result;
}

static int chk_epmem_access_v920(struct exynos_pcie *exynos_pcie)
{
	u32 val = 0;
	int test_result = 0;
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;

	struct pci_bus *ep_pci_bus;
	void __iomem *reg_addr;

	ep_pci_bus = pci_find_bus(exynos_pcie->pci_dev->bus->domain_nr, 1);
	if (!ep_pci_bus) {
		dev_err(pci->dev, "ep_pci_bus is NULL\n");
		return -EINVAL;
	}

	exynos_v920_pcie_wr_other_conf(pp, ep_pci_bus, 0, PCI_BASE_ADDRESS_0,
				4, lower_32_bits(exynos_pcie->mem_base));
	exynos_v920_pcie_rd_other_conf(pp, ep_pci_bus, 0, PCI_BASE_ADDRESS_0,
				4, &val);
	dev_info(pci->dev, "Set BAR0 : 0x%x\n", val);

	reg_addr = ioremap(exynos_pcie->mem_base, SZ_4K);

	val = readl(reg_addr);
	iounmap(reg_addr);
	if (val != 0xffffffff) {
		dev_info(pci->dev, "PCIe EP Outbound mem access Success.\n");
	} else {
		dev_info(pci->dev, "PCIe EP Outbound mem access Fail...\n");
		test_result = -EPERM;
	}

	return test_result;
}

static int chk_epmem_outofbar_access_v920(struct exynos_pcie *exynos_pcie)
{
	u32 val = 0;
	int test_result = 0;
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;

	struct pci_bus *ep_pci_bus;
	void __iomem *reg_addr;

	struct pci_dev *ep_pci_dev[PCIE_VF_MAX];

	unsigned long flags;
	resource_size_t start, end;

	u32 vendor_id, device_id;

	int size = 0;

	ep_pci_bus = pci_find_bus(exynos_pcie->pci_dev->bus->domain_nr, 1);
	if (!ep_pci_bus) {
		dev_err(pci->dev, "ep_pci_bus is NULL\n");
		return -EINVAL;
	}

	exynos_v920_pcie_wr_other_conf(pp, ep_pci_bus, 0, PCI_BASE_ADDRESS_0,
				4, lower_32_bits(exynos_pcie->mem_base));
	exynos_v920_pcie_rd_other_conf(pp, ep_pci_bus, 0, PCI_BASE_ADDRESS_0,
				4, &val);
	dev_info(pci->dev, "Set BAR0 : 0x%x\n", val);

	vendor_id = 0;
	device_id = 0;

	if (exynos_v920_pcie_rd_other_conf(pp, ep_pci_bus, 0, PCI_VENDOR_ID, 4,
		&val) == 0) {
		vendor_id = val & ID_MASK;
		device_id = (val >> 0x10) & ID_MASK;
		dev_info(pci->dev, "EP vendor_id: 0x%x device_id: 0x%x\n",
				vendor_id, device_id);
	} else {
		dev_err(pci->dev, "Failed to get device info\n");
		return -ENODEV;
	}


	ep_pci_dev[PCIE_PF] = pci_get_device(
				vendor_id, device_id, NULL);
	if (ep_pci_dev[PCIE_PF] == NULL) {
		dev_err(pci->dev, "Failed to get ep pci device\n");
		return -ENODEV;
	}

	flags = ep_pci_dev[PCIE_PF]->resource[0].flags;

	/* Get size of BAR0 */
	if (flags > 0) {
		start = ep_pci_dev[PCIE_PF]->resource[0].start;
		end = ep_pci_dev[PCIE_PF]->resource[0].end;
		dev_info(pci->dev, "BAR %d: [mem 0x%llx-0x%llx",
			0, start, end);

		size = end - start;
	}

	/* Set region outside of BAR */
	reg_addr = ioremap(exynos_pcie->mem_base + size + 1, SZ_4K);

	dev_info(pci->dev, "Access test to 0x%x ...\n",
				(uint) exynos_pcie->mem_base + size + 1);

	val = readl(reg_addr);
	iounmap(reg_addr);
	if (val != 0xffffffff) {
		dev_info(pci->dev, "PCIe EP Outbound mem access Success.\n");
	} else {
		dev_info(pci->dev, "PCIe EP Outbound mem access Fail...\n");
		test_result = -EPERM;
	}

	return test_result;
}

static int chk_link_recovery_v920(struct exynos_pcie *exynos_pcie)
{
	struct dw_pcie *pci = exynos_pcie->pci;
	u32 val;
	int test_result = 0;

	exynos_elbi_write(exynos_pcie, (0x1) << PCIE_EXIT_ASPM_L1_BIT,
			PCIE_APP_REQ_EXIT_L1);
	dev_info(pci->dev, "Before set perst, gpio val = %d\n",
		gpio_get_value(exynos_pcie->perst_gpio));
	gpio_set_value(exynos_pcie->perst_gpio, 0);
	dev_info(pci->dev, "After set perst, gpio val = %d\n",
		gpio_get_value(exynos_pcie->perst_gpio));
	exynos_elbi_write(exynos_pcie, (0x0) << PCIE_EXIT_ASPM_L1_BIT,
			PCIE_APP_REQ_EXIT_L1);
	msleep(5000);

	val = exynos_elbi_read(exynos_pcie,
				PCIE_LINK_DBG_2) & PCIE_LINK_LTSSM_MASK;
	if (val >= S_RCVRY_LOCK && val <= S_L1_IDLE) {
		dev_info(pci->dev, "PCIe link Recovery test Success.\n");
	} else {
		/* If recovery callback is defined, pcie poweron
		 * function will not be called.
		 */
		exynos_v920_pcie_poweroff(exynos_pcie->ch_num);
		exynos_v920_pcie_poweron(exynos_pcie->ch_num);
		val = exynos_elbi_read(exynos_pcie,
				PCIE_LINK_DBG_2) & PCIE_LINK_LTSSM_MASK;
		if (val >= S_RCVRY_LOCK && val <= S_L1_IDLE) {
			dev_info(pci->dev,
				"PCIe link Recovery test Success.\n");
		} else {
			dev_info(pci->dev, "PCIe Link Recovery test Fail...\n");
			test_result = -EPERM;
		}
	}

	return test_result;
}

static int chk_pcie_dislink_v920(struct exynos_pcie *exynos_pcie)
{
	struct dw_pcie *pci = exynos_pcie->pci;
	u32 val;
	int test_result = 0;

	exynos_v920_pcie_poweroff(exynos_pcie->ch_num);

	val = exynos_elbi_read(exynos_pcie,
				PCIE_LINK_DBG_2) & PCIE_LINK_LTSSM_MASK;
	if (val == S_L2_IDLE) {
		dev_info(pci->dev, "PCIe link Down test Success.\n");
	} else {
		dev_info(pci->dev, "PCIe Link Down test Fail...\n");
		test_result = -EPERM;
	}

	return test_result;
}

static int chk_hard_access_v920(struct exynos_pcie *exynos_pcie,
				void __iomem *check_mem)
{
	struct dw_pcie *pci = exynos_pcie->pci;
	u64 count = 0;
	u32 chk_val[10];
	int i, test_result = 0;
	int ret;

	memset(&chk_val, 0, sizeof(chk_val));

	while (count < HARD_ACCESS_TOTAL) {
		for (i = 0; i < 10; i++) {
			ret = exynos_v920_pcie_read_other(exynos_pcie,
					check_mem + (i * 4), 4, &chk_val[i]);
			if (ret < 0)
				return -EPERM;
		}
		count++;
		usleep_range(100, 101);
		if (count % HARD_ACCESS_PRINT_INTERVAL == 0) {
			dev_info(pci->dev,
				"================================\n");
			for (i = 0; i < 10; i++)
				dev_info(pci->dev, "Check Value : 0x%x\n",
					chk_val[i]);

			dev_info(pci->dev,
				"================================\n");
		}

		/* Check Value */
		for (i = 0; i < 10; i++) {
			if (chk_val[i] != 0xffffffff)
				break;

			if (i == 9)
				test_result = -1;
		}
	}

	return test_result;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Get PCIe link info"
 * @logic "Find EP device\n
 *	print configuration of EP device"
 * @params
 * @param{in, exynos_pcie, struct ::exynos_pcie *, not NULL}
 * @endparam
 * @retval{ret, int, 0, not NULL, not 0}
 */
static int get_pcie_link_info_v920(struct exynos_pcie *exynos_pcie)
{
	int i, num_vfs, domain_nr;
	u32 val = 0;
	u32 vendor_id, device_id;
	unsigned long flags;
	resource_size_t start, end;
	char sInfo[100];
#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	u16 offset, stride;
	int j;
	u64 size;
#endif
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;
	struct pci_bus *ep_pci_bus;
	struct pci_dev *ep_pci_dev[PCIE_VF_MAX];

	if (exynos_pcie->state == STATE_LINK_UP) {
		exynos_v920_pcie_rd_own_conf(pp, PCI_VENDOR_ID, 4, &val);
		vendor_id = val & ID_MASK;
		device_id = (val >> 16) & ID_MASK;
		dev_info(pci->dev, "RC vendor_id: 0x%x device_id: 0x%x\n",
				vendor_id, device_id);

		exynos_pcie->pci_dev = pci_get_device(vendor_id,
				device_id, NULL);
		if (exynos_pcie->pci_dev == NULL) {
			dev_err(pci->dev, "Failed to get pci device\n");
			return -ENODEV;
		}
		domain_nr = exynos_pcie->pci_dev->bus->domain_nr;
		ep_pci_bus = pci_find_bus(domain_nr, 1);
		if (ep_pci_bus == NULL) {
			dev_err(pci->dev, "Failed to get ep pci bus\n");
			return -ENODEV;
		}

		if (exynos_v920_pcie_rd_other_conf(pp, ep_pci_bus, 0,
				PCI_VENDOR_ID, 4, &val) == 0) {
			vendor_id = val & ID_MASK;
			device_id = (val >> 0x10) & ID_MASK;
			dev_info(pci->dev,
				"EP vendor_id: 0x%x device_id: 0x%x\n",
				vendor_id, device_id);
		} else {
			dev_err(pci->dev,
				"Failed to get device info\n");
			return -ENODEV;
		}

		ep_pci_dev[PCIE_PF] = pci_get_device(
				vendor_id, device_id, NULL);
		if (ep_pci_dev[PCIE_PF] == NULL) {
			dev_err(pci->dev, "Failed to get ep pci device\n");
			return -ENODEV;
		}

		if (exynos_v920_pcie_rd_own_conf(pp,
				PCIE_LINK_CTRL_STAT, 4, &val) == 0) {
			val = (val >> 0x10) & 0xf;

			dev_info(pci->dev, "Current Link Speed is");
			dev_info(pci->dev, "GEN%d (MAX GEN%d)\n",
				val, exynos_pcie->max_link_speed);

			num_vfs = pci_num_vf(ep_pci_dev[PCIE_PF]);
		} else {
			dev_err(pci->dev,
				"Failed to get device info\n");
			return -ENODEV;
		}

		for (i = PCI_STD_RESOURCES; i <= PCI_STD_RESOURCE_END; i++) {
			flags = ep_pci_dev[PCIE_PF]->resource[i].flags;
			if (flags > 0) {
				start = ep_pci_dev[PCIE_PF]->resource[i].start;
				end = ep_pci_dev[PCIE_PF]->resource[i].end;
				if (sprintf(sInfo, "BAR %d: [mem 0x%llx-0x%llx",
					i, start, end) < 0) {
					dev_err(pci->dev,
						"sprintf failed\n");
					return -EINVAL;
				}
				if (flags & IORESOURCE_MEM_64) {
					dev_info(pci->dev,
						"%s 64bit]\n", sInfo);
				} else {
					dev_info(pci->dev,
						"%s 32bit]\n", sInfo);
				}
			}
		}

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
		if (num_vfs && num_vfs > PCIE_VF4) {
			dev_err(pci->dev, "Number of VFs: %d is over 4.\n",
				num_vfs);
			return -EINVAL;
		}

		dev_info(pci->dev, "Number of VFs: %d\n", num_vfs);
		offset = ep_pci_dev[PCIE_PF]->sriov->offset;
		stride = ep_pci_dev[PCIE_PF]->sriov->stride;
		for (j = 1; j <= num_vfs; j++) {
			ep_pci_dev[j] = pci_get_domain_bus_and_slot(
					domain_nr, 1,
					offset + stride * (j-1));
			if (ep_pci_dev[j] == NULL) {
				sprintf(sInfo,
					"Failed to get vf dev%d\n", j);
				dev_err(pci->dev, sInfo);
				return -ENODEV;
			}

			for (i = PCI_IOV_RESOURCES;
				i <= PCI_IOV_RESOURCE_END; i++) {
				flags =
				ep_pci_dev[PCIE_PF]->resource[i].flags;
				if (flags > 0) {
					size =
					ep_pci_dev[PCIE_PF]->sriov->barsz
						[i - PCI_IOV_RESOURCES];

					start =
					ep_pci_dev[PCIE_PF]->resource[i].start;
					sprintf(sInfo, "VF%d BAR %d: [mem 0x%llx-0x%llx ",
						j, i-PCI_IOV_RESOURCES,
						start + size * (j - 1),
						start + (size * j) - 1);
					if (flags & IORESOURCE_MEM_64)
						dev_info(pci->dev,
							"%s 64bit]\n", sInfo);
					else
						dev_info(pci->dev,
							"%s 32bit]\n", sInfo);
				}
			}
		}
#endif
		return 0;
	}

	dev_warn(pci->dev, "Link up is not established\n");
	return -EPERM;
}

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_sriov}
 * @purpose "Find ep device"
 * @logic "Find ep device from bus number\n
 *	Return ep device."
 * @params
 * @param{in, exynos_pcie, struct ::exynos_pcie *, not NULL}
 * @param{in, bus_num, int, 0~255}
 * @endparam
 * @retval{ep_pci_dev, struct ::pci_dev *, NULL, not NULL, NULL}
 */
static struct pci_dev *exynos_v920_pcie_get_pci_dev
	(struct exynos_pcie *exynos_pcie, int bus_num)
{
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;
	struct pci_bus *ep_pci_bus = NULL;
	struct pci_dev *ep_pci_dev = NULL;
	u32 val = 0;

	ep_pci_bus = pci_find_bus(exynos_pcie->pci_dev->bus->domain_nr,
		bus_num);
	exynos_v920_pcie_rd_other_conf(pp, ep_pci_bus, 0, PCI_VENDOR_ID,
					4, &val);
	dev_info(pci->dev, "[%s] Vendor: 0x%x Device: 0x%x\n",
		__func__, val & ID_MASK, (val >> HDR_ID_DID_BIT) & ID_MASK);
	ep_pci_dev = pci_get_device(val & ID_MASK,
				(val >> HDR_ID_DID_BIT) & ID_MASK, NULL);

	return ep_pci_dev;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_sriov}
 * @purpose "Print pci device information"
 * @logic "Print pci bdf number and vendor id, device id, msi setting"
 * @params
 * @param{in, pci_dev, struct ::pci_dev *, not NULL}
 * @endparam
 * @noret
 */
static void print_pci_dev_info_v920(struct pci_dev *pci_dev)
{
	if (pci_dev != NULL) {
		dev_info(NULL, "###############################\n");
		dev_info(NULL, "pci %04d:%02d:%02d.%d\n",
			pci_dev->bus->domain_nr, pci_dev->bus->number,
			PCI_SLOT(pci_dev->devfn), PCI_FUNC(pci_dev->devfn));
		dev_info(NULL, "Vendor: 0x%x, Device: 0x%x\n",
			pci_dev->vendor, pci_dev->device);
		dev_info(NULL, "MSI Capability - enabled: %d\n",
			pci_dev->msi_enabled);
		dev_info(NULL, "MSI-X Capability - enabled: %d\n",
			pci_dev->msix_enabled);
		dev_info(NULL, "###############################\n");
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_sriov}
 * @purpose "Handler for msi interrupt in VM"
 * @logic "Print irq log"
 * @params
 * @param{in, irq, int, >=0}
 * @param{in, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED, not IRQ_HANDLED}
 */
static irqreturn_t exynos_v920_pcie_msi_irq_handler_vm(int irq, void *arg)
{
	struct pcie_port *pp = arg;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

	dev_info(pci->dev, "[%s] irq:%d\n", __func__, irq);
	return IRQ_HANDLED;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_sriov}
 * @purpose "Set MSI interrupt for VM"
 * @logic "Allocate MSI interrupts\n
 *	Request irq."
 * @params
 * @param{in/out, exynos_pcie, struct ::exynos_pcie *, not NULL}
 * @param{in/out, pci_dev, struct ::pci_dev *, not NULL}
 * @param{in, vf_num, int, 0~4}
 * @param{in, int_num, int, 0~31}
 * @endparam
 * @retval{ret, int, 0, >=-ERANGE, -EFAULT/-ENOSPC}
 */
static int sriov_request_msi_v920(struct exynos_pcie *exynos_pcie,
	struct pci_dev *pci_dev, int vf_num, int int_num)
{
	struct dw_pcie *pci = exynos_pcie->pci;
	int ret = 0;
	int i, irq;

	if (pci_dev == NULL) {
		dev_err(pci->dev, "Failed to get ep pci device\n");
		return -EFAULT;
	}

	if (!exynos_pcie->sriov_msi_enable[vf_num]) {
		irq = pci_alloc_irq_vectors_affinity(pci_dev, MSIX_MIN_VEC,
				MSIX_MAX_VEC, PCI_IRQ_MSIX, NULL);
		if (irq < 0) {
			dev_err(pci->dev,
				"Can't get msi IRQ (err: %d)\n", ret);
			return ret;
		}

		for (i = 0; i < irq; i++) {
			ret = pci_irq_vector(pci_dev, i);
			ret = devm_request_irq(pci->dev,
					ret,
					exynos_v920_pcie_msi_irq_handler_vm,
					IRQF_SHARED, "exynos-pcie-vf",
					exynos_pcie);

			if (ret) {
				dev_err(pci->dev,
					"failed to request msi_irq[%d]\n",
					vf_num);
				return ret;
			}
		}

		exynos_pcie->sriov_msi_enable[vf_num] = 1;
	}
	return exynos_pcie->msi_irq[vf_num];
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_sriov}
 * @purpose "Creat SR-IOV VF"
 * @logic "Enable SR-IOV\n
 *	Set MSI interrupt for VM\n
 *	Set outbound ATU for VM\n
 *	Use vlink to update status to link up."
 * @params
 * @param{in/out, exynos_pcie, struct ::exynos_pcie *, not NULL}
 * @param{in, num_vfs, int, 0~4}
 * @endparam
 * @retval{-, int, 0, 0/-1, -1}
 */
static int sriov_create_vf_v920(struct exynos_pcie *exynos_pcie, int num_vfs)
{
	u32 val, devfn;
	int i, j, domain_nr, irq_num, reg;
	u16 offset, stride;
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;
	u64 vf_bar_size = 0;
	unsigned long flags;
	struct iommu_domain *domain;

	if (exynos_pcie->state == STATE_LINK_UP) {

		if (!exynos_pcie->sriov_supported) {
			exynos_pcie->ep_pci_dev[PCIE_PF] =
				exynos_v920_pcie_get_pci_dev(exynos_pcie, 1);
			if (exynos_pcie->ep_pci_dev[PCIE_PF] == NULL) {
				dev_err(pci->dev, "Failed to get pf_dev\n");
				return -ENODEV;
			}

			val = pci_find_ext_capability
				(exynos_pcie->ep_pci_dev[PCIE_PF],
				PCI_EXT_CAP_ID_SRIOV);
			if (val > 0) {
				exynos_pcie->sriov_supported = 1;
				dev_info(pci->dev,
					"[%s]sriov is supported\n",
					__func__);
			} else {
				dev_err(pci->dev,
					"[%s]sriov is not supported!\n",
					__func__);
				return -EPERM;
			}
		}

		if (exynos_pcie->ep_pci_dev[PCIE_PF] != NULL) {
			val = pci_vfs_assigned
					(exynos_pcie->ep_pci_dev[PCIE_PF]);
			if (val) {
				dev_info(pci->dev,
					"%d VFs are already created\n", val);
				return -1;
			}
		} else {
			dev_err(pci->dev, "Failed to get pf_dev\n");
			return -ENODEV;
		}
		domain_nr = exynos_pcie->pci_dev->bus->domain_nr;
#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
		offset = exynos_pcie->ep_pci_dev[PCIE_PF]->sriov->offset;
		stride = exynos_pcie->ep_pci_dev[PCIE_PF]->sriov->stride;
#else
		/* default value */
		offset = VF_OFFSET;
		stride = VF_STRIDE;
#endif

		for (i = PCIE_PF; i <= num_vfs; i++) {
			if (i == PCIE_PF) {
				devfn = 0;
				val = pci_enable_sriov
					(exynos_pcie->ep_pci_dev[PCIE_PF],
					num_vfs);
				if (val) {
					dev_err(pci->dev,
						"Failed to enable sriov %d\n",
						val);
					return -EPERM;
				}

				val = pci_num_vf
					(exynos_pcie->ep_pci_dev[PCIE_PF]);
				if (val != num_vfs && num_vfs > PCIE_VF4) {
					dev_err(pci->dev,
						"Failed to create vf %d\n",
						val);
					return -EPERM;
				}
			} else {
				devfn = offset + stride * (i-1);

				if (!exynos_pcie->sriov_msi_enable[i]) {
					exynos_pcie->ep_pci_dev[i] =
						pci_get_domain_bus_and_slot(
						domain_nr, 1, devfn);

					if (exynos_pcie->ep_pci_dev[i]
						== NULL) {
						dev_err(pci->dev, "Failed to");
						dev_err(pci->dev,
							"get vf%d_dev\n", i);
						return -ENODEV;
					}
				}
			}

			/* initialize current_state info */
			exynos_pcie->ep_pci_dev[i]->current_state = 0;

			pci_set_master(exynos_pcie->ep_pci_dev[i]);

			if (exynos_pcie->use_msi) {

				irq_num = sriov_request_msi_v920(exynos_pcie,
						exynos_pcie->ep_pci_dev[i], i,
						PCIE_VF_MSI_VECTOR_NUMBER);
				if (irq_num < 0) {
					dev_err(pci->dev, "[%s] Can't get MSI",
							__func__);
					dev_err(pci->dev, "irq (erro: %d)\n",
							irq_num);
					return -EPERM;
				}

				dev_info(pci->dev,
						"[%s] Assigned MSI irq[%d]\n",
						__func__, irq_num);

				/* enable MSI */
				reg = exynos_pcie->ep_pci_dev[i]->msi_cap +
					PCI_MSI_FLAGS;
				exynos_v920_pcie_rd_other_conf(pp,
					exynos_pcie->ep_pci_dev[PCIE_PF]->bus,
					devfn, reg, 2, &val);

				val |= PCI_MSI_FLAGS_ENABLE;

				exynos_v920_pcie_wr_other_conf(pp,
					exynos_pcie->ep_pci_dev[PCIE_PF]->bus,
					devfn, reg, 2, val);
			}

			print_pci_dev_info_v920(exynos_pcie->ep_pci_dev[i]);

			if (i > PCIE_PF)
				pci_set_dev_assigned
					(exynos_pcie->ep_pci_dev[i]);

			for (j = PCI_IOV_RESOURCES;
					j <= PCI_IOV_RESOURCE_END; j++) {
				flags =
					exynos_pcie->ep_pci_dev[PCIE_PF]->resource[j].flags;
				if (flags > 0) {
					vf_bar_size = exynos_pcie->ep_pci_dev[PCIE_PF]->sriov->barsz
						[j - PCI_IOV_RESOURCES];
				}
			}
			exynos_v920_pcie_prog_viewport_mem_outbound_for_vf(pp,
				i, exynos_pcie->mem_base,
				exynos_pcie->mem_bus_addr,
				PCIE_VF_OB_SIZE, vf_bar_size);
		}

		if (exynos_pcie->use_sysmmu) {
			domain = iommu_get_domain_for_dev(pci->dev);
			iommu_map(domain, GIC_TRANSLATER_ADDR,
				GIC_TRANSLATER_ADDR, SZ_4K,
				IOMMU_READ | IOMMU_WRITE);
		}
	} else {
		dev_err(pci->dev, "Link up is not established\n");
		return -EPERM;
	}
	return 0;
}
#endif

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Information for PCIe RC test"
 * @logic "Print PCIe RC test info"
 * @params
 * @param{in, dev, struct ::device *, not NULL}
 * @param{in, attr, struct ::device_attribute *, not NULL}
 * @param{out, buf, char *, not NULL}
 * @endparam
 * @retval{ret, ssize_t, 0, >=0, 0}
 */
static ssize_t pcie_sysfs_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	int ret = 0;

	ret += snprintf(buf + ret, PAGE_SIZE - ret, ">>>> PCIe Test <<<<\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "0 : PCIe Unit Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "1 : Link Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "2 : DisLink Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"9 : HardAccess Test\n");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"14 : SR-IOV Create Test\n");

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Handler for sysfs tests"
 * @logic "Parse test command from sysfs\n
 *	Perform test function."
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @param{in, attr, struct ::device_attribute *, not NULL}
 * @param{in, buf, char *, not NULL}
 * @param{in, count, size_t, >=0}
 * @endparam
 * @retval{count, ssize_t, >=-ERANGE, int, -EINVAL}
 */
static ssize_t pcie_sysfs_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int op_num, domain_nr;
	struct exynos_pcie *exynos_pcie = dev_get_drvdata(dev);
	u64 iommu_addr, iommu_size;
	int wifi_reg_on = of_get_named_gpio(dev->of_node,
		"gpio_wifi_reg_on", 0);
	struct pci_bus *ep_pci_bus;
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;
	u32 val;

	if (sscanf(buf, "%10d%llx%llx", &op_num, &iommu_addr, &iommu_size) == 0)
		return -EINVAL;

	switch (op_num) {
	case 0:
		dev_info(dev, "PCIe UNIT test START.\n");

		if (wifi_reg_on >= 0)
			gpio_set_value(wifi_reg_on, 1);

		mdelay(100);

		/* Test PCIe Link */
		if (chk_pcie_link_v920(exynos_pcie) != 0) {
			dev_info(dev, "PCIe UNIT test FAIL[1/7]!!!\n");
			break;
		}

		/* Test PCIe DBI access */
		if (chk_dbi_access_v920(exynos_pcie) != 0) {
			dev_info(dev, "PCIe UNIT test FAIL[2/7]!!!\n");
			break;
		}

		/* Test EP configuration access */
		if (chk_epconf_access_v920(exynos_pcie) != 0) {
			dev_info(dev, "PCIe UNIT test FAIL[3/7]!!!\n");
			break;
		}

		/* Test EP Outbound memory region */
		if (chk_epmem_access_v920(exynos_pcie) != 0) {
			dev_info(dev, "PCIe UNIT test FAIL[4/7]!!!\n");
			break;
		}

		/* PCIe Link recovery test */
		if (chk_link_recovery_v920(exynos_pcie) != 0) {
			dev_info(dev, "PCIe UNIT test FAIL[6/7]!!!\n");
			break;
		}

		/* PCIe DisLink Test */
		if (chk_pcie_dislink_v920(exynos_pcie) != 0) {
			dev_info(dev, "PCIe UNIT test FAIL[7/7]!!!\n");
			break;
		}

		dev_info(dev, "PCIe UNIT test SUCCESS!!!\n");
		break;

	case 1:
		if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT01 &&
			exynos_pcie->state == STATE_LINK_DOWN) {
			gpio_set_value(exynos_pcie->perst_gpio, 0);
			mdelay(100);
		}
		dev_dbg(dev, "PCIe Link up go\n");
		if (wifi_reg_on >= 0)
			gpio_set_value(wifi_reg_on, 1);
		exynos_v920_pcie_poweron(exynos_pcie->ch_num);
		break;

	case 2:
		exynos_v920_pcie_poweroff(exynos_pcie->ch_num);

		if (wifi_reg_on >= 0)
			gpio_set_value(wifi_reg_on, 0);
		break;

	case 3:
		get_pcie_link_info_v920(exynos_pcie);
		break;

	case 9:
		if (exynos_pcie->state == STATE_LINK_UP) {
			/*
			 * Hard access test is needed to check PHY configuration
			 * values. If PHY values are NOT stable, Link down
			 * interrupt will occur.
			 */

			/* Prepare to access EP memory */
			domain_nr = exynos_pcie->pci_dev->bus->domain_nr;
			ep_pci_bus = pci_find_bus(domain_nr, 1);
			if (!ep_pci_bus) {
				dev_err(pci->dev, "ep_pci_bus is NULL\n");
				return -EINVAL;
			}

			exynos_v920_pcie_wr_other_conf(pp, ep_pci_bus,
						0, PCI_COMMAND, 4, 0x146);
			exynos_v920_pcie_rd_other_conf(pp, ep_pci_bus,
						0, PCI_COMMAND, 4, &val);
			exynos_v920_pcie_wr_other_conf(pp, ep_pci_bus, 0,
					PCI_BASE_ADDRESS_0, 4,
					lower_32_bits(exynos_pcie->mem_base));
			exynos_v920_pcie_rd_other_conf(pp, ep_pci_bus, 0,
					PCI_BASE_ADDRESS_0, 4, &val);

			/* Hard Access Test start At Work Q */
			queue_work(exynos_pcie->pcie_wq,
				&exynos_pcie->hardaccess_work.work);
		} else {
			dev_warn(dev, "Link up is not established\n");
		}
		break;

	case 11:
		dev_info(pci->dev, "Before set perst, gpio val = %d\n",
				gpio_get_value(exynos_pcie->perst_gpio));
		gpio_set_value(exynos_pcie->perst_gpio, 0);
		dev_info(pci->dev, "After set perst, gpio val = %d\n",
				gpio_get_value(exynos_pcie->perst_gpio));
		break;

	case 12:
		dev_info(dev, "Link state:0x%x\n",
			 exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2));
		break;

	case 13:
		if (exynos_pcie->state == STATE_LINK_UP)
			chk_epmem_outofbar_access_v920(exynos_pcie);
		else
			dev_warn(dev, "Link up is not established\n");
		break;

	case 14:
#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
		sriov_create_vf_v920(exynos_pcie, exynos_pcie->num_vfs);
#else
		dev_warn(dev, "Please check CONFIG_PCI_IOV\n");
#endif
		break;

	default:
		dev_warn(dev, "Unsupported Test Number(%d)...\n", op_num);
		break;
	}

	return count;
}

static DEVICE_ATTR_RW(pcie_sysfs);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Create PCIe sys file"
 * @logic "Create PCIe RC device sys file"
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @endparam
 * @retval{-, int, 0, >=-EINVAL, not 0}
 */
static inline int create_pcie_sys_file_v920(struct device *dev)
{
	return device_create_file(dev, &dev_attr_pcie_sysfs);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Remove PCIe sys file"
 * @logic "Remove PCIe sysfs file"
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @endparam
 * @noret
 */
static inline void remove_pcie_sys_file_v920(struct device *dev)
{
	device_remove_file(dev, &dev_attr_pcie_sysfs);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Call back event for dislink work"
 * @logic "If callback event is registrated, perform event callback\n
 *	If not, force link down and link up the link."
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @param{in, event, int, 0~0x10}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_notify_callback(struct pcie_port *pp, int event)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	if (exynos_pcie->event_reg && exynos_pcie->event_reg->callback &&
			(exynos_pcie->event_reg->events & event)) {
		struct exynos_pcie_notify *notify =
			&exynos_pcie->event_reg->notify;
		notify->event = event;
		notify->user = exynos_pcie->event_reg->user;
		dev_info(pci->dev, "Callback for the event : %d\n", event);
		exynos_pcie->event_reg->callback(notify);
	} else {
		dev_info(pci->dev, "Client driver does not have");
		dev_info(pci->dev, "registration of the event : %d\n", event);

		exynos_v920_pcie_poweroff(exynos_pcie->ch_num);
#if PCIE_RC_LINKRESTORE
		dev_info(pci->dev, "Force PCIe poweroff --> poweron\n");
		exynos_v920_pcie_poweron(exynos_pcie->ch_num);
#endif
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Dump status in dislink work"
 * @logic "Dump information in dislink work"
 * @params
 * @param{in, ch_num, int, 0~5}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
int exynos_v920_pcie_dump_link_down_status(int ch_num)
{
	struct exynos_pcie *exynos_pcie = &g_pcie_v920[ch_num];
	struct dw_pcie *pci = exynos_pcie->pci;

	if (exynos_pcie->state == STATE_LINK_UP) {
		dev_info(pci->dev, "LTSSM: 0x%08x\n",
			exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2));
	} else
		dev_info(pci->dev, "PCIE link state is %d\n",
				exynos_pcie->state);

	return 0;
}
EXPORT_SYMBOL(exynos_v920_pcie_dump_link_down_status);

#ifdef USE_PANIC_NOTIFIER
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Dump link down status and register when panic occurs"
 * @logic "Dump link down status\n
 *	Dump register info."
 * @params
 * @param{in, nb, struct ::notifier_block *, not NULL}
 * @param{in, event, unsigned long, >0}
 * @param{in, data, void *, not NULL}
 * @endparam
 * @retval{ret, int, 0x0001, 0x0001~0x8002, >0x0001}
 */
static int exynos_v920_pcie_dma_mon_event(struct notifier_block *nb,
				unsigned long event, void *data)
{
	int ret;
	struct exynos_pcie *exynos_pcie =
		container_of(nb, struct exynos_pcie, ss_dma_mon_nb);

	ret = exynos_v920_pcie_dump_link_down_status(exynos_pcie->ch_num);
	if (exynos_pcie->state == STATE_LINK_UP)
		exynos_v920_pcie_register_dump(exynos_pcie->ch_num);

	return notifier_from_errno(ret);
}
#endif

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Enable/disable PCIe DBI HWACG"
 * @logic "Call clk_prepare_enable or clk_disable_unprepare"
 * @params
 * @param{in, dev, struct device, not NULL}
 * @param{in, dev, struct exynos_pcie_clks, not NULL}
 * @param{in, enable, bool, true/false}
 * @endparam
 * @retval{ret, int, 0, <=0, <0}
 */
static int exynos_v920_pcie_dbiclk_enable(struct device *dev,
					  struct exynos_pcie_clks *clks,
					  bool enable)
{
	int ret = 0;

	if (enable)
		ret = clk_prepare_enable(clks->dbi_hwacg);
	else
		clk_disable_unprepare(clks->dbi_hwacg);

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Get PCIe clock"
 * @logic "Get phy clock"
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0/-ENODEV, -ENODEV}
 */
static int exynos_v920_pcie_clock_get(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	struct exynos_pcie_clks	*clks = &exynos_pcie->clks;
	int i, total_clk_num, phy_count;
	int index = 0, offset = 0;
	int ret_hwacg = 0;

	/* Index[0] is for DBI HWACG enable/disable.
	 * V920 EVT2, EVT2.1 only.
	 * Need to check this in the next chip again.
	 */
	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
		clks->dbi_hwacg = devm_clk_get(pci->dev, "pcie_dbi_hwacg");
		if (IS_ERR(clks->dbi_hwacg)) {
			dev_err(pci->dev, "%s Fail: dbi_hwacg!\n", __func__);
			return -ENODEV;
		}

		offset = 1;
		dev_info(pci->dev, "%s CH%d dbi_hwacg(%d)\n", __func__,
			 exynos_pcie->ch_num, offset);

		ret_hwacg =
			exynos_v920_pcie_dbiclk_enable(pci->dev, clks, true);
		if (ret_hwacg)
			dev_err(pci->dev, "%s dbi_clk fail to enable(%d)\n",
				__func__, ret_hwacg);
		else
			dev_info(pci->dev, "%s dbi_clk(%d) enable\n",
				 __func__, offset);
	}

	/*
	 * CAUTION - PCIe and phy clock have to define in order.
	 * You must define related PCIe clock first in DT.
	 */
	total_clk_num = exynos_pcie->pcie_clk_num + exynos_pcie->phy_clk_num;

	for (i = 0; i < total_clk_num; i++) {
		index = i + offset;

		if (i < exynos_pcie->pcie_clk_num) {
			clks->pcie_clks[i] = of_clk_get(pci->dev->of_node,
							index);
			if (IS_ERR(clks->pcie_clks[i])) {
				dev_err(pci->dev, "Failed to get pcie clock\n");
				return -ENODEV;
			}
		} else {
			phy_count = i - exynos_pcie->pcie_clk_num;
			if (phy_count < 0) {
				dev_err(pci->dev,
					"phy_count value is negative\n");
				return -EINVAL;
			}
			clks->phy_clks[phy_count] =
				of_clk_get(pci->dev->of_node, index);
			if (IS_ERR(clks->phy_clks[phy_count])) {
				dev_err(pci->dev, "Failed to get pcie clock\n");
				return -ENODEV;
			}
		}
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Enable/disable PCIe clock"
 * @logic "Enable/disable PCIe clock"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in, enable, int, 0~1}
 * @endparam
 * @retval{ret, int, 0, <=0, <0}
 */
static int exynos_v920_pcie_clock_enable(struct pcie_port *pp, int enable)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	struct exynos_pcie_clks	*clks = &exynos_pcie->clks;
	int i;
	int ret = 0;

	if (enable) {
		for (i = 0; i < exynos_pcie->pcie_clk_num; i++)
			ret = clk_prepare_enable(clks->pcie_clks[i]);
	} else {
		for (i = 0; i < exynos_pcie->pcie_clk_num; i++)
			clk_disable_unprepare(clks->pcie_clks[i]);
	}

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Enable/disable PCIe phy clock"
 * @logic "Enable/disable PCIe phy clock"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in, enable, int, 0~1}
 * @endparam
 * @retval{ret, int, 0, <=0, <0}
 */
static int exynos_v920_pcie_phy_clock_enable(struct pcie_port *pp, int enable)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	struct exynos_pcie_clks	*clks = &exynos_pcie->clks;
	int i;
	int ret = 0;

	if (enable) {
		for (i = 0; i < exynos_pcie->phy_clk_num; i++)
			ret = clk_prepare_enable(clks->phy_clks[i]);
	} else {
		for (i = 0; i < exynos_pcie->phy_clk_num; i++)
			clk_disable_unprepare(clks->phy_clks[i]);
	}

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_link}
 * @purpose "Print link history"
 * @logic "Print link state history"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_print_link_history(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct device *dev = pci->dev;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	u32 history_buffer[32];
	int i;

	for (i = 31; i >= 0; i--)
		history_buffer[i] = readl(exynos_pcie->soc_ctrl_base +
				PCIE_HISTORY_REG(i));
	for (i = 31; i >= 0; i--) {
		dev_info(dev, "LTSSM: 0x%02x, L1sub: 0x%x, PHY Lock: 0x%x\n",
				PCIE_HISTORY_LTSSM_STATE(history_buffer[i]),
				PCIE_HISTORY_L1SUB_STATE(history_buffer[i]),
				PCIE_HISTORY_PHY_LOCK(history_buffer[i]));
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Setup RC configuration"
 * @logic "Disable RC bar\n
 *	Setup RC configuration."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_setup_rc(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	u32 pcie_cap_off = PCIE_CAP_OFFSET;
	u32 pm_cap_off = PM_CAP_OFFSET;
	u32 val;

	int ret;

	/* enable writing to DBI read-only registers */
	exynos_v920_pcie_wr_own_conf(pp, PCIE_MISC_CONTROL, 4,
				PCIE_MISC_DBI_RO_WR_EN);

	/* RC BAR disable */
	exynos_v920_pcie_rd_own_conf(pp,
			DBI_SHADOW_BASE_ADDR + PCI_BASE_ADDRESS_0, 4, &val);
	dev_info(pci->dev, "[%s] Before RC BAR0 = 0x%x\n", __func__, val);
	val &= ~(0x1);
	exynos_v920_pcie_wr_own_conf(pp,
			DBI_SHADOW_BASE_ADDR + PCI_BASE_ADDRESS_0, 4, val);
	dev_info(pci->dev, "[%s] After RC BAR0 = 0x%x\n", __func__, val);

	exynos_v920_pcie_rd_own_conf(pp, DBI_SHADOW_BASE_ADDR + PCI_ROM_ADDRESS,
				4, &val);
	dev_info(pci->dev, "[%s] Before RC Expansion ROM = 0x%x\n",
		__func__, val);
	val &= ~(PCI_ROM_ADDRESS_ENABLE);
	exynos_v920_pcie_wr_own_conf(pp, DBI_SHADOW_BASE_ADDR + PCI_ROM_ADDRESS,
				4, val);
	dev_info(pci->dev, "[%s] After RC Expansion ROM = 0x%x\n",
		__func__, val);

	/* change vendor ID and device ID for PCIe */
	exynos_v920_pcie_wr_own_conf(pp, PCI_VENDOR_ID, 2,
				     PCI_VENDOR_ID_SAMSUNG);
	exynos_v920_pcie_wr_own_conf(pp, PCI_DEVICE_ID,
				     2, PCI_DEVICE_ID_EXYNOS +
				     exynos_pcie->ch_num);

	/* set max link width & speed : Gen2, Lane1 */
	exynos_v920_pcie_rd_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCAP,
				     4, &val);
	val &= ~(PCI_EXP_LNKCAP_L1EL | PCI_EXP_LNKCAP_MLW | PCI_EXP_LNKCAP_SLS);
	/* Need to check */
	/* val |= PCI_EXP_LNKCAP_L1EL_64USEC | PCI_EXP_LNKCAP_MLW_X1; */
	val |= PCI_EXP_LNKCAP_L1EL_64USEC;
	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2)
		val |= PCI_EXP_LNKCTL2_TLS_16_0GB;
	else
		val |= PCI_EXP_LNKCTL2_TLS_8_0GB;

	exynos_v920_pcie_wr_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCAP, 4, val);

	/* set auxiliary clock frequency: */
	/* 26MHz default, clock for check timing */
	exynos_v920_pcie_wr_own_conf(pp, PCIE_AUX_CLK_FREQ_OFF, 4,
			PCIE_AUX_CLK_FREQ_26MHZ);

	/* set duration of L1.2 & L1.2.Entry : */
	/* L1 substates timing register.*/
	exynos_v920_pcie_wr_own_conf(pp, PCIE_L1_SUBSTATES_OFF,
	4, PCIE_L1_SUB_VAL);

	/* clear power management control and status register */
	exynos_v920_pcie_wr_own_conf(pp, pm_cap_off + PCI_PM_CTRL, 4, 0x0);

	/* set target speed from DT */
	exynos_v920_pcie_rd_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCTL2,
				     4, &val);
	val &= ~PCI_EXP_LNKCTL2_TLS;
	val |= exynos_pcie->max_link_speed;
	exynos_v920_pcie_wr_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCTL2,
				     4, val);

	/* initiate link retraining */
	exynos_v920_pcie_rd_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCTL,
				     4, &val);
	val |= PCI_EXP_LNKCTL_RL;
	exynos_v920_pcie_wr_own_conf(pp, pcie_cap_off + PCI_EXP_LNKCTL, 4, val);

	/* Equalization off */
	exynos_v920_pcie_rd_own_conf(pp, PCIE_GEN3_RELATED_OFF, 4, &val);
	val |= PCIE_GEN3_EQUALIZATION_DISABLE;
	exynos_v920_pcie_wr_own_conf(pp, PCIE_GEN3_RELATED_OFF, 4, val);
	dev_info(pci->dev, "[%s] EQ-Off(0x12001) 0x%x\n", __func__, val);

	/*	Header 1 change */
	exynos_v920_pcie_rd_own_conf(pp, PCIE_HEADERTYPE_L_C, 4, &val);
	val |= PCIE_HEADER_1;
	ret = exynos_v920_pcie_wr_own_conf(pp, PCIE_HEADERTYPE_L_C, 4, val);

	/*	Port Link Control Register, lane set	*/
	exynos_v920_pcie_rd_own_conf(pp, PCIE_PORT_LINK_CTRL, 4, &val);
	val &= ~(PCIE_PORT_LINK_CAPABLE_MASK);
	if (exynos_pcie->ch_num == PCIE_CH0) {
		if (!exynos_pcie->use_bifurcation) {
			val |= PCIE_PORT_LINK_CAPABLE_4L;
			dev_info(pci->dev, "[%s] Ch:%d no bif, 4lane.\n",
				__func__, exynos_pcie->ch_num);
		} else {
			val |= PCIE_PORT_LINK_CAPABLE_2L;
			dev_info(pci->dev, "[%s] Ch:%d bif, 2lane.\n",
				__func__, exynos_pcie->ch_num);
		}
	} else if (exynos_pcie->ch_num == PCIE_CH1) {
		val |= PCIE_PORT_LINK_CAPABLE_2L;
		dev_info(pci->dev, "[%s] Ch:%d bif, 2lane.\n", __func__,
			exynos_pcie->ch_num);
	} else if (exynos_pcie->ch_num == PCIE_CH2) {
		if (!exynos_pcie->use_bifurcation) {
			val |= PCIE_PORT_LINK_CAPABLE_2L;
			dev_info(pci->dev, "[%s] Ch:%d no bif, 2lane.\n",
				__func__, exynos_pcie->ch_num);
		} else {
			val |= PCIE_PORT_LINK_CAPABLE_1L;
			dev_info(pci->dev, "[%s] Ch:%d  bif, 1lane.\n",
				__func__, exynos_pcie->ch_num);
		}
	} else if (exynos_pcie->ch_num == PCIE_CH3) {
		val |= PCIE_PORT_LINK_CAPABLE_1L;
		dev_info(pci->dev, "[%s] Ch:%d  bif, 1lane.\n", __func__,
			exynos_pcie->ch_num);
	}

	ret = exynos_v920_pcie_wr_own_conf(pp, PCIE_PORT_LINK_CTRL, 4, val);

	exynos_v920_pcie_rd_own_conf(pp, PCIE_PORT_LINK_CTRL, 4, &val);
	dev_info(pci->dev, "PCIE_PORT_LINK_CTRL  %x\n", val);

	/* MSI Vector Count Set to 32 */
	exynos_v920_pcie_rd_own_conf(pp, MSI_CAP_OFFSET, 4, &val);
	val &= ~(MSI_MULTIPLE_MSG_CAP_MASK);
	val |= MSI_MULTIPLE_MSG_CAP_32;
	exynos_v920_pcie_wr_own_conf(pp, MSI_CAP_OFFSET, 4, val);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Set outbound for cfg0"
 * @logic "Set outbound for cfg0"
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @param{in, busdev, u32, 0~255}
 * @param{in, type, u32, 0x4~0x5}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_prog_viewport_cfg(struct pcie_port *pp,
						u32 busdev, u32 type)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	/* Program viewport 0 : OUTBOUND : CFG0 */
	writel(pp->cfg0_base, pci->atu_base + PCIE_ATU_LOWER_BASE_OUTBOUND0);

	writel((pp->cfg0_base >> 32), pci->atu_base +
		PCIE_ATU_UPPER_BASE_OUTBOUND0);

	writel(pp->cfg0_base + pp->cfg0_size - 1, pci->atu_base +
		PCIE_ATU_LIMIT_OUTBOUND0);

	writel(busdev, pci->atu_base + PCIE_ATU_LOWER_TARGET_OUTBOUND0);

	writel(0, pci->atu_base + PCIE_ATU_UPPER_TARGET_OUTBOUND0);

	writel(type, pci->atu_base + PCIE_ATU_CR1_OUTBOUND0);

	writel(PCIE_ATU_ENABLE, pci->atu_base + PCIE_ATU_CR2_OUTBOUND0);

	exynos_pcie->atu_ok = 1;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_link}
 * @purpose "Set outbound for mem"
 * @logic "Set outbound for mem"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_prog_viewport_mem_outbound(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	writel(PCIE_ATU_TYPE_MEM, pci->atu_base  + PCIE_ATU_CR1_OUTBOUND1);

	writel(exynos_pcie->mem_base, pci->atu_base +
		PCIE_ATU_LOWER_BASE_OUTBOUND1);

	writel((exynos_pcie->mem_base >> 32), pci->atu_base +
		PCIE_ATU_UPPER_BASE_OUTBOUND1);

	writel(exynos_pcie->mem_base + exynos_pcie->mem_size - 1,
		pci->atu_base + PCIE_ATU_LIMIT_OUTBOUND1);

	writel(exynos_pcie->mem_bus_addr, pci->atu_base +
		PCIE_ATU_LOWER_TARGET_OUTBOUND1);

	writel(upper_32_bits(exynos_pcie->mem_bus_addr), pci->atu_base +
		PCIE_ATU_UPPER_TARGET_OUTBOUND1);

	writel(PCIE_ATU_ENABLE, pci->atu_base  + PCIE_ATU_CR2_OUTBOUND1);
}

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_sriov}
 * @purpose "Set outbound for vf mem"
 * @logic "Set outbound for vf mem"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in, vf_num, int, 0~4}
 * @param{in, base_addr, u64, >=0}
 * @param{in, target_addr, u64, >=0}
 * @param{in, size, u32, 0~0xffffffff}
 * @param{in, vf_bar_size, u64, 0~0xffffffff}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_prog_viewport_mem_outbound_for_vf(
	struct pcie_port *pp, int vf_num, u64 base_addr, u64 target_addr,
	u32 size, u64 vf_bar_size)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	u32 offset = 0;
	u64 vf_base_addr = base_addr + vf_num * size;
	u64 vf_target_addr = target_addr + vf_num * vf_bar_size;

	if (vf_num <= exynos_pcie->num_vfs) {
		offset = PCIE_ATU_OUTBOUND_OFFSET * (vf_num + 1);

		writel(PCIE_ATU_TYPE_MEM, pci->atu_base +
			PCIE_ATU_CR1_OUTBOUND0 + offset);

		writel(lower_32_bits(vf_base_addr), pci->atu_base +
			PCIE_ATU_LOWER_BASE_OUTBOUND0 + offset);

		writel(upper_32_bits(vf_base_addr), pci->atu_base +
			PCIE_ATU_UPPER_BASE_OUTBOUND0 + offset);

		writel((vf_base_addr + size - 1), pci->atu_base +
			PCIE_ATU_LIMIT_OUTBOUND0 + offset);

		writel(lower_32_bits(vf_target_addr), pci->atu_base +
			PCIE_ATU_LOWER_TARGET_OUTBOUND0 + offset);

		writel(upper_32_bits(vf_target_addr), pci->atu_base +
			PCIE_ATU_UPPER_TARGET_OUTBOUND0 + offset);

		writel(PCIE_ATU_ENABLE, pci->atu_base + PCIE_ATU_CR2_OUTBOUND0 +
			offset);

		dev_info(pci->dev,
			"[%s] vf_num: %d base_addr: 0x%llx target_addr: 0x%llx offset: 0x%x\n",
			__func__, vf_num, vf_base_addr, vf_target_addr, offset);
	} else {
		dev_warn(pci->dev, "[%s] num_vfs:%d vf_num:%d\n", __func__,
			exynos_pcie->num_vfs, vf_num);
	}
}
#endif

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_link}
 * @purpose "Establish RC link"
 * @logic "Reset phy\n
 *	Enable SRIS(if enabled)\n
 *	Set PERST# high\n
 *	Setup RC\n
 *	Enable LTSSM\n
 *	Wait link up(20ms)."
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @retval{ret, int, 0, 0/-EPIPE, -EPIPE}
 */
static int exynos_v920_pcie_establish_link(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	struct device *dev = pci->dev;
	u32 val, preval = 0;
	int count = 0, try_cnt = 0;
	int ret = 0;

retry:
	ret = exynos_v920_pcie_assert_phy_reset(pp);
	if (ret < 0)
		return -EPERM;

	/* set #PERST high */
	gpio_set_value(exynos_pcie->perst_gpio, PERST_HIGH);
	dev_info(dev, "[%s] Set PERST to HIGH, gpio val = %d\n",
			__func__, gpio_get_value(exynos_pcie->perst_gpio));

	/* setup root complex */
	dw_pcie_setup_rc(pp);

	exynos_v920_pcie_setup_rc(pp);

	dev_info(dev, "D state: %x, %x\n",
		 readl(exynos_pcie->rc_dbi_base + PM_CAP_CON_STATUS_OFFSET) &
		PM_CAP_POWER_STATE_MASK,
		exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2));

	/* assert LTSSM enable */
	exynos_elbi_write(exynos_pcie, PCIE_LTSSM_ENABLE, PCIE_GEN_CTRL_3);

	count = 0;
	while (count < MAX_TIMEOUT) {
		val = exynos_elbi_read(exynos_pcie,
					PCIE_LINK_DBG_2) & PCIE_LINK_LTSSM_MASK;

		if (preval != val)
			dev_info(dev, "LTSSM: %x\n", val);

		if (val == S_L0)
			break;

		preval = val;

		count++;

		udelay(10);
	}

	if (count >= MAX_TIMEOUT) {
		try_cnt++;
		dev_info(dev, "[%s] Link is not up, try count: %d, %x\n",
			__func__, try_cnt,
			exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2));
		if (try_cnt < 10) {
			gpio_set_value(exynos_pcie->perst_gpio, PERST_LOW);
			dev_info(dev, "[%s] Set PERST to LOW, gpio val = %d\n",
					__func__,
				gpio_get_value(exynos_pcie->perst_gpio));
			/* LTSSM disable */
			exynos_elbi_write(exynos_pcie, PCIE_LTSSM_DISABLE,
					PCIE_GEN_CTRL_3);
			exynos_v920_pcie_phy_clock_enable(pp,
							PCIE_DISABLE_CLOCK);

			goto retry;
		} else {
			if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2)
				exynos_v920_pcie_print_link_history(pp);
			return -EPIPE;
		}
	} else {
		dev_info(dev, "[%s] Link up:%x\n", __func__,
			 exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2));

		/* enable interrupt */
		exynos_elbi_write(exynos_pcie, PCIE_INT_EN, PCIE_INT_STS);

		/* enable link-down interrupt */
		exynos_elbi_write(exynos_pcie, PCIE_IRQ_LINK_DOWN,
				PCIE_ERR_INT_CTRL);

		/* To set LINK FLUSH TIME */
		if (exynos_pcie->linkflushtime > PCIE_LINK_FLUSH_TIME_NONE) {
			preval = exynos_elbi_read(exynos_pcie, PCIE_GEN_CTRL4);
			val = preval | PCIE_LINK_FLUSH_TIME_M(
				exynos_pcie->linkflushtime);
			exynos_elbi_write(exynos_pcie, val, PCIE_GEN_CTRL4);
			dev_info(dev, "%s Set LFT(0x%x)\n", __func__,
				 exynos_pcie->linkflushtime);
		}

		/* To enable USER_INT CPL TIMEOUT interrupt */
		if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
			preval = exynos_elbi_read(exynos_pcie,
						PCIE_USER_INT_EN);
			val = preval | PCIE_IRQ_CPL_TIMEOUT_EN;
			exynos_elbi_write(exynos_pcie, val, PCIE_USER_INT_EN);
		}
	}
	return 0;
}

void exynos_v920_pcie_hardaccess_work(struct work_struct *work)
{
	struct exynos_pcie *exynos_pcie =
	container_of(work, struct exynos_pcie, hardaccess_work.work);
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;

	void __iomem *ep_mem_region;

	dev_info(pci->dev, "PCIe hard access Start.(6min)\n");
	/* DBI access */
	if (chk_hard_access_v920(exynos_pcie, exynos_pcie->rc_dbi_base)) {
		dev_info(pci->dev, "DBI hared access test FAIL!\n");
		return;
	}

	/* EP configuration access */
	if (chk_hard_access_v920(exynos_pcie, pp->va_cfg0_base)) {
		dev_info(pci->dev, "EP config access test FAIL!\n");
		return;
	}

	/* EP outbound memory access */
	ep_mem_region = ioremap(exynos_pcie->mem_base, SZ_4K);
	if (chk_hard_access_v920(exynos_pcie, ep_mem_region)) {
		dev_info(pci->dev, "EP mem access test FAIL!\n");
		iounmap(ep_mem_region);
		return;
	}
	iounmap(ep_mem_region);
	dev_info(pci->dev, "PCIe hard access Success.\n");

}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_link}
 * @purpose "If link down occurred, try recovery"
 * @logic "Try link recovery"
 * @params
 * @param{in/out, work, struct ::work_struct *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_dislink_work(struct work_struct *work)
{
	struct exynos_pcie *exynos_pcie =
		container_of(work, struct exynos_pcie, dislink_work.work);
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;

	if (exynos_pcie->state == STATE_LINK_DOWN)
		return;

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2)
		exynos_v920_pcie_print_link_history(pp);
	exynos_v920_pcie_dump_link_down_status(exynos_pcie->ch_num);
	exynos_v920_pcie_register_dump(exynos_pcie->ch_num);

	exynos_pcie->linkdown_cnt++;
	dev_info(pci->dev, "link down and recovery cnt: %d\n",
			exynos_pcie->linkdown_cnt);

	exynos_v920_pcie_notify_callback(pp, EXYNOS_PCIE_EVENT_LINKDOWN);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Reset phy"
 * @logic "Phy clock enable\n
 *	Phy init\n
 *	Bus number enable."
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_assert_phy_reset(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret;

	ret = exynos_v920_pcie_phy_clock_enable(&pci->pp, PCIE_ENABLE_CLOCK);
	dev_info(pci->dev, "phy clk enable, ret value = %d\n", ret);
	if (exynos_pcie->phy_ops.phy_config != NULL)
		ret = exynos_pcie->phy_ops.phy_config(pp,
						exynos_pcie->fw_update);
	if (ret < 0)
		return -EINVAL;

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Enable interrupt"
 * @logic "Enable int0~2 legacy interrupts"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_enable_interrupts(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	/* disable interrupt */
	exynos_elbi_write(exynos_pcie, 0x0, PCIE_INT_STS);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "MSI interrupt operation"
 * @logic "Read MSI status and call generic irq"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_handle_msi_irq(struct pcie_port *pp)
{
	u32 val;
	unsigned long val_ul;
	int i, pos, irq;

	for (i = 0; i < MAX_MSI_SET; i++) {
		exynos_v920_pcie_rd_own_conf(pp, PCIE_MSI_CTRL_INTR0_STATUS +
			i * PCIE_MSI_CTRL_INT_OFFSET, 4, &val);
		exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_CTRL_INTR0_STATUS +
			i * PCIE_MSI_CTRL_INT_OFFSET, 4, val);

		if (!val)
			continue;

		/* For Coverity Check */
		val_ul = (unsigned long) val;

		pos = 0;
		while ((pos = find_next_bit(&val_ul, 32, pos)) != 32) {
			irq = irq_find_mapping(pp->irq_domain, i * 32 + pos);
			generic_handle_irq(irq);
			pos++;
		}
	}
}


/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Handler for legacy interrupts"
 * @logic "Handle irq1 and start link down if IRQ_LINK_DOWN occurs\n
	Handle irq2 and call MSI hanlder."
 * @params
 * @param{in, irq, int, >0}
 * @param{in/out, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED, not IRQ_HANDLED}
 */
static irqreturn_t exynos_v920_pcie_irq_handler(int irq, void *arg)
{
	struct pcie_port *pp = arg;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	u32 val, lft;

	/* handle IRQ1 interrupt */
	val = exynos_elbi_read(exynos_pcie, PCIE_ERR_INT_STS);
	exynos_elbi_write(exynos_pcie, val, PCIE_ERR_INT_STS);
	dev_dbg(pci->dev, "[%s] IRQ1 = 0x%x\n", __func__, val);

	if (val & PCIE_IRQ_LINK_DOWN) {
		exynos_pcie->state = STATE_LINK_DOWN_TRY;
		lft = exynos_elbi_read(exynos_pcie, PCIE_GEN_CTRL4);
		dev_info(pci->dev, "%s PCIE LINK DOWN(state:0x%x LFT:0x%x)\n",
			 __func__, val, lft);

		queue_work(exynos_pcie->pcie_wq,
				&exynos_pcie->dislink_work.work);
	}

	/* To handle Completion timeout interrupt */
	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
		val = exynos_elbi_read(exynos_pcie, PCIE_USER_INT_STS);
		exynos_elbi_write(exynos_pcie, val, PCIE_USER_INT_STS);

		if (val & PCIE_IRQ_CPL_TIMEOUT) {
			dev_err(pci->dev, "%s IRQ_CPL_TIMEOUT(0x%x)\n",
				__func__, val);
		}
	}

	return IRQ_HANDLED;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Handler for MSI (EVT1)"
 * @logic "Call MSI operation"
 * @params
 * @param{in, irq, int, >=0}
 * @param{in, arg, void *, not NULL}
 * @endparam
 * @retval{-, irqreturn_t, 0, IRQ_HANDLED, not IRQ_HANDLED}
 */
static irqreturn_t exynos_v920_pcie_msi_irq_handler(int irq, void *arg)
{
	struct pcie_port *pp = arg;

	exynos_v920_handle_msi_irq(pp);

	return IRQ_HANDLED;
}

#ifdef CONFIG_PCI_MSI
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_link}
 * @purpose "Initialize MSI"
 * @logic "Program MSI data; Enable MSI"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_msi_init(struct pcie_port *pp)
{
	u32 val;
	u64 msi_target;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	if (!exynos_pcie->use_msi)
		return;

	/*
	 * dw_pcie_msi_init() function allocates msi_data.
	 * The following code is added to avoid duplicated allocation.
	 */
	msi_target = (dma_addr_t)(void *)pp->msi_data;

	/* program the msi_data */
	exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_ADDR_LO, 4,
			    (u32)(msi_target & 0xffffffff));
	exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_ADDR_HI, 4,
			    (u32)(msi_target >> 32 & 0xffffffff));

	/* Enable MSI interrupt after PCIe reset */
	val = (u32)(*pp->msi_irq_in_use);
	dev_info(pci->dev, "[%s] val:0x%x\n", __func__, val);
	exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_CTRL_INTR0_ENABLE, 4, val);
}
#endif

static void __iomem *exynos_v920_pcie_own_conf_map_bus(struct pci_bus *bus,
	unsigned int devfn, int where)
{
	return dw_pcie_own_conf_map_bus(bus, devfn, where);
}

static int exynos_v920_generic_own_config_read(struct pci_bus *bus,
	unsigned int devfn, int where, int size, u32 *val)
{
	struct pcie_port *pp = bus->sysdata;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret, is_linked = 0;	unsigned long flags;

	spin_lock_irqsave(&exynos_pcie->reg_lock, flags);

	if (exynos_pcie->state == STATE_LINK_UP ||
		exynos_pcie->state == STATE_LINK_UP_TRY)
		is_linked = 1;

	if (is_linked == 0) {
		exynos_v920_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
		exynos_v920_pcie_phy_clock_enable(pp, PCIE_ENABLE_CLOCK);
	}

	ret = pci_generic_config_read(bus, devfn, where, size, val);

	if (is_linked == 0) {
		exynos_v920_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
		exynos_v920_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);
	}

	spin_unlock_irqrestore(&exynos_pcie->reg_lock, flags);

	return ret;
}

static int exynos_v920_generic_own_config_write(struct pci_bus *bus,
	unsigned int devfn, int where, int size, u32 val)
{
	struct pcie_port *pp = bus->sysdata;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	int ret, is_linked = 0;
	unsigned long flags;

	spin_lock_irqsave(&exynos_pcie->reg_lock, flags);

	if (exynos_pcie->state == STATE_LINK_UP ||
		exynos_pcie->state == STATE_LINK_UP_TRY)
		is_linked = 1;
	if (is_linked == 0) {
		exynos_v920_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
		exynos_v920_pcie_phy_clock_enable(pp, PCIE_ENABLE_CLOCK);
	}

	ret = pci_generic_config_write(bus, devfn, where, size, val);

	if (is_linked == 0) {
		exynos_v920_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
		exynos_v920_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);
	}

	spin_unlock_irqrestore(&exynos_pcie->reg_lock, flags);

	return ret;
}

static int exynos_v920_pcie_check_link(struct dw_pcie *pci)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	u32 val;

	if (exynos_pcie->state != STATE_LINK_UP)
		return 0;

	val = exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2) &
		PCIE_LINK_LTSSM_MASK;
	if (val >= 0x0d && val <= 0x15)
		return 1;

	return 0;
}

static void __iomem *exynos_v920_pcie_other_conf_map_bus(struct pci_bus *bus,
	unsigned int devfn, int where)
{
	struct pcie_port *pp = bus->sysdata;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	u32 busdev;
	int type;

	if (!exynos_v920_pcie_check_link(pci))
		return NULL;

	busdev = PCIE_ATU_BUS(bus->number) | PCIE_ATU_DEV(PCI_SLOT(devfn)) |
		PCIE_ATU_FUNC(PCI_FUNC(devfn));

	if (pci_is_root_bus(bus->parent))
		type = PCIE_ATU_TYPE_CFG0;
	else
		type = PCIE_ATU_TYPE_CFG1;

	/* setup ATU for cfg/mem outbound */
	exynos_v920_pcie_prog_viewport_cfg(pp, busdev, type);

	return pp->va_cfg0_base + where;
}

static int exynos_v920_generic_other_config_read(struct pci_bus *bus,
	unsigned int devfn, int where, int size, u32 *val)
{
	struct pcie_port *pp = bus->sysdata;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret, is_linked = 0;

	if (exynos_pcie->state == STATE_LINK_UP)
		is_linked = 1;

	ret = pci_generic_config_read(bus, devfn, where, size, val);

	return ret;
}

static int exynos_v920_generic_other_config_write(struct pci_bus *bus,
	unsigned int devfn, int where, int size, u32 val)
{
	struct pcie_port *pp = bus->sysdata;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret, is_linked = 0;

	if (exynos_pcie->state == STATE_LINK_UP)
		is_linked = 1;

	ret = pci_generic_config_write(bus, devfn, where, size, val);

	return ret;
}

static struct pci_ops exynos_own_pcie_ops = {
	.map_bus = exynos_v920_pcie_own_conf_map_bus,
	.read = exynos_v920_generic_own_config_read,
	.write = exynos_v920_generic_own_config_write,
};

static struct pci_ops exynos_child_pcie_ops = {
	.map_bus = exynos_v920_pcie_other_conf_map_bus,
	.read = exynos_v920_generic_other_config_read,
	.write = exynos_v920_generic_other_config_write,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Read DBI region"
 * @logic "Read DBI configuration"
 * @params
 * @param{in, pci, struct ::dw_pcie *, not NULL}
 * @param{in, base, void __iomem *, not NULL}
 * @param{in, reg, u32, 0~0xffffffff}
 * @param{in, size, size_t, 1/2/4}
 * @endparam
 * @retval{val, u32, 0, 0~0xffffffff, 0}
 */
static u32 exynos_v920_pcie_read_dbi(struct dw_pcie *pci, void __iomem *base,
				u32 reg, size_t size)
{
	struct pcie_port *pp = &pci->pp;
	u32 val = 0;

	exynos_v920_pcie_rd_own_conf(pp, reg, size, &val);
	return val;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Write on DBI region"
 * @logic "Write on DBI configuration"
 * @params
 * @param{in, pci, struct ::dw_pcie *, not NULL}
 * @param{in, base, void __iomem *, not NULL}
 * @param{in, reg, u32, 0x~0xffffffff}
 * @param{in, size, size_t, 1/2/4}
 * @param{in, val, u32, 0~0xffffffff}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_write_dbi(struct dw_pcie *pci, void __iomem *base,
				  u32 reg, size_t size, u32 val)
{
	struct pcie_port *pp = &pci->pp;

	exynos_v920_pcie_wr_own_conf(pp, reg, size, val);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Read RC configuration"
 * @logic "If link is not established, enable clocks\n
 *	Read config region\n
 *	If link is not established, disable clocks."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in, where, int, 0~0x5000/0x200000~0x201000}
 * @param{in, size, int, 1/2/4}
 * @param{in, val, u32, 0~0xffffffff}
 * @endparam
 * @retval{ret, int, 0, 0~PCIBIOS_SUCCESSFUL, PCIBIOS_BAD_REGISTER_NUMBER}
 */
static int exynos_v920_pcie_rd_own_conf(struct pcie_port *pp, int where,
					int size, u32 *val)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int is_linked = 0;
	int ret = 0;
	u32 __maybe_unused reg_val;

	if (exynos_pcie->state == STATE_LINK_UP)
		is_linked = 1;

	if (is_linked == 0) {
		exynos_v920_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
		exynos_v920_pcie_phy_clock_enable(&pci->pp,
				PCIE_ENABLE_CLOCK);
	}

	if (where < DBI_SHADOW_BASE_ADDR)
		ret = dw_pcie_read(exynos_pcie->rc_dbi_base + (where),
				size, val);
	else
		ret = dw_pcie_read(exynos_pcie->shadow_dbi_base +
				(where) - DBI_SHADOW_BASE_ADDR,
				size, val);

	if (is_linked == 0) {
		exynos_v920_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
		exynos_v920_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);
	}

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Write to RC configuration region"
 * @logic "If link is not established, enable clocks\n
 *	Write to RC configuration region\n
 *	If link is not established, disable clocks."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in, where, int, 0~0x5000/0x200000~0x201000}
 * @param{in, size, int, 1/2/4}
 * @param{in, val, u32, 0~0xffffffff}
 * @endparam
 * @retval{ret, int, 0, 0~PCIBIOS_SUCCESSFUL, PCIBIOS_BAD_REGISTER_NUMBER}
 */
static int exynos_v920_pcie_wr_own_conf(struct pcie_port *pp, int where,
					int size, u32 val)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int is_linked = 0;
	int ret = 0;
	u32 __maybe_unused reg_val;

	if (exynos_pcie->state == STATE_LINK_UP)
		is_linked = 1;

	if (is_linked == 0) {
		exynos_v920_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
		exynos_v920_pcie_phy_clock_enable(&pci->pp,
				PCIE_ENABLE_CLOCK);
	}

	if (where < DBI_SHADOW_BASE_ADDR)
		ret = dw_pcie_write(exynos_pcie->rc_dbi_base + (where),
				size, val);
	else
		ret = dw_pcie_write(exynos_pcie->shadow_dbi_base +
				(where) - DBI_SHADOW_BASE_ADDR,
				size, val);

	if (is_linked == 0) {
		exynos_v920_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);
		exynos_v920_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);
	}

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Read from remote"
 * @logic "Check link is available\n
 *	Read from remote."
 * @params
 * @param{in, exynos_pcie, struct ::exynos_pcie *, not NULL}
 * @param{in, addr, void __iomem *, not NULL}
 * @param{in, size, int, 1/2/4}
 * @param{in, val, u32 *, 0~0xffffffff}
 * @endparam
 * @retval{ret, int, 0, 0~PCIBIOS_SUCCESSFUL, PCIBIOS_FUNC_NOT_SUPPORTED}
 */
static int exynos_v920_pcie_read_other(struct exynos_pcie *exynos_pcie,
					void __iomem *addr, int size, u32 *val)
{
	if (exynos_pcie->state == STATE_LINK_UP)
		return dw_pcie_read(addr, size, val);
	else
		return PCIBIOS_FUNC_NOT_SUPPORTED;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Write to remote"
 * @logic "Check link is available\n
 *	Write to remote."
 * @params
 * @param{in, exynos_pcie, struct ::exynos_pcie *, not NULL}
 * @param{in, addr, void __iomem *, not NULL}
 * @param{in, size, int, 1/2/4}
 * @param{in, val, u32, 0~0xffffffff}
 * @endparam
 * @retval{ret, int, 0, 0~PCIBIOS_SUCCESSFUL, PCIBIOS_FUNC_NOT_SUPPORTED}
 */
static int exynos_v920_pcie_write_other(struct exynos_pcie *exynos_pcie,
					void __iomem *addr, int size, u32 val)
{
	if (exynos_pcie->state == STATE_LINK_UP)
		return dw_pcie_write(addr, size, val);
	else
		return PCIBIOS_FUNC_NOT_SUPPORTED;
}


/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Read configuration from device"
 * @logic "Set outbound atu for target configuration region\n
 *	Read configuration."
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @param{in, bus, struct ::pci_bus *, not NULL}
 * @param{in, devfn, u32, 0~255}
 * @param{in, where, int, 0~0x1000}
 * @param{in, size, int, 1/2/4}
 * @param{out, val, u32 *, 0~0xffffffff}
 * @endparam
 * @retval{ret, int, 0, 0~PCIBIOS_SUCCESSFUL, PCIBIOS_BAD_REGISTER_NUMBER}
 */
static int exynos_v920_pcie_rd_other_conf(struct pcie_port *pp,
		struct pci_bus *bus, u32 devfn, int where, int size, u32 *val)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret = 0;
	int type;
	u32 busdev, cfg_size;
	u64 cpu_addr;
	void __iomem *va_cfg_base;

	busdev = PCIE_ATU_BUS(bus->number) | PCIE_ATU_DEV(PCI_SLOT(devfn)) |
		 PCIE_ATU_FUNC(PCI_FUNC(devfn));

	if (pci_is_root_bus(bus->parent))
		type = PCIE_ATU_TYPE_CFG0;
	else
		type = PCIE_ATU_TYPE_CFG1;

	cpu_addr = pp->cfg0_base;
	cfg_size = pp->cfg0_size;
	va_cfg_base = pp->va_cfg0_base;
	/* setup ATU for cfg/mem outbound */
	exynos_v920_pcie_prog_viewport_cfg(pp, busdev, type);

	ret = exynos_v920_pcie_read_other(exynos_pcie,
					va_cfg_base + where, size, val);

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Write to device's configuration region"
 * @logic "Set outbound atu for target configuration region\n
 *	Write configuration."
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @param{in, bus, struct ::pci_bus *, not NULL}
 * @param{in, devfn, u32, 0~255}
 * @param{in, where, int, 0~0x1000}
 * @param{in, size, int, 1/2/4}
 * @param{in, val, u32, 0~0xffffffff}
 * @endparam
 * @retval{ret, int, 0, 0~PCIBIOS_SUCCESSFUL, PCIBIOS_BAD_REGISTER_NUMBER}
 */
static int exynos_v920_pcie_wr_other_conf(struct pcie_port *pp,
		struct pci_bus *bus, u32 devfn, int where, int size, u32 val)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret = 0;
	int type;
	u32 busdev, cfg_size;
	u64 cpu_addr;
	void __iomem *va_cfg_base;

	busdev = PCIE_ATU_BUS(bus->number) |
		PCIE_ATU_DEV(PCI_SLOT(devfn)) | PCIE_ATU_FUNC(PCI_FUNC(devfn));

	if (pci_is_root_bus(bus->parent))
		type = PCIE_ATU_TYPE_CFG0;
	else
		type = PCIE_ATU_TYPE_CFG1;

	cpu_addr = pp->cfg0_base;
	cfg_size = pp->cfg0_size;
	va_cfg_base = pp->va_cfg0_base;
	/* setup ATU for cfg/mem outbound */
	exynos_v920_pcie_prog_viewport_cfg(pp, busdev, type);

	ret = exynos_v920_pcie_write_other(exynos_pcie,
		va_cfg_base + where, size, val);

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Check PCIe link up"
 * @logic "Check PCIe link up"
 * @params
 * @param{in, pci, struct ::dw_pcie *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0~1, 0}
 */
static int exynos_v920_pcie_link_up(struct dw_pcie *pci)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	u32 val;

	if (!exynos_pcie->is_probe_done) {
		dev_dbg(pci->dev, "Force link-up return in probe time!\n");
		return 1;
	}

	if (exynos_pcie->state != STATE_LINK_UP)
		return 0;

	val = exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2) &
		PCIE_LINK_LTSSM_MASK;
	if (val >= S_RCVRY_LOCK && val <= S_L2_IDLE)
		return 1;

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "irq_ack callback for MSI"
 * @logic "Set MSI status if irq_ack"
 * @params
 * @param{in, data, struct ::irq_data *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_msi_irq_ack(struct irq_data *data)
{
	struct pcie_port *pp  = irq_data_get_irq_chip_data(data);
#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
#endif
	u32 irq = data->hwirq;
	unsigned int res = 0;

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	if (exynos_pcie->sriov_supported)
		res = (irq / 32) % 2;
#endif

	exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_CTRL_INTR0_STATUS +
				res * PCIE_MSI_CTRL_INT_OFFSET, 4, BIT(irq));
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Compose MSI message"
 * @logic "Set MSI address and data"
 * @params
 * @param{in, data, struct ::irq_data *, not NULL}
 * @param{out, msg, struct ::msi_msg *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_compose_msi_msg(struct irq_data *data,
							struct msi_msg *msg)
{
	struct pcie_port *pp = irq_data_get_irq_chip_data(data);
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	u64 msi_target;

	msi_target = (dma_addr_t)(void *)pp->msi_data;

	msg->address_lo = lower_32_bits(msi_target);
	msg->address_hi = upper_32_bits(msi_target);
	msg->data = data->hwirq;

	dev_dbg(pci->dev, "msi#%d address_hi %#x address_lo %#x\n",
		(int)data->hwirq, msg->address_hi, msg->address_lo);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Blank callback for irq_set_affinity"
 * @logic "N/A"
 * @params
 * @param{in, data, struct ::irq_data *, not NULL}
 * @param{in, mask, struct ::cpumask *, not NULL}
 * @param{in, force, bool, 0~1}
 * @endparam
 * @retval{-, int, -EINVAL, -EINVAL, not -EINVAL}
 */
static int exynos_v920_pcie_msi_set_affinity(struct irq_data *irq_data,
				    const struct cpumask *mask, bool force)
{
	return -EINVAL;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "irq_mask callback for MSI"
 * @logic "Disable MSI interrupt"
 * @params
 * @param{in, data, struct ::irq_data *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_msi_mask(struct irq_data *data)
{
	struct pcie_port *pp = irq_data_get_irq_chip_data(data);
#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
#endif
	u32 irq = data->hwirq;
	unsigned long flags;
	unsigned int res = 0, val;

	raw_spin_lock_irqsave(&pp->lock, flags);

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	if (exynos_pcie->sriov_supported)
		res = (irq / 32) % 2;
#endif


	exynos_v920_pcie_rd_own_conf(pp, PCIE_MSI_CTRL_INTR0_ENABLE
		+ res * PCIE_MSI_CTRL_INT_OFFSET, 4, &val);
	val &= ~(1 << irq);
	exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_CTRL_INTR0_ENABLE
		+ res * PCIE_MSI_CTRL_INT_OFFSET, 4, val);

	raw_spin_unlock_irqrestore(&pp->lock, flags);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "irq_unmask callback for MSI"
 * @logic "Enable MSI interrupt"
 * @params
 * @param{in, data, struct ::irq_data *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_msi_unmask(struct irq_data *data)
{
	struct pcie_port *pp = irq_data_get_irq_chip_data(data);
#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
#endif
	u32 irq = data->hwirq;
	unsigned long flags;
	unsigned int res = 0, val;

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	if (exynos_pcie->sriov_supported)
		res = (irq / 32) % 2;
#endif


	raw_spin_lock_irqsave(&pp->lock, flags);

	exynos_v920_pcie_rd_own_conf(pp, PCIE_MSI_CTRL_INTR0_ENABLE
		+ res * PCIE_MSI_CTRL_INT_OFFSET, 4, &val);
	val |= 1 << irq;
	exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_CTRL_INTR0_ENABLE
		+ res * PCIE_MSI_CTRL_INT_OFFSET, 4, val);

	raw_spin_unlock_irqrestore(&pp->lock, flags);
}

static struct irq_chip exynos_pcie_msi_irq_chip = {
	.name = "EXYNOS-PCI-MSI",
	.irq_ack = exynos_v920_pcie_msi_irq_ack,
	.irq_compose_msi_msg = exynos_v920_pcie_compose_msi_msg,
	.irq_set_affinity = exynos_v920_pcie_msi_set_affinity,
	.irq_mask = exynos_v920_pcie_msi_mask,
	.irq_unmask = exynos_v920_pcie_msi_unmask,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "alloc callback for MSI domain ops"
 * @logic "allocate irq domain for MSI"
 * @params
 * @param{in/out, domain, struct ::irq_domain *, not NULL}
 * @param{in, virq, int, >=0}
 * @param{in, nr_irqs, int, >=0}
 * @param{in, args, void *, -}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_irq_domain_alloc(struct irq_domain *domain,
				    unsigned int virq, unsigned int nr_irqs,
				    void *args)
{
	struct pcie_port *pp = domain->host_data;
	unsigned long flags;
	u32 i;
	int bit;

	raw_spin_lock_irqsave(&pp->lock, flags);

	bit = bitmap_find_free_region(pp->msi_irq_in_use, pp->num_vectors,
				      order_base_2(nr_irqs));

	raw_spin_unlock_irqrestore(&pp->lock, flags);

	if (bit < 0)
		return -ENOSPC;

	for (i = 0; i < nr_irqs; i++)
		irq_domain_set_info(domain, virq + i, bit + i,
				    pp->msi_irq_chip,
				    pp, handle_edge_irq,
				    NULL, NULL);

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "free callback for MSI domain ops"
 * @logic "release irq domain for MSI"
 * @params
 * @param{in/out, domain, struct ::irq_domain *, not NULL}
 * @param{in, virq, int, >=0}
 * @param{in, nr_irqs, int, >=0}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_irq_domain_free(struct irq_domain *domain,
				    unsigned int virq, unsigned int nr_irqs)
{
	struct irq_data *d;
	struct pcie_port *pp;
	unsigned long flags;

	d = irq_domain_get_irq_data(domain, virq);
	if (!d)
		return;

	pp = irq_data_get_irq_chip_data(d);
	if (!pp)
		return;
	raw_spin_lock_irqsave(&pp->lock, flags);

	bitmap_release_region(pp->msi_irq_in_use, d->hwirq,
			      order_base_2(nr_irqs));

	raw_spin_unlock_irqrestore(&pp->lock, flags);
}

static const struct irq_domain_ops exynos_pcie_msi_domain_ops = {
	.alloc	= exynos_v920_pcie_irq_domain_alloc,
	.free	= exynos_v920_pcie_irq_domain_free,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "irq_ack callback for DesignWare msi irq"
 * @logic "call irq_chip_ack_parent"
 * @params
 * @param{in, d, struct ::irq_data *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_dw_msi_ack_irq(struct irq_data *d)
{
	irq_chip_ack_parent(d);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "irq_mask callback for DesignWare msi irq"
 * @logic "call mask irq apis"
 * @params
 * @param{in, d, struct ::irq_data *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_dw_msi_mask_irq(struct irq_data *d)
{
	pci_msi_mask_irq(d);
	irq_chip_mask_parent(d);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "irq_unmask callback for DesignWare msi irq"
 * @logic "call unmask irq apis"
 * @params
 * @param{in, d, struct ::irq_data *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_dw_msi_unmask_irq(struct irq_data *d)
{
	pci_msi_unmask_irq(d);
	irq_chip_unmask_parent(d);
}

static struct irq_chip exynos_dw_pcie_msi_irq_chip = {
	.name = "PCI-MSI",
	.irq_ack = exynos_v920_dw_msi_ack_irq,
	.irq_mask = exynos_v920_dw_msi_mask_irq,
	.irq_unmask = exynos_v920_dw_msi_unmask_irq,
};

static struct msi_domain_info exynos_pcie_msi_domain_info = {
	.flags	= (MSI_FLAG_USE_DEF_DOM_OPS | MSI_FLAG_USE_DEF_CHIP_OPS |
		   MSI_FLAG_PCI_MSIX | MSI_FLAG_MULTI_PCI_MSI),
	.chip	= &exynos_dw_pcie_msi_irq_chip,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "call back for msi_host_init"
 * @logic "Create MSI irq domain"
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_msi_host_init(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct fwnode_handle *fwnode = of_node_to_fwnode(pci->dev->of_node);

	pp->msi_irq_chip = &exynos_pcie_msi_irq_chip;

	pp->irq_domain = irq_domain_create_linear(fwnode, pp->num_vectors,
					       &exynos_pcie_msi_domain_ops, pp);
	if (!pp->irq_domain) {
		dev_err(pci->dev, "Failed to create IRQ domain\n");
		return -ENOMEM;
	}

	irq_domain_update_bus_token(pp->irq_domain, DOMAIN_BUS_NEXUS);

	pp->msi_domain = pci_msi_create_irq_domain(fwnode,
						   &exynos_pcie_msi_domain_info,
						   pp->irq_domain);
	if (!pp->msi_domain) {
		dev_err(pci->dev, "Failed to create MSI domain\n");
		irq_domain_remove(pp->irq_domain);
		return -ENOMEM;
	}

	return 0;
}


/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Initialize PCIe host"
 * @logic "Setup RC device"
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_host_init(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

	/* Setup RC to avoid initialization faile in PCIe stack */

	/* Set default bus ops */
	pp->bridge->ops = &exynos_own_pcie_ops;
	pp->bridge->child_ops = &exynos_child_pcie_ops;

	/* Get MSI address */
	pp->msi_page = alloc_page(GFP_DMA32);
	pp->msi_data = dma_map_page(pci->dev, pp->msi_page, 0, PAGE_SIZE,
				    DMA_FROM_DEVICE);
	if (dma_mapping_error(pci->dev, pp->msi_data)) {
		dev_err(pci->dev, "Failed to map MSI data\n");
		__free_page(pp->msi_page);
		pp->msi_page = NULL;
		pp->msi_data = 0;
	}

	dw_pcie_setup_rc(pp);
	return 0;
}

static struct dw_pcie_host_ops exynos_pcie_host_ops = {
	.host_init = exynos_v920_pcie_host_init,
	.msi_host_init = exynos_v920_pcie_msi_host_init,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Add PCIe port"
 * @logic "Get irq from dt\n
 *	Set up RC\n
 *	Initialize MSI."
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @param{in, pdev, struct ::platform_device *, not NULL}
 * @endparam
 * @retval{ret, int, 0, 0/1/-ENODEV, 1/-ENODEV}
 */
static int add_pcie_port_v920(struct pcie_port *pp,
				struct platform_device *pdev)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret;
	u64 __maybe_unused msi_target;
#ifdef MSI_TARGET_FROM_DT
	unsigned long msi_addr_from_dt;
#endif

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
	int i;
#endif
	struct resource_entry *tmp, *entry = NULL;
	u32 val = 0;

	/* from dts */
	pp->irq = platform_get_irq(pdev, 0);
	if (!pp->irq) {
		dev_err(&pdev->dev, "failed to get irq\n");
		return -ENODEV;
	}
	ret = devm_request_irq(&pdev->dev, pp->irq,
			       exynos_v920_pcie_irq_handler,
			       IRQF_SHARED | IRQF_TRIGGER_HIGH,
			       "exynos-pcie", pp);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq\n");
		return ret;
	}

	if (IS_ENABLED(CONFIG_PCI_MSI) && !(exynos_pcie->use_its)) {
		exynos_pcie->msi_irq[0] = platform_get_irq(pdev, 1);
		if (exynos_pcie->msi_irq[0] < 0) {
			dev_err(&pdev->dev, "failed to get msi_irq\n");
			return -ENODEV;
		}
		ret = devm_request_irq(&pdev->dev,
					exynos_pcie->msi_irq[0],
					exynos_v920_pcie_msi_irq_handler,
					IRQF_SHARED, "exynos-pcie", pp);

		if (ret) {
			dev_err(&pdev->dev, "failed to request msi_irq\n");
			return ret;
		}

#if IS_ENABLED(CONFIG_PCI_EXYNOS_V920_IOV)
		if (exynos_pcie->num_vfs > 0) {
			for (i = 1; i < exynos_pcie->num_vfs; i++) {
				exynos_pcie->msi_irq[i] =
					platform_get_irq(pdev, i+1);

				dev_info(&pdev->dev,
					"exynos_pcie->msi_irq[%d]: %d\n",
					i, exynos_pcie->msi_irq[i]);
				if (exynos_pcie->msi_irq[i] < 0) {
					dev_err(&pdev->dev,
						"failed to get msi_irq  VM%d\n",
						i+1);
					return -ENODEV;
				}
			}
		}
#endif
	}

	pp->ops = &exynos_pcie_host_ops;

	/* set up RC default */
	exynos_v920_pcie_setup_rc(pp);
	spin_lock_init(&exynos_pcie->conf_lock);
	spin_lock_init(&exynos_pcie->reg_lock);

	ret = dw_pcie_host_init(pp);

	/* Add Memory resource region from  resource entry */

	/* Get last memory resource entry */
	resource_list_for_each_entry(tmp, &pp->bridge->windows)
		if (resource_type(tmp->res) == IORESOURCE_MEM)
			entry = tmp;

	exynos_pcie->mem_base = entry->res->start;
	exynos_pcie->mem_size = resource_size(entry->res);
	exynos_pcie->mem_bus_addr = entry->res->start - entry->offset;

#ifdef CONFIG_PCI_MSI
#ifdef MODIFY_MSI_ADDR
	/* Change MSI address to fit within a 32bit address boundary */
	free_page(pp->msi_data);

#ifdef MSI_TARGET_FROM_DT
	/* get the MSI target address from DT */
	msi_addr_from_dt = shm_get_msi_base();

	pp->msi_data = (unsigned long)phys_to_virt(msi_addr_from_dt);
#else
	pp->msi_data = (u64)kmalloc(4, GFP_DMA);
#endif

	msi_target = virt_to_phys((void *)pp->msi_data);

	if ((msi_target >> 32) != 0)
		dev_info(&pdev->dev,
			"MSI memory is allocated over 32bit boundary\n");

	/* program the msi_data */
	exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_ADDR_LO, 4,
			    (u32)(msi_target & 0xffffffff));
	exynos_v920_pcie_wr_own_conf(pp, PCIE_MSI_ADDR_HI, 4,
			    (u32)(msi_target >> 32 & 0xffffffff));
#endif
#endif
	if (ret) {
		dev_err(&pdev->dev, "failed to initialize host\n");
		return ret;
	}

	dev_info(&pdev->dev, "MSI Message Address : 0x%llx\n",
			((dma_addr_t)(void *)pp->msi_data));

	if (exynos_pcie->use_sharability) {
		val = readl(exynos_pcie->sysreg_base +
			PCIE_SYSREG_SHARABILITY_CTRL +
			PCIE_SYSREG_SHARABLE_RW_OFFSET +
			PCIE_SYSREG_SHARABLE_CH_OFFSET * exynos_pcie->ch_num);
		val |= PCIE_SYSREG_SHARABLE_RW_ENABLE;
		writel(val, exynos_pcie->sysreg_base +
			PCIE_SYSREG_SHARABILITY_CTRL +
			PCIE_SYSREG_SHARABLE_RW_OFFSET +
			PCIE_SYSREG_SHARABLE_CH_OFFSET * exynos_pcie->ch_num);
		dev_err(&pdev->dev, "Set RW Sharability\n");
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Enable/disable cache coherency"
 * @logic "Enable/disable cache coherency"
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in, enable, int, 0~1}
 * @endparam
 * @noret
 */
static void enable_cache_coherency_v920(struct pcie_port *pp, int enable)
{
	int set_val = PCIE_SYSREG_SHARABLE_DISABLE;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);

	/* Set enable value */
	if (enable) {
		dev_info(pci->dev, "Enable cache coherency.\n");
		set_val = PCIE_SYSREG_SHARABLE_ENABLE;
#if !defined(CONFIG_SOC_EXYNOSAUTO9)
	} else {
		dev_info(pci->dev, "Disable cache coherency.\n");
		set_val = PCIE_SYSREG_SHARABLE_DISABLE;
#endif
	}

	/* Set configurations */
	regmap_update_bits(exynos_pcie->sysreg,
			PCIE_SYSREG_SHARABILITY_CTRL, /* SYSREG address */
			(0x3 << (PCIE_SYSREG_SHARABLE_OFFSET
				 + exynos_pcie->ch_num * 4)), /* Mask */
			(set_val << (PCIE_SYSREG_SHARABLE_OFFSET
				     + exynos_pcie->ch_num * 4))); /* Set val */
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Parse fixed device info from dt"
 * @logic "Parse fixed device info from dt"
 * @params
 * @param{in, np, struct ::device_node *, not NULL}
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @retval{-, int, 0, int, Not 0}
 */
static int exynos_v920_pcie_parse_fixed_dt(struct device_node *np,
				struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	const char *use_cache_coherency;
	const char *use_msi;
	const char *use_sicd;
	const char *use_sysmmu;
	const char *use_ia;
	const char *use_sharability;

	if (of_property_read_u32(np, "pmu-phy-offset",
					&exynos_pcie->pmu_phy_offset)) {
		dev_err(pci->dev, "Failed to parse the pmu phy offset\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "ip-ver",
					&exynos_pcie->ip_ver)) {
		dev_err(pci->dev, "Failed to parse the number of ip-ver\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "pcie-clk-num",
					&exynos_pcie->pcie_clk_num)) {
		dev_err(pci->dev, "Failed to parse the number of pcie clock\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "phy-clk-num",
					&exynos_pcie->phy_clk_num)) {
		dev_err(pci->dev, "Failed to parse the number of phy clock\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "ep-device-type",
				&exynos_pcie->ep_device_type)) {
		dev_info(pci->dev,
			"EP device type is NOT defined. (Set to default)\n");
		exynos_pcie->ep_device_type = EP_NO_DEVICE;
	}

	exynos_pcie->use_cache_coherency = false;
	if (!(of_property_read_string(np, "use-cache-coherency",
			&use_cache_coherency)) &&
			!strcmp(use_cache_coherency, "true"))
		exynos_pcie->use_cache_coherency = true;

	dev_info(pci->dev, "PCIe coherency:%d(0:Disabled,1: Enabled)\n",
			exynos_pcie->use_cache_coherency);

	exynos_pcie->use_msi = false;
	if (!(of_property_read_string(np, "use-msi", &use_msi)) &&
			!strcmp(use_msi, "true"))
		exynos_pcie->use_msi = true;

	dev_info(pci->dev, "PCIe MSI:%d(0:Disabled,1: Enabled)\n",
				exynos_pcie->use_msi);

	exynos_pcie->use_sicd = false;
	if (!(of_property_read_string(np, "use-sicd", &use_sicd)) &&
			!strcmp(use_sicd, "true"))
		exynos_pcie->use_sicd = true;

	dev_info(pci->dev, "PCIe SICD:%d(0:Disabled,1: Enabled)\n",
						exynos_pcie->use_sicd);

	exynos_pcie->use_sysmmu = false;
	if (!(of_property_read_string(np, "use-sysmmu", &use_sysmmu)) &&
			!strcmp(use_sysmmu, "true"))
		exynos_pcie->use_sysmmu = true;

	dev_info(pci->dev, "PCIe SysMMU:%d(0:Disabled,1: Enabled)\n",
						exynos_pcie->use_sysmmu);

	exynos_pcie->use_sharability = false;
	if (!(of_property_read_string(np, "use-sharability", &use_sharability))
		&& !strcmp(use_sharability, "true"))
		exynos_pcie->use_sharability = true;

	dev_info(pci->dev, "PCIe RW Sharability:%d(0:Disabled,1: Enabled)\n",
						exynos_pcie->use_sharability);

	exynos_pcie->use_ia = false;
	if (!(of_property_read_string(np, "use-ia", &use_ia)) &&
			!strcmp(use_ia, "true"))
		exynos_pcie->use_ia = true;

	dev_info(pci->dev, "PCIe I/A:%d(0:Disabled,1: Enabled)\n",
						exynos_pcie->use_ia);

	exynos_pcie->sysreg = syscon_regmap_lookup_by_phandle(np,
			"samsung,sysreg-phandle");
	/* Check definitions to access SYSREG in DT*/
	if (IS_ERR(exynos_pcie->sysreg) && IS_ERR(exynos_pcie->sysreg_base)) {
		dev_err(pci->dev, "SYSREG is not defined.\n");
		return PTR_ERR(exynos_pcie->sysreg);
	}

	if (of_property_read_u32(np, "s2mpu-base", &exynos_pcie->s2mpu_base))
		exynos_pcie->s2mpu_base = 0;

	if (of_property_read_u32(np, "vgen-base", &exynos_pcie->vgen_base))
		exynos_pcie->vgen_base = 0;

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Parse configurable device info from dt"
 * @logic "Parse configurable device info from dt"
 * @params
 * @param{in, np, struct ::device_node *, not NULL}
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @retval{-, int, 0, int, Not 0}
 */
static int exynos_v920_pcie_parse_configurable_dt(struct device_node *np,
							struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	const char *use_bifuraction;
	const char *use_sris;
	const char *msi_map;
	int wifi_reg_on;

	if (of_property_read_u32(np, "num-vfs",
					&exynos_pcie->num_vfs)) {
		dev_err(pci->dev, "Failed to parse the number of VFs\n");
		return -EINVAL;
	}

	if (exynos_pcie->num_vfs > MAX_VF_NUM) {
		dev_info(pci->dev, "Requested num_vfs is %d ->",
					exynos_pcie->num_vfs);
		dev_info(pci->dev, "set to MAX_VF_NUM(4)\n");

		exynos_pcie->num_vfs = MAX_VF_NUM;
	}

	if (of_property_read_u32(np, "max-link-speed",
				&exynos_pcie->max_link_speed)) {
		dev_info(pci->dev, "MAX Link Speed is NOT defined...(GEN1)\n");
		/* Default Link Speet is GEN1 */
		exynos_pcie->max_link_speed = LINK_SPEED_GEN1;
	}

	exynos_pcie->use_bifurcation = false;
	if (!(of_property_read_string(np, "use-bifurcation",
					&use_bifuraction)) &&
					!strcmp(use_bifuraction, "true"))
		exynos_pcie->use_bifurcation = true;

	dev_info(pci->dev, "PCIe bifurcation:%d(0:Disabled,1: Enabled)\n",
						exynos_pcie->use_bifurcation);

	exynos_pcie->use_sris = false;
	if (!(of_property_read_string(np, "use-sris", &use_sris)) &&
			!strcmp(use_sris, "true"))
		exynos_pcie->use_sris = true;

	dev_info(pci->dev, "PCIe SRIS:%d(0:Disabled,1: Enabled)\n",
	 exynos_pcie->use_sris);

	wifi_reg_on = of_get_named_gpio(np, "gpio_wifi_reg_on", 0);
	if (wifi_reg_on >= 0) {
		dev_info(pci->dev, "gpio_wifi_reg_on is defined for test.\n");
		gpio_direction_output(wifi_reg_on, 0);
	}

	if (of_property_read_u32(np, "link-flush-time",
				&exynos_pcie->linkflushtime)) {
		dev_info(pci->dev, "Link Flush Time is NOT defined...(0x0)\n");
		exynos_pcie->linkflushtime = PCIE_LINK_FLUSH_TIME_NONE;
	} else {
		dev_info(pci->dev, "Link Flush Time(0x%x)\n",
			 exynos_pcie->linkflushtime);
	}

	if (of_property_read_string(np, "msi-map",
				&msi_map)) {
		dev_info(pci->dev, "msi-map not defined. not using ITS\n");
		exynos_pcie->use_its = false;
	} else {
		dev_info(pci->dev, "msi-map defined. ITS is used\n");
		exynos_pcie->use_its = true;
	}

	return 0;
}


static const struct dw_pcie_ops dw_pcie_ops = {
	.read_dbi = exynos_v920_pcie_read_dbi,
	.write_dbi = exynos_v920_pcie_write_dbi,
	.link_up = exynos_v920_pcie_link_up,
};

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Config pin controls for PCIe RC"
 * @logic "config pin controls from DT."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @param{in/out, pdev, struct ::platform_device *, not NULL}
 * @param{in, np,struct ::device_node *, not NULL}
 * @endparam
 * @retval{ret, int, 0, not NULL, not 0}
 */
static int exynos_v920_pcie_pinconfig(struct pcie_port *pp,
			struct platform_device *pdev, struct device_node *np)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret;

	exynos_pcie->perst_gpio = of_get_gpio(np, 0);

	if (exynos_pcie->perst_gpio < 0) {
		dev_info(pci->dev, "cannot get perst_gpio\n");
	} else {
		/* set perst gpio to output mode low */
		ret = devm_gpio_request_one(pci->dev, exynos_pcie->perst_gpio,
				GPIOF_OUT_INIT_LOW,
				dev_name(pci->dev));
		if (ret)
			return ret;
	}

	/* Get pin state */
	exynos_pcie->pcie_pinctrl = devm_pinctrl_get(pci->dev);
	if (IS_ERR(exynos_pcie->pcie_pinctrl)) {
		dev_err(pci->dev, "Can't get pcie pinctrl!!!\n");
		return -EINVAL;
	}

	exynos_pcie->pin_state[PCIE_PIN_IDLE] =
		pinctrl_lookup_state(exynos_pcie->pcie_pinctrl, "idle");
	if (IS_ERR(exynos_pcie->pin_state[PCIE_PIN_IDLE]))
		dev_info(pci->dev, "No idle pin state\n");

	exynos_pcie->pin_state[PCIE_PIN_DEFAULT] =
		pinctrl_lookup_state(exynos_pcie->pcie_pinctrl, "default");
	if (IS_ERR(exynos_pcie->pin_state[PCIE_PIN_DEFAULT])) {
		dev_err(pci->dev, "Can't set pcie clkerq to output high!\n");
		return -EINVAL;
	}
	else
		pinctrl_select_state(exynos_pcie->pcie_pinctrl,
				exynos_pcie->pin_state[PCIE_PIN_DEFAULT]);

	ret = exynos_v920_pcie_clock_get(pp);
	if (ret)
		return ret;

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Mapping resources for PCIe RC"
 * @logic "mapping resources from DT."
 * @params
 * @param{in/out, pp, struct ::pcie_port *, not NULL}
 * @param{in/out, pdev, struct ::platform_device *, not NULL}
 * @endparam
 * @retval{ret, int, 0, not NULL, not 0}
 */
static int exynos_v920_pcie_resource_mapping(struct pcie_port *pp,
						struct platform_device *pdev)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	struct resource *temp_rsc;

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "elbi");
	exynos_pcie->elbi_base = ioremap(temp_rsc->start,
			resource_size(temp_rsc));

	if (IS_ERR(exynos_pcie->elbi_base))
		return PTR_ERR(exynos_pcie->elbi_base);

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						"gen_subsys");
	exynos_pcie->gen_subsys_base = ioremap(temp_rsc->start,
			resource_size(temp_rsc));

	if (IS_ERR(exynos_pcie->gen_subsys_base))
		return PTR_ERR(exynos_pcie->gen_subsys_base);

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						"e32_phy");
	exynos_pcie->e32_phy_base = ioremap(temp_rsc->start,
			resource_size(temp_rsc));

	if (IS_ERR(exynos_pcie->e32_phy_base))
		return PTR_ERR(exynos_pcie->e32_phy_base);

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
		temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							"soc_ctrl");
		exynos_pcie->soc_ctrl_base = ioremap(temp_rsc->start,
				resource_size(temp_rsc));

		if (IS_ERR(exynos_pcie->soc_ctrl_base))
			return PTR_ERR(exynos_pcie->soc_ctrl_base);

		temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							"pmu");
		exynos_pcie->pmu_base = ioremap(temp_rsc->start,
				resource_size(temp_rsc));

		if (IS_ERR(exynos_pcie->pmu_base))
			return PTR_ERR(exynos_pcie->pmu_base);
	} else {
		temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							"cmu");
		exynos_pcie->cmu_base = ioremap(temp_rsc->start,
				resource_size(temp_rsc));

		if (IS_ERR(exynos_pcie->cmu_base))
			return PTR_ERR(exynos_pcie->cmu_base);
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						"elbi_other");
	exynos_pcie->elbi_base_other = ioremap(temp_rsc->start,
			resource_size(temp_rsc));

	if (IS_ERR(exynos_pcie->elbi_base_other))
		return PTR_ERR(exynos_pcie->elbi_base_other);

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "phy");
	exynos_pcie->phy_base = ioremap(temp_rsc->start,
			resource_size(temp_rsc));

	if (IS_ERR(exynos_pcie->phy_base))
		return PTR_ERR(exynos_pcie->phy_base);

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2 &&
		exynos_pcie->ch_num == PCIE_CH2) {
		/* Use SRAM update only for CH2 */
		temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							"sram");
		exynos_pcie->sram_base = devm_ioremap_resource(pci->dev,
								temp_rsc);

		if (IS_ERR(exynos_pcie->sram_base))
			return PTR_ERR(exynos_pcie->sram_base);
	}

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "sysreg");
	exynos_pcie->sysreg_base = ioremap(temp_rsc->start,
			resource_size(temp_rsc));

	if (IS_ERR(exynos_pcie->sysreg_base))
		return PTR_ERR(exynos_pcie->sysreg_base);

	temp_rsc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dbi");
	exynos_pcie->rc_dbi_base = ioremap(temp_rsc->start,
			resource_size(temp_rsc));
	if (IS_ERR(exynos_pcie->rc_dbi_base))
		return PTR_ERR(exynos_pcie->rc_dbi_base);

	temp_rsc = platform_get_resource_byname(pdev,
					IORESOURCE_MEM, "dbi_shadow");
	exynos_pcie->shadow_dbi_base =
			devm_ioremap_resource(pci->dev, temp_rsc);
	if (IS_ERR(exynos_pcie->shadow_dbi_base))
		return PTR_ERR(exynos_pcie->shadow_dbi_base);

	if (exynos_pcie->use_ia) {
		temp_rsc = platform_get_resource_byname(pdev,
						IORESOURCE_MEM, "ia");
		exynos_pcie->ia_base =
				devm_ioremap_resource(pci->dev, temp_rsc);
		if (IS_ERR(exynos_pcie->ia_base))
			return PTR_ERR(exynos_pcie->ia_base);
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Probe PCIe RC device"
 * @logic "Creat sys file\n
 *	Alloc memory\n
 *	Parse data from dt\n
 *	Set pin control\n
 *	Phy init\n
 *	Phy power down\n
 *	Add PCIe port."
 * @params
 * @param{in/out, pdev, struct ::platform_device *, not NULL}
 * @endparam
 * @retval{ret, int, 0, not NULL, not 0}
 */
static int exynos_v920_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct exynos_pcie *exynos_pcie;
	struct dw_pcie *pci;
	struct pcie_port *pp;
	struct device_node *np;
	int ret = 0;
	int ch_num;
	const int *soc_variant;
	struct iommu_domain *domain;

	if (dev == NULL) {
		ret = -EINVAL;
		goto probe_fail;
	}
	np = pdev->dev.of_node;

	if (create_pcie_sys_file_v920(&pdev->dev))
		dev_warn(&pdev->dev, "Failed to create pcie sys file\n");

	if (of_property_read_u32(np, "ch-num", &ch_num)) {
		dev_err(&pdev->dev, "Failed to parse the channel number\n");
		return -EINVAL;
	}

	/* dynamic memory allocation */
	pci = devm_kzalloc(&pdev->dev, sizeof(*pci), GFP_KERNEL);

	if (!pci)
		return -ENOMEM;

	exynos_pcie = &g_pcie_v920[ch_num];
	pci->dev = &pdev->dev;
	pci->ops = &dw_pcie_ops;

	pp = &pci->pp;
	if (pp == NULL) {
		ret = -EINVAL;
		goto probe_fail;
	}
	exynos_pcie->pci = pci;

	dev_info(pci->dev, "%s: V920 start probe\n", __func__);

	soc_variant = (int *)of_device_get_match_data(dev);
	if (!soc_variant) {
		pr_err("failed to look up compatible string\n");
		return -EINVAL;
	}
	exynos_pcie->soc_variant = *soc_variant;
	dev_info(pci->dev, "%s: Version: %s\n", __func__,
		(exynos_pcie->soc_variant ==
		EXYNOSAUTOV920_EVT2) ? "EVT2" : "EVT01");

	/* Initialize struct exynos_pcie */
	exynos_pcie->ch_num = ch_num;
	exynos_pcie->state = STATE_LINK_DOWN;
	exynos_pcie->linkdown_cnt = 0;
	exynos_pcie->boot_cnt = 0;
	exynos_pcie->atu_ok = 0;  /* Address Translation Unit */

	platform_set_drvdata(pdev, exynos_pcie);

	/* parsing pcie dts data for exynos */
	ret = exynos_v920_pcie_parse_fixed_dt(pdev->dev.of_node, pp);
	if (ret)
		goto probe_fail;

	ret = exynos_v920_pcie_parse_configurable_dt(pdev->dev.of_node, pp);
	if (ret)
		goto probe_fail;

	ret = exynos_v920_pcie_pinconfig(pp, pdev, np);
	if (ret)
		goto probe_fail;

	ret = exynos_v920_pcie_resource_mapping(pp, pdev);
	if (ret)
		goto probe_fail;

	pci->dbi_base = exynos_pcie->rc_dbi_base;

	exynos_pcie->phy_fw_ver = 0;

	exynos_pcie->fw_update = false;
	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2 &&
		exynos_pcie->ch_num == PCIE_CH2) {

		exynos_pcie->pcie_fw_buf = dma_alloc_coherent(pci->dev,
				FW_SIZE, &exynos_pcie->fw_paddr, GFP_KERNEL);
		if (!exynos_pcie->pcie_fw_buf) {
			dma_free_coherent(pci->dev, FW_SIZE,
					  exynos_pcie->pcie_fw_buf,
					  exynos_pcie->fw_paddr);
			dev_err(pci->dev,
				"PCIe CH2 DMA alloc fail for FW update.\n");
		} else {
			memcpy(exynos_pcie->pcie_fw_buf,
				pcie_fw_205_hex, FW_SIZE);
		}

		exynos_pcie->phy_fw_size = FW_SIZE;
	}

	/* Mapping PHY functions */
	exynos_v920_pcie_phy_init(pp);

	if (exynos_pcie->use_sysmmu) {
		samsung_sysmmu_activate(pci->dev);
		pm_runtime_enable(pci->dev);
		pm_runtime_get_sync(pci->dev);
		domain = iommu_get_domain_for_dev(dev);
	}

	if (exynos_pcie->use_cache_coherency)
		enable_cache_coherency_v920(pp, 1);

	ret = exynos_v920_pcie_resumed_phydown(pp);
	if (ret < 0)
		goto probe_fail;

	/* get IRQ number & request irq & settup RC default */
	ret = add_pcie_port_v920(pp, pdev);
	if (ret)
		goto probe_fail;

#ifdef USE_PANIC_NOTIFIER
	/* Register Panic notifier */
	exynos_pcie->ss_dma_mon_nb.notifier_call =
		exynos_v920_pcie_dma_mon_event;
	exynos_pcie->ss_dma_mon_nb.next = NULL;
	exynos_pcie->ss_dma_mon_nb.priority = 0;
	atomic_notifier_chain_register(&panic_notifier_list,
				&exynos_pcie->ss_dma_mon_nb);
#endif

	exynos_pcie->pcie_wq = create_freezable_workqueue("pcie_wq");
	if (IS_ERR(exynos_pcie->pcie_wq)) {
		dev_err(pci->dev, "couldn't create workqueue\n");
		ret = EBUSY;
		goto probe_fail;
	}

	INIT_DELAYED_WORK(&exynos_pcie->dislink_work,
				exynos_v920_pcie_dislink_work);

	INIT_DELAYED_WORK(&exynos_pcie->hardaccess_work,
				exynos_v920_pcie_hardaccess_work);

	platform_set_drvdata(pdev, exynos_pcie);

#ifdef CONFIG_S2MPU
	if (exynos_pcie->s2mpu_base) {
		if (hyp_call_s2mpu_vgen_v9_protection(
			exynos_pcie->s2mpu_base, exynos_pcie->vgen_base) != 0)
			dev_info(&pdev->dev,
				"%s Fail s2mpu v9=%08x vgen v9=%08x\n",
				__func__, exynos_pcie->s2mpu_base,
				exynos_pcie->vgen_base);
		else
			dev_info(&pdev->dev, "%s Success s2mpu v9=%08x\n",
				__func__, exynos_pcie->s2mpu_base);
	} else {
		dev_info(&pdev->dev, "%s s2mpu is disabled [s2mpu=%08x]\n",
			__func__, exynos_pcie->s2mpu_base);
	}
#endif

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT01)
		gpio_set_value(exynos_pcie->perst_gpio, 1);

	exynos_pcie->is_probe_done = true;

probe_fail:
	if (ret)
		dev_err(&pdev->dev, "%s: pcie probe failed\n", __func__);
	else
		dev_info(&pdev->dev, "%s: pcie probe success\n", __func__);

	return ret;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Remove PCIe RC device"
 * @logic "Link down PCIe\n
 *	Remove sysfs file\n
 *	Pm disable."
 * @params
 * @param{in/out, pdev, struct ::platform_device *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_remove(struct platform_device *pdev)
{
	struct exynos_pcie *exynos_pcie = platform_get_drvdata(pdev);
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;

	if (exynos_pcie->state != STATE_LINK_DOWN)
		exynos_v920_pcie_poweroff(exynos_pcie->ch_num);

	if (pp->msi_data) {
		dma_unmap_page(&pdev->dev, pp->msi_data, PAGE_SIZE,
			       DMA_FROM_DEVICE);
		if (pp->msi_page)
			__free_page(pp->msi_page);
	}

	remove_pcie_sys_file_v920(&pdev->dev);

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2 &&
		exynos_pcie->ch_num == PCIE_CH2) {

		if (exynos_pcie->pcie_fw_buf)
			dma_free_coherent(pci->dev, FW_SIZE,
					exynos_pcie->pcie_fw_buf,
					exynos_pcie->fw_paddr);
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Shutdown PCIe RC"
 * @logic "Linkdown PCIe\n
 *	Unregister panic notifier"
 * @params
 * @param{in/out, pdev, struct :;platform_device *, not NULL}
 * @endparam
 * @noret
 */
static void exynos_v920_pcie_shutdown(struct platform_device *pdev)
{
	struct exynos_pcie *exynos_pcie = platform_get_drvdata(pdev);
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;
#ifdef USE_PANIC_NOTIFIER
	int ret;
#endif

	if (exynos_pcie->state == STATE_LINK_DOWN) {
		dev_info(&pdev->dev,
			"[%s] PCIe interface controls PCIe link %d\n",
			__func__, exynos_pcie->ch_num);
	} else {
		dev_info(&pdev->dev,
			"[%s] PCIe interface does not control PCIe link %d\n",
			__func__, exynos_pcie->ch_num);
		exynos_v920_pcie_poweroff(exynos_pcie->ch_num);
	}

	/*	For DBI Access in Reboot sequence */
	if (exynos_pcie->phy_ops.phy_all_pwrdn_clear != NULL)
		exynos_pcie->phy_ops.phy_all_pwrdn_clear(pp);

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2 &&
		exynos_pcie->ch_num == PCIE_CH2) {

		if (exynos_pcie->pcie_fw_buf)
			dma_free_coherent(pci->dev, FW_SIZE,
					exynos_pcie->pcie_fw_buf,
					exynos_pcie->fw_paddr);
	}

#ifdef USE_PANIC_NOTIFIER
	ret = atomic_notifier_chain_unregister(&panic_notifier_list,
			&exynos_pcie->ss_dma_mon_nb);
	if (ret)
		dev_warn(&pdev->dev,
			"Failed to unregister snapshot panic notifier\n");
#endif
}

static const struct of_device_id exynos_v920_pcie_of_match[] = {
	{ .compatible = "samsung,exynos-v920-evt01-pcie-rc",
			.data = &exynosautov920_evt01 },
	{ .compatible = "samsung,exynos-v920-evt2-pcie-rc",
			.data = &exynosautov920_evt2 },
	{},
};
MODULE_DEVICE_TABLE(of, exynos_v920_pcie_of_match);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_init}
 * @purpose "Power down phy"
 * @logic "Reset phy\n
 *	Phy power down."
 * @params
 * @param{in, pp, struct ::pcie_port *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_resumed_phydown(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	int ret;

	/* phy all power down on wifi off during suspend/resume */
	ret = exynos_v920_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
	dev_info(pci->dev, "pcie clk enable, ret value = %d\n", ret);

	exynos_v920_pcie_enable_interrupts(pp);

	ret = exynos_v920_pcie_assert_phy_reset(pp);
	if (ret < 0)
		return ret;

	/* phy all power down */
	if (exynos_pcie->phy_ops.phy_all_pwrdn != NULL)
		exynos_pcie->phy_ops.phy_all_pwrdn(pp);

	exynos_v920_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);

	exynos_v920_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);

	return ret;
}

static void exynos_v920_pcie_history_buffer_enable(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pci);
	u32 val = 0;

	/* Disable HSI_DEBUG */
	writel(0x0, exynos_pcie->soc_ctrl_base + PCIE_DEBUG_MODE);

	/* debug_mask */
	val = readl(exynos_pcie->soc_ctrl_base + PCIE_DEBUG_SIGNAL_MASK);
	writel(val | (0xFF << (LTSSM_STATE_0)) | (0xF << (L1_SUB_STATE_0)) |
		(0xF << (PHY_LOCK_STATE_0)),
		exynos_pcie->soc_ctrl_base + PCIE_DEBUG_SIGNAL_MASK);

	/* debug_store_start stm_mode_en history buffer mode en  */
	val = readl(exynos_pcie->soc_ctrl_base + PCIE_DEBUG_MODE);
	writel(val | 0x1 << (DEBUG_STORE_START) | 0x1 << (DEBUG_STM_MODEM_EN)
		| 0x1 << (DEBUG_BUFFER_MODE_EN),
		exynos_pcie->soc_ctrl_base + PCIE_DEBUG_MODE);

	/* Enable HSI_DEBUG */
	val = readl(exynos_pcie->soc_ctrl_base + PCIE_DEBUG_MODE);
	writel(val | 0x1 << (HSI_DEBUG_EN),
		exynos_pcie->soc_ctrl_base + PCIE_DEBUG_MODE);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_link}
 * @purpose "Link up PCIe"
 * @logic "Enable clock\n
 *	Enable sysmmu\n
 *	Enable phy\n
 *	Power on phy\n
 *	Call establish link\n
 *	Set payload and scan bus\n
 *	Initialize msi."
 * @params
 * @param{in, ch_num, int, 0~5}
 * @endparam
 * @retval{-, int, 0, -EPIPE~0, -EPIPE}
 */
int exynos_v920_pcie_poweron(int ch_num)
{
	struct exynos_pcie *exynos_pcie = &g_pcie_v920[ch_num];
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;

	u32 val, vendor_id, device_id, busdev, type;
	int max_payload = (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) ?
		EXYNOS_PCIE_EVT2_MAX_PAYLOAD : EXYNOS_PCIE_EVT01_MAX_PAYLOAD;
	int ret;

	dev_info(pci->dev, "[%s] start of poweron, pcie state: %d\n", __func__,
							 exynos_pcie->state);
	if (exynos_pcie->state == STATE_LINK_DOWN) {
		ret = exynos_v920_pcie_clock_enable(pp, PCIE_ENABLE_CLOCK);
		dev_info(pci->dev, "pcie clk enable, ret value = %d\n", ret);

		/* Enable SysMMU */
		if (exynos_pcie->use_sysmmu)
			pcie_sysmmu_enable(ch_num);
		pinctrl_select_state(exynos_pcie->pcie_pinctrl,
				exynos_pcie->pin_state[PCIE_PIN_DEFAULT]);

		/* phy all power down clear */
		if (exynos_pcie->phy_ops.phy_all_pwrdn_clear != NULL)
			exynos_pcie->phy_ops.phy_all_pwrdn_clear(pp);

		exynos_pcie->state = STATE_LINK_UP_TRY;

		/* Enable history buffer */
		if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2)
			exynos_v920_pcie_history_buffer_enable(pp);

		if (exynos_v920_pcie_establish_link(pp)) {
			dev_err(pci->dev, "pcie link up fail\n");
			goto poweron_fail;
		}
		exynos_pcie->state = STATE_LINK_UP;

		if (!exynos_pcie->probe_ok) {
			exynos_v920_pcie_rd_own_conf(pp, PCI_VENDOR_ID,
							4, &val);
			vendor_id = val & ID_MASK;
			device_id = (val >> 16) & ID_MASK;

			exynos_pcie->pci_dev = pci_get_device(vendor_id,
							device_id, NULL);
			if (!exynos_pcie->pci_dev) {
				dev_err(pci->dev, "Failed to get pci device\n");
				goto poweron_fail;
			}

			val = pcie_get_mps(exynos_pcie->pci_dev);

			if (val > max_payload)
				val = max_payload;

			/* set max payload size */
			pcie_set_mps(exynos_pcie->pci_dev, val);

#ifdef CONFIG_PCI_MSI
			exynos_v920_pcie_msi_init(pp);
#endif

			pci_rescan_bus(exynos_pcie->pci_dev->bus);

			if (pci_save_state(exynos_pcie->pci_dev)) {
				dev_err(pci->dev,
					"Failed to save pcie state\n");
				goto poweron_fail;
			}
			exynos_pcie->pci_saved_configs =
				pci_store_saved_state(exynos_pcie->pci_dev);
			exynos_pcie->probe_ok = 1;
		} else {
#ifdef CONFIG_PCI_MSI
			exynos_v920_pcie_msi_init(pp);
#endif
			if (pci_load_saved_state(exynos_pcie->pci_dev,
					     exynos_pcie->pci_saved_configs)) {
				dev_err(pci->dev,
					"Failed to load pcie state\n");
				goto poweron_fail;
			}
			pci_restore_state(exynos_pcie->pci_dev);

			/* IF Device is Force removed, rescan again */
			if (!exynos_pcie->pci_dev->is_managed)
				pci_rescan_bus(exynos_pcie->pci_dev->bus);
		}

		/* setup ATU for cfg/mem outbound */
		busdev = PCIE_ATU_BUS(
				exynos_pcie->pci_dev->subordinate->number) |
				PCIE_ATU_DEV(0) | PCIE_ATU_FUNC(0);

		if (exynos_pcie->pci_dev->subordinate->number == 1)
			type = PCIE_ATU_TYPE_CFG0;
		else
			type = PCIE_ATU_TYPE_CFG1;

		exynos_v920_pcie_prog_viewport_cfg(pp, busdev, type);
		exynos_v920_pcie_prog_viewport_mem_outbound(pp);
		dev_info(pci->dev,
			"[%s]pci_dev->subordinate->number:%d, busdev:%x\n",
			__func__, exynos_pcie->pci_dev->subordinate->number,
			busdev);

		exynos_v920_pcie_rd_own_conf(pp, PCIE_LINK_CTRL_STAT, 4, &val);
		val = (val >> 16) & 0xf;
		dev_info(pci->dev, "Current Link Speed is GEN%d (MAX GEN%d)\n",
				val, exynos_pcie->max_link_speed);
	}

	dev_info(pci->dev, "[%s] end of poweron, pcie state: %d\n", __func__,
		 exynos_pcie->state);

	return 0;

poweron_fail:
	exynos_pcie->state = STATE_LINK_UP;
	exynos_v920_pcie_poweroff(exynos_pcie->ch_num);

	return -EPIPE;
}
EXPORT_SYMBOL(exynos_v920_pcie_poweron);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_link}
 * @purpose "Link down PCIe"
 * @logic "Send link down interrupt\n
 *	Send pme turn off\n
 *	Update state using vlink\n
 *	Set PERST# low\n
 *	Disable LTSSM, phy, clock."
 * @params
 * @param{in, ch_num, int, 0~5}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_poweroff(int ch_num)
{
	struct exynos_pcie *exynos_pcie = &g_pcie_v920[ch_num];
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;
	unsigned long flags;
	u32 val;
	u32 lft_sleep = 0;

	dev_info(pci->dev, "[%s] start of poweroff, pcie state: %d\n", __func__,
		 exynos_pcie->state);

	if (exynos_pcie->state == STATE_LINK_UP ||
	    exynos_pcie->state == STATE_LINK_DOWN_TRY) {
		exynos_pcie->state = STATE_LINK_DOWN_TRY;
		val = exynos_elbi_read(exynos_pcie, PCIE_ERR_INT_CTRL);
		val &= ~PCIE_IRQ_LINK_DOWN;
		exynos_elbi_write(exynos_pcie, val, PCIE_ERR_INT_CTRL);

		if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2)
			exynos_elbi_write(exynos_pcie,
					  EVT2_PCIE_PME_ACK_INT_EN,
					  PCIE_RX_MSG_INT_EN);
		else
			exynos_elbi_write(exynos_pcie,
					  EVT01_PCIE_PME_ACK_INT_EN,
					  PCIE_RX_MSG_INT_EN);

		spin_lock_irqsave(&exynos_pcie->conf_lock, flags);
		exynos_v920_pcie_send_pme_turn_off(exynos_pcie);
		exynos_pcie->state = STATE_LINK_DOWN;

		/* Disable SysMMU */
		if (exynos_pcie->use_sysmmu)
			pcie_sysmmu_disable(ch_num);

		/* Disable history buffer */
		if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2)
			writel(0x0,
				exynos_pcie->soc_ctrl_base + PCIE_DEBUG_MODE);

		/* Delay for EP L2 Entering */
		mdelay(10);

		gpio_set_value(exynos_pcie->perst_gpio, PERST_LOW);
		dev_info(pci->dev, "%s: Set PERST to LOW, gpio val = %d\n",
			__func__, gpio_get_value(exynos_pcie->perst_gpio));

		/* Do Phy Warm Reset Before ELBI Access */
		if (ch_num == PCIE_CH0 || ch_num == PCIE_CH2) {
			/* PE0 WARM reset */
			val = readl(exynos_pcie->gen_subsys_base +
					GENERAL_SS_RST_CTRL_1);

			writel(val | (0x1 << 1),
					exynos_pcie->gen_subsys_base +
					GENERAL_SS_RST_CTRL_1);
			udelay(10);
			writel(val & ~(0x1 << 1),
					exynos_pcie->gen_subsys_base +
					GENERAL_SS_RST_CTRL_1);

		} else if (ch_num == PCIE_CH1 || ch_num == PCIE_CH3) {
			/* PE1 WARM reset */
			val = readl(exynos_pcie->gen_subsys_base +
					GENERAL_SS_RST_CTRL_1);

			writel(val | (0x1 << 9),
					exynos_pcie->gen_subsys_base +
					GENERAL_SS_RST_CTRL_1);
			udelay(10);
			writel(val & ~(0x1 << 9),
					exynos_pcie->gen_subsys_base +
					GENERAL_SS_RST_CTRL_1);
		}
		dev_info(pci->dev, "%s: PHY Reset done before accessing ELBI\n",
			__func__);

		/* LTSSM disable */
		exynos_elbi_write(exynos_pcie, PCIE_LTSSM_DISABLE,
				PCIE_GEN_CTRL_3);

		/* phy all power down */
		if (exynos_pcie->phy_ops.phy_all_pwrdn != NULL)
			exynos_pcie->phy_ops.phy_all_pwrdn(pp);

		spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);

		lft_sleep = exynos_pcie->linkflushtime;
		if (lft_sleep > PCIE_LINK_FLUSH_TIME_NONE) {
			if (lft_sleep <= PCIE_LINK_FLUSH_TIME_1MS_VALUE)
				lft_sleep = PCIE_LINK_FLUSH_TIME_1MS_VALUE;

			lft_sleep = PCIE_LFT_SLEEP_CAL(lft_sleep);
			msleep(lft_sleep);
		}

		exynos_v920_pcie_phy_clock_enable(pp, PCIE_DISABLE_CLOCK);

		exynos_v920_pcie_clock_enable(pp, PCIE_DISABLE_CLOCK);

		exynos_pcie->atu_ok = 0;

		if (!IS_ERR(exynos_pcie->pin_state[PCIE_PIN_IDLE]))
			pinctrl_select_state(exynos_pcie->pcie_pinctrl,
					exynos_pcie->pin_state[PCIE_PIN_IDLE]);
	}

	dev_info(pci->dev, "[%s] end of poweroff, pcie state: %d (LFT_S:%d)\n",
		 __func__, exynos_pcie->state, lft_sleep);
}
EXPORT_SYMBOL(exynos_v920_pcie_poweroff);

bool exynos_v920_pcie_get_probe_done(int ch_num)
{
	struct exynos_pcie *exynos_pcie = &g_pcie_v920[ch_num];

	return exynos_pcie->is_probe_done;
}
EXPORT_SYMBOL(exynos_v920_pcie_get_probe_done);

struct device *exynos_v920_pcie_get_dev(u8 ch_num)
{
	struct exynos_pcie *exynos_pcie = &g_pcie_v920[ch_num];
	struct dw_pcie *pci = NULL;

	if (exynos_pcie->pci == NULL)
		return NULL;

	pci = exynos_pcie->pci;

	return pci->dev;
}
EXPORT_SYMBOL(exynos_v920_pcie_get_dev);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_link}
 * @purpose "Send pme turn off to EP"
 * @logic "Send pme turn off to EP\n
 *	Wait enter L23 ready packet."
 * @params
 * @param{in, exynos_pcie, struct ::exynos_pcie *, not NULL}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_send_pme_turn_off(struct exynos_pcie *exynos_pcie)
{
	struct dw_pcie *pci = exynos_pcie->pci;
	struct device *dev = pci->dev;
	int __maybe_unused count = 0;
	int __maybe_unused retry_cnt = 0;
	u32 __maybe_unused val;

	val = exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2) &
		PCIE_LINK_LTSSM_MASK;
	dev_info(dev, "%s: link state:%x\n", __func__, val);
	if (!(val >= S_RCVRY_LOCK && val <= S_L1_IDLE)) {
		dev_info(dev, "[%s] pcie link is not up\n", __func__);
		return;
	}

	exynos_elbi_write(exynos_pcie, PCIE_EXIT_ASPM_L1_ENABLE,
			PCIE_APP_REQ_EXIT_L1);

retry_pme_turnoff:
	val = exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2) &
		PCIE_LINK_LTSSM_MASK;
	dev_info(dev, "Current LTSSM State is 0x%x with retry_cnt =%d.\n",
			val, retry_cnt);

	exynos_elbi_write(exynos_pcie, PCIE_PME_TURNOFF_REQ, PCIE_TX_MSG_REQ);

	while (count < PHY_TIMEOUT) {
		if ((exynos_elbi_read(exynos_pcie, PCIE_RX_MSG_INT_STS)
			& PCIE_PM_TO_ACK)) {
			dev_info(dev, "ack message is ok\n");
			udelay(10);
			break;
		}

		udelay(10);
		count++;
	}
	if (count >= PHY_TIMEOUT)
		dev_info(dev, "cannot receive ack message from wifi\n");

	exynos_elbi_write(exynos_pcie, PCIE_PME_TURNOFF_REQ_RST,
			PCIE_TX_MSG_REQ);

	count = 0;
	do {
		val = exynos_elbi_read(exynos_pcie, PCIE_LINK_DBG_2);
		val = val & PCIE_LINK_LTSSM_MASK;
		if (val == S_L2_IDLE) {
			dev_info(dev, "received Enter_L23_READY DLLP packet\n");
			break;
		}
		udelay(10);
		count++;
	} while (count < PHY_TIMEOUT);

	if (count >= PHY_TIMEOUT) {
		if (retry_cnt < 10) {
			retry_cnt++;
			goto retry_pme_turnoff;
		}
		dev_info(dev, "cannot receive L23_READY DLLP packet(0x%x)\n",
			val);
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Suspend PCIe RC"
 * @logic "Link down PCIe"
 * @params
 * @param{in/out, ch_num, int, 0~5}
 * @endparam
 * @noret
 */
void exynos_v920_pcie_pm_suspend(int ch_num)
{
	struct exynos_pcie *exynos_pcie = &g_pcie_v920[ch_num];
	struct dw_pcie *pci = exynos_pcie->pci;
	unsigned long flags;

	spin_lock_irqsave(&exynos_pcie->conf_lock, flags);

	if (exynos_pcie->state == STATE_LINK_DOWN) {
		dev_info(pci->dev, "RC%d already off\n", exynos_pcie->ch_num);
		spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);
		return;
	}

	exynos_pcie->state = STATE_LINK_DOWN_TRY;
	spin_unlock_irqrestore(&exynos_pcie->conf_lock, flags);

	exynos_v920_pcie_poweroff(ch_num);
}
EXPORT_SYMBOL(exynos_v920_pcie_pm_suspend);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Resume PCIe RC"
 * @logic "Link up PCIe"
 * @params
 * @param{in, ch_num, int, 0~5}
 * @endparam
 * @retval{-, int, 0, -EPIPE~0, -EPIPE}
 */
int exynos_v920_pcie_pm_resume(int ch_num)
{
	return exynos_v920_pcie_poweron(ch_num);
}
EXPORT_SYMBOL(exynos_v920_pcie_pm_resume);

#ifdef CONFIG_PM
/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Suspend PCIe RC without irq"
 * @logic "Link down PCIe"
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_suspend_noirq(struct device *dev)
{
	struct exynos_pcie *exynos_pcie = dev_get_drvdata(dev);
	int ret_hwacg = 0;

	if (exynos_pcie->state == STATE_LINK_DOWN) {
		dev_info(dev, "[%s] PCIe interface controls PCIe link %d\n",
			__func__, exynos_pcie->ch_num);
		exynos_pcie->s2r_if_control = true;
	} else {
		dev_info(dev,
			"[%s] PCIe interface does not control PCIe link %d\n",
			__func__, exynos_pcie->ch_num);
		exynos_pcie->s2r_if_control = false;
		exynos_v920_pcie_poweroff(exynos_pcie->ch_num);
	}

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
		ret_hwacg = exynos_v920_pcie_dbiclk_enable(dev,
							   &exynos_pcie->clks,
							   false);
		if (ret_hwacg)
			dev_err(dev, "%s Fail to disable dbi_clk(%d)\n",
				__func__, ret_hwacg);
		else
			dev_info(dev, "%s Success to disable dbi_clk\n",
				 __func__);
	}

	return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Resume PCIe RC without irq"
 * @logic "If link is down, power down phy\n
 *	If not, try link up."
 * @params
 * @param{in/out, dev, struct ::device *, not NULL}
 * @endparam
 * @retval{-, int, 0, 0, not 0}
 */
static int exynos_v920_pcie_resume_noirq(struct device *dev)
{
	struct exynos_pcie *exynos_pcie = dev_get_drvdata(dev);
	struct dw_pcie *pci = exynos_pcie->pci;
	struct pcie_port *pp = &pci->pp;
	int i, ret = 0, ret_hwacg = 0;

	dev_info(dev, "[%s] PCIe ch%d Resume: Interface control:%d\n",
		__func__, exynos_pcie->ch_num, exynos_pcie->s2r_if_control);

	if (exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2) {
		ret_hwacg = exynos_v920_pcie_dbiclk_enable(dev,
							   &exynos_pcie->clks,
							   true);
		if (ret_hwacg)
			dev_err(dev, "%s Fail to enalbe dbi_clk(%d)\n",
				__func__, ret_hwacg);
		else
			dev_info(dev, "%s Success to enable dbi_clk\n",
				 __func__);
	}

	if (exynos_pcie->ch_num == PCIE_CH2 &&
		exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2 &&
		exynos_pcie->evt2_minor_ver == EVT20)
		/* For Link A only */
		exynos_pcie->fw_update = true;

	exynos_pcie->phy_fw_ver = 0;

	if (!exynos_pcie->s2r_if_control) {
		if ((exynos_pcie->ch_num == PCIE_CH2 ||
			exynos_pcie->ch_num == PCIE_CH3) &&
			exynos_pcie->soc_variant == EXYNOSAUTOV920_EVT2 &&
			exynos_pcie->evt2_minor_ver == EVT20) {
			/* 2L phy need to check FW */

			for (i = 0; i < PCIE_RC_RETRY; i++) {
				/* only Link A should try FW update */
				if (exynos_pcie->phy_ops.phy_config != NULL &&
					exynos_pcie->fw_update &&
					exynos_pcie->ch_num == PCIE_CH2)
					ret = exynos_pcie->phy_ops.phy_config(
						pp, exynos_pcie->fw_update);

				if (!ret)
					dev_err(dev,
						"FW update Fail [%d/10]\n", i);

				if (exynos_pcie->phy_fw_ver == FW_VER_205)
					break;
			}

			ret = 0;

			if (exynos_pcie->phy_fw_ver == FW_VER_205) {
				exynos_pcie->fw_update = false;

				exynos_v920_pcie_poweron(exynos_pcie->ch_num);
			} else if (exynos_pcie->phy_fw_ver != 0) {
				dev_err(dev, "FW is not 205VER FW ver:%x\n",
					exynos_pcie->phy_fw_ver);
				exynos_v920_pcie_poweron(exynos_pcie->ch_num);
			} else {
				dev_err(dev,
				"PCIe PHY is not configured. FW ver:%x\n",
				exynos_pcie->phy_fw_ver);
			}
		} else {
			/* For 4L phy */
			exynos_v920_pcie_poweron(exynos_pcie->ch_num);
		}
	}

	return ret;
}
#else
#define exynos_pcie_suspend_noirq	NULL
#define exynos_pcie_resume_noirq	NULL
#endif

static const struct dev_pm_ops exynos_pcie_dev_pm_ops = {
	.suspend_noirq	= exynos_v920_pcie_suspend_noirq,
	.resume_noirq	= exynos_v920_pcie_resume_noirq,
};

static struct platform_driver exynos_pcie_driver = {
	.probe		= exynos_v920_pcie_probe,
	.remove		= exynos_v920_pcie_remove,
	.shutdown	= exynos_v920_pcie_shutdown,
	.driver = {
		.name	= "exynos-v920-pcie",
		.owner	= THIS_MODULE,
		.of_match_table = exynos_v920_pcie_of_match,
		.pm	= &exynos_pcie_dev_pm_ops,
	},
};

/* Exynos PCIe driver does not allow module unload */

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "initialize PCIe RC"
 * @logic "register PCIe RC driver"
 * @noparam
 * @retval{-, int, 0, -EINVAL~0, not 0}
 */
static int pcie_init_v920(void)
{
	return platform_driver_register(&exynos_pcie_driver);
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "remove PCIe RC"
 * @logic "unregister PCIe RC driver"
 * @noparam
 * @retval{-, int, 0, -EINVAL~0, not 0}
 */
static void pcie_remove_v920(void)
{
	platform_driver_unregister(&exynos_pcie_driver);
}


/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Register event"
 * @logic "Register event"
 * @params
 * @param{in, reg, struct ::exynos_pcie_register_event *, not NULL}
 * @endparam
 * @retval{ret, int, 0, 0/-ENODEV, -ENODEV}
 */
int exynos_v920_pcie_register_event(struct exynos_pcie_register_event *reg)
{
	int ret = 0;
	struct pcie_port *pp;
	struct exynos_pcie *exynos_pcie;
	struct dw_pcie *pci;

	if (!reg) {
		dev_err(NULL, "PCIe: Event registration is NULL\n");
		return -ENODEV;
	}
	if (!reg->user) {
		dev_err(NULL, "PCIe: User of event registration is NULL\n");
		return -ENODEV;
	}
	pp = PCIE_BUS_PRIV_DATA(((struct pci_dev *)reg->user));

	if (pp) {
		pci = to_dw_pcie_from_pp(pp);
		exynos_pcie = to_exynos_pcie(pci);
		exynos_pcie->event_reg = reg;
		dev_info(pci->dev,
				"Event 0x%x is registered for RC %d\n",
				reg->events, exynos_pcie->ch_num);
	} else {
		dev_err(NULL,
			"PCIe: did not find RC for pci endpoint device\n");
		ret = -ENODEV;
	}
	return ret;
}
EXPORT_SYMBOL(exynos_v920_pcie_register_event);

/**
 * @cnotice
 * @prdcode
 * @unit_name{pcie_rc_internal}
 * @purpose "Deregister evnet"
 * @logic "Deregister events for PCIe RC"
 * @params
 * @param{in, reg, struct ::exynos_pcie_register_event *, not NULL}
 * @endparam
 * @retval{ret, int, 0, 0/-ENODEV, -ENODEV}
 */
int exynos_v920_pcie_deregister_event(struct exynos_pcie_register_event *reg)
{
	int ret = 0;
	struct pcie_port *pp;
	struct exynos_pcie *exynos_pcie;
	struct dw_pcie *pci;

	if (!reg) {
		dev_err(NULL, "PCIe: Event deregistration is NULL\n");
		return -ENODEV;
	}
	if (!reg->user) {
		dev_err(NULL, "PCIe: User of event deregistration is NULL\n");
		return -ENODEV;
	}
	pp = PCIE_BUS_PRIV_DATA(((struct pci_dev *)reg->user));

	if (pp) {
		pci = to_dw_pcie_from_pp(pp);
		exynos_pcie = to_exynos_pcie(pci);
		exynos_pcie->event_reg = NULL;
		dev_info(pci->dev, "Event is deregistered for RC %d\n",
				exynos_pcie->ch_num);
	} else {
		dev_err(NULL,
			"PCIe: did not find RC for pci endpoint device\n");
		ret = -ENODEV;
	}
	return ret;
}
EXPORT_SYMBOL(exynos_v920_pcie_deregister_event);

module_init(pcie_init_v920);
module_exit(pcie_remove_v920);
MODULE_AUTHOR("Kwangho Kim <kwangho2.kim@samsung.com>");
MODULE_AUTHOR("Jiheon Oh <jiheon.oh@samsung.com>");
MODULE_DESCRIPTION("Samsung PCIe RC driver");
MODULE_LICENSE("GPL v2");
