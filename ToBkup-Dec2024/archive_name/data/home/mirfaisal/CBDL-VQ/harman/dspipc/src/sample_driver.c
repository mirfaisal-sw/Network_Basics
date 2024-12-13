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

#include "dev-dspipc.h"
#include "memdev.h"

MODULE_LICENSE("Dual BSD/GPL");

static char* argv = "hello";
module_param(argv, charp, S_IRUGO);

static int mem_major = MEMDEV_MAJOR;
static struct mem_dev* mem_devp;
static struct cdev chrdev;
static struct class* driverclass;

static int mem_open(struct inode* inode, struct file* filp)
{
        printk("[Sample driver]:%s:called!\n", __FUNCTION__);
        return 0;
}

static int mem_release(struct inode* inode, struct file* filp)
{
        printk("[Sample driver]:%s:called!\n", __FUNCTION__);
        return 0;
}

static ssize_t mem_read(struct file* filp, char __user* buf, size_t size, loff_t* ppos)
{
        printk("[Sample driver]:%s:called!\n", __FUNCTION__);
        return 0;
}

static ssize_t mem_write(struct file* filp, const char __user* buf, size_t size, loff_t* ppos)
{
        printk("[Sample driver]:%s:called!\n", __FUNCTION__);
        return 0;
}

static loff_t mem_llseek(struct file* filp, loff_t offset, int whence)
{
        printk("[Sample driver]:%s:called!\n", __FUNCTION__);
        return 0;
}

static const struct file_operations mem_fops = {
        .owner = THIS_MODULE,
        .llseek = mem_llseek,
        .read = mem_read,
        .write = mem_write,
        .open = mem_open,
        .release = mem_release,
};

static int hello_init(void)
{
        int result;
        int i;

        printk("[Sample driver]:%s:argv: %s\n", __FUNCTION__, argv);

        dev_t devno = MKDEV(mem_major, 0);

        if (mem_major) {
                result = register_chrdev_region(devno, 2, "sampledev");
        } else {
                result = alloc_chrdev_region(&devno, 0, 2, "sampledev");
                mem_major = MAJOR(devno);
        }

        if (result < 0) {
                printk("[Sample driver]:%s:register char device region failed!\n", __FUNCTION__);
                return result;
        }
        printk("[Sample driver]:%s:major devno is %d\n", __FUNCTION__, mem_major);

        /* initialize cdev object. */
        cdev_init(&chrdev, &mem_fops);
        chrdev.owner = THIS_MODULE;
        chrdev.ops = &mem_fops;

        /* register char device. */
        cdev_add(&chrdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

        /* allocate memory for every device. */
        mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);
        if (!mem_devp) {
                result = -ENOMEM;
                goto fail_malloc;
        }
        memset(mem_devp, 0, sizeof(struct mem_dev));

        /* allocate memory for buffer. */
        for (i = 0; i < MEMDEV_NR_DEVS; i++) {
                mem_devp[i].size = MEMDEV_SIZE;
                mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
                memset(mem_devp[i].data, 0, MEMDEV_SIZE);
        }

        /* add udev support. */
        driverclass = class_create(THIS_MODULE, "sampledev");
        if (!driverclass) {
                result = -ENOMEM;
                printk("[Sample driver]:%s:class create failed!\n", __FUNCTION__);
                goto failed_class_create;
        }
        device_create(driverclass, NULL, MKDEV(mem_major, 0), NULL, "sampledev");

        printk("[Sample driver]:%s:successfully!\n", __FUNCTION__);
        return 0;

failed_class_create:
        for (i = 0; i < MEMDEV_NR_DEVS; i++) {
                if (mem_devp[i].data) {
                        kfree(mem_devp[i].data);
                }
        }

fail_malloc:
        cdev_del(&chrdev);
        unregister_chrdev_region(devno, 1);
        printk("[Sample driver]:%s:failed!\n", __FUNCTION__);
        return result;
}

static void hello_exit(void)
{
        int i;
        printk("[Sample driver]:%s:byebye!\n", __FUNCTION__);
        cdev_del(&chrdev);
        for (i = 0; i < MEMDEV_NR_DEVS; i++) {
                if (mem_devp[i].data) {
                        kfree(mem_devp[i].data);
                }
        }
        kfree(mem_devp);
        device_destroy(driverclass, MKDEV(mem_major, 0));
        class_destroy(driverclass);
        unregister_chrdev_region(MKDEV(mem_major, 0), 2);
}

module_init(hello_init);
module_exit(hello_exit);
