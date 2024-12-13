// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * EXYNOS - Bootloader log extractor
 */

#include <linux/err.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_reserved_mem.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/debugfs.h>
#include <linux/mm.h>
#include <linux/io.h>

#define BOOTLOG_MAGIC			0xEC00BABA

/* Boot log buffer information */
#define BL2LOG_BASE			0xE5C00000
#define BL2LOG_MAGIC			0xCAFEBABE

#define BOOTLOG_BASE			0xE5C01000
#define BOOTLOG_SIZE			0xff000			/* 1020 KB */
#define BOOTLOG_AREA_SIZE		0x100000		/* 1MB */

static struct dentry *bootlog_debugfs_dir;

struct dram_bootlog_data {
	void *virt_addr;
	unsigned long phys_addr;
	unsigned long size;
};

static struct dram_bootlog_data dram_log_data;
static u64 pLineCore[10];
static u64 BOOTLOG_SECTOR_SIZE;
static u32 nr_cpus;

static void *exynos_dram_bootlog_request_region(unsigned long addr,
		unsigned int size)
{
	int i;
	unsigned int num_pages = (size >> PAGE_SHIFT);
	pgprot_t prot = pgprot_writecombine(PAGE_KERNEL);
	struct page **pages = NULL;
	void *v_addr = NULL;

	if (!addr)
		return NULL;

	pages = kmalloc_array(num_pages, sizeof(struct page *), GFP_ATOMIC);
	if (!pages)
		return NULL;

	for (i = 0; i < num_pages; i++) {
		pages[i] = phys_to_page(addr);
		addr += PAGE_SIZE;
	}

	v_addr = vmap(pages, num_pages, VM_MAP, prot);
	kfree(pages);

	return v_addr;
}

static int show_dram_log_just_line(char *log, struct seq_file *seq)
{
	int index = 0;
	char buffer[1024] = {0, };

	index = 0;
	while (index < sizeof(buffer) &&
			log != 0 && *log != 0 && *log != '\0' && *log != '\n') {
		buffer[index] = *log++;
		index++;
	}

	if (buffer[0] != '\n' && buffer[0] != 0)
		seq_printf(seq, "%s\n", buffer);
	else
		index = 1;

	return index;
}

static void show_dram_log_lines(char *log, struct seq_file *seq)
{
	int index = 0;

	while (log != 0 && *log != 0 && *log != '\0') {
		index = show_dram_log_just_line(log, seq);
		log += index;
	}

	return;
}

static char *find_term(char *in, const char *word)
{
	char *tmp = in;

	while (*tmp != '\0') {
		if (!strncmp(word, tmp, strlen(word))) {
			return tmp;
		}
		tmp++;
	}

	return 0;
}

static u32 get_integer_by_char(char *in)
{
	u32 ret = 0;
	char *tmp = in;

	while (*tmp != '\0' && *tmp != ']') {
		if (*tmp == ' ' || *tmp == '.') {
			tmp++;
			continue;
		}

		if (*tmp > '9' || *tmp < '0') {
			ret = -1;
			break;
		}

		ret = ret * 10 + (*tmp - '0');
		tmp++;
	}

	return ret;
}

static u32 get_arch_timer_from_core_log_line(char *line)
{
	char *archtime;

	if (line == NULL || *line == '\0' || *line == '\n')
		return 0;

	archtime = find_term(line, "][ARCH:");
	if (archtime != 0)
		return get_integer_by_char(archtime + strlen("][ARCH:"));

	return 0;
}

static void show_curr_log_line(unsigned int core, unsigned int minimal,
	unsigned int next_minimal, struct seq_file *seq)
{
	char *line_start;
	int next_offset = 0;
	u32 archtime_value;
	void *timebase = (dram_log_data.virt_addr + BOOTLOG_BASE - BL2LOG_BASE)
						+ (core * BOOTLOG_SECTOR_SIZE);
	u64 next_log_Offset = readq(timebase);

	line_start = (char *) pLineCore[core];
	while (next_log_Offset >= (u64)((void *)line_start - timebase)) {
		archtime_value = get_arch_timer_from_core_log_line(line_start);

		if (archtime_value < next_minimal) {
			next_offset = show_dram_log_just_line(line_start, seq);
			line_start += next_offset;
		} else {
			break;
		}
	}

	pLineCore[core] = (u64) line_start;
}

static u32 get_minimal_time_value(unsigned int *minimal, unsigned int *next_minimal)
{
	u32 ret = 99999999;
	int index = 0;
	u32 archtime_value;
	int core = -1;
	u64 next_log_Offset = 0;
	u64 dram_magic_code = 0;
	void *timebase = 0;

	for (index = 0 ; index < nr_cpus ; index++) {
		timebase = (dram_log_data.virt_addr + BOOTLOG_BASE - BL2LOG_BASE)
					+ (index * BOOTLOG_SECTOR_SIZE);
		next_log_Offset = readq(timebase);
		dram_magic_code = readq(timebase + 0x8);

		if (dram_magic_code != BOOTLOG_MAGIC || next_log_Offset == 0x0)
			continue;

		if ((pLineCore[index] - (u64)timebase) >= next_log_Offset)
			continue;

		archtime_value = get_arch_timer_from_core_log_line((char *) pLineCore[index]);

		if (archtime_value > 0 && ret > archtime_value) {
			ret = archtime_value;
			core = index;
		}
	}
	*minimal = ret;

	ret = 99999999;
	for (index = 0 ; index < nr_cpus ; index++) {
		timebase = (dram_log_data.virt_addr + BOOTLOG_BASE - BL2LOG_BASE)
					+ (index * BOOTLOG_SECTOR_SIZE);
		next_log_Offset = readq(timebase);
		dram_magic_code = readq(timebase + 0x8);

		if (dram_magic_code != BOOTLOG_MAGIC || next_log_Offset == 0x0)
			continue;

		if (index == core)
			continue;

		if ((pLineCore[index] - (u64)timebase) >= next_log_Offset)
			continue;

		archtime_value = get_arch_timer_from_core_log_line((char *) pLineCore[index]);
		if (archtime_value > 0 && ret > archtime_value)
			ret = archtime_value;
	}
	*next_minimal = ret;

	return core;
}

static u32 condition_loop_end(void)
{
	int count = 0, core = 0;
	void *timebase;
	u64 next_log_Offset;
	u64 dram_magic_code = 0;

	for (core = 0 ; core < nr_cpus ; core++) {
		timebase = (dram_log_data.virt_addr + BOOTLOG_BASE - BL2LOG_BASE)
					+ (core * BOOTLOG_SECTOR_SIZE);
		dram_magic_code = readq(timebase + 0x8);
		if (dram_magic_code != BOOTLOG_MAGIC)
			continue;

		next_log_Offset = readq(timebase);

		if (next_log_Offset > 0x10
				&& (pLineCore[core] - (u64)timebase) < next_log_Offset) {
			count++;
		}
	}

	if (count < 2)
		return 0;

	return 1;
}

static u32 get_only_one_left_log_core(void)
{
	int core = 0;
	void *timebase;
	u64 next_log_Offset;
	u64 dram_magic_code = 0;

	for (core = 0 ; core < nr_cpus ; core++) {
		timebase = (dram_log_data.virt_addr + BOOTLOG_BASE - BL2LOG_BASE)
					+ (core * BOOTLOG_SECTOR_SIZE);
		dram_magic_code = readq(timebase + 0x8);
		if (dram_magic_code != BOOTLOG_MAGIC)
			continue;

		next_log_Offset = readq(timebase);

		if (next_log_Offset > 0x10
				&& (pLineCore[core] - (u64)timebase) < next_log_Offset) {
			return core;
		}
	}

	return 999;
}

static void show_dram_log_integrated_sight(struct seq_file *seq)
{
	u32 core = 0;
	u32 min_archtime_value;
	u32 next_min_archtime_value;

	for (core = 0; core < nr_cpus ; core++) {
		pLineCore[core] = (u64)((dram_log_data.virt_addr + BOOTLOG_BASE - BL2LOG_BASE)
							+ (core * BOOTLOG_SECTOR_SIZE) + 0x10);
	}

	while (condition_loop_end()) {
		core = get_minimal_time_value(&min_archtime_value, &next_min_archtime_value);
		show_curr_log_line(core, min_archtime_value, next_min_archtime_value, seq);
	}

	core = get_only_one_left_log_core();

	if (core < nr_cpus)
		show_dram_log_lines((char *) pLineCore[core], seq);

	seq_printf(seq, "\n");
}

static void show_bl2_dram_log(struct seq_file *seq)
{
	u32 magic_base = readl(dram_log_data.virt_addr + 0x8);
	u32 end_log = readl(dram_log_data.virt_addr);

	if (magic_base != BL2LOG_MAGIC) {
		seq_printf(seq, "[DramBootLog] BL2 MAGIC is wrong!\n");
		return;
	}

	writel('\0', dram_log_data.virt_addr + end_log);
	show_dram_log_lines((char *) (dram_log_data.virt_addr + 0x10), seq);
	seq_printf(seq, "\n");
}

static int bootlog_show(struct seq_file *s, void *data)
{
	show_bl2_dram_log(s);
	show_dram_log_integrated_sight(s);

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(bootlog);

static int exynos_dram_bootlog_probe(struct platform_device *pdev)
{
	struct reserved_mem *rmem;
	struct device_node *rmem_np;

	dev_info(&pdev->dev, "%s start \n", __func__);

	if (!of_property_read_u32(pdev->dev.of_node, "nr_cpus", &nr_cpus)) {
		BOOTLOG_SECTOR_SIZE = BOOTLOG_SIZE/nr_cpus;
	} else {
		dev_err(&pdev->dev, "no such nr_cpus\n");
		return -ENODEV;
	}

	rmem_np = of_parse_phandle(pdev->dev.of_node, "memory-region", 0);
	if (!rmem_np) {
		dev_err(&pdev->dev, "no such memory-region\n");
		return -ENODEV;
	}

	rmem = of_reserved_mem_lookup(rmem_np);
	if (!rmem) {
		dev_err(&pdev->dev, "no such reserved mem of node name %s\n",
				pdev->dev.of_node->name);
		return -ENODEV;
	}

	if (!rmem->base || !rmem->size)
		return -EPROBE_DEFER;

	dram_log_data.phys_addr = rmem->base;
	dram_log_data.size = rmem->size;

	dev_info(&pdev->dev, "Base(0x%lx) Size(0x%lx) \n",
		 dram_log_data.phys_addr, dram_log_data.size);

	dram_log_data.virt_addr =
		exynos_dram_bootlog_request_region(dram_log_data.phys_addr,
						   dram_log_data.size);
	if (!dram_log_data.virt_addr) {
		dev_err(&pdev->dev, "Fail to translate DRAM Boot log AREA\n");
		return -EFAULT;
	}

	bootlog_debugfs_dir = debugfs_create_dir("dram_bootlog", NULL);

	debugfs_create_file("bootlog", 0400, bootlog_debugfs_dir,
			    NULL, &bootlog_fops);

	return 0;
}

static int exynos_dram_bootlog_remove(struct platform_device *pdev)
{
	debugfs_remove_recursive(bootlog_debugfs_dir);

	return 0;
}

static const struct of_device_id exynos_dram_bootlog_of_match_table[] = {
	{ .compatible = "samsung,exynos-drambootlog", },
	{ },
};

static struct platform_driver exynos_dram_bootlog_driver = {
	.probe = exynos_dram_bootlog_probe,
	.remove = exynos_dram_bootlog_remove,
	.driver = {
		.name = "exynos-drambootlog",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(exynos_dram_bootlog_of_match_table),
	}
};

module_platform_driver(exynos_dram_bootlog_driver);

MODULE_DESCRIPTION("ExynosAuto9 DRAM boot log printing driver");
MODULE_AUTHOR("<jaewoo.yang@samsung.com>");
MODULE_LICENSE("GPL");
