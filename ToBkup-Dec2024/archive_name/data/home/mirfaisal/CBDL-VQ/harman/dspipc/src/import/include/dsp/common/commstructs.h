/**************************************************************
 * Project        Harman Car Multimedia System
 * (c) Copyright  2005
 * Company        Harman/Becker Automotive Systems GmbH
 *             All rights reserved
 **************************************************************/
/**
 * @file       commstructs.h
 *
 * This file declares the communication structures in the DSP that
 * are used to establish a IPC mechanism over PCI. 
 * 
 * @date       06.12.2007
 * @author     Markus Schweikhardt
 */
#ifndef _COMMSTRUCTS_
#define _COMMSTRUCTS_

#ifdef __cplusplus
extern "C" {
#endif

#include "DSPIPCTypes.h"

/**
 * @brief Command message payload size
 * 
 * This value defines the size of the payload that can be delivered by a single
 * communication message. This value gives the number of 8 bit values. 
 */
#define COMMANDMSG_SIZE (256)

/**
 * @brief Channel type mask
 *
 * While in the chennelID the lower 30 bits determine the channel number, the upper
 * two bits determine the type of the channel. 
 * This mask can be used to strip the channel type value from a 32 bit value.
 */
#define DSPIPC_CHANNELTYPE_MSK (0xc0000000)

/**
 * @brief Message id channeltype for stream data
 *
 * This value for the channel type indicates that the data beeing transferred is
 * stream data.
 */
#define DSPIPC_CHANNELTYPE_STREAMDATA (0x00000000U)

/**
 * @brief Message id channeltype internal
 *
 * This is the value for the messages for internal communication. This
 * is mainly used for administration use
 */
#define DSPIPC_CHANNELTYPE_INTERNAL (0x40000000U)

/**
 * @brief Message id channeltype command
 *
 * This is the value for the messages for command communication. This
 * is used for sending requests from/to the client via MMIPC lib.
 */
#define DSPIPC_CHANNELTYPE_COMMAND (0x80000000U)

/**
 * @brief Message id channeltype stream
 *
 * This is the value for the messages for stream packet communication.
 * This is used for sending data blocks from/to the client via MMIPC lib.
 */
#define DSPIPC_CHANNELTYPE_STREAM (0xC0000000U)

/**
 * @page MsgId Message id
 *
 * The message id send with each message consists of two parts: The message counter and
 * the mesage direction. Both together, they build a unique message id (that is unique only
 * for the first 2^32 messages). 
 * 
 * The following pictures shows the layout of the message id. 
 *
 * @image html ChannelNum_MsgId.gif
 *
 * MD, Bit 31: The highest bit of the message number contains the direction of the message.
 * @li MD == #DSPIPC_MSGID_DIRECTION_DSPOUT : The message is send from the DSP to the HOST
 * @li MD == #DSPIPC_MSGID_DIRECTION_DSPIN : The message is send from the HOST to the DSP
 *
 * 
 */

/**
 * @brief Message id counter mask
 *
 * The message id consists of two parts: The message counter and the message direction. 
 * This mask can be used to extract the counter from the message id. @ref MsgId
 */
#define DSPIPC_MSGID_COUNTER_MSK (0x7fffffffU)

/**
 * @brief Message id direction mask
 *
 * The message id consists of two parts: The message counter and the message direction. 
 * this mask can be used to extract the direction information from the message id
 */
#define DSPIPC_MSGID_DIRECTION_MSK (0x80000000U)

/**
 * @brief Message id direction define for DSP output messages. 
 * 
 * This define defines the value for the direction field in the message id to use for 
 * messages from the DSP to the HOST
 */
#define DSPIPC_MSGID_DIRECTION_DSPOUT (0x80000000U)

/**
 * @brief Message id direction define for DSP input messages.
 * 
 * This define defines the value for the direction field in teh message id to use for 
 * messages from the HOST to the DSP
 */
#define DSPIPC_MSGID_DIRECTION_DSPIN (0x00000000U)

/**
 * @brief Message type mask
 *
 * The message type consists of two parts: The message type (stream or command) and the 
 * information whether this message expects a reply or not. 
 * This mask can be used to extract the command type from the message type field of a 
 * message
 */
#define DSPIPC_MSGTYPE_TYPE_MSK (0x00000001U)

/**
 * @brief Message type definition for command messages
 *
 * If the type bits specified by @ref DSPIPC_MSGTYPE_TYPE_MSK is equal to DSPIPC_MSGTYPE_TYPE_COMMAND, 
 * then the message is a command message. 
 */
#define DSPIPC_MSGTYPE_TYPE_COMMAND (0x00000000U)

/**
 * @brief Message type definition for stream messages
 * 
 * If the type bits specified by @ref DSPIPC_MSGTYPE_TYPE_MSK is equal to DSPIPC_MSGTYPE_TYPE_STREAM
 * then the message is a stream message.
 */
#define DSPIPC_MSGTYPE_TYPE_STREAM (0x00000001U)

/**
 * @brief Message reply mask
 * 
 * The message type consists of two parts: The message type (stream or command) and the 
 * information whether this message should be acknowledged or not. 
 * This mask can be used to extract the ack rquest from the messag type field of a message.
 */
#define DSPIPC_MSGTYPE_REPLY_MSK (0x00000002U)

/**
 * @brief Message ack definition for no acknowledge requested
 * 
 * If the ack bits in the message type are set to DSPIPC_MSGTYPE_ACK_NOACK, then no acknowledge is 
 * requested for this message. 
 */
#define DSPIPC_MSGTYPE_REPLY_NOREPLY (0x00000000U)

/**
 * @brief Message ack definition for acknowledge request
 * 
 * If the ack bits in the message type are set to DSPIPC_MSGTYPE_ACK_REQACK, then 
 * an acknowledge message is requested for this message. 
 */
#define DSPIPC_MSGTYPE_REPLY_REQREPLY (0x00000002U)

/** 
 * @brief Maximum numer of channels that fit into one channel information message. 
 *
 * This defines give the number of channel information entries that can be send
 * with a single message
 */
#define DSPIPC_MSG_MAXCHANNELS (4)

/** @name Streaming protocol definition
*/
/*@{*/

/**
 * @brief Provide buffer message
 *
 * This message is sent when an empty buffer has been provided.
 */
#define DSPIPC_SM_PROVIDEBUFFER (1)

/**
 * @brief Transfer request message
 *
 * This message indicates that a transfer request (request to transfer data)
 * has been issued. 
 */
#define DSPIPC_SM_TRANSFERREQUEST (2)

/**
 * @brief Buffer complete message
 *
 * This message indicates that a buffer that has been provided using a 
 * PROVIDEBUFFER or TRANSFREREQUEST message has now been completed and may be 
 * reused
 */
#define DSPIPC_SM_BUFFERCOMPLETE (3)

/**
 * @brief Flush message
 *
 * This message indicates that all buffers that are still pending (full or 
 * empty!) should be completed imidiately. 
 */
#define DSPIPC_SM_FLUSHBUFFERS (4)

/*@}*/

/** @ingroup dm642mmipc */
/** @defgroup ErrorMessages DM642 Error message definition
 * This section conatains the definitions of the error values that are 
 * provided when the admin channel sends an error message. 
 */
/*@{*/

/**
 * @brief Invalid channel id 
 *
 * The channel id specified with the operation was not valid, e.g. the channel 
 * does not exist or the given operation can not be applied to the channel 
 * specified. 
 */
#define DSPIPC_ERR_INVALID_CHANNEL (1)

/**
 * @brief Invalid command
 * 
 * The command specified is not supported/
 */
#define DSPIPC_ERR_INVALID_COMMAND (2)

/**
 * @brief IPC not initialized
 *
 * A message could not be completed due to the fact that the IPC has not been 
 * initialized priort to the command
 */
#define DSPIPC_ERR_NOT_INITIALIZED (3)

/**
 * @brief IPC already initialized
 *
 * A message could not be completed due to the fact theat the IPC has already
 * been initialzied. 
 */

#define DSPIPC_ERR_ALREADY_INITIALIZED (4)

/**
 * @brief No acknowledge although requested
 * 
 * Although a reply has been requested by the sender of the message, no 
 * reply has been provided by the receiver of the message
 */
#define DSPIPC_ERR_NO_REPLY (5)

/**
 * @brief Feature is not implemented
 *
 * The feature required for the requested message operation.
 */
#define DSPIPC_ERR_UNIMPLEMENTED (6)

/**
 * @brief There is insufficient space to complete the operation
 * 
 * Although there is no more buffer space on the receiver side, the sender 
 * sent more data
 */
#define DSPIPC_ERR_INSUFFICIENT_SPACE (7)

/*@}*/

/** 
 * @brief Default channel IDs
 * 
 * This enumeration defines default channel IDs. These channel IDs are reserverved
 * for internal use and can therefore not be used when calling the DSPIPC. 
 * If a user want to define a channel ID for its use, he must use an ID which numerical
 * value is >= #DSPIPC_USER. 
 */
enum /*DSPIPCDefaultChannelIDs */
{
        DSPIPC_INVALID = 0,
        DSPIPC_IPCADMIN = 1,
        DSPIPC_PRINT = 2,
        DSPIPC_LOG = 3,
        DSPIPC_ALIVE = 4,
        DSPIPC_USER = 16
};

/** @ingroup Jacintommipc */
/** @defgroup Jacintomsg Jacinto Message definition
 * This section documents the various internal messages that are used to 
 * establish a communication from DSP to the HOST (and vice versa)
 */
/*@{*/

/**
 * @brief Version message payload
 * 
 * This structure defines the payload of a version admin message. 
 * The version number determines the version of the implementation and 
 * estpecially of the communication structure
 */
struct _DSPIPCVersionMsg {
        /**
    * @brief Software major version
    *
    */
        mUInt32_t swMajor;

        /**
    * @brief Software minor version
    */
        mUInt32_t swMinor;

        /**
    * @brief Software bugfix number
    */
        mUInt32_t swBugfix;

        /**
    * @brief Special field. This field must NOT be include in the version check
    */
        mUInt32_t swSpecial;

        /**
    * @brief Should always be 0xbeefbeef if dsp version message is sent
    */
        mUInt32_t preSign;

        mUInt8_t dspVersionInfoARR[150];

        /**
    * @brief Should always be 0xdeaddead if dsp version message is sent
    */
        mUInt32_t postSign;
};

/**
 * @brief DSPIPCVersionMsg typedef
 */
typedef struct _DSPIPCVersionMsg DSPIPCVersionMsg;

/**
 * @brief Error information message payload
 *
 * This structure defines the message payload that is sent with an error admin
 * message (@ref _DSPIPCAdminMsg, cmdType = @ref DCCMID_ERROR). It contains the 
 * message id of the message that caused the problem (if applicable) and the 
 * error code. For the list of available error codes, see @ref ErrorMessages
 */
struct _DSPIPCErrorInformation {
        /**
    * @brief The message id of the errornous message
    * 
    * This field specifies the message that caused the error 
    */
        mUInt32_t msgId;

        /**
    * @brief Error code
    *
    * This field contains the error code, specifying the error reason
    */
        mUInt32_t errorCode;
};

/**
 * @brief DSPIPCErrorInformation typedef
 */
typedef struct _DSPIPCErrorInformation DSPIPCErrorInformation;

/**
 * @brief Channel information message payload
 *
 * This structure defines the message payload that is sent with a channel 
 * information admin message (@ref _DSPIPCAdminMsg) where the command type is
 * set to one of { @ref DCCMID_ADDCHANNELS, @ref DCCMID_REMOVECHANNELS}. A channel
 * information message can provide information about more then one channel, therefore, 
 * it specifies the number of channels that it provides information for. For each 
 * channel, it specifies the channel number and the channel type. 
 */
struct _DSPIPCChannelInformation {
        /** 
    * @brief Number of channel information records in this message
    * 
    * This number specifies the number of valid channel information records in the 
    * message.
    */
        mUInt32_t numChannels;

        /**
    * @brief Channel information for each channel
    */
        struct
        {
                /** 
       * @brief Channel number
       *
       * This number specifies the channel number of a channel for which this message
       * applies. 
       */
                mUInt32_t channelNum;

                /**
       * @brief Channel type.
       *
       * This value specifies the channel type. Together with the channel number, 
       * it uniquely specifies the channel
       */
                mUInt32_t channelType;

                /**
       * @brief Channel priority.
       *
       * This value specifies the channel priority. 
       */
                mUInt32_t channelPriority;
        } channel[DSPIPC_MSG_MAXCHANNELS];
};

/**
 * @brief DSPIPCChannelInformation typedef
 */
typedef struct _DSPIPCChannelInformation DSPIPCChannelInformation;

/**
 * @brief Channel information message payload
 *
 * This structure defines the message payload that is sent with a Drain 
 * information admin message (@ref _DSPIPCAdminMsg) where the command type is
 * set to one of { @ref DCCMID_DRAINREQ, @ref DCCMID_DRAINACK,
 * @ref DCCMID_DRAINCOMPLETE}. A Drain information message provides 
 * information about the channel which should be purged.
 */
struct _DSPIPCDrainInformation {
        /** 
    * @brief  Channel Number
    * 
    * This number specifies the channel which should be purged.
    */
        mUInt32_t chanId;
};

/**
 * @brief DSPIPCChannelInformation typedef
 */
typedef struct _DSPIPCDrainInformation DSPIPCDrainInformation;

/** 
 * @brief Admin message payload
 * 
 * This structure defines the message payload of an admin message. 
 * Admin messages are provided to establish internal communication, e.g. to check 
 * software versions, provide error information, add or remove channels etc...
 */
struct _DSPIPCAdminMsg {
        /** 
    * @brief Command union
    */
        union {
                /** 
       * @brief The admin message command type
       * 
       * This command type specifies the requested operation.
       */
                enum {
                        /** 
          * @brief Invalid message ID. 
          * 
          * This message ID is reserved for debugging and sanity checking. If must not be set
          * in any message. This message has no payload.
          */
                        DCCMID_INVALID = 0,

                        /** 
          * @brief Error messge ID
          * 
          * The error message can be sent to indicate an error. The payload of this message 
          * contains at least the information which error occured. It may contain additional 
          * information, e.g. the message ID of the operation that failed. 
          */
                        DCCMID_ERROR = 1,

                        /** 
          * @brief Version message
          * 
          * This message ID is used to transmit the version number of the DSPIPC interface. 
          * In its payload, the message carries the version information. 
          */
                        DCCMID_VERSION = 2,

                        /** 
          * @brief Add a channel 
          * 
          * This message is used to register channel(s) to the host system. The message payload
          * contains the id and the priority of the requested channel. 
          */
                        DCCMID_ADDCHANNELS = 3,

                        /**
          * @brief Remove a channel
          * 
          * With this message, a channel which has been registered previously can be removed. 
          * This message will result in an error message when this channel is still in use (e.g. 
          * messages pending) for this channel
          */
                        DCCMID_REMOVECHANNELS = 4,

                        /**
          * @brief Closedown request
          * 
          * When this message is send, the communication port will closedown. This requires
          * that all channels have been closed properly before. 
          */
                        DCCMID_CLOSEDOWN = 5,

                        /**
          * @brief Drain request
          * 
          * With this message, the Client initiated a drain of all pending messages and data
          * packages on a selected channel on the DSP.  
          */
                        DCCMID_DRAINREQ = 6,

                        /**
          * @brief Drain acknowledge
          * 
          * When this message is send, all pending stream messages as well as all stream data
          * packages of a selected channel are purged and passed back to the application on
          * the DSP.
          */
                        DCCMID_DRAINACK = 7,

                        /**
          * @brief Drain complete
          * 
          * With this message, the Client signalize the DSP that the Drain is completed.  
          */
                        DCCMID_DRAINCOMPLETE = 8,

                        /** 
          * @brief Add a channel is complete
          * 
          * This message is used to inform the dsp that adding the channel(s). The message payload
          * contains the id of the added channel. 
          */
                        DCCMID_ADDCHANNELS_COMPLETE = 9,

                        /** 
          * @brief Change channel priority
          * 
          * This message is used to inform the host that the priority of a already opened
          * channel has changed. The message payload contains the id of the channel, the 
          * channel type and the new priority. 
          */
                        DCCMID_CHANGECHANNELPRIO = 10,

                        /** 
          * @brief Data flow control for a channel: XOFF
          * 
          * This message sets the COMMAND Channel into a XOFF State.
          * No Message is send to the DSP until the DSP sends DCCMID_XON for this channel 
          * 
          */
                        DCCMID_XOFF = 11,

                        /** 
          * @brief Data flow control for a channel: XON
          * 
          * This message sets the COMMAND Channel into a XON State.
          * Messages can be send via this channel
          * 
          */
                        DCCMID_XON = 12

                } cmdType;
        } generic;

        /** 
    * @brief admin message payload
    * 
    * The following union defines the possible payloads for an admin message: It can be either
    * a version number (@ref cmdType == @ref DCCMID_VERSION), an error information (@ref cmdType == 
    * @ref DCCMID_ERROR) or a channel information (@ref cmdType == (@ref DCCMID_ADDCHANNELS | @ref 
    * DCCMID_REMOVECHANNELS)
    */
        union {
                DSPIPCVersionMsg version;
                DSPIPCErrorInformation errorInformation;
                DSPIPCChannelInformation channelInfo;
                DSPIPCDrainInformation drainInfo;
        } content;
};

/**
 * @brief DSPIPCAdminMsg typedef
 */
typedef struct _DSPIPCAdminMsg DSPIPCAdminMsg;

/** 
 * @brief Logging message structure
 * 
 * This structure defines the layout of a logging message. 
 * This message contains a log id, the severity of the message. The complete
 * remaining bytes of the payload are free for charaters.
 * This structure is placed inside the payload filed.
 */
struct _DSPIPCLogMsg {
        /**
    * @brief Log Id
    */
        mUInt32_t logId;

        /**
    * @brief Log severity
    */
        mUInt32_t severity;

        /**
    * @brief log message
    */
        mUInt8_t data[COMMANDMSG_SIZE - 2 * sizeof(mUInt32_t)];
};

/** 
 * @brief Number of ranges per buffer
 *
 * This definition defines the maximum number of ranges that can be stored in a 
 * single  _DSPIPCStreamMsg structure. 
 */
#define DSPIPC_RANGES_PER_BUFFER (8)

/** 
 * @brief Stream message structure
 * 
 * This structure defines the layout of a stream message. 
 * This message contains all required information to start a scatter/gather
 * dma transfer. This information is send to the DSP to communicate the 
 * addresses and sizes of the stream memory segments.
 * This structure is placed inside the payload filed.
 */
struct _DSPIPCStreamMsg {
        /**
    * @brief Command 
    * 
    * This member defines the streaming commmand that is to be performed. 
    */
        mUInt32_t cmd;

        /**
    * @brief Command id
    * 
    * This member defines a unique command id for this command
    */
        mUInt32_t cmdId;

        /**
    * @brief Number of ranges
    * 
    * This number determines the number of valid entries in the following array
    * or data ranges
    */
        mUInt32_t numRanges;

        /**
    * @brief Buffer flags
    *
    * This flag field contains additional information about the buffer. These
    * flags are valid for the whole bunch of data ranges.
    */
        mUInt32_t bufferFlags;

        /**
    * @brief Range list. 
    * 
    * This list contains an array of memory range definition. Each range 
    * consists of a physical address and a size of the data area. 
    */
        struct
        {
                /** 
       * @brief The physical address
       * 
       * This member defines the physical start address of the memory area. It
       * determines the address of the data buffer as seen on the PCI bus. 
       * This address must be aligned to 32bit.
       */
                mUInt32_t physicalAddress;

                /**
       * @brief range size
       * 
       * The range size determines the size of the range. The valid memory area
       * to work on is [@ref physicalAddress .. @ref physicalAddress + dataSize]
       */
                mUInt32_t rangeSize;

                /**
       * @brief Data size
       * 
       * The data size determines the size of the valid data inside a range. 
       * The valid memory area to work on is 
       * [@ref physicalAddress .. @ref physicalAddress + dataSize]
       */
                mUInt32_t dataSize;
        } ranges[DSPIPC_RANGES_PER_BUFFER];
};

/**
 * @brief DSPIPCStreamMsg typedef
 */
typedef struct _DSPIPCStreamMsg DSPIPCStreamMsg;

/*@}*/

/** @ingroup Jacintommipc */
/** @defgroup JacintoQueues Transmit and receive queue
 *
 * The transmit and receive queues establish a common location where both
 * DSP and host can access messages. There is one queue for each direction.
 * Each queue consists of 
 * two pointers, a read index and a write index, and an array of 
 * message elements. The read and the write index determines which elements
 * of the queue are valid. The write index points to the next entry 
 * in the array that can be written by the producer of queue messages. The 
 * read pointer points to the next entry in the array that can be read by 
 * the producer. 
 * @image html CommStructure.gif "Read and write pointer in a queue"
 * Be aware that the indices are free running, that means that
 * they give the position of an element in a virtually infinite array of
 * elements. To get the actual position in the physical memory array, this 
 * position can be calculated as the reminder of the division of the index
 * by the size of the array. The queue is empty when the equation 
 * [ writeIndex - readIndex > 0 ] 
 * holds. The queue is full when
 * [ writeIndex - readIndex - NUMELEMENTS >= 0 ] evaluates to 'true'.
 *
 */

/** @ingroup JacintoQueues */
/** 
 * @brief Communication message structure
 * 
 * This structure defines the layout of a single communication message. 
 * A communication message contains a channel id, a message id and 
 * a number of 8 bit values for the actual message payload. 
 */
struct _DSPIPCMessage {
        /** 
    * @brief Channel id
    *
    * Each message is send by (and to) a specific channel. This channel is 
    * determined by the channel id. 
    */
        mUInt32_t channelId;

        /**
    * @brief Message id
    *
    * The message id is determined by the usage of the channel. 
    */
        mUInt32_t msgId;

        /** 
    * @brief Actual length of the payload. 
    * 
    * This value determines how many bytes of the payload contain
    * valid values (and therefore have to be taken into account)
    */
        mUInt32_t msgSize;

        /**
    * @brief The message type
    *
    * This value determines the type of the messag as well as the information
    * whether an reply is requested for this message.
    */
        mUInt32_t msgType;

        /**
    * @brief Message payload
    *
    * This array contains the actual message payload. 
    */
        union {
                mUInt8_t message[COMMANDMSG_SIZE];
                DSPIPCAdminMsg adminMsg;
                DSPIPCStreamMsg streamMsg;
        } payload;
};

/**
 * @brief DSPIPCMessageQueueEntry typedef
 */
typedef struct _DSPIPCMessage DSPIPCMessageQueueEntry;
typedef struct _DSPIPCMessage DSPIPCMessage;
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _COMMSTRUCTS_ */
