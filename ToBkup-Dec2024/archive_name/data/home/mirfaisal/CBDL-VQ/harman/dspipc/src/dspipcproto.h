/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file dspipcproto.h
 * 
 * This is the header file for the prototype declarations for all functions
 * of the dev-dspipc ressource handler which could be called by the chipset
 * layer modules.
 * 
 * @author Andreas Heiner
 * @author Andreas Abel
 * @date   04.09.2006
 * 
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

#ifndef _DSPIPCPROTO_H_INCLUDED_
#define _DSPIPCPROTO_H_INCLUDED_

#include "dev-dspipc.h"
#include <linux/types.h> /*uintptr_t, uint32_t, .. */

typedef int iofunc_attr_t;

/*=========================  Prototype Definitions   =========================== */
/*** file device.c ** */
extern int dspipcLockSubDevices(deviceTable_t* pDevice);
extern int dspipcUnlockSubDevices(deviceTable_t* pDevice);
extern changeReason_t dspipcWaitChangeSubDevices(deviceTable_t* pDevice);
extern int dspipcSignalChangeSubDevices(deviceTable_t* pDevice, changeReason_t reason);
extern int dspipcStopInterruptThread(deviceTable_t* pDevice);
extern int dspipc_getNewMessageId(deviceTable_t* pDevice, uint32_t* pId);

/*** file subdevice.c ** */
extern subDeviceTable_t* dspipcAddSubDevice(deviceTable_t* pDevice,
    mode_t subAccessMode,
    void* pExtra,
    subDeviceType_t type,
    subDeviceDir_t dir,
    uint32_t channelNum,
    unsigned int prio);
extern subDeviceTable_t* dspipcSearchSubDevice(const deviceTable_t* pDevice,
    subDeviceDir_t dir,
    uint32_t channelNum);
extern int dspipcRemSubDevice(deviceTable_t* pDevice, subDeviceTable_t* pSubDevice);
extern int dspipcChangeSubDevicePrio(deviceTable_t* pDevice, uint32_t channelNum, int newPrio);
extern int dspipcDeleteSubDevice(deviceTable_t* pDevice, subDeviceTable_t* pSubDevice);
extern int dspipcUnblockSubDevice(const subDeviceTable_t* pSubDevice, int reason);
extern int dspipcSubDeviceLockBuffer(subDeviceTable_t* pSubDevice);
extern int dspipcSubDeviceUnlockBuffer(subDeviceTable_t* pSubDevice);
extern int dspipcSubDeviceAddBuffer(subDeviceTable_t* pSubDevice,
    bufferQueueType_t type,
    subDeviceBufferQueue_t* pElem,
    uint32_t rescanONOFF);
extern int dspipcSubDeviceCheckBuffer(const subDeviceTable_t* pSubDevice,
    uint32_t* pLen,
    subDeviceBufferQueue_t** ppTransferQueue,
    unsigned int* pTransferIndex,
    subDeviceBufferQueue_t** ppProvideQueue,
    unsigned int* pProvideIndex);
extern int dspipcSubDeviceGetNextBuffer(const subDeviceTable_t* pSubDevice,
    transferDir_t direction,
    uint32_t maxLen,
    void** ppHostVirt,
    uint32_t* pHostPhys,
    uint32_t* pTransLen,
    int* pNextAvail);
extern int dspipcSubDeviceRemoveBuffer(const subDeviceTable_t* pSubDevice, uint32_t len);
extern int dspipcSubDeviceCleanupQueue(subDeviceTable_t* pSubDevice,
    transferDir_t direction,
    cleanupMode_t mode);
extern int dspipcWaitDrainComplete(subDeviceTable_t* pSubDevice, unsigned int timeoutMs);
extern int dspipcSignalDrainComplete(subDeviceTable_t* pSubDevice);
extern int dspipcDrainClear(subDeviceTable_t* pSubDevice);
extern int dspipcDrainComplete(subDeviceTable_t* pSubDevice);
extern int dspipcCleanAllSubDevices(deviceTable_t* pDevice);

/*** file memobj.c ** */
extern int InitMemoryObjects(void);
extern int InitBaseObjects(void);
extern int DeinitMemoryObjects(void);
extern int RequestMemoryElement(dspipc_reqMemObj_t* pReqMemObj, memObjList_t** ppNewElement, uint32_t req_new);
extern int RemoveMemoryElement_hold(const dspipc_delMemObj_t* pDelMemObj, int checkUseCnt);
extern memObjList_t* SearchMemoryElement(uint32_t memId);
extern memObjList_t* SearchMemId(memObjList_t* pMemObj, memSearchType_t memSearchType,
    uint32_t memAddr, uint32_t memSize,
    uint32_t* pMemId, uint32_t* pMemOffset);
extern int IncUsageMemoryElement(memObjList_t* pMemObj, uint32_t count);
extern int DecUsageMemoryElement(memObjList_t* pMemObj, uint32_t count);
extern int ResetMemoryElementCounter(void);

/*** file msgqueue.c ** */
extern msgQueue_t* dspipcQueueCreate(const deviceTable_t* pDevice, unsigned int numElements);
extern int dspipcQueueDestroy(msgQueue_t* pQueue);
extern int dspipcQueueAddMessage(deviceTable_t* pDevice,
    queueMsgCaller_t calledBy,
    msgQueue_t* pQueue,
    dspipc_msgContent_t contentType,
    uint32_t msgId, uint32_t cmdId,
    dspipc_msgType_t msgType,
    dspipc_msgReply_t msgReply,
    messagePointer_t pMsg,
    uint32_t msgSize);
extern int dspipcQueueGetMessage(msgQueue_t* pQueue,
    dspipc_msgContent_t* pContentType,
    uint32_t* pMsgId, uint32_t* pCmdId,
    dspipc_msgType_t* pMsgType,
    dspipc_msgReply_t* pMsgReply,
    messagePointer_t* ppMsg,
    uint32_t* pMsgSize,
    uint32_t* pNumLost,
    iofunc_attr_t* pAttr);
extern int dspipcQueueNumAvail(msgQueue_t* pQueue, int* pNumElements);
extern int dspipcQueueClear(msgQueue_t* pQueue);

/*** file boottable.c ** */
extern int dspipcBootTableInit(deviceTable_t* pDevice, int fd, const sw_regions_t* pRegions);
extern sectionState_t dspipcBootTableGetNext(deviceTable_t* pDevice, void* pBuf, unsigned int maxSize, int* pLen);

/*** file intthread.c ** */
extern int dspipcIntrThread(void* arg);

/*** file sendthread.c ** */
extern void* dspipcSendThread(void* arg);
extern int WakeupSendThread(const deviceTable_t* pDevice);

/*** file options.c ** */
extern int options(int argc, char* const argv[], deviceTable_t** ppDevice);
extern int startChipset(deviceTable_t* pDevice);
extern void DllDeinit(deviceTable_t* pDevice);
struct file_operations* dspipc_getfops(void);

#endif /*_DSPIPCPROTO_H_INCLUDED_ */
