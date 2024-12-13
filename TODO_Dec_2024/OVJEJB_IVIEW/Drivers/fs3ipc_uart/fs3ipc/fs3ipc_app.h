/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         fs3ipc_app.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * FS3IPC App layer header file.
 *****************************************************************************/
#ifndef FS3IPC_APP_H
#define FS3IPC_APP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "fs3ipc_cfg.h"
#include "fs3ipc_flowControl.h"

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
/**
 * Defines the invalid parameter for Application layer
 */
#define FS3IPC_APP_HANDLE_INVALID (255)

/**
 * Defines the FlowControl channel number
 */
#define FS3IPC_APP_FLOWCONTROL_CHAN (1)

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
#ifdef CONFIG_FS3IPC_DEBUG

/**
 * structure dedicated for collecting statics values
 */
typedef struct fs3ipc_app_StatsT {
	fs3ipc_u32 rxWrappedMsgCnt; /**< Total Wrapped Receiver messages */
	fs3ipc_u32 txWrappedMsgCnt; /**< Total Wrapped Transmitted messages*/

	fs3ipc_u32 rxWrappedMsgErrorCnt;			/**< Total Wrapped with error messages*/
	fs3ipc_u32 rxMsgLengthErrorCnt;			/**< Total length error counter */
	fs3ipc_u32 rxUnhandledChannelErrorCnt; /**< Total invalid channel counter */
	fs3ipc_u32 rxGenErrorCnt;					/**< Total dropped error due to configuration error*/
	fs3ipc_u32 rxBufferOverflow;				/**< Total buffer receiver overflow error detected*/
	fs3ipc_u32 txGenErrorCnt;					/**< Total buffer transmitted errors */
} fs3ipc_app_stats_t;
#endif

/**
 * This structure collects the client information
 */
struct fs3ipc_client_ltcfg {
	uint8_t handle;				/**< the client ID information >**/
	fs3ipc_os_event_t *osCfg;		/**< OS configuration to handle events/flags >**/
	fs3ipc_os_EventMaskType rxEvent; /**< The expected rx events related to this channel >**/
	fs3ipc_os_EventMaskType txevent; /**< The expected tx events related to this channel >**/
};

/*
 * This structure collects the Application Channel information
 */
typedef struct Tagfs3ipc_app_LTChannelCfgType {
	fs3ipc_u8 *TX_Buffer;		/**< Pointer to access the Tx buffer >**/
	fs3ipc_u8 *RX_Buffer;		/**< Pointer to access the Rx buffer >**/
	fs3ipc_u8 priority;			/**< the corresponding priority for this channel>**/
	fs3ipc_u8 channel;			/**< The client identifier  >**/
	fs3ipc_u16 TX_Buffer_Size; /**< Configuration value to determine the buffer size >**/
	fs3ipc_u16 RX_Buffer_Size; /**< Configuration value to determine the buffer size>**/

	fs3ipc_app_queueType *TX_Q;		 /**< OS resource dedicated for  transmission  >**/
	fs3ipc_os_resource_t *TX_QLock; /**< OS lock for transmission >**/
	fs3ipc_app_queueType *RX_Q;		 /**< OS resource dedicated for  reception >**/
	fs3ipc_os_resource_t *RX_QLock; /**< OS lock for reception >**/

	struct fs3ipc_client_ltcfg client_cfg; /**< client - flow control related >**/
	fs3ipc_u8 *loggingEnabled;
} fs3ipc_app_ltchancfg_t;

/**
 * struct fs3ipc_client - FS3IPC client structure.
 * @instance: FS3IPC instance.
 * @ch: Channel.
 * @registered: Registration state.
 */
struct fs3ipc_client {
	uint8_t instance;
	uint8_t ch;
	uint8_t registered;
};

#pragma pack(1)
/**
 * the fs3ipc_app_messageHdr provides the application header, it determines
 * the length and channel.
 */
typedef struct {
	/**< the channel Length associated to this channel  >**/
	fs3ipc_u16 length : 11;
	/**< Related Channel ID >**/
	fs3ipc_u16 channel : 5;
} fs3ipc_app_messageHdr;

/**
 * structure dedicated to configure the message header and the buffer data
 */
typedef struct {
	/**< Application header, provides length and channel parameters >**/
	fs3ipc_app_messageHdr hdr;
	/**<  buffer dedicated to send data >**/
	fs3ipc_u8 data[FS3_IPC_MAX_MSG_SIZE];
} fs3ipc_app_messageType;
#pragma pack()

/**
 * handles the application message information for transmitting  and receiving
 * data
 */
typedef struct {
	/**< Message header and data information >**/
	fs3ipc_app_messageType message;
	/**< provides the address position >**/
	fs3ipc_u16 position;
} fs3ipc_app_message_buffer_t;

/**
 * Structure dedicated to configure all the application layer parameters
 * this structure shall be initialized at the fs3ipc_cfg.h
 */
typedef struct fs3ipc_app_ltconfig_s {
	/**<all channel/client configuration  */
	const fs3ipc_app_ltchancfg_t *ltChannelCfg[FS3IPC_MAX_NUMBER_CHANNELS];
	/**< this parameter counts the channel operation*/
	fs3ipc_u8 channelCount;

	/**< channel priority configuration parameters */
	fs3ipc_app_priorityNodeType *orderedChannelListHead;
	fs3ipc_app_priorityNodeType *orderedChannelNodes;

	/**< */
	fs3ipc_os_event_t *phys_txEventOsCfg;

	/**< bitmask associated with client in order to transmit data*/
	fs3ipc_os_EventMaskType phys_txEvent;

	/**<flowcontrol configuration parameters >*/
	const fs3ipc_fc_ltcfg_t *flowControlCfg;

	/**<reception buffer configuration message */
	fs3ipc_app_message_buffer_t *rxMsg;

	/**< Transmission buffer configuration */
	fs3ipc_app_message_buffer_t *txMsg;

	fs3ipc_length *rxMsgBufferOverflowIndex;
#ifdef CONFIG_FS3IPC_DEBUG
	fs3ipc_app_stats_t *stats;
#endif

} fs3ipc_app_ltcfg_t;

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
extern struct fs3ipc_client fs3ipc_client_lookup_table[FS3IPC_CLIENT_HANDLE_COUNT];

extern const fs3ipc_app_ltcfg_t fs3ipc_app_appConfigs[FS3IPC_APP_NUM_OF_INSTANCES];

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
fs3ipc_StatusType fs3ipc_app_Encoder(
	fs3ipc_handleType handle,
	fs3ipc_dataPtr data,
	fs3ipc_length *ptrLength);

fs3ipc_StatusType fs3ipc_app_Decoder(
	fs3ipc_handleType handle,
	fs3ipc_cDataPtr data,
	fs3ipc_length length);

fs3ipc_StatusType fs3ipc_app_HandleExtNodeReset(
	fs3ipc_handleType handle);

/**
 * fs3ipc_get_client() - Get channel client from client handle.
 * @handle: Client handle.
 * @instance: Return FS3IPC instance of the client.
 * @ch: Return channel no. of client.
 *
 * If @instance & @ch are not NULL, then they are updated with FS3IPC instance
 * and channel of the client respectively. They will hold valid data only if
 * valid channel client is returned.
 *
 * Return: Channel client if valid client handle otherwise NULL.
 */
const struct fs3ipc_client *fs3ipc_get_client(unsigned int handle,
					      unsigned int *instance,
					      unsigned int *ch);

/**
 * fs3ipc_app_get_client_cfg() - Get client cfg from client.
 * @client: FS3IPC client.
 *
 * It is assumed that the argument is valid.
 *
 * Return: Client cfg.
 */
const struct fs3ipc_client_ltcfg *
fs3ipc_app_get_client_cfg(const struct fs3ipc_client *client);

/**
 * fs3ipc_app_write_msg() - Write message to FS3IPC channel client.
 * @client: FS3IPC client.
 * @msg: Message to write.
 * @length: Length of message in bytes.
 *
 * It is assumed that @client, @msg are not NULL and @msg is of at least @length
 * byte sized.
 *
 * Return:
 * * fs3ipc_StatusType_OK                  - OK.
 * * fs3ipc_StatusType_ChannelNotAvailable - Channel not available.
 * * fs3ipc_StatusType_MessageSize         - Invalid message size.
 * * fs3ipc_StatusType_BufferFull          - Tx-Q full.
 * * fs3ipc_StatusType_ErrorGen            - Invalid channel or FC config or
 *                                           Tx-Q error.
 */
fs3ipc_StatusType fs3ipc_app_write_msg(const struct fs3ipc_client *client,
				       const uint8_t *msg, uint32_t length);

/**
 * fs3ipc_app_read_msg() - Read message from FS3IPC channel client.
 * @client: FS3IPC client.
 * @msg: Buffer to read message.
 * @length: Length of message in bytes.
 *
 * It is assumed that @client, @msg are not NULL and @msg is of at least @length
 * byte sized.
 *
 * Return:
 * * fs3ipc_StatusType_OK                - OK.
 * * fs3ipc_StatusType_BufferEmpty       - Rx-Q empty.
 * * fs3ipc_StatusType_ErrorGen          - Rx-Q error.
 */
fs3ipc_StatusType fs3ipc_app_read_msg(const struct fs3ipc_client *client,
				      uint8_t *msg, uint32_t *length);

/**
 * fs3ipc_app_close_ch() - Close FS3IPC channel client.
 * @client: FS3IPC client.
 *
 * It is assumed that @client is not NULL.
 *
 * Return:
 * * fs3ipc_StatusType_OK               - OK.
 * * fs3ipc_StatusType_ErrorGen         - Invalid FC config or channel.
 * * fs3ipc_StatusType_ErrorClientState - Client is already closed.
 */
fs3ipc_StatusType fs3ipc_app_close_ch(const struct fs3ipc_client *client);

#ifdef CONFIG_FS3IPC_DEBUG
const fs3ipc_app_stats_t *fs3ipc_app_GetStats(
	fs3ipc_handleType Hndl);

fs3ipc_StatusType fs3ipc_app_ClearStats(
	fs3ipc_handleType Hndl);

const fs3ipc_fc_chan_pbcfg_t *fs3ipc_app_GetChanStats(
	fs3ipc_handleType Hndl,
	fs3ipc_u8 channel);

fs3ipc_StatusType fs3ipc_app_ClearChanStats(
	fs3ipc_handleType Hndl,
	fs3ipc_u8 channel);
#endif

fs3ipc_StatusType fs3ipc_app_SetLogging(
	fs3ipc_handleType Hndl,
	fs3ipc_u8 channel,
	fs3ipc_u8 enabled);

#ifdef __cplusplus
}
#endif

#endif /* FS3IPC_APP_H */
