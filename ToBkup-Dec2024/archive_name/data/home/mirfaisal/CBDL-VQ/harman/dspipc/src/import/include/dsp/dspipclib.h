/**************************************************************
 * Project			Harman Car Multimedia System
 * (c) Copyright	2005
 * Company			Harman/Becker Automotive Systems GmbH
 *					All rights reserved
 **************************************************************/
/**
 * @file			dspipclib.h
 *
 * This file declars the user interface of the IPC mechanism. 
 * 
 * @ingroup		MMIPCinterface
 * @date       03.03.2006
 * @author		Ulrich Mohr
 * 
 */

#ifndef DM642PCIIPC
#define DM642PCIIPC

#ifdef __cplusplus
extern "C" {
#endif

#include "linux/types.h"

/** @defgroup MMIPCinterface MMIPC library interface
 * These are the definitons that are available on the library from a library client
 */

/** @ingroup MMIPCinterface */
/** @defgroup MMIPCif_errors MMIPC library error message definitions 
 * This section describes the error (return value) definitions used by 
 * the interface to indicate the success status of the individual interface
 * calls.
 */
/*@{*/

/** 
 * @brief Success error message
 * 
 * This return value indicates that everything is ok. 
 */
#define HOSTIPC_OK (0)

/** 
 * @brief Channel already exists
 *
 * If this error message is thrown, then a function was requested to register
 * a new channel, although the channel already exists
 */
#define HOSTIPCERR_CHANNELEXISTS (1)

/**
 * @brief Channel is unavailable
 *
 * An operation was requested on a channel that does not exist or is not suitable for
 * the requested operation.
 */
#define HOSTIPCERR_CHANNELUNAVAILABLE (2)

/**
 * @brief Some resource is full
 *
 * An operation could not be completed cause a resource (e.g. a queue) was completely full.
 * This may indicate that there is either a dead lock or a over-load situation
 */
#define HOSTIPCERR_FULL (3)

/** 
 * @brief Invalid parameters
 *
 * The parameters given to the current operation was invalid (out of range etc...)
 */
#define HOSTIPCERR_INVALID_PARAMETERS (4)

/**
 * @brief Host IPC not attached
 * 
 * The host did not yet attach to the IPC. 
 */
#define HOSTIPCERR_NOT_ATTACHED (5)

/**
 * @brief The resource is empty
 *
 * The operation failed because of insuficcient date available
 */

#define HOSTIPCERR_EMPTY (6)

/**
 * @brief The resource is in processing
 * 
 * The requested operation failed because there is still an operation in processing.
 */
#define HOSTIPCERR_INPROCESSING (7)

/**
 * @brief There is not enough memory 
 *
 * The requested operation failed due to insufficient memory on the heap. 
 */
#define HOSTIPCERR_OUTOFMEMORY (8)

/**
 * @brief Drain is processing for this channel
 *
 * The requested operation failed because the channel is drained at the moment. 
 */
#define HOSTIPCERR_DRAININPROCESS (9)

/**
 * @brief Message Pool is empty
 *
 * No more messages are avialable inside the Message Pool. 
 */
#define HOSTIPCERR_COMMANDMESSAGEPOOLEMTY (10)

/**
 * @brief Free memory failed 
 *
 * Free memory during cleanup failed. 
 */
#define HOSTIPCERR_FREEMEMORYFAILED (11)

/**
 * @brief Function not implemented 
 *
 * The called function is currently not implemented. 
 */
#define HOSTIPCERR_FUNCTIONNOTIMPLEMENTED (12)

/*@}*/

/** @ingroup MMIPCinterface */
/** @defgroup MMIPCif_types MMIPC library type definition 
 * The defintions in this paragraph define the types and structures 
 * used by the MMIPC interface library */
/*@{*/

/**
 * @brief IPCHandle type
 * 
 */
typedef void* IPCHandle;

/**
 * @brief IPC Channel handle type
 */
typedef void* IPCChannelHandle;

/**
 * @brief Memory range definition
 *
 * A memory range structure defines a chunk of physical memory. This chunk is 
 * described by its start address, the size of the memory area and, if applicable, 
 * the amount of data currently valid in this memory range. 
 */
struct _MemoryRange {
        /** 
	 * @brief Data address
	 */
        uint32_t dataAddress;

        /**
	 * @brief data range size
	 */
        uint32_t rangeSize;

        /**
	 * @brief data size
	 */
        uint32_t dataSize;
};

/**
 * @brief memory range typedef. 
 */
typedef struct _MemoryRange IPCMemRange;

/**
 * @brief maximum number of ranges per buffer
 */
#define IPC_RANGES_PER_BUFFER (16)

/**
 * @brief Stream buffer type
 * 
 * This structure declares a stream buffer, which is the structure that the MMIPC
 * interface works on. These stream buffers will be passed in by the user, using the 
 * hoststream_senddata(...) and hoststream_providebuffer(...) interface functions. 
 * As well, the stream buffers are the entity that will be stored in the internal queues. 
 * A stream buffer can hold up to @ref IPC_RANGES_PER_BUFFER memory ranges of type @ref IPCMemRange. 
 * The number of ranges gives the number of valid memory areas available in the buffer at
 * the moment. The buffer flags give additional information how this buffer must be 
 * processed.
 */
struct _IPCStreamBuffer {
        /**
	 * @brief Stream buffer id
	 */
        uint32_t streamBufferId;

        /**
	 * @brief Number of ranges contained in the packet
    * 
    * This field gives the number of valid entries in the @ref ranges array. Therefore
    * this number must never exceed @ref IPC_RANGES_PER_BUFFER
	 */
        uint32_t numRanges;

        /**
    * @brief Buffer flags
    *
    * These buffer flags specify additional properties of the buffer, such as
    * discontinuities etc....
    */
        uint32_t bufferFlags;

        /**
	 * @brief Range definitions
	 * 
	 * For each memory data range, the buffer specifies the data address, the 
	 * maximum size of the data area and the number of bytes set in this area. 
	 */
        IPCMemRange ranges[IPC_RANGES_PER_BUFFER];
};

/**
 * @brief Stream buffer typedef. 
 */
typedef struct _IPCStreamBuffer IPCStreamBuffer;

/** 
 * @brief No buffer flags
 * 
 * This value, if set on the buffer flags, indicates that there are no special 
 * buffer flags set on this buffer. 
 */
#define BUFFERFLAG_NONE (0x00000000U)

/** 
 * @brief Data discontinuity flag
 * 
 * If this flag is set on a buffer, the data after this buffer may be discontinous 
 * with the content of the current buffer. This indicates that either it is uncertain
 * that more data will come or the data that will be next is uncorrelated to the 
 * data in the buffer. 
 * As a consequence, if this flag is set on a buffer that provides data to the 
 * library, the receiver of the data in the buffer is asserted to receive the data 
 * emmidiately after the data has been transfer. This may lead to an not-full receive
 * buffer delivered back to the receiver. 
 */
#define BUFFERFLAG_DATA_DISCONTINOUS (0x00000001U)

/** 
 * @brief Provide buffer flag
 * 
 * This flag is added to the set of buffer flags by the library before a buffer 
 * is returned to the client to indicate that the buffer returned is the result 
 * of a provide-buffer call. 
 * This flag is set by the library, setting it by the client has no effect. 
 */
#define BUFFERFLAG_PROVIDEBUFFER (0x00010000U)

/** 
 * @brief Send data buffer flag
 * 
 * This flag is added to the set of buffer flags by the library before a buffer
 * is returned to the client to indicate that the buffer returned is the result 
 * of a send-buffer call. 
 * This flag is set by the library, setting it by the client has no effect. 
 */
#define BUFFERFLAG_SENDDATA (0x00020000U)

/** 
 * @brief Buffer has been drained
 * 
 * This flag is set by the library to indicate that the buffer has been returned
 * to the user due to a flush command, issued by the sender of the data. 
 * The flag is no statement about the validity of the data: The amount of data 
 * that is valid in each buffer is specified by the data size that is given with 
 * each data range in the buffer. 
 *
 * This flag is set by the library, setting it by the client has no effect. 
 */
#define BUFFERFLAG_DRAINED (0x00040000U)

/** 
 * @brief Receive Stream message flag
 * 
 * This flag is set by the library to indicate that the buffer has been returned
 * to the user due to incoming message on the streaming channel. The pending 
 * buffer has to be completed(if any), before we deliver the command message. 
 * Otherwise, command messages may overtake stream messages.
 */
#define BUFFERFLAG_INBANDMSGRECEIVED (0x00000002U)

/**
 * @brief IPC configuration type
 * 
 * This structure declares the IPC configuration.
 *
 */
struct _IPCConfig {
        enum {
                DSPIPC_EMIF = 0xbe,
                DSPIPC_PCI,
                DSPIPC_SHMEM,
                DSPIPC_TDM
        } ipcType;

        uint32_t ipcHeapId;

        struct
        {
                uint32_t ipcBaseAddress;
        } shmemIpcConfig;

        struct
        {
                uint32_t blablub;
        } tdmIpcConfig;
};

/**
 * @brief IPCConfig structure typedef. 
 */
typedef struct _IPCConfig IPCConfig;

/*@}*/

/** @ingroup MMIPCinterface */
/** @defgroup MMIPC_chanPrio MMIPC channel priority levels 
 * This section contains the MMIPC channel priority levels.
 */
/*@{*/

/**
 * @brief Admin Channel Priority Level
 *
 * This Priority level is the highest priority level for command channels
 * and at the same time the highest channel priority level at all. Stream
 * channels can't be opened with this channel priority.
 * This priority level is also used as default priority for control channels.
 * This means that if there is no channel priority passed during the channel
 * registration the channel will be opened with this channel priority.
 */
#define HOSTIPC_CHANNELPRIORITY_ADMIN 4

/**
 * @brief Control Channel Priority Level
 *
 * This Priority level is the highest priority level for command channels
 * and at the same time the highest channel priority level at all. Stream
 * channels can't be opened with this channel priority.
 * This priority level is also used as default priority for control channels.
 * This means that if there is no channel priority passed during the channel
 * registration the channel will be opened with this channel priority.
 */
#define HOSTIPC_CHANNELPRIORITY_REALTIMECOMMAND 3

/** 
 * @brief Stream Channel Priority Level
 * 
 * This Priority level is the highest priority level for stream channels
 * and the seond  highest channel priority level at all. Control
 * channels can't be opened with this channel priority.
 * This priority level is also used as default priority for stream channels.
 * This means that if there is no channel priority passed during the channel
 * registration the channel will be opened with this channel priority.
 */
#define HOSTIPC_CHANNELPRIORITY_REALTIMESTREAM 2

/** 
 * @brief Normal Channel Priority Level
 *
 * This Priority level is the third highest channel priority level.
 */
#define HOSTIPC_CHANNELPRIORITY_NORMAL 1

/** 
 * @brief Idle Channel Priority Level
 *
 * This Priority level is the lowest channel priority level. This level can
 * be used for channels with no realtime requirements, e.g tracing.
 */
#define HOSTIPC_CHANNELPRIORITY_IDLE 0

/*@}*/

/** @ingroup MMIPCinterface */
/** @defgroup MMIPC_FlowControl MMIPC command channel flow control types
 * This section contains the MMIPC flow control types. 
 */
/*@{*/

/** 
 * @brief Xon definition for the flow control protocol.
 *
 * The Xon and Xoff defines has to be used to control the message
 * transmission of a command channel. 
 * The Xon command is sent by the receiver, when the receiver
 * is available to take more messages. 
 * Default channel state is Xon state.
 */
#define HOSTIPC_XON 0

/** 
 * @brief Xoff definition for the flow control protocol.
 *
 * The Xon and Xoff defines has to be used to control the message transmission
 * of a command channel. 
 * Typically the receiver sends an Xoff command when it can't take any more 
 * messages (e.g. it may need time to process something). 
 * The application programmer has to keep in mind, that it is still possible 
 * to receive up to three messages after issuing the Xoff command. This is a 
 * currently limitation of the physical implementation. 
 */
#define HOSTIPC_XOFF 1

/*@}*/

/** @ingroup MMIPCinterface */
/** @defgroup MMIPCif_functions Interface functions 
 * This section contains the public interface accessible on the MMIPC library. 
 */
/*@{*/

/**
 * @brief Receive callback function
 * 
 * For each channel (streaming or command) you must registre a receive callback. 
 * This callback will be called whenenver a command message is received on this
 * specific channe. 
 * 
 * This callback may be called on the receiver thread context. Since this 
 * thread handles all channels, blocking this call will block all other channels
 * as well. Therefore, the processing in the callback should be as short as 
 * possible. If you need to do further processing in response to a message
 * received, you should signal a thread and complete the processing on this thread.  
 * 
 * Nevertheless, it is allowed (since often necessary) to send a message as a 
 * reply to this received message, either to the same or a different channel. 
 * 
 * After the callback returns, the content of 'msg' is no longer valid. So if you 
 * want to keep the message content for later use, you must have a copy of it. 
 * 
 * @param msg The message received on the channel
 * @param replySize The size of the reply that should be sent
 * @param reply The reply data to send. 
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
typedef uint32_t (*ReceiveCallback)(uint32_t msgSize, uint8_t* msg, uint32_t* replySize, uint8_t* reply, void* param);

/**
 * @brief Buffer complete callback
 * 
 * For each streaming channel, you must register a buffer complete callback. 
 * This callback will be called whenever a buffer (input or output) has been 
 * completely processed. 
 * @param buf The buffer that has been completed
 * @param param The user data pointer passed in when registering the callback
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
typedef uint32_t (*BufferCompleteCallback)(IPCStreamBuffer* buf, void* param);

/**
 * @brief IO-Controler callback function
 * 
 * You must register one IO-Controler callback function. 
 * This callback will be called whenever an IO-Controller message is received.
 * This function is called on the EMIF-ISR. Since this ISR handles all DSP-IPC 
 * Interrupts, blocking this call will block the entire DSP-IPC.
 *
 * @param msg 32Bit value of the IOC mailbox register.
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
typedef uint32_t (*IOControllerRcvCallback)(uint32_t msg);

/**
 * @brief Create an IPC instance
 *
 * This function creates a handle to the IPC driver
 * @param handle A pointer to an IPC handle. This handle will be valid after 
 *               hostipc_create returns successfully
 * @param segId  Heap identifier. If channels are registered, the heap specified
 *               by this id will be used to allocate resources (e.g. queues...)
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_create(IPCHandle* handle, uint32_t segId);

/**
 * @brief Create an IPC instance extension
 *
 * This function creates a handle to the IPC driver
 * @param handle A pointer to an IPC handle. This handle will be valid after 
 *               hostipc_createInstance returns successfully
 * @param config A pointer to the DspIpc configuration structure @ref MMIPC_Contig.
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_createInstance(IPCHandle* handle, IPCConfig* config);

/**
 * @brief Release an IPC instance
 *
 * This function releases an instance that has been initialized using @ref hostipc_create
 * @param handle A handle created using hostipc_create(...)
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_destroy(IPCHandle handle);

/**
 * @brief Initialize an IPC handle
 *
 * This call can be used to initialize an ipc handle that has been created calling
 * @ref hostipc_create. 
 * @param handle A handle created using hostipc_create(...)
 * @param taskPriority Priority of the dspipc internal used Tasks
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_initialize(IPCHandle handle, uint32_t taskPriority);

/**
 * @brief Cleanup an IPC hanlde
 * 
 * This call cleans up an IPC context. After this call, no other function than 
 * @ref hostipc_destroy or hostipc_create must be called on the given handle
 * @param handle A handle create using hostipc_create(...)
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_cleanup(IPCHandle handle);

/**
 * @brief Send a message using a specific IPC channel
 * 
 * This function sends a message to the host using the channel id specified. A channel with 
 * the given ID must have been registered prior to this call. The data pointer shows to the 
 * actual message to send. The size of this data area is specified by the size passed in to 
 * this call. This size must not exceed the maximum size of a IPC message (256 bytes at the
 * moment)
 * @param handle A handle to a valid host IPC instance
 * @param data Pointer to the message data to send
 * @param size Size (in bytes) of the message to send. 
 * @param replyReq This parameter determines whether a reply is requested (=1) or not
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_sendmessage(IPCChannelHandle handle, uint8_t* data, uint32_t size, uint32_t replyReq);

/**
 * @brief Send a XonXoff command for a specific command channel
 * 
 * This function sends a XonXoff message to the host using the channel id specified.
 * A channel with the given ID must have been registered prior to this call. 
 * See @ref MMIPC_FlowControl for possible channel states.
 * The application programmer has to keep in mind, that it is still possible 
 * to receive up to three messages after issuing the Xoff command. This is a 
 * currently limitation of the physical implementation. The function is currently 
 * only supported for the DRA300.
 * @param handle A handle to a valid host IPC channel
 * @param channelState See @ref MMIPC_FlowControl for possible channel states.
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_controlChannelFlowCtrl(IPCChannelHandle handle, uint32_t channelState);

/** 
 * @brief Register a channel with default priority
 *
 * This function registers a channel on the MMIPC interface with default channel
 * priority (@ref MMIPC_chanPrio). No message can be send using the 
 * channel id specified prior to this call. You have to specify the numerical id 
 * the channel. This channel id must not be in the range of the reserved channel
 * ids (0..15). The channel id must not be assigned to a valid channel at the 
 * same time. In both cases, the call will fail. 
 * The function, if successfull, returns a channel handle. This channel handle 
 * will be used when sending messages on this channel or de-registering the channel 
 * later on.
 * 
 * @param handle Handle to a valid host IPC instance
 * @param channelId The numerical id of the channel 
 * @param cb Callback to call when a message receives from the host on this channel
 * @param param User parameter. 
 * @param chh The channel handle allocted for this channel. 
 * @returns Status of the call. See @ref  MMIPCif_errors for possible values
 */
uint32_t hostipc_registerControlChannel(IPCHandle handle, uint32_t channelId, ReceiveCallback cb, void* param, IPCChannelHandle* chh);

/** 
 * @brief Register a channel with a specified priority
 *
 * This function registers a channel on the MMIPC interface with a user specified priority.
 * No message can be send using the  channel id specified prior to this call.
 * You have to specify the numerical id the channel. This channel id must not be in the
 * range of the reserved channel ids (0..15). The channel id must not be assigned to a
 * valid channel at the same time. In both cases, the call will fail. 
 * The function, if successfull, returns a channel handle. This channel handle 
 * will be used when sending messages on this channel or de-registering the channel 
 * later on.
 * 
 * @param handle Handle to a valid host IPC instance
 * @param channelId The numerical id of the channel 
 * @param cb Callback to call when a message receives from the host on this channel
 * @param param User parameter. 
 * @param chh The channel handle allocted for this channel.
 * @param channelPriority The channel priority. See @ref MMIPC_chanPrio for possible values
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_registerPrioControlChannel(IPCHandle handle, uint32_t channelId, ReceiveCallback cb, void* param, IPCChannelHandle* chh, uint32_t channelPriority);

/**
 * @brief Register a stream channel
 * 
 * This function registers a ipc channel, with default channel
 * priority (@ref MMIPC_chanPrio) that can be used to stream media data. 
 * You have to specify the numerical id of
 * the channel. This channel id must not be in the range of the reserved channel
 * ids (0..15). The channel id must not be assigned to a valid channel at the 
 * same time. In both cases, the call will fail. 
 * The function, if successfull, returns a channel handle. This channel handle 
 * will be used when sending data on this channel or de-registering the channel 
 * later on.
 * This channel has the same functionality as a control channel opened using 
 * @ref hostipc_registerControlChannel. 
 * In addition, it is possible to send and receive media data packets on this 
 * channel using @ref hoststream_senddata and @ref hoststream_providebuffer. 
 * @param handle Handle to a valid host IPC instance
 * @param channelId The numerical id of the channel 
 * @param cb Callback to call when a message receives from the host on this channel
 * @param bufferComplete Callback to call when a buffer has been completed. 
 * @param param User parameter. 
 * @param chh The channel handle allocted for this channel. 
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_registerStreamChannel(
    IPCHandle ipc, uint32_t channelId,
    ReceiveCallback cb,
    BufferCompleteCallback bufferComplete,
    void* param,
    IPCChannelHandle* handle);

/**
 * @brief Register a stream channel
 * 
 * This function registers a ipc channel, with a user specified channel priority
 * that can be used to stream media data. You have to specify the numerical id of
 * the channel. This channel id must not be in the range of the reserved channel
 * ids (0..15). The channel id must not be assigned to a valid channel at the 
 * same time. In both cases, the call will fail. 
 * The function, if successfull, returns a channel handle. This channel handle 
 * will be used when sending data on this channel or de-registering the channel 
 * later on.
 * This channel has the same functionality as a control channel opened using 
 * @ref hostipc_registerControlChannel. 
 * In addition, it is possible to send and receive media data packets on this 
 * channel using @ref hoststream_senddata and @ref hoststream_providebuffer. 
 * @param handle Handle to a valid host IPC instance
 * @param channelId The numerical id of the channel 
 * @param cb Callback to call when a message receives from the host on this channel
 * @param bufferComplete Callback to call when a buffer has been completed. 
 * @param param User parameter. 
 * @param chh The channel handle allocted for this channel.
 * @param channelPriority The channel priority. See @ref MMIPC_chanPrio for possible values
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_registerPrioStreamChannel(
    IPCHandle ipc, uint32_t channelId,
    ReceiveCallback cb,
    BufferCompleteCallback bufferComplete,
    void* param,
    IPCChannelHandle* handle,
    uint32_t channelPriority);
/** 
 * @brief Un-Register a channel
 * 
 * This function unregisters an IPC channel. After this call, no message can be send using this
 * channel id until @ref hostipc_registerControlChannel or @ref hostipc_registerStreamChannel 
 * is called again. 
 * @param handle Handle to a valid host IPC instance
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_unregisterChannel(IPCChannelHandle handle);

/** 
 * @brief Change the priority of a channel
 * 
 * This function changes the priority of a channel.
 * @param handle Handle to a valid host IPC instance
 * @param channelPriority The new channel priority. See @ref MMIPC_chanPrio for possible values
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_changeChannelPriority(IPCChannelHandle handle, uint32_t channelPriority);

/** 
 * @brief initialize the IO-Controller communication
 * 
 * This function initializes the IO-Controller communication. After this call a rudimental
 * via a 32 Bit mailbox register is possible. 
 * @param handle Handle to a valid host IPC instance
 * @param cb Callback to call when a message receives from the IO-Controller
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_initIOControllerCommunication(IPCHandle handle, IOControllerRcvCallback cb);

/** 
 * @brief write a 32bit value to the IO-Controller mailbox register
 * 
 * This function writes a 32 bit value to the IO-Controller mailbox register
 * within the FPGA. The application has to take care, that this function is not called
 * twice at the same time. This function is only supported for the DRA300.
 * @param msg 32 bit data value
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_sendIOControlerMessage(uint32_t msg);

/** 
 * @brief wait until host is ready
 *
 * This function blocks the calling application until the host is ready.
 * Normally this function is only needed for debug purpose and should be called directly
 * after all channels are registered. Currently (26-Jan-2007) this function
 * is only helpful for the DM642, on the DRA300 its only a dummy function.  
 * @param handle Handle to a valid host IPC instance
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hostipc_waitforHost(IPCHandle handle);

/**
 * @brief Send a data packet
 * 
 * This function sends a data packet to the host. When the transfer is completed by the host, 
 * a callback will be issued on the streaming channel. 
 * @param handle Handle to the stream channel 
 * @param buf Stream data buffer
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hoststream_senddata(IPCChannelHandle handle, IPCStreamBuffer* buf);

/** 
 * @brief Provide an empty buffer
 * 
 * This function sends an empty buffer to the IPC driver. The IPC driver will use this buffer
 * when a transfer from the host to the client is requested by the host. 
 * The completion of this transfer will be signaled by a buffer completion callback by the 
 * host. 
 * @param handle Handle to the stream channel 
 * @param buf Stream data buffer
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hoststream_providebuffer(IPCChannelHandle handle, IPCStreamBuffer* buf);

/**
 * @brief drain a stream channel
 *
 * This function call drains all buffer commands that are still pending on the channel. 
 * Buffers that are not yet completed will be notified as completed as for a normal channel 
 * operation, but the size of the buffer will be smaller than on an completed request (or 
 * may even be zero)
 * @param handle Handle to the stream channel 
 * @returns Status of the call. See @ref MMIPCif_errors for possible values
 */
uint32_t hoststream_drain(IPCChannelHandle handle);

/*@}*/

/** @ingroup MMIPCinterface */
/** @defgroup mmipcStreaming Data streaming 
 * This section decribes the buffer passing mechanism. 
 * The buffer passing mechanims is used to establish a data 
 * (e.g. DMA) transfer from the DSP do the host (or vice-versa). 
 * To transfer a buffer, both the source buffer containing he data and the destination 
 * buffer where to DMA the data to must be available. Therefore the producer of the 
 * data must provide a filled buffer (on the DSP, this can be done using the @ref 
 * hoststream_senddata(...) function) and the consumer must provide an empty buffer 
 * (on the DSP side by using the @ref hoststream_providebuffer) function
 * befor a transfer can start. The order whether the data is available first or the 
 * buffer is arbitrary. 
 * Two transfers from the DSP to host are shown in the image below: the 
 * first one where the buffer is available first, the second one where the data buffer 
 * is available first. 
 *
 *@image html StreamCommunication.gif Communication diagram for buffer passing */

#ifdef __cplusplus
}
#endif

#endif /* DM642PCIIPC */
