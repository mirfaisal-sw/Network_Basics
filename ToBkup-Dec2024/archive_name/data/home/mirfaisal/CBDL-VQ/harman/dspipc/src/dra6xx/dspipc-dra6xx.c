/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file dspipc-dra6xx.c
 *
 * This is the implementation for the dra6xx dspipc chipset layer.
 * for Jacinto 5
 *
 * @author Andreas Abel
 * @date   25.10.2010
 *
 * Copyright (c) 2010 Harman/Becker Automotive Systems GmbH
 */

/*=========================    Includes   ====================================== */
#include <asm/fcntl.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "../cache_ops.h"
#include "../debug.h"
#include "../dev-dspipc.h"
#include "../dspipcproto.h"
#include "../misc.h"
#include "../string_def_ext.h"
#include "armdef.h" /* Register Value Definition of ARM+DSP Core */
#include "coff.h"
#include "dra6xx_defs.h"
#include "dsp/DSPIPCTypes.h"
#include "dsp/dra6xxcommstructs.h"  /* description of the communication to dra6xx */
#include "dsp/dra6xxemifipcdebug.h" /* filename array for assert messages */
#include "dspipc-dra6xx.h"
#include "malloc_trc.h" /* Trace malloc */

#if COMMANDMSG_SIZE > DEVCTL_MSG_SIZE
#error size of devctl interface is too small for communicion
#endif

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

#ifdef dspIpcChipset_t
#error More then one chipset header included
#endif

#define ALLOW_FREE_CALLS

/**
 * Define the chipset handle type to the dra6xx handle type
 */
#define dspIpcChipset_t dspIpcDra6xx_t

#ifdef VARIANT_dll
/**
 * Define the symbol to be created for holding the version information (will be
 * patched into this symbol array after build process by patch_qinfo tool). This
 * symbol is usualy _QNX_info but if used inside a library it is extended by
 * the name of the library (e.g. _QNX_info_pcidma). All '-' inside the name has
 * to be replaced by a '_'.
 */
#define QNX_INFO_SYM _QNX_info_dspipc_dra6xx
#endif

#define MAP_FAILED 0

/**
 *  types for memory access directions
 * @enum MEMACCESSDIR_E
 */
enum MEMACCESSDIR_E {
        MEM_READ, /* set access direction to read */
        MEM_WRITE /* set access direction to write */
};

/**
 *  Create a typedef to use the MEMACCESSDIR_E enumeration
 * @typedef memAccessDir_t
 */
typedef enum MEMACCESSDIR_E memAccessDir_t;

/**
 * Macro to convert the DSP addresses used on Host side. If a address (pointer)
 * is used a
 */
#define DRA6XX_CONV_DSP2HOST(x, y) ((uint32_t)(y) - (uint32_t)NULL) /* DSP MemAdr is now used for internal mepped Memory !!! */

/**
 * Define the time to wait for the acknowledge from the DSP after downloading
 * and starting the bootloader
 */
#define BOOTLOADER_ACK_WAIT_US 100000

/*============================================================================== */
/*===================    Modul Global Variables   ============================== */
dspIpcChipset_t* pChipset_Coff = NULL;

#ifdef VARIANT_dll
/**
 * Placeholder for 256 bytes inside the ELF data section. This section cannot
 * be stripped and therefore is the only position to store label information
 * permanent.
 * The contents will be filled afterwars during the build process (after
 * linking) by calling the patch_qinfo untility (located inside the BuildProcess
 * folder)
 */
const char QNX_INFO_SYM[256] = { 0 };

/**
   This function browse through the strings added into the ELF header and search
   for the specified field. If the field is found in the ELF header, the pointer
   to the assigned content is returned. If the specified field is not inside the
   ELF header a pointer to a "?" string is delivered.

   @param name Pointer to a string containing the entry to search for inside the
               ELF header
   @return static const char* pointer to the entry or pointer to a "?" if not found
*/
static const char* get_QNX_info(const char* name)
{
        /*
    unsigned   nl;
    const char *p, *e;

    if(name!=NULL)
    {
       nl = strlen(name);
        for (p = QNX_INFO_SYM, e = p + sizeof(QNX_INFO_SYM); p < e && p[0]; p += strlen(p) + 1)
                if (strncmp(p, name, nl) == 0 && p[nl] == '=')
                        return (p + nl + 1);
}
*/
return "?";
}
#endif

#ifdef VERBOSE_ENABLE
static int opt_nocomm = 0;
#endif

/**
 * @brief Array with all filnumber/filename combinations
 *
 * This array is used to identiy the name of the file in which
 * an assert was taken
 */
static DSPIPCDra6xxFileNames DSPIPCdra6xxFileNameArray[] = {
        { FNUM_DRA6XXEMIFIPC_C, "DRA6XXEMIFIPC.c" },
        { 0, NULL }
};

/**
 * Define the suboption enumeration. The numbers behind the identifiers has
 * to match the order inside the subOpts array
 * @enum DRA6XX_SUBOPTIONS_E
 */
enum DRA6XX_SUBOPTIONS_E {
        DRA6XX_SUB_OPT_FILENAME = 0,           /* Index of the 'filename' token inside the suboption string array */
        DRA6XX_SUB_OPT_VERBOSE = 1,            /* Index of the 'verbose' token inside the suboption string array */
        DRA6XX_SUB_OPT_INTPRIO = 2,            /* Index of the 'intprio' token inside the suboption string array */
        DRA6XX_SUB_OPT_SUPPRESS = 3,           /* Index of the 'suppress' token inside the suboption string array */
        DRA6XX_SUB_OPT_SIMULATOR = 4,          /* Index of the 'simulator' token inside the suboption string array */
        DRA6XX_SUB_OPT_LOADER = 5,             /* Index of the 'loader' token inside the suboption string array */
        DRA6XX_SUB_OPT_COPYMODE = 6,           /* Index of the 'copymode' token inside the suboption string array */
        DRA6XX_SUB_OPT_DMACHAN = 7,            /* Index of the 'dmachan' token inside the suboption string array */
        DRA6XX_SUB_OPT_INT = 8,                /* Index of the 'int' token inside the suboption string array */
        DRA6XX_SUB_OPT_BASE = 9,               /* Index of the 'base' token inside the suboption string array */
        DRA6XX_SUB_OPT_DUMMYBL = 10,           /* Index of the 'dummybl' token inside the suboption string array */
        DRA6XX_SUB_OPT_MEMSIZE = 11,           /* Index of the 'memsize' token inside the suboption string array */
        DRA6XX_SUB_OPT_DSP_STARTUP_PARAM = 12, /* Index of the 'startupParam' token inside the suboption string array */
        DRA6XX_SUB_OPT_CLOCK_ENABLE = 13,      /* Index of the 'clockenable' token inside the suboption string array */
        DRA6XX_SUB_OPT_STREAM_BASE = 14,       /* Index of the 'streambase' token inside the suboption string array */
        DRA6XX_SUB_OPT_STREAM_SIZE = 15,       /* Index of the 'streamsize' token inside the suboption string array */
#ifdef VERBOSE_ENABLE
        DRA6XX_SUB_OPT_NOCOMM = 16, /* Index of the 'nocomm' token inside the suboption string array */
#endif
        DRA6XX_SUB_OPT_FILETYPE = 17 /*/coff or elf?  add by da */
};

/**
 * Array with all possible suboption token for the DRA300 chipset layer
 */
static char* subOpts[] = { /*  0 */ "filename",
        /*  1 */ "verbose",
        /*  2 */ "intprio",
        /*  3 */ "suppress",
        /*  4 */ "simulator",
        /*  5 */ "loader",
        /*  6 */ "copymode",
        /*  7 */ "dmachan",
        /*  8 */ "int",
        /*  9 */ "base",
        /* 10 */ "dummybl",
        /* 11 */ "memsize",
        /* 12 */ "dspstartupparam",
        /* 13 */ "clockenable",
        /* 14 */ "streambase",
        /* 15 */ "streamsize",
#ifdef VERBOSE_ENABLE
        /* 16 */ "nocomm",
#endif
        /* 17 */ "filetype",
        NULL };

static int irqVirtualNum = 0;
static int numFail = 0;
static int notReadyTimes = 0;
/*============================================================================== */
/*===================    forward declarations    =============================== */

int dspipc_dra6xx_loadFirmware(const deviceTable_t* pDevice);
int dspipc_dra6xx_startFirmware(const deviceTable_t* pDevice);
int dspipc_dra6xx_startApplication(const deviceTable_t* pDevice);
int dspipc_dra6xx_startCommunication(const deviceTable_t* pDevice);
int dspipc_dra6xx_registerShmObj(const deviceTable_t* pDevice, const char* pName,
    uint32_t physAddr, uint32_t size);
int dspipc_dra6xx_deregisterShmObj(const deviceTable_t* pDevice, const char* pName);
int dspipc_dra6xx_send(subDeviceTable_t* pSubDevice);
int dspipc_dra6xx_send_drain(subDeviceTable_t* pSubDevice, drainSendMode_t mode);
int dspipc_dra6xx_intrThread(deviceTable_t* pDevice);
int dspipc_dra6xx_close(deviceTable_t* pDevice);
/*int dspipc_dra6xx_getNewMessageId( const deviceTable_t* pDevice, uint32_t* pId ); */
int dspipc_dra6xx_getDspSwVers(const deviceTable_t* pDevice, dspipc_dspSwVers_t* pVersion);
int dspipc_dra6xx_resetState(const deviceTable_t* pDevice, resetType_t type);

/*==== internal helper functions == */
static void dra6xxRegWrite32(const dspIpcChipset_t* pChipset, uint32_t RegModSel, uint32_t addr, volatile uint32_t val);
static void dra6xxRegRead32(const dspIpcChipset_t* pChipset, uint32_t RegModSel, uint32_t addr, volatile uint32_t* pVal);
static int dra6xxAttach(dspIpcChipset_t* pChipset);
static int dra6xxSendBootTable(dspIpcChipset_t* pChipset, int bootFd);
static int dra6xxSetConfigState(dspIpcChipset_t* pChipset, dra6xxConfigState_t newState);
#ifdef SIMULATOR
static int dra6xxPrepareInterrupt(const deviceTable_t* pDevice);
#endif
static int dra6xxInterruptUnmask(const deviceTable_t* pDevice);
static void dra6xxSetInterrupt(dspIpcChipset_t* pChipset);
static int dra6xxMemWrite(const dspIpcChipset_t* pChipset, const uint32_t addr, const uint32_t value);
static int dra6xxMemRead(const dspIpcChipset_t* pChipset, const uint32_t addr, volatile uint32_t* dest);
int dra6xxAccessMem32(dspIpcChipset_t* pChipset,
    uint32_t addr,
    uint32_t numLongWords,
    memAccessDir_t dir,
    uint32_t* pVal);
int dra6xxAccessMem8(dspIpcChipset_t* pChipset,
    uint32_t addr,
    uint32_t numBytes,
    memAccessDir_t dir,
    uint8_t* pVal);

static void dra6xxDumpMessage(const char* pString, DSPIPCMessageQueueEntry* pMsg);
static const char* CmdToString(uint32_t cmd);
#ifdef DUMP_DATA
static void dra6xxDumpData(const char* pString, void* pMsg, unsigned int size);
#endif
static uint32_t dra6xx_get_queue_index(dspIpcChipset_t* pChipset, uint32_t queueOff,
    int32_t* pRead, int32_t* pWrite);
static uint32_t dra6xx_set_queue_index(dspIpcChipset_t* pChipset, uint32_t queueOff,
    const int32_t* pRead, const int32_t* pWrite);
static int dra6xxSendMsg(dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg, uint32_t pPhysMsg,
    const dra6xxSendIndicator_t setInterrupt);
static int ProcessNewMessage(dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg, int* newScan);
static int ProcessInternalMessage(dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg);
static int ProcessCommandStreamMessage(const dspIpcChipset_t* pChipset, const DSPIPCMessageQueueEntry* pMsg, int* newScan);
static int ProcessInternalAdmin(dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg);
static int dra6xxDrain(subDeviceTable_t* pSubDevice);
static int ProcessInternalPrint(const dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg);
static int ProcessInternalLog(const dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg);

static void dra6xxMemWrite_oc(const dspIpcChipset_t* pChipset, uint32_t addr, const uint32_t* value, uint32_t numLongWords);
static void dra6xxMemRead_oc(const dspIpcChipset_t* pChipset, uint32_t addr, uint32_t* dest, uint32_t numLongWords);
static int dra6xxAccessMem32_DMA(dspIpcChipset_t* pChipset, uint32_t dps_addr, uint32_t numLongWords, memAccessDir_t dir, uint32_t* pVal, uint32_t pVal_phys);

#if 0
static int irqMailboxNum = 0;
static int dra6xx_mailboxInitDrv(void);
static int dra6xx_mailboxExitDrv(void);
#endif
static int dra6xx_mailbox_queue_status(dspIpcChipset_t* pChipset);
static int setMemSelAdr(dspIpcChipset_t* pChipset, uint32_t addr);

extern uint32_t load_elf_file(uint32_t iaddr, int verbose);
extern uint32_t load_coff_file(uint32_t iaddr, int verbose);
/*============================================================================== */
/*===================    Function Definitions    =============================== */

#define __DO_INIT_PRINTOUTS

/**
 *  This function initialize the dm642 dsp chipset layer
 *
 *  @param pDevice Pointer to the device table structure
 *  @param pOptions Pointer to the option string
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_init(deviceTable_t* pDevice, char* pOptions)
{
        int retValue = 0;
        unsigned long int value;
        dspIpcChipset_t* pChipset;
        char* pValue;
        int opt;
        copyMode_t copyMode;
        volatile uint32_t mem_val;
        uint32_t count = 0;
#ifdef SIMULATOR
        const char* pTemp;
        int enumDigits;
        unsigned long int enumVal;
#endif

#ifdef VARIANT_dll
        /* Make an slog entry in the defined format */
        hbSlogProcessStart();
        hb_slogf_info("Label: %s QNX: %s Built: %s",
            get_QNX_info("PLABEL"),
            get_QNX_info("QNX_LABEL"),
            get_QNX_info("DATE"));
#endif

        if (pDevice != NULL) {
                pDevice->accessMode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;
                pDevice->flags = PROVIDE_PHYSICAL | CLIENT_INITIATE_STREAM_COPY;
                pDevice->dllIfaceVersion = DSPIPC_DLL_INTERFACE_VERSION;
                pDevice->loadFWFunc = dspipc_dra6xx_loadFirmware;
                pDevice->startFWFunc = dspipc_dra6xx_startFirmware;
                pDevice->startAppFunc = dspipc_dra6xx_startApplication;
                pDevice->startComFunc = dspipc_dra6xx_startCommunication;
                pDevice->registerShmObjFunc = NULL;
                pDevice->deregisterShmObjFunc = NULL;
                pDevice->sendFunc = dspipc_dra6xx_send;
                pDevice->sendStreamDataFunc = NULL;
                pDevice->sendDrainFunc = dspipc_dra6xx_send_drain;
                pDevice->intThread = dspipc_dra6xx_intrThread;
                pDevice->closeFunc = dspipc_dra6xx_close;
                pDevice->dumpStateFunc = NULL;
                pDevice->getDspSwVers = dspipc_dra6xx_getDspSwVers;
                pDevice->resetState = dspipc_dra6xx_resetState;
                pDevice->intNumber = -1;

                pChipset = (dspIpcChipset_t*)CALLOC_TRC(1, sizeof(dspIpcChipset_t));
                if (pChipset != NULL) {
                        pDevice->pChipset = (void*)pChipset;
                        pChipset->pDevice = pDevice; /*backlink the chipset structure to the device structure */
                        /* initialize structure */
                        pChipset->memAddr = NULL;
                        pChipset->memAddr_phys = 0;
                        pChipset->dspMemBase = 0;
                        pChipset->EmifMemBase = 0;
                        pChipset->dspRootAddr = DRA6XX_ROOTPOINTER_ADDR; /* DRA6XX_ROOTPOINTER_ADDR   ROOTPOINTER_ADDR */
                        pChipset->regModCtrl = NULL;
                        pChipset->regModConf = NULL;
                        pChipset->verbose = 0;
                        pChipset->suppress = 0;
                        pChipset->heartBeatPeriodeMs = 0;
                        pChipset->configState = DRA6XX_NOT_INITIALIZED;
                        pChipset->copyMode = COPYMODE_CPU;
                        pChipset->dmaChannel = -1;
                        pChipset->dummyBL = 0;
                        pChipset->memShiftDummyBL = 0;
                        pChipset->MemSize = 0x1000000; /* default 16MB */
                        pChipset->pageMask = 0xFFFFFFFFU - (pChipset->MemSize - 1);
                        pChipset->AdrCheckPageMask = (pChipset->MemSize - 1);
                        pChipset->DspStartupParam = 0;
                        pChipset->pReadDMABuffer = NULL;
                        pChipset->ReadDMABuffer_phys = 0;
                        pChipset->pWriteDMABuffer = NULL;
                        pChipset->WriteDMABuffer_phys = 0;
                        pChipset->memAddr_1 = NULL;
                        pChipset->memAddr_1_phys = 0;
                        pChipset->clockenable = 0;
                        pChipset->pBootLoaderData = MALLOC_TRC(sizeof(bootLoaderData_t));
                        if (pChipset->pBootLoaderData == NULL) {
                                hb_slogf_error("%s: Allocate memory for bootloader data failed", __FUNCTION__);
                                retValue = -1;
                        }
                        if ((retValue = pthread_mutex_init(&(pChipset->configStateCondMutex), NULL)) != EOK) {
                                hb_slogf_error("%s: Config state mutex init failed <%d>", __FUNCTION__, retValue);
                                retValue = -1;
                        }
                        if ((retValue = pthread_cond_init(&(pChipset->configStateCondVar), NULL)) != EOK) {
                                hb_slogf_error("%s: Config state cond var init failed <%d>", __FUNCTION__, retValue);
                                retValue = -1;
                        }
                        pChipset->currMsgId = 0;
                        if ((retValue = pthread_mutex_init(&(pChipset->currMsgIdMutex), NULL)) != EOK) {
                                hb_slogf_error("%s: MsgId mutex init failed <%d>", __FUNCTION__, retValue);
                        }
                        if ((retValue = pthread_mutex_init(&(pChipset->memAccessMutex), NULL)) != EOK) {
                                hb_slogf_error("%s: MemAccess mutex init failed <%d>", __FUNCTION__, retValue);
                        }
                        pChipset->dspIntMarker = 0;
                        if ((retValue = pthread_mutex_init(&(pChipset->dspIntCondMutex), NULL)) != EOK) {
                                hb_slogf_error("%s: dsp int cond mutex init failed <%d>", __FUNCTION__, retValue);
                                retValue = -1;
                        }
                        if ((retValue = pthread_cond_init(&(pChipset->dspIntCondVar), NULL)) != EOK) {
                                hb_slogf_error("%s: dsp int cond var init failed <%d>", __FUNCTION__, retValue);
                                retValue = -1;
                        }

                        pChipset->pLoaderFileName = NULL;
                        pChipset->pFileName = NULL;
                        pChipset->pFileType = NULL;

                        /* parse suboptions */
                        if (pOptions != NULL) {
                                while (*pOptions != '\0') {
                                        if ((opt = getsubopt(&pOptions, (const char* const*)subOpts, &pValue)) != -1) {
                                                switch (opt) {
                                                case DRA6XX_SUB_OPT_FILETYPE:
                                                        pChipset->pFileType = CALLOC_TRC(strlen(pValue) + 1, sizeof(char));
                                                        if (pChipset->pFileType != NULL) {
                                                                strncpy(pChipset->pFileType, pValue, strlen(pValue) + 1);
                                                        } else {
                                                                hb_slogf_error("%s: No mem for filetype", __FUNCTION__);
                                                                retValue = -1;
                                                        }
                                                        break;
                                                case DRA6XX_SUB_OPT_FILENAME:
                                                        pChipset->pFileName = CALLOC_TRC(strlen(pValue) + 1, sizeof(char));
                                                        if (pChipset->pFileName != NULL) {
                                                                strncpy(pChipset->pFileName, pValue, strlen(pValue) + 1);
                                                        } else {
                                                                hb_slogf_error("%s: No mem for filename", __FUNCTION__);
                                                                retValue = -1;
                                                        }
                                                        break;
                                                case DRA6XX_SUB_OPT_VERBOSE:
                                                        pChipset->verbose = strtoul(pValue, NULL, 0);
#ifdef VERBOSE_ENABLE
#else
                                                        hb_slogf_error(V_VERBOSE_MESSAGE, __FUNCTION__, "-verbose");
                                                        pChipset->verbose = 0;
#endif
                                                        break;
                                                case DRA6XX_SUB_OPT_INTPRIO:
                                                        value = strtoul(pValue, NULL, 0);
                                                        if ((value > 0) && (value < 255)) {
                                                                pDevice->intThreadPrio = (int)(value & 0xFFUL);
                                                        } else {
                                                                hb_slogf_error("%s: int prio %ld out of range (1..254)", __FUNCTION__, value);
                                                                retValue = -1;
                                                        }
                                                        break;
                                                case DRA6XX_SUB_OPT_INT:
                                                        value = strtoul(pValue, NULL, 0);
                                                        if (value < 80) {
                                                                pDevice->intNumber = (int)(value & 0x7FUL);
                                                        } else {
                                                                hb_slogf_error("%s: int number %ld out of range (0..14)", __FUNCTION__, value);
                                                                retValue = -1;
                                                        }
                                                        break;
                                                case DRA6XX_SUB_OPT_SUPPRESS:
                                                        pChipset->suppress = 1; /* suppress is alway allowed to suppress download of DSP-Application */
                                                        break;

                                                case DRA6XX_SUB_OPT_DUMMYBL:
#ifdef DSP_DEBUG_ENABLE
                                                        pChipset->suppress = 1;
                                                        pChipset->dummyBL = 1;
                                                        pChipset->memShiftDummyBL = 1024;
#else
                                                        if (pChipset->verbose > 0) {
                                                                hb_slogf_error(V_VERBOSE_MESSAGE, __FUNCTION__, "-dummybl");
                                                        }
#endif
                                                        break;

                                                case DRA6XX_SUB_OPT_COPYMODE:
                                                        if (pValue != NULL) {
                                                                copyMode = COPYMODE_UNKNOWN;
                                                                if (strcmp(pValue, "cpu") == 0) {
                                                                        copyMode = COPYMODE_CPU;
                                                                }
                                                                if (strcmp(pValue, "hostdma") == 0) {
                                                                        copyMode = COPYMODE_HOSTDMA;
                                                                }

                                                                if (copyMode != COPYMODE_UNKNOWN) {
                                                                        pChipset->copyMode = copyMode;
                                                                } else {
                                                                        hb_slogf_error("Suboption copymode=%s is invalid", pValue);
                                                                        retValue = -1;
                                                                }
                                                        } else {
                                                                hb_slogf_error("Suboption copymode without value");
                                                                retValue = -1;
                                                        }
                                                        break;
                                                case DRA6XX_SUB_OPT_DMACHAN:
                                                        value = strtoul(pValue, NULL, 0);

                                                        hb_slogf_error("%s: INFO: DMA usage not available up to now!!!!value is %ld", __FUNCTION__, value);

                                                        break;

                                                case DRA6XX_SUB_OPT_BASE:
                                                        value = strtoul(pValue, NULL, 0);
#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 1) {
                                                                hb_slogf_error("%s: Got baseaddress %lX", __FUNCTION__, value);
                                                        }
#endif
                                                        if (value < (unsigned long int)0xFFFFFFFFU) {
                                                                pChipset->EmifMemBase = (uint32_t)(value & 0xFFFFFFFFU);
                                                                pChipset->dspRootAddr = pChipset->EmifMemBase;
                                                        } else {
                                                                hb_slogf_error("%s: base %lX out of range (0..0xFFFFFFFF)", __FUNCTION__, value);
                                                                retValue = -1;
                                                        }
                                                        break;

                                                case DRA6XX_SUB_OPT_LOADER:
                                                        pChipset->pLoaderFileName = CALLOC_TRC(strlen(pValue) + 1, sizeof(char));
                                                        if (pChipset->pLoaderFileName != NULL) {
                                                                strncpy(pChipset->pLoaderFileName, pValue, strlen(pValue) + 1);
                                                        } else {
                                                                hb_slogf_error("%s: No mem for loader filename", __FUNCTION__);
                                                                retValue = -1;
                                                        }
                                                        break;
                                                case DRA6XX_SUB_OPT_MEMSIZE:
                                                        value = strtoul(pValue, NULL, 0);
#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 1) {
                                                                hb_slogf_error("%s: MemSize:%lX", __FUNCTION__, value);
                                                        }
#endif
                                                        if (value <= (unsigned long int)0x2000000U) /* 32MB */
                                                        {
                                                                pChipset->MemSize = (uint32_t)value;
                                                                pChipset->pageMask = 0xFFFFFFFFU - (pChipset->MemSize - 1);
                                                                pChipset->AdrCheckPageMask = (pChipset->MemSize - 1);
                                                        } else {
                                                                hb_slogf_error("%s: DSP-MemSize %lX out of range (0..0x2000000U)", __FUNCTION__, value);
                                                                retValue = -1;
                                                        }
                                                        break;

#ifdef VERBOSE_ENABLE
                                                case DRA6XX_SUB_OPT_NOCOMM:
                                                        hb_slogf_error("%s: DO NOT start Communication !!!!!!", __FUNCTION__);
                                                        opt_nocomm++;
                                                        break;
#endif

                                                case DRA6XX_SUB_OPT_DSP_STARTUP_PARAM:
                                                        value = strtoul(pValue, NULL, 0);
#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 1) {
                                                                hb_slogf_error("%s: DspStartupParam: %lX", __FUNCTION__, value);
                                                        }
#endif
                                                        pChipset->DspStartupParam = (uint32_t)value;
                                                        break;

                                                case DRA6XX_SUB_OPT_CLOCK_ENABLE:
                                                        value = strtoul(pValue, NULL, 0);
#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 1) {
                                                                hb_slogf_error("%s: do clockanable", __FUNCTION__);
                                                        }
#endif
                                                        pChipset->clockenable = (uint32_t)value;
                                                        break;
                                                case DRA6XX_SUB_OPT_STREAM_BASE:
                                                        value = strtoul(pValue, NULL, 0);
#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 1) {
                                                                hb_slogf_error("%s:set stream base:%lX", __FUNCTION__, value);
                                                        }
#endif
                                                        if (value < (unsigned long int)0xFFFFFFFFU) {
                                                                shareMemPhyAddr = (uint32_t)(value & 0xFFFFFFFFU);
                                                                hb_slogf_error("%s: stream memory base: 0x%x", __FUNCTION__, shareMemPhyAddr);
                                                        } else {
                                                                hb_slogf_error("%s: stream memory base %lX out of range (0..0xFFFFFFFF)", __FUNCTION__, value);
                                                                retValue = -1;
                                                        }
                                                        break;
                                                case DRA6XX_SUB_OPT_STREAM_SIZE:
                                                        value = strtoul(pValue, NULL, 0);
#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 1) {
                                                                hb_slogf_error("%s:set stream size:%lX", __FUNCTION__, value);
                                                        }
#endif
                                                        if (value < (unsigned long int)0x00400000U) {
                                                                shareMemPhySize = (uint32_t)value;
                                                                hb_slogf_error("%s: stream size:%X", __FUNCTION__, shareMemPhySize);
                                                        } else {
                                                                hb_slogf_error("%s: stream memory size 0x%lx is out of range (0..0x00400000U)", __FUNCTION__, value);
                                                                retValue = -1;
                                                        }
                                                        break;
                                                default:
                                                        break;
                                                }
                                        } else {
                                                /* option not found */
                                                if (*pOptions != '\0') {
                                                        hb_slogf_error("%s: unknown sub option", __FUNCTION__);
                                                        retValue = -1;
                                                }
                                                break;
                                        }
                                } /* end while Option parsing */

                                if (pChipset->suppress == 0) /* suppress=0 make Download */
                                {
                                        if (pChipset->pFileName == NULL) {
                                                hb_slogf_error(NO_APPLICATION_FILE_FOUND, __FUNCTION__);
                                                retValue = -1;
                                        }
                                }
                        } else {
                                hb_slogf_error("%s: no option pointer", __FUNCTION__);
                                retValue = -1;
                        }

                        if (irqVirtualNum <= 0) {
                                irqVirtualNum = irq_create_mapping(NULL, pChipset->pDevice->intNumber);
                                if (irqVirtualNum <= 0) {
                                        hb_slogf_error("%s: Failed to irq_create_mapping from hardware irq %d to vitural irq,use the default hardware irq number.", __FUNCTION__, pChipset->pDevice->intNumber);
                                } else {

                                        hb_slogf_error("%s:irq_create_mapping from hardware irq %d to vitural irq %d", __FUNCTION__, pChipset->pDevice->intNumber, irqVirtualNum);
                                        pChipset->pDevice->intNumber = irqVirtualNum;
                                }
                        } else {
                                pChipset->pDevice->intNumber = irqVirtualNum;
                                hb_slogf_error("%s: mailbox irq has been mapped, use mapped irq:%d", __FUNCTION__, pChipset->pDevice->intNumber);
                        }

                        if (retValue == 0) {
                                if (dra6xxAttach(pChipset) == 0) {
                                        if (pChipset->suppress == 0) /* Download of DSP-Application should be done */
                                        {

                                                mem_val = 0x00000003;                                                /* ; */
                                                dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTST, mem_val); /* Read RM_DSP_RSTST Register */

                                                /* hard reset DSP */
                                                hb_slogf_error("%s: step 1) Reset", __FUNCTION__);

                                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, DSP_IDLE_CFG_REG, &mem_val); /* Read DSP_IDLE_CFG_REG Register                   */

                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, CM_DSP_CLKSTCTRL, &mem_val); /* Read CM_DSP_CLKSTCTRL Register                   */
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, CM_DSP_CLKCTRL, &mem_val);   /* Read CM_DSP_CLKCTRL Register */

                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, PM_DSP_PWRSTCTRL, &mem_val); /* Read PM_DSP_PWRSTCTRL Register */
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, PM_DSP_PWRSTST, &mem_val);   /* Read PM_DSP_PWRSTST Register */
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTCTRL, &mem_val);   /* Read RM_DSP_RSTCTRL Register                   */
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTST, &mem_val);     /* Read RM_DSP_RSTST Register */

                                                /* Set Local Reset on DSP */
                                                mem_val = 0x00000001; /* (mem_val) | MDCTL_LRST; */
                                                dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTCTRL, (mem_val));
                                                hb_slogf_error("%s: 1) After DSP set reset 0x%x", __FUNCTION__, mem_val);
                                                do {
                                                        mdelay(1);
                                                        count++;
                                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTST, &mem_val); /* Read RM_DSP_RSTST Register */
                                                } while (((mem_val & 0x2) != 0x2) && (count < 100));

                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, PM_DSP_PWRSTCTRL, &mem_val); /* Read PM_DSP_PWRSTCTRL Register */
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, PM_DSP_PWRSTST, &mem_val);   /* Read PM_DSP_PWRSTST Register */
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTCTRL, &mem_val);   /* Read RM_DSP_RSTCTRL Register                   */
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTST, &mem_val);     /* Read RM_DSP_RSTST Register */

                                                /* Power up DSP */
                                                dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, CM_DSP_CLKSTCTRL, 0x2);
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, CM_DSP_CLKCTRL, &mem_val);
                                                dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, CM_DSP_CLKCTRL, mem_val | 0x2);

                                                /*Enable Clock to MMU CFG*/
                                                dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, CM_MMUCFG_CLKSTCTRL, 0x2);
                                                dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, CM_ALWON_MMUCFG_CLKCTRL, 0x2);

                                                /*Enable Clock to Data*/
                                                dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, CM_MMU_CLKSTCTRL, 0x2);
                                                dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, CM_ALWON_MMUDATA_CLKCTRL, 0x2);
                                        } else {
                                                hb_slogf_error("%s: NO DSP-Reset due to suppress=1", __FUNCTION__);
                                        }
                                } else {
                                        hb_slogf_error("%s: attach failed", __FUNCTION__);
                                        retValue = -1;
                                }
                        }
                } else {
                        hb_slogf_error("%s: no mem for admin struct", __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error("%s: device is NULL", __FUNCTION__);
                retValue = -1;
        }

        return (retValue);
}

#define __NEW_MEM_ALLOC
#ifdef NEW_MEM_ALLOC          /* set in common.mk */
#define USE_MEM_OFFSET64_CALL /* use mem_offset64()  */
static char mem_name[MEM_NAME_LENGTH] = "/dev/dspipc/shmem/dspipc_dspmem";
#endif
/**
 *  This function is attaching the chipset to the dra6xx dsp
 *

 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
static int dra6xxAttach(dspIpcChipset_t* pChipset)
{
        int retValue = -1;
        deviceTable_t* pDevice;
        int errorMarker;
        int errCode;

#ifdef NEW_MEM_ALLOC
#else
        size_t mem_len = (size_t)NULL;
#endif
        if (pChipset != NULL) {
                pDevice = pChipset->pDevice;

                if (pDevice != NULL) {
                        errorMarker = 0;
                        if (errorMarker == 0) {
                                /* Set Memory Address where DSP memory starts */
                                pChipset->memAddr_phys_Const = pChipset->EmifMemBase;
                                pChipset->memAddr_phys = pChipset->EmifMemBase;

#ifndef NEW_MEM_ALLOC
                                mem_len = (size_t)pChipset->MemSize;
                                if (pChipset->MemSize >= DSP_MEMORY_SIZE_8M) {
                                        mem_len = (size_t)DSP_MEMORY_SIZE_8M;
                                        pChipset->MemSize = DSP_MEMORY_SIZE_8M;
                                        pChipset->pageMask = 0xFFFFFFFFU - (pChipset->MemSize - 1);
                                        pChipset->AdrCheckPageMask = (pChipset->MemSize - 1);
                                }

                                hb_slogf_error("%s: MemSize:%08X (PageMask:%08X)(AdrPageMask:%08X), phys membase 0x%08X",
                                    __FUNCTION__, pChipset->MemSize, pChipset->pageMask, pChipset->AdrCheckPageMask, pChipset->memAddr_phys_Const);

                                pChipset->memAddr = ioremap_nocache((unsigned long)(pChipset->memAddr_phys_Const - pChipset->memShiftDummyBL), mem_len);

                                if (pChipset->memAddr != NULL) {
                                        pChipset->memAddr = (void __iomem*)((uint32_t __force)pChipset->memAddr + pChipset->memShiftDummyBL);
                                } else {
                                        hb_slogf_error("%s: mmap_device_memory() failed", __FUNCTION__);
                                }

#else /* NEW_MEM_ALLOC */
                                if ((pChipset->MemSize != NULL) && (pChipset->MemSize >= 0x400000)) {
                                        hb_slogf_error("%s: SHM-Name:%s (size:%d)", __FUNCTION__, &mem_name[0], pChipset->MemSize);
                                        pChipset->dsp_mem_Name = &mem_name[0];
                                        pChipset->memAddr = mmap(NULL,
                                            pChipset->MemSize,
                                            PROT_READ | PROT_WRITE | PROT_NOCACHE,
                                            MAP_SHARED | MAP_PHYS,
                                            NOFD,
                                            (off_t)(pChipset->memAddr_phys_Const - pChipset->memShiftDummyBL));
                                        if ((pChipset->memAddr == MAP_FAILED) && (errno == 12)) {
                                                pChipset->dsp_mem_fd = shm_open(pChipset->dsp_mem_Name, O_RDWR | O_CREAT, 0777);
                                                if (pChipset->dsp_mem_fd != -1) {
                                                        if (shm_ctl(pChipset->dsp_mem_fd,
                                                                SHMCTL_PHYS | SHMCTL_LOWERPROT,
                                                                (uint64_t)(pChipset->memAddr_phys_Const - pChipset->memShiftDummyBL),
                                                                (_uint64)pChipset->MemSize)
                                                            != 0) {
                                                                return EXIT_FAILURE;
                                                        } else {
                                                                pChipset->memAddr = mmap(NULL,
                                                                    pChipset->MemSize,
                                                                    PROT_READ | PROT_WRITE | PROT_NOCACHE,
                                                                    MAP_SHARED,
                                                                    pChipset->dsp_mem_fd,
                                                                    0);
                                                                if ((pChipset->memAddr == MAP_FAILED) && (errno == 12)) {
                                                                        hb_slogf_error("%s: Error mmap()", __FUNCTION__);
                                                                        return EXIT_FAILURE;
                                                                }
                                                        }
                                                } else {
                                                        hb_slogf_error("%s: shm_open() failed!", __FUNCTION__);
                                                        return EXIT_FAILURE;
                                                }
                                                if (shm_unlink(pChipset->dsp_mem_Name) != 0) {
                                                        hb_slogf_error("%s: shm_unlink falied", __FUNCTION__);
                                                }
                                        } else {
                                                hb_slogf_error("%s: mmap() failed", __FUNCTION__);
                                        }

                                        if (pChipset->memAddr == MAP_FAILED) {
                                                return EXIT_FAILURE;
                                        }
                                }
#endif

                                DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s: before internal memory, phys membase 0x%08X\n",
                                    __FUNCTION__, (unsigned int)DSP_INTERNAL_MEMORY_START);

                                DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s: before DEV_CONF, phys membase 0x%08X\n",
                                    __FUNCTION__, (unsigned int)PCI_MEM_ADDR(DEV_CONF_MODULE));
                                /* Module Conf Register */
                                pChipset->regModConf = regModConf_dspipc;

                                DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s: before MOD_STAT, phys membase 0x%08X\n",
                                    __FUNCTION__, (unsigned int)PCI_MEM_ADDR(MOD_STAT_CRTL_BASE));

#define DO_INIT_PRINTOUTS
#ifdef DO_INIT_PRINTOUTS
                                hb_slogf_error("%s: VDSPmemAdr=0x%08X     PDSPmemAdr=0x%08X   Len=0x%08X", __FUNCTION__, (uint32_t __force)pChipset->memAddr, pChipset->memAddr_phys_Const, mem_len);
                                hb_slogf_error("%s: VRegConfAdr=0x%08X    PRegConfAdr=0x%08X   Len=0x%08X", __FUNCTION__, (uint32_t __force)pChipset->regModConf, DEV_CONF_MODULE, DEV_CONF_MODULE_MEM_SIZE);
                                hb_slogf_error("%s: VRegCtrlAdr=0x%08X    PRegCtrlAdr=0x%08X   Len=0x%08X", __FUNCTION__, (uint32_t)pChipset->regModCtrl, MOD_STAT_CRTL_BASE, MOD_STAT_CRTL_MEM_SIZE);
                                hb_slogf_error("%s: VdspIntMemBase=0x%08X PdspIntMemBase=0x%08X   Len=0x%08X", __FUNCTION__, pChipset->dspMemBase, DSP_INTERNAL_MEMORY_START, DSP_INTERNAL_MEMORY_SIZE);
#endif

                                if ((pChipset->memAddr != NULL) && (pChipset->regModConf != NULL)) {

#ifdef VERBOSE_ENABLE
                                        if (pChipset->verbose > 0) {
                                                hb_slogf_error("%s: Mapping successful", __FUNCTION__);
                                        }
#endif
                                        /* attach the dma channel */
                                        errCode = EOK;
                                        switch (pChipset->copyMode) {
                                        case COPYMODE_CPU:
                                                break;
                                        case COPYMODE_HOSTDMA:

                                                break;
                                        default:
                                                errCode = -1;
                                                break;
                                        }

                                        if ((errCode != EOK) && (errCode != EBUSY)) /* BUSY means it is initialized */
                                        {
                                                hb_slogf_error("%s: init dma lib failed, fallback to CPU mode",
                                                    __FUNCTION__);
                                                pChipset->dmaChannel = -1;
                                                pChipset->copyMode = COPYMODE_CPU;
                                        }
                     /*---------------------------------------
                      * Read of Message is always done to this buffer (if DMA or not available!!)
                      * no special Var for Message needed in INT Thread
                      *---------------------------------------
                      * Get Chipset READ-DMA Buffer
                      */
                                        pChipset->pReadDMABuffer = (uint32_t*)my_kmalloc(sizeof(DSPIPCMessageQueueEntry));

                                        if (pChipset->pReadDMABuffer != NULL) {

                                                pChipset->ReadDMABuffer_phys = VIRT_TO_PHYS(pChipset->pReadDMABuffer);

#ifdef VERBOSE_ENABLE
                                                {
                                                        hb_slogf_error("%s: R-BUFFER: vAdr:0x%08X  pAdr:0x%08X", __FUNCTION__, (uint32_t)pChipset->pReadDMABuffer, pChipset->ReadDMABuffer_phys);
                                                }
#endif
                                        } else {
                                                hb_slogf_error("%s: Failed to alloc READ-DMA buffer", __FUNCTION__);
                                                return (-1);
                                        }

                                        /* Get Chipset WRITE-DMA Buffer */
                                        pChipset->pWriteDMABuffer = (uint32_t*)my_kmalloc(sizeof(DSPIPCMessageQueueEntry));

                                        if (pChipset->pWriteDMABuffer != NULL) {

                                                pChipset->WriteDMABuffer_phys = VIRT_TO_PHYS(pChipset->pWriteDMABuffer);

#ifdef VERBOSE_ENABLE
                                                {
                                                        hb_slogf_error("%s: W-BUFFER: vAdr:0x%08X  pAdr:0x%08X", __FUNCTION__, (uint32_t)pChipset->pWriteDMABuffer, pChipset->WriteDMABuffer_phys);
                                                }
#endif
                                        } else {
                                                hb_slogf_error("%s: Failed to alloc WRITE-DMA buffer", __FUNCTION__);
                                                return (-1);
                                        }

                                        pChipset->pReadMsg.generic = (void*)my_kmalloc(MESSAGE_POINTER_U_SIZE);
                                        if (pChipset->pReadMsg.generic != NULL) {
                                                hb_slogf_error("%s: read message buf len:%d", __FUNCTION__, MESSAGE_POINTER_U_SIZE);
                                        } else {
                                                hb_slogf_error("%s: init read message pointer failed",
                                                    __FUNCTION__);
                                                return (-1);
                                        }

                                        pChipset->pWriteMsg.generic = (void*)my_kmalloc(MESSAGE_POINTER_U_SIZE);
                                        if (pChipset->pWriteMsg.generic != NULL) {
                                                hb_slogf_error("%s: write message len:%d", __FUNCTION__, MESSAGE_POINTER_U_SIZE);
                                        } else {
                                                hb_slogf_error("%s: init write message pointer failed",
                                                    __FUNCTION__);
                                                return (-1);
                                        }

                                        retValue = 0;

                                } else {
                                        hb_slogf_error("%s: mapping failed", __FUNCTION__);
                                }
                        }
                } else {
                        hb_slogf_error(NO_DEVICE, __FUNCTION__);
                }
        } else {
                hb_slogf_error("%s: invalid parameter", __FUNCTION__);
        }

        return (retValue);
}

/**
 *  This function downloads the firmware to the dra6xx dsp
 *
 *  @param pDevice Pointer to the device table structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_loadFirmware(const deviceTable_t* pDevice)
{
        dspIpcChipset_t* pChipset;
        int retValue = -1;
        int i;
        volatile uint32_t mem_val;
        bootLoaderData_t* pBLData;

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        pChipset = (dspIpcChipset_t*)pDevice->pChipset;
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        /* use binary download with bootloader */
        if ((pChipset->regModCtrl != NULL) || (pChipset->simuFd != -1)) {
                if (pChipset->suppress == 0) /* dev-dspipc is doing the download */
                {
                        pBLData = (bootLoaderData_t*)pChipset->pBootLoaderData;
                        for (i = 0; i < NUM_REGIONS_IN_BL_DATA; i++) {
                                pBLData->region[i].start = 0;
                                pBLData->region[i].end = 0;
                        }
                        pBLData->region[0].start = 0x11800000;
                        pBLData->region[0].end = 0x11FFFFFF;
                        pBLData->region[1].start = pChipset->EmifMemBase; /*0x60000000 */
                        pBLData->region[1].end = pChipset->EmifMemBase + (pChipset->MemSize - 1);

                        retValue = 0;
                } else {
                        if (pChipset->dummyBL != 0) {
                                hb_slogf_error("%s: Set DSP-BOOTADDR", __FUNCTION__);
                                /* Set Startaddress of DSP (Internal L2-RAM of DSP) */
                                dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, DCM_DSPBOOTADDR, pChipset->memAddr_phys_Const);

                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, DCM_DSPBOOTADDR, &mem_val);
                                hb_slogf_error("%s: read: DSP-BOOTADDR: 0x%X", __FUNCTION__, mem_val);

                                /* TEST Fill Mem. before Startaddr  */
                                if (setMemSelAdr(pChipset, pChipset->memAddr_phys_Const - pChipset->memShiftDummyBL) != 0) {
                                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                                }

                                for (i = 0; i < 32; i++) {
                                        (void)dra6xxMemWrite(pChipset, (pChipset->memShiftDummyBL - (4 * (i + 1))), 0x00000000);
                                }

                                /* Set the memory Selector */
                                if (setMemSelAdr(pChipset, pChipset->memAddr_phys_Const) != 0) {
                                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                                }

                                for (i = 0; i < 32; i++) {
                                        (void)dra6xxMemWrite(pChipset, (i * 4), 0x00000000);
                                }

                                (void)dra6xxMemWrite(pChipset, 0x00000000, 0xA120);

                                /* TEST: read written Data */
                                for (i = 0; i < 32; i++) {
                                        (void)dra6xxMemRead(pChipset, (i * 4), &mem_val);
                                }

                                hb_slogf_error("%s: DSP with dummy bootloader started", __FUNCTION__);
                                retValue = 0;
                        } else {
                                hb_slogf_error("%s: Load firmware skipped", __FUNCTION__);
                                retValue = 0;
                        }
                } /* end dummyBL!=0 */
        } else {
                hb_slogf_error("%s: DSP register not mapped", __FUNCTION__);
        }

        return (retValue);
}

/**
 *  This function starts the dsp software and waits for the communication
 *  acknowledge from the DSP. The Jacinto software writes a magic value to a
 *  fixed address to signalize that the communication can start.
 *
 *  @param pDevice Pointer to the device table structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_startFirmware(const deviceTable_t* pDevice)
{
        int retValue = -1;
        dspIpcChipset_t* pChipset;

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        pChipset = (dspIpcChipset_t*)pDevice->pChipset;
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        retValue = 0;

        return (retValue);
}

#define NEW_STRATUP

#define __DOWNLOAD_TRACE
/**
 *  This function starts the dsp application software and waits for the
 *  acknowledge from the DSP. This download and start procedures are done AFTER
 *  enabling the interrupt thread
 *
 *  @param pDevice Pointer to the device table structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_startApplication(const deviceTable_t* pDevice)
{
        int retValue = -1;
        int hFd = -1;
        int sendResult = -1;
        dspIpcChipset_t* pChipset;
        int i;
        uint32_t value, value_1;
        unsigned int delayRemain;

        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        pChipset = (dspIpcChipset_t*)pDevice->pChipset;
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        if (pChipset->suppress == 0) /* suppress=0 make Download */
        {
                /*Reset the variable */
                value = 0;
                if (dra6xxAccessMem32(pChipset,
                        DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, magic),
                        1, MEM_WRITE, &value)
                    != 0) {
                        hb_slogf_error("%s: clear magic value access failed", __FUNCTION__);
                }

                /* It is ensured during Init that these Filenames are there       */
                hFd = open(pChipset->pFileName, O_RDONLY);
                if ((hFd != -1)) {
                        sendResult = dra6xxSendBootTable(pChipset, hFd);
                        if (sendResult != 0) {
                                hb_slogf_error("%s: Application download fails", __FUNCTION__);
                        }
                        if (hFd != -1) {
                                close(hFd);
                        }

                } /* check File descriptor */
                else {
                        hb_slogf_error("%s: Error unable to open File: %s", __FUNCTION__, pChipset->pFileName);
                }
        } /* End Suppress */
        else {
                sendResult = 0;
        }

#ifdef VERBOSE_ENABLE
        if (opt_nocomm != 0) {
                return (-1);
        }
#endif

        /* Set DSP Startup Parameter Value */
        if (dra6xxAccessMem32(pChipset,
                DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, devDspIpcCmdLineParam),
                1, MEM_WRITE, &(pChipset->DspStartupParam)) != 0) {
                hb_slogf_error("%s: set DSP-Startup-Parameter-Value failed", __FUNCTION__);
        } else {
                hb_slogf_error("%s: DSP-Startup-Parameter-Value: %X", __FUNCTION__, pChipset->DspStartupParam);
        }

        if (sendResult == 0) /* If download was successful */
        {
#ifdef IPC_DSP_M3
                value = 0;
                /*wait for DSP ready */
                for (i = 0; i < 3000; i++) {
                        /* read status value */
                        if(value != 0xBEEFDEAD) {
                                if (dra6xxAccessMem32(pChipset,
                                        DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, currentStatus),
                                        1, MEM_READ, &value) != 0) {
                                        hb_slogf_error("%s: Access read current status failed(0x%x) ", __FUNCTION__, value);
                                }
                        }
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 2) {
                                hb_slogf_error("%s: Read current status Value: 0x%x", __FUNCTION__, value);
                        }
#endif

                        if (value == 0xBEEFDEAD) {
                                value_1 = 0;
                                dra6xxAccessMem32(pChipset,
                                    DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, magic),
                                    1, MEM_READ, &value_1);
                                if ((value_1 & 0x000000ff) == 0x5a) {
                                        value_1 = value_1 & 0xffffff00;
                                        dra6xxAccessMem32(pChipset,
                                            DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, magic),
                                            1, MEM_WRITE, &value_1);
                                        retValue = 0;
                                        break;
                                }
#ifdef VERBOSE_ENABLE
                                if (pChipset->verbose > 2) {
                                        hb_slogf_error("%s: Read magic: 0x%x", __FUNCTION__, value_1);
                                }
#endif
                        }
                        delayRemain = 1; /* 1ms */
                        msleep(delayRemain);
                }

                if (retValue == 0) {
                        /* read base address of the dsp admin structure */
                        if (dra6xxAccessMem32(pChipset,
                                DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, rootAdminAddr),
                                1, MEM_READ, &pChipset->dspAdminStructAdr)
                            != 0) {
                                hb_slogf_error("%s: Access failed 3", __FUNCTION__);
                        }

                        hb_slogf_error("%s: %s RootPointer:0x%x,commstructaddress:0x%x", __FUNCTION__, pChipset->pDevice->pDeviceName, pChipset->dspRootAddr, pChipset->dspAdminStructAdr);

                        if (dra6xxSetConfigState(pChipset, DRA6XX_STARTED) != EOK) {
                                hb_slogf_error("%s: failed to call dra6xxSetConfigState", __FUNCTION__);
                        }
                } else {
                        hb_slogf_error("%s: No communication established", __FUNCTION__);
                        hb_slogf_error("%s: Read currentStatus:0x%08X, magic:0x%08X.", __FUNCTION__, value, value_1);
                }

#else

#ifdef VERBOSE_ENABLE
                if (pChipset->verbose) {
                        hb_slogf_error("%s: Before ARM writes 1", __FUNCTION__);
                }
#endif
                value = 1;
                if (dra6xxAccessMem32(pChipset,
                        DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, magic),
                        1, MEM_WRITE, &value) != 0) {
                        hb_slogf_error("%s: clear magic value access failed", __FUNCTION__);
                }

                /*wait for DSP sets 2 */
                for (i = 0; i < 1000; i++) {
                        /* read magic value */
                        if (dra6xxAccessMem32(pChipset,
                                DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, magic),
                                1, MEM_READ, &value) != 0) {
                                hb_slogf_error("%s: Access failed 2", __FUNCTION__);
                        }

#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 2) {
                                hb_slogf_error("%s: Read Value: %d", __FUNCTION__, value);
                        }
#endif
                        if (value == 2) {
                                retValue = 0;
                                break;
                        }
                        delayRemain = 3; /* 3ms */
                        msleep(delayRemain);
                }

                if (retValue == 0) {
                        /* read base address of the dsp admin structure */
                        if (dra6xxAccessMem32(pChipset,
                                DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, rootAdminAddr),
                                1, MEM_READ, &pChipset->dspAdminStructAdr)
                            != 0) {
                                hb_slogf_error("%s: Access failed 3", __FUNCTION__);
                        }
                        if (dra6xxSetConfigState(pChipset, DRA6XX_STARTED) != EOK) {
                                hb_slogf_error("%s: failed to call dra6xxSetConfigState", __FUNCTION__);
                        }
                } else {
                        hb_slogf_error("%s: No communication established", __FUNCTION__);
                        hb_slogf_error("%s: Read 0x%08X instead of magic word 2", __FUNCTION__, value);
                }

                /*after this has happend, the arm knows the dsp is ready, because dsp has correctly reacted to the writing of the 1 */
                /*this cannot be a coincidence, because the arm knows that shared was 1 after his write */
                /*arm can now start to initialize the dsp */
                /*before that, he must get dsp out of its while loop */
                /*shared=3; */
#ifdef VERBOSE_ENABLE
                if (pChipset->verbose) {
                        hb_slogf_error("%s: Before ARM writes 3", __FUNCTION__);
                }
#endif
                value = 3;
                if (dra6xxAccessMem32(pChipset,
                        DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspRootAddr) + offsetof(struct _DSPIPCRootPointer, magic),
                        1, MEM_WRITE, &value) != 0) {
                        hb_slogf_error("%s: clear magic value access failed", __FUNCTION__);
                }
#endif /*endif for ifdef IPC_DSP_M3 */

        } else /* If download was NOT successful */
        {
                hb_slogf_error("%s: Start Application failed", __FUNCTION__);
                retValue = -2;
        }

        if (strcmp(pDevice->pDeviceName, "dsp") == 0) {

                hb_slogf_error("%s: register maibox-0 for DSP=>A8", __FUNCTION__);
                /* register maibox-0 for DSP=>A8 */
                dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQENABLE_SET_REG(0), MAILBOX_IRQENABLE_NEWMSG_SET(0));
        } else if (strcmp(pDevice->pDeviceName, "m3") == 0) {

                hb_slogf_error("%s: register maibox-4 for M3=>A8", __FUNCTION__);
                /* register maibox-4 for M3=>A8 */
                dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQENABLE_SET_REG(0), MAILBOX_IRQENABLE_NEWMSG_SET(8));
        }
        return (retValue);
}

/**
 *  This function starts the dsp communication.
 *
 *  @param pDevice Pointer to the device table structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_startCommunication(const deviceTable_t* pDevice)
{
        int retValue = -1;
        dspIpcChipset_t* pChipset;
        DSPIPCMessageQueueEntry msg;
        int errCode;
#ifdef VERBOSE_ENABLE
        int checkNum = 0;
#endif
        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        pChipset = (dspIpcChipset_t*)pDevice->pChipset;
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }
#ifdef VERBOSE_ENABLE
        if (opt_nocomm != 0) {
                return (-1);
        }
#endif

        /* send version info to DSP */
        msg.channelId = DSPIPC_CHANNELTYPE_INTERNAL;
        msg.channelId |= (DSPIPC_IPCADMIN >> DSPIPC_CHANNELNUMBER_SHIFT) & DSPIPC_CHANNELNUMBER_MSK;
        if (dspipc_getNewMessageId((deviceTable_t*)pDevice, &msg.msgId) == 0) {
                msg.msgId |= DSPIPC_MSGID_DIRECTION_DSPIN;
                msg.msgType = DSPIPC_MSGTYPE_TYPE_COMMAND | DSPIPC_MSGTYPE_REPLY_NOREPLY;
                msg.msgSize = sizeof(msg.payload.adminMsg.generic) + sizeof(msg.payload.adminMsg.content.version);
                msg.payload.adminMsg.generic.cmdType = DCCMID_VERSION;
                msg.payload.adminMsg.content.version.swMajor = DSP_INTERFACE_VERSION_MAJOR;
                msg.payload.adminMsg.content.version.swMinor = DSP_INTERFACE_VERSION_MINOR;
                msg.payload.adminMsg.content.version.swBugfix = DSP_INTERFACE_VERSION_BUGFIX;
                msg.payload.adminMsg.content.version.swSpecial = DSP_INTERFACE_VERSION_SPECIAL;

                if (dra6xxSendMsg(pChipset, &msg, 0, DRA6XX_SEND_DO_INT_TO_DSP) == 0) {
                        errCode = pthread_mutex_lock(&(pChipset->configStateCondMutex));
                        if (errCode == EOK) {
                                switch (pChipset->configState) {
                                case DRA6XX_STARTED:
                                        do {
#ifdef VERBOSE_ENABLE
                                                checkNum++;
#endif
                                                /* condvar signaling is missing due to some magic things, has to be fixed */
                                                errCode = pthread_cond_wait_timeout(&(pChipset->configStateCondVar), &(pChipset->configStateCondMutex), 600);

#ifdef VERBOSE_ENABLE
                                        } while ((errCode != 0) && (checkNum < 10));
#else
                                        } while (errCode != 0);
#endif
                                        switch (errCode) {
                                        case 0:
                                                switch (pChipset->configState) {
                                                case DRA6XX_VERSION_MATCH:
#ifdef VERBOSE_ENABLE
                                                        hb_slogf_error("%s: Version message received in about %dms", __FUNCTION__, (checkNum * 20));
#endif
                                                        retValue = 0;
                                                        break;
                                                case DRA6XX_VERSION_MISMATCH:
                                                        hb_slogf_error("%s: Version not supported",
                                                            __FUNCTION__);
                                                        break;
                                                default:
                                                        hb_slogf_error("%s: Unknown config state",
                                                            __FUNCTION__);
                                                        break;
                                                }
                                                break;
                                        case -ETIMEDOUT:
#ifdef VERBOSE_ENABLE
                                                hb_slogf_error("%s: Version message timed out failed (DSP did not send Version Message in %dms)",
                                                    __FUNCTION__, (checkNum * 20));
#else
                                                hb_slogf_error("%s: Version message timed out failed",
                                                    __FUNCTION__);
#endif
                                                break;
                                        default:
                                                hb_slogf_error("%s: Cond wait failed",
                                                    __FUNCTION__);
                                                break;
                                        }
                                        break;
                                case DRA6XX_VERSION_MATCH:
                                        retValue = 0;
                                        break;
                                case DRA6XX_NOT_INITIALIZED:
                                        hb_slogf_error("%s: State is not initialized", __FUNCTION__);
                                        break;
                                default:
                                        hb_slogf_error("%s: Unknown config state", __FUNCTION__);
                                        break;
                                }
                                errCode = pthread_mutex_unlock(&(pChipset->configStateCondMutex));
                                if (errCode != EOK) {
                                        hb_slogf_error("%s: Config %s failed", __FUNCTION__, MUTEX_UNLOCK_FAILED);
                                }
                        } else {
                                hb_slogf_error("%s: Config state %s", __FUNCTION__, MUTEX_LOCK_FAILED);
                        }
                } else {
                        hb_slogf_error(SEND_MESSAGE_FAILED, __FUNCTION__);
                }
        } else {
                hb_slogf_error(GET_MESSAGE_ID_FAILED, __FUNCTION__);
        }

        /*dump mailbox and communication queue when no response from DSP or Mcore */
        if (retValue != 0) {
                dra6xx_mailbox_queue_status(pChipset);
        }

        return (retValue);
}

/**
 *  This function close the dra6xx dsp chipset layer
 *
 *  @param pDevice Pointer to the device table structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_close(deviceTable_t* pDevice)
{
        int retValue = 0;
        dspIpcChipset_t* pChipset;
        int errCode;

        if (pDevice != NULL) {
                pChipset = (dspIpcChipset_t*)pDevice->pChipset;
                if (pChipset != NULL) {
                        /* stop irq first */
                        if (pDevice->intEvent != -1) {
                                free_irq(pDevice->intNumber, (void*)pDevice);
                                pDevice->intEvent = -1;
                        }
                        DEBUG_INFO_LEVEL(LEVEL_VVV, "%s: irq is freeing\n", __FUNCTION__);

                        /* terminate interrupt thread */
                        /* at this point the blocking state of the interrupt has to be unblocked */
                        /* For this the InterruptWait could be replaced with a MsgReceive and */
                        /* a special message can be send */
                        if (pDevice->intThread != NULL) {
                                DEBUG_INFO_LEVEL(LEVEL_VVV, "%s: interrupt thread is terminating\n", __FUNCTION__);
                                /* Stop Interrupt Thread */
                                if ((retValue = dspipcStopInterruptThread(pDevice)) != EOK) {
                                        hb_slogf_error("%s: Stop interrupt thread failed for %s", __FUNCTION__, pDevice->pDeviceName);
                                }
                        }

                        /* Detach interrupt now done in intthread.c */
                        /* Unmap memory areas */
                        if (pChipset->memAddr != NULL && ((void __force*)pChipset->memAddr != (void*)-1)) {

#ifndef NEW_MEM_ALLOC /* set in common.mk */
                                /* no simulator mode */
                                iounmap((void __iomem*)((uint32_t __force)pChipset->memAddr - (uint32_t)pChipset->memShiftDummyBL));

                                if ((pChipset->memAddr_1 != NULL) && ((void __force*)pChipset->memAddr_1 != (void*)-1)) {
                                        /* no simulator mode */
                                        iounmap(pChipset->memAddr_1);
                                }
#else
                                /* no simulator mode */
                                if (munmap(pChipset->memAddr,
                                        (size_t)pChipset->MemSize) == (void*)-1) 
                                {
                                        hb_slogf_error("%s: Unmap memory area failed", __FUNCTION__);
                                        retValue = -1;
                                }

                                close(pChipset->dsp_mem_fd);
#endif
                        }
                        /*unregister mailbox driver */
                        numFail++;
                        if ((numFail == 2) && (irqVirtualNum > 0)) {
                                irq_dispose_mapping(irqVirtualNum);
                                numFail = 0;
                                irqVirtualNum = 0;
                        }

                        switch (pChipset->copyMode) {
                        case COPYMODE_CPU:
                                break;
                        case COPYMODE_HOSTDMA:

                                break;
                        default:
                                break;
                        }

#ifdef ALLOW_FREE_CALLS
                        if (pChipset->pReadDMABuffer != NULL) {
                                /* Free READ-DMA Buffer */
                                my_kfree(pChipset->pReadDMABuffer);
                        }

                        if (pChipset->pWriteDMABuffer != NULL) {
                                /* Free WRITE-DMA Buffer */
                                my_kfree(pChipset->pWriteDMABuffer);
                        }

                        if (pChipset->pReadMsg.generic != NULL) {
                                /* Free read message Buffer */
                                my_kfree(pChipset->pReadMsg.generic);
                        }

                        if (pChipset->pWriteMsg.generic != NULL) {
                                /* Free write message Buffer */
                                my_kfree(pChipset->pWriteMsg.generic);
                        }

                        if (pChipset->suppress == 0) /* suppress=0 make Download */
                        {
                                /* free Application filename */
                                if (pChipset->pFileName != NULL) {
                                        FREE_TRC(pChipset->pFileName);
                                }
                        }

                        /* destroy mutex/condvar */
                        errCode = pthread_mutex_destroy(&(pChipset->configStateCondMutex));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Destroy config state mutex failed",
                                    __FUNCTION__);
                                retValue = -1;
                        }
                        errCode = pthread_cond_destroy(&(pChipset->configStateCondVar));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Destroy config state convar failed",
                                    __FUNCTION__);
                                retValue = -1;
                        }
                        errCode = pthread_mutex_destroy(&(pChipset->currMsgIdMutex));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Destroy msg id mutex failed",
                                    __FUNCTION__);
                                retValue = -1;
                        }
                        if (pChipset->pBootLoaderData != NULL) {
                                FREE_TRC(pChipset->pBootLoaderData);
                        }

                        errCode = pthread_mutex_destroy(&(pChipset->dspIntCondMutex));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Destroy dsp int mutex failed",
                                    __FUNCTION__);
                                retValue = -1;
                        }
                        errCode = pthread_cond_destroy(&(pChipset->dspIntCondVar));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Destroy dsp int condvar failed",
                                    __FUNCTION__);
                                retValue = -1;
                        }
#if 0
                        /* Close all subdevices  */
                        hb_slogf_error("%s: Begin to remove all sub devices",
                            __FUNCTION__);
                        while (pChipset->pDevice->pSubDevices != NULL) {
                                if (dspipcRemSubDevice(pChipset->pDevice, pChipset->pDevice->pSubDevices) != 0) {
                                        hb_slogf_error("%s: Rem subdevice failed", __FUNCTION__);
                                        retValue = -1;
                                        break;
                                }
                        }
#endif
                        FREE_TRC(pChipset);
                        pDevice->pChipset = NULL;

#endif
                } else {
                        hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                retValue = -1;
        }

#ifdef VARIANT_dll
        /* Log termination of the programm to slog */
        hbSlogProcessTerminate();
#endif

        return (retValue);
}

/**
 *  This function is the getDspSwVers callout for the dra6xx dsp chipset layer
 *
 *  @param pDevice Pointer to the device table structure
 *  @param pVersion Pointer to the version structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_getDspSwVers(const deviceTable_t* pDevice, dspipc_dspSwVers_t* pVersion)
{
        int retValue = -1;

        if (pDevice == NULL) {
                hb_slogf_error("%s invalid device", __FUNCTION__);
                return (-1);
        }

        if (pVersion == NULL) {
                hb_slogf_error("%s invalid parameter", __FUNCTION__);
                return (-1);
        }

        pVersion->swProjCode = 0;
        pVersion->swMajor = 0;
        pVersion->swMinor = 0;
        pVersion->swBugfix = 0;

        return (retValue);
}

/**
 *  This function resets the the DSP (hard-reset) and also resets theState of the Chipset
 *
 *  @param pDevice Pointer to the device table structure
 *  @param type Type of reset (PREPARE_RESET/DO_RESET)
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
/*lint -esym(613,pDevice) pointer is checked in the calling function */
int dspipc_dra6xx_resetState(const deviceTable_t* pDevice, resetType_t type)
{
        dspIpcChipset_t* pChipset;
        int delayRemain;

        pChipset = (dspIpcChipset_t*)pDevice->pChipset;
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }
        if (type == PREPARE_RESET) {
                /* wait 3ms */
                delayRemain = 3;
                msleep(delayRemain);
        } else {
#ifdef DSP_DEBUG_ENABLE
                if (pChipset->suppress == 0)

                {
#endif

#ifdef DSP_DEBUG_ENABLE
                }
#endif
                /* wait 3ms */
                delayRemain = 3;
                mdelay(delayRemain);

                if (dra6xxSetConfigState(pChipset, DRA6XX_NOT_INITIALIZED) != EOK) {
                        hb_slogf_error("%s: failed to call JacintoSetConfigState", __FUNCTION__);
                }
        }

        return 0;
}

/**
 * Send function to handle an outgoing message queue to the DSP
 *
 *  @param pSubDevice Pointer to the sub device structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_send(subDeviceTable_t* pSubDevice)
{
        uint32_t i;
        deviceTable_t* pDevice;
        dspIpcChipset_t* pChipset = NULL;
        int retValue = -1;
        int errCode;
        uint32_t queueMsgSize;
        uint32_t queueMsgId;
        uint32_t queueCmdId;
        uint32_t queueNumLost;
        dspipc_msgType_t queueMsgType;
        dspipc_msgReply_t queueMsgReply;
        messagePointer_t pQueueMsg;
        int typeOk;
        dspipc_sendStreamMessage_t* pStreamMsg = NULL;
        dspipc_msgContent_t contentType;
        DSPIPCMessageQueueEntry* pMsg;
        uint32_t pMsg_Phys = 0;

        if (pSubDevice == NULL) {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                return (-1);
        }

        pDevice = pSubDevice->pDevice;
        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        pChipset = (dspIpcChipset_t*)pDevice->pChipset;
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

#ifdef VERBOSE_ENABLE
        if (pChipset->verbose > 0) {
                hb_slogf_error("%s: Send msg from <%s>", __FUNCTION__, pSubDevice->pSubDeviceName);
        }
#endif
        pQueueMsg.generic = pChipset->pWriteMsg.generic;
        contentType = DSPIPC_CONT_ANY;
        errCode = dspipcQueueGetMessage(pSubDevice->pSendMsgQueue,
            &contentType,
            &queueMsgId,
            &queueCmdId,
            &queueMsgType,
            &queueMsgReply,
            &pQueueMsg,
            &queueMsgSize,
            &queueNumLost,
            NULL);

        pMsg = (DSPIPCMessageQueueEntry*)pChipset->pWriteDMABuffer;
        pMsg_Phys = 0;

        if (errCode == 0) {
                if (pQueueMsg.generic != NULL) {
                        pMsg->msgType = 0;
                        typeOk = 0;
                        switch (pSubDevice->type) {
                        case SD_CONTROL:
                                pMsg->channelId = DSPIPC_CHANNELTYPE_INTERNAL;
                                pMsg->msgType |= DSPIPC_MSGTYPE_TYPE_COMMAND;
                                pMsg->msgType |= DSPIPC_MSGTYPE_REPLY_NOREPLY;
                                typeOk = 1;
                                break;
                        case SD_COMMAND:
                                pMsg->channelId = DSPIPC_CHANNELTYPE_COMMAND;
                                switch (queueMsgType) {
                                case DSPIPC_MSGTYPE_COMMAND:
                                        pMsg->msgType |= DSPIPC_MSGTYPE_TYPE_COMMAND;
                                        break;
                                case DSPIPC_MSGTYPE_STREAM:
                                        pMsg->msgType |= DSPIPC_MSGTYPE_TYPE_STREAM;
                                        break;
                                default:
                                        hb_slogf_error("%s:%s", __FUNCTION__, MESSAGE_TYPE_INVALID);
                                        break;
                                }
                                switch (queueMsgReply) {
                                case DSPIPC_MSGREPLY_NO:
                                        pMsg->msgType |= DSPIPC_MSGTYPE_REPLY_NOREPLY;
                                        break;
                                case DSPIPC_MSGREPLY_YES:
                                        pMsg->msgType |= DSPIPC_MSGTYPE_REPLY_REQREPLY;
                                        break;
                                default:
                                        hb_slogf_error("%s:%s", __FUNCTION__, MESSAGE_REPLY_INVALID);
                                        break;
                                }
                                typeOk = 1;
                                break;
                        case SD_STREAM:
                                pMsg->channelId = DSPIPC_CHANNELTYPE_STREAM;
                                switch (queueMsgType) {
                                case DSPIPC_MSGTYPE_COMMAND:
                                        pMsg->msgType |= DSPIPC_MSGTYPE_TYPE_COMMAND;
                                        break;
                                case DSPIPC_MSGTYPE_STREAM:
                                        pMsg->msgType |= DSPIPC_MSGTYPE_TYPE_STREAM;
                                        break;
                                default:
                                        hb_slogf_error("%s:%s", __FUNCTION__, MESSAGE_TYPE_INVALID);
                                        break;
                                }
                                switch (queueMsgReply) {
                                case DSPIPC_MSGREPLY_NO:
                                        pMsg->msgType |= DSPIPC_MSGTYPE_REPLY_NOREPLY;
                                        break;
                                case DSPIPC_MSGREPLY_YES:
                                        pMsg->msgType |= DSPIPC_MSGTYPE_REPLY_REQREPLY;
                                        break;
                                default:
                                        hb_slogf_error("%s:%s", __FUNCTION__, MESSAGE_REPLY_INVALID);
                                        break;
                                }
                                typeOk = 1;
                                break;
                        default:
                                hb_slogf_error("%s: Subdevice type invalid", __FUNCTION__);
                                break;
                        }

                        /* check if channel number fit into the mask */
                        if ((pSubDevice->channelNum & (DSPIPC_CHANNELNUMBER_MSK >> DSPIPC_CHANNELNUMBER_SHIFT)) != pSubDevice->channelNum) {
                                hb_slogf_error("%s: channelnumber %d does not fit into protocoll",
                                    __FUNCTION__, pSubDevice->channelNum);
                                typeOk = 0;
                        }

                        if (typeOk) {
                                pMsg->channelId |= (pSubDevice->channelNum & DSPIPC_CHANNELNUMBER_MSK) << DSPIPC_CHANNELNUMBER_SHIFT;
                                pMsg->msgId = queueMsgId;
                                pMsg->msgSize = queueMsgSize;

                                /* check if Message exceeds size */
                                if (((pMsg->msgSize > COMMANDMSG_SIZE) && (queueMsgType == DSPIPC_MSGTYPE_COMMAND)) || (pMsg->msgSize > 272)) {
                                        hb_slogf_error("%s: message exceeds message len (%d>%d) content:%d subdevice:%s",
                                            __FUNCTION__, pMsg->msgSize, COMMANDMSG_SIZE, contentType, pSubDevice->pSubDeviceName);
                                        retValue = -1;
                                } else {

                                        switch (queueMsgType) {
                                        case DSPIPC_MSGTYPE_STREAM:
                                                pMsg->payload.streamMsg.cmdId = queueCmdId;
                                                pMsg->payload.streamMsg.bufferFlags = 0;
                                                switch (contentType) {
                                                case DSPIPC_CONT_SENDSTREAM:
#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 2) {
                                                                hb_slogf_error("%s:DSPIPC_CONT_SENDSTREAM", __FUNCTION__);
                                                        }
#endif
                                                        pStreamMsg = pQueueMsg.sendStream;

                                                        if (pStreamMsg->hdr.numEntries <= DSPIPC_NUM_RANGES) {
                                                                pMsg->payload.streamMsg.cmd = DSPIPC_SM_TRANSFERREQUEST;
                                                                pMsg->payload.streamMsg.numRanges = pStreamMsg->hdr.numEntries;
                                                                if (pStreamMsg->hdr.flags & (uint32_t)STREAMFLAG_DISCONTINUITY) {
                                                                        pMsg->payload.streamMsg.bufferFlags |= SMF_DATA_DISCONTINIOUS;
                                                                }

                                                                for (i = 0; i < pStreamMsg->hdr.numEntries; i++) {
                                                                        pMsg->payload.streamMsg.ranges[i].physicalAddress = pStreamMsg->entry[i].mem.physAddr + pStreamMsg->entry[i].start;
                                                                        pMsg->payload.streamMsg.ranges[i].rangeSize = pStreamMsg->entry[i].size;
                                                                        pMsg->payload.streamMsg.ranges[i].dataSize = pStreamMsg->entry[i].remain;
                                                                        /* it is not allowed to touch the data here, because the */
                                                                        /* cpu has a virtually tagged cache. This means, the data */
                                                                        /* access is only allowed on client side, cache handling  */
                                                                        /* has also be done on client side */
                                                                }
                                                        } else {
                                                                hb_slogf_error("%s: Send stream with to much ranges (%d), max=%d",
                                                                    __FUNCTION__, pStreamMsg->hdr.numEntries, DSPIPC_NUM_RANGES);
                                                        }
                                                        break;
                                                case DSPIPC_CONT_PROVIDEBUFSTREAM:
#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 2) {
                                                                hb_slogf_error("%s:DSPIPC_CONT_PROVIDEBUFSTREAM", __FUNCTION__);
                                                        }
#endif

                                                        pStreamMsg = pQueueMsg.sendStream;
                                                        if (pStreamMsg->hdr.numEntries <= DSPIPC_NUM_RANGES) {
                                                                pMsg->payload.streamMsg.cmd = DSPIPC_SM_PROVIDEBUFFER;
                                                                pMsg->payload.streamMsg.numRanges = pStreamMsg->hdr.numEntries;
                                                                if (pStreamMsg->hdr.flags & (uint32_t)STREAMFLAG_DISCONTINUITY) {
                                                                        pMsg->payload.streamMsg.bufferFlags |= SMF_DATA_DISCONTINIOUS;
                                                                }

                                                                for (i = 0; i < pStreamMsg->hdr.numEntries; i++) {
                                                                        pMsg->payload.streamMsg.ranges[i].physicalAddress = pStreamMsg->entry[i].mem.physAddr + pStreamMsg->entry[i].start;
                                                                        pMsg->payload.streamMsg.ranges[i].rangeSize = pStreamMsg->entry[i].size;
                                                                        pMsg->payload.streamMsg.ranges[i].dataSize = pStreamMsg->entry[i].remain;
                                                                        /* it is not allowed to touch the data here, because the */
                                                                        /* cpu has a virtually tagged cache. This means, the data */
                                                                        /* access is only allowed on client side, cache handling  */
                                                                        /* has also be done on client side */
                                                                }
                                                        } else {
                                                                hb_slogf_error("%s: Provide buffer with to much ranges (%d), max=%d",
                                                                    __FUNCTION__, pStreamMsg->hdr.numEntries, DSPIPC_NUM_RANGES);
                                                        }
                                                        break;
                                                default:
                                                        hb_slogf_error(UNHANDLED_CONTENT_TYPE, __FUNCTION__);
                                                        break;
                                                }
                                                break;
                                        case DSPIPC_MSGTYPE_COMMAND:
#ifdef VERBOSE_ENABLE
                                                if (pChipset->verbose > 2) {
                                                        hb_slogf_error("%s:DSPIPC_MSGTYPE_COMMAND", __FUNCTION__);
                                                }
#endif
                                                memcpy(&(pMsg->payload.message[0]), pQueueMsg.generic, pMsg->msgSize);
                                                break;
                                        default:
                                                hb_slogf_error(GET_INVALID_MESSAGE_TYPE_OUT_OF_QUEUE,
                                                    __FUNCTION__);
                                                break;
                                        }
                                        retValue = dra6xxSendMsg(pChipset, pMsg, pMsg_Phys, DRA6XX_SEND_DO_INT_TO_DSP);
                                }
                        } else {
                                hb_slogf_error("%s: Invalid device type",
                                    __FUNCTION__);
                        }
                } else {
                        hb_slogf_error(GET_NULL_PTR_FOR_MSG,
                            __FUNCTION__);
                }
        } else {
                hb_slogf_error(GET_MESSAGE_FROM_QUEUE_FAILED,
                    __FUNCTION__, errCode);
        }

        return (retValue);
}

/**
 * Drain function to handle a drain sequence to the DSP
 *
 *  @param mode       Mode of drain call
 *  @param pSubDevice Pointer to the sub device structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_send_drain(subDeviceTable_t* pSubDevice, drainSendMode_t mode)
{
        deviceTable_t* pDevice;
        dspIpcChipset_t* pChipset;
        int retValue = -1;
        DSPIPCMessageQueueEntry msg;

        if (pSubDevice == NULL) {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                return (-1);
        }

        pDevice = pSubDevice->pDevice;
        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        pChipset = (dspIpcChipset_t*)pDevice->pChipset;
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

#ifdef VERBOSE_ENABLE
        if (pChipset->verbose > 0) {
                hb_slogf_error("%s: Drain from <%s>", __FUNCTION__, pSubDevice->pSubDeviceName);
        }
#endif

        /* prepare the drain msg to DSP */
        msg.channelId = DSPIPC_CHANNELTYPE_INTERNAL;
        msg.channelId |= (DSPIPC_IPCADMIN >> DSPIPC_CHANNELNUMBER_SHIFT) & DSPIPC_CHANNELNUMBER_MSK;
        msg.msgType = DSPIPC_MSGTYPE_TYPE_COMMAND | DSPIPC_MSGTYPE_REPLY_NOREPLY;

        switch (mode) {
        case SEND_DRAIN_START:
                /* initiate drain sequence on DSP */
                if (dspipc_getNewMessageId((deviceTable_t*)pDevice, &msg.msgId) == 0) {
                        msg.msgId |= DSPIPC_MSGID_DIRECTION_DSPIN;
                        msg.payload.adminMsg.generic.cmdType = DCCMID_DRAINREQ;
                        /* put channel into the content */
                        msg.payload.adminMsg.content.drainInfo.chanId = pSubDevice->channelNum;

                        if (dra6xxSendMsg(pChipset, &msg, 0, DRA6XX_SEND_DO_INT_TO_DSP) == 0) {
                                retValue = 0;
                        } else {
                                hb_slogf_error(SEND_MESSAGE_FAILED, __FUNCTION__);
                        }
                } else {
                        hb_slogf_error(GET_MESSAGE_ID_FAILED, __FUNCTION__);
                }
                break;
        case SEND_DRAIN_END:
                /* send drain complete to DSP */
                if (dspipc_getNewMessageId((deviceTable_t*)pDevice, &msg.msgId) == 0) {
                        msg.msgId |= DSPIPC_MSGID_DIRECTION_DSPIN;
                        msg.payload.adminMsg.generic.cmdType = DCCMID_DRAINCOMPLETE;
                        /* put channel into the content */
                        msg.payload.adminMsg.content.drainInfo.chanId = pSubDevice->channelNum;

                        if (dra6xxSendMsg(pChipset, &msg, 0, DRA6XX_SEND_DO_INT_TO_DSP) == 0) {
                                if (dspipcSignalDrainComplete(pSubDevice) == 0) {
                                        retValue = 0;
                                } else {
                                        hb_slogf_error("%s Signal drain ack failed", __FUNCTION__);
                                }
                        } else {
                                hb_slogf_error(SEND_MESSAGE_FAILED, __FUNCTION__);
                        }
                } else {
                        hb_slogf_error(GET_MESSAGE_ID_FAILED, __FUNCTION__);
                }
                break;
        default:
                hb_slogf_error("%s: Unknown drain action received", __FUNCTION__);
                break;
        }

        return (retValue);
}

static irqreturn_t mbx_irq_handler(int irq, void* dev_id)
{
        deviceTable_t* pDevice;
        dspIpcChipset_t* pChipset;
        int msg;
        uint32_t value;
        uint32_t msgNum;
        int cnt = 0;

        pDevice = (deviceTable_t*)dev_id;

        if (pDevice && pDevice->pChipset != NULL) {
                pChipset = (dspIpcChipset_t*)pDevice->pChipset;
                if (strcmp(pDevice->pDeviceName, "dsp") == 0) {
                        cnt = 0;
                        do {
                                /* Read interrupt status bit */
                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(0), &value); /* Read IRQenable Reg 0 */
                                cnt++;
                        } while (cnt < 10 && ((value & 0x00000001) == 0));

                        if ((value & 0x1) == 0x1) {
                                /* Read Num MSG's */
                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MSGSTATUS(0), &msgNum); /* Msg Status */
                                while (msgNum > 0) {
                                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MESSAGE(0), &value); /* Read Msg Reg 0 */
                                        msgNum--;
                                }

                                if (value != 0xBEEFDEAD) {
                                        hb_slogf_error("%s: Invalid Msg: 0x%x", __FUNCTION__, value);
                                        goto out;
                                }

                                if (pDevice->intCoId != 0 && pDevice->intCoId != -1) {
                                        msg = INTTHREAD_INT;
                                        MsgPut(pDevice->intCoId, &msg, 1);
                                }

                        } else {
                                return IRQ_HANDLED;
                        }
                }
        } else {
                hb_slogf_error("%s: device is not available", __FUNCTION__);
                return IRQ_HANDLED;
        }

out:
        dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(0), 0x00000001);

        return IRQ_HANDLED;
}

static irqreturn_t m3_mbx_irq_handler(int irq, void* dev_id)
{
        deviceTable_t* pDevice;
        dspIpcChipset_t* pChipset;
        int msg;
        uint32_t value;
        uint32_t msgNum;
        int cnt = 0;

        pDevice = (deviceTable_t*)dev_id;

        if (pDevice && pDevice->pChipset != NULL) {
                pChipset = (dspIpcChipset_t*)pDevice->pChipset;
                if (strcmp(pDevice->pDeviceName, "m3") == 0) {
                        cnt = 0;
                        do {
                                /* Read interrupt status bit */
                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(0), &value); /* Read IRQenable Reg 0 */
                                cnt++;
                        } while (cnt < 10 && ((value & 0x00000100) == 0));

                        if ((value & 0x100) == 0x100) {
                                /* Read Num MSG's */
                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MSGSTATUS(4), &msgNum); /* Msg Status */
                                while (msgNum > 0) {
                                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MESSAGE(4), &value); /* Read Msg Reg 0 */
                                        msgNum--;
                                }
                                if (value != 0xBEEFDEAD) {
                                        hb_slogf_error("%s: Invalid Msg: 0x%x", __FUNCTION__, value);
                                        goto out;
                                }

                                if (pDevice->intCoId != 0 && pDevice->intCoId != -1) {
                                        msg = INTTHREAD_INT;
                                        MsgPut(pDevice->intCoId, &msg, 1);
                                }
                        } else {
                                return IRQ_HANDLED;
                        }
                }
        } else {
                hb_slogf_error("%s: device is not available", __FUNCTION__);
                return IRQ_HANDLED;
        }

out:
        dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(0), 0x00000100);

        return IRQ_HANDLED;
}

#define INIT_INT_THREAD_HERE
#define __USE_TIMERTIMEOUT

/**
 * Interrupt thread to handle the interrupts from the DSP
 *  @callgraph
 *  @param pDevice Pointer to the device table structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
int dspipc_dra6xx_intrThread(deviceTable_t* pDevice)
{
        dspIpcChipset_t* pChipset;
        int retValue = 0;
        int i;
        int readnum;
        uint32_t wakeUpReq;
        uint32_t dspTxQueueAdr;
        int32_t dspTxQueueWriteIndex; /* must be signed to ensure correct difference calculation */
        int32_t dspTxQueueReadIndex;  /* must be signed to ensure correct difference calculation */
        int32_t numElements;          /* must be int to ensure correct difference calculation */
        int errCode;
        int intrCode;
        uint32_t dspAssertLine;
        uint32_t dspAssertFile;
        int signalNewScan;
        DSPIPCMessageQueueEntry* pMsg = NULL;
#ifdef USE_TIMERTIMEOUT
        struct sigevent timeoutEvent;
        uint64_t timeout;
#endif
#ifdef IN_OUT_TRACE
        char pass[100];
#endif

        if (pDevice != NULL) {
                pChipset = (dspIpcChipset_t*)pDevice->pChipset;
                if (pChipset != NULL) {
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 0) {
                                hb_slogf_error("%s: InterruptThread running", __FUNCTION__);
                        }
#endif

                        hb_slogf_error("%s:start to register int,num:%d", __FUNCTION__, pDevice->intNumber);

#ifdef INIT_INT_THREAD_HERE
                        /* create channel/connection */
                        retValue = MsgQueueInit(&pDevice->intChId, sizeof(int), 8192);
                        if (retValue != -1 && pDevice->intChId != -1) {
                                pDevice->intCoId = pDevice->intChId;
                                if (pDevice->intCoId != -1) {
                                        if (pDevice->intNumber != -1) {

                                                if (strcmp(pDevice->pDeviceName, "dsp") == 0) {
                                                        hb_slogf_error("%s:start to register DSP ## int,num:%d", __FUNCTION__, pDevice->intNumber);

                                                        retValue = request_irq(pDevice->intNumber,
                                                            mbx_irq_handler,
                                                            /*IRQF_DISABLED |*/ IRQF_SHARED,
                                                            "dspipc_irq",
                                                            (void*)pDevice);

                                                } else if (strcmp(pDevice->pDeviceName, "m3") == 0) {
                                                        hb_slogf_error("%s:start to register m3 $$ int,num:%d", __FUNCTION__, pDevice->intNumber);

                                                        retValue = request_irq(pDevice->intNumber,
                                                            m3_mbx_irq_handler,
                                                            /*IRQF_DISABLED |*/ IRQF_SHARED,
                                                            "m3ipc_irq",
                                                            (void*)pDevice);
                                                }

                                                if (!retValue) {
                                                        pDevice->intEvent = pDevice->intNumber;
                                                } else {
                                                        hb_slogf_error("%s: Can't attach Interrupt handler for %s, intNum %d ",
                                                            __FUNCTION__, pDevice->pDeviceName, pDevice->intNumber);
                                                }
                                        }
                                } else {
                                        hb_slogf_error("%s: Connection attach failed", __FUNCTION__);
                                }
                        } else {
                                hb_slogf_error("%s: Channel create failed", __FUNCTION__);
                        }
#endif /* INIT_INT_THREAD_HERE */

                        hb_slogf_error("%s: Thread is ready, current state is:%d, loop condition:%d", __FUNCTION__, pChipset->configState,pDevice->doLoop);
                        while ((pDevice->doLoop)) {
                                signalNewScan = 0;

                                intrCode = INTHREAD_INVALID;
                                readnum = 0;
                                if ((readnum = MsgReceive(pDevice->intChId, &intrCode, 1, (unsigned int)-1)) > 0) {
                                        switch (intrCode) {
                                        case INTTHREAD_SEND: /* pulse from threadpool due to new message entry */
                                                DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s: intr send called\n", __FUNCTION__);
#ifdef VERBOSE_ENABLE
                                                if (pChipset->verbose > 2) {
                                                        hb_slogf_error("%s: Send pulse received", __FUNCTION__);
                                                }
#endif
                                                signalNewScan = 1;
                                                break;

                                        case INTTHREAD_INT: /* Real Interrupt */
                                                DEBUG_INFO_LEVEL(LEVEL_VVVV, "%s: intr interrupt called\n", __FUNCTION__);

                                                if (pChipset->configState != DRA6XX_NOT_INITIALIZED) {
                                                        /* check asserting marker for DSP exception */
                                                        if (dra6xxAccessMem32(pChipset,
                                                                DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspAdminStructAdr) + ADMIN_OFFSET(dspAssertLine),
                                                                1, MEM_READ, &dspAssertLine)
                                                            != 0) {
                                                                hb_slogf_error("%s: Accessing assert line failed", __FUNCTION__);
                                                        }

                                                        if (dspAssertLine != 0) {
                                                                if (dra6xxAccessMem32(pChipset,
                                                                        DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspAdminStructAdr) + ADMIN_OFFSET(dspAssertFile),
                                                                        1, MEM_READ, &dspAssertFile)
                                                                    != 0) {
                                                                        hb_slogf_error("%s: Accessing assert line failed", __FUNCTION__);
                                                                }

                                                                i = 0;
                                                                while (DSPIPCdra6xxFileNameArray[i].name != NULL) {
                                                                        if (DSPIPCdra6xxFileNameArray[i].num == dspAssertFile) {
                                                                                hb_slogf_error("%s: DSP assertion found in file %s at line %d",
                                                                                    __FUNCTION__, DSPIPCdra6xxFileNameArray[i].name, dspAssertLine);
                                                                                break;
                                                                        }
                                                                        i++;
                                                                }
                                                                if (DSPIPCdra6xxFileNameArray[i].name == NULL) {
                                                                        hb_slogf_error("%s: DSP assertion found in file %d at line %d",
                                                                            __FUNCTION__, dspAssertFile, dspAssertLine);
                                                                }
                                                        }

                                                        /* check if message is available inside the queue */
                                                        dspTxQueueAdr = dra6xx_get_queue_index(pChipset,
                                                            ADMIN_OFFSET(dspTxBufferAddr),
                                                            &dspTxQueueReadIndex,
                                                            &dspTxQueueWriteIndex);

                                                        if ((void*)dspTxQueueAdr == NULL) {
                                                                hb_slogf_error("%s: dspTxQueueAdr is null, DSP or Mcore was not booting up.", __FUNCTION__);
                                                                return (-1);
                                                        }

#ifdef VERBOSE_ENABLE
                                                        if (pChipset->verbose > 2) {
                                                                hb_slogf_error("%s: Current DSP-TX index: read=%d, write=%d, numElements:%d,queueAddr:0x%x", __FUNCTION__, dspTxQueueReadIndex, dspTxQueueWriteIndex, dspTxQueueWriteIndex - dspTxQueueReadIndex, dspTxQueueAdr);
                                                        }
#endif
                                                        if (dra6xxAccessMem32(pChipset,
                                                                dspTxQueueAdr + offsetof(struct _DSPIPCMessageQueue, wakeupRequest),
                                                                1, MEM_READ, &wakeUpReq) != 0) {
                                                                hb_slogf_error("%s: Access failed 2", __FUNCTION__);
                                                        }
                                                        numElements = dspTxQueueWriteIndex - dspTxQueueReadIndex;

                                                        /* read message */
                                                        while (numElements > 0) {
                                                                pMsg = (DSPIPCMessageQueueEntry*)pChipset->pReadDMABuffer;

                                                                {
                                                                        if (dra6xxAccessMem32(pChipset,
                                                                                dspTxQueueAdr + offsetof(struct _DSPIPCMessageQueue, msg) + (((uint32_t)dspTxQueueReadIndex) % COMMANDQUEUE_LENGTH) * sizeof(DSPIPCMessageQueueEntry),
                                                                                sizeof(DSPIPCMessageQueueEntry) >> 2, MEM_READ, (uint32_t*)pMsg)
                                                                            != 0) {
                                                                                hb_slogf_error("%s: Access failed", __FUNCTION__);
                                                                        }
                                                                }

#ifdef VERBOSE_ENABLE
                                                                if (pChipset->verbose > 3) {
                                                                        dra6xxDumpMessage("received", pMsg);
                                                                }
#endif
                                                                /* process message */
                                                                if (ProcessNewMessage(pChipset, pMsg, &signalNewScan) != 0) {
                                                                        /*hb_slogf_error("%s: Process new message failed <%d elem>", __FUNCTION__, numElements);*/
                                                                }

                                                                /* calculate new read index */
                                                                dspTxQueueReadIndex++;
#ifdef VERBOSE_ENABLE
                                                                if (pChipset->verbose > 1) {
                                                                        hb_slogf_error("%s: Set DSP-TX index read to %d", __FUNCTION__, dspTxQueueReadIndex);
                                                                }
#endif
                                                                /* set queue read index */
                                                                (void)dra6xx_set_queue_index(pChipset,
                                                                    ADMIN_OFFSET(dspTxBufferAddr),
                                                                    &dspTxQueueReadIndex, NULL);

                                                                switch (wakeUpReq) {
                                                                case 0:
                                                                        /* do nothing */
                                                                        break;
                                                                case 1:
                                                                        /* signalize interrupt to DSP for queue not longer full event */
                                                                        dra6xxSetInterrupt(pChipset);
                                                                        wakeUpReq--;
                                                                        break;
                                                                default:
                                                                        wakeUpReq--;
                                                                        break;
                                                                }
                                                                numElements--;
                                                        } /*while numElements > 0 */

                                                } else {
                                                        notReadyTimes++;
                                                        hb_slogf_error("%s:driver state is not ready ", __FUNCTION__);
                                                        /* firmware (bootloader) not started yet */
                                                        errCode = pthread_mutex_lock(&(pChipset->dspIntCondMutex));
                                                        if (errCode != EOK) {
                                                                hb_slogf_error("%s: %s ", __FUNCTION__, MUTEX_LOCK_FAILED);
                                                        }

                                                        pChipset->dspIntMarker++;
                                                        retValue = pthread_cond_signal(&(pChipset->dspIntCondVar));
                                                        if (errCode != EOK) {
                                                                hb_slogf_error("%s: cond signal failed ", __FUNCTION__);
                                                        }
                                                        errCode = pthread_mutex_unlock(&(pChipset->dspIntCondMutex));
                                                        if (errCode != EOK) {
                                                                hb_slogf_error("%s: %s ", __FUNCTION__, MUTEX_UNLOCK_FAILED);
                                                        }
                                                }

                                                if ((errCode = dra6xxInterruptUnmask(pDevice)) != EOK) {
                                                        hb_slogf_error("%s: Interrupt unmask failed", __FUNCTION__);
                                                }
                                                break;
                                        case INTTHREAD_TERMINATE: /* terminate int thread */
                                                hb_slogf_error("%s: Terminate pulse code received", __FUNCTION__);
                                                i = 0;
                                                while (i < 20) {
                                                        if (pthread_should_stop()) {
                                                                break;
                                                        }
                                                        hb_slogf_error("%s: wait for kthread_stop", __FUNCTION__);
                                                        pthread_sleep(50);
                                                        i++;
                                                }
                                                pDevice->doLoop = 0;
                                                break;
                                        default:
                                                hb_slogf_error("%s: Unknown pulse code received", __FUNCTION__);
                                                retValue = -1;
                                                pDevice->doLoop = 0;
                                                break;
                                        }
                                } else {
                                        if ((-readnum) == ETIME) {
                                                hb_slogf_error("%s: Hearbeatperiod expired", __FUNCTION__);
                                        } else {
                                                hb_slogf_error("%s: MsgReceive failed, error code %d ", __FUNCTION__, (-readnum));
                                                retValue = -1;
                                        }
                                }
                                if (signalNewScan != 0) {
                                        /* call of SendThread-function */
                                        if (dspipcSendThread((void*)pDevice) != NULL) {
                                                hb_slogf_error("%s: transmit ready failed", __FUNCTION__);
                                        }
                                }
                        } /* while loop */
                        hb_slogf_error("%s: INT thread existed", __FUNCTION__);
                } else {
                        hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                retValue = -1;
        }
        return (retValue);
}

/******************************************************************************* */
/* */
/* INTERNAL FUNCTIONS */
/* */
/******************************************************************************* */

/**
 *  This function writes a 32bit value to the dsp. In normal mode this is done
 *  by a direct register access. In simulation mode a devctl to the simulator
 *  is executed.DSPIPCTypes.h
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param RegModSel Register Selector 0:INT-Reg   1:Module Conf Reg
 *  @param addr Address for the write access
 *  @param val Value to be written to the specified address
 *  @return void
 */
static void dra6xxRegWrite32(const dspIpcChipset_t* pChipset, uint32_t RegModSel, uint32_t addr, uint32_t val)
{
        if (pChipset != NULL) {
                if (RegModSel == MOD_DEVCONF_REG) {
                        /* Module Conf Register */
                        iowrite32(val, (void __iomem*)((char __iomem*)pChipset->regModConf + addr));
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 3) {
                                hb_slogf_error("%s: Adr:0x%08X Val:0x%08X", __FUNCTION__, (uint32_t)((char __force*)pChipset->regModConf + addr), val);
                        }
#endif
                } else /* MOD_STATCTRL_REG */
                {
                        /* Ctrl + Status Reg */
                        iowrite32(val, (void __iomem*)((char __iomem*)pChipset->regModCtrl + addr));
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 3) {
                                hb_slogf_error("%s: Adr:0x%08X Val:0x%08X", __FUNCTION__, (uint32_t)((char __force*)pChipset->regModCtrl + addr), val);
                        }
#endif
                }
        } else {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
        }
        return;
}

/**
 *  This function reads a 32bit value from the dsp. In normal mode this is done
 *  by a direct register access. In simulation mode a devctl to the simulator
 *  is executed.
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param RegModSel Register Selector 0:INT-Reg   1:Module Conf Reg
 *  @param addr Address for the write access
 *  @param pVal Pointer to variable to store the read value from the specified address,
 *              NULL if the value is not needed
 *  @return void
 */
static void dra6xxRegRead32(const dspIpcChipset_t* pChipset, uint32_t RegModSel, uint32_t addr, volatile uint32_t* pVal)
{
        if (pChipset != NULL) {
                if (pVal != NULL) {
                        if (RegModSel == MOD_DEVCONF_REG) {
                                /* Module Conf Register */
                                *pVal = ioread32((void __iomem*)((char __iomem*)pChipset->regModConf + addr));
#ifdef VERBOSE_ENABLE
                                if (pChipset->verbose > 3) {
                                        hb_slogf_error("%s: Adr:0x%08X Val:0x%08X", __FUNCTION__, (uint32_t)((char __force*)pChipset->regModConf + addr), *pVal);
                                }
#endif
                        } else /* MOD_STATCTRL_REG */
                        {
                                /* Ctrl + Status Reg */
                                *pVal = ioread32((void __iomem*)((char __iomem*)pChipset->regModCtrl + addr));
#ifdef VERBOSE_ENABLE
                                if (pChipset->verbose > 3) {
                                        hb_slogf_error("%s: Adr:0x%08X Val:0x%08X", __FUNCTION__, (uint32_t)((char __force*)pChipset->regModCtrl + addr), *pVal);
                                }
#endif
                        }
                } else {
                        hb_slogf_error("%s: invalid pointer to store value", __FUNCTION__);
                }
        } else {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
        }
        return;
}

#define USE_TIMEOUT
/**
 *  This function transfers a 32bit aligned data array from the PCI address (DSP)
 *  to a local address. 
 *  
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param physSrcAddr Physical address of the buffer within the DSP
 *  @param pVirtDestAddr Virtual address of the buffer within local address 
 *  @param physDestAddr Physical address of the buffer within local address 
 *  @param numBytes Number of bytes to be transfered (must be a multiple of 4)
 *  @return void
 */
static int ReadDma(const dspIpcChipset_t* pChipset,
    uint32_t physSrcAddr,
    const uint32_t* pVirtDestAddr, uint32_t physDestAddr,
    uint32_t numBytes)
{
        int loopCnt;
        int retVal = -1;

        if (pChipset != NULL) {
                if (pVirtDestAddr != NULL) {
                        /* if non multiple of 4 requested -> round to next 4 bytes */
                        numBytes = (numBytes + 3) & ~0x00000003U;

                        /* use direct copy instead of DMA */
                        loopCnt = numBytes >> 2;
                        while (loopCnt != 0) {
                                *(volatile uint32_t*)pVirtDestAddr++ = *(volatile uint32_t*)physSrcAddr++;
                                loopCnt--;
                        }
                } else {
                        hb_slogf_error("%s: No valid address", __FUNCTION__);
                }
        } else {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
        }

        return retVal;
}

#define __DMA_WRITE_NEU
/**
 *  This function transfers a 32bit aligned data array to the PCI address (DSP)
 *  from a local address. 
 *  
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param physSrcAddr Physical address of the buffer within local address
 *  @param pVirtSrcAddr Pointer to buffer within local address
 *  @param physDestAddr Physical address of the buffer within the DSP
 *  @param numBytes Number of bytes to be transfered (must be a multiple of 4)
 *  @return void
 */
static void WriteDma(const dspIpcChipset_t* pChipset,
    const uint32_t* pVirtSrcAddr, uint32_t physSrcAddr,
    uint32_t physDestAddr,
    uint32_t numBytes)
{
        int loopCnt;

        if (pChipset != NULL) {
                if (pVirtSrcAddr != NULL) {
                        /* if non multiple of 4 requested -> round to next 4 bytes */
                        numBytes = (numBytes + 3) & ~0x00000003U;

                        /* use direct copy instead of DMA */
                        loopCnt = numBytes >> 2;
                        while (loopCnt != 0) {
                                *((volatile unsigned long*)((char __force*)(pChipset->memAccAddr) + (physDestAddr++))) = *(volatile uint32_t*)pVirtSrcAddr++;
                                loopCnt--;
                        }

                } else {
                        hb_slogf_error("%s: No value source", __FUNCTION__);
                }
        } else {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
        }
        return;
}

/**
 *  This function access the memory inside the DSP with 32 bit accesses
 *
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param dps_addr     Address inside the DSP memory map
 *  @param numLongWords number of consecutive accesses
 *  @param dir Access type (MEM_READ | MEM_WRITE )
 *  @param pVal Pointer to source or destination value
 *  @param pVal_phys Pointer to source or destination physical value
 *  @return int error code 0=OK, -1=failure
 */
static int dra6xxAccessMem32_DMA(dspIpcChipset_t* pChipset,
    uint32_t dps_addr,
    uint32_t numLongWords,
    memAccessDir_t dir,
    uint32_t* pVal, uint32_t pVal_phys)
{
        uint32_t page;
        uint32_t page_end;
        uint32_t addr_end;
        uint32_t value;
        int errCode;

#ifdef IN_OUT_TRACE
        char pass[100];
#endif
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        if (pVal == NULL) {
                hb_slogf_error("%s: No value", __FUNCTION__);
                return (-1);
        }

        errCode = pthread_mutex_lock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_LOCK_FAILED, __FUNCTION__);
        }

#ifdef VERBOSE_ENABLE
        if (pChipset->verbose > 0) {
                hb_slogf_error("%s: DSP_ADR:0x%08X  pAdr:0x%08X  pPhysAdr:0x%08X  D:0x%08X  Num:%d",
                    __FUNCTION__, dps_addr, (uint32_t)pVal, pVal_phys, dir, numLongWords);
        }
#endif

        if (setMemSelAdr(pChipset, dps_addr) != 0) {
                hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
        }

        addr_end = dps_addr + ((numLongWords << 2) - 1); /* Claculate end address of Transfer */
        page_end = (addr_end & MEMORY_PAGE_MASK) >> 22;  /* Cal. page for end of transfer */
        page = (dps_addr & MEMORY_PAGE_MASK) >> 22;      /* Cal. Page for start address */

        if (page != pChipset->dspp) {
                pChipset->dspp = page;
        }

        if (page == page_end) /* Wenn Start und Ende in einer Page sind */
        {
                /* Check the alignment of the address (not needed in *_oc functions) */
                if ((dps_addr & 0x03U) != 0) {
                        hb_slogf_error("%s: Address 0x%08X is not valid", __FUNCTION__, dps_addr);
                        return -1;
                }

                if (dir == MEM_READ) {
                        if (ReadDma(pChipset,
                                (dps_addr),
                                pVal, pVal_phys,
                                (numLongWords << 2))
                            != 0) {
                                hb_slogf_error("%s: ReadDma() failed", __FUNCTION__);
                        }
                } else {
                        WriteDma(pChipset,
                            pVal, pVal_phys,
                            (dps_addr),
                            (numLongWords << 2));
                }
        } else /* do transfer on Overlapping Pages */
        {
                if (page != page_end) {
                        hb_slogf_error("%s: 2 Page Access 0x%08X S:%d", __FUNCTION__, dps_addr, (numLongWords << 2));
                }

                if (dir == MEM_READ) {
                        while (numLongWords != 0) {
                                if (dra6xxMemRead(pChipset, dps_addr, &value) != 0) {
                                        hb_slogf_error(DRA6XXREAD_FAILED, __FUNCTION__);
                                }
                                *pVal++ = value;
                                dps_addr += 4;

                                if (setMemSelAdr(pChipset, dps_addr) != 0) {
                                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                                }
                                numLongWords--;
                        }
                } else /* MEM_WRITE */
                {

                        while (numLongWords != 0) {
                                value = *pVal++;
                                if (dra6xxMemWrite(pChipset, dps_addr, value) != 0) {
                                        hb_slogf_error(DRA6XXMEMWRITE_FAILED_2, __FUNCTION__);
                                }
                                dps_addr += 4;

                                if (setMemSelAdr(pChipset, dps_addr) != 0) {
                                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                                }
                                numLongWords--;
                        }
                }
        }

        errCode = pthread_mutex_unlock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_UNLOCK_FAILED, __FUNCTION__);
        }
        return (0);
}

#define __CHECK_APPL_DATA
#ifdef CHECK_APPL_DATA
/**
 *  This function compares a given number of 32bit values of two virtual memory elements
 *
 *  @param pReadBuf_1 Pointer to the first memory lement
 *  @param pReadBuf_2 Pointer to the second memory lement
 *  @param rdLen number of elements to compare
 *  @param section Addr that matches to the first memory address (for report)
 *  @return void
 */
static void compareValues(uint32_t* pReadBuf_1, uint32_t* pReadBuf_2, int rdLen, uint32_t sectionAddr)
{
        int i = 0;
        uint32_t sectionAddr_local = sectionAddr;

        if ((pReadBuf_1 != NULL) && (pReadBuf_2 != NULL)) {
                for (i = 0; i < rdLen; i = i + 4) {
                        if (*pReadBuf_1 != *pReadBuf_2) {
                                hb_slogf_error("ReadBack Error: DW:0x%x DR:0x%x   SectionAddr:0x%x", *pReadBuf_1, *pReadBuf_2, sectionAddr_local);
                        }
                        pReadBuf_1++;
                        pReadBuf_2++;
                        sectionAddr_local = sectionAddr_local + 4;
                }
        } else {
                hb_slogf_error("%s: invalid parameter", __FUNCTION__);
        }
}
#endif

/**
 * This function transmit an opened boot-table format file to the DSP using
 * the direct PCI interface. It can be used in host-DMA or PIO mode.
 *
 * @param pChipset Pointer to the dra44xi chipset layer administration structure
 * @param bootFd Filedescriptor to the boot table file
 * @return int 0 if everything was ok, -1 if an error has occurred
 */
static int dra6xxSendBootTable(dspIpcChipset_t* pChipset, int bootFd)
{
        int retValue = 0;
        deviceTable_t* pDevice;
        uint32_t entryPoint = 0;
        volatile uint32_t mem_val;
        uint32_t count = 0;

#ifdef CHECK_APPL_DATA
        uint32_t* pReadBuffer_rb;
        uint32_t ReadBuffer_phys_rb;
#endif

        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        pDevice = pChipset->pDevice;
        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        DEBUG_INFO_LEVEL(LEVEL_VVVV, "[%s]: we don't support file format other than .out! fileName %s\n", __FUNCTION__, pChipset->pFileName);
        if (strstr(pChipset->pFileName, ".out") != NULL) /* Coff File handling */
        {
                uint32_t file_address = 0;
                uint32_t ret_val = 0;
                off_t end_off = 0;
                int lokal_verbose = 4;

                pChipset_Coff = pChipset;
#ifdef VERBOSE_ENABLE
                if (pChipset->verbose) {
                        lokal_verbose = pChipset->verbose;
                }
#endif

                end_off = lseek(bootFd, 0, SEEK_END);
                if (end_off == 0) {
                        hb_slogf_error("%s: lseek() SEEK_END failed", __FUNCTION__);
                        return -1;
                } else {
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose) {
                                hb_slogf_error("%s: File Size %ld", __FUNCTION__, end_off);
                        }
#endif

                        file_address = (uint32_t)VMALLOC_TRC(end_off);
                        if (file_address != 0) {
                                if (lseek(bootFd, 0, SEEK_SET) != 0) {
                                        hb_slogf_error("%s: lseek() SEEK_SET 0 failed", __FUNCTION__);
                                }

                                /* Load File  */
                                if ((ret_val = read(bootFd, (void*)file_address, end_off)) != end_off) {
                                        hb_slogf_error("%s: error read() File (%ld/%d)", __FUNCTION__, end_off, ret_val);
                                        return -1;
                                } else {
                                        if (pChipset->pFileType != NULL) {
                                                if (strcmp(pChipset->pFileType, "coff") == 0) {

                                                        entryPoint = load_coff_file(file_address, lokal_verbose);

                                                } else if (strcmp(pChipset->pFileType, "elf") == 0) {

                                                        entryPoint = load_elf_file(file_address, lokal_verbose);

                                                } else {

                                                        hb_slogf_error("%s:unkown filetype(not coff or elf)!!!", __FUNCTION__);
                                                }
                                        } else {

                                                hb_slogf_error("%s:File type is not set!!!", __FUNCTION__);
                                        }

                                        if (entryPoint == 0) {
                                                hb_slogf_error("%s: error loading file failed (%d)", __FUNCTION__, entryPoint);
                                        } else {
                                                if (entryPoint & 0x000003FF) {
                                                        hb_slogf_error("%s: Invalid entry-Point: 0x%X  (NOT aligned to 1024Byte!!!!)", __FUNCTION__, entryPoint);
                                                        retValue = -1;
                                                        return -1;
                                                } else {
                                                        hb_slogf_error("%s: loading file done: DSP-StartAddr: 0x%X", __FUNCTION__, entryPoint);
                                                }
                                        }

                                        /* Set Startaddress of DSP (Internal L2-RAM of DSP) */
                                        dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, DCM_DSPBOOTADDR, entryPoint);
                                        dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, DCM_DSPBOOTADDR, entryPoint);
                                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, DCM_DSPBOOTADDR, &mem_val);
#ifdef VERBOSE_ENABLE
                                        if (pChipset->verbose) {
                                                hb_slogf_error("%s: read: DSP-BOOTADDR: 0x%x", __FUNCTION__, mem_val);
                                        }
#endif

                                        mem_val = 0x00000003;                                                    /* ; */
                                        dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTST, mem_val);     /* Read RM_DSP_RSTST Register */
                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, PM_DSP_PWRSTCTRL, &mem_val); /* Read PM_DSP_PWRSTCTRL Register */
                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, PM_DSP_PWRSTST, &mem_val);   /* Read PM_DSP_PWRSTST Register */
                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTCTRL, &mem_val);   /* Read RM_DSP_RSTCTRL Register                   */
                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTST, &mem_val);     /* Read RM_DSP_RSTST Register */

                                        mem_val = 0x00000000; /* (mem_val) | MDCTL_LRST; */
                                        dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTCTRL, (mem_val));
                                        mdelay(3);

                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, PM_DSP_PWRSTCTRL, &mem_val); /* Read PM_DSP_PWRSTCTRL Register */
                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, PM_DSP_PWRSTST, &mem_val);   /* Read PM_DSP_PWRSTST Register */
                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTCTRL, &mem_val);   /* Read RM_DSP_RSTCTRL Register                   */
                                        dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTST, &mem_val);     /* Read RM_DSP_RSTST Register */

                                        /* Set Local Reset on DSP */
                                        mem_val = 0x00000000; /* (mem_val) | MDCTL_LRST; */
                                        dra6xxRegWrite32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTCTRL, (mem_val));
                                        hb_slogf_error("%s: 1) After DSP out of reset 0x%x", __FUNCTION__, mem_val);
                                        do {
                                                mdelay(1);
                                                count++;
                                                dra6xxRegRead32(pChipset, MOD_STATCTRL_REG, RM_DSP_RSTST, &mem_val); /* Read RM_DSP_RSTST Register */
                                        } while (((mem_val & 0x3) != 0x3) && (count < 100));

                                } /* end ok of file load */

                                VFREE_TRC((void*)file_address);
                                retValue = 0;
                        } else {
                                hb_slogf_error("%s: error alloc memory for File", __FUNCTION__);
                                return -1;
                        }
                }
        } else /* .bin File */
        {
                /* don't support .bin file read */
                DEBUG_ERROR("[%s]: we don't support file format other than .out! fileName %s\n", __FUNCTION__, pChipset->pFileName);

        } /* End .bin File Load */

        return (retValue);
}

static int dra6xxSetConfigState(dspIpcChipset_t* pChipset, dra6xxConfigState_t newState)
{
        int retValue = EOK;
        int errCode;

        if (pChipset != NULL) {
                errCode = pthread_mutex_lock(&(pChipset->configStateCondMutex));
                if (errCode == EOK) {
                        pChipset->configState = newState;

                        errCode = pthread_cond_signal(&(pChipset->configStateCondVar));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Config state cond signal failed",
                                    __FUNCTION__);
                                retValue = errCode;
                        }
                        errCode = pthread_mutex_unlock(&(pChipset->configStateCondMutex));
                        if (errCode != EOK) {
                                hb_slogf_error("%s: Config state %s ", __FUNCTION__, MUTEX_UNLOCK_FAILED);
                                retValue = errCode;
                        }
                } else {
                        hb_slogf_error("%s: Config state %s ",
                            __FUNCTION__, MUTEX_LOCK_FAILED);
                        retValue = errCode;
                }
        } else {
                hb_slogf_error("%s: pChipset is not valid (==NULL)", __FUNCTION__);
                retValue = -1;
        }

        return (retValue);
}

/**
 *  This function is doing the interrupt unmask operation. In normal mode (real
 *  chip) this is the standard InterruptUnmask call to reactivate the interrupt
 *  after the handling. In the simulator mode an unmask devctl is delivered to
 *  the simulator to allow sending the next pending interrupt.
 *
 *  @param pDevice Pointer to the device structure
 *  @return int EOK if everything was ok, else an error code from errno.h is
 *          returned
 */
static int dra6xxInterruptUnmask(const deviceTable_t* pDevice)
{
        int retValue = EOK;
        dspIpcChipset_t* pChipset;

        if (pDevice != NULL) {
                pChipset = (dspIpcChipset_t*)pDevice->pChipset;
                if (pChipset != NULL) {
                        /* real mode */
                        if (pDevice->intNumber != -1) { /* yanghuihua delete
               if( InterruptUnmask( pDevice->intNumber, pDevice->intEvent) == -1 )
               {
                  retValue = errno;
               }*/
                        } else {
                                hb_slogf_error("%s: invalid interupt number", __FUNCTION__);
                        }
                } else {
                        hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                }
        } else {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
        }
        return (retValue);
}

/**
 * Function to write to the 4MB memory window area of the dra44xi.
 *
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param addr Address inside the DSP memory map (has to be a multiple of 4,
 *              upper bis above 4MByte will be cut)
 *  @param value Value to be written to the DSP
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
static int dra6xxMemWrite(const dspIpcChipset_t* pChipset, const uint32_t addr, const uint32_t value)
{
        uint32_t addr_masked = (addr - pChipset->memAddr_phys_Const) % pChipset->MemSize;

        if ((pChipset != NULL) && (pChipset->memAccAddr != NULL)) {
                if ((addr & 0x03U) == 0) {
                        *((volatile unsigned long*)((char __force*)(pChipset->memAccAddr) + addr_masked)) = value;
                } else {
                        hb_slogf_error("%s: Address is not valid: memAddr:%08x addr:%08x", __FUNCTION__, (uint32_t __force)pChipset->memAddr, addr);
                        return -1;
                }
        } else {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return -1;
        }
        return 0;
}

/**
 * Function to read from the 4MB memory window area of the dra44xi.
 *
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param addr Address inside the DSP memory map (has to be a multiple of 4, upper bis above 4MByte will be cut)
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param dest Pointer to write the read value to
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
static int dra6xxMemRead(const dspIpcChipset_t* pChipset, const uint32_t addr, volatile uint32_t* dest)
{
        uint32_t value = 0;
        uint32_t addr_masked = (addr - pChipset->memAddr_phys_Const) % pChipset->MemSize;

        if ((pChipset != NULL) && (pChipset->memAccAddr != NULL)) {
                if ((addr & 0x03U) == 0) {
                        if (dest != NULL) {
                                value = *((volatile unsigned long*)((char __force*)(pChipset->memAccAddr) + addr_masked));
                                *dest = value;
                        } else {
                                hb_slogf_error("%s: No destination address", __FUNCTION__);
                        }
                } else {
                        hb_slogf_error("%s: Address is not valid: memAddr:%08x addr:%08x", __FUNCTION__, (uint32_t __force)pChipset->memAddr, addr);
                        return -1;
                }
        } else {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return -1;
        }

        return 0;
}

/**
 * Function to read from the 4MB memory window area of the dra44xi.
 *
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param addr Address inside the DSP memory map (has to be a multiple of 4,
 *              upper bis above 4MByte will be cut) to read from
 *  @param dest destination to save read results
 *  @param numLongWords Number of LongeWords to READ
 *  @return void
 */
static void dra6xxMemRead_oc(const dspIpcChipset_t* pChipset,
    uint32_t addr, /* addr */
    uint32_t* dest,
    uint32_t numLongWords)
{
        uint32_t value;
        uint32_t addr_masked = (addr - pChipset->memAddr_phys_Const) % pChipset->MemSize;

        while (numLongWords != 0) {
                value = *((volatile unsigned long*)((char __force*)(pChipset->memAccAddr) + (addr_masked)));
                *dest = ENDIAN_LE32(value);
                dest++;
                addr_masked += 4;
                numLongWords--;
        }
}

/**
 * Function to write to the 4MB memory window area of the dra44xi.
 *
 *  @param pChipset     Pointer to the dra44xi chipset layer administration structure
 *  @param addr         Address inside the DSP memory map (has to be a multiple of 4,
 *                      upper bis above 4MByte will be cut) to write to
 *  @param value        Address to read the values to be written to the DSP
 *  @param numLongWords Number of LongeWords to WRITE
 *  @return void
 */
static void dra6xxMemWrite_oc(const dspIpcChipset_t* pChipset,
    uint32_t addr, /* addr */
    const uint32_t* value,
    uint32_t numLongWords)
{
        uint32_t addr_masked = (addr - pChipset->memAddr_phys_Const) % pChipset->MemSize;
        while (numLongWords != 0) {
                *((volatile unsigned long*)((char __force*)((uint32_t __force)pChipset->memAccAddr + addr_masked))) = ENDIAN_LE32(*value);
                value++;
                addr_masked += 4;
                numLongWords--;
        }
}

#define DYN_PAGE_MASK

/**
 * Function that sets the memory Area that is used in the following access.
 *
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param addr Memory Address (physical)
 *
 *  @return int error code 0=OK, -1=failure
 */
static int setMemSelAdr(dspIpcChipset_t* pChipset, uint32_t addr)
{

#if 1
        if ((addr >= (uint32_t)pChipset->memAddr_phys_Const) && (addr < (uint32_t)(pChipset->memAddr_phys_Const + pChipset->MemSize))) {
                pChipset->memAccAddr = pChipset->memAddr;
        } else {
                /* Remapping */
                if ((pChipset->memAddr_1 != NULL)) {
                        iounmap(pChipset->memAddr_1); /* DSP_MEMORY_SIZE_8M */
                }

                /* Map the new Address */
                pChipset->memAddr_1 = ioremap_nocache((unsigned long)addr, DSP_MEMORY_SIZE_8M);

                if (pChipset->memAddr_1 == NULL) {
                        hb_slogf_error("%s: Map failed (Addr=0x%x)", __FUNCTION__, addr);
                        return -1;
                } else {
                        pChipset->memAccAddr = pChipset->memAddr_1;
                        pChipset->memAddr_1_phys = addr;
                        hb_slogf_error("%s error unknown addr:0x%x, remap it to virtual: 0x%x", __FUNCTION__, addr, (uint32_t __force)pChipset->memAccAddr);
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 4) {
                                hb_slogf_error("%s: Map done MapAddr=0x%x (Addr=0x%x)", __FUNCTION__, (uint32_t __force)pChipset->memAddr_1, addr);
                        }
#endif
                }
        }

        return 0;
#else
        uint32_t addr_masked;
#ifdef DYN_PAGE_MASK
        addr_masked = addr & pChipset->pageMask;
#else
        addr_masked = addr & MEMORY_PAGE_MASK;
#endif

        if (addr == (((uint32_t)pChipset->memAddr_phys_Const - 1024))) {
                pChipset->memAccAddr = (void*)((uint32_t)pChipset->memAddr - 1024);
#ifdef VERBOSE_ENABLE
                if (pChipset->verbose > 4) {
                        hb_slogf_error("%s (1): 0x%x", __FUNCTION__, (uint32_t)pChipset->memAccAddr);
                }
#endif
                return 0;
        }
#ifdef DYN_PAGE_MASK
        else if ((addr_masked) == (((uint32_t)pChipset->memAddr_phys_Const) & pChipset->pageMask))
#else
        else if ((addr_masked) == (((uint32_t)pChipset->memAddr_phys_Const) & MEMORY_PAGE_MASK))
#endif
        {

                pChipset->memAccAddr = pChipset->memAddr;
#ifdef VERBOSE_ENABLE
                if (pChipset->verbose > 4) {
                        hb_slogf_error("%s (2): 0x%x", __FUNCTION__, (uint32_t)pChipset->memAccAddr);
                }
#endif
                return 0;
        }
#ifdef DYN_PAGE_MASK
        else if ((addr_masked) == (DSP_INTERNAL_MEMORY_START & pChipset->pageMask))
#else
        else if ((addr_masked) == (DSP_INTERNAL_MEMORY_START & MEMORY_PAGE_MASK))
#endif
        {
                pChipset->memAccAddr = (void*)pChipset->dspMemBase;
#ifdef VERBOSE_ENABLE
                if (pChipset->verbose > 4) {
                        hb_slogf_error("%s (3): 0x%x", __FUNCTION__, pChipset->dspMemBase);
                }
#endif
                return 0;
        } else {

                /* Check if actual mapped matches */
#ifdef DYN_PAGE_MASK
                if ((addr_masked) == (((uint32_t)pChipset->memAddr_1_phys) & pChipset->pageMask))
#else
                if ((addr_masked) == (((uint32_t)pChipset->memAddr_1_phys) & MEMORY_PAGE_MASK))
#endif
                {
                        pChipset->memAccAddr = pChipset->memAddr_1;
                        return 0; /* Yes it matches */
                } else {
                        /* Remapping */
                        if ((pChipset->memAddr_1 != NULL)) {
                                iounmap((void*)pChipset->memAddr_1); /* DSP_MEMORY_SIZE_8M */
                        }

                        /* Map the new Address */
                        pChipset->memAddr_1 = (void*)ioremap_nocache((unsigned long)addr, DSP_MEMORY_SIZE_8M);

                        if (pChipset->memAddr_1 == 0) {
                                hb_slogf_error("%s: Map failed (Addr=0x%x)", __FUNCTION__, addr);
                                return -1;
                        } else {
                                pChipset->memAccAddr = pChipset->memAddr_1;
                                pChipset->memAddr_1_phys = addr;
#ifdef VERBOSE_ENABLE
                                if (pChipset->verbose > 4) {
                                        hb_slogf_error("%s: Map done MapAddr=0x%x (Addr=0x%x)", __FUNCTION__, (uint32_t)pChipset->memAddr_1, addr);
                                }
#endif
                        }
                }

                return 0;
        }
#endif
}

/**
 *  This function access the memory inside the DSP with 32 bit accesses
 *
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param addr Address inside the DSP memory map
 *  @param numLongWords number of consecutive accesses
 *  @param dir Access type (MEM_READ | MEM_WRITE )
 *  @param pVal Pointer to source or destination value
 *  @return int error code 0=OK, -1=failure
 */
int dra6xxAccessMem32(dspIpcChipset_t* pChipset,
    uint32_t addr,
    uint32_t numLongWords,
    memAccessDir_t dir,
    uint32_t* pVal)
{
        uint32_t page = 0xbeefdead;
        uint32_t page_end = 0xdeadbeef;
        uint32_t value;
        int errCode;
        uint32_t addr_end;

        /* Check the alignment of the address (check not needed in *_oc functions */
        if ((addr & 0x03U) != 0) {
                hb_slogf_error("%s: Address 0x%08X is not valid", __FUNCTION__, addr);
                return -1;
        }

        errCode = pthread_mutex_lock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_LOCK_FAILED, __FUNCTION__);
        }

        if (setMemSelAdr(pChipset, addr) != 0) {
                hb_slogf_error("%s: error setting Mem-Select", __FUNCTION__);
        }
#ifdef DYN_PAGE_MASK
        addr_end = addr + ((numLongWords << 2) - 1); /* Claculate end address of Transfer */
        if ((addr_end >= (uint32_t)pChipset->memAddr_phys_Const) && (addr_end < (uint32_t)(pChipset->memAddr_phys_Const + pChipset->MemSize))) {
                page_end = 1;
        }
        if ((addr >= (uint32_t)pChipset->memAddr_phys_Const) && (addr < (uint32_t)(pChipset->memAddr_phys_Const + pChipset->MemSize))) {
                page = 1;
        }
#else
        addr_end = addr + ((numLongWords << 2) - 1);                   /* Claculate end address of Transfer */
        page_end = (addr_end & MEMORY_PAGE_MASK) >> MEMORY_PAGE_SHIFT; /* Cal. page for end of transfer */
        page = (addr & MEMORY_PAGE_MASK) >> MEMORY_PAGE_SHIFT;         /* Cal. Page for start address */
#endif

        if (page != page_end) {
                pChipset->dspp = page;
                hb_slogf_error("%s: error 2 Page Access 0x%08X S:%d", __FUNCTION__, addr, (numLongWords << 2));
        }
        if (page == page_end) {
                if (dir == MEM_READ) {
                        dra6xxMemRead_oc(pChipset, addr, pVal, numLongWords);
                } else {
                        dra6xxMemWrite_oc(pChipset, addr, pVal, numLongWords);
                }
        } else {
                if (dir == MEM_READ) {

                        while (numLongWords != 0) {
                                if (dra6xxMemRead(pChipset, addr, &value) != 0) {
                                        hb_slogf_error(DRA6XXREAD_FAILED, __FUNCTION__);
                                }
                                *pVal++ = value;
                                addr += 4;
                                if (setMemSelAdr(pChipset, addr) != 0) {
                                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                                }
                                numLongWords--;
                        }
                } else {
                        while (numLongWords != 0) {
                                value = *pVal++;
                                if (dra6xxMemWrite(pChipset, addr, value) != 0) {
                                        hb_slogf_error(DRA6XXMEMWRITE_FAILED, __FUNCTION__);
                                }
                                addr += 4;

                                if (setMemSelAdr(pChipset, addr) != 0) {
                                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                                }
                                numLongWords--;
                        }
                }
        }
        errCode = pthread_mutex_unlock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_UNLOCK_FAILED, __FUNCTION__);
        }

        return (0);
}

/**
 *  This function access the memory inside the DSP with 8 bit accesses
 *
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param addr Address inside the DSP memory map
 *  @param numLongWords number of consecutive accesses
 *  @param dir Access type (MEM_READ | MEM_WRITE )
 *  @param pVal Pointer to source or destination value
 *  @return int error code 0=OK, -1=failure
 */
int dra6xxAccessMem8(dspIpcChipset_t* pChipset,
    uint32_t addr,
    uint32_t numBytes,
    memAccessDir_t dir,
    uint8_t* pVal)
{
        uint32_t page = 0xbeefdead;
        uint32_t page_end = 0xdeadbeef;
        int errCode;
        uint32_t addr_end;
        uint8_t* pVal8 = NULL;
        uint8_t value8 = 0;

        /* Check the alignment of the address (check not needed in *_oc functions */
        if ((addr & 0x03U) != 0) {
                hb_slogf_error("%s: Address 0x%08X is not valid", __FUNCTION__, addr);
                return -1;
        }

        errCode = pthread_mutex_lock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_LOCK_FAILED, __FUNCTION__);
        }

        if (setMemSelAdr(pChipset, addr) != 0) {
                hb_slogf_error("%s: error setting Mem-Select", __FUNCTION__);
        }
#ifdef DYN_PAGE_MASK
        addr_end = addr + ((numBytes)-1); /* Claculate end address of Transfer */
        if ((addr_end >= (uint32_t)pChipset->memAddr_phys_Const) && (addr_end < (uint32_t)(pChipset->memAddr_phys_Const + pChipset->MemSize))) {
                page_end = 1;
        }
        if ((addr >= (uint32_t)pChipset->memAddr_phys_Const) && (addr < (uint32_t)(pChipset->memAddr_phys_Const + pChipset->MemSize))) {
                page = 1;
        }

#else
        addr_end = addr + ((numBytes)-1);                              /* Claculate end address of Transfer */
        page_end = (addr_end & MEMORY_PAGE_MASK) >> MEMORY_PAGE_SHIFT; /* Cal. page for end of transfer */
        page = (addr & MEMORY_PAGE_MASK) >> MEMORY_PAGE_SHIFT;         /* Cal. Page for start address */
#endif

        if (page != page_end) {
                pChipset->dspp = page;
                hb_slogf_error("%s: 2 Page Access 0x%08X S:%d", __FUNCTION__, addr, (numBytes));
        }

        if (page == page_end) /* Wenn Start und Ende in einer Page sind */
        {
                if (dir == MEM_READ) {
                        uint8_t* dest = pVal;
                        uint32_t addr_masked = (addr & pChipset->AdrCheckPageMask); /* addr */

                        while (numBytes != 0) {
                                value8 = *((volatile char*)((char __force*)(pChipset->memAccAddr) + (addr_masked)));
                                *dest = value8;
                                dest++;
                                addr_masked += 1;
                                numBytes--;
                        }
                } else {
                        uint32_t addr_masked = (addr & pChipset->AdrCheckPageMask); /* addr */
                        while (numBytes != 0) {
                                *((volatile char*)((char __force*)(pChipset->memAccAddr + addr_masked))) = *pVal;
                                pVal++;
                                addr_masked += 1;
                                numBytes--;
                        }
                }
        } else {
                if (dir == MEM_READ) {
                        while (numBytes != 0) {
                                value8 = *((volatile char*)((char __force*)(pChipset->memAccAddr) + (addr & pChipset->AdrCheckPageMask)));
                                *pVal8++ = value8;
                                addr += 1;

                                if (setMemSelAdr(pChipset, addr) != 0) {
                                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                                }
                                numBytes--;
                        }
                } else {
                        while (numBytes != 0) {
                                value8 = *pVal8++;
                                *((volatile char*)((char __force*)(pChipset->memAccAddr) + (addr & pChipset->AdrCheckPageMask))) = value8;
                                addr += 1;
                                if (setMemSelAdr(pChipset, addr) != 0) {
                                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                                }
                                numBytes--;
                        }
                }
        }
        errCode = pthread_mutex_unlock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_UNLOCK_FAILED, __FUNCTION__);
        }

        return (0);
}

/**
 *  This function writes the current read and/or write index values to the
 *  specified queue. The queue is specified with its offset inside the dsp
 *  administation structure. The offset of the queue inside the dsp is returned,
 *  the value of the index values written to the queue.
 *
 *  @param pChipset Pointer to the dm642 chipset layer administration structure
 *  @param queueOff Offset of the queue inside the dsp administration structure
 *  @param pRead Pointer to new read index value (NULL if not to be set)
 *  @param pWrite Pointer to new write index value (NULL if not to be set)
 *  @return unsigned short converted value
 */
static uint32_t dra6xx_set_queue_index(dspIpcChipset_t* pChipset, uint32_t queueOff,
    const int32_t* pRead, const int32_t* pWrite)
{
        uint32_t accAdr;
        uint32_t dspQueueAdr;
        int errCode;
        uint32_t value;

        errCode = pthread_mutex_lock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_LOCK_FAILED, __FUNCTION__);
        }

        /* get DSP queue adress */
        accAdr = DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspAdminStructAdr) + queueOff;
        if (setMemSelAdr(pChipset, accAdr) != 0) /* Set to Mapped RAM */
        {
                hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
        }

        if (dra6xxMemRead(pChipset, accAdr, &value) != 0) {
                hb_slogf_error(DRA6XXMEMREAD_FAILED, __FUNCTION__, (-4));
        }
        dspQueueAdr = DRA6XX_CONV_DSP2HOST(pChipset, value);

        if (pWrite != NULL) {
                /* get DSP queue write index */
                accAdr = dspQueueAdr + offsetof(struct _DSPIPCMessageQueue, writeIndex);
                if (setMemSelAdr(pChipset, accAdr) != 0) /* Set to Mapped RAM */
                {
                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                }
                if (dra6xxMemWrite(pChipset, accAdr, *pWrite) != 0) {
                        hb_slogf_error(DRA6XXMEMWRITE_FAILED_2, __FUNCTION__, (-1)); /* DRA6XXMEMWRITE_FAILED_2  DM642MEMWRITE_FAILED_2 */
                }
        }

        if (pRead != NULL) {
                /* get DSP queue read index */
                accAdr = dspQueueAdr + offsetof(struct _DSPIPCMessageQueue, readIndex);
                if (setMemSelAdr(pChipset, accAdr) != 0) /* Set to Mapped RAM */
                {
                        hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                }
                if (dra6xxMemWrite(pChipset, accAdr, *pRead) != 0) {
                        hb_slogf_error(DRA6XXMEMWRITE_FAILED_2, __FUNCTION__, (-2));
                }
        }

        errCode = pthread_mutex_unlock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_UNLOCK_FAILED, __FUNCTION__);
        }

        return (dspQueueAdr);
}

/**
 *  This function read the current read and/or write index values from the
 *  specified queue. The queue is specified with its offset inside the dsp
 *  administation structure. The offset of the queue inside the dsp is returned,
 *  the value of the index values written to the referenced variables.
 *
 *  @param pChipset Pointer to the dra44xi chipset layer administration structure
 *  @param queueOff Offset of the queue inside the dsp administration structure
 *  @param pRead Pointer to store the read index to (NULL if not required)
 *  @param pWrite Pointer to store the write index to (NULL if not required)
 *  @return delivers the converted offset to the queue
 */
static uint32_t dra6xx_get_queue_index(dspIpcChipset_t* pChipset, uint32_t queueOff,
    int32_t* pRead, int32_t* pWrite)
{
        uint32_t accAdr;
        uint32_t dspQueueAdr;
        int errCode;
        uint32_t value;

        errCode = pthread_mutex_lock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_LOCK_FAILED, __FUNCTION__);
        }

        /* get DSP queue adress */
        accAdr = DRA6XX_CONV_DSP2HOST(pChipset, pChipset->dspAdminStructAdr) + queueOff;

        if (setMemSelAdr(pChipset, accAdr) != 0) /* Set to Mapped RAM */
        {
                hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
        }

        if (dra6xxMemRead(pChipset, accAdr, &value) != 0) {
                hb_slogf_error(DRA6XXMEMREAD_FAILED, __FUNCTION__, (-1)); /* DRA6XXMEMREAD_FAILED */
        }
        dspQueueAdr = DRA6XX_CONV_DSP2HOST(pChipset, value);

        if ((dspQueueAdr >= pChipset->dspRootAddr) && (dspQueueAdr < pChipset->dspRootAddr + pChipset->MemSize)) {
                if (pWrite != NULL) {
                        /* get DSP queue write index */
                        accAdr = dspQueueAdr + offsetof(struct _DSPIPCMessageQueue, writeIndex);
                        if (setMemSelAdr(pChipset, accAdr) != 0) /* Set to Mapped RAM */
                        {
                                hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                        }

                        if (dra6xxMemRead(pChipset, accAdr, (uint32_t*)pWrite) != 0) {
                                hb_slogf_error(DRA6XXMEMREAD_FAILED, __FUNCTION__, (-2)); /* DRA6XXMEMREAD_FAILED */
                        }
                }

                if (pRead != NULL) {
                        /* get DSP queue read index */
                        accAdr = dspQueueAdr + offsetof(struct _DSPIPCMessageQueue, readIndex);
                        if (setMemSelAdr(pChipset, accAdr) != 0) /* Set to Mapped RAM */
                        {
                                hb_slogf_error("%s: setting Mem-Select", __FUNCTION__);
                        }

                        if (dra6xxMemRead(pChipset, accAdr, (uint32_t*)pRead) != 0) {
                                hb_slogf_error(DRA6XXMEMREAD_FAILED, __FUNCTION__, (-3));
                        }
                }
        } else {
                hb_slogf_error("%s: invalid dspQueueAdr=0x%08x", __FUNCTION__, dspQueueAdr);
        }

        errCode = pthread_mutex_unlock(&(pChipset->memAccessMutex));
        if (errCode != EOK) {
                hb_slogf_error(MEM_ACC_MUTEX_UNLOCK_FAILED, __FUNCTION__);
        }

        return (dspQueueAdr);
}

/**
 *  This function sends a command to the dsp and wait for the response from the
 *  DSP.
 *
 *  @param pChipset Pointer to the dm642 chipset layer administration structure
 *  @param pMsg Pointer to a communication message for the DSP
 *  @param pPhysMsg  this is the physical address of communication message for the DSP
 *  @param setInterrupt Signalize the packet with an interrupt (DRA6XX_SEND_NO_INT_TO_DSP|DRA6XX_SEND_DO_INT_TO_DSP)
 *  @return int 0 if everything was ok, -1 if an error has occurred, -2 if queue is full
 */
static int dra6xxSendMsg(dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg, uint32_t pPhysMsg,
    const dra6xxSendIndicator_t setInterrupt)
{
        int retValue = -1;
        uint32_t dspRxQueueAdr;
        int32_t dspRxQueueWriteIndex; /* must be int to ensure correct difference calculation */
        int32_t dspRxQueueReadIndex;  /* must be int to ensure correct difference calculation */
        int32_t numElements;          /* must be int to ensure correct difference calculation */

        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        if (pMsg == NULL) {
                hb_slogf_error("%s: No message send to %s Rx queue", __FUNCTION__, pChipset->pDevice->pDeviceName);
                return (-1);
        }

        dspRxQueueAdr = dra6xx_get_queue_index(pChipset,
            ADMIN_OFFSET(dspRxBufferAddr),
            &dspRxQueueReadIndex,
            &dspRxQueueWriteIndex);

        if ((void*)dspRxQueueAdr == NULL) {
                hb_slogf_error("%s: %s Rx queue dspRxQueueAdr is null", __FUNCTION__, pChipset->pDevice->pDeviceName);
                return (-1);
        }

#ifdef VERBOSE_ENABLE
        if (pChipset->verbose > 2) {
                hb_slogf_error("%s: Current %s-RX index: read=%d, write=%d, addr=0x%08x", __FUNCTION__, pChipset->pDevice->pDeviceName, dspRxQueueReadIndex, dspRxQueueWriteIndex, dspRxQueueAdr);
        }
#endif

        numElements = dspRxQueueWriteIndex - dspRxQueueReadIndex;

        if (numElements >= COMMANDQUEUE_LENGTH) {
                hb_slogf_error("%s: %s RX Queue full numElem:%d (wIdx:%d/rIdx:%d)", __FUNCTION__, pChipset->pDevice->pDeviceName, numElements, dspRxQueueWriteIndex, dspRxQueueReadIndex);
                if (setInterrupt == DRA6XX_SEND_DO_INT_TO_DSP) {
                        hb_slogf_error("%s:set interrupt to %s, exclude the condition that Queue is full but no entry to set INT.", __FUNCTION__, pChipset->pDevice->pDeviceName);
                        /* set Host to DSP interrupt */
                        dra6xxSetInterrupt(pChipset);
                }
                return (-2);
        }

#ifdef VERBOSE_ENABLE
        if (pChipset->verbose > 3) {
                dra6xxDumpMessage("before send call", pMsg);
        }
#endif

        /* write message */
        if (pPhysMsg == 0) {
                if (dra6xxAccessMem32(pChipset,
                        dspRxQueueAdr + offsetof(struct _DSPIPCMessageQueue, msg) + (((uint32_t)dspRxQueueWriteIndex) % COMMANDQUEUE_LENGTH) * sizeof(DSPIPCMessageQueueEntry),
                        sizeof(DSPIPCMessageQueueEntry) >> 2, MEM_WRITE, (uint32_t*)pMsg) != 0) {
                        hb_slogf_error("%s: Access failed", __FUNCTION__);
                }
        } else {
                if (dra6xxAccessMem32_DMA(pChipset,
                        dspRxQueueAdr + offsetof(struct _DSPIPCMessageQueue, msg) + (((uint32_t)dspRxQueueWriteIndex) % COMMANDQUEUE_LENGTH) * sizeof(DSPIPCMessageQueueEntry),
                        sizeof(DSPIPCMessageQueueEntry) >> 2, MEM_WRITE, (uint32_t*)pMsg, pPhysMsg)
                    != 0) {
                        hb_slogf_error("%s: DMA Access failed", __FUNCTION__);
                }
        }

        /* calculate new write index */
        dspRxQueueWriteIndex++;

#ifdef VERBOSE_ENABLE
        if (pChipset->verbose > 2) {
                hb_slogf_error("%s: Set DSP-RX index write to %d", __FUNCTION__, dspRxQueueWriteIndex);
        }
#endif

        /* set write queue write index */
        /* return value is address offset of queue, not required here */
        (void)dra6xx_set_queue_index(pChipset,
            ADMIN_OFFSET(dspRxBufferAddr),
            NULL,
            &dspRxQueueWriteIndex);

        if (setInterrupt == DRA6XX_SEND_DO_INT_TO_DSP) {
                /* set Host to DSP interrupt */
                dra6xxSetInterrupt(pChipset);
        }

        retValue = 0;
        return (retValue);
}

/**
 *  This function set the interrupt flag inside the DSP to signalize a
 *  communication request
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @return void
 */
static void dra6xxSetInterrupt(dspIpcChipset_t* pChipset)
{
        uint32_t value;
        uint32_t value_2;
        uint32_t counter = 0;
#ifdef VERBOSE_ENABLE
        uint32_t mem_val;
#endif

        if (pChipset != NULL) {
                /* Check Status of ARM to DSP Interrupt */
#ifdef VERBOSE_ENABLE
                if (pChipset->verbose > 0) {
                        if (strcmp(pChipset->pDevice->pDeviceName, "dsp") == 0) {
                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(1), &mem_val); /* Read Fifo-Status Register of Mailbox 1 */
                        } else if (strcmp(pChipset->pDevice->pDeviceName, "m3") == 0) {
                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(5), &mem_val); /* Read Fifo-Status Register of Mailbox 5 */
                        }

                        if (mem_val & 0x00000000) {
                                hb_slogf_error("%s: %s Interrupt not set back", __FUNCTION__, pChipset->pDevice->pDeviceName);
                        }
                }
#endif
                if (strcmp(pChipset->pDevice->pDeviceName, "dsp") == 0) {
                        do {
                                counter++;
                                value_2 = MAILBOX_FIFOSTATUS(1);
                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(1), &value); /* Read Fifo-Status Register of Mailbox 1 */
                        } while ((value == 0x00000001) && (counter < 1000));

                        if ((value == 0x00000001)) {
                                /* wait until DSP has noticed the INT */
                                counter = 0;
                                do {
                                        mdelay(1);
                                        counter++;
                                        /* get DSP interrupt status */
                                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(1), &value); /* Read Fifo-Status Register of Mailbox 1 */
                                } while ((value == 0x00000001) && (counter < 10));

                                if ((value == 0x00000001)) {
                                        hb_slogf_error("%s: ERROR: %s is not handling the Interrupt after 10ms when check mailbox fifo status!!", __FUNCTION__, pChipset->pDevice->pDeviceName);
                                }
                        }
                        /* get DSP interrupt status */
                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(1), &value);
                        if (value & 0x00000004) /*interrupt is set to DSP */
                        {
                                hb_slogf_error("%s: interrupt to %s is already set", __FUNCTION__, pChipset->pDevice->pDeviceName);
                                counter = 0;
                                do {
                                        mdelay(1);
                                        counter++;
                                        /* get DSP interrupt status */
                                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(1), &value); /* Read Fifo-Status Register of Mailbox 1 */
                                } while (((value & 0x00000004) == 0x00000004) && (counter < 10));
                                if (((value & 0x00000004) == 0x00000004)) {
                                        hb_slogf_error("%s: ERROR: %s is not handling the Interrupt after 10ms when check mailbox interrupt pending flag!!", __FUNCTION__, pChipset->pDevice->pDeviceName);
                                }
                                return;
                        } else {
                                value_2 = MAILBOX_MESSAGE(1);
                                dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, MAILBOX_MESSAGE(1), 0xDEADBEEF); /* write message  */
                        }
                } else if (strcmp(pChipset->pDevice->pDeviceName, "m3") == 0) {
                        do {
                                counter++;
                                value_2 = MAILBOX_FIFOSTATUS(5);
                                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(5), &value); /* Read Fifo-Status Register of Mailbox 1 */
                        } while ((value == 0x00000001) && (counter < 1000));

                        if ((value == 0x00000001)) {
                                /* wait until DSP has noticed the INT */
                                counter = 0;
                                do {
                                        mdelay(1);
                                        counter++;
                                        /* get DSP interrupt status */
                                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(5), &value); /* Read Fifo-Status Register of Mailbox 1 */
                                } while ((value == 0x00000001) && (counter < 10));

                                if ((value == 0x00000001)) {
                                        hb_slogf_error("%s: ERROR: %s is not handling the Interrupt after 10ms when check mailbox fifo status!!", __FUNCTION__, pChipset->pDevice->pDeviceName);
                                }
                        }
                        /* get M3 interrupt status */
                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(2), &value);
                        if (value & 0x00000400) /*interrupt is set to M3 */
                        {
                                hb_slogf_error("%s: interrupt to %s is already set", __FUNCTION__, pChipset->pDevice->pDeviceName);
                                counter = 0;
                                do {
                                        mdelay(1);
                                        counter++;
                                        /* get DSP interrupt status */
                                        dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(2), &value); /* Read Fifo-Status Register of Mailbox 1 */
                                } while (((value & 0x00000400) == 0x00000400) && (counter < 10));

                                if (((value & 0x00000400) == 0x00000400)) {
                                        hb_slogf_error("%s: ERROR: %s is not handling the Interrupt after 10ms when check mailbox interrupt pending flag!!", __FUNCTION__, pChipset->pDevice->pDeviceName);
                                }
                                return;
                        } else {
                                value_2 = MAILBOX_MESSAGE(5);
                                dra6xxRegWrite32(pChipset, MOD_DEVCONF_REG, MAILBOX_MESSAGE(5), 0xDEADBEEF); /* write message	   */
                        }
                }

        } else {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
        }
        return;
}

/**
 *  This function dumps the binary contents of a dsp message
 *
 *  @param pString Pointer to the identification string
 *  @param pMsg Pointer to a communication message for the DSP
 *  @return void
 */
static void dra6xxDumpMessage(const char* pString, DSPIPCMessageQueueEntry* pMsg)
{
        unsigned i;
        unsigned char* ptr = NULL;
        char* pBuffer = NULL;
        const char* pFormat = "Msg structure %s: ";
        int newPos;
        DSPIPCStreamMsg* pStreamMsg;
        DSPIPCAdminMsg* pAdminMsg;

        if (pMsg != NULL) {
                if (pString != NULL) {

                        pBuffer = MALLOC_TRC(3 * sizeof(DSPIPCMessageQueueEntry) + strlen(pString) + strlen(pFormat) + 16);

                        if (pBuffer != NULL) {
                                newPos = snprintf(pBuffer, 16 + strlen(pString) + 1, pFormat, pString);
                                ptr = (unsigned char*)pMsg;
                                for (i = 0; i < sizeof(DSPIPCMessageQueueEntry); i++) {
                                        newPos += snprintf(&pBuffer[newPos], 4, "%02X ", *(ptr++));
                                }
                                hb_slogf_error("%s: %s", __FUNCTION__, pBuffer);
                                FREE_TRC(pBuffer);
                                pBuffer = NULL;
                                hb_slogf_error("%s: ChannelId=0x%08X MsgId=0x%08X MsgSize=%d byte", __FUNCTION__,
                                    pMsg->channelId, pMsg->msgId, pMsg->msgSize);
                                switch ((pMsg->msgType) & DSPIPC_MSGTYPE_TYPE_MSK) {
                                case DSPIPC_MSGTYPE_TYPE_COMMAND:
                                        hb_slogf_error("%s: Content type is <command message>", __FUNCTION__);
                                        if ((pMsg->channelId & ~DSPIPC_CHANNELTYPE_MSK) < 16) {
                                                switch (pMsg->channelId & 0x0FU) {
                                                case DSPIPC_INVALID:
                                                        hb_slogf_error("%s: Message channel id is invalid", __FUNCTION__);
                                                        break;
                                                case DSPIPC_IPCADMIN:
                                                        hb_slogf_error("%s: Message channel id is IPCADMIN", __FUNCTION__);
                                                        pAdminMsg = &(pMsg->payload.adminMsg);
                                                        switch (pAdminMsg->generic.cmdType) {
                                                        case DCCMID_INVALID:
                                                                hb_slogf_error("%s: admin message is <invalid>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_ERROR:
                                                                hb_slogf_error("%s: admin message is <ERROR>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_VERSION:
                                                                hb_slogf_error("%s: admin message is <VERSION>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_ADDCHANNELS:
                                                                hb_slogf_error("%s: admin message is <ADDCHANNELS>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_REMOVECHANNELS:
                                                                hb_slogf_error("%s: admin message is <REMOVECHANNELS>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_ADDCHANNELS_COMPLETE:
                                                                hb_slogf_error("%s: admin message is <ADDCHANNELSCOMPLETE>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_CHANGECHANNELPRIO:
                                                                hb_slogf_error("%s: admin message is <CHANGECHANNELPRIO>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_CLOSEDOWN:
                                                                hb_slogf_error("%s: admin message is <CLOSEDOWN>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_DRAINREQ:
                                                                hb_slogf_error("%s: admin message is <DRAINREQ>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_DRAINACK:
                                                                hb_slogf_error("%s: admin message is <DRAINACK>", __FUNCTION__);
                                                                break;
                                                        case DCCMID_DRAINCOMPLETE:
                                                                hb_slogf_error("%s: admin message is <DRAINCOMPLETE>", __FUNCTION__);
                                                                break;
                                                        default:
                                                                hb_slogf_error("%s: admin message is <unknown>", __FUNCTION__);
                                                                break;
                                                        }
                                                        break;
                                                case DSPIPC_PRINT:
                                                        hb_slogf_error("%s: Message channel id is PRINT", __FUNCTION__);
                                                        break;
                                                case DSPIPC_LOG:
                                                        hb_slogf_error("%s: Message channel id is LOG", __FUNCTION__);
                                                        break;
                                                case DSPIPC_ALIVE:
                                                        hb_slogf_error("%s: Message channel id is ALIVE", __FUNCTION__);
                                                        break;
                                                default:
                                                        hb_slogf_error("%s: Message channel id is not defined", __FUNCTION__);
                                                        break;
                                                }
                                        } else {
                                                /* user channel */
                                                hb_slogf_error("%s: Message channel id is in USER area", __FUNCTION__);
                                        }
                                        break;
                                case DSPIPC_MSGTYPE_TYPE_STREAM:
                                        hb_slogf_error("%s: Content type is <stream message>", __FUNCTION__);
                                        pStreamMsg = &(pMsg->payload.streamMsg);
                                        hb_slogf_error("%s: Cmd=%d (%s) CmdId=0x%08X NumRanges=%d Bufferflags=0x%08X",
                                            __FUNCTION__,
                                            pStreamMsg->cmd, CmdToString(pStreamMsg->cmd), pStreamMsg->cmdId, pStreamMsg->numRanges,
                                            pStreamMsg->bufferFlags);
                                        for (i = 0; i < DSPIPC_RANGES_PER_BUFFER; i++) {
                                                hb_slogf_error("%s: Range[%d]: Phys=0x%08X RangeSize=%u DataSize=%d",
                                                    __FUNCTION__,
                                                    i,
                                                    pStreamMsg->ranges[i].physicalAddress,
                                                    pStreamMsg->ranges[i].rangeSize,
                                                    pStreamMsg->ranges[i].dataSize);
                                        }
                                        break;
                                default:
                                        hb_slogf_error("%s: Unknown content type 0x%08X", __FUNCTION__,
                                            pMsg->msgType);
                                        break;
                                }
                        } else {
                                hb_slogf_error("%s: No mem for line buffer", __FUNCTION__);
                        }
                } else {
                        hb_slogf_error("%s: Invalid string", __FUNCTION__);
                }
        } else {
                hb_slogf_error("%s: Invalid message pointer", __FUNCTION__);
        }
        return;
}

static const char* CmdToString(uint32_t cmd)
{
        const char* pString = "Unknown";

        switch (cmd) {
        case DSPIPC_SM_PROVIDEBUFFER:
                pString = "ProvideBuffer";
                break;
        case DSPIPC_SM_TRANSFERREQUEST:
                pString = "TransferRequest";
                break;
        case DSPIPC_SM_BUFFERCOMPLETE:
                pString = "BufferComplete";
                break;
        case DSPIPC_SM_FLUSHBUFFERS:
                pString = "FlushBuffers";
                break;
        default:
                break;
        }
        return (pString);
}

#ifdef DUMP_DATA
/**
 *  This function dumps the binary contents of a data segment
 *
 *  @param pString Pointer to the identification string
 *  @param pMsg Pointer to a data array to be dumped
 *  @param size Size of the array to be dumped
 *  @return void
 */
static void dra6xxDumpData(const char* pString, void* pMsg, unsigned int size)
{
        unsigned int i;
        unsigned char* ptr = NULL;
        char* pBuffer = NULL;
        const char* pFormat = "Data content %s: ";
        int bufferLen;
        int newPos;

        if (pMsg != NULL) {
                if (pString != NULL) {
                        bufferLen = (3 * size) + strlen(pString) + strlen(pFormat) + 14;
                        pBuffer = MALLOC_TRC(bufferLen);
                        if (pBuffer != NULL) {
                                newPos = snprintf(pBuffer, bufferLen, pFormat, pString);
                                newPos = 0;
                                ptr = (unsigned char*)pMsg;
                                for (i = 0; (i < size) && (newPos < bufferLen); i++) {
                                        newPos += snprintf(&pBuffer[newPos], bufferLen - newPos, "%02X ", *(ptr++));
                                        if ((i % 4) == 3) {
                                                newPos += snprintf(&pBuffer[newPos], bufferLen - newPos, " ");
                                        }
                                        if ((newPos > 76)) {
                                                hb_slogf_error("%s: %s", __FUNCTION__, pBuffer);
                                                newPos = 0;
                                        }
                                }
                                if (newPos != 0) {
                                        hb_slogf_error("%s: %s", __FUNCTION__, pBuffer);
                                }
                                FREE_TRC(pBuffer);
                                pBuffer = NULL;
                        } else {
                                hb_slogf_error("%s: No mem for line buffer", __FUNCTION__);
                        }
                } else {
                        hb_slogf_error("%s: Invalid string", __FUNCTION__);
                }
        } else {
                hb_slogf_error("%s: Invalid message pointer", __FUNCTION__);
        }
        return;
}
#endif

/**
 *  This function process a new incoming message. This message is separated into
 *  the basic channel type of the message. This function calls the reponsible
 *  functions doing the next processing
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param pMsg Pointer to the new incoming message
 *  @param newScan pointer to integer value that shows if new scan should be done
 *  @return int Error code, 0=ok, -1=failure
 */
static int ProcessNewMessage(dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg, int* newScan)
{
        int retValue = -1;

        switch (pMsg->channelId & DSPIPC_CHANNELTYPE_MSK) {
        case DSPIPC_CHANNELTYPE_STREAM:
        case DSPIPC_CHANNELTYPE_COMMAND:
                /* command message received */
                if (ProcessCommandStreamMessage(pChipset, pMsg, newScan) == 0) {
                        retValue = 0;
                }
                break;
        case DSPIPC_CHANNELTYPE_INTERNAL:
                /* internal message received */
                if (ProcessInternalMessage(pChipset, pMsg) == 0) {
                        retValue = 0;
                }
                break;
        default:
                hb_slogf_error("%s: Unsupported message type", __FUNCTION__);
                break;
        }

        return (retValue);
}

/**
 *  This function process a new internal message. This message is separated into
 *  the basic internal channels. This function calls the reponsible
 *  functions doing the next processing
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param pMsg Pointer to the new incoming message
 *  @return int Error code, 0=ok, -1=failure
 */
static int ProcessInternalMessage(dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg)
{
        int retValue = -1;

        switch (pMsg->channelId & DSPIPC_CHANNELNUMBER_MSK) {
        case DSPIPC_IPCADMIN:
                if (ProcessInternalAdmin(pChipset, pMsg) == 0) {
                        retValue = 0;
                }
                break;
        case DSPIPC_PRINT:
                if (ProcessInternalPrint(pChipset, pMsg) == 0) {
                        retValue = 0;
                }
                break;
        case DSPIPC_LOG:
                if (ProcessInternalLog(pChipset, pMsg) == 0) {
                        retValue = 0;
                }
                break;
        default:
                hb_slogf_error("%s: Unsupported internal message", __FUNCTION__);
                break;
        }

        return (retValue);
}

#define __QUEUE_SCAN_INFO

static char* ADD_CMD_MSG = "%s: Add CMD message failed CH:%d %d";
/**
 *  This function process a new command or stream message. This message is copied into the
 *  input queue of the related sub device.
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param pMsg Pointer to the new incoming message
 *  @param newScan pointer to integer value that shows if new scan should be done
 *  @return int Error code, 0=ok, -1=failure
 */
static int ProcessCommandStreamMessage(const dspIpcChipset_t* pChipset, const DSPIPCMessageQueueEntry* pMsg, int* newScan)
{
        uint32_t i;
        int retValue;
        int error;
        subDeviceTable_t* pSubDevice;
        deviceTable_t* pDevice;
        messagePointer_t pPayload;
        dspipc_recvStreamMessage_t* pRecvStreamMsg;
        messagePointer_t pMessage;
        uint32_t cmdId;
        const DSPIPCStreamMsg* pStreamMsg;
        uint32_t numEntries;
        memObjList_t* pMemObj;
        uint32_t memId;
        uint32_t memOffset;
        memSearchType_t memSearchType;

        retValue = -1;

        pDevice = pChipset->pDevice;
        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        *newScan = 0;

        /* locate subdevice */
        pSubDevice = dspipcSearchSubDevice(pDevice,
            SD_DIR_IN,
            pMsg->channelId & DSPIPC_CHANNELNUMBER_MSK);
        if (pSubDevice != NULL) {
                switch (pSubDevice->type) {
                case SD_COMMAND:
                        pPayload.generic = (void*)(&(pMsg->payload.message[0]));
                        if (pPayload.generic != NULL) {
                                cmdId = 0;
                                if ((retValue = dspipcQueueAddMessage(pDevice,
                                         RECEIVE_CALLED,
                                         pSubDevice->pRecvMsgQueue,
                                         DSPIPC_CONT_MESSAGE,
                                         pMsg->msgId,
                                         cmdId,
                                         /* get the next paramters out of the message */
                                         DSPIPC_MSGTYPE_COMMAND,
                                         DSPIPC_MSGREPLY_NO,
                                         pPayload,
                                         pMsg->msgSize))
                                    != 0) {
                                        hb_slogf_error(ADD_CMD_MSG, __FUNCTION__, (pMsg->channelId & DSPIPC_CHANNELNUMBER_MSK), retValue);
                                        retValue = -1;
                                }
#ifdef QUEUE_SCAN_INFO
                                else {
                                        hb_slogf_error("%s: Recv CMD-Msg:%d bytes, %s",
                                            __FUNCTION__, pMsg->msgSize, pSubDevice->pSubDeviceName);
                                }
#endif
                        } else {
                                hb_slogf_error(NO_MEMORY, __FUNCTION__);
                        }
                        break;
                case SD_STREAM:
                        if ((pMsg->msgType & DSPIPC_MSGTYPE_TYPE_MSK) == DSPIPC_MSGTYPE_TYPE_COMMAND) {
                                pPayload.generic = (void*)(&(pMsg->payload.message[0]));
                                if (pPayload.generic != NULL) {
                                        cmdId = 0;
                                        if ((retValue = dspipcQueueAddMessage(pDevice,
                                                 RECEIVE_CALLED,
                                                 pSubDevice->pRecvMsgQueue,
                                                 DSPIPC_CONT_MESSAGE,
                                                 pMsg->msgId,
                                                 cmdId,
                                                 /* get the next paramters out of the message */
                                                 DSPIPC_MSGTYPE_COMMAND,
                                                 DSPIPC_MSGREPLY_NO,
                                                 pPayload,
                                                 pMsg->msgSize))
                                            != 0) {
                                                hb_slogf_error(ADD_CMD_MSG, __FUNCTION__, (pMsg->channelId & DSPIPC_CHANNELNUMBER_MSK), retValue);
                                                retValue = -1;
                                        }
#ifdef QUEUE_SCAN_INFO
                                        else {
                                                hb_slogf_error("%s: Recv CMD-Msg:%d bytes, %s",
                                                    __FUNCTION__, pMsg->msgSize, pSubDevice->pSubDeviceName);
                                        }
#endif
                                } else {
                                        hb_slogf_error(NO_MEMORY, __FUNCTION__);
                                }
                        } else if ((pMsg->msgType & DSPIPC_MSGTYPE_TYPE_MSK) == DSPIPC_MSGTYPE_TYPE_STREAM) {
                                if (pMsg->msgSize == sizeof(DSPIPCStreamMsg)) {
                                        pStreamMsg = &(pMsg->payload.streamMsg);
                                        switch (pStreamMsg->cmd) {
                                        case DSPIPC_SM_BUFFERCOMPLETE:
                                                error = 0;

                                                numEntries = pStreamMsg->numRanges;
                                                while (numEntries != 0) {
                                                        pRecvStreamMsg = (dspipc_recvStreamMessage_t*)pChipset->pReadMsg.recvStream;
                                                        memset((void*)pRecvStreamMsg, 0, sizeof(dspipc_recvStreamMessage_t));
                                                        if (pRecvStreamMsg != NULL) {
                                                                pRecvStreamMsg->hdr.flags = 0;
                                                                if (pStreamMsg->bufferFlags & SMF_TRANSFER_BUFFER_RET) {
                                                                        pRecvStreamMsg->hdr.flags |= (uint32_t)STREAMFLAG_SEND_DATA;
#ifdef MARKUS
                                                                        hb_slogf_error("%s: Recv STR-Msg:BC STREAMFLAG_SEND_DATA: %s",
                                                                            "PCmdStrMes", pSubDevice->pSubDeviceName);
#endif
                                                                }
                                                                if (pStreamMsg->bufferFlags & SMF_PROVIDE_BUFFER_RET) {
                                                                        pRecvStreamMsg->hdr.flags |= (uint32_t)STREAMFLAG_PROVIDE_BUFFER;
                                                                        *newScan = 1; /* if new Provide Buffer then new scan !! */
                                                                }
                                                                if (pStreamMsg->bufferFlags & SMF_DATA_DISCONTINIOUS) {
                                                                        pRecvStreamMsg->hdr.flags |= (uint32_t)STREAMFLAG_DISCONTINUITY;
                                                                }
                                                                if (pStreamMsg->bufferFlags & SMF_DRAINED) {
                                                                        pRecvStreamMsg->hdr.flags |= (uint32_t)STREAMFLAG_DRAINED;
                                                                }
                                                                if (numEntries > NUM_SCAT_GATH_ENTRIES) {
                                                                        pRecvStreamMsg->hdr.numEntries = NUM_SCAT_GATH_ENTRIES;
                                                                        numEntries -= NUM_SCAT_GATH_ENTRIES;
                                                                } else {
                                                                        pRecvStreamMsg->hdr.numEntries = numEntries;
                                                                        numEntries = 0;
                                                                }
                                                                pRecvStreamMsg->hdr.cmdId = pStreamMsg->cmdId;

                                                                pMemObj = NULL;
                                                                for (i = 0; i < pRecvStreamMsg->hdr.numEntries; i++) {
                                                                        memId = 0;
                                                                        memOffset = 0;
                                                                        memSearchType = MEMOBJ_SEARCH_PHYS;
                                                                        pMemObj = SearchMemId(pMemObj, memSearchType,
                                                                            pStreamMsg->ranges[i].physicalAddress,
                                                                            pStreamMsg->ranges[i].dataSize,
                                                                            &memId, &memOffset);
                                                                        if (pMemObj != NULL) {
                                                                                /* range found */
                                                                                pRecvStreamMsg->entry[i].memId = memId;
                                                                                pRecvStreamMsg->entry[i].mem.pMemHdl = (void*)pMemObj;
                                                                                pRecvStreamMsg->entry[i].mem.physAddr = pMemObj->physAddr;
                                                                                pRecvStreamMsg->entry[i].mem.pVirtAddr = pMemObj->pVirtAddr;
                                                                                pRecvStreamMsg->entry[i].start = memOffset;
                                                                                pRecvStreamMsg->entry[i].size = pStreamMsg->ranges[i].dataSize;
                                                                                pRecvStreamMsg->entry[i].remain = 0;
                                                                                pRecvStreamMsg->entry[i].valid = pStreamMsg->ranges[i].dataSize;
#ifdef VERBOSE_ENABLE
                                                                                if (pChipset->verbose > 3) {
                                                                                        if (pStreamMsg->bufferFlags & SMF_PROVIDE_BUFFER_RET) {
#ifdef DUMP_DATA
                                                                                                dra6xxDumpData("(DATA RECEIVE)", (void*)(((char*)(pMemObj->pVirtAddr) + memOffset)), pStreamMsg->ranges[i].dataSize);
#endif
                                                                                        }
                                                                                }
#endif
                                                                        } else {
                                                                                hb_slogf_error("%s: Phys addr (0x%08X) with size 0x%08X not found",
                                                                                    __FUNCTION__,
                                                                                    pStreamMsg->ranges[i].physicalAddress,
                                                                                    pStreamMsg->ranges[i].dataSize);
                                                                                error = 1;
                                                                        }
                                                                }

                                                                pMessage.recvStream = pRecvStreamMsg;
                                                                if (dspipcQueueAddMessage(pDevice,
                                                                        RECEIVE_CALLED,
                                                                        pSubDevice->pRecvMsgQueue,
                                                                        DSPIPC_CONT_BUFCOMPLETE,
                                                                        pMsg->msgId,
                                                                        pStreamMsg->cmdId,
                                                                        /* get the next paramters out of the message */
                                                                        DSPIPC_MSGTYPE_STREAM,
                                                                        DSPIPC_MSGREPLY_NO,
                                                                        pMessage,
                                                                        sizeof(dspipc_recvStreamMessage_t))
                                                                    != 0) {
                                                                        hb_slogf_error("%s: Add STREAM msg msgid %d cmdid %d to queue (%s) failed",
                                                                            __FUNCTION__, pMsg->msgId, pStreamMsg->cmdId, pSubDevice->pSubDeviceName);
                                                                        error = 1;
                                                                }
                                                        } else {
                                                                hb_slogf_error(NO_MEMORY, __FUNCTION__);
                                                                error = 1;
                                                        }
                                                } /* while remaining ranges */
                                                if (error == 0) {
                                                        retValue = 0;
                                                }
                                                break;
                                        default:
                                                hb_slogf_error("%s: Unsupported stream command", __FUNCTION__);
                                                break;
                                        }
                                } else {
                                        hb_slogf_error("%s: Wrong size received:%d expected:%d",
                                            __FUNCTION__, pMsg->msgSize, sizeof(DSPIPCStreamMsg));
                                }
                        } else {
                                hb_slogf_error("%s: Received unsupproted stream class", __FUNCTION__);
                        }
                        break;
                default:
                        hb_slogf_error("%s: Received undefined class", __FUNCTION__);
                        break;
                }

        } else {
                hb_slogf_error("%s: Subdevice not found", __FUNCTION__);
        }

        return (retValue);
}

/**
 *  This function process a new incoming internal administration message. This
 *  message is separated into the basic administration messages.
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param pMsg Pointer to the new incoming message
 *  @return int Error code, 0=ok, -1=failure
 */
static int ProcessInternalAdmin(dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg)
{
        int retValue = -1;
        uint32_t count;
        int matched;
        subDeviceTable_t* pSubDevice;
        DSPIPCAdminMsg* pAdminMsg;
        DSPIPCVersionMsg* pVersion;
        DSPIPCChannelInformation* pChannelInfo;
        DSPIPCErrorInformation* pErrorInfo;
        DSPIPCDrainInformation* pDrainInfo = NULL;

        pAdminMsg = &(pMsg->payload.adminMsg);
        if ((pChipset->configState == DRA6XX_VERSION_MATCH) || (pAdminMsg->generic.cmdType == DCCMID_VERSION)) {
                switch (pAdminMsg->generic.cmdType) {
                case DCCMID_INVALID:
                        hb_slogf_error("%s: Invalid command from %s received", __FUNCTION__, pChipset->pDevice->pDeviceName);
                        retValue = 0;
                        break;
                case DCCMID_VERSION:
                        hb_slogf_error("%s: ######### %s image version received #########", __FUNCTION__, pChipset->pDevice->pDeviceName);
                        pVersion = &(pAdminMsg->content.version);
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 0) {
                                hb_slogf_error("%s: Version from DSP received <%d.%d.%d>", __FUNCTION__,
                                    pVersion->swMajor, pVersion->swMinor, pVersion->swBugfix);
                        }
#endif
                        retValue = 0;
                        matched = 0;
                        if (pVersion->swMajor == DSP_INTERFACE_VERSION_MAJOR) {
                                if (pVersion->swMinor == DSP_INTERFACE_VERSION_MINOR) {
#ifdef VERBOSE_ENABLE
                                        if (pChipset->verbose > 0) {
                                                hb_slogf_error("%s: Version matched", __FUNCTION__);
                                        }
#endif
                                        matched = 1;
                                }
                                if (pVersion->swMinor > DSP_INTERFACE_VERSION_MINOR) {
                                        hb_slogf_error("%s: Major version matched, minor mismatch (Driver:%d DSP:%d), please update driver",
                                            __FUNCTION__, DSP_INTERFACE_VERSION_MINOR, pVersion->swMinor);
                                        matched = 1;
                                }
                        }

                        if (matched == 0) {
                                hb_slogf_error("%s: Version mismatch (Driver:%d.%d.%d DSP:%d.%d.%d)",
                                    __FUNCTION__,
                                    DSP_INTERFACE_VERSION_MAJOR,
                                    DSP_INTERFACE_VERSION_MINOR,
                                    DSP_INTERFACE_VERSION_BUGFIX,
                                    pVersion->swMajor, pVersion->swMinor, pVersion->swBugfix);
                                if (dra6xxSetConfigState(pChipset, DRA6XX_VERSION_MISMATCH) != EOK) {
                                        hb_slogf_error("%s: failed to call dra6xxSetConfigState", __FUNCTION__);
                                }
                        } else {
                                if (dra6xxSetConfigState(pChipset, DRA6XX_VERSION_MATCH) != EOK) {
                                        hb_slogf_error("%s: failed to call dra6xxSetConfigState", __FUNCTION__);
                                }
                        }
#ifdef DUMP_DATA
                        dra6xxDumpData("(DSP VERSION)", (void*)((char*)(&(pAdminMsg->content.version))), sizeof(struct _DSPIPCVersionMsg));
#endif
                        if ((pVersion->preSign == 0xbeefbeef) && (pVersion->postSign == 0xdeaddead)) {
                                if ((pVersion->dspVersionInfoARR[149] == '\0') && (pVersion->dspVersionInfoARR[0] != '\0')) {
                                        memcpy(&DSP_IMAGE_VERSION_INFO[0], &(pVersion->dspVersionInfoARR[0]), 150);
                                        hb_slogf_error("%s: %s Image Version Info: %s", __FUNCTION__, pChipset->pDevice->pDeviceName, &(pVersion->dspVersionInfoARR[0]));
                                } else {
                                        hb_slogf_error("%s: unterminated %s Image Version Info found", __FUNCTION__, pChipset->pDevice->pDeviceName);
                                }
                        } else {
                                hb_slogf_error("%s: No %s Image Version Info available", __FUNCTION__, pChipset->pDevice->pDeviceName);
                        }
                        break;

                case DCCMID_ERROR:
                        pErrorInfo = &(pAdminMsg->content.errorInformation);
                        hb_slogf_error("%s: Error Information from %s received msg id <%X> errorCode <%d>",
                            __FUNCTION__,
                            pChipset->pDevice->pDeviceName,
                            pErrorInfo->msgId,
                            pErrorInfo->errorCode);
                        retValue = 0;
                        break;
                case DCCMID_ADDCHANNELS:
                        pChannelInfo = &(pAdminMsg->content.channelInfo);
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 0) {
                                hb_slogf_error("%s: Add %d channels from %s received num = %d type = %X", __FUNCTION__,
                                    pChannelInfo->numChannels,
                                    pChipset->pDevice->pDeviceName,
                                    pChannelInfo->channel[0].channelNum,
                                    pChannelInfo->channel[0].channelType);
                        }
#endif
                        if (pChannelInfo->numChannels > DSPIPC_MSG_MAXCHANNELS) {
                                hb_slogf_error("%s: Invalid number of channels %d > %d", __FUNCTION__,
                                    pChannelInfo->numChannels, DSPIPC_MSG_MAXCHANNELS);
                        }

                        if (pChannelInfo->numChannels <= DSPIPC_MSG_MAXCHANNELS) {
                                count = 0;
                                while ((count < pChannelInfo->numChannels) && (count < DSPIPC_MSG_MAXCHANNELS)) {
                                        switch (pChannelInfo->channel[count].channelType & DSPIPC_CHANNELTYPE_MSK) {
                                        case DSPIPC_CHANNELTYPE_COMMAND:
                                                retValue = 0;
                                                /* create command channels */
                                                if (dspipcAddSubDevice(pChipset->pDevice,
                                                        S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH,
                                                        NULL, SD_COMMAND, SD_DIR_IN,
                                                        pChannelInfo->channel[count].channelNum,
                                                        pChannelInfo->channel[count].channelPriority)
                                                    == NULL) {
                                                        hb_slogf_error(ADD_SUBDEVICE_FAILED, __FUNCTION__);
                                                        retValue = -1;
                                                }
                                                if (dspipcAddSubDevice(pChipset->pDevice,
                                                        S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH,
                                                        NULL, SD_COMMAND, SD_DIR_OUT,
                                                        pChannelInfo->channel[count].channelNum,
                                                        pChannelInfo->channel[count].channelPriority)
                                                    == NULL) {
                                                        hb_slogf_error(ADD_SUBDEVICE_FAILED, __FUNCTION__);
                                                        retValue = -1;
                                                }
                                                break;
                                        case DSPIPC_CHANNELTYPE_STREAM:
                                                retValue = 0;
                                                /* create stream channels */
                                                if (dspipcAddSubDevice(pChipset->pDevice,
                                                        S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH,
                                                        NULL, SD_STREAM, SD_DIR_IN,
                                                        pChannelInfo->channel[count].channelNum,
                                                        pChannelInfo->channel[count].channelPriority)
                                                    == NULL) {
                                                        hb_slogf_error(ADD_SUBDEVICE_FAILED, __FUNCTION__);
                                                        retValue = -1;
                                                }
                                                if (dspipcAddSubDevice(pChipset->pDevice,
                                                        S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH,
                                                        NULL, SD_STREAM, SD_DIR_OUT,
                                                        pChannelInfo->channel[count].channelNum,
                                                        pChannelInfo->channel[count].channelPriority)
                                                    == NULL) {
                                                        hb_slogf_error(ADD_SUBDEVICE_FAILED, __FUNCTION__);
                                                        retValue = -1;
                                                }
                                                break;
                                        default:
                                                hb_slogf_error("%s: Unsupported internal message", __FUNCTION__);
                                                break;
                                        }
                                        count++;
                                }
                        } else {
                                hb_slogf_error("%s: Channelcount out of bounds", __FUNCTION__);
                        }
                        break;

                case DCCMID_CHANGECHANNELPRIO:
                        pChannelInfo = &(pAdminMsg->content.channelInfo);
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 0) {
                                hb_slogf_error("%s: Change channel priority message from %s received. Channel:%d type=%X prio:%d",
                                    __FUNCTION__,
                                    pChipset->pDevice->pDeviceName,
                                    pChannelInfo->channel[0].channelNum,
                                    pChannelInfo->channel[0].channelType,
                                    pChannelInfo->channel[0].channelPriority);
                        }
#endif
                        if (pChannelInfo->numChannels > DSPIPC_MSG_MAXCHANNELS) {
                                hb_slogf_error("%s: Invalid number of channels %d > %d", __FUNCTION__,
                                    pChannelInfo->numChannels, DSPIPC_MSG_MAXCHANNELS);
                        }

                        if (pChannelInfo->numChannels <= DSPIPC_MSG_MAXCHANNELS) {
                                count = 0;
                                while ((count < pChannelInfo->numChannels) && (count < DSPIPC_MSG_MAXCHANNELS)) {
                                        if (dspipcChangeSubDevicePrio(pChipset->pDevice,
                                                pChannelInfo->channel[count].channelNum,
                                                pChannelInfo->channel[count].channelPriority)
                                            != 0) {
                                                hb_slogf_error("%s: change of channel priority failed", __FUNCTION__);
                                                retValue = -1;
                                        }
                                }
                        } else {
                                hb_slogf_error("%s: Channelcount out of bounds", __FUNCTION__);
                                retValue = -1;
                        }
                        break;

                case DCCMID_REMOVECHANNELS:
                        pChannelInfo = &(pAdminMsg->content.channelInfo);
#ifdef VERBOSE_ENABLE
                        if (pChipset->verbose > 0) {
                                hb_slogf_error("%s: Remove %d channels from %s received num = %d type = %X", __FUNCTION__,
                                    pChannelInfo->numChannels,
                                    pChipset->pDevice->pDeviceName,
                                    pChannelInfo->channel[0].channelNum,
                                    pChannelInfo->channel[0].channelType);
                        }
#endif
                        if (pChannelInfo->numChannels <= DSPIPC_MSG_MAXCHANNELS) {
                                count = 0;
                                while (count < pChannelInfo->numChannels) {
                                        switch (pChannelInfo->channel[count].channelType & DSPIPC_CHANNELTYPE_MSK) {
                                        case DSPIPC_CHANNELTYPE_COMMAND:
                                        case DSPIPC_CHANNELTYPE_STREAM:
                                                /* remove command or stream channels */
                                                retValue = 0;
                                                pSubDevice = dspipcSearchSubDevice(pChipset->pDevice, SD_DIR_IN,
                                                    pChannelInfo->channel[count].channelNum);
                                                if (pSubDevice != NULL) {
                                                        if (dspipcRemSubDevice(pChipset->pDevice, pSubDevice) != 0) {
                                                                hb_slogf_error(REM_SUBDEVICE_FAILED, __FUNCTION__);
                                                                retValue = -1;
                                                        }
                                                } else {
                                                        hb_slogf_error("%s: IN sub device not found", __FUNCTION__);
                                                        retValue = -1;
                                                }
                                                pSubDevice = dspipcSearchSubDevice(pChipset->pDevice, SD_DIR_OUT,
                                                    pChannelInfo->channel[count].channelNum);
                                                if (pSubDevice != NULL) {
                                                        if (dspipcRemSubDevice(pChipset->pDevice, pSubDevice) != 0) {
                                                                hb_slogf_error(REM_SUBDEVICE_FAILED, __FUNCTION__);
                                                                retValue = -1;
                                                        }
                                                } else {
                                                        hb_slogf_error("%s: OUT sub device not found", __FUNCTION__);
                                                        retValue = -1;
                                                }
                                                break;
                                        default:
                                                hb_slogf_error("%s: Unsupported internal message", __FUNCTION__);
                                                break;
                                        }
                                        count++;
                                }
                        } else {
                                hb_slogf_error("%s: Channelcount out of bounds", __FUNCTION__);
                        }
                        break;
                case DCCMID_DRAINACK:
                        pDrainInfo = &(pAdminMsg->content.drainInfo);
                        pSubDevice = dspipcSearchSubDevice(pChipset->pDevice, SD_DIR_OUT,
                            pDrainInfo->chanId);
                        if (pSubDevice != NULL) {
                                if (dra6xxDrain(pSubDevice) == 0) {
                                        retValue = 0;
                                } else {
                                        hb_slogf_error("%s: Drain on %s failed", __FUNCTION__, pSubDevice->pSubDeviceName);
                                }
                        } else {
                                hb_slogf_error("%s: No SubDevice of drain acknowledge MSG found",
                                    __FUNCTION__);
                        }
                        break;
                default:
                        hb_slogf_error("%s: Unsupported internal message cmd %d", __FUNCTION__, pAdminMsg->generic.cmdType);
                        break;
                }
        } else {
                hb_slogf_error("%s: Skip message due to non matching version", __FUNCTION__);
        }

        return (retValue);
}

/**
 *  Drain function to handle a drain sequence to the DSP
 *
 *  @param pSubDevice Pointer to the sub device structure
 *  @return int 0 if everything was ok, -1 if an error has occurred
 */
static int dra6xxDrain(subDeviceTable_t* pSubDevice)
{
        deviceTable_t* pDevice;
        subDeviceTable_t* pSubDeviceIn;
        dspIpcChipset_t* pChipset;
        int retValue = -1;
        int errCode;
        int numElements;
        uint32_t queueMsgSize;
        uint32_t queueMsgId;
        uint32_t queueCmdId;
        uint32_t queueNumLost;
        dspipc_msgType_t queueMsgType;
        dspipc_msgReply_t queueMsgReply;
        messagePointer_t pQueueMsg;
        messagePointer_t pMessage;
        dspipc_msgContent_t contentType;
        unsigned int currEntry;
        int currElement;
        dspipc_recvStreamMessage_t streamMessage;

        if (pSubDevice == NULL) {
                hb_slogf_error(NO_SUBDEVICE, __FUNCTION__);
                return (-1);
        }

        /* check if channel number fit into the mask */
        if ((pSubDevice->channelNum & (DSPIPC_CHANNELNUMBER_MSK >> DSPIPC_CHANNELNUMBER_SHIFT)) != pSubDevice->channelNum) {
                hb_slogf_error("%s: channelnumber %d does not fit into protocoll",
                    __FUNCTION__, pSubDevice->channelNum);
                return (-1);
        }

        pDevice = pSubDevice->pDevice;
        if (pDevice == NULL) {
                hb_slogf_error(NO_DEVICE, __FUNCTION__);
                return (-1);
        }

        pChipset = (dspIpcChipset_t*)pDevice->pChipset;
        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        retValue = 0;
        if (dspipcLockSubDevices(pDevice) == EOK) {
                /* Get Info of corresponding IN subdevice (doesn't have to be done in every for loop) */
                pSubDeviceIn = dspipcSearchSubDevice(pSubDevice->pDevice,
                    SD_DIR_IN,
                    pSubDevice->channelNum);

                /* cleanup message queue and respond the data  */
                /* first: No action on transfer and/or provide queues, not used on C64x */
                /* second: handle all remaining elements inside the queue */
                if (dspipcQueueNumAvail(pSubDevice->pSendMsgQueue, &numElements) == 0) {
                        for (currElement = 0; currElement < numElements; currElement++) {
                                pQueueMsg.generic = pSubDevice->pMsg.generic;
                                contentType = DSPIPC_CONT_ANY;
                                errCode = dspipcQueueGetMessage(pSubDevice->pSendMsgQueue,
                                    &contentType,
                                    &queueMsgId,
                                    &queueCmdId,
                                    &queueMsgType,
                                    &queueMsgReply,
                                    &pQueueMsg,
                                    &queueMsgSize,
                                    &queueNumLost,
                                    NULL);

                                if (errCode == 0) {
                                        if (pQueueMsg.generic != NULL) {
                                                switch (queueMsgType) {
                                                case DSPIPC_MSGTYPE_STREAM:
                                                        switch (contentType) {
                                                        case DSPIPC_CONT_SENDSTREAM:
                                                                /*pMessage.recvStream = (dspipc_recvStreamMessage_t*) MALLOC_TRC(sizeof(dspipc_recvStreamMessage_t)); */
                                                                pMessage.recvStream = &streamMessage;
                                                                if (pMessage.recvStream != NULL) {
                                                                        if (pSubDeviceIn != NULL) {
                                                                                /* copy the ranges */
                                                                                for (currEntry = 0; currEntry < pQueueMsg.sendStream->hdr.numEntries; currEntry++) {
                                                                                        dspipc_scatGat_t* pEntryRecv = &(pMessage.recvStream->entry[currEntry]);
                                                                                        dspipc_scatGat_t* pEntrySend = &(pQueueMsg.sendStream->entry[currEntry]);
                                                                                        memcpy(pEntryRecv, pEntrySend, sizeof(dspipc_scatGat_t));
                                                                                        pEntryRecv->valid = pEntrySend->size - pEntrySend->remain;
                                                                                }
                                                                                pMessage.recvStream->hdr.numEntries = pQueueMsg.sendStream->hdr.numEntries;
                                                                                pMessage.recvStream->hdr.flags = (uint32_t)((uint32_t)STREAMFLAG_SEND_DATA | (uint32_t)STREAMFLAG_DRAINED);

                                                                                /* put the buffer complete to the message queue */
                                                                                if (dspipcQueueAddMessage(pSubDeviceIn->pDevice,
                                                                                        RECEIVE_CALLED,
                                                                                        pSubDeviceIn->pRecvMsgQueue,
                                                                                        DSPIPC_CONT_BUFCOMPLETE,
                                                                                        queueMsgId,
                                                                                        queueCmdId,
                                                                                        /* get the next paramters out of the message */
                                                                                        DSPIPC_MSGTYPE_STREAM,
                                                                                        DSPIPC_MSGREPLY_NO,
                                                                                        pMessage,
                                                                                        sizeof(dspipc_recvStreamMessage_t))
                                                                                    != 0) {
                                                                                        hb_slogf_error("%s: Add msg msgid %d cmdid %d to queue (%s) failed",
                                                                                            __FUNCTION__, queueMsgId, queueCmdId, pSubDevice->pSubDeviceName);
                                                                                        pMessage.recvStream = NULL;
                                                                                        retValue = -1;
                                                                                }
                                                                        } else {
                                                                                hb_slogf_error(CANNOT_FIND_CORRESPONDING_INPUT_DEVICE,
                                                                                    __FUNCTION__);
                                                                                pQueueMsg.sendStream = NULL;
                                                                                retValue = -1;
                                                                        }
                                                                } else {
                                                                        hb_slogf_error("%s: No memory for buffer complete message",
                                                                            __FUNCTION__);
                                                                        retValue = -1;
                                                                }
                                                                break;
                                                        case DSPIPC_CONT_PROVIDEBUFSTREAM:
                                                                break;
                                                        case DSPIPC_CONT_BUFCOMPLETE:
                                                                /* Do nothing, buffer complete is skipped */
                                                                break;
                                                        default:
                                                                retValue = -1;
                                                                hb_slogf_error(UNHANDLED_CONTENT_TYPE,
                                                                    __FUNCTION__);
                                                                break;
                                                        }
                                                        break;
                                                case DSPIPC_MSGTYPE_COMMAND:
                                                        /* Do nothing, commands are skipped */
                                                        break;
                                                default:
                                                        retValue = -1;
                                                        hb_slogf_error(GET_INVALID_MESSAGE_TYPE_OUT_OF_QUEUE,
                                                            __FUNCTION__);
                                                        break;
                                                }
                                        } else {
                                                hb_slogf_error(GET_NULL_PTR_FOR_MSG,
                                                    __FUNCTION__);
                                                retValue = -1;
                                        }
                                } else {
                                        hb_slogf_error(GET_MESSAGE_FROM_QUEUE_FAILED,
                                            __FUNCTION__, errCode);
                                        retValue = -1;
                                }
                        } /* for all messages */
                } else {
                        hb_slogf_error("%s: Get number of messages in queue failed",
                            __FUNCTION__);
                        retValue = -1;
                }

                /* Cleanup pending recieve MSG on In device */
                /* This is not needed for C64x, bacause the DSP is handling these Messages */
                if (dspipcUnlockSubDevices(pDevice) != EOK) {
                        hb_slogf_error(UNLOCK_SUBDEVICE_FAILED, __FUNCTION__);
                        retValue = -1;
                }
                if (dspipcDrainComplete(pSubDevice) != 0) {
                        hb_slogf_error("%s: Drain complete failed",
                            __FUNCTION__);
                        retValue = -1;
                }
        } else {
                hb_slogf_error(LOCK_SUBDEVICE_FAILED, __FUNCTION__);
                retValue = -1;
        }

        return (retValue);
}

/**
 *  This function process a new incoming internal print message.
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param pMsg Pointer to the new incoming message
 *  @return int Error code, 0=ok, -1=failure
 */
static int ProcessInternalPrint(const dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg)
{
        int retValue = -1;
        char* pPrintString;
        uint32_t pStringLen;

        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        if (pMsg == NULL) {
                hb_slogf_error("%s: No message", __FUNCTION__);
                return (-1);
        }

        pPrintString = (char*)&(pMsg->payload.message[0]);
        pStringLen = pMsg->msgSize;
        if (pStringLen <= COMMANDMSG_SIZE) {
                if (pMsg->payload.message[pStringLen - 1] != '\0') {
                        pMsg->payload.message[COMMANDMSG_SIZE - 1] = '\0'; /*terminate string */
                }
                hb_slogf_error("%s: DSP: %s", __FUNCTION__, pPrintString);
        } else {
                hb_slogf_error("%s: message size %d out of bounds (max. %d)",
                    __FUNCTION__, pStringLen, COMMANDMSG_SIZE);
        }

        retValue = 0;

        return (retValue);
}

/**
 *  This function process a new incoming internal log message.
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @param pMsg Pointer to the new incoming message
 *  @return int Error code, 0=ok, -1=failure
 */
static int ProcessInternalLog(const dspIpcChipset_t* pChipset, DSPIPCMessageQueueEntry* pMsg)
{
        int retValue = -1;
        char* pPrintString;
        uint32_t pStringLen;

        if (pChipset == NULL) {
                hb_slogf_error(NO_CHIPSET, __FUNCTION__);
                return (-1);
        }

        if (pMsg == NULL) {
                hb_slogf_error("%s: No message", __FUNCTION__);
                return (-1);
        }

        pPrintString = (char*)&(pMsg->payload.message[0]);
        pStringLen = pMsg->msgSize;
        if (pStringLen <= COMMANDMSG_SIZE) {
                if (pMsg->payload.message[pStringLen - 1] == '\0') {
                        hb_slogf_error("DSP: <%s>", pPrintString);
                } else {
                        hb_slogf_error("%s: print message unterminated", __FUNCTION__);
                        pMsg->payload.message[COMMANDMSG_SIZE - 1] = '\0'; /*terminate string */
                        hb_slogf_error("%s: print message recovered <%s>", __FUNCTION__, pPrintString);
                }
        } else {
                hb_slogf_error("%s: message size %d out of bounds (max. %d)",
                    __FUNCTION__, pStringLen, COMMANDMSG_SIZE);
        }

        retValue = 0;

        return (retValue);
}

/**
 *  This function print the status of mailbox and queue status.
 *
 *  @param pChipset Pointer to the dra6xx chipset layer administration structure
 *  @return int Error code, 0=ok, -1=failure
 */
static int dra6xx_mailbox_queue_status(dspIpcChipset_t* pChipset)
{

        uint32_t dspRxQueueAdr;
        int32_t dspRxQueueWriteIndex;
        int32_t dspRxQueueReadIndex;
        uint32_t dspTxQueueAdr;
        int32_t dspTxQueueWriteIndex;
        int32_t dspTxQueueReadIndex;
        int32_t numElements;
        uint32_t mb_int, mb_irq, mb_num, mb_msg, mb_fifo;
        DSPIPCMessageQueueEntry msg;

        /*get current state */
        hb_slogf_error("%s:driver to %s current state:%d, driver not ready times:%d", __FUNCTION__, pChipset->pDevice->pDeviceName, pChipset->configState, notReadyTimes);
        /*get mailbox status */
        if (strcmp(pChipset->pDevice->pDeviceName, "dsp") == 0) {
                /*get mailbox interrupt status/mailbox num/mailbox message which is used from DSP to Acore */
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQENABLE_SET_REG(0), &mb_int);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(0), &mb_irq);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(0), &mb_fifo);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MSGSTATUS(0), &mb_num);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MESSAGE(0), &mb_msg);
                hb_slogf_error("%s:mlb DSP-->Acore:MAILBOX_IRQENABLE_SET:0x%x,MAILBOX_IRQSTATUS_CLR:0x%x,MAILBOX_FIFOSTATUS:0x%x,MAILBOX_MSGSTATUS:0x%x,MAILBOX_MESSAGE:0x%x",
                    __FUNCTION__, mb_int, mb_irq, mb_fifo, mb_num, mb_msg);

                /*get mailbox interrupt status/mailbox num/mailbox message which is used from Acore to DSP */
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQENABLE_SET_REG(1), &mb_int);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(1), &mb_irq);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(1), &mb_fifo);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MSGSTATUS(1), &mb_num);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MESSAGE(1), &mb_msg);
                hb_slogf_error("%s:mlb Acore-->DSP:MAILBOX_IRQENABLE_SET:0x%x,MAILBOX_IRQSTATUS_CLR:0x%x,MAILBOX_FIFOSTATUS:0x%x,MAILBOX_MSGSTATUS:0x%x,MAILBOX_MESSAGE:0x%x",
                    __FUNCTION__, mb_int, mb_irq, mb_fifo, mb_num, mb_msg);
        } else if (strcmp(pChipset->pDevice->pDeviceName, "m3") == 0) {
                /*get mailbox interrupt status/mailbox num/mailbox message which is used from DSP to Acore */
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQENABLE_SET_REG(0), &mb_int);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(0), &mb_irq);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(4), &mb_fifo);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MSGSTATUS(4), &mb_num);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MESSAGE(4), &mb_msg);
                hb_slogf_error("%s:mlb Mcore-->Acore:MAILBOX_IRQENABLE_SET:0x%x,MAILBOX_IRQSTATUS_CLR:0x%x,MAILBOX_FIFOSTATUS:0x%x,MAILBOX_MSGSTATUS:0x%x,MAILBOX_MESSAGE:0x%x",
                    __FUNCTION__, mb_int, mb_irq, mb_fifo, mb_num, mb_msg);

                /*get mailbox interrupt status/mailbox num/mailbox message which is used from Acore to DSP */
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQENABLE_SET_REG(2), &mb_int);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_IRQSTATUS_CLR_REG(2), &mb_irq);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_FIFOSTATUS(5), &mb_fifo);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MSGSTATUS(5), &mb_num);
                dra6xxRegRead32(pChipset, MOD_DEVCONF_REG, MAILBOX_MESSAGE(5), &mb_msg);
                hb_slogf_error("%s:mlb Acore-->Mcore:MAILBOX_IRQENABLE_SET:0x%x,MAILBOX_IRQSTATUS_CLR:0x%x,MAILBOX_FIFOSTATUS:0x%x,MAILBOX_MSGSTATUS:0x%x,MAILBOX_MESSAGE:0x%x",
                    __FUNCTION__, mb_int, mb_irq, mb_fifo, mb_num, mb_msg);
        }

        /*get DSP/Mcore Rx queue status */
        dspRxQueueAdr = dra6xx_get_queue_index(pChipset,
            ADMIN_OFFSET(dspRxBufferAddr),
            &dspRxQueueReadIndex,
            &dspRxQueueWriteIndex);
        numElements = dspRxQueueWriteIndex - dspRxQueueReadIndex;
        hb_slogf_error("%s:%s-RX index: read=%d, write=%d, addr=0x%08x,numElements:%d", __FUNCTION__, pChipset->pDevice->pDeviceName, dspRxQueueReadIndex, dspRxQueueWriteIndex, dspRxQueueAdr, numElements);

        if ((void*)dspRxQueueAdr == NULL) {
                hb_slogf_error("%s: dspRxQueueAdr is null, %s was not booting up.", __FUNCTION__, pChipset->pDevice->pDeviceName);
                return (-1);
        }

        /*force to dump the whole message in communication queue */
        dspRxQueueReadIndex = 0;
        numElements = COMMANDQUEUE_LENGTH;

        while (numElements > 0) {
                /*get the queue content */
                if (dra6xxAccessMem32(pChipset,
                        dspRxQueueAdr + offsetof(struct _DSPIPCMessageQueue, msg) + (((uint32_t)dspRxQueueReadIndex) % COMMANDQUEUE_LENGTH) * sizeof(DSPIPCMessageQueueEntry),
                        sizeof(DSPIPCMessageQueueEntry) >> 2, MEM_READ, (uint32_t*)&msg)
                    != 0) {
                        hb_slogf_error("%s: Access failed", __FUNCTION__);
                }
                /*dump message */
                dra6xxDumpMessage("send", &msg);
                dspRxQueueReadIndex++;
                numElements--;
        }

        /*get DSP/Mcore Tx queue status */
        dspTxQueueAdr = dra6xx_get_queue_index(pChipset,
            ADMIN_OFFSET(dspTxBufferAddr),
            &dspTxQueueReadIndex,
            &dspTxQueueWriteIndex);
        numElements = dspTxQueueWriteIndex - dspTxQueueReadIndex;
        hb_slogf_error("%s:%s-TX index: read=%d, write=%d, addr=0x%08x,numElements:%d", __FUNCTION__, pChipset->pDevice->pDeviceName, dspTxQueueReadIndex, dspTxQueueWriteIndex, dspTxQueueAdr, numElements);

        if ((void*)dspTxQueueAdr == NULL) {
                hb_slogf_error("%s: dspTxQueueAdr is null, %s was not booting up.", __FUNCTION__, pChipset->pDevice->pDeviceName);
                return (-1);
        }

        /*force to dump the whole message in communication queue */
        dspTxQueueReadIndex = 0;
        numElements = COMMANDQUEUE_LENGTH;

        while (numElements > 0) {
                /*get the queue content */
                if (dra6xxAccessMem32(pChipset,
                        dspTxQueueAdr + offsetof(struct _DSPIPCMessageQueue, msg) + (((uint32_t)dspTxQueueReadIndex) % COMMANDQUEUE_LENGTH) * sizeof(DSPIPCMessageQueueEntry),
                        sizeof(DSPIPCMessageQueueEntry) >> 2, MEM_READ, (uint32_t*)&msg)
                    != 0) {
                        hb_slogf_error("%s: Access failed", __FUNCTION__);
                }
                /*dump message */
                dra6xxDumpMessage("received", &msg);
                dspTxQueueReadIndex++;
                numElements--;
        }

        return 0;
}

/*===========================   End Of File   ================================= */
