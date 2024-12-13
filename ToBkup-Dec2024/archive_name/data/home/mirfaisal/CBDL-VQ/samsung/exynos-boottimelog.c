// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * EXYNOS - Boot time table extractor
 */

#include <linux/err.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_reserved_mem.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched/clock.h>

#include <linux/soc/samsung/exynos-kernel-boottime.h>
#include <linux/soc/samsung/exynos-soc.h>

/* Time Stamp size per core */
#define EXYNOSAUTO_TIMESTAMP_PER_CORE_SIZE (8 * 1024)
#define IMPL_PROC_OPS

struct boottimelog_data {
	struct proc_dir_entry *proc_file;
	void *virt_addr;
	unsigned long phys_addr;
	unsigned long size;
};

static struct boottimelog_data timedata = { 0 };

static void *exynos_request_region(unsigned long addr, unsigned int size)
{
	int i;
	unsigned int num_pages = (size >> PAGE_SHIFT);
	pgprot_t prot = pgprot_writecombine(PAGE_KERNEL);
	struct page **pages = NULL;
	void *v_addr = NULL;

	if (!addr) {
		return NULL;
	}

	pages = kmalloc_array(num_pages, sizeof(struct page *), GFP_ATOMIC);
	if (!pages) {
		return NULL;
	}

	for (i = 0; i < num_pages; i++) {
		pages[i] = phys_to_page(addr);
		addr += PAGE_SIZE;
	}

	v_addr = vmap(pages, num_pages, VM_MAP, prot);
	kfree(pages);

	return v_addr;
}

static int boottime_show(struct seq_file *seq, void *pdata)
{
	seq_puts(seq, "Open Boottime\n");
	seq_printf(seq, "Base(0x%lx) Size(0x%lx)\n", timedata.phys_addr,
		 timedata.size);

	return 0;
}

static int boottime_proc_open(struct inode *inode, struct file *file)
{
	struct platform_device *pdev = PDE_DATA(file_inode(file));
	int ret;

	ret = single_open(file, boottime_show, pdev);

	return ret;
}

static int boottime_proc_release(struct inode *inode, struct file *file)
{
	int res = single_release(inode, file);

	return res;
}

static ssize_t boottime_proc_read(struct file *file,
		char *buff, size_t buff_size, loff_t *offset)
{
	ssize_t read_cnt = 0;

	if (buff == NULL || offset == NULL)
		return read_cnt;

	if ((loff_t) timedata.size > *offset) {
		read_cnt = min(buff_size,
					timedata.size - (size_t) *offset);

		if (copy_to_user(buff,
				timedata.virt_addr + *offset, read_cnt))
			read_cnt = -EFAULT;
		else
			*offset += read_cnt;
	}

	return read_cnt;
}

#ifdef IMPL_PROC_OPS
static const struct proc_ops boottime_proc_fops = {
	.proc_lseek	= default_llseek,
	.proc_open      = boottime_proc_open,
	.proc_read      = boottime_proc_read,
	.proc_release   = boottime_proc_release,
};
#else
static const struct file_operations boottime_proc_fops = {
	.llseek     = default_llseek,
	.open      = boottime_proc_open,
	.read      = boottime_proc_read,
	.release   = boottime_proc_release,
};
#endif

static void log_kernel_clock(
			struct platform_device *pdev,
			struct device_node *btime_dev)
{
	u32 cur_rtc = 0;
	static void __iomem *rtc_base = (void __iomem *)0xffffffff;
	u32 boottime_src_clk_base, rem_nsec, sec;
	u64 ts_nsec, value_rtc, value_local, mili, diff;

	if (of_property_read_u32(btime_dev, "btime-clk-src", &boottime_src_clk_base)) {
		dev_err(&pdev->dev, "failed to get boottime source clock information\n");
		return;
	}

	if (boottime_src_clk_base == 0) {
		dev_err(&pdev->dev, "Clock information is wrong\n");
		return;
	}

	rtc_base = ioremap(boottime_src_clk_base, 0x4);
	if (rtc_base == NULL) {
		dev_err(&pdev->dev, "Clock information is wrong - wrong clk cnt sfr\n");
		return;
	}

	cur_rtc = ioread32(rtc_base);
	ts_nsec = local_clock();

	sec = (cur_rtc / 2048);
	mili = (((u64) (((u64) (cur_rtc)) * 1000000000)) / 2048) % 1000000000;

	rem_nsec = do_div(ts_nsec, 1000000000);

	dev_info(&pdev->dev,
		"Current RTC time clk(%u) %u.%09llu sec", cur_rtc, sec, mili);
	dev_info(&pdev->dev,
		"Current local time %llu.%09u sec", ts_nsec, rem_nsec);

	value_rtc = (((u64) sec) * ((u64) 1000000000)) + mili;
	value_local = (ts_nsec * ((u64) 1000000000)) + rem_nsec;
	if (value_rtc >= value_local) {
		diff = (value_rtc - value_local);
		rem_nsec = do_div(diff, 1000000000);
		dev_info(&pdev->dev, "diff [RTC] - [LOCAL] = %llu.%u sec", diff, rem_nsec);
	} else {
		diff = (value_local - value_rtc);
		rem_nsec = do_div(diff, 1000000000);
		dev_info(&pdev->dev, "diff [RTC] - [LOCAL] = -%llu.%u sec", diff, rem_nsec);
	}
}


static int exynos_boottime_probe(struct platform_device *pdev)
{
	struct reserved_mem *rmem;
	struct device_node *rmem_np;

	dev_info(&pdev->dev, "%s start\n", __func__);

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

	timedata.phys_addr = rmem->base;
	timedata.size = rmem->size;

	dev_info(&pdev->dev, "Base(0x%lx) Size(0x%lx) \n", timedata.phys_addr,
		 timedata.size);

	timedata.virt_addr =
		exynos_request_region(timedata.phys_addr, timedata.size);

	if (!timedata.virt_addr) {
		dev_err(&pdev->dev, "Failed to remap boottime log area\n");
		return -EFAULT;
	}

	timedata.proc_file = proc_create_data("boottime", 0400, NULL,
				&boottime_proc_fops, pdev);

	log_kernel_clock(pdev, pdev->dev.of_node);

	return 0;
}

static int exynos_boottime_remove(struct platform_device *pdev)
{
	if (timedata.virt_addr)
		vunmap(timedata.virt_addr);

	proc_remove(timedata.proc_file);

	return 0;
}

static const struct of_device_id exynos_boottimelog_of_match_table[] = {
	{
		.compatible = "samsung,exynos-boottimelog",
	},
	{},
};

static struct platform_driver exynos_boottime_driver = {
	.probe = exynos_boottime_probe,
	.remove = exynos_boottime_remove,
	.driver = {
		.name = "exynos-boottimelog",
		.owner = THIS_MODULE,
		.of_match_table = exynos_boottimelog_of_match_table,
	}
};

module_platform_driver(exynos_boottime_driver);

MODULE_DESCRIPTION("ExynosAuto9 Boottime log driver");
MODULE_AUTHOR("<jaewoo.yang@samsung.com>");
MODULE_LICENSE("GPL");
