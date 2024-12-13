#include <assert.h>
#include <csl.h>
#include <std.h>

#if !(DRA44XI || DRA44XE)
#include <csl_cache.h>
#endif

#include <sem.h>
#include <tsk.h>
#ifndef __QNXNTO__
#include <log.h>
#endif
#include <stdio.h>
#include <string.h>

#include "dspipclib.h"

#include "TestServer.h"
#include "test/TestProtocol.h"

#ifndef __QNXNTO__

extern LOG_Obj LOG_TestServer;

#else
#include "mem_simu.h"
#endif

/*
 * Cache line length definitions an checks
 */
#if !(DRA44XI || DRA44XE)
#define TESTSERVERCACHE_LINESIZE CACHE_L2_LINESIZE
#else
#define TESTSERVERCACHE_LINESIZE 128
#endif
typedef struct _TestServerContext {
        IPCHandle ipc;
        IPCChannelHandle cmdChannel;
        IPCChannelHandle testChannel;
        SEM_Handle testFinishedSignal;
        SEM_Handle testSignal;
        uint32_t testResult;
        TestCommandMsg pendingCmd;
        uint32_t testPending;
        uint32_t sentBytes;
        uint32_t rangeSize;
        uint32_t terminated;
        uint32_t termTest;
        uint32_t rangeIdx;

        uint32_t numReceived;
} TestServerContext;

#ifdef __QNXNTO__
extern int gSimDelayBufComplete;
#endif

/*
 * Send - response test
 */
static uint32_t sendResponseRcvMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t i;

        assert(ctx != NULL);     /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(payload != NULL); /*lint -esym(613,payload) payload is never NULL, ensured by assert */

        ctx->testResult = 1;
        for (i = 0; i < msgSize; i++) {
                if (payload[i] != (i & 0xff)) {
                        ctx->testResult = 0;
                }
        }
        SEM_post(ctx->testFinishedSignal);

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

static uint32_t sendResponseTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        uint32_t dataSize = 32;
        uint8_t data[32];
        uint32_t i;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        status = hostipc_registerControlChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &sendResponseRcvMsg, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);

        TSK_sleep(10);
        do {
                for (i = 0; i < dataSize; i++) {
                        data[i] = ~i;
                }

                status = hostipc_sendmessage(ctx->testChannel, data, dataSize, 0);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, "sendResponseTest: hostipc_sendmessage() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif

                (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER); /* wait for reply or abort */

        } while ((ctx->terminated == 0) && (ctx->termTest == 0));

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "sendResponseTest:  hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Send - response on stream channel test
 */
uint32_t sendStreamOnStreamBufferComplete(IPCStreamBuffer* buf, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        unsigned int i;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        /* check content */
        for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                uint32_t* pData = (uint32_t*)buf->ranges[rangeIdx].dataAddress;
                for (i = 0; i < buf->ranges[rangeIdx].dataSize / sizeof(uint32_t); i++) {
                        if (pData[i] != --ctx->numReceived)
                                ctx->testResult = 0;
                        if (ctx->numReceived == 0)
                                ctx->numReceived = HOST_SENDS_STREAM_DATASIZE / sizeof(uint32_t);
                }
        }

        status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "sendStreamOnStreamBufferComplete: hoststream_providebuffer()"
                                            " return value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        return 0;
}

static uint32_t sendResponseOnStreamRcvMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t i;

        assert(ctx != NULL);     /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(payload != NULL); /*lint -esym(613,payload) payload is never NULL, ensured by assert */

        ctx->testResult = 1;
        for (i = 0; i < msgSize; i++) {
                if (payload[i] != (i & 0xff))
                        ctx->testResult = 0;
        }

        SEM_post(ctx->testFinishedSignal);

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

static uint32_t sendResponseTestOnStreamChannel(TestServerContext* ctx, TestCommandMsg* testResult)
{
        uint32_t dataSize = 32;
        uint8_t data[32];
        uint32_t i;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL,
            &sendResponseOnStreamRcvMsg, &sendStreamOnStreamBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);
        do {
                for (i = 0; i < dataSize; i++) {
                        data[i] = ~i;
                }

                status = hostipc_sendmessage(ctx->testChannel, data, dataSize, 0);

#ifndef __QNXNTO__
                LOG_printf(&LOG_TestServer, "sendResponseTestOnStreamChannel: hostipc_sendmessage()"
                                            " return value %d TestServer.c Line: %d \n",
                    status, __LINE__);
#endif

                (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER); /* wait for reply or abort */

        } while ((ctx->terminated == 0) && (ctx->termTest == 0));

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "sendResponseTestOnStreamChannel: hostipc_unregisterChannel()"
                                            "return value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Receive - response test
 */

static uint32_t receiveResponseRcvMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t i;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);     /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(payload != NULL); /*lint -esym(613,payload) payload is never NULL, ensured by assert */

        /* prepare reply message */
        if (reply != NULL) /* check if reply is requested */
        {
                if (replySize != NULL) {
                        *replySize = msgSize; /* set reply size */
                }
                for (i = 0; i < msgSize; i++) {
                        reply[i] = i; /* reply a special message of 256 bytes (count 0 - 255) */
                }
        }

        /* invert received message an send it back */
        for (i = 0; i < msgSize; i++) {
                payload[i] = ~payload[i];
        }
        status = hostipc_sendmessage(ctx->testChannel, (uint8_t*)&payload[0], msgSize, 0);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "receiveResponseRcvMsg:  hostipc_sendmessage() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        ctx->testResult = 1;

        return 0;
}

static uint32_t receiveResponseTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        status = hostipc_registerControlChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &receiveResponseRcvMsg, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "receiveResponseTest:  hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Send control message burst test
 */

static uint32_t dummyReceive(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->numReceived--;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

#ifndef __QNXNTO__
#pragma DATA_ALIGN(burstContent, 4);
#endif

char burstContent[] = SEND_BURST_TEST_MSG_CONTENT;

static uint32_t sendBurstTest(TestServerContext* ctx, BurstTestArgs* args, TestCommandMsg* testResult)
{
        uint32_t burstCount;
        uint32_t endIndex;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */
        assert(args != NULL);       /*lint -esym(613,args) args is never NULL, ensured by assert */

        status = hostipc_registerControlChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &dummyReceive, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        burstCount = 0;
        endIndex = args->numBursts == 0 ? 1 : args->numBursts;
        while ((burstCount < endIndex) && (ctx->terminated == 0) && (ctx->termTest == 0)) {
                uint32_t i;

                TSK_sleep(args->interBurstDelayTimeNs / 1000000);
                for (i = 0; i < args->messagesPerBurst; i++) {
                        status = hostipc_sendmessage(ctx->testChannel, (uint8_t*)&(burstContent[0]), sizeof(SEND_BURST_TEST_MSG_CONTENT), 0);

#ifndef __QNXNTO__
                        if (status != HOSTIPC_OK) {
                                LOG_printf(&LOG_TestServer, "sendBurstTest:  hostipc_sendmessage() return"
                                                            " value %d TestServer.c Line: %d \n",
                                    status, __LINE__);
                        }
#endif

                        if (args->interMessageDelayTimeNs > 0)
                                TSK_sleep(args->interMessageDelayTimeNs / 1000000);
                }

                if (args->numBursts != 0)
                        burstCount++;
        }

        status = hostipc_unregisterChannel(ctx->testChannel);

        testResult->arguments.result.error = 0;

        return 0;
}

/*
 * Receive control message burst test
 */

static uint32_t receiveBurstRcvMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t i = 0;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        if (ctx->numReceived == 0) {
                ctx->testResult = 0;
        } else {
                ctx->numReceived--;
                if (ctx->numReceived == 0) {
                        ctx->testResult = 1;
                }
        }

        /*TSK_sleep(ctx->pendingCmd.arguments.burstTest.msgReceiveDelayTimeNs / 1000000); */
        if (reply != NULL) /* check if reply is requested */
        {
                if (replySize != NULL) {
                        *replySize = msgSize; /* set reply size */
                }
                for (i = 0; i < msgSize; i++) {
                        reply[i] = ~payload[i]; /* reply the inverted message */
                }
        } else {
                if (replySize != NULL) {
                        *replySize = 0;
                }
        }

        return 0;
}

static uint32_t receiveBurstTest(TestServerContext* ctx, BurstTestArgs* args, TestCommandMsg* testResult)
{
        uint32_t status = HOSTIPC_OK;
        ctx->numReceived = args->numBursts * args->messagesPerBurst;

        ctx->testResult = 0;

        status = hostipc_registerControlChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &receiveBurstRcvMsg, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "receiveBurstTest:  hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        return 0;
}

/********************************************************
 * Stream tests
 ********************************************************/
#define RANGESIZE (1 * 1024) /*size of provided buffer in byte  */
#define NUMBUFFERS 2
#define RANGESPERBUFFER 2
#define RANGEWORDSIZE (RANGESIZE / sizeof(uint32_t))
#define DATABUFFERSIZE ((RANGESIZE * NUMBUFFERS * RANGESPERBUFFER) / sizeof(uint32_t))
#define DATABUFFERBYTESIZE (RANGESIZE * NUMBUFFERS * RANGESPERBUFFER)
#define BUFFERBYTESIZE (RANGESIZE * RANGESPERBUFFER)

#pragma DATA_ALIGN(dataBuffer, TESTSERVERCACHE_LINESIZE);
static uint32_t dataBuffer[DATABUFFERSIZE];

struct _IPCStreamBuffer streamBuffer[NUMBUFFERS];

#pragma DATA_ALIGN(receiveMem, TESTSERVERCACHE_LINESIZE);
static uint32_t receiveMem[DATABUFFERSIZE];

struct _IPCStreamBuffer receiveBuffer[NUMBUFFERS];

/*
 * Receive stream test
 */

static uint32_t receiveStreamReceiveMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t receiveStreamBufferComplete(IPCStreamBuffer* buf, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        unsigned int i;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

#ifdef __QNXNTO__
        if (gSimDelayBufComplete != 0) {
                (void)delay(gSimDelayBufComplete);
        }
#endif

        /* check content */
        for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                uint32_t* pData = (uint32_t*)buf->ranges[rangeIdx].dataAddress;
                for (i = 0; i < buf->ranges[rangeIdx].dataSize / sizeof(uint32_t); i++) {
                        if (pData[i] != --ctx->numReceived)
                                ctx->testResult = 0;
                        if (ctx->numReceived == 0)
                                ctx->numReceived = HOST_SENDS_STREAM_DATASIZE / sizeof(uint32_t);
                }
        }

        status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "receiveStreamBufferComplete:  hoststream_providebuffer()"
                                            " return value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        return 0;
}

static uint32_t receiveStreamTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        unsigned int bufIdx;
        unsigned int rangeIdx;
        int status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        ctx->testResult = 1;

        ctx->numReceived = HOST_SENDS_STREAM_DATASIZE / sizeof(uint32_t);

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &receiveStreamReceiveMsg, &receiveStreamBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        for (bufIdx = 0; bufIdx < NUMBUFFERS; bufIdx++) {
                streamBuffer[bufIdx].numRanges = RANGESPERBUFFER;
                for (rangeIdx = 0; rangeIdx < streamBuffer[bufIdx].numRanges; rangeIdx++) {
                        streamBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&dataBuffer[(bufIdx * RANGESPERBUFFER + rangeIdx) * RANGEWORDSIZE];
                        streamBuffer[bufIdx].ranges[rangeIdx].rangeSize = RANGESIZE;
                        streamBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                        streamBuffer[bufIdx].bufferFlags = 0;
                }

                status = hoststream_providebuffer(ctx->testChannel, &streamBuffer[bufIdx]);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, "receiveStreamTest:  hoststream_providebuffer() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "receiveStreamTest:  hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Send stream test
 */

static uint32_t sendStreamReceiveMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t sendStreamBufferComplete(IPCStreamBuffer* buf, void* param)
{
        uint32_t status = HOSTIPC_OK;
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        status = hoststream_senddata(ctx->testChannel, buf);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "sendStreamBufferComplete:  hoststream_senddata() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        ctx->testResult = 1;

        return status;
}

static uint32_t sendStreamTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        unsigned int bufIdx;
        unsigned int rangeIdx;
        unsigned int i;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &sendStreamReceiveMsg, &sendStreamBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        for (i = sizeof(dataBuffer) / sizeof(uint32_t); i > 0; i--) {
                dataBuffer[i - 1] = i;
        }

        for (bufIdx = 0; bufIdx < NUMBUFFERS; bufIdx++) {
                streamBuffer[bufIdx].numRanges = RANGESPERBUFFER;
                for (rangeIdx = 0; rangeIdx < streamBuffer[bufIdx].numRanges; rangeIdx++) {
                        streamBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&dataBuffer[(bufIdx * RANGESPERBUFFER + rangeIdx) * RANGEWORDSIZE];
                        streamBuffer[bufIdx].ranges[rangeIdx].rangeSize = RANGESIZE;
                        streamBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                        streamBuffer[bufIdx].bufferFlags = 0;
                }

                status = hoststream_senddata(ctx->testChannel, &streamBuffer[bufIdx]);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendStreamTest: hoststream_senddata() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " sendStreamTest: hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Send discontinuity test
 */

static uint32_t sendDiscontinuityReceiveMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t sendDiscontinuityBufferComplete(IPCStreamBuffer* buf, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t bbdisc;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        bbdisc = ctx->pendingCmd.arguments.incompleteStreamArgs.bytesBeforeDisc;

        for (rangeIdx = 0; (rangeIdx < 2) && (ctx->sentBytes + ctx->rangeSize <= bbdisc); rangeIdx++) {
                uint32_t rs = 0;
                if (ctx->sentBytes + ctx->rangeSize >= bbdisc) {
                        rs = bbdisc - ctx->sentBytes;
                        buf->bufferFlags = BUFFERFLAG_DATA_DISCONTINOUS;
                } else {
                        rs = ctx->rangeSize;
                        buf->bufferFlags = BUFFERFLAG_NONE;
                }
                buf->ranges[rangeIdx].dataAddress = (uint32_t)&dataBuffer[(ctx->rangeIdx % 4) * 256];
                buf->ranges[rangeIdx].rangeSize = rs;
                buf->ranges[rangeIdx].dataSize = 0;
                ctx->sentBytes += rs;
                ctx->rangeIdx++;
        }

        buf->numRanges = rangeIdx;

        if (buf->bufferFlags & BUFFERFLAG_DATA_DISCONTINOUS)
                ctx->sentBytes = 0;

        status = hoststream_senddata(ctx->testChannel, buf);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " sendDiscontinuityBufferComplete: hoststream_senddata()"
                                            " return value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        ctx->testResult = 1;

        return 0;
}

static uint32_t sendDiscontinuityTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        unsigned int bufIdx;
        unsigned int rangeIdx;
        unsigned int i;

        uint32_t bbdisc;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        bbdisc = ctx->pendingCmd.arguments.incompleteStreamArgs.bytesBeforeDisc;
        ctx->rangeSize = 1024;

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &sendDiscontinuityReceiveMsg, &sendDiscontinuityBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        for (i = sizeof(dataBuffer) / sizeof(uint32_t); i > 0; --i) {
                dataBuffer[i - 1] = i;
        }

        ctx->sentBytes = 0;
        ctx->rangeIdx = 0;

        for (bufIdx = 0; bufIdx < 2; bufIdx++) {
                for (rangeIdx = 0; (rangeIdx < 2) && (ctx->sentBytes + ctx->rangeSize <= bbdisc); rangeIdx++) {
                        uint32_t rs = 0;
                        if (ctx->sentBytes + ctx->rangeSize >= bbdisc) {
                                rs = bbdisc - ctx->sentBytes;
                                streamBuffer[bufIdx].bufferFlags = BUFFERFLAG_DATA_DISCONTINOUS;
                        } else {
                                rs = ctx->rangeSize;
                                streamBuffer[bufIdx].bufferFlags = BUFFERFLAG_NONE;
                        }
                        streamBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&dataBuffer[(bufIdx * 2 + rangeIdx) * 256];
                        streamBuffer[bufIdx].ranges[rangeIdx].rangeSize = rs;
                        streamBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                        ctx->sentBytes += rs;
                        ctx->rangeIdx++;
                }

                streamBuffer[bufIdx].numRanges = rangeIdx;

                if (streamBuffer[bufIdx].bufferFlags & BUFFERFLAG_DATA_DISCONTINOUS)
                        ctx->sentBytes = 0;
                status = hoststream_senddata(ctx->testChannel, &streamBuffer[bufIdx]);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendDiscontinuityTest: hoststream_senddata() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " sendDiscontinuityTest: hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Receive discontinuity test
 */

static uint32_t receiveDiscontinuityReceiveMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t receiveDiscontinuityBufferComplete(IPCStreamBuffer* buf, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        /* check content */
        for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                if (ctx->numReceived > buf->ranges[rangeIdx].dataSize) {
                        ctx->numReceived -= buf->ranges[rangeIdx].dataSize;
                } else {
                        if (ctx->numReceived != buf->ranges[rangeIdx].dataSize) {
                                ctx->testResult = 0;
                        }
                        ctx->numReceived = ctx->pendingCmd.arguments.incompleteStreamArgs.bytesBeforeDisc;
                }
        }

        status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " receiveDiscontinuityBufferComplete: hoststream_providebuffer()"
                                            " return value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        return 0;
}

static uint32_t receiveDiscontinuityTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        unsigned int bufIdx;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        ctx->testResult = 1;

        ctx->numReceived = ctx->pendingCmd.arguments.incompleteStreamArgs.bytesBeforeDisc;

        ctx->rangeSize = 1024;
        while ((ctx->numReceived >= ctx->rangeSize) && ((ctx->numReceived % ctx->rangeSize) == 0)) {
                ctx->rangeSize -= 4;
        }

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &receiveDiscontinuityReceiveMsg, &receiveDiscontinuityBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        for (bufIdx = 0; bufIdx < 2; bufIdx++) {
                streamBuffer[bufIdx].numRanges = 2;
                for (rangeIdx = 0; rangeIdx < streamBuffer[bufIdx].numRanges; rangeIdx++) {
                        streamBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&dataBuffer[(bufIdx * 2 + rangeIdx) * 256];
                        streamBuffer[bufIdx].ranges[rangeIdx].rangeSize = ctx->rangeSize;
                        streamBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                        streamBuffer[bufIdx].bufferFlags = 0;
                }

                status = hoststream_providebuffer(ctx->testChannel, &streamBuffer[bufIdx]);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " receiveDiscontinuityTest: hoststream_providebuffer()"
                                                    " return value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " receiveDiscontinuityTest: hostipc_unregisterChannel()"
                                            " return value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Send - abort test
 */

static uint32_t sendAbortReceiveMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t sendAbortBufferComplete(IPCStreamBuffer* buf, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        status = hoststream_senddata(ctx->testChannel, buf);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " sendAbortBufferComplete: hoststream_senddata() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        ctx->testResult = 1;

        return 0;
}

static uint32_t sendAbortTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        unsigned int bufIdx;
        unsigned int rangeIdx;
        unsigned int i;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &sendStreamReceiveMsg, &sendStreamBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        for (i = 0; i < sizeof(dataBuffer) / sizeof(uint32_t); i++) {
                dataBuffer[i] = i;
        }

        for (bufIdx = 0; bufIdx < NUMBUFFERS; bufIdx++) {
                streamBuffer[bufIdx].numRanges = RANGESPERBUFFER;
                for (rangeIdx = 0; rangeIdx < streamBuffer[rangeIdx].numRanges; i++) {
                        streamBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&dataBuffer[((bufIdx * RANGESPERBUFFER) + rangeIdx) * RANGEWORDSIZE];
                        streamBuffer[bufIdx].ranges[rangeIdx].rangeSize = RANGESIZE;
                        streamBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                        streamBuffer[bufIdx].bufferFlags = BUFFERFLAG_NONE;
                }

                status = hoststream_senddata(ctx->testChannel, &streamBuffer[bufIdx]);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendAbortTest: hoststream_senddata() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " sendAbortTest: hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Send - reply stream test
 */

static uint32_t sendReplyStreamReceiveMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t sendReplyStreamBufferComplete(IPCStreamBuffer* buf, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        unsigned int i;
        unsigned int j;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        if (buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) {
                for (i = 0; i < buf->numRanges; i++) {
                        uint8_t* pData = (uint8_t*)buf->ranges[i].dataAddress;
                        for (j = 0; j < buf->ranges[i].dataSize; j++) {
                                *pData = ~*pData;
                                pData++;
                        }
                }

                status = hoststream_senddata(ctx->testChannel, buf);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendReplyStreamBufferComplete: hoststream_senddata()"
                                                    " return value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif

        }

        else if (buf->bufferFlags & BUFFERFLAG_SENDDATA) {
                status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendReplyStreamBufferComplete: hoststream_providebuffer()"
                                                    " return value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        ctx->testResult = 1;

        return 0;
}

static uint32_t receiveStreamReplyTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        unsigned int rangeIdx;
        unsigned int bufIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &sendReplyStreamReceiveMsg, &sendReplyStreamBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        for (bufIdx = 0; bufIdx < NUMBUFFERS; bufIdx++) {
                receiveBuffer[bufIdx].numRanges = RANGESPERBUFFER;
                for (rangeIdx = 0; rangeIdx < receiveBuffer[bufIdx].numRanges; rangeIdx++) {
                        receiveBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&receiveMem[(rangeIdx + RANGESPERBUFFER * bufIdx) * RANGEWORDSIZE];
                        receiveBuffer[bufIdx].ranges[rangeIdx].rangeSize = RANGESIZE;
                        receiveBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                        receiveBuffer[bufIdx].bufferFlags = BUFFERFLAG_NONE;
                }

                status = hoststream_providebuffer(ctx->testChannel, &receiveBuffer[bufIdx]);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " receiveStreamReplyTest: hoststream_providebuffer() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " receiveStreamReplyTest: hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Send - reply stream test ABEL (variable Message size)
 */
static uint32_t numDataToTransfer_start = 0;
static uint32_t numDataToTransfer = 0;
static uint32_t numLoopCount = 0;

static uint32_t sendReplyStreamVarReceiveMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t sendReplyStreamVarBufferComplete(IPCStreamBuffer* buf, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        unsigned int i;
        unsigned int j;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        if (buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) {
                /* Check Data from Host  */

                /* Invert the data */
                for (i = 0; i < buf->numRanges; i++) {
                        uint8_t* pData = (uint8_t*)buf->ranges[i].dataAddress;
                        for (j = 0; j < buf->ranges[i].dataSize; j++) {
                                *pData = ~*pData;
                                pData++;
                        }
                }
                status = hoststream_senddata(ctx->testChannel, buf);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendReplyStreamVarBufferComplete: hoststream_senddata() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif

        }

        else if (buf->bufferFlags & BUFFERFLAG_SENDDATA) {
                if (numDataToTransfer > 0) {
                        /* Check how many bytes have to be transfered */
                        if (numDataToTransfer >= BUFFERBYTESIZE) {
                                buf->numRanges = RANGESPERBUFFER;
                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        buf->ranges[rangeIdx].rangeSize = RANGESIZE;
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " sendReplyStreamVarBufferComplete: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif
                                numDataToTransfer -= BUFFERBYTESIZE;
                        } else /* if Size is smaller than 2048 */
                        {
                                /* calculate the number of 1024 ranges   */
                                if ((numDataToTransfer % RANGESIZE) == 0) {
                                        buf->numRanges = ((numDataToTransfer / RANGESIZE));
                                } else {
                                        buf->numRanges = ((numDataToTransfer / RANGESIZE) + 1);
                                }

                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        /* No new Address assignment (just setting the range size) */
                                        if (numDataToTransfer >= RANGESIZE) {
                                                buf->ranges[rangeIdx].rangeSize = RANGESIZE;
                                                numDataToTransfer -= RANGESIZE;
                                        } else {
                                                buf->ranges[rangeIdx].rangeSize = numDataToTransfer;
                                                numDataToTransfer -= numDataToTransfer;
                                        }
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                assert(numDataToTransfer == NULL); /* No data should be left to transfer */

                                status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " sendReplyStreamVarBufferComplete: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif
                        }
                } else /* a transfer was complete */
                {
                        /* start a new Transfer if needed */
                        if (numLoopCount > 0) /* still a transfer to do?? */
                        {
                                numLoopCount--;
                                /* Reset the Transfer size to start value */
                                numDataToTransfer = numDataToTransfer_start;

                                /* Check how many bytes have to be transfered */
                                if (numDataToTransfer >= BUFFERBYTESIZE) {
                                        buf->numRanges = RANGESPERBUFFER;
                                        for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                                buf->ranges[rangeIdx].rangeSize = RANGESIZE;
                                                buf->ranges[rangeIdx].dataSize = 0;
                                                buf->bufferFlags = BUFFERFLAG_NONE;
                                        }
                                        status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
                                        if (status != HOSTIPC_OK) {
                                                LOG_printf(&LOG_TestServer, " sendReplyStreamVarBufferComplete: hoststream_providebuffer()"
                                                                            " return value %d TestServer.c Line: %d \n",
                                                    status, __LINE__);
                                        }
#endif
                                        numDataToTransfer -= BUFFERBYTESIZE;
                                } else /* if Size is smaller than 2048 */
                                {
                                        /* calculate the number of 1024 ranges   */
                                        if ((numDataToTransfer % RANGESIZE) == 0) {
                                                buf->numRanges = ((numDataToTransfer / RANGESIZE));
                                        } else {
                                                buf->numRanges = ((numDataToTransfer / RANGESIZE) + 1);
                                        }

                                        for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                                /* No new Address assignment (just setting the range size) */
                                                if (numDataToTransfer >= RANGESIZE) {
                                                        buf->ranges[rangeIdx].rangeSize = RANGESIZE;
                                                        numDataToTransfer -= RANGESIZE;
                                                } else {
                                                        buf->ranges[rangeIdx].rangeSize = numDataToTransfer;
                                                        numDataToTransfer -= numDataToTransfer;
                                                }
                                                buf->ranges[rangeIdx].dataSize = 0;
                                                buf->bufferFlags = BUFFERFLAG_NONE;
                                        }
                                        assert(numDataToTransfer == NULL); /* No data should be left to transfer */

                                        status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
                                        if (status != HOSTIPC_OK) {
                                                LOG_printf(&LOG_TestServer, " sendReplyStreamVarBufferComplete: hoststream_providebuffer()"
                                                                            " return value %d TestServer.c Line: %d \n",
                                                    status, __LINE__);
                                        }
#endif
                                }
                        } else /* numLoopCount==0 */
                        {
                                if (numDataToTransfer == 0) {
                                }
                        }
                }
        }

        ctx->testResult = 1;

        return 0;
}

static uint32_t receiveStreamVarReplyTest(TestServerContext* ctx, VarMsgTestArgs* args, TestCommandMsg* testResult)
{
        unsigned int rangeIdx;
        unsigned int bufIdx = 0;
        unsigned int erg = 0;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &sendReplyStreamVarReceiveMsg, &sendReplyStreamVarBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        /* check message Size */
        numDataToTransfer = args->msgSize == 0 ? DATABUFFERBYTESIZE : args->msgSize;
        numDataToTransfer_start = numDataToTransfer; /* remember Start value */

        /* check Loop Count */
        numLoopCount = args->loopCount == 0 ? 1 : args->loopCount;

        if (numDataToTransfer >= DATABUFFERBYTESIZE) {
                /* Gen. 2 Buffer with 2k */
                for (bufIdx = 0; bufIdx < NUMBUFFERS; bufIdx++) {
                        receiveBuffer[bufIdx].numRanges = RANGESPERBUFFER;
                        for (rangeIdx = 0; rangeIdx < receiveBuffer[bufIdx].numRanges; rangeIdx++) {
                                receiveBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&receiveMem[(rangeIdx + RANGESPERBUFFER * bufIdx) * RANGEWORDSIZE];
                                receiveBuffer[bufIdx].ranges[rangeIdx].rangeSize = RANGESIZE;
                                receiveBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                                receiveBuffer[bufIdx].bufferFlags = BUFFERFLAG_NONE;
                        }
                        status = hoststream_providebuffer(ctx->testChannel, &receiveBuffer[bufIdx]);

#ifndef __QNXNTO__
                        if (status != HOSTIPC_OK) {
                                LOG_printf(&LOG_TestServer, " receiveStreamVarReplyTest: hoststream_providebuffer()"
                                                            " return value %d TestServer.c Line: %d \n",
                                    status, __LINE__);
                        }
#endif
                }
                numDataToTransfer -= DATABUFFERBYTESIZE;
        } else {
                /* Gen. 1 Buffer with < 4k       */
                /* calculate the number of 1024 ranges  */
                erg = (numDataToTransfer % RANGESIZE);
                if (erg == 0) {
                        receiveBuffer[0].numRanges = ((numDataToTransfer / RANGESIZE));
                } else {
                        receiveBuffer[0].numRanges = ((numDataToTransfer / RANGESIZE) + 1);
                }

                for (rangeIdx = 0; rangeIdx < receiveBuffer[0].numRanges; rangeIdx++) {
                        receiveBuffer[0].ranges[rangeIdx].dataAddress = (uint32_t)&receiveMem[(rangeIdx + RANGESPERBUFFER * bufIdx) * RANGEWORDSIZE];
                        if (numDataToTransfer >= RANGESIZE) {
                                receiveBuffer[0].ranges[rangeIdx].rangeSize = RANGESIZE;
                                numDataToTransfer -= RANGESIZE;
                        } else {
                                receiveBuffer[0].ranges[rangeIdx].rangeSize = numDataToTransfer;
                                numDataToTransfer -= numDataToTransfer;
                        }
                        receiveBuffer[0].ranges[rangeIdx].dataSize = 0;
                        receiveBuffer[0].bufferFlags = BUFFERFLAG_NONE;
                }
                assert(numDataToTransfer == NULL); /* No data should be left to transfer */

                if (numDataToTransfer == NULL) {
                        numLoopCount--;
                }

                status = hoststream_providebuffer(ctx->testChannel, &receiveBuffer[0]);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " receiveStreamVarReplyTest: hoststream_providebuffer()"
                                                    " return value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " receiveStreamVarReplyTest: hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * DRAIN Test - 
 */
static uint32_t numDataToAccept = 0;
static uint32_t numDataToAcceptStart = 0;
static uint32_t numDataToSend = 0;

static uint32_t sendReplyStreamDrainReceiveMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t i;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);     /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(payload != NULL); /*lint -esym(613,payload) payload is never NULL, ensured by assert */

        for (i = 0; i < msgSize; i++) {
                payload[i] = ~payload[i];
        }

        status = hostipc_sendmessage(ctx->testChannel, (uint8_t*)&payload[0], msgSize, 0);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " sendReplyStreamDrainReceiveMsg: hostipc_sendmessage() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        ctx->testResult = 0; /* no message expected */

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t sendReplyStreamDrainBufferComplete(IPCStreamBuffer* buf, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        unsigned int i;
        unsigned int rangeIdx;
        unsigned int bufIdx = 0;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

#ifdef __QNXNTO__
        if (gSimDelayBufComplete != 0) {
                (void)delay(gSimDelayBufComplete);
        }
#endif

        if ((buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) && (!(buf->bufferFlags & BUFFERFLAG_DRAINED))) {
                /* Check how many bytes have to be transfered */
                /* if more than 2K left over provide a buffer with 2 ranges  */
                /* 1K each. */
                if (numDataToAccept >= 2048) {

                        /* check content */
                        for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                uint32_t* pData = (uint32_t*)buf->ranges[rangeIdx].dataAddress;
                                for (i = 0; i < buf->ranges[rangeIdx].dataSize / sizeof(uint32_t); i++) {
                                        if (pData[i] != --ctx->numReceived)
                                                ctx->testResult = 0;
                                        if (ctx->numReceived == 0)
                                                ctx->numReceived = HOST_SENDS_STREAM_DATASIZE / sizeof(uint32_t);
                                }
                        }
                        buf->bufferFlags = BUFFERFLAG_NONE;
                        status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
                        if (status != HOSTIPC_OK) {
                                LOG_printf(&LOG_TestServer, " sendReplyStreamDrainBufferComplete: hoststream_providebuffer() return"
                                                            " value %d TestServer.c Line: %d \n",
                                    status, __LINE__);
                        }
#endif
                        numDataToAccept -= 2048;
                } else {
                        /* if less than 2K left over provide a buffer with 1 range  */
                        /* with the remaining datasize. */
                        if ((numDataToAccept < 2048) && (numDataToAccept != 0)) {
                                /* check content */
                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        uint32_t* pData = (uint32_t*)buf->ranges[rangeIdx].dataAddress;
                                        for (i = 0; i < buf->ranges[rangeIdx].dataSize / sizeof(uint32_t); i++) {
                                                if (pData[i] != --ctx->numReceived)
                                                        ctx->testResult = 0;
                                                if (ctx->numReceived == 0)
                                                        ctx->numReceived = HOST_SENDS_STREAM_DATASIZE / sizeof(uint32_t);
                                        }
                                }

                                buf->numRanges = 1;
                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        buf->ranges[rangeIdx].dataAddress = (uint32_t)&receiveMem[(rangeIdx + 2 * bufIdx) * 256];
                                        buf->ranges[rangeIdx].rangeSize = numDataToAccept;
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                status = hoststream_providebuffer(ctx->testChannel, buf);

#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " sendReplyStreamDrainBufferComplete: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif
                                numDataToAccept = 0;
                        } else {
                                /* if all data is received check the content of the last buffer. */
                                if (0 == numDataToAccept) {
                                        /* check content */
                                        for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                                uint32_t* pData = (uint32_t*)buf->ranges[rangeIdx].dataAddress;
                                                for (i = 0; i < buf->ranges[rangeIdx].dataSize / sizeof(uint32_t); i++) {
                                                        if (pData[i] != --ctx->numReceived)
                                                                ctx->testResult = 0;
                                                        if (ctx->numReceived == 0)
                                                                ctx->numReceived = HOST_SENDS_STREAM_DATASIZE / sizeof(uint32_t);
                                                }
                                        }
                                }
                        }
                }
        }
        /* if data remain on HOST side */
        if (numDataToAcceptStart < numDataToSend) {
                ctx->testResult = 1;
        } else {
                /*if data remain on DSP side a buffer has to be given back to the testserver  */
                /*with BUFFERFLAG_DRAINED set. */
                if ((buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) && ((buf->bufferFlags & BUFFERFLAG_DRAINED))) {
                        ctx->testResult = 1;
                } else {
                        ctx->testResult = 0;
                }
        }
        return 0;
}

static uint32_t receiveStreamDrainReplyTest(TestServerContext* ctx, DrainTestArgs* args, TestCommandMsg* testResult)
{
        unsigned int rangeIdx;
        unsigned int bufIdx = 0;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &sendReplyStreamDrainReceiveMsg, &sendReplyStreamDrainBufferComplete, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        /* check size of data to be accepted */
        numDataToAccept = args->sizeDataToAccept == 0 ? 2048 : args->sizeDataToAccept;
        numDataToAcceptStart = numDataToAccept;

        numDataToSend = args->sizeDataToSend;

        if (numDataToAccept >= 2048) {
                for (bufIdx = 0; bufIdx < 2; bufIdx++) {
                        receiveBuffer[bufIdx].numRanges = 2;
                        for (rangeIdx = 0; rangeIdx < receiveBuffer[bufIdx].numRanges; rangeIdx++) {
                                receiveBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&receiveMem[(rangeIdx + 2 * bufIdx) * 256];
                                receiveBuffer[bufIdx].ranges[rangeIdx].rangeSize = 1024;
                                receiveBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                                receiveBuffer[bufIdx].bufferFlags = BUFFERFLAG_NONE;
                        }
                        status = hoststream_providebuffer(ctx->testChannel, &receiveBuffer[bufIdx]);

#ifndef __QNXNTO__
                        if (status != HOSTIPC_OK) {
                                LOG_printf(&LOG_TestServer, " receiveStreamDrainReplyTest: hoststream_providebuffer() return"
                                                            " value %d TestServer.c Line: %d \n",
                                    status, __LINE__);
                        }
#endif
                }
                numDataToAccept -= 4096;
        } else {
                /* calculate the number of 1024 ranges   */
                receiveBuffer[bufIdx].numRanges = ((numDataToAccept / 1024) + 1);

                for (rangeIdx = 0; rangeIdx < receiveBuffer[bufIdx].numRanges; rangeIdx++) {
                        receiveBuffer[bufIdx].ranges[rangeIdx].dataAddress = (uint32_t)&receiveMem[(rangeIdx + 2 * bufIdx) * 256];
                        if (numDataToAccept >= 1024) {
                                receiveBuffer[bufIdx].ranges[rangeIdx].rangeSize = 1024;
                                numDataToAccept -= 1024;
                        } else {
                                receiveBuffer[bufIdx].ranges[rangeIdx].rangeSize = numDataToAccept;
                                numDataToAccept -= numDataToAccept;
                        }
                        receiveBuffer[bufIdx].ranges[rangeIdx].dataSize = 0;
                        receiveBuffer[bufIdx].bufferFlags = BUFFERFLAG_NONE;
                }
                assert(numDataToAccept == NULL); /* No data should be left to transfer */

                status = hoststream_providebuffer(ctx->testChannel, &receiveBuffer[bufIdx]);
#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " receiveStreamDrainReplyTest: hoststream_providebuffer() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " receiveStreamDrainReplyTest: hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * MultiChannel Stream Test ABEL (Send - reply)
 */
#define HEAP extHeap
/*#define HEAP intHeap */
extern int HEAP; /* Pointer to the memory shared memory segment (defined in mem_s */

static uint32_t MCbufferSizeStart = 0;
static uint32_t MCnumDataToTransfer_start = 0;

/* Number of Requested Channels */
static uint32_t numChannelRequested = 0;

/* Definition of a pointer to an array of pointer to IPCStreamBuffer-Structures */
IPCStreamBuffer** pp_rcvBuffer;

/* A Pointer to an Array with pointern to BufferMemory of 4sk */
void** ppArrayBuffer;

/* Special Structure that is given for each Channel when calling */
/* the buffer complete function */
/*  */
struct _MCTestBCParam {
        TestServerContext* ctx;           /* TestServer Context */
        uint32_t channelId;               /* Channel ID */
        uint32_t numDataToTransfer;       /* Number of Data to transfer */
                                          /*  IPCChannelHandle *ppTestChannel;     /* Pointer to IPCChannelHandle */
        IPCChannelHandle TestChannel;     /* Pointer to IPCChannelHandle   */
        IPCStreamBuffer** ppStreamBuffer; /* Pointer to Array with two pointern to Stream Buffer Structure */
};

typedef struct _MCTestBCParam MCTestBCParam;

/* A Pointer to an Array with pointern to a MCTestBCParam-Structure */
MCTestBCParam** ppMCTestBCParamArray;

static uint32_t sendReplyStreamVarReceiveMsgMulti(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t sendReplyStreamVarBufferCompleteMulti(IPCStreamBuffer* buf, void* param)
{

        MCTestBCParam* MCparam = (MCTestBCParam*)param;
        TestServerContext* ctx = (TestServerContext*)MCparam->ctx;
        unsigned int i;
        unsigned int j;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        /* Check if Callback is called with the expected buffer */
        assert((MCparam->ppStreamBuffer[0] != buf) || (MCparam->ppStreamBuffer[1] != buf));

        if (buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) {
                /* Invert the data */
                for (i = 0; i < buf->numRanges; i++) {
                        uint8_t* pData = (uint8_t*)buf->ranges[i].dataAddress;
                        assert(pData != NULL);
                        for (j = 0; j < buf->ranges[i].dataSize; j++) {
                                *pData = ~*pData;
                                pData++;
                        }
                }

                status = hoststream_senddata(MCparam->TestChannel, buf);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendReplyStreamVarBufferCompleteMulti: hoststream_senddata()"
                                                    " return value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif

        }

        else if (buf->bufferFlags & BUFFERFLAG_SENDDATA) {
                if (MCparam->numDataToTransfer > 0) {
                        /* Check how many bytes have to be transfered */
                        if (MCparam->numDataToTransfer >= 2048) {
                                buf->numRanges = RANGESPERBUFFER;
                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        buf->ranges[rangeIdx].rangeSize = 1024;
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                status = hoststream_providebuffer(MCparam->TestChannel, buf);
#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " sendReplyStreamVarBufferCompleteMulti: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif

                                MCparam->numDataToTransfer -= 2048;
                        } else /* if Size is smaller than 2048 */
                        {
                                /* calculate the number of 1024 ranges   */
                                if ((MCparam->numDataToTransfer % 1024) == 0) {
                                        buf->numRanges = ((MCparam->numDataToTransfer / 1024));
                                } else {
                                        buf->numRanges = ((MCparam->numDataToTransfer / 1024) + 1);
                                }

                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        /* No new Address assignment (just setting the range size) */
                                        if (MCparam->numDataToTransfer >= 1024) {
                                                buf->ranges[rangeIdx].rangeSize = 1024;
                                                MCparam->numDataToTransfer -= 1024;
                                        } else {
                                                buf->ranges[rangeIdx].rangeSize = MCparam->numDataToTransfer;
                                                MCparam->numDataToTransfer = 0;
                                        }
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                assert(MCparam->numDataToTransfer == 0); /* No data should be left to transfer */

                                status = hoststream_providebuffer(MCparam->TestChannel, buf);
#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " sendReplyStreamVarBufferCompleteMulti: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif
                        }
                }
        }

        /*   ctx->testResult = 1; */

        return 0;
}

static uint32_t receiveStreamVarReplyTestMulti(TestServerContext* ctx, MultiChannelTestArgs* args, TestCommandMsg* testResult)
{
        unsigned int rangeIdx;
        unsigned int bufIdx = 0;
        int i = 0;
        IPCStreamBuffer* pSBuf;
        uint32_t* pArr;
        uint32_t status = HOSTIPC_OK;
        uint32_t offsetVar = 256;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        ctx->testResult = 1; /* Default = OK */

        /* check message Size (for each channel) */
        /* Remember Start value for all Channels */
        MCnumDataToTransfer_start = args->msgSize == 0 ? (100 * 1024) : (args->msgSize * 1024); /* Default 100k */

        /* Set Size of Buffers. (min. 4k) */
        if ((args->bufSize % 4) == 0) /* Immer in 4k Blöcken
   {
      /* check buffer Size (for each channel) */
                /* Remember Start value for all Channels */
                MCbufferSizeStart = args->bufSize < 4 ? (4 * 1024) : (args->bufSize * 1024); /* Default 4k */
        /* Set Offset Generator Variable */
        offsetVar = ((uint32_t)(args->bufSize / 4)) * 256;
}
else
{
        ctx->testResult = 0;          /* Wrong Buffer Request !!!! */
        MCbufferSizeStart = 4 * 1024; /* Default 4k */
}

/* check for Maximum of requested Steam Channel (min. 1 Channel) */
numChannelRequested = args->channelNumberReq == 0 ? 1 : args->channelNumberReq; /* Default 1 Channel */

/* (1) Allocate Memory for Array of pointern to MCTestBCParam-Structures */
ppMCTestBCParamArray = MEM_alloc(HEAP, numChannelRequested * sizeof(MCTestBCParam*), TESTSERVERCACHE_LINESIZE);
assert(ppMCTestBCParamArray != MEM_ILLEGAL);
/* Open the requested Number of Channels */
for (i = 0; i < numChannelRequested; i++) {
        /* (2) Allocate MCTestBCParam-Structure */
        ppMCTestBCParamArray[i] = MEM_alloc(HEAP, sizeof(MCTestBCParam), TESTSERVERCACHE_LINESIZE);
        assert(ppMCTestBCParamArray[i] != MEM_ILLEGAL);
        ppMCTestBCParamArray[i]->ctx = ctx;
        ppMCTestBCParamArray[i]->channelId = TESTPROTOCOL_TESTCHANNEL + 1 + i;
        ppMCTestBCParamArray[i]->numDataToTransfer = MCnumDataToTransfer_start;

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL + 1 + i,
            &sendReplyStreamVarReceiveMsgMulti,
            &sendReplyStreamVarBufferCompleteMulti,
            ppMCTestBCParamArray[i],
            &(ppMCTestBCParamArray[i]->TestChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(2);
}

/* (3) Allocate 2 IPCStreamBuffer-Structures for all Channels */
for (i = 0; i < numChannelRequested; i++) {
        /* Allo the Array of Pointer */
        ppMCTestBCParamArray[i]->ppStreamBuffer = MEM_alloc(HEAP, 2 * sizeof(IPCStreamBuffer*), TESTSERVERCACHE_LINESIZE);
        assert(ppMCTestBCParamArray[i]->ppStreamBuffer != MEM_ILLEGAL);

        ppMCTestBCParamArray[i]->ppStreamBuffer[0] = MEM_alloc(HEAP, sizeof(IPCStreamBuffer), TESTSERVERCACHE_LINESIZE);
        assert(ppMCTestBCParamArray[i]->ppStreamBuffer[0] != MEM_ILLEGAL);

        ppMCTestBCParamArray[i]->ppStreamBuffer[1] = MEM_alloc(HEAP, sizeof(IPCStreamBuffer), TESTSERVERCACHE_LINESIZE);
        assert(ppMCTestBCParamArray[i]->ppStreamBuffer[1] != NULL);
}

/* (4) allocate an Array to hold the startaddresses for all allocated buffers */
ppArrayBuffer = MEM_alloc(HEAP, numChannelRequested * sizeof(void*), TESTSERVERCACHE_LINESIZE);
assert(ppArrayBuffer != MEM_ILLEGAL);

/* generate & provide Buffer for all Channels */
for (i = 0; i < numChannelRequested; i++) {
        /* (5) Allocate Buffer for all Channels (remember the start address) */
        ppArrayBuffer[i] = MEM_alloc(HEAP, MCbufferSizeStart, TESTSERVERCACHE_LINESIZE);
        assert(ppArrayBuffer[i] != MEM_ILLEGAL);

        pArr = (uint32_t*)ppArrayBuffer[i];

        /* Gen. 2 Buffers with 2 Ranges with a size of MCbufferSizeStart/4 */
        for (bufIdx = 0; bufIdx < 2; bufIdx++) {
                /* get pointer to IPCStreamBuffer structure */
                pSBuf = ppMCTestBCParamArray[i]->ppStreamBuffer[bufIdx];
                pSBuf->numRanges = 2;
                for (rangeIdx = 0; rangeIdx < pSBuf->numRanges; rangeIdx++) {
                        pSBuf->ranges[rangeIdx].dataAddress = (uint32_t)&pArr[(rangeIdx + 2 * bufIdx) * offsetVar];
                        pSBuf->ranges[rangeIdx].rangeSize = MCbufferSizeStart / 4;
                        pSBuf->ranges[rangeIdx].dataSize = 0;
                        pSBuf->bufferFlags = BUFFERFLAG_NONE;
                }
                status = hoststream_providebuffer(ppMCTestBCParamArray[i]->TestChannel, pSBuf);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " receiveStreamVarReplyTestMulti: hoststream_providebuffer()"
                                                    " return value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }
        ppMCTestBCParamArray[i]->numDataToTransfer -= MCbufferSizeStart;

} /* end for buffer generation */

/* Wait for END of Test */
(void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

/* Unregister all Channels (Only allowd in reverse order???) */
for (i = (numChannelRequested - 1); i >= 0; i--) {
        status = hostipc_unregisterChannel(ppMCTestBCParamArray[i]->TestChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " receiveStreamVarReplyTestMulti: hostipc_unregisterChannel()"
                                            " return value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif
}

/*---------------------------------------- */
/* free all allocated Memory */
/*---------------------------------------- */
for (i = 0; i < numChannelRequested; i++) {
        /* (5) Deallocate Buffer for all Channels  */
        MEM_free(HEAP, ppArrayBuffer[i], MCbufferSizeStart);

        /* (3) deallocate IPCStreamBuffer-Structures for all Channels */
        MEM_free(HEAP, ppMCTestBCParamArray[i]->ppStreamBuffer[0], sizeof(IPCStreamBuffer));
        MEM_free(HEAP, ppMCTestBCParamArray[i]->ppStreamBuffer[1], sizeof(IPCStreamBuffer));
        MEM_free(HEAP, ppMCTestBCParamArray[i]->ppStreamBuffer, 2 * sizeof(IPCStreamBuffer*)); /*/*/
        *!!!!!!!!!!!!!* /

            /* (2) Allocate MCTestBCParam-Structure */
            MEM_free(HEAP, ppMCTestBCParamArray[i], sizeof(MCTestBCParam));
}

/* (1) Deallocate Memory for Array of pointern to MCTestBCParam-Structures */
MEM_free(HEAP, ppMCTestBCParamArray, numChannelRequested * sizeof(MCTestBCParam*));

/* (4) Deallocate an Array to hold the startaddresses for all allocated buffers */
MEM_free(HEAP, ppArrayBuffer, numChannelRequested * sizeof(void*));

testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

return 0;
}

/*
 * TEST_HOST_SENDS_STREAM_DSP_REPLIES_SPECIAL ABEL (Send - reply)
 * This Test creates 2 Channels, one for recieving one for sending Data.
 * Data that is received will be inverted and send back.
 */
/*#define HEAP intHeap */
extern int HEAP; /* Pointer to the memory shared memory segment (defined in mem_s */

struct _SRSTestBCParamSpecial {
        TestServerContext* ctx;           /* TestServer Context */
        uint32_t channelId;               /* Channel ID */
        uint32_t recieveChannelId;        /* Recieve Channel ID   */
        uint32_t sendChannelId;           /* Send Channel ID     */
        uint32_t numDataToTransfer;       /* Number of Data to transfer */
        IPCChannelHandle TestChannel;     /* Pointer to IPCChannel   */
        IPCStreamBuffer** ppStreamBuffer; /* Pointer to Array with two pointern to Stream Buffer Structure */

        uint32_t dataBuffer[1024];       /* Data buffer 4k */
        IPCStreamBuffer streamBuffer[2]; /* Streambuffer  */
};

typedef struct _SRSTestBCParamSpecial SRSTestBCParamSpecial;
static SRSTestBCParamSpecial SRSTestParamSpecial[2];

static uint32_t sendReplyStreamReceiveMsgSpecial(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->testResult = 1;
        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

uint32_t sendReplyStreamBufferCompleteSpecial(IPCStreamBuffer* buf, void* param)
{
        SRSTestBCParamSpecial* SRSparam = (SRSTestBCParamSpecial*)param;
        TestServerContext* ctx = (TestServerContext*)SRSparam->ctx;
        unsigned int i;
        unsigned int j;
        uint32_t status = HOSTIPC_OK;
        int index; /* Index for accessing the SRSTestParamSpecial-Array */

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        index = SRSparam->channelId - TESTPROTOCOL_TESTCHANNEL;
        assert((index >= NULL) && (index <= 1));

        /* Check if Callback is called with the expected buffer */
        assert((&SRSparam->streamBuffer[0] != buf) || (&SRSparam->streamBuffer[1] != buf));

        if (buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) {
                /* Invert the data */
                for (i = 0; i < buf->numRanges; i++) {
                        uint8_t* pData = (uint8_t*)buf->ranges[i].dataAddress;
                        assert(pData != NULL);
                        for (j = 0; j < buf->ranges[i].dataSize; j++) {
                                *pData = ~*pData;
                                pData++;
                        }
                }
                /* Send data to channel TESTPROTOCOL_TESTCHANNEL+1 */
                status = hoststream_senddata(SRSTestParamSpecial[1].TestChannel, buf);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendReplyStreamBufferCompleteSpecial: hoststream_senddata()"
                                                    " return value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        } else if (buf->bufferFlags & BUFFERFLAG_SENDDATA) {
                /* Receive data on channel TESTPROTOCOL_TESTCHANNEL */
                status = hoststream_providebuffer(SRSTestParamSpecial[0].TestChannel, buf);
#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " sendReplyStreamBufferCompleteSpecial: hoststream_providebuffer()"
                                                    " return value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
        }

        ctx->testResult = 1;

        return 0;
}

static uint32_t receiveStreamReplyTestSpecial(TestServerContext* ctx, TestCommandMsg* testResult)
{
        unsigned int rangeIdx;
        unsigned int bufIdx = 0;
        int i = 0;
        IPCStreamBuffer* pSBuf;
        uint32_t* pArr;
        uint32_t status = HOSTIPC_OK;
        uint32_t SRSnumDataToTransfer_start = 0;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        /* check message Size (for each channel) */
        /* Remember Start value for all Channels */
        SRSnumDataToTransfer_start = 0; /* args->msgSize == 0 ? (100*1024) : (args->msgSize*1024); /* Default 100k */

        /* Open the 2 Channels  */
        SRSTestParamSpecial[0].ctx = ctx;
        SRSTestParamSpecial[0].channelId = TESTPROTOCOL_TESTCHANNEL;
        SRSTestParamSpecial[0].numDataToTransfer = SRSnumDataToTransfer_start;

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL,
            &sendReplyStreamReceiveMsgSpecial,
            &sendReplyStreamBufferCompleteSpecial,
            &(SRSTestParamSpecial[0]),
            &(SRSTestParamSpecial[0].TestChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        SRSTestParamSpecial[1].ctx = ctx;
        SRSTestParamSpecial[1].channelId = TESTPROTOCOL_TESTCHANNEL + 1;
        SRSTestParamSpecial[1].numDataToTransfer = SRSnumDataToTransfer_start;

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL + 1,
            &sendReplyStreamReceiveMsgSpecial,
            &sendReplyStreamBufferCompleteSpecial,
            &(SRSTestParamSpecial[1]),
            &(SRSTestParamSpecial[1].TestChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        /* generate & provide Buffer for the 2 Channels */
        for (i = 0; i < 2; i++) {
                pArr = (uint32_t*)&(SRSTestParamSpecial[i].dataBuffer);

                /* Gen. 2 Buffers with 2 Ranges with a size of 4k */
                for (bufIdx = 0; bufIdx < NUMBUFFERS; bufIdx++) {
                        /* get pointer to IPCStreamBuffer structure */
                        pSBuf = &(SRSTestParamSpecial[i].streamBuffer[bufIdx]);
                        pSBuf->numRanges = RANGESPERBUFFER;
                        for (rangeIdx = 0; rangeIdx < pSBuf->numRanges; rangeIdx++) {
                                pSBuf->ranges[rangeIdx].dataAddress = (uint32_t)&pArr[(rangeIdx + 2 * bufIdx) * RANGEWORDSIZE];
                                pSBuf->ranges[rangeIdx].rangeSize = RANGESIZE;
                                pSBuf->ranges[rangeIdx].dataSize = 0;
                                pSBuf->bufferFlags = BUFFERFLAG_NONE;
                        }
                        status = hoststream_providebuffer(SRSTestParamSpecial[i].TestChannel, pSBuf);

#ifndef __QNXNTO__
                        if (status != HOSTIPC_OK) {
                                LOG_printf(&LOG_TestServer, " receiveStreamReplyTestSpecial: hoststream_providebuffer()"
                                                            "return value %d TestServer.c Line: %d \n",
                                    status, __LINE__);
                        }
#endif
                }
        } /* end for buffer generation */

        /* Wait for END of Test */
        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        /* Unregister the 2 Channels  */
        status = hostipc_unregisterChannel(&(SRSTestParamSpecial[0].TestChannel));

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " receiveStreamReplyTestSpecial: hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        status = hostipc_unregisterChannel(&(SRSTestParamSpecial[1].TestChannel));

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, " receiveStreamReplyTestSpecial: hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*struct that defines the result of this test */
typedef struct TestDrain2Result_ {
        uint32_t countDrainedProvideCallbacks;
        uint32_t countDrainedDataCallbacks;
        /* uint32_t dataLeftAfterDrain; */
} TestDrain2Result;
/*init struct members to 0 */
static void initTestDrain2Result(TestDrain2Result* initMe)
{
        initMe->countDrainedProvideCallbacks = 0;
        initMe->countDrainedDataCallbacks = 0;
}
/*checks equality of these structs */
static uint32_t isEqualTestDrain2Result(TestDrain2Result* a, TestDrain2Result* b)
{
        if (
            (a->countDrainedProvideCallbacks == b->countDrainedProvideCallbacks)
            && (a->countDrainedDataCallbacks == b->countDrainedDataCallbacks)) {
                return 1;
        } else {
                return 0;
        }
}
/*bc callback for drain2 test, updates TestDrainResult structure */
static uint32_t bufferCompleteTestDrain2(IPCStreamBuffer* buf, void* param)
{
        TestDrain2Result* shared = (TestDrain2Result*)param;
        if (buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) {
                if (buf->bufferFlags & BUFFERFLAG_DRAINED) {
                        shared->countDrainedProvideCallbacks++;
                }
        } else if (buf->bufferFlags & BUFFERFLAG_SENDDATA) {
                if (buf->bufferFlags & BUFFERFLAG_DRAINED) {
                        shared->countDrainedDataCallbacks++;
                }
        }
        return 0;
}

/*sc callback for testDrain2, this function doesnt do anything */
static uint32_t sendCompleteTestDrain2(uint32_t msgSize, uint8_t* msg, uint32_t* replySize, uint8_t* reply, void* param)
{
        return 0;
}

static uint32_t testDrain2(TestServerContext* ctx, DrainTestArgs_2* args, TestCommandMsg* testResult)
{
        uint32_t status = HOSTIPC_OK;
        uint32_t bytesToSendEachTime;               /*how many bytes does one send message contain */
        uint32_t bytesToProvideEachTime;            /*how many bytes does one received message contain */
        uint8_t dummySendBufferMemory[1 * 1024];    /*data is not looked at, so we have one data region only for all IPCStreamBuffers of a type (receive, send) */
        uint8_t dummyReceiveBufferMemory[1 * 1024]; /*data is not looked at, so we have one data region only for all IPCStreamBuffers of a type (receive, send) */
        IPCStreamBuffer* dummySendBuffers;          /*pointer to send buffers - how many are needed is transmitted by sh4 (test parameter) */
        IPCStreamBuffer* dummyReceiveBuffers;       /*pointer to receive buffers - how many are needed is transmitted by sh4 (test parameter) */
        TestDrain2Result sharedResult;              /*used by the callbacks to log what IS happening */
        TestDrain2Result expectedResult;            /*calculate what we would expect to happen */
        uint32_t iter;                              /*iterate */

        /*assertions: */
        /*no pointer shall be NULL */
        /*theres more than 0 messages */
        /*no message will be larger than 4K or equal to 0 */
        /*the total amount of data to be sent/provided can be devided by numMsg without remainder */
        assert(args != NULL);
        assert(ctx != NULL);
        assert(testResult != NULL);
        assert(args->msgNum);
        assert(args->sizeDSPDataToProvide);
        assert(args->sizeDSPDataToSend);
        bytesToSendEachTime = args->sizeDSPDataToSend / args->msgNum;
        bytesToProvideEachTime = args->sizeDSPDataToProvide / args->msgNum;
        assert(bytesToProvideEachTime * args->msgNum == args->sizeDSPDataToProvide);
        assert(bytesToSendEachTime * args->msgNum == args->sizeDSPDataToSend);
        assert(bytesToSendEachTime <= 4 * 1024);
        assert(bytesToProvideEachTime <= 4 * 1024);

        /*set members zero */
        initTestDrain2Result(&expectedResult);
        initTestDrain2Result(&sharedResult);

        expectedResult.countDrainedDataCallbacks = args->msgNum;
        expectedResult.countDrainedProvideCallbacks = args->msgNum;

        dummySendBuffers = MEM_alloc(HEAP, args->msgNum * sizeof(IPCStreamBuffer), TESTSERVERCACHE_LINESIZE);
        dummyReceiveBuffers = MEM_alloc(HEAP, args->msgNum * sizeof(IPCStreamBuffer), TESTSERVERCACHE_LINESIZE);
        /*currently all receivers point to the same receiver memory, senders have memory of their own (but shared among them) */
        for (iter = 0; iter < args->msgNum; iter++) {
                dummySendBuffers[iter].numRanges = 1;
                dummySendBuffers[iter].ranges[0].dataAddress = (uint32_t)dummySendBufferMemory;
                dummyReceiveBuffers[iter].numRanges = 1;
                dummyReceiveBuffers[iter].ranges[0].dataAddress = (uint32_t)dummyReceiveBufferMemory;
        }

        status = hostipc_registerStreamChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, sendCompleteTestDrain2, bufferCompleteTestDrain2, &sharedResult, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10); /* must wait for SH4 to ack "attached" */

        /*put all data to send on the channel */

        /*try to send data and provide buffers as dictated by SH4` wishess (none of the messages and none of the buffers will be actually used, they`l all be drained) */
        for (iter = 0; iter < args->msgNum; iter++) {
                dummySendBuffers[iter].ranges[0].dataSize = bytesToSendEachTime;
                dummySendBuffers[iter].ranges[0].rangeSize = bytesToSendEachTime;
                status = hoststream_senddata(ctx->testChannel, &(dummySendBuffers[iter]));
                assert(status == HOSTIPC_OK);
                dummyReceiveBuffers[iter].ranges[0].dataSize = bytesToProvideEachTime;
                dummyReceiveBuffers[iter].ranges[0].rangeSize = bytesToProvideEachTime;
                status = hoststream_providebuffer(ctx->testChannel, &(dummyReceiveBuffers[iter]));
                assert(status == HOSTIPC_OK);
        }

        /*do some waiting to allow for drain to happen */
        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        /*check wheter loop was exited because of equality or lack of patience */
        if (isEqualTestDrain2Result(&expectedResult, &sharedResult)) {
                testResult->arguments.result.error = 0;
        } else {
                testResult->arguments.result.error = 1;
        }

        /*cleanup used buffers */
        MEM_free(HEAP, dummySendBuffers, args->msgNum * sizeof(IPCStreamBuffer));
        MEM_free(HEAP, dummyReceiveBuffers, args->msgNum * sizeof(IPCStreamBuffer));
        /*cleanup channel used */
        status = hostipc_unregisterChannel(ctx->testChannel);

        return status;
}

/*
 * channel Priority test 
 */
#define NUMBUFFERPERCHANNEL 4
#define CHANNELPRIORANGESPERBUFFER 1
#define NUMCOMMANDMSGTOSEND 16
extern int extHeap;

static uint32_t channelPrioBufferSizeStart = 0;
static uint32_t channelPrioNumDataToTransfer_start = 0;

/* Special Structure that is given for each Channel when calling */
/* the buffer complete function */
/*  */
struct _channelPrioTestParam {
        TestServerContext* ctx;     /* TestServer Context */
        uint32_t channelId;         /* Channel ID */
        uint32_t numDataToTransfer; /* Number of Data to transfer */
        uint32_t bufferSize;
        uint32_t CMDnumReceived;          /* number of Command Messages to recieve */
        uint32_t CMDtestResult;           /* 1:OK   0: Error */
        IPCChannelHandle testChannel;     /* Pointer to IPCChannelHandle   */
        IPCStreamBuffer** ppStreamBuffer; /* Pointer to Array with two pointern to Stream Buffer Structure */
};

typedef struct _channelPrioTestParam channelPrioTestParam;

static uint32_t initChannelPrioTestStruct(channelPrioTestParam** channelPrioTestParamArray, ChannelPrioTestArgs* args)
{
        uint32_t i;

        for (i = 0; i < args->channelNumberReq; i++) {
                channelPrioTestParamArray[i] = MEM_alloc(extHeap, sizeof(channelPrioTestParam), TESTSERVERCACHE_LINESIZE);
                assert(channelPrioTestParamArray[i] != MEM_ILLEGAL);
                channelPrioTestParamArray[i]->ctx = NULL;
                channelPrioTestParamArray[i]->channelId = 0;
                channelPrioTestParamArray[i]->numDataToTransfer = 0;
                channelPrioTestParamArray[i]->testChannel = NULL;
                channelPrioTestParamArray[i]->ppStreamBuffer = NULL;
        }

        channelPrioNumDataToTransfer_start = 0;
        channelPrioBufferSizeStart = 0;

        return HOSTIPC_OK;
}

static uint32_t openChannelForPrioTest(TestServerContext* ctx,
    ChannelPrioTestArgs* args,
    channelPrioTestParam** channelPrioTestParamArray,
    BufferCompleteCallback bufferCB,
    ReceiveCallback msgCB)
{
        uint32_t i;
        uint32_t status = HOSTIPC_OK;
        /* open all requested test channels. */
        for (i = 0; i < args->channelNumberReq; i++) {
                if (args->channel[i].channelType == CHANNELTYPE_CMD) {

                        channelPrioTestParamArray[i]->ctx = ctx;
                        channelPrioTestParamArray[i]->channelId = args->channel[i].channelId;
                        channelPrioTestParamArray[i]->CMDnumReceived = 16; /* is now set to 16 for every Thread/channel */
                        channelPrioTestParamArray[i]->CMDtestResult = 0;   /* at the beginnig error */
                        status = hostipc_registerPrioControlChannel(ctx->ipc, args->channel[i].channelId,
                            msgCB,
                            channelPrioTestParamArray[i],
                            &channelPrioTestParamArray[i]->testChannel,
                            args->channel[i].channelPrio);
                        assert(status == HOSTIPC_OK);
                } else if (args->channel[i].channelType == CHANNELTYPE_STREAM) {
                        /*        /* check message Size (for each channel) */
                        /* Remember Start value for all Channels */
                        channelPrioNumDataToTransfer_start = args->msgSize == 0 ? (100 * 1024) : (args->msgSize * 1024); /* Default 100k */

                        /* Set Size of Buffers. (min. 4k) */
                        if ((args->bufSize % 2) == 0) {
                                /* check buffer Size (for each channel) */
                                /* Remember Start value for all Channels */
                                channelPrioBufferSizeStart = args->bufSize < 4 ? (4 * 1024) : (args->bufSize * 1024); /* Default 4k */
                        } else {
                                /* check buffer Size (for each channel) */
                                /* Remember Start value for all Channels */
                                channelPrioBufferSizeStart = args->bufSize < 4 ? (4 * 1024) : ((args->bufSize + 1) * 1024); /* Default 4k */
                        }

                        * / channelPrioTestParamArray[i]->ctx = ctx;
                        channelPrioTestParamArray[i]->channelId = args->channel[i].channelId;
                        channelPrioTestParamArray[i]->numDataToTransfer = args->msgSize;
                        status = hostipc_registerPrioStreamChannel(ctx->ipc, args->channel[i].channelId,
                            msgCB,
                            bufferCB,
                            channelPrioTestParamArray[i],
                            &channelPrioTestParamArray[i]->testChannel,
                            args->channel[i].channelPrio);
                        if (status != HOSTIPC_OK)
                                assert(status == HOSTIPC_OK);
                }
        }

        return HOSTIPC_OK;
}

static uint32_t freeMemChannelPrioTest(ChannelPrioTestArgs* args,
    channelPrioTestParam** channelPrioTestParamArray,
    void** channelPrioArrayBuffer)
{
        uint32_t i;
        uint32_t bufIdx;

        /* free all allocated Memory */
        /*---------------------------------------- */
        for (i = 0; i < args->channelNumberReq; i++) {
                if (args->channel[i].channelType == CHANNELTYPE_STREAM) {
                        /* (5) Deallocate Buffer for all streaming Channels  */
                        MEM_free(HEAP, channelPrioArrayBuffer[i], (args->channel[i].numBuffer * args->channel[i].bufferSize));

                        /* (3) deallocate IPCStreamBuffer-Structures for all Channels */
                        for (bufIdx = 0; bufIdx < args->channel[i].numBuffer; bufIdx++) {
                                MEM_free(HEAP, channelPrioTestParamArray[i]->ppStreamBuffer[bufIdx], sizeof(IPCStreamBuffer));
                        }

                        MEM_free(HEAP, channelPrioTestParamArray[i]->ppStreamBuffer, args->channel[i].numBuffer * sizeof(IPCStreamBuffer*));
                }
                /* (2) Allocate MCTestBCParam-Structure */
                MEM_free(HEAP, channelPrioTestParamArray[i], sizeof(channelPrioTestParam));
        }

        /*Free all allocated memory */
        MEM_free(HEAP, channelPrioTestParamArray, args->channelNumberReq * sizeof(channelPrioTestParam*));

        /* (4) Deallocate an Array to hold the startaddresses for all allocated buffers */
        MEM_free(HEAP, channelPrioArrayBuffer, args->channelNumberReq * sizeof(void*));

        return HOSTIPC_OK;
}

static uint32_t closeChannelOfPrioTest(ChannelPrioTestArgs* args,
    channelPrioTestParam** channelPrioTestParamArray)
{
        int32_t i;
        uint32_t status = HOSTIPC_OK;
        /* Unregister all Channels */
        for (i = (args->channelNumberReq - 1); i >= 0; i--) {
                status = hostipc_unregisterChannel(channelPrioTestParamArray[i]->testChannel);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " %s: hostipc_unregisterChannel() return value %d"
                                                    " TestServer.c Line: %d \n",
                            __FUNCTION__, status);
                }
#endif
        }

        return status;
}

static uint32_t channelPriorityReceiveMsgCB(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        uint32_t status = HOSTIPC_OK;
        uint32_t i;
        channelPrioTestParam* ctx = (channelPrioTestParam*)param;

        assert(ctx != NULL);     /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(payload != NULL); /*lint -esym(613,payload) payload is never NULL, ensured by assert */

        for (i = 0; i < msgSize; i++) {
                payload[i] = ~payload[i];
        }

        status = hostipc_sendmessage(ctx->testChannel, (uint8_t*)&payload[0], msgSize, 0);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "channelPriorityReceiveMsgCB:  hostipc_sendmessage() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        ctx->ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return status;
}

static uint32_t channelPriorityBufferComplete(IPCStreamBuffer* buf, void* param)
{
        channelPrioTestParam* channelPrioParam = (channelPrioTestParam*)param;
        TestServerContext* ctx = (TestServerContext*)channelPrioParam->ctx;
        unsigned int i;
        unsigned int j;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        if (buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) {
                /* Invert the data */
                for (i = 0; i < buf->numRanges; i++) {
                        uint8_t* pData = (uint8_t*)buf->ranges[i].dataAddress;
                        assert(pData != NULL);
                        for (j = 0; j < buf->ranges[i].dataSize; j++) {
                                *pData = ~*pData;
                                pData++;
                        }
                }
#ifndef __QNXNTO__
                LOG_printf(&LOG_TestServer, "%s: BUFFERFLAG_PROVIDEBUFFER; channelID %u.", __FUNCTION__, channelPrioParam->channelId);
#endif

                status = hoststream_senddata(channelPrioParam->testChannel, buf);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " channelPriorityBufferComplete: hoststream_senddata() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif

        }

        else if (buf->bufferFlags & BUFFERFLAG_SENDDATA) {

#ifndef __QNXNTO__
                LOG_printf(&LOG_TestServer, "%s: BUFFERFLAG_SENDDATA; channelID %u.", __FUNCTION__, channelPrioParam->channelId);
#endif

                if (channelPrioParam->numDataToTransfer > 0) {
                        /* Check how many bytes have to be transfered */
                        if (channelPrioParam->numDataToTransfer >= channelPrioBufferSizeStart) {
                                buf->numRanges = CHANNELPRIORANGESPERBUFFER;
                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        buf->ranges[rangeIdx].rangeSize = channelPrioBufferSizeStart / buf->numRanges;
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                status = hoststream_providebuffer(channelPrioParam->testChannel, buf);
#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " channelPriorityBufferComplete: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif

                                channelPrioParam->numDataToTransfer -= channelPrioBufferSizeStart;
                        } else /* if Size is smaller than 2048 */
                        {
                                /* calculate the number of 1024 ranges   */
                                if ((channelPrioParam->numDataToTransfer % 1024) == 0) {
                                        buf->numRanges = ((channelPrioParam->numDataToTransfer / 1024));
                                } else {
                                        buf->numRanges = ((channelPrioParam->numDataToTransfer / 1024) + 1);
                                }

                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        /* No new Address assignment (just setting the range size) */
                                        if (channelPrioParam->numDataToTransfer >= 1024) {
                                                buf->ranges[rangeIdx].rangeSize = 1024;
                                                channelPrioParam->numDataToTransfer -= 1024;
                                        } else {
                                                buf->ranges[rangeIdx].rangeSize = channelPrioParam->numDataToTransfer;
                                                channelPrioParam->numDataToTransfer = 0;
                                        }
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                assert(channelPrioParam->numDataToTransfer == 0); /* No data should be left to transfer */

                                status = hoststream_providebuffer(channelPrioParam->testChannel, buf);
#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " channelPriorityBufferComplete: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif
                        }
                }
        }

        ctx->testResult = 1;

        return status;
}

static uint32_t channelPriorityTest(TestServerContext* ctx, ChannelPrioTestArgs* args, TestCommandMsg* testResult)
{
        int i;
        uint32_t rangeIdx;
        uint32_t bufIdx = 0;
        IPCStreamBuffer* pSBuf;
        uint32_t* pArr;
        uint32_t addressDiv = 0;

        /* A Pointer to an Array with pointern to a MCTestBCParam-Structure */
        channelPrioTestParam** ppChannelPrioTestParamArray;
        /* A Pointer to an Array with pointern to BufferMemory of 4sk */
        void** ppChannelPrioArrayBuffer;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(args != NULL);       /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        ctx->testResult = 1;

        /*allocate memory for test structure and Init test structure */
        ppChannelPrioTestParamArray = MEM_alloc(extHeap, args->channelNumberReq * sizeof(channelPrioTestParam*), TESTSERVERCACHE_LINESIZE);
        /* (4) allocate an Array to hold the startaddresses for all allocated buffers */
        ppChannelPrioArrayBuffer = MEM_alloc(extHeap, args->channelNumberReq * sizeof(void*), TESTSERVERCACHE_LINESIZE);
        assert(ppChannelPrioArrayBuffer != MEM_ILLEGAL);

        initChannelPrioTestStruct(ppChannelPrioTestParamArray, args);
        openChannelForPrioTest(ctx, args,
            ppChannelPrioTestParamArray,
            &channelPriorityBufferComplete,
            &channelPriorityReceiveMsgCB);

        /* The sleep is required due to the higher priority of the TestServer Task. */
        /* The TestServer Task has to give other Tasks a change to run. */
        /* With out a sleep the channels are not attached and provide buffer messages */
        /* return with error. */
        TSK_sleep(100);

        /* provide buffers for all requested streaming channels. */
        /* This has to be done after the channels has been opened. */
        /* Otherwise transfers aren't start in parallel. */
        for (i = 0; i < args->channelNumberReq; i++) {
                if (args->channel[i].channelType == CHANNELTYPE_STREAM) {
                        /* Allocate the Array of Pointer */
                        ppChannelPrioTestParamArray[i]->ppStreamBuffer = MEM_alloc(extHeap, NUMBUFFERPERCHANNEL * sizeof(IPCStreamBuffer*), TESTSERVERCACHE_LINESIZE);
                        assert(ppChannelPrioTestParamArray[i]->ppStreamBuffer != MEM_ILLEGAL);

                        for (bufIdx = 0; bufIdx < NUMBUFFERPERCHANNEL; bufIdx++) {
                                ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx] = MEM_alloc(extHeap, sizeof(IPCStreamBuffer), TESTSERVERCACHE_LINESIZE);
                                assert(ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx] != MEM_ILLEGAL);
                        }

                        ppChannelPrioArrayBuffer[i] = MEM_alloc(extHeap, NUMBUFFERPERCHANNEL * (channelPrioBufferSizeStart), TESTSERVERCACHE_LINESIZE);
                        assert(ppChannelPrioArrayBuffer[i] != MEM_ILLEGAL);
                        pArr = (uint32_t*)ppChannelPrioArrayBuffer[i];

                        /* Gen. 2 Buffers with 2 Ranges with a size of MCbufferSizeStart/4 */
                        for (bufIdx = 0; bufIdx < NUMBUFFERPERCHANNEL; bufIdx++) {
                                /* get pointer to IPCStreamBuffer structure */
                                pSBuf = ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx];
                                pSBuf->numRanges = CHANNELPRIORANGESPERBUFFER;
                                addressDiv = channelPrioBufferSizeStart >> 3;
                                for (rangeIdx = 0; rangeIdx < pSBuf->numRanges; rangeIdx++) {
                                        pSBuf->ranges[rangeIdx].dataAddress = (uint32_t)&pArr[(rangeIdx + pSBuf->numRanges * bufIdx) * addressDiv];
                                        pSBuf->ranges[rangeIdx].rangeSize = channelPrioBufferSizeStart / pSBuf->numRanges;
                                        pSBuf->ranges[rangeIdx].dataSize = 0;
                                        pSBuf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                status = hoststream_providebuffer(ppChannelPrioTestParamArray[i]->testChannel, pSBuf);

#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " %s: hoststream_providebuffer() return value %d"
                                                                    " TestServer.c Line: %d \n",
                                            __FUNCTION__, status);
                                }
#endif
                        }
                        ppChannelPrioTestParamArray[i]->numDataToTransfer -= channelPrioBufferSizeStart;
                }
        }

        /* Wait for END of Test */
        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        closeChannelOfPrioTest(args, ppChannelPrioTestParamArray);

        freeMemChannelPrioTest(args, ppChannelPrioTestParamArray, ppChannelPrioArrayBuffer);

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return status;
}

/**
 *  channel priority test host sends data.
 *
 */
static uint32_t channelPriorityHostSendsMsgCB(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        channelPrioTestParam* channelPrioParam = (channelPrioTestParam*)param;
        TestServerContext* ctx = (TestServerContext*)channelPrioParam->ctx;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        if (channelPrioParam->CMDnumReceived == 0) {
                channelPrioParam->CMDtestResult = 0;
        } else {
                channelPrioParam->CMDnumReceived--;
                if (channelPrioParam->CMDnumReceived == 0) {
                        channelPrioParam->CMDtestResult = 1; /* ok  */
                }
        }

        if (replySize != NULL) {
                *replySize = 0;
        }

        return HOSTIPC_OK;
}

static uint32_t channelPriorityHostSendsBufferComplete(IPCStreamBuffer* buf, void* param)
{
        channelPrioTestParam* channelPrioParam = (channelPrioTestParam*)param;
        TestServerContext* ctx = (TestServerContext*)channelPrioParam->ctx;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        if (buf->bufferFlags & BUFFERFLAG_PROVIDEBUFFER) {
                if (channelPrioParam->numDataToTransfer > 0) {
                        /* Check how many bytes have to be transfered */
                        if (channelPrioParam->numDataToTransfer >= channelPrioParam->bufferSize) {
                                buf->numRanges = CHANNELPRIORANGESPERBUFFER;
                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        buf->ranges[rangeIdx].rangeSize = channelPrioParam->bufferSize / buf->numRanges;
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                status = hoststream_providebuffer(channelPrioParam->testChannel, buf);
#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " %s: hoststream_providebuffer() return value %d"
                                                                    " TestServer.c Line: %d \n",
                                            __FUNCTION__, status);
                                }
#endif

                                channelPrioParam->numDataToTransfer -= channelPrioParam->bufferSize;
                        } else /* if Size is smaller than channelPrioBufferSizeStart */
                        {
                                /* calculate the number of ranges   */
                                if ((channelPrioParam->numDataToTransfer % (channelPrioParam->bufferSize / buf->numRanges)) == 0) {
                                        buf->numRanges = ((channelPrioParam->numDataToTransfer / (channelPrioParam->bufferSize / buf->numRanges)));
                                } else {
                                        buf->numRanges = ((channelPrioParam->numDataToTransfer / (channelPrioParam->bufferSize / buf->numRanges)) + 1);
                                }

                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        /* No new Address assignment (just setting the range size) */
                                        if (channelPrioParam->numDataToTransfer >= (channelPrioParam->bufferSize / buf->numRanges)) {
                                                buf->ranges[rangeIdx].rangeSize = (channelPrioParam->bufferSize / buf->numRanges);
                                                channelPrioParam->numDataToTransfer -= (channelPrioParam->bufferSize / buf->numRanges);
                                        } else {
                                                buf->ranges[rangeIdx].rangeSize = channelPrioParam->numDataToTransfer;
                                                channelPrioParam->numDataToTransfer = 0;
                                        }
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                assert(channelPrioParam->numDataToTransfer == 0); /* No data should be left to transfer */

                                status = hoststream_providebuffer(channelPrioParam->testChannel, buf);
#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " channelPriorityBufferComplete: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif
                        }
                }
        } else {
                /* Normally this should never happen. All Buffers has to returns with  */
                /* Bufferflag set to BUFFERFLAG_PROVIDEBUFFER. */
                assert(NULL);
        }

        return status;
}

static uint32_t channelPriorityTestHostSends(TestServerContext* ctx, ChannelPrioTestArgs* args, TestCommandMsg* testResult)
{

        int i;
        uint32_t rangeIdx;
        uint32_t bufIdx = 0;
        IPCStreamBuffer* pSBuf;
        uint32_t* pArr;
        uint32_t addressDiv = 0;
        /* A Pointer to an Array with pointern to a MCTestBCParam-Structure */
        channelPrioTestParam** ppChannelPrioTestParamArray;
        /* A Pointer to an Array with pointern to BufferMemory of 4sk */
        void** ppChannelPrioArrayBuffer;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(args != NULL);       /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        ctx->testResult = 1;

        /*allocate memory for test structure and Init test structure */
        ppChannelPrioTestParamArray = MEM_alloc(extHeap, args->channelNumberReq * sizeof(channelPrioTestParam*), TESTSERVERCACHE_LINESIZE);
        assert(ppChannelPrioTestParamArray != MEM_ILLEGAL);
        /* (4) allocate an Array to hold the startaddresses for all allocated buffers */
        ppChannelPrioArrayBuffer = MEM_alloc(extHeap, args->channelNumberReq * sizeof(void*), TESTSERVERCACHE_LINESIZE);
        assert(ppChannelPrioArrayBuffer != MEM_ILLEGAL);

        initChannelPrioTestStruct(ppChannelPrioTestParamArray, args);
        openChannelForPrioTest(ctx, args,
            ppChannelPrioTestParamArray,
            &channelPriorityHostSendsBufferComplete,
            &channelPriorityHostSendsMsgCB);

        /* The sleep is required due to the higher priority of the TestServer Task. */
        /* The TestServer Task has to give other Tasks a change to run. */
        /* With out a sleep the channels are not attached and provide buffer messages */
        /* return with error. */
        TSK_sleep(2000);

        /* provide buffers for all requested streaming channels. */
        /* This has to be done after the channels has been opened. */
        /* Otherwise transfers aren't start in parallel. */
        for (i = 0; i < args->channelNumberReq; i++) {
                if (args->channel[i].channelType == CHANNELTYPE_STREAM) {
                        /* Allocate the Array of Pointer */
                        ppChannelPrioTestParamArray[i]->ppStreamBuffer = MEM_alloc(extHeap, args->channel[i].numBuffer * sizeof(IPCStreamBuffer*), TESTSERVERCACHE_LINESIZE);
                        assert(ppChannelPrioTestParamArray[i]->ppStreamBuffer != MEM_ILLEGAL);

                        for (bufIdx = 0; bufIdx < args->channel[i].numBuffer; bufIdx++) {
                                ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx] = MEM_alloc(extHeap, sizeof(IPCStreamBuffer), TESTSERVERCACHE_LINESIZE);
                                assert(ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx] != MEM_ILLEGAL);
                        }

                        ppChannelPrioArrayBuffer[i] = MEM_alloc(extHeap, args->channel[i].numBuffer * args->channel[i].bufferSize, TESTSERVERCACHE_LINESIZE);
                        assert(ppChannelPrioArrayBuffer[i] != MEM_ILLEGAL);
                        pArr = (uint32_t*)ppChannelPrioArrayBuffer[i];

                        ppChannelPrioTestParamArray[i]->bufferSize = args->channel[i].bufferSize;

                        /* Gen. 2 Buffers with 2 Ranges with a size of MCbufferSizeStart/4 */
                        for (bufIdx = 0; bufIdx < args->channel[i].numBuffer; bufIdx++) {
                                /* get pointer to IPCStreamBuffer structure */
                                pSBuf = ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx];
                                pSBuf->numRanges = CHANNELPRIORANGESPERBUFFER;
                                addressDiv = args->channel[i].bufferSize >> 3;
                                for (rangeIdx = 0; rangeIdx < pSBuf->numRanges; rangeIdx++) {
                                        pSBuf->ranges[rangeIdx].dataAddress = (uint32_t)&pArr[(rangeIdx + pSBuf->numRanges * bufIdx) * addressDiv];
                                        pSBuf->ranges[rangeIdx].rangeSize = args->channel[i].bufferSize / pSBuf->numRanges;
                                        pSBuf->ranges[rangeIdx].dataSize = 0;
                                        pSBuf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                status = hoststream_providebuffer(ppChannelPrioTestParamArray[i]->testChannel, pSBuf);

#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " %s: hoststream_providebuffer() return value %d"
                                                                    " TestServer.c Line: %d \n",
                                            __FUNCTION__, status);
                                }
#endif
                        }
                        ppChannelPrioTestParamArray[i]->numDataToTransfer -= args->channel[i].bufferSize;
                }
        }

        /* Wait for END of Test */
        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        /* Check the results of all Command Channels  */
        for (i = 0; i < args->channelNumberReq; i++) {
                if (args->channel[i].channelType == CHANNELTYPE_CMD) {
                        if (ppChannelPrioTestParamArray[i]->CMDtestResult == NULL) /* 1:OK   0:Error */
                        {
                                ctx->testResult = 0;
                        }
                }
        }

        closeChannelOfPrioTest(args, ppChannelPrioTestParamArray);
        freeMemChannelPrioTest(args, ppChannelPrioTestParamArray, ppChannelPrioArrayBuffer);

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return status;
}

/**
 *  channel priority test DSP sends data.
 *
 */
static uint32_t channelPriorityDspSendsMsgCB(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        channelPrioTestParam* channelPrioParam = (channelPrioTestParam*)param;
        TestServerContext* ctx = (TestServerContext*)channelPrioParam->ctx;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */

        ctx->numReceived--;

        if (replySize != NULL)
                *replySize = 0;

        return HOSTIPC_OK;
}

static uint32_t channelPriorityDspSendsBufferComplete(IPCStreamBuffer* buf, void* param)
{
        channelPrioTestParam* channelPrioParam = (channelPrioTestParam*)param;
        TestServerContext* ctx = (TestServerContext*)channelPrioParam->ctx;
        unsigned int rangeIdx;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL); /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(buf != NULL); /*lint -esym(613,buf) buf is never NULL, ensured by assert */

        if (buf->bufferFlags & BUFFERFLAG_SENDDATA) {
                if (channelPrioParam->numDataToTransfer > 0) {
                        /* Check how many bytes have to be transfered */
                        if (channelPrioParam->numDataToTransfer >= channelPrioParam->bufferSize) {
                                status = hoststream_senddata(channelPrioParam->testChannel, buf);
#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " %s: hoststream_senddata() return value %d"
                                                                    " TestServer.c Line: %d \n",
                                            __FUNCTION__, status);
                                }
#endif

                                channelPrioParam->numDataToTransfer -= channelPrioParam->bufferSize;
                        } else /* if Size is smaller than channelPrioBufferSizeStart */
                        {
                                /* calculate the number of ranges   */
                                if ((channelPrioParam->numDataToTransfer % (channelPrioParam->bufferSize / buf->numRanges)) == 0) {
                                        buf->numRanges = ((channelPrioParam->numDataToTransfer / (channelPrioParam->bufferSize / buf->numRanges)));
                                } else {
                                        buf->numRanges = ((channelPrioParam->numDataToTransfer / (channelPrioParam->bufferSize / buf->numRanges)) + 1);
                                }

                                for (rangeIdx = 0; rangeIdx < buf->numRanges; rangeIdx++) {
                                        /* No new Address assignment (just setting the range size) */
                                        if (channelPrioParam->numDataToTransfer >= (channelPrioParam->bufferSize / buf->numRanges)) {
                                                buf->ranges[rangeIdx].rangeSize = (channelPrioParam->bufferSize / buf->numRanges);
                                                channelPrioParam->numDataToTransfer -= (channelPrioParam->bufferSize / buf->numRanges);
                                        } else {
                                                buf->ranges[rangeIdx].rangeSize = channelPrioParam->numDataToTransfer;
                                                channelPrioParam->numDataToTransfer = 0;
                                        }
                                        buf->ranges[rangeIdx].dataSize = 0;
                                        buf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                assert(channelPrioParam->numDataToTransfer == 0); /* No data should be left to transfer */

                                status = hoststream_senddata(channelPrioParam->testChannel, buf);
#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " channelPriorityBufferComplete: hoststream_providebuffer()"
                                                                    " return value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif
                        }
                }
        } else {
                /* Normally this should never happen. All Buffers has to returns with  */
                /* Bufferflag set to BUFFERFLAG_PROVIDEBUFFER. */
                assert(NULL);
        }

        return status;
}

static uint32_t channelPriorityTestDspSends(TestServerContext* ctx, ChannelPrioTestArgs* args, TestCommandMsg* testResult)
{
        int i;
        uint32_t rangeIdx;
        uint32_t bufIdx = 0;
        IPCStreamBuffer* pSBuf;
        uint32_t* pArr;
        uint32_t addressDiv = 0;
        /* A Pointer to an Array with pointern to a MCTestBCParam-Structure */
        channelPrioTestParam** ppChannelPrioTestParamArray;
        /* A Pointer to an Array with pointern to BufferMemory of 4sk */
        void** ppChannelPrioArrayBuffer;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(args != NULL);       /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        ctx->testResult = 1;

        /*allocate memory for test structure and Init test structure */
        ppChannelPrioTestParamArray = MEM_alloc(extHeap, args->channelNumberReq * sizeof(channelPrioTestParam*), TESTSERVERCACHE_LINESIZE);
        /* (4) allocate an Array to hold the startaddresses for all allocated buffers */
        ppChannelPrioArrayBuffer = MEM_alloc(extHeap, args->channelNumberReq * sizeof(void*), TESTSERVERCACHE_LINESIZE);
        assert(ppChannelPrioArrayBuffer != MEM_ILLEGAL);

        initChannelPrioTestStruct(ppChannelPrioTestParamArray, args);
        openChannelForPrioTest(ctx, args,
            ppChannelPrioTestParamArray,
            &channelPriorityDspSendsBufferComplete,
            &channelPriorityDspSendsMsgCB);

        /* The sleep is required due to the higher priority of the TestServer Task. */
        /* The TestServer Task has to give other Tasks a change to run. */
        /* With out a sleep the channels are not attached and provide buffer messages */
        /* return with error. */
        /* We have to wait 1 sec. to guarantee that the SH4 has provided all buffers. */
        TSK_sleep(3000);

        /* send data for all requested streaming channels. */
        /* This has to be done after the channels has been opened. */
        /* Otherwise transfers aren't start in parallel. */
        for (i = 0; i < args->channelNumberReq; i++) {
                if (args->channel[i].channelType == CHANNELTYPE_STREAM) {
                        /* Allocate the Array of Pointer */
                        ppChannelPrioTestParamArray[i]->ppStreamBuffer = MEM_alloc(extHeap, args->channel[i].numBuffer * sizeof(IPCStreamBuffer*), TESTSERVERCACHE_LINESIZE);
                        assert(ppChannelPrioTestParamArray[i]->ppStreamBuffer != MEM_ILLEGAL);

                        for (bufIdx = 0; bufIdx < args->channel[i].numBuffer; bufIdx++) {
                                ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx] = MEM_alloc(extHeap, sizeof(IPCStreamBuffer), TESTSERVERCACHE_LINESIZE);
                                assert(ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx] != MEM_ILLEGAL);
                        }

                        ppChannelPrioArrayBuffer[i] = MEM_alloc(extHeap, args->channel[i].numBuffer * args->channel[i].bufferSize, TESTSERVERCACHE_LINESIZE);

                        assert(ppChannelPrioArrayBuffer[i] != MEM_ILLEGAL);
                        pArr = (uint32_t*)ppChannelPrioArrayBuffer[i];

                        /* Gen. 2 Buffers with 2 Ranges with a size of MCbufferSizeStart/4 */
                        for (bufIdx = 0; bufIdx < args->channel[i].numBuffer; bufIdx++) {
                                /* get pointer to IPCStreamBuffer structure */
                                pSBuf = ppChannelPrioTestParamArray[i]->ppStreamBuffer[bufIdx];
                                pSBuf->numRanges = CHANNELPRIORANGESPERBUFFER;
                                addressDiv = args->channel[i].bufferSize >> 3;
                                for (rangeIdx = 0; rangeIdx < pSBuf->numRanges; rangeIdx++) {
                                        uint32_t cnt;
                                        /*fill Buffer with dummy data */
                                        for (cnt = args->channel[i].bufferSize / sizeof(uint32_t); cnt > 0; cnt--) {
                                                pArr[cnt - 1] = cnt;
                                        }
                                        pSBuf->ranges[rangeIdx].dataAddress = (uint32_t)&pArr[(rangeIdx + pSBuf->numRanges * bufIdx) * addressDiv];
                                        pSBuf->ranges[rangeIdx].rangeSize = args->channel[i].bufferSize / pSBuf->numRanges;
                                        pSBuf->ranges[rangeIdx].dataSize = 0;
                                        pSBuf->bufferFlags = BUFFERFLAG_NONE;
                                }
                                status = hoststream_senddata(ppChannelPrioTestParamArray[i]->testChannel, pSBuf);

#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, " %s: hoststream_providebuffer() return"
                                                                    " value %d TestServer.c Line: %d \n",
                                            __FUNCTION__, status);
                                }
#endif
                        }
                        ppChannelPrioTestParamArray[i]->numDataToTransfer -= args->channel[i].bufferSize;
                } else if (args->channel[i].channelType == CHANNELTYPE_CMD) {
                        int cnt1;
                        uint8_t msgData[256];
                        for (cnt1 = 255; cnt1 > 0; cnt1--) {
                                msgData[cnt1] = i;
                        }
                        for (cnt1 = 0; cnt1 < args->cmdMsgToSend; cnt1++) {
                                status = hostipc_sendmessage(ppChannelPrioTestParamArray[i]->testChannel,
                                    (uint8_t*)&msgData[0],
                                    256, 0);

#ifndef __QNXNTO__
                                if (status != HOSTIPC_OK) {
                                        LOG_printf(&LOG_TestServer, "sendBurstTest:  hostipc_sendmessage() return"
                                                                    " value %d TestServer.c Line: %d \n",
                                            status, __LINE__);
                                }
#endif
                        }
                }
        }

        /* Wait for END of Test */
        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        closeChannelOfPrioTest(args, ppChannelPrioTestParamArray);

        freeMemChannelPrioTest(args, ppChannelPrioTestParamArray, ppChannelPrioArrayBuffer);

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return status;
}

/*
 * Xon/Xoff Test
 */
#define FLOWCONTROL_XOFF_MESSAGE_LEVEL 8
#define FLOWCONTROL_WAIT_MILLISECONDS 2000

struct _flowControlTestContext {
        uint32_t messageCnt;
        enum {
                FLOWCTRL_DONE,
                FLOWCTRL_NOTDONE
        } testState;
};

typedef struct _flowControlTestContext flowControlTestContext;

flowControlTestContext flowControlCtx;

static uint32_t flowControlTestMsgCB(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t i;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);     /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(payload != NULL); /*lint -esym(613,payload) payload is never NULL, ensured by assert */

        for (i = 0; i < msgSize; i++) {
                payload[i] = ~payload[i];
        }

        status = hostipc_sendmessage(ctx->testChannel, (uint8_t*)&payload[0], msgSize, 0);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "flowControlTestMsgCB:  hostipc_sendmessage() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        /* count number of processed messages. */
        flowControlCtx.messageCnt++;

        /* send a XOFF message after a specified number of processed messages to the host and sleep */
        /* a specified time. After wake up send a XON message to the host and set test state to done, */
        /* because all further messages has to be processed in a normal manner. */
        if ((flowControlCtx.messageCnt >= FLOWCONTROL_XOFF_MESSAGE_LEVEL) && (flowControlCtx.testState == FLOWCTRL_NOTDONE)) {
                status = hostipc_controlChannelFlowCtrl(ctx->testChannel, HOSTIPC_XOFF);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, "flowControlTestMsgCB:  hostipc_controlChannelFlowCtrl() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif

                (void)TSK_sleep(FLOWCONTROL_WAIT_MILLISECONDS);

                status = hostipc_controlChannelFlowCtrl(ctx->testChannel, HOSTIPC_XON);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, "flowControlTestMsgCB:  hostipc_controlChannelFlowCtrl() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif
                flowControlCtx.testState = FLOWCTRL_DONE;
        }

        ctx->testResult = 1;

        if (replySize != NULL)
                *replySize = 0;

        return 0;
}

static uint32_t flowControlTest(TestServerContext* ctx, TestCommandMsg* testResult)
{
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);        /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testResult != NULL); /*lint -esym(613,testResult) testResult is never NULL, ensured by assert */

        flowControlCtx.messageCnt = 0;
        flowControlCtx.testState = FLOWCTRL_NOTDONE;
        status = hostipc_registerControlChannel(ctx->ipc, TESTPROTOCOL_TESTCHANNEL, &flowControlTestMsgCB, ctx, &(ctx->testChannel));
        assert(status == HOSTIPC_OK);
        TSK_sleep(10);

        (void)SEM_pend(ctx->testFinishedSignal, SYS_FOREVER);

        status = hostipc_unregisterChannel(ctx->testChannel);

#ifndef __QNXNTO__
        if (status != HOSTIPC_OK) {
                LOG_printf(&LOG_TestServer, "flowControlTest:  hostipc_unregisterChannel() return"
                                            " value %d TestServer.c Line: %d \n",
                    status, __LINE__);
        }
#endif

        testResult->arguments.result.error = ctx->testResult == 1 ? 0 : 1;

        return 0;
}

/*
 * Test server implementation
 */
static uint32_t testCmdChannelRcvMsg(uint32_t msgSize, uint8_t* payload, uint32_t* replySize, uint8_t* reply, void* param)
{
        TestCommandMsg* testCmd = (TestCommandMsg*)payload;
        TestServerContext* ctx = (TestServerContext*)param;
        uint32_t status = HOSTIPC_OK;

        assert(ctx != NULL);     /*lint -esym(613,ctx) ctx is never NULL, ensured by assert */
        assert(testCmd != NULL); /*lint -esym(613,testCmd) testCmd is never NULL, ensured by assert */

        if (ctx->testPending) {
                if (testCmd->cmd == ABORT) {
                        ctx->termTest = 1;
                        SEM_post(ctx->testFinishedSignal);
                } else {
                        TestCommandMsg testResult;
                        testResult.cmd = PROTOCOL_ERROR;
                        testResult.arguments.result.error = TPERR_STILLRUNNING;
                        status = hostipc_sendmessage(ctx->cmdChannel, (uint8_t*)&testResult, sizeof(testResult), 0);

#ifndef __QNXNTO__
                        if (status != HOSTIPC_OK) {
                                LOG_printf(&LOG_TestServer, " testCmdChannelRcvMsg: hostipc_sendmessage()"
                                                            " return value %d TestServer.c Line: %d \n",
                                    status, __LINE__);
                        }
#endif
                }

        } else {
                memcpy(&(ctx->pendingCmd), testCmd, sizeof(TestCommandMsg));
                ctx->testPending = 1;
                SEM_post(ctx->testSignal);
        }

        return 0;
}

uint32_t testServer(IPCHandle ipc)
{
        TestServerContext ctx;
        TestCommandMsg testResult;
        uint32_t status = HOSTIPC_OK;

        ctx.ipc = ipc;
        ctx.pendingCmd.cmd = INVALID;
        ctx.testSignal = SEM_create(0, NULL);
        ctx.testPending = 0;
        ctx.terminated = 0;

        status = hostipc_registerControlChannel(ctx.ipc, TESTPROTOCOL_COMMANDCHANNEL, &testCmdChannelRcvMsg, &ctx, &ctx.cmdChannel);
        assert(status == HOSTIPC_OK);
        TSK_sleep(1U);

        while (ctx.terminated == 0) {
                ctx.testFinishedSignal = SEM_create(0, NULL);

                (void)SEM_pend(ctx.testSignal, SYS_FOREVER);
                ctx.termTest = 0;

                switch (ctx.pendingCmd.cmd) {
                case TEST_HOST_SENDS_DSP_REPLIES:
                        receiveResponseTest(&ctx, &testResult);
                        break;

                case TEST_DSP_SENDS_HOST_REPLIES:
                        sendResponseTest(&ctx, &testResult);
                        break;

                case TEST_HOST_SENDS_BURST:
                        receiveBurstTest(&ctx, &ctx.pendingCmd.arguments.burstTest, &testResult);
                        break;

                case TEST_DSP_SENDS_BURST:
                        sendBurstTest(&ctx, &ctx.pendingCmd.arguments.burstTest, &testResult);
                        break;

                case TEST_HOST_SENDS_STREAM:
                        receiveStreamTest(&ctx, &testResult);
                        break;

                case TEST_DSP_SENDS_STREAM:
                        sendStreamTest(&ctx, &testResult);
                        break;

                case TEST_HOST_SENDS_STREAM_DSP_REPLIES:
                        receiveStreamReplyTest(&ctx, &testResult);
                        break;

                case TEST_HOST_SENDS_DISCONTINUITY:
                        receiveDiscontinuityTest(&ctx, &testResult);
                        break;

                case TEST_DSP_SENDS_DISCONTINUITY:
                        sendDiscontinuityTest(&ctx, &testResult);
                        break;

                case TEST_DSP_ABORT:
                        sendAbortTest(&ctx, &testResult);
                        break;

                case TEST_ON_STREAM_DSP_SENDS_HOST_REPLIES:
                        sendResponseTestOnStreamChannel(&ctx, &testResult);
                        break;

                case TEST_HOST_SENDS_STREAM_VAR_DSP_REPLIES:
                        receiveStreamVarReplyTest(&ctx, &ctx.pendingCmd.arguments.varmsgtest, &testResult);
                        break;

                case TEST_DRAIN:
                        receiveStreamDrainReplyTest(&ctx, &ctx.pendingCmd.arguments.drainTest, &testResult);
                        break;

                case TEST_DRAIN_2:
                        testDrain2(&ctx, &ctx.pendingCmd.arguments.drainTest_2, &testResult);
                        break;

                case TEST_HOST_SENDS_STREAM_MULTI_CHANNEL:
                        receiveStreamVarReplyTestMulti(&ctx, &ctx.pendingCmd.arguments.multichanneltest, &testResult);
                        break;

                case TEST_CHANNEL_PRIORITY:
                        channelPriorityTest(&ctx, &ctx.pendingCmd.arguments.channelPrioTest, &testResult);
                        break;

                case TEST_CHANNEL_PRIORITY_HOSTSENDS:
                        channelPriorityTestHostSends(&ctx, &ctx.pendingCmd.arguments.channelPrioTest, &testResult);
                        break;

                case TEST_CHANNEL_PRIORITY_DSPSENDS:
                        channelPriorityTestDspSends(&ctx, &ctx.pendingCmd.arguments.channelPrioTest, &testResult);
                        break;

                case TEST_HOST_SENDS_STREAM_DSP_REPLIES_SPECIAL:
                        receiveStreamReplyTestSpecial(&ctx, &testResult);
                        break;

                case TEST_FLOW_CONTROL:
                        flowControlTest(&ctx, &testResult);
                        break;

                default:
                        testResult.arguments.result.error = TPERR_UNKNOWNCOMMAND;
                        break;
                }

                ctx.testPending = 0;

                testResult.cmd = RESULT;
                status = hostipc_sendmessage(ctx.cmdChannel, (uint8_t*)&testResult, sizeof(testResult), 0);

#ifndef __QNXNTO__
                if (status != HOSTIPC_OK) {
                        LOG_printf(&LOG_TestServer, " testServer: hostipc_sendmessage() return"
                                                    " value %d TestServer.c Line: %d \n",
                            status, __LINE__);
                }
#endif

                SEM_delete(ctx.testFinishedSignal);
        }

        return 0;
}
