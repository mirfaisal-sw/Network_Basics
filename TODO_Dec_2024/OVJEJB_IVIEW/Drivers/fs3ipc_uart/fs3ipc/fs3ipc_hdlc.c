/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_hdlc.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 * @author       Jorge Daza <Jorge.Daza@harman.com>
 *
 * Derived from HDLC implementation with Go-Back-N arq. Modified version of HDLC
 * from SOC Side by Bhagat & Ansa tailorized to FCA R1 requirements
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_hdlc.h"
#include "fs3ipc_timer.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#ifdef LOG_MODULE
#undef LOG_MODULE
#endif
#define LOG_MODULE "fs3ipc_hdlc"

#define HDLC_INIT_NONE    (0)
/** flag indicates U-Frame Reset Ack has been sent */
#define HDLC_INIT_UACK_TX (1u<<0)
/** flag indicates U-Frame Reset Ack has been received */
#define HDLC_INIT_UACK_RX (1u<<1)
/** flag indicates other node has completed initialization and is now
 * actively transmitting S and I frames
 */
#define HDLC_INIT_EXT_COMM_FLAG (1u<<2)
/** combination of flags indicate it is possible to send and receive U
 * and S frames.
 */
#define HDLC_INIT_COMM_EN_FLAGS (HDLC_INIT_UACK_TX | HDLC_INIT_UACK_RX)
/** HDLC Initialization has been completed for both nodes. If all flags are set
 * and a U-frame reset request is received, that node has undergone a reset.
 */
#define HDLC_INIT_ALL_FLAGS (HDLC_INIT_COMM_EN_FLAGS | HDLC_INIT_EXT_COMM_FLAG)

/* Total HDLC instances supported by this layer.*/
#define FS3IPC_HDLC_MAX_INSTANCES (1)

/** increment statistics macro*/
#ifdef CONFIG_FS3IPC_DEBUG
#define INC_STATS(var) ((var)++)
#else
#define INC_STATS(var)
#endif

/* Defines the max CRC table size.*/
#define CRC_TABLE_SIZE (0x100)

#if FS3IPC_DATA_LAYER_ENABLE_TEST_SECTION == 1
/**Internal bit masking  control*/

/** Gets if the pos bit is enabled*/
#define GET_EVENT(var, pos) (var & (1 << pos))
/** Enables the bit/state required */
#define SET_EVENT(var, pos) (var |= (1 << pos))
/** Consumes the bit/state*/
#define CLEAR_EVENT(var, pos) (var &= ~(1 << pos))
#endif

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
/** Buffer Type Enumeration. Defines size of buffer. */
typedef enum FS3IPC_HDLC_BUFFER_tag {
	FS3IPC_HDLC_NORMAL_MODE, /* 512 Bytes */
	FS3IPC_HDLC_BOOST_MODE,  /* 1024 Bytes */
	SPI_INVALID_MODE
} FS3IPC_HDLC_BUFFER_T;

/* Structure Declarations */
/*
 * Structure responsible to save the transmitted data. The total length
 * transmitted, those values are used for retransmission process.
 */
typedef struct TagHdlcTxQueue {
	fs3ipc_u16 wLength;						/**< Total length sent*/
	fs3ipc_u8 uData[HDLC_DATA_MAX_LEN]; /**< buffer used to save the data*/
} THdlcTxQueue;


#pragma pack(1)
/* Defines the I frames Control byte format.*/
typedef struct TagIframe {
	fs3ipc_u8 addr_sap : 6; /**< hdlc station address, not used for point-point*/
	fs3ipc_u8 addr_cr : 1;  /**< command/response bit used to identify a poll/final frame */
	fs3ipc_u8 addr_ext : 1; /**< not used */

	fs3ipc_u8 hdlc_0 : 1; /**< used to identify an I-frame */
	fs3ipc_u8 ns : 3; /**< transmit seq no */
	fs3ipc_u8 pf : 1; /**< pf bit used in poll/final frame */
	fs3ipc_u8 nr : 3; /**< receive seq no */
} TIframe;

/* Defines the S frames Control byte format.*/
typedef struct TagSframe {
	fs3ipc_u8 addr_ext_na : 6; /**< hdlc station address, not used for point-point */
	fs3ipc_u8 addr_cr : 1; /**< command/response bit used to identify a poll/final frame */
	fs3ipc_u8 addr_sap_na : 1; /**< not used */

	fs3ipc_u8 hdlc_01 : 2; /**< used to identify a S-frame */
	fs3ipc_u8 type_ss : 2; /**< used to identify the type of s-frame */
	fs3ipc_u8 pf : 1; /**< pf bit used in poll/final frame */
	fs3ipc_u8 nr : 3; /**< receive seq no */
} TSframe;

/* Defines the U frames Control byte format.*/
typedef struct TagUframe {
	fs3ipc_u8 addr;	/**< hdlc station address, not used for point-point */
	fs3ipc_u8 hdlc_11 : 2;  /**< used to identify a U-frame */
	fs3ipc_u8 type_12 : 2;  /**< used to identify the type of U-frame */
	fs3ipc_u8 pf : 1; /**< pf bit used in poll/final frame */
	fs3ipc_u8 type_345 : 3; /**< used to identify the type of U-frame */
	fs3ipc_u8 window_size;  /**< used to configure the window size */
	fs3ipc_u8 frame_size_h; /**< used to configure the hdlc buffer size, most significant first */
	fs3ipc_u8 frame_size_l; /**< used to configure the hdlc buffer size lease significant after */
} TUframe;
#pragma pack()

/*
 * Defines the retransmission variables to keep the transmitted/received
 * frames.
 */
typedef struct TagGoBackN {
	fs3ipc_u8 tx_seq; /**< sequence number of next frame to be sent */
	fs3ipc_u8 ack_exp; /**< sequence number of the first unacknowledged frame (ack expected) */
	fs3ipc_u8 rx_seq; /**< Sequence number of next frame expected to be received (frame expected)*/
	fs3ipc_u8 prevSeq; /**< sequence number of the last I frame processed*/
	fs3ipc_u8 rejAlreadySent; /**< 0 => Can send Nak. 1 => Can't send Nak */
} TGoBackN;

/*
 * Defines the HDLC manager structure, it handles the main variables in order
 * to send and receive data.
 */
typedef struct TagHdlcManager {
	THdlcTxQueue zTxQueue[DEFAULT_WINDOW_SIZE]; /**< Re-Transmit Queue >**/
	fs3ipc_u8 FS3ipc_Rx_Buffer[HDLC_DATA_MAX_LEN]; /**< Reception buffer to store Rx*/

	TGoBackN zGoBackN; /**< Retransmit statics  >**/

	volatile fs3ipc_u32 qEventFlag; /**< Event Flag used for synchronization >**/

	uint8_t fs3ipc_instance; /**< FS3IPC instance >**/
	fs3ipc_u8 uInitialized; /**< this flag registers the complete initialization*/

	FS3IPC_HDLC_BUFFER_T spiIpcMode; /**< buffer size >*/
	fs3ipc_u8 uLastTxSeqSent; /**< last Tx Seq used by retransmit sequence >*/
	fs3ipc_u8 uSuspended;
} THdlcManager;

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
static THdlcManager pHdlcManager[FS3IPC_HDLC_MAX_INSTANCES];

/* The table provides the 16 - bit frame checking sequence.*/
static const fs3ipc_u16 wCrcTable[CRC_TABLE_SIZE] = {
		  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
		  0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
		  0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
		  0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
		  0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
		  0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
		  0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
		  0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
		  0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
		  0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
		  0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
		  0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
		  0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
		  0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
		  0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
		  0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
		  0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
		  0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
		  0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
		  0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
		  0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
		  0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		  0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
		  0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
		  0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
		  0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
		  0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
		  0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
		  0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
		  0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
		  0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
		  0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0};

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
/**********************************************************************
 * helper functions for Encoding the Frames
 **********************************************************************/
static fs3ipc_u16 CalculateCRC(const fs3ipc_u8 *pBuf, fs3ipc_s32 iBufLen);
static void hdlcEncodeCRCFrame(fs3ipc_dataPtr pData, fs3ipc_s32 iLen);
static void hdlcEncodeLengthFrame(fs3ipc_dataPtr pData, const fs3ipc_s32 iLen);
static void hdlcEncodeTxHeaderSFrame(uint8_t fs3ipc_instance,
				     fs3ipc_dataPtr pBuffer,
				     fs3ipc_length *length);
static void hdlc_calculate_buffer_size(fs3ipc_length *buffer_size,
				       const fs3ipc_u16 remove_data,
				       uint8_t fs3ipc_instance);
static fs3ipc_StatusType fs3ipc_hdlc_processRxPacket(fs3ipc_hdlc_AppConfigType *cfg,
						     fs3ipc_cDataPtr pData,
						     fs3ipc_u16 wFrameLen,
						     uint8_t fs3ipc_instance);
static void fs3ipc_IncSeqNo(uint8_t fs3ipc_instance, fs3ipc_u8 *window_index);
static void fs3ipcSetValidIFrameIndex(uint8_t fs3ipc_instance,
				      fs3ipc_cDataPtr pRxFrame);
static void fs3ipcSetITimerMode(uint8_t fs3ipc_instance, fs3ipc_cDataPtr pRxFrame);

/***********************************************************************
 * helper functions for processing the events.
 **********************************************************************/
static void fs3ipc_hdlc_reset(uint8_t fs3ipc_instance);
static fs3ipc_StatusType fs3ipc_hdlcTransmit(uint8_t fs3ipc_instance,
					     fs3ipc_dataPtr dataPtr,
					     fs3ipc_length *length);
static fs3ipc_StatusType fs3ipc_checkValidFrame(fs3ipc_cDataPtr pData,
						fs3ipc_dataPtr16 uPktLen,
						uint8_t fs3ipc_instance);
static fs3ipc_u8 WindowOpen(uint8_t fs3ipc_instance);
static fs3ipc_u8 fs3ipcCatchInvalidIFrame(uint8_t fs3ipc_instance,
					  fs3ipc_cDataPtr pRxFrame);
static void fs3ipc_processSframe(uint8_t fs3ipc_instance, fs3ipc_cDataPtr pRxFrame);
static void fs3ipc_processUframe(uint8_t fs3ipc_instance, fs3ipc_cDataPtr pRxFrame);
static void fs3ipc_processIframe(uint8_t fs3ipc_instance, fs3ipc_cDataPtr pRxFrame,
				 fs3ipc_u16 wFrameLen);

/***********************************************************************
 * helper functions for encoding frame format
 **********************************************************************/
static void hdlcCreateIframe(fs3ipc_u8 snd_seq, fs3ipc_u8 rcv_seq,
	fs3ipc_dataPtr pBuffer);
static void hdlcCreateRRAckFrame(fs3ipc_u8 uRxSeq, fs3ipc_dataPtr pBuffer);
static void hdlcCreateRRPollFrame(fs3ipc_u8 rcv_seq, fs3ipc_dataPtr pBuffer);
static void hdlcCreateRRFinalFrame(fs3ipc_u8 rcv_seq, fs3ipc_dataPtr pBuffer);
static void hdlcCreateREJFrame(fs3ipc_u8 rcv_seq, fs3ipc_dataPtr pBuffer);
static void hdlcCreateUAckFrame(fs3ipc_dataPtr pBuffer, fs3ipc_u8 window_size,
	fs3ipc_u16 frame_size);
static void hdlcCreateURstFrame(fs3ipc_dataPtr pBuffer, fs3ipc_u8 window_size,
	fs3ipc_u16 frame_size);

/********************************************************************
 * helper functions for timer management
 *********************************************************************/
static void hdlc_restart_timer(struct fs3ipc_timer *timer, uint32_t timeout);
static void hdlc_stop_timer(struct fs3ipc_timer *timer);

/*******************************************************************
 * handle functions for event handling
 ******************************************************************/
static void HDLC_Handle_EVENT_TX_MSG(uint8_t fs3ipc_instance,
				     fs3ipc_dataPtr dataPtr,
				     fs3ipc_length *length);
static void HDLC_Handle_EVENT_TX_TIMEOUT(uint8_t fs3ipc_instance,
					 fs3ipc_dataPtr dataPtr,
					 fs3ipc_length *length);
static void HDLC_Handle_EVENT_RX_OUT_OF_SEQ_FRAME(uint8_t fs3ipc_instance,
						  fs3ipc_dataPtr dataPtr,
						  fs3ipc_length *length);
static void HDLC_Handle_EVENT_RX_RR_POLL_FRAME(uint8_t fs3ipc_instance,
					       fs3ipc_dataPtr dataPtr,
					       fs3ipc_length *length);
static void HDLC_Handle_EVENT_TX_Retransmission(uint8_t fs3ipc_instance,
						fs3ipc_dataPtr dataPtr,
						fs3ipc_length *length);
static void HDLC_Handle_EVENT_RX_IFRAME(uint8_t fs3ipc_instance,
					fs3ipc_dataPtr dataPtr,
					fs3ipc_length *length);
static void HDLC_Handle_EVENT_RX_ACK_UFRAME(uint8_t fs3ipc_instance,
					    fs3ipc_dataPtr dataPtr,
					    fs3ipc_length *length);
static void HDLC_Handle_EVENT_RX_RST_UFRAME(uint8_t fs3ipc_instance,
					    fs3ipc_dataPtr dataPtr,
					    fs3ipc_length *length);
static void HDLC_Handle_EVENT_EXT_WAKEUP(uint8_t fs3ipc_instance,
					 fs3ipc_dataPtr dataPtr,
					 fs3ipc_length *length);
static void HDLC_Handle_EVENT_SUSPEND(uint8_t fs3ipc_instance,
				      fs3ipc_dataPtr dataPtr,
				      fs3ipc_length *length);
static void HDLC_Handle_EVENT_RESUME(uint8_t fs3ipc_instance,
				     fs3ipc_dataPtr dataPtr,
				     fs3ipc_length *length);

/********************************************************
 *helper functions for debugging
 ********************************************************/
static void FS3IPC_PRINT_BUFFER(const fs3ipc_u8 *printbuffer,
	const fs3ipc_u16 length);

#if FS3IPC_DATA_LAYER_ENABLE_TEST_SECTION == 1
static fs3ipc_StatusType fs3ipc_test_app_Encoder(uint8_t fs3ipc_instance,
						 fs3ipc_dataPtr data,
						 fs3ipc_length *ptrLength);
#endif

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
/**
 * fs3ipc_hdlc_suspend() - Suspends transmission.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * Return: None.
 */
void fs3ipc_hdlc_suspend(uint8_t fs3ipc_instance)
{
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	LOG_ALERT("%u: Triggered", (unsigned int)fs3ipc_instance);
	fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_SUSPEND);
}

/**
 * fs3ipc_hdlc_resume() - This function resumes transmission.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * Return: None.
 */
void fs3ipc_hdlc_resume(uint8_t fs3ipc_instance)
{
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	LOG_ALERT("%u: Triggered", (unsigned int)fs3ipc_instance);
	fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RESUME);
}


/**
 * fs3ipc_hdlc_GetRxBuffer_Cb() - This function returns the RX buffer address.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The input buffer.
 * @buffer_length: Current buffer size selected.
 *
 * Return: fs3ipc_StatusType Result operation.
 */
fs3ipc_StatusType fs3ipc_hdlc_GetRxBuffer_Cb(uint8_t fs3ipc_instance,
					     fs3ipc_dataPtr *dataPtr,
					     fs3ipc_length *buffer_length)
{
	fs3ipc_StatusType fs3ipc_stat = fs3ipc_StatusType_OK;

	LOG_DEBUG("");

	if (fs3ipc_instance <= FS3IPC_NUM_OF_INSTANCES) {
		fs3ipc_length current_buffer_length;
		THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];

		/*provide Tx buffer address to the caller*/
		*dataPtr = &pHdlc->FS3ipc_Rx_Buffer[0];

		/*calculate the current buffer size defined  */
		hdlc_calculate_buffer_size(&current_buffer_length,
					   FS3IPC_SPI_IPC_NO_DATA_REMOVED,
					   fs3ipc_instance);
		*buffer_length = current_buffer_length;
	} else {
		/* This shouldn't happen. Config issue*/
		fs3ipc_stat = fs3ipc_StatusType_ErrorHandle;
	}

	return fs3ipc_stat;
}

/**
 * fs3ipc_hdlc_GetTxBuffer_Cb() - This function returns the TX buffer address.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The input buffer.
 * @length: Current buffer size selected.
 *
 * Return: fs3ipc_StatusType Result operation.
 */
fs3ipc_StatusType fs3ipc_hdlc_GetTxBuffer_Cb(uint8_t fs3ipc_instance,
					     fs3ipc_dataPtr *dataPtr,
					     fs3ipc_length *length)
{

	fs3ipc_StatusType fs3ipc_stat = fs3ipc_StatusType_OK;

	if (fs3ipc_instance <= FS3IPC_NUM_OF_INSTANCES) {
		THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
		fs3ipc_u8 window_idx = pHdlc->zGoBackN.tx_seq;

		LOG_DEBUG("Tx Window idx =[%d]", window_idx);

		*dataPtr = &pHdlc->zTxQueue[window_idx].uData[0];
		hdlc_calculate_buffer_size(length, FS3IPC_SPI_IPC_NO_DATA_REMOVED,
					   fs3ipc_instance);
	} else {
		/* This shouldn't happen. Config issue*/
		fs3ipc_stat = fs3ipc_StatusType_ErrorHandle;
	}
	return fs3ipc_stat;
}

/**
 * fs3ipc_hdlc_Encode_Cb() - Encodes the transmission buffer.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The input buffer.
 * @length: Returns the number of bytes to transmit.
 *
 * Return: fs3ipc_StatusType Result operation.
 */
fs3ipc_StatusType fs3ipc_hdlc_Encode_Cb(uint8_t fs3ipc_instance,
					fs3ipc_dataPtr dataPtr,
					fs3ipc_length *length)
{
	fs3ipc_StatusType fs3ipc_stat = fs3ipc_StatusType_OK;

	if (fs3ipc_instance <= FS3IPC_NUM_OF_INSTANCES) {
		LOG_DEBUG("");

		/*before transmitting, the length value is initialized to zero */
		fs3ipc_stat = fs3ipc_hdlcTransmit(fs3ipc_instance, dataPtr,
						  length);
	} else {
		/* This shouldn't happen. Config issue*/
		fs3ipc_stat = fs3ipc_StatusType_ErrorHandle;
	}

	return fs3ipc_stat;
}

void fs3ipc_TxThread(uint8_t fs3ipc_instance)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;

	if (fs3ipc_instance >= FS3IPC_NUM_OF_INSTANCES)
		return;


	for (;;) {
		fs3ipc_u8 idx = pHdlc->zGoBackN.tx_seq;
		fs3ipc_dataPtr dataPtr =
			&pHdlcManager[fs3ipc_instance].zTxQueue[idx].uData[0];
		fs3ipc_length length = 0;

		hdlc_calculate_buffer_size(&length, FS3IPC_SPI_IPC_NO_DATA_REMOVED,
					   fs3ipc_instance);

		ret = fs3ipc_hdlcTransmit(fs3ipc_instance, dataPtr, &length);
		if (ret != fs3ipc_StatusType_OK)
			LOG_ERROR("%u: fs3ipc_hdlcTransmit() failed: %u",
				  (unsigned int)fs3ipc_instance, ret);
	}
}

/**
 * fs3ipc_hdlc_Decode_Cb() - Decodes the received buffer.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The input buffer.
 * @length: The number of bytes received.
 *
 * This function decodes and validates the received buffer.
 *
 * Return: fs3ipc_StatusType Result operation.
 */
fs3ipc_StatusType fs3ipc_hdlc_Decode_Cb(uint8_t fs3ipc_instance,
					fs3ipc_cDataPtr dataPtr,
					fs3ipc_length length)
{
	fs3ipc_StatusType fs3ipc_stat = fs3ipc_StatusType_OK;
	fs3ipc_u16 wFrameLen = (fs3ipc_u16)length;
	THdlcManager *pHdlc;
	fs3ipc_hdlc_AppConfigType *cfg;

	if (fs3ipc_instance >= FS3IPC_NUM_OF_INSTANCES) {
		/* This shouldn't happen. Config issue*/
		LOG_ERROR("Invalid FS3IPC instance received [%u]- Discarded",
			  (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorHandle;
	}

	cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];
	pHdlc = &pHdlcManager[fs3ipc_instance];

	/* don't process Rx message if suspended*/
	if (pHdlc->uSuspended == FS3IPC_TRUE) {
		LOG_ALERT("%u: Msg received while being suspended", (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorClientState;
	}

	/*
	 * 1. Check whether the information retrieved during the transaction is
	 *    having valid length & passes CRC. If yes, update the current
	 *    buffer length.
	 */
	fs3ipc_stat = fs3ipc_checkValidFrame(dataPtr, &wFrameLen, fs3ipc_instance);
	if (fs3ipc_stat != fs3ipc_StatusType_OK) {
		LOG_DEBUG("Invalid frame received - Discarded");
		return fs3ipc_stat;
	}

	if (wFrameLen) {
		LOG_DEBUG("Valid frame received, decoded frame size = %u", wFrameLen);

		FS3IPC_PRINT_BUFFER(dataPtr, wFrameLen + 2);
		/*
		 * 2. Process the rx information, move the pointer address to
		 *    the data address.
		 */
		fs3ipc_hdlc_processRxPacket(cfg, dataPtr + HDLC_ADDR_INDEX,
					    wFrameLen, fs3ipc_instance);
	} else {
		/* 0 length frame*/
		LOG_DEBUG("0 length frame received");
	}

	return fs3ipc_StatusType_OK;
}

/**
 * hdlc_restart_timer() - Restart timer.
 * @timer: Timer to be restarted.
 * @timeout: Timeout period in msec.
 *
 * It is assumed that the arguments are valid.
 *
 * Return: None.
 */
static void hdlc_restart_timer(struct fs3ipc_timer *timer, uint32_t timeout)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	uint8_t fs3ipc_instance = fs3ipc_get_timer_instance(timer);
	const char *name = fs3ipc_get_timer_name(timer);

	LOG_DEBUG("%u: '%s' - timeout period: %u msec",
		  (unsigned int)fs3ipc_instance, name, timeout);

	ret = fs3ipc_restart_timer(timer, timeout);
	if (ret != fs3ipc_StatusType_OK)
		LOG_ERROR("%u: Failed for '%s' (timeout period: %u msec): %u",
			  (unsigned int)fs3ipc_instance, name, timeout, ret);
}

/**
 * hdlc_stop_timer() - Stop timer.
 * @timer: Timer to be stopped.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
static void hdlc_stop_timer(struct fs3ipc_timer *timer)
{
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	uint8_t fs3ipc_instance = fs3ipc_get_timer_instance(timer);
	const char *name = fs3ipc_get_timer_name(timer);

	LOG_DEBUG("%u: '%s'", (unsigned int)fs3ipc_instance, name);

	ret = fs3ipc_stop_timer(timer);
	if (ret != fs3ipc_StatusType_OK)
		LOG_ERROR("%u: Failed for '%s': %u", (unsigned int)fs3ipc_instance,
			  name, ret);
}

/**
 * restart_uframe_timer() - Restart the U frame timer.
 * @cfg: HDLC configuration.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
static inline void restart_uframe_timer(const fs3ipc_hdlc_AppConfigType *cfg)
{
	hdlc_restart_timer(cfg->uframe_timer, cfg->uframe_timeout);
}

/**
 * stop_uframe_timer() - Stop the U frame timer.
 * @cfg: HDLC configuration.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
static inline void stop_uframe_timer(const fs3ipc_hdlc_AppConfigType *cfg)
{
	hdlc_stop_timer(cfg->uframe_timer);
}

/**
 * process_uframe_timeout() - Handle the U frame timeout.
 * @timer: Expired U frame FS3IPC timer.
 *
 * It sets event for the processing of the U frame timeout.
 *
 * Return: None.
 */
void process_uframe_timeout(const struct fs3ipc_timer *timer)
{
	const char *name;
	uint8_t fs3ipc_instance = 0u;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const fs3ipc_hdlc_AppConfigType *cfg = NULL;

	if (!timer) {
		LOG_ERROR("NULL argument");
		return;
	}

	name = fs3ipc_get_timer_name(timer);
	fs3ipc_instance = fs3ipc_get_timer_instance(timer);
	cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	LOG_ERROR("%u: '%s' timeout", (unsigned int)fs3ipc_instance, name);

	ret = fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_RST_UFRAME);
	if (ret != fs3ipc_StatusType_OK)
		LOG_ERROR("%u: Failed to set event for '%s': %u",
			  (unsigned int)fs3ipc_instance, name, ret);
}

/**
 * restart_iframe_timer() - Restart the I frame timer.
 * @cfg: HDLC configuration.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
static inline void restart_iframe_timer(const fs3ipc_hdlc_AppConfigType *cfg)
{
	hdlc_restart_timer(cfg->iframe_timer, cfg->iframe_timeout);
}

/**
 * stop_iframe_timer() - Stop the I frame timer.
 * @cfg: HDLC configuration.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
static inline void stop_iframe_timer(const fs3ipc_hdlc_AppConfigType *cfg)
{
	hdlc_stop_timer(cfg->iframe_timer);
}

/**
 * process_iframe_timeout() - Handle the I frame timeout.
 * @timer: Expired I frame FS3IPC timer.
 *
 * If all the transmitted messages have not been acked, it sets event for
 * processing of the I Frame Timeout.
 *
 * Return: None.
 */
void process_iframe_timeout(const struct fs3ipc_timer *timer)
{
	const char *name;
	uint8_t fs3ipc_instance = 0u;
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	const THdlcManager *hdlc = NULL;
	const fs3ipc_hdlc_AppConfigType *cfg = NULL;

	if (!timer) {
		LOG_ERROR("NULL argument");
		return;
	}

	name = fs3ipc_get_timer_name(timer);
	fs3ipc_instance = fs3ipc_get_timer_instance(timer);
	hdlc = &pHdlcManager[fs3ipc_instance];
	cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	if (hdlc->zGoBackN.ack_exp != hdlc->zGoBackN.tx_seq) {
		LOG_WARNING("%u: '%s' timeout", (unsigned int)fs3ipc_instance,
			    name);
		ret = fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_TIMEOUT);
		if (ret != fs3ipc_StatusType_OK)
			LOG_ERROR("%u: Failed to set event for '%s': %u",
				  (unsigned int)fs3ipc_instance, name, ret);
	} else {
		LOG_WARNING("%u: '%s' timeout is ignored as all frames are acked",
			    (unsigned int)fs3ipc_instance, name);
	}
}

/**
 * CalculateCRC() - Calculate CRC for a particular data.
 * @pBuf: Pointer to input data.
 * @iBufLen: Size of the data.
 *
 * The table CRC takes 54.92 micro seconds.
 *
 * Return: The 16-bit calculated CRC.
 */
static fs3ipc_u16 CalculateCRC(const fs3ipc_u8 *pBuf, fs3ipc_s32 iBufLen)
{
	fs3ipc_u16 wCrc = 0xffff;
	fs3ipc_u16 wTmp, wShortC;

	for (; iBufLen > 0; iBufLen--) {
		wShortC = 0x00FF & (fs3ipc_u16)*pBuf;
		wTmp = (wCrc >> 8) ^ wShortC;
		wCrc = (wCrc << 8) ^ wCrcTable[wTmp];
		pBuf++;
	}

	return wCrc;
}

/**
 * validNr() - Check if Nr (rcv seq no) is valid.
 * @uAckExp: Seq no of the ack expected  no.
 * @uNr: The received seq no.
 * @uTxSeq: Seq no of the next frame to be transmitted.
 *
 * This function ensures the validity of the received sequence Number.
 *
 * Return:
 * * FS3IPC_TRUE  - Nr is valid.
 * * FS3IPC_FALSE - Otherwise.
 */
static fs3ipc_u8 validNr(fs3ipc_u16 uAckExp, fs3ipc_u16 uNr, fs3ipc_u16 uTxSeq)
{
	fs3ipc_u8 uRet = FS3IPC_FALSE;

	if (((uAckExp <= uNr) && (uNr <= uTxSeq)) ||
		 ((uTxSeq < uAckExp) && (uAckExp <= uNr)) ||
		 ((uNr <= uTxSeq) && (uTxSeq < uAckExp))) {
		uRet = FS3IPC_TRUE;
	}

	return uRet;
}

/**
 * WindowOpen() - Check if window is open for data to be sent.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * Return: TRUE if Window Open, and ready to send more data.
 */
static fs3ipc_u8 WindowOpen(uint8_t fs3ipc_instance)
{
	fs3ipc_u16 uAckExp;
	fs3ipc_u16 uTxSeq;

	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_os_GetResource(cfg->lock);
	uAckExp = pHdlc->zGoBackN.ack_exp;
	uTxSeq = ((pHdlc->zGoBackN.tx_seq + 1) % cfg->uWindowSiz);
	fs3ipc_os_ReleaseResource(cfg->lock);

	return (uAckExp != uTxSeq);
}

/**
 * fs3ipc_processSframe() - Process the received Frame detected as S-frame.
 * @fs3ipc_instance: FS3IPC instance.
 * @pRxFrame: Frame for Decoding.
 *
 * According the S-frame request, action will be taken and corresponding Events
 * are processed, and updated to other side.
 *
 * Return: None.
 */
static void fs3ipc_processSframe(uint8_t fs3ipc_instance, fs3ipc_cDataPtr pRxFrame)
{
	const TSframe *pSframe = (TSframe *)pRxFrame;
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	/*
	 * HDLC U-frame handshake is completed and S-frame is received. So, set
	 * external communication flag and wake-up the transmit thread to
	 * evaluate if anything needs to be sent.
	 */
	fs3ipc_os_GetResource(cfg->lock);
	if (!(pHdlc->uInitialized & HDLC_INIT_EXT_COMM_FLAG)) {
		pHdlc->uInitialized |= HDLC_INIT_EXT_COMM_FLAG;
		fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_WAKEUP);
	}
	fs3ipc_os_ReleaseResource(cfg->lock);

	switch (pSframe->type_ss) {
	case REJ_FRAME:
	{
		INC_STATS(cfg->stats->RxSFrameCnt);

		LOG_WARNING("[SF] REJ_FRAME Ack Exp Window[%d %d] Nr-%d",
						pHdlc->zGoBackN.ack_exp,
						pHdlc->zGoBackN.tx_seq,
						pSframe->nr);

		if (validNr(pHdlc->zGoBackN.ack_exp, pSframe->nr, pHdlc->zGoBackN.tx_seq)) {
			if (pHdlc->zGoBackN.ack_exp != pSframe->nr) {
				pHdlc->zGoBackN.ack_exp = pSframe->nr;
				/* window may now be open. Set event*/
				fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_WAKEUP);
			}

			if (pSframe->nr == pHdlc->zGoBackN.tx_seq) {
				stop_iframe_timer(cfg);
				LOG_DEBUG("REJ_FRAME stop timer window[%d %d] Nr-%d",
					  pHdlc->zGoBackN.ack_exp,
					  pHdlc->zGoBackN.tx_seq, pSframe->nr);
			} else {
				restart_iframe_timer(cfg);

				fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task,
					EVENT_RX_REJ_FRAME);

				LOG_WARNING("[SF] REJ_FRAME restart timer");
			}
		} else {
			LOG_WARNING("[SF] REJ_FRAME:Invalid Nr, out-of-window..Exp Ack Window[%d %d] Nr=%d",
				pHdlc->zGoBackN.ack_exp,
				pHdlc->zGoBackN.tx_seq, pSframe->nr);
		}

		break;
	}

	case RR_FRAME:
	{
		INC_STATS(cfg->stats->RxSFrameCnt);

		/**< P/F bit is clear, so it is a normal ack frame from the receiver */
		LOG_DEBUG("[SF] RR FRAME with P/F = [%d] C/R =[%d]", pSframe->pf,
			pSframe->addr_cr);
		if (pSframe->pf == FS3IPC_FALSE) {
			if (validNr(pHdlc->zGoBackN.ack_exp, pSframe->nr, pHdlc->zGoBackN.tx_seq)) {
				if (pSframe->nr == pHdlc->zGoBackN.ack_exp) {
					LOG_WARNING("[SF] RR_FRAME: Invalid Nr, Already acked- Exp Ack Window[%d %d] Nr=%d",
						pHdlc->zGoBackN.ack_exp,
						pHdlc->zGoBackN.tx_seq, pSframe->nr);
				} else {
					if (pSframe->nr == pHdlc->zGoBackN.tx_seq) {
						stop_iframe_timer(cfg);
						LOG_DEBUG("RR_FRAME stop timer window[%d %d] Nr-%d",
							pHdlc->zGoBackN.ack_exp, pHdlc->zGoBackN.tx_seq,
							pSframe->nr);
					} else {
						restart_iframe_timer(cfg);
						LOG_DEBUG("RR_FRAME restart timer window[%d %d] Nr-%d",
							pHdlc->zGoBackN.ack_exp, pHdlc->zGoBackN.tx_seq,
							pSframe->nr);
					}
				}
				if (pHdlc->zGoBackN.ack_exp != pSframe->nr) {
					pHdlc->zGoBackN.ack_exp = pSframe->nr;
					/* window may now be open. Set event*/
					fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_WAKEUP);
				}
			} else {
				LOG_WARNING("[SF] RR_FRAME: Invalid Nr, Already acked- Exp Ack Window[%d %d] Nr=%d",
					pHdlc->zGoBackN.ack_exp,
					pHdlc->zGoBackN.tx_seq, pSframe->nr);
			}
		} else {
			 /*
			  * P/F bit is set, so it is either a poll frame or a final frame
			  * based on the command/response bit
			  */
			if (pSframe->addr_cr == POLL_RESPONSE) { /**< the command bit is 0, so it is a final frame*/

				LOG_WARNING("[SF] POLL_RESPONSE Ack Exp Window[%d %d] Nr-%d",
					pHdlc->zGoBackN.ack_exp, pHdlc->zGoBackN.tx_seq, pSframe->nr);
				if (validNr(pHdlc->zGoBackN.ack_exp, pSframe->nr,
					pHdlc->zGoBackN.tx_seq)) {
					pHdlc->zGoBackN.ack_exp = pSframe->nr;
					/*
					 * Valid Nr, restart the timer and
					 * update the acknowledgment window
					 */
					if (pSframe->nr == pHdlc->zGoBackN.tx_seq) {
						stop_iframe_timer(cfg);
						LOG_DEBUG("[SF] POLL_RESPONSE stop timer");
					} else {
						restart_iframe_timer(cfg);
						fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task,
							EVENT_RX_RR_FINAL_FRAME);

						LOG_DEBUG("POLL_RESPONSE restart timer start FF");
					}
				} else {
					LOG_WARNING("[SF] POLL_RESPONSE: Invalid Nr, Already acked- Exp Ack Window[%d %d] Nr=%d",
						pHdlc->zGoBackN.ack_exp,
						pHdlc->zGoBackN.tx_seq, pSframe->nr);
				}
			} else if (pSframe->addr_cr == POLL_COMMAND) { /**< the command bit is 1, so it is a poll frame */
				LOG_DEBUG("[SF] POLL_COMMAND");

				fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task,
					EVENT_RX_RR_POLL_FRAME);
			}
		}
		break;
	}

	default:
		/*
		 * Invalid frame received (RNR or SREJ frame not used in our
		 *   implementation)
		 */
	{
		INC_STATS(cfg->stats->RxInvalidFrameCnt);

		LOG_ERROR("[SF]Invalid frame type received FTyp:%d", pSframe->type_ss);
	}
	break;
	}
}

/**
 * fs3ipcCatchInvalidIFrame() - Used to process received frame after detected
 *                              as I-frame
 * @fs3ipc_instance: FS3IPC instance.
 * @pRxFrame: Frame.
 *
 * This function can detect OOS and send reject frame accordingly. If a valid
 * frame, corresponding Pr is updated, also it checks for piggy ack which again
 * updates the acknowledgment.
 *
 * Return: TRUE if valid frame otherwise FALSE.
 */
static fs3ipc_u8 fs3ipcCatchInvalidIFrame(uint8_t fs3ipc_instance,
					  fs3ipc_cDataPtr pRxFrame)
{
	const TIframe *pIframe = (TIframe *)pRxFrame;
	fs3ipc_u8 uRet = FS3IPC_TRUE;

	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	/*Check if the Sender Sequence number is the expected*/
	if (pIframe->ns != pHdlc->zGoBackN.rx_seq) {
		/*invalid index frame received start the error handling*/
		if (pHdlc->zGoBackN.prevSeq != pHdlc->zGoBackN.rx_seq) {
			if (pIframe->ns == pHdlc->zGoBackN.prevSeq) {
				/*duplicate packet received, do nothing*/
				LOG_WARNING("[IFRAME] Duplicate packet received %d", pIframe->ns);
				return FS3IPC_FALSE;
			}
		}

		fs3ipc_os_GetResource(cfg->lock);
		if (pHdlc->zGoBackN.rejAlreadySent == FS3IPC_FALSE) {
			pHdlc->zGoBackN.rejAlreadySent = FS3IPC_TRUE;
			fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_OUT_OF_SEQ_FRAME);
			LOG_WARNING("[IFRAME] Detected EVENT_RX_OUT_OF_SEQ_FRAME error handling %d",
				pIframe->ns);
		}
		fs3ipc_os_ReleaseResource(cfg->lock);

		fs3ipcSetITimerMode(fs3ipc_instance, pRxFrame);

		uRet = FS3IPC_FALSE;
	}
	/*the else condition is covered by fs3ipcSetValidIFrameIndex*/

	return uRet;
}

/**
 * fs3ipc_processUframe() - Handles the U frame reception.
 * @fs3ipc_instance: FS3IPC instance.
 * @pRxFrame: The data pointer to process the U frame.
 *
 * This function processes the U frame reception, it only supports Uframe reset
 * and U frame ACK. The U frame RST command is used for configuring the window
 * size and frame size.
 *
 * Warning: The URST frame resets all the transmission window parameters.
 *
 * Return: None.
 */
static void fs3ipc_processUframe(uint8_t fs3ipc_instance, fs3ipc_cDataPtr pRxFrame)
{
	const TUframe *pUframe = (TUframe *)pRxFrame;
	fs3ipc_u8 uFrameType;
	fs3ipc_u16 uFrameSize;

	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	uFrameType = (pUframe->type_12 | (pUframe->type_345 << 2));
	switch (uFrameType) {
	case URST_FRAME:
	{
		LOG_ALERT("%u: U frame RESET received", (unsigned int)fs3ipc_instance);

		uFrameSize = ((fs3ipc_u16)pUframe->frame_size_h << 8) | pUframe->frame_size_l;
		/*
		 * Check if the current window size and frame size are supported by this
		 * instance
		 */
		if ((pUframe->window_size == cfg->uWindowSiz) &&
			(uFrameSize <= cfg->uBufferSiz)) {

			/* reset the zGoBackN window parameters */
			fs3ipc_memset(&pHdlc->zGoBackN, 0, sizeof(pHdlc->zGoBackN));

			LOG_DEBUG("[UFRAME] ACK, change window size to %d",
				pUframe->window_size);
			if ((pUframe->window_size == WINDOW_SIZE) ||
				(pUframe->window_size == DEFAULT_WINDOW_SIZE)) {
				cfg->uWindowSiz = pUframe->window_size;
			} else {
				LOG_WARNING("[UFRAME] INVALID ACK, ignore change window size to %d",
					pUframe->window_size);
			}

			cfg->uBufferSiz = ((fs3ipc_u16)pUframe->frame_size_h << 8) |
				pUframe->frame_size_l;

			if ((pHdlc->uInitialized & HDLC_INIT_ALL_FLAGS) ==
				HDLC_INIT_ALL_FLAGS) {
				/* undo HDLC initialization and clear queues */
				fs3ipc_hdlc_ext_HandleExtNodeReset(fs3ipc_instance);
				stop_iframe_timer(cfg);
				stop_uframe_timer(cfg);
				fs3ipc_hdlc_reset(fs3ipc_instance);
				fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task,
						   EVENT_RX_RST_UFRAME);
				LOG_ERROR("Unexpected Node Reset Detected");
			}

			fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_ACK_UFRAME);
		} else {
			/*
			 * Error detected, the set EVENT_RX_ACK_UFRAME is not called,
			 * consequently the RQST is not executed
			 */
			LOG_ERROR("[UFRAME] invalid parameters Rx window size %d frame size %d"
						 , pUframe->window_size,
						 uFrameSize);
		}
		break;
	}

	/*UA - Unnumbered acknowledgment*/
	case UACK_FRAME:
	{

		LOG_ALERT("%u: U frame ACK received", (unsigned int)fs3ipc_instance);
		/*reset the zGoBackN window parameters and restart the Uframe timer */
		stop_uframe_timer(cfg);
		fs3ipc_memset(&pHdlc->zGoBackN, 0, sizeof(pHdlc->zGoBackN));

		fs3ipc_os_GetResource(cfg->lock);
		pHdlc->uInitialized |= HDLC_INIT_UACK_RX;
		fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_WAKEUP);
		fs3ipc_os_ReleaseResource(cfg->lock);

		break;
	}

	default:
	{
		INC_STATS(cfg->stats->RxInvalidFrameCnt);

		LOG_ERROR("[UFRAME]Invalid U-frame type Received: %d %d %d %d",
			uFrameType, pUframe->type_12, pUframe->type_345, pHdlc->fs3ipc_instance);
		break;
	}
	}
}

/**
 * fs3ipc_processIframe() - Handles the I frame reception.
 * @fs3ipc_instance: FS3IPC instance.
 * @pRxFrame: The data pointer to process the I frame.
 * @wFrameLen: Total length.
 *
 * This function processes the I frame reception. If the frame is valid,
 * it will be passed to next layer and ack will be sent otherwise, it will
 * be rejected.
 *
 * Return: None.
 */
static void fs3ipc_processIframe(uint8_t fs3ipc_instance, fs3ipc_cDataPtr pRxFrame,
				 fs3ipc_u16 wFrameLen)
{
	fs3ipc_u32 uDataLen = 0;
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_StatusType decode_response = fs3ipc_StatusType_OK;
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];


	/*
	 * HDLC U-frame handshake is completed and I-frame is received. So, set
	 * external communication flag and wake-up the transmit thread to
	 * evaluate if anything needs to be sent.
	 */
	fs3ipc_os_GetResource(cfg->lock);
	if (!(pHdlc->uInitialized & HDLC_INIT_EXT_COMM_FLAG)) {
		pHdlc->uInitialized |= HDLC_INIT_EXT_COMM_FLAG;
		fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_WAKEUP);
	}
	fs3ipc_os_ReleaseResource(cfg->lock);

	/*
	 * Check and confirm if the i frame is valid and prepare
	 * the ack or error handling in case if the current i
	 * frame is not valid
	 */
	if (fs3ipcCatchInvalidIFrame(fs3ipc_instance, pRxFrame)) {

		/*remove the crc size and header length*/
		uDataLen = wFrameLen - (CRC_SIZE + HDLC_FRAME_HEADER_LEN);

		/*call  the decode function to higher communication layer*/
		decode_response = fs3ipc_hdlc_ext_Decode(fs3ipc_instance,
							 pRxFrame + HDLC_FRAME_HEADER_OFFSET,
							 uDataLen);

		/*only process the message if the decoding returns OK*/
		if (fs3ipc_StatusType_OK == decode_response)
			/*valid frame received set the proper parameters*/
			fs3ipcSetValidIFrameIndex(fs3ipc_instance, pRxFrame);
		else
			/*encode error retrieved force timeout condition*/
			LOG_ERROR("Decode Error =%d", decode_response);
		INC_STATS(cfg->stats->RxIframeCnt);
	} else {
		INC_STATS(cfg->stats->RxInvalidFrameCnt);
		LOG_ERROR("Invalid IFRAME_RECEIVED!");
	}

}

/**
 * fs3ipc_hdlc_processRxPacket() - Processes the  message received
 * @cfg: fs3ipc_hdlc_AppConfigType configuration structure.
 * @pData: The input buffer.
 * @wFrameLen: Total length.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * This function processes the RX messages, it validates the CRC value, it
 * looks for the message type.
 *
 * Return: fs3ipc_StatusType Result Operation.
 */
static fs3ipc_StatusType fs3ipc_hdlc_processRxPacket(fs3ipc_hdlc_AppConfigType *cfg,
						     fs3ipc_cDataPtr pData,
						     fs3ipc_u16 wFrameLen,
						     uint8_t fs3ipc_instance)
{
	fs3ipc_u8 uFrameType;
	fs3ipc_u8 hdlcInit = 0;
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];

	fs3ipc_os_GetResource(cfg->lock);
	hdlcInit = ((pHdlc->uInitialized & HDLC_INIT_COMM_EN_FLAGS) ==
		HDLC_INIT_COMM_EN_FLAGS);
	fs3ipc_os_ReleaseResource(cfg->lock);

	/* Determine Frame type */
	uFrameType = pData[HDLC_CONTROL_LEN] & HDLC_FRAME_TYPE_MASK;
	switch (uFrameType) {
	case S_FRAME:
		LOG_DEBUG("SFRAME_RECEIVED");
		/* make sure system has been initialized */
		if (!hdlcInit) {
			INC_STATS(cfg->stats->RxInvalidFrameCnt);
			LOG_ERROR("SFRAME RX - NOINIT");
			return fs3ipc_StatusType_NOINIT;
		}
		fs3ipc_processSframe(fs3ipc_instance, pData);
		break;

	case U_FRAME:
		LOG_DEBUG("UFRAME_RECEIVED");
		fs3ipc_processUframe(fs3ipc_instance, pData);
		break;

	default:
		LOG_DEBUG("IFRAME_RECEIVED");
		/* make sure system has been initialized */
		if (!hdlcInit) {
			INC_STATS(cfg->stats->RxInvalidFrameCnt);
			LOG_ERROR("IFRAME RX - NOINIT");
			return fs3ipc_StatusType_NOINIT;
		}
		fs3ipc_processIframe(fs3ipc_instance, pData, wFrameLen);
	}

	return fs3ipc_StatusType_OK;
}

/**
 * fs3ipc_IncSeqNo() - Increments the window index parameter.
 * @fs3ipc_instance: FS3IPC instance.
 * @window_index: Window index to increment.
 *
 * This function increments the window index parameter based on window size
 * parameters.
 *
 * Return: None.
 */
static void fs3ipc_IncSeqNo(uint8_t fs3ipc_instance, fs3ipc_u8 *window_index)
{
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_u8 l_index = *window_index;

	l_index = (l_index + 1) % (cfg->uWindowSiz);
	*window_index = l_index;
}

/**
 * fs3ipc_checkValidFrame() - Checks if frame is valid or not.
 * @pData: Data to validate if the length is the correct.
 * @uPktLen: Returns the packet size value.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * Return: TRUE if valid frame otherwise FALSE.
 */
static fs3ipc_StatusType fs3ipc_checkValidFrame(fs3ipc_cDataPtr pData,
						fs3ipc_dataPtr16 uPktLen,
						uint8_t fs3ipc_instance)
{
	fs3ipc_u16 uLen = 0;
	fs3ipc_length buffer_size = 0;
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	if (*uPktLen < HDLC_TOTAL_FRAME) {
		LOG_WARNING("Invalid message size received: %u", *uPktLen);
		return fs3ipc_StatusType_MessageSize;
	}

	uLen = *((fs3ipc_u16 *)&pData[0]);
	hdlc_calculate_buffer_size(&buffer_size, HDLC_FRAME_HEADER_LEN, fs3ipc_instance);

	/* 0xFFFF denotes an empty frame*/
	if (uLen == 0xFFFFu) {
		/* empty frame */
		*uPktLen = 0;
		return fs3ipc_StatusType_OK;
	}

	if ((uLen < FRAME_MIN_LENGTH_SUPPORTED) || (uLen > buffer_size)) {
		INC_STATS(cfg->stats->RxInvalidFrameCnt);
		LOG_ERROR("Invalid header, ignore the packet, frame size: %u",
			uLen);
		return fs3ipc_StatusType_Invalid_Frame_Size;
	}

	if (*uPktLen < (uLen + HDLC_FRAME_HEADER_LEN)) {
		LOG_WARNING("Invalid message size received: %u, expected: %u",
			*uPktLen, (uLen + HDLC_FRAME_HEADER_LEN));
		return fs3ipc_StatusType_MessageSize;
	}

	/* Verify the CRC, if failed drop the frame, else process the frame */
	if (fs3ipc_hdlc_ext_CalculateCRC(pData + HDLC_ADDR_INDEX, uLen) != 0) {
		INC_STATS(cfg->stats->RxCrcErrorCnt);
		LOG_ERROR("CRC check failed, frame dropped!");
		return fs3ipc_StatusType_Rx_InvalidCRC;
	}
	LOG_DEBUG("CRC check passed");

	*uPktLen = uLen;

	return fs3ipc_StatusType_OK;
}

/**
 * fs3ipc_hdlcTransmit() - Handles the Tx operation.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: Input buffer to transmit.
 * @length: Packet size to transmit.
 *
 * This function is used to encode the data based on the state machine.
 *
 * Return: Operation Result.
 */
static fs3ipc_StatusType fs3ipc_hdlcTransmit(uint8_t fs3ipc_instance,
					     fs3ipc_dataPtr dataPtr,
					     fs3ipc_length *length)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];
	fs3ipc_StatusType ret = fs3ipc_StatusType_OK;
	fs3ipc_os_EventMaskType ev = 0, activeEvents = 0;
	fs3ipc_u8 WaitForEvent;
	fs3ipc_u8 initialized = 0;
	fs3ipc_u8 window_open = 0;

	window_open = WindowOpen(fs3ipc_instance);

	fs3ipc_os_GetResource(cfg->lock);
	initialized = ((pHdlc->uInitialized & HDLC_INIT_COMM_EN_FLAGS) ==
		HDLC_INIT_COMM_EN_FLAGS);
	if (pHdlc->uSuspended == FS3IPC_TRUE) {
		activeEvents = EVENT_RESUME | EVENT_SUSPEND;
	} else if (initialized) {
		if (window_open)
			activeEvents = EVENT_ALL;
		else
			activeEvents = EVENT_ALL & ~(EVENT_TX_MSG);
	} else {
		activeEvents = (EVENT_SUSPEND|EVENT_RESUME|EVENT_RX_RST_UFRAME|
			EVENT_RX_ACK_UFRAME|EVENT_EXT_WAKEUP|EVENT_TX_WAKEUP);
	}
	fs3ipc_os_ReleaseResource(cfg->lock);

	fs3ipc_os_GetEvent(cfg->hdlcOSCfg.OS_Task, &ev);
	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, ev & EVENT_TX_WAKEUP);

	LOG_INFO("events Rec:%x, Act:%x, Init=%02x", ev, activeEvents,
		pHdlc->uInitialized);

	ev &= activeEvents;
	WaitForEvent = FS3IPC_FALSE;

	if (ev & EVENT_SUSPEND) {
		HDLC_Handle_EVENT_SUSPEND(fs3ipc_instance, dataPtr, length);
		pHdlc->uSuspended = FS3IPC_TRUE;
	} else if (ev & EVENT_RESUME) {
		HDLC_Handle_EVENT_RESUME(fs3ipc_instance, dataPtr, length);
		pHdlc->uSuspended = FS3IPC_FALSE;
	} else if (ev & EVENT_RX_RST_UFRAME) {
		HDLC_Handle_EVENT_RX_RST_UFRAME(fs3ipc_instance, dataPtr, length);
	} else if (ev & EVENT_RX_ACK_UFRAME) {
		INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->RxResetReqCnt);

		HDLC_Handle_EVENT_RX_ACK_UFRAME(fs3ipc_instance, dataPtr, length);
	} else if (ev & EVENT_TX_TIMEOUT) {
		INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->TxAckTimeout);

		HDLC_Handle_EVENT_TX_TIMEOUT(fs3ipc_instance, dataPtr, length);
	} else if (ev & EVENT_RX_OUT_OF_SEQ_FRAME) {
		INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->RxSequenceErrorCnt);

		HDLC_Handle_EVENT_RX_OUT_OF_SEQ_FRAME(fs3ipc_instance, dataPtr, length);
	} else if (ev & EVENT_RX_RR_POLL_FRAME) {
		INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->RxPollReqCnt);

		HDLC_Handle_EVENT_RX_RR_POLL_FRAME(fs3ipc_instance, dataPtr, length);
	} else if (ev & (EVENT_RX_REJ_FRAME | EVENT_RX_RR_FINAL_FRAME)) {
		INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->TxRetransmit);
		/*Retransmission process*/
		HDLC_Handle_EVENT_TX_Retransmission(fs3ipc_instance, dataPtr, length);
	} else if (ev & EVENT_TX_MSG) {
		/* Normal Transmission section*/
		HDLC_Handle_EVENT_TX_MSG(fs3ipc_instance, dataPtr, length);

		if (*length == 0) {
			if (ev & EVENT_RX_IFRAME)
				/* Handle dedicated Ack*/
				HDLC_Handle_EVENT_RX_IFRAME(fs3ipc_instance, dataPtr, length);
			else if (ev & EVENT_EXT_WAKEUP)
				/* External wakeup request - SPI only*/
				HDLC_Handle_EVENT_EXT_WAKEUP(fs3ipc_instance, dataPtr, length);
			else
				WaitForEvent = FS3IPC_TRUE;
		}
	} else if (ev & EVENT_RX_IFRAME) {
		/* Handle dedicated Ack*/
		HDLC_Handle_EVENT_RX_IFRAME(fs3ipc_instance, dataPtr, length);
	} else if (ev & EVENT_EXT_WAKEUP) {
		/* External wakeup request - SPI only*/
		HDLC_Handle_EVENT_EXT_WAKEUP(fs3ipc_instance, dataPtr, length);
	} else {
		WaitForEvent = FS3IPC_TRUE;
	}

	/*
	 * Since only one event is handled per call, only wait if all events have
	 * been handled. Transmit only if there is something to transmit.
	 */
	if (WaitForEvent == FS3IPC_FALSE) {
		if (*length > 0)
			ret = fs3ipc_hdlc_ext_transmit(fs3ipc_instance, dataPtr,
						       *length);
	} else {
		fs3ipc_os_WaitEvent(cfg->hdlcOSCfg.OS_Task, activeEvents);
	}

	return ret;
}

/**
 * fs3ipc_hdlc_reset() - Reset HDLC state machine.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * It is assumed that the argument is valid.
 *
 * Return: None.
 */
static void fs3ipc_hdlc_reset(uint8_t fs3ipc_instance)
{
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[fs3ipc_instance];
	THdlcManager *hdlc = &pHdlcManager[fs3ipc_instance];

	fs3ipc_os_GetResource(cfg->lock);
	fs3ipc_memset(hdlc, 0, sizeof(THdlcManager));
	fs3ipc_os_ReleaseResource(cfg->lock);

	hdlc->spiIpcMode = FS3IPC_HDLC_NORMAL_MODE;
	hdlc->uLastTxSeqSent = INVALID_SEQ;
	hdlc->fs3ipc_instance = fs3ipc_instance;
	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, (EVENT_ALL));
	hdlc->uInitialized = HDLC_INIT_NONE;

	LOG_DEBUG("%u: HDLC reset", (unsigned int)fs3ipc_instance);
}

fs3ipc_StatusType fs3ipc_hdlc_init(uint8_t fs3ipc_instance)
{
	const fs3ipc_hdlc_AppConfigType *cfg = NULL;
	struct fs3ipc_timer *uframe_timer = NULL;
	struct fs3ipc_timer *iframe_timer = NULL;

	if (fs3ipc_instance >= FS3IPC_HDLC_NUM_OF_INSTANCES) {
		LOG_ERROR("Invalid FS3IPC instance: %u",
			  (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorHandle;
	}

	cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_hdlc_reset(fs3ipc_instance);

	uframe_timer = cfg->uframe_timer;
	iframe_timer = cfg->iframe_timer;
	if (!uframe_timer || !iframe_timer) {
		LOG_ERROR("%u: HDLC timers are not configured: %p, %p",
			  (unsigned int)fs3ipc_instance, (void *)uframe_timer,
			  (void *)iframe_timer);
		return fs3ipc_StatusType_ErrorCfg;
	}

	if (fs3ipc_init_timer(uframe_timer, fs3ipc_instance, process_uframe_timeout) !=
			fs3ipc_StatusType_OK) {
		LOG_ERROR("%u: U frame timer initialization failed",
			  (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorGen;
	}
	LOG_DEBUG("%u: '%s' initialized", (unsigned int)fs3ipc_instance,
		  fs3ipc_get_timer_name(uframe_timer));

	if (fs3ipc_init_timer(iframe_timer, fs3ipc_instance, process_iframe_timeout) !=
			fs3ipc_StatusType_OK) {
		LOG_ERROR("%u: I frame timer initialization failed",
			  (unsigned int)fs3ipc_instance);
		return fs3ipc_StatusType_ErrorGen;
	}
	LOG_DEBUG("%u: '%s' initialized", (unsigned int)fs3ipc_instance,
		  fs3ipc_get_timer_name(iframe_timer));

	fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_RST_UFRAME);

	LOG_DEBUG("%u: HDLC initialized", (unsigned int)fs3ipc_instance);

	return fs3ipc_StatusType_OK;
}

/**********************************************************
 *                  ENCODE function section              *
 **********************************************************/

/**
 * hdlcEncodeLengthFrame() - Encodes the length size.
 * @pBuffer: The input buffer.
 * @iLen: Total length.
 *
 * This function encodes the length value at the established position.
 *
 * Return: None.
 */
static void hdlcEncodeLengthFrame(fs3ipc_dataPtr pBuffer, const fs3ipc_s32 iLen)
{
	if (pBuffer) {
		pBuffer[0] = 0x00FF & iLen;
		pBuffer[1] = ((0xFF00 & iLen) >> 8);
	} else {
		LOG_WARNING("Encode length is null");
	}
}

/**
 * hdlcEncodeCRCFrame() - Encodes the CRS size.
 * @pData: The input buffer.
 * @iLen: Total length.
 *
 * This function encodes the CRC frame into the correct position.
 *
 * Return: None.
 */
static void hdlcEncodeCRCFrame(fs3ipc_dataPtr pData, fs3ipc_s32 iLen)
{
	fs3ipc_u16 wCrc;
	/* Calculate CRC on the original data including header*/
	wCrc = fs3ipc_hdlc_ext_CalculateCRC(pData, iLen);
	pData[iLen + 1] = (fs3ipc_u8)(wCrc & 0x00ff);
	pData[iLen] = (fs3ipc_u8)(wCrc >> 8);
}

/**********************************************************
 *             CREATE function section                    *
 **********************************************************/
/**
 * hdlcEncodeTxHeaderSFrame() - Encodes all the parameter required for S-frames.
 * @fs3ipc_instance: FS3IPC instance.
 * @pBuffer: The input buffer.
 * @length: Total length.
 *
 * Return: None.
 */
static void hdlcEncodeTxHeaderSFrame(uint8_t fs3ipc_instance,
				     fs3ipc_dataPtr pBuffer,
				     fs3ipc_length *length)
{
	fs3ipc_dataPtr pData = NULL;

	if (pBuffer) {
		fs3ipc_length total_length = HDLC_TOTAL_FRAME;

		pData = pBuffer;
		hdlcEncodeCRCFrame(pData + HDLC_FRAME_HEADER_OFFSET,
			HDLC_FRAME_HEADER_LEN);
		hdlcEncodeLengthFrame(pData, HDLC_TOTAL_HEADER);
		*length = total_length;
	} else {
		LOG_ERROR("Invalid argument:- pBuffer: %p", (void *)pBuffer);
	}
}

/**
 * hdlcCreateIframe() - Encodes the I frame parameters.
 * @snd_seq: Send sequence number.
 * @rcv_seq: Receive sequence number.
 * @pBuffer: Input buffer to modify.
 *
 * This function encodes the NR,NS, PF parameters.
 *
 * Return: None.
 */
static void hdlcCreateIframe(fs3ipc_u8 snd_seq, fs3ipc_u8 rcv_seq,
	fs3ipc_dataPtr pBuffer)
{
	if (pBuffer) {
		TIframe *pIframe = (TIframe *)pBuffer;

		fs3ipc_memset(pIframe, 0x0, sizeof(TIframe));
		pIframe->ns = snd_seq;
		pIframe->nr = rcv_seq;
		pIframe->pf = 0;
		pIframe->hdlc_0 = 0;
	} else {
		LOG_ERROR("Invalid argument:- pBuffer: %p", (void *)pBuffer);
	}
}

/**
 * hdlcCreateRRAckFrame() - Encodes the I frame parameters.
 * @uRxSeq: Receive sequence number.
 * @pBuffer: Input buffer to modify.
 *
 * This function encodes the NR,NS, PF parameters.
 *
 * Return: None.
 */
static void hdlcCreateRRAckFrame(fs3ipc_u8 uRxSeq, fs3ipc_dataPtr pBuffer)
{
	if (pBuffer) {
		TSframe *sframe = (TSframe *)pBuffer;

		fs3ipc_memset(sframe, 0, sizeof(TSframe));
		sframe->nr = uRxSeq;
		sframe->pf = FRAME_REQUEST;
		sframe->hdlc_01 = 1;
		sframe->type_ss = 0;
	} else {
		LOG_ERROR("Invalid argument:- pBuffer: %p", (void *)pBuffer);
	}
}

/**
 * hdlcCreateRRPollFrame() - Encodes the Poll  Frame command.
 * @rcv_seq: Receive sequence number.
 * @pBuffer: Input buffer to modify.
 *
 * Return: None.
 */
static void hdlcCreateRRPollFrame(fs3ipc_u8 rcv_seq, fs3ipc_dataPtr pBuffer)
{
	if (pBuffer) {
		TSframe *sframe = (TSframe *)pBuffer;

		fs3ipc_memset(sframe, 0, sizeof(TSframe));
		sframe->addr_cr = POLL_COMMAND;
		sframe->nr = rcv_seq;
		sframe->pf = POLL_REQUEST;
		sframe->hdlc_01 = 1;
	} else {
		LOG_ERROR("Invalid argument:- pBuffer: %p", (void *)pBuffer);
	}
}

/**
 * hdlcCreateRRFinalFrame() - Encodes the final frame response with the latest
 *                            received frame index.
 * @rcv_seq: Receive sequence number.
 * @pBuffer: Input buffer to modify.
 *
 * This function encodes the Receive Ready frame.
 *
 * Return: None.
 */
static void hdlcCreateRRFinalFrame(fs3ipc_u8 rcv_seq, fs3ipc_dataPtr pBuffer)
{
	if (pBuffer) {
		TSframe *sframe = (TSframe *)pBuffer;

		fs3ipc_memset(sframe, 0, sizeof(TSframe));
		sframe->addr_cr = POLL_RESPONSE;
		sframe->nr = rcv_seq;
		sframe->pf = POLL_REQUEST;
		sframe->hdlc_01 = 1;
	} else {
		LOG_ERROR("Invalid argument:- pBuffer: %p", (void *)pBuffer);
	}
}

/**
 * hdlcCreateREJFrame() - Encodes the reject command.
 * @rcv_seq: Unexpected counter index received.
 * @pBuffer: Input buffer to modify.
 *
 * Return: None.
 */
static void hdlcCreateREJFrame(fs3ipc_u8 rcv_seq, fs3ipc_dataPtr pBuffer)
{
	if (pBuffer) {
		TSframe *sframe = (TSframe *)pBuffer;

		fs3ipc_memset(sframe, 0, sizeof(TSframe));
		sframe->nr = rcv_seq;
		sframe->pf = FRAME_REQUEST;
		sframe->hdlc_01 = 1;
		sframe->type_ss = 1;
	} else {
		LOG_ERROR("Invalid argument:- pBuffer: %p", (void *)pBuffer);
	}
}

/**
 * hdlcCreateUAckFrame() - Encodes the U frame acknowledge  frame.
 * @pBuffer: Input buffer to modify.
 * @window_size: Current window size configured.
 * @frame_size: Current frame size established.
 *
 * This function is called when the RST command is received.
 *
 * Return: None.
 */
static void hdlcCreateUAckFrame(fs3ipc_dataPtr pBuffer, fs3ipc_u8 window_size,
										  fs3ipc_u16 frame_size)
{
	TUframe *zUframe = (TUframe *)pBuffer;

	fs3ipc_memset(zUframe, 0, sizeof(TUframe));
	zUframe->type_12 = 0x0;
	zUframe->type_345 = 0x3;
	zUframe->hdlc_11 = U_FRAME;
	zUframe->window_size = window_size;
	zUframe->frame_size_l = 0x00FF & frame_size;
	zUframe->frame_size_h = ((0xFF00 & frame_size) >> 8);
}

/**
 * hdlcCreateURstFrame() - Encodes the U frame acknowledge frame.
 * @pBuffer: Input buffer to modify.
 * @window_size: Current window size configured.
 * @frame_size: Current frame size established.
 *
 * This function is called when the Reset command is called.
 *
 * Return: None.
 */
static void hdlcCreateURstFrame(fs3ipc_dataPtr pBuffer, fs3ipc_u8 window_size,
										  fs3ipc_u16 frame_size)
{
	TUframe *zUframe = (TUframe *)pBuffer;

	fs3ipc_memset(zUframe, 0, sizeof(TUframe));
	zUframe->type_12 = 0x3;
	zUframe->type_345 = 0x4;
	zUframe->hdlc_11 = U_FRAME;
	zUframe->window_size = window_size;
	zUframe->frame_size_l = 0x00FF & frame_size;
	zUframe->frame_size_h = ((0xFF00 & frame_size) >> 8);
}

/**
 * hdlc_calculate_buffer_size() - Calculates the current buffer size and the
 *                                size is subtracted in case if it is required.
 * @buffer_size: The buffer size to be calculated.
 * @remove_data: Removes the number of bytes.
 * @fs3ipc_instance: The FS3IPC instance.
 *
 * Return: None.
 */
static void hdlc_calculate_buffer_size(fs3ipc_length *buffer_size,
				       const fs3ipc_u16 remove_data,
				       uint8_t fs3ipc_instance)
{
	FS3IPC_HDLC_BUFFER_T SPI_IPC_Mode = pHdlcManager[fs3ipc_instance].spiIpcMode;

	switch (SPI_IPC_Mode) {
	case FS3IPC_HDLC_NORMAL_MODE:
		*buffer_size = (HDLC_DATA_MIN_LEN - remove_data);
	break;

	case FS3IPC_HDLC_BOOST_MODE:
		*buffer_size = (HDLC_DATA_MAX_LEN - remove_data);
	break;

	default:
		LOG_ERROR("Invalid Frame Size Requested");
		break;
	}
}

/**
 * HDLC_Handle_EVENT_TX_TIMEOUT() - Handles the EVENT_TX_TIMEOUT sending the
 *                                  proper data information to process the event
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_TX_TIMEOUT event, encodes
 *           the proper data to send the timeout.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_TX_TIMEOUT(uint8_t fs3ipc_instance,
					 fs3ipc_dataPtr dataPtr,
					 fs3ipc_length *length)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_TIMEOUT);

	if (pHdlc->zGoBackN.ack_exp != pHdlc->zGoBackN.tx_seq) {
		fs3ipc_dataPtr pData = dataPtr;

		hdlcCreateRRPollFrame(INVALID_SEQ, pData + HDLC_ADDR_INDEX);
		hdlcEncodeTxHeaderSFrame(fs3ipc_instance, pData, length);
		INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->TxSFrameCnt);
		restart_iframe_timer(cfg);
		LOG_WARNING("[EVENT_TX_TIMEOUT] ESeq:%d TSeq:%d",
			pHdlc->zGoBackN.ack_exp, pHdlc->zGoBackN.tx_seq);
	} else {
		*length = 0;

		LOG_WARNING("[EVENT_TX_TIMEOUT] ignored EVENT_TX_TIMEOUT due to expected ack_exp and tx_seq");
	}
}

/**
 * HDLC_Handle_EVENT_RX_OUT_OF_SEQ_FRAME() - Handles the EVENT_RX_OUT_OF_SEQ_FRAME,
 *                                           it encodes the proper data information
 *                                           to process the event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_RX_OUT_OF_SEQ_FRAME event,
 *           encodes the proper data to send out of sequence reception.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_RX_OUT_OF_SEQ_FRAME(uint8_t fs3ipc_instance,
						  fs3ipc_dataPtr dataPtr,
						  fs3ipc_length *length)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_OUT_OF_SEQ_FRAME);

	/*based on HDLC spec only one reject command is allowed to be sent*/
	if (pHdlc->zGoBackN.rejAlreadySent == FS3IPC_TRUE) {
		fs3ipc_dataPtr pData = dataPtr;

		hdlcCreateREJFrame(pHdlc->zGoBackN.rx_seq, pData + HDLC_ADDR_INDEX);
		hdlcEncodeTxHeaderSFrame(fs3ipc_instance, pData, length);
		INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->TxSFrameCnt);

		LOG_WARNING("[EVENT_RX_OUT_OF_SEQ_FRAME] rejected frame = %d",
			    pHdlc->zGoBackN.rx_seq);
	} else {
		*length = 0;

		LOG_WARNING("[EVENT_RX_OUT_OF_SEQ_FRAME] already rejected FRAME RX Seq:%d",
			    pHdlc->zGoBackN.rx_seq);
	}
}

/**
 * HDLC_Handle_EVENT_RX_RR_POLL_FRAME - Handles the EVENT_RX_RR_POLL_FRAME, it
 *                                      encodes the proper data information to
 *                                      process the event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_RX_RR_POLL_FRAME event,
 *           encodes the proper data to send out of sequence reception.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 **/
static void HDLC_Handle_EVENT_RX_RR_POLL_FRAME(uint8_t fs3ipc_instance,
					       fs3ipc_dataPtr dataPtr,
					       fs3ipc_length *length)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_dataPtr pData = dataPtr;

	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_RR_POLL_FRAME);

	hdlcCreateRRFinalFrame(pHdlc->zGoBackN.rx_seq, pData + HDLC_ADDR_INDEX);
	hdlcEncodeTxHeaderSFrame(fs3ipc_instance, pData, length);
	INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->TxSFrameCnt);

	LOG_WARNING("[EVENT_RX_RR_POLL_FRAME] RR Final FRAME RX Seq:%d",
		    pHdlc->zGoBackN.rx_seq);
}

/**
 * HDLC_Handle_EVENT_TX_Retransmission() - Handles the retransmission based on
 *                                         EVENT_RX_REJ_FRAME or
 *                                         EVENT_RX_RR_FINAL_FRAME encoding the
 *                                         proper data information to process
 *                                         the event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_RX_REJ_FRAME or
 *           EVENT_RX_RR_FINAL_FRAME events, encodes the proper data to send the
 *           retransmission.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
void HDLC_Handle_EVENT_TX_Retransmission(uint8_t fs3ipc_instance,
					 fs3ipc_dataPtr dataPtr,
					 fs3ipc_length *length)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_u8 uLastTxSeqSent = pHdlcManager[fs3ipc_instance].uLastTxSeqSent;
	fs3ipc_length outLength = 0;

	fs3ipc_u8 uLastFIdx = 0;
	fs3ipc_u8 uFirstFIdx = 0;
	fs3ipc_u8 uRxSeq = 0;

	fs3ipc_dataPtr pData = dataPtr;

	fs3ipc_os_GetResource(cfg->lock);
	uFirstFIdx = pHdlc->zGoBackN.ack_exp;
	uLastFIdx = pHdlc->zGoBackN.tx_seq;
	uRxSeq = pHdlc->zGoBackN.rx_seq;
	fs3ipc_os_ReleaseResource(cfg->lock);

	if (uLastTxSeqSent == INVALID_SEQ) {
		LOG_WARNING("Retransmitting FRAME: ESeq:%d Index:%d",
			    pHdlc->zGoBackN.ack_exp, uLastFIdx);
		uLastTxSeqSent = uFirstFIdx % cfg->uWindowSiz;
	}

	if (uLastTxSeqSent != uLastFIdx) {
		fs3ipc_length wLength = 0;
		fs3ipc_length hdlc_layer_size = HDLC_DATA_MAX_LEN;
		fs3ipc_os_EventMaskType ev = 0;

		fs3ipc_os_GetResource(cfg->lock);
		fs3ipc_os_GetEvent(cfg->hdlcOSCfg.OS_Task, &ev);
		if (ev & EVENT_RX_IFRAME)
			fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_IFRAME);
		fs3ipc_os_ReleaseResource(cfg->lock);

		LOG_WARNING("Current Retransmitted FRAME: Index: [%d of %d]",
			    uLastTxSeqSent, uLastFIdx);
		/*obtain the length size*/
		wLength = pHdlc->zTxQueue[uLastTxSeqSent].wLength;

		hdlc_calculate_buffer_size(&hdlc_layer_size, HDLC_TOTAL_FRAME,
					   fs3ipc_instance);

		if (wLength <= hdlc_layer_size) {

			/*
			 * Copy transmitted data from Tx Circular Buffer to the available
			 * index buffer
			 */
			fs3ipc_memcpy(pData + HDLC_FRAME_DATA_OFFSET,
				pHdlc->zTxQueue[uLastTxSeqSent].uData + HDLC_FRAME_DATA_OFFSET,
				wLength);

			/*update the NR index */
			hdlcCreateIframe(uLastTxSeqSent, uRxSeq,
				(fs3ipc_dataPtr)&pData[HDLC_ADDR_INDEX]);

			/*recalculate the CRC due to NR index update */
			hdlcEncodeCRCFrame(&pData[HDLC_ADDR_INDEX], wLength + CRC_SIZE);

			hdlcEncodeLengthFrame(pData, wLength + HDLC_CTR_ADDR_LEN +
				HDLC_FRAME_HEADER_LEN);

			outLength = wLength + HDLC_DATA_IDX + CRC_SIZE;

			/*move to the following frame to be retransmitted*/
			fs3ipc_IncSeqNo(fs3ipc_instance, &uLastTxSeqSent);

			restart_iframe_timer(cfg);
		} else {
			LOG_ERROR("Retransmission Error, invalid Frame Length received = %d",
				  wLength);
		}
	} else {
		/* all the data has been retransmitted*/
		fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task,
			EVENT_RX_REJ_FRAME|EVENT_RX_RR_FINAL_FRAME);

		/*
		 * Reset the variable to  invalid parameter for subsequent
		 * retransmission event
		 */
		uLastTxSeqSent = INVALID_SEQ;
	}

	*length = outLength;

	pHdlcManager[fs3ipc_instance].uLastTxSeqSent = uLastTxSeqSent;
}

/**
 * HDLC_Handle_EVENT_SUSPEND() - Handles the EVENT_RESUME sending the proper
 *                               data information to process the event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_RX_IFRAME event, encodes the
 *           proper data to send the ack.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_SUSPEND(uint8_t fs3ipc_instance,
				      fs3ipc_dataPtr dataPtr,
				      fs3ipc_length *length)
{
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_hdlc_ext_HandleExtNodeReset(fs3ipc_instance);
	stop_iframe_timer(cfg);
	stop_uframe_timer(cfg);
	fs3ipc_hdlc_reset(fs3ipc_instance);
	LOG_ALERT("%u: Done", (unsigned int)fs3ipc_instance);

	/* nothing to write out*/
	*length = 0;
}

/**
 * HDLC_Handle_EVENT_RESUME() - Handles the EVENT_RESUME.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_RX_IFRAME event, encodes the
 *           proper data to send the ack.
 * @length: Return the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_RESUME(uint8_t fs3ipc_instance,
				     fs3ipc_dataPtr dataPtr,
				     fs3ipc_length *length)
{
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RESUME);
	fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_RST_UFRAME);
	LOG_ALERT("%u: Done", (unsigned int)fs3ipc_instance);

	/* nothing to write out*/
	*length = 0;
}

/**
 * HDLC_Handle_EVENT_RX_IFRAME() - Handles the EVENT_RX_IFRAME sending the
 *                                 proper data information to process the
 *                                 event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_RX_IFRAME event, encodes the
 *           proper data to send the ack.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_RX_IFRAME(uint8_t fs3ipc_instance,
					fs3ipc_dataPtr dataPtr,
					fs3ipc_length *length)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_u8 uRxSeq;
	fs3ipc_dataPtr pData = dataPtr;

	fs3ipc_os_GetResource(cfg->lock);
	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_IFRAME);
	uRxSeq = pHdlc->zGoBackN.rx_seq;
	fs3ipc_os_ReleaseResource(cfg->lock);

	hdlcCreateRRAckFrame(uRxSeq, pData + HDLC_ADDR_INDEX);
	hdlcEncodeTxHeaderSFrame(fs3ipc_instance, pData, length);
	INC_STATS(fs3ipc_hdlc_appConfigs[fs3ipc_instance].stats->TxSFrameCnt);

	LOG_DEBUG("[EVENT_RX_IFRAME] Rx ACK =[%d]", uRxSeq);
}

/**
 * HDLC_Handle_EVENT_RX_ACK_UFRAME() - Handles the EVENT_RX_ACK_UFRAME sending
 *                                     the proper data information to process
 *                                     the event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the HDLC_Handle_EVENT_RX_ACK_UFRAME
 *           event, encodes the proper data to send u frame ack reception.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_RX_ACK_UFRAME(uint8_t fs3ipc_instance,
					    fs3ipc_dataPtr dataPtr,
					    fs3ipc_length *length)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_dataPtr pData = dataPtr;

	fs3ipc_os_GetResource(cfg->lock);
	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_ACK_UFRAME);
	pHdlc->uInitialized |= HDLC_INIT_UACK_TX;
	fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_WAKEUP);
	fs3ipc_os_ReleaseResource(cfg->lock);

	hdlcCreateUAckFrame(pData + HDLC_ADDR_INDEX, cfg->uWindowSiz,
		cfg->uBufferSiz);
	hdlcEncodeCRCFrame(pData + HDLC_FRAME_HEADER_OFFSET,
		HDLC_UFRAME_PAYLOAD_LEN + HDLC_FRAME_HEADER_LEN);
	hdlcEncodeLengthFrame(pData, HDLC_TOTAL_HEADER + HDLC_UFRAME_PAYLOAD_LEN);

	*length = HDLC_TOTAL_HEADER + HDLC_UFRAME_PAYLOAD_LEN + CRC_SIZE;

	LOG_ALERT("%u: Sending U frame ACK", (unsigned int)fs3ipc_instance);
}

/**
 * HDLC_Handle_EVENT_RX_RST_UFRAME() - Handles the EVENT_RX_RST_UFRAME sending
 *                                     the proper data information to process
 *                                     the event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the HDLC_Handle_EVENT_RX_ACK_UFRAME
 *           event, encodes the proper data to send u frame reset command.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_RX_RST_UFRAME(uint8_t fs3ipc_instance,
					    fs3ipc_dataPtr dataPtr,
					    fs3ipc_length *length)
{
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	fs3ipc_dataPtr pData = dataPtr;

	hdlcCreateURstFrame(pData + HDLC_ADDR_INDEX, cfg->uWindowSiz,
		cfg->uBufferSiz);
	hdlcEncodeCRCFrame(pData + HDLC_FRAME_HEADER_OFFSET,
		HDLC_UFRAME_PAYLOAD_LEN + HDLC_FRAME_HEADER_LEN);
	hdlcEncodeLengthFrame(pData, HDLC_TOTAL_HEADER + HDLC_UFRAME_PAYLOAD_LEN);

	*length = HDLC_TOTAL_HEADER + HDLC_UFRAME_PAYLOAD_LEN + CRC_SIZE;

	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_RST_UFRAME);

	restart_uframe_timer(cfg);

	LOG_ALERT("%u: Sending U frame RESET", (unsigned int)fs3ipc_instance);
}

/**
 * HDLC_Handle_EVENT_EXT_WAKEUP() - Handles the EVENT_EXT_WAKEUP sending the
 *                                  proper data information to process the
 *                                  event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_TX_MSG event, encodes the
 *           proper data to send u frame reset command.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_EXT_WAKEUP(uint8_t fs3ipc_instance,
					 fs3ipc_dataPtr dataPtr,
					 fs3ipc_length *length)
{
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	/* empty frame - send all 0xFF packet*/
	memset(dataPtr, 0xFF, *length);
	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_EXT_WAKEUP);
}

/**
 * HDLC_Handle_EVENT_TX_MSG() - Handles the EVENT_TX_MSG sending the proper
 *                              data information to process the event.
 * @fs3ipc_instance: FS3IPC instance.
 * @dataPtr: The data pointer to process the EVENT_TX_MSG event, encodes the
 *           proper data to send u frame reset command.
 * @length: Returns the total bytes to transmit.
 *
 * Return: None.
 */
static void HDLC_Handle_EVENT_TX_MSG(uint8_t fs3ipc_instance,
				     fs3ipc_dataPtr dataPtr,
				     fs3ipc_length *length)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_StatusType fs3ipc_return;

	fs3ipc_length wLength = *length - (HDLC_FRAME_DATA_OFFSET + CRC_SIZE);
	fs3ipc_length maxLength = wLength;

	fs3ipc_u8 uRxSeq = 0;
	fs3ipc_u8 uTxSeq = 0;

	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

	LOG_DEBUG("Processing WindowOpen");
	fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_MSG);

	uTxSeq = pHdlc->zGoBackN.tx_seq;

	/* moving the dataPtr to data position*/
	fs3ipc_return = fs3ipc_hdlc_ext_Encode(fs3ipc_instance, dataPtr +
					       HDLC_FRAME_DATA_OFFSET, &wLength);

	LOG_DEBUG("Encoding length[%d]", wLength);

	/*validate response from fs3ipc_hdlc_ext_Encode*/
	if ((fs3ipc_return == fs3ipc_StatusType_OK) && (wLength != 0) &&
	    (wLength <= maxLength)) {
		/* keep rearming send event as there may be more data to send */
		fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_MSG);

		pHdlc->zTxQueue[uTxSeq].wLength = wLength;
		uRxSeq = pHdlc->zGoBackN.rx_seq;

		INC_STATS(cfg->stats->TxIframeCnt);

		/*
		 * Verify if ACK needs to be sent, if it is the case the piggy ack
		 * is configured
		 */
		fs3ipc_os_ClearEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_IFRAME);

		/**add I frame with proper Tx and Rx parameter*/
		hdlcCreateIframe(pHdlc->zGoBackN.tx_seq, uRxSeq,
			(fs3ipc_dataPtr)&dataPtr[HDLC_ADDR_INDEX]);

		/** Encode CRC and length frames */
		hdlcEncodeCRCFrame(&dataPtr[HDLC_ADDR_INDEX], wLength + CRC_SIZE);
		hdlcEncodeLengthFrame(dataPtr, wLength + HDLC_CTR_ADDR_LEN +
			HDLC_FRAME_HEADER_LEN);

		*length = wLength + HDLC_DATA_IDX + CRC_SIZE;

		/*increment the subsequent Tx index for the next index to send*/
		fs3ipc_IncSeqNo(fs3ipc_instance, &(pHdlc->zGoBackN.tx_seq));

		/* Restart I frame timer */
		restart_iframe_timer(cfg);
	} else {
		pHdlc->zTxQueue[uTxSeq].wLength = 0;
		LOG_DEBUG("returned [%d] error ", fs3ipc_return);

		/* set buffer size to 0. There is no message to encode*/
		*length = 0;
	}
}

/**
 * fs3ipcSetValidIFrameIndex() - Sets the reception counter besides it
 *                               validates if the Rx index is the expected.
 * @fs3ipc_instance: FS3IPC instance.
 * @pRxFrame: The input buffer.
 *
 * This function validates the expected RX index, prepares the ACK mode for
 * transmission sets the subsequent RX index to process.
 *
 * Return: None.
 */
static void fs3ipcSetValidIFrameIndex(uint8_t fs3ipc_instance,
				      fs3ipc_cDataPtr pRxFrame)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];
	const TIframe *pIframe = (TIframe *)pRxFrame;

	if (pIframe->ns == pHdlc->zGoBackN.rx_seq) {
		pHdlc->zGoBackN.prevSeq = pHdlc->zGoBackN.rx_seq;
		fs3ipc_IncSeqNo(fs3ipc_instance, &(pHdlc->zGoBackN.rx_seq));
		pHdlc->zGoBackN.rejAlreadySent = FS3IPC_FALSE;

		fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_RX_IFRAME);

		fs3ipcSetITimerMode(fs3ipc_instance, pRxFrame);
	}
}

/**
 * fs3ipcSetITimerMode() - Sets the correct timer mode besides it validates
 *                         the expected counter indexes.
 * @fs3ipc_instance: FS3IPC instance.
 * @pRxFrame: The input buffer.
 *
 * This function validates the indexes to determine if it is required to stop or
 * restart the timer.
 *
 * Return: None.
 */
static void fs3ipcSetITimerMode(uint8_t fs3ipc_instance, fs3ipc_cDataPtr pRxFrame)
{
	THdlcManager *pHdlc = &pHdlcManager[fs3ipc_instance];
	fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];
	const TIframe *pIframe = (TIframe *)pRxFrame;
	fs3ipc_u8 fs3ipc_oos = FS3IPC_FALSE;
	fs3ipc_os_EventMaskType ev = 0;

	/* check if it is processing out of sequence error (OOS) */
	fs3ipc_os_GetEvent(cfg->hdlcOSCfg.OS_Task, &ev);
	if (ev & EVENT_RX_OUT_OF_SEQ_FRAME)
		fs3ipc_oos = FS3IPC_TRUE;

	/*
	 * Verify the nr of the received frame to check for acknowledgment of the
	 * frames which we have sent. the ack should be : ack_exp <= ack < tx_seq
	 */
	if ((pIframe->nr != INVALID_SEQ) && (pHdlc->zGoBackN.ack_exp !=
	    pHdlc->zGoBackN.tx_seq)) {
		if (validNr(pHdlc->zGoBackN.ack_exp, pIframe->nr, pHdlc->zGoBackN.tx_seq)) {
			if (pIframe->nr == pHdlc->zGoBackN.ack_exp) {
				LOG_WARNING("[IFRAME] %s I-frame Piggy Ack: Invalid Nr, already acked: exp ack window[%d %d]",
					fs3ipc_oos == FS3IPC_TRUE ?
					"OOS" : "", pHdlc->zGoBackN.ack_exp, pHdlc->zGoBackN.tx_seq);
			} else {
				if (pIframe->nr == pHdlc->zGoBackN.tx_seq) {
					stop_iframe_timer(cfg);
					LOG_DEBUG("[IFRAME] %s stop timer window[%d %d] Nr-%d",
						  fs3ipc_oos == FS3IPC_TRUE ? "OOS" : "",
						  pHdlc->zGoBackN.ack_exp,
						  pHdlc->zGoBackN.tx_seq, pIframe->nr);
				} else {
					restart_iframe_timer(cfg);
					LOG_DEBUG("[IFRAME] %s restart timer window[%d %d] Nr-%d",
						  fs3ipc_oos == FS3IPC_TRUE ? "OOS" : "",
						  pHdlc->zGoBackN.ack_exp,
						  pHdlc->zGoBackN.tx_seq,
						  pIframe->nr);
				}
				/*registering the last received message*/
				pHdlc->zGoBackN.ack_exp = pIframe->nr;
				/* window may now be open. Set event*/
				fs3ipc_os_SetEvent(cfg->hdlcOSCfg.OS_Task, EVENT_TX_WAKEUP);
			}
		} else {
			LOG_ERROR("[IFRAME] %s Piggy Ack: Invalid Nr, out of window: exp ack window[%d %d] nr=%d",
				fs3ipc_oos == FS3IPC_TRUE ? "OOS" : "",
				pHdlc->zGoBackN.ack_exp, pHdlc->zGoBackN.tx_seq, pIframe->nr);
		}
	} else {
		/*
		 * This else condition only is informative, it shows the case when all
		 * the messages have been "acked" and transmitted
		 */
		LOG_DEBUG("[IFRAME]: %s exp ack window[%d %d] nr=%d",
			fs3ipc_oos == FS3IPC_TRUE ? "OOS" : "",
			pHdlc->zGoBackN.ack_exp, pHdlc->zGoBackN.tx_seq, pIframe->nr);
	}
}

/**
 * fs3ipc_hdlc_GetStats() - Returns the statistics related to hdlc layer.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * Return: fs3ipc_hdlc_stats_t the structure related to hdlc layer.
 */
#ifdef CONFIG_FS3IPC_DEBUG
const fs3ipc_hdlc_stats_t *fs3ipc_hdlc_GetStats(uint8_t fs3ipc_instance)
{
	fs3ipc_hdlc_stats_t *ret = FS3IPC_NULL;

	if (fs3ipc_instance < FS3IPC_NUM_OF_INSTANCES) {
		fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

		if (cfg->stats)
			ret = cfg->stats;
	}

	return ret;
}

/**
 * fs3ipc_hdlc_ClearStats() - Clears the statistics related to hdlc layer.
 * @fs3ipc_instance: FS3IPC instance.
 *
 * Return: The result operation.
 */
fs3ipc_StatusType fs3ipc_hdlc_ClearStats(uint8_t fs3ipc_instance)
{
	fs3ipc_StatusType stat = fs3ipc_StatusType_ErrorHandle;

	if (fs3ipc_instance < FS3IPC_NUM_OF_INSTANCES) {
		fs3ipc_hdlc_AppConfigType *cfg = &fs3ipc_hdlc_appConfigs[fs3ipc_instance];

		if (cfg->stats) {
			fs3ipc_memset(cfg->stats, 0, sizeof(fs3ipc_hdlc_stats_t));
			stat = fs3ipc_StatusType_OK;
		}
	}
	return stat;
}
#endif

/*******************************************************************************
 * The following section only was created for Manual Unit testing executing
 * white box profile, it ensures and tests the bit stream and induce some errors
 * Please enable the DATA_LAYER_TEST test definition.
 * Warning!
 *****************************************************************************
 */
/**
 * FS3IPC_PRINT_BUFFER() - Prints the buffer.
 * @printbuffer: The buffer to print.
 * @length: The number of bytes to print.
 *
 * Warning: Only enable this function for debug proposes, the print process
 * delays the TX/RX messaging.
 *
 * Return: None.
 */
static void FS3IPC_PRINT_BUFFER(const fs3ipc_u8 *printbuffer,
	const fs3ipc_u16 length)
{
#if FS3IPC_DATA_LAYER_ENABLE_TEST_SECTION == 1
	fs3ipc_u16 i = 0;
	fs3ipc_u16 si = 0;
	char  s[36];

	while (i < length) {
		if (si) {
			if ((i & 0xFu) == 0) {
				s[si] = '\0';
				LOG_WARNING(s);
				si = 0;
			} else if ((i & 0x3u) == 0) {
				s[si++] = ' ';
			} else {
			}
		}

		si += snprintf(&s[si], sizeof(s)-si, "%02x", printbuffer[i++]);
	}

	if (si > 0) {
		s[si] = '\0';
		LOG_WARNING(s);
	}

	/*
	 * Old logic - one byte per debug message
	 * fs3ipc_u16 index;
	 * DBG_PRINT(FS3IPC_DBG_WARNING, "\n");
	 * for(index = 0; index < length; index++)
	 * {
	 *	DBG_PRINT(FS3IPC_DBG_WARNING, "[%x]", printbuffer[index]);
	 * }
	 * DBG_PRINT(FS3IPC_DBG_WARNING, "\n");
	 */
#endif
}

#if FS3IPC_DATA_LAYER_ENABLE_TEST_SECTION == 1

#define PASS (1)
#define FAIL (0)

/* @test:  checks valid length 4 size
 * @brief: verifies the condition when only 4 bytes are injected
 *
 * @pre:  none
 *
 * -test steps
 *    -#  set the local buffer to {4,0,0,0,0}
 *
 * -test verification
 *    -# verify the uPktLen if it corresponds to 4
 *    -# verify the function return true
 *
 * @post
 *    NONE
 *
 * @return
 *    PASS
 *    FAIL
 *
 */
static fs3ipc_s32 Test001(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;
	fs3ipc_u16 wFrameLen = 0;

	/** test steps*/
	fs3ipc_u8 local_buffer[] = {4, 0, 0, 0, 0};

	delay(2000);
	LOG_WARNING("Test %d Function: checkValidFrame, Case Valid Header",
		test_case_counter);

	/* function to test */
	ret = fs3ipc_checkValidFrame(local_buffer, &wFrameLen, FS3IPC_INSTANCE_0);

	/*test verification*/
	if ((ret == FS3IPC_TRUE) && (wFrameLen == 4))
		ret = PASS;

	return ret;
}

/*
 * @test:  checks invalid length  parameter
 * @brief: verifies the condition when it is injected
 *         invalid length
 * @pre:  none
 *
 * -test steps
 *    -#  set the local buffer to {0xFF,0xFF,0,0,0}
 *
 * -test verification
 *    -# verify the return if it is false
 *    -# verify if length is zero
 *
 * @post
 *    NONE
 *
 * @return
 *    PASS
 *    FAIL
 *
 */
static fs3ipc_s32 Test002(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;
	fs3ipc_u16 wFrameLen = 0;

	/** test steps*/
	fs3ipc_u8 local_buffer[] = {0xFF, 0xFF, 0, 0, 0};

	LOG_WARNING("Test %d checkValidFrame , Case InValid Header",
		test_case_counter);

	/* function to test */
	ret = fs3ipc_checkValidFrame(local_buffer, &wFrameLen, FS3IPC_INSTANCE_0);

	/*test verification*/
	if ((ret == FS3IPC_FALSE) && (wFrameLen == 0))
		ret = PASS;

	return ret;
}

/*
 * @test:  Checks valid CRC
 * @brief: verifies the condition when the CRC is calculated
 *
 * @pre:  none
 *
 * -test steps
 *    -#  set the local buffer
 *
 * -test verification
 *    -# verify the return if it is true
 *
 * @post
 *    NONE
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test003(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0x5, 0x0, 0x0, 0x63, 0x05, 0xc2, 0x40};

	LOG_WARNING("Test %d Function: CalculateCRC,    Case Valid Parameter ",
		total_test_cases);

	/** function to test */
	ret = (CalculateCRC(&test_buffer[HDLC_ADDR_INDEX],
		*((fs3ipc_u16 *)&test_buffer[0])) == 0);

	return ret;
}

/*
 * @test:  Checks invalid CRC
 * @brief: invalid CRC injected
 *
 * @pre:  none
 *
 * -test steps
 *    -#  set the local buffer
 *
 * -test verification
 *    -# verify the function returns zero
 *
 * @post
 *    NONE
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test004(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0x5, 0x0, 0x0, 0x63, 0x05, 0xFF, 0xFF};

	LOG_WARNING("Test %d Function CalculateCRC,     Case: InValid Parameter ",
		test_case_counter);

	/** function to test */
	ret = (CalculateCRC(&test_buffer[HDLC_ADDR_INDEX],
		*((fs3ipc_u16 *)&test_buffer[0])) == 0);

	/*test verification*/
	if (ret == FS3IPC_FALSE)
		ret = PASS;
	else
		ret = FAIL;

	return ret;
}

/*
 * @test:  Checks RX  EVENT_RX_ACK_UFRAME handling
 * @brief: verifies the condition when the message is received
 *
 * @pre:  none
 *
 * -test steps
 *    -#  set the local buffer
 *
 * -test verification
 *    -# verify if the event is activated
 *
 * @post
 *    clear the event set
 *
 * @return
 *    PASS
 *    FAIL
 */

static fs3ipc_s32 Test005(fs3ipc_u32 test_case_counter)
{
	fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];
	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	fs3ipc_s32 ret = FAIL;
	fs3ipc_StatusType ret_func;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0x7, 0x0, 0x0, 0x8f, 0x05, 0x02, 0x00, 0x95,
		0x48};

	LOG_WARNING("Test %d Function: processRxPacket Case Valid Parameter",
		total_test_cases);

	/** function to test */
	ret_func = fs3ipc_hdlc_processRxPacket(cfg, &test_buffer[HDLC_ADDR_INDEX],
		*((fs3ipc_u16 *)&test_buffer[0]), FS3IPC_INSTANCE_0);

	/*test verification*/
	if (GET_EVENT(pHdlc->qEventFlag, EVENT_RX_ACK_UFRAME) &&
		(ret_func == fs3ipc_StatusType_OK)) {
		ret = PASS;
	}

	/*post*/
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_ACK_UFRAME);

	return ret;
}

/*
 * @test:  Checks RX  invalid parameter
 * @brief: verifies the condition when the CRC received message is invalid
 *
 * @pre:  none
 *
 * -test steps
 *    -#  set the local buffer
 *
 * -test verification
 *    -# verify if the event is not activated
 *
 * @post
 *    clear the event set
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test006(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_StatusType ret_func;
	fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];
	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0x5, 0x0, 0x0, 0x63, 0x05, 0xEE, 0xEE};

	LOG_WARNING("Test %d Function: processRxPacket: Case checks for invalid U FRAME format",
		total_test_cases);

	/** function to test */
	ret_func = fs3ipc_hdlc_processRxPacket(cfg, &test_buffer[HDLC_ADDR_INDEX],
		*((fs3ipc_u16 *)&test_buffer[0]), FS3IPC_INSTANCE_0);

	/*test verification*/
	if (GET_EVENT(pHdlc->qEventFlag, EVENT_RX_ACK_UFRAME) == FS3IPC_FALSE &&
		(fs3ipc_StatusType_Rx_InvalidCRC == ret_func))
		ret = PASS;

	return ret;
}

/*
 * @test:  Checks RX  valid I frame with incomplete initialization
 * @brief: verifies the condition when valid IFrame is received
 *
 * @pre:  no initialization completed
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# verify if the event is not activated
 *
 * @post
 *    clear the event set
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test007(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_StatusType ret_func;
	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xb, 0x0, 0x0, 0x20, 0x6, 0x0, 0x3, 0x0, 0xd0,
		0x0, 0x1, 0x59, 0x58};

	LOG_WARNING("Test %d Function processRxPacket, case No init I FRAME Received",
		    total_test_cases);

	/** function to test */
	ret_func = fs3ipc_hdlc_processRxPacket(cfg, &test_buffer[HDLC_ADDR_INDEX],
					       *((fs3ipc_u16 *)&test_buffer[0]), FS3IPC_INSTANCE_0);

	/*test verification*/
	if (GET_EVENT(pHdlc->qEventFlag, EVENT_RX_IFRAME) == FS3IPC_FALSE &&
	    (ret_func == fs3ipc_StatusType_NOINIT)) {
		ret = PASS;
	}

	/**post*/
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_IFRAME);

	return ret;
}

/*
 * @test:  Checks RX  valid I frame with complete initialization
 * @brief: verifies the condition when valid IFrame is received
 *
 * @pre: set uInitialized completed
 *       set the expected reception value
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# verify if the event is  activated
 *    -# verify if the function return shows OK
 *
 * @post
 *    clear the event set
 *    stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test034(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_StatusType ret_func;
	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];

	fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xb, 0x0, 0x0, 0x20, 0x6, 0x0, 0x3, 0x0, 0xd0,
		0x0, 0x1, 0x59, 0x58};
	pHdlc->zGoBackN.rx_seq = 0;

	/*precondition*/
	pHdlc->uInitialized |= HDLC_INIT_UACK_TX;
	pHdlc->uInitialized |= HDLC_INIT_UACK_RX;

	/** function to test */
	ret_func = fs3ipc_hdlc_processRxPacket(cfg, &test_buffer[HDLC_ADDR_INDEX],
		*((fs3ipc_u16 *)&test_buffer[0]), FS3IPC_INSTANCE_0);

	/*test verification*/
	if (GET_EVENT(pHdlc->qEventFlag, EVENT_RX_IFRAME) &&
		 (ret_func == fs3ipc_StatusType_OK)) {
		ret = PASS;
	}

	/**post*/
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_IFRAME);
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test:  Checks for S frame Reception with incomplete initialization
 * @brief: verifies the condition when valid IFrame is received
 *
 * @pre:   incomplete initialization
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# verify if the return function shows no init condition
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test008(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_StatusType ret_func;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0x4, 0x0, 0x0, 0x01, 0x0d, 0x2e};

	LOG_WARNING("Test %d Function processRxPacket  Case S FRAME Received No init ",
		total_test_cases);

	/*precondition*/
	pHdlc->uInitialized = HDLC_INIT_NONE;

	/** function to test */
	ret_func = fs3ipc_hdlc_processRxPacket(cfg, &test_buffer[HDLC_ADDR_INDEX],
		*((fs3ipc_u16 *)&test_buffer[0]), FS3IPC_INSTANCE_0);

	/*test verification*/
	if (ret_func == fs3ipc_StatusType_NOINIT)
		ret = PASS;

	return ret;
}

/*
 * @test:  Checks Length encode function valid frame
 * @brief: verifies the length encode functional
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test009(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[255] = 0xFF;
	fs3ipc_u8 expected_result[] = {4, 0};

	LOG_WARNING("Test %d Function: hdlcEncodeLengthFrame, Case Valid Length 4 ");

	/** function to test */
	hdlcEncodeLengthFrame(&test_buffer[0], 4);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks hdlcEncodeLengthFrame valid frame
 * @brief: verifies the length encode functional with 1024bytes
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */

static fs3ipc_s32 Test010(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[255] = 0xFF;
	fs3ipc_u8 expected_result[] = {0x0, 0x04};

	LOG_WARNING("Test %d Function: hdlcEncodeLengthFrame, Case Valid Length 1024 encode ",
		total_test_cases);

	/** function to test */
	hdlcEncodeLengthFrame(&test_buffer[0], 1024);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: encodes CRC hdlcEncodeCRCFrame
 * @brief: verifies the CRC encode function
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test011(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xb, 0x0, 0x0, 0x20, 0x6, 0x0, 0x3, 0x0, 0xd0,
		0x0, 0x1, 0xFF, 0xFF};
	fs3ipc_u8 expected_result[] = {0xb, 0x0, 0x0, 0x20, 0x6, 0x0, 0x3, 0x0, 0xd0,
		0x0, 0x1, 0x59, 0x58};

	LOG_WARNING("Test %d Function: hdlcEncodeCRCFrame, Case Valid Parameter ",
		total_test_cases);

	/** function to test */
	hdlcEncodeCRCFrame(&test_buffer[HDLC_ADDR_INDEX], 9);

	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));

	/*test verification*/
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks for hdlcCreateRRAckFrame valid parameter
 * @brief: checks for hdlcCreateRRAckFrame valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test012(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	fs3ipc_u8 expected_result[] = {0xFF, 0xFF, 0x00, 0x21, 0xFF, 0xFF};

	LOG_WARNING("Test %d Function hdlcCreateRRAckFrame, Case Valid PaArameter",
		total_test_cases);

	/** function to test */
	hdlcCreateRRAckFrame(1, &test_buffer[HDLC_ADDR_INDEX]);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks for hdlcCreateRRPollFrame valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test013(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	fs3ipc_u8 expected_result[] = {0xFF, 0xFF, 0x40, 0x31, 0xFF, 0xFF};

	LOG_WARNING("Test %d Function hdlcCreateRRPollFrame, Case Valid  ",
		total_test_cases);

	/** function to test */
	hdlcCreateRRPollFrame(1, &test_buffer[HDLC_ADDR_INDEX]);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks for hdlcCreateRRFinalFrame valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test014(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	fs3ipc_u8 expected_result[] = {0xFF, 0xFF, 0x00, 0x31, 0xFF, 0xFF};

	LOG_WARNING("Test %d Function hdlcCreateRRFinalFrame", total_test_cases);

	/** function to test */
	hdlcCreateRRFinalFrame(1, &test_buffer[HDLC_ADDR_INDEX]);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks for hdlcCreateREJFrame valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test015(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	fs3ipc_u8 expected_result[] = {0xFF, 0xFF, 0x00, 0x25, 0xFF, 0xFF};

	LOG_WARNING("Test %d Function hdlcCreateREJFrame ", total_test_cases);

	/** function to test */
	hdlcCreateREJFrame(1, &test_buffer[HDLC_ADDR_INDEX]);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks for hdlcCreateUAckFrame valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test016(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	fs3ipc_u8 expected_result[] = {0xFF, 0xFF, 0x00, 0x63, 0x04, 0x02, 0x00};

	LOG_WARNING("Test %d Function hdlcCreateUAckFrame ", total_test_cases);

	/** function to test */
	hdlcCreateUAckFrame(&test_buffer[HDLC_ADDR_INDEX], 4, 512);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks for hdlcCreateURstFrame valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test017(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_s32 stat;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	fs3ipc_u8 expected_result[] = {0xFF, 0xFF, 0x00, 0x8f, 0x05, 0x02, 0x00};

	LOG_WARNING("Test %d Function hdlcCreateURstFrame ", total_test_cases);

	/** function to test */
	hdlcCreateURstFrame(&test_buffer[HDLC_ADDR_INDEX], 5, 512);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks for hdlcCreateIframe valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    none
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test018(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_u8 index = 0;

	/** test steps*/
	fs3ipc_u8 test_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	fs3ipc_u8 expected_result[] = {0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF};

	LOG_WARNING("Test %d Function hdlcCreateIframe", total_test_cases);

	for (index = 0; ((index < 5) && (ret == 0)); index++) {
		/** function to test */
		hdlcCreateIframe(index, 0, &test_buffer[HDLC_ADDR_INDEX]);

		expected_result[HDLC_CTRL_INDEX] = (index << 1);
		/*test verification*/
		ret = fs3ipc_memcmp(expected_result, test_buffer,
			sizeof(expected_result));
	}

	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	return ret;
}

/*
 * @test: checks for HDLC_Handle_EVENT_TX_TIMEOUT is encoding  valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: set ack and tx counter indexes
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *
 * @post
 *    -# stop timer
 *    -# clean transmission indexes parameters
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test019(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = PASS;
	fs3ipc_length length;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	/** test steps*/
	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX];
	fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x40, 0xF1,
		0xEF, 0xFD};

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_TX_TIMEOUT ",
		total_test_cases);

	/** precondition */
	pHdlc->zGoBackN.ack_exp = 0;
	pHdlc->zGoBackN.tx_seq = 4;

	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	/** function to test */
	HDLC_Handle_EVENT_TX_TIMEOUT(FS3IPC_INSTANCE_0, &test_buffer, &length);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0) || (length != HDLC_TOTAL_FRAME))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	pHdlc->zGoBackN.ack_exp = 0;
	pHdlc->zGoBackN.tx_seq = 0;
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: checks for HDLC_Handle_EVENT_RX_RR_POLL_FRAME is encoding valid params
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *    -# clean transmission indexes parameters
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test020(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = PASS;
	fs3ipc_length length;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];
	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX];

	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_RX_RR_POLL_FRAME",
		total_test_cases);

	/** test steps*/
	for (pHdlc->zGoBackN.rx_seq = 0; (pHdlc->zGoBackN.rx_seq < WINDOW_SIZE) &&
		(ret = !0); pHdlc->zGoBackN.rx_seq++) {
		/** function to test */
		HDLC_Handle_EVENT_RX_RR_POLL_FRAME(0, &test_buffer, &length);
		switch (pHdlc->zGoBackN.rx_seq) {
		case 0:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x11, 0x1f, 0x1f};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;

		case 1:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x31, 0x3b, 0x7d};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;

		case 2:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x51, 0x57, 0xdb};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;

		case 3:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x71, 0x73, 0xb9};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;

		case 4:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[512] = {0x04, 0x00, 0x00, 0x91, 0x8e, 0x97};

			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;
		}
	}

	if ((ret != 0) || (length != HDLC_TOTAL_FRAME))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	pHdlc->zGoBackN.rx_seq = 0;
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: checks for HDLC_Handle_EVENT_RX_IFRAME is encoding  valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *    -# clean transmission indexes parameters
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test021(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;
	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX];

	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_RX_IFRAME ",
		total_test_cases);

	/** test steps*/
	for (pHdlc->zGoBackN.rx_seq = 0; (pHdlc->zGoBackN.rx_seq < WINDOW_SIZE);
		pHdlc->zGoBackN.rx_seq++) {
		/** function to test */
		HDLC_Handle_EVENT_RX_IFRAME(0, &test_buffer, &length);
		switch (pHdlc->zGoBackN.rx_seq) {
		case 0:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x01, 0x0d, 0x2e};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;

		case 1:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x21, 0x29, 0x4c};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;

		case 2:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x41, 0x45, 0xea};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;

		case 3:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x61, 0x61, 0x88};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;

		case 4:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x04, 0x00, 0x00,
				0x81, 0x9c, 0xa6};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
		}
		break;
		}
	}

	/*test verification*/
	if ((ret != 0) || (length != HDLC_TOTAL_FRAME))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	pHdlc->zGoBackN.rx_seq = 0;
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: checks for HDLC_Handle_EVENT_RX_RST_UFRAME is encoding valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *    -# clean transmission indexes parameters
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test022(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;
	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX];

	/** test steps*/
	fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x07, 0x00, 0x00, 0x8f,
		0x05, 0x02, 0x00, 0x95, 0x48};
	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_RX_RST_UFRAME ",
		total_test_cases);

	/** function to test */
	HDLC_Handle_EVENT_RX_RST_UFRAME(FS3IPC_INSTANCE_0, &test_buffer,
		&length);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0) || (length != HDLC_TOTAL_HEADER + HDLC_UFRAME_PAYLOAD_LEN))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	stop_uframe_timer(cfg);

	return ret;
}

/*
 * @test: checks for HDLC_Handle_EVENT_RX_ACK_UFRAME is encoding valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: none
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test023(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX];

	/** test steps*/
	fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x07, 0x00, 0x00, 0x63,
		0x05, 0x02, 0x00, 0x5e, 0x90};
	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_RX_ACK_UFRAME ",
		total_test_cases);

	/** function to test */
	HDLC_Handle_EVENT_RX_ACK_UFRAME(FS3IPC_INSTANCE_0, &test_buffer,
		&length);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0) || (length != HDLC_TOTAL_HEADER + HDLC_UFRAME_PAYLOAD_LEN))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	stop_uframe_timer(cfg);

	return ret;
}

/*
 * @test: checks for HDLC_Handle_EVENT_TX_MSG is encoding  valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: disable transmission
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test024(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX];
	fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0};

	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_TX_MSG ", total_test_cases);

	/* test with disabled transmission */
	/* pHdlc->uTxEnable = FS3IPC_FALSE; */

	/** function to test */
	HDLC_Handle_EVENT_TX_MSG(FS3IPC_INSTANCE_0, &test_buffer, &length);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0) || (length != 0))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	stop_iframe_timer(cfg);

	return ret;
}

static fs3ipc_u8 fs3ipc_test_buffeFull;
/*
 * @test: checks for HDLC_Handle_EVENT_TX_MSG is encoding  valid parameters
 * @brief: checks for encoding valid parameter
 *
 * @pre: enable transmission
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test025(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX];
	fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0};

	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_TX_MSG Full Buffer case ",
		total_test_cases);

	/* test with disabled transmission */
	/* pHdlc->uTxEnable = FS3IPC_TRUE; */
	/*induce full buffer case*/
	fs3ipc_test_buffeFull = FS3IPC_TRUE;

	/** function to test */
	HDLC_Handle_EVENT_TX_MSG(FS3IPC_INSTANCE_0, &test_buffer, &length);

	/*test verification*/
	ret = fs3ipc_memcmp(expected_result, test_buffer, sizeof(expected_result));
	if ((ret != 0) || (length != 0))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	fs3ipc_test_buffeFull = FS3IPC_FALSE;
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: test normal transmission and window full
 * @brief: checks for encoding valid parameter
 *
 * @pre: enable transmission
		  disable fs3ipc_test_buffeFull
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test026(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;
	fs3ipc_StatusType fs3ipc_return;
	fs3ipc_u8 i;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0};
	/** init */
	fs3ipc_memset(&pHdlc->zGoBackN, 0, sizeof(pHdlc->zGoBackN));

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_TX_MSG  ", total_test_cases);

	/* test with disabled transmission */
	/* pHdlc->uTxEnable = FS3IPC_TRUE; */
	fs3ipc_test_buffeFull = FS3IPC_FALSE;

	/** test steps*/
	for (i = 0; (i < WINDOW_SIZE) && (ret = !0); i++) {
		/** function to test */
		HDLC_Handle_EVENT_TX_MSG(FS3IPC_INSTANCE_0, &pHdlc->zTxQueue[
			pHdlc->zGoBackN.tx_seq].uData[0],
			&length);

		switch (i) {
		case 0:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0e, 0x00, 0x00,
				0xe0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x15, 0x4d};
			ret = fs3ipc_memcmp(expected_result, pHdlc->zTxQueue[i].uData,
				sizeof(expected_result));
			break;
		}

		case 1:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0e, 0x00, 0x00,
				0xe2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0xbe, 0xc};
			ret = fs3ipc_memcmp(expected_result, pHdlc->zTxQueue[i].uData,
				sizeof(expected_result));
			break;
		}

		case 2:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0e, 0x00, 0x00,
				0xe4, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0xb9, 0x49};
			ret = fs3ipc_memcmp(expected_result, pHdlc->zTxQueue[i].uData,
				sizeof(expected_result));
			break;
		}

		case 3:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0e, 0x00, 0x00,
				0xe6, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0xf8, 0xaf};
			ret = fs3ipc_memcmp(expected_result, pHdlc->zTxQueue[i].uData,
				sizeof(expected_result));
			break;
		}

		case 4:
		{
			/*test verification*/
			/*force full window case*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0};

			ret = fs3ipc_memcmp(expected_result, pHdlc->zTxQueue[i].uData,
				sizeof(expected_result));
			break;
		}
		}
	}

	/*test verification*/
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: test normal transmission and window full
 * @brief: checks for encoding valid parameter
 *
 * @pre: enable transmission disable fs3ipc_test_buffeFull
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test027(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;
	fs3ipc_u8 i;
	fs3ipc_StatusType fs3ipc_return;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0};
	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX] = {0};

	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	LOG_WARNING("Test %d Function HDLC_Handle_EVENT_TX_Retransmission",
		total_test_cases);

	/* precondition */
	/* pHdlc->uTxEnable = FS3IPC_TRUE; */
	fs3ipc_test_buffeFull = FS3IPC_FALSE;

	/** test steps*/
	for (i = 0; (i < WINDOW_SIZE) && (ret = !0); i++) {
		/** function to test */
		HDLC_Handle_EVENT_TX_Retransmission(FS3IPC_INSTANCE_0, &test_buffer,
			&length);

		switch (i) {
		case 0:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0e, 0x00, 0x00,
				0xe0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x15, 0x4d};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
			break;
		}

		case 1:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0e, 0x00, 0x00,
				0xe2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0xbe, 0xc};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
			break;
		}

		case 2:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0e, 0x00, 0x00,
				0xe4, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0xb9, 0x49};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
			break;
		}

		case 3:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0x0e, 0x00, 0x00,
				0xe6, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0xf8, 0xaf};
			ret = fs3ipc_memcmp(expected_result, test_buffer,
				sizeof(expected_result));
			break;
		}

		case 4:
		{
			/*test verification*/
			fs3ipc_u8 expected_result[FS3IPC_TRANS_SIZE_MAX] = {0};

			ret = fs3ipc_memcmp(expected_result, pHdlc->zTxQueue[i].uData,
				sizeof(expected_result));
			break;
		}
		}
	}

	/*test verification*/
	if ((ret != 0))
		ret = FAIL;
	else
		ret = PASS;

	/**post*/
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: test fs3ipcSetITimerMode function
 * @brief: there is no transmitted frames condition
 *
 * @pre: enable transmission disable fs3ipc_test_buffeFull
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set transmission indexes
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test028(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;
	fs3ipc_StatusType fs3ipc_return;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX] = {0x0, 0x0};

	fs3ipc_memset(&test_buffer, 0x00, sizeof(test_buffer));

	LOG_WARNING("Test %d Function fs3ipcSetITimerMode  ", total_test_cases);

	/*precondition*/
	/*pHdlc->uTxEnable = FS3IPC_TRUE; */
	fs3ipc_test_buffeFull = FS3IPC_FALSE;
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_OUT_OF_SEQ_FRAME);

	/*test -  no frames transmitted*/
	pHdlc->zGoBackN.ack_exp = 0;
	pHdlc->zGoBackN.tx_seq = 0;

	/** function to test */
	fs3ipcSetITimerMode(FS3IPC_INSTANCE_0, test_buffer);

	/*verification */
	if (pHdlc->zGoBackN.ack_exp == pHdlc->zGoBackN.tx_seq)
		ret = PASS;

	/*post */
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: test fs3ipcSetITimerMode function
 * @brief: test the case when only duplicated frame is received
 *
 * @pre: enable transmission
		  disable fs3ipc_test_buffeFull
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test029(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];

	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX] = {0x0, 0x0};

	LOG_WARNING("Test %d Function fs3ipcSetITimerMode", total_test_cases);

	/*initial conditions*/
	/*pHdlc->uTxEnable = FS3IPC_TRUE;*/
	fs3ipc_test_buffeFull = FS3IPC_FALSE;
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_OUT_OF_SEQ_FRAME);

	/*test -  frame duplicated*/
	pHdlc->zGoBackN.ack_exp = 0;
	pHdlc->zGoBackN.tx_seq = 1;

	/** function to test */
	fs3ipcSetITimerMode(FS3IPC_INSTANCE_0, test_buffer);

	/*verification */
	if (pHdlc->zGoBackN.ack_exp != pHdlc->zGoBackN.tx_seq)
		ret = PASS;

	/**post*/
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: test fs3ipcSetITimerMode function
 * @brief: test the case when only duplicated frame is received
 *
 * @pre: enable transmission
		  disable fs3ipc_test_buffeFull
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test030(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_length length;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];
	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX] = {0x0, 0x20};

	LOG_WARNING("Test %d Function fs3ipcSetITimerMode  ", total_test_cases);

	/*initial conditions*/
	/*pHdlc->uTxEnable = FS3IPC_TRUE;*/
	fs3ipc_test_buffeFull = FS3IPC_FALSE;
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_OUT_OF_SEQ_FRAME);

	/*test -  Piggy Ack: Invalid Nr, already acked*/
	pHdlc->zGoBackN.ack_exp = 0;
	pHdlc->zGoBackN.tx_seq = 1;

	/** function to test */
	fs3ipcSetITimerMode(FS3IPC_INSTANCE_0, test_buffer);

	/*verification */
	if (pHdlc->zGoBackN.ack_exp == pHdlc->zGoBackN.tx_seq)
		ret = PASS;

	/**post*/
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: test fs3ipcCatchInvalidIFrame function
 * @brief: this test case probes the error handling if it receives
 * out of sequence error.
 *
 * @pre: enable transmission
		  -# disable fs3ipc_test_buffeFull
		  -# enable transmission
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  clear the expected event
 *    -#  set the transmission parameters
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test031(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];
	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX] = {0x00, 0x22};

	LOG_WARNING("Test %d Function fs3ipcCatchInvalidIFrame  ", total_test_cases);

	/*initial conditions*/
	/*pHdlc->uTxEnable = FS3IPC_TRUE;*/
	fs3ipc_test_buffeFull = FS3IPC_FALSE;
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_OUT_OF_SEQ_FRAME);
	pHdlc->zGoBackN.rejAlreadySent = FS3IPC_FALSE;

	/** test steps -  Piggy Ack: out of sequence error*/
	pHdlc->zGoBackN.ack_exp = 0;
	pHdlc->zGoBackN.tx_seq = 3;
	pHdlc->zGoBackN.rx_seq = 0;

	/** function to test */
	ret = fs3ipcCatchInvalidIFrame(FS3IPC_INSTANCE_0, test_buffer);

	/*test verification*/
	if ((ret == FS3IPC_FALSE) && (pHdlc->zGoBackN.rejAlreadySent == FS3IPC_TRUE)
		&& GET_EVENT(pHdlc->qEventFlag, EVENT_RX_OUT_OF_SEQ_FRAME))
		ret = PASS;

	/**post*/
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: test fs3ipcCatchInvalidIFrame function
 * @brief: this test case probes the error handling if it receives
 *valid frame.
 *
 * @pre: enable transmission
		  -# disable fs3ipc_test_buffeFull
		  -# enable transmission
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  clear the expected event
 *    -#  set the transmission parameters
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test032(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_StatusType fs3ipc_return;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];
	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX] = {0x00, 0x22};

	LOG_WARNING("Test %d Function fs3ipcCatchInvalidIFrame  ", total_test_cases);

	/*initial conditions*/
	/*pHdlc->uTxEnable = FS3IPC_TRUE;*/
	fs3ipc_test_buffeFull = FS3IPC_FALSE;
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_OUT_OF_SEQ_FRAME);
	pHdlc->zGoBackN.rejAlreadySent = FS3IPC_FALSE;

	/** test steps - Piggy Ack: out of sequence error*/
	pHdlc->zGoBackN.ack_exp = 0;
	pHdlc->zGoBackN.tx_seq = 3;
	pHdlc->zGoBackN.rx_seq = 1;

	/** function to test */
	fs3ipc_return = fs3ipcCatchInvalidIFrame(FS3IPC_INSTANCE_0, test_buffer);

	/*test verification*/
	if ((fs3ipc_return == FS3IPC_TRUE) && (pHdlc->zGoBackN.rejAlreadySent ==
		FS3IPC_FALSE))
		ret = PASS;

	/**post*/
	stop_iframe_timer(cfg);

	return ret;
}

/*
 * @test: test fs3ipcCatchInvalidIFrame function
 * @brief: this test case probes the error handling if it receives
 *valid frame.
 *
 * @pre: enable transmission
		  -# disable fs3ipc_test_buffeFull
		  -# enable transmission
 *
 * -test steps
 *    -#  set the local buffer
 *    -#  set expected value buffer
 *    -#  clear the expected event
 *    -#  set the transmission parameters
 *    -#  call the function to test
 *
 * -test verification
 *    -# compare the expected value with the decoded value
 *    -# test all the possible indexes value through switch test case
 *
 * @post
 *    -# stop timer
 *
 * @return
 *    PASS
 *    FAIL
 */
static fs3ipc_s32 Test033(fs3ipc_u32 test_case_counter)
{
	fs3ipc_s32 ret = FAIL;
	fs3ipc_StatusType fs3ipc_return;

	THdlcManager *pHdlc = &pHdlcManager[FS3IPC_INSTANCE_0];
	const fs3ipc_hdlc_AppConfigType *cfg =
		&fs3ipc_hdlc_appConfigs[FS3IPC_INSTANCE_0];
	fs3ipc_u8 test_buffer[FS3IPC_TRANS_SIZE_MAX] = {0x00, 0x20};

	LOG_WARNING("Test %d Function fs3ipcCatchInvalidIFrame  ", total_test_cases);

	/*initial conditions*/
	/*pHdlc->uTxEnable = FS3IPC_TRUE;*/
	fs3ipc_test_buffeFull = FS3IPC_FALSE;
	CLEAR_EVENT(pHdlc->qEventFlag, EVENT_RX_OUT_OF_SEQ_FRAME);
	pHdlc->zGoBackN.rejAlreadySent = FS3IPC_FALSE;

	/** test steps  -  Piggy Ack: out of sequence error*/
	pHdlc->zGoBackN.ack_exp = 0;
	pHdlc->zGoBackN.tx_seq = 3;
	pHdlc->zGoBackN.rx_seq = 1;

	/** function to test */
	fs3ipc_return = fs3ipcCatchInvalidIFrame(FS3IPC_INSTANCE_0, test_buffer);

	/*test verification*/
	if ((fs3ipc_return == FS3IPC_FALSE) && (pHdlc->zGoBackN.rejAlreadySent ==
		FS3IPC_FALSE))
		ret = PASS;

	/**post*/
	stop_iframe_timer(cfg);

	return ret;
}
void Spi_RunUnitTest(void)
{
	fs3ipc_s32 stat;
	fs3ipc_u32 total_test_cases = 1;
	fs3ipc_u32 total_pass = 1;
	fs3ipc_u32 total_fail = 0;

	stat = Test001(total_test_cases);
	LOG_ERROR("Test %d, Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test002(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test003(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test004(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test005(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test006(total_test_cases);
	LOG_ERROR("Test %d Result [%s] ", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test007(total_test_cases);
	LOG_ERROR("Test %d  Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test008(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test009(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	/*SetRelAlarm (Al_CtCddSPI_IPC_HDLC_TESTING, 1000, 0);
	 *WaitEvent(Ev_FS3IPC_TEST_HDLC);
	 *ClearEvent(Ev_FS3IPC_TEST_HDLC);
	 */

	stat = Test010(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test011(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test012(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test013(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test014(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test015(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test016(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test017(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test018(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test019(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test020(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test021(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test022(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test023(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test024(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test025(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	stat == PASS ? total_pass++ : total_fail++;
	total_test_cases++;

	stat = Test026(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	/*SetRelAlarm (Al_CtCddSPI_IPC_HDLC_TESTING, 1000, 0);
	 *WaitEvent(Ev_FS3IPC_TEST_HDLC);
	 *ClearEvent(Ev_FS3IPC_TEST_HDLC);
	 */

	stat = Test027(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test028(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test029(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
	 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test030(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test031(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test032(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
	 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test033(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	stat = Test034(total_test_cases);
	LOG_ERROR("Test %d Result [%s]", total_test_cases, stat == PASS ? "\033[32m Pass \033[0m"
		 : "\033[31m Fail \033[0m");
	total_test_cases++;
	stat == PASS ? total_pass++ : total_fail++;

	LOG_ERROR(" \033[33m Total Test Cases =[%d]  Pass Result [%d] Fail Result[%d] \033[0m",
		total_test_cases, total_pass, total_fail);
}

/*
 * this stub function is used for emulating higher layer
 * this function is not created for production software
 */
static fs3ipc_StatusType fs3ipc_test_app_Encoder(uint8_t fs3ipc_instance,
						 fs3ipc_dataPtr data,
						 fs3ipc_length *ptrLength)
{
	fs3ipc_StatusType stat = fs3ipc_StatusType_OK;

	fs3ipc_dataPtr pData = data;

	if (fs3ipc_instance >= FS3IPC_APP_NUM_OF_INSTANCES) {
		/* This shouldn't happen. Config issue*/
		stat = fs3ipc_StatusType_ErrorHandle;
		LOG_ERROR("Invalid FS3IPC instance: %u; cfg issue",
			  (unsigned int)fs3ipc_instance);
	} else if (!ptrLength) {
		stat = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("null length ptr arg");
	} else if (!data) {
		stat = fs3ipc_StatusType_ErrorGen;
		LOG_ERROR("null data ptr arg");
	} else if (fs3ipc_test_buffeFull == FS3IPC_TRUE) {
		stat = fs3ipc_StatusType_BufferFull;
		*ptrLength = 0;
	} else {
		static fs3ipc_u8 data_value;

		data_value++;

		stat = fs3ipc_StatusType_OK;

		fs3ipc_memset(pData, data_value, 10);

		*ptrLength = 10;
	}
	return stat;
}
#endif
