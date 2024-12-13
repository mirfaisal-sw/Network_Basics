/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file options.c
 *
 * This is the implementation for the options parsing functionality of the
 * dev-dspipc ressource handler.
 *
 * @author Howard Yang
 * @date   12.OCT.2012
 *
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/unistd.h>

#include "DSPIPCTypes.h"
#include "debug.h"
#include "dev-dspipc.h"
#include "dspipclib.h"
#include "dspipcproto.h"
#include "malloc_trc.h"
#include "misc.h"

#ifdef CHIP_DRA6XX
#include "dra6xx/dspipc-dra6xx.h"
#endif

#ifdef CHIP_DRA7XX
#include "dra7xx/dspipc-dra7xx.h"
#endif

#ifdef CHIP_SYSLINKS
#include "syslinks/src/dspipc-syslinks.h"
#endif

static int optr = 1;

/**
 *  This function is doing the following steps:
 *  start interrupt handler thread / load firmware / start firmware /
 *  start application / start communication
 *
 *  @param pDevice Pointer to the device adminstration structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int startChipset(deviceTable_t* pDevice)
{
        int retValue = 0;
        int errCode;

        /* start interrupt handler thread */
        if (pDevice != NULL) {
                if (pDevice->intThread != NULL) {
                        if (pthread_attr_init(&(pDevice->intThreadAttr)) == EOK) {

                                if ((errCode = pthread_create(&(pDevice->intThreadHndl),
                                         &(pDevice->intThreadAttr),
                                         dspipcIntrThread,
                                         pDevice))
                                    == EOK) {
#ifdef VVVV_ENABLE
                                        if (gCmdOptions.optv > 2) {
                                                hb_slogf_error("%s: Interrupt thread for %s attached",
                                                    __FUNCTION__, pDevice->pDeviceName);
                                        }
#endif
                                } else {
                                        hb_slogf_error("%s: %s Interrupt thread create failed <%s>",
                                            __FUNCTION__, pDevice->pDeviceName, strerror(errCode));
                                        retValue = -1;
                                }
                        } else {
                                hb_slogf_error("%s: %s Attribute init for interrupt thread failed",
                                    __FUNCTION__, pDevice->pDeviceName);
                                retValue = -1;
                        }
                }
        } else {
                hb_slogf_error("%s: Invalid parameter", __FUNCTION__);
                retValue = -1;
        }

        /* Establish the communication */
#ifndef __X86__
        if (retValue == 0) {
                if (pDevice->loadFWFunc != NULL) {
                        if ((*(pDevice->loadFWFunc))(pDevice) == 0) {
                                hb_slogf_error("%s: %s Firmware loaded", __FUNCTION__, pDevice->pDeviceName);
                        } else {
                                hb_slogf_error("%s: %s load firmware failed",
                                    __FUNCTION__, pDevice->pDeviceName);
                                retValue = -1;
                        }
                }
        }
#endif

#ifndef __X86__
        if (retValue == 0) {
                if (pDevice->startFWFunc != NULL) {
                        /*start communication */
                        if ((*(pDevice->startFWFunc))(pDevice) == 0) {
                                hb_slogf_error("%s: %s Firmware started", __FUNCTION__, pDevice->pDeviceName);
                        } else {
                                hb_slogf_error("%s: %s Cannot start firmware",
                                    __FUNCTION__, pDevice->pDeviceName);
                                retValue = -1;
                        }
                }
        }
#endif

        /*wait for intThreadAttr running up*/
        msleep(1);

        /* start application */
        if (retValue == 0) {
                if (pDevice->startAppFunc != NULL) {
                        DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s: start Application func addr 0x%08x\n", __FUNCTION__, (unsigned int)pDevice->startAppFunc);
                        /*start application */
                        if ((*(pDevice->startAppFunc))(pDevice) == 0) {
                                hb_slogf_error("%s: %s Application started\n", __FUNCTION__, pDevice->pDeviceName);
                        } else {
                                hb_slogf_error("%s: %s Cannot start application",
                                    __FUNCTION__, pDevice->pDeviceName);
                                retValue = -1;
                        }
                }
        }

        /* start communication */
        if (retValue == 0) {
                if (dspipcAddSubDevice(pDevice,
                        S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH,
                        NULL, SD_CONTROL, SD_DIR_NODIR, 0, HOSTIPC_CHANNELPRIORITY_ADMIN)
                    != NULL) {
                        if (pDevice->startComFunc != NULL) {
                                /*start communication */
                                if ((*(pDevice->startComFunc))(pDevice) == 0) {
                                        hb_slogf_error("%s: %s Communication started", __FUNCTION__, pDevice->pDeviceName);
                                } else {
                                        hb_slogf_error("%s: %s Cannot start communication",
                                            __FUNCTION__, pDevice->pDeviceName);
                                        retValue = -1;
                                }
                        }
                } else {
                        hb_slogf_error("%s: %s Add ctrl sub device failed",
                            __FUNCTION__, pDevice->pDeviceName);
                        retValue = -1;
                }
        }

        return retValue;
}

static int DllLoadAndInit(const char* pDllFile, char* pDllOptions, deviceTable_t* pDevice)
{
        int retValue = 0;
        int loopCnt = 0;
        int errCode;

        if (pDevice == NULL) {
                hb_slogf_error("No device structure pointer");
                return (-1);
        }

        DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s: Open chipset with options <%s>\n",
            __FUNCTION__, pDllOptions);
#ifdef CHIP_DRA6XX
        {
                nextMemId = 32001;
                /* dll is opened */
                pDevice->initFunc = dspipc_dra6xx_init; /*replace with DLL symbol lookup */
        }
#endif

#ifdef CHIP_DRA7XX
        {
                nextMemId = 32001;
                /* dll is opened */
                pDevice->initFunc = dspipc_dra7xx_init; /*replace with DLL symbol lookup */
        }
#endif

#ifdef CHIP_SYSLINKS
        {
                nextMemId = 32001;
                /* dll is opened */
                pDevice->initFunc = dspipc_syslinks_init; /*replace with DLL symbol lookup */
        }
#endif

        if (retValue == 0) {
                if (pDevice->initFunc != NULL) {
                        if ((*(pDevice->initFunc))(pDevice, pDllOptions) == 0) {
                                if (pDevice->dllIfaceVersion != DSPIPC_DLL_INTERFACE_VERSION) {
                                        hb_slogf_error("%s: wrong interface version",
                                            __FUNCTION__);
                                        retValue = -1;
                                }
                                hb_slogf_error("%s: %s init func success!",
                                    __FUNCTION__, pDevice->pDeviceName);
                        } else {
                                hb_slogf_error("%s: %s init func failed!",
                                    __FUNCTION__, pDevice->pDeviceName);
                                retValue = -1;
                        }
                }
        }

        if (retValue == 0) {
                do {
                        retValue = -1;
                        loopCnt++;
                        if ((retValue = startChipset(pDevice)) != 0) {
                                hb_slogf_error("%s: Start Chipset %s failed", __FUNCTION__, pDevice->pDeviceName);
                        }
                        if (retValue != 0 && (loopCnt < optr)) {
                                if ((*(pDevice->resetState))(pDevice, PREPARE_RESET) != 0) {
                                        hb_slogf_error("%s: Reset State func failed for %s", __FUNCTION__, pDevice->pDeviceName);
                                }
                                mdelay(3);

                                /* Stop Interrupt Thread */
                                if (dspipcStopInterruptThread(pDevice) != EOK) {
                                        hb_slogf_error("%s: Reset State func failed for %s", __FUNCTION__, pDevice->pDeviceName);
                                }
                        }
                        if (retValue != 0) {
                                hb_slogf_error("%s: %d. Start Chipset %s failed", __FUNCTION__, loopCnt, pDevice->pDeviceName);
                        } else {
                                hb_slogf_error("%s: %d. Start Chipset %s success", __FUNCTION__, loopCnt, pDevice->pDeviceName);
                        }
                } while ((loopCnt < optr) && (retValue != 0));
        }

        if (retValue != 0) {
                /* anything has been failed, close the chipset layer (has to terminate the interrupt thread) */
                if (pDevice->closeFunc != NULL) {
                        DEBUG_INFO_LEVEL(LEVEL_VVVV, "[%s]: before close func!\n", __FUNCTION__);
                        if ((*(pDevice->closeFunc))(pDevice) == 0) {
                                DEBUG_INFO_LEVEL(LEVEL_V, "%s: %s Chipset layer closed\n", __FUNCTION__, pDevice->pDeviceName);
                        } else {
                                hb_slogf_error("%s: %s Close chipset layer failed", __FUNCTION__, pDevice->pDeviceName);
                        }
                } else {
                        /* in case of no specified close function terminate interrupt thread */
                        if ((errCode = pthread_cancel(pDevice->intThreadHndl)) != EOK) {
                                hb_slogf_error("%s: %s Cancel int thread failed",
                                    __FUNCTION__, pDevice->pDeviceName);
                        }
                }
        }
        return retValue;
}

int options(int argc, char* const argv[], deviceTable_t** ppDevice)
{
        int i;
        int ret = 0;
        deviceTable_t* pNewDevice = NULL;
        deviceTable_t* pTempDevice = NULL;
        int loadSuccess = 0;
        char* optarg;
        dev_t devno;
        int deviceIndex;
        /*mutiple devices supported. */
        char* deviceName[] = { "dsp", "m3" };
        int deviceCnt = 2;

        if (ppDevice == NULL) {
                DEBUG_ERROR("%s: ppDevice is NULL\n", __FUNCTION__);
                return -1;
        }

        /* At first stage, let hard code all the parameters. */

        for (deviceIndex = 0; deviceIndex < deviceCnt; deviceIndex++) {
                optarg = argv[deviceIndex];
                if (strlen(argv[deviceIndex]) == 0) {
                        continue;
                }
                pNewDevice = (deviceTable_t*)CALLOC_TRC(1, sizeof(deviceTable_t));
                if (pNewDevice == NULL) {
                        ret = -ENOMEM;
                        goto failed;
                }
                pNewDevice->deviceState = STATE_NORMAL;
                pNewDevice->terminate = 0;
                pNewDevice->intNumber = -1;
                pNewDevice->intEvent = -1;
                pNewDevice->intThreadPrio = 20;
                pNewDevice->accessMode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;
                pNewDevice->pSubDevices = NULL;
                pNewDevice->pLastSendSubDevice = NULL;
                pNewDevice->pSubDevices_0 = NULL;
                pNewDevice->pLastSendSubDevice_0 = NULL;
                pNewDevice->pSubDevices_1 = NULL;
                pNewDevice->pLastSendSubDevice_1 = NULL;
                pNewDevice->pSubDevices_2 = NULL;
                pNewDevice->pLastSendSubDevice_2 = NULL;
                pNewDevice->pSubDevices_3 = NULL;
                pNewDevice->pLastSendSubDevice_3 = NULL;
                pNewDevice->pSubDevices_4 = NULL;
                pNewDevice->pLastSendSubDevice_4 = NULL;

                pNewDevice->intChId = -1;
                pNewDevice->intCoId = -1;
                pNewDevice->doLoop = 1;

                pNewDevice->bootTable.state = SECTION_NOINIT;
                pNewDevice->bootTable.fd = -1;
                pNewDevice->bootTable.offset = 0;
                pNewDevice->currMsgId = 0;

                pNewDevice->dev_send_counter = 0;
                pNewDevice->dev_send_counter_done = 0;

                for (i = 0; i < NUM_SW_REGIONS; i++) {
                        pNewDevice->bootTable.regions.region[i].start = 0;
                        pNewDevice->bootTable.regions.region[i].end = 0;
                }
                pthread_mutex_init(&(pNewDevice->sendCntMutex), NULL);
                pthread_mutex_init(&(pNewDevice->currMsgIdMutex), NULL);
                pthread_mutex_init(&(pNewDevice->subDevicesMutex), NULL);
                pthread_cond_init(&(pNewDevice->subDevicesCondVar), NULL);
                pNewDevice->subDevicesReason = REASON_ERROR;

#ifdef CHIP_DRA6XX
                strncpy(pNewDevice->pCslName, "dra6xx", CSL_NAME_LENGTH);
                /*here, we use the same dll file to initial the devices, the difference will be defined inside the dll file. */
                strncpy(pNewDevice->pDllName, "dspipc-dra6xx.so", DLL_NAME_LENGTH);
                /*here, we use the core name as the device name, e.g. "dsp", "m3" */
                strncpy(pNewDevice->pDeviceName, deviceName[deviceIndex], DEV_NAME_LENGTH);

                /*optarg = "int=77,intprio=20,base=0x8f000000,memsize=0x01000000,clockenable=1,dspstartupparam=0x00000031,filetype=elf,filename=DspIpcTestServer.out"; */

#endif

#ifdef CHIP_DRA7XX
                strncpy(pNewDevice->pCslName, "dra7xx", CSL_NAME_LENGTH);
                /*here, we use the same dll file to initial the devices, the difference will be defined inside the dll file. */
                strncpy(pNewDevice->pDllName, "dspipc-dra7xx.so", DLL_NAME_LENGTH);
                /*here, we use the core name as the device name, e.g. "dsp", "m3" */
                strncpy(pNewDevice->pDeviceName, deviceName[deviceIndex], DEV_NAME_LENGTH);

                /*optarg = "int=40,intprio=20,verbose=1,base=0xbf000000,memsize=0x1000000,clockenable=1,dspstartupparam=0x00000031,filename=/mada/DspIpcTestServer.out"; */

#endif

                if (strcmp(pNewDevice->pDeviceName, "dsp") == 0) {
                        /* Linux only. */
                        pNewDevice->major_num = MAJOR_NUMBER;
                        pNewDevice->cur_minor_num = 0;
                        devno = MKDEV(pNewDevice->major_num, pNewDevice->cur_minor_num);

                        ret = register_chrdev_region(devno, MINOR_COUNT, "dspipc");
                        if (ret < 0) {
                                ret = alloc_chrdev_region(&devno, 0, MINOR_COUNT, "dspipc");
                                if (ret == 0) {
                                        pNewDevice->major_num = MAJOR(devno);
                                        DEBUG_ERROR("%s: register chrdev region failed, alloced successfully, major=(%d)\n", __FUNCTION__, MAJOR(devno));
                                } else {
                                        DEBUG_ERROR("%s: alloc chrdev region failed!\n", __FUNCTION__);
                                        goto failed;
                                }
                        }

                        /* add udev support. */
                        pNewDevice->driverclass = class_create(THIS_MODULE, "dspipc");
                        if (!pNewDevice->driverclass) {
                                ret = -ENOMEM;
                                DEBUG_ERROR("%s:class create failed!\n", __FUNCTION__);
                        }
                } else if (strcmp(pNewDevice->pDeviceName, "m3") == 0) {

                        pNewDevice->major_num = MAJOR_NUMBER + 1;
                        pNewDevice->cur_minor_num = 0;
                        devno = MKDEV(pNewDevice->major_num, pNewDevice->cur_minor_num);

                        ret = register_chrdev_region(devno, MINOR_COUNT, "m3ipc");
                        if (ret < 0) {
                                ret = alloc_chrdev_region(&devno, 0, MINOR_COUNT, "m3ipc");
                                if (ret == 0) {
                                        pNewDevice->major_num = MAJOR(devno);
                                        DEBUG_ERROR("%s: register chrdev region failed, alloced successfully, major=(%d)\n", __FUNCTION__, MAJOR(devno));
                                } else {
                                        DEBUG_ERROR("%s: alloc chrdev region failed!\n", __FUNCTION__);
                                        goto failed;
                                }
                        }

                        /* add udev support. */
                        pNewDevice->driverclass = class_create(THIS_MODULE, "m3ipc");
                        if (!pNewDevice->driverclass) {
                                ret = -ENOMEM;
                                DEBUG_ERROR("%s:class create failed!\n", __FUNCTION__);
                        }

                } else {
                        continue;
                }

                pTempDevice = *ppDevice;

                if (DllLoadAndInit(pNewDevice->pDllName, optarg, pNewDevice) == 0) {
                        pNewDevice->pNext = *ppDevice;
                        *ppDevice = pNewDevice;
                        loadSuccess++;

                } else {
                        /*FREE_TRC( pNewDevice ); */
                        DllDeinit(pNewDevice);
                }
        }

        if (loadSuccess == 0) {
                ret = -1;
        }

        return ret;

failed:
        return ret;
}

void DllDeinit(deviceTable_t* pDevice)
{
        deviceTable_t* pTempDevice;
        while (pDevice) {
                pTempDevice = pDevice;
                if (pDevice->closeFunc != NULL) {
                        if ((*(pDevice->closeFunc))(pDevice) == 0) {
                                DEBUG_INFO_LEVEL(LEVEL_V, "%s: %s Chipset layer closed\n", __FUNCTION__, pDevice->pDeviceName);
                        } else {
                                hb_slogf_error("%s: %s Close chipset layer failed", __FUNCTION__, pDevice->pDeviceName);
                        }
                }
                unregister_chrdev_region(MKDEV(pDevice->major_num, 0), MINOR_COUNT);
                class_destroy(pDevice->driverclass);
                pDevice = pDevice->pNext;
                FREE_TRC(pTempDevice);
        }
}
