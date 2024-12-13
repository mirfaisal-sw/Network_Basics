#ifndef __MEMDEV_H__
#define __MEMDEV_H__

#define MEMDEV_MAJOR 260

#define MEMDEV_NR_DEVS 2
#define MEMDEV_SIZE 4096

struct mem_dev {
        char* data;
        unsigned long size;
};

#endif
