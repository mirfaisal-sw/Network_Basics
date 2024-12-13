/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_app.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 * @author       Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * FS3IPC App layer source file.
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_app.h"
#include "fs3ipc.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#ifdef LOG_MODULE
#undef LOG_MODULE
#endif
#define LOG_MODULE "fs3ipc_app"

/* increment statistics macro*/
#ifdef CONFIG_FS3IPC_DEBUG
#define INC_STATS(var) ((var)++)
#else
#define INC_STATS(var)
#endif

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
typedef int32_t (*fs3ipc_qinfo_t)(fs3ipc_app_queueType *app_q);

typedef int32_t (*fs3ipc_get_qinfo_t)(const fs3ipc_app_ltchancfg_t *chan_cfg,
				      fs3ipc_qinfo_t qinfo);

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
static fs3ipc_u32 handleEncodeResumeMessage(fs3ipc_dataPtr out,
	fs3ipc_s32 outLength, fs3ipc_app_message_buffer_t *partialMessage,
	const fs3ipc_fc_ltcfg_t *fcCfg);

static fs3ipc_u32 handleEncodeFlowControlMessage(fs3ipc_dataPtr out,
	fs3ipc_s32 outLength, fs3ipc_app_message_buffer_t *partialMessage,
	const fs3ipc_app_ltcfg_t *cfg);

static fs3ipc_u32 handleEncodeNextMessage(fs3ipc_dataPtr out,
	fs3ipc_s32 outLength, fs3ipc_app_message_buffer_t *partialMessage,
	const fs3ipc_app_ltchancfg_t *channelCfg,
	const fs3ipc_app_ltcfg_t *cfg);

static fs3ipc_u32 handleDecodeResumeMessage(fs3ipc_cDataPtr in,
	fs3ipc_length outLength, const fs3ipc_app_ltcfg_t *cfg,
	fs3ipc_StatusType *stat);

static fs3ipc_u32 handleDecodeNewMessage(fs3ipc_cDataPtr in,
	fs3ipc_length outLength, const fs3ipc_app_ltcfg_t *cfg,
	fs3ipc_StatusType *stat);

static fs3ipc_StatusType fs3ipc_app_set_ch_state(const struct fs3ipc_client *client,
						 uint8_t enable);

static fs3ipc_StatusType fs3ipc_set_ch_state(unsigned int handle, uint8_t enable);

static fs3ipc_StatusType fs3ipc_app_wait_event(const struct fs3ipc_client *client,
					       fs3ipc_os_event_t *wait_obj,
					       fs3ipc_os_EventMaskType event,
					       const char *name);

static fs3ipc_StatusType
fs3ipc_app_wait_event_timeout(const struct fs3ipc_client *client,
			      fs3ipc_os_event_t *wait_obj,
			      fs3ipc_os_EventMaskType event, const char *name,
			      uint32_t timeout_msec);

static fs3ipc_StatusType fs3ipc_get_qinfo(unsigned int handle,
					  fs3ipc_get_qinfo_t get_qinfo,
					  fs3ipc_qinfo_t qinfo,
					  const char *name, uint32_t *info);

static int32_t fs3ipc_get_txq_info(const fs3ipc_app_ltchancfg_t *chan_cfg,
				   fs3ipc_qinfo_t qinfo);

static int32_t fs3ipc_get_rxq_info(const fs3ipc_app_ltchancfg_t *chan_cfg,
				   fs3ipc_qinfo_t qinfo);

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
/**
 * handleEncodeResumeMessage() - Resumes/encodes general information to send.
 * @out: Buffer to store the encoded data.
 * @outLength: The total bytes to transmit.
 * @partialMessage: The partial message to decode.
 * @fcCfg: Application configuration parameter.
 *
 * Return: The total bytes copied/encoded.
 */
static fs3ipc_u32 handleEncodeResumeMessage(fs3ipc_dataPtr out,
	fs3ipc_s32 outLength, fs3ipc_app_message_buffer_t *partialMessage,
	const fs3ipc_fc_ltcfg_t *fcCfg)
{
	fs3ipc_app_messageHdr *resumeMsgHdr = (fs3ipc_app_messageHdr *)out;
	fs3ipc_length bytesCoppied;

	/* Skip the resume message header*/
	if (partialMessage->position > 0) {
		fs3ipc_length dataSize = partialMessage->message.hdr.length -
			partialMessage->position;
		fs3ipc_dataPtr inData =
			&partialMessage->message.data[partialMessage->position];
		fs3ipc_u8 msgOverflow;

		LOG_DEBUG("Partial Msg %d", partialMessage->position);

		/*
		 * Skip past the resume message header and update remaining buffer
		 * length
		 */
		out += sizeof(fs3ipc_app_messageHdr);
		outLength -= sizeof(fs3ipc_app_messageHdr);

		/* Does message fit in the amount of remaining buffer?*/
		msgOverflow = (dataSize > outLength);

		if (msgOverflow) {
			LOG_DEBUG("Msg wrap");
			bytesCoppied = outLength;
		} else { /*!msgOverflow*/
			bytesCoppied = dataSize;
		}

		/* Write the resume header */
		resumeMsgHdr->length = dataSize;
		resumeMsgHdr->channel = partialMessage->message.hdr.channel;

		fs3ipc_memcpy(&out[0], &inData[0], bytesCoppied);

		if (msgOverflow) {
			partialMessage->position += outLength;
		} else { /*(index < length)*/
			fs3ipc_app_messageType *msg = &partialMessage->message;

			/*
			 * If the message is not a flow control message, inform the flow
			 * control module of the number of bytes transmitted
			 */
			if (msg->hdr.channel != FS3IPC_APP_FLOWCONTROL_CHAN) {
				(void)fs3ipc_flowControl_AddByteCountTxFrame(fcCfg,
					msg->hdr.channel, msg->hdr.length + FS3IPC_APP_QUEUE_HDR_SIZE);
			}

			/* Message has been completed. clear partial message struct*/
			partialMessage->position = 0;
			msg->hdr.channel = 0;
			msg->hdr.length = 0;
		}
		bytesCoppied += sizeof(fs3ipc_app_messageHdr);
	} else {
		LOG_DEBUG("No Partial Msg");
		resumeMsgHdr->length = 0;
		resumeMsgHdr->channel = 0;
		bytesCoppied = sizeof(fs3ipc_app_messageHdr);
	}

	LOG_DEBUG("%d bytes consumed", bytesCoppied);

	return bytesCoppied;
}

/**
 * handleEncodeFlowControlMessage() - Resumes/encodes flowcontrol message.
 * @out: Buffer to store the encoded data.
 * @outLength: The total bytes to transmit.
 * @partialMessage: The partial message to decode.
 * @cfg: Application configuration parameter.
 *
 * Return: The total bytes copied/encoded.
 */
static fs3ipc_u32 handleEncodeFlowControlMessage(fs3ipc_dataPtr out,
	fs3ipc_s32 outLength, fs3ipc_app_message_buffer_t *partialMessage,
	const fs3ipc_app_ltcfg_t *cfg)
{
	fs3ipc_length fcLength;
	fs3ipc_StatusType fcStat;
	fs3ipc_app_messageType *outPtr;
	fs3ipc_length bytesTransferred = 0;

	if (fs3ipc_flowControl_MsgPending(cfg->flowControlCfg) == FS3IPC_TRUE) {
		fs3ipc_u8 msgOverflow =
			(fs3ipc_flowControl_MsgSize(cfg->flowControlCfg->channelCount) +
			sizeof(fs3ipc_app_messageHdr) > outLength);

		/* See which buffer should be used*/
		if (!msgOverflow) {
			outPtr = (fs3ipc_app_messageType *)out;
			fcLength = outLength - sizeof(fs3ipc_app_messageHdr);
		} else {
			outPtr = &partialMessage->message;
			fcLength = FS3_IPC_MAX_MSG_SIZE;
		}

		fcStat = fs3ipc_flowControl_Encode(cfg->flowControlCfg, &outPtr->data[0],
						   &fcLength);

		if (fcStat == fs3ipc_StatusType_OK && fcLength != 0) {
			outPtr->hdr.length = fcLength;
			outPtr->hdr.channel = FS3IPC_APP_FLOWCONTROL_CHAN;

			if (msgOverflow) {
				INC_STATS(cfg->stats->txWrappedMsgCnt);
				fs3ipc_memcpy(&out[0], (void *)outPtr, outLength);
				partialMessage->position += outLength -
					sizeof(fs3ipc_app_messageHdr);
				bytesTransferred = outLength;
			} else {
				bytesTransferred = fcLength + sizeof(fs3ipc_app_messageHdr);
			}
		} else {
			LOG_ERROR("fc encode:%d ", fcStat);
		}
	} else {
	}

	return bytesTransferred;
}

/**
 * handleEncodeNextMessage() - Encodes the next message.
 * @out: Buffer to store the encoded data.
 * @outLength: The total bytes to transmit.
 * @partialMessage: The partial message to decode.
 * @channelCfg: Application channel configuration
 * @cfg: Application configuration parameter.
 *
 * Loads new message from message queue.
 *
 * Return: The total bytes copied/encoded.
 */
static fs3ipc_u32 handleEncodeNextMessage(fs3ipc_dataPtr out,
	fs3ipc_s32 outLength, fs3ipc_app_message_buffer_t *partialMessage,
	const fs3ipc_app_ltchancfg_t *channelCfg,
	const fs3ipc_app_ltcfg_t *cfg)
{
	fs3ipc_length bytesCoppied = 0;
	fs3ipc_s32 qStat;
	fs3ipc_u32 outSize;
	fs3ipc_length freeByteTx = 0;
	fs3ipc_length maxTxMsg;
	fs3ipc_app_messageType *outPtr;

	/* make sure channel config is valid and channel is enabled*/
	if (!channelCfg) {
		LOG_DEBUG("Channel information is not configured");
		return 0;
	}
	if (fs3ipc_flowControl_GetTxEnabled(cfg->flowControlCfg,
	    channelCfg->channel) != fs3ipc_StatusType_OK) {
		LOG_DEBUG("Channel not enabled:%d", channelCfg->channel);
		return 0;
	}

	fs3ipc_os_GetResource(channelCfg->TX_QLock);
	qStat = fs3ipc_app_QueueFrontSize(channelCfg->TX_Q);
	fs3ipc_os_ReleaseResource(channelCfg->TX_QLock);

	(void)fs3ipc_flowControl_GetFreeByteCountTxFrame(cfg->flowControlCfg,
		channelCfg->channel,
		&freeByteTx);

	maxTxMsg = fs3ipc_flowControl_GetMaxTxPerFrame(cfg->flowControlCfg,
		channelCfg->channel);
	LOG_DEBUG("Channel is enabled:%d", channelCfg->channel);

	if (qStat == fs3ipc_app_qStat_empty) {
		LOG_DEBUG("Tx Q empty channel:%d ", channelCfg->channel);
	} else if (qStat <= 0) {
		/* buffer empty or there is some other error*/
		INC_STATS(cfg->stats->txGenErrorCnt);
		LOG_ERROR("vlqueue_fsize error:%d chan:%02d",
			qStat, channelCfg->channel);
	} else if ((qStat + FS3IPC_APP_QUEUE_HDR_SIZE) > maxTxMsg) {
		/*
		 * message is larger than maximum size. This should never happen if it
		 * does, the application is misbehaving and
		 * FS3IPC_APP_ALLOW_MSG_Q_WO_FC_CHECK == 1.
		 */
		INC_STATS(cfg->stats->txGenErrorCnt);
		LOG_ERROR("CRITICAL! Chan %d blocked; msg too large %d - %d expected",
			channelCfg->channel, qStat, maxTxMsg);

		/* pop message to unused tx partial message buffer to free the buffer*/
		fs3ipc_app_QueuePop(channelCfg->TX_Q,
			&partialMessage->message.data[0], FS3_IPC_MAX_MSG_SIZE);
		partialMessage->position = 0;

		if (fs3ipc_StatusType_OK != fs3ipc_os_SetEvent(channelCfg->client_cfg.osCfg,
			channelCfg->client_cfg.txevent)) {
			LOG_ERROR("CH-%d: Cannot set tx event", channelCfg->channel);
		}

	} else if ((qStat + FS3IPC_APP_QUEUE_HDR_SIZE) > freeByteTx) {
		/*
		 * Too many bytes have been transmitted for
		 * the given channel and frame
		 */
		LOG_DEBUG("Tx Channel:%d throttled. overflow:%d ",
			channelCfg->channel,
			qStat - freeByteTx);
	} else {
		/* message is valid size*/
		fs3ipc_u8 msgOverflow = (qStat + sizeof(fs3ipc_app_messageHdr) >
			(outLength));
		LOG_DEBUG("Valid Msg Size:%d ", qStat);

		/* see which buffer should be used*/
		if (!msgOverflow) {
			LOG_DEBUG("msg complete");
			outPtr = (fs3ipc_app_messageType *)(&out[0]);
			outSize = outLength - sizeof(fs3ipc_app_messageHdr);
		} else {
			//msgOverflow == TRUE
			LOG_DEBUG("msg wrapped");
			outPtr = &partialMessage->message;
			outSize = FS3_IPC_MAX_MSG_SIZE;
		}

		fs3ipc_os_GetResource(channelCfg->TX_QLock);
		qStat = fs3ipc_app_QueuePop(channelCfg->TX_Q,
			&outPtr->data[0], outSize);
		fs3ipc_os_ReleaseResource(channelCfg->TX_QLock);

		if (qStat > fs3ipc_app_qStat_OK) {
			/* message transmitted, increment stats*/
			fs3ipc_flowControl_IncTxMsgCnt(cfg->flowControlCfg,
				channelCfg->channel);
			LOG_DEBUG("vlqueue_pop OK");

			outPtr->hdr.channel = channelCfg->channel;
			outPtr->hdr.length = qStat;

			if (msgOverflow) {
				INC_STATS(cfg->stats->txWrappedMsgCnt);
				fs3ipc_memcpy(&out[0], (void *)outPtr, outLength);
				partialMessage->position += outLength -
					sizeof(fs3ipc_app_messageHdr);
				bytesCoppied = outLength;
			} else {
				/*
				 * fs3ipc_app_messageType* msg = &partialMessage->message;
				 * If the message is not a flow control message, inform the flow
				 * c1ontrol module of the number of bytes transmitted
				 */
				if (outPtr->hdr.channel != FS3IPC_APP_FLOWCONTROL_CHAN) {
					LOG_DEBUG("FC Msg");
					(void)fs3ipc_flowControl_AddByteCountTxFrame(
						cfg->flowControlCfg, outPtr->hdr.channel,
						qStat + FS3IPC_APP_QUEUE_HDR_SIZE);
				}

				bytesCoppied = qStat + sizeof(fs3ipc_app_messageHdr);
			}

			if (fs3ipc_StatusType_OK !=
				fs3ipc_os_SetEvent(channelCfg->client_cfg.osCfg,
					channelCfg->client_cfg.txevent)) {
				LOG_ERROR("CH-%d, Cannot set tx event",
					channelCfg->channel);
			}

		} else if (qStat == fs3ipc_app_qStat_empty) {
			/* Queue is empty. continue on to next message*/
			LOG_DEBUG("Q Emtpy chan:%d", channelCfg->channel);
		} else {
			/* message does not fit into buffer*/
			/* error - VLQ call failed. This should never happen*/
			INC_STATS(cfg->stats->txGenErrorCnt);
			LOG_ERROR("vlqueue_pop:%d", qStat);
		}
	}

	LOG_DEBUG("Bytes copied :%d", bytesCoppied);

	return bytesCoppied;
}

/**
 * handleDecodeResumeMessage() - Resumes the message.
 * @in: Buffer to decode the data.
 * @outLength: Remaining bytes to process.
 * @cfg: Provides the Application Configuration Parameter.
 * @stat: Returns the result of the function call.
 *
 * This function verifies the correct configuration parameters, it resumes the
 * decoding operation when message is correct, the message is written to the
 * queue.
 *
 * Return: The number of bytes processed.
 */
static fs3ipc_u32 handleDecodeResumeMessage(fs3ipc_cDataPtr in,
	fs3ipc_length outLength, const fs3ipc_app_ltcfg_t *cfg,
	fs3ipc_StatusType *stat)
{
	fs3ipc_StatusType myStatus = fs3ipc_StatusType_OK;
	fs3ipc_app_message_buffer_t *partialMessage = cfg->rxMsg;
	const fs3ipc_length rxResumeMsgSize =
		((const fs3ipc_app_messageHdr *)in)->length;
	fs3ipc_s32 storedMsgRemSize = partialMessage->message.hdr.length -
		partialMessage->position;
	fs3ipc_length bytesCoppied;

	bytesCoppied = ((const fs3ipc_app_messageHdr *)in)->length +
		sizeof(fs3ipc_app_messageHdr);
	if (bytesCoppied > outLength)
		bytesCoppied = outLength;

	/* No partial message encoded in current frame */
	if (rxResumeMsgSize == 0) {
		/* Nothing to do, exit*/
		if (storedMsgRemSize != 0) {
			/* Error - partial message discarded. erase message*/
			INC_STATS(cfg->stats->rxWrappedMsgErrorCnt);

			LOG_ERROR("Partial Msg Discarded. Not in Rx %d",
				storedMsgRemSize);

			fs3ipc_memset(partialMessage, 0,
				sizeof(fs3ipc_app_message_buffer_t));
		}
	} else if (storedMsgRemSize != rxResumeMsgSize) {
		/*
		 * Error - stored message does not match incoming partial message
		 * discarded. erase message
		 */
		INC_STATS(cfg->stats->rxWrappedMsgErrorCnt);

		LOG_ERROR("Partial Msg Discarded. Size mismatch %d, %d",
			storedMsgRemSize, rxResumeMsgSize);

		fs3ipc_memset(partialMessage, 0, sizeof(fs3ipc_app_message_buffer_t));
	} else {
		/* Message appears to be good*/
		fs3ipc_dataPtr storedMsgData =
			&partialMessage->message.data[partialMessage->position];

		/* Update in pointer to point to beginning of message*/
		in += sizeof(fs3ipc_app_messageHdr);
		outLength -= sizeof(fs3ipc_app_messageHdr);

		if (storedMsgRemSize <= outLength) {
			fs3ipc_u8 channel = partialMessage->message.hdr.channel;
			fs3ipc_length msgLength = partialMessage->message.hdr.length;
			const fs3ipc_app_ltchancfg_t *ltChanCfg =
				cfg->ltChannelCfg[channel];

			/* Copy data */
			fs3ipc_memcpy(storedMsgData, in, rxResumeMsgSize);

			if (channel == FS3IPC_APP_FLOWCONTROL_CHAN) {
				/* Message fits in frame buffer. Add message to application Q*/
				(void)fs3ipc_flowControl_Decode(cfg->flowControlCfg,
					&partialMessage->message.data[0],
					partialMessage->message.hdr.length);
				/* Wake up hdlc thread for any new messages to be sent*/
				(void)fs3ipc_os_SetEvent(cfg->phys_txEventOsCfg,
						cfg->phys_txEvent);
			} else if ((channel < cfg->channelCount) && (ltChanCfg)) {
				fs3ipc_s32 qstat;

				LOG_DATA(*ltChanCfg->loggingEnabled,
					&partialMessage->message.data[0], msgLength,
					LOG_DATA_MAX_BYTES, "Channel Rx (C%02d L%03d)", channel, msgLength);
				fs3ipc_os_GetResource(ltChanCfg->RX_QLock);
				/* Message fits in frame buffer. Add message to application Q*/
				qstat = fs3ipc_app_QueuePush(ltChanCfg->RX_Q,
					&partialMessage->message.data[0], msgLength);
				fs3ipc_os_ReleaseResource(ltChanCfg->RX_QLock);

				if (qstat == fs3ipc_app_qStat_OK) {
					fs3ipc_flowControl_IncRxMsgCnt(cfg->flowControlCfg, channel);
					fs3ipc_flowctrl_inc_rx_byte_cnt(cfg->flowControlCfg,
									channel,
									msgLength);

					fs3ipc_os_GetResource(ltChanCfg->RX_QLock);
					qstat = fs3ipc_app_QueueFreeSpace(ltChanCfg->RX_Q);
					fs3ipc_os_ReleaseResource(ltChanCfg->RX_QLock);

					if ((qstat > fs3ipc_app_qStat_OK) &&
						(qstat < (fs3ipc_flowControl_GetMaxRxPerFrame(
							cfg->flowControlCfg, channel) * 2))) {
						(void)fs3ipc_flowControl_SetOverflow(cfg->flowControlCfg,
							channel, FS3IPC_TRUE);

						if (fs3ipc_flowControl_MsgPending(cfg->flowControlCfg) != FS3IPC_FALSE) {
							(void)fs3ipc_os_SetEvent(cfg->phys_txEventOsCfg,
								cfg->phys_txEvent);
							LOG_DEBUG("Pre-emptive off: event channel:%d", channel);
						}
					}

					/* Set receive event */
					if (fs3ipc_StatusType_OK !=
						fs3ipc_os_SetEvent(ltChanCfg->client_cfg.osCfg,
						ltChanCfg->client_cfg.rxEvent)) {
						LOG_ERROR("cannot set rx event");
					}

					fs3ipc_memset(partialMessage, 0,
						sizeof(fs3ipc_app_message_buffer_t));
				} else if (qstat == fs3ipc_app_qStat_full) {
					/* Message doesn't fit in current buffer*/
					myStatus = fs3ipc_StatusType_BufferFull;
					INC_STATS(cfg->stats->rxBufferOverflow);
					LOG_ERROR("Q Full. Dropped Message chan:%d len:%d", channel,
						msgLength);

					(void)fs3ipc_flowControl_SetOverflow(cfg->flowControlCfg,
						channel, FS3IPC_TRUE);

					if (fs3ipc_flowControl_MsgPending(cfg->flowControlCfg) !=
						FS3IPC_FALSE) {
						(void)fs3ipc_os_SetEvent(cfg->phys_txEventOsCfg,
							cfg->phys_txEvent);
					}
				} else {
					INC_STATS(cfg->stats->rxWrappedMsgErrorCnt);
					LOG_ERROR("Dropped Message chan:%d len:%d stat:%d", channel,
						msgLength, qstat);

					fs3ipc_memset(partialMessage, 0,
						sizeof(fs3ipc_app_message_buffer_t));
				}
			} else {
				/* Error - unhandled channel*/
				INC_STATS(cfg->stats->rxUnhandledChannelErrorCnt);
				LOG_ERROR("Partial Msg Discarded. Unhandled channel:%d",
					channel);

				fs3ipc_memset(partialMessage, 0,
					sizeof(fs3ipc_app_message_buffer_t));
			}
		} else {
			LOG_DEBUG("Partial Msg Wrapped OF:%d",
				storedMsgRemSize - outLength);

			fs3ipc_memcpy(storedMsgData, in, outLength);
			/*
			 * Complete message has not been received. update partial rx message
			 * buffer
			 */
			partialMessage->position += outLength;
		}
	}

	*stat = myStatus;

	return bytesCoppied;
}

/**
 * handleDecodeNewMessage() - Decodes the new messages.
 * @in: Buffer to decode the data.
 * @outLength: Remaining bytes to process.
 * @cfg: Provides the Application Configuration Parameter.
 * @stat: Returns the result of the function call.
 *
 * This function validates the proper condition to decode new messages, it
 * retrieves the Application layer configuration parameters.
 *
 * Return: The number of bytes processed.
 */
static fs3ipc_u32 handleDecodeNewMessage(fs3ipc_cDataPtr in,
	fs3ipc_length outLength, const fs3ipc_app_ltcfg_t *cfg,
	fs3ipc_StatusType *stat)
{
	fs3ipc_StatusType myStatus = fs3ipc_StatusType_OK;
	fs3ipc_app_message_buffer_t *partialMessage = cfg->rxMsg;
	fs3ipc_u8 msgChannel = ((const fs3ipc_app_messageHdr *)&in[0])->channel;
	fs3ipc_length msgLength = ((const fs3ipc_app_messageHdr *)&in[0])->length;
	fs3ipc_length bytesCoppied = msgLength + sizeof(fs3ipc_app_messageHdr);
	const fs3ipc_app_ltchancfg_t *ltChanCfg = cfg->ltChannelCfg[msgChannel];

	/* Return number of bytes for next message */
	if (bytesCoppied > outLength)
		bytesCoppied = outLength;

	if (msgLength == 0 || msgLength > FS3_IPC_MAX_MSG_SIZE) {
		/* Nothing to send */
		INC_STATS(cfg->stats->rxMsgLengthErrorCnt);
		LOG_ERROR("Invalid Length Message %d", msgLength);
	} else if (msgChannel > cfg->channelCount) {
		/* Error - invalid channel*/
		INC_STATS(cfg->stats->rxUnhandledChannelErrorCnt);
		LOG_ERROR("Invalid Channel:%d", msgChannel);
	} else {
		/* msgHdr->length != 0*/
		if (msgChannel == FS3IPC_APP_FLOWCONTROL_CHAN) {
			/* Flow control message - requires special handling*/
			if (msgLength + sizeof(fs3ipc_app_messageHdr) > outLength) {
				/*
				 * Message does not fit in frame buffer. copy the partial message
				 * into the rx buffer
				 */
				INC_STATS(cfg->stats->rxWrappedMsgCnt);
				fs3ipc_memcpy(&partialMessage->message, in, outLength);
				partialMessage->position = outLength -
					sizeof(fs3ipc_app_messageHdr);
			} else {
				/* Message fits in frame buffer. Add message to application Q*/
				(void)fs3ipc_flowControl_Decode(cfg->flowControlCfg,
					&((fs3ipc_app_messageType *)in)->data[0], msgLength);
				/* Wake up hdlc thread for any new messages to be sent*/
				(void)fs3ipc_os_SetEvent(cfg->phys_txEventOsCfg,
						cfg->phys_txEvent);
			}
		} else if (ltChanCfg) {
			fs3ipc_app_queueType *rxQ = ltChanCfg->RX_Q;
			fs3ipc_s32 qstat;

			LOG_DATA(*ltChanCfg->loggingEnabled,
				&((fs3ipc_app_messageType *)in)->data[0], msgLength,
				LOG_DATA_MAX_BYTES, "Channel Rx (C%02d L%03d)", ltChanCfg->channel,
				msgLength);

			/* Message fits in application Q*/
			if (msgLength + sizeof(fs3ipc_app_messageHdr) > outLength) {
				/*
				 * Message does not fit in frame buffer. copy the partial message
				 * into the rx buffer
				 */
				INC_STATS(cfg->stats->rxWrappedMsgCnt);
				fs3ipc_memcpy(&partialMessage->message, in, outLength);
				partialMessage->position = outLength -
					sizeof(fs3ipc_app_messageHdr);
			} else {
				fs3ipc_os_GetResource(ltChanCfg->RX_QLock);
				/* Message fits in frame buffer. Add message to application Q*/
				qstat = fs3ipc_app_QueuePush(rxQ,
					&((fs3ipc_app_messageType *)in)->data[0], msgLength);
				fs3ipc_os_ReleaseResource(ltChanCfg->RX_QLock);

				if (qstat == fs3ipc_app_qStat_OK) {
					fs3ipc_flowControl_IncRxMsgCnt(cfg->flowControlCfg, msgChannel);
					fs3ipc_flowctrl_inc_rx_byte_cnt(cfg->flowControlCfg,
									msgChannel,
									msgLength);

					fs3ipc_os_GetResource(ltChanCfg->RX_QLock);
					qstat = fs3ipc_app_QueueFreeSpace(rxQ);
					fs3ipc_os_ReleaseResource(ltChanCfg->RX_QLock);

					if ((qstat > fs3ipc_app_qStat_OK) &&
						(qstat < (fs3ipc_flowControl_GetMaxRxPerFrame(
						cfg->flowControlCfg, msgChannel) * 2))) {
						(void)fs3ipc_flowControl_SetOverflow(cfg->flowControlCfg,
							msgChannel, FS3IPC_TRUE);

						if (fs3ipc_flowControl_MsgPending(cfg->flowControlCfg) != FS3IPC_FALSE) {
							(void)fs3ipc_os_SetEvent(cfg->phys_txEventOsCfg,
								cfg->phys_txEvent);
							LOG_DEBUG("Pre-emptive off: event channel:%d", msgChannel);
						}
					}

					/* Set receive event */
					if (fs3ipc_StatusType_OK !=
						fs3ipc_os_SetEvent(ltChanCfg->client_cfg.osCfg,
						ltChanCfg->client_cfg.rxEvent)) {
						LOG_ERROR("cannot set rx event");
					}
				} else if (qstat == fs3ipc_app_qStat_full) {
					/* Message doesn't fit in current buffer*/
					myStatus = fs3ipc_StatusType_BufferFull;
					INC_STATS(cfg->stats->rxBufferOverflow);
					LOG_ERROR("Q Full. Dropped Message chan:%d len:%d", msgChannel,
						msgLength);

					(void)fs3ipc_flowControl_SetOverflow(cfg->flowControlCfg,
						msgChannel, FS3IPC_TRUE);

					if (fs3ipc_flowControl_MsgPending(cfg->flowControlCfg) !=
						FS3IPC_FALSE) {
						(void)fs3ipc_os_SetEvent(cfg->phys_txEventOsCfg,
							cfg->phys_txEvent);
					}
				} else {
					INC_STATS(cfg->stats->rxGenErrorCnt);
					LOG_ERROR("Dropped Message chan:%d len:%d stat:%d", msgChannel,
						msgLength, qstat);
				}
			}
		} else {
			INC_STATS(cfg->stats->rxUnhandledChannelErrorCnt);
			/* Warning - invalid channel!. Continue parsing*/
			LOG_ERROR("Invalid channel chan:%d", msgChannel);
		}
	}

	*stat = myStatus;

	return bytesCoppied;
}

/**
 * fs3ipc_app_set_ch_state() - Set the flow control state of channel client.
 * @client: FS3IPC client.
 * @enable: Enable or disable.
 *
 * It is assumed that all the arguments are valid.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorGen         - Invalid FC config or channel.
 * * fs3ipc_StatusType_ErrorClientState - Client is already in requested state.
 */
static fs3ipc_StatusType fs3ipc_app_set_ch_state(const struct fs3ipc_client *client,
						 uint8_t enable)
{
	const fs3ipc_app_ltcfg_t *app_cfg = &fs3ipc_app_appConfigs[client->instance];
	fs3ipc_StatusType ret =
		fs3ipc_flowControl_EnableRxClient(app_cfg->flowControlCfg,
						  client->ch, enable);

	if (ret == fs3ipc_StatusType_OK)
		(void)fs3ipc_os_SetEvent(app_cfg->phys_txEventOsCfg,
					 app_cfg->phys_txEvent);

	return ret;
}

/**
 * fs3ipc_set_ch_state() - Set the flow control state of channel client.
 * @handle: Client handle.
 * @enable: Enable or disable.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorHandle      - Invalid client handle.
 * * fs3ipc_StatusType_ErrorGen         - Invalid FC config or channel.
 * * fs3ipc_StatusType_ErrorClientState - Client is already in requested state.
 */
static fs3ipc_StatusType fs3ipc_set_ch_state(unsigned int handle, uint8_t enable)
{
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		NULL, NULL);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	return fs3ipc_app_set_ch_state(client, enable);
}

/**
 * fs3ipc_app_chan_init() - Initialize all channels.
 * @app_cfg: App configuration.
 * @instance: FS3IPC instance.
 * @ch: Channel.
 *
 * It is assumed that all arguments are valid.
 *
 * Return:
 * * fs3ipc_StatusType_OK       - OK.
 * * fs3ipc_StatusType_ErrorCfg - Invalid configuration.
 */
static fs3ipc_StatusType fs3ipc_app_chan_init(const fs3ipc_app_ltcfg_t *app_cfg,
					      unsigned int instance,
					      unsigned int ch)
{
	int32_t qstat = fs3ipc_app_qStat_OK;
	fs3ipc_app_priorityNodeType *node = NULL;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const fs3ipc_app_ltchancfg_t *chan_cfg = NULL;
	unsigned int handle = FS3IPC_CLIENT_HANDLE_COUNT;
	struct fs3ipc_client *client = fs3ipc_client_lookup_table;

	chan_cfg = app_cfg->ltChannelCfg[ch];
	if (!chan_cfg) {
		LOG_WARNING("[%u] CH-%u: Not configured", instance, ch);
		return fs3ipc_StatusType_OK;
	}

	if (ch != chan_cfg->channel) {
		LOG_ERROR("[%u] CH-%u: Channel number mismatch: %u", instance,
			  ch, chan_cfg->channel);
		return fs3ipc_StatusType_ErrorCfg;
	}

	handle = chan_cfg->client_cfg.handle;
	if (handle >= FS3IPC_CLIENT_HANDLE_COUNT) {
		LOG_ERROR("[%u] CH-%u: Invalid client handle: %u", instance, ch,
			  handle);
		return fs3ipc_StatusType_ErrorCfg;
	}
	if (client[handle].registered == FS3IPC_TRUE) {
		LOG_ERROR("[%u] CH-%u: Client(%u) already registered for CH-%u",
			  instance, ch, handle,
			  (unsigned int)client[handle].ch);
		return fs3ipc_StatusType_ErrorCfg;
	}

	/* Initialize Tx-Q & Rx-Q */
	fs3ipc_os_GetResource(chan_cfg->TX_QLock);
	qstat = fs3ipc_app_QueueInit(chan_cfg->TX_Q, chan_cfg->TX_Buffer,
				     chan_cfg->TX_Buffer_Size);
	fs3ipc_os_ReleaseResource(chan_cfg->TX_QLock);
	if (qstat != fs3ipc_app_qStat_OK) {
		LOG_ERROR("[%u] CH-%u: Tx-Q init failed: %d", instance, ch, ret);
		return fs3ipc_StatusType_ErrorCfg;
	}

	fs3ipc_os_GetResource(chan_cfg->RX_QLock);
	qstat = fs3ipc_app_QueueInit(chan_cfg->RX_Q, chan_cfg->RX_Buffer,
				     chan_cfg->RX_Buffer_Size);
	fs3ipc_os_ReleaseResource(chan_cfg->RX_QLock);
	if (qstat != fs3ipc_app_qStat_OK) {
		LOG_ERROR("[%u] CH-%u: Rx-Q init failed: %d", instance, ch, qstat);
		return fs3ipc_StatusType_ErrorCfg;
	}

	/* Initialize Tx priority list */
	node = &app_cfg->orderedChannelNodes[ch];
	fs3ipc_app_PriorityNodeInit(node, (void *)chan_cfg, chan_cfg->priority);
	fs3ipc_app_PriorityNodeInsert(app_cfg->orderedChannelListHead, node);

	/* Add entry to client lookup table */
	client[handle].instance = instance;
	client[handle].ch = ch;
	client[handle].registered = FS3IPC_TRUE;

	if (chan_cfg->loggingEnabled)
		*chan_cfg->loggingEnabled = FS3IPC_FALSE;
	else
		LOG_WARNING("[%u] CH-%u: Logging not configured", instance, ch);

	return fs3ipc_StatusType_OK;
}

fs3ipc_StatusType fs3ipc_init(unsigned int instance)
{
	unsigned int ch = 0u;
	const fs3ipc_app_ltcfg_t *app_cfg = NULL;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	if (instance >= FS3IPC_NUM_OF_INSTANCES) {
		LOG_ERROR("Invalid FS3IPC instance: %u", instance);
		return fs3ipc_StatusType_ErrorHandle;
	}

	app_cfg = &fs3ipc_app_appConfigs[instance];
	if (app_cfg->channelCount > FS3IPC_MAX_NUMBER_CHANNELS ||
	    !app_cfg->orderedChannelListHead || !app_cfg->orderedChannelNodes ||
	    !app_cfg->phys_txEventOsCfg || !app_cfg->txMsg || !app_cfg->rxMsg ||
	    !app_cfg->rxMsgBufferOverflowIndex) {
		LOG_ERROR("[%u] App cfg check failed: %u, %p, %p, %p, %p, %p, %p",
			  instance, (unsigned int)app_cfg->channelCount,
			  (void *)app_cfg->orderedChannelListHead,
			  (void *)app_cfg->orderedChannelNodes,
			  (void *)app_cfg->phys_txEventOsCfg,
			  (void *)app_cfg->txMsg, (void *)app_cfg->rxMsg,
			  (void *)app_cfg->rxMsgBufferOverflowIndex);
		return fs3ipc_StatusType_ErrorCfg;
	}

	ret = fs3ipc_flowcontrol_Init(app_cfg->flowControlCfg);
	if (ret != fs3ipc_StatusType_OK)
		return ret;

	for (ch = 0u; ch < app_cfg->channelCount; ch++) {
		ret = fs3ipc_app_chan_init(app_cfg, instance, ch);
		if (ret != fs3ipc_StatusType_OK)
			return ret; /* TODO: Cleanup in case of error */
	}

	return fs3ipc_StatusType_OK;
}

const struct fs3ipc_client *fs3ipc_get_client(unsigned int handle,
					      unsigned int *instance,
					      unsigned int *ch)
{
	const struct fs3ipc_client *client = NULL;

	if (handle >= FS3IPC_CLIENT_HANDLE_COUNT) {
		LOG_ERROR("Invalid client handle: %u", handle);
		return NULL;
	}

	client = &fs3ipc_client_lookup_table[handle];

	/*
	 * No need to validate client->instance & client->ch as their
	 * uninitialized value is 0. Also, no need to check whether
	 * client->registered is true or not as it is set to true only when
	 * channel is configured properly.
	 */
	if (!fs3ipc_app_appConfigs[client->instance].ltChannelCfg[client->ch]) {
		LOG_ERROR("[%u]: CH-%u: Not configured; handle: %u",
			  client->instance, client->ch, handle);
		return NULL;
	}

	if (instance)
		*instance = client->instance;
	if (ch)
		*ch = client->ch;

	return client;
}

const struct fs3ipc_client_ltcfg *
fs3ipc_app_get_client_cfg(const struct fs3ipc_client *client)
{
	const fs3ipc_app_ltcfg_t *app_cfg =
		&fs3ipc_app_appConfigs[client->instance];

	return &app_cfg->ltChannelCfg[client->ch]->client_cfg;
}

#if FS3IPC_APP_ALLOW_MSG_Q_WO_FC_CHECK != 1
/**
 * fs3ipc_tx_fc_check() - Check for transmission flowcontrol.
 * @fc_cfg: Flow control configuration.
 * @ch: Channel.
 * @length: Length of message in bytes.
 *
 * Return:
 * * fs3ipc_StatusType_OK                  - OK.
 * * fs3ipc_StatusType_ErrorGen            - Invalid channel or FC config.
 * * fs3ipc_StatusType_ChannelNotAvailable - Channel not available.
 * * fs3ipc_StatusType_MessageSize         - Invalid message size.
 */
static int fs3ipc_tx_fc_check(const fs3ipc_fc_ltcfg_t *fc_cfg, unsigned int ch,
			      uint32_t length)
{
	fs3ipc_StatusType ret = fs3ipc_flowControl_GetTxEnabled(fc_cfg, ch);

	if (ret == fs3ipc_StatusType_OK &&
	    length > fs3ipc_flowControl_GetMaxTxPerFrame(fc_cfg, ch)) {
		LOG_ERROR("CH-%u: Invalid msg size: %u", ch, length);
		ret = fs3ipc_StatusType_MessageSize;
	}

	return ret;
}
#else
static inline int fs3ipc_tx_fc_check(const fs3ipc_fc_ltcfg_t *fc_cfg,
				     unsigned int ch, uint32_t length)
{
	(void)fc_cfg;
	(void)ch;
	(void)length;

	return fs3ipc_StatusType_OK;
}
#endif

fs3ipc_StatusType fs3ipc_app_write_msg(const struct fs3ipc_client *client,
				       const uint8_t *msg, uint32_t length)
{
	int32_t qstat = 0;
	unsigned int ch = client->ch;
	unsigned int instance = client->instance;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const fs3ipc_app_ltcfg_t *app_cfg = &fs3ipc_app_appConfigs[instance];
	const fs3ipc_app_ltchancfg_t *chan_cfg = app_cfg->ltChannelCfg[ch];

	LOG_DATA(*chan_cfg->loggingEnabled, msg, length, LOG_DATA_MAX_BYTES,
		 "[%u] CH-%u: Write (L%03d)", instance, ch, length);

	ret = fs3ipc_tx_fc_check(app_cfg->flowControlCfg, ch, length);
	if (ret != fs3ipc_StatusType_OK)
		return ret;

	fs3ipc_os_GetResource(chan_cfg->TX_QLock);
	qstat = fs3ipc_app_QueuePush(chan_cfg->TX_Q, msg, length);
	fs3ipc_os_ReleaseResource(chan_cfg->TX_QLock);
	if (qstat == fs3ipc_app_qStat_OK) {
		(void)fs3ipc_os_SetEvent(app_cfg->phys_txEventOsCfg,
					 app_cfg->phys_txEvent);
		fs3ipc_flowctrl_inc_tx_byte_cnt(app_cfg->flowControlCfg, ch,
						length);
		fs3ipc_flowctrl_update_largest_tx_pkt(app_cfg->flowControlCfg,
						      ch, length);
	} else if (qstat == fs3ipc_app_qStat_full) {
		LOG_WARNING("[%u] CH-%u: Tx-Q full", instance, ch);
		ret = fs3ipc_StatusType_BufferFull;
	} else {
		LOG_ERROR("[%u] CH-%u: Tx-Q push error for size(%u): %d",
			  instance, ch, length, qstat);
		ret = fs3ipc_StatusType_ErrorGen;
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_write_msg(unsigned int handle, const uint8_t *msg,
				   uint32_t length)
{
	unsigned int ch = 0;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		&instance, &ch);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	if (!msg) {
		log_error("[%u] CH-%u: NULL msg", instance, ch);
		return fs3ipc_StatusType_ErrorGen;
	}

	if (length == 0 || length > FS3_IPC_MAX_MSG_SIZE) {
		LOG_ERROR("[%u] CH-%u: Invalid msg size: %u", instance, ch,
			  length);
		return fs3ipc_StatusType_MessageSize;
	}

	return fs3ipc_app_write_msg(client, msg, length);
}

fs3ipc_StatusType fs3ipc_app_read_msg(const struct fs3ipc_client *client,
				      uint8_t *msg, uint32_t *length)
{
	int32_t qstat = 0;
	unsigned int ch = client->ch;
	unsigned int instance = client->instance;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const fs3ipc_app_ltcfg_t *app_cfg = &fs3ipc_app_appConfigs[instance];
	const fs3ipc_app_ltchancfg_t *chan_cfg = app_cfg->ltChannelCfg[ch];

	fs3ipc_os_GetResource(chan_cfg->RX_QLock);
	qstat = fs3ipc_app_QueuePop(chan_cfg->RX_Q, msg, *length);
	fs3ipc_os_ReleaseResource(chan_cfg->RX_QLock);
	if (qstat == fs3ipc_app_qStat_empty) {
		(void)fs3ipc_flowControl_SetOverflow(app_cfg->flowControlCfg,
						     ch, FS3IPC_FALSE);
		if (fs3ipc_flowControl_MsgPending(app_cfg->flowControlCfg) !=
		    FS3IPC_FALSE)
			(void)fs3ipc_os_SetEvent(app_cfg->phys_txEventOsCfg,
						 app_cfg->phys_txEvent);

		LOG_DEBUG("[%u] CH-%u: Rx-Q empty", instance, ch);

		ret = fs3ipc_StatusType_BufferEmpty;
	} else if (qstat < fs3ipc_app_qStat_OK) {
		LOG_ERROR("[%u] CH-%u: Rx-Q pop error for size(%u): %d",
			  instance, ch, *length, qstat);
		ret = fs3ipc_StatusType_ErrorGen;
	} else {
		*length = qstat;
		fs3ipc_flowctrl_update_largest_rx_pkt(app_cfg->flowControlCfg,
						      ch, *length);
		LOG_DATA(*chan_cfg->loggingEnabled, msg, *length,
			 LOG_DATA_MAX_BYTES, "[%u] CH-%u: Client Read (L%03d)",
			 instance, ch, *length);
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_read_msg(unsigned int handle, uint8_t *msg,
				  uint32_t *length)
{
	unsigned int ch = 0;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		&instance, &ch);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	if (!msg) {
		log_error("[%u] CH-%u: NULL msg", instance, ch);
		return fs3ipc_StatusType_ErrorGen;
	}

	if (!length || (*length == 0)) {
		LOG_ERROR("[%u] CH-%u: Invalid msg size: %p", instance, ch,
			  (void *)length);
		return fs3ipc_StatusType_MessageSize;
	}

	return fs3ipc_app_read_msg(client, msg, length);
}

/**
 * fs3ipc_app_wait_event() - Wait on event.
 * @client: FS3IPC client.
 * @wait_obj: OS object to wait on.
 * @event: Event mask to wait for.
 * @name: Name of event.
 *
 * It is assumed that @client & @name are valid.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorInterrupted - Waiting interrupted.
 * * fs3ipc_StatusType_ErrorGen         - General error.
 */
static fs3ipc_StatusType fs3ipc_app_wait_event(const struct fs3ipc_client *client,
					       fs3ipc_os_event_t *wait_obj,
					       fs3ipc_os_EventMaskType event,
					       const char *name)
{
	unsigned int ch = client->ch;
	unsigned int instance = client->instance;
	fs3ipc_os_EventMaskType mask = 0;
	fs3ipc_StatusType ret = fs3ipc_os_WaitEvent(wait_obj, event);

	if (ret == fs3ipc_StatusType_OK) {
		fs3ipc_os_GetEvent(wait_obj, &mask);
		fs3ipc_os_ClearEvent(wait_obj, mask & event);
	} else {
		LOG_ERROR("[%u] CH-%u: Waiting for '%s' failed: %u", instance,
			  ch, name, ret);
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_wait_txq_space(unsigned int handle)
{
	const struct fs3ipc_client_ltcfg *ccfg = NULL;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		NULL, NULL);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	ccfg = fs3ipc_app_get_client_cfg(client);

	return fs3ipc_app_wait_event(client, ccfg->osCfg, ccfg->txevent,
				     "Tx-Q space");
}

fs3ipc_StatusType fs3ipc_wait_rxq_msg(unsigned int handle)
{
	const struct fs3ipc_client_ltcfg *ccfg = NULL;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		NULL, NULL);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	ccfg = fs3ipc_app_get_client_cfg(client);

	return fs3ipc_app_wait_event(client, ccfg->osCfg, ccfg->rxEvent,
				     "Rx-Q msg");
}

/**
 * fs3ipc_app_wait_event_timeout() - Wait on event with timeout.
 * @client: FS3IPC client.
 * @wait_obj: OS object to wait on.
 * @event: Event mask to wait for.
 * @name: Name of event.
 * @timeout_msec: Timeout in msecs.
 *
 * It is assumed that @client & @name are valid.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_Timeout          - Timeout and event is not set.
 * * fs3ipc_StatusType_ErrorInterrupted - Waiting interrupted.
 * * fs3ipc_StatusType_ErrorGen         - General error.
 */
static fs3ipc_StatusType
fs3ipc_app_wait_event_timeout(const struct fs3ipc_client *client,
			      fs3ipc_os_event_t *wait_obj,
			      fs3ipc_os_EventMaskType event, const char *name,
			      uint32_t timeout_msec)
{
	unsigned int ch = client->ch;
	unsigned int instance = client->instance;
	fs3ipc_os_EventMaskType mask = 0;
	fs3ipc_StatusType ret = fs3ipc_os_WaitEventTimeout(wait_obj, event,
		timeout_msec);

	if (ret == fs3ipc_StatusType_OK) {
		fs3ipc_os_GetEvent(wait_obj, &mask);
		fs3ipc_os_ClearEvent(wait_obj, mask & event);
	} else if (ret == fs3ipc_StatusType_ErrorTimeout) {
		LOG_INFO("[%u] CH-%u: Waiting for '%s' timed out", instance, ch,
			 name);
	} else {
		LOG_ERROR("[%u] CH-%u: Waiting for '%s' failed: %u", instance,
			  ch, name, ret);
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_wait_txq_space_timeout(unsigned int handle,
						uint32_t timeout_msec)
{
	const struct fs3ipc_client_ltcfg *ccfg = NULL;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		NULL, NULL);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	ccfg = fs3ipc_app_get_client_cfg(client);

	return fs3ipc_app_wait_event_timeout(client, ccfg->osCfg, ccfg->txevent,
					     "Tx-Q space", timeout_msec);
}

fs3ipc_StatusType fs3ipc_wait_rxq_msg_timeout(unsigned int handle,
					      uint32_t timeout_msec)
{
	const struct fs3ipc_client_ltcfg *ccfg = NULL;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		NULL, NULL);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	ccfg = fs3ipc_app_get_client_cfg(client);

	return fs3ipc_app_wait_event_timeout(client, ccfg->osCfg, ccfg->rxEvent,
					     "Rx-Q msg", timeout_msec);
}

/**
 * fs3ipc_get_qinfo() - Get queue info.
 * @handle: Client handle.
 * @get_qinfo: Function to call based on which queue's info is required.
 * @qinfo: Function to call to get the required info.
 * @name: Name of info.
 * @info: To store the info.
 *
 * It is assumed that @get_qinfo, @qinfo & @name are valid.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK.
 * * fs3ipc_StatusType_ErrorHandle - Invalid client handle.
 * * Any other error               - Based on @qinfo.
 */
static fs3ipc_StatusType fs3ipc_get_qinfo(unsigned int handle,
					  fs3ipc_get_qinfo_t get_qinfo,
					  fs3ipc_qinfo_t qinfo,
					  const char *name, uint32_t *info)
{
	int32_t ret = 0;
	unsigned int ch = 0u;
	const fs3ipc_app_ltchancfg_t *chan_cfg = NULL;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		&instance, &ch);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	if (!info) {
		LOG_ERROR("[%u] CH-%u: '%s' info: NULL argument", instance, ch,
			  name);
		return fs3ipc_StatusType_ErrorGen;
	}

	chan_cfg = fs3ipc_app_appConfigs[instance].ltChannelCfg[ch];
	ret = get_qinfo(chan_cfg, qinfo);
	if (ret < 0) {
		LOG_ERROR("[%u] CH-%u: Failed to get '%s' info: %d", instance,
			  ch, name, ret);
		return fs3ipc_StatusType_ErrorGen;
	}

	*info = ret;

	return fs3ipc_StatusType_OK;
}

static int32_t fs3ipc_get_txq_info(const fs3ipc_app_ltchancfg_t *chan_cfg,
				   fs3ipc_qinfo_t qinfo)
{
	int32_t ret = 0;

	fs3ipc_os_GetResource(chan_cfg->TX_QLock);
	ret = qinfo(chan_cfg->TX_Q);
	fs3ipc_os_ReleaseResource(chan_cfg->TX_QLock);

	return ret;
}

static int32_t fs3ipc_get_rxq_info(const fs3ipc_app_ltchancfg_t *chan_cfg,
				   fs3ipc_qinfo_t qinfo)
{
	int32_t ret = 0;

	fs3ipc_os_GetResource(chan_cfg->RX_QLock);
	ret = qinfo(chan_cfg->RX_Q);
	fs3ipc_os_ReleaseResource(chan_cfg->RX_QLock);

	return ret;
}

fs3ipc_StatusType fs3ipc_get_txq_msg_pending(unsigned int handle, uint32_t *info)
{
	return fs3ipc_get_qinfo(handle, fs3ipc_get_txq_info,
				fs3ipc_app_QueueMsgCnt, "Tx-Q msgs", info);
}

fs3ipc_StatusType fs3ipc_get_rxq_msg_pending(unsigned int handle, uint32_t *info)
{
	return fs3ipc_get_qinfo(handle, fs3ipc_get_rxq_info,
				fs3ipc_app_QueueMsgCnt, "Rx-Q msgs", info);
}

fs3ipc_StatusType fs3ipc_get_txq_free_space(unsigned int handle, uint32_t *info)
{
	return fs3ipc_get_qinfo(handle, fs3ipc_get_txq_info,
				fs3ipc_app_QueueFreeSpace, "Tx-Q space", info);
}

fs3ipc_StatusType fs3ipc_get_rxq_free_space(unsigned int handle, uint32_t *info)
{
	return fs3ipc_get_qinfo(handle, fs3ipc_get_rxq_info,
				fs3ipc_app_QueueFreeSpace, "Rx-Q space", info);
}

fs3ipc_StatusType fs3ipc_get_txq_size(unsigned int handle, uint32_t *info)
{
	return fs3ipc_get_qinfo(handle, fs3ipc_get_txq_info,
				fs3ipc_app_queue_total_size, "Tx-Q size", info);
}

fs3ipc_StatusType fs3ipc_get_rxq_size(unsigned int handle, uint32_t *info)
{
	return fs3ipc_get_qinfo(handle, fs3ipc_get_rxq_info,
				fs3ipc_app_queue_total_size, "Rx-Q size", info);
}

fs3ipc_StatusType fs3ipc_get_txq_free_msg_space(unsigned int handle,
						uint32_t *info)
{
	int32_t ret = 0;
	unsigned int ch = 0u;
	const fs3ipc_app_ltchancfg_t *chan_cfg = NULL;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		&instance, &ch);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	if (!info) {
		LOG_ERROR("[%u] CH-%u: NULL argument", instance, ch);
		return fs3ipc_StatusType_ErrorGen;
	}

	chan_cfg = fs3ipc_app_appConfigs[instance].ltChannelCfg[ch];
	fs3ipc_os_GetResource(chan_cfg->TX_QLock);
	ret = fs3ipc_app_queue_msg_freespace(chan_cfg->TX_Q, FS3_IPC_MAX_MSG_SIZE);
	fs3ipc_os_ReleaseResource(chan_cfg->TX_QLock);
	if (ret < 0) {
		LOG_ERROR("[%u] CH-%u: Failed to get TX-Q info: %d", instance,
			  ch, ret);
		return fs3ipc_StatusType_ErrorGen;
	}

	*info = ret;

	return fs3ipc_StatusType_OK;
}

fs3ipc_StatusType fs3ipc_open_ch(unsigned int handle)
{
	return fs3ipc_set_ch_state(handle, FS3IPC_TRUE);
}

fs3ipc_StatusType fs3ipc_app_close_ch(const struct fs3ipc_client *client)
{
	return fs3ipc_app_set_ch_state(client, FS3IPC_FALSE);
}

fs3ipc_StatusType fs3ipc_close_ch(unsigned int handle)
{
	return fs3ipc_set_ch_state(handle, FS3IPC_FALSE);
}

fs3ipc_StatusType fs3ipc_get_ch_available(unsigned int handle)
{
	unsigned int ch = 0u;
	const fs3ipc_app_ltcfg_t *app_cfg = NULL;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;
	const struct fs3ipc_client *client = fs3ipc_get_client(handle,
		&instance, &ch);

	if (!client)
		return fs3ipc_StatusType_ErrorHandle;

	app_cfg = &fs3ipc_app_appConfigs[instance];

	return fs3ipc_flowControl_GetTxEnabled(app_cfg->flowControlCfg, ch);
}

/**
 * fs3ipc_app_HandleExtNodeReset() - Resets the queue data.
 * @handle: Driver context index.
 *
 * This function flushes the information contained into the queues. This
 * function normally is called with error handling process.
 *
 * Return: The number of bytes encoded.
 */
fs3ipc_StatusType fs3ipc_app_HandleExtNodeReset(fs3ipc_handleType handle)
{
	fs3ipc_StatusType stat = fs3ipc_StatusType_OK;

	if (handle >= FS3IPC_APP_NUM_OF_INSTANCES) {
		/* This shouldn't happen. Config issue*/
		stat = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("invalid handle %d cfg", handle);
	}

	if (stat == fs3ipc_StatusType_OK) {
		const fs3ipc_app_ltcfg_t *appCfg = &fs3ipc_app_appConfigs[handle];

		fs3ipc_u8 chanIndex = appCfg->channelCount;

		fs3ipc_flowcontrol_HandleNodeReset(appCfg->flowControlCfg);
		/* Reset queues*/
		while (chanIndex-- > 0) {
			const fs3ipc_app_ltchancfg_t *chanCfg =
				appCfg->ltChannelCfg[chanIndex];

			if (chanCfg) {
				fs3ipc_os_GetResource(chanCfg->RX_QLock);
				fs3ipc_app_QueueClear(chanCfg->RX_Q);
				fs3ipc_os_ReleaseResource(chanCfg->RX_QLock);

				fs3ipc_os_GetResource(chanCfg->TX_QLock);
				fs3ipc_app_QueueClear(chanCfg->TX_Q);
				fs3ipc_os_ReleaseResource(chanCfg->TX_QLock);
			}
		}

		/* Reset flow control state*/
		memset(appCfg->rxMsg, 0, sizeof(*appCfg->rxMsg));
		memset(appCfg->txMsg, 0, sizeof(*appCfg->txMsg));
		*appCfg->rxMsgBufferOverflowIndex = 0;
	}
	return stat;
}

/**
 * fs3ipc_app_Encoder() - Encodes the received data buffer.
 * @handle: Driver context index.
 * @data: The received data for encoding.
 * @ptrLength: Number of bytes to encode.
 *
 * Validates the configuration parameters, encodes the partial message, it
 * provides the connection to  load the new message from the queues.
 *
 * Return: The number of bytes encoded.
 */
fs3ipc_StatusType fs3ipc_app_Encoder(fs3ipc_handleType handle, fs3ipc_dataPtr data,
				     fs3ipc_length *ptrLength)
{
	fs3ipc_StatusType stat = fs3ipc_StatusType_OK;
	fs3ipc_length index = 0;

	if (handle >= FS3IPC_APP_NUM_OF_INSTANCES) {
		/* This shouldn't happen. Config issue*/
		stat = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("invalid handle %d cfg", handle);
	} else if (!ptrLength) {
		stat = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("null length ptr arg");
	} else if (!data) {
		stat = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("null data ptr arg");
	} else if (*ptrLength <= (sizeof(fs3ipc_app_messageHdr) * 2)) {
		stat = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("buffer too small to encode message: %d", *ptrLength);
	}

	if (stat == fs3ipc_StatusType_OK) {
		const fs3ipc_app_ltcfg_t *cfg = &fs3ipc_app_appConfigs[handle];
		fs3ipc_app_priorityNodeType *currentChannel = cfg->orderedChannelListHead;
		fs3ipc_length length = *ptrLength;
		fs3ipc_s32 remBytes = length;
		fs3ipc_app_message_buffer_t *partialMessage = cfg->txMsg;

		/* Reset flow control, logical channel byte counter*/
		(void)fs3ipc_flowControl_ResetByteCountTxFrame(cfg->flowControlCfg);

		/*
		 * Step 1.
		 * Finish encoding partial message
		 */
		index += handleEncodeResumeMessage(&data[index], remBytes,
		partialMessage, cfg->flowControlCfg);

		/* Update number of remaining bytes*/
		remBytes = length - index;

		/*
		 * Step 2.
		 * Finish handle flow control
		 */
		if (remBytes > sizeof(fs3ipc_app_messageHdr))
			index += handleEncodeFlowControlMessage(&data[index], remBytes,
				partialMessage, cfg);
		/* Update number of remaining bytes*/
		remBytes = length - index;

		/* Get first channel */
		fs3ipc_app_PriorityNodeNext(&currentChannel);

		/* Step 3 - load new message from message queues*/
		while ((currentChannel) &&
			(remBytes > sizeof(fs3ipc_app_messageHdr))) {
			fs3ipc_s32 bytesTransferred = 0;

			bytesTransferred = handleEncodeNextMessage(&data[index],
				remBytes, partialMessage,
				(fs3ipc_app_ltchancfg_t *)currentChannel->data, cfg);
			if (bytesTransferred == 0)
				/* Get the next channel */
				fs3ipc_app_PriorityNodeNext(&currentChannel);

			/* Update number of remaining bytes*/
			index += bytesTransferred;
			remBytes = length - index;
		}

		/*
		 * If the index is equal to or less than the size of the resumed
		 * message header, it means that there is nothing to send.
		 */
		if (index <= sizeof(fs3ipc_app_messageHdr))
			*ptrLength = 0;
		else
			/* Update length*/
			*ptrLength = index;
	}

	return stat;
}

/**
 * fs3ipc_app_Decoder() - Decode the received data.
 * @handle: Driver context index.
 * @data: The received data for decoding.
 * @length: Length of the buffer.
 *
 * This function decodes the received buffer data, it verifies configuration
 * parameters, it resumes the received message or treat the message as new.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_app_Decoder(fs3ipc_handleType handle,
	fs3ipc_cDataPtr data,
	fs3ipc_length length)
{
	fs3ipc_StatusType stat = fs3ipc_StatusType_OK;

	if (handle >= FS3IPC_APP_NUM_OF_INSTANCES) {
		/* This shouldn't happen. Config issue*/
		stat = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("invalid handle %d cfg", handle);
	} else if (!data || length <= (sizeof(fs3ipc_app_messageHdr))) {
		stat = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("invalid data buffer addr:%p len:%d", (void *)data, length);
	}

	if (stat == fs3ipc_StatusType_OK) {
		const fs3ipc_app_ltcfg_t *appCfg = &fs3ipc_app_appConfigs[handle];
		fs3ipc_length index = 0;
		fs3ipc_s32 remBytes = length - index;

		if (*appCfg->rxMsgBufferOverflowIndex == 0 || length <=
			*appCfg->rxMsgBufferOverflowIndex) {
			index += handleDecodeResumeMessage(&data[index], remBytes, appCfg,
				&stat);
			remBytes = length - index;
			*appCfg->rxMsgBufferOverflowIndex = 0;
		} else {
		/*
		 * appCfg->rxMsgBufferOverflowIndex != 0
		 * Recovering from a buffer overflow. Skip wrapped message and resume from
		 * previous index so that messages are not queued twice. Per HDLC, the
		 * transmitter will retransmitted the same frame.
		 */
			index = *appCfg->rxMsgBufferOverflowIndex;
			remBytes = length - index;
			*appCfg->rxMsgBufferOverflowIndex = 0;
		}

		while (stat == fs3ipc_StatusType_OK &&
			remBytes > sizeof(fs3ipc_app_messageHdr)) {
			fs3ipc_s32 cnt;

			cnt = handleDecodeNewMessage(&data[index], remBytes, appCfg, &stat);
			if (stat == fs3ipc_StatusType_BufferFull)
				*appCfg->rxMsgBufferOverflowIndex = index;
			else
				index += cnt;

			remBytes = length - index;
		}
	}

	return stat;
}

#ifdef CONFIG_FS3IPC_DEBUG
/**
 * fs3ipc_app_GetStats() - Gets the Application layer statistics.
 * @Hndl: Driver context index.
 *
 * Return: The current status counter.
 */
const fs3ipc_app_stats_t *fs3ipc_app_GetStats(fs3ipc_handleType Hndl)
{
	fs3ipc_app_stats_t *ret = FS3IPC_NULL;

	if (Hndl < FS3IPC_NUM_OF_INSTANCES) {
		const fs3ipc_app_ltcfg_t *cfg = &fs3ipc_app_appConfigs[Hndl];

		ret = cfg->stats;
	}

	return ret;
}

/**
 * fs3ipc_app_ClearStats() - Clears the Application layer statistics.
 * @Hndl: Driver context index.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_app_ClearStats(fs3ipc_handleType Hndl)
{
	fs3ipc_StatusType stat = fs3ipc_StatusType_ErrorHandle;

	if (Hndl < FS3IPC_NUM_OF_INSTANCES) {
		const fs3ipc_app_ltcfg_t *cfg = &fs3ipc_app_appConfigs[Hndl];

		if (cfg->stats) {
			fs3ipc_memset(cfg->stats, 0, sizeof(fs3ipc_app_stats_t));
			stat = fs3ipc_StatusType_OK;
		}
	}
	return stat;
}

/**
 * fs3ipc_app_GetChanStats() - Returns the channel statistics.
 * @Hndl: Driver context index.
 * @channel: The channel to request the information.
 *
 * This function checks for the correct number of instances, if the Arguments
 * are correct, the get operation is executed.
 *
 * Return: The pointer to access the structure defined for collecting
 *         the statics.
 */
const fs3ipc_fc_chan_pbcfg_t *
fs3ipc_app_GetChanStats(fs3ipc_handleType Hndl, fs3ipc_u8 channel)
{
	const fs3ipc_fc_chan_pbcfg_t *ret = NULL;

	if ((Hndl < FS3IPC_NUM_OF_INSTANCES) &&
		(channel < FS3IPC_MAX_NUMBER_CHANNELS))
		ret = fs3ipc_flowcontrol_GetStats(
			fs3ipc_app_appConfigs[Hndl].flowControlCfg, channel);
	return ret;
}

/**
 * fs3ipc_app_ClearChanStats() - Empties the channel statistics.
 * @Hndl: Driver context index.
 * @channel: The channel to clear the information.
 *
 * This function checks for the correct number of instances, if the Arguments
 * are correct, the clear operation is executed.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_app_ClearChanStats(fs3ipc_handleType Hndl, fs3ipc_u8 channel)
{
	fs3ipc_StatusType stat = fs3ipc_StatusType_ErrorHandle;

	if ((Hndl < FS3IPC_NUM_OF_INSTANCES) &&
	    (channel < FS3IPC_MAX_NUMBER_CHANNELS))
		stat = fs3ipc_flowcontrol_ClearStats(
			fs3ipc_app_appConfigs[Hndl].flowControlCfg, channel);

	return stat;
}
#endif /*CONFIG_FS3IPC_DEBUG*/

/**
 * fs3ipc_app_SetLogging() - Enables message logging for a channel.
 * @Hndl: Driver context index.
 * @channel: The channel to clear the information.
 * @enabled: 0 to disable logging otherwise enable logging.
 *
 * This function enables message logging for a channel.
 *
 * Return: fs3ipc_StatusType_OK if OK otherwise failure.
 */
fs3ipc_StatusType fs3ipc_app_SetLogging(fs3ipc_handleType Hndl, fs3ipc_u8 channel,
					fs3ipc_u8 enabled)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const fs3ipc_app_ltchancfg_t *chanCfg;

	if ((Hndl >= FS3IPC_NUM_OF_INSTANCES) ||
	    (channel >= FS3IPC_MAX_NUMBER_CHANNELS))
		ret = fs3ipc_StatusType_ErrorHandle;

	if (ret == fs3ipc_StatusType_OK) {
		enabled = enabled ? FS3IPC_TRUE : FS3IPC_FALSE;
		if (channel == FS3IPC_APP_FLOWCONTROL_CHAN) {
			ret = fs3ipc_flowcontrol_SetLogging(
				 fs3ipc_app_appConfigs[Hndl].flowControlCfg, enabled);
		} else {
			chanCfg = fs3ipc_app_appConfigs[Hndl].ltChannelCfg[channel];
			if (!chanCfg)
				ret = fs3ipc_StatusType_ErrorCfg;
			else
				*chanCfg->loggingEnabled = enabled;
		}
	}
	return ret;
}
