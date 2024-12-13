/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_phys.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * Physical Layer header file
 *****************************************************************************/

#ifndef FS3IPC_PHYS_H
#define FS3IPC_PHYS_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
fs3ipc_StatusType fs3ipc_phys_uart_Init(fs3ipc_handleType handle, void *ptr);
fs3ipc_StatusType fs3ipc_phys_uart_Encode(fs3ipc_handleType handle,
					  fs3ipc_dataPtr dataPtr,
					  fs3ipc_length length);
void fs3ipc_RxThread(fs3ipc_handleType handle, fs3ipc_cDataPtr data,
		     fs3ipc_length length);
fs3ipc_StatusType fs3ipc_phys_setLogging(fs3ipc_handleType handle,
					 uint8_t enable);
fs3ipc_StatusType fs3ipc_hdlc_setLogging(fs3ipc_handleType handle,
					 uint8_t enable);

#endif /* FS3IPC_FS3IPC_PHYS_H */
