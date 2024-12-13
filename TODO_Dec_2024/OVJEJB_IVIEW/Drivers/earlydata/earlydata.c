// SPDX-License-Identifier: GPL-2.0
/*
 * earlydata.c
 * Copyright (C) 2023 Harman International Ltd,
 *  by Shreeharsha I <Shreeharsha.Indhudhara@harman.com>
 *  Created on: 10-Aug-2023
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2.0 as published by
 * the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include "earlydata.h"
static struct early_data *eddev;
static struct class *cl;
static struct device *dev_rt;
static bool eddev_probed;

#ifdef CONFIG_OF_RESERVED_MEM
static int __init early_data_reserved_mem_setup(struct reserved_mem *rmem)
{
	pr_err("%s: early-data reserved mem addr=0x%lx, size=0x%lx\n",
			__func__, (unsigned long)rmem->base, (unsigned long)rmem->size);

	return 0;
}
RESERVEDMEM_OF_DECLARE(early_data, "harman,idcevo_earlydata", early_data_reserved_mem_setup);
#endif  /* CONFIG_OF_RESERVED_MEM */

int read_adjustdata(u8 *buf, int offset, int len)
{

	pr_info("%s: offset:%d len:%d\n", __func__, offset, len);

	if (!eddev_probed) {
		pr_err("Early data driver has not probed\n");
		return -EPERM;
	}

	if (!eddev->adjust_data) {
		dev_err(&eddev->pdev->dev, "No adjust data present\n");
		return -ENOMEM;
	}

	if (!buf)
		return -ENOMEM;

	if (offset + len > ADJUSTDATA_LENGTH) {
		dev_err(&eddev->pdev->dev, "beyond adjustdata region, access denied\n");
		return -EINVAL;
	}

	if (offset < ADJUSTDATA_OFFSET) {
		dev_err(&eddev->pdev->dev, "outside adjustdata region, access denied\n");
		return -EINVAL;
	}

	memcpy(buf, (eddev->adjust_data + offset), len);

	return 0;
}
EXPORT_SYMBOL(read_adjustdata);
/**
 * eddev_open() - open method for character device
 * @i: inode pointer
 * @f: pointer to device file
 *
 * This function is invoked on fopen call on char device
 * Return: 0 on suceess else failure
 **/
static int eddev_open(struct inode *i, struct file *f)
{
	struct platform_device  *pdev;
	f->private_data	= eddev;
	pdev = eddev->pdev;
	dev_info(&pdev->dev, "device opened\n");
	return 0;
}

static ssize_t eddev_read(struct file *file, char __user *buffer,
		size_t count, loff_t *offset)
{
	struct  early_data *eddev;
	struct platform_device  *pdev;

	eddev = file->private_data;
	pdev = eddev->pdev;
	if ((*offset + count) > ADJUSTDATA_LENGTH) {
		dev_err(&pdev->dev,
			"Access beyond adjustdata can only copy %lld bytes from offset:%lld\n",
			ADJUSTDATA_LENGTH - *offset, *offset);
		return -EINVAL;
	}

	dev_info(&pdev->dev, "offset :%lld count:%ld\n", *offset, count);
	/* copy data from kernel to user space*/
	if (copy_to_user(buffer, eddev->adjust_data + *offset,
				count) != 0) {
		dev_err(&pdev->dev, "copy to user failed\n");
		return -EFAULT;
	}

	*offset += count;
	return count;
}
/**
 * eddev_llseek() - seek method for character device
 * @file: pointer to device file
 * @offset: position to seek in file
 * @whence: From what position in file
 * This function is invoked on fclose call on char device
 * Return: 0 on suceess else failure
 **/

static loff_t eddev_llseek(struct file *file, loff_t offset, int whence)
{
	loff_t newpos;
	struct  early_data *eddev;
	struct platform_device  *pdev;

	eddev = file->private_data;
	pdev = eddev->pdev;
	/*
	 * offset can not be  less than zero
	 * nor beyond ADJUSTDATA region
	 */
	if (offset < 0 || offset > ADJUSTDATA_LENGTH) {
		dev_err(&pdev->dev, "Access beyond adjust region not allowed\n");
		return -EINVAL;
	}

	switch (whence) {
	case 0: /* SEEK_SET */
		newpos = offset;
		break;
	case 1: /* SEEK_CUR */
		newpos = file->f_pos + offset;
		break;
	case 2: /* SEEK_END */
		newpos = ADJUSTDATA_LENGTH;
		break;
	default:
		return -EINVAL; /* Invalid argument */
	}
	if (newpos < 0)
		return -EINVAL; /* Invalid argument */
	file->f_pos = newpos;

	return newpos;
}

/**
 * eddev_close() - close method for character device
 * @i: inode pointer
 * @f: pointer to device file
 *
 * This function is invoked on fclose call on char device
 * Return: 0 on suceess else failure
 **/
static int eddev_close(struct inode *i, struct file *f)
{

	struct platform_device  *pdev = NULL;

	pdev = eddev->pdev;
	dev_info(&pdev->dev, "%s device closed", "eddev");
	return 0;
}

/**
 * mmap_open() - open method applied on VMA
 * @vma: pointer to vm_area_struct
 *
 * This function is invoked anytime a process forks
 * and creates a new reference to the VMA.
 **/

#ifdef MMAP_ENABLE
static void mmap_open(struct vm_area_struct *vma)
{
	struct early_data *eddev = NULL;
	struct platform_device  *pdev = NULL;

	eddev = vma->vm_private_data;
	pdev = eddev->pdev;
	dev_info(&pdev->dev, "%s process is mapped to Reserved memory\n",
			current->comm);
}
#endif

/**
 * mmap_close() - open method applied on VMA
 * @vma: pointer to vm_area_struct
 *
 * This function is invoked anytime a process closes VMA.
 **/
#ifdef MMAP_ENABLE
static void mmap_close(struct vm_area_struct *vma)
{
	struct early_data *eddev = NULL;
	struct platform_device  *pdev = NULL;

	eddev = vma->vm_private_data;
	pdev = eddev->pdev;
	dev_info(&pdev->dev, "%s process unmapping  Reserved memory\n",
			current->comm);
}
#endif

#ifdef MMAP_ENABLE
static const struct vm_operations_struct mmap_ops = {

	.open           = mmap_open,
	.close          = mmap_close,
	.fault          = NULL,
};
#endif

#ifdef MMAP_ENABLE
/**
 * eddev_mmap() - mmap method
 * @vma: pointer to vm_area_struct
 * @f:pointer file associated with character device
 *
 * This function is invoked anytime a mmap call is
 * done from userspace. Basically rempas virtual pages
 * to physical pages.
 **/
static int eddev_mmap(struct file *f, struct vm_area_struct *vma)
{
	struct early_data *eddev = NULL;
	struct platform_device  *pdev = NULL;
	unsigned long off;
	int size, ret;

	size  = (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;
	vma->vm_ops = &mmap_ops;
	eddev = f->private_data;
	vma->vm_private_data = f->private_data;
	pdev = eddev->pdev;

	if (eddev == NULL || pdev == NULL) {
		dev_err(&pdev->dev, "eddev error\n ");
		return -EINVAL;
	}

	dev_info(&pdev->dev, "size_vm: %lu shift_vm:%d page_flag:0x%lx\n",
		(vma->vm_end - vma->vm_start), size, vma->vm_flags);

	//check for access permission flag
	if ((vma->vm_flags & VM_WRITE) || (vma->vm_flags & VM_EXEC)
			|| (vma->vm_flags & VM_SHARED)) {
		dev_info(&pdev->dev, "Only read allowed page_flag:0x%lx\n", vma->vm_flags);
		return -EINVAL;
	}

	// check for access beyond adjust data
	if (vma->vm_pgoff > OFFSET) {
		dev_info(&pdev->dev, "mapping beyond adjust data page not allowed\n");
		return -EINVAL;
	}
	off = eddev->start_pfn + vma->vm_pgoff;

	dev_info(&pdev->dev, "size: %lu shift:%lu\n",
		eddev->psize, eddev->psize >> PAGE_SHIFT);

	if (size + vma->vm_pgoff  > (eddev->psize >> PAGE_SHIFT)) {
		dev_err(&pdev->dev, "beyond reserved memory vm_pgoff:%lu size in pages:%d\n",
			vma->vm_pgoff, size);
		return -EINVAL;
	}
	ret = remap_pfn_range(vma, vma->vm_start, off,
			(vma->vm_end - vma->vm_start), vma->vm_page_prot);
	if (ret < 0) {
		dev_err(&pdev->dev, "memory map failed\n");
		return ret;
	}

	mmap_open(vma);
	return 0;
}
#endif
static const struct file_operations eddev_fops = {
	.owner	= THIS_MODULE,
	.open	= eddev_open,
	.read	= eddev_read,
	.write	= NULL,
	.llseek  = eddev_llseek,
	.release = eddev_close,
#ifdef MMAP_ENABLE
	.mmap	= eddev_mmap,
#endif
};

static char *ed_devnode(struct device *dev, umode_t *mode)
{
	if (!mode)
		return NULL;

	*mode = 0400;
	return NULL;
}

/**
 * eddev_debug_store- Enable debug logs for earlydata
 * @dev: pointer to struct device
 * @attr: pointer to device attribute structure
 * @buf: constant char pointer from userspace
 * @count: number of bytes to write
 * Return: string length of buf
 **/
static ssize_t eddev_debug_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int ret;
	u8 val;

	ret = kstrtou8(buf, 0, &val);
	if (ret)
		return ret;

	if (val > 1 || val < 0) {
		dev_info(dev, "%s: Invalid input accepts either 0 or 1\n",
			__func__);
		return -EINVAL;
	}

	if (val)
		eddev->__eddev_debug = 1;
	else
		eddev->__eddev_debug = 0;
	dev_info(dev, "%s: eddev debug :%d\n", __func__,
			eddev->__eddev_debug);
	return strlen(buf);
}

static DEVICE_ATTR_WO(eddev_debug);
static struct attribute *eddev_attrs[] = {
	&dev_attr_eddev_debug.attr,
	NULL
};

static const struct attribute_group eddev_group = {
	.attrs = eddev_attrs,
};

static int earlydata_probe(struct platform_device *pdev)
{
	int ret = 0;
	int j;
	u8 *addr;

	eddev = kzalloc(sizeof(struct early_data), GFP_KERNEL);
	eddev->adjust_data = kmalloc(ADJUSTDATA_LENGTH, GFP_KERNEL);

	if (eddev == NULL || eddev->adjust_data == NULL)
		return -ENOMEM;

	eddev->pdev = pdev;

	eddev->m_np = of_parse_phandle(pdev->dev.of_node, "memory-region", 0);
	if (eddev->m_np == NULL) {
		dev_err(&pdev->dev, "No %s specified\n", "memory-region");
		ret = -ENOENT;
		goto eddev_error;
	}

	ret = of_address_to_resource(eddev->m_np, 0, &eddev->res);
	if (ret) {
		dev_err(&pdev->dev, "No memory address assigned to the region\n");
		goto eddev_error;
	}

	eddev->psize = eddev->res.end - eddev->res.start + 1;


	if (eddev->psize < PAGE_SIZE) {
		dev_err(&pdev->dev, "invalid reserved memory size %lu Pagesize:%lu\n",
			eddev->psize, PAGE_SIZE);
		ret = -EINVAL;
		goto eddev_error;
	}
	eddev->paddr = eddev->res.start;
	eddev->start_pfn = eddev->paddr >> PAGE_SHIFT;
	eddev->end_pfn = (eddev->paddr  + (eddev->psize)) >> PAGE_SHIFT;
	dev_info(&pdev->dev, "start pfn: %lu end pfn:%lu\n", eddev->start_pfn, eddev->end_pfn);


	eddev->vaddr = ioremap((eddev->paddr + ADJUSTDATA_OFFSET), ADJUSTDATA_LENGTH);
	if (!eddev->vaddr) {
		dev_err(&pdev->dev, "ioremap fail\n");
		ret = -ENOMEM;
		goto eddev_error;
	}

	memcpy_fromio(eddev->adjust_data, eddev->vaddr, ADJUSTDATA_LENGTH);
	if (eddev->__eddev_debug) {
		addr  = eddev->adjust_data;
		for (j = 0; j < ADJUSTDATA_LENGTH; j = j + 8)
			dev_dbg(&pdev->dev,
				"[%08d:%04d] %02X %02X %02X %02X %02X %02X %02X %02X ...%c%c%c%c%c%c%c%c\n",
				0, j,
				*(char *)(addr+j+0), *(char *)(addr+j+1),
				*(char *)(addr+j+2), *(char *)(addr+j+3),
				*(char *)(addr+j+4), *(char *)(addr+j+5),
				*(char *)(addr+j+6), *(char *)(addr+j+7),
				*(char *)(addr+j+0), *(char *)(addr+j+1),
				*(char *)(addr+j+2), *(char *)(addr+j+3),
				*(char *)(addr+j+4), *(char *)(addr+j+5),
				*(char *)(addr+j+6), *(char *)(addr+j+7)
				);
	}

	ret = alloc_chrdev_region(&eddev->dev, MINOR_NUM,
			DEV_COUNT, "ed-dev");

	if (ret != 0) {
		dev_err(&pdev->dev, "device file registration failed %d\n", ret);
		goto eddev_error;
	}

	/*
	 * dev_info(&pdev->dev, "<Major:Minor> <%d:%d>\n", MAJOR(eddev->dev),
	 MINOR(eddev->dev));
	 */

	cl = class_create(THIS_MODULE, "edclass");
	if (IS_ERR(cl)) {
		dev_err(&pdev->dev, "edclass creation failed\n");
		ret = PTR_ERR(cl);
		goto eddev_error;
	}
	cl->devnode = ed_devnode;
	dev_rt = device_create(cl, &pdev->dev, eddev->dev, eddev, "adjust_data");

	if (dev_rt == NULL) {
		dev_err(&pdev->dev, "device node creation failed\n");
		class_destroy(cl);
		unregister_chrdev_region(eddev->dev, DEV_COUNT);
		ret = PTR_ERR(dev_rt);
		goto eddev_error;

	}

	cdev_init(&eddev->cdev, &eddev_fops);

	ret = cdev_add(&eddev->cdev, eddev->dev, DEV_COUNT);

	if (ret < 0) {
		dev_err(&pdev->dev, "char device add failed %d\n", ret);
		device_destroy(cl, eddev->dev);
		class_destroy(cl);
		unregister_chrdev_region(eddev->dev, DEV_COUNT);
		goto eddev_error;
	}

	ret = sysfs_create_group(&pdev->dev.kobj, &eddev_group);
	if (ret)
		goto eddev_error;
	dev_set_drvdata(&eddev->pdev->dev, eddev);
	dev_info(&pdev->dev, "eddev added\n");

	eddev_probed = true;

	return 0;
eddev_error:
	kfree(eddev->adjust_data);
	eddev->adjust_data = NULL;
	kfree(eddev);
	return ret;
}

static const struct of_device_id earlydata_id = {
	.compatible = "harman,idcevo-earlydata",
};
MODULE_DEVICE_TABLE(of, earlydata_id);

static struct platform_driver earlydata_driver = {
	.driver	= {
		.name = "idcevo-data",
		.owner = THIS_MODULE,
		.of_match_table = &earlydata_id,
	},
	.probe	= earlydata_probe,
};

static int __init earlydata_init(void)
{
	platform_driver_register(&earlydata_driver);

	return 0;
}

static void __exit earlydata_exit(void)
{
	device_destroy(cl, eddev->dev);
	class_destroy(cl);
	unregister_chrdev_region(eddev->dev, DEV_COUNT);
	kfree(eddev->adjust_data);
	kfree(eddev);
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shreeharsha <shreeharsha.indhudhara@harman.com>");
MODULE_DESCRIPTION("Earlydata Driver");

module_init(earlydata_init);
module_exit(earlydata_exit);
