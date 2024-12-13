/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  earlydata.h
 *  Copyright (C) 2024 Harman International Ltd,
 *
 *  Created on: 01-Mar-2024
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
#ifndef EARLYDATA_H
#define EARLYDATA_H

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/major.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>

struct early_data {
	dev_t                   dev;
	struct cdev             cdev;
	struct platform_device  *pdev;
	struct device_node      *m_np;
	struct resource         res;
	size_t                  size;
	size_t                  psize;
	phys_addr_t             paddr;
	void __iomem            *vaddr;
	u8                      *adjust_data;
	unsigned long           start_pfn;
	unsigned long           end_pfn;
	bool                    __eddev_debug;
};
extern int read_adjustdata(u8 *buf, int offset, int len);

#define MINOR_NUM       0
#define DEV_COUNT       1
#define OFFSET          0
#define ADJUSTDATA_LENGTH       512
#define ADJUSTDATA_OFFSET       16
#endif /* end of earlydata*/
