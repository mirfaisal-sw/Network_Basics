/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file device.c
 *
 * This is the file containing the functions to handle the device structures
 * inside the dev-dspipc. Device structures are created for each connected 
 * DSP device instance.
 *
 *
 * @author Andreas Heiner
 * @date   24.07.2006
 *
 * @modifier Howard Yang
 * @date   15.10.2012
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
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "debug.h"
#include "dev-dspipc.h"
#include "dspipcproto.h"
#include "malloc_trc.h"
#include "misc.h"
#include "string_def_ext.h"
/*============================================================================== */
/*=========================   Define's   ======================================= */

/* none */

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
 * dspipcLockSubDevices locks the accesses to the subdevice chain inside the
 * device structure.
 *
 * @param pDevice Pointer to the device table structure to lock the subdevices
 *                list
 * @return int Return value of mutex_lock function (EOK = ok)
 */
int dspipcLockSubDevices(deviceTable_t* pDevice)
{
        int retValue = EOK;

        if (pDevice != NULL) {
                retValue = pthread_mutex_lock(&(pDevice->subDevicesMutex));
                if (retValue != EOK) {
                        hb_slogf_error("%s: Lock for subdevices mutex failed (%s)",
                            __FUNCTION__, strerror(retValue));
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
        }

        return (retValue);
}

/**
 * dspipcUnlockSubDevices unlocks the accesses to the subdevice chain inside
 * the device structure.
 *
 * @param pDevice Pointer to the device table structure to unlock the 
 *                subdevices list
 * @return int Return value of mutex_unlock function (EOK = ok)
 */
int dspipcUnlockSubDevices(deviceTable_t* pDevice)
{
        int retValue = EOK;

        if (pDevice != NULL) {
                retValue = pthread_mutex_unlock(&(pDevice->subDevicesMutex));
                if (retValue != EOK) {
                        hb_slogf_error("%s: Unlock for subdevices mutex failed (%s)",
                            __FUNCTION__, strerror(retValue));
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
        }

        return (retValue);
}

/**
 * dspipcWaitChangeSubDevices wait for a change inside the subdevices
 * structure and delivers the reason of the change.
 *
 * @param pDevice Pointer to the device table structure to unlock the 
 *                subdevices list
 * @return changeReason_t Reason of change (REASON_ERROR | ADD_CHANNEL | 
 *                                          DELETE_CHANNEL| CONNECT_UNBLOCK)
 */
changeReason_t dspipcWaitChangeSubDevices(deviceTable_t* pDevice)
{
        changeReason_t reason = REASON_ERROR;
#if 0   
   struct sigevent    timeoutEvent;
   uint64_t           timeout;
   int errCode;

   if( pDevice != NULL )
   {
      timeoutEvent.sigev_notify = SIGEV_UNBLOCK;
      timeout = (uint64_t)10*1000*1000; /* 10ms */
      if( TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_CONDVAR, &timeoutEvent, &timeout, NULL) == -1)
      {
         hb_slogf_error("%s: TimerTimeout failed %d <%s>", __FUNCTION__ );
      }

      errCode = pthread_cond_wait( &(pDevice->subDevicesCondVar), 
                                   &(pDevice->subDevicesMutex));
      switch( errCode )
      {
         case EOK:
            reason = pDevice->subDevicesReason;
            break;
         case ETIMEDOUT:
            reason = TIMEOUT;
            break;
         default:
            hb_slogf_error("%s: Cond wait failed %d (%s)", 
                           __FUNCTION__, errCode, strerror(errCode) );
            break;
      }
   }
   else
   {
      hb_slogf_error(NO_DEVICE, __FUNCTION__ );
   }
#endif
        return (reason);
}

/**
 * dspipcSignalChangeSubDevices signals a change inside the subdevices
 * structure and stores the reason of the change into the device structure.
 *
 * @param pDevice Pointer to the device table structure to unlock the 
 *                subdevices list
 * @param reason  Reason of change (ADD_CHANNEL | DELETE_CHANNEL| CONNECT_UNBLOCK)
 * @return int    Error code of the pthread_cond_broadcast function (ok = EOK)
 */
int dspipcSignalChangeSubDevices(deviceTable_t* pDevice, changeReason_t reason)
{
        int errCode = EINVAL;

        if (pDevice != NULL) {
                pDevice->subDevicesReason = reason;
                errCode = pthread_cond_signal(&(pDevice->subDevicesCondVar));
                if (errCode != EOK) {
                        hb_slogf_error("%s: Cond signal failed <%s>",
                            __FUNCTION__, strerror(errCode));
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
        }

        return (errCode);
}

/**
 * dspipcStopInterruptThread send a Terminate Pulse to the Inter Thread 
 * and waits until the Thread is finished.
 *
 * @param pDevice Pointer to the device table structure to unlock the subdevices list
 * @return int    Error code of the pthread_cond_broadcast function (ok = EOK)
 */
int dspipcStopInterruptThread(deviceTable_t* pDevice)
{
        int errCode = EOK;
        pthread_t thread_int;
        int msg;

        if (pDevice != NULL) {
                if (pDevice->intThreadHndl != 0) {
                        /* Stop INT Thread */
                        pDevice->terminate = 1; /* set terminate flag for INT Thread */

                        if (pDevice->intCoId != 0 && pDevice->intCoId != -1) {
                                msg = INTTHREAD_TERMINATE;
                                errCode = MsgPut(pDevice->intCoId, &msg, 1);
                                if(errCode != 0) {
                                        DEBUG_INFO("[%s]: terminate set!\n", __FUNCTION__);
                                        /* Wait for END of INT Thread */
                                        thread_int = pDevice->intThreadHndl;

                                        errCode = pthread_join(thread_int, NULL);
                                        DEBUG_INFO("[%s]: after pthread_join!\n", __FUNCTION__);
                                } else {
                                        DEBUG_INFO("[%s]: kfifo full, terminate cannot be set, terminate INT thread directly without wait!\n", __FUNCTION__);
                                        pDevice->doLoop = 0;
                                }
                        }
#if 0   
         switch( errCode )
         {
            case EOK:
               break;
            default:
               hb_slogf_error( "%s: (%s) thread join failed (%s), cancel thread",
                               __FUNCTION__, pDevice->pDeviceName, strerror(errCode) );
               errCode = pthread_cancel( thread_int );
               if( errCode != EOK )
               {
                  hb_slogf_error( "%s: (%s) thread cancel failed (%s)",
                                  __FUNCTION__, pDevice->pDeviceName, strerror(errCode) );
               }
               break;
         }
#endif
                        pDevice->terminate = 0; /* set back the terminate flag for INT Thread  */

                } else {
                        hb_slogf_error("[%s]: no valid interrupt thread handle", __FUNCTION__);
                        errCode = EINVAL;
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                errCode = EINVAL;
        }

        return (errCode);
}

/**
 *  This function delivers a continous unique message id for the device
 *
 *  @param pDevice Pointer to the device table structure
 *  @param pId Pointer to store the unique message id
 *  @return int Error code 0=OK, -1 = Failure
 */
int dspipc_getNewMessageId(deviceTable_t* pDevice, uint32_t* pId)
{
        uint32_t uniqueMsgId;
        int errCode;
        int retValue = -1;

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (retValue);
        }

        if (pId == NULL) {
                hb_slogf_error("%s no space for id", __FUNCTION__);
                return (retValue);
        }

        errCode = pthread_mutex_lock(&(pDevice->currMsgIdMutex));
        if (errCode == EOK) {
                uniqueMsgId = pDevice->currMsgId;
                uniqueMsgId = (uniqueMsgId == DSPIPC_MSGID_COUNTER_MSK) ? 1 : uniqueMsgId + 1;
                pDevice->currMsgId = uniqueMsgId;
                *pId = uniqueMsgId & DSPIPC_MSGID_COUNTER_MSK;

                errCode = pthread_mutex_unlock(&(pDevice->currMsgIdMutex));
                if (errCode == EOK) {
                        retValue = 0;
                } else {
                        hb_slogf_error("%s: MsgId access %s %d", __FUNCTION__, MUTEX_UNLOCK_FAILED, errCode);
                }
        } else {
                hb_slogf_error("%s: MsgId access %s %d", __FUNCTION__, MUTEX_LOCK_FAILED, errCode);
        }

        return (retValue);
}

/*===========================   End Of File   ================================= */
