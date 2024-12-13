/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_types.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala<david.rogala@harman.com>
 *
 * fs3ipc_type header file.
 * This file used for defining the types
 *****************************************************************************/
#ifndef FS3IPC_TYPES_H
#define FS3IPC_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define FS3IPC_FALSE (0)
#define FS3IPC_TRUE (1)
#define FS3IPC_NULL ((void *)0)

#ifndef fs3ipc_StatusType
#define fs3ipc_StatusType fs3ipc_u32
#endif
/**
 * No Error condition
 */
#ifndef fs3ipc_StatusType_OK
#define fs3ipc_StatusType_OK (0)
#endif
/**
 * Error configuration error condition
 */
#ifndef fs3ipc_StatusType_ErrorGen
#define fs3ipc_StatusType_ErrorGen (1)
#endif

/**
 * Configuration error condition
 */
#ifndef fs3ipc_StatusType_ErrorCfg
#define fs3ipc_StatusType_ErrorCfg (2)
#endif

/**
 * Timeout occurred
 */
#ifndef fs3ipc_StatusType_ErrorTimeout
#define fs3ipc_StatusType_ErrorTimeout (3)
#endif

/**
 * Invalid handle condition
 */
#ifndef fs3ipc_StatusType_ErrorHandle
#define fs3ipc_StatusType_ErrorHandle (4)
#endif

/**
 * message doesn't fit in current buffer error condition
 */
#ifndef fs3ipc_StatusType_BufferFull
#define fs3ipc_StatusType_BufferFull (5)
#endif

/**
 * Invalid frame size detected
 */
#ifndef fs3ipc_StatusType_Invalid_Frame_Size
#define fs3ipc_StatusType_Invalid_Frame_Size (6)
#endif

/**
 *
 */
#ifndef fs3ipc_StatusType_ErrorLostMessage
#define fs3ipc_StatusType_ErrorLostMessage (7)
#endif

/**
 * Buffer empty error detected
 */
#ifndef fs3ipc_StatusType_BufferEmpty
#define fs3ipc_StatusType_BufferEmpty (8)
#endif

/**
 *Channel not available, the Flowcontrol message has not
 *been enable the Channel
 */
#ifndef fs3ipc_StatusType_ChannelNotAvailable
#define fs3ipc_StatusType_ChannelNotAvailable (9)
#endif

/**
 * Invalid length detected
 */
#ifndef fs3ipc_StatusType_MessageSize
#define fs3ipc_StatusType_MessageSize (10)
#endif

/**
 * Transmission Window is full, normally the transmission window is full
 * when there is no received any ACK message
 */
#ifndef fs3ipc_StatusType_Tx_WindowFull
#define fs3ipc_StatusType_Tx_WindowFull (11)
#endif

/**
 *The client already enabled. only one client allowed
 */
#ifndef fs3ipc_StatusType_ErrorClientState
#define fs3ipc_StatusType_ErrorClientState (12)
#endif

/**
 * Invalid CRC detected
 */
#ifndef fs3ipc_StatusType_Rx_InvalidCRC
#define fs3ipc_StatusType_Rx_InvalidCRC (13)
#endif

/**
 * Invalid initialization detected
 */
#ifndef fs3ipc_StatusType_NOINIT
#define fs3ipc_StatusType_NOINIT (14)
#endif

/**
 * Invalid initialization detected
 */
#ifndef fs3ipc_StatusType_ErrorInterrupted
#define fs3ipc_StatusType_ErrorInterrupted (15)
#endif

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
typedef uint8_t fs3ipc_handleType;
typedef uint8_t *fs3ipc_dataPtr;
typedef uint16_t *fs3ipc_dataPtr16;
typedef const uint8_t *fs3ipc_cDataPtr;
typedef uint32_t fs3ipc_length;

typedef uint32_t fs3ipc_u32;
typedef uint16_t fs3ipc_u16;
typedef uint8_t fs3ipc_u8;
typedef int8_t fs3ipc_s8;
typedef int32_t fs3ipc_s32;
typedef int16_t fs3ipc_s16;

/**
 * UNUSED fs3ipc_fp_GetBuffer function pointer
 */
typedef fs3ipc_StatusType(*fs3ipc_fp_GetBuffer)(fs3ipc_handleType,
		fs3ipc_dataPtr *,
		fs3ipc_length *);

/**
 * UNUSED fs3ipc_fp_EncodeMessage function pointer
 */
typedef fs3ipc_StatusType(*fs3ipc_fp_EncodeMessage)(fs3ipc_handleType,
		fs3ipc_dataPtr,
		fs3ipc_length *);

/**
 * UNUSED fs3ipc_fp_DecodeMessage function pointer
 */
typedef fs3ipc_StatusType(*fs3ipc_fp_DecodeMessage)(fs3ipc_handleType,
	fs3ipc_cDataPtr,
	fs3ipc_length);

/**
 * Function pointer called when flowcontrol message is received
 */
typedef void (*fs3ipc_fp_flowControlCb)(fs3ipc_u8, fs3ipc_u8);

/* Forward declaration of timer instance type */
struct fs3ipc_timer;

/**
 * fs3ipc_timer_callback_t - Timer expiry callback.
 * @timer: FS3IPC timer.
 *
 * Context: (Interrupt based timers)Interrupt context.
 *          (Periodic timer handler)Caller holds timer lock.
 *
 * Return: None.
 */
typedef void (*fs3ipc_timer_callback_t)(const struct fs3ipc_timer *timer);

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_TYPES_H_ */
