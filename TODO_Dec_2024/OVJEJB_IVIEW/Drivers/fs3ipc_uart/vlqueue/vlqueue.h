/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         vlqueue.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * Variable Length Queue. This module implements an efficient variable length
 * queue where messages are variable length. The queue operates as FIFO, where
 * messages must be read in the same order which they are queued.
 *****************************************************************************/

#ifndef SRC_VLQUEUE_H
#define SRC_VLQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#ifdef LINUX_KERNEL_INC
#include <linux/types.h>
#else
#include <stdint.h>
#endif
#include "vlqueue_cfg.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
/* IMPORTANT: Update module version when revisions are changed.
 * BCD: 00[major-2][minor-2][patch-2]
 */
#define VLQUEUE_MODULE_VERSION_BCD  (0x010001)

#define VLQUEUE_OK                  (0)
#define VLQUEUE_ERR_PARAM           (-1)
#define VLQUEUE_ERR_QUEUE_FULL      (-2)
#define VLQUEUE_ERR_QUEUE_EMTPY     (-3)
#define VLQUEUE_ERR_READ_BUFFER_SZ  (-4)
#define VLQUEUE_ERR_RANGE           (-5)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
typedef struct{
	uint8_t *buffer;
#if VLQUEUE_16_BIT_LEN == 1
	uint16_t bufferLength;
	uint16_t writeIndex;
	uint16_t readIndex;
	uint16_t msgCnt;
#else
	uint32_t bufferLength;
	uint32_t writeIndex;
	uint32_t readIndex;
	uint32_t msgCnt;
#endif
#if VLQUEUE_ENABLE_USER_PTR == 1
	void *usrPtr;
#endif
} VLQueue_ccb_t;

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
/******************************************************************************
 * FUNCTION:       vlqueue_getversion
 * DESCRIPTION:    This function returns the version of the vlqueue module
 * INPUT(S):       ccb: context block to be initialized (also output)
 * OUTPUT(S):      void
 * RETURN VALUE:   return: less than 0, error; else buffer free space
 *****************************************************************************
 */
uint32_t vlqueue_getversion(VLQueue_ccb_t *ccb);

/******************************************************************************
 * FUNCTION:       VLQueue_Init
 * DESCRIPTION:    This function initializes a vlqueue context block
 * INPUT(S):       ccb: context block to be initialized (also output)
 *                 buffer: byte array to be used as buffer
 *                 length: length of the buffer
 *                 *usrPtr: user pointer useful for user defined synchronization
 * OUTPUT(S):      ccb: context block
 * RETURN VALUE:   return: if 0, success, failure otherwise
 *****************************************************************************
 */
#if VLQUEUE_ENABLE_USER_PTR == 1
int32_t vlqueue_init(VLQueue_ccb_t *ccb, uint8_t *buffer, uint32_t length,
		     void *usrPtr);
#else
int32_t vlqueue_init(VLQueue_ccb_t *ccb, uint8_t *buffer, uint32_t length);
#endif

/******************************************************************************
 * FUNCTION:       vlqueue_empty
 * DESCRIPTION:    This function empties the buffer
 * INPUT(S):       ccb: context block to be initialized (also output)
 * OUTPUT(S):      none
 * RETURN VALUE:   return: less than 0, error; else buffer free space
 *****************************************************************************
 */
int32_t vlqueue_empty(VLQueue_ccb_t *ccb);

/******************************************************************************
 * FUNCTION:       vlqueue_size
 * DESCRIPTION:    This function returns the number of queued messages in the
 *                 buffer
 * INPUT(S):       ccb: context block to be initialized (also output)
 * OUTPUT(S):      void
 * RETURN VALUE:   return: less than 0, error; else number of messages queued
 *****************************************************************************
 */
int32_t vlqueue_size(VLQueue_ccb_t *ccb);

/******************************************************************************
 * FUNCTION:       vlqueue_front
 * DESCRIPTION:    This function reads the message at the front of the buffer,
 *                 without removing it
 * INPUT(S):       ccb: context block to be initialized (also output)
 *                 bufSize: size of output buffer
 * OUTPUT(S):      *buf: output buffer
 * RETURN VALUE:   return: less than 0, error; else the size of the message read
 *****************************************************************************
 */
int32_t vlqueue_front(VLQueue_ccb_t *ccb, uint8_t *buf, uint32_t bufSize);

/******************************************************************************
 * FUNCTION:       vlqueue_back
 * DESCRIPTION:    This function reads the message from the back of the buffer,
 *                 without removing it
 * INPUT(S):       ccb: context block to be initialized (also output)
 *                 bufSize: size of output buffer
 * OUTPUT(S):      *buf: output buffer
 * RETURN VALUE:   return: less than 0, error; else the size of the message read
 *****************************************************************************
 */
int32_t vlqueue_back(VLQueue_ccb_t *ccb, uint8_t *buf, uint32_t bufSize);

/******************************************************************************
 * FUNCTION:       vlqueue_push
 * DESCRIPTION:    This function queues a variable length message at the
 *                 back of the circular buffer it free space permits
 * INPUT(S):       ccb: context block to be initialized (also output)
 *                 *msg: pointer to message
 *                 msgSize: message size
 * OUTPUT(S):      none
 * RETURN VALUE:   return: equal to zero, success; else error
 *****************************************************************************
 */
int32_t vlqueue_push(VLQueue_ccb_t *ccb, const uint8_t *msg, uint32_t msgSize);

/******************************************************************************
 * FUNCTION:       vlqueue_pop
 * DESCRIPTION:    This function reads the message at the front of the buffer,
 *                 and removes it
 * INPUT(S):       ccb: context block to be initialized (also output)
 *                 bufSize: size of output buffer
 * OUTPUT(S):      *buf: output buffer
 * RETURN VALUE:   return: less than 0, error; else the size of the message read
 *****************************************************************************
 */
int32_t vlqueue_pop(VLQueue_ccb_t *ccb, uint8_t *buf, uint32_t bufSize);

/******************************************************************************
 * FUNCTION:       VLQueue_fsize
 * DESCRIPTION:    This function returns the size of the message in the front
 *                 of the queue
 * INPUT(S):       ccb: context block to be initialized (also output)
 * RETURN VALUE:   return: less than zero, error; else size of message read
 *****************************************************************************
 */
int32_t vlqueue_fsize(VLQueue_ccb_t *ccb);

/******************************************************************************
 * FUNCTION:       vlqueue_btotal
 * DESCRIPTION:    This function returns the total size of the buffer
 * INPUT(S):       ccb: context block to be initialized (also output)
 * OUTPUT(S):      void
 * RETURN VALUE:   return: less than 0, error; else buffer free space
 *****************************************************************************
 */
int32_t vlqueue_btotal(VLQueue_ccb_t *ccb);

/******************************************************************************
 * FUNCTION:       vlqueue_bfree
 * DESCRIPTION:    This function returns the number of free bytes remaining in
 *                 the buffer
 * INPUT(S):       ccb: context block to be initialized (also output)
 * OUTPUT(S):      void
 * RETURN VALUE:   return: less than 0, error; else buffer free space
 ******************************************************************************/
int32_t vlqueue_bfree(VLQueue_ccb_t *ccb);

/**
 * vlqueue_mfree() - This function returns the number of free bytes remaining
 *                   for atleast single message in buffer
 * @ccb: context block to be initialized
 * @max_msg_size: maximum message size possible
 *
 * Return:
 * >= 0              - Buffer free space on success
 * VLQUEUE_ERR_PARAM - Invalid ccb
 * VLQUEUE_ERR_RANGE - max_msg_size is out of range
 */
int32_t vlqueue_mfree(VLQueue_ccb_t *ccb, uint32_t max_msg_size);

#ifdef __cplusplus
}
#endif

#endif /* SRC_VLQUEUE_H */
