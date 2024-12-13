/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/

/**
 * @file dev-dspipc.h
 * 
 * This header file contains all definitions required for the dev-dspipc
 * ressource manager
 * 
 * @author Andreas Heiner
 * @author Andreas Abel
 * @date   03.03.2006
 *
 * @author Howard Yang
 * @date   11.OCT.2012
 * 
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

#ifndef _DEVDSPIPC_H_INCLUDED_
#define _DEVDSPIPC_H_INCLUDED_

#include <linux/device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/wait.h>

#include "dspipc.h"
#include "dspipc_pthread.h"

 /**
 * DSPIPC kernel version define
 */
#define DSPIPC_DRIVER_VERSION "18444A"

/**
 * Define base device path to be used in front of each device specified inside
 * the config file
 */
#define DEV_BASE_PATH "/dev/dspipc" /* mountpoint base path */

/**
 * Macro to convert number into megabyte
 */
#define MEGABYTE(x) ((x)*1024 * 1024)

/**
 * Macro to convert number into kilobyte
 */
#define KILOBYTE(x) ((x)*1024)

/**
 * Specify the current interface identifier
 */
#define DSPIPC_DLL_INTERFACE_VERSION 0x00010000 /*0xaabbbbcc aaaa=major bbbb=build */

#define MAJOR_NUMBER 260
#define MINOR_COUNT 255

/**
 * length of share memory which is used for stream channel
 */
#define SHARE_MEM_LEN 0x00100000

/*=========================    Global Variables   ============================== */
/**
 * Handle of the attached FPGA
 */
extern void* Hndl_pciD;

/**
 * Structure holding PCI info structure after pci_device_attach()
 */
/*extern struct pci_dev_info pci_info; */

/**
 * Mutex to lock any change inside the device/subdevice structure to be atomic
 */
extern pthread_mutex_t deviceStructMutex;
/*extern struct semaphore deviceStructMutex; */

/*=========================    Type Definitions   ============================== */
/** 
 *  Drain send type to be done
 * @enum DRAIN_SEND_MODE_E
 */
enum DRAIN_SEND_MODE_E {
        SEND_DRAIN_START, /* Start drain signaling has to be send */
        SEND_DRAIN_END    /* End drain signaling has to be send  */
};

/**
 *  Create a typedef to use the DRAIN_SEND_MODE_E enumeration
 * @typedef drainSendMode_t
 */
typedef enum DRAIN_SEND_MODE_E drainSendMode_t;

/** 
 *  Create a typedef to use the DEVICETABLE_S structure 
 * @typedef deviceTable_t
 */
typedef struct DEVICETABLE_S deviceTable_t;

/** 
 *  Create a typedef to use the SUBDEVICETABLE_S structure 
 * @typedef subDeviceTable_t
 */
typedef struct SUBDEVICETABLE_S subDeviceTable_t;

/**
 *  Create a typedef to use the DSPIPCATTR_S structure
 * @typedef dspIpcAttr_t
 */
typedef struct DSPIPCATTR_S dspIpcAttr_t;

/** 
 *  Reset type to be done
 * @enum RESET_TYPE_E
 */
enum RESET_TYPE_E {
        PREPARE_RESET, /* prepare the reset (turn INT's off,...) */
        DO_RESET       /* do the DSP reset */
};

/**
 *  Create a typedef to use the RESET_TYPE_E enumeration
 * @typedef resetType_t
 */
typedef enum RESET_TYPE_E resetType_t;

/** 
 * Function pointer type to DLL init function
 * func     ( pDevice, pOptions )
 * @typedef initFunc_t
 */
typedef int (*initFunc_t)(deviceTable_t* pDevice, char* pOptions);

/** 
 * Function pointer type to DLL load firmware function
 * func     ( pDevice )
 * @typedef loadFWFunc_t
 */
typedef int (*loadFWFunc_t)(const deviceTable_t* pDevice);

/** 
 * Function pointer type to DLL start firmware function
 * func     ( pDevice )
 * @typedef startFWFunc_t
 */
typedef int (*startFWFunc_t)(const deviceTable_t* pDevice);

/** 
 * Function pointer type to DLL start application function
 * func     ( pDevice )
 * @typedef startAppFunc_t
 */
typedef int (*startAppFunc_t)(const deviceTable_t* pDevice);

/** 
 * Function pointer type to DLL start communication function
 * func     ( pDevice )
 * @typedef startComFunc_t
 */
typedef int (*startComFunc_t)(const deviceTable_t* pDevice);

/** 
 * Function pointer type to DLL register memory object function
 * func     ( pDevice )
 * @typedef registerShmObjFunc_t
 */
typedef int (*registerShmObjFunc_t)(const deviceTable_t* pDevice,
    const char* pName,
    uint32_t physAddr, uint32_t size);

/** 
 * Function pointer type to DLL de-register memory object function
 * func     ( pDevice )
 * @typedef deregisterShmObjFunc_t
 */
typedef int (*deregisterShmObjFunc_t)(const deviceTable_t* pDevice,
    const char* pName);

/** 
 * Function pointer type to DLL send function
 * func     ( pSubDevice )
 * @typedef sendFunc_t
 */
typedef int (*sendFunc_t)(subDeviceTable_t* pSubDevice);

/** 
 * Function pointer type to DLL sendStreamData function
 * func     ( pSubDevice )
 * @typedef sendStreamDataFunc_t
 */
typedef int (*sendStreamDataFunc_t)(subDeviceTable_t* pSubDevice);

/** 
 * Function pointer type to DLL send drain function
 * func     ( pSubDevice )
 * @typedef sendDrainFunc_t
 */
typedef int (*sendDrainFunc_t)(subDeviceTable_t* pSubDevice, drainSendMode_t mode);

/** 
 * Function pointer type to DLL interrupt handler thread
 * func     ( pDevice )
 * @typedef intThread_t
 */
typedef int (*intThread_t)(deviceTable_t* pDevice);

/** 
 * Function pointer type to DLL close function
 * func     ( pDevice )
 * @typedef closeFunc_t
 */
typedef int (*closeFunc_t)(deviceTable_t* pDevice);

/** 
 * Function pointer type to DLL dump state function
 * func     ( pDevice )
 * @typedef dumpStateFunc_t
 */
typedef int (*dumpStateFunc_t)(const deviceTable_t* pDevice);

/** 
 * Function pointer type to DLL get DSP SW version function
 * func     ( pDevice, pVersion )
 * @typedef getDspSwVersFunc_t
 */
typedef int (*getDspSwVersFunc_t)(const deviceTable_t* pDevice, dspipc_dspSwVers_t* pVersion);

/** 
 * Function pointer type to reset State of chipset
 * func     ( pDevice, versionArray )
 * @typedef resetState_t
 */
typedef int (*resetState_t)(const deviceTable_t* pDevice, resetType_t type);

/** 
 *  command line option structure
 * @struct CMDOPTIONS_S
 */
struct CMDOPTIONS_S {
        int optv; /*number of specified v options in the command line for verbosity */

        /* suppress background (deamon) mode */
        int optb; /* no background marker is set to suppress deamon mode */

        /* slog severity */
        unsigned char slogSeverity; /* set level of severity used inside the slog macros for slogger */

        /* threadpool control */
        unsigned short lo_water;  /* The minimum number of threads that the pool should keep in the blocked state (i.e. threads that are ready to do work). */
        unsigned short increment; /* The number of new threads created at one time */
        unsigned short hi_water;  /* The maximum number of threads to keep in a blocked state */
        unsigned short maximum;   /* The maximum number of threads that the pool can create */

        /* found chipset layers */
        int numChipset; /* Number of specified and loaded chipset layers */
};

/** 
 *  Create a typedef to use the CMDOPTIONS_S structure 
 * @typedef cmdOptions_t
 */
typedef struct CMDOPTIONS_S cmdOptions_t;

/**
 * Global structure holding all command line settings
 */
extern cmdOptions_t gCmdOptions;

/** 
 *  enumeration containing all the possible flag values for the device table
 *  structure
 */
enum {
        PROVIDE_PHYSICAL = 0x00000000U,            /* Provide physical adresses for the stream data transfers */
        PROVIDE_VIRTUAL = 0x00000001U,             /* Provide virtual adresses for the stream data transfer */
        CLIENT_INITIATE_STREAM_COPY = 0x00000000U, /* Client will do the stream data transfer */
        DRIVER_INITIATE_STREAM_COPY = 0x00000002U, /* Drver will do the stream data transfer */
};

/** 
 *  Reason of change inside the subdevices structure
 * @enum CHANGE_REASON_E
 */
enum CHANGE_REASON_E {
        REASON_ERROR,    /* Wait for subdevices change has been interrupted due to an internal error */
        ADD_CHANNEL,     /* Wait for subdevices change has been interrupted because of an add channel */
        DELETE_CHANNEL,  /* Wait for subdevices change has been interrupted because of a delete channel */
        CONNECT_UNBLOCK, /* Wait for subdevices change has been interrupted because of a connect timeout */
        TIMEOUT          /* Wait for subdevices change has been interrupted because of a condvar timertimeout */
};

/**
 *  Create a typedef to use the CHANGE_REASON_E enumeration
 * @typedef changeReason_t
 */
typedef enum CHANGE_REASON_E changeReason_t;

/**
 *  current state of the boot table file parser
 * @enum SECTION_STATE_E
 */
enum SECTION_STATE_E {
        SECTION_NOINIT,      /* state is not initialized */
        SECTION_ENTRY_POINT, /* read entry point info */
        SECTION_SIZES,       /* read header information */
        SECTION_DATA,        /* read binary data */
        SECTION_NONE,        /* last section header was found (len=0) */
        SECTION_FILEERROR    /* a file format error has occured */
};

/**
 *  Create a typedef to use the SECTION_STATE_E enumeration
 * @typedef sectionState_t
 */
typedef enum SECTION_STATE_E sectionState_t;

/**
 * Number of maximum allowed software regions
 */
#define NUM_SW_REGIONS 8

/**
 * Structure to define the valid areas for the application download
 * @struct SW_REGIONS_S
 */
struct SW_REGIONS_S {
        struct
        {
                uint32_t start;   /* start address of this region */
                uint32_t end;     /* end address of this region */
        } region[NUM_SW_REGIONS]; /* valid regions for data download */
};

/** 
 *  Create a typedef to use the SW_REGIONS_S structure 
 * @typedef sw_regions_t
 */
typedef struct SW_REGIONS_S sw_regions_t;

/**
 * Structure for storing all information for the boottable appilication
 * download
 * @struct BOOTTABLE_ADMIN_S
 */
struct BOOTTABLE_ADMIN_S {
        sectionState_t state;    /* current state of the boottable download */
        sw_regions_t regions;    /* valid software address regions */
        int fd;                  /* filedescriptor for base file */
        uint32_t flashStartAddr; /* fash Start address */
        off_t fileLen;           /* Size of the boottable file */
        off_t offset;            /* Next position in file to continue */
        int sectionNumber;       /* current section number */
        uint32_t sectionAddr;    /* Address of the current section  */
        uint32_t sectionSize;    /* Size of current section */
        uint32_t sectionRemain;  /* Remaining datasize of current section */
};

/** 
 *  Create a typedef to use the BOOTTABLE_ADMIN_S structure 
 * @typedef boottable_admin_t
 */
typedef struct BOOTTABLE_ADMIN_S boottable_admin_t;

/** 
 *  shared memory object entry structure
 * @struct MEMOBJLIST_S
 */
struct MEMOBJLIST_S {
        char memName[MEM_NAME_LENGTH]; /* name of the memory object */
        uint32_t memId;                /* internal identifier of the memory object */
        int protFlags;                 /* Protection flags of the memory object */
        int fd;                        /* file descriptor for the shmem object */
        uint32_t usageCount;           /* Counter for pending transfer requests */
        void __iomem* pVirtAddr;       /* Virtual address pointer of mapped element */
        uint32_t physAddr;             /* Physical address of mapped element   */
        uint32_t size;                 /* size of the memory object                          */
        struct MEMOBJLIST_S* pNext;    /* linked list, pointer to next element, NULL at end */
};

/** 
 *  Create a typedef to use the MEMOBJLIST_S structure 
 * @typedef memObjList_t
 */
typedef struct MEMOBJLIST_S memObjList_t;

/** 
 *  shared memory management structure
 * @struct SHAREMEMDESC_S
 */
struct SHAREMEMDESC_S {
        void __iomem* pvirtAddrStart;
        uint32_t useoffset;
        uint32_t phyAddrStart;
};

typedef struct SHAREMEMDESC_S ShareMemDesc_t;

/**
 * Enumeration to specify the kind of the specified address
 * @enum MEMSEARCHTYPE_E
 */
enum MEMSEARCHTYPE_E {
        MEMOBJ_SEARCH_PHYS = 0,
        MEMOBJ_SEARCH_VIRT = 1
};

/** 
 *  Create a typedef to use the MEMSEARCHTYPE_E enumeration 
 * @typedef memSearchType_t
 */
typedef enum MEMSEARCHTYPE_E memSearchType_t;

/**
 *  current state of the device layer
 * @enum DEVICE_STATE_E
 */
enum DEVICE_STATE_E {
        STATE_NORMAL,    /* normal state */
        STATE_DSP_REBOOT /* DSP is rebooted now */
};

/**
 *  Create a typedef to use the DEVICE_STATE_E enumeration
 * @typedef deviceState_t
 */
typedef enum DEVICE_STATE_E deviceState_t;

/**
 * @brief Message id counter mask
 *
 * The message counter mask defines the maximum message ID 
 */
#define DSPIPC_MSGID_COUNTER_MSK (0x7fffffffU)

#define CSL_NAME_LENGTH 16
#define DLL_NAME_LENGTH 16
#define DEV_NAME_LENGTH 16

/** 
 *  device table entry structure
 * @struct DEVICETABLE_S
 */
struct DEVICETABLE_S {
        int major_num; /* Linux driver major number.  */
        unsigned int cur_minor_num;
        struct class* driverclass;            /* driver class, used for udev */
        deviceTable_t* pNext;                 /* Linked list, this is the pointer to the next element, NULL at last element */
        subDeviceTable_t* pSubDevices;        /* Pointer to linked list with sub devices */
        subDeviceTable_t* pLastSendSubDevice; /* Marker for last active sending subdevice for orund robin */

        subDeviceTable_t* pSubDevices_0;        /* Pointer to linked list with sub devices    HOSTIPC_CHANNELPRIORITY_ADMIN */
        subDeviceTable_t* pLastSendSubDevice_0; /* Marker for last active sending subdevice for round robin    */
        subDeviceTable_t* pSubDevices_1;        /* Pointer to linked list with sub devices    HOSTIPC_CHANNELPRIORITY_REALTIMECOMMAND */
        subDeviceTable_t* pLastSendSubDevice_1; /* Marker for last active sending subdevice for round robin    */
        subDeviceTable_t* pSubDevices_2;        /* Pointer to linked list with sub devices    HOSTIPC_CHANNELPRIORITY_REALTIMESTREAM */
        subDeviceTable_t* pLastSendSubDevice_2; /* Marker for last active sending subdevice for round robin */
        subDeviceTable_t* pSubDevices_3;        /* Pointer to linked list with sub devices    HOSTIPC_CHANNELPRIORITY_NORMAL */
        subDeviceTable_t* pLastSendSubDevice_3; /* Marker for last active sending subdevice for round robin */
        subDeviceTable_t* pSubDevices_4;        /* Pointer to linked list with sub devices    HOSTIPC_CHANNELPRIORITY_IDLE */
        subDeviceTable_t* pLastSendSubDevice_4; /* Marker for last active sending subdevice for round robin */

        volatile uint32_t dev_send_counter;      /* Counter that shows how many Messages sends (Queue-Scans) have to be done */
        volatile uint32_t dev_send_counter_done; /* Counter that shows how many Messages have to be sends (Queue-Scans) have to be done */
        pthread_mutex_t sendCntMutex;            /* Mutex to lock on changes on sendCounter */

        deviceState_t deviceState;                   /* State of the device layer */
        uint32_t drain_Avail_Cnt;                    /* Drain avail Counter. Shows SendThread that just scanning for Drain if shoud be done) */
        uint32_t currMsgId;                          /* messageId of last message to DSP */
        pthread_mutex_t currMsgIdMutex;              /* Mutex to lock change sequence for currMsgId */
        int deviceIndex;                             /* Device enumeration for seperation in io_*() functions */
        char pCslName[CSL_NAME_LENGTH];              /* Pointer to the chipset name (e.g. dra300) extracted from the command line */
        int enumerate;                               /* enumeration value of this device */
        char pDeviceName[DEV_NAME_LENGTH];           /* Pointer to the device name (e.g. dra300.0) extracted from the command line */
        char pDllName[DLL_NAME_LENGTH];              /* Pointer to the dll name (e.g. dspipc-dra300.so) extracted from the command line */
        mode_t accessMode;                           /* accessmode of the device */
        uint32_t flags;                              /* flags to control the internal behaviour    ???? */
        dspIpcAttr_t* pDeviceAttr;                   /* Device attribute structure */
        void* dllHandle;                             /* Handle to the opened chipset DLL */
        uint32_t dllIfaceVersion;                    /* Place where the DLL stores the interface version for check */
        initFunc_t initFunc;                         /* Pointer to chipset DLL init function */
        loadFWFunc_t loadFWFunc;                     /* Pointer to chipset DLL load firmware function */
        startFWFunc_t startFWFunc;                   /* Pointer to chipset DLL start firmware function */
        startAppFunc_t startAppFunc;                 /* Pointer to chipset DLL start application function */
        startComFunc_t startComFunc;                 /* Pointer to chipset DLL start communication function */
        registerShmObjFunc_t registerShmObjFunc;     /* Pointer to chipset DLL register shm object function */
        deregisterShmObjFunc_t deregisterShmObjFunc; /* Pointer to chipset DLL register shm object function */
        sendFunc_t sendFunc;                         /* Pointer to chipset DLL send function */
        sendStreamDataFunc_t sendStreamDataFunc;     /* Pointer to chipset DLL sendStreamData function */
        sendDrainFunc_t sendDrainFunc;               /* Pointer to chipset DLL send drain function */
        intThread_t intThread;                       /* Pointer to chipset DLL interrupt thread function */
        closeFunc_t closeFunc;                       /* Pointer to chipset DLL close function */
        dumpStateFunc_t dumpStateFunc;               /* Pointer to chipset DLL dump State function */
        getDspSwVersFunc_t getDspSwVers;             /* Pointer to chipset DLL getDspSwVers function */
        resetState_t resetState;                     /* Pointer to chipset DLL resetState function */

        pthread_attr_t intThreadAttr;     /* Attrubute structure for the interrupt thread */
        pthread_t intThreadHndl;          /* Handle of the interrupt thread; */
        int intNumber;                    /* Number of the interrupt vector */
        struct sigevent intAttachEvent;   /* Event to be used with interrupt_attach_event */
        struct sigevent intSendEvent;     /* Event to be used to signal a new message to send */
        int intEvent;                     /* interrupt id from interrupt attach event */
        int intThreadPrio;                /* Priority for the interrupt thread */
        int intChId;                      /* Channelid for interrupt receive */
        int intCoId;                      /* Connection ID for sending to interrupt thread */
        void* pChipset;                   /* Pointer to chipset layer specific administraton structure */
        int DeviceEnum;                   /* Device-Enumerator (0=DRA300, 1=DM642, ....) */
        unsigned char slogSeverity;       /* Current slog severity */
        int terminate;                    /* marker to signal terminate sequence */
        pthread_mutex_t subDevicesMutex;  /* Mutex to lock on changes inside the subdevices structure */
        pthread_cond_t subDevicesCondVar; /* Condvar to signal any changes inside the subdevices structure */
        changeReason_t subDevicesReason;  /* Reason for signaling the condition variable */
        boottable_admin_t bootTable;      /* Structure holding all infos for the current state of boottable download */
        volatile int doLoop;              /* interrupt thread loop condition */
};

/** 
 *  sub device usage types
 * @enum SUBDEVICETYPE_E
 */
enum SUBDEVICETYPE_E {
        SD_CONTROL, /* sub device is an internal control device */
        SD_COMMAND, /* sub device is a command channel device */
        SD_STREAM   /* sub device is a stream channel device */
};

/** 
 *  Create a typedef to use the SUBDEVICETYPE_E enumeration 
 * @typedef subDeviceType_t
 */
typedef enum SUBDEVICETYPE_E subDeviceType_t;

/** 
 *  sub device communication direction
 * @enum SUBDEVICEDIR_E
 */
enum SUBDEVICEDIR_E {
        SD_DIR_IN,   /* sub device is an input device */
        SD_DIR_OUT,  /* sub device is an output device */
        SD_DIR_NODIR /* sub device has no specific direction (control port) */
};

/** 
 *  Create a typedef to use the SUBDEVICEDIR_E enumeration 
 * @typedef subDeviceDir_t
 */
typedef enum SUBDEVICEDIR_E subDeviceDir_t;

/** 
 *  sub device status
 * @enum SUBDEVICESTATUS_E
 */
enum SUBDEVICESTATUS_E {
        SD_STAT_INIT = 0,       /* sub device is initializing */
        SD_STAT_READY,          /* sub device is operational */
        SD_STAT_BLOCKED,        /* sub device is blocked */
        SD_STAT_DRAIN,          /* sub device is currently drained */
        SD_STAT_DRAIN_WAIT_ACK, /* sub device is waiting for dain acknowledge */
        SD_STAT_DRAIN_COMPLETE, /* sub device has finished drain */
        SD_STAT_CLOSED,         /* sub device is already closed */
        SD_STAT_DELETED,        /* sub device structure is deleted */
        SD_STAT_REBOOT          /* sub device structure is in reboot */

};

/** 
 *  Create a typedef to use the SUBDEVICESTATUS_E enumeration 
 * @typedef subDeviceStatus_t
 */
typedef enum SUBDEVICESTATUS_E subDeviceStatus_t;

/**
 * Define default number of queue elements for receive/transmit queues
 */
#define DEFAULT_NUM_QUEUE_ELEMENTS 64

/**
 * define the infinite len value to be used inside the queues to indicate an
 * infinite queue length
 */
#define SUBDEVICE_QUEUE_LEN_INFINITE 0xFFFFFFFFU

/** 
 * enumeration of the different source to select the right condition variable
 * to be signaled after added elements
 * @enum MSG_QUEUE_CALLER_E
 */
enum MSG_QUEUE_CALLER_E {
        RECEIVE_CALLED, /* called by receiver thread (e.g. interrupt thread) */
        SEND_CALLED     /* called by sender thread */
};

/** 
 *  Create a typedef to use the MSG_QUEUE_CALLER_E enumeration 
 * @typedef queueMsgCaller_t
 */
typedef enum MSG_QUEUE_CALLER_E queueMsgCaller_t;

/**
 * This structure describes one entry into the transfer or provide buffer queues
 * of the subdevice structure.
 *  
 * @struct SUBDEVICE_BUFFERQUEUE_S
 */
struct SUBDEVICE_BUFFERQUEUE_S {
        uint32_t numEntries;                           /* number of entries in this element */
        uint32_t cmdId;                                /* cmd id bind to this queue entry */
        int complete;                                  /* marker to indicate that this element should be returned */
        dspipc_scatGat_t entry[NUM_SCAT_GATH_ENTRIES]; /* array of scatter gather entries */
        struct SUBDEVICE_BUFFERQUEUE_S* pNext;         /* Linked list, this is the pointer to the next element, NULL at last element */
};

/**
 *  Create a typedef to use the SUBDEVICE_BUFFERQUEUE_S structure
 * @typedef subDeviceBufferQueue_t
 */
typedef struct SUBDEVICE_BUFFERQUEUE_S subDeviceBufferQueue_t;

/** 
 *  buffer type enumeration
 * @enum BUFFER_QUEUE_TYPE_E
 */
enum BUFFER_QUEUE_TYPE_E {
        PROVIDE_BUFFER_DSP,  /* Buffer type is a provided one from DSP (empty buffers) */
        TRANSFER_BUFFER_DSP, /* Buffer type is a transfer one from DSP (full buffers) */
        PROVIDE_BUFFER_HOST, /* Buffer type is a provided one from Host (empty buffers) */
        TRANSFER_BUFFER_HOST /* Buffer type is a transfer one from Host (full buffers) */
};

/** 
 *  Create a typedef to use the BUFFER_QUEUE_TYPE_E enumeration 
 * @typedef bufferQueueType_t
 */
typedef enum BUFFER_QUEUE_TYPE_E bufferQueueType_t;

/** 
 *  Transfer direction enumeration
 * @enum TRANSFER_DIR_E
 */
enum TRANSFER_DIR_E {
        HOST_TO_DSP, /* Transfer direction is from host to dsp */
        DSP_TO_HOST  /* Transfer direction is from dsp to host */
};

/** 
 *  Create a typedef to use the TRANSFER_DIR_E enumeration 
 * @typedef transferDir_t
 */
typedef enum TRANSFER_DIR_E transferDir_t;

/** 
 *  Cleanup mode for dspipcCleanupSubDevice
 * @enum CLEANUP_MODE_E
 */
enum CLEANUP_MODE_E {
        MODE_NORMAL, /* Cleanup mode is normal clean up */
        MODE_DRAIN   /* Cleanup mode is cleanup for drain  */
};

/** 
 *  Create a typedef to use the CLEANUP_MODE_E enumeration 
 * @typedef cleanupMode_t
 */
typedef enum CLEANUP_MODE_E cleanupMode_t;

/** 
 *  Define all possible message pointer types to allow examination inside debugger
 * @union MESSAGE_POINTER_U
 */
union MESSAGE_POINTER_U {
        void* generic;                          /* generic pointer */
        dspipc_message_t* normal;               /* normal message */
        dspipc_getMessage_t* get;               /* get message */
        dspipc_sendMessage_t* send;             /* send message */
        dspipc_recvStreamMessage_t* recvStream; /* receive stream message */
        dspipc_sendStreamMessage_t* sendStream; /* send stream message */
        dspipc_specialCmdMessage_t* special;    /* special message */
};

/** 
 *  Create a typedef to use the MESSAGE_POINTER_U union 
 * @typedef messagePointer_t
 */
typedef union MESSAGE_POINTER_U messagePointer_t;

/** 
 *  max
 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/** 
 *  Create a typedef to use the MESSAGE_POINTER_U union 
 * @typedef messagePointer_t
 */
#define MESSAGE_POINTER_U_SIZE MAX(DEVCTL_MSG_SIZE, MAX(sizeof(dspipc_message_t), MAX(sizeof(dspipc_getMessage_t), MAX(sizeof(dspipc_sendMessage_t), MAX(sizeof(dspipc_recvStreamMessage_t), MAX(sizeof(dspipc_sendStreamMessage_t), sizeof(dspipc_specialCmdMessage_t)))))))

/** 
 * structure of one of message queue elements
 * @struct MSG_QUEUE_ELEMENT_S
 */
struct MSG_QUEUE_ELEMENT_S {
        uint32_t msgId;               /* message id of the contained message */
        uint32_t cmdId;               /* command id for higher level software */
        uint32_t msgSize;             /* message size in bytes */
        dspipc_msgType_t msgType;     /* message type (command | stream) */
        dspipc_msgReply_t msgReply;   /* message reply request (no | yes) */
        dspipc_msgContent_t contType; /* content type (message type normal, bufcomplete,... */
        messagePointer_t pMsg;        /* Pointer to message */
        uint32_t numLostBefore;       /* Number of message lost before this message due to queue overruns */
};

/** 
 *  Create a typedef to use the MSG_QUEUE_ELEMENT_S structure
 * @typedef msgQueueElement_t
 */
typedef struct MSG_QUEUE_ELEMENT_S msgQueueElement_t;

/** 
 * structure of a receive or send message queue
 * @struct MSG_QUEUE_S
 */
struct MSG_QUEUE_S {
        int numElements;               /* Total number of possible queue elements */
        volatile int numMessagesLost;  /* In case of queue full this is the counter of lost messages */
        volatile int readIndex;        /* current read index inside the queue */
        volatile int writeIndex;       /* current write index inside the queue */
        volatile int unblockReason;    /* Reason for forced unblock */
        pthread_mutex_t condMutex;     /* mutex to deal with condition variable and to lock changes inside the queue */
        pthread_cond_t condVar;        /* condition variable to unblock devctl */
        msgQueueElement_t* pArray;     /* Pointer to the array of message queue elements */
        wait_queue_head_t inWaitQueue; /* read wait queue  */
};

/** 
 *  Create a typedef to use the MSG_QUEUE_S enumeration 
 * @typedef msgQueue_t
 */
typedef struct MSG_QUEUE_S msgQueue_t;

/** 
 * enumeration of the different source to select the right condition variable
 * to be signaled after added elements
 * @enum SUBDEVICE_XONOFF_STATE_E
 */
enum SUBDEVICE_XONOFF_STATE_E {
        XON_STATE, /* CMD Message can be transferrred to DSP */
        XOFF_STATE /* CMD Message can NOT be transferrred to DSP */
};

/** 
 *  Create a typedef to use the SUBDEVICE_XONOFF_STATE_E enumeration 
 * @typedef queueMsgCaller_t
 */
typedef enum SUBDEVICE_XONOFF_STATE_E subdevice_xonoff_state_t;

/** 
 *  sub device table entry structure
 * @struct SUBDEVICETABLE_S
 */
struct SUBDEVICETABLE_S {
        struct cdev chrdev;
        unsigned int minor_num;
        subDeviceTable_t* pNext;                /* Linked list, this is the pointer to the next element, NULL at last element */
        subDeviceStatus_t status;               /* Status of the subdevice */
        subDeviceType_t type;                   /* Usage type of this sub device */
        unsigned int channelPrio;               /* Priority for sending messages from this channel */
        msgQueue_t* pRecvMsgQueue;              /* Queue for incoming messages */
        msgQueue_t* pSendMsgQueue;              /* Queue for outgoing messages */
        subDeviceBufferQueue_t* pTransferQueue; /* Queue for stream data transfer requests */
        subDeviceBufferQueue_t* pProvideQueue;  /* Queue for provided buffers for stream data transfers */
        subdevice_xonoff_state_t xonoffState;   /* State for CMD channels for transferring CMD messages to DSP */

        deviceTable_t* pDevice;           /* Pointer to device administration structure */
        char* pSubDeviceName;             /* Pointer to the device name of the subdevice */
        dspIpcAttr_t* pDeviceAttr;        /* Device attribute structure */
        void* pExtra;                     /* Pointer to linked subdevice specific data  */
        int numOpenFd;                    /* Number of opened filedescriptors */
        subDeviceDir_t dir;               /* Communication direction of this device */
        uint32_t channelNum;              /* Channel number of the device */
        int removed;                      /* marker for subdevice already removed */
        pthread_mutex_t bufferQueueMutex; /* mutex to lock the access to the contents of the transfer and provide queue */
        subDeviceTable_t* pPrev;          /* Linked list, this is the pointer to the previous element, NULL at first element */

        int drainComplete;                  /* Variable to signal the acknowledge for a drain ack */
        pthread_cond_t drainCompCondVar;    /* Condition variable to wait for a drain complete */
        pthread_mutex_t drainCompCondMutex; /* Mutex to lock condition variable for drain complete    */
        messagePointer_t pMsg;              /* Pointer to message  */
        pthread_mutex_t thisMutex;          /* Mutex to lock this sub device */
};

/**
 *  Define the private device attribute structure. This structure will contain
 *  the common iofunc_attr_t structure and additionally private parameters.
 *  Currently only the number of the device is added to differ the devices
 *  inside the io_*() functions.
 * @struct DSPIPCATTR_S
 */
struct DSPIPCATTR_S {
        deviceTable_t* pDevice;       /* pointer to the device structure */
        subDeviceTable_t* pSubDevice; /* pointer for subDevice number, NULL=chipset root */
};

/**
 *  Define the private device ocb structure. This structure will contain
 *  the common iofunc_ocb_t structure and additionally private parameters.
 *  Currently only the a reference to the device table or sub device entry is
 *  added.
 * @struct DSPIPCOCB_S
 */
struct DSPIPCOCB_S {
        deviceTable_t* pDevice;       /* Pointer to store device table reference */
        subDeviceTable_t* pSubDevice; /* Pointer to store sub device table reference */
};

/**
 *  Create a typedef to use the DSPIPCOCB_S structure
 * @typedef dspIpcOcb_t
 */
typedef struct DSPIPCOCB_S dspIpcOcb_t;

/** 
 *  termination mode for signal handler
 * @enum TERMINATE_MODE_E
 */
enum TERMINATE_MODE_E {
        TERM_NOW,   /* terminate immediatly */
        WAIT_SIGNAL /* wait for an termination signal */
};

/**
 *  Create a typedef to use the TERMINATE_MODE_E enumeration
 * @typedef terminateMode_t
 */
typedef enum TERMINATE_MODE_E terminateMode_t;

/**
 * Define the different copy modes
 * @enum COPY_MODE_E
 */
enum COPY_MODE_E {
        COPYMODE_UNKNOWN = 0, /* Uninitialized */
        COPYMODE_CPU,         /* Use direct cpu copy commands */
        COPYMODE_HOSTDMA,     /* Use the dma controller inside the host bridge */
        COPYMODE_FPGADMA      /* Use the dma controller inside the FPGA (initiator) */
};

/**
 *  Create a typedef to use the COPY_MODE_E enumeration
 * @typedef copyMode_t
 */
typedef enum COPY_MODE_E copyMode_t;

/*=========================  external variable Definitions   =========================== */
/**
 * Pointer to the root element of the device table linked list
 */
extern deviceTable_t* pDeviceTableRoot;

extern cmdOptions_t gCmdOptions;

/**
 * Counter value for the next memory segemnt id to be used
 */
extern uint32_t nextMemId;

extern uint32_t num_ShmElem_to_alloc;

/**
 * use this variable to sync initialization and io entry.
 */
extern uint32_t init_success;

extern char DSP_IMAGE_VERSION_INFO[256];

extern deviceTable_t* pDevice;

extern void __iomem* regModConf_dspipc; /* Device Configuration Module Register Area */
extern uint32_t shareMemPhyAddr;        /* Address of stream shared memory */
extern uint32_t shareMemPhySize;        /* Size of stream shared memory */

/*=========================  Prototype Definitions   =========================== */

#endif /*_DEVDSPIPC_H_INCLUDED_ */
