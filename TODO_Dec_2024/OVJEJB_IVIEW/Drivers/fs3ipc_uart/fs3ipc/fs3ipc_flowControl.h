/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_flowControl.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * FS3IPC flowcontrol header file.
 *****************************************************************************/
#ifndef FS3IPC_FLOWCONTROL_H
#define FS3IPC_FLOWCONTROL_H

#ifdef __cplusplus
extern "C"
{
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
/**
 * fc channel dynamic configuration data
 */
typedef struct {
	fs3ipc_u8 txEnabled;
	fs3ipc_u8 rxOverflow;
	fs3ipc_u8 rxEnabled;
	fs3ipc_u16 maxTxPerFrame;
	fs3ipc_u16 byteCountTxFrame;
#ifdef CONFIG_FS3IPC_DEBUG
	fs3ipc_u16 rx_xoff_cnt;
	fs3ipc_u16 tx_xoff_cnt;
	fs3ipc_u32 rxCnt;
	fs3ipc_u32 txCnt;
	fs3ipc_u16 largest_pkt_sent;
	fs3ipc_u16 largest_pkt_rcvd;
	fs3ipc_u32 tx_byte_cnt;
	fs3ipc_u32 rx_byte_cnt;
#endif
} fs3ipc_fc_chan_pbcfg_t;

/**
 * fc instance dynamic configuration data
 */
typedef struct {
	fs3ipc_u8 pendingRxReply;
	fs3ipc_u8 pendingTxRequest;
	fs3ipc_u8 loggingEnabled;
} fs3ipc_flowCtrlPBCfgType;

/**
 * fc instance constant channel configuration data
 */
typedef struct {
	fs3ipc_fp_flowControlCb rxCallback;
	fs3ipc_u16 maxRxPerFrame;
} fs3ipc_fc_chan_ltcfg_t;

/**
 * fc instance constant instance configuration data
 */
typedef struct {
	fs3ipc_u8 channelCount;
	fs3ipc_flowCtrlPBCfgType *pbStConfig;
	fs3ipc_fc_chan_pbcfg_t *pbStChanConfig;
	const fs3ipc_fc_chan_ltcfg_t *ltStChanConfig;
	fs3ipc_os_resource_t *resLock;
} fs3ipc_fc_ltcfg_t;

/******************************************************************************
 * VARIABLES
 *****************************************************************************/

/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/
fs3ipc_u8 fs3ipc_flowControl_MsgPending(
	const fs3ipc_fc_ltcfg_t *appCfg);

fs3ipc_u32 fs3ipc_flowControl_MsgSize(
	fs3ipc_u8 channelCount);

fs3ipc_StatusType fs3ipc_flowControl_Decode(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_cDataPtr data,
	fs3ipc_length length);

fs3ipc_StatusType fs3ipc_flowControl_Encode(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_dataPtr data,
	fs3ipc_length *length);

fs3ipc_StatusType fs3ipc_flowControl_GetTxEnabled(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel);

fs3ipc_StatusType fs3ipc_flowControl_ResetByteCountTxFrame(
	const fs3ipc_fc_ltcfg_t *appCfg);
fs3ipc_StatusType fs3ipc_flowControl_GetFreeByteCountTxFrame(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel,
	fs3ipc_length *output);

fs3ipc_StatusType fs3ipc_flowControl_AddByteCountTxFrame(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel,
	fs3ipc_length byteCount);

fs3ipc_StatusType fs3ipc_flowControl_SetOverflow(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel,
	fs3ipc_u8 value);
fs3ipc_u32 fs3ipc_flowControl_GetMaxTxPerFrame(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel);

fs3ipc_u32 fs3ipc_flowControl_GetMaxRxPerFrame(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel);

fs3ipc_StatusType fs3ipc_flowControl_EnableRxClient(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel,
	fs3ipc_u8 enabled);

fs3ipc_StatusType fs3ipc_flowcontrol_Init(
	const fs3ipc_fc_ltcfg_t *appCfg);

fs3ipc_StatusType fs3ipc_flowcontrol_HandleNodeReset(
	const fs3ipc_fc_ltcfg_t *appCfg);

#ifdef CONFIG_FS3IPC_DEBUG

fs3ipc_StatusType fs3ipc_flowControl_IncRxMsgCnt(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel);

/**
 * fs3ipc_flowctrl_inc_rx_byte_cnt() - Inc no. of bytes received for channel
 * @app_cfg: Flow control config
 * @channel: Channel no.
 * @bytes: No. of bytes received in a packet
 *
 *
 * Return:
 * * fs3ipc_StatusType_OK       - OK
 * * fs3ipc_StatusType_ErrorGen - Invalid parameter
 */
fs3ipc_StatusType fs3ipc_flowctrl_inc_rx_byte_cnt(const fs3ipc_fc_ltcfg_t *app_cfg,
						  fs3ipc_u8 channel,
						  fs3ipc_u8 bytes);

/**
 * fs3ipc_flowctrl_update_largest_rx_pkt() - Update if largest packet received for channel
 * @app_cfg: Flow control config
 * @channel: Channel no.
 * @bytes: No. of bytes received in a packet
 *
 *
 * Return:
 * * fs3ipc_StatusType_OK       - OK
 * * fs3ipc_StatusType_ErrorGen - Invalid parameter
 */
fs3ipc_StatusType fs3ipc_flowctrl_update_largest_rx_pkt(const fs3ipc_fc_ltcfg_t *app_cfg,
							fs3ipc_u8 channel,
							fs3ipc_u8 bytes);

fs3ipc_StatusType fs3ipc_flowControl_IncTxMsgCnt(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel);

/**
 * fs3ipc_flowctrl_inc_tx_byte_cnt() - Inc no. of bytes transmitted for channel
 * @app_cfg: Flow control config
 * @channel: Channel no.
 * @bytes: No. of bytes transmitted in a packet
 *
 *
 * Return:
 * * fs3ipc_StatusType_OK       - OK
 * * fs3ipc_StatusType_ErrorGen - Invalid parameter
 */
fs3ipc_StatusType fs3ipc_flowctrl_inc_tx_byte_cnt(const fs3ipc_fc_ltcfg_t *app_cfg,
						  fs3ipc_u8 channel,
						  fs3ipc_u8 bytes);

/**
 * fs3ipc_flowctrl_update_largest_tx_pkt() - Update if largest packet transmitted for channel
 * @app_cfg: Flow control config
 * @channel: Channel no.
 * @bytes: No. of bytes transmitted in a packet
 *
 *
 * Return:
 * * fs3ipc_StatusType_OK       - OK
 * * fs3ipc_StatusType_ErrorGen - Invalid parameter
 */
fs3ipc_StatusType fs3ipc_flowctrl_update_largest_tx_pkt(const fs3ipc_fc_ltcfg_t *app_cfg,
							fs3ipc_u8 channel,
							fs3ipc_u8 bytes);

const fs3ipc_fc_chan_pbcfg_t *fs3ipc_flowcontrol_GetStats(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel);

fs3ipc_StatusType fs3ipc_flowcontrol_ClearStats(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 channel);

#else

#define fs3ipc_flowControl_IncRxMsgCnt(a, b)
#define fs3ipc_flowControl_IncTxMsgCnt(a, b)

#endif

fs3ipc_StatusType fs3ipc_flowcontrol_SetLogging(
	const fs3ipc_fc_ltcfg_t *appCfg,
	fs3ipc_u8 enabled);

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_FLOWCONTROL_H */
