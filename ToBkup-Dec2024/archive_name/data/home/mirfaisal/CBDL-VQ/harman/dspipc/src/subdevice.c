/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file subdevice.c
 *
 * This is the file containing the functions to handle the subdevices inside
 * the dev-dspipc. Subdevices are the names of the ctrl, command and stream
 * ports. This module contains all functions to create, add, search and delete
 * for these structures inside the device handle structure.
 *
 *
 * @author Andreas Heiner
 * @author Andreas Abel
 * @date   03.03.2006
 *
 * @modifier Huihua Yang
 * @date   17.OCT.2012
 *
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

/*============================================================================== */
/*=========================    Includes   ====================================== */

#include <asm/fcntl.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/wait.h>

#include "dev-dspipc.h" /* optv, struct DEVICETABLE_S */
#include "dspipcproto.h"
#include "malloc_trc.h" /* Trace malloc */
#include "string_def_ext.h"

#include "debug.h"
#include "dsp/dspipclib.h" /* Channel Prio definition */
#include "misc.h"

/*============================================================================== */
/*=========================   Define's   ======================================= */

/* none */

/*============================================================================== */
/*=========================    Macro's   ======================================= */
#define __IN_OUT_TRACE /* if defined then for every function IN and OUT generates a TRACE-Log. */

#define __TRACE_ON

#ifdef TRACE_ON
#include <sys/trace.h> /*for TraceEvent() */
#define TRACEEVENT(a, b, c) (void)TraceEvent(a, b, c);
#else
#define TRACEEVENT(a, b, c)
#endif

/* none */

/*============================================================================== */
/*===================    Modul Global Variables   ============================== */

/* none */

/*============================================================================== */
/*===================    forward declarations    =============================== */

static void CleanupSubDevice(subDeviceTable_t* pSubDevice);
static int PutBufferCompleteToQueue(const subDeviceTable_t* pSubDevice,
    const subDeviceBufferQueue_t* pQueue,
    subDeviceDir_t direction,
    uint32_t flags);
static int RemoveFromBuffers(const subDeviceTable_t* pSubDevice,
    subDeviceBufferQueue_t* pQueue,
    uint32_t len);

static int dspipcAddSubDeviceToList(deviceTable_t* pDevice,
    subDeviceTable_t* pNewSubDevice);

static int dspipcRemSubDeviceFromList(deviceTable_t* pDevice,
    const subDeviceTable_t* pSubDevice);

static void dspipcUpdateDeviceStruct(deviceTable_t* pDevice);

static int dspipcCleanTransProvidQueue(subDeviceTable_t* pSubDevice);
/*============================================================================== */
/*=======================    Global Variables    =============================== */
/* yanghuihua delete. to be continue
extern iofunc_mount_t mountpoint;
*/

/*============================================================================== */
/*===================    Function Definitions    =============================== */

/**
 * dspipcUpdateDeviceStruct updates the device structure (startpoints of all priorities) and sets
 * all startpoints of the subdevice priorities and sets back the LastSendSubdevice pointers.
 *
 * @param pDevice Pointer to the device table structure to add the new subdevice
 * @return int Error code, returns 0 on success, -1 in case of an error
 */
static void dspipcUpdateDeviceStruct(deviceTable_t* pDevice)
{
        /*lint -esym(613,pDevice) pDevice is never NULL, ensured by calling function */
        subDeviceTable_t* pCurrSubDevice = NULL;
        int found_0 = 0;
        int found_1 = 0;
        int found_2 = 0;
        int found_3 = 0;
        int found_4 = 0;

        pDevice->pSubDevices_0 = NULL;
        pDevice->pSubDevices_1 = NULL;
        pDevice->pSubDevices_2 = NULL;
        pDevice->pSubDevices_3 = NULL;
        pDevice->pSubDevices_4 = NULL;

        /* Update of Pointer to Subdevice Prio Groops */
        /* Check if the prio of the new Subdevice is available in List */
        pCurrSubDevice = pDevice->pSubDevices; /* Get Root Subdevice */
        while (pCurrSubDevice != NULL) {
                if (pCurrSubDevice->channelPrio == HOSTIPC_CHANNELPRIORITY_ADMIN) {
                        if (found_0 == 0) {
                                found_0 = 1;
                                pDevice->pSubDevices_0 = pCurrSubDevice;
                        }
                } else if (pCurrSubDevice->channelPrio == HOSTIPC_CHANNELPRIORITY_REALTIMECOMMAND) {
                        if (found_1 == 0) {
                                found_1 = 1;
                                pDevice->pSubDevices_1 = pCurrSubDevice;
                        }
                } else if (pCurrSubDevice->channelPrio == HOSTIPC_CHANNELPRIORITY_REALTIMESTREAM) {
                        if (found_2 == 0) {
                                found_2 = 1;
                                pDevice->pSubDevices_2 = pCurrSubDevice;
                        }
                } else if (pCurrSubDevice->channelPrio == HOSTIPC_CHANNELPRIORITY_NORMAL) {
                        if (found_3 == 0) {
                                found_3 = 1;
                                pDevice->pSubDevices_3 = pCurrSubDevice;
                        }
                } else /* HOSTIPC_CHANNELPRIORITY_IDLE */
                {
                        if (found_4 == 0) {
                                found_4 = 1;
                                pDevice->pSubDevices_4 = pCurrSubDevice;
                        }
                }
                pCurrSubDevice = pCurrSubDevice->pNext;
        }
        /* set back all LastSendSubdevice */
        pDevice->pLastSendSubDevice_0 = NULL;
        pDevice->pLastSendSubDevice_1 = NULL;
        pDevice->pLastSendSubDevice_2 = NULL;
        pDevice->pLastSendSubDevice_3 = NULL;
        pDevice->pLastSendSubDevice_4 = NULL;
}

/**
 * dspipcChangeSubDevicePrio cganes the priority of the channels that is given
 * via the channel number
 * 
 *
 * @param pDevice Pointer to the device table structure to add the new subdevice
 * @param channelNum channel number to set the new prio
 * @param newPrio  new priority
 * @return int 0: no error, else Error rem. Subdevice from list
 */
int dspipcChangeSubDevicePrio(deviceTable_t* pDevice,
    uint32_t channelNum,
    int newPrio)
{
        int retval = 0;
        subDeviceTable_t* pSubDeviceIn;
        subDeviceTable_t* pSubDeviceOut;

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        if ((newPrio < HOSTIPC_CHANNELPRIORITY_IDLE) || (newPrio > HOSTIPC_CHANNELPRIORITY_REALTIMECOMMAND)) {
                hb_slogf_error("%s: invalid prio for change of subdevice <%d> ", __FUNCTION__, newPrio);
                return (-1);
        }

        if (dspipcLockSubDevices(pDevice) == EOK) {
                /*search corresponding IN stream device */
                pSubDeviceIn = dspipcSearchSubDevice(pDevice, SD_DIR_IN, channelNum);
                if (pSubDeviceIn != NULL) {
                        if ((retval = dspipcRemSubDeviceFromList(pDevice, pSubDeviceIn)) != 0) {
                                hb_slogf_error("%s: error removeing subdevice from list <%d> ", __FUNCTION__, retval);
                                retval = -1;
                        }
                        /* set new Priority */
                        pSubDeviceIn->channelPrio = newPrio;

                        /*put back to subdevice chain          */
                        if ((retval = dspipcAddSubDeviceToList(pDevice, pSubDeviceIn)) != 0) {
                                hb_slogf_error("%s: error removeing subdevice from list <%d> ", __FUNCTION__, retval);
                                retval = -1;
                        }
                } else {
                        hb_slogf_error("%s: No corresponding IN device found", __FUNCTION__);
                        retval = -1;
                }

                if (retval == 0) {
                        /*search corresponding OUT stream device */
                        pSubDeviceOut = dspipcSearchSubDevice(pDevice, SD_DIR_OUT, channelNum);
                        if (pSubDeviceOut != NULL) {
                                if ((retval = dspipcRemSubDeviceFromList(pDevice, pSubDeviceOut)) != 0) {
                                        hb_slogf_error("%s: error removeing subdevice from list <%d> ", __FUNCTION__, retval);
                                        retval = -1;
                                }
                                /* set new Priority */
                                pSubDeviceOut->channelPrio = newPrio;

                                /*put back to subdevice chain          */
                                if ((retval = dspipcAddSubDeviceToList(pDevice, pSubDeviceOut)) != 0) {
                                        hb_slogf_error("%s: error removeing subdevice from list <%d> ", __FUNCTION__, retval);
                                        retval = -1;
                                }
                        } else {
                                hb_slogf_error("%s: No corresponding OUT device found", __FUNCTION__);
                                retval = -1;
                        }
                }

                /* Update device struct for the complet device */
                dspipcUpdateDeviceStruct(pDevice);

                if (dspipcUnlockSubDevices(pDevice) != EOK) {
                        hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                }
        } else {
                hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
        }

        return retval;
}

/**
 * dspipcRemSubDeviceFromList removes a subdevice from the subdevice list in the specified device structure.
 * Before this functio is called, the subdevices must belocked
 *
 * @param pDevice Pointer to the device table structure to add the new subdevice
 * @param pSubDevice Pointer to Subdevice to remove
 * @return int 0: no error, else Error rem. Subdevice from list
 */
static int dspipcRemSubDeviceFromList(deviceTable_t* pDevice,
    const subDeviceTable_t* pSubDevice)
{

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }
        if (pSubDevice == NULL) {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                return (-1);
        }

        /* remove device out of the chain */
        if (pSubDevice->pPrev != NULL) {
                /* element is not first element */
                pSubDevice->pPrev->pNext = pSubDevice->pNext;
                if (pSubDevice->pNext != NULL) {
                        /* element is in the middle of the list */
                        pSubDevice->pNext->pPrev = pSubDevice->pPrev;
                }
        } else {
                /* element is first in the list */
                pDevice->pSubDevices = pSubDevice->pNext;
                if (pSubDevice->pNext != NULL) {
                        /* there are additional elements available */
                        /* clear previous pointer */
                        pSubDevice->pNext->pPrev = NULL;
                }
        }

        dspipcUpdateDeviceStruct(pDevice);

        return 0;
}

/**
 * dspipcAddSubDeviceToList adds a new subdevice into the specified device structure.
 * Before this function is called, the subdevices must be locked.
 *
 * @param pDevice Pointer to the device table structure to add the new subdevice
 * @param pNewSubDevice Pointer to new Subdevice to add
 * @return int Error code, returns 0 on success, -1 in case of an error
 */
static int dspipcAddSubDeviceToList(deviceTable_t* pDevice,
    subDeviceTable_t* pNewSubDevice)
{
        subDeviceTable_t* pLastSubDevice = NULL;
        subDeviceTable_t* pCurrSubDevice = NULL;
        int retval = -1;
        int prioAvail = 0; /* 0: Prio not available in list  1: Prio already available in List */

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        if (pNewSubDevice == NULL) {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                return (-1);
        }

        /* Check if the prio of the new Subdevice is available in List */
        pCurrSubDevice = pDevice->pSubDevices; /* Get Root Subdevice */
        while (pCurrSubDevice != NULL) {
                if (pNewSubDevice->channelPrio == pCurrSubDevice->channelPrio) {
                        prioAvail = 1;
                        break;
                }
                pCurrSubDevice = pCurrSubDevice->pNext;
        }

        pCurrSubDevice = pDevice->pSubDevices; /* Get Root Subdevice */
        pLastSubDevice = NULL;
        if (pCurrSubDevice == NULL) {
                /* new subdevice = first subdevice */
                pDevice->pSubDevices = pNewSubDevice;
                retval = 0;
        } else {
                if (prioAvail == 1) {
                        while (pCurrSubDevice != NULL) {
                                /* Check if Prio is equal (insert device before current) */
                                if (pNewSubDevice->channelPrio == pCurrSubDevice->channelPrio) /* Prio equal than actual first element */
                                {
                                        break;
                                }
                                pLastSubDevice = pCurrSubDevice;
                                pCurrSubDevice = pCurrSubDevice->pNext;
                        }
                } else {
                        while (pCurrSubDevice != NULL) {
                                /* Check if Prio is higher (insert device before current) */
                                if (pNewSubDevice->channelPrio > pCurrSubDevice->channelPrio) /* Prio higher than actual first element */
                                {
                                        break;
                                }
                                pLastSubDevice = pCurrSubDevice;
                                pCurrSubDevice = pCurrSubDevice->pNext;
                        }
                }

                /* Insert New Subdevice before Current */
                if ((pLastSubDevice == NULL)) /* Current is first in the List */
                {
                        if (pCurrSubDevice != NULL)
                        {
                                /* Insert element before the first element */
                                pNewSubDevice->pNext = pCurrSubDevice;
                                pCurrSubDevice->pPrev = pNewSubDevice;

                                pDevice->pSubDevices = pNewSubDevice;
                                pNewSubDevice->pPrev = NULL;
                                retval = 0;
                        } else {
                                retval = -2;
                        }
                } else {
                        if ((pCurrSubDevice != NULL)) /* Last & Curr  != NULL */
                        {
                                /* add new element between Last and Curr */
                                pNewSubDevice->pNext = pCurrSubDevice;
                                pCurrSubDevice->pPrev = pNewSubDevice;

                                pLastSubDevice->pNext = pNewSubDevice;
                                pNewSubDevice->pPrev = pLastSubDevice;
                                retval = 0;
                        } else /* Last != NULL  &  Curr== NULL  Prio is smaller then all and not avail in List */
                        {
                                /* Add  */
                                pNewSubDevice->pNext = NULL;
                                pLastSubDevice->pNext = pNewSubDevice;
                                pNewSubDevice->pPrev = pLastSubDevice;
                                retval = 0;
                        }
                }
        } /* end insert element in list that is not empty */

        dspipcUpdateDeviceStruct(pDevice);

        return retval;
}

static char* dsp_get_devnode(struct device* dev, umode_t* mode)
{
        return kasprintf(GFP_KERNEL, "ipc/dsp/%s", dev_name(dev));
}
static char* m3_get_devnode(struct device* dev, umode_t* mode)
{
        return kasprintf(GFP_KERNEL, "ipc/m3/%s", dev_name(dev));
}

/**
 * dspipcAddSubDevice adds a new subdevice into the specified device structure.
 * A new attribute structure with the specified access rights is created and
 * linked into the linked sub device list.
 *
 * @param pDevice Pointer to the device table structure to add the new sub device
 * @param subAccessMode Access mode of the added sub device
 * @param pExtra Pointer to variable chipset structure
 * @param type The type of this sub device
 * @param dir The communication direction of this sub device
 * @param channelNum Number of the channel to be used inside the name
 * @param prio priority of the channel to add
 * @return subDeviceTable_t* Pointer to the new element, NULL for failure
 */
subDeviceTable_t* dspipcAddSubDevice(deviceTable_t* pDevice,
    mode_t subAccessMode, void* pExtra,
    subDeviceType_t type, subDeviceDir_t dir,
    uint32_t channelNum, unsigned int prio)
{
        subDeviceTable_t* pCurrSubDevice = NULL;
        subDeviceTable_t* pNewSubDevice = NULL;
        int subNameMaxLen;
        int errCode;
        char* pSubName = NULL;
        int error;
        char cDir;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcAddSubDevice - IN");
#endif

        if (pDevice != NULL) {
                /* check for correct prio */
                if (prio > HOSTIPC_CHANNELPRIORITY_ADMIN) {
                        hb_slogf_error("%s: invalid priority <%d>", __FUNCTION__, prio);
                        return NULL;
                }
                subNameMaxLen = 20;
                pSubName = (char*)MALLOC_TRC(subNameMaxLen);
                if (pSubName != NULL) {
                        switch (dir) {
                        case SD_DIR_IN:
                                cDir = 'i';
                                break;
                        case SD_DIR_OUT:
                                cDir = 'o';
                                break;
                        case SD_DIR_NODIR:
                                cDir = '\0';
                                break;
                        default:
                                cDir = 'x';
                                break;
                        }
                        switch (type) {
                        case SD_CONTROL:
                                snprintf(pSubName, subNameMaxLen, "ctrl");
                                channelNum = 0;
                                break;
                        case SD_COMMAND:
                                snprintf(pSubName, subNameMaxLen, "command_%08X%c", channelNum, cDir);
                                break;
                        case SD_STREAM:
                                snprintf(pSubName, subNameMaxLen, "stream_%08X%c", channelNum, cDir);
                                break;
                        default:
                                snprintf(pSubName, subNameMaxLen, "Unknown");
                                channelNum = 0;
                                break;
                        }

                        error = 0;

                        if (dspipcLockSubDevices(pDevice) == EOK) {
                                pCurrSubDevice = pDevice->pSubDevices;
                                /*search for end of list and check if device is already existing */
                                while (pCurrSubDevice != NULL) {
                                        if ((pCurrSubDevice->channelNum == channelNum) && (pCurrSubDevice->dir == dir)) {
                                                switch (pCurrSubDevice->status) {
                                                case SD_STAT_CLOSED:
                                                case SD_STAT_DELETED:
                                                        break;
                                                case SD_STAT_INIT:
                                                case SD_STAT_READY:
                                                case SD_STAT_BLOCKED:
                                                        /*                        hb_slogf_error("%s: device <%s> already exists", __FUNCTION__, pSubName ); */
                                                        /*                        error = 1; */
                                                        /*                        break; */
                                                case SD_STAT_REBOOT:
                                                        /* In this version it is acceptable to find Subdevices (in Reboot) */
                                                        FREE_TRC(pSubName);

                                                        pCurrSubDevice->status = SD_STAT_READY; /* switch back to ready */
                                                        pCurrSubDevice->drainComplete = 0;
                                                        dspipcUpdateDeviceStruct(pDevice);

                                                        if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                                                hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                                        }
                                                        return pCurrSubDevice;
                                                default:
                                                        hb_slogf_error("%s: device <%s> has unknown state", __FUNCTION__, pSubName);
                                                        error = 1;
                                                        break;
                                                }
                                        }
                                        pCurrSubDevice = pCurrSubDevice->pNext;
                                }

                                if (error == 0) {
                                        pNewSubDevice = (subDeviceTable_t*)MALLOC_TRC(sizeof(subDeviceTable_t));
                                        if (pNewSubDevice != NULL) {
                                                pNewSubDevice->pDevice = pDevice;
                                                pNewSubDevice->pSubDeviceName = pSubName;
                                                pNewSubDevice->pExtra = pExtra;
                                                pNewSubDevice->dir = dir;
                                                pNewSubDevice->type = type;
                                                pNewSubDevice->status = SD_STAT_READY;
                                                pNewSubDevice->numOpenFd = 0;
                                                pNewSubDevice->removed = 0;
                                                pNewSubDevice->channelNum = channelNum;
                                                pNewSubDevice->xonoffState = XON_STATE;

                                                pNewSubDevice->channelPrio = prio;

                                                pNewSubDevice->pPrev = NULL;
                                                pNewSubDevice->pNext = NULL;
                                                pNewSubDevice->pDeviceAttr = NULL;
                                                pNewSubDevice->pMsg.generic = (void*)MALLOC_TRC(MESSAGE_POINTER_U_SIZE);
                                                if (pNewSubDevice->pMsg.generic == NULL) {
                                                        hb_slogf_error("%s: Failed to malloc msg buf, len:%d", __FUNCTION__, MESSAGE_POINTER_U_SIZE);
                                                        error = 1;
                                                }

                                                pNewSubDevice->minor_num = pDevice->cur_minor_num++;
                                                cdev_init((struct cdev*)pNewSubDevice, dspipc_getfops());
                                                pNewSubDevice->chrdev.owner = THIS_MODULE;
                                                errCode = cdev_add((struct cdev*)pNewSubDevice, MKDEV(pDevice->major_num, pNewSubDevice->minor_num), 1);
                                                if (errCode == 0) {
                                                        if (strcmp(pDevice->pDeviceName, "dsp") == 0) {

                                                                pDevice->driverclass->devnode = dsp_get_devnode;

                                                        } else if (strcmp(pDevice->pDeviceName, "m3") == 0) {

                                                                pDevice->driverclass->devnode = m3_get_devnode;
                                                        }

                                                        if (NULL == device_create(pDevice->driverclass, NULL, MKDEV(pDevice->major_num, pNewSubDevice->minor_num), NULL, pNewSubDevice->pSubDeviceName)) {
                                                                hb_slogf_error("%s: device_create failed ",
                                                                    __FUNCTION__);
                                                                error = 1;
                                                        }

                                                        pNewSubDevice->drainComplete = 0;
                                                        errCode = pthread_mutex_init(&(pNewSubDevice->drainCompCondMutex), NULL);
                                                        if (errCode != EOK) {
                                                                hb_slogf_error("%s: init of drain complete mutex failed",
                                                                    __FUNCTION__);
                                                                error = 1;
                                                        }
                                                        errCode = pthread_cond_init(&(pNewSubDevice->drainCompCondVar), NULL);
                                                        if (errCode != EOK) {
                                                                hb_slogf_error("%s: init of drain complete condvar failed",
                                                                    __FUNCTION__);
                                                                error = 1;
                                                        }
                                                        errCode = pthread_mutex_init(&(pNewSubDevice->thisMutex), NULL);
                                                        if (errCode != EOK) {
                                                                hb_slogf_error("%s: init of sub device mutex failed",
                                                                    __FUNCTION__);
                                                                error = 1;
                                                        }

                                                        /* create message queue */
                                                        pNewSubDevice->pSendMsgQueue = NULL;
                                                        pNewSubDevice->pRecvMsgQueue = NULL;
                                                        errCode = pthread_mutex_init(&(pNewSubDevice->bufferQueueMutex), NULL);
                                                        if (errCode != EOK) {
                                                                hb_slogf_error("%s: init of queue mutex failed",
                                                                    __FUNCTION__);
                                                                error = 1;
                                                        }
                                                        pNewSubDevice->pTransferQueue = NULL;
                                                        pNewSubDevice->pProvideQueue = NULL;
                                                        if ((dir == SD_DIR_OUT) || (dir == SD_DIR_NODIR)) {
                                                                pNewSubDevice->pSendMsgQueue = dspipcQueueCreate(pDevice, DEFAULT_NUM_QUEUE_ELEMENTS);
                                                                if (pNewSubDevice->pSendMsgQueue == NULL) {
                                                                        hb_slogf_error("%s: send message queue create failed",
                                                                            __FUNCTION__);
                                                                        error = 1;
                                                                }
                                                        }
                                                        if (dir == SD_DIR_IN) {
                                                                pNewSubDevice->pRecvMsgQueue = dspipcQueueCreate(pDevice, DEFAULT_NUM_QUEUE_ELEMENTS);
                                                                if (pNewSubDevice->pRecvMsgQueue == NULL) {
                                                                        hb_slogf_error("%s: receive message queue create failed",
                                                                            __FUNCTION__);
                                                                        error = 1;
                                                                }
                                                        }

                                                        if (error == 0) {

#ifdef VVVV_ENABLE
                                                                if (gCmdOptions.optv) {
                                                                        hb_slogf_error("%s: add subdevice %s",
                                                                            __FUNCTION__,
                                                                            pNewSubDevice->pSubDeviceName);
                                                                }
#endif

                                                                /* Add New Subdevice to List */
                                                                if (dspipcAddSubDeviceToList(pDevice, pNewSubDevice) != 0) {
                                                                        error = 1;
                                                                        hb_slogf_error("%s: add new subdevuice to list failed", __FUNCTION__);
                                                                }
                                                        }
                                                } else {
                                                        error = 1;
                                                        hb_slogf_error("%s: can't register chr device!", __FUNCTION__);
                                                }
                                        } else {
                                                error = 1;
                                                hb_slogf_error("%s: no mem for subdevice", __FUNCTION__);
                                        }
                                } else {
                                        FREE_TRC(pSubName);
                                }

                                if (error == 0) {
                                        /* signal the addition of a new subdevice */
                                        errCode = dspipcSignalChangeSubDevices(pDevice, ADD_CHANNEL);
                                        if (errCode != EOK) {
                                                hb_slogf_error("%s: Signal subdevices failed", __FUNCTION__);
                                        }
                                }
                                if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                        hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                }
                        } else {
                                hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                        }
                } else {
                        error = 1;
                        hb_slogf_error("%s: No mem for subdevice name", __FUNCTION__);
                }
        } else {
                error = 1;
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
        }

        if (error != 0) {
                CleanupSubDevice(pNewSubDevice);
                pNewSubDevice = NULL;
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcAddSubDevice - OUT");
#endif
        return (pNewSubDevice);
}

/**
 * dspipcSearchSubDevice searches a subdevice inside the specified device
 * structure.
 *
 * @param pDevice Pointer to the device table structure to add the new sub device
 * @param dir The communication direction of this sub device
 * @param channelNum Number of the channel to be used inside the name
 * @return subDeviceTable_t* Pointer to the found element, NULL for failure
 */
subDeviceTable_t* dspipcSearchSubDevice(const deviceTable_t* pDevice,
    subDeviceDir_t dir,
    uint32_t channelNum)
{
        subDeviceTable_t* pSubDevice = NULL;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSearchSubDevice - IN");
#endif

        if (pDevice == NULL) {
                hb_slogf_error("%s: device invalid", __FUNCTION__);
                return (NULL);
        }

        pSubDevice = pDevice->pSubDevices;
        while (pSubDevice != NULL) {
                if ((pSubDevice->dir == dir) && (pSubDevice->channelNum == channelNum)) {
                        break;
                }
                pSubDevice = pSubDevice->pNext;
        }

        if (pSubDevice == NULL) {
                hb_slogf_error("%s: subdevice(%d) not found", __FUNCTION__, channelNum);
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSearchSubDevice - OUT");
#endif
        return (pSubDevice);
}

/**
   dspipcRemSubDevice removes a subdevice from the specified device structure.

   @param pDevice Pointer to the device table structure to add the new sub device
   @param pSubDevice Pointer to the sub device structure
   @return int Error return code, 0 for success, -1 for failure
*/
int dspipcRemSubDevice(deviceTable_t* pDevice,
    subDeviceTable_t* pSubDevice)
{
        int retValue = -1;
        /*   int       errCode; */
        char* pSubDeviceName;

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcRemSubDevice - IN");
#endif

        if (pDevice != NULL) {
                if (pSubDevice != NULL) {
                        pSubDeviceName = pSubDevice->pSubDeviceName;
                        if (pSubDeviceName == NULL) {
                                pSubDeviceName = "<NULL>";
                        }
                        switch (pSubDevice->status) {
                        case SD_STAT_BLOCKED:
                                /* unblock the device */
                                hb_slogf_error("%s: unblock subdevice %s", __FUNCTION__, pSubDevice->pSubDeviceName);
                                if (dspipcUnblockSubDevice(pSubDevice, EBADF) != 0) {
                                        hb_slogf_error("%s: unblock subdevice %s failed", __FUNCTION__, pSubDevice->pSubDeviceName);
                                        retValue = -1;
                                }
                                /*lint -fallthrough */
                        case SD_STAT_REBOOT:
                        case SD_STAT_READY:
                        case SD_STAT_DRAIN_COMPLETE:
                                retValue = 0;
                                hb_slogf_error("%s: before delete subdevice %s", __FUNCTION__, pSubDevice->pSubDeviceName);
                                if (pSubDevice->numOpenFd == 0) {
                                        hb_slogf_error("%s: delete subdevice %s", __FUNCTION__, pSubDevice->pSubDeviceName);
                                        if (dspipcDeleteSubDevice(pDevice, pSubDevice) != 0) {
                                                hb_slogf_error("%s: delete subdevice %s failed", __FUNCTION__, pSubDevice->pSubDeviceName);
                                                retValue = -1;
                                        }
                                }
                                break;
                        case SD_STAT_INIT:
                        case SD_STAT_CLOSED:
                        case SD_STAT_DELETED:
                                hb_slogf_error("%s: cannot delete subdevice %s failed <stat:%d>", __FUNCTION__, pSubDevice->pSubDeviceName, pSubDevice->status);
                                break;
                        case SD_STAT_DRAIN:
                        case SD_STAT_DRAIN_WAIT_ACK:
                                hb_slogf_error("%s: cannot delete subdevice %s during drain", __FUNCTION__, pSubDevice->pSubDeviceName);
                                break;
                        default:
                                hb_slogf_error("%s: unknown state of subdevice %s", __FUNCTION__, pSubDevice->pSubDeviceName);
                                break;
                        }

                } else {
                        hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcRemSubDevice - OUT");
#endif
        return (retValue);
}

/**
   dspipcDeleteSubDevice remove the subdevice out of the chain and releases all
   memory requested for this subdevice.

   @param pDevice Pointer to the device table structure to delete the sub device
   @param pSubDevice Pointer to the sub device structure to be deleted
   @return int Error return code, 0 for success, -1 for failure
*/
int dspipcDeleteSubDevice(deviceTable_t* pDevice,
    subDeviceTable_t* pSubDevice)
{
        int retValue = -1;
        int errCode;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcDeleteSubDevice - IN");
#endif

        if (pDevice != NULL) {
                if (pSubDevice != NULL) {
#ifdef VVVV_ENABLE
                        if (gCmdOptions.optv) {
                                hb_slogf_error("%s: remove subdevice %s",
                                    __FUNCTION__,
                                    pSubDevice->pSubDeviceName);
                        }
#endif
                        if (dspipcLockSubDevices(pDevice) == EOK) {
                                pSubDevice->status = SD_STAT_DELETED;

                                /* remove device out of the chain */
                                if (pSubDevice->pPrev != NULL) {
                                        /* element is not first element */
                                        pSubDevice->pPrev->pNext = pSubDevice->pNext;
                                        if (pSubDevice->pNext != NULL) {
                                                /* element is in the middle of the list */
                                                pSubDevice->pNext->pPrev = pSubDevice->pPrev;
                                        }
                                } else {
                                        /* element is first in the list */
                                        pDevice->pSubDevices = pSubDevice->pNext;
                                        if (pSubDevice->pNext != NULL) {
                                                /* there are additional elements available */
                                                /* clear previous pointer */
                                                pSubDevice->pNext->pPrev = NULL;
                                        }
                                }
                                dspipcUpdateDeviceStruct(pDevice);

                                /* release memory for sub device structure */
                                CleanupSubDevice(pSubDevice);

                                if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                        hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                }

                                /* signal the addition of a new subdevice */
                                errCode = dspipcSignalChangeSubDevices(pDevice, DELETE_CHANNEL);
                                if (errCode != EOK) {
                                        hb_slogf_error("%s: Signal subdevices failed (%s)", __FUNCTION__, strerror(errCode));
                                }

                                retValue = 0;
                        } else {
                                hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                        }
                } else {
                        hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcDeleteSubDevice - OUT");
#endif
        return (retValue);
}

/**
   dspipcUnblockSubDevice will specify the unblocking reason to the structure
   and will signalize a new queue information to get message. This would
   unblock the waiting for a new message and the reason is delivered to higher
   level of the softwar

   @param pSubDevice Pointer to the sub device structure
   @param reason Contains the reason for the unblocking (ETIMEDOUT | ENODEV)
   @return int Error code, returns 0 on success, -1 in case of an error
*/
int dspipcUnblockSubDevice(const subDeviceTable_t* pSubDevice, int reason)
{
        int retValue = 0;
        msgQueue_t* pQueue;
        int errCode;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcUnblockSubDevice - IN");
#endif

        if (pSubDevice == NULL) {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                return (-1);
        }

        if ((pSubDevice->type != SD_STREAM) && (pSubDevice->type != SD_COMMAND)) {
                hb_slogf_error("%s: Unblock message to non command or stream device <%s> <R:%d>", __FUNCTION__, pSubDevice->pSubDeviceName, reason);
                return (-1);
        }
        if ((pSubDevice->dir != SD_DIR_IN) && (pSubDevice->dir != SD_DIR_NODIR)) {
                /*      hb_slogf_error("%s: Unblock message to non input device <%s> [PID=%d;TID=%d]", __FUNCTION__, pSubDevice->pSubDeviceName ); */
                hb_slogf_error("%s: Unblock message to non input device <%s> <R:%d>", __FUNCTION__, pSubDevice->pSubDeviceName, reason);
                return (-1);
        }

        /* unblock the condition variable in the specified device */
        pQueue = pSubDevice->pRecvMsgQueue;
        if (pQueue == NULL) {
                hb_slogf_error("%s: No receive queue", __FUNCTION__);
                return (-1);
        }

        errCode = pthread_mutex_lock(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error("%s: Buffer %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
                retValue = -1;
        }

        pQueue->unblockReason = reason;
        errCode = pthread_cond_signal(&(pQueue->condVar));
        if (errCode != EOK) {
                hb_slogf_error("%s: Buffer cond signal failed <%s>", __FUNCTION__, strerror(errCode));
                retValue = -1;
        }

        wake_up_interruptible(&(pQueue->inWaitQueue)); /* added by yozheng */

        errCode = pthread_mutex_unlock(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error("%s: Buffer %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
                retValue = -1;
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcUnblockSubDevice - OUT");
#endif
        return (retValue);
}

/**
   dspipcSubDeviceAddBuffer will add the new element to the provide/transfer
   buffer queue of the subdevice.

   @param pSubDevice Pointer to the sub device structure
   @param pElem Pointer to a new provided buffer structure
   @param type Queue to be used (PROVIDE_BUFFER | TRANSFER_BUFFER)
   @param rescanONOFF 0:do NOT do a rescan    1:do a rescan
   @return int Error code, returns 0 on success, -1 in case of an error
*/
int dspipcSubDeviceAddBuffer(subDeviceTable_t* pSubDevice, bufferQueueType_t type, subDeviceBufferQueue_t* pElem, uint32_t rescanONOFF)
{
        int retValue = 0;
        int numElements;
        int errCode;
        unsigned int i;
        deviceTable_t* pDevice;
        subDeviceBufferQueue_t* pTemp;
        subDeviceBufferQueue_t** ppQueue;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceAddBuffer - IN");
#endif

        if (pSubDevice == NULL) {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                return (-1);
        }

        pDevice = pSubDevice->pDevice;
        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        if (pElem == NULL) {
                hb_slogf_error("%s: No element", __FUNCTION__);
                return (-1);
        }

        errCode = pthread_mutex_lock(&(pSubDevice->bufferQueueMutex));
        if (errCode == EOK) {
                ppQueue = NULL;
                switch (type) {
                case PROVIDE_BUFFER_DSP:
                case PROVIDE_BUFFER_HOST:
                        ppQueue = &(pSubDevice->pProvideQueue);
                        break;
                case TRANSFER_BUFFER_DSP:
                case TRANSFER_BUFFER_HOST:
                        ppQueue = &(pSubDevice->pTransferQueue);
                        break;
                default:
                        retValue = -1;
                        break;
                }

                if (ppQueue != NULL) {
                        pTemp = *ppQueue;

                        pElem->pNext = NULL;
                        /*copy size into remain field */
                        for (i = 0; i < pElem->numEntries; i++) {
                                pElem->entry[i].remain = pElem->entry[i].size;
                                pElem->entry[i].valid = 0;
                        }
                        retValue = 0;
                        if (pTemp != NULL) {
                                numElements = 0;
                                while (pTemp->pNext != NULL) {
                                        pTemp = pTemp->pNext;
                                        numElements++;
                                }

                                if (numElements < 128) {
                                        pTemp->pNext = pElem;
                                } else {
                                        hb_slogf_error("%s: Ch:%s More than 128 elements, queue overflow (BufferType:%d)",
                                            __FUNCTION__, pSubDevice->pSubDeviceName, type);
                                        retValue = -1;
                                }
                        } else {
                                *ppQueue = pElem;
                        }

                        if (rescanONOFF == 1) {
                                /* signal to rescan */
                                /*            hb_slogf_error("%s: call WakeupSendThread", __FUNCTION__); */
                                if (WakeupSendThread(pDevice) != 0) {
                                        hb_slogf_error("%s: WakeupSendThread failed", __FUNCTION__);
                                }
                        }
                } else {
                        hb_slogf_error("%s: Unsupported queue type", __FUNCTION__);
                        retValue = -1;
                }
                errCode = pthread_mutex_unlock(&(pSubDevice->bufferQueueMutex));
                if (errCode != EOK) {
                        hb_slogf_error(BUFFER_QUEUE_MUTEX_UNLOCK_FAILED, __FUNCTION__, strerror(errCode));
                        retValue = -1;
                }
        } else {
                hb_slogf_error(BUFFER_QUEUE_MUTEX_LOCK_FAILED, __FUNCTION__, strerror(errCode));
                retValue = -1;
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceAddBuffer - OUT");
#endif
        return (retValue);
}

/**
   dspipcSubDeviceLockBuffer locks the access to any of the buffer queues
   of a subdevice. This is required to avoid intermediate conditions in case
   of incoming interrupts which also can change the queues.

   @param pSubDevice Pointer to the sub device structure
   @return int Return code of the mutex_lock function (EOK=ok)
*/
int dspipcSubDeviceLockBuffer(subDeviceTable_t* pSubDevice)
{
        return (pthread_mutex_lock(&(pSubDevice->bufferQueueMutex)));
}

/**
   dspipcSubDeviceUnlockBuffer unlocks the access to any of the buffer queues
   of a subdevice.

   @param pSubDevice Pointer to the sub device structure
   @return int Return code of the mutex_unlock function (EOK=ok)
*/
int dspipcSubDeviceUnlockBuffer(subDeviceTable_t* pSubDevice)
{
        return (pthread_mutex_unlock(&(pSubDevice->bufferQueueMutex)));
}

/**
   dspipcSubDeviceCheckBuffer will check the transfer and provide queues and
   will deliver back the next possible transfer size. If no transfer is possible
   a len of 0 will be returned

   @param pSubDevice Pointer to the sub device structure
   @param pLen Pointer to store the maximum length of next transfer
   @param ppTransferQueue Pointer to store to transfer queue pointer  (NULL if not needed)
   @param pTransferIndex Pointer to store the current index of the first remaining
                         transfer queue entry (NULL if not needed)
   @param ppProvideQueue Pointer to store to provide queue pointer (NULL if not needed)
   @param pProvideIndex Pointer to store the current index of the first remaining
                         provide queue entry (NULL if not needed)
   @return int 0=ok; -1 = error
*/
int dspipcSubDeviceCheckBuffer(const subDeviceTable_t* pSubDevice, uint32_t* pLen,
    subDeviceBufferQueue_t** ppTransferQueue, unsigned int* pTransferIndex,
    subDeviceBufferQueue_t** ppProvideQueue, unsigned int* pProvideIndex)
{
        int retValue = 0;
        unsigned int tIndex = 0;
        unsigned int pIndex = 0;
        uint32_t tRemain;
        uint32_t pRemain;
        subDeviceBufferQueue_t* pTransferQueue;
        subDeviceBufferQueue_t* pProvideQueue;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceCheckBuffer - IN");
#endif

        if (pSubDevice != NULL) {
                if (pLen != NULL) {
                        *pLen = 0;
                        /* search for next valid element inside transfer queue with remaining transfer size */
                        pTransferQueue = pSubDevice->pTransferQueue;
                        tRemain = 0;
                        while ((pTransferQueue != NULL) && (tRemain == 0)) {
                                for (tIndex = 0; (tIndex < pTransferQueue->numEntries) && (tRemain == 0); tIndex++) {
                                        /* Check for ramaining data */
                                        if (pTransferQueue->entry[tIndex].remain != 0) {
                                                tRemain = pTransferQueue->entry[tIndex].remain;

                                                if ((tIndex + 1) < pTransferQueue->numEntries) /* Ist noch ein weiterer Range vorhanden? */
                                                {
                                                        /* ----------------------- */
                                                        /* Check if next Range is direct behind the actual one */
                                                        if ((pTransferQueue->entry[tIndex].start + pTransferQueue->entry[tIndex].size) == pTransferQueue->entry[tIndex + 1].start) {
                                                                tRemain += pTransferQueue->entry[tIndex + 1].remain;

                                                                /* ----------------------- */
                                                                /* 2. weiterer Range?? */
                                                                if ((tIndex + 2) < pTransferQueue->numEntries) /* Ist noch ein 2. weiterer Range vorhanden? */
                                                                {
                                                                        /* Check if next Range is direct behind the actual one */
                                                                        if ((pTransferQueue->entry[tIndex + 1].start + pTransferQueue->entry[tIndex + 1].size) == pTransferQueue->entry[tIndex + 2].start) {
                                                                                tRemain += pTransferQueue->entry[tIndex + 2].remain;

                                                                                /* ----------------------- */
                                                                                /* 3. weiterer Range?? */
                                                                                if ((tIndex + 3) < pTransferQueue->numEntries) /* Ist noch ein 3. weiterer Range vorhanden? */
                                                                                {
                                                                                        /* Check if next Range is direct behind the actual one */
                                                                                        if ((pTransferQueue->entry[tIndex + 2].start + pTransferQueue->entry[tIndex + 2].size) == pTransferQueue->entry[tIndex + 3].start) {
                                                                                                tRemain += pTransferQueue->entry[tIndex + 3].remain;
                                                                                        }
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                }

                                                break; /*terminate for loop without incrementing tIndex */
                                        }
                                }
                                if (tRemain == 0) {
                                        pTransferQueue = pTransferQueue->pNext;
                                }
                        }

                        /* Only check ProvideQueue if TransferQueue has remaining transfer size !!!!!! */
                        if (pTransferQueue != NULL) {
                                /* Only check ProvideQueue if TransferQueue has remaining transfer size !!!!!! */
                                /* search for next valid element inside PROVIDE queue with remaining transfer size */
                                pProvideQueue = pSubDevice->pProvideQueue;
                                pRemain = 0;
                                while ((pProvideQueue != NULL) && (pRemain == 0)) {
                                        for (pIndex = 0; (pIndex < pProvideQueue->numEntries) && (pRemain == 0); pIndex++) {
                                                if (pProvideQueue->entry[pIndex].remain != 0) {
                                                        pRemain = pProvideQueue->entry[pIndex].remain;
                                                        break; /*terminate for loop without incrementing pIndex */
                                                }
                                        }
                                        if (pRemain == 0) {
                                                pProvideQueue = pProvideQueue->pNext;
                                        }
                                }

                                if (pProvideQueue != NULL) {
                                        /* only if transfer and provide elements are available */
                                        if (pTransferIndex != NULL)
                                                *pTransferIndex = tIndex;
                                        if (pProvideIndex != NULL)
                                                *pProvideIndex = pIndex;
                                        if (ppTransferQueue != NULL)
                                                *ppTransferQueue = pTransferQueue;
                                        if (ppProvideQueue != NULL)
                                                *ppProvideQueue = pProvideQueue;

                                        if (tRemain > pProvideQueue->entry[pIndex].remain) {
                                                *pLen = pProvideQueue->entry[pIndex].remain;
                                        } else {
                                                *pLen = tRemain;
                                        }
                                }
                        }
                } else {
                        hb_slogf_error("%s: No len pointer", __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                retValue = -1;
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceCheckBuffer - OUT");
#endif
        return (retValue);
}

/**
   dspipcSubDeviceGetNextBuffer will check the transfer and provide queues and
   will deliver back the next possible transfer parameters. The related queue
   elements will be corrected (e.g. the parts will be removed) and completed
   queue elements will be freed.

   @param pSubDevice Pointer to the sub device structure
   @param direction Direction of this transfer (HOST_TO_DSP|DSP_TO_HOST)
   @param maxLen Maximum allowed size of the transfer
   @param ppHostVirt Pointer to store the virtual address of the data
   @param pHostPhys Pointer to store the virtual address of the data
   @param pTransLen Pointer to store the length of the next possible transfer.
   @param pNextAvail Pointer to store information if an additional buffer is
                     available (NULL if not required)
   @return int Error code, returns 0 on success, -1 in case of an error
*/
int dspipcSubDeviceGetNextBuffer(const subDeviceTable_t* pSubDevice,
    transferDir_t direction,
    uint32_t maxLen,
    void** ppHostVirt,
    uint32_t* pHostPhys,
    uint32_t* pTransLen,
    int* pNextAvail)
{
        int retValue = 0;
        uint32_t useLen = 0;
        unsigned int tIndex = 0;
        unsigned int pIndex = 0;
        uint32_t start;
        int nextAvail = 0;
        subDeviceBufferQueue_t* pTransferQueue;
        subDeviceBufferQueue_t* pProvideQueue;
        unsigned int i = 0;
        uint32_t remainSum = 0;
        ;

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceGetNextBuffer - IN");
#endif

        /* initialize passed variables to 0 defaults */
        if (pTransLen != NULL)
                *pTransLen = 0;
        if (ppHostVirt != NULL)
                *ppHostVirt = NULL;
        if (pHostPhys != NULL)
                *pHostPhys = 0;

        if (pSubDevice != NULL) {
                if (dspipcSubDeviceCheckBuffer(pSubDevice, &useLen,
                        &pTransferQueue, &tIndex,
                        &pProvideQueue, &pIndex)
                    == 0) {
                        if (useLen > maxLen) {
                                useLen = maxLen;
                        }
                        if (useLen != 0) {
                                if (pTransLen != NULL)
                                        *pTransLen = useLen;
                                if (direction == HOST_TO_DSP) {

                                        start = pTransferQueue->entry[tIndex].start + pTransferQueue->entry[tIndex].size - pTransferQueue->entry[tIndex].remain;

                                        /* Check if Transfer over more than 1 range */
                                        if (useLen > pTransferQueue->entry[tIndex].remain) {
                                                for (i = tIndex; i < pTransferQueue->numEntries; i++) {
                                                        remainSum += pTransferQueue->entry[i].remain;
                                                }

                                                if ((remainSum > useLen) || (pTransferQueue->pNext != NULL)) {
                                                        nextAvail = 1;
                                                }
                                        } else {
                                                if ((pTransferQueue->entry[tIndex].remain > useLen) || ((pTransferQueue->numEntries - 1) > tIndex) || (pTransferQueue->pNext != NULL)) {
                                                        nextAvail = 1;
                                                }
                                        }

                                        if (ppHostVirt != NULL) {
                                                *ppHostVirt = (void*)((char __force*)(pTransferQueue->entry[tIndex].mem.pVirtAddr) + start);
                                        }
                                        if (pHostPhys != NULL) {
                                                *pHostPhys = (uint32_t)(pTransferQueue->entry[tIndex].mem.physAddr) + start;
                                        }

                                } else /* DSP_TO_HOST */
                                {
                                        start = pProvideQueue->entry[pIndex].start + pProvideQueue->entry[pIndex].size - pProvideQueue->entry[pIndex].remain;
                                        if (ppHostVirt != NULL) {
                                                *ppHostVirt = (void*)((char __force*)(pProvideQueue->entry[pIndex].mem.pVirtAddr) + start);
                                        }
                                        if (pHostPhys != NULL) {
                                                *pHostPhys = (uint32_t)(pProvideQueue->entry[pIndex].mem.physAddr) + start;
                                        }
                                        if ((pProvideQueue->entry[pIndex].remain > useLen) || ((pProvideQueue->numEntries - 1) > pIndex) || (pProvideQueue->pNext != NULL)) {
                                                nextAvail = 1;
                                        }
                                }
                        }
                } else {
                        hb_slogf_error("%s: Check buffer failed", __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                retValue = -1;
        }
        if (pNextAvail != NULL)
                *pNextAvail = nextAvail;

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceGetNextBuffer - OUT");
#endif
        return (retValue);
}

/**
 * dspipcSubDeviceRemoveBuffer removes the specified amount of transfer bytes
 * out of the transfer and provide queues. After a transfer of data the number
 * of transfered bytes will be removed from the remain field of both queues.
 *
 * @param pSubDevice Pointer to the device table structure which holds the
 *                   transfer/provide queues
 * @param len Number of bytes which are transfered and has to removed from the
 *                   transfer/provide queues
 * @return int status 0 = OK, -1 = an error has occured
 */
int dspipcSubDeviceRemoveBuffer(const subDeviceTable_t* pSubDevice,
    uint32_t len)
{
        int retValue = 0;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceRemoveBuffer - IN");
#endif

        if (pSubDevice != NULL) {
                if (RemoveFromBuffers(pSubDevice, pSubDevice->pProvideQueue, len) != 0) {
                        hb_slogf_error("%s: Remove from provide queue failed", __FUNCTION__);
                        retValue = -1;
                }
                if (RemoveFromBuffers(pSubDevice, pSubDevice->pTransferQueue, len) != 0) {
                        hb_slogf_error("%s: Remove from transfer queue failed", __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                retValue = -1;
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceRemoveBuffer - OUT");
#endif
        return (retValue);
}

/**
 * dspipcSubDeviceCleanupQueue will cleanup the queues of the specified subdevice.
 * If not all remain fields of the current scatter/gather entry are zero,
 * nothing happens. If all these fields are zero the element will be removed
 * and the corresponding ready message is put into the queue to be moved to
 * the client.
 *
 * @param pSubDevice Pointer to the device table structure which holds the
 *                   transfer/provide queues
 * @param direction Direction of the current data flow (HOST_TO_DSP|DSP_TO_HOST)
 * @param mode Special mode to be used for this cleanup (MODE_NORMAL | MODE_DRAIN)
 * @return int status 0 = OK, -1 = an error has occured
 */
int dspipcSubDeviceCleanupQueue(subDeviceTable_t* pSubDevice,
    transferDir_t direction,
    cleanupMode_t mode)
{
        deviceTable_t* pDevice;
        int retValue = 0;
        int errCode;
        unsigned int i;
        uint32_t remain = 0;
        subDeviceBufferQueue_t* pQueue = NULL;
        subDeviceBufferQueue_t* pQueueNext = NULL;
        int completeMarker;
        int sendQueueChanged;
        uint32_t flags = 0;

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceCleanupQueue - IN");
#endif

        completeMarker = 0;
        sendQueueChanged = 0;
        if (pSubDevice != NULL) {
                pDevice = pSubDevice->pDevice;
                sendQueueChanged = 0;
                if (pDevice != NULL) {
                        if (direction == HOST_TO_DSP) {
                                pQueue = pSubDevice->pTransferQueue;
                                while (pQueue != NULL) {
                                        pQueueNext = pQueue->pNext;
                                        remain = 0;
                                        for (i = 0; i < pQueue->numEntries; i++) {
                                                remain += pQueue->entry[i].remain;
                                        }
                                        if (remain == 0) {
                                                /* element is empty */
                                                /* remember if discontinuity was set */
                                                completeMarker += pQueue->complete;
                                                /* generate message */
                                                errCode = PutBufferCompleteToQueue(pSubDevice, pQueue, SD_DIR_IN, (uint32_t)STREAMFLAG_SEND_DATA);
                                                if (errCode != 0) {
                                                        hb_slogf_error("%s: Add buffer complete (STREAMFLAG_SEND_DATA) to queue failed",
                                                            __FUNCTION__);
                                                }
                                                pSubDevice->pTransferQueue = pQueue->pNext;
                                                FREE_TRC(pQueue);
                                        } else {
                                                if (mode == MODE_DRAIN) {
                                                        /* generate message */
                                                        errCode = PutBufferCompleteToQueue(pSubDevice, pQueue, SD_DIR_IN, (uint32_t)((uint32_t)STREAMFLAG_SEND_DATA | (uint32_t)STREAMFLAG_DRAINED));
                                                        if (errCode != 0) {
                                                                hb_slogf_error("%s: Add buffer complete for drain (STREAMFLAG_SEND_DATA|STREAMFLAG_DRAINED) to queue failed",
                                                                    __FUNCTION__);
                                                        }
                                                        pSubDevice->pTransferQueue = pQueue->pNext;
                                                        FREE_TRC(pQueue);
                                                } else {
                                                        pQueueNext = NULL; /* terminate loop if first non empty element is found */
                                                }
                                        }
                                        pQueue = pQueueNext;
                                } /* end while loop */
                        }

                        pQueue = pSubDevice->pProvideQueue;
                        while (pQueue != NULL) {
                                pQueueNext = pQueue->pNext;
                                remain = 0;
                                for (i = 0; i < pQueue->numEntries; i++) {
                                        remain += pQueue->entry[i].remain;
                                }
                                if (direction == DSP_TO_HOST) {
                                        if ((remain == 0) || (pQueue->complete != 0)) {
                                                /* element is empty */
                                                flags = (uint32_t)STREAMFLAG_PROVIDE_BUFFER;

                                                if (pQueue->complete != 0) {
                                                        /* or discontinued */
                                                        flags = (uint32_t)STREAMFLAG_PROVIDE_BUFFER | (uint32_t)STREAMFLAG_DISCONTINUITY;
                                                }
                                                /* generate message */
                                                errCode = PutBufferCompleteToQueue(pSubDevice, pQueue, SD_DIR_IN, flags);
                                                if (errCode != 0) {
                                                        hb_slogf_error("%s: Add buffer complete (STREAMFLAG_PROVIDE_BUFFER) to queue failed",
                                                            __FUNCTION__);
                                                }
                                                pSubDevice->pProvideQueue = pQueue->pNext;
                                                FREE_TRC(pQueue);
                                        } else {
                                                if (mode == MODE_DRAIN) {
                                                        /* generate message */
                                                        errCode = PutBufferCompleteToQueue(pSubDevice, pQueue, SD_DIR_IN, (uint32_t)((uint32_t)STREAMFLAG_PROVIDE_BUFFER | (uint32_t)STREAMFLAG_DRAINED));
                                                        if (errCode != 0) {
                                                                hb_slogf_error("%s: Add buffer complete for drain (STREAMFLAG_PROVIDE_BUFFER|STREAMFLAG_DRAINED) to queue failed",
                                                                    __FUNCTION__);
                                                        }
                                                        pSubDevice->pProvideQueue = pQueue->pNext;
                                                        FREE_TRC(pQueue);
                                                } else {
                                                        pQueueNext = NULL; /* terminate loop if first non empty element is found */
                                                }
                                        }
                                } else { /* HOST_TO_DSP */
                                        if ((remain == 0) || (mode == MODE_DRAIN)) {
                                                /* buffer is empty or drain mode, buffer complete is suppressed */
                                                /* remove queue element */
                                                pSubDevice->pProvideQueue = pQueue->pNext;
                                                FREE_TRC(pQueue);
                                        } else {
                                                if ((completeMarker != 0) && (pQueue->entry[0].size != pQueue->entry[0].remain)) {
                                                        /* message has discontinuity and the current buffer element is partially transfered */
                                                        /* generate message */
                                                        errCode = PutBufferCompleteToQueue(pSubDevice, pQueue, SD_DIR_OUT, (uint32_t)STREAMFLAG_DISCONTINUITY);
                                                        if (errCode != 0) {
                                                                hb_slogf_error("%s: Add buffer complete (STREAMFLAG_DISCONTINUITY) to queue failed",
                                                                    __FUNCTION__);
                                                        }
                                                        pSubDevice->pProvideQueue = pQueue->pNext;
                                                        FREE_TRC(pQueue);
                                                        if (pSubDevice->pProvideQueue != NULL) {
                                                                sendQueueChanged = 1;
                                                        }
                                                } else {
                                                        pQueueNext = NULL; /* terminate loop if first non empty element is found */
                                                }
                                        }
                                }
                                pQueue = pQueueNext;
                        }
                        if (sendQueueChanged != 0) {
                                if (WakeupSendThread(pDevice) != 0) {
                                        hb_slogf_error("%s: Wakeup sendthread failed", __FUNCTION__);
                                        retValue = -1;
                                }
                        }
                } else {
                        hb_slogf_error(NO_DEVICE, __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                retValue = -1;
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSubDeviceCleanupQueue - OUT");
#endif
        return (retValue);
}

/**
 * dspipcWaitDrainComplete wait for the comppletion of the Drain on a given Subdevice
 * Completion is done if Complete-MSG has been send successfully to the DSP.
 *
 * @param pSubDevice Pointer to the subdevice table structure
 * @param timeoutMs Maximum timeout to wait for the drain acknowledge
 * @return int Error code (0=ok, -1=error)
 */
int dspipcWaitDrainComplete(subDeviceTable_t* pSubDevice, unsigned int timeoutMs)
{
        int retValue = -1;
        int errCode;
        /*struct sigevent timeoutEvent; */
        /*uint64_t        timeout; */
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcWaitDrainComplete - IN");
#endif

        if (pSubDevice != NULL) {
                errCode = pthread_mutex_lock(&(pSubDevice->drainCompCondMutex));
                if (errCode == EOK) {

#ifdef DRAIN_INFO
                        hb_slogf_error("%s: DRAIN Wait Drain Complete started <%s>", __FUNCTION__, pSubDevice->pSubDeviceName);
#endif
                        /* wait for drain acknowledge */
                        if (pSubDevice->drainComplete == 0) {
                                /* condvar signaling is missing due to some magic things, has to be fixed */
                                errCode = pthread_cond_wait_timeout(&(pSubDevice->drainCompCondVar), &(pSubDevice->drainCompCondMutex), timeoutMs);
                                if (errCode == EOK) {
                                        if (pSubDevice->drainComplete != 0) {
                                                pSubDevice->drainComplete--;
                                                retValue = 0;
                                        } else {
                                                hb_slogf_error("%s: Cond wait returned, but variable not set",
                                                    __FUNCTION__);
                                        }
                                } else {
                                        hb_slogf_error("%s: Cond wait failed %d <%s>",
                                            __FUNCTION__, errCode, strerror(errCode));
                                }
                        } else {
                                pSubDevice->drainComplete--;
                                retValue = 0;
                        }

                        errCode = pthread_mutex_unlock(&(pSubDevice->drainCompCondMutex));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Drain complete %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
                        }
                } else {
                        hb_slogf_error("%s: Drain complete %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
                }
        } else {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcWaitDrainComplete - OUT");
#endif
        return (retValue);
}

/**
 * dspipcSignalDrainComplete signals a drain complete.
 *
 * @param pSubDevice Pointer to the subdevice table structure
 * @return int Error code (0=ok, -1=error)
 */
int dspipcSignalDrainComplete(subDeviceTable_t* pSubDevice)
{
        int retValue = -1;
        int errCode;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSignalDrainComplete - IN");
#endif
        if (pSubDevice != NULL) {
                errCode = pthread_mutex_lock(&(pSubDevice->drainCompCondMutex));
                if (errCode == EOK) {
                        pSubDevice->drainComplete++;
                        errCode = pthread_cond_signal(&(pSubDevice->drainCompCondVar));
                        if (errCode == EOK) {
                                retValue = 0;
#ifdef DRAIN_INFO
                                hb_slogf_error("%s: DRAIN Signal Drain Complete done <%s>", __FUNCTION__, pSubDevice->pSubDeviceName);
#endif
                        } else {
                                hb_slogf_error("%s: Cond signal failed <%s>",
                                    __FUNCTION__, strerror(errCode));
                        }

                        errCode = pthread_mutex_unlock(&(pSubDevice->drainCompCondMutex));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Buffer %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
                        }
                } else {
                        hb_slogf_error("%s: Buffer %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
                }
        } else {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcSignalDrainComplete - OUT");
#endif
        return (retValue);
}

/**
 * This funtion has to be called from the interrupt thread of the chipset
 * layer whenever the drain execution is completed
 *
 * @param pSubDevice Pointer to the subdevice table structure
 * @return int Status of the action (0=OK,-1=Failure)
 */
int dspipcDrainComplete(subDeviceTable_t* pSubDevice)
{
        deviceTable_t* pDevice;
        int retValue = -1;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcDrainComplete - IN");
#endif

        if (pSubDevice != NULL) {
                pDevice = pSubDevice->pDevice;
                if (pDevice != NULL) {
                        pSubDevice->status = SD_STAT_DRAIN_COMPLETE;
                        if (WakeupSendThread(pDevice) == 0) {
                                retValue = 0;
                        } else {
                                hb_slogf_error("%s: WakeupSendThread failed", __FUNCTION__);
                        }
                } else {
                        hb_slogf_error(NO_DEVICE, __FUNCTION__);
                }
        } else {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
        }

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcDrainComplete - OUT");
#endif
        return (retValue);
}

/******************************************************************************* */
/************************ Internal helper functions **************************** */
/******************************************************************************* */

/**
   dspipcCleanupSubDevice releases all elements of the specified subdevice
   structure.

   @param pSubDevice Pointer to the sub device structure
   @return void
*/
static void CleanupSubDevice(subDeviceTable_t* pSubDevice)
{
        int errCode;

        subDeviceBufferQueue_t* pQueue;
        subDeviceBufferQueue_t* pQueueNext;
        /*   TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "CleanupSubDevice - IN"); */

        if (pSubDevice != NULL) {
                /* release character device  */
                device_destroy(pSubDevice->pDevice->driverclass, MKDEV(pSubDevice->pDevice->major_num, pSubDevice->minor_num));
                cdev_del((struct cdev*)pSubDevice);
                /* cleanup and release all ressources */
                if (pSubDevice->pSubDeviceName != NULL) {
                        FREE_TRC(pSubDevice->pSubDeviceName);
                        pSubDevice->pSubDeviceName = NULL;
                }
                if (pSubDevice->pDeviceAttr != NULL) {
                        pSubDevice->pDeviceAttr = NULL;
                }
                if (pSubDevice->pSendMsgQueue != NULL) {
                        if (dspipcQueueClear(pSubDevice->pSendMsgQueue) != 0) {
                                hb_slogf_error("%s: clear send queue failed", __FUNCTION__);
                        }
                        if (dspipcQueueDestroy(pSubDevice->pSendMsgQueue) != 0) {
                                hb_slogf_error("%s: cleanup destroy send queue failed", __FUNCTION__);
                        }
                        pSubDevice->pSendMsgQueue = NULL;
                }
                if (pSubDevice->pRecvMsgQueue != NULL) {
                        if (dspipcQueueClear(pSubDevice->pRecvMsgQueue) != 0) {
                                hb_slogf_error("%s: clear recv queue failed", __FUNCTION__);
                        }
                        if (dspipcQueueDestroy(pSubDevice->pRecvMsgQueue) != 0) {
                                hb_slogf_error("%s: cleanup destroy recv queue failed", __FUNCTION__);
                        }
                        pSubDevice->pRecvMsgQueue = NULL;
                }
                /* cleanup buffer queues */
                pQueue = pSubDevice->pProvideQueue;
                while (pQueue != NULL) {
                        pQueueNext = pQueue->pNext;
                        FREE_TRC(pQueue);
                        pQueue = pQueueNext;
                }

                pQueue = pSubDevice->pTransferQueue;
                while (pQueue != NULL) {
                        pQueueNext = pQueue->pNext;
                        FREE_TRC(pQueue);
                        pQueue = pQueueNext;
                }

                errCode = pthread_mutex_destroy(&(pSubDevice->drainCompCondMutex));
                if (errCode != EOK) {
                        hb_slogf_error("%s: destroy of drain mutex failed (%s)",
                            __FUNCTION__, strerror(errCode));
                }
                errCode = pthread_cond_destroy(&(pSubDevice->drainCompCondVar));
                if (errCode != EOK) {
                        hb_slogf_error("%s: destroy of drain condvar failed (%s)",
                            __FUNCTION__, strerror(errCode));
                }

                errCode = pthread_mutex_destroy(&(pSubDevice->bufferQueueMutex));
                if (errCode != EOK) {
                        hb_slogf_error("%s: destroy of buffer queue mutex failed (%s)",
                            __FUNCTION__, strerror(errCode));
                }
                errCode = pthread_mutex_destroy(&(pSubDevice->thisMutex));
                if (errCode != EOK) {
                        hb_slogf_error("%s: destroy of sub device mutex failed (%s)",
                            __FUNCTION__, strerror(errCode));
                }
                if (pSubDevice->pMsg.generic != NULL) {
                        FREE_TRC(pSubDevice->pMsg.generic);
                }
                FREE_TRC(pSubDevice);
        }
        return;
}

/**
 * This function will deliver an extended buffer complete message with the specified
 * flags to the specified queue of the subdevice. Depending on the direction of
 * this request either the receive or the transmit queue is used.
 *
 * @param pSubDevice Pointer to the sub device structure
 * @param pQueue Pointer to the sub device buffer queue to be used (transfer or
 *               provide queue)
 * @param direction Direction of this request (SD_DIR_IN | SD_DIR_OUT)
 * @param flags Flags to be set inside the buffercomplete message
 * @return int status 0 = OK, -1 = an error has occured
 */
static int PutBufferCompleteToQueue(const subDeviceTable_t* pSubDevice,
    const subDeviceBufferQueue_t* pQueue,
    subDeviceDir_t direction,
    uint32_t flags)
{
        int retValue = 0;
        unsigned int i;
        dspipc_recvStreamMessage_t* pRecvStreamMsg = NULL;
        subDeviceTable_t* pReplySubDevice = NULL;
        uint32_t msgId;
        messagePointer_t pMessage;
        queueMsgCaller_t calledBy;
        msgQueue_t* pMsgQueue = NULL;

        pMessage.generic = NULL;

        if (pSubDevice != NULL) {
                if (pQueue != NULL) {
                        pRecvStreamMsg = (dspipc_recvStreamMessage_t*)CALLOC_TRC(1, sizeof(dspipc_recvStreamMessage_t));
                        if (pRecvStreamMsg != NULL) {
                                pReplySubDevice = dspipcSearchSubDevice(pSubDevice->pDevice,
                                    direction,
                                    pSubDevice->channelNum);
                                if (pReplySubDevice != NULL) {
                                        msgId = 0;
                                        /* copy the ranges */
                                        pRecvStreamMsg->hdr.numEntries = pQueue->numEntries;

                                        for (i = 0; i < pQueue->numEntries; i++) {
                                                dspipc_scatGat_t* pEntryRecv = &(pRecvStreamMsg->entry[i]);
                                                dspipc_scatGat_t* pQueue_entry = (dspipc_scatGat_t*)&(pQueue->entry[i]);
                                                memcpy(pEntryRecv, pQueue_entry, sizeof(dspipc_scatGat_t));
                                                pEntryRecv->valid = pQueue_entry->size - pQueue_entry->remain;
                                        }
                                        pRecvStreamMsg->hdr.flags = flags;

                                        /* put the buffer complete to the message queue */
                                        pMessage.recvStream = pRecvStreamMsg;
                                        calledBy = (direction == SD_DIR_IN) ? RECEIVE_CALLED : SEND_CALLED;
                                        pMsgQueue = (direction == SD_DIR_IN) ? pReplySubDevice->pRecvMsgQueue : pReplySubDevice->pSendMsgQueue;
                                        if (dspipcQueueAddMessage(pSubDevice->pDevice,
                                                calledBy,
                                                pMsgQueue,
                                                DSPIPC_CONT_BUFCOMPLETE,
                                                msgId,
                                                pQueue->cmdId,
                                                /* get the next paramters out of the message */
                                                DSPIPC_MSGTYPE_STREAM,
                                                DSPIPC_MSGREPLY_NO,
                                                pMessage,
                                                sizeof(dspipc_recvStreamMessage_t))
                                            != 0) {
                                                hb_slogf_error("%s: Add msg msgid %d cmdid %d to queue (%s) failed",
                                                    __FUNCTION__, msgId, pQueue->cmdId,
                                                    (pSubDevice->pSubDeviceName == NULL) ? "NULL" : pSubDevice->pSubDeviceName);
                                                FREE_TRC(pRecvStreamMsg);
                                                pRecvStreamMsg = NULL;
                                                retValue = -1;
                                        }
                                } else {
                                        hb_slogf_error(CANNOT_FIND_CORRESPONDING_INPUT_DEVICE, __FUNCTION__);
                                        FREE_TRC(pRecvStreamMsg);
                                        pRecvStreamMsg = NULL;
                                        retValue = -1;
                                }
                        } else {
                                hb_slogf_error("%s: No memory for buffer complete message", __FUNCTION__);
                                retValue = -1;
                        }
                } else {
                        hb_slogf_error("%s: No queue", __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                retValue = -1;
        }

        return (retValue);
}

/**
 * RemoveFromBuffers removes the specified amount of transfer bytes
 * out of the specified queue. After a transfer of data the number
 * of transfered bytes will be removed from the remain field of this queue. To
 * prevent unrequired transfers it is possible to add a queue entry with an
 * infinite length. If such a queue entry is found the remain length will be
 * not changed (we be INFINITE after call again). To clear an infinite length
 * element you can specify infinite to the len, in this case the remain value
 * will be cleared to zero.
 *
 * @param pSubDevice Pointer to the device table structure which holds the
 *                   transfer/provide queues
 * @param pQueue Pointer to the transfer or provide queue
 * @param len Number of bytes which are transfered and has to removed from the
 *                   transfer/provide queue (value or SUBDEVICE_QUEUE_LEN_INFINITE)
 * @return int status 0 = OK, -1 = an error has occured
 */
static int RemoveFromBuffers(const subDeviceTable_t* pSubDevice,
    subDeviceBufferQueue_t* pQueue,
    uint32_t len)
{
        int retValue = 0;
        unsigned int i;

        while ((pQueue != NULL) && (len != 0)) {
                for (i = 0; (i < pQueue->numEntries) && (len != 0); i++) {
                        if (pQueue->entry[i].remain != SUBDEVICE_QUEUE_LEN_INFINITE) {
                                /* normal buffer entry */
                                if (pQueue->entry[i].remain >= len) {
                                        pQueue->entry[i].remain -= len;
                                        len = 0;
                                } else {
                                        len -= pQueue->entry[i].remain;
                                        pQueue->entry[i].remain = 0;
                                }
                        } else {
                                /* infinite buffer entry */
                                if (len == SUBDEVICE_QUEUE_LEN_INFINITE) {
                                        pQueue->entry[i].remain = 0;
                                }
                                len = 0;
                        }
                }
                pQueue = pQueue->pNext;
        }

        if (len != 0) {
                hb_slogf_error("%s: Unable to remove %d bytes from queue", __FUNCTION__, len);
                retValue = -1;
        }

        return (retValue);
}

/**
 * dspipcCleanTransProvQueue frres all elements in the TransferQueue and ProvideQueue
 * regardless of the data that may still have to be transferred (received)
 *
 * @param pSubDevice Pointer to the subdevice to clean the queues
 * @return int status 0 = OK, -1 = an error has occured
 */
static int dspipcCleanTransProvidQueue(subDeviceTable_t* pSubDevice)
{
        int retValue = 0;
        subDeviceBufferQueue_t* pQueue = NULL;
        subDeviceBufferQueue_t* pQueueNext = NULL;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcCleanTransProvQueue - IN");
#endif

        if (pSubDevice == NULL) {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                return (-1);
        }

        /* Clean TransferQueue */
        pQueue = pSubDevice->pTransferQueue;
        while (pQueue != NULL) {
                pQueueNext = pQueue->pNext;
                FREE_TRC(pQueue);
                pQueue = pQueueNext;
        } /* end while loop */
        pSubDevice->pTransferQueue = NULL;

        /* Clean ProvideQueue */
        pQueue = pSubDevice->pProvideQueue;
        while (pQueue != NULL) {
                pQueueNext = pQueue->pNext;
                FREE_TRC(pQueue);
                pQueue = pQueueNext;
        } /* end while loop */
        pSubDevice->pProvideQueue = NULL;

#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcCleanTransProvQueue - OUT");
#endif
        return (retValue);
}

/**
 * dspipcCleanAllSubDevice cleans all Queues of all Subdevices
 * This function is only called during a REBOOT.
 *
 * @param pDevice Pointer to the device table structure to add the new sub device
 * @return int status 0 = OK, -1 = an error has occured
 */
int dspipcCleanAllSubDevices(deviceTable_t* pDevice)
{
        int retValue = 0;
        subDeviceTable_t* pSubDevice;
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcCleanAllSubDevices - IN");
#endif

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        if (dspipcLockSubDevices(pDevice) == EOK) {
                pSubDevice = pDevice->pSubDevices;
                while (pSubDevice != NULL) {
                        /* Unblock all pending transfer requests on IN devices */
                        if ((pSubDevice->type == SD_STREAM) || (pSubDevice->type == SD_COMMAND)) {
                                if ((pSubDevice->dir == SD_DIR_IN) || (pSubDevice->dir == SD_DIR_NODIR)) {
                                        if (dspipcUnblockSubDevice(pSubDevice, EBUSY) != 0) {
                                                hb_slogf_error("%s: dspipcUnblockSubDevice() failed for %s ", __FUNCTION__, pSubDevice->pSubDeviceName);
                                                retValue = -1;
                                        }
                                }
                        }

                        /* Clean Provide and TransferQueue */
                        if (dspipcCleanTransProvidQueue(pSubDevice) != 0) {
                                hb_slogf_error("%s: dspipcCleanTransProvidQueue() failed for %s ", __FUNCTION__, pSubDevice->pSubDeviceName);
                                retValue = -1;
                        }

                        /* Clean SendQueue */
                        if (pSubDevice->pSendMsgQueue != NULL) {
                                if (dspipcQueueClear(pSubDevice->pSendMsgQueue) != 0) {
                                        hb_slogf_error("%s: Clear SendQueue failed <%s>", __FUNCTION__, pSubDevice->pSubDeviceName);
                                        retValue = -1;
                                }
                        }

                        /* Clean RecvQueue */
                        if (pSubDevice->pRecvMsgQueue != NULL) {
                                if (dspipcQueueClear(pSubDevice->pRecvMsgQueue) != 0) {
                                        hb_slogf_error("%s: Clear RecvQueue failed <%s>", __FUNCTION__, pSubDevice->pSubDeviceName);
                                        retValue = -1;
                                }
                        }
                        /* set state to Reboot */
                        pSubDevice->status = SD_STAT_REBOOT;
                        pSubDevice = pSubDevice->pNext;
                }

                if (dspipcUnlockSubDevices(pDevice) != EOK) {
                        hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                retValue = -1;
        }
#ifdef IN_OUT_TRACE
        TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, "dspipcCleanAllSubDevices - OUT");
#endif
        return (retValue);
}

/*===========================   End Of File   ================================= */
