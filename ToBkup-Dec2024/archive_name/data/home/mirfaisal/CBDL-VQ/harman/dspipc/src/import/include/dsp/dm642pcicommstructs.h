/**************************************************************
 * Project        Harman Car Multimedia System
 * (c) Copyright  2005
 * Company        Harman/Becker Automotive Systems GmbH
 *             All rights reserved
 **************************************************************/
/**
 * @file       dm642pcicommstructs.h
 *
 * This file declares the communication structures in the DSP that
 * are used to establish a IPC mechanism over PCI. 
 *
 * @author     Ulrich Mohr
 *             Markus Schweikhardt
 */

/** @defgroup dm642mmipc TI DM642 MMIPC library implementation */

#ifndef __DM642PCICOMMUNICATIONSTRUCTURES_H__
#define __DM642PCICOMMUNICATIONSTRUCTURES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*#include "publish/include/common/commstructs.h" */
#include "commstructs.h"

/** 
 * @brief Cache line size
 * 
 * The cache line size is used to assure the alignment only. 
 */
#define DSPIPC_CACHELINESIZE 128

/** @ingroup dm642mmipc */
/**
 * @brief Interface version number. 
 * 
 * This version number defines the version of the communication buffer
 * structures @ref PCIMessageEntry, @ref PCICommunicationStructures etc...
 */
/*@{*/
#define DSP_INTERFACE_VERSION_MAJOR (3)
#define DSP_INTERFACE_VERSION_MINOR (0)
#define DSP_INTERFACE_VERSION_BUGFIX (0)
#define DSP_INTERFACE_VERSION_SPECIAL (0)
/*@}*/

/** 
 * @brief COMMAND QUEUE LENGTH
 * 
 * This value defines the number of entries that the transmit buffers can 
 * hold at max. 
 */
#define COMMANDQUEUE_LENGTH (32)

/**
 * @brief Channel number mask
 * 
 * The channel number is not a 32 bit value, but only consists of the lower 30 bits,
 * while the upper two bits in the channelId determine the type of the channel.
 * This mask can be used to get the channel number 
 */
#define DSPIPC_CHANNELNUMBER_MSK (0x3fffffffU)
#define DSPIPC_CHANNELNUMBER_SHIFT 0

/**
 * @brief Data discontinuity flag
 * 
 * If this flag is set on a stream message, it indicates that the data in this 
 * buffer will be not contiguous with the data following this buffer.
 * This will result in the completetion of the target buffers after this 
 * buffer has been processed completely, even if it is not full. 
 */
#define SMF_DATA_DISCONTINIOUS (0x00000001U)

/**
 * @brief Transfer request return
 * 
 * If this flag is set on a buffer complete stream message, it indicates that 
 * this buffer was submitted by a provide buffer request.
 */
#define SMF_PROVIDE_BUFFER_RET (0x00000002U)

/**
 * @brief Transfer request return
 * 
 * If this flag is set on a buffer complete stream message, it indicates that 
 * this buffer was submitted by a provide buffer request.
 */
#define SMF_TRANSFER_BUFFER_RET (0x00000004U)

/**
 * @brief Request terminated with drain return
 * 
 * If this flag is set on a buffer complete stream message, it indicates that 
 * this buffer was submitted by a provide buffer request.
 */
#define SMF_DRAINED (0x00000008U)

/*
 * @brief Number of ranges inside one stream message
 * 
 * This number specifies the maximum number of ranges inside one stream message
 * This number should be choosed to get nearly the same size as one command
 * message to avoid unefficient memory usage inside the queue (the size of
 * the stream message is 16 + Number of ranges * 8
 */
#define DSPIPC_NUM_RANGES 8 /*24 */

/** @ingroup dm642Queues
 * @brief DM642 message queue structures.
 */
struct _DSPIPCMessageQueue {
        /** 
    * @brief Write index
    * 
    * This index gives the position of the next empty slot you can write to
    */
        mInt32_t writeIndex;

        /**
    * @brief Wake up request
    *
    * This field indicates that, after 'wakeupRequest' packets, the dsp wants
    * to be notified.
    */
        mUInt32_t wakeupRequest;

        /**
    * @brief Cache alignment dummy
    * 
    * This dummy is used only to assure cache alignment. 
    */
        mUInt8_t alignmentDummy0[DSPIPC_CACHELINESIZE - 2 * sizeof(mInt32_t)];

        /** 
    * @brief Read index
    *
    * This index gives the position of the next occupied slot you can read from.
    */
        mInt32_t readIndex;

        /**
    * @brief Cache alignment dummy
    *
    * This dummy is used only to assure cache alignment. 
    */
        mUInt8_t alignmentDummy1[DSPIPC_CACHELINESIZE - sizeof(mInt32_t)];

        /**
    * @brief Message queue
    */
        DSPIPCMessageQueueEntry msg[COMMANDQUEUE_LENGTH];

        /**
    * @brief Cache alignment dummy
    *
    * This dummy assures that, when we write back the messages, don't affect 
    * any memory after this structure. 
    */
        mUInt8_t alignmentDummy2[DSPIPC_CACHELINESIZE - (sizeof(DSPIPCMessageQueueEntry) * COMMANDQUEUE_LENGTH) % DSPIPC_CACHELINESIZE];
};

/**
 * @brief DSPIPCMessageQueue typedef
 */
typedef struct _DSPIPCMessageQueue DSPIPCMessageQueue;

/** @ingroup dm642mmipc */
/** @defgroup dm642communicationStructures DM642 MMIPC communication structures
 * The communication structures define a common data structure that is used to 
 * access common memory for basic communication. 
 * The communication structures are located in the memory of the DSP. The entry point 
 * of the communciation structures (root pointer) is located on a 
 * fixed address location on the DSP memory. The root pointer consists
 * of a marker, indicating that the DSP mmipc module has been started, 
 * and a pointer to an arbitrary memory location, where pointers to 
 * the @ref dm642Queues can be found. 
 * @image html CommStructures.gif "Communication structures overview"
 */
/*@{*/
/**  
 * @brief DSP Communication structure 
 *
 */
struct _DSPIPCCommunicationStructure {
        /** 
    * @brief Transmit buffer. 
    *
    */

        mUInt32_t dspTxBufferAddr;

        /** 
    * @brief Receive buffer
    *
    */
        mUInt32_t dspRxBufferAddr;

        /** 
    * @brief assertion msg param
    *
    */
        mUInt32_t dspAssertLine;

        mUInt32_t dspAssertFile;

        /** This dummy assures that, when writing back the pointers at the beginning, 
    * we don't affect any other cache line
    */
        mUInt8_t alignmentDummy[DSPIPC_CACHELINESIZE - (2 * sizeof(mUInt32_t)) % DSPIPC_CACHELINESIZE];
};

/**
 * @brief DSPIPCCommunicationStructure typedef
 */
typedef struct _DSPIPCCommunicationStructure DSPIPCCommunicationStructure;

/**
 * @brief The root pointer magic
 * 
 */
#define ROOTPOINTER_MAGIC (0xdeadbeefU)

/**
 * @brief The root pointer address
 */
#define ROOTPOINTER_ADDR (0x80000000U)

/**
 * @brief Bootloader start address
 *
 * Define the address where the Bootloader acknowledge value, the DSP entrypoint and the 
 * section information can be found in the new bootloader structure
 */
#define BOOTLOADER_DATA_START_ADDR (0x00000008U)

/** 
 * @brief Root pointer structure
 * 
 * This structure defines the layout of the root pointer structure. This 
 * structure is placed to a known fixed address to be available after software
 * download. This structure will contain a magic word to signalize the startup
 * of the DSP and will also contain an address reference for the position of
 * administration structure. 
 */
struct _DSPIPCRootPointer {
        /**
    * @brief Address of the admin structure
    * 
    * This member determines the physical address of the root admin structure. 
    * On this address, you will find a ::DSPIPCCommunicationStructure
    */
        mUInt32_t rootAdminAddr;

        /**
    * @brief Magic word
    * 
    * This magic word, if set to #ROOTPOINTER_MAGIC, will indicate that the 
    * root pointer structure is valid
    */
        mUInt32_t magic;
};

/**
 * @brief Root pointer typedef
 */
typedef struct _DSPIPCRootPointer DSPIPCRootPointer;

/*@}*/

/**
 * @brief Define the acknowledge value from the DSP after downloading to guarantee that
 * no wrong software was started
 */
#define BOOTLOADER_ACK_VALUE 0x8A6E4F19U

/*
 * @brief Number of regions inside the bootloader data field
 */
#define NUM_REGIONS_IN_BL_DATA 8

/** 
 * @brief Define the data to be delivered by the bootloader
 * @enum BOOTLOADER_DATA_S
 */
struct BOOTLOADER_DATA_S {
        uint32_t entryPoint;  /* field to write the entry point address, has to be zero after reset */
        uint32_t acknowledge; /* magic value written by the bootloader at the end */
        struct
        {
                uint32_t start;           /* start address of a valid download area */
                uint32_t end;             /* end address of a valid download area */
        } region[NUM_REGIONS_IN_BL_DATA]; /* valid download regions */
};

/** 
 * @brief Create a typedef to use the BOOTLOADER_DATA_S enumeration 
 * @typedef bootLoaderData_t
 */
typedef struct BOOTLOADER_DATA_S bootLoaderData_t;

#ifdef __cplusplus
}
#endif

#endif /* __DM642PCICOMMUNICATIONSTRUCTURES_H__ */
