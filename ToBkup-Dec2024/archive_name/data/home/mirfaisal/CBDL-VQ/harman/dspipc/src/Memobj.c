/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/

/**
 * @file Memobj.c
 * 
 * This is the implementation for the shared memory object functionality of the
 * dev-dspipc ressource handler.

 * 
 * @author Xiujing Luo, yong zheng
 * @date   20.Nov.2012
 * 
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */
#include <asm/io.h>
#include <asm/mman.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/version.h>

#include "debug.h"
#include "dev-dspipc.h"
#include "dspipc.h"
#include "dspipc_pthread.h"
#include "dspipcproto.h"
#include "malloc_trc.h"
#include "misc.h"
#include "string_def_ext.h"

/*============================================================================== */
/*===================    Modul Local Variables   ============================== */
/**
 * Character array with the 64 characters to build the shared memory segment
 * name
 */
static const char nameCharacters[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz-_";

/**
 * Pointer to the root element of the memory object table linked list
 */
static memObjList_t* pMemObjListRoot = NULL;

/**
*share memory management description
*/
static ShareMemDesc_t stShareMemDesc;

/**
 * Define base max number of elements in the hold list
 */
/*static int MAX_SHM_OBJ_HOLD = 13; */

/**
 * Pointer to the root element of the memory object table linked list
 * for elements that have benn released
 */
static memObjList_t* pMemObjListRoot_HOLD = NULL;

/**
 * Counter for num ber of mem object in hold list
 */
static int num_mem_obj_hold = 0;

static pthread_mutex_t mutexUsageCount; /* mutex to control access to usage counter */

extern uint32_t num_ShmElem_to_alloc;

static uint32_t startup_size_array_dra300[14] = {
        32768, /* 0 */
        32768, /* 1 */
        49152, /* 2 */
        49152, /* 3 */
        32768, /* 4 */
        32768, /* 5  */
        49152, /* 6 */
        49152, /* 7 ------------ */
        49152, /* 8 */
        49152, /* 9 */
        49152, /* 10 */
        49152, /* 11 */
        49152, /* 12 */
        49152
}; /* 13 */

static uint32_t startup_size_array_dm642[14] = { 49152, /* 0 */
        49152,                                          /* 1 */
        49152,                                          /* 2 */
        49152,                                          /* 3 ------------ */
        49152,                                          /* 4 */
        49152,                                          /* 5  */
        49152,                                          /* 6 */
        49152,                                          /* 7 */
        49152,                                          /* 8 */
        49152,                                          /* 9 */
        49152,                                          /* 10 */
        49152,                                          /* 11 */
        49152,                                          /* 12 */
        49152 };                                        /* 13 */

#define MEM_DEVICE_NAME "ipc/dsp/sharemem"

static int mem_open(struct inode* inode, struct file* file);
static int mem_release(struct inode* inode, struct file* filp);
static int mem_mmap(struct file* fd, struct vm_area_struct* vma);

static struct file_operations mem_fops = {
        .owner = THIS_MODULE,
        .open = mem_open,
        .mmap = mem_mmap,
        .release = mem_release,
};

static struct miscdevice mem_miscdev = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = MEM_DEVICE_NAME,
        .fops = &mem_fops,
};

#define ARM_SHM_CTRL

/*============================================================================== */
/*===================    Function Definitions    =============================== */
static int mem_open(struct inode* inode, struct file* file)
{
        printk("[dev-dspipc]:%s:called!\n", __FUNCTION__);
        return 0;
}

static int mem_release(struct inode* inode, struct file* filp)
{
        printk("[dev-dspipc]:%s:called!\n", __FUNCTION__);
        return 0;
}

static int mem_mmap(struct file* fd, struct vm_area_struct* vma)
{
        unsigned long size;
        unsigned long page;
        unsigned long phy_addr = shareMemPhyAddr;

        printk("[dev-dspipc]:%s:called!\n", __FUNCTION__);

        if (phy_addr == 0) {
                printk("[dev-dspipc]:%s:error, stream shared memory is NULL\n", __FUNCTION__);
                return -1;
        } else {
                size = (unsigned long)(vma->vm_end - vma->vm_start);
                page = (phy_addr >> PAGE_SHIFT) + vma->vm_pgoff;
                vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

                printk("%s vm_start :0x%lx ,vm_pgoff:0x%lx, vm_page_prot:0x%x\n", __FUNCTION__, vma->vm_start, vma->vm_pgoff,
                    vma->vm_page_prot);

                if (remap_pfn_range(vma, vma->vm_start, page, size, vma->vm_page_prot))
                        return -1;

                return 0;
        }
}

/**
 *  This function initializes the shared memory pool structures
 *
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int InitMemoryObjects(void)
{
        int retValue = -1;
        memObjList_t* pMemObj;

        if (pMemObjListRoot != NULL) {
                hb_slogf_error("%s: Already initialized", __FUNCTION__);
                return (-1);
        }

        /* Init mutex */
        if (pthread_mutex_init(&mutexUsageCount, NULL) != EOK) {
                hb_slogf_error("%s: init mutex failed", __FUNCTION__);
        }

        DEBUG_INFO("%s: Begin initialized\n", __FUNCTION__);

        /* Req. first element of List */
        pMemObj = CALLOC_TRC(1, sizeof(memObjList_t));
        if (pMemObj != NULL) {
                pMemObj->memName[0] = '\0';
                pMemObj->memId = 0;
                pMemObj->fd = -1;
                pMemObj->usageCount = 0;
                pMemObj->pVirtAddr = NULL;
                pMemObj->physAddr = 0;
                pMemObj->pNext = NULL;
                pMemObj->size = 0;
                pMemObjListRoot = pMemObj;
        } else {
                hb_slogf_error("%s: No mem for root element", __FUNCTION__);
        }

        /* Req. first element of Hold-List */
        pMemObj = CALLOC_TRC(1, sizeof(memObjList_t));
        if (pMemObj != NULL) {
                pMemObj->memName[0] = '\0';
                pMemObj->memId = 0;
                pMemObj->fd = -1;
                pMemObj->usageCount = 0;
                pMemObj->pVirtAddr = NULL;
                pMemObj->physAddr = 0;
                pMemObj->pNext = NULL;
                pMemObj->size = 0;
                pMemObjListRoot_HOLD = pMemObj;
                retValue = 0;
        } else {
                hb_slogf_error("%s: No mem for root element", __FUNCTION__);
        }

        /*register miscdevice use for memory map  */
        retValue = misc_register(&mem_miscdev);
        if (retValue != 0) {
                hb_slogf_error("%s: misc_register error:%d", __FUNCTION__, retValue);
        }

        return (retValue);
}

/**
 *  This function search a list for memory element with the given size and the given memory flags
 *
 *  @param pListRoot Pointer to the list root element
 *  @param memSize   Size of the memory area to compare to
 *  @param mmapFlags Flags of memory object  to compare to
 *  @return memObjList_t* Pointer to the memory object if found, 
 *                        NULL ifno element was not found
 */
static memObjList_t* SearchMemObjSizeFlags(const memObjList_t* pListRoot, uint32_t memSize, int mmapFlags)
{
        memObjList_t* pMemSearchObj;
        int found;
        memObjList_t* pFoundMemObj = NULL;

        if (pListRoot == NULL) {
                hb_slogf_error("%s: No root element", __FUNCTION__);
                return (NULL);
        }

        pMemSearchObj = pListRoot->pNext;
        found = 0;
        if (pMemSearchObj != NULL) {
                do {
                        if ((pMemSearchObj->size >= memSize) && (pMemSearchObj->protFlags == mmapFlags)) {
                                pFoundMemObj = pMemSearchObj;
                                found = 1; /* size is equal inside of the elements */
                        }
                        /* got to next */
                        pMemSearchObj = pMemSearchObj->pNext;
                } while ((found == 0) && (pMemSearchObj != NULL));
        }

        return (pFoundMemObj);
}

/**
 *  This function add an element to a list
 *
 *  @param pListRoot  Pointer to the list root element
 *  @param pAddMemObj Pointer to the memory object structure to add
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
static int AddElementToList(memObjList_t* pListRoot, memObjList_t* pAddMemObj)
{
        int retValue = -1;
        memObjList_t* pLastMemObj;

        if (pListRoot == NULL) {
                hb_slogf_error("%s: No root element", __FUNCTION__);
                return (-1);
        }
        if (pAddMemObj == NULL) {
                hb_slogf_error("%s: No element to add", __FUNCTION__);
                return (-1);
        }

        /*find last element */
        pLastMemObj = pListRoot;
        while (pLastMemObj->pNext != NULL)
                pLastMemObj = pLastMemObj->pNext;

        /* link new element into the list */
        pAddMemObj->pNext = NULL;
        pLastMemObj->pNext = pAddMemObj;
        retValue = 0;

        return (retValue);
}

/**
 *  This function removes element from a list
 *
 *  @param pListRoot  Pointer to the list root element
 *  @param pDelMemObj Pointer to the memory object structure to remove
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
static int RemoveElementFromList(memObjList_t* pListRoot, memObjList_t* pDelMemObj)
{
        int retValue = -1;
        memObjList_t* pLastMemObj;
        memObjList_t* pCurrMemObj;

        if (pListRoot == NULL) {
                hb_slogf_error("%s: No root element", __FUNCTION__);
                return (-1);
        }
        if (pDelMemObj == NULL) {
                hb_slogf_error("%s: No element to delete", __FUNCTION__);
                return (-1);
        }

        /*find related element */
        pLastMemObj = pListRoot;
        pCurrMemObj = pLastMemObj->pNext;
        while (pCurrMemObj != NULL) {
                if (pCurrMemObj->memId == pDelMemObj->memId) {
                        /* remove element out of the chain */
                        pLastMemObj->pNext = pCurrMemObj->pNext;
                        retValue = 0;
                        break;
                }
                pLastMemObj = pCurrMemObj;
                pCurrMemObj = pCurrMemObj->pNext;
        }

        pDelMemObj->pNext = NULL;

        return (retValue);
}

int RequestMemoryElement(dspipc_reqMemObj_t* pReqMemObj, memObjList_t** ppNewElement, uint32_t req_new)
{
        int retValue = 0;
        memObjList_t* pNewMemObj;
        void __iomem* pVirtAddr = NULL;
        off_t physAddr; /* mem_offset64 */
        uint32_t numMemPages;
        uint64_t shmCtlSize;
#ifndef ARM_SHM_CTRL
        uint64_t shmAddr;
#endif
        int errCode;

        if (pReqMemObj == NULL) {
                hb_slogf_error("%s: No request structure", __FUNCTION__);
                return (-1);
        }
        if (ppNewElement == NULL) {
                hb_slogf_error("%s: No pointer for new mem object to store", __FUNCTION__);
                return (-1);
        }

        /* Only one Thread should use this function to access these structures */
        errCode = pthread_mutex_lock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: Mutex lock failed <%s>", __FUNCTION__, strerror(errCode));
        }

        numMemPages = ((pReqMemObj->size - 1) / (uint32_t)PAGE_SIZE) + 1;
        shmCtlSize = (uint64_t)numMemPages * PAGE_SIZE;

        /* Search in HOLD list for element with size that is requested */
        if ((req_new == 0) && ((pNewMemObj = SearchMemObjSizeFlags(pMemObjListRoot_HOLD, (uint32_t)shmCtlSize, pReqMemObj->mmapFlags)) != NULL)) {
                /* Remove element from HOLD list  */
                if (RemoveElementFromList(pMemObjListRoot_HOLD, pNewMemObj) != 0) {
                        hb_slogf_error("%s: rem element from main list failed", __FUNCTION__);
                }
                num_mem_obj_hold--;
                /* Add element to main list */
                if (AddElementToList(pMemObjListRoot, pNewMemObj) != 0) {
                        hb_slogf_error("%s: add element to main list failed", __FUNCTION__);
                }

                /* fill request structure with results */
                strncpy(pReqMemObj->memName, &(pNewMemObj->memName[0]), MEM_NAME_LENGTH);
                pReqMemObj->memId = pNewMemObj->memId;

                /*memory allocation policy can gurantee the physics address no lower than the start physics address */
                pReqMemObj->addroffset = (pNewMemObj->physAddr - stShareMemDesc.phyAddrStart);

                if (ppNewElement != NULL) {
                        *ppNewElement = pNewMemObj;
                }

                hb_slogf_error("####%s: request mem id:%d, name:%s, offset:0x%x, Physics:0x%x",
                    __FUNCTION__, pReqMemObj->memId, pNewMemObj->memName, pReqMemObj->addroffset, pNewMemObj->physAddr);
                memset((void __force*)pNewMemObj->pVirtAddr, 0, (size_t)pNewMemObj->size);
                retValue = 0;
        } else /* generate new element */
        {

                hb_slogf_error("%s: begin create memobj, addroffset:0x%x ", __FUNCTION__, stShareMemDesc.useoffset);

                /* ignore already specified name (maybe could be changed later on) */
                pReqMemObj->memName[0] = '\0';
                memset(&pReqMemObj->memName[0], 0, MEM_NAME_LENGTH - 1);

                if (pMemObjListRoot == NULL) {
                        hb_slogf_error("%s: No root element", __FUNCTION__);

                        errCode = pthread_mutex_unlock(&(mutexUsageCount));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Mutex unlock failed <%s>", __FUNCTION__, strerror(errCode));
                        }
                        return (-1);
                }

                if (stShareMemDesc.useoffset + shmCtlSize > SHARE_MEM_LEN) {
                        hb_slogf_error("%s: Not enough share memory to request", __FUNCTION__);

                        errCode = pthread_mutex_unlock(&(mutexUsageCount));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Mutex unlock failed <%s>", __FUNCTION__, strerror(errCode));
                        }
                        return (-1);
                }

                pVirtAddr = stShareMemDesc.pvirtAddrStart + stShareMemDesc.useoffset;
                physAddr = stShareMemDesc.phyAddrStart + stShareMemDesc.useoffset;

                /* create new element */
                pNewMemObj = CALLOC_TRC(1, sizeof(memObjList_t));
                if (pNewMemObj != NULL) {
                        strncpy(pNewMemObj->memName, "/dev/ipc/dsp/sharemem", MEM_NAME_LENGTH);
                        pNewMemObj->fd = (int)&mem_fops;
                        pNewMemObj->usageCount = 0;
                        pNewMemObj->size = (uint32_t)shmCtlSize;
                        pNewMemObj->pVirtAddr = pVirtAddr;
                        pNewMemObj->physAddr = physAddr;
                        pNewMemObj->memId = nextMemId++;
                        pNewMemObj->protFlags = pReqMemObj->mmapFlags;
                        pNewMemObj->pNext = NULL;

                        if (AddElementToList(pMemObjListRoot, pNewMemObj) != 0) {
                                hb_slogf_error("%s: add element to main list failed", __FUNCTION__);
                                retValue = -23;
                        } else {
                                /* fill request structure with results */
                                strncpy(pReqMemObj->memName, &(pNewMemObj->memName[0]), MEM_NAME_LENGTH);
                                pReqMemObj->memId = pNewMemObj->memId;
                                pReqMemObj->addroffset = stShareMemDesc.useoffset;

                                hb_slogf_error("################%s: request mem name:%s", __FUNCTION__, pReqMemObj->memName);

                                if (ppNewElement != NULL) {
                                        *ppNewElement = pNewMemObj;
                                }

#ifdef VVVV_ENABLE
                                if (gCmdOptions.optv > 3) {
                                        hb_slogf_error("%s: created shmem object <%s> id %d for size %d(%d) (%d cache pages, size 0x%08X%08X) phys adr 0x%08X virt adr 0x%08X",
                                            __FUNCTION__,
                                            pNewMemObj->memName, pNewMemObj->memId, (size_t)pReqMemObj->size, pNewMemObj->size,
                                            numMemPages, (uint32_t)((shmCtlSize >> 32) & 0xFFFFFFFF), (uint32_t)(shmCtlSize & 0xFFFFFFFF),
                                            pNewMemObj->physAddr, pNewMemObj->pVirtAddr);
                                }
#endif
                                retValue = 0;
                        }
                        stShareMemDesc.useoffset += shmCtlSize;
                } else {
                        retValue = -1;
                        hb_slogf_error("%s: No mem for root element", __FUNCTION__);
                }

                if (retValue != 0) {
                        if (pNewMemObj != NULL) {
                                FREE_TRC(pNewMemObj);
                                pNewMemObj = NULL;
                        }

                        hb_slogf_error("%s: fail", __FUNCTION__);
                }
        } /* end generate new element */

        errCode = pthread_mutex_unlock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: Mutex unlock failed <%s>", __FUNCTION__, strerror(errCode));
        }

        return (retValue);
}

int InitBaseObjects(void)
{
        int retValue = -1;
        dspipc_reqMemObj_t ReqMemObj;
        memObjList_t* pMemObjElement;
        uint32_t i;
        uint32_t start_array_sel = 0;
        int delayRemain;

        DEBUG_INFO("%s: Begin initialized base\n ", __FUNCTION__);

        if (nextMemId > 8000) /* Check first MemiD => says which Chipset is started  (DM642 starts with 8001) */
        {
                /* DM642 o.  */
                start_array_sel = 1;
                num_ShmElem_to_alloc = 4;
        } else {
                /* DRA300 */
                start_array_sel = 0;
                num_ShmElem_to_alloc = 6; /* default value */
        }

        ReqMemObj.mmapFlags = PROT_READ | PROT_WRITE; /* | PROT_NOCACHE;  Allocate always Cached Memory */

        memset(&stShareMemDesc, 0, sizeof(ShareMemDesc_t));

        stShareMemDesc.phyAddrStart = shareMemPhyAddr;
        if (shareMemPhySize == 0) {
                hb_slogf_error("%s: [error] size of stream shared memory should not be zero", __FUNCTION__);
                return -1;
        }
        /*map share memory to kernel virture address */
        stShareMemDesc.pvirtAddrStart = ioremap_nocache(stShareMemDesc.phyAddrStart, shareMemPhySize);
        if (stShareMemDesc.pvirtAddrStart == NULL) {
                hb_slogf_error("%s: map share memory failed", __FUNCTION__);
                return -1;
        }

        hb_slogf_error("%s: sharemem phyaddr:0x%x, virtaddr:0x%x,size:0x%x", __FUNCTION__,
            stShareMemDesc.phyAddrStart, (uint32_t __force)stShareMemDesc.pvirtAddrStart, shareMemPhySize);

        for (i = 0; i < num_ShmElem_to_alloc; i++) {
                /* This Delay prevents this function to fail */
                /* Give the Kernel time to restucture the Memory Information (PR39898) */
                delayRemain = 4;
                /*
      do
      {
         delayRemain = mdelay( delayRemain );
      }while( delayRemain != 0);
      */
                mdelay(delayRemain);

                if (start_array_sel == 0) /* DRA300 */
                {
                        ReqMemObj.size = startup_size_array_dra300[i];
                } else /* DM642 and others */
                {
                        ReqMemObj.size = startup_size_array_dm642[i];
                }

                strncpy(ReqMemObj.memName, "/dummy", MEM_NAME_LENGTH); /* should be ignored */

                if ((retValue = RequestMemoryElement(&ReqMemObj, &pMemObjElement, 1)) == 0) {
                        /* Remove from main list */
                        if (RemoveElementFromList(pMemObjListRoot, pMemObjElement) != 0) {
                                hb_slogf_error("%s: remove element from main list at startup failed", __FUNCTION__);
                        } else {
                                if (AddElementToList(pMemObjListRoot_HOLD, pMemObjElement) != 0) {
                                        hb_slogf_error("%s: add element to main list failed", __FUNCTION__);
                                } else {
                                        num_mem_obj_hold++;
                                }
                        }
                } else {
                        hb_slogf_error("%s: error allocationg memory element at startup (%d)", __FUNCTION__, i);
                        retValue = -1;
                        break;
                }
        } /*end for */

        hb_slogf_error("%s:  request  num:%d memobj success", __FUNCTION__, i);

        return (retValue);
}

/**
 *  This function removes one memory object from the pool and puts it to the hold List if there is
 *  enough place in the Hold List.
 *
 *  @param pDelMemObj Pointer to the memory object delete structure
 *  @param checkUseCnt 0:Pointer to the memory object delete structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int RemoveMemoryElement_hold(const dspipc_delMemObj_t* pDelMemObj, int checkUseCnt)
{
        int retValue = -1;
        memObjList_t* pLastMemObj;
        memObjList_t* pCurrMemObj;
        memObjList_t* pFoundMemObj;
        int errCode;
#ifdef USE_LOCAL_NAME
        char name[MEM_NAME_LENGTH];
#endif

        if (pDelMemObj == NULL) {
                hb_slogf_error("%s: No delete structure", __FUNCTION__);
                return (-1);
        }
        if (pMemObjListRoot == NULL) {
                hb_slogf_error("%s: No root element", __FUNCTION__);
                return (-1);
        }

        /* Only one Thread should use this function to access these structures */
        errCode = pthread_mutex_lock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
                return -11;
        }
#ifdef USE_LOCAL_NAME
        memset(&name[0], NULL, MEM_NAME_LENGTH - 1);
#endif

        /*find related element */
        pLastMemObj = pMemObjListRoot;
        pCurrMemObj = pLastMemObj->pNext;
        pFoundMemObj = NULL;
        while (pCurrMemObj != NULL) {
                if (pCurrMemObj->memId == pDelMemObj->memId) {
                        pFoundMemObj = pCurrMemObj;
                        break;
                }
                pLastMemObj = pCurrMemObj;
                pCurrMemObj = pCurrMemObj->pNext;
        }

        /* Check if object can be put to hold list */
        /*if( num_mem_obj_hold < MAX_SHM_OBJ_HOLD ) */
        {
                if (pFoundMemObj != NULL) {
                        if (pCurrMemObj != NULL) {
                                if (((pFoundMemObj->usageCount == 0) && (checkUseCnt == 1)) || (checkUseCnt == 0)) {
                                        /* Remove from main list */
                                        if (RemoveElementFromList(pMemObjListRoot, pFoundMemObj) != 0) {
                                                hb_slogf_error("%s: rem element from main list failed", __FUNCTION__);
                                        }
                                        /* Add to HOLD list */
                                        if (AddElementToList(pMemObjListRoot_HOLD, pFoundMemObj) != 0) {
                                                hb_slogf_error("%s: add element to main list failed", __FUNCTION__);
                                        }
                                        num_mem_obj_hold++;
                                        retValue = 0;
#ifdef USE_DSP_SHM_PRINTOUTS
                                        hb_slogf_error("%s: Rem object from Maim-List, put to Hold-List: object <%s> id %d for size %d phys adr 0x%08X virt adr 0x%08X",
                                            __FUNCTION__,
                                            pFoundMemObj->memName, pFoundMemObj->memId, (size_t)pFoundMemObj->size,
                                            pFoundMemObj->physAddr, pFoundMemObj->pVirtAddr);
#endif
                                } else {
                                        hb_slogf_error(TRY_REM_MEMOBJ_WHILE_IN_USE, __FUNCTION__, pFoundMemObj->usageCount, pFoundMemObj->memId);
                                }
                        }
                } else {
                        hb_slogf_error("%s: Mem object not found", __FUNCTION__);
                }
        }

        errCode = pthread_mutex_unlock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
                return -12;
        }

        return (retValue);
}

/**
 *  This function removes one memory object from the pool.
 * 
 *  @param pListRoot pointer the the list root
 *  @param pDelMemObj Pointer to the memory object delete structure
 *  @param checkUseCnt 0:Pointer to the memory object delete structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
static int RemoveMemoryElement(memObjList_t* pListRoot, const dspipc_delMemObj_t* pDelMemObj, int checkUseCnt)
{
        int retValue = -1;
        memObjList_t* pLastMemObj;
        memObjList_t* pCurrMemObj;
        memObjList_t* pFoundMemObj;
        int errCode;

        if (pDelMemObj == NULL) {
                hb_slogf_error("%s: No delete structure", __FUNCTION__);
                return (-1);
        }
        if (pListRoot == NULL) {
                hb_slogf_error("%s: No root element", __FUNCTION__);
                return (-1);
        }

        /* Only one Thread should use this function to access these structures */
        errCode = pthread_mutex_lock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
        }

        /*find related element */
        pLastMemObj = pListRoot;
        pCurrMemObj = pLastMemObj->pNext;
        pFoundMemObj = NULL;
        while (pCurrMemObj != NULL) {
                if (pCurrMemObj->memId == pDelMemObj->memId) {
                        pFoundMemObj = pCurrMemObj;
                        break;
                }
                pLastMemObj = pCurrMemObj;
                pCurrMemObj = pCurrMemObj->pNext;
        }

        if (pFoundMemObj != NULL) {
                if (pCurrMemObj != NULL) {
                        if (((pFoundMemObj->usageCount == 0) && (checkUseCnt == 1)) || (checkUseCnt == 0)) {
                                retValue = 0;
                                /* remove element out of the chain */
                                pLastMemObj->pNext = pCurrMemObj->pNext;

                                /* release memory */
                                FREE_TRC(pCurrMemObj);
                        } else {
                                hb_slogf_error(TRY_REM_MEMOBJ_WHILE_IN_USE, __FUNCTION__, pFoundMemObj->usageCount, pFoundMemObj->memId);
                        }
                } else {
                        hb_slogf_error("%s: Mem object root element not initialized", __FUNCTION__);
                }
        } else {
                hb_slogf_error("%s: Mem object not found", __FUNCTION__);
        }

        errCode = pthread_mutex_unlock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
        }

        return (retValue);
}

/**
 *  This function search the memory object from the pool with the related memId.
 *
 *  @param memId Memory id of the element to be searched for
 *  @return memObjList_t* Pointer to the element structure if found, NULL if an 
 *                        the element was not found
 */
memObjList_t* SearchMemoryElement(uint32_t memId)
{
        memObjList_t* pMemObj;

        /* Only one Thread should use this function to access these structures */
        int errCode;

        if (pMemObjListRoot == NULL) {
                hb_slogf_error("%s: No root element", __FUNCTION__);
                return (NULL);
        }

        errCode = pthread_mutex_lock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
        }

        /*find related element */
        pMemObj = pMemObjListRoot->pNext;
        while (pMemObj != NULL) {
                if (pMemObj->memId == memId) {
                        break;
                }
                pMemObj = pMemObj->pNext;
        }

        if (pMemObj == NULL) {
                hb_slogf_error("%s: Element %d not found", __FUNCTION__, memId);
        }

        errCode = pthread_mutex_unlock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
        }

        return (pMemObj);
}

/**
 *  This function search the memory id of the object for which either the 
 *  virtual or physical address matches the calling parameters. If the search 
 *  type is set to PHYS than addr will be compared with against the physical
 *  addresses. If VIRT is specified, than addr will be compared against the
 *  virtual addresses. 
 *
 *  @param pMemObj Pointer to an element to check the range, NULL = scan all
 *                 elements and start at root element
 *  @param memSearchType Specify type of memory address (MEMOBJ_SEARCH_PHYS | 
 *                       MEMOBJ_SEARCH_VIRT)
 *  @param memAddr Address for which the related memory segment has to be searched
 *  @param memSize Size of the memory area (for consistency check)
 *  @param pMemId Pointer to variable to store the found memory id
 *  @param pMemOffset Pointer to stotre the found memory offset value
 *  @return memObjList_t* Pointer to the memory object if found, 
 *                        NULL ifno element was not found
 */
memObjList_t* SearchMemId(memObjList_t* pMemObj, memSearchType_t memSearchType,
    uint32_t memAddr, uint32_t memSize,
    uint32_t* pMemId, uint32_t* pMemOffset)
{
        memObjList_t* pMemSearchObj;
        memObjList_t* pMemNextSearchObj;
        uint32_t memId;
        uint32_t memOffset;
        uint32_t memObjAddr;
        int found;

        /* Only one Thread should use this function to access these structures */
        int errCode;
        errCode = pthread_mutex_lock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
        }

        if (pMemObjListRoot == NULL) {
                hb_slogf_error("%s: No root element", __FUNCTION__);

                errCode = pthread_mutex_unlock(&(mutexUsageCount));
                if (errCode != EOK) {
                        hb_slogf_error("%s: Mutex unlock failed <%s>", __FUNCTION__, strerror(errCode));
                }
                return (NULL);
        }

        memId = memOffset = memObjAddr = 0;
        pMemNextSearchObj = pMemObjListRoot->pNext;
        found = 0;
        if (pMemObj != NULL) {
                /* if a special element is specified, just verify in this element */
                pMemNextSearchObj = pMemObj;
                found = 1;
        }

        hb_slogf_error("%s: root list physics:0x%x, size:0x%x", __FUNCTION__, pMemNextSearchObj->physAddr, pMemNextSearchObj->size);

        do {
                pMemSearchObj = pMemNextSearchObj;
                /*find related element */
                switch (memSearchType) {
                case MEMOBJ_SEARCH_PHYS:
                        memObjAddr = pMemSearchObj->physAddr;
                        break;
                case MEMOBJ_SEARCH_VIRT:
                        memObjAddr = (uint32_t __force)pMemSearchObj->pVirtAddr;
                        break;
                default:
                        hb_slogf_error("%s: Unknown search type", __FUNCTION__);
                        break;
                }

                if ((memAddr >= memObjAddr) && (memAddr < (memObjAddr + pMemSearchObj->size))) {
                        memOffset = memAddr - pMemSearchObj->physAddr;
                        if (memSize <= pMemSearchObj->size - memOffset) {
                                memId = pMemSearchObj->memId;
                        } else {
                                hb_slogf_error("%s: Size exceeds memory segement", __FUNCTION__);
                        }
                        found = 1; /* address found inside one of the elements */
                }
                pMemNextSearchObj = pMemSearchObj->pNext;
        } while ((found == 0) && (pMemNextSearchObj != NULL));

        if (memId != 0) {
                if (pMemId != NULL)
                        *pMemId = memId;
                if (pMemOffset != NULL)
                        *pMemOffset = memOffset;
        } else {
                pMemSearchObj = NULL; /* set returnValue to NULL for error reporting */
        }

        errCode = pthread_mutex_unlock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
        }

        return (pMemSearchObj);
}

/**
 *  This function increments the usage count of the object
 * 
 *  @param pMemObj Pointer to the element for changing usage count
 *  @param count Number of usages to be added 
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int IncUsageMemoryElement(memObjList_t* pMemObj, uint32_t count)
{
        int retValue = -1;
        int errCode;

        errCode = pthread_mutex_lock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
        }

        if (pMemObj != NULL) {
                pMemObj->usageCount += count;
                retValue = 0;
        }

#ifdef VVVV_ENABLE
        if (gCmdOptions.optv > 2) {
                if (pMemObj != NULL) {
                        hb_slogf_error("%s: memID:%d Count:%d", __FUNCTION__, pMemObj->memId, pMemObj->usageCount);
                }
        }
#endif

        errCode = pthread_mutex_unlock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
        }

        return (retValue);
}

/**
 *  This function decrements the usage count of the object
 * 
 *  @param pMemObj Pointer to the element for changing usage count
 *  @param count Number of usages to be added 
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int DecUsageMemoryElement(memObjList_t* pMemObj, uint32_t count)
{
        int retValue = -1;
        int errCode;

        errCode = pthread_mutex_lock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
        }

        if (pMemObj != NULL) {
                if (pMemObj->usageCount >= count) {
                        pMemObj->usageCount -= count;
                } else {
                        pMemObj->usageCount = 0;
                }
                retValue = 0;
        }

#ifdef VVVV_ENABLE
        if (gCmdOptions.optv > 2) {
                if (pMemObj != NULL) {
                        hb_slogf_error("%s: memID:%d Count:%d", __FUNCTION__, pMemObj->memId, pMemObj->usageCount);
                }
        }
#endif

        errCode = pthread_mutex_unlock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
        }

        return (retValue);
}

/**
 *  This function resets the usage counter of all memory elements in actual memory list
 * 
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int ResetMemoryElementCounter(void)
{
        int retValue = 0;
        int errCode;
        memObjList_t* pMemObj;

        errCode = pthread_mutex_lock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
                retValue = -1;
        }

        if (pMemObjListRoot != NULL) {
                pMemObj = pMemObjListRoot->pNext; /* Zeige auf erstes Element */
                while (pMemObj != NULL)           /* Wenn Element noch vorhanden */
                {
                        pMemObj->usageCount = 0;
                        /* go to next */
                        pMemObj = pMemObj->pNext;
                }
        }

        errCode = pthread_mutex_unlock(&(mutexUsageCount));
        if (errCode != EOK) {
                hb_slogf_error("%s: %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
                retValue = -1;
        }

        return (retValue);
}

/**
 *  This function Deinitializes the shared memory pool structures
 *
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int DeinitMemoryObjects(void)
{
        int retValue = -1;
        dspipc_delMemObj_t DelMemObj;

        if (pMemObjListRoot != NULL) {
                while (pMemObjListRoot->pNext != NULL) /* Wenn Element noch vorhanden */
                {
                        DelMemObj.memId = pMemObjListRoot->pNext->memId;
                        if (RemoveMemoryElement(pMemObjListRoot, &DelMemObj, 0) != 0) {
                                hb_slogf_error("%s: Failed to remove element id=%d", __FUNCTION__, DelMemObj.memId);
                        }
                }

                FREE_TRC(pMemObjListRoot);
                pMemObjListRoot = NULL;
                retValue = 0;
        }

        /* Remove all Elements in HOLD List */
        if (pMemObjListRoot_HOLD != NULL) {
                while (pMemObjListRoot_HOLD->pNext != NULL) /* Wenn Element noch vorhanden */
                {
                        DelMemObj.memId = pMemObjListRoot_HOLD->pNext->memId;
                        if (RemoveMemoryElement(pMemObjListRoot_HOLD, &DelMemObj, 0) != 0) {
                                hb_slogf_error("%s: Failed to remove element id=%d", __FUNCTION__, DelMemObj.memId);
                        }
                }
                FREE_TRC(pMemObjListRoot_HOLD);
                pMemObjListRoot_HOLD = NULL;

                retValue = 0;
        }

        /* Destroy Usage-mutex */
        if (pthread_mutex_destroy(&mutexUsageCount) != EOK) {
                hb_slogf_error("%s: destroy mutex failed", __FUNCTION__);
        }

        /* unmap memory area */
        if (stShareMemDesc.pvirtAddrStart) {
                iounmap(stShareMemDesc.pvirtAddrStart);
                stShareMemDesc.pvirtAddrStart = NULL;
        }

        /*de-register misc dev */
        misc_deregister(&mem_miscdev);

        return (retValue);
}

/*===========================   End Of File   ================================= */
