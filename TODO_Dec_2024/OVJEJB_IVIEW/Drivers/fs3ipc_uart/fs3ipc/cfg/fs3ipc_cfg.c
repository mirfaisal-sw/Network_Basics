/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_cfg.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <David Rogala@harman.com>
 *
 * FS3IPC configuration source file.
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_phys.h"
#include "fs3ipc_hdlc.h"
#include "fs3ipc_app.h"
#include "fs3ipc_timer_priv.h"
#include "ipc.h"
#include "orrlist.h"

#if FS3IPC_LOOPBACK_ENABLED
#include "../fs3ipc_loopback.h"
#endif

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
/*
 * Timer configuration
 */
#define __FS3IPC_TIMER_INITIALIZER(timer_var, timer_name, instance, cb)  \
	{                                                                \
		.fs3ipc_instance = instance,                             \
		.name = #timer_name "/" #instance,                       \
		.callback = &(cb),                                       \
		.ctx = {                                                 \
			.timer = &(timer_var),                           \
		}                                                        \
	}

#define __DEFINE_FS3IPC_TIMER(timer_var, timer_name, instance, cb)       \
	static struct fs3ipc_timer timer_var =                           \
		__FS3IPC_TIMER_INITIALIZER(timer_var, timer_name, instance, cb)

#define _DEFINE_FS3IPC_TIMER(timer, instance, cb)                        \
	__DEFINE_FS3IPC_TIMER(fs3ipc_##instance##_##timer, timer, instance, cb)

/*
 * Helper macro to define timer.
 * Timer variable name will be 'fs3ipc_<instance>_<timer>' & timer.name will be
 * "<timer>/<instance>"
 */
#define DEFINE_FS3IPC_TIMER(timer, instance, cb)                         \
	_DEFINE_FS3IPC_TIMER(timer, instance, cb)

DEFINE_FS3IPC_TIMER(uframe_timer, FS3IPC_INSTANCE_0, process_uframe_timeout);
DEFINE_FS3IPC_TIMER(iframe_timer, FS3IPC_INSTANCE_0, process_iframe_timeout);

static fs3ipc_os_event_t app_0_hdlcEvent;
static fs3ipc_os_resource_t app_0_hdlcLock;

/*
 * physical layer configuration
 */


/*
 * Data-Link (HDLC) layer configuration
 */

#ifdef CONFIG_FS3IPC_DEBUG
static fs3ipc_hdlc_stats_t hdlc_stats_i0;
#endif

#define FS3IPC_UFRAME_TIMEOUT_MSEC      (200u)
#define FS3IPC_IFRAME_TIMEOUT_MSEC      (200u)

fs3ipc_hdlc_AppConfigType
	fs3ipc_hdlc_appConfigs[FS3IPC_HDLC_NUM_OF_INSTANCES] = {
	/* instance 0 - FS3IPC_HDLC_I0_HANDLE*/
	{
		.uframe_timer = &fs3ipc_0_uframe_timer,
		.iframe_timer = &fs3ipc_0_iframe_timer,
		.uframe_timeout = FS3IPC_UFRAME_TIMEOUT_MSEC,
		.iframe_timeout = FS3IPC_IFRAME_TIMEOUT_MSEC,
		.uWindowSiz = WINDOW_SIZE,
		.uBufferSiz = FS3IPC_TRANS_SIZE_MAX,
		.hdlcOSCfg = {
			.OS_Task = &app_0_hdlcEvent,
			.event_task = EVENT_EXT_WAKEUP,
		},
#ifdef CONFIG_FS3IPC_DEBUG
		.stats = &hdlc_stats_i0,
#endif
		.lock = &app_0_hdlcLock,
	},
};


/*
 * Application Layer Configuration
 */
#ifdef CONFIG_FS3IPC_DEBUG
static fs3ipc_app_stats_t app_0_stats;
#endif

struct fs3ipc_client fs3ipc_client_lookup_table[FS3IPC_CLIENT_HANDLE_COUNT];

#define ONOFF_IPC_PRIO (1)
#define RTC_IPC_PRIO (2)
#define NAV_IPC_PRIO (3)

#define TX_BUFFER_SIZE (1024)
#define RX_BUFFER_SIZE (32768)

#define LTCHANCFG_DEF(name, inst, chan, prio, hndl, txSize, rxSize) \
static fs3ipc_u8 app_##inst##_TxChannel##chan##Buf[txSize];\
static fs3ipc_u8 app_##inst##_RxChannel##chan##Buf[rxSize];\
static VLQueue_ccb_t app_##inst##_TxChannel##chan##Q;\
static VLQueue_ccb_t app_##inst##_RxChannel##chan##Q;\
static struct mutex app_##inst##_TxChannel##chan##QLock;\
static struct mutex app_##inst##_RxChannel##chan##QLock;\
static fs3ipc_os_event_t app_##inst##_Channel##chan##TxEvent;\
static fs3ipc_u8 app_##inst##_RxChannel##chan##loggingEnabled;\
static const fs3ipc_app_ltchancfg_t name = {\
	.TX_Buffer = &app_##inst##_TxChannel##chan##Buf[0],\
	.RX_Buffer = &app_##inst##_RxChannel##chan##Buf[0],\
	.priority = prio,\
	.channel = chan,\
	.TX_Buffer_Size = txSize,\
	.RX_Buffer_Size = rxSize,\
	.TX_Q = &app_##inst##_TxChannel##chan##Q,\
	.TX_QLock = &app_##inst##_TxChannel##chan##QLock,\
	.RX_Q = &app_##inst##_RxChannel##chan##Q,\
	.RX_QLock = &app_##inst##_RxChannel##chan##QLock,\
	.client_cfg = {\
		.handle = hndl,\
		.osCfg = &app_##inst##_Channel##chan##TxEvent,\
		.rxEvent = (1),\
		.txevent = (2),\
	},\
	.loggingEnabled = &app_##inst##_RxChannel##chan##loggingEnabled,\
}


LTCHANCFG_DEF(app_0_Chan2Cfg, 0, 2, 1, FS3IPC_0_CH2_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan3Cfg, 0, 3, 1, FS3IPC_0_CH3_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan4Cfg, 0, 4, 1, FS3IPC_0_CH4_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan5Cfg, 0, 5, 1, FS3IPC_0_CH5_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan6Cfg, 0, 6, 1, FS3IPC_0_CH6_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan7Cfg, 0, 7, 1, FS3IPC_0_CH7_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan8Cfg, 0, 8, 1, FS3IPC_0_CH8_HANDLE,
	1029, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan9Cfg, 0, 9, 1, FS3IPC_0_CH9_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan10Cfg, 0, 10, 1, FS3IPC_0_CH10_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan11Cfg, 0, 11, 1, FS3IPC_0_CH11_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan12Cfg, 0, 12, 1, FS3IPC_0_CH12_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan13Cfg, 0, 13, 1, FS3IPC_0_CH13_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan14Cfg, 0, 14, 1, FS3IPC_0_CH14_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan15Cfg, 0, 15, 1, FS3IPC_0_CH15_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan16Cfg, 0, 16, 1, FS3IPC_0_CH16_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan17Cfg, 0, 17, 1, FS3IPC_0_CH17_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan18Cfg, 0, 18, 1, FS3IPC_0_CH18_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan19Cfg, 0, 19, 1, FS3IPC_0_CH19_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan20Cfg, 0, 20, 1, FS3IPC_0_CH20_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan21Cfg, 0, 21, 1, FS3IPC_0_CH21_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan22Cfg, 0, 22, 1, FS3IPC_0_CH22_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan23Cfg, 0, 23, 1, FS3IPC_0_CH23_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan24Cfg, 0, 24, 1, FS3IPC_0_CH24_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan25Cfg, 0, 25, 1, FS3IPC_0_CH25_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan26Cfg, 0, 26, 1, FS3IPC_0_CH26_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan27Cfg, 0, 27, 1, FS3IPC_0_CH27_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan28Cfg, 0, 28, 1, FS3IPC_0_CH28_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan29Cfg, 0, 29, 1, FS3IPC_0_CH29_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan30Cfg, 0, 30, 1, FS3IPC_0_CH30_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);
LTCHANCFG_DEF(app_0_Chan31Cfg, 0, 31, 1, FS3IPC_0_CH31_HANDLE,
	TX_BUFFER_SIZE, RX_BUFFER_SIZE);

static orrlist_node_t app_0_ChanlistHead;
static orrlist_node_t app_0_nodes[FS3IPC_MAX_NUMBER_CHANNELS];
static fs3ipc_app_message_buffer_t app_0_rxMsg;
static fs3ipc_app_message_buffer_t app_0_txMsg;

/* flow control */
static fs3ipc_os_resource_t app_0_flowControl_Lock;

static fs3ipc_flowCtrlPBCfgType app_0_flowCtrlPBCfg;

static fs3ipc_fc_chan_pbcfg_t
	app_0_flowCtrlChanPBCfg[FS3IPC_MAX_NUMBER_CHANNELS];

#define LTFLOWCTRLCHANCFG(inst, chan, callback, xoffpercent) {\
	.rxCallback = callback,\
	.maxRxPerFrame = ((sizeof(app_##inst##_RxChannel##chan##Buf) * xoffpercent) / 100),\
}

static const fs3ipc_fc_chan_ltcfg_t
	app_0_flowCtrlChanLTCfg[FS3IPC_MAX_NUMBER_CHANNELS] = {
	{.rxCallback = NULL, .maxRxPerFrame = 0}, /*special channel, no buffer*/
	{.rxCallback = NULL, .maxRxPerFrame = 0}, /*special channel, no buffer*/
	LTFLOWCTRLCHANCFG(0, 2, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 3, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 4, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 5, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 6, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 7, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 8, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 9, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 10, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 11, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 12, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 13, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 14, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 15, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 16, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 17, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 18, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 19, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 20, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 21, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 22, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 23, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 24, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 25, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 26, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 27, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 28, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 29, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 30, NULL, 25),
	LTFLOWCTRLCHANCFG(0, 31, NULL, 25),
};

static const fs3ipc_fc_ltcfg_t app_0_flowCtrlLTCfg = {
	.channelCount = FS3IPC_MAX_NUMBER_CHANNELS,
	.pbStConfig = &app_0_flowCtrlPBCfg,
	.pbStChanConfig = app_0_flowCtrlChanPBCfg,
	.ltStChanConfig = app_0_flowCtrlChanLTCfg,
	.resLock = &app_0_flowControl_Lock};

static fs3ipc_length app_0_rxOverflowIndex;

const fs3ipc_app_ltcfg_t
	fs3ipc_app_appConfigs[FS3IPC_APP_NUM_OF_INSTANCES] = {
	/* instance 0 - FS3IPC_APP_I0_HANDLE*/
	{
		.ltChannelCfg = {
			NULL,
			NULL,
			&app_0_Chan2Cfg,
			&app_0_Chan3Cfg,
			&app_0_Chan4Cfg,
			&app_0_Chan5Cfg,
			&app_0_Chan6Cfg,
			&app_0_Chan7Cfg,
			&app_0_Chan8Cfg,
			&app_0_Chan9Cfg,
			&app_0_Chan10Cfg,
			&app_0_Chan11Cfg,
			&app_0_Chan12Cfg,
			&app_0_Chan13Cfg,
			&app_0_Chan14Cfg,
			&app_0_Chan15Cfg,
			&app_0_Chan16Cfg,
			&app_0_Chan17Cfg,
			&app_0_Chan18Cfg,
			&app_0_Chan19Cfg,
			&app_0_Chan20Cfg,
			&app_0_Chan21Cfg,
			&app_0_Chan22Cfg,
			&app_0_Chan23Cfg,
			&app_0_Chan24Cfg,
			&app_0_Chan25Cfg,
			&app_0_Chan26Cfg,
			&app_0_Chan27Cfg,
			&app_0_Chan28Cfg,
			&app_0_Chan29Cfg,
			&app_0_Chan30Cfg,
			&app_0_Chan31Cfg,
		},
		.channelCount = FS3IPC_MAX_NUMBER_CHANNELS,
		.orderedChannelListHead = &app_0_ChanlistHead,
		.orderedChannelNodes = app_0_nodes,
		.phys_txEventOsCfg = &app_0_hdlcEvent,
		.phys_txEvent = (EVENT_TX_MSG),
		.flowControlCfg = &app_0_flowCtrlLTCfg,
		.rxMsg = &app_0_rxMsg,
		.txMsg = &app_0_txMsg,
		.rxMsgBufferOverflowIndex = &app_0_rxOverflowIndex,
#ifdef CONFIG_FS3IPC_DEBUG
		.stats = &app_0_stats,
#endif

	}
};

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
void app_0_CustomInit(struct fs3ipc_uart *fs3ipc)
{
	int32_t chan = 0;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	int32_t chanCount = fs3ipc_app_appConfigs[0].channelCount;

	ret = fs3ipc_os_init_resource(&app_0_flowControl_Lock);
	if (ret != fs3ipc_StatusType_OK) {
		LOG_ERROR("'app_0_flowControl_Lock' initialization failed: %u",
			  ret);
	}
	ret = fs3ipc_os_init_resource(&app_0_hdlcLock);
	if (ret != fs3ipc_StatusType_OK)
		LOG_ERROR("'app_0_hdlcLock' initialization failed: %u", ret);

	spin_lock_init(&app_0_hdlcEvent.lock);
	init_waitqueue_head(&app_0_hdlcEvent.wq);

	fs3ipc->pm_wait_obj =
		fs3ipc_app_appConfigs[fs3ipc->fs3ipc_instance].phys_txEventOsCfg;

	for (chan = 0; chan < chanCount; chan++) {
		const fs3ipc_app_ltchancfg_t *chanCfg =
			fs3ipc_app_appConfigs[0].ltChannelCfg[chan];

		if (chanCfg) {
			fs3ipc_os_event_t *ev = chanCfg->client_cfg.osCfg;

			if (ev) {
				spin_lock_init(&ev->lock);
				init_waitqueue_head(&ev->wq);
			} else {
				log_error("null eventCfg channel %d ", chan);
			}


			/* initialize buffer spin lock*/
			if (chanCfg->TX_QLock) {
				ret = fs3ipc_os_init_resource(chanCfg->TX_QLock);
				if (ret != fs3ipc_StatusType_OK) {
					LOG_ERROR("CH-%d: 'TX_QLock' initialization failed: %u",
						 chan, ret);
				}
			} else {
				log_error("null TX_QLock channel %d ", chan);
			}

			if (chanCfg->RX_QLock) {
				ret = fs3ipc_os_init_resource(chanCfg->RX_QLock);
				if (ret != fs3ipc_StatusType_OK) {
					LOG_ERROR("CH-%d: 'RX_QLock' initialization failed: %u",
						 chan, ret);
				}
			} else {
				log_error("null RX_QLock channel %d ", chan);
			}
		}
	}
}
