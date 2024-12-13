/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file dspipc-dra7xx.h
 *
 * This is the header file for the DRA7XX ARM dsp-ipc-chipset layer.
 *
 * @author Andreas Abel
 * @date   25.10.2010
 *
 * Copyright (c) 2010 Harman/Becker Automotive Systems GmbH
 */

#ifndef _DSPIPC_DRA7XX_H_INCLUDED_
#define _DSPIPC_DRA7XX_H_INCLUDED_

#include "../dev-dspipc.h"
#include <linux/types.h>

#define DRA7XX_READ_DMA_READY CL_USER_1

/**
 * @brief The root pointer address
 */
#define DRA7XX_ROOTPOINTER_ADDR (0xC2800000) /* 0x647FFFFF  800000   x86800000 */

#define DRA7XX_READ_DMA_READY CL_USER_1

/** 
 * Define the different mailbox states
 * @enum DRA7XX_CONFIG_STATE_E
 */
enum DRA7XX_CONFIG_STATE_E {
        DRA7XX_NOT_INITIALIZED, /* Configuration state is uninitialized */
        DRA7XX_STARTED,         /* DSP application is started */
        DRA7XX_VERSION_MATCH,   /* Matching version message has arrived */
        DRA7XX_VERSION_MISMATCH /* Unsupported version number received */
};

/** 
 *  Create a typedef to use the DRA7XX_CONFIG_STATE_E enumeration 
 * @typedef dra7xxConfigState_t
 */
typedef enum DRA7XX_CONFIG_STATE_E dra7xxConfigState_t;

/** 
 * Define the indicators for specifying if interrupt has been signalled
 * immediately or delayed
 * @enum DRA7XX_SEND_INDICATOR_E
 */
enum DRA7XX_SEND_INDICATOR_E {
        DRA7XX_SEND_NO_INT_TO_DSP, /* Put message to queue without setting interrupt to DSP */
        DRA7XX_SEND_DO_INT_TO_DSP  /* Put message to queue with immediate interrupt signaling to DSP */
};

/** 
 *  Create a typedef to use the DRA7XX_SEND_INDICATOR_E enumeration 
 * @typedef dra7xxSendIndicator_t
 */
typedef enum DRA7XX_SEND_INDICATOR_E dra7xxSendIndicator_t;

#define MEMORY_PAGE_MASK (0xFF800000U) /* 8M 0x00800000U */
#define MEMORY_PAGE_SHIFT (23)
#define ADDR_CECK_PAGE_MASK (0x007FFFFFU)
#define ADDR_CECK_PAGE_MASK_DMA (0x007FFFFCU)

/**
 *  chipsetlayer specific administration structure
 * @struct DSPIPCDRA7XX_S
 */
struct DSPIPCDRA7XX_S {
        void* pciHdl;                /* Handle of attached DM642 device */
        deviceTable_t* pDevice;      /* backlink to deviceTable structure */
        void __iomem* memAccAddr;    /* Base Address for all Memory Address Accessses */
        void __iomem* memAddr;       /* Address of mapped memory area */
        uint32_t memAddr_phys;       /* Physical Memory Address of mapped memory area  (DSP) */
        uint32_t EmifMemBase;        /* Base adress offset of the emif core */
        uint32_t dspMemBase;         /* DSP base memory address (SIMU: Virtual addr. of SHM else 0) */
        uint32_t dspRootAddr;        /* Root address of the communication structure */
        void __iomem* regModConf;    /* Device Configuration Module Register Area */
        void* regModCtrl;            /* Module Status/Ctrl Register Area */
        uint32_t memAddr_phys_Const; /* Physical Memory Address of mapped memory area  (DSP) */
        uint32_t dspp;               /* value of the DSP memory page register */
#ifdef NEW_MEM_ALLOC
        int dsp_mem_fd;     /* file descriptor for DSP SHR Object */
        char* dsp_mem_Name; /* Name of DSP SHR Object */
#endif
        int intPulseChId;                /* Channel id for receiving pulse from the int thread */
        copyMode_t copyMode;             /* Mode for the data transfer */
        int dmaChannel;                  /* Channel for dma transfers (<0 = invalid) */
        int dmaCoId;                     /* DMA-Lib Connection ID */
        int dmaPulseChId;                /* DMA-Lib Channel id for receiving pulse from the DMA */
        unsigned int heartBeatPeriodeMs; /* Maximum period time between heartbeat interrupts in ms */
        void __iomem* memAddr_1;
        uint32_t memAddr_1_phys; /* Physical Memory Address of mapped memory area  (DSP) */
        void* memAddr_2;
        void* memAddr_3;
        uint32_t currMsgId;                   /* messageId of last message to DSP */
        pthread_mutex_t currMsgIdMutex;       /* Mutex to lock change sequence for currMsgId */
        int verbose;                          /* Verbosity level of the chipset layer */
        int suppress;                         /* Suppress download of DSP-Application */
        pthread_mutex_t memAccessMutex;       /* Mutex to lock paged memory access */
        dra7xxConfigState_t configState;      /* configuration state of the chipset layer */
        pthread_mutex_t configStateCondMutex; /* Mutex to lock access to the configOk member with configCondVar */
        pthread_cond_t configStateCondVar;    /* Condition variable to signal change of configOk member */
        uint32_t dspAdminStructAdr;           /* Base address of the dsp administration sructure */
        void* pBootLoaderData;                /* Data returned from bootloader */
        ssize_t bootLoaderSize;               /* Size of the bootloader */
        char* pLoaderFileName;                /* Pointer to bootloader filename specified in command line */
        char* pFileName;                      /* Pointer to filename specified in command line */
        pthread_mutex_t dspIntCondMutex;      /* Mutex to lock access to the dspIntCondVar */
        pthread_cond_t dspIntCondVar;         /* Condition variable to register the dsp interrupt before communication is established */
        uint32_t dspIntMarker;                /* Marker to signalize that an dsp interrupt was got */
        char* pSimulatorName;                 /* Pointer to simulator device name specified in command line */
        uint32_t simuEnumerator;              /* Enumeration value for connection to simulator */
        int simuFd;                           /* Filedescriptor for opened simulator */
        int simMemFd;                         /* Filedescriptor for opened shared memory object */
        size_t simMemSize;                    /* Size of the mapped shared memory object */
        int dummyBL;                          /* Reset DSP and download dummy bootloader in suppress mode */
        uint32_t memShiftDummyBL;             /* value Memory Shift for using DummyBL */
        uint32_t MemSize;                     /* Size of Memory to allocate for the dev-dspipc */
        uint32_t pageMask;                    /* Value for Page-Masking */
        uint32_t AdrCheckPageMask;            /* Value for AddressCheck-Page-Masking */
        uint32_t DspStartupParam;             /* DSP Startup parameter */
        uint32_t* pReadDMABuffer;             /* Pointer to Virtual Mem.-Address of READ-DMA-Buffer */
        uint32_t ReadDMABuffer_phys;          /* Pointer to Physical Mem.-Address of READ-DMA-Buffer */
        uint32_t* pWriteDMABuffer;            /* Pointer to Virtual Mem.-Address of READ-DMA-Buffer */
        uint32_t WriteDMABuffer_phys;         /* Pointer to Physical Mem.-Address of READ-DMA-Buffer */
        uint32_t clockenable;                 /* do enabling the Mailbox Clocks during attach() */
        messagePointer_t pReadMsg;            /* Pointer to read message */
        messagePointer_t pWriteMsg;           /* Pointer to write message */
};

/**
 * Access macro to reduce the access lines for the DSP administration
 * structure. This macro delivers the byte offset of the specified structure
 * member inside the DSP communication administration structure
 */
#define ADMIN_OFFSET(x) offsetof(struct _DSPIPCCommunicationStructure, x)

/**
 *  Create a typedef to use the DSPIPCDRA7XX_S structure
 * @typedef dspIpcdra7xx_t
 */
typedef struct DSPIPCDRA7XX_S dspIpcDra7xx_t;

/**
 *  Coff variable
 *
 */
extern dspIpcDra7xx_t* pChipset_Coff;

/*=========================  Prototype Definitions   =========================== */
int dspipc_dra7xx_init(deviceTable_t* pDevice, char* pOptions);

#endif /*_DSPIPC_DRA7XX_H_INCLUDED_ */
