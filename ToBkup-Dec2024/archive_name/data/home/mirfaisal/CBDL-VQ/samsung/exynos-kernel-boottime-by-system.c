// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * EXYNOS - Exynosauto Boot time measurement support
 */

#include <linux/types.h>
#include <asm/io.h>
#include <linux/smc.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/soc/samsung/exynos-kernel-boottime.h>
#include <linux/soc/samsung/exynos-soc.h>

#ifdef CONFIG_VLM
#include <nk/nkern.h>
#endif

#define  MAX_CORE_CNT      10

static u32 chip_id;
static void __iomem *g_rtc_base = (void __iomem *)0xffffffff;

static u32 get_kernel_running_cpu_id(void)
{
	u32 ret = 0;
#ifdef CONFIG_VLM
	static u32 cpus[MAX_CORE_CNT] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
	u32 id = raw_smp_processor_id();

	if (cpus[id] >= MAX_CORE_CNT) {
		cpus[id] = hyp_call_get_cpuid();
	}
	ret = cpus[id];
#else
	ret = raw_smp_processor_id();
#endif

	return ret;
}

static void *exynos_boottime_request_region(unsigned long addr,
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

static u32 get_vm_category(void)
{
	static u32 category = 0xffffffff;

	if (category == 0xffffffff) {
#ifdef CONFIG_VLM
		static int vmid_for_category = -1;

		if (vmid_for_category < 0)
			vmid_for_category = (int)hyp_call_id_get();

		if (vmid_for_category == 2)
			category = EXYNOSAUTO_TIMELINE_DOM_VM2_BASE;
		else if (vmid_for_category == 3)
			category = EXYNOSAUTO_TIMELINE_DOM_VM3_BASE;
		else if (vmid_for_category == 4)
			category = EXYNOSAUTO_TIMELINE_DOM_VM4_BASE;
		else if (vmid_for_category == 5)
			category = EXYNOSAUTO_TIMELINE_DOM_VM5_BASE;
		else
			return 0x0;

#else
		category = EXYNOSAUTO_TIMELINE_BARE_KERNEL_BASE;
#endif
	}

	return category;
}

static void logging_core_sector(u32 index, u32 cur_rtc)
{
	u32 cur_cpu;
	u32 next_offset = 0;
	int i = 0;
	static u32 *timebase[MAX_CORE_CNT] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

	cur_cpu = get_kernel_running_cpu_id();

	if (cur_cpu >= MAX_CORE_CNT)
		return;

	if (timebase[cur_cpu] == 0x0) {
		for (i = 0; i < MAX_CORE_CNT; i++) {
			timebase[i] = exynos_boottime_request_region(
				BOOTTIME_LOG_BASE_FROM +
					(i * BOOTTIME_LOG_PER_CORE_SIZE),
				BOOTTIME_LOG_PER_CORE_SIZE);
		}
	}

	next_offset = readl(timebase[cur_cpu]) + 0x10;

	if (((next_offset + 0x10) < BOOTTIME_LOG_PER_CORE_SIZE) &&
	    (cur_cpu * BOOTTIME_LOG_PER_CORE_SIZE + (next_offset + 0x10)) <
		    BOOTTIME_LOG_SIZE) {
		u32 category = get_vm_category();

		if (category == 0x0)
			return;

		writel(next_offset, timebase[cur_cpu]);
		next_offset = next_offset / 4;
		writel(category, (timebase[cur_cpu] + next_offset));
		writel(index, (timebase[cur_cpu] + next_offset) + 0x01);

		writel(cur_rtc, (timebase[cur_cpu] + next_offset + 0x2));
		writel(0x0, (timebase[cur_cpu] + next_offset + 0x3));
	}
}

void exynos_kernel_boottime_log(u32 index)
{
	u32 cur_rtc = 0;
	static void __iomem *chip_id_base = (void __iomem *)0xffffffff;

	if (chip_id == 0) {
	chip_id_base = ioremap(EXYNOS_CHIPID_BASE, 0x4);
	if (chip_id_base == 0)
		return;

	chip_id = ioread32(chip_id_base);
	if (chip_id < 0)
		return;

	if (chip_id == EXYNOSAUTO9_SOC_ID)
		g_rtc_base = ioremap(EXYNOS_AUTO9_RTC_CURTICCNT_0, 0x4);
	else
		g_rtc_base = ioremap(EXYNOS_AUTO920_RTC_CURTICCNT_0, 0x4);
	}

	cur_rtc = ioread32(g_rtc_base);

	logging_core_sector(index, cur_rtc);
}
EXPORT_SYMBOL(exynos_kernel_boottime_log);

void exynos_kernel_smc_boottime_log(u32 index)
{
}
EXPORT_SYMBOL(exynos_kernel_smc_boottime_log);
