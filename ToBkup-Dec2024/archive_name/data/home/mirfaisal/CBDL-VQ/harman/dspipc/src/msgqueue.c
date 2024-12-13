/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file msgqueue.c
 *
 * This is the file containing the functions to handle the message queues for
 * the dev-dspipc communication scheme.
 *
 *
 * @author Andreas Heiner
 * @author Andreas Abel
 * @date   03.03.2006
 *
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

/*============================================================================== */
/*=========================    Includes   ====================================== */
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
#include <linux/wait.h>

#include "debug.h"
#include "dev-dspipc.h"
#include "dspipc_pthread.h"
#include "dspipcproto.h"
#include "malloc_trc.h"
#include "misc.h"
#include "string_def_ext.h"

/*============================================================================== */
/*=========================   Define's   ======================================= */
#define __TRACE_ON

#ifdef TRACE_ON
#include <sys/trace.h> /*for TraceEvent() */
#define TRACEEVENT(a, b, c) (void)TraceEvent(a, b, c);
#else
#define TRACEEVENT(a, b, c)
#endif

/*============================================================================== */
/*=========================    Macro's   ======================================= */

/* none */

/*============================================================================== */
/*===================    Modul Global Variables   ============================== */

/* none */

/*============================================================================== */
/*===================    forward declarations    =============================== */

/* none */

/*============================================================================== */
/*=======================    Global Variables    =============================== */

/* none */

/*============================================================================== */
/*===================    Function Definitions    =============================== */

/**
   dspipcQueueCreate create an empty message queue containing the specified
   amount of message elements

   @param pDevice Device for this queue
   @param numElements Number of possible elements inside the queue
   @return msgQueue_t* Pointer to the message queue, NULL in case of failure
*/
msgQueue_t* dspipcQueueCreate(const deviceTable_t* pDevice, unsigned int numElements)
{
        msgQueue_t* pQueue;
        int errCode;
        int i, j;

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (NULL);
        }

        if (numElements == 0) {
                /* No queue required */
                return (NULL);
        }

        pQueue = (msgQueue_t*)MALLOC_TRC(sizeof(msgQueue_t));
        if (pQueue != NULL) {
                pQueue->numElements = numElements;
                pQueue->numMessagesLost = 0;
                pQueue->readIndex = 0;
                pQueue->writeIndex = 0;
                pQueue->unblockReason = 0;

                pQueue->pArray = (msgQueueElement_t*)CALLOC_TRC(numElements,
                    sizeof(msgQueueElement_t));
                if (pQueue->pArray != NULL) {
                        errCode = pthread_mutex_init(&(pQueue->condMutex), NULL);
                        if (errCode == EOK) {
                                errCode = pthread_cond_init(&(pQueue->condVar), NULL);
                                if (errCode == EOK) {
                                        init_waitqueue_head(&(pQueue->inWaitQueue));
                                        if (errCode == EOK) {
                                                for (i = 0; i < numElements; i++) {
                                                        pQueue->pArray[i].pMsg.generic = (void*)CALLOC_TRC(1, MESSAGE_POINTER_U_SIZE);
                                                        if (pQueue->pArray[i].pMsg.generic == NULL) {
                                                                errCode++;
                                                                break;
                                                        }
                                                }
                                                if (errCode == EOK) {
                                                        return (pQueue);
                                                } else {
                                                        hb_slogf_error("%s: Failed to calloc buf in message queue(%d)", __FUNCTION__, (errCode));
                                                        for (j = 0; j < i; j++) {
                                                                FREE_TRC(pQueue->pArray[j].pMsg.generic);
                                                                pQueue->pArray[j].pMsg.generic = NULL;
                                                        }
                                                        FREE_TRC(pQueue->pArray);
                                                        pQueue->pArray = NULL;
                                                        pthread_mutex_destroy(&(pQueue->condMutex));
                                                }

                                        } else {
                                                hb_slogf_error("%s: init in wait queue failed (%d)", __FUNCTION__, (errCode));
                                        }

                                } else {
                                        hb_slogf_error("%s: init cond var failed (%d)", __FUNCTION__, (errCode));
                                }
                                /* cleanup initialized mutex */
                                errCode = pthread_mutex_destroy(&(pQueue->condMutex));
                                if (errCode != EOK) {
                                        hb_slogf_error("%s: destroy mutex failed (%d)", __FUNCTION__, (errCode));
                                }
                        } else {
                                hb_slogf_error("%s: init mutex failed (%d)", __FUNCTION__, (errCode));
                        }
                        /* release queue element array */
                        FREE_TRC(pQueue->pArray);
                        pQueue->pArray = NULL;
                } else {
                        hb_slogf_error("%s: no mem for queue elements", __FUNCTION__);
                }
                /* release queue */
                FREE_TRC(pQueue);
                pQueue = NULL;
        } else {
                hb_slogf_error("%s: no mem for queue", __FUNCTION__);
        }

        return (NULL);
}

/**
   dspipcQueueDestroy removes a complete queue structure, all memory is released

   @param pQueue Pointer to the message queue
   @return int Error return code, 0 for success, -1 for failure
*/
int dspipcQueueDestroy(msgQueue_t* pQueue)
{
        int retValue = 0;
        int errCode;
        int i;

        if (pQueue == NULL) {
                hb_slogf_error("%s: No queue", __FUNCTION__);
                return (-1);
        }

        /* destroy initialized mutex */
        errCode = pthread_mutex_destroy(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error("%s: destroy mutex failed (%d)", __FUNCTION__, (errCode));
                retValue = -1;
        }

        /* destroy initialized cond var */
        errCode = pthread_cond_destroy(&(pQueue->condVar));
        if (errCode != EOK) {
                hb_slogf_error("%s: destroy cond var failed (%d)", __FUNCTION__, (errCode));
                retValue = -1;
        }

        for (i = 0; i < pQueue->numElements; i++) {
                if (pQueue->pArray[i].pMsg.generic != NULL) {
                        FREE_TRC(pQueue->pArray[i].pMsg.generic);
                        pQueue->pArray[i].pMsg.generic = NULL;
                }
        }

        FREE_TRC(pQueue->pArray);
        pQueue->pArray = NULL;

        FREE_TRC(pQueue);
        pQueue = NULL;

        return (retValue);
}

/**
 * dspipcQueueAddMessage stores the message as next element inside the incoming
 * message queue
 *
 * @param pDevice Pointer to the device table structure (needed for WakeupSendThread() call)
 * @param calledBy Source of this packet (SEND_CALLED or RECEIVE_CALLED)
 * @param pQueue Pointer to the message queue
 * @param contentType Specify type of the content (ANY for any)
 * @param msgId Message id of this message
 * @param cmdId Command id of this message
 * @param msgType Type of the message (DSPIPC_MSGTYPE_COMMAND | DSPIPC_MSGTYPE_STREAM)
 * @param msgReply Specify if an internal reply is requested (DSPIPC_MSGREPLY_NO | DSPIPC_MSGREPLY_YES)
 * @param pMsg Pointer to message array
 * @param msgSize Size of the message
 * @return int Error return code, 0 for success, -1 for failure, -2 for message skip
 */
int dspipcQueueAddMessage(deviceTable_t* pDevice,
    queueMsgCaller_t calledBy,
    msgQueue_t* pQueue,
    dspipc_msgContent_t contentType,
    uint32_t msgId,
    uint32_t cmdId,
    dspipc_msgType_t msgType,
    dspipc_msgReply_t msgReply,
    messagePointer_t pMsg, uint32_t msgSize)
{
        int retValue = 0;
        int errCode;
        int nextWriteIndex;
        msgQueueElement_t* pQueueElement;
        int sendWakeupMarker;

        if (pQueue == NULL) {
                hb_slogf_error("%s: No queue", __FUNCTION__);
                return (-1);
        }

        if (pMsg.generic == NULL) {
                hb_slogf_error("%s: No message", __FUNCTION__);
                return (-1);
        }

        if ((pQueue->writeIndex >= pQueue->numElements) || (pQueue->numElements == 0)) {
                hb_slogf_error("%s: Write index out of bounds %d (max %d)", __FUNCTION__, pQueue->writeIndex, pQueue->numElements);
                return (-1);
        }

        sendWakeupMarker = 0;

        errCode = pthread_mutex_lock(&(pQueue->condMutex));
        if (errCode == EOK) {
                TRACEEVENT(_NTO_TRACE_INSERTUSRSTREVENT, 0, (calledBy == SEND_CALLED) ? "Queue add SEND_CALLED" : "Queue add RECEIVE_CALLED");

                nextWriteIndex = pQueue->writeIndex + 1;
                /* check for wrap around */
                if (nextWriteIndex >= pQueue->numElements) {
                        nextWriteIndex = 0;
                }

                if (nextWriteIndex != pQueue->readIndex) {
                        /* an element is available */
                        pQueueElement = &(pQueue->pArray[pQueue->writeIndex]);

                        pQueueElement->msgId = msgId;
                        pQueueElement->cmdId = cmdId;
                        pQueueElement->msgSize = msgSize;
                        pQueueElement->msgType = msgType;
                        pQueueElement->msgReply = msgReply;
                        pQueueElement->contType = contentType;
                        memcpy(pQueueElement->pMsg.generic, pMsg.generic, msgSize);
                        pQueueElement->numLostBefore = pQueue->numMessagesLost;
                        pQueue->numMessagesLost = 0;
                        pQueue->writeIndex = nextWriteIndex;

                        if (calledBy == RECEIVE_CALLED) {
                                errCode = pthread_cond_signal(&(pQueue->condVar));
                                if (errCode != EOK) {
                                        hb_slogf_error("%s: Queue cond signal failed <%d>", __FUNCTION__, (errCode));
                                        retValue = -1;
                                }
                                wake_up_interruptible(&(pQueue->inWaitQueue)); /* added by yozheng */
                        }

                        if (calledBy == SEND_CALLED) {
                                sendWakeupMarker = 1; /* erst Mutex wieder freigeben!!! Dann Wakeup SndThread */
                        }
                } else {
                        /* queue is full */
                        pQueue->numMessagesLost += 1;
#ifdef VVVV_ENABLE
                        if (gCmdOptions.optv > 2) {
                                hb_slogf_error("%s: Queue full skip %s messages (total skipped = %d)",
                                    __FUNCTION__, (calledBy == RECEIVE_CALLED) ? "receive" : "send",
                                    pQueue->numMessagesLost);
                                if (pDevice->dumpStateFunc != NULL) {
                                        if ((*(pDevice->dumpStateFunc))((void*)pDevice) != 0) {
                                                hb_slogf_error("%s: Dump state func failed for device %s",
                                                    __FUNCTION__, pDevice->pDeviceName);
                                        }
                                }
                        }
#endif
                        retValue = -2;
                }

                if (sendWakeupMarker != 0) {
                        errCode = pthread_mutex_lock(&(pDevice->sendCntMutex));
                        if (errCode == EOK) {
                                /* incement Send Counter */
                                pDevice->dev_send_counter++;

                                errCode = pthread_mutex_unlock(&(pDevice->sendCntMutex));
                                if (errCode != EOK) {
                                        hb_slogf_error("%s: SendCnt %s <%d>", __FUNCTION__, MUTEX_UNLOCK_FAILED, (errCode));
                                        retValue = -1;
                                }
                        } else {
                                hb_slogf_error("%s: SendCnt %s <%d>", __FUNCTION__, MUTEX_LOCK_FAILED, (errCode));
                                retValue = -1;
                        }
                }

                errCode = pthread_mutex_unlock(&(pQueue->condMutex));
                if (errCode != EOK) {
                        hb_slogf_error("%s: Queue %s <%d>", __FUNCTION__, MUTEX_UNLOCK_FAILED, (errCode));
                        retValue = -1;
                }

        } else {
                hb_slogf_error("%s: Queue %s <%d>", __FUNCTION__, MUTEX_LOCK_FAILED, (errCode));
                retValue = -1;
        }

        if (sendWakeupMarker != 0) {
                if (WakeupSendThread(pDevice) != 0) {
                        hb_slogf_error("%s: Wakeup sendthread failed", __FUNCTION__);
                        retValue = -1;
                }
        }
        return (retValue);
}

/**
   dspipcQueueGetMessage get the next element out of the incoming message queue

   @param pQueue Pointer to the message queue
   @param pContentType Pointer to content type, on call specified required type
                       (ANY for any), after call contains the delivered type
   @param pMsgId Pointer to variable for delivering the message id (NULL if not required)
   @param pCmdId Pointer to variable for delivering the command id (NULL if not required)
   @param pMsgType Pointer to store the type of the message
                   (DSPIPC_MSGTYPE_COMMAND | DSPIPC_MSGTYPE_STREAM)
   @param pMsgReply Pointer to store if an internal reply is requested
                    (DSPIPC_MSGREPLY_NO | DSPIPC_MSGREPLY_YES)
   @param ppMsg Pointer to store Pointer to message array
   @param pMsgSize Pointer to variable to store the size of the message
   @param pNumLost Pointer to variable for storing the number of lost messages before
                   this message (NULL if not required)
   @param pAttr Pointer to an attribute structure for locking/unlocking the
                ressource manager framework
   @return int Error return code, 0 for success,
                                  -1 for failure,
                                  1 for other content available
                                  2 for unblocked without data (timeout)
                                  3 for unblocked without data (device removed)
                                  4 for unblocked by devctl
*/
int dspipcQueueGetMessage(msgQueue_t* pQueue,
    dspipc_msgContent_t* pContentType,
    uint32_t* pMsgId, uint32_t* pCmdId,
    dspipc_msgType_t* pMsgType,
    dspipc_msgReply_t* pMsgReply,
    messagePointer_t* ppMsg, uint32_t* pMsgSize, uint32_t* pNumLost,
    iofunc_attr_t* pAttr)
{
        int retValue = -1;
        int errCode;
        int nextReadIndex;
        msgQueueElement_t* pQueueElement;

        if (pQueue == NULL) {
                hb_slogf_error("%s: No queue", __FUNCTION__);
                return (-1);
        }

        if (ppMsg == NULL) {
                hb_slogf_error("%s: No message pointer", __FUNCTION__);
                return (-1);
        }
        if (pMsgSize == NULL) {
                hb_slogf_error("%s: No message size pointer", __FUNCTION__);
                return (-1);
        }
        *pMsgSize = 0;

        if (pQueue->pArray == NULL) {
                hb_slogf_error("%s: No array pointer", __FUNCTION__);
                return (-1);
        }

        if (pContentType == NULL) {
                hb_slogf_error("%s: No content type pointer", __FUNCTION__);
                return (-1);
        }

        if ((pQueue->readIndex >= pQueue->numElements) || (pQueue->numElements == 0)) {
                hb_slogf_error("%s: Read index out of bounds %d (max %d)", __FUNCTION__, pQueue->readIndex, pQueue->numElements);
                return (-1);
        }

        errCode = pthread_mutex_lock(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error(BUFFER_QUEUE_MUTEX_LOCK_FAILED, __FUNCTION__, (errCode));
                return (-1);
        }

        retValue = 0;

        nextReadIndex = pQueue->readIndex + 1;
        if (nextReadIndex >= pQueue->numElements) {
                nextReadIndex = 0;
        }

        if (pQueue->writeIndex == pQueue->readIndex) {
         /* Release the device */
               /* 
			   if (pAttr != NULL) {
                        if (iofunc_attr_unlock(pAttr) != EOK) {
                                hb_slogf_error("%s: Unlock of attribute structure failed", __FUNCTION__);
                        }
                }
                */
                /* queue empty wait for new message */
                errCode = pthread_cond_wait(&(pQueue->condVar), &(pQueue->condMutex));
                if (errCode != EOK) {
                        hb_slogf_error("%s: Cond wait failed <%d> [PNAME=%s;PID=%d]", __FUNCTION__, (errCode), current->comm, current->pid);
                        retValue = -1;
                }
        }

        if (pQueue->writeIndex != pQueue->readIndex) {
                pQueueElement = &(pQueue->pArray[pQueue->readIndex]);

                if ((*pContentType == DSPIPC_CONT_ANY) || (pQueueElement->contType == DSPIPC_CONT_ANY) || (*pContentType == pQueueElement->contType)) {
                        /* get data out of queue element */
                        if (pNumLost != NULL)
                                *pNumLost = pQueueElement->numLostBefore;
                        if (pQueueElement->numLostBefore != 0) {
#ifdef VVVV_ENABLE
                                if (gCmdOptions.optv > 3) {
                                        hb_slogf_error("%s: Skipped %d messages",
                                            __FUNCTION__, pQueueElement->numLostBefore);
                                }
#endif
                                pQueueElement->numLostBefore = 0;
                        }

                        if (pMsgId != NULL)
                                *pMsgId = pQueueElement->msgId;
                        if (pCmdId != NULL)
                                *pCmdId = pQueueElement->cmdId;
                        if (pMsgType != NULL)
                                *pMsgType = pQueueElement->msgType;
                        if (pMsgReply != NULL)
                                *pMsgReply = pQueueElement->msgReply;
                        *pContentType = pQueueElement->contType;
                        *pMsgSize = pQueueElement->msgSize;
                        memcpy((*ppMsg).generic, pQueueElement->pMsg.generic, pQueueElement->msgSize);

                        pQueueElement->msgSize = 0;

                        /* update read index */
                        pQueue->readIndex = nextReadIndex;
                } else {
                        /* element in queue does not match requested type */
                        retValue = 1;
                }
        } else {
                /* cond_wait was armed without valid queue element (e.g. io_unblock */
                /* from a TimerTimeout setting) */
#ifdef VVVV_ENABLE
                if (gCmdOptions.optv > 2) {
                        hb_slogf_error("%s: Unblocked while queue is empty reason <%s>",
                            __FUNCTION__, strerror(pQueue->unblockReason));
                }
#endif
                switch (pQueue->unblockReason) {
                case ETIMEDOUT:
                        retValue = 2;
                        break;
                case EBADF:
                        retValue = 3;
                        break;
                case ECANCELED:
                        retValue = 4;
                        break;
                case EBUSY:
                        retValue = 5;
                        break;
                default:
                        retValue = 2;
                        break;
                }
                pQueue->unblockReason = 0;
        }

        errCode = pthread_mutex_unlock(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error(BUFFER_QUEUE_MUTEX_UNLOCK_FAILED, __FUNCTION__, (errCode));
                retValue = -1;
        }

        return (retValue);
}

/**
   dspipcQueueClear clear all messages which are inside the message queue

   @param pQueue Pointer to the message queue
   @return int Error return code, 0 for success, -1 for failure
*/
int dspipcQueueClear(msgQueue_t* pQueue)
{
        int retValue = -1;
        int errCode;
        int nextReadIndex;
        msgQueueElement_t* pQueueElement;

        if (pQueue == NULL) {
                hb_slogf_error("%s: No queue", __FUNCTION__);
                return (-1);
        }

        if ((pQueue->readIndex >= pQueue->numElements) || (pQueue->numElements == 0)) {
                hb_slogf_error("%s: Read index out of bounds %d (max %d)", __FUNCTION__, pQueue->readIndex, pQueue->numElements);
                return (-1);
        }

        errCode = pthread_mutex_lock(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error(BUFFER_QUEUE_MUTEX_LOCK_FAILED, __FUNCTION__, (errCode));
                return (-1);
        }

        retValue = 0;

        while (pQueue->readIndex != pQueue->writeIndex) {
                /* element in queue available */
                nextReadIndex = pQueue->readIndex + 1;
                while (nextReadIndex >= pQueue->numElements)
                        nextReadIndex -= pQueue->numElements;

                pQueueElement = &(pQueue->pArray[pQueue->readIndex]);

                /* go to next element */
                pQueue->readIndex = nextReadIndex;
        }

        errCode = pthread_mutex_unlock(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error(BUFFER_QUEUE_MUTEX_UNLOCK_FAILED, __FUNCTION__, (errCode));
                retValue = -1;
        }

        return (retValue);
}

/**
   dspipcQueueNumAvil delivers the number of available message elements inside
   the specified queue

   @param pQueue Pointer to the message queue
   @param pNumElements Pointer to variable for storing number of available elements
   @return int Error return code, 0 for success, -1 for failure
*/
int dspipcQueueNumAvail(msgQueue_t* pQueue, int* pNumElements)
{
        int retValue = 0;
        int num;
        int errCode;

        if (pQueue == NULL) {
                hb_slogf_error("%s: No queue", __FUNCTION__);
                return (-1);
        }

        if (pNumElements == NULL) {
                hb_slogf_error("%s: No result space", __FUNCTION__);
                return (-1);
        }

        errCode = pthread_mutex_lock(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error(BUFFER_QUEUE_MUTEX_LOCK_FAILED, __FUNCTION__, (errCode));
                retValue = -1;
        }

        num = (pQueue->writeIndex < pQueue->readIndex) ? pQueue->numElements : 0;
        num += pQueue->writeIndex;
        num -= pQueue->readIndex;

        errCode = pthread_mutex_unlock(&(pQueue->condMutex));
        if (errCode != EOK) {
                hb_slogf_error(BUFFER_QUEUE_MUTEX_UNLOCK_FAILED, __FUNCTION__, (errCode));
                retValue = -1;
        }

        *pNumElements = num;

        return (retValue);
}

/*===========================   End Of File   ================================= */
