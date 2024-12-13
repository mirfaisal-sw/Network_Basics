/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/

/**
 * @file dev-dspipc.c
 * 
 * This source file contains main functions for the dev-dspipc
 * 
 * @author Howard Yang
 * @date   11.OCT.2012
 * 
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

/*=========================  include header file   =========================== */
#include <asm/fcntl.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
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
#ifdef CHIP_DRA6XX
#include "dra6xx/armdef.h"
#endif

#ifdef CHIP_DRA7XX
#include "dra7xx/armdef.h"
#endif

/*=========================  global variable Definitions  =========================== */
cmdOptions_t gCmdOptions;

/**
 * Counter value for the next memory segemnt id to be used
 */
uint32_t nextMemId = 0; /* NULL is invalid MemID */

uint32_t num_ShmElem_to_alloc = 8;

/**
 * use this variable to sync initialization and io entry.
 */
uint32_t init_success = 0;

char DSP_IMAGE_VERSION_INFO[256] = { 0 };

deviceTable_t* pDeviceTableRoot = NULL;

deviceTable_t* pDevice = NULL;

void __iomem* regModConf_dspipc = NULL; /* Device Configuration Module Register Area */

uint32_t shareMemPhyAddr = 0; /* Address of stream shared memory */

uint32_t shareMemPhySize = 0; /* Size of stream shared memory */

/*=========================  local variable Definitions   =========================== */
static char* dspOptions = "no";
static char* m3Options = "no";
static char* pDSP_NOT_AVAIL_MSG = "**** No DSP-IMAGE Version Info available ****";

/*=========================function prototype Definitions =========================== */
static void __exit dspipc_exit(void);
static int __init dspipc_init(void);

/*=========================function implementation        =========================== */
static int __init dspipc_init(void)
{
        char* argv[] = { dspOptions, m3Options };
        int argc = 2;

        DEBUG_INFO("DSPIPC kernel driver version is: %s\n", DSPIPC_DRIVER_VERSION);

        if ((strcmp(dspOptions, "no") == 0) && (strcmp(m3Options, "no") == 0)) {
                DEBUG_ERROR("Arguments Error!!!\n");
                return -1;
        } else {
                DEBUG_INFO("dspOptions: %s\n", dspOptions);
                DEBUG_INFO("m3Options: %s\n", m3Options);
        }

        memcpy(&DSP_IMAGE_VERSION_INFO[0], pDSP_NOT_AVAIL_MSG, strlen(pDSP_NOT_AVAIL_MSG) + 1);

        MALLOC_TRC_INIT;

        if (InitMemoryObjects() != 0) {
                DEBUG_ERROR("Init mem pool failed!\n");
                dspipc_exit();
                return -1;
        }
        gCmdOptions.optv = 2;
        gCmdOptions.optb = 0;
        gCmdOptions.slogSeverity = 5;
        gCmdOptions.lo_water = 2;
        gCmdOptions.increment = 1;
        gCmdOptions.hi_water = 8;
        gCmdOptions.maximum = 40;
        gCmdOptions.numChipset = 0;

        /* Attach Module Conf Register */
        regModConf_dspipc = ioremap_nocache((unsigned long)PCI_MEM_ADDR(DEV_CONF_MODULE), DEV_CONF_MODULE_MEM_SIZE);
        if (regModConf_dspipc == NULL) {
                hb_slogf_error("%s Error:Failed to ioremap mailbox register region\n", __FUNCTION__);
                return -1;
        }

        if (options(argc, argv, &pDeviceTableRoot) != 0) {
                DEBUG_ERROR("Error starting dev-dspipc!!\n");
                dspipc_exit();
                return -1;
        }

        /* allocate a min. number of shared memory elements */
        if (shareMemPhyAddr != 0) {
                if (InitBaseObjects() != 0) {
                        hb_slogf_error("InitBaseObjects() failed\n");
                        dspipc_exit();
                        return -1;
                }
        }
        init_success = 1;
        return 0;
}

static void __exit dspipc_exit(void)
{
#if 0
   kthread_stop(task1);
   kthread_stop(task2);
return;
#endif
        DllDeinit(pDeviceTableRoot);

        /* Deinit Memory Objects */
        if (DeinitMemoryObjects() != 0) {
                hb_slogf_error("Deinit mem pool failed");
        }

        /*Detach Module Conf Register */
        if (regModConf_dspipc != NULL) {
                iounmap(regModConf_dspipc);
        }
}

module_param(dspOptions, charp, 0000);
MODULE_PARM_DESC(dspOptions, "DLL options.");

module_param(m3Options, charp, 0000);
MODULE_PARM_DESC(m3Options, "DLL options.");

MODULE_LICENSE("Dual BSD/GPL");
module_init(dspipc_init);
module_exit(dspipc_exit);
MODULE_DESCRIPTION("dspipc Driver");
MODULE_AUTHOR("Howard Yang");
