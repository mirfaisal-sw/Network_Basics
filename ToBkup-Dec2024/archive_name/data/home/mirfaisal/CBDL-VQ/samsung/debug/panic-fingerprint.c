// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *
 * Panic Fingerprint driver for Exynos specific extensions
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/of_reserved_mem.h>
#include <linux/panic_notifier.h>
#include <linux/sched/debug.h>
#include <asm/io.h>
#include <asm/stacktrace.h>

#define PANIC_FINGERPRINT_MAGIC_INIT	0xBAC28ACE
#define PANIC_FINGERPRINT_MAGIC_PANIC	0x0F010FD1
#define PANIC_FINGERPRINT_MAGIC_LEN		4

static struct device *panic_fingerprint_dev;
void *rmem_va;
unsigned long rmem_pa;
unsigned long rmem_size;

static void *panic_fingerprint_request_region(unsigned long addr,
		unsigned long size)
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

static int panic_fingerprint_panic_handler(struct notifier_block *np,
		unsigned long l, void *msg)
{
	char *fingerprint_buf = (char *) rmem_va;
	int fingerprint_cnt = PANIC_FINGERPRINT_MAGIC_LEN;
	char func_name_buf[100];
	int func_name_cnt = 0;
	unsigned long stack_arr[64];
	int stack_cnt = 0;
	int i;

	if (readl((void *)fingerprint_buf) != PANIC_FINGERPRINT_MAGIC_INIT)
		return 0;

	writel(PANIC_FINGERPRINT_MAGIC_PANIC, (void *)fingerprint_buf);

	stack_cnt = stack_trace_save(stack_arr, 64, 0);

	fingerprint_cnt += sprintf(fingerprint_buf + fingerprint_cnt, "%s\n", (char *)msg) + 1;
	for (i = 0; i < stack_cnt; i++) {
		func_name_cnt = sprintf(func_name_buf, "%ps\n", (void *)stack_arr[i]) + 1;
		if (fingerprint_cnt + func_name_cnt > rmem_size / (sizeof(char))) {
			dev_info(panic_fingerprint_dev, "fingerprint data over reserved size\n");
			break;
		}

		snprintf(fingerprint_buf + fingerprint_cnt, func_name_cnt, "%s", func_name_buf);
		fingerprint_cnt += func_name_cnt;
	}

	return 0;
}

static struct notifier_block panic_fingerprint_nb = {
	.notifier_call = panic_fingerprint_panic_handler,
	.priority = INT_MAX,
};

static int panic_fingerprint_reserved_memory_map(struct platform_device *pdev)
{
	struct reserved_mem *rmem;
	struct device_node *rmem_np;

	rmem_np = of_parse_phandle(pdev->dev.of_node, "memory-region", 0);
	if (!rmem_np) {
		dev_err(&pdev->dev, "no such memory-region\n");
		return -ENODEV;
	}

	rmem = of_reserved_mem_lookup(rmem_np);
	if (!rmem) {
		dev_err(&pdev->dev, "no such reserced memory of node %s\n",
				pdev->dev.of_node->name);
		return -ENODEV;
	}

	if (!rmem->base || !rmem->size)
		return -EPROBE_DEFER;

	rmem_pa = rmem->base;
	rmem_size = rmem->size;
	rmem_va = panic_fingerprint_request_region(rmem_pa, rmem_size);

	if (!rmem_va)
		return -EFAULT;

	dev_info(&pdev->dev, "base: 0x%lX, size: 0x%lX\n", rmem_pa, rmem_size);

	if (readl(rmem_va) == PANIC_FINGERPRINT_MAGIC_INIT ||
			readl(rmem_va) == PANIC_FINGERPRINT_MAGIC_PANIC)
		return 0;

	writel(PANIC_FINGERPRINT_MAGIC_INIT, rmem_va);
	memset(rmem_va + PANIC_FINGERPRINT_MAGIC_LEN, 0,
			rmem_size - PANIC_FINGERPRINT_MAGIC_LEN);

	return 0;

}

static int panic_fingerprint_probe(struct platform_device *pdev)
{
	int ret = 0;

	panic_fingerprint_dev = &pdev->dev;

	ret = panic_fingerprint_reserved_memory_map(pdev);
	if (ret)
		return ret;

	ret = atomic_notifier_chain_register(&panic_notifier_list,
			&panic_fingerprint_nb);


	return ret;
}

static int panic_fingerprint_remove(struct platform_device *pdev)
{
	atomic_notifier_chain_unregister(&panic_notifier_list, &panic_fingerprint_nb);

	return 0;
}

static const struct of_device_id panic_fingerprint_match[] = {
	{ .compatible = "samsung,panic-fingerprint",},
	{},
};
MODULE_DEVICE_TABLE(of, panic_fingerprint_match);

static struct platform_driver panic_fingerprint_driver = {
	.probe = panic_fingerprint_probe,
	.remove = panic_fingerprint_remove,
	.driver = {
		.name = "panic_fingerprint",
		.of_match_table = panic_fingerprint_match,
		.owner = THIS_MODULE,
	},
};
module_platform_driver(panic_fingerprint_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Panic Fingerprint");
