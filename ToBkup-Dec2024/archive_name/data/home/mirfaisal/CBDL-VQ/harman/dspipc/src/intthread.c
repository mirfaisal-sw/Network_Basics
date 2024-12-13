/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file intthread.c
 *
 * This is the implementation for the interrupt thread stub of the
 * dev-dspipc ressource handler.
 *
 * @author Andreas Heiner
 * @date   03.03.2006
 *
 * @author Huihua Yang
 * @date   Nov.02.2012
 *
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

/*============================================================================== */
/*=========================    Includes   ====================================== */
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
/*#include <asm/system.h> */
#include <asm/uaccess.h>
#include <linux/unistd.h>

#include "debug.h"
#include "dev-dspipc.h"
#include "dsp/DSPIPCTypes.h"
#include "dspipcproto.h"
#include "malloc_trc.h"
#include "misc.h"

/*============================================================================== */
/*=========================    Macro's   ======================================= */

/* none */

/*============================================================================== */
/*===================    Modul Global Variables   ============================== */

/* none */

/*============================================================================== */
/*===================    forward declarations    =============================== */

/* none */

#define __INT_THREAD_INIT_HERE
/*============================================================================== */
/*===================    Function Definitions    =============================== */

/**
 * Interrupt thread to handle the interrupts from the device
 *
 * @param arg Argument pointer passed by pthread_create(), this is the
 *            pointer to the device structure
 * @return void* Always NULL
 */
int dspipcIntrThread(void* arg)
{
        deviceTable_t* pDevice;
        struct sched_param param;
        param.sched_priority = 5;
        sched_setscheduler(current, SCHED_RR, &param);
        pDevice = (deviceTable_t*)arg;
        if (pDevice != NULL) {
                if (pDevice->intThread != NULL) {
                        if ((*(pDevice->intThread))(pDevice) == 0) {
#ifdef VVVV_ENABLE
                                if (gCmdOptions.optv > 0) {
                                        hb_slogf_error("%s: %s Interrupt thread terminated with ok",
                                            __FUNCTION__, pDevice->pDeviceName);
                                }
#endif
                        } else {
                                hb_slogf_error("%s: %s Interrupt thread terminated with error",
                                    __FUNCTION__, pDevice->pDeviceName);
                        }

                        /* Detach interrupt */
                        if (pDevice->intEvent != -1) {
                                free_irq(pDevice->intNumber, (void*)pDevice);
                                pDevice->intEvent = -1;
                        }

                        if (pDevice->intCoId != -1 && pDevice->intCoId != 0) {
                                /* Detach Connection */
                                if (MsgDeinit(pDevice->intCoId) == -1) {
                                        hb_slogf_error("%s: MsgDeinit failed", __FUNCTION__);
                                }
                        }
                        pDevice->intCoId = -1;

                        pDevice->intChId = -1;
                }
                hb_slogf_error("%s: InterruptThread for %s terminated", __FUNCTION__, pDevice->pDeviceName);
        } else {
                hb_slogf_error("%s: Internal error, Interruptthread for %s cannot be started",
                    __FUNCTION__, pDevice->pDeviceName);
        }

        return (0);
}
