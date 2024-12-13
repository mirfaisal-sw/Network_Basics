/**************************************************************
 * Project        Harman Car Multimedia System
 * (c) Copyright  2005
 * Company        Harman/Becker Automotive Systems GmbH
 *                  All rights reserved
 **************************************************************/
/**
 * @file       dra300emificommstructs.h
 *
 * This file declares the communication structures in the DSP that
 * are used to establish a IPC mechanism over PCI. 
 *
 * @author          Uli Mohr
 *                  MSchweikhardt
 */

/** @defgroup dm642mmipc TI DM642 MMIPC library implementation */

#ifndef __DRA300EMIFCOMMUNICATIONSTRUCTURES_H__
#define __DRA300EMIFCOMMUNICATIONSTRUCTURES_H__

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

/** @ingroup dra300mmipc */
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
 * @brief Channel number mask
 * 
 * The channel number is not a 32 bit value, but only consists of the lower 30 bits,
 * while the upper two bits in the channelId determine the type of the channel.
 * This mask can be used to get the channel number 
 */
#define DSPIPC_CHANNELNUMBER_MSK (0x3fff0000U)

/**
 * @brief Channel number bit position
 * 
 * This number gives the channel number bit position inside the mailbox register. 
 * To get the value of the channel number, AND-it with the mask, then shift it 
 * by this byte postion. 
 */
#define DSPIPC_CHANNELNUMBER_BITPOS (16)

/** 
 * @brief Message length bit mask
 * 
 * The message length is a subfield in the media mailbox register. The value gives the 
 * mask which to use to extract the message length from the media mailbox value. 
 * To get the length, AND the mailbox value with the bitmask, then shift it by the 
 * bitposition specified below to the rigth.
 */
#define DSPIPC_MESSAGELENGHT_MSK (0x0000ffffU)

/**
 * @brief Message length bit position
 * 
 * The message length is a subfiled in the media mailbox register. The value gives the 
 * postion of the LSB of the length in the media mailbox value. 
 * To get the length, AND the mailbox value with the bitmask specified above and 
 * shift it by this value to the right.
 */
#define DSPIPC_MESSAGELENGTH_BITPOS (0)

/** 
 * @brief Channel type bit postion. 
 * 
 * The channel type is a sub-field in the media mailbox register. The value gives the 
 * position of the LSB of the channel type. 
 * To get the type of the channel, AND the mailbox register value with the bitmask 
 * specified above and shift it by this value to the right. 
 */
#define DSPIPC_CHANNELTYPE_BITPOS (30)

/**
 * @brief Application startup ack value
 *
 * This value will be transmitted using the control mailbox to indicate to the 
 * SH4 that the dspipc library has been initialized and is ready to use. 
 */
#define DSPIPC_APPLICATION_STARTUP_ACK (0x1234)

/**
 * @brief Default value buffer flag
 * 
 * This value should be used to set up the flag field in the buffer structure
 * to indicate that no special flags are requested on the buffer
 */
#define SMF_NONE (0x00000000U)

/**
 * @brief Data discontinuity flag
 * 
 * If this flag is set on a stream message, it indicates that the data in this 
 * buffer will be not contiguous with the data following this buffer.
 * This will result in the completetion of the target buffers after this 
 * buffer has been processed completely, even if it is not full. 
 */
#define SMF_DATA_DISCONTINIOUS (0x00000001U)

#ifdef __cplusplus
}
#endif

#endif /* __DRA300EMIFCOMMUNICATIONSTRUCTURES_H__ */