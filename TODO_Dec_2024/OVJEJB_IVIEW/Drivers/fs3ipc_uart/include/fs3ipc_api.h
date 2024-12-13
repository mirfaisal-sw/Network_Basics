/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2023
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_api.h
 * @ingroup      FS3IPC Component
 * @author       Prasad Lavande <Prasad.Lavande@harman.com>
 *               Sharon Jerome <Sharon.Jerome@harman.com>
 *
 * FS3IPC in-kernel client APIs header file.
 * This header file defines the in-kernel client APIs for communicating over
 * FS3IPC.
 *****************************************************************************/

#ifndef FS3IPC_API_H
#define FS3IPC_API_H

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <linux/types.h>
#include <linux/limits.h>

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define FS3IPC_INFINITE_TIMEOUT (U32_MAX)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
/**
 * fs3ipc_client_open() - Open FS3IPC client.
 * @instance: FS3IPC instance.
 * @ch: Channel.
 *
 * Return:
 * * >= 0    - Client handle on success.
 * * -EINVAL - Invalid argument.
 * * -EBUSY  - Device or resource busy.
 * * -EIO    - General input/output error.
 *
 * Client handle returned on success refers to a valid open FS3IPC client and
 * should be used in subsequent FS3IPC client API calls.
 */
int fs3ipc_client_open(unsigned int instance, unsigned int ch);

/**
 * fs3ipc_client_timed_read() - Read msg from FS3IPC client within a timeout.
 * @handle: Client handle.
 * @msg: To store read message.
 * @length: Length of message in bytes.
 * @timeout: Timeout in msecs.
 *
 * It is assumed that the @msg is of at least @length byte sized.
 * For infinite timeout, use 'FS3IPC_INFINITE_TIMEOUT'.
 *
 * Return:
 * * > 0     - On success, number of bytes read.
 * * 0       - On timeout and no message received.
 * * -EINVAL - Invalid argument.
 * * -EINTR  - Waiting interrupted.
 * * -EIO    - General input/output error.
 */
int fs3ipc_client_timed_read(int handle, char *msg, size_t length,
			     uint32_t timeout);

/**
 * fs3ipc_client_timed_write() - Write msg to FS3IPC client within a timeout.
 * @handle: Client handle.
 * @msg: Message to write.
 * @length: Length of message in bytes.
 * @timeout: Timeout in msecs.
 *
 * It is assumed that the @msg is of at least @length byte sized.
 * For infinite timeout, use 'FS3IPC_INFINITE_TIMEOUT'.
 *
 * Return:
 * * > 0     - On success, number of bytes written.
 * * 0       - On timeout and no space to write.
 * * -EINVAL - Invalid argument.
 * * -EINTR  - Waiting interrupted.
 * * -EIO    - General input/output error.
 */
int fs3ipc_client_timed_write(int handle, const char *msg, size_t length,
			      uint32_t timeout);

/**
 * fs3ipc_client_close() - Close FS3IPC client.
 * @client_handle: Client handle.
 *
 * Return:
 * * 0       - Success.
 * * -EINVAL - Invalid argument.
 * * -EIO    - General input/output error.
 */
int fs3ipc_client_close(int client_handle);

#endif /* FS3IPC_API_H */
