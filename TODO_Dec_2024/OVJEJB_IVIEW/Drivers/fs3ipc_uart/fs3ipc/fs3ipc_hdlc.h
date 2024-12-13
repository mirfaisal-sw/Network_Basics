/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_hdlc.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * Data linker layer header.
 *****************************************************************************/

#ifndef FS3IPC_HDLC_H
#define FS3IPC_HDLC_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_timer.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define HDLC_DATA_MAX_LEN (FS3IPC_TRANS_SIZE_MAX)
#define HDLC_DATA_MIN_LEN (FS3IPC_TRANS_SIZE_MAX)

/* hdlc*/
#define FS3IPC_SPI_IPC_NO_DATA_REMOVED (0)

/*TODO modify to 4 when SOC sends 4 windows */
#ifndef WINDOW_SIZE
#define WINDOW_SIZE 5							/**< Window Size        >**/
#endif

#define DEFAULT_WINDOW_SIZE (WINDOW_SIZE) /**< Window Size        >**/
#define INVALID_SEQ (0x7)						/**< HDLC Header Length >**/

/***************************************
 * Defines related to length
 ***************************************/
#define HDLC_FRAME_HEADER_LEN (2) /**< HDLC Header Length >**/
#define HDLC_CONTROL_LEN (1)
#define HDLC_ADDRESS_LEN (1)
/* HDLC_CONTROL_LEN+ HDLC_ADDRESS_LEN */
#define HDLC_CTR_ADDR_LEN (2)
/* window size + buffer size */
#define HDLC_UFRAME_PAYLOAD_LEN (3)
/* CRC Length */
#define CRC_SIZE (2)
/* HDLC_FRAME_HEADER_LEN+HDLC_CONTROL_LEN+ HDLC_ADDRESS_LEN+CRC_SIZE */
#define HDLC_TOTAL_FRAME (6)
/* HDLC_FRAME_HEADER_LEN + HDLC_CONTROL_LEN +HDLC_ADDRESS_LEN */
#define HDLC_TOTAL_HEADER (4)


/* DRogala - length and index definitions */
#define HDLC_MSG_SIZE_LEN (2)
#define HDLC_HDR_ADDR_LEN (1)
#define HDLC_HDR_CTRL_LEN (1)
#define HDLC_CRC_LEN      (2)

#define HDLC_MSG_SIZE_IDX (0)
#define HDLC_HDR_ADDR_IDX (HDLC_MSG_SIZE_IDX + HDLC_MSG_SIZE_LEN)
#define HDLC_HDR_CTRL_IDX (HDLC_HDR_ADDR_IDX + HDLC_HDR_ADDR_LEN)
#define HDLC_DATA_IDX     (HDLC_HDR_CTRL_IDX + HDLC_HDR_CTRL_LEN)
/***************************************
 * defines for Position
 * HDLC FORMAT
 * POS0*** POS1***POS2***POS3***POS4***POS5***POS6*** POS7***POS8***
 * Len *** len ***Addr***CTRL***Chan***RES ***Len *** Len ***Data***
 ***************************************/
#define HDLC_ADDR_INDEX (2)
#define HDLC_CTRL_INDEX (3)
#define HDLC_CHANNEL_INDEX (4)

/************************************
 * defines for offset
 ************************************/
#define HDLC_FRAME_HEADER_OFFSET (2)
#define HDLC_FRAME_DATA_OFFSET (4)

/**********************************
 * Mask defines
 *********************************/
#define HDLC_FRAME_TYPE_MASK (3)

/***************************************
 * defines used for calculate position
 */
#define FRAME_DATA_INDEX (HDLC_FRAME_HEADER_LEN + HDLC_CONTROL_LEN + \
								  HDLC_ADDRESS_LEN)
#define FRAME_MIN_LENGTH_SUPPORTED (HDLC_CTR_ADDR_LEN + CRC_SIZE)

/*
 * Indicates that a timeout has happened waiting for acknowledgment, need
 * to send a POLL frame
 */
#define EVENT_TX_TIMEOUT (1u << BIT_TX_TIMEOUT)
/*
 * Indicates that a valid frame has been received and it has to be
 * acknowledged, need to send a RR ack frame
 */
#define EVENT_RX_IFRAME (1u << BIT_RX_IFRAME)
/*
 * Indicates that we have received an out-of-seq frame, need to send a
 * REJ frame
 */
#define EVENT_RX_OUT_OF_SEQ_FRAME (1u << BIT_RX_OUT_OF_SEQ_FRAME)
/*
 * Indicates that we have received a REJ frame and we need to retransmit
 * the failed frames
 */
#define EVENT_RX_REJ_FRAME (1u << BIT_RX_REJ_FRAME)
/*
 * Indicates that we have received a POLL frame, need to send our status
 * by sending a final frame
 */
#define EVENT_RX_RR_POLL_FRAME (1u << BIT_RX_RR_POLL_FRAME)
/*
 * Indicates that we have received a Final frame and we need to retransmit
 * the failed frames
 */
#define EVENT_RX_RR_FINAL_FRAME (1u << BIT_RX_RR_FINAL_FRAME)
/*
 * Indicates that we have received U FRAME ACK message, we need to respond
 * ACK
 */
#define EVENT_RX_ACK_UFRAME (1u << BIT_RX_ACK_UFRAME)
/*
 * Indicates that we have received U FRAME RST message, we need to respond
 * ACK
 */
#define EVENT_RX_RST_UFRAME (1u << BIT_RX_RST_UFRAME)
#define EVENT_TX_MSG (1u << BIT_TX_MSG)
#define EVENT_TX_WAKEUP (1u << BIT_TX_WAKEUP)
#define EVENT_EXT_WAKEUP (1u << BIT_EXT_WAKEUP)
#define EVENT_SUSPEND (1u << BIT_SUSPEND)
#define EVENT_RESUME (1u << BIT_RESUME)

#define EVENT_ALL (EVENT_TX_MSG|EVENT_RX_RST_UFRAME|EVENT_RX_ACK_UFRAME|\
		EVENT_TX_TIMEOUT|EVENT_RX_OUT_OF_SEQ_FRAME|EVENT_RX_RR_POLL_FRAME|\
		EVENT_RX_REJ_FRAME|EVENT_RX_RR_FINAL_FRAME|EVENT_RX_IFRAME|\
		EVENT_EXT_WAKEUP|EVENT_SUSPEND|EVENT_RESUME|EVENT_TX_WAKEUP)

/* Frame types */
#define I_FRAME (0x0)
#define S_FRAME (0x1)
#define U_FRAME (0x3)

/* Types of S-frame */
#define RR_FRAME (0x00)
#define REJ_FRAME (0x01)
#define RNR_FRAME (0x10)
#define SREJ_FRAME (0x11)

/* Types of U-frame */
#define URST_FRAME (19)
#define UACK_FRAME (12) /*UA - Unnumbered acknowledgement*/

#define POLL_COMMAND (0x1)
#define POLL_RESPONSE (0x0)

#define POLL_REQUEST (0x1)
#define FRAME_REQUEST (0x0)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
/**************************************
 * Events used for HDLC handling
 ************************************
 */
typedef enum EVENT_HDR_Tag {
/*
 * Indicates that a timeout has happened waiting for acknowledgment, need
 * to send a poll frame
 */
	BIT_TX_TIMEOUT,
/*
 * Indicates that a valid frame has been received and it has to be
 * acknowledged, need to send a RR ack frame
 */
	BIT_RX_IFRAME,
/*
 * Indicates that we have received an out-of-seq frame, need to send a
 * REJ frame
 */
	BIT_RX_OUT_OF_SEQ_FRAME,
/*
 * Indicates that we have received a REJ frame and we need to retransmit
 * the failed frames
 */
	BIT_RX_REJ_FRAME,
/*
 * Indicates that we have received a POLL frame, need to send our status
 * by sending a final frame
 */
	BIT_RX_RR_POLL_FRAME,
/*
 * Indicates that we have received a Final frame and we need to retransmit
 *  the failed frames
 */
	BIT_RX_RR_FINAL_FRAME,
/*
 * Indicates that we have received U FRAME ACK message, we need to respond
 * ACK
 */
	BIT_RX_ACK_UFRAME,
/*
 * Indicates that we have received U FRAME RST message, we need to respond
 * ACK
 */
	BIT_RX_RST_UFRAME,
	BIT_TX_MSG,
	BIT_TX_WAKEUP,
	BIT_EXT_WAKEUP,
	BIT_SUSPEND,
	BIT_RESUME,
} EVENT_HDR_Type;

typedef enum {
	DIAG_MSG_ID_IFRAME_TIMEOUT,
	DIAG_MSG_ID_UFRAME_TIMEOUT
} HDLCMsgID_e;

/** Exposed API's **/

typedef struct {
	/*OS*/
	fs3ipc_os_event_t *OS_Task;
	fs3ipc_os_EventMaskType event_task;
} fs3ipc_hdlc_OSCfgType;

typedef struct {
	fs3ipc_u32 RxIframeCnt;
	fs3ipc_u32 TxIframeCnt;
	fs3ipc_u32 RxSFrameCnt;
	fs3ipc_u32 TxSFrameCnt;

	fs3ipc_u32 RxInvalidFrameCnt;
	fs3ipc_u32 TxRetransmit;
	fs3ipc_u32 TxAckTimeout;
	fs3ipc_u32 RxCrcErrorCnt;
	fs3ipc_u32 RxSequenceErrorCnt;
	fs3ipc_u32 RxResetReqCnt;
	fs3ipc_u32 RxPollReqCnt;
} fs3ipc_hdlc_stats_t;

typedef struct {
	struct fs3ipc_timer * const uframe_timer;
	struct fs3ipc_timer * const iframe_timer;
	const uint32_t uframe_timeout;
	const uint32_t iframe_timeout;
	fs3ipc_u8 uWindowSiz;
	fs3ipc_u16 uBufferSiz;

	fs3ipc_hdlc_OSCfgType hdlcOSCfg;

#ifdef CONFIG_FS3IPC_DEBUG
	fs3ipc_hdlc_stats_t *stats;
#endif

	fs3ipc_os_resource_t *lock;
} fs3ipc_hdlc_AppConfigType;

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
extern fs3ipc_hdlc_AppConfigType fs3ipc_hdlc_appConfigs[FS3IPC_HDLC_NUM_OF_INSTANCES];

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
fs3ipc_StatusType fs3ipc_hdlc_GetRxBuffer_Cb(uint8_t fs3ipc_instance,
					     fs3ipc_dataPtr *dataPtr,
					     fs3ipc_length *buffer_length);

fs3ipc_StatusType fs3ipc_hdlc_GetTxBuffer_Cb(uint8_t fs3ipc_instance,
					     fs3ipc_dataPtr *dataPtr,
					     fs3ipc_length *length);

fs3ipc_StatusType fs3ipc_hdlc_Encode_Cb(uint8_t fs3ipc_instance,
					fs3ipc_dataPtr dataPtr,
					fs3ipc_length *length);

fs3ipc_StatusType fs3ipc_hdlc_Decode_Cb(uint8_t fs3ipc_instance,
					fs3ipc_cDataPtr dataPtr,
					fs3ipc_length length);

void process_uframe_timeout(const struct fs3ipc_timer *timer);

void process_iframe_timeout(const struct fs3ipc_timer *timer);

/**
 * fs3ipc_hdlc_init() - Initialize HDLC state machine.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * It verifies HDLC timer configurations, resets the proper state machine,
 * initializes the HDLC timers and sets event for sending the U Frame RESET
 * command.
 *
 * Return:
 * * fs3ipc_StatusType_OK          - OK
 * * fs3ipc_StatusType_ErrorHandle - Invalid FS3IPC instance
 * * fs3ipc_StatusType_ErrorCfg    - Config issue
 */
fs3ipc_StatusType fs3ipc_hdlc_init(uint8_t fs3ipc_instance);

#ifdef CONFIG_FS3IPC_DEBUG
const fs3ipc_hdlc_stats_t *fs3ipc_hdlc_GetStats(uint8_t fs3ipc_instance);

fs3ipc_StatusType fs3ipc_hdlc_ClearStats(uint8_t fs3ipc_instance);
#endif

void fs3ipc_hdlc_suspend(uint8_t fs3ipc_instance);
void fs3ipc_hdlc_resume(uint8_t fs3ipc_instance);
void fs3ipc_TxThread(uint8_t fs3ipc_instance);

/*******************************************************************
 * Unit test define space
 */
/*
 * only enable DATA_LAYER_TEST for calling unit test
 */
/*#define DATA_LAYER_TEST*/
/*extern void Spi_RunUnitTest(void);*/
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_HDLC_H */
