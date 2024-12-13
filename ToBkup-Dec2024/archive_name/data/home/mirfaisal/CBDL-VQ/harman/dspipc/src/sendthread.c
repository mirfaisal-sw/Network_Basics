/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file sendthread.c
 *
 * This is the implementation for the send thread stub of the
 * dev-dspipc ressource handler.
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

#include "debug.h"
#include "dev-dspipc.h"
#include "dsp/dspipclib.h"
#include "dspipc_pthread.h"
#include "dspipcproto.h"
#include "malloc_trc.h"
#include "misc.h"
#include "string_def_ext.h"

/*============================================================================== */
/*=========================    Macro's   ======================================= */
#define __TRACE_ON

#ifdef TRACE_ON
#include <sys/trace.h> /*for TraceEvent() */
#define TRACEEVENT(a, b, c) (void)TraceEvent(a, b, c);
#else
#define TRACEEVENT(a, b, c)
#endif

#ifdef TRACE_ON
#include <sys/trace.h> /*for TraceEvent() */
#define TRACEEVENT_INT(a, b, c, d) (void)TraceEvent(a, b, c, d);
#else
#define TRACEEVENT_INT(a, b, c, d)
#endif

/* none */
/*============================================================================== */
/*===================    Modul Global Variables   ============================== */
extern void print_subdeviceInfo(const deviceTable_t* pDevice);
/* none */

/*============================================================================== */
/*===================    forward declarations    =============================== */
/* none */
/*============================================================================== */
/*===================    Function Definitions    =============================== */

/**
 * Funtion to signal a change inside the queues to the send thread. This
 * function will increase the sendPending information of the specified
 * device and will signal the condition variable change to the send thread.
 *
 * @param pDevice Pointer to the device table structure for the download
 * @return int Status of the action (0=OK,-1=Failure)
 */
int WakeupSendThread(const deviceTable_t* pDevice)
{
        int retValue = 0;
        int msg = INTTHREAD_SEND;

        /* signal interrupt thread to initiate a queue scan */
        /* send pulse to interrupt thread */
        if (MsgPut(pDevice->intCoId,
                &msg, /* pulse code for send */
                1)
            != 1) {
                hb_slogf_error("%s: Sending pulse to int thread failed",
                    __FUNCTION__);
                retValue = -1;
        }

        return (retValue);
}

/**
 * Send thread to handle the outgoing message elements out of the send queue
 *
 * @param arg Argument pointer passed by pthread_create(), this is the
 *            pointer to the device structure
 * @return void* Always NULL
 */
void* dspipcSendThread(void* arg)
{
        deviceTable_t* pDevice;
        subDeviceTable_t* pSubDevice;
        subDeviceTable_t* pCMDSubDevice = NULL;
        subDeviceTable_t* pSTREAMSubDevice = NULL;
        subDeviceTable_t* pDrainSubDevice = NULL;
        subDeviceTable_t* pFirstSubDevice = NULL;
        subDeviceTable_t* pLastSendSubDevice = NULL;
        subDeviceTable_t* pLastSendSubDeviceSetBack = NULL;

        int numElements;
        int errCode;
        uint32_t bufferLen;

        drainSendMode_t drain_mode_call;
        subDeviceStatus_t device_status_set;

        int PS;
        int PS_send = 5;         /* allowed values.0-4 */
        int subdevice_Found = 0; /* var. to stop scanning Queues bacause a subevice is found */

        subDeviceTable_t* PrioStufe_StartSubDevice;

        pDevice = (deviceTable_t*)arg;
        if (pDevice != NULL) {
                if (dspipcLockSubDevices(pDevice) == EOK) {
                        pDrainSubDevice = NULL;

                        /* Beginn Queu Scan */
                        for (PS = HOSTIPC_CHANNELPRIORITY_ADMIN; (PS >= 0) && (subdevice_Found == 0); PS--) /*   Setze Prio_Stufe = PS */
                        {
                                /*   Setze PrioStufe_StartSubDevice */
                                if (PS == HOSTIPC_CHANNELPRIORITY_ADMIN) {
                                        PrioStufe_StartSubDevice = pDevice->pSubDevices_0;  /* Setze PrioStufe_StartSubDevice */
                                        pLastSendSubDevice = pDevice->pLastSendSubDevice_0; /* Setze pLastSendSubDevice von dieser PrioStufe */
                                }
                                /*   Setze PrioStufe_StartSubDevice */
                                else if (PS == HOSTIPC_CHANNELPRIORITY_REALTIMECOMMAND) {
                                        PrioStufe_StartSubDevice = pDevice->pSubDevices_1;  /* Setze PrioStufe_StartSubDevice */
                                        pLastSendSubDevice = pDevice->pLastSendSubDevice_1; /* Setze pLastSendSubDevice von dieser PrioStufe */
                                } else if (PS == HOSTIPC_CHANNELPRIORITY_REALTIMESTREAM) {
                                        PrioStufe_StartSubDevice = pDevice->pSubDevices_2;
                                        pLastSendSubDevice = pDevice->pLastSendSubDevice_2;
                                } else if (PS == HOSTIPC_CHANNELPRIORITY_NORMAL) {
                                        PrioStufe_StartSubDevice = pDevice->pSubDevices_3;
                                        pLastSendSubDevice = pDevice->pLastSendSubDevice_3;
                                } else /* HOSTIPC_CHANNELPRIORITY_IDLE */
                                {
                                        PrioStufe_StartSubDevice = pDevice->pSubDevices_4;
                                        pLastSendSubDevice = pDevice->pLastSendSubDevice_4;
                                }

                                if (pLastSendSubDevice == NULL) {
                                        /* if no last element or end of list continue with list start */
                                        pLastSendSubDevice = PrioStufe_StartSubDevice;
                                }
                                if (pLastSendSubDevice != NULL) {
                                        /* Setzt FistSubDevice */
                                        pFirstSubDevice = pLastSendSubDevice->pNext;
                                        if ((pFirstSubDevice == NULL) || (pFirstSubDevice->channelPrio != PS)) {
                                                pFirstSubDevice = PrioStufe_StartSubDevice;
                                        }
                                        pSubDevice = pFirstSubDevice;
                                } else {
                                        pFirstSubDevice = NULL;
                                        pSubDevice = NULL;
                                }

                                if (pSubDevice != NULL) {
                                        do /* ein Durchlauf durch die PrioStufe */
                                        {
                                                if (pSubDevice->xonoffState == XON_STATE) /* Check if state is XON (else no transfer from this channel) */
                                                {
                                                        if (pSubDevice->pSendMsgQueue != NULL) /* Check pSendMsgQueue!=NULL */
                                                        {
                                                                if (((pSubDevice->status == SD_STAT_DRAIN) && (pSubDevice->type == SD_STREAM)) || (pSubDevice->status == SD_STAT_DRAIN_COMPLETE)) {
                                                                        /* setze Drain Subdevice */
                                                                        pDrainSubDevice = pSubDevice;
                                                                        PS_send = PS;
                                                                } else /* Normal state */
                                                                {
                                                                        /* get only new message out of the queue if no transfer is pending */
                                                                        if (pSubDevice->pTransferQueue == NULL) /* Wenn kein Stream Transfer anliegt!!! => Command */
                                                                        {
                                                                                /* Check for normale SendMsg's */
                                                                                if (dspipcQueueNumAvail(pSubDevice->pSendMsgQueue, &numElements) == 0) {
                                                                                        if (numElements != 0) {
                                                                                                pCMDSubDevice = pSubDevice;
                                                                                                PS_send = PS;
                                                                                        }
                                                                                } else {
                                                                                        hb_slogf_error("%s: %s Req num of elements failed for device %s",
                                                                                            __FUNCTION__, pDevice->pDeviceName,
                                                                                            pSubDevice->pSubDeviceName);
                                                                                }
                                                                        } else {
                                                                                /* And also Check possible Stream-Transfer */
                                                                                errCode = dspipcSubDeviceLockBuffer(pSubDevice);
                                                                                if (errCode == EOK) {
                                                                                        if (dspipcSubDeviceCheckBuffer(pSubDevice, &bufferLen,
                                                                                                NULL, NULL,
                                                                                                NULL, NULL)
                                                                                            != -1) {
                                                                                                if ((bufferLen != 0) && (pSubDevice->status == SD_STAT_READY)) {
                                                                                                        /* store current subdevice to be remind after the loop to */
                                                                                                        /* send the pending data */
                                                                                                        pSTREAMSubDevice = pSubDevice;
                                                                                                        PS_send = PS;
                                                                                                }
                                                                                        } else {
                                                                                                hb_slogf_error("%s: Check buffer failed", __FUNCTION__);
                                                                                        }

                                                                                        errCode = dspipcSubDeviceUnlockBuffer(pSubDevice);
                                                                                        if (errCode != EOK) {
                                                                                                hb_slogf_error("%s: Buffer queue %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode));
                                                                                        }
                                                                                } else {
                                                                                        hb_slogf_error("%s: Buffer queue %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode));
                                                                                }
                                                                        }
                                                                } /* ENDE: Normal state */
                                                        }         /* ENDE: Check pSendMsgQueue!=NULL */
                                                }                 /* End check for XONOFF state */

                                                pSubDevice = pSubDevice->pNext;

                                                /* Check if next Subdevice has the same Prio or End of List is reached */
                                                if ((pSubDevice == NULL) || (pSubDevice->channelPrio != PS)) {
                                                        /* Set to start of PrioStufe */
                                                        pSubDevice = PrioStufe_StartSubDevice;
                                                }
                                        } while ((pSubDevice != pFirstSubDevice) && (PS_send == 5)); /* END: ein Durchlauf durch die PrioStufe */
                                }                                                                    /* ENDE: pSubDevice != NULL */

                                if ((PS_send < 5)) {
                                        if (pDevice->drain_Avail_Cnt > 0) /* If drain is available then scan all subdevices!!! */
                                        {
                                                subdevice_Found = 0;
                                        } else {
                                                subdevice_Found = 1; /* else scan can be stopped because a transfer was found */
                                        }
                                }
                        } /* End for loop alle PS */

                        if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                        }

                        if (pDrainSubDevice != NULL) {
                                if ((pDrainSubDevice->status == SD_STAT_DRAIN) || (pDrainSubDevice->status == SD_STAT_DRAIN_COMPLETE)) {
                                        if (pDrainSubDevice->status == SD_STAT_DRAIN) {
                                                drain_mode_call = SEND_DRAIN_START;
                                                device_status_set = SD_STAT_DRAIN_WAIT_ACK;
                                        } else {
                                                drain_mode_call = SEND_DRAIN_END;
                                                device_status_set = SD_STAT_READY;
                                        }

                                        if (pDevice->sendDrainFunc != NULL) {
                                                if ((*(pDevice->sendDrainFunc))((void*)pDrainSubDevice, drain_mode_call) == 0) {
                                                        pLastSendSubDeviceSetBack = pDrainSubDevice;
                                                        /* Set new Subdevice state */
                                                        if (dspipcLockSubDevices(pDevice) == EOK) {
                                                                pDrainSubDevice->status = device_status_set;
                                                                if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                                                        hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                                                }
                                                        } else {
                                                                hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                                                        }
                                                } else {
                                                        hb_slogf_error("%s: Acore Send drain to %s func (%d) failed on device %s",
                                                            __FUNCTION__, pDevice->pDeviceName,
                                                            drain_mode_call,
                                                            pDrainSubDevice->pSubDeviceName);
                                                }
                                        } else {
                                                /* if no drain function is defined, ignore drain and set status back to ready */
                                                pDrainSubDevice->status = SD_STAT_READY;
                                        }
                                }
                        } else if (pCMDSubDevice != NULL) {
                                if (pDevice->sendFunc != NULL) {
                                        if ((*(pDevice->sendFunc))(pCMDSubDevice) != 0) {
                                                hb_slogf_error("%s: Acore Send to %s failed on device %s",
                                                    __FUNCTION__, pDevice->pDeviceName, pCMDSubDevice->pSubDeviceName);
                                        } else {
                                                pLastSendSubDeviceSetBack = pCMDSubDevice;
                                        }
                                }
                        } else {
                                if (pSTREAMSubDevice != NULL) {
                                        if (pDevice->sendStreamDataFunc != NULL) {
                                                if ((*(pDevice->sendStreamDataFunc))((void*)pSTREAMSubDevice) != 0) {
                                                        hb_slogf_error("%s: Acore SendStreamData to %s failed on device %s",
                                                            __FUNCTION__, pDevice->pDeviceName, pSTREAMSubDevice->pSubDeviceName);
                                                } else {
                                                        pLastSendSubDeviceSetBack = pSTREAMSubDevice;
                                                }
                                        }
                                }
                        }
                } else {
                        hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
        }

        /* If pLastSendSubDeviceSetBack was set, set the new LastSendSubDevice, else not */
        if (pLastSendSubDeviceSetBack != NULL) {
                /* If transfer was doen update the corresponding pLastSendSubDevice in PrioStufe */
                if (dspipcLockSubDevices(pDevice) == EOK) {
                        if (PS_send == HOSTIPC_CHANNELPRIORITY_ADMIN) {
                                pDevice->pLastSendSubDevice_0 = pLastSendSubDeviceSetBack;
                        }
                        /*   Set pLastSendSubDevice to PS where subdevice was from */
                        if (PS_send == HOSTIPC_CHANNELPRIORITY_REALTIMECOMMAND) {
                                pDevice->pLastSendSubDevice_1 = pLastSendSubDeviceSetBack;
                        } else if (PS_send == HOSTIPC_CHANNELPRIORITY_REALTIMESTREAM) {
                                pDevice->pLastSendSubDevice_2 = pLastSendSubDeviceSetBack;
                        } else if (PS_send == HOSTIPC_CHANNELPRIORITY_NORMAL) {
                                pDevice->pLastSendSubDevice_3 = pLastSendSubDeviceSetBack;
                        } else if (PS_send == HOSTIPC_CHANNELPRIORITY_IDLE) /* HOSTIPC_CHANNELPRIORITY_IDLE */
                        {
                                pDevice->pLastSendSubDevice_4 = pLastSendSubDeviceSetBack;
                        }

                        if (dspipcUnlockSubDevices(pDevice) != EOK) {
                                hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                        }
                } else {
                        hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                }
        }

        return NULL;
}

/*===========================   End Of File   ================================= */
