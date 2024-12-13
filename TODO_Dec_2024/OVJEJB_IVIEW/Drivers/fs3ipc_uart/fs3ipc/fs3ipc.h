/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * FS3IPC header file.
 *****************************************************************************/

#ifndef FS3IPC_H
#define FS3IPC_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_types.h"

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
 * EXTERNAL VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
/**
 * fs3ipc_init() - Initialize all configured client/channel.
 * @instance: FS3IPC instance.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK
 * * fs3ipc_StatusType_ErrorHandle - Invalid instance or FC config.
 * * fs3ipc_StatusType_ErrorCfg    - Invalid configuration.
 */
fs3ipc_StatusType fs3ipc_init(unsigned int instance);

/**
 * fs3ipc_open_ch() - Open FS3IPC channel.
 * @handle: Client handle.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorHandle      - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen         - Invalid FC config or channel.
 * * fs3ipc_StatusType_ErrorClientState - Client is already opened.
 */
fs3ipc_StatusType fs3ipc_open_ch(unsigned int handle);

/**
 * fs3ipc_close_ch() - Close FS3IPC channel.
 * @handle: Client handle.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorHandle      - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen         - Invalid FC config or channel.
 * * fs3ipc_StatusType_ErrorClientState - Client is already closed.
 */
fs3ipc_StatusType fs3ipc_close_ch(unsigned int handle);

/**
 * fs3ipc_write_msg() - Write data to FS3IPC channel.
 * @handle: Client handle.
 * @msg: Message to write.
 * @length: Length of message in bytes.
 *
 * It is assumed that the @msg is of at least @length byte sized.
 *
 * Return:
 * * fs3ipc_StatusType_OK                  - OK.
 * * fs3ipc_StatusType_ErrorHandle         - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen            - Invalid @msg or channel or FC
 *                                           config or Tx-Q error.
 * * fs3ipc_StatusType_ChannelNotAvailable - Channel not available.
 * * fs3ipc_StatusType_MessageSize         - Invalid message size.
 * * fs3ipc_StatusType_BufferFull          - Tx-Q full.
 */
fs3ipc_StatusType fs3ipc_write_msg(unsigned int handle, const uint8_t *msg,
				   uint32_t length);

/**
 * fs3ipc_read_msg() - Read data from FS3IPC channel.
 * @handle: Client handle.
 * @msg: To store read message.
 * @length: Length of message in bytes.
 *
 * It is assumed that the @msg is of at least @length byte sized.
 *
 * Return:
 * * fs3ipc_StatusType_OK                - OK.
 * * fs3ipc_StatusType_ErrorHandle       - Invalid client handle.
 * * fs3ipc_StatusType_MessageSize       - Invalid message size.
 * * fs3ipc_StatusType_BufferEmpty       - Rx-Q empty.
 * * fs3ipc_StatusType_ErrorGen          - Invalid @msg or Rx-Q error.
 */
fs3ipc_StatusType fs3ipc_read_msg(unsigned int handle, uint8_t *msg,
				  uint32_t *length);

/**
 * fs3ipc_wait_txq_space() - Wait for free space in FS3IPC channel Tx-Q.
 * @handle: Client handle.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorHandle      - Invalid client handle.
 * * fs3ipc_StatusType_ErrorInterrupted - Waiting interrupted.
 * * fs3ipc_StatusType_ErrorGen         - General error.
 */
fs3ipc_StatusType fs3ipc_wait_txq_space(unsigned int handle);

/**
 * fs3ipc_wait_rxq_msg() - Wait for message in FS3IPC channel Rx-Q.
 * @handle: Client handle.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorHandle      - Invalid client handle.
 * * fs3ipc_StatusType_ErrorInterrupted - Waiting interrupted.
 * * fs3ipc_StatusType_ErrorGen         - General error.
 */
fs3ipc_StatusType fs3ipc_wait_rxq_msg(unsigned int handle);

/**
 * fs3ipc_wait_txq_space_timeout() - Wait for free space in FS3IPC channel Tx-Q
 *                                   with timeout.
 * @handle: Client handle.
 * @timeout_msec: Timeout in msecs.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorHandle      - Invalid client handle.
 * * fs3ipc_StatusType_ErrorTimeout     - Timeout and no free space.
 * * fs3ipc_StatusType_ErrorInterrupted - Waiting interrupted.
 * * fs3ipc_StatusType_ErrorGen         - General error.
 */
fs3ipc_StatusType fs3ipc_wait_txq_space_timeout(unsigned int handle,
						uint32_t timeout_msec);

/**
 * fs3ipc_wait_rxq_msg_timeout() - Wait for message in FS3IPC channel Rx-Q with
 *                                 timeout.
 * @handle: Client handle.
 * @timeout_msec: Timeout in msecs.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorHandle      - Invalid client handle.
 * * fs3ipc_StatusType_ErrorTimeout     - Timeout and no message received.
 * * fs3ipc_StatusType_ErrorInterrupted - Waiting interrupted.
 * * fs3ipc_StatusType_ErrorGen         - General error.
 */
fs3ipc_StatusType fs3ipc_wait_rxq_msg_timeout(unsigned int handle,
					      uint32_t timeout_msec);

/**
 * fs3ipc_get_ch_available() - Get the flow control status of FS3IPC channel.
 * @handle: Client handle.
 *
 * Return:
 * * fs3ipc_StatusType_OK                  - OK.
 * * fs3ipc_StatusType_ErrorHandle         - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen            - Invalid FC config or channel.
 * * fs3ipc_StatusType_ChannelNotAvailable - Channel not available.
 */
fs3ipc_StatusType fs3ipc_get_ch_available(unsigned int handle);

/**
 * fs3ipc_get_txq_message_pending() - Get the number of messages in Tx-Q.
 * @handle: Client handle.
 * @info: To store the info.
 *
 * @info will hold valid data only if 'fs3ipc_StatusType_OK' is returned.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen    - Invalid parameter or reading from Tx-Q
 *                                   failed.
 */
fs3ipc_StatusType fs3ipc_get_txq_msg_pending(unsigned int handle, uint32_t *info);

/**
 * fs3ipc_get_rxq_msg_pending() - Get the number of messages in Rx-Q.
 * @handle: Client handle.
 * @info: To store the info.
 *
 * @info will hold valid data only if 'fs3ipc_StatusType_OK' is returned.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen    - Invalid parameter or reading from Rx-Q
 *                                   failed.
 */
fs3ipc_StatusType fs3ipc_get_rxq_msg_pending(unsigned int handle, uint32_t *info);

/**
 * fs3ipc_get_txq_free_space() - Get amount of free space in Tx-Q in Bytes.
 * @handle: Client handle.
 * @info: To store the info.
 *
 * @info will hold valid data only if 'fs3ipc_StatusType_OK' is returned.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen    - Invalid parameter or reading from Tx-Q
 *                                   failed.
 */
fs3ipc_StatusType fs3ipc_get_txq_free_space(unsigned int handle, uint32_t *info);

/**
 * fs3ipc_get_txq_free_msg_space() - Get amount of free space in Tx-Q in msg units.
 * @handle: Client handle.
 * @info: To store the info.
 *
 * @info will hold valid data only if 'fs3ipc_StatusType_OK' is returned.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen    - Invalid parameter or reading from Tx-Q
 *                                   failed.
 */
fs3ipc_StatusType fs3ipc_get_txq_free_msg_space(unsigned int handle,
						uint32_t *info);

/**
 * fs3ipc_get_rxq_free_space() - Get amount of free space in Rx-Q in Bytes.
 * @handle: Client handle.
 * @info: To store the info.
 *
 * @info will hold valid data only if 'fs3ipc_StatusType_OK' is returned.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen    - Invalid parameter or reading from Rx-Q
 *                                   failed.
 */
fs3ipc_StatusType fs3ipc_get_rxq_free_space(unsigned int handle, uint32_t *info);

/**
 * fs3ipc_get_txq_size() - Get Tx-Q size in bytes.
 * @handle: Client handle.
 * @info: To store the info.
 *
 * @info will hold valid data only if 'fs3ipc_StatusType_OK' is returned.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen    - Invalid parameter or reading from Tx-Q
 *                                   failed.
 */
fs3ipc_StatusType fs3ipc_get_txq_size(unsigned int handle, uint32_t *info);

/**
 * fs3ipc_get_rxq_size() - Get Rx-Q size in bytes.
 * @handle: Client handle.
 * @info: To store the info.
 *
 * @info will hold valid data only if 'fs3ipc_StatusType_OK' is returned.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen    - Invalid parameter or reading from Rx-Q
 *                                   failed.
 */
fs3ipc_StatusType fs3ipc_get_rxq_size(unsigned int handle, uint32_t *info);

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_H */
