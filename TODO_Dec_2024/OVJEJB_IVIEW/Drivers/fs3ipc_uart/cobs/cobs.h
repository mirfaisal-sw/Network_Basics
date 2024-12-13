/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         cobs.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * Custom implementation of consistent overhead byte stuffing,
 * which includes support of encoding and decoding partial
 * messages and internal checks to prevent buffer overflow
 *****************************************************************************/

#ifndef COBS_H
#define COBS_H

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

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
/** Minimum number of encoded bytes per additional overhead byte*/
#define COBS_MIN_BYTES_PER_OH_BYTE (254)

/*
 * Helper macro which calculates the maximum number of bytes required to encode
 * a message of a given size
 */
#define COBS_MAX_ENCODED_SIZE(size) ((size) + (2) + \
	((size) / COBS_MIN_BYTES_PER_OH_BYTE))

/** operation successful*/
#define COBS_OK (0)
/** general error*/
#define COBS_ERR_GEN (-1)
/** error decoding COBS stream*/
#define COBS_ERR_DEC (-2)
/** output buffer of insufficient size*/
#define COBS_ERR_BUF_SZ (-3)
/** decoding COBS stream message is incomplete. More data is expected*/
#define COBS_DEC_INCOMP (1)

#define cobs_enc_get_buf(cobs) ((cobs)->buf)
#define cobs_enc_get_count(cobs) ((cobs)->count)
#define cobs_dec_get_buf(cobs) ((cobs)->buf)
#define cobs_dec_get_count(cobs) ((cobs)->count)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
/** context data req. for encoding a COBS stream. must call cobs_enc_init()*/
struct cobs_enc_buf {
	uint8_t *buf;
	uint32_t buf_size;
	uint32_t count;
	uint8_t *code_ptr;
	uint8_t  code;
};

/** context data req. for decoding a COBS stream. must call cobs_dec_init()*/
struct cobs_dec_buf {
	uint8_t *buf;
	uint32_t buf_size;
	uint32_t count;
	uint8_t  code;
	uint8_t  code_backup;
};

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
void cobs_enc_init(struct cobs_enc_buf *cobs, uint8_t *buf, uint32_t size);
int8_t cobs_enc(struct cobs_enc_buf *cobs, const uint8_t *in, uint32_t len);
int8_t cobs_enc_finish(struct cobs_enc_buf *cobs);
void cobs_dec_init(struct cobs_dec_buf *cobs, uint8_t *buf, uint32_t size);
int8_t cobs_dec(struct cobs_dec_buf *cobs, const uint8_t *in, uint32_t *len);

#ifdef __cplusplus
}
#endif

#endif /* COBS_H */
