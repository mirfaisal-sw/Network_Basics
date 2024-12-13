
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/vmalloc.h>

#include "malloc_trc.h"

void* my_kmalloc(size_t size)
{
        return kmalloc(size, GFP_KERNEL);
}

void my_kfree(void* addr)
{
        kfree(addr);
}

void* my_kcalloc(size_t num, size_t size)
{
        return kcalloc(num, size, GFP_KERNEL);
}

void* my_vmalloc(size_t size)
{
        return vmalloc(size);
}

void my_vfree(void* addr)
{
        return vfree(addr);
}
