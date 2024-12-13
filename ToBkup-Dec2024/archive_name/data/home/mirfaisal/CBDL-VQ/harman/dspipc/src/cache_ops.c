/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file cache_ops.c
 *
 * This file contains functions for cache handling. There are different
 * implementations available. Before QNX6.3.0 a special handling was
 * required, since QNX6.3.0 a cache library is part of the OS.
 *
 * @author Bruno Achauer
 * @date   31.08.2006
 *
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

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
#include <linux/unistd.h>

#include "cache_ops.h"

#if defined(__SH4__)

#define CACHE_LINESIZE 32

/**
 * Cache initialization. This function initialize the cache library to be
 * functional for the following calls. This has to be done once inside the
 * programm. In has to be initialized before the first call to any chipset
 * layer.
 *
 * @return int Status value (cacheline size, -1= error, errno is set)
 */
int hbCacheInit(void)
{
        return CACHE_LINESIZE;
}

/**
 * Cache finalization. This function close the cache library. This has to
 * be done once before the end of the programm.
 *
 * @return int Status value (0=OK, -1= error, errno is set)
 */
int hbCacheFini(void)
{
        return 0;
}

/**
 * Cache flush function. This function flushes the specified portion of
 * the memory to the memory. The physical and virtual addresses are needed
 * to be portable across different processor platforms (supports virtually
 * and physically tagged caches)
 *
 * @param vaddr Virtual address of the memory region
 * @param paddr Physical address of the memory region
 * @param len Size of the region in bytes
 * @return void
 */
void hbCacheFlush(void* vaddr, uint64_t paddr, size_t len)
{
        unsigned linesize = CACHE_LINESIZE;
        unsigned __dst = (unsigned)vaddr & ~(linesize - 1);
        int __nlines = (((unsigned)vaddr + len + linesize - 1) - __dst) / linesize;

        while (__nlines) {
                __asm__ __volatile__("ocbwb @%0;"
                                     :
                                     : "r"(__dst));
                __dst += linesize;
                __nlines--;
        }
}

/**
 * Cache invalidate function. This function invalidates the specified portion
 * of the memory to the memory. The physical and virtual addresses are needed
 * to be portable across different processor platforms (supports virtually
 * and physically tagged caches)
 *
 * @param vaddr Virtual address of the memory region
 * @param paddr Physical address of the memory region
 * @param len Size of the region in bytes
 * @return void
 */
void hbCacheInval(void* vaddr, uint64_t paddr, size_t len)
{
        unsigned linesize = CACHE_LINESIZE;
        unsigned __dst = (unsigned)vaddr & ~(linesize - 1);
        int __nlines = (((unsigned)vaddr + len + linesize - 1) - __dst) / linesize;

        while (__nlines) {
                __asm__ __volatile__("ocbi @%0;"
                                     :
                                     : "r"(__dst));
                __dst += linesize;
                __nlines--;
        }
}

#elif defined(__X86__)

#define CACHE_LINESIZE 0

/**
 * Cache initialization. This function initialize the cache library to be
 * functional for the following calls. This has to be done once inside the
 * programm. In has to be initialized before the first call to any chipset
 * layer.
 *
 * @return int Status value (cahceline size, -1= error, errno is set)
 */
int hbCacheInit(void)
{
        return CACHE_LINESIZE;
}

/**
 * Cache finalization. This function close the cache library. This has to
 * be done once before the end of the programm.
 *
 * @return int Status value (0=OK, -1= error, errno is set)
 */
int hbCacheFini(void)
{
        return 0;
}

/**
 * Cache flush function. This function flushes the specified portion of
 * the memory to the memory. The physical and virtual addresses are needed
 * to be portable across different processor platforms (supports virtually
 * and physically tagged caches)
 *
 * @param vaddr Virtual address of the memory region
 * @param paddr Physical address of the memory region
 * @param len Size of the region in bytes
 * @return void
 */
void hbCacheFlush(void* vaddr, uint64_t paddr, size_t len)
{
}

/**
 * Cache invalidate function. This function invalidates the specified portion
 * of the memory to the memory. The physical and virtual addresses are needed
 * to be portable across different processor platforms (supports virtually
 * and physically tagged caches)
 *
 * @param vaddr Virtual address of the memory region
 * @param paddr Physical address of the memory region
 * @param len Size of the region in bytes
 * @return void
 */
void hbCacheInval(void* vaddr, uint64_t paddr, size_t len)
{
}

#elif defined(__ARM__) | defined(ARM)

#include <asm/hwcap.h>
#include <asm/page.h>
#include <asm/pgtable-hwdef.h>
#include <asm/pgtable.h>
#include <asm/ptrace.h>
#include <linux/init.h>
#include <linux/linkage.h>

#define CACHE_LINESIZE 32

/**
 * Cache initialization. This function initialize the cache library to be
 * functional for the following calls. This has to be done once inside the
 * programm. In has to be initialized before the first call to any chipset
 * layer.
 *
 * @return int Status value (cacheline size, -1= error, errno is set)
 */
int hbCacheInit(void)
{

        return CACHE_LINESIZE;
}

/**
 * Cache finalization. This function close the cache library. This has to
 * be done once before the end of the programm.
 *
 * @return int Status value (0=OK, -1= error, errno is set)
 */
int hbCacheFini(void)
{

        return 0;
}

/**
 * Cache flush function. This function flushes the specified portion of
 * the memory to the memory. The physical and virtual addresses are needed
 * to be portable across different processor platforms (supports virtually
 * and physically tagged caches)
 *
 * @param vaddr Virtual address of the memory region
 * @param paddr Physical address of the memory region
 * @param len Size of the region in bytes
 * @return void
 */
void hbCacheFlush(void* vaddr, uint64_t paddr, size_t len)
{
        unsigned linesize = CACHE_LINESIZE;
        unsigned __dst = (unsigned)vaddr & ~(linesize - 1);
        int __nlines = (((unsigned)vaddr + len + linesize - 1) - __dst) / linesize;

        while (__nlines) {
                __asm__ __volatile__("MCR p15, 0, %0, c7, c10, 1"
                                     :
                                     : "r"(__dst));
                __dst += linesize;
                __nlines--;
        }
}

/**
 * Cache invalidate function. This function invalidates the specified portion
 * of the memory to the memory. The physical and virtual addresses are needed
 * to be portable across different processor platforms (supports virtually
 * and physically tagged caches)
 *
 * @param vaddr Virtual address of the memory region
 * @param paddr Physical address of the memory region
 * @param len Size of the region in bytes
 * @return void
 */
void hbCacheInval(void* vaddr, uint64_t paddr, size_t len)
{
        unsigned linesize = CACHE_LINESIZE;
        unsigned __dst = (unsigned)vaddr & ~(linesize - 1);
        int __nlines = (((unsigned)vaddr + len + linesize - 1) - __dst) / linesize;

        while (__nlines) {
                __asm__ __volatile__("MCR p15, 0, %0, c7, c6, 1"
                                     :
                                     : "r"(__dst));
                __dst += linesize;
                __nlines--;
        }
}

#else
#error CPU FAMILY NOT SUPPORTED INSIDE CACHE LIB
#endif
