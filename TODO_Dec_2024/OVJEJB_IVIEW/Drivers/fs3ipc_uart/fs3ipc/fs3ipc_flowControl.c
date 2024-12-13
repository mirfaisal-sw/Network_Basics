/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_flowControl.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * FS3IPC flowcontrol source file.
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_flowControl.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#ifdef LOG_MODULE
#undef LOG_MODULE
#endif
#define LOG_MODULE "fs3ipc_flowControl"

/** Expected protocol version*/
#define PROTOCOL_VERSION_ID_EXP (0x20)
/** Request message type*/
#define MSG_ID_CHANNEL_STATUS_REQUEST (0x01)
/** Reply message type*/
#define MSG_ID_CHANNEL_STATUS_REPLY (0x02)
/** Flow control header size */
#define CHANNEL_STATUS_HDR_SIZE (3)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
#pragma pack(1)
/** Header structure used for parsing/encoding flow control frame*/
typedef struct {
	fs3ipc_u8 protocolVersion;
	fs3ipc_u8 msgId;
} flowControlMsgHeader_t;

/** channel status field structure used for parsing/encoding flow control frame*/
typedef struct {
	fs3ipc_u16 maximumBytesPerFrame : 15;
	fs3ipc_u16 channelEnabled : 1;
} flowControlMsgChannelStatusField_t;

/** message Header structure used for parsing/encoding flow control frame*/
typedef struct {
	flowControlMsgHeader_t header;
	fs3ipc_u8 channelCount;
	flowControlMsgChannelStatusField_t channelStatusArray[FS3IPC_MAX_NUMBER_CHANNELS];
} flowControlMsgChannelStatus_t;
#pragma pack()

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
 * fs3ipc_flowcontrol_Init() - Initializes flow control.
 * @appCfg: Pointer to flow control config block.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Channel available.
 * * fs3ipc_StatusType_ErrorHandle - General error.
 */
fs3ipc_StatusType fs3ipc_flowcontrol_Init(const fs3ipc_fc_ltcfg_t *appCfg)
{
	fs3ipc_StatusType stat = fs3ipc_StatusType_OK;

	if (appCfg) {
		fs3ipc_fc_chan_pbcfg_t *chanCfg;
		fs3ipc_flowCtrlPBCfgType *cfg = appCfg->pbStConfig;
		fs3ipc_u8 channel = appCfg->channelCount;

		fs3ipc_os_GetResource(appCfg->resLock);
		fs3ipc_memset(cfg, 0, sizeof(fs3ipc_flowCtrlPBCfgType));

		for (channel = 0; channel < appCfg->channelCount; channel++) {
			chanCfg = &appCfg->pbStChanConfig[channel];
			fs3ipc_memset(chanCfg, 0, sizeof(fs3ipc_fc_chan_pbcfg_t));
		}

		/* flow control is always enabled*/
		chanCfg = &appCfg->pbStChanConfig[FS3IPC_APP_FLOWCONTROL_CHAN];
		chanCfg->rxEnabled = FS3IPC_TRUE;

		/* Set initial flow control tx pending*/
		cfg->pendingTxRequest = FS3IPC_TRUE;
		cfg->loggingEnabled = FS3IPC_FALSE;

		fs3ipc_os_ReleaseResource(appCfg->resLock);
	} else {
		stat = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("Invalid ccb ptr");
	}

	return stat;
}

/**
 * fs3ipc_flowControl_MsgPending() - Returns if a flow control message is
 *                                   pending transmit.
 * @appCfg: Pointer to flow control config block.
 *
 * Return:
 * * FS3IPC_TRUE  - True.
 * * FS3IPC_FALSE - False.
 */
fs3ipc_u8 fs3ipc_flowControl_MsgPending(const fs3ipc_fc_ltcfg_t *appCfg)
{
	fs3ipc_u8 ret = FS3IPC_FALSE;

	if (appCfg) {
		fs3ipc_flowCtrlPBCfgType *appPbCfg = appCfg->pbStConfig;

		if (appPbCfg  && ((appPbCfg->pendingRxReply > 0) ||
		    (appPbCfg->pendingTxRequest > 0)))
			ret = FS3IPC_TRUE;

		/*Nothing to do, empty else is removed*/
	} else {
		/* nothing to do*/
		LOG_ERROR("Invalid ccb ptr");
	}
	return ret;
}

/**
 * fs3ipc_flowControl_MsgSize() - Returns the size of a flow control message.
 * @channelCount: Channel count.
 *
 * Return: Size of flow control message.
 */
fs3ipc_u32 fs3ipc_flowControl_MsgSize(fs3ipc_u8 channelCount)
{
	return CHANNEL_STATUS_HDR_SIZE + (sizeof(flowControlMsgChannelStatusField_t)
		* channelCount);
}

/**
 * fs3ipc_flowControl_Decode() - Decodes a flow control message.
 * @appCfg: Pointer to flow control config block.
 * @data: Input buffer.
 * @length: Buffer size.
 *
 * Return:
 * * fs3ipc_StatusType_OK                  - Channel available.
 * * fs3ipc_StatusType_ChannelNotAvailable - Channel not available.
 * * fs3ipc_StatusType_ErrorGen            - General error.
 */
fs3ipc_StatusType fs3ipc_flowControl_Decode(const fs3ipc_fc_ltcfg_t *appCfg,
					    fs3ipc_cDataPtr data,
					    fs3ipc_length length)
{
	/* this should be called in a critical section*/

	fs3ipc_StatusType status = fs3ipc_StatusType_OK;

	if (!appCfg) {
		status = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("Invalid ccb ptr");
	} else {
		uint8_t channelCount = appCfg->channelCount;
		const flowControlMsgHeader_t *hdr = (const flowControlMsgHeader_t *)&data[0];

		LOG_DATA(appCfg->pbStConfig->loggingEnabled, &data[0], length,
			LOG_DATA_MAX_BYTES, "FLowcontrol Rx (C%02d L%03d)",
			FS3IPC_APP_FLOWCONTROL_CHAN, length);
		fs3ipc_flowctrl_inc_rx_byte_cnt(appCfg, FS3IPC_APP_FLOWCONTROL_CHAN, length);
		fs3ipc_flowctrl_update_largest_rx_pkt(appCfg, FS3IPC_APP_FLOWCONTROL_CHAN, length);
		if (length < sizeof(flowControlMsgHeader_t)) {
			LOG_ERROR("Buffer too small:%d", length);
		} else if (hdr->protocolVersion != PROTOCOL_VERSION_ID_EXP) {
			LOG_ERROR("Wrong protocol version:%02X", hdr->protocolVersion);
		} else {
			switch (hdr->msgId) {
			/* update */
			case MSG_ID_CHANNEL_STATUS_REQUEST:
				/* deliberate fall-through. no break*/

			case MSG_ID_CHANNEL_STATUS_REPLY:
			{
				const flowControlMsgChannelStatus_t *channelStatusMsg =
					 (const flowControlMsgChannelStatus_t *)hdr;

				if (length < CHANNEL_STATUS_HDR_SIZE) {
					status = fs3ipc_StatusType_ErrorGen;
					LOG_ERROR("Buffer too small:%d", length);
				} else if (channelStatusMsg->channelCount >
					FS3IPC_MAX_NUMBER_CHANNELS) {
					status = fs3ipc_StatusType_ErrorGen;
					LOG_ERROR("Invalid channel count:%d",
						channelStatusMsg->channelCount);
				} else {
#ifdef CONFIG_FS3IPC_DEBUG
					appCfg->pbStChanConfig[FS3IPC_APP_FLOWCONTROL_CHAN].rxCnt++;
#endif

					/* validate size of message*/
					if (length < (CHANNEL_STATUS_HDR_SIZE +
						(channelStatusMsg->channelCount *
						sizeof(flowControlMsgChannelStatusField_t)))) {
						status = fs3ipc_StatusType_ErrorGen;
						LOG_ERROR("Buffer too small:%d", length);
					} else {
						fs3ipc_u8 i = 0;

						if (channelCount != channelStatusMsg->channelCount) {
							LOG_NOTICE("Channel count mismatch: configured %d, received %d",
								channelCount, channelStatusMsg->channelCount);
							/* clamp channel*/
							channelCount = (channelCount <  channelStatusMsg->channelCount)
								? channelCount : channelStatusMsg->channelCount;
						}
						for (i = 0; i < channelCount; i++) {

							const fs3ipc_fp_flowControlCb rxCbFn =
								 appCfg->ltStChanConfig[i].rxCallback;
							fs3ipc_fc_chan_pbcfg_t *chanPbConfig =
								 &appCfg->pbStChanConfig[i];
							fs3ipc_u8 channelStatus;
							fs3ipc_u8 newChannelStatus;

							fs3ipc_os_GetResource(appCfg->resLock);

							channelStatus = chanPbConfig->txEnabled;
							newChannelStatus =
								channelStatusMsg->channelStatusArray[i].channelEnabled;
							chanPbConfig->txEnabled = newChannelStatus;
							chanPbConfig->maxTxPerFrame =
								channelStatusMsg->channelStatusArray[i].maximumBytesPerFrame;
							fs3ipc_os_ReleaseResource(appCfg->resLock);

							/* call channel callback*/
							if (channelStatus != newChannelStatus) {

								if (newChannelStatus)
									LOG_NOTICE("Channel %d Available", i);
								else
									LOG_NOTICE("Channel %d Unavailable", i);

								if (rxCbFn) {
									LOG_DEBUG("Calling channel %d CB", i);

									/*
									 * If the image supports position independent code add
									 * code offset to the linker time function pointer before
									 * executing
									 */
									#if FS3IPC_PIC_EN == 1
										rxCbFn = (fs3ipc_fp_flowControlCb)((void *)rxCbFn +
											fs3ipc_getCodeOffset());
									#endif /*#if FS3IPC_PIC_EN == 1*/

									rxCbFn(newChannelStatus, channelStatus);
								}
							}
						}
					}
				}
				break;
			}
			default:
				LOG_ERROR("Invalid MSG ID:%02Xh", hdr->msgId);
				status = fs3ipc_StatusType_ErrorGen;
				break;
			}
		}
	}

	return status;
}

/**
 * fs3ipc_flowControl_Encode() - Encodes a flow control message.
 * @appCfg: Pointer to flow control config block.
 * @data: Output buffer.
 * @length:  Length of input buffer and returns amount of data decoded.
 *
 * Return:
 * * fs3ipc_StatusType_OK                  - Channel available.
 * * fs3ipc_StatusType_ChannelNotAvailable - Channel not available.
 * * fs3ipc_StatusType_ErrorGen            - General error.
 */
fs3ipc_StatusType fs3ipc_flowControl_Encode(const fs3ipc_fc_ltcfg_t *appCfg,
					    fs3ipc_dataPtr data,
					    fs3ipc_length *length)
{
	fs3ipc_StatusType status = fs3ipc_StatusType_OK;

	if (!appCfg) {
		status = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("Invalid ccb ptr");
	} else if (!data) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Null data ptr arg");
	} else if (!length) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Null length arg ptr");
	} else {
		fs3ipc_flowCtrlPBCfgType *pbCfg = appCfg->pbStConfig;
		flowControlMsgChannelStatus_t *flowCtrlMsg =
			(flowControlMsgChannelStatus_t *)data;
		fs3ipc_u8 pendingRxReply = pbCfg->pendingRxReply;
		fs3ipc_u8 pendingTxRequest = pbCfg->pendingTxRequest;
		fs3ipc_u32 msgSize = CHANNEL_STATUS_HDR_SIZE +
			(sizeof(flowControlMsgChannelStatusField_t) * appCfg->channelCount);

		if (*length < msgSize) {
			status = fs3ipc_StatusType_ErrorGen;
			LOG_ERROR("Buffer too small:%d", *length);
			*length = 0;
		} else if (pendingRxReply || pendingTxRequest) {
			fs3ipc_u32 chan;

			pbCfg->pendingRxReply = FS3IPC_FALSE;
			pbCfg->pendingTxRequest = FS3IPC_FALSE;

#ifdef CONFIG_FS3IPC_DEBUG
			appCfg->pbStChanConfig[FS3IPC_APP_FLOWCONTROL_CHAN].txCnt++;
#endif

			flowCtrlMsg->header.protocolVersion = PROTOCOL_VERSION_ID_EXP;

			/* pending reply takes priority. This will minimize the amount of
			 *flow control traffic
			 */
			if (pendingRxReply != FS3IPC_FALSE)
				flowCtrlMsg->header.msgId = MSG_ID_CHANNEL_STATUS_REPLY;
			else if (pendingTxRequest != FS3IPC_FALSE)
				flowCtrlMsg->header.msgId = MSG_ID_CHANNEL_STATUS_REQUEST;

			fs3ipc_os_GetResource(appCfg->resLock);

			flowCtrlMsg->channelCount = appCfg->channelCount;

			for (chan = 0; chan < appCfg->channelCount; chan++) {
				if (chan != FS3IPC_APP_FLOWCONTROL_CHAN) {
					fs3ipc_fc_chan_pbcfg_t *chanPBCfg =
						&appCfg->pbStChanConfig[chan];
					flowCtrlMsg->channelStatusArray[chan].channelEnabled =
						((chanPBCfg->rxEnabled == FS3IPC_TRUE) &&
						(chanPBCfg->rxOverflow == FS3IPC_FALSE));

					flowCtrlMsg->channelStatusArray[chan].maximumBytesPerFrame =
						appCfg->ltStChanConfig[chan].maxRxPerFrame;
				} else {
					flowCtrlMsg->channelStatusArray[chan].channelEnabled =
						FS3IPC_TRUE;
					flowCtrlMsg->channelStatusArray[chan].maximumBytesPerFrame =
						FS3_IPC_MAX_MSG_SIZE;
				}
			}

			fs3ipc_os_ReleaseResource(appCfg->resLock);

			LOG_DATA(pbCfg->loggingEnabled, &data[0], msgSize,
				LOG_DATA_MAX_BYTES, "FLowcontrol Tx (C%02d L%03d)",
				FS3IPC_APP_FLOWCONTROL_CHAN, msgSize);
			*length = msgSize;
			fs3ipc_flowctrl_inc_tx_byte_cnt(appCfg,
							FS3IPC_APP_FLOWCONTROL_CHAN,
							*length);
			fs3ipc_flowctrl_update_largest_tx_pkt(appCfg,
							      FS3IPC_APP_FLOWCONTROL_CHAN,
							      *length);

		} else {
			/* nothing to do*/
			*length = 0;
		}
	}

	return status;
}

/**
 * fs3ipc_flowControl_GetTxEnabled() - Returns if a channel is available for
 *                                     transmit.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 *
 * Return:
 * * fs3ipc_StatusType_OK                  - Channel available.
 * * fs3ipc_StatusType_ChannelNotAvailable - Channel not available.
 */
fs3ipc_StatusType fs3ipc_flowControl_GetTxEnabled(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel)
{
	fs3ipc_StatusType status = fs3ipc_StatusType_OK;

	if (appCfg && channel < appCfg->channelCount) {

		if (appCfg->pbStChanConfig[channel].txEnabled == FS3IPC_FALSE) {
			status = fs3ipc_StatusType_ChannelNotAvailable;
			LOG_DEBUG("CH-%u: Channel unavailable", (unsigned int)channel);
		}

			/*Else channel is enabled, empty else is removed*/
	} else {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Invalid ccb ptr or channel(%u)", (unsigned int)channel);
	}

	return status;
}

/**
 * fs3ipc_flowControl_ResetByteCountTxFrame() - Gets the resets the number of
 *                                              bytes transmitted for all
 *                                              logical channels.
 * @appCfg: Pointer to flow control config block.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorHandle - Invalid handle.
 */
fs3ipc_StatusType fs3ipc_flowControl_ResetByteCountTxFrame(
	const fs3ipc_fc_ltcfg_t *appCfg)
{
	fs3ipc_StatusType status = fs3ipc_StatusType_OK;
	fs3ipc_u8 i = 0;

	if (appCfg) {
		/*
		 * No need to syncrhonize. byteCountTxFrame is only set by a
		 * single thread
		 */
		for (i = 0; i < appCfg->channelCount; i++)
			appCfg->pbStChanConfig[i].byteCountTxFrame = 0;
	} else {
		status = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("Invalid ccb ptr");
	}

	return status;
}

/**
 * fs3ipc_flowControl_GetFreeByteCountTxFrame() - Gets the number of bytes
 *                                                transmitted for a given
 *                                                logical channel, per frame.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 * @output: Number of bytes transmitted.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorGen    - General error.
 * * fs3ipc_StatusType_ErrorHandle - Invalid handle.
 */
fs3ipc_StatusType fs3ipc_flowControl_GetFreeByteCountTxFrame(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel,
	fs3ipc_length *output)
{
	fs3ipc_StatusType status = fs3ipc_StatusType_OK;

	if (!appCfg) {
		status = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("Invalid ccb ptr");
	} else if (channel >= appCfg->channelCount) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Invalid channel number:%d", channel);
	} else if (!output) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Null output ptr arg");
	} else {
		fs3ipc_fc_chan_pbcfg_t *pbCfg = &appCfg->pbStChanConfig[channel];

		if (pbCfg->maxTxPerFrame > pbCfg->byteCountTxFrame)
			*output = pbCfg->maxTxPerFrame - pbCfg->byteCountTxFrame;
		else
			*output = 0;
	}

	return status;
}

/**
 * fs3ipc_flowControl_AddByteCountTxFrame() - Increments the number of bytes
 *                                            which have been transmitted to a
 *                                            given client per frame.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 * @byteCount: Number of bytes transmitted.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorGen    - General error.
 * * fs3ipc_StatusType_ErrorHandle - Invalid handle.
 */
fs3ipc_StatusType fs3ipc_flowControl_AddByteCountTxFrame(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel,
	fs3ipc_length byteCount)
{
	fs3ipc_StatusType status = fs3ipc_StatusType_OK;

	if (!appCfg) {
		status = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("Invalid ccb ptr");
	} else if (channel >= appCfg->channelCount) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Invalid channel number:%d", channel);
	} else {
		fs3ipc_fc_chan_pbcfg_t *pbCfg = &appCfg->pbStChanConfig[channel];

		pbCfg->byteCountTxFrame += byteCount;
	}

	return status;
}

/**
 * fs3ipc_flowControl_SetOverflow() - Sets a queue overflow state.
 * @appCfg:  Pointer to flow control config block.
 * @channel: Logical channel number.
 * @value: 0 to overflow inactive and 1 to overflow active.
 *
 * This queues a flowcontrol message informing the external client not to send
 * any additional message for a given client.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorGen    - General error.
 * * fs3ipc_StatusType_ErrorHandle - Invalid handle.
 */
fs3ipc_StatusType fs3ipc_flowControl_SetOverflow(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel,
	fs3ipc_u8 value)
{
	fs3ipc_StatusType status = fs3ipc_StatusType_OK;

	if (!appCfg) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Invalid ccb ptr");
	} else if (channel >= appCfg->channelCount) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Invalid channel number: %u", (unsigned int)channel);
	} else {
		fs3ipc_u8 currentStat;

		fs3ipc_os_GetResource(appCfg->resLock);
		currentStat = appCfg->pbStChanConfig[channel].rxOverflow;
		value = (value == FS3IPC_FALSE ? FS3IPC_FALSE : FS3IPC_TRUE);

		if (value != currentStat) {
			appCfg->pbStChanConfig[channel].rxOverflow = value;
			appCfg->pbStConfig->pendingTxRequest = FS3IPC_TRUE;

#ifdef CONFIG_FS3IPC_DEBUG
			if (value == FS3IPC_TRUE)
				appCfg->pbStChanConfig[channel].tx_xoff_cnt++;
#endif
		}
		fs3ipc_os_ReleaseResource(appCfg->resLock);
	}

	return status;
}

/**
 * fs3ipc_flowControl_GetMaxTxPerFrame() - Gets the maximum number of bytes that
 *                                         can be transmitted per frame for a
 *                                         given client, for the external node.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 *
 * Return: Number of bytes if available, 0 if not available or error.
 */
fs3ipc_u32 fs3ipc_flowControl_GetMaxTxPerFrame(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel)
{
	fs3ipc_u32 size = 0;

	if (!appCfg)
		LOG_ERROR("Invalid ccb ptr");
	else if (channel >= appCfg->channelCount)
		LOG_ERROR("Invalid channel number: %u", (unsigned int)channel);
	else
		size = appCfg->pbStChanConfig[channel].maxTxPerFrame;

	return size;
}

/**
 * fs3ipc_flowControl_GetMaxRxPerFrame() - Gets the maximum number of bytes that
 *                                         can be received per frame for a given
 *                                         client, for the internal node.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 *
 * Return: Number of bytes if available, 0 if not available or error.
 */
fs3ipc_u32 fs3ipc_flowControl_GetMaxRxPerFrame(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel)
{
	fs3ipc_u32 size = 0;

	if (!appCfg)
		LOG_ERROR("Invalid ccb ptr");
	else if (channel >= appCfg->channelCount)
		LOG_ERROR("Invalid channel number:%d", channel);
	else
		size = appCfg->ltStChanConfig[channel].maxRxPerFrame;

	return size;
}

/**
 * fs3ipc_flowControl_EnableRxClient() - Schedules a flow control message to
 *                                       inform the external client that a
 *                                       given channel is ready or not to
 *                                       receive.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 * @enabled: 0 to disable and 1 to enable.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - Operation successful.
 * * fs3ipc_StatusType_ErrorGen         - General error.
 * * fs3ipc_StatusType_ErrorClientState - Client is already in requested state.
 * * fs3ipc_StatusType_ErrorHandle      - Invalid handle.
 */
fs3ipc_StatusType fs3ipc_flowControl_EnableRxClient(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel,
	fs3ipc_u8 enabled)
{
	fs3ipc_StatusType status = fs3ipc_StatusType_OK;

	if (!appCfg) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Invalid ccb ptr");
	} else if (channel >= appCfg->channelCount) {
		status = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("Invalid channel number: %u", (unsigned int)channel);
	} else {
		fs3ipc_u8 currentStat;

		fs3ipc_os_GetResource(appCfg->resLock);
		currentStat = appCfg->pbStChanConfig[channel].rxEnabled;
		enabled = enabled == FS3IPC_FALSE ? FS3IPC_FALSE : FS3IPC_TRUE;

		if (enabled != currentStat) {
			appCfg->pbStConfig->pendingTxRequest = FS3IPC_TRUE;

			appCfg->pbStChanConfig[channel].rxEnabled = enabled;
#ifdef CONFIG_FS3IPC_DEBUG
			if (enabled == FS3IPC_FALSE)
				appCfg->pbStChanConfig[channel].rx_xoff_cnt++;
#endif
		} else {
			if (currentStat)
				LOG_ERROR("CH-%u: Channel already open", (unsigned int)channel);
			else
				LOG_ERROR("CH-%u: Channel is not open", (unsigned int)channel);
			/* client already enabled. only one client allowed*/
			status = fs3ipc_StatusType_ErrorClientState;
		}

		fs3ipc_os_ReleaseResource(appCfg->resLock);
	}

	return status;
}

/**
 * fs3ipc_flowcontrol_HandleNodeReset() - Clears data related to an external
 *                                        node, and should called upon reset.
 * @appCfg: Pointer to flow control config block.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorHandle - Operation not successful.
 */
fs3ipc_StatusType fs3ipc_flowcontrol_HandleNodeReset(
	 const fs3ipc_fc_ltcfg_t *appCfg)
{
	fs3ipc_StatusType status = fs3ipc_StatusType_OK;

	if (!appCfg) {
		status = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("Invalid ccb ptr");
	}

	if (status == fs3ipc_StatusType_OK) {
		fs3ipc_flowCtrlPBCfgType *pbcfg = appCfg->pbStConfig;
		fs3ipc_u8 cnt = appCfg->channelCount;

		while (cnt-- > 0) {
			fs3ipc_fc_chan_pbcfg_t *chcfg = &appCfg->pbStChanConfig[cnt];

			if (chcfg) {
				const fs3ipc_fp_flowControlCb rxCbFn =
					 appCfg->ltStChanConfig[cnt].rxCallback;
				fs3ipc_u8 myTxEnabled;

				fs3ipc_os_GetResource(appCfg->resLock);
				myTxEnabled = chcfg->txEnabled;

#ifdef CONFIG_FS3IPC_DEBUG
				if (myTxEnabled)
					chcfg->tx_xoff_cnt++;
#endif
				chcfg->txEnabled = 0;
				chcfg->maxTxPerFrame = 0;
				fs3ipc_os_ReleaseResource(appCfg->resLock);

				if (myTxEnabled != 0 && rxCbFn) {
					/*
					 * If the image supports position independent code add code
					 * offset to the linker time  function pointer before executing
					 */
					#if FS3IPC_PIC_EN == 1
					rxCbFn = (fs3ipc_fp_flowControlCb)((void *)rxCbFn +
						fs3ipc_getCodeOffset());
					#endif /*#if FS3IPC_PIC_EN == 1*/

					rxCbFn(FS3IPC_FALSE, FS3IPC_TRUE);
				}
			}
		}

		pbcfg->pendingTxRequest = FS3IPC_TRUE;
	}
	return status;
}

/**
 * fs3ipc_flowControl_IncRxMsgCnt() - Increments the number of messages
 *                                    received stats.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorHandle - Operation not successful.
 */
#ifdef CONFIG_FS3IPC_DEBUG
fs3ipc_StatusType fs3ipc_flowControl_IncRxMsgCnt(const fs3ipc_fc_ltcfg_t *appCfg,
						 fs3ipc_u8 channel)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_ErrorGen;

	if (appCfg && channel < appCfg->channelCount) {
		fs3ipc_fc_chan_pbcfg_t *chanCfg = &appCfg->pbStChanConfig[channel];

		chanCfg->rxCnt++;
		ret = fs3ipc_StatusType_OK;
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_flowctrl_inc_rx_byte_cnt(const fs3ipc_fc_ltcfg_t *app_cfg,
						  fs3ipc_u8 channel,
						  fs3ipc_u8 bytes)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_ErrorGen;

	if (app_cfg && channel < app_cfg->channelCount) {
		fs3ipc_fc_chan_pbcfg_t *chan_cfg =
			&app_cfg->pbStChanConfig[channel];

		chan_cfg->rx_byte_cnt += bytes;
		ret = fs3ipc_StatusType_OK;
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_flowctrl_update_largest_rx_pkt(const fs3ipc_fc_ltcfg_t *app_cfg,
							fs3ipc_u8 channel,
							fs3ipc_u8 bytes)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_ErrorGen;

	if (app_cfg && channel < app_cfg->channelCount) {
		fs3ipc_fc_chan_pbcfg_t *chan_cfg =
			&app_cfg->pbStChanConfig[channel];

		if (bytes > chan_cfg->largest_pkt_rcvd)
			chan_cfg->largest_pkt_rcvd = bytes;
		ret = fs3ipc_StatusType_OK;
	}

	return ret;
}

/**
 * fs3ipc_flowControl_IncTxMsgCnt() - Increments the number of messages
 *                                    transmitted stats.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorHandle - Operation not successful.
 */
fs3ipc_StatusType fs3ipc_flowControl_IncTxMsgCnt(const fs3ipc_fc_ltcfg_t *appCfg,
						 fs3ipc_u8 channel)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_ErrorGen;

	if (appCfg && channel < appCfg->channelCount) {
		fs3ipc_fc_chan_pbcfg_t *chanCfg = &appCfg->pbStChanConfig[channel];

		chanCfg->txCnt++;
		ret = fs3ipc_StatusType_OK;
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_flowctrl_inc_tx_byte_cnt(const fs3ipc_fc_ltcfg_t *app_cfg,
						  fs3ipc_u8 channel,
						  fs3ipc_u8 bytes)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_ErrorGen;

	if (app_cfg && channel < app_cfg->channelCount) {
		fs3ipc_fc_chan_pbcfg_t *chan_cfg = &app_cfg->pbStChanConfig[channel];

		chan_cfg->tx_byte_cnt += bytes;
		ret = fs3ipc_StatusType_OK;
	}

	return ret;
}

fs3ipc_StatusType fs3ipc_flowctrl_update_largest_tx_pkt(const fs3ipc_fc_ltcfg_t *app_cfg,
							fs3ipc_u8 channel,
							fs3ipc_u8 bytes)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_ErrorGen;

	if (app_cfg && channel < app_cfg->channelCount) {
		fs3ipc_fc_chan_pbcfg_t *chan_cfg =
			&app_cfg->pbStChanConfig[channel];

		if (bytes > chan_cfg->largest_pkt_sent)
			chan_cfg->largest_pkt_sent = bytes;
		ret = fs3ipc_StatusType_OK;
	}

	return ret;
}

/**
 * fs3ipc_flowcontrol_GetStats() - Returns flow control statistics.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 *
 * Return: NULL if error, else flow control channel statistics.
 */
const fs3ipc_fc_chan_pbcfg_t *fs3ipc_flowcontrol_GetStats(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel)
{
	fs3ipc_fc_chan_pbcfg_t *ret = FS3IPC_NULL;

	if (appCfg && channel < appCfg->channelCount)
		ret = &appCfg->pbStChanConfig[channel];

	return ret;
}

/**
 * fs3ipc_flowcontrol_ClearStats() - Resets flow control statistics buffer.
 * @appCfg: Pointer to flow control config block.
 * @channel: Logical channel number.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorHandle - Operation not successful.
 */
fs3ipc_StatusType fs3ipc_flowcontrol_ClearStats(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel)
{

	fs3ipc_StatusType ret = fs3ipc_StatusType_ErrorGen;

	if (appCfg && channel < appCfg->channelCount) {
		fs3ipc_fc_chan_pbcfg_t *stat = &appCfg->pbStChanConfig[channel];

		stat->rxCnt = 0;
		stat->txCnt = 0;
		stat->rx_xoff_cnt = 0;
		stat->tx_xoff_cnt = 0;

		ret = fs3ipc_StatusType_OK;
	}
	return ret;
}
#endif

/**
 * fs3ipc_flowcontrol_SetLogging() - Enables message logging for the flow
 *                                   control channel.
 * @appCfg: Pointer to flow control config block.
 * @enabled: 0 to disable logging and 1 to enable logging.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - Operation successful.
 * * fs3ipc_StatusType_ErrorHandle - Operation not successful.
 */
fs3ipc_StatusType fs3ipc_flowcontrol_SetLogging(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 enabled)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	if (!appCfg)
		ret = fs3ipc_StatusType_ErrorGen;

	if (ret == fs3ipc_StatusType_OK) {
		if (enabled)
			appCfg->pbStConfig->loggingEnabled = FS3IPC_TRUE;
		else
			appCfg->pbStConfig->loggingEnabled = FS3IPC_FALSE;
	}
	return ret;
}
