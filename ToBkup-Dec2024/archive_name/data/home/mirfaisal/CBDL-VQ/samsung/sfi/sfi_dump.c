// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - sfi dump driver support to dump SFI CPU
 */

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_reserved_mem.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/string.h>
#include <linux/syscalls.h>
#include <soc/samsung/sfi_ipc_ctrl.h>

#define SFI_DUMP_CH_NAME "sfi_dump"
#define SFI_DUMP_RX_BUFF_SIZE (4)
#define SFI_DUMP_MSG_BUFF_SIZE (2048)
#define SFI_DUMP_READ_BYTE_SIZE (512)

static int dump_ch_no;
static char *sfi_dump_ch_name = SFI_DUMP_CH_NAME;
static char dump_ch_rx_buff[SFI_DUMP_RX_BUFF_SIZE];
static struct sfi_ipc_buf_info sfi_dump_ch_rx_info;

static void *sfi_dump_mem_virt_addr;
static int sfi_dump_mem_size;
static char sfi_dump_msg_buff[SFI_DUMP_MSG_BUFF_SIZE];
static int sfi_dump_read_skip;

static void sfi_dump_ipc_rx_cb(struct sfi_ipc_buf_info *arg)
{
	pr_info("[sfi_dump] %s : sfi call dump request \n", __func__);
	panic("Request of SFI dump\n");
}

static void dump_ch_open(void)
{
	sfi_dump_ch_rx_info.size = SFI_DUMP_RX_BUFF_SIZE;
	sfi_dump_ch_rx_info.addr = dump_ch_rx_buff;
	dump_ch_no = sfi_ipc_open(sfi_dump_ch_name, sfi_dump_ipc_rx_cb,
							  &(sfi_dump_ch_rx_info));
	if (dump_ch_no < 0 || dump_ch_no >= SFI_IPC_CHANNEL) {
		/*invalid ch number */
		pr_info("[sfi_dump] %s : invalid ipc ch number assigned %d \n", __func__,
			dump_ch_no);
	} else {
		pr_info("[sfi_dump] %s : ipc ch opened ch No. %d\n",
			__func__, dump_ch_no);
	}
}

static void *exynos_sfi_dump_request_region(unsigned long addr,
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

static ssize_t fops_read(struct file *file, char __user *data, size_t count, loff_t *ppos)
{
	unsigned int *ptr;
	int ptr_offset;
	int i;
	char *buff_ptr;
	ssize_t un_copied;

	if (sfi_dump_read_skip) {
		sfi_dump_read_skip = 0;
		return 0;
	}

	for (i = 0; i < SFI_DUMP_MSG_BUFF_SIZE; i++)
		sfi_dump_msg_buff[i] = 0;
	ptr = sfi_dump_mem_virt_addr;
	/*  to jump last SFI_DUMP_READ_BYTE_SIZE area */
	ptr_offset = (sfi_dump_mem_size - SFI_DUMP_READ_BYTE_SIZE) / 4;
	ptr += ptr_offset;
	buff_ptr = sfi_dump_msg_buff;
	for (i = 0 ; i < (SFI_DUMP_READ_BYTE_SIZE/4) ; i = i+4) {
		if (sprintf(buff_ptr, "%08x %08x %08x %08x\n",
			(*ptr), (*(ptr+1)), (*(ptr+2)), (*(ptr+3))) < 0) {
			pr_info("[sfi_dump] %s : sprintf failed\n", __func__);
			return 0;
		}
		buff_ptr += 37;
		ptr += 4;
	}
	un_copied = copy_to_user(data, sfi_dump_msg_buff, SFI_DUMP_MSG_BUFF_SIZE);

	sfi_dump_read_skip = 1;
	return SFI_DUMP_MSG_BUFF_SIZE;
}


struct file_operations sfi_dump_fops = {
	.owner = THIS_MODULE,
	.read		= fops_read,
};

static struct miscdevice sfi_dump_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "sfi_dump",
	.fops		= &sfi_dump_fops,
};

static int sfi_dump_probe(struct platform_device *pdev)
{
	struct reserved_mem *rmem;
	struct device_node *rmem_np;
	void *virt_addr;
	int result;

	dev_info(&pdev->dev, "%s start \n", __func__);

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

	virt_addr = exynos_sfi_dump_request_region(rmem->base, rmem->size);
	if (!virt_addr) {
		dev_err(&pdev->dev, "Fail to translate SFI DUMP AREA\n");
		return -EFAULT;
	}
	sfi_dump_mem_virt_addr = virt_addr;
	sfi_dump_mem_size = rmem->size;

	if (of_get_property(pdev->dev.of_node, "panic-request-mode", NULL)) {
		dev_info(&pdev->dev, "%s panic-request-mode\n", __func__);
		dump_ch_open();
	}

	result = misc_register(&sfi_dump_miscdev);
	if (result) {
		dev_err(&pdev->dev, "failed to register misc driver\n");
		return result;
	}

	return 0;
}

static int sfi_dump_remove(struct platform_device *pdev)
{
	sfi_ipc_close(sfi_dump_ch_name);

	return 0;
}

static const struct of_device_id of_sfi_dump_ids[] = {
	{
		.compatible = "samsung,exynos-sfi-dump",
		.data = NULL,
	}, {
		/* sentinel */
	}
};

static struct platform_driver sfi_dump_driver = {
	.probe		= sfi_dump_probe,
	.remove		= sfi_dump_remove,
	.driver = {
		.name	= "sfi_dump",
		.of_match_table = of_match_ptr(of_sfi_dump_ids),
	}
};

static int __init sfi_dump_init(void)
{
	pr_info("[sfi_dump] %s\n", __func__);

	return platform_driver_register(&sfi_dump_driver);
}

static void __exit sfi_dump_exit(void)
{
	pr_info("[sfi_dump] %s\n", __func__);

	return platform_driver_unregister(&sfi_dump_driver);
}

late_initcall(sfi_dump_init);
module_exit(sfi_dump_exit);

MODULE_LICENSE("GPL v2");
