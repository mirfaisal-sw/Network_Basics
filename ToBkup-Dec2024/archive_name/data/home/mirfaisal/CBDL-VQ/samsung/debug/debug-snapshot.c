// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/of_reserved_mem.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/arm-smccc.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/console.h>
#include <linux/android_debug_symbols.h>
#include <linux/kmsg_dump.h>
#include <linux/mutex.h>

#include <soc/samsung/exynos-smc.h>
#include "debug-snapshot-local.h"

#ifdef CONFIG_DEBUG_SNAPSHOT_USE_ANDROID_VH_LOGBUF
#include <trace/hooks/logbuf.h>
#include <linux/sched.h>
#include "../../../../kernel/printk/printk_ringbuffer.h"
#endif

#define DSS_VERSION	(1 << 28)
#define DSS_MAGIC	0x0D550D55
#define DSS_BL_MAGIC	0x1234ABCD
#define DSS_ITEM_NAME_LEN	(SZ_16)

struct dbg_snapshot_bl {
	unsigned int magic;
	unsigned int item_count;
	struct {
		char name[DSS_ITEM_NAME_LEN];
		unsigned long paddr;
		unsigned long size;
	} item[DSS_MAX_BL_SIZE];
};

struct dbg_snapshot_interface {
	struct dbg_snapshot_log *info_event;
};

static struct dbg_snapshot_bl *dss_bl;
static struct dbg_snapshot_item dss_items[] = {
	[DSS_ITEM_HEADER_ID] = {
		.name = DSS_ITEM_HEADER,
		.persist = true,
		.entry = {
			.enabled  = true,
		},
	},
	[DSS_ITEM_KERNEL_ID] = {
		.name = DSS_ITEM_KERNEL,
		.persist = true,
		.entry = {
			.enabled = false,
		},
	},
	[DSS_ITEM_PLATFORM_ID] = {
		.name = DSS_ITEM_PLATFORM,
		.persist = true,
		.entry = {
			.enabled = false,
		},
	},
	[DSS_ITEM_KEVENTS_ID]	= {
		.name = DSS_ITEM_KEVENTS,
		.persist = false,
		.entry = {
			.enabled = false,
		},
	},
	[DSS_ITEM_S2D_ID] = {
		.name = DSS_ITEM_S2D,
		.persist = false,
		.entry = {
			.enabled = true,
		},
	},
	[DSS_ITEM_ARRDUMP_RESET_ID] = {
		.name = DSS_ITEM_ARRDUMP_RESET,
		.persist = false,
		.entry = {
			.enabled = false,
		},
	},
	[DSS_ITEM_ARRDUMP_PANIC_ID] = {
		.name = DSS_ITEM_ARRDUMP_PANIC,
		.persist = false,
		.entry = {
			.enabled = false,
		},
	},
	[DSS_ITEM_FIRST_ID] = {
		.name = DSS_ITEM_FIRST,
		.persist = false,
		.entry = {
			.enabled = false,
		},
	},
	[DSS_ITEM_LAST_ID] = {
		.name = DSS_ITEM_LAST,
		.persist = false,
		.entry = {
			.enabled = false,
		},
	},
	[DSS_ITEM_BCM_ID] = {
		.name = DSS_ITEM_BCM,
		.persist = false,
		.entry = {
			.enabled = true,
		},
	},
	[DSS_ITEM_KMODULE_ID] = {
		.name = DSS_ITEM_KMODULE,
		.persist = false,
		.entry = {
			.enabled = true,
		},
	},
	[DSS_ITEM_DBGC_HEADER_ID] = {
		.name = DSS_ITEM_DBGC_HEADER,
		.persist = false,
		.entry = {
			.enabled = true,
		},
	},
	[DSS_ITEM_SFRDUMP_ID] = {
		.name = DSS_ITEM_SFRDUMP,
		.persist = false,
		.entry = {
			.enabled = true,
		},
	},
};

/*  External interface variable for trace debugging */
static struct dbg_snapshot_interface dss_info __attribute__ ((used));
static struct dbg_snapshot_interface *ess_info __attribute__ ((used));
char *last_kmsg;
EXPORT_SYMBOL_GPL(last_kmsg);
int last_kmsg_size;
EXPORT_SYMBOL_GPL(last_kmsg_size);

static char *dss_kmsg;
static size_t dss_kmsg_size;
static DEFINE_MUTEX(dss_kmsg_mutex);

static struct dbg_snapshot_base *dss_base;
static struct dbg_snapshot_base *ess_base __attribute__ ((used));

struct dbg_snapshot_log *dss_log;
struct dbg_snapshot_desc dss_desc;

int dbg_snapshot_add_bl_item_info(const char *name, unsigned long paddr,
				  unsigned long size)
{
	unsigned int i;

	if (!paddr || !size) {
		dev_err(dss_desc.dev, "Invalid address(0x%lx) or size(0x%lx)\n",
				paddr, size);
		return -EINVAL;
	}

	if (!dss_bl || dss_bl->item_count >= DSS_MAX_BL_SIZE) {
		dev_err(dss_desc.dev, "Item list full or null!\n");
		return -ENOMEM;
	}

	for (i = 0; i < dss_bl->item_count; i++) {
		if (!strncmp(dss_bl->item[i].name, name,
					DSS_ITEM_NAME_LEN - 1)) {
			dev_err(dss_desc.dev, "Already has %s item\n", name);
			return -EEXIST;
		}
	}

	strncpy(dss_bl->item[dss_bl->item_count].name, name,
			DSS_ITEM_NAME_LEN - 1);
	dss_bl->item[dss_bl->item_count].paddr = paddr;
	dss_bl->item[dss_bl->item_count].size = size;
	dss_bl->item_count++;

	return 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_add_bl_item_info);

void __iomem *dbg_snapshot_get_header_vaddr(void)
{
	if (dbg_snapshot_get_item_enable(DSS_ITEM_HEADER))
		return (void __iomem *)(dss_items[DSS_ITEM_HEADER_ID].entry.vaddr);
	else
		return NULL;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_header_vaddr);

unsigned long dbg_snapshot_get_header_paddr(void)
{
	if (dbg_snapshot_get_item_enable(DSS_ITEM_HEADER))
		return (unsigned long)dss_items[DSS_ITEM_HEADER_ID].entry.paddr;
	else
		return 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_header_paddr);

unsigned int dbg_snapshot_get_val_offset(unsigned int offset)
{
	if (dbg_snapshot_get_item_enable(DSS_ITEM_HEADER))
		return __raw_readl(dbg_snapshot_get_header_vaddr() + offset);
	else
		return 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_val_offset);

void dbg_snapshot_set_val_offset(unsigned int val, unsigned int offset)
{
	if (dbg_snapshot_get_item_enable(DSS_ITEM_HEADER))
		__raw_writel(val, dbg_snapshot_get_header_vaddr() + offset);
}
EXPORT_SYMBOL_GPL(dbg_snapshot_set_val_offset);

u64 dbg_snapshot_get_val64_offset(unsigned int offset)
{
	if (dbg_snapshot_get_item_enable(DSS_ITEM_HEADER))
		return readq_relaxed(dbg_snapshot_get_header_vaddr() + offset);
	else
		return 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_val64_offset);

void dbg_snapshot_set_val64_offset(u64 val, unsigned int offset)
{
	if (dbg_snapshot_get_item_enable(DSS_ITEM_HEADER))
		writeq_relaxed(val, dbg_snapshot_get_header_vaddr() + offset);
}
EXPORT_SYMBOL_GPL(dbg_snapshot_set_val64_offset);

static void dbg_snapshot_set_linux_banner(void)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();
	void *banner_addr = android_debug_symbol(ADS_LINUX_BANNER);
	size_t banner_len;

	memset(header + DSS_OFFSET_LINUX_BANNER, 0, SZ_512);
	if (IS_ERR_OR_NULL(banner_addr))
		return;

	banner_len = strlen(banner_addr);
	banner_len = banner_len > SZ_512 ? SZ_512 : banner_len;
	if (header)
		memcpy(header + DSS_OFFSET_LINUX_BANNER, banner_addr, banner_len);
}

static void dbg_snapshot_set_sjtag_status(void)
{
#ifdef SMC_CMD_GET_SJTAG_STATUS
	struct arm_smccc_res res;

	arm_smccc_smc(SMC_CMD_GET_SJTAG_STATUS, 0x3, 0, 0, 0, 0, 0, 0, &res);
	dss_desc.sjtag_status = res.a0;
	dev_info(dss_desc.dev, "SJTAG is %sabled\n",
			dss_desc.sjtag_status ? "en" : "dis");
#endif
}

int dbg_snapshot_get_sjtag_status(void)
{
	return dss_desc.sjtag_status;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_sjtag_status);

void dbg_snapshot_scratch_reg(unsigned int val)
{
	dbg_snapshot_set_val_offset(val, DSS_OFFSET_SCRATCH);
}

void dbg_snapshot_scratch_clear(void)
{
	dbg_snapshot_scratch_reg(DSS_SIGN_RESET);
}
EXPORT_SYMBOL_GPL(dbg_snapshot_scratch_clear);

bool dbg_snapshot_is_scratch(void)
{
	return dbg_snapshot_get_val_offset(DSS_OFFSET_SCRATCH) == DSS_SIGN_SCRATCH;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_is_scratch);

void dbg_snapshot_set_debug_test_buffer_addr(u64 paddr, unsigned int cpu)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	if (header)
		__raw_writeq(paddr, header + DSS_OFFSET_DEBUG_TEST_BUFFER(cpu));
}
EXPORT_SYMBOL_GPL(dbg_snapshot_set_debug_test_buffer_addr);

unsigned int dbg_snapshot_get_debug_test_buffer_addr(unsigned int cpu)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	return header ? __raw_readq(header + DSS_OFFSET_DEBUG_TEST_BUFFER(cpu)) : 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_debug_test_buffer_addr);

int dbg_snapshot_get_dpm_none_dump_mode(void)
{
	unsigned int val;
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	if (header) {
		val = __raw_readl(header + DSS_OFFSET_NONE_DPM_DUMP_MODE);
		if ((val & GENMASK(31, 16)) == DSS_SIGN_MAGIC)
			return (val & GENMASK(15, 0));
	}

	return -1;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_dpm_none_dump_mode);

void dbg_snapshot_set_dpm_none_dump_mode(unsigned int mode)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	if (header) {
		if (mode)
			mode |= DSS_SIGN_MAGIC;
		else
			mode = 0;

		__raw_writel(mode, header + DSS_OFFSET_NONE_DPM_DUMP_MODE);
	}
}
EXPORT_SYMBOL_GPL(dbg_snapshot_set_dpm_none_dump_mode);

struct dbg_snapshot_item *dbg_snapshot_get_item(const char *name)
{
	unsigned long i;

	for (i = 0; i < ARRAY_SIZE(dss_items); i++) {
		if (dss_items[i].name &&
				!strncmp(dss_items[i].name, name,
					DSS_ITEM_NAME_LEN - 1))
			return &dss_items[i];
	}

	return NULL;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_item);

unsigned int dbg_snapshot_get_item_size(const char *name)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item(name);

	return item ? item->entry.size : 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_item_size);

unsigned long dbg_snapshot_get_item_vaddr(const char *name)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item(name);

	return item ? item->entry.vaddr : 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_item_vaddr);

unsigned int dbg_snapshot_get_item_paddr(const char *name)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item(name);

	return item && item->entry.enabled ? item->entry.paddr : 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_item_paddr);

int dbg_snapshot_get_item_enable(const char *name)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item(name);

	return item && item->entry.enabled ? item->entry.enabled : 0;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_item_enable);

void dbg_snapshot_set_item_enable(const char *name, int en)
{
	struct dbg_snapshot_item *item = NULL;

	if (!name || !dss_dpm.enabled_debug || dss_dpm.dump_mode == NONE_DUMP)
		return;

	/* This is default for debug-mode */
	item = dbg_snapshot_get_item(name);
	if (item) {
		item->entry.enabled = en;
		pr_info("item - %s is %sabled\n", name, en ? "en" : "dis");
	}
}
EXPORT_SYMBOL_GPL(dbg_snapshot_set_item_enable);

static void dbg_snapshot_set_enable(int en)
{
	dss_base->enabled = en;
	dev_info(dss_desc.dev, "%sabled\n", en ? "en" : "dis");
}

int dbg_snapshot_get_enable(void)
{
	return dss_base && dss_base->enabled;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_enable);

void *dbg_snapshot_get_item_by_index(int index)
{
	if (index < 0 || index > ARRAY_SIZE(dss_items))
		return NULL;

	return &dss_items[index];
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_item_by_index);

int dbg_snapshot_get_num_items(void)
{
	return ARRAY_SIZE(dss_items);
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_num_items);

static inline int dbg_snapshot_check_eob(struct dbg_snapshot_item *item,
						size_t size)
{
	size_t max = (size_t)(item->head_ptr + item->entry.size - 0x400);
	size_t cur = (size_t)(item->curr_ptr + size);

	return (unlikely(cur >= max)) ? -1 : 0;
}

void dbg_snapshot_hook_logger(const char *buf, size_t size,
				    unsigned int id)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item_by_index(id);
	size_t last_buf;
	u32 offset = 0;

	if (!item)
		return;

	if (!dbg_snapshot_get_enable() || !item->entry.enabled)
		return;

	if (id == DSS_ITEM_KERNEL_ID) {
		offset = DSS_OFFSET_LAST_LOGBUF;
	} else if (id == DSS_ITEM_PLATFORM_ID) {
		offset = DSS_OFFSET_LAST_PLATFORM_LOGBUF;
	} else if (id == DSS_ITEM_FIRST_ID) {
		if (dbg_snapshot_check_eob(item, size)) {
			item->entry.enabled = false;
			return;
		}
	}
	if (dbg_snapshot_check_eob(item, size)) {
		if (id == DSS_ITEM_KERNEL_ID)
			dbg_snapshot_set_val_offset(1, DSS_OFFSET_CHECK_EOB);
		item->curr_ptr = item->head_ptr;
	}

	/*  save the address of last_buf to physical address */
	last_buf = (size_t)item->curr_ptr + size;
	if (offset)
		dbg_snapshot_set_val_offset(item->entry.paddr +
					(last_buf - item->entry.vaddr),
					offset);
	memcpy(item->curr_ptr, buf, size);
	item->curr_ptr += size;
}

void dbg_snapshot_output(void)
{
	unsigned long i, size = 0;

	dev_info(dss_desc.dev, "debug-snapshot physical / virtual memory layout:\n");
	for (i = 0; i < ARRAY_SIZE(dss_items); i++) {
		if (!dss_items[i].entry.enabled)
			continue;
		pr_info("%-16s: phys:0x%zx / virt:0x%zx / size:0x%zx / en:%d\n",
				dss_items[i].name,
				dss_items[i].entry.paddr,
				dss_items[i].entry.vaddr,
				dss_items[i].entry.size,
				dss_items[i].entry.enabled);
		size += dss_items[i].entry.size;
	}

	dev_info(dss_desc.dev, "total_item_size: %ldKB, dbg_snapshot_log struct size: %dKB\n",
			size / SZ_1K, (int)sizeof(struct dbg_snapshot_log) / SZ_1K);
}

static void dbg_snapshot_init_desc(struct device *dev)
{
	u32 val;

	/* initialize dss_desc */
	memset((void *)&dss_desc, 0, sizeof(struct dbg_snapshot_desc));
	raw_spin_lock_init(&dss_desc.ctrl_lock);
	dss_desc.dev = dev;
	dbg_snapshot_set_sjtag_status();

	if (!of_property_read_u32(dev->of_node, "panic_to_wdt", &val))
		dss_desc.panic_to_wdt = val;

	if (!of_property_read_u32(dev->of_node, "last_kmsg", &val))
		dss_desc.last_kmsg = val;

	if (!of_property_read_u32(dev->of_node, "hold-key", &val))
		dss_desc.hold_key = val;

	if (!of_property_read_u32(dev->of_node, "trigger-key", &val))
		dss_desc.trigger_key = val;

	if (!of_property_read_u32(dev->of_node, "mmu_regs_offset", &val))
		dss_desc.mmu_regs_offset = val;
	else
		dss_desc.mmu_regs_offset = DSS_OFFSET_MMU_REGS_DEFAULT;

	if (!of_property_read_u32(dev->of_node, "core_regs_offset", &val))
		dss_desc.core_regs_offset = val;
	else
		dss_desc.core_regs_offset = DSS_OFFSET_CORE_REGS_DEFAULT;
}

static void dbg_snapshot_fixmap(void)
{
	size_t last_buf;
	size_t vaddr, paddr, size, offset;
	unsigned long i;

	for (i = 0; i < ARRAY_SIZE(dss_items); i++) {
		if (!dss_items[i].entry.enabled)
			continue;

		/*  assign dss_item information */
		paddr = dss_items[i].entry.paddr;
		vaddr = dss_items[i].entry.vaddr;
		size = dss_items[i].entry.size;

		if (i == DSS_ITEM_HEADER_ID) {
			/*  initialize kernel event to 0 except only header */
			memset((size_t *)(vaddr + DSS_KEEP_HEADER_SZ), 0,
					size - DSS_KEEP_HEADER_SZ);

			dss_bl = dbg_snapshot_get_header_vaddr() + DSS_OFFSET_ITEM_INFO;
			memset(dss_bl, 0, sizeof(struct dbg_snapshot_bl));
			dss_bl->magic = DSS_BL_MAGIC;
			dss_bl->item_count = 0;
		} else if (i == DSS_ITEM_KERNEL_ID || i == DSS_ITEM_PLATFORM_ID) {
			/*  load last_buf address value(phy) by virt address */
			if (i == DSS_ITEM_KERNEL_ID)
				offset = DSS_OFFSET_LAST_LOGBUF;
			else if (i == DSS_ITEM_PLATFORM_ID)
				offset = DSS_OFFSET_LAST_PLATFORM_LOGBUF;

			last_buf = (size_t)dbg_snapshot_get_val_offset(offset);
			/*  check physical address offset of logbuf */
			if (last_buf >= paddr && (last_buf <= paddr + size)) {
				/*  assumed valid address, conversion to virt */
				dss_items[i].curr_ptr = (unsigned char *)(vaddr +
						(last_buf - paddr));
			} else {
				/*  invalid address, set to first line */
				dss_items[i].curr_ptr = (unsigned char *)vaddr;
				/*  initialize logbuf to 0 */
				memset((void *)vaddr, 0, size);
			}
		} else {
			/*  initialized log to 0 if persist == false */
			if (!dss_items[i].persist)
				memset((void *)vaddr, 0, size);
		}

		dbg_snapshot_add_bl_item_info(dss_items[i].name, paddr, size);
	}

	dss_log = (struct dbg_snapshot_log *)(dss_items[DSS_ITEM_KEVENTS_ID].entry.vaddr);
	/*  set fake translation to virtual address to debug trace */
	dss_info.info_event = dss_log;
	ess_info = &dss_info;

	/* output the information of debug-snapshot */
	dbg_snapshot_output();
}

static void dbg_snapshot_boot_cnt(void)
{
	struct dbg_snapshot_item *item = &dss_items[DSS_ITEM_PLATFORM_ID];
	unsigned int reg;

	reg = dbg_snapshot_get_val_offset(DSS_OFFSET_KERNEL_BOOT_CNT_MAGIC);
	if (reg != DSS_BOOT_CNT_MAGIC) {
		dbg_snapshot_set_val_offset(DSS_BOOT_CNT_MAGIC,
				DSS_OFFSET_KERNEL_BOOT_CNT_MAGIC);
		reg = 0;
	} else {
		reg = dbg_snapshot_get_val_offset(DSS_OFFSET_KERNEL_BOOT_CNT);
	}
	dbg_snapshot_set_val_offset(++reg, DSS_OFFSET_KERNEL_BOOT_CNT);

	dev_info(dss_desc.dev, "Kernel Booting SEQ #%u\n", reg);
	if (dbg_snapshot_get_enable() && item->entry.enabled) {
		char _buf[SZ_128];

		if (snprintf(_buf, SZ_128 - 1, "\nBooting SEQ #%u\n", reg) < 0) {
			dev_err(dss_desc.dev, "snprintf is failed");
			return;
		}
		dbg_snapshot_hook_logger((const char *)_buf, strlen(_buf),
				DSS_ITEM_PLATFORM_ID);
	}
}

static struct reserved_mem *_dbg_snapshot_rmem_available(struct device *dev, int i)
{
	struct device_node *rmem_np;
	struct reserved_mem *rmem;

	rmem_np = of_parse_phandle(dev->of_node, "memory-region", i);
	if (!rmem_np) {
		dev_err(dev, "no such memory-region of index %d\n", i);
		return NULL;
	}
	if (!of_device_is_available(rmem_np)) {
		dev_err(dev, "%s item is disabled, Skip alloc rmem\n",
				rmem_np->name);
		return NULL;
	}
	rmem = of_reserved_mem_lookup(rmem_np);
	if (!rmem) {
		dev_err(dev, "no such reserved mem of node name %s\n",
				rmem_np->name);
		return NULL;
	}
	if (!rmem->base || !rmem->size) {
		dev_err(dev, "%s item wrong base(0x%llx) or size(0x%llx)\n",
				rmem->name, rmem->base, rmem->size);
		return NULL;
	}

	return rmem;
}

static void *_dbg_snapshot_rmem_set_vmap(struct reserved_mem *rmem)
{
	pgprot_t prot = __pgprot(PROT_NORMAL_NC);
	int i, page_size;
	struct page *page, **pages;
	void *vaddr;

	page_size = rmem->size / PAGE_SIZE;
	pages = kzalloc(sizeof(struct page *) * page_size, GFP_KERNEL);
	page = phys_to_page(rmem->base);

	for (i = 0; i < page_size; i++)
		pages[i] = page++;

	vaddr = vmap(pages, page_size, VM_NO_GUARD | VM_MAP, prot);
	kfree(pages);

	return vaddr;
}

static int dbg_snapshot_rmem_setup(struct device *dev)
{
	struct reserved_mem *rmem;
	struct dbg_snapshot_item *item;
	int i, mem_count = 0;
	void *vaddr;

	mem_count = of_count_phandle_with_args(dev->of_node, "memory-region", NULL);
	if (mem_count <= 0) {
		dev_err(dev, "no such memory-region\n");
		return -ENOMEM;
	}

	for (i = 0; i < mem_count; i++) {
		rmem = _dbg_snapshot_rmem_available(dev, i);
		if (!rmem)
			continue;

		vaddr = _dbg_snapshot_rmem_set_vmap(rmem);
		if (!vaddr) {
			dev_err(dev, "%s: paddr:%llx page_size:%llx failed to vmap\n",
					rmem->name, rmem->base, rmem->size / PAGE_SIZE);
			continue;
		}

		item = dbg_snapshot_get_item(rmem->name);
		if (!item) {
			dev_err(dev, "no such item %s\n", rmem->name);
			continue;
		}

		item->entry.paddr = rmem->base;
		item->entry.size = rmem->size;
		item->entry.vaddr = (size_t)vaddr;
		item->head_ptr = item->curr_ptr = (unsigned char *)vaddr;
		dbg_snapshot_set_item_enable(rmem->name, true);

		if (item == dbg_snapshot_get_item_by_index(DSS_ITEM_HEADER_ID)) {
			dss_base = (struct dbg_snapshot_base *)vaddr;
			dss_base->magic = DSS_MAGIC;
			dss_base->version = DSS_VERSION;
			dss_base->vaddr = (size_t)vaddr;
			dss_base->paddr = rmem->base;
			ess_base = dss_base;
		}

		if (dss_base)
			dss_base->size += rmem->size;
	}

	return 0;
}

static ssize_t dbg_snapshot_last_kmsg_read(struct file *file, char __user *buf,
					 size_t count, loff_t *ppos)
{
	int ret = 0;
	unsigned int size = last_kmsg_size;

	if (!last_kmsg || !size)
		goto out;

	if (*ppos > size)
		goto out;

	if (*ppos + count > size)
		count = size - *ppos;

	ret = copy_to_user(buf, last_kmsg + (*ppos), count);
	if (ret)
		return -EFAULT;

	*ppos += count;
	ret = count;
out:
	return ret;
}

static ssize_t dbg_snapshot_first_kmsg_read(struct file *file, char __user *buf,
					 size_t count, loff_t *ppos)
{
	int ret = 0;
	long first_vaddr = dbg_snapshot_get_item_vaddr(DSS_ITEM_FIRST);
	long log_size = dbg_snapshot_get_item_size(DSS_ITEM_FIRST);

	if (!first_vaddr || !log_size)
		goto out;

	if (*ppos > log_size)
		goto out;

	if (*ppos + count > log_size)
		count = log_size - *ppos;

	ret = copy_to_user(buf, (char *)first_vaddr + (*ppos), count);
	if (ret) {
		dev_info(dss_desc.dev, "kmsg: read failed\n");
		return -EFAULT;
	}

	*ppos += count;
	ret = count;
out:
	return ret;
}

static int dbg_snapshot_dss_kmsg_open(struct inode *inode, struct file *file)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item_by_index(DSS_ITEM_KERNEL_ID);
	size_t offset;
	bool eob = dbg_snapshot_get_val_offset(DSS_OFFSET_CHECK_EOB);

	if (!item) {
		dev_info(dss_desc.dev, "kmsg: debug-snapshot item get failed\n");
		return -EFAULT;
	}

	mutex_lock(&dss_kmsg_mutex);

	dss_kmsg_size = item->entry.size;
	dss_kmsg = vzalloc(dss_kmsg_size);

	if (!dss_kmsg) {
		mutex_unlock(&dss_kmsg_mutex);
		dev_info(dss_desc.dev, "kmsg: debug-snapshot kmsg alloc failed\n");
		return -ENOMEM;
	}

	offset = dbg_snapshot_get_val_offset(DSS_OFFSET_LAST_LOGBUF) - item->entry.paddr;

	if (eob) {
		memcpy(dss_kmsg, (void *)item->entry.vaddr + offset, dss_kmsg_size - offset);
		memcpy(dss_kmsg + dss_kmsg_size - offset, (void *)item->entry.vaddr, offset);
	} else {
		memcpy(dss_kmsg, (void *)item->entry.vaddr, offset);
		dss_kmsg_size = offset;
	}

	mutex_unlock(&dss_kmsg_mutex);
	return 0;
}

static ssize_t dbg_snapshot_dss_kmsg_read(struct file *file, char __user *buf,
					 size_t count, loff_t *ppos)
{
	int ret = 0;
	long log_size = dss_kmsg_size;

	if (!log_size)
		goto out;

	if (*ppos > log_size)
		goto out;

	if (*ppos + count > log_size)
		count = log_size - *ppos;

	mutex_lock(&dss_kmsg_mutex);
	if (!dss_kmsg_size || !dss_kmsg) {
		mutex_unlock(&dss_kmsg_mutex);
		return -EFAULT;
	}
	ret = copy_to_user(buf, dss_kmsg + (*ppos), count);

	if (ret) {
		mutex_unlock(&dss_kmsg_mutex);
		return -EFAULT;
	}

	mutex_unlock(&dss_kmsg_mutex);

	*ppos += count;
	ret = count;
out:
	return ret;
}


static int dbg_snapshot_dss_kmsg_release(struct inode *inode, struct file *file)
{
	mutex_lock(&dss_kmsg_mutex);
	vfree(dss_kmsg);
	dss_kmsg = NULL;
	dss_kmsg_size = 0;
	mutex_unlock(&dss_kmsg_mutex);
	return 0;
}

static const struct proc_ops proc_last_kmsg_op = {
	.proc_lseek = default_llseek,
	.proc_read = dbg_snapshot_last_kmsg_read,
};

static const struct proc_ops proc_first_kmsg_op = {
	.proc_lseek = default_llseek,
	.proc_read = dbg_snapshot_first_kmsg_read,
};

static const struct proc_ops proc_dss_kmsg_op = {
	.proc_lseek = default_llseek,
	.proc_open = dbg_snapshot_dss_kmsg_open,
	.proc_read = dbg_snapshot_dss_kmsg_read,
	.proc_release = dbg_snapshot_dss_kmsg_release,
};

static void dbg_snapshot_init_proc(void)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item(DSS_ITEM_KERNEL);
	size_t start, max, cur;
	bool eob;

	if (!item)
		return;

	start = (size_t)item->head_ptr;
	max = (size_t)(item->head_ptr + item->entry.size);
	cur = (size_t)item->curr_ptr;
	eob = dbg_snapshot_get_val_offset(DSS_OFFSET_CHECK_EOB);

	proc_create("dss_kmsg", 0, NULL, &proc_dss_kmsg_op);

	last_kmsg_size = eob ? item->entry.size : cur - start;
	if (dbg_snapshot_get_item_enable(DSS_ITEM_FIRST))
		proc_create("first_kmsg", 0, NULL, &proc_first_kmsg_op);

	if (!item->entry.enabled || !dss_desc.last_kmsg)
		return;

	last_kmsg = (char *)vzalloc(last_kmsg_size);
	if (!last_kmsg)
		return;

	if (eob) {
		memcpy(last_kmsg, item->curr_ptr, max - cur);
		memcpy(last_kmsg + max - cur, item->head_ptr, cur - start);
	}  else {
		memcpy(last_kmsg, item->head_ptr, cur - start);
	}
	proc_create("last_kmsg", 0, NULL, &proc_last_kmsg_op);
}

static ssize_t dss_panic_to_wdt_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%sabled\n",
			dss_desc.panic_to_wdt ? "En" : "Dis");
}

static ssize_t dss_panic_to_wdt_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	unsigned long mode;

	if (!kstrtoul(buf, 10, &mode))
		dss_desc.panic_to_wdt = !!mode;

	return count;
}

static ssize_t dss_dpm_none_dump_mode_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
			"currnet DPM dump_mode: %x, DPM none dump_mode: %x\n",
			dss_dpm.dump_mode, dss_dpm.dump_mode_none);
}

static ssize_t dss_dpm_none_dump_mode_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	unsigned int mode;

	if (kstrtoint(buf, 10, &mode))
		return count;

	dbg_snapshot_set_dpm_none_dump_mode(mode);
	if (mode && dss_dpm.version) {
		dss_dpm.dump_mode_none = 1;
		dbg_snapshot_scratch_clear();
	} else {
		dss_dpm.dump_mode_none = 0;
		dbg_snapshot_scratch_reg(DSS_SIGN_SCRATCH);
	}

	dev_info(dss_desc.dev, "DPM: success to switch %sNONE_DUMP mode\n",
			dss_dpm.dump_mode_none ? "" : "NOT ");
	return count;
}

static ssize_t dss_logging_item_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int i;
	ssize_t n = 0;

	if (!dbg_snapshot_get_enable())
		goto out;

	n += scnprintf(buf + n, SZ_128, "item_count : %d\n", dss_bl->item_count);

	for (i = 0; i < dss_bl->item_count; i++) {
		n += scnprintf(buf + n, SZ_128, "%s paddr : 0x%lX / size : 0x%lX\n",
				dss_bl->item[i].name,
				dss_bl->item[i].paddr,
				dss_bl->item[i].size);
	}
out:
	return n;
}

DEVICE_ATTR_RW(dss_panic_to_wdt);
DEVICE_ATTR_RW(dss_dpm_none_dump_mode);
DEVICE_ATTR_RO(dss_logging_item);

static struct attribute *dss_sysfs_attrs[] = {
	&dev_attr_dss_dpm_none_dump_mode.attr,
	&dev_attr_dss_logging_item.attr,
	&dev_attr_dss_panic_to_wdt.attr,
	NULL,
};

static struct attribute_group dss_sysfs_group = {
	.attrs = dss_sysfs_attrs,
};

static const struct attribute_group *dss_sysfs_groups[] = {
	&dss_sysfs_group,
	NULL,
};

#define PRINTK_LOG_LINE_MAX	(1024)

void dbg_snapshot_kmsg_dump_initial_log(void)
{
	struct kmsg_dump_iter dss_kmsg_dumper;
	char text_buf[PRINTK_LOG_LINE_MAX];
	size_t len;

	kmsg_dump_rewind(&dss_kmsg_dumper);

	while (kmsg_dump_get_line(&dss_kmsg_dumper, true, text_buf,
				PRINTK_LOG_LINE_MAX, &len)) {

		dbg_snapshot_hook_logger(text_buf, len, DSS_ITEM_KERNEL_ID);
		dbg_snapshot_hook_logger(text_buf, len, DSS_ITEM_FIRST_ID);
	}
}

#ifdef CONFIG_DEBUG_SNAPSHOT_USE_ANDROID_VH_LOGBUF
enum log_flags {
	LOG_NEWLINE	= 2,	/* text ended with a newline */
	LOG_CONT	= 8,	/* text is a fragment of a continuation line */
};
#define INFO_BUF_SIZE	(255)

static size_t print_syslog(unsigned int level, char *buf)
{
	return sprintf(buf, "<%u>", level);
}

size_t dss_print_time(u64 ts, char *buf)
{
	unsigned long rem_nsec = do_div(ts, NSEC_PER_SEC);

	return sprintf(buf, "[%5lu.%06lu]",
		       (unsigned long)ts, rem_nsec / 1000);
}

static const struct of_device_id arch_timer_of_match[] = {
	{ .compatible   = "arm,armv7-timer",	},
	{ .compatible   = "arm,armv8-timer",	},
	{},
};
static u32 arch_timer_rate;

inline u64 dss_arch_timer_get_ns(void)
{
	u32 rate = arch_timer_rate / 1000;
	u64 ret = arch_timer_read_counter() * 1000 / rate;
	if (rate)
		return ret * 1000;
	else
		return 0;
}

static size_t print_process(char *buf)
{
	return sprintf(buf, "%c[%1d:%15s:%5d]",
			in_interrupt() ? 'I' : ' ',
			smp_processor_id(),
			current->comm,
			task_pid_nr(current));
}

#ifdef CONFIG_PRINTK_CALLER
static size_t print_caller(u32 id, char *buf)
{
	char caller[12];

	snprintf(caller, sizeof(caller), "%c%u",
			id & 0x80000000 ? 'C' : 'T', id & ~0x80000000);
	return sprintf(buf, "[%6s]", caller);
}
#else
#define print_caller(id, buf) 0
#endif

static void __dbg_snapshot_logbuf(unsigned int id,
		struct printk_ringbuffer *rb, struct printk_record *r)
{
	struct printk_info *info;
	char info_buf[INFO_BUF_SIZE];
	int info_buf_offset = 0;

	if (!rb || !r || !r->info)
		return;

	info = r->info;

	info_buf_offset += print_syslog((info->facility << 3) | info->level,
			info_buf + info_buf_offset);
	info_buf_offset += dss_print_time(dss_arch_timer_get_ns(), info_buf + info_buf_offset);
	info_buf_offset += dss_print_time(info->ts_nsec, info_buf + info_buf_offset);
	info_buf_offset += print_process(info_buf + info_buf_offset);
	info_buf_offset +=
		print_caller(info->caller_id, info_buf + info_buf_offset);

	dbg_snapshot_hook_logger(info_buf, info_buf_offset, id);

	dbg_snapshot_hook_logger(r->text_buf, info->text_len, id);
	if (info->flags & LOG_NEWLINE)
		dbg_snapshot_hook_logger("\n", 1, id);
}

static void dbg_snapshot_logbuf(void *ignore,
		struct printk_ringbuffer *rb, struct printk_record *r)
{
	__dbg_snapshot_logbuf(DSS_ITEM_KERNEL_ID, rb, r);
	__dbg_snapshot_logbuf(DSS_ITEM_FIRST_ID, rb, r);
}

static void __dbg_snapshot_logbuf_pr_cont(unsigned int id,
	struct printk_record *r, size_t text_len)
{
	char *text;

	if (!r || !r->info)
		return;

	text = r->text_buf + r->info->text_len - text_len;

	dbg_snapshot_hook_logger(text, text_len, id);

	if (r->info->flags & LOG_NEWLINE)
		dbg_snapshot_hook_logger("\n", 1, id);
}

static void dbg_snapshot_logbuf_pr_cont(void *ignore,
		struct printk_record *r, size_t text_len)
{
	__dbg_snapshot_logbuf_pr_cont(DSS_ITEM_KERNEL_ID, r, text_len);
	__dbg_snapshot_logbuf_pr_cont(DSS_ITEM_FIRST_ID, r, text_len);
}

#else
static void dbg_snapshot_console_write(struct console *con, const char *s,
				       unsigned c)
{
	dbg_snapshot_hook_logger(s, c, DSS_ITEM_KERNEL_ID);
	dbg_snapshot_hook_logger(s, c, DSS_ITEM_FIRST_ID);
}

static struct console dss_console = {
	.name = "dss",
	.write = dbg_snapshot_console_write,
	.flags = CON_PRINTBUFFER | CON_ENABLED | CON_ANYTIME,
	.index = -1,
};
#endif

static int dbg_snapshot_probe(struct platform_device *pdev)
{
	if (dbg_snapshot_dt_scan_dpm()) {
		pr_err("%s: no such dpm node\n", __func__);
		return -ENODEV;
	}

	dbg_snapshot_init_desc(&pdev->dev);
	if (dbg_snapshot_rmem_setup(&pdev->dev)) {
		dev_err(&pdev->dev, "%s failed\n", __func__);
		return -ENODEV;
	}
	dbg_snapshot_fixmap();

	dbg_snapshot_init_dpm();
	dbg_snapshot_init_utils();
	dbg_snapshot_init_log();
	dbg_snapshot_init_proc();

	dbg_snapshot_set_val_offset(DSS_ORDERING_VCPU, DSS_OFFSET_CPU_ORDERING);
	dbg_snapshot_set_val_offset(cpumask_weight(cpu_possible_mask), DSS_OFFSET_CPU_ORDERING + 4);
	dbg_snapshot_set_val_offset(*cpumask_bits(cpu_possible_mask), DSS_OFFSET_CPU_ORDERING + 8);
	dbg_snapshot_set_enable(true);

	if (dbg_snapshot_get_item_enable(DSS_ITEM_KERNEL)) {
#ifdef CONFIG_DEBUG_SNAPSHOT_USE_ANDROID_VH_LOGBUF
		struct device_node *np =
			of_find_matching_node(NULL, arch_timer_of_match);
#endif
		dbg_snapshot_kmsg_dump_initial_log();

#ifdef CONFIG_DEBUG_SNAPSHOT_USE_ANDROID_VH_LOGBUF
		if (of_property_read_u32(np, "clock-frequency", &arch_timer_rate))
			arch_timer_rate = 26000000;

		dev_info(dss_desc.dev, "arch_timer_rate: %d\n", arch_timer_rate);
		register_trace_android_vh_logbuf(dbg_snapshot_logbuf, NULL);
		register_trace_android_vh_logbuf_pr_cont(dbg_snapshot_logbuf_pr_cont, NULL);
#else
		register_console(&dss_console);
		console_suspend_enabled = false;
#endif
	}
	if (dbg_snapshot_get_item_enable(DSS_ITEM_PLATFORM)) {
		int ret;

		dbg_snapshot_init_pmsg();
		ret = dss_pmsg_tty_init();
		if (ret < 0)
			dev_warn(dss_desc.dev,
				"Failed to initialize pmsg-tty (%d)\n", ret);
	}

	dbg_snapshot_boot_cnt();
	dbg_snapshot_set_linux_banner();

	if (sysfs_create_groups(&pdev->dev.kobj, dss_sysfs_groups))
		dev_err(dss_desc.dev, "fail to register debug-snapshot sysfs\n");

	dev_info(&pdev->dev, "%s successful.\n", __func__);
	return 0;
}

static const struct of_device_id dss_of_match[] = {
	{ .compatible	= "samsung,debug-snapshot" },
	{},
};
MODULE_DEVICE_TABLE(of, dss_of_match);

static struct platform_driver dbg_snapshot_driver = {
	.probe = dbg_snapshot_probe,
	.driver  = {
		.name  = "debug-snapshot",
		.of_match_table = of_match_ptr(dss_of_match),
	},
};

static __init int dbg_snapshot_init(void)
{
	return platform_driver_register(&dbg_snapshot_driver);
}
core_initcall(dbg_snapshot_init);

static void __exit dbg_snapshot_exit(void)
{
	if (last_kmsg)
		vfree(last_kmsg);
	platform_driver_unregister(&dbg_snapshot_driver);
}
module_exit(dbg_snapshot_exit);

MODULE_DESCRIPTION("Debug-Snapshot");
MODULE_LICENSE("GPL v2");
