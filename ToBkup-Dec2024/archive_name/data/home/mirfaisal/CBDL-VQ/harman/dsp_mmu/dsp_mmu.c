/*****************************************************************************
 * Description     : Trace driver to output fault address of DSP MMU,
 *                   also adds TLB entry for the fault address -
 *                   to unlock and allow DSP to run its exception handler.
 *
 * Design Goals    : Should be usable across multiple chipsets (j5, j6..)
 *
 * Author(s)       : Padmanabha S <padmanabha.srinivasaiah@harman.com>,
 *                   Arora, Saurabh <Saurabh.Arora@harman.com>
 *
 * Copyright (c) 2017 Harman International Industries [connected car division]
 *****************************************************************************/

#define DEBUG

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/irqdomain.h>

#include <asm/io.h>        /* for ioremap() */

#include "dsp_mmu.h"

/* MMU Events bits in MMU_IRQSTATUS */
#define MMU_MULTIHITFAULT     0x10
#define MMU_TABLEWALKFAULT    0x08
#define MMU_EMUMISS           0x04
#define MMU_TRANSLATIONFAULT  0x02
#define MMU_TLBMISS           0x01

#define MMU_IRQ_TST_EVENTS    ( MMU_TRANSLATIONFAULT |  MMU_TLBMISS )


/* Register offset in IP, same across J5Eco/J6 */
#define MMU_REVISION          0x00
#define MMU_IRQSTATUS         0x18
#define MMU_IRQENABLE         0x1C
#define MMU_FAULT_AD          0x48
#define MMU_LOCK              0x50
#define MMU_LD_TLB            0x54
#define MMU_CAM               0x58
#define MMU_RAM               0x5C


#ifdef CONFIG_IUKRAM_HIGH /*For J6*/

/*DSP1 */
#define DSP1_MMU0_DEV        "dsp1-mmu0"
#define DSP1_MMU0_BASE        0x40d01000
#define DSP1_MMU0_SIZE        256
#define DSP1_FAULT_TLB        21 
#define UNUSED_MEMORY_DSP1    0x88990000

/*DSP2 */
#define DSP2_MMU0_DEV        "dsp2-mmu0"
#define DSP2_MMU0_BASE        0x41501000
#define DSP2_MMU0_SIZE        256
#define DSP2_FAULT_TLB        22
#define UNUSED_MEMORY_DSP2    0x88991000

#define UNUSED_MEMORY_SZ      4096

#define DSP1_MMU0_HW_IRQ      23
#define DSP2_MMU0_HW_IRQ      146

#define OMAP_REV_J6	      0x07500000		      

#else  /*For J5Eco*/

/*J5Eco has only one DSP */
#define DSP_MMU0_DEV          "dsp1-mmu0"
#define DSP_MMU0_BASE         0x48010000
#define DSP_MMU0_SIZE         0x1000
#define DSP_FAULT_TLB         19
#define UNUSED_MEMORY_DSP     0x87FD0000
#define DSP_SYSMMU_HW_IRQ     122

/*Mapping the macros for code reuse, DSP1->J6 to DSP->J5Eco */
#define DSP1_MMU0_DEV         DSP_MMU0_DEV
#define DSP1_MMU0_BASE        DSP_MMU0_BASE
#define DSP1_MMU0_SIZE        DSP_MMU0_SIZE
#define DSP1_FAULT_TLB        DSP_FAULT_TLB
#define UNUSED_MEMORY_DSP1    UNUSED_MEMORY_DSP
#define UNUSED_MEMORY_SZ      (64 * 1024) /*64K*/

#define DSP1_MMU0_HW_IRQ      DSP_SYSMMU_HW_IRQ

/* DSP2 IP not available on J5Eco */
#define DSP2_MMU0_DEV         NULL
#define DSP2_MMU0_BASE        0
#define DSP2_MMU0_SIZE        0
#define DSP2_FAULT_TLB        0
#define UNUSED_MEMORY_DSP2    0

#define DSP2_MMU0_HW_IRQ      0
#endif

typedef struct dsp_mmu {
    void __iomem *base;
    void __iomem *shm_mem;

    phys_addr_t phy_base;
    phys_addr_t fault_page;    /* reserved to map page */
    
    int  size;    /* cfg space size */

    u32  hwirq;    /* hardware irq wire number */
    u32  virq;    /* linux IRQ number */

    u32  fault_tlb_entry;    /* tlb entry to be used during fault */
    mmu_tlbentry_t fault_tlb_dummy;

    const char *name;
} dsp_mmu_t;

static dsp_mmu_t dsp_mmu, dsp2_mmu;
static unsigned int omap_curr_rev;

/* Hardware IRQ number */
static uint16_t dsp_mmu_irq  = DSP1_MMU0_HW_IRQ;
static uint16_t dsp2_mmu_irq = DSP2_MMU0_HW_IRQ;

module_param(dsp2_mmu_irq, ushort, 0644);
module_param(dsp_mmu_irq, ushort, 0644);

/* 
 * Populate TLB paremters used to configure-
 * MMU_CAM and MMU_RAM to add static TLB entry
 * during DSP core fault
 */
static int mmu_fault_tlb_static_init (mmu_tlbentry_t *mmu_entry,\
                            uint32_t phy_addr, uint32_t virt_addr)
{
    pr_devel("@@@@ %s, map virt %#x to phys %#x \n",
             __func__, virt_addr, phy_addr);
 
    mmu_entry->phyaddr = phy_addr;
    mmu_entry->virtaddr = virt_addr;
    mmu_entry->valid = TRUE;
    mmu_entry->pg_sz = MMU_PAGESIZE_SMALL_4KB;
    mmu_entry->mmu_endian = MMU_LITTLE_ENDIAN;
    mmu_entry->elm_sz =  MMU_ELM_SZ_NOTRANSLATION;
    mmu_entry->tlb_elm_sz = MMU_CPU_ELM_SZ;

    mmu_entry->resv = TRUE; /* preserve TLB */

    return 0;
}

/*
 * Populate MMU_CAM and MMU_RAM and load static TLB
 */
static int mmu_tlb_set_entry(mmu_tlbentry_t *mmu_entry,
                 u32 tlb_entry,
                 void __iomem *base)
{
    int ret = 0;
    u32 val;

    val = mmu_entry->virtaddr & MMU_CAM_VATAG_MASK;
    val |= (mmu_entry->resv) << MMU_CAM_P_SHIFT;
    val |= (mmu_entry->valid) << MMU_CAM_V_SHIFT;
    val |= mmu_entry->pg_sz;

    /* write CAM register */
    writel(val, base + MMU_CAM);

    pr_devel("@@@@ %s, writen to MMU CAM %#x\n", __func__, val);

    val = mmu_entry->phyaddr & MMU_RAM_PHYADDR_MASK;
    val |= (mmu_entry->mmu_endian) << MMU_RAM_ENDIANNESS_SHIFT;
    val |= (mmu_entry->elm_sz) << MMU_RAM_ELEMENTSZ_SHIFT;
    val |= (mmu_entry->tlb_elm_sz) << MMU_RAM_MIXED_SHIFT;

    writel(val, base + MMU_RAM);

    pr_devel("@@@@ %s, writen to MMU RAM %#x\n", __func__, val);

    /* set TLB entry */
    val = __raw_readl(base + MMU_LOCK);
    val |= val | (tlb_entry << 0x4);
    writel(val, base + MMU_LOCK);

    pr_devel("@@@@ %s, writen to MMU LOCK %#x\n", __func__, val);

    /* load TLB */ 
    writel(0x1, base + MMU_LD_TLB);    
    
    return ret;
}

/*
 * Fault handler, 
 *     Acknowledges MMU TLB MISS fault,
 *     Adds indicator to DSP core to indicate TLB MISS
 *     Maps reserved page to fault address to unlock DSP
 */
static irqreturn_t dsp_mmu_trap_irq( int irq, void *data)
{
    u32 status, fault_addr;
    dsp_mmu_t *dsp_mmu = data;
    void __iomem *base = dsp_mmu->base;
    phys_addr_t map_pg = dsp_mmu->fault_page;
    mmu_tlbentry_t *tlb_entry = &(dsp_mmu->fault_tlb_dummy);

    status = __raw_readl(base + MMU_IRQSTATUS);

    if (!status)
        return IRQ_NONE;

    fault_addr = __raw_readl(base + MMU_FAULT_AD);
    printk(KERN_INFO "XXXX %s exception, FAULT ADDR %#x\n",\
                                     dsp_mmu->name, fault_addr);

    /* Acknowledge fault event */
    __raw_writel(status, base + MMU_IRQSTATUS);

    /* indicator for DSP core */
    if(dsp_mmu->shm_mem)
        writel(0xDEADBEEF, dsp_mmu->shm_mem);
 
    /* map reserved page to fault address to unlock DSP core */
    mmu_fault_tlb_static_init(tlb_entry, map_pg , fault_addr );
    mmu_tlb_set_entry(tlb_entry, dsp_mmu->fault_tlb_entry, dsp_mmu->base);

#if 0
    /* Experimental purpose disable IRQ Events, 
     * DSP have to do it after unlock (???)
     */
    __raw_writel(0, base + MMU_IRQENABLE);
#endif
    return IRQ_HANDLED;
}

/*
 * Utility function to memory map CFG space of DSPx- MMUX
 */
static int mmap_dsp_mmu(const char *name, phys_addr_t phy_base,
            int sz, u32 irq, 
            phys_addr_t fault_pg, dsp_mmu_t *obj)
{
    u32 val;
    void __iomem *addr;

    if (phy_base == 0)
        return -EINVAL;

    obj->name = name;
    obj->size = sz;
    obj->hwirq = irq;
    obj->phy_base = phy_base ;
    obj->fault_page = fault_pg;

    addr = ioremap(phy_base, sz);
    if (!addr) {
        pr_err("@@@@ %s, failed to map cfg space \n", obj->name);
        return -ENOMEM;
    }
    obj->base = addr;

    val = __raw_readl(obj->base + MMU_REVISION);

    /* shared memory to indicate TLB FAULT */
    obj->shm_mem = ioremap(fault_pg, UNUSED_MEMORY_SZ);

    return 0;
}

/* 
 * Book keep TLB entry need to be used for fault handler
 */
static void set_tlb_entry(dsp_mmu_t *obj, u32 tlb_entry)
{
    obj->fault_tlb_entry = tlb_entry;
    return;
}
    
/*
 *  Utility function to map hwirq to linux irq and
 *  register handler for same 
 */
static int map_hwirq (dsp_mmu_t *obj)
{
    u32 val;
    int ret;
    struct irq_domain *domain = NULL;

    ret = irq_find_mapping(domain, obj->hwirq);
    if(ret <= 0) {
        pr_err("@@@@ %s, unable to map hwirq %d\n",\
                    __func__, obj->hwirq);
        goto out;
    }

    obj->virq = ret;
    pr_devel("@@@@ %s, register handler to Linux IRQ %u\n",\
             obj->name, ret);

    ret = request_irq( obj->virq, dsp_mmu_trap_irq, \
                IRQF_SHARED, "dsp-mmu-trap", obj);

    if(ret < 0){
        pr_err("@@@@ %s, failed to register Linux irq %d\n",\
               __func__, obj->virq);
        goto out;
    }

    /* time to enable  MMU IRQ events... */
    val = MMU_IRQ_TST_EVENTS;
    __raw_writel(val, obj->base + MMU_IRQENABLE);

out:
    return ret;
}

/*
 *  Utility function to unmap hwirq handler
 */
static void unmap_hwirq (dsp_mmu_t *obj)
{
    u32 val;

    if(!obj->virq)
        return;

    /* disable MMU IRQ events... */
    val = 0;
    __raw_writel(val, obj->base + MMU_IRQENABLE);
    
    free_irq(obj->virq, obj);
}

/*
 * Utility function to unregister handler,
 * and unmap resource mapping for respective DSP
 */
static void dsp_mmu_unregister(dsp_mmu_t *obj)
{
    /* release DSP MMU handler,
     * and un-map shared
     */
    unmap_hwirq(obj);

    if(obj->shm_mem)
        iounmap(obj->shm_mem);

    if (obj->base)
        iounmap(obj->base);
}

/*
 * MMAP DSP-MMU cfg space and register FAULT handler
 */
static int __init dsp_mmu_trap_init(void)
{
    int ret = 0;
    extern unsigned int omap_rev(void);

    omap_curr_rev = omap_rev();

    ret = mmap_dsp_mmu(DSP1_MMU0_DEV, DSP1_MMU0_BASE,\
                       DSP1_MMU0_SIZE, dsp_mmu_irq, \
                       UNUSED_MEMORY_DSP1, &dsp_mmu);

    if (dsp_mmu.base == NULL)
        goto next;

    /* use tlb entry */
    set_tlb_entry(&dsp_mmu, DSP1_FAULT_TLB);

    /* register handler for HW IRQ by mapping into linux virtual irq */
    map_hwirq(&dsp_mmu);

next:
#ifdef CONFIG_IUKRAM_HIGH
    if ( OMAP_REV_J6 == (omap_curr_rev & 0xfff00000))
    {
        // not for DRA72x
        ret = mmap_dsp_mmu(DSP2_MMU0_DEV, DSP2_MMU0_BASE,\
                           DSP2_MMU0_SIZE, dsp2_mmu_irq,\
                           UNUSED_MEMORY_DSP2, &dsp2_mmu);

         if (dsp2_mmu.base != NULL) {
             set_tlb_entry(&dsp2_mmu, DSP2_FAULT_TLB);
             map_hwirq(&dsp2_mmu);
         } else {
             pr_warn("%@@@@ %s, failed with %s\n", __func__, DSP2_MMU0_DEV);
             /* As earlier one, with DSP1 succedded,
              * return success so that module remove should work
              */
             if(dsp_mmu.base)
                 ret = 0;
         }
     }
#endif

    return ret;
}

/*
 * We are supporting clean removal of module too
 */
static void __exit dsp_mmu_trap_exit(void)
{
    dsp_mmu_unregister(&dsp_mmu);

#ifdef CONFIG_IUKRAM_HIGH
    if ( OMAP_REV_J6 == (omap_curr_rev & 0xfff00000))
    {
        // not for DRA72x
        dsp_mmu_unregister(&dsp2_mmu);
    }
#endif

    return;
}

module_init(dsp_mmu_trap_init);
module_exit(dsp_mmu_trap_exit);

MODULE_DESCRIPTION("Trace driver for DSP MMU");
MODULE_VERSION("0.5");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Padmanabha S");
