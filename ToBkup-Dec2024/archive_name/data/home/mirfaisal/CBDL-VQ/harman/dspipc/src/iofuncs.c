/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/

/**
 * @file iofuncs.c
 * 
 * This source file contains IO functions for the dev-dspipc
 * 
 * @author Howard Yang
 * @date   20.Nov.2012
 * 
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

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
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/wait.h>

#include "debug.h"
#include "dev-dspipc.h"
#include "dspipcproto.h"
#include "malloc_trc.h"
#include "misc.h"
#include "string_def_ext.h"

static int dspipcSendMessage(const deviceTable_t* pDevice, const subDeviceTable_t* pSubDevice,
    const dspipc_sendMessage_t* pSendMessage)
{
        unsigned int i;
        uint32_t queueMsgSize;
        messagePointer_t pPayload;
        uint32_t numMessages;
        uint32_t queueMsgId;
        int errCode;

        pPayload.generic = NULL;
        numMessages = pSendMessage->hdr.numMessages;
        if (numMessages == 0) {
                /*no messages requested??? */
                hb_slogf_error("%s: Num of messages to send is zero! process name %s, pid %d",
                    __FUNCTION__, current->comm, current->pid);
                return (-ENOSYS);
        }
        if (numMessages > MAX_NUM_MESSAGES) {
                /*more messages requested than possible */
                numMessages = MAX_NUM_MESSAGES;
                hb_slogf_error("%s: Num of Send messages reduced,process name %s, pid %d",
                    __FUNCTION__, current->comm, current->pid);
        }
        for (i = 0; i < numMessages; i++) {
                queueMsgSize = pSendMessage->message[i].size;

#ifdef VVVV_ENABLE
                if (gCmdOptions.optv > 2) {
                        hb_slogf_error("%s: CMD-Msg (size:%d) [process name %s, pid %d]", __FUNCTION__, queueMsgSize, current->comm, current->pid);
                }
#endif

                if (queueMsgSize > DEVCTL_MSG_SIZE) {
                        hb_slogf_error("%s: msg size larger than field [PNAME=%s;PID=%d]",
                            __FUNCTION__, current->comm, current->pid);
                        return (-ENOSYS);
                }
                pPayload.generic = (void*)&(pSendMessage->message[i].data[0]);

                if (pPayload.generic == NULL) {
                        hb_slogf_error("%s: No mem for queue entry", __FUNCTION__);
                        return (-ENOMEM);
                }
                queueMsgId = 0;
                if (dspipc_getNewMessageId((deviceTable_t*)pDevice, &queueMsgId) != 0) {
                        hb_slogf_error(GET_MESSAGE_ID_FUNC_FOR_DEVICE_FAILED,
                            __FUNCTION__, pDevice->pDeviceName,
                            pSubDevice->pSubDeviceName);
                }

                if ((errCode = dspipcQueueAddMessage((deviceTable_t*)pDevice,
                         SEND_CALLED,
                         pSubDevice->pSendMsgQueue,
                         DSPIPC_CONT_MESSAGE,
                         queueMsgId,
                         pSendMessage->message[i].cmdid,
                         pSendMessage->message[i].msgType,
                         pSendMessage->message[i].msgReply,
                         pPayload,
                         queueMsgSize))
                    != 0) {
                        hb_slogf_error("%s: Add to queue (%s) failed %d [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->pSubDeviceName,
                            errCode, current->comm, current->pid);

                        pPayload.generic = NULL;
                        return (EAGAIN);
                }
                pPayload.generic = NULL;
        }
        return (0);
}

static int dspipcGetMessage(const deviceTable_t* pDevice, subDeviceTable_t* pSubDevice,
    dspipc_getMessage_t* pGetMessage,
    int* pNbytes,
    iofunc_attr_t* pAttr)
{
        unsigned int currMsgNum;
        int retValue;
        int stopLoop;
        int messagesRead;
        messagePointer_t pQueueMsg;
        uint32_t queueNumLost;
        uint32_t queueMsgSize;
        uint32_t numMessages;
        uint32_t queueMsgId;
        uint32_t queueCmdId;
        uint32_t currEntry;
        dspipc_msgContent_t useContentType;
        dspipc_msgType_t msgType;
        dspipc_msgReply_t msgReply;
        memObjList_t* pShmemObj;
        dspipc_recvStreamMessage_t* pRecvStreamMsg;

        retValue = 0;
        useContentType = DSPIPC_CONT_ANY;
        numMessages = pGetMessage->hdr.numMessages;
        if (numMessages == 0) {
                /*no messages requested??? */
                hb_slogf_error("%s: Num of messages to get is zero! [PNAME=%s;PID=%d]",
                    __FUNCTION__, current->comm, current->pid);
                return (-ENOSYS);
        }
        pGetMessage->hdr.msgContent = DSPIPC_CONT_INVALID;
        if (numMessages > MAX_NUM_MESSAGES) {
                /*more messages requested than possible */
                hb_slogf_error("%s: Num of Get messages reduced (before=%d, after=%d) [PNAME=%s;PID=%d]",
                    __FUNCTION__, numMessages, MAX_NUM_MESSAGES,
                    current->comm, current->pid);
                numMessages = MAX_NUM_MESSAGES;
        }
        messagesRead = 0;
        queueNumLost = 0;
        stopLoop = 0;
        for (currMsgNum = 0; (currMsgNum < numMessages) && (stopLoop == 0); currMsgNum++) {

                /* Get message out of the queue */
                pQueueMsg.generic = pSubDevice->pMsg.generic;

                /* the next call can be blocked if the queue is empty */
                retValue = dspipcQueueGetMessage(pSubDevice->pRecvMsgQueue,
                    &useContentType,
                    &queueMsgId,
                    &queueCmdId,
                    &msgType,
                    &msgReply,
                    &pQueueMsg,
                    &queueMsgSize,
                    &queueNumLost,
                    pAttr);
                pSubDevice->status = SD_STAT_READY;
                if (retValue == 0) {
                        if (msgReply != DSPIPC_MSGREPLY_NO) {
                                hb_slogf_error("%s: Reply requested but not supported, yet", __FUNCTION__);
                        }
                }

                switch (retValue) {
                case 0:
                        if (pQueueMsg.generic != NULL) {
                                switch (useContentType) {
                                case DSPIPC_CONT_MESSAGE:
                                        pGetMessage->hdr.msgContent = useContentType;
                                        if (queueMsgSize <= DEVCTL_MSG_SIZE) {
                                                pGetMessage->message[currMsgNum].size = queueMsgSize;
                                                pGetMessage->message[currMsgNum].msgid = queueMsgId;
                                                pGetMessage->message[currMsgNum].cmdid = queueCmdId;
                                                pGetMessage->message[currMsgNum].msgType = msgType;
                                                pGetMessage->message[currMsgNum].msgReply = msgReply;

                                                memcpy(&(pGetMessage->message[currMsgNum].data[0]), pQueueMsg.generic, queueMsgSize);

                                                /*free memeory space of queue message element, allocated inside message receive */
                                                pQueueMsg.generic = NULL;
                                                messagesRead++;
                                                if (queueNumLost != 0) {
                                                        stopLoop = 1;
                                                }
#ifdef VVVV_ENABLE
                                                if (gCmdOptions.optv > 2) {
                                                        hb_slogf_error("%s: CMD-Msg (size:%d) [PNAME=%d;PID=%d]",
                                                            __FUNCTION__, queueMsgSize, current->comm, current->pid);
                                                }
#endif
                                        } else {
                                                stopLoop = 1;
                                                hb_slogf_error("%s: Message size (%d) doesn't fit (max %d)",
                                                    __FUNCTION__, queueMsgSize, DEVCTL_MSG_SIZE);
                                                retValue = -ENOSYS;
                                        }
                                        break;
                                case DSPIPC_CONT_BUFCOMPLETE:
                                        stopLoop = 1; /* only one receive message possible */
                                        useContentType = DSPIPC_CONT_BUFCOMPLETE;
                                        pGetMessage->hdr.msgContent = DSPIPC_CONT_BUFCOMPLETE;
                                        if (queueMsgSize == sizeof(dspipc_recvStreamMessage_t)) {
                                                pRecvStreamMsg = pQueueMsg.recvStream;
                                                pGetMessage->bufComplete.hdr.numEntries = pRecvStreamMsg->hdr.numEntries;
                                                pGetMessage->bufComplete.hdr.cmdId = queueCmdId;
                                                pGetMessage->bufComplete.hdr.flags = pRecvStreamMsg->hdr.flags;

                                                for (currEntry = 0; currEntry < pRecvStreamMsg->hdr.numEntries; currEntry++) {
                                                        pGetMessage->bufComplete.entry[currEntry].memId = pRecvStreamMsg->entry[currEntry].memId;
                                                        pGetMessage->bufComplete.entry[currEntry].start = pRecvStreamMsg->entry[currEntry].start;
                                                        pGetMessage->bufComplete.entry[currEntry].size = pRecvStreamMsg->entry[currEntry].size;
                                                        pGetMessage->bufComplete.entry[currEntry].remain = pRecvStreamMsg->entry[currEntry].remain;
                                                        pGetMessage->bufComplete.entry[currEntry].valid = pRecvStreamMsg->entry[currEntry].valid;
                                                        pShmemObj = (memObjList_t*)pRecvStreamMsg->entry[currEntry].mem.pMemHdl;

                                                        if (pShmemObj != NULL) {
                                                                /* remove range out of the usage counter of the memory object */
                                                                if (DecUsageMemoryElement(pShmemObj, 1) != 0) {
                                                                        hb_slogf_error("%s: Decrement usage count failed [PNAME=%s;PID=%d]",
                                                                            __FUNCTION__, current->comm, current->pid);
                                                                }

                                                        } else {
                                                                hb_slogf_error("%s: Shm Object pointer is NULL [PNAME=%s;PID=%d]",
                                                                    __FUNCTION__, current->comm, current->pid);
                                                        }
                                                } /* End for loop */

                                                /*free memeory space of queue message element, allocated inside message receive */
                                                pQueueMsg.generic = NULL;
                                                pGetMessage->bufComplete.hdr.cmdId = queueCmdId;
                                                retValue = 0;
                                        } else {
                                                hb_slogf_error("%s: Message size in queue wrong [PNAME=%s;PID=%d]",
                                                    __FUNCTION__, current->comm, current->pid);
                                        }
                                        break;
                                default:
                                        stopLoop = 1;
                                        hb_slogf_error("%s: Get unsupported message content", __FUNCTION__);
                                        retValue = -ENOSYS;
                                        break;
                                }
                        } else {
                                stopLoop = 1;
                                hb_slogf_error("%s: Get back null message", __FUNCTION__);
                                retValue = -ENOSYS;
                        }
                        break;
                case 1:
                        /* message type inside queue has changed, stop this loop */
                        stopLoop = 1;
                        retValue = 0; /* mark for normal termination */
                        break;
                case 2:
                        /* call was unblocked with timeout */
                        stopLoop = 1;
                        retValue = -ETIMEDOUT;
                        break;
                case 3:
                        /* call was unblocked because of device removal */
                        stopLoop = 1;
                        retValue = -EBADF;
                        break;
                case 4:
                        /* call was unblocked because of a special devctl */
                        stopLoop = 1;
                        retValue = -ECANCELED;
                        break;
                case 5:
                        /* call was unblocked during a reboot of the DSP */
                        stopLoop = 1;
                        retValue = -EBUSY;
                        break;
                default:
                        stopLoop = 1;
                        hb_slogf_error(GET_MESSAGE_FROM_QUEUE_FAILED,
                            __FUNCTION__, retValue);
                        retValue = -ENOSYS;
                        break;
                }
        } /* for loop, for all messages */

        if (retValue == 0) {
                pGetMessage->hdr.numMessagesLost = queueNumLost;
                switch (useContentType) {
                case DSPIPC_CONT_MESSAGE:
                        pGetMessage->hdr.numMessages = messagesRead;
                        *pNbytes = (messagesRead * sizeof(struct DSPIPC_MESSAGE_S)) + sizeof(dspipc_message_hdr_t);
                        break;
                case DSPIPC_CONT_BUFCOMPLETE:
                        pGetMessage->hdr.numMessages = 1;
                        *pNbytes = sizeof(dspipc_getMessage_t);
                        break;
                default:
                        pGetMessage->hdr.numMessages = 0;
                        *pNbytes = 0;
                        hb_slogf_error("%s: Unknown content type (%d)",
                            __FUNCTION__, useContentType);
                        break;
                }
        }
        return (retValue);
}

#define NEW_FILLMESSAGE

#ifdef NEW_FILLMESSAGE
/**
    This function handles the send of an extended recevice stream message. The
    physical addresses of the shared memory segments are calculated and the
    message to the DSP is created. This message is allocated and added to the
    send queue of the subdevice. Via this message the available buffer is
    communicated to the DSP.

    @param pCtp pointer to resource manager context
    @param pDevice Pointer to the device table structure to add the new sub device
    @param pSubDevice Pointer to the sub device structure
    @param pSendRecvStreamMessage Pointer to answer message array that will transmitted to the client
    @param calledBy shows if called by SEND or RECIEVE
    @param contentType  content type of message
    @return int status 0=success or failure=return code for the ressource manager
*/
static int dspipcFillSendStreamMessage(const deviceTable_t* pDevice, const subDeviceTable_t* pSubDevice,
    const dspipc_sendrecvStreamMessage_t* pSendRecvStreamMessage,
    queueMsgCaller_t calledBy,
    dspipc_msgContent_t contentType)
{
        int error;
        int errCode;
        unsigned int i;
        uint32_t queueMsgSize;
        uint32_t queueMsgId;
        uint32_t queueCmdId;
        memObjList_t* pShmemObj;
        dspipc_sendrecvStreamMessage_t* pQueueStreamMsg = NULL;
        messagePointer_t pMessage;
        dspipc_scatGat_t* pEntry;
        dspipc_scatGat_t* pEntryMsg;

        if (pSubDevice->pSendMsgQueue == NULL) {
                hb_slogf_error("%s: Recv stream devctl with no queue, process name %s, pid %d",
                    __FUNCTION__, current->comm, current->pid);
                return ENOSYS;
        }

        /* get address of related shmem object */

        if (pSendRecvStreamMessage->hdr.numEntries == 0) {
                hb_slogf_error("%s: error:Receive 0 entries, process name %s, pid %d",
                    __FUNCTION__, current->comm, current->pid);
                return ENOSYS;
        }

        if (pSendRecvStreamMessage->hdr.numEntries > NUM_SCAT_GATH_ENTRIES) {
                hb_slogf_error("%s: error:entries should be limited in %d, process name %s, pid %d",
                    __FUNCTION__, NUM_SCAT_GATH_ENTRIES, current->comm, current->pid);
                return ENOSYS;
        }

        if (pSubDevice->dir == SD_DIR_IN) {
                pQueueStreamMsg = (dspipc_sendrecvStreamMessage_t*)pSubDevice->pMsg.recvStream;
        } else if (pSubDevice->dir == SD_DIR_OUT) {
                pQueueStreamMsg = (dspipc_sendrecvStreamMessage_t*)pSubDevice->pMsg.sendStream;
        } else {
                hb_slogf_error("%s: error:No direction for this device, ch ID: %d, process name %s, pid %d",
                    __FUNCTION__, pSubDevice->channelNum, current->comm, current->pid);
                return ENOSYS;
        }

        /* create queue element */
        queueMsgSize = pSendRecvStreamMessage->hdr.numEntries * sizeof(dspipc_scatGat_t) + sizeof(pSendRecvStreamMessage->hdr);

        if (pQueueStreamMsg == NULL) {
                hb_slogf_error("%s: No mem for queue entry , process name %s, pid %d",
                    __FUNCTION__, current->comm, current->pid);
                return ENOMEM;
        }

        error = 0;
        if (calledBy == SEND_CALLED) {
                pQueueStreamMsg->hdr.flags = pSendRecvStreamMessage->hdr.flags;
        }

        if (pSendRecvStreamMessage->hdr.numEntries <= NUM_SCAT_GATH_ENTRIES) {
                pQueueStreamMsg->hdr.numEntries = pSendRecvStreamMessage->hdr.numEntries;
                if (calledBy == RECEIVE_CALLED) {
                        pQueueStreamMsg->hdr.cmdId = pSendRecvStreamMessage->hdr.cmdId;
                }
                queueCmdId = pSendRecvStreamMessage->hdr.cmdId;

                /* copy all the elements */
                for (i = 0; i < pSendRecvStreamMessage->hdr.numEntries; i++) {
                        /* Get pointer to Entry */
                        pEntryMsg = (dspipc_scatGat_t*)&(pSendRecvStreamMessage->entry[i]);
                        pEntry = &(pQueueStreamMsg->entry[i]);

                        pEntry->memId = pEntryMsg->memId;
                        pShmemObj = SearchMemoryElement(pEntryMsg->memId);

                        if (pShmemObj != NULL) {
                                pEntry->mem.physAddr = pShmemObj->physAddr;
                                pEntry->mem.pVirtAddr = pShmemObj->pVirtAddr;
                                pEntry->mem.pMemHdl = (void*)pShmemObj;

                                /* check start */
                                if ((pEntryMsg->start & 0x03) == 0) {
                                        pEntry->start = pEntryMsg->start;
                                } else {
                                        hb_slogf_error("%s: Start of range %d (%d) not alligned to longword , process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->start,
                                            current->comm, current->pid);
                                        error = 1;
                                }

                                /* check Size */
                                if ((pEntryMsg->size & 0x03) == 0) {
                                        pEntry->size = pEntryMsg->size;
                                        pEntry->remain = pEntryMsg->size;
                                        if (calledBy == RECEIVE_CALLED) {
                                                pEntry->valid = 0;
                                        }
                                } else {
                                        hb_slogf_error("%s: Size of range %d (%d) not alligned to longword , process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->size,
                                            current->comm, current->pid);
                                        error = 1;
                                }

                                /* check Range */
                                if ((pEntryMsg->start + pEntryMsg->size) > pShmemObj->size) {
                                        hb_slogf_error("%s: invalid range: range:%d start:%d size:%d exceeds mem object size:%d , process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->start,
                                            pEntryMsg->size,
                                            pShmemObj->size,
                                            current->comm, current->pid);
                                        error = 1;
                                }

                                /* Incement Usage count */
                                if (error == 0) {
                                        if (IncUsageMemoryElement((memObjList_t*)pEntry->mem.pMemHdl, 1) != 0) {
                                                hb_slogf_error("%s: Inc mem object usage count failed , process name %s, pid %d",
                                                    __FUNCTION__, current->comm, current->pid);
                                                error = 1;
                                        }
                                }

                        } else {
                                hb_slogf_error("%s: Got invalid mem id %d , process name %s, pid %d",
                                    __FUNCTION__,
                                    pSendRecvStreamMessage->entry[i].memId,
                                    current->comm, current->pid);
                                error = 1;
                        }
                } /* END: copy all the entries (ranges) */

                queueMsgId = 0;
                if (error == 0) {
                        if (dspipc_getNewMessageId((deviceTable_t*)pDevice, &queueMsgId) != 0) {
                                hb_slogf_error(GET_MESSAGE_ID_FUNC_FOR_DEVICE_FAILED,
                                    __FUNCTION__, pDevice->pDeviceName, pSubDevice->pSubDeviceName);
                                error = 1;
                        }
                }

                if (error == 0) {
                        if (calledBy == RECEIVE_CALLED) {
                                pMessage.recvStream = (dspipc_recvStreamMessage_t*)pQueueStreamMsg;

                                hb_slogf_error("%s: add provided buff msg to msg queue, num:%d, contentType:%d, msgsize:%d",
                                    __FUNCTION__,
                                    pMessage.recvStream->hdr.numEntries,
                                    contentType, queueMsgSize);
                        } else {
                                pMessage.sendStream = (dspipc_sendStreamMessage_t*)pQueueStreamMsg;
                        }
                        if ((errCode = dspipcQueueAddMessage((deviceTable_t*)pDevice,
                                 SEND_CALLED,
                                 pSubDevice->pSendMsgQueue,
                                 contentType,
                                 queueMsgId, queueCmdId,
                                 DSPIPC_MSGTYPE_STREAM, DSPIPC_MSGREPLY_NO,
                                 pMessage, queueMsgSize))
                            != 0) {
                                hb_slogf_error("%s: Add to queue (%s) failed %d , process name %s, pid %d",
                                    __FUNCTION__, pSubDevice->pSubDeviceName, errCode, current->comm, current->pid);
                                error = 1;
                        }
                }
        } else {
                hb_slogf_error("%s: Number of entries to high , process name %s, pid %d",
                    __FUNCTION__, current->comm, current->pid);
                error = 1;
        }

        if (error != 0) {
                return ENOSYS;
        }
        return EOK;
}
#endif

/**
    This function handles the send of an extended stream message. The physical
    addresses of the shared memory segments are calculated and the message to
    the DSP is created. This message is allocated and added to the send queue
    of the subdevice.

    @param pCtp pointer to resource manager context
    @param pDevice Pointer to the device table structure to add the new sub device
    @param pSubDevice Pointer to the sub device structure
    @param pSendStreamMessage Pointer to original message that was received from client
    @return int status 0=success or failure=return code for the ressource manager
*/
static int dspipcSendStreamMessage(const deviceTable_t* pDevice, const subDeviceTable_t* pSubDevice,
    const dspipc_sendStreamMessage_t* pSendStreamMessage)
{
#ifndef NEW_FILLMESSAGE
        int error;
        int errCode;
        unsigned int i;
        uint32_t queueMsgSize;
        uint32_t queueMsgId;
        uint32_t queueCmdId;
        memObjList_t* pShmemObj;
        dspipc_sendStreamMessage_t* pStreamSendMsg = NULL;
        messagePointer_t pMessage;

        dspipc_scatGat_t* pEntry;
        dspipc_scatGat_t* pEntryMsg;
#endif

#ifdef NEW_FILLMESSAGE
        return dspipcFillSendStreamMessage(pDevice,
            pSubDevice,
            (dspipc_sendrecvStreamMessage_t*)pSendStreamMessage,
            SEND_CALLED,
            DSPIPC_CONT_SENDSTREAM);
#else
        if (pSubDevice->pSendMsgQueue == NULL) {
                hb_slogf_error("%s: Send stream devctl with no queue, process name %s, pid %d",
                    __FUNCTION__, current->comm, current->pid);
                return ENOSYS;
        }

        /* get address of related shmem object */
#ifdef VVVV_ENABLE
        if (gCmdOptions.optv > 1) {
                hb_slogf_error("%s: Request contains %d entries", __FUNCTION__, pSendStreamMessage->hdr.numEntries);
        }
#endif

        if (pSendStreamMessage->hdr.numEntries == 0) {
                hb_slogf_error("%s: Receive 0 entries , process name %s, pid %d",
                    __FUNCTION__,
                    current->comm, current->pid);
      return ENOSYS);
        }

        /* create queue element */
        queueMsgSize = pSendStreamMessage->hdr.numEntries * sizeof(dspipc_scatGat_t) + sizeof(pSendStreamMessage->hdr);

        pStreamSendMsg = (dspipc_sendStreamMessage_t*)MALLOC_TRC(queueMsgSize);

        if (pStreamSendMsg == NULL) {
                hb_slogf_error("%s: No mem for queue entry , process name %s, pid %d",
                    __FUNCTION__,
                    current->comm, current->pid);
                return ENOMEM;
        }

        error = 0;
        pStreamSendMsg->hdr.flags = pSendStreamMessage->hdr.flags;
        if (pSendStreamMessage->hdr.numEntries <= NUM_SCAT_GATH_ENTRIES) {
                pStreamSendMsg->hdr.numEntries = pSendStreamMessage->hdr.numEntries;
                queueCmdId = pSendStreamMessage->hdr.cmdId;

                /* copy all the elements */
                for (i = 0; i < pSendStreamMessage->hdr.numEntries; i++) {
                        /* Get pointer to Entry */
                        pEntryMsg = (dspipc_scatGat_t*)&(pSendStreamMessage->entry[i]);
                        pEntry = &(pStreamSendMsg->entry[i]);

                        pEntry->memId = pEntryMsg->memId;
                        pShmemObj = SearchMemoryElement(pEntryMsg->memId);

                        if (pShmemObj != NULL) {
                                pEntry->mem.physAddr = pShmemObj->physAddr;
                                pEntry->mem.pVirtAddr = pShmemObj->pVirtAddr;
                                pEntry->mem.pMemHdl = (void*)pShmemObj;

                                if ((pEntryMsg->start & 0x03) == 0) {
                                        pEntry->start = pEntryMsg->start;
                                } else {
                                        hb_slogf_error("%s: Start of range %d (%d) not alligned to longword , process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->start,
                                            current->comm, current->pid);
                                        error = 1;
                                }

                                if ((pEntryMsg->size & 0x03) == 0) {
                                        pEntry->size = pEntryMsg->size;
                                        pEntry->remain = pEntryMsg->size;
                                } else {
                                        hb_slogf_error("%s: Size of range %d (%d) not alligned to longword , process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->size,
                                            current->comm, current->pid);
                                        error = 1;
                                }

                                /* check Range */
                                if ((pEntryMsg->start + pEntryMsg->size) > pShmemObj->size) {
                                        hb_slogf_error("%s: invalid range: range:%d start:%d size:%d exceeds mem object size:%d, process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->start,
                                            pEntryMsg->size,
                                            pShmemObj->size,
                                            current->comm, current->pid);
                                        error = 1;
                                }
                        } else {
                                hb_slogf_error("%s: Got invalid mem id %d , process name %s, pid %d",
                                    __FUNCTION__,
                                    pSendStreamMessage->entry[i].memId,
                                    current->comm, current->pid);
                                error = 1;
                        }
                } /* END: copy all the elements */

                queueMsgId = 0;
                if (error == 0) {
                        if (dspipc_getNewMessageId(pDevice, &queueMsgId) != 0) {
                                hb_slogf_error(GET_MESSAGE_ID_FUNC_FOR_DEVICE_FAILED,
                                    __FUNCTION__, pDevice->pDeviceName,
                                    pSubDevice->pSubDeviceName);
                                error = 1;
                        }
                }

                if (error == 0) {
                        for (i = 0; i < pSendStreamMessage->hdr.numEntries; i++) {
                                if (IncUsageMemoryElement((memObjList_t*)pStreamSendMsg->entry[i].mem.pMemHdl, 1) != 0) {
                                        hb_slogf_error("%s: Inc mem object usage count failed , process name %s, pid %d",
                                            __FUNCTION__, pCtp->info.pid, pCtp->info.tid);
                                        error = 1;
                                }
                        }
                }

                if (error == 0) {
                        pMessage.sendStream = pStreamSendMsg;
                        if ((errCode = dspipcQueueAddMessage(pDevice,
                                 SEND_CALLED,
                                 pSubDevice->pSendMsgQueue,
                                 DSPIPC_CONT_SENDSTREAM,
                                 queueMsgId, queueCmdId,
                                 DSPIPC_MSGTYPE_STREAM, DSPIPC_MSGREPLY_NO,
                                 pMessage, queueMsgSize))
                            != 0) {
                                hb_slogf_error("%s: Add to queue (%s) failed %d, process name %s, pid %d",
                                    __FUNCTION__, pSubDevice->pSubDeviceName,
                                    errCode, current->comm, current->pid);
                                error = 1;
                        }
                }
        } else {
                hb_slogf_error("%s: Number of entries to high, process name %s, pid %d",
                    __FUNCTION__,
                    current->comm, current->pid);
                error = 1;
        }
        if (error != 0) {
                FREE_TRC(pStreamSendMsg);
                return ENOSYS;
        }

        return EOK;
#endif
}

/**
    This function handles the send of an extended recevice stream message. The
    physical addresses of the shared memory segments are calculated and the
    message to the DSP is created. This message is allocated and added to the
    send queue of the subdevice. Via this message the available buffer is
    communicated to the DSP.

    @param pCtp pointer to resource manager context
    @param pDevice Pointer to the device table structure to add the new sub device
    @param pSubDevice Pointer to the sub device structure
    @param pRecvStreamMessage Pointer to answer message array that will transmitted to the client
    @return int status 0=success or failure=return code for the ressource manager
*/
static int dspipcRecvStreamMessage(const deviceTable_t* pDevice, const subDeviceTable_t* pSubDevice,
    const dspipc_recvStreamMessage_t* pRecvStreamMessage)
{
#ifndef NEW_FILLMESSAGE
        int error;
        int errCode;
        unsigned int i;
        uint32_t queueMsgSize;
        uint32_t queueMsgId;
        uint32_t queueCmdId;
        memObjList_t* pShmemObj;
        dspipc_recvStreamMessage_t* pStreamRecvMsg = NULL;
        messagePointer_t pMessage;
        dspipc_scatGat_t* pEntry;
        dspipc_scatGat_t* pEntryMsg;
#endif

#ifdef NEW_FILLMESSAGE
        return dspipcFillSendStreamMessage(pDevice,
            pSubDevice,
            (dspipc_sendrecvStreamMessage_t*)pRecvStreamMessage,
            RECEIVE_CALLED,
            DSPIPC_CONT_PROVIDEBUFSTREAM);

#else
        if (pSubDevice->pSendMsgQueue == NULL) {
                hb_slogf_error("%s: Recv stream devctl with no queue , process name %s, pid %d",
                    __FUNCTION__, current->comm, current->pid);
                return ENOSYS;
        }

        /* get address of related shmem object */
#ifdef VVVV_ENABLE
        if (gCmdOptions.optv > 1) {
                hb_slogf_error("%s: Request contains %d entries", __FUNCTION__, pRecvStreamMessage->hdr.numEntries);
        }
#endif

        if (pRecvStreamMessage->hdr.numEntries == 0) {
                hb_slogf_error("%s: Receive 0 entries, process name %s, pid %d",
                    __FUNCTION__,
                    current->comm, current->pid);
                return ENOSYS;
        }

        /* create queue element */
        queueMsgSize = pRecvStreamMessage->hdr.numEntries * sizeof(dspipc_scatGat_t) + sizeof(pRecvStreamMessage->hdr);

        pStreamRecvMsg = (dspipc_recvStreamMessage_t*)MALLOC_TRC(queueMsgSize);

        if (pStreamRecvMsg == NULL) {
                hb_slogf_error("%s: No mem for queue entry , process name %s, pid %d",
                    __FUNCTION__,
                    current->comm, current->pid);
                return ENOMEM;
        }

        error = 0;
        if (pRecvStreamMessage->hdr.numEntries <= NUM_SCAT_GATH_ENTRIES) {
                pStreamRecvMsg->hdr.numEntries = pRecvStreamMessage->hdr.numEntries;
                pStreamRecvMsg->hdr.cmdId = pRecvStreamMessage->hdr.cmdId;
                queueCmdId = pStreamRecvMsg->hdr.cmdId;

                /* copy all the elements */
                for (i = 0; i < pRecvStreamMessage->hdr.numEntries; i++) {
                        /* Get pointer to Entry */
                        pEntryMsg = (dspipc_scatGat_t*)&(pRecvStreamMessage->entry[i]);
                        pEntry = &(pStreamRecvMsg->entry[i]);

                        pEntry->memId = pEntryMsg->memId;
                        pShmemObj = SearchMemoryElement(pEntryMsg->memId);

                        if (pShmemObj != NULL) {
                                pEntry->mem.physAddr = pShmemObj->physAddr;
                                pEntry->mem.pVirtAddr = pShmemObj->pVirtAddr;
                                pEntry->mem.pMemHdl = (void*)pShmemObj;

                                /* check Start */
                                if ((pEntryMsg->start & 0x03) == 0) {
                                        pEntry->start = pEntryMsg->start;
                                } else {
                                        hb_slogf_error("%s: Start of range %d (%d) not alligned to longword, process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->start,
                                            current->comm, current->pid);
                                        error = 1;
                                }

                                /* check Size */
                                if ((pEntryMsg->size & 0x03) == 0) {
                                        pEntry->size = pEntryMsg->size;
                                        pEntry->remain = pEntryMsg->size;
                                        pEntry->valid = 0;
                                } else {
                                        hb_slogf_error("%s: Size of range %d (%d) not alligned to longword, process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->size,
                                            current->comm, current->pid);
                                        error = 1;
                                }

                                /* check Range */
                                if ((pEntryMsg->start + pEntryMsg->size) > pShmemObj->size) {
                                        hb_slogf_error("%s: invalid range: range:%d start:%d size:%d exceeds mem object size:%d, process name %s, pid %d",
                                            __FUNCTION__,
                                            i,
                                            pEntryMsg->start,
                                            pEntryMsg->size,
                                            pShmemObj->size,
                                            current->comm, current->pid);
                                        error = 1;
                                }

                                /* invalidate the cached stream data */
                                if ((pShmemObj->protFlags & PROT_NOCACHE) != PROT_NOCACHE) {
                                        /* shared memory object is cached */
                                        hbCacheInval((void*)((uint32_t)pShmemObj->pVirtAddr + pEntryMsg->start),
                                            (uint64_t)(pShmemObj->physAddr + pEntryMsg->start),
                                            pEntryMsg->size);
                                }
                        } else {
                                hb_slogf_error("%s: Got invalid mem id %d [PID=%d;TID=%d]",
                                    __FUNCTION__,
                                    pRecvStreamMessage->entry[i].memId,
                                    current->comm, current->pid);
                                error = 1;
                        }
                }

                queueMsgId = 0;
                if (error == 0) {
                        if (dspipc_getNewMessageId(pDevice, &queueMsgId) != 0) {
                                hb_slogf_error(GET_MESSAGE_ID_FUNC_FOR_DEVICE_FAILED,
                                    __FUNCTION__, pDevice->pDeviceName,
                                    pSubDevice->pSubDeviceName);
                                error = 1;
                        }
                }

                if (error == 0) {
                        for (i = 0; i < pRecvStreamMessage->hdr.numEntries; i++) {
                                if (IncUsageMemoryElement((memObjList_t*)pStreamRecvMsg->entry[i].mem.pMemHdl, 1) != 0) {
                                        hb_slogf_error("%s: Inc mem object usage count failed , process name %s, pid %d",
                                            __FUNCTION__, current->comm, current->pid);
                                        error = 1;
                                }
                        }
                }

                if (error == 0) {
                        pMessage.recvStream = pStreamRecvMsg;
                        if ((errCode = dspipcQueueAddMessage(pDevice,
                                 SEND_CALLED,
                                 pSubDevice->pSendMsgQueue,
                                 DSPIPC_CONT_PROVIDEBUFSTREAM,
                                 queueMsgId, queueCmdId,
                                 DSPIPC_MSGTYPE_STREAM, DSPIPC_MSGREPLY_NO,
                                 pMessage, queueMsgSize))
                            != 0) {
                                hb_slogf_error("%s: Add to queue (%s) failed %d , process name %s, pid %d",
                                    __FUNCTION__, pSubDevice->pSubDeviceName,
                                    errCode, current->comm, current->pid);
                                error = 1;
                        }
                }
        } else {
                hb_slogf_error("%s: Number of entries to high , process name %s, pid %d",
                    __FUNCTION__,
                    current->comm, current->pid);
                error = 1;
        }
        if (error != 0) {
                FREE_TRC(pStreamRecvMsg);
                return ENOSYS;
        }
        return EOK;
#endif
}

/**
    This function handles the send of a special command message. The command
    type field will be examined the the resulting operation is performed.

    @param pCtp pointer to resource manager context
    @param pDevice Pointer to the device table structure to add the new sub device
    @param pSubDevice Pointer to the sub device structure
    @param pSpecialCmdMessage Pointer to transmitted message array that will
                              contain the special command info
    @return int status 0=success or failure=return code for the ressource manager
*/
static int dspipcSpecialCmdMessage(deviceTable_t* pDevice,
    subDeviceTable_t* pSubDevice,
    const dspipc_specialCmdMessage_t* pSpecialCmdMessage)
{
        int retValue;
        retValue = 0;

        if (pSpecialCmdMessage == NULL) {
                hb_slogf_error("%s: message pointer is invalid", __FUNCTION__);
                return ENOSYS;
        }

        switch (pSpecialCmdMessage->specialCmdCode) {
        case UNBLOCK:
                if ((pSubDevice->type == SD_STREAM) || (pSubDevice->type == SD_COMMAND)) {
                        if (pSubDevice->dir == SD_DIR_IN) {
                                if (pSubDevice->status == SD_STAT_BLOCKED) {

                                        if (dspipcUnblockSubDevice(pSubDevice, ECANCELED) != 0) {
                                                hb_slogf_error("%s: Unblock failed , process name %s, pid %d",
                                                    __FUNCTION__, current->comm, current->pid);
                                                retValue = ENOSYS;
                                        }
                                }
                        } else {
                                hb_slogf_error("%s: Unblock to non input device , process name %s, pid %d",
                                    __FUNCTION__, current->comm, current->pid);
                                retValue = ENOSYS;
                        }
                } else {
                        hb_slogf_error("%s: Unblock to non stream or command device , process name %s, pid %d",
                            __FUNCTION__, current->comm, current->pid);
                        retValue = ENOSYS;
                }
                break;
        case CLEARQUEUE:
                if ((pSubDevice->type == SD_STREAM) || (pSubDevice->type == SD_COMMAND)) {
                        if (pSubDevice->dir == SD_DIR_IN) {
                                if (pSubDevice->pRecvMsgQueue != NULL) {
                                        if (dspipcQueueClear(pSubDevice->pRecvMsgQueue) != 0) {
                                                hb_slogf_error("%s: Clear queue failed , process name %s, pid %d",
                                                    __FUNCTION__, current->comm, current->pid);
                                                retValue = ENOSYS;
                                        }
                                } else {
                                        hb_slogf_error("%s: Device has no input queue , process name %s, pid %d",
                                            __FUNCTION__, current->comm, current->pid);
                                        retValue = ENOSYS;
                                }
                        } else {
                                hb_slogf_error("%s: Clear queue to non input device , process name %s, pid %d",
                                    __FUNCTION__, current->comm, current->pid);
                                retValue = ENOSYS;
                        }
                } else {
                        hb_slogf_error("%s: Clear queue to non stream or command device, process name %s, pid %d",
                            __FUNCTION__, current->comm, current->pid);
                        retValue = ENOSYS;
                }
                break;
        case DRAIN:
                if ((pSubDevice->type == SD_STREAM) || (pSubDevice->type == SD_COMMAND)) {
                        if (pSubDevice->dir == SD_DIR_OUT) {
                                if (dspipcLockSubDevices(pDevice) == EOK) {
                                        /* Check if drain is already done */
                                        if (pSubDevice->status == SD_STAT_DRAIN) {
                                                hb_slogf_error("%s: subdevice %s is in DRAIN state. No further DRAIN allowed", __FUNCTION__, pSubDevice->pSubDeviceName);
                                                retValue = EBUSY;

                                                if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                                        hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                                        retValue = ENOSYS;
                                                }
                                        } else {
                                                /* Set State to DRAIN */
                                                pSubDevice->status = SD_STAT_DRAIN;
                                                /* In case of command channel just clear the SendMsgQueue */
                                                if (pSubDevice->type == SD_COMMAND) {
                                                        if (pSubDevice->pSendMsgQueue != NULL) {
                                                                if (dspipcQueueClear(pSubDevice->pSendMsgQueue) != 0) {
                                                                        hb_slogf_error("%s: Clear queue failed , process name %s, pid %d",
                                                                            __FUNCTION__, current->comm, current->pid);
                                                                        retValue = ENOSYS;
                                                                } else {
                                                                        /* Set Status back to Ready */
                                                                        pSubDevice->status = SD_STAT_READY;
                                                                }
                                                        } else {
                                                                hb_slogf_error("%s: Device has no output queue , process name %s, pid %d",
                                                                    __FUNCTION__, current->comm, current->pid);
                                                                retValue = ENOSYS;
                                                        }

                                                        if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                                                hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                                                retValue = ENOSYS;
                                                        }
                                                } else /* SD_STREAM */
                                                {
                                                        pDevice->drain_Avail_Cnt++;
                                                        if (WakeupSendThread(pDevice) == 0) {
                                                                if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                                                        hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                                                        retValue = ENOSYS;
                                                                }
                                                                /* wait for drain Complete from DSP */
                                                                if (dspipcWaitDrainComplete(pSubDevice, 100) != 0) {
                                                                        hb_slogf_error("%s: Drain dspipcWaitDrainComplete not received withing 100ms <%s>",
                                                                            __FUNCTION__, pSubDevice->pSubDeviceName);
                                                                        retValue = ENOSYS;
                                                                }

                                                                /* Set Drain Available Counter back */
                                                                if (dspipcLockSubDevices(pDevice) == EOK) {
                                                                        pDevice->drain_Avail_Cnt--;
                                                                        if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                                                                hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                                                                retValue = ENOSYS;
                                                                        }
                                                                } else {
                                                                        hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                                                        retValue = ENOSYS;
                                                                }
                                                        } else {
                                                                hb_slogf_error("%s: Wake send failed", __FUNCTION__);
                                                                retValue = ENOSYS;
                                                        }
                                                }
                                        } /* End Check if device is already in DRAIN State */
                                } else {
                                        hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                        retValue = ENOSYS;
                                }
                        } else {
                                hb_slogf_error("%s: Drain to non output device , process name %s, pid %d",
                                    __FUNCTION__, current->comm, current->pid);
                                retValue = EINVAL;
                        }
                } else {
                        hb_slogf_error("%s: Drain to non stream or command device , process name %s, pid %d",
                            __FUNCTION__, current->comm, current->pid);
                        retValue = EINVAL;
                }
                break;
        default:
                hb_slogf_error("%s: Unknown special command code , process name %s, pid %d",
                    __FUNCTION__,
                    current->comm, current->pid);
                retValue = EINVAL;
                break;
        }

        return (retValue);
}

static int dspipc_open(struct inode* inode, struct file* filp)
{
        uint32_t major_num;
        uint32_t minor_num;
        int32_t errCode;
        deviceTable_t* pDevice = NULL;
        subDeviceTable_t* pSubDevice = NULL;
        subDeviceTable_t* pTargetSubDevice = NULL;

        DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s:called!\n", __FUNCTION__);

        if (!init_success) {
                DEBUG_ERROR("%s:initialization is not finished!\n", __FUNCTION__);
                return -1;
        }

        major_num = imajor(inode);
        minor_num = iminor(inode);

        pDevice = pDeviceTableRoot;
        while (pDevice) {
                if (pDevice->major_num == major_num) {
                        errCode = dspipcLockSubDevices(pDevice);
                        if (errCode == EOK) {
                                pSubDevice = pDevice->pSubDevices;
                                while (pSubDevice) {
                                        if (pSubDevice->minor_num == minor_num && (pSubDevice->status == SD_STAT_READY || pSubDevice->status == SD_STAT_BLOCKED)) {
                                                pTargetSubDevice = pSubDevice;
                                                DEBUG_INFO_LEVEL(LEVEL_V, "%s: io_open, we found the right sub device, major %d, minor %d\n", __FUNCTION__, major_num, minor_num);
                                                break;
                                        }
                                        pSubDevice = pSubDevice->pNext;
                                }
                                dspipcUnlockSubDevices(pDevice);
                        } else {
                                DEBUG_ERROR("%s: dspipcLockSubDevice failed\n", __FUNCTION__);
                        }
                        break;
                }
                pDevice = pDevice->pNext;
        }

        if (pTargetSubDevice == NULL) {
                DEBUG_ERROR("%s: Didn't find sub device, major %d, minor %d, open failed!\n", __FUNCTION__, major_num, minor_num);
                return -1;
        }

        (pTargetSubDevice->numOpenFd)++;
        filp->private_data = pTargetSubDevice;
        return 0;
}

static int dspipc_release(struct inode* inode, struct file* filp)
{
        uint32_t major_num;
        uint32_t minor_num;
        int32_t errCode;
        deviceTable_t* pDevice = NULL;
        subDeviceTable_t* pSubDevice = NULL;
        subDeviceTable_t* pTargetSubDevice = NULL;

        DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s:called!\n", __FUNCTION__);

        if (!init_success) {
                DEBUG_ERROR("%s:initialization is not finished!\n", __FUNCTION__);
                return -1;
        }

        major_num = imajor(inode);
        minor_num = iminor(inode);

        pDevice = pDeviceTableRoot;
        while (pDevice) {
                if (pDevice->major_num == major_num) {
                        errCode = dspipcLockSubDevices(pDevice);
                        if (errCode == EOK) {
                                pSubDevice = pDevice->pSubDevices;
                                while (pSubDevice) {
                                        if (pSubDevice->minor_num == minor_num && (pSubDevice->status == SD_STAT_READY || pSubDevice->status == SD_STAT_BLOCKED)) {
                                                pTargetSubDevice = pSubDevice;
                                                break;
                                        }
                                        pSubDevice = pSubDevice->pNext;
                                }
                                dspipcUnlockSubDevices(pDevice);
                        } else {
                                DEBUG_ERROR("%s: dspipcLockSubDevice failed\n", __FUNCTION__);
                        }
                        break;
                }
                pDevice = pDevice->pNext;
        }

        if (pTargetSubDevice == NULL) {
                DEBUG_ERROR("%s: Didn't find sub device, major %d, minor %d, close failed!\n", __FUNCTION__, major_num, minor_num);
                return -1;
        }
        /*
   else
   {
      DEBUG_ERROR("%s: sub device status:%d!\n", __FUNCTION__, (int)(pTargetSubDevice->status));
   }
   */
        (pTargetSubDevice->numOpenFd)--;
        filp->private_data = NULL;

        if (pTargetSubDevice->numOpenFd == 0) {
                switch (pTargetSubDevice->status) {
                case SD_STAT_BLOCKED:
                        /* unblock the client */
                        if (dspipcUnblockSubDevice(pTargetSubDevice, EBADF) != 0) {
                                hb_slogf_error("%s: Unblock %s failed", __FUNCTION__, pTargetSubDevice->pSubDeviceName);
                                return -1;
                        }
                        break;
                case SD_STAT_CLOSED:
                case SD_STAT_REBOOT: /* ???? */
                        /* delete the element */
                        if (dspipcDeleteSubDevice(pTargetSubDevice->pDevice, pTargetSubDevice) != 0) {
                                hb_slogf_error("%s: delete subdevice %s failed", __FUNCTION__, pTargetSubDevice->pSubDeviceName);
                                return -1;
                        } else {
                                hb_slogf_error("%s: delete subdevice %s sucess", __FUNCTION__, pTargetSubDevice->pSubDeviceName);
                        }

                        break;
                default:
                        break;
                }
        }

        return 0;
}

static ssize_t dspipc_read(struct file* filp, char __user* buf, size_t size, loff_t* ppos)
{
        DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s:should not be called!\n", __FUNCTION__);
        return -1;
}

static ssize_t dspipc_write(struct file* filp, const char __user* buf, size_t size, loff_t* ppos)
{
        DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s:should not be called!\n", __FUNCTION__);
        return -1;
}

static loff_t dspipc_llseek(struct file* filp, loff_t offset, int whence)
{
        DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s:called!\n", __FUNCTION__);
        return 0;
}

static long dspipc_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)
{
        long ret = 0;
        dspipc_devctl_msg_t* pData;
        deviceTable_t* pDevice;
        subDeviceTable_t* pSubDevice;
        const char* pErrorMsg = NULL;
        int numElements;
        int nbytes;
        dspipc_devctl_msg_t cmd_data;
        dspipc_reqMemObj_t* pReqMemObj;
        memObjList_t* pMemObjElement;
        dspipc_delMemObj_t* pDelMemObj;
        int delayRemain;

        DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s:called! cmd 0x%08x\n", __FUNCTION__, cmd);

        pData = (dspipc_devctl_msg_t*)arg;

        pSubDevice = (subDeviceTable_t*)filp->private_data;
        if (pSubDevice == NULL) {
                DEBUG_ERROR("%s: Subdevice is NULL, command 0x%08x\n", __FUNCTION__, cmd);
                return -1;
        }

        pDevice = pSubDevice->pDevice;
        if (pDevice == NULL) {
                DEBUG_ERROR("%s: pDevice is NULL, command 0x%08x\n", __FUNCTION__, cmd);
                return -1;
        }

        if (cmd != DSPIPC_DELMEMOBJ) {
                if (pDevice->deviceState == STATE_DSP_REBOOT) {
                        DEBUG_ERROR("%s: driver in DSP REBOOT state, command 0x%08x\n", __FUNCTION__, cmd);
                        return -1;
                }
        }

        switch (cmd) {
        case DSPIPC_GETDRVVERS:
        case DSPIPC_GETDSPSWVERS:
        case DSPIPC_MEMTRACECMD:
                if (pSubDevice->type != SD_CONTROL) {
                        pErrorMsg = "non control";
                        ret = -1;
                }
                break;
        case DSPIPC_GET_DSP_IMAGE_VERSION:
        case DSPIPC_REQMEMOBJ:
        case DSPIPC_DELMEMOBJ:
                break;
        case DSPIPC_SENDMESSAGE:
        case DSPIPC_GETMESSAGE:
        case DSPIPC_SPECIALCMD:
                if ((pSubDevice->type != SD_STREAM) && (pSubDevice->type != SD_COMMAND)) {
                        pErrorMsg = "non command or stream";
                        ret = -1;
                }
                break;
        case DSPIPC_REBOOT:
                break;
        case DSPIPC_SENDSTREAMMESSAGE:
        case DSPIPC_RECVSTREAMMESSAGE:
                if (pSubDevice->type != SD_STREAM) {
                        pErrorMsg = "non stream";
                        ret = -1;
                }
                break;
        default:
                break;
        }
        if (ret != 0) {
                DEBUG_ERROR("%s: Error <%s>, command 0x%08x\n", __FUNCTION__, pErrorMsg, cmd);
                return ret;
        }

        /* Do we need check the direction? */
        switch (cmd) {
        case DSPIPC_GETDRVVERS:
        case DSPIPC_GETDSPSWVERS:
        case DSPIPC_MEMTRACECMD:
                if (pSubDevice->dir != SD_DIR_NODIR) {
                        pErrorMsg = "not bidirectional";
                        ret = -1;
                }
                break;
        case DSPIPC_SPECIALCMD:
                if ((pSubDevice->dir != SD_DIR_IN) && (pSubDevice->dir != SD_DIR_OUT)) {
                        pErrorMsg = "non input or output";
                        ret = -1;
                }
                break;
        case DSPIPC_REQMEMOBJ:
        case DSPIPC_DELMEMOBJ:
                if (pSubDevice->dir == SD_DIR_IN) {
                        pErrorMsg = "input";
                        ret = -1;
                }
                break;
        case DSPIPC_SENDMESSAGE:
        case DSPIPC_SENDSTREAMMESSAGE:
        case DSPIPC_RECVSTREAMMESSAGE:
                if (pSubDevice->dir != SD_DIR_OUT) {
                        pErrorMsg = "not output";
                        ret = -1;
                }
                break;
        case DSPIPC_REBOOT:
                break;
        case DSPIPC_GETMESSAGE:
                if (pSubDevice->dir != SD_DIR_IN) {
                        pErrorMsg = "not input";
                        ret = -1;
                }
                break;
        default:
                break;
        }

        if (ret != 0) {
                DEBUG_ERROR("%s: Error <%s>, command 0x%08x\n", __FUNCTION__, pErrorMsg, cmd);
                return ret;
        }

        /* check for device STATUS */
        switch (cmd) {
        case DSPIPC_GETDRVVERS:
        case DSPIPC_GETDSPSWVERS:
        case DSPIPC_REQMEMOBJ:
        case DSPIPC_DELMEMOBJ:
        case DSPIPC_SPECIALCMD:
        case DSPIPC_MEMTRACECMD:
                /* these commands are working also with closed channels */
                break;
        case DSPIPC_SENDMESSAGE:
        case DSPIPC_SENDSTREAMMESSAGE:
        case DSPIPC_RECVSTREAMMESSAGE:
        case DSPIPC_REBOOT:
                /* these commands are working only with not closed channels */
                switch (pSubDevice->status) {
                case SD_STAT_REBOOT:
                        ret = -EAGAIN;
                        break;
                case SD_STAT_BLOCKED:
                        break;
                case SD_STAT_DRAIN:
                        ret = -EBUSY;
                        break;
                case SD_STAT_READY:
                        break;
                default:
                        pErrorMsg = "removed";
                        ret = -EBADF;
                        break;
                }
                break;
        case DSPIPC_GETMESSAGE:
                /* these commands are working with not closed channels or if
          * the channel is closed only as long as messages are inside the
          * queue
          */
                switch (pSubDevice->status) {
                case SD_STAT_BLOCKED:
                        /*ret = -EAGAIN; */
                        break;
                case SD_STAT_REBOOT:
                        ret = -EAGAIN;
                        break;
                case SD_STAT_DRAIN:
                        ret = -EBUSY;
                        break;
                case SD_STAT_READY:
                        break;
                case SD_STAT_CLOSED:
                        /* check if queue is empty */
                        if (dspipcQueueNumAvail(pSubDevice->pRecvMsgQueue, &numElements) == 0) {
                                if (numElements == 0) {
                                        pErrorMsg = "removed";
                                        ret = -ENODEV;
                                }
                        } else {
                                pErrorMsg = "has queue error";
                                ret = -ENOSYS;
                        }
                        break;
                default:
                        pErrorMsg = "removed";
                        ret = -ENODEV;
                        break;
                }
                break;
        default:
                break;
        }

        if (ret != 0) {
                DEBUG_ERROR("%s: Error <%s>, command 0x%08x\n", __FUNCTION__, pErrorMsg, cmd);
                return ret;
        }

        switch (cmd) {
        case DSPIPC_GETDRVVERS:
                cmd_data.drvVers.drvMajor = DSP_IPC_VERSION_MAJOR;
                cmd_data.drvVers.drvMinor = DSP_IPC_VERSION_MINOR;
                cmd_data.drvVers.drvBugfix = DSP_IPC_VERSION_BUGFIX;
                nbytes = sizeof(dspipc_drvVers_t);
                if (copy_to_user((dspipc_devctl_msg_t __user*)arg, &cmd_data, nbytes)) {
                        pErrorMsg = "copy_to_user error";
                        ret = -1;
                }
                break;

        case DSPIPC_GETDSPSWVERS:
                if (pDevice->getDspSwVers != NULL) {
                        if (((*(pDevice->getDspSwVers))((void*)pSubDevice,
                                (dspipc_dspSwVers_t*)&cmd_data))
                            == 0) {
                                nbytes = sizeof(dspipc_dspSwVers_t);
                                if (copy_to_user((dspipc_devctl_msg_t __user*)arg, &cmd_data, nbytes)) {
                                        pErrorMsg = "copy_to_user error";
                                        ret = -1;
                                }
                        } else {
                                pErrorMsg = "Get DSP software version";
                                ret = -ENOSYS;
                        }
                } else {
                        pErrorMsg = "Get DSP software version not available";
                        ret = -ENOSYS;
                }
                break;
        case DSPIPC_REQMEMOBJ:
                /* request shared memory object */
                if (!copy_from_user(&cmd_data, (dspipc_devctl_msg_t __user*)arg, sizeof(dspipc_reqMemObj_t))) {
                        pReqMemObj = &cmd_data.reqMemObj;
                        if ((ret = RequestMemoryElement(pReqMemObj, &pMemObjElement, 0)) == 0) {
                                nbytes = sizeof(dspipc_reqMemObj_t);
                                if (copy_to_user((dspipc_devctl_msg_t __user*)arg, &(cmd_data.reqMemObj), nbytes)) {
                                        pErrorMsg = "copy_to_user error";
                                        ret = -1;
                                }
                        } else {
                                pErrorMsg = "Request memory element fail";
                                ret = ENOSYS;
                        }

                        /* This Delay prevents this function to fail */
                        /* Give the Kernel time to restucture the Memory Information (PR39898) */
                        delayRemain = 1;
                        mdelay(delayRemain);
                } else {
                        pErrorMsg = "copy_from_user error";
                        ret = -1;
                }
                break;

        case DSPIPC_DELMEMOBJ:
                /* delete (remove) shared memory object */
                if (!copy_from_user(&cmd_data, (dspipc_devctl_msg_t __user*)arg, sizeof(dspipc_delMemObj_t))) {
                        pDelMemObj = &cmd_data.delMemObj;
                        pMemObjElement = SearchMemoryElement(pDelMemObj->memId);
                        if (pMemObjElement != NULL) {
                                if ((ret = RemoveMemoryElement_hold(pDelMemObj, 1)) != 0) {
                                        pErrorMsg = "Remove memory elemen fail";
                                        ret = EBUSY;
                                }
                        } else {
                                pErrorMsg = "Delete memory object no exsit ";
                                ret = ENOSYS;
                        }

                        /* This Delay prevents this function to fail */
                        /* Give the Kernel time to restucture the Memory Information (PR39898) */
                        delayRemain = 1;
                        mdelay(delayRemain);
                } else {
                        pErrorMsg = "copy_from_user error";
                        ret = -1;
                }
                break;

        case DSPIPC_SENDMESSAGE:
                if (EOK != pthread_mutex_lock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_lock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }

                if (!copy_from_user(&cmd_data, (dspipc_devctl_msg_t __user*)arg, sizeof(dspipc_sendMessage_t))) {
                        if ((ret = dspipcSendMessage(pDevice, pSubDevice, &(cmd_data.sendMessage))) != 0) {
                                pErrorMsg = "Send message";
                        }
                } else {
                        pErrorMsg = "copy_from_user error";
                        ret = -1;
                }

                if (EOK != pthread_mutex_unlock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_unlock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }

                break;

        case DSPIPC_GETMESSAGE:
                if (EOK != pthread_mutex_lock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_lock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }

                if (!copy_from_user(&cmd_data, (dspipc_devctl_msg_t __user*)arg, sizeof(dspipc_getMessage_t))) {
                        if ((ret = dspipcGetMessage(pDevice, pSubDevice,
                                 &(cmd_data.getMessage),
                                 &nbytes, NULL))
                            != 0) {
                                /*reduce output log for read time out, mark as normal*/
                                if (ret != -ECANCELED) {
                                        pErrorMsg = "Get message";
                                }
                        } else {
                                if (copy_to_user((dspipc_devctl_msg_t __user*)arg, &(cmd_data.getMessage), nbytes)) {
                                        pErrorMsg = "copy_to_user error";
                                        ret = -1;
                                }
                        }
                } else {
                        pErrorMsg = "copy_from_user error";
                        ret = -1;
                }

                if (EOK != pthread_mutex_unlock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_unlock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }

                break;

        case DSPIPC_SENDSTREAMMESSAGE:
                if (EOK != pthread_mutex_lock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_lock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }
                if (!copy_from_user(&cmd_data, (dspipc_devctl_msg_t __user*)arg, sizeof(dspipc_sendStreamMessage_t))) {
                        if ((ret = dspipcSendStreamMessage(pDevice, pSubDevice,
                                 &(cmd_data.sendStreamMessage)))
                            != 0) {
                                pErrorMsg = "Send stream message erorr";
                        }
                } else {
                        pErrorMsg = "copy_from_user error";
                        ret = -1;
                }

                if (EOK != pthread_mutex_unlock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_unlock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }

                break;

        case DSPIPC_RECVSTREAMMESSAGE:
                if (EOK != pthread_mutex_lock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_lock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }
                if (!copy_from_user(&cmd_data, (dspipc_devctl_msg_t __user*)arg, sizeof(dspipc_recvStreamMessage_t))) {
                        if ((ret = dspipcRecvStreamMessage(pDevice, pSubDevice,
                                 &(cmd_data.recvStreamMessage)))
                            != 0) {
                                pErrorMsg = "Receive stream message error";
                        }
                } else {
                        pErrorMsg = "copy_from_user error";
                        ret = -1;
                }

                if (EOK != pthread_mutex_unlock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_unlock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }

                break;

        case DSPIPC_SPECIALCMD:
                if (EOK != pthread_mutex_lock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_lock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }
                if (!copy_from_user(&cmd_data, (dspipc_devctl_msg_t __user*)arg, sizeof(dspipc_specialCmdMessage_t))) {
                        if ((ret = dspipcSpecialCmdMessage(pDevice, pSubDevice,
                                 &(cmd_data.specialCmdMessage)))
                            != 0) {
                                pErrorMsg = "special command message";
                        }
                } else {
                        pErrorMsg = "copy_from_user error";
                        ret = -1;
                }

                if (EOK != pthread_mutex_unlock(&(pSubDevice->thisMutex))) {
                        pErrorMsg = "pthread_mutex_unlock error";
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                        return (-1);
                }

                break;

        default:
                DEBUG_ERROR("%s: wrong ioctl command, command 0x%08x\n", __FUNCTION__, cmd);
                ret = -1;
                break;
        }

        if (ret != 0) {
                if (pErrorMsg != NULL) {
                        hb_slogf_error("%s: CH%d %s failed [PNAME=%s;PID=%d]",
                            __FUNCTION__, pSubDevice->channelNum, pErrorMsg, current->comm, current->pid);
                }
        }
        return ret;
}

/* this poll is just used for read only*/
static unsigned int dspipc_poll(struct file* filp, poll_table* wait)
{
        subDeviceTable_t* pSubDevice;
        unsigned int mask = 0;
        int retValue = 0;
        int numElements = 0;
        int errCode;

        pSubDevice = (subDeviceTable_t*)filp->private_data;
        if (pSubDevice == NULL) {
                DEBUG_ERROR("%s: Subdevice is NULL\n", __FUNCTION__);
                return -1;
        }

        pSubDevice->status = SD_STAT_BLOCKED;
        poll_wait(filp, &(pSubDevice->pRecvMsgQueue->inWaitQueue), wait);

        retValue = dspipcQueueNumAvail(pSubDevice->pRecvMsgQueue, &numElements);
        if (retValue != 0) {
                hb_slogf_error("%s:dspipcQueueNumAvail() failed", __FUNCTION__);
                return 0;
        }

        /* if there is data can be read, set POLLIN event. */
        if (numElements > 0) {
                mask |= POLLIN | POLLRDNORM;
        } else {
                switch (pSubDevice->pRecvMsgQueue->unblockReason) {
                case ETIMEDOUT:
                case EBADF:
                case ECANCELED:
                case EBUSY:
                        errCode = pthread_mutex_lock(&(pSubDevice->pRecvMsgQueue->condMutex));
                        if (errCode != EOK) {
                                hb_slogf_error(BUFFER_QUEUE_MUTEX_LOCK_FAILED, __FUNCTION__, (errCode));
                                retValue = -1;
                        }

                        pSubDevice->pRecvMsgQueue->unblockReason = 0;

                        errCode = pthread_mutex_unlock(&(pSubDevice->pRecvMsgQueue->condMutex));
                        if (errCode != EOK) {
                                hb_slogf_error(BUFFER_QUEUE_MUTEX_UNLOCK_FAILED, __FUNCTION__, (errCode));
                                retValue = -1;
                        }

                        mask |= POLLERR;

                        break;
                default:
                        break;
                }
        }
        return mask;
}

static struct file_operations dspipc_fops = {
        .owner = THIS_MODULE,
        .llseek = dspipc_llseek,
        .read = dspipc_read,
        .write = dspipc_write,
        .open = dspipc_open,
        .release = dspipc_release,
        .unlocked_ioctl = dspipc_ioctl,
        .poll = dspipc_poll,
};

struct file_operations* dspipc_getfops(void)
{
        return (&dspipc_fops);
}
