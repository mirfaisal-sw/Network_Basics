/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file dspipc.h
 *
 * This is the public header file for the devctl functionality for the dev-dspipc
 * resource handler.
 *
 * @author Andreas Heiner
 * @author Andreas Abel
 * @date   03.03.2006
 *
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

#ifndef _DSPIPC_H_INCLUDED_
#define _DSPIPC_H_INCLUDED_

#include <linux/device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>

#define DSP_IPC_VERSION_MAJOR 2
#define DSP_IPC_VERSION_MINOR 0
#define DSP_IPC_VERSION_BUGFIX 0

#define DEVCTL_MSG_SIZE 256

/*
 * Specifiy the length of the shared memory name strings
 */
#define MEM_NAME_LENGTH 64

/*
 * Specifiy base code group of the dspipc devctls
 */
#define DSPIPC_CMD_CODE 1

#define _DCMD_MISC 100

#define MAX_NUM_MESSAGES 1 /*!!!!!!!!! */
#define NUM_SCAT_GATH_ENTRIES 16

/**
 *  driver Memory Trace Structure
 * @struct DSPIPC_DRVMEMTRC_S
 */
struct DSPIPC_DRVMEMTRC_S {
        uint32_t prtInfo;
};
/**
 *  Create a typedef to use the DSPIPC_DRVMEMTRC_S structure
 * @typedef dspipc_drvMemtrc_t
 */
typedef struct DSPIPC_DRVMEMTRC_S dspipc_drvMemtrc_t;

/**
 *  driver version structure
 * @struct DSPIPC_DRVVERS_S
 */
struct DSPIPC_DRVVERS_S {
        uint32_t drvMajor;
        uint32_t drvMinor;
        uint32_t drvBugfix;
        uint32_t reserved[1]; /* reserved field for future extension */
};

/**
 *  Create a typedef to use the DSPIPC_DRVVERS_S structure
 * @typedef dspipc_drvVers_t
 */
typedef struct DSPIPC_DRVVERS_S dspipc_drvVers_t;

/**
 *  driver version structure
 * @struct DSPIPC_DSPSWVERS_S
 */
struct DSPIPC_DSPSWVERS_S {
        uint32_t swProjCode;
        uint32_t swMajor;
        uint32_t swMinor;
        uint32_t swBugfix;
        uint32_t reserved[1]; /* reserved field for future extension */
};

/**
 *  Create a typedef to use the DSPIPC_DSPSWVERS_S structure
 * @typedef dspipc_dspSwVers_t
 */
typedef struct DSPIPC_DSPSWVERS_S dspipc_dspSwVers_t;

/**
 *  request shared memory object structure
 * @struct DSPIPC_REQMEMOBJ_S
 */
struct DSPIPC_REQMEMOBJ_S {
        uint32_t size;
        uint32_t mmapFlags;
        char memName[MEM_NAME_LENGTH];
        uint32_t memId;      /* id of the memory segment */
        uint32_t addroffset; /*physics address offset */
};

/**
 *  Create a typedef to use the DSPIPC_DSPSWVERS_S structure
 * @typedef dspipc_reqMemObj_t
 */
typedef struct DSPIPC_REQMEMOBJ_S dspipc_reqMemObj_t;

/**
 *  delete shared memory object structure
 * @struct DSPIPC_DELMEMOBJ_S
 */
struct DSPIPC_DELMEMOBJ_S {
        uint32_t memId;       /* id of the memory segment */
        uint32_t reserved[1]; /* reserved field for future extension */
};

/**
 *  Create a typedef to use the DSPIPC_DSPSWVERS_S structure
 * @typedef dspipc_delMemObj_t
 */
typedef struct DSPIPC_DELMEMOBJ_S dspipc_delMemObj_t;

/**
 *  qualifier to identify the content type of the message
 * @enum DSPIPC_MSG_CONTENT_E
 */
enum DSPIPC_MSG_CONTENT_E {
        DSPIPC_CONT_INVALID = 0,          /* message contains no valid data */
        DSPIPC_CONT_MESSAGE = 1,          /* message contains standard message data */
        DSPIPC_CONT_SENDSTREAM = 2,       /* message contains scatter/gather list for a send stream packet */
        DSPIPC_CONT_PROVIDEBUFSTREAM = 3, /* message contains scatter/gather list for a receive stream packet */
        DSPIPC_CONT_BUFCOMPLETE = 4,      /* message contains scatter/gather info for completed memory chunks */
        DSPIPC_CONT_ANY = 0xFFFF          /* message can contain any type */
};

/**
 *  Create a typedef to use the DSPIPC_MSG_CONTENT_E structure
 * @typedef dspipc_msgContent_t
 */
typedef enum DSPIPC_MSG_CONTENT_E dspipc_msgContent_t;

/**
 *  define a single scatter/gather entry (containing mem address info)
 * @struct DSPIPC_SCATGAT_S
 */
struct DSPIPC_SCATGAT_S {
        uint32_t memId; /* id of the memory segment */
        struct
        {
                void __iomem* pVirtAddr; /* virtual address of the memory segment */
                uint32_t physAddr;       /* physical address of the memory segment */
                void* pMemHdl;           /* Internal handle of the memory segment */
        } mem;
        uint32_t start;  /* start offset inside the shmem object */
        uint32_t size;   /* size of the data transfer */
        uint32_t remain; /* remaining size of the data transfer */
        uint32_t valid;  /* valid size of the data transfer (=size-remain) */
};

/**
 *  Create a typedef to use the DSPIPC_SCATGAT_S structure
 * @typedef dspipc_scatGat_t
 */
typedef struct DSPIPC_SCATGAT_S dspipc_scatGat_t;

/**
 * Flags for specifying the content of a stream message
 */
enum DSPIPC_STREAMFLAGS_E {
        STREAMFLAG_NONE = 0x00000000,           /* no special flags */
        STREAMFLAG_DISCONTINUITY = 0x00000001,  /* Bit 0: data has discontinuity, means could be processed */
        STREAMFLAG_PROVIDE_BUFFER = 0x00000002, /* Bit 1: data is normal return from provide buffer */
        STREAMFLAG_DRAINED = 0x00000004,        /* Bit 2: data has been drained */
        STREAMFLAG_SEND_DATA = 0x00000008       /* Bit 3: data was a send data command */
};

/**
 *  define the structure of the header for an stream message
 * @struct DSPIPC_STREAMHEADER_S
 */
struct DSPIPC_STREAMHEADER_S {
        uint32_t numEntries;  /* number of scatter/gather entries */
        uint32_t cmdId;       /* command id for completion events */
        uint32_t flags;       /* flags for this stream message, defined in @ref DSPIPC_STREAMFLAGS_E */
        uint32_t reserved[1]; /* reserved field for future extension */
};

/**
 *  Create a typedef to use the DSPIPC_STREAMHEADER_S structure
 * @typedef dspipc_streamHeader_t
 */
typedef struct DSPIPC_STREAMHEADER_S dspipc_streamHeader_t;

/**
 *  provide pieces inside a memory segment for stream data from the dsp
 * @struct DSPIPC_SENDRECVSTREAMMESSAGE_S
 */
struct DSPIPC_SENDRECVSTREAMMESSAGE_S {
        dspipc_streamHeader_t hdr;                     /* header of the stream message */
        dspipc_scatGat_t entry[NUM_SCAT_GATH_ENTRIES]; /* array of scatter gather entries */
};
/**
 *  Create a typedef to use the DSPIPC_SENDRECVSTREAMMESSAGE_S structure
 * @typedef dspipc_sendrecvStreamMessage_t
 */
typedef struct DSPIPC_SENDRECVSTREAMMESSAGE_S dspipc_sendrecvStreamMessage_t;

/**
 *  provide pieces inside a memory segment for stream data from the dsp
 * @struct DSPIPC_RECVSTREAMMESSAGE_S
 */
struct DSPIPC_RECVSTREAMMESSAGE_S {
        dspipc_streamHeader_t hdr;                     /* header of the stream message */
        dspipc_scatGat_t entry[NUM_SCAT_GATH_ENTRIES]; /* array of scatter gather entries */
};
/**
 *  Create a typedef to use the DSPIPC_RECVSTREAMMESSAGE_S structure
 * @typedef dspipc_recvStreamMessage_t
 */
typedef struct DSPIPC_RECVSTREAMMESSAGE_S dspipc_recvStreamMessage_t;

/**
 *  send pieces inside a memory segment as stream to the dsp
 * @struct DSPIPC_SENDSTREAMMESSAGE_S
 */
struct DSPIPC_SENDSTREAMMESSAGE_S {
        dspipc_streamHeader_t hdr;                     /* header of the stream message */
        dspipc_scatGat_t entry[NUM_SCAT_GATH_ENTRIES]; /* array of scatter gather entries */
};
/**
 *  Create a typedef to use the DSPIPC_SENDSTREAMMESSAGE_S structure
 * @typedef dspipc_sendStreamMessage_t
 */
typedef struct DSPIPC_SENDSTREAMMESSAGE_S dspipc_sendStreamMessage_t;

/**
 *  message structure header
 * @struct DSPIPC_MESSAGE_HDR_S
 */
struct DSPIPC_MESSAGE_HDR_S {
        uint32_t numMessages;           /* number of included messages */
        uint32_t numMessagesLost;       /* number of lost messages for get message */
        dspipc_msgContent_t msgContent; /* describes the content type of the data */
        uint32_t reserved[1];           /* reserved field for future extension */
};

/**
 *  Create a typedef to use the DSPIPC_MESSAGE_HDR_S structure
 * @typedef dspipc_message_hdr_t
 */
typedef struct DSPIPC_MESSAGE_HDR_S dspipc_message_hdr_t;

enum DSPIPC_MSGTYPE_E {
        DSPIPC_MSGTYPE_COMMAND = 0, /* Enum for a COMMAND message Type */
        DSPIPC_MSGTYPE_STREAM = 1   /* Enum for a STREAM message Type */
};

typedef enum DSPIPC_MSGTYPE_E dspipc_msgType_t;

enum DSPIPC_MSGREPLY_E {
        DSPIPC_MSGREPLY_NO = 0, /* Enum for NO reply requested */
        DSPIPC_MSGREPLY_YES = 1 /* Enum for reply requested */
};

typedef enum DSPIPC_MSGREPLY_E dspipc_msgReply_t;

/**
 *  single message structure
 * @struct DSPIPC_MESSAGE_S
 */
struct DSPIPC_MESSAGE_S {
        uint32_t size;                 /* message data size */
        dspipc_msgType_t msgType;      /* message type (Command | Stream) */
        dspipc_msgReply_t msgReply;    /* message reply request (No reply | Req reply) */
        uint32_t cmdid;                /* command id to find related answers */
        uint32_t msgid;                /* message id, ignored for send, used for get message */
        uint32_t reserved[1];          /* reserved field for future extension */
        uint8_t data[DEVCTL_MSG_SIZE]; /* message data array */
};

/**
 *  Create a typedef to use the DSPIPC_MESSAGE_S structure
 * @typedef dspipc_message_t
 */
typedef struct DSPIPC_MESSAGE_S dspipc_message_t;

/**
 *  transmit command structure to dsp
 * @struct DSPIPC_SENDMESSAGE_S
 */
struct DSPIPC_SENDMESSAGE_S {
        dspipc_message_hdr_t hdr;                   /* header of the message packet */
        dspipc_message_t message[MAX_NUM_MESSAGES]; /* Included messages */
};

/**
 *  Create a typedef to use the DSPIPC_SENDMESSAGE_S structure
 * @typedef dspipc_sendMessage_t
 */
typedef struct DSPIPC_SENDMESSAGE_S dspipc_sendMessage_t;

/**
 *  get data out of receive queue
 * @struct DSPIPC_GETMESSAGE_S
 */
struct DSPIPC_GETMESSAGE_S {
        dspipc_message_hdr_t hdr; /* header of the message packet */
        union {
                struct DSPIPC_MESSAGE_S message[MAX_NUM_MESSAGES]; /* Included standard messages */
                struct DSPIPC_SENDRECVSTREAMMESSAGE_S bufComplete; /* Included buffer complete message       */
        };
};

/**
 *  Create a typedef to use the DSPIPC_GETMESSAGE_S structure
 * @typedef dspipc_getMessage_t
 */
typedef struct DSPIPC_GETMESSAGE_S dspipc_getMessage_t;

/**
 * Coding of the different special command codes to avoid a huge amount of devctl codes
 * @enum DSPIPC_SPECIALCMD_E
 */
typedef enum DSPIPC_SPECIALCMD_E {
        UNBLOCK = 0,    /* Unblock the device (out of a get message state) */
        CLEARQUEUE = 1, /* Clear the message queue */
        DRAIN = 2       /* Execute drain sequence on the device */
} dspipc_specialCmd_t;

/**
 *  Structure to be passed with the specialcmd message devctl
 * @struct DSPIPC_SPECIALCMDMESSAGE_S
 */
struct DSPIPC_SPECIALCMDMESSAGE_S {
        dspipc_specialCmd_t specialCmdCode; /* command code of this special message */
        uint32_t reserved[1];               /* reserved field for future extension */
};

/**
 *  Create a typedef to use the DSPIPC_SPECIALCMDMESSAGE_S structure
 * @typedef dspipc_specialCmdMessage_t
 */
typedef struct DSPIPC_SPECIALCMDMESSAGE_S dspipc_specialCmdMessage_t;

/**
 *  Structure to be passed with the set send priority message devctl
 * @struct DSPIPC_REBOOT_MESSAGE_S
 */
struct DSPIPC_REBOOT_MESSAGE_S {
        uint32_t reserved[1]; /* reserved field for future extension */
};

/**
 *  Create a typedef to use the DSPIPC_REBOOT_MESSAGE_S structure
 * @typedef dspipc_rebootMessage_t
 */
typedef struct DSPIPC_REBOOT_MESSAGE_S dspipc_rebootMessage_t;

/**
 *  DSP IMAGE Version Message
 * @struct DSPIPC_DSP_IMAGE_VERION_MESSAGE_S
 */
struct DSPIPC_DSP_IMAGE_VERION_MESSAGE_S {
        uint8_t data[DEVCTL_MSG_SIZE]; /* message data array (max. 256 bytes) */
};

/**
 *  Create a typedef to use the DSPIPC_DSP_IMAGE_VERION_MESSAGE_S structure
 * @typedef dspipc_dsp_image_version_message_t
 */
typedef struct DSPIPC_DSP_IMAGE_VERION_MESSAGE_S dspipc_dsp_image_version_message_t;

/**
 * Specifiy dspipc devctl reading the driver version
 */
#define DSPIPC_GETDRVVERS _IOR(_DCMD_MISC, DSPIPC_CMD_CODE + 0, dspipc_drvVers_t)

/**
 * Specifiy dspipc devctl reading the driver version
 */
#define DSPIPC_GETDSPSWVERS _IOR(_DCMD_MISC, DSPIPC_CMD_CODE + 1, dspipc_dspSwVers_t)

/**
 * Specifiy dspipc devctl for request new shmem object
 */
#define DSPIPC_REQMEMOBJ _IOWR(_DCMD_MISC, DSPIPC_CMD_CODE + 2, dspipc_reqMemObj_t)

/**
 * Specifiy dspipc devctl for delete new shmem object
 */
#define DSPIPC_DELMEMOBJ _IOWR(_DCMD_MISC, DSPIPC_CMD_CODE + 3, dspipc_delMemObj_t)

/**
 * Specifiy dspipc devctl for sending a message to the dsp
 */
#define DSPIPC_SENDMESSAGE _IOW(_DCMD_MISC, DSPIPC_CMD_CODE + 4, dspipc_sendMessage_t)

/**
 * Specifiy dspipc devctl for getting a message out of the receive queue from the dsp
 */
#define DSPIPC_GETMESSAGE _IOR(_DCMD_MISC, DSPIPC_CMD_CODE + 5, dspipc_getMessage_t)

/**
 * Specifiy dspipc devctl for sending a stream packet to the dsp
 */
#define DSPIPC_SENDSTREAMMESSAGE _IOW(_DCMD_MISC, DSPIPC_CMD_CODE + 6, dspipc_sendStreamMessage_t)

/**
 * Specifiy dspipc devctl for receiving a stream packet from the dsp (Provide buffers)
 */
#define DSPIPC_RECVSTREAMMESSAGE _IOR(_DCMD_MISC, DSPIPC_CMD_CODE + 7, dspipc_recvStreamMessage_t)

/**
 * Specifiy dspipc devctl for sending a special cmd packet to the dsp
 */
#define DSPIPC_SPECIALCMD _IOW(_DCMD_MISC, DSPIPC_CMD_CODE + 9, dspipc_specialCmdMessage_t)

/**
 * Specifiy dspipc devctl for sending Memory Trace Print Info
 */
#define DSPIPC_MEMTRACECMD _IOW(_DCMD_MISC, DSPIPC_CMD_CODE + 10, dspipc_drvMemtrc_t)

/**
 * Specifiy dspipc devctl for rebooting the DSP
 */
#define DSPIPC_REBOOT _IOW(_DCMD_MISC, DSPIPC_CMD_CODE + 11, dspipc_rebootMessage_t)

/**
 * Specifiy dspipc devctl for getting the DSP IMAGE Version message
 */
#define DSPIPC_GET_DSP_IMAGE_VERSION _IOWR(_DCMD_MISC, DSPIPC_CMD_CODE + 12, dspipc_dsp_image_version_message_t)

/**
 *  Union containing all possible devctl message types
 * @typedef dspipc_devctl_msg_t
 */
typedef union DSPIPC_DEVCTL_MSG_U {

        dspipc_drvVers_t drvVers;                             /* data exchange structure for GETDRVVERS devctl */
        dspipc_dspSwVers_t dspSwVers;                         /* data exchange structure for GETDSPSWVERS devctl */
        dspipc_reqMemObj_t reqMemObj;                         /* data exchange structure for REQMEMOBJ devctl */
        dspipc_delMemObj_t delMemObj;                         /* data exchange structure for DELMEMOBJ devctl */
        dspipc_sendMessage_t sendMessage;                     /* data exchange structure for SENDMESSAGE devctl */
        dspipc_getMessage_t getMessage;                       /* data exchange structure for GETMESSAGE devctl */
        dspipc_sendStreamMessage_t sendStreamMessage;         /* data exchange structure for SENDSTREAMMESSAGE devctl */
        dspipc_recvStreamMessage_t recvStreamMessage;         /* data exchange structure for RECVSTREAMMESSAGE devctl */
        dspipc_specialCmdMessage_t specialCmdMessage;         /* data exchange structure for SPECIALCMDMESSAGE devctl */
        dspipc_drvMemtrc_t prtWay;                            /* data exchange structure for DRVMEMTRC devctl */
        dspipc_rebootMessage_t rebootMessage;                 /* data exchange structure for DSPIPC_REBOOT_MESSAGE devctl */
        dspipc_dsp_image_version_message_t image_version_msg; /* data exchange structure for DSPIPC_GET_DSP_IMAGE_VERSION devctl */
} dspipc_devctl_msg_t;

enum DSPIPC_PULSE_CODES_E {
        INTHREAD_INVALID = 0,
        INTTHREAD_INT = 1,       /* Pulse code used to signalize normal interrupt */
        INTTHREAD_TERMINATE = 2, /* Pulse code used to signalize interrupt thread termination */
        CL_USER_1 = 3,           /* Pulse code used to signalize Read-DMA ready */
        INTTHREAD_SEND = 4       /* Pulse code used to signalize new scan of queues */
};

#endif /*_DSPIPC_H_INCLUDED_ */
