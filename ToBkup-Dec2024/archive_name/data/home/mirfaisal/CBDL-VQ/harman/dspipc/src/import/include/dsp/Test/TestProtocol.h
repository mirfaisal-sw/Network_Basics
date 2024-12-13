/** 
 * @mainpage DspIpc Test suite.
 *
 * The test suite consists of two parts a TestServer which is running on the DSP
 * and the TestClient which is running on the SH4A. The test suite can either be used
 * as a sample project or for the verification of the basic features offered by the DspIpc. 
 *
 * The test protocol defines a simple protocol to execute different test 
 * cases on the DSP. The DSP acts thereby as client while the host is the 
 * master of the communication. 
 *
 * At the startup of the test protocol, the DSP registers a single command 
 * channel at the DSPIPC. The host attaches to this test channel when he starts
 * his test application.
 * 
 * When the host wants to start a particular test case, he sends a command 
 * message on the command channel. This message contains the id of the test
 * to execute. The reception of the message triggers the start of the desired 
 * test case on the DSP. 
 * 
 * The first thing that the test case does is to register an additional channel, 
 * the test channel. The type of this test channel depends on the test that is 
 * beeing executed. 
 * 
 * When the test started, all communication takes place on the test channel, 
 * until the test is beeing aborted by the host. 
 *
 * When the host aborts the test, the DSP closes the test channel. Finally it 
 * sends a test result back to the host and waits for the next test to start. 
 * 
 *
 * At the moment, the following test cases are specified:
 * - @subpage testCaseDSPSendHostReplies 
 *            "The host sends a command message, the DSP replies with a command message"
 * - @subpage testCaseHostSendDSPReplies 
 *            "The DSP sends a command message and the host replies with a command message"
 * - @subpage testCaseHostSendsBurst 
 *            "The hosts sends a burst of command messages"
 * - @subpage testCaseDspSendsBurst 
 *            "The dsp sends a burst of command messages"
 * - @subpage testCaseHostSendsStream 
 *            "The host sends stream data to the DSP"
 * - @subpage testCaseDSPSendsStream 
 *            "The DSP sends stream data to the host"
 * - @subpage testCaseHostSendsStreamDSPReplies 
 *            "The host sends a stream to the DSP and the DSP replies on this stream"
 * - @subpage testCaseHostSendsDiscontinuity 
 *            "The host sends stream data annotated with a data discontinuity"
 * - @subpage testCaseDSPSendsDiscontinuity 
 *            "The DSP sends stream data annotated with a data discontinuity"
 * - @subpage testCaseHostAbort 
 *            "The host sends an abort command after some streaming data"
 * - @subpage testCaseDSPAbort 
 *            "The DSP sends an abort command after some streaming data"
 * - @subpage testCaseSendResponseTestOnStreamChannel
 *            "The DSP sends command messages on a stream channel and SH4A replies." 
 * - @subpage testcaseReceiveStreamDrainReplyTest
 *            "Either on the DSP or on the SH4 side are buffers provided when the drain is called."
 * - @subpage testCaseDrain2
 *            "The DSP tries to send data and provides buffers to receive data, SH4 does nothing and sends a drain command."
 * - @subpage testCaseReceiveStreamVarReplyTestMulti
 *            "Parallel duplex data transfer on eight stream channel."
 * - @subpage testChannelPriority
 *            "Channel priority test."
 * - @subpage flowControlTest
 *            "Flow Control test for command channels."
 *
 */

/** 
 * @page testCaseDSPSendHostReplies Test case: The host sends a command message, the DSP replies with a command message
 * 
 * This page describes the test case where the host sends a command message to 
 * the DSP and the DSP replies with a command message. 
 * 
 * For the test, the test channel is a command channel. 
 *
 * When this test is beeing executed, the host sends a command message to the DSP. 
 * When the DSP receives this control message, it inverts each byte of the message, 
 * This inverted message is sent back to the host. 
 * 
 * The host tests the received message and checks whether the content of the
 * message matches the inverted content of the message sent. If this constaint
 * holds, the test is marked as "successfull", otherwise it is marked as "failed".
 */

/** 
 * @page testCaseHostSendDSPReplies Test case: The DSP sends a command message and the host replies with a command message
 * 
 * This page describes the test case where the DSP sends a command message to the 
 * DSP and the DSP replies with a command message. 
 * 
 * For the test, the test channel is a command channel.
 *
 * When this test is beeing executed, the DSP sends a command message to the host.
 * The host inverts each byte of the message and sends the inverted message back 
 * to the DSP. 
 * 
 * The DSP tests whether the content of the replied message matches the inverted 
 * content of the original message. If this containt does not hold, the test is 
 * marked as "failed". 
 * 
 * When the test is beeing completed by the host, the DSP sends the information 
 * back whether the test has been successfull or not. 
 */

/**
 * @page testCaseHostSendsBurst Test case: The host sends a burst of command messages to the DSP
 * 
 * This page describes the test case where the host sends a burst of command messages
 * to the DSP without waiting for a reply. 
 * 
 * For the test, the test channel is a command channel. 
 * 
 * With the test start, the host transfers a set of parameters. These parameters
 * define 
 * - the number of messages a single burst will contain
 * - the time the DSP should wait in between two burst. 
 * 
 * The host starts to send a burst of command messages to the DSP. The number of 
 * messages in the burst matches the number of messages transfered within the 
 * parameters. Each message start at the first byte with the number of the 
 * burst. The value of each consecutive byte of the message is the value of the 
 * previous byte increase by one. 
 *
 * When the DSP receives a message, it tests the content of the message. In addition 
 * it counts the messages that has been received in one burst. If the content 
 * of the message does not match the expected values or the number of messages does 
 * not match the number of messages per burst, then the test is marked as "failed" 
 */

/**
 * @page testCaseDspSendsBurst Test case: The DSP sends a burst of command messages to the host
 *
 * This page describes the test case where the DSP sends a burst of command messages
 * to the host without waiting for a reply.
 * 
 * For the test, the test channel is a command channel. 
 * 
 * With the start of the test, the DSP gets a set of parameters from the host. These
 * parameters contain
 * - The number of burst to send
 * - The number of messages per burst
 * - The time to wait in between two bursts
 * - The time to wait in between the send of two consecutive messages
 * 
 * The dsp start to send a burst of command messaegs to the DSP. The number of 
 * messages in the burst matches the number received in the parameter set for the 
 * test. The content of the message are the characters a..z, followed by the 
 * characters 0..9. 
 * 
 * The host checks the content of each message and counts the messages received. 
 * If the content of the number of messages does not match the values specified 
 * in the parameter set for the test, the test is marked as "failed". 
 */

/** 
 * @page testCaseHostSendsStream Test case: The host sends stream data to the DSP 
 * 
 * This page describes the test case where the host sends stream data to the DSP. 
 * 
 * The test channel used for this test is a stream channel. 
 * 
 * The host sends a stream packet that contains a set of 32 bit values. Theses 
 * 32 bit values starts with the number of 32 bit values in the message and are 
 * decresed consecutively so that the last value of the message has the value of
 * 0. 
 * 
 * The DSP checks the content of the buffer to meet the expectation. If the 
 * content does not match the data expected the test is marked as having 
 * "failed".
 */

/**  
 * @page testCaseDSPSendsStream Test case: The DSP sends stream data to the host
 * 
 * This page describes the test case where the DSP send stream data to the host. 
 * 
 * The test channel used for this test is a stream channel. 
 * 
 * The dsp sends a stream packet that contains a set of 32 bit values. Theses 
 * 32 bit values starts with the number of 32 bit values in the message and are 
 * decresed consecutively so that the last value of the message has the value of
 * 0. 
 * 
 * The host checks the content of the buffer whether the value in the buffer meet
 * the expectation. If the content does not match the data expected, the test is 
 * marked as having failed. 
 */

/** 
 * @page testCaseHostSendsStreamDSPReplies Test case: The host sends a stream to the DSP and the DSP replies with stream data 
 * correlated to the message it receives. 
 * 
 * This page describes the test case where the Host sends some stream data to 
 * the DSP and the DSP replies with a stream messages thats content is 
 * correlated to the input data it receives. 
 * 
 * The test channel used for this test is a stream channel. 
 * 
 * The host sends a stream packet to the DSP. The DSP receives the packet, inverts
 * each byte of the messages content and send this inverted stream data back 
 * to the host. 
 * 
 * The host checks the content of the buffer and compares it to the inverted 
 * content of the original message. If the content does not match the inverted
 * content of the original message, the test is marked as having failed. 
 */

/** 
 * @page testCaseHostSendsDiscontinuity Test case: The host sends a data packet to the DSP, marked with a data discontinuity. 
 * 
 * This page describes the test case where the host sends some packets with 
 * streaming data to the DSP. The last packet of the streaming data is 
 * marked with a discontinuity. 
 * 
 * The test channel used for this test is a stream channel. 
 * 
 * When starting the test, the host provides a parameter set containing the 
 * following parameters:
 * - The number of bytes before the discontinuity
 * The host start sending stream data. 
 * 
 * The DSP provides a buffer in a way that the last 32 bit value that the 
 * host will send with the last packet will be put somewhere in the middle 
 * of a packet, e.g. if the host sends 3 packets 1024 bytes each, the DSP 
 * provides a 4096 byte buffer so that the last data value will be placed
 * at position 3072. 
 */

/** 
 * @page testCaseDSPSendsDiscontinuity Test case: The dsp sends a data packet to the host, marked with a data discontinuity. 
 * 
 * This page describes the test case where the dsp sends some packets with 
 * streaming data to the host. The last packet of the streaming data is 
 * marked with a discontinuity. 
 * 
 * The test channel used for this test is a stream channel. 
 *
 * When starting the test, the host provides a paremeter set containing the 
 * following parameters
 * - The number of total bytes before the discontinuity
 * The dsp then starts sending stream data. 
 * 
 * The host provides a buffer in a way that the last 32 bit value that the 
 * dsp will send with the last packet will be put somewhere in the middle 
 * of a packet, e.g. if the DSP sends 3 packets 1024 bytes each, the host 
 * provides a 4096 byte buffer so that the the last data word will be placed
 * at position 3072. 
 */

/** 
 * @page testCaseHostAbort Test case: The host sends data and then aborts. 
 * 
 * This page describes the test case where the host sends some packets with 
 * streaming data to the DSP and then abort the transmission. 
 * 
 * The test channel used for this test is a stream channel. 
 * 
 * When stating the test, the host provides a parameter set containing the 
 * following parameters:
 * - The number of bytes per packet
 * - The number of packets before the abort 
 * The host start sending stream data. The DSP provides an output buffer that 
 * can hold less (!) bytes then the host will send before the abort command. 
 * After the host send all the data (they are not yet delivered because the 
 * DSP has not enough space), he will send an abort. 
 * 
 * After the abort has been send, both the DSP and the host wait for all buffers
 * to be returned to them. They wait with a timeout. When the timeout has passed
 * before all the buffers returned, the test fails. 
 */

/** 
 * @page testCaseDSPAbort Test case: The host sends data and then aborts. 
 * 
 * This page describes the test case where the DSP sends some packets with 
 * streaming data to the host and then abort the transmission. 
 * 
 * The test channel used for this test is a stream channel. 
 * 
 * When stating the test, the host provides a parameter set containing the 
 * following parameters:
 * - The number of bytes per packet
 * - The number of packets before the abort 
 * The DSP start sending stream data. The host provides an output buffer that 
 * can hold less (!) bytes then the host will send before the abort command. 
 * After the DSP send all the data (they are not yet delivered because the 
 * host has not enough space), he will send an abort. 
 * 
 * After the abort has been send, both the DSP and the host wait for all buffers
 * to be returned to them. They wait with a timeout. When the timeout has passed
 * before all the buffers returned, the test failed. 
 */

/** 
 * @page testCaseSendResponseTestOnStreamChannel Test case: The DSP sends messages on a stream channel and SH4A replies. 
 * 
 * This page describes the test case where the DSP sends command messages on a stream channel and the SH4A repiels.
 * 
 * The test channel used for this test is a stream channel. 
 * 
 * The DSP tests the received message and checks whether the content of the
 * message matches the inverted content of the message sent. If this constraint
 * holds, the test is marked as "successfull", otherwise it is marked as "failed".
 */

/**
 * @page testcaseReceiveStreamDrainReplyTest Test case: Either on the DSP or on the SH4 side are the data and buffer queues filled when the drain is called.
 *
 * Depending on the test configuration the DSP provides 15-KByte buffers and the SH4A sends 16-KByte data bevor the Drain is called or vice versa.
 * After the drain is called all pending buffers must given back to the application, otherwise the test failes.
 */

/**
 * @page testCaseDrain2 Test case: DSP provides buffers and sends data. SH4A sends drain after a specified time.
 *
 * The host provides parameters for this test:
 * -number of messages to send and buffers to provide
 * -total data to send
 * -total memory to provide
 * The dsp expects total data to send and total memory to provide to not produce a remainder when divided by  of messages and number of messages to be >0
 * The dsp calculates the expected output (number of messages data drains and buffer drains),provides and sends the requested buffers/data and waits for the abort command.
 * If the expected number of each type of drain occured, the dsp declares the test to be successfull, else it failed.
 */

/**
 * @page testCaseReceiveStreamVarReplyTestMulti Test case: Parallel duplex data transfer on eight stream channel
 *
 * The DSP registers eight stream channels and provides on each stream channel two 2-KByte buffers. The host starts sending 
 * data on each channel to the DSP. The DSP inverts the receieved data and sends the inverted data back to the host.   
 */

/**
 * @page testChannelPriority Test case: Channel priority test.
 *
 * This page describes the test case where DSP registers command and stream channels in different 
 * priority levels.
 *
 * The host provides parameters for this test:
 * -channelNumberReq: total number of requested test channels.
 * -channel: channel structure arry contains information about the channelType, 
 *           channelPriority and the channelID for each test channel.
 * -msgSize: total number of transfered bytes via a streaming channel. This parameter is only valid
 *           if the it is a streaming channel.
 * -bufSize: buffer size of the provided buffers. This parameter is only valid
 *           if the it is a streaming channel.
 *  
 * The DSP gets the total number of channels to open and for each channel the desired channelId, channelType and channelPriority.
 * The DSP additionally gets the total number of Bytes which are transfered and the Size of the Buffers which should be provided.
 * The total number of Bytes which are transfered and the Size of the Buffers which should be provided are only required for streaming channels.
 * The Test opens at least one channel within each priority range. The SH4 checks if the transfers are finished in the correct order.
 * The order dependes on the channel priority. 
 * 
 */

/**
 * @page testChannelPriorityDSPsends Test case: Channel priority test where only DSP sends data.
 *
 * This page describes the test case where DSP registers command and stream channels in different 
 * priority levels.
 *
 * The host provides parameters for this test:
 * -channelNumberReq: total number of requested test channels.
 * -channel: channel structure arry contains information about the channelType, 
 *           channelPriority and the channelID for each test channel.
 * -msgSize: total number of transfered bytes via a streaming channel. This parameter is only valid
 *           if the it is a streaming channel.
 * -bufSize: buffer size of the provided buffers. This parameter is only valid
 *           if the it is a streaming channel.
 *  
 * The DSP gets the total number of channels to open and for each channel the desired channelId, channelType and channelPriority.
 * The DSP additionally gets the total number of Bytes which should be sent in case of streaming channel.
 * The Test opens at least one channel within each priority range. The SH4 checks if the transfers are finished in the correct order.
 * The order dependes on the channel priority. 
 * 
 */

/**
 * @page testChannelPriorityhostsends Test case: Channel priority test where only host sends data.
 *
 * This page describes the test case where DSP registers command and stream channels in different 
 * priority levels.
 *
 * The host provides parameters for this test:
 * -channelNumberReq: total number of requested test channels.
 * -channel: channel structure arry contains information about the channelType, 
 *           channelPriority and the channelID for each test channel.
 * -msgSize: total number of transfered bytes via a streaming channel. This parameter is only valid
 *           if the it is a streaming channel.
 * -bufSize: buffer size of the provided buffers. This parameter is only valid
 *           if the it is a streaming channel.
 *  
 * The DSP gets the total number of channels to open and for each channel the desired channelId, channelType and channelPriority.
 * The DSP additionally gets the total number of Bytes which should be provided in case of a streaming channel.
 * The Test opens at least one channel within each priority range. The SH4 checks if the transfers are finished in the correct order.
 * The order dependes on the channel priority. 
 * 
 */

/**
 * @page flowControlTest Test case: Flow Control test for command channels.
 *
 * This page describes the test case where the host sends a command message to the 
 * DSP and the DSP replies with the inverted command message. After eight messages
 * the DSP sends a Xoff message to the host. The host still tries to send messages to the DSP.
 * As long as the dspipc internal queue overflows. After two seconds the DSP sends a Xon message
 * to the host and all messages, which stucks in the dspipc internal queue has to be processed. 
 * The host tests whether the number of messages, which are processed correctly are smaller than 
 * the entire number of messages. 
 *
 * For the test, the test channel is a command channel.
 *
 * When the test is beeing completed by the host, the DSP sends the information 
 * back whether the test has been successfull or not.  
 */

#ifndef TESTPROTOCOL_H
#define TESTPROTOCOL_H

#include "DSPIPCTypes.h"

#define TESTPROTOCOL_COMMANDCHANNEL 16
#define TESTPROTOCOL_TESTCHANNEL 17

#define TPERR_STILLRUNNING 1

#define TPERR_UNKNOWNCOMMAND 2

struct _BurstTestArgs {
        uint32_t numBursts;
        uint32_t messagesPerBurst;
        uint32_t interMessageDelayTimeNs;
        uint32_t interBurstDelayTimeNs;
        uint32_t msgReceiveDelayTimeNs;
};

typedef struct _BurstTestArgs BurstTestArgs;

struct _DiscontinuityTestArgs {
        uint32_t bytesBeforeDisc;
};

typedef struct _DiscontinuityTestArgs DiscontinuityTestArgs;

struct _DrainTestArgs {
        uint32_t sizeDataToAccept;
        uint32_t sizeDataToSend;
        uint32_t dirHostToDsp;
};
typedef struct _DrainTestArgs DrainTestArgs;

struct _DrainTestArgs_2 {
        uint32_t sizeDSPDataToProvide;
        uint32_t sizeDSPDataToSend;
        uint32_t msgNum;
};
typedef struct _DrainTestArgs_2 DrainTestArgs_2;

struct _VarMsgTestArgs {
        uint32_t msgSize;
        uint32_t loopCount;
};
typedef struct _VarMsgTestArgs VarMsgTestArgs;

struct _MultiChannelTestArgs {
        uint32_t msgSize; /* size in number of kb's. Message-Size=msgSize*1024 */
        uint32_t bufSize; /* size in number of kb's. ProvideBuffer-Size=bufSize*1024   */
        uint32_t channelNumberReq;
};
typedef struct _MultiChannelTestArgs MultiChannelTestArgs;

#define MAX_NUMBER_OF_CHANNELS 10

struct _ChannelPrioTestArgs /* total number of requested channels */
{
        uint32_t channelNumberReq;
        struct
        {
                enum {
                        CHANNELTYPE_CMD = 0,
                        CHANNELTYPE_STREAM = 1
                } channelType;

                uint32_t channelPrio;
                uint32_t channelId;
                uint32_t bufferSize;
                uint32_t numBuffer;
        } channel[MAX_NUMBER_OF_CHANNELS];

        uint32_t msgSize;      /* Only used for Streaming channels:  */
                               /* size in number of kb's. Message-Size=msgSize*1024 */
        uint32_t bufSize;      /* Only used for Streaming channels:  */
                               /* size in number of kb's. ProvideBuffer-Size=bufSize*1024  */
        uint32_t cmdMsgToSend; /* number of command messages which should be send on  */
                               /* a command channel. Must be a number between 1-16. */
};
typedef struct _ChannelPrioTestArgs ChannelPrioTestArgs;

#define SEND_BURST_TEST_MSG_CONTENT "abcdefghijklmnopqrstuvwxyz0123456789"

#define HOST_SENDS_STREAM_DATASIZE (16 * 1024)

struct _TestCommandMsg {
        enum {
                INVALID = 0,
                RESULT,
                ABORT,
                PROTOCOL_ERROR,
                TEST_HOST_SENDS_DSP_REPLIES,
                TEST_DSP_SENDS_HOST_REPLIES,
                TEST_DSP_SENDS_BURST,
                TEST_HOST_SENDS_BURST,
                TEST_DSP_SENDS_STREAM,  /*receive streaming data packet from host */
                TEST_HOST_SENDS_STREAM, /* send streaming data packet to host */
                TEST_HOST_SENDS_STREAM_DSP_REPLIES,
                TEST_HOST_SENDS_DISCONTINUITY,
                TEST_DSP_SENDS_DISCONTINUITY,
                TEST_HOST_ABORT,
                TEST_DSP_ABORT,
                TEST_ON_STREAM_DSP_SENDS_HOST_REPLIES,
                TEST_HOST_SENDS_STREAM_VAR_DSP_REPLIES,
                TEST_HOST_SENDS_STREAM_MULTI_CHANNEL,
                TEST_DRAIN,
                TEST_DRAIN_2,
                TEST_CHANNEL_PRIORITY,
                TEST_CHANNEL_PRIORITY_HOSTSENDS,
                TEST_CHANNEL_PRIORITY_DSPSENDS,
                TEST_HOST_SENDS_STREAM_DSP_REPLIES_SPECIAL,
                TEST_FLOW_CONTROL
        } cmd;

        union {
                BurstTestArgs burstTest;
                DiscontinuityTestArgs incompleteStreamArgs;
                DrainTestArgs drainTest;
                VarMsgTestArgs varmsgtest;
                MultiChannelTestArgs multichanneltest;
                DrainTestArgs_2 drainTest_2;
                ChannelPrioTestArgs channelPrioTest;

                struct
                {
                        uint32_t error;
                } result;
        } arguments;
};

typedef struct _TestCommandMsg TestCommandMsg;

#endif /* TESTPROTOCOL_H */
