/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_phys_uart.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * Physical Layer source file.
 * The Physical layer interacts directly with UART driver and FS3IPC Data
 * Link Layer, the layers connection are defined by configuration definition,
 * and it defines the function to call properly.
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_phys.h"
#include "cobs.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#ifdef LOG_MODULE
#undef LOG_MODULE
#endif
#define LOG_MODULE "fs3ipc_phys"

#define FS3IPC_PHY_WRITE_RETRY_CNT             (5u)
#define FS3IPC_PHY_WRITE_RETRY_SLEEP_MSEC      (1u)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
/** context block for physical layer*/
struct phys_ctx {
	uint8_t enc_buf[COBS_MAX_ENCODED_SIZE(FS3IPC_TRANS_SIZE_MAX)];
	struct cobs_enc_buf cobs_enc;

	fs3ipc_u8 dec_buf[FS3IPC_TRANS_SIZE_MAX];
	struct cobs_dec_buf cobs_dec;

	void *user_ptr;
	uint8_t cobs_log_transfers;
	uint8_t hdlc_log_transfers;
};

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
static struct phys_ctx phys_ctxs[FS3IPC_NUM_OF_INSTANCES];

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
/**
 * fs3ipc_phys_uart_Init() - Initialize the uart physical layer.
 * @handle: Driver handle.
 * @ptr: User pointer to be passed when writing out.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Error.
 */
fs3ipc_StatusType fs3ipc_phys_uart_Init(fs3ipc_handleType handle, void *ptr)
{
	struct phys_ctx *ctx;

	if (handle >= FS3IPC_NUM_OF_INSTANCES)
		return fs3ipc_StatusType_ErrorHandle;
	ctx = &phys_ctxs[handle];

	cobs_dec_init(&ctx->cobs_dec, ctx->dec_buf, sizeof(ctx->dec_buf));
	ctx->user_ptr = ptr;
	ctx->cobs_log_transfers = FS3IPC_FALSE;
	ctx->hdlc_log_transfers = FS3IPC_FALSE;
	return fs3ipc_StatusType_OK;
}

/**
 * fs3ipc_phys_uart_Encode() - Encodes and writes out an COBS encoded input
 *                             data stream.
 * @handle: Driver handle.
 * @dataPtr: Output data.
 * @length: Length of output buffer.
 *
 * This encodes and writes out an COBS encoded input data stream. A maximum
 * unencoded message size of FS3IPC_TRANS_SIZE_MAX is supported.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_phys_uart_Encode(fs3ipc_handleType handle,
					  fs3ipc_dataPtr dataPtr,
					  fs3ipc_length length)
{
	int ret = 0;
	unsigned int retry = 0u;
	unsigned int tx_count = 0u;
	struct phys_ctx *ctx;
	uint8_t *buf = NULL;
	fs3ipc_s8 cobs_stat = COBS_OK;

	if (handle >= FS3IPC_NUM_OF_INSTANCES)
		return fs3ipc_StatusType_ErrorHandle;

	ctx = &phys_ctxs[handle];
	LOG_DATA(ctx->hdlc_log_transfers, dataPtr, length, LOG_DATA_MAX_BYTES,
		 "COBS encode input Tx (L%03u)", length);

	cobs_enc_init(&ctx->cobs_enc, ctx->enc_buf, sizeof(ctx->enc_buf));
	cobs_stat = cobs_enc(&ctx->cobs_enc, dataPtr, length);
	if (cobs_stat != COBS_OK)
		LOG_ERROR("%u: cobs_enc failed: %d", (unsigned int)handle,
			  (int)cobs_stat);

	cobs_stat = cobs_enc_finish(&ctx->cobs_enc);
	if (cobs_stat != COBS_OK) {
		LOG_ERROR("%u: cobs_enc_finish failed: %d", (unsigned int)handle,
			  (int)cobs_stat);
		return fs3ipc_StatusType_ErrorGen;
	}

	tx_count =  cobs_enc_get_count(&ctx->cobs_enc);
	buf = cobs_enc_get_buf(&ctx->cobs_enc);
	do {
		ret = fs3ipc_hw_write(handle, buf, tx_count, ctx->user_ptr);
		if (ret == tx_count) {
			LOG_DATA(ctx->cobs_log_transfers, buf, tx_count,
				 LOG_DATA_MAX_BYTES, "COBS encode output Tx (L%03u)",
				 tx_count);
			return fs3ipc_StatusType_OK;
		}
		if (ret > 0 && ret < tx_count) {
			LOG_ERROR("%u: Partial write to phy(attempt: %u): %d; pending: %u",
				  (unsigned int)handle, retry + 1, ret,
				  tx_count - ret);
			LOG_DATA(ctx->cobs_log_transfers, buf, tx_count,
				 LOG_DATA_MAX_BYTES, "COBS encode output Tx (L%03u)",
				 tx_count);
			tx_count -= ret;
		} else {
			LOG_ERROR("%u: Write to phy failed(attempt: %u): %d",
				  (unsigned int)handle, retry + 1, ret);
			ret = 0;
		}
		buf = &buf[ret];
		retry++;
		fs3ipc_msleep(FS3IPC_PHY_WRITE_RETRY_SLEEP_MSEC);
	} while (retry < FS3IPC_PHY_WRITE_RETRY_CNT);

	return fs3ipc_StatusType_ErrorGen;
}

/**
 * fs3ipc_RxThread() - Decodes an COBS encoded input data stream.
 * @handle: Driver handle.
 * @data: Input data.
 * @length: Length of input buffer.
 *
 * This function decodes an COBS encoded input data stream. It will
 * automatically separate data in packets and forward to the next layer. Any
 * malformed packets will result in the packet being dropped. A maximum decoded
 * message size of FS3IPC_TRANS_SIZE_MAX is supported.
 *
 * Return: None.
 */
void fs3ipc_RxThread(fs3ipc_handleType handle,
		fs3ipc_cDataPtr data, fs3ipc_length length)
{
	struct phys_ctx *ctx;
	fs3ipc_s8 cobs_stat;
	fs3ipc_cDataPtr data_end;

	if (handle >= FS3IPC_NUM_OF_INSTANCES) {
		/* This shouldn't happen. Config issue*/
		LOG_ERROR("Invalid handle received [%d]- Discarded", handle);
		return;
	}

	ctx = &phys_ctxs[handle];
	data_end = data + length;
	LOG_DATA(ctx->cobs_log_transfers, data, length, LOG_DATA_MAX_BYTES,
		"COBS decode input Rx (L%03d)",	length);

	while (data < data_end) {
		cobs_stat = cobs_dec(&ctx->cobs_dec, data, &length);

		switch (cobs_stat) {
		case COBS_DEC_INCOMP:
			/* more data is needed to decode message*/
			break;

		case COBS_OK:
			LOG_DATA(ctx->hdlc_log_transfers, cobs_dec_get_buf(&ctx->cobs_dec),
				 cobs_dec_get_count(&ctx->cobs_dec), LOG_DATA_MAX_BYTES,
				 "COBS decode output Rx (L%03d)",
				 cobs_dec_get_count(&ctx->cobs_dec));
			fs3ipc_phys_ext_Decode(handle, cobs_dec_get_buf(&ctx->cobs_dec),
					       cobs_dec_get_count(&ctx->cobs_dec));
			cobs_dec_init(&ctx->cobs_dec, ctx->dec_buf, sizeof(ctx->dec_buf));
			break;

		default:
			LOG_ERROR("cobs_dec: %d, %u", cobs_stat, handle);
			cobs_dec_init(&ctx->cobs_dec, ctx->dec_buf, sizeof(ctx->dec_buf));
			break;
		}

		data += length;
		length = data_end - data;
	}
}

/**
 * fs3ipc_phys_setLogging() - Enables and disabled logging of transfers.
 * @handle: Driver handle.
 * @enable: 0 to disable logging otherwise enable logging.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Error.
 */
fs3ipc_StatusType fs3ipc_phys_setLogging(fs3ipc_handleType handle,
		uint8_t enable)
{
	if (handle >= FS3IPC_NUM_OF_INSTANCES)
		return fs3ipc_StatusType_ErrorHandle;

	phys_ctxs[handle].cobs_log_transfers = enable ? FS3IPC_TRUE : FS3IPC_FALSE;
	return fs3ipc_StatusType_OK;
}

/**
 * fs3ipc_hdlc_setLogging() - Enables and disabled logging of hdlc data.
 * @handle: Driver handle.
 * @enable: 0 to disable logging otherwise enable logging.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Error.
 */
fs3ipc_StatusType fs3ipc_hdlc_setLogging(fs3ipc_handleType handle,
		uint8_t enable)
{
	if (handle >= FS3IPC_NUM_OF_INSTANCES)
		return fs3ipc_StatusType_ErrorHandle;

	phys_ctxs[handle].hdlc_log_transfers = enable ? FS3IPC_TRUE : FS3IPC_FALSE;
	return fs3ipc_StatusType_OK;
}
