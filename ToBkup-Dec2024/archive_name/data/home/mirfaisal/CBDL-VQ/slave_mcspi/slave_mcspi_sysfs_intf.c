/*
 * Description            : Provide sysfs entry for slave mcspi driver for vics 
 *
 * Author                : Padmanabha.S <padmanabha.srinivasaiah@harman.com>
 *
 * Copyright (c) 2016 Harman International Industries [connected car]
 *
 */

#include <linux/kernel.h>
#include <linux/sysfs.h>            /* for struct attribute, and more...*/
#include <asm/page.h>                /* for PAGE_SIZE */
#include <linux/kobject.h>            /* for kobj_type */
#include "spi-omap2-mcspiSlave.h"     /* for PERIOD_SZ */

/* sysfs interface for debug */
struct slave_attr {
    struct attribute attr;
    uint32_t value;
};
 
static struct slave_attr period_sz_attr = {
    .attr.name = "period_sz",
    .attr.mode = 0644,            /* read(), write() */
    .value     = PERIOD_SZ,        
};
 
static struct attribute *slave_attrs[] = {
    &period_sz_attr.attr,
    NULL
};

/* utility function for sysfs interface... used to write and store */
static ssize_t slave_attr_show(struct kobject *kobj,\
                                 struct attribute *attr, char *buf)
{
    struct slave_attr *slave_attr = container_of(attr, \
                                                 struct slave_attr, attr);
 
    return  scnprintf(buf, PAGE_SIZE, "%u\n", slave_attr->value);
}
 
static ssize_t slave_attr_store(struct kobject *kobj, struct attribute *attr,
                                                  const char *buf, size_t len)
{
    struct slave_attr *slave_attr = container_of(attr, \
                                                  struct slave_attr, attr);
    sscanf(buf, "%u", &slave_attr->value);
    return sizeof(uint32_t);
}
 
static struct sysfs_ops slave_sysfs_ops = {
     .show = slave_attr_show,
     .store = slave_attr_store,
};

/* sysfs interface utilities and files */ 
struct kobj_type slave_kobj_type = {
     .sysfs_ops = &slave_sysfs_ops,
     .default_attrs = slave_attrs,
};
EXPORT_SYMBOL(slave_kobj_type);

/* get user set speriod size from sysfs interface */
uint32_t get_period_sz(void)
{
    return  period_sz_attr.value;
}
EXPORT_SYMBOL(get_period_sz);
