/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         cobs.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * Custom implementation of consistent overhead byte stuffing,
 * which includes support of encoding and decoding partial
 * messages and internal checks to prevent buffer overflow
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "cobs.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define COBS_END_OF_FRAME (0u)
#define COBS_ZERO_CONT_CODE (255u)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
/**
 * cobs_enc_init() - Initializes cobs encoder struct.
 * @cobs: Driver context.
 * @buf: Pionter to output data buffer.
 * @size: Length of output buffer.
 *
 * This function initializes the encoder structure with initial values and
 * output buffer configuration.
 *
 * Return: None.
 */
void cobs_enc_init(struct cobs_enc_buf *cobs, uint8_t *buf, uint32_t size)
{
	cobs->buf = buf;
	cobs->buf_size = size;
	cobs->code = 1;
	cobs->code_ptr = cobs->buf;
	cobs->count = 1;
}

/**
 * cobs_enc() - Encodes as cob byte stream.
 * @cobs: Structure containing encode contextual data.
 * @in: Input data buffer.
 * @len: Length of input buffer.
 *
 * This function encodes a cobs byte stream with . Please note that
 * cobs_enc_init must be called when beginning decoding of a new message.
 * cobs_enc_finish must be called when finishing a message. It supports
 * encoding a message with partial chunks.
 *
 * Return:
 * * COBS_OK         - Operation successful.
 * * COBS_ERR_BUF_SZ - Output buffer is too small.
 */
int8_t cobs_enc(struct cobs_enc_buf *cobs, const uint8_t *in, uint32_t len)
{
	const uint8_t *end = in + len;
	uint8_t *out = cobs->buf + cobs->count;
	uint8_t *out_end = cobs->buf + cobs->buf_size;

	while (in < end) {
		if (out >= out_end) {
			return COBS_ERR_BUF_SZ;
		} else if (cobs->code != COBS_ZERO_CONT_CODE) {
			uint8_t byte = *in++;

			if (byte != COBS_END_OF_FRAME) {
				*out++ = byte;
				cobs->code++;
			} else {
				*cobs->code_ptr = cobs->code;
				cobs->code = 1;
				cobs->code_ptr = out++;
			}
		} else {
			*cobs->code_ptr = cobs->code;
			cobs->code = 1;
			cobs->code_ptr = out++;
		}
	}
	cobs->count = out - cobs->buf;

	return COBS_OK;
}

/**
 * cobs_enc_finish() - Finishes a cobs encoding stream.
 * @cobs: Structure containing encode contextual data.
 *
 * Return:
 * * COBS_OK         - Operation successful.
 * * COBS_ERR_BUF_SZ - Output buffer is too small.
 */
int8_t cobs_enc_finish(struct cobs_enc_buf *cobs)
{
	uint8_t *data = cobs->buf + cobs->count;
	uint8_t *data_end = cobs->buf + cobs->buf_size;

	if (data < data_end) {
		*data = COBS_END_OF_FRAME;
		*cobs->code_ptr = cobs->code;
		cobs->count++;
	} else {
		return COBS_ERR_BUF_SZ;
	}

	return COBS_OK;
}

/**
 * cobs_dec_init() - Initializes COBS decoder stream.
 * @cobs: Driver context.
 * @buf: Pointer to output data buffer.
 * @size: Length of output buffer.
 *
 * This function initializes the decoder stream structure with initial values
 * and output buffer configuration.
 *
 * Return: None.
 */
void cobs_dec_init(struct cobs_dec_buf *cobs, uint8_t *buf, uint32_t size)
{
	cobs->buf = buf;
	cobs->buf_size = size;
	cobs->code = 0;
	cobs->code_backup = COBS_ZERO_CONT_CODE;
	cobs->count = 0;
}

/**
 * cobs_dec() - Decodes COBS a byte stream.
 * @cobs: Structure containing encode contextual data.
 * @in: Input data buffer.
 * @len_ptr: Length of input buffer and returns amount of data decoded.
 *
 * This function decodes a COBS byte stream. Please note that cobs_dec_init
 * must be called when beginning decoding of a new message. Decoding will stop
 * upon completion of a message or upon encountering an error. It is possible
 * that only a partial amount of the input buffer has been consumed and the
 * application should closely pay attention to the value returned by len_ptr
 * and the returned error code.
 *
 * Return:
 * * COBS_DEC_INCOMP - Message is incomplete. More input data is expected to
 *                     finish the message.
 * * COBS_OK         - A message has been successfully decoded and is available
 *                     in the output buffer.
 * * COBS_ERR_BUF_SZ - Output buffer is too small.
 * * COBS_ERR_DEC    - Encoding error.
 */
int8_t cobs_dec(struct cobs_dec_buf *cobs, const uint8_t *in, uint32_t *len_ptr)
{
	int8_t stat = COBS_DEC_INCOMP;
	const uint8_t *in_beg = in;
	const uint8_t *in_end = in + *len_ptr;
	uint8_t *out = cobs->buf + cobs->count;
	uint8_t *out_end = cobs->buf + cobs->buf_size;

	while (in < in_end) {
		if (cobs->code) {
			if (out < out_end) {
				uint8_t val = *in++;

				cobs->code--;
				if (val != COBS_END_OF_FRAME) {
					*out++ = val;
				} else {
					stat = COBS_ERR_DEC;
					break;
				}
			} else {
				stat = COBS_ERR_BUF_SZ;
				break;
			}
		} else {

			uint8_t val = *in++;

			if (val == COBS_END_OF_FRAME) {
				stat = COBS_OK;
				break;
			} else if (cobs->code_backup != COBS_ZERO_CONT_CODE) {
				if (out < out_end) {
					*out++ = COBS_END_OF_FRAME;
				} else {
					stat = COBS_ERR_BUF_SZ;
					break;
				}
			}
			cobs->code = val - 1;
			cobs->code_backup = val;
		}
	}
	cobs->count = out - cobs->buf;
	*len_ptr = in - in_beg;
	return stat;
}
