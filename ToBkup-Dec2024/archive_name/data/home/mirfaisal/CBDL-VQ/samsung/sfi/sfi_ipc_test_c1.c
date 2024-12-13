// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * Exynos sfi ipc test
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/sched/clock.h>
#include <linux/module.h>
#include <soc/samsung/sfi_ipc_ctrl.h>

extern void sfi_ipc_c1_debug(void);

#define SFI_IPC_C1_TEST_SIZE (SFI_IPC_C1_SIZE * 3)
#define SFI_IPC_C1_TEST_BIGSIZE (SFI_IPC_C1_SIZE * 128)
#define SFI_IPC_C1_TESTMSG_NUM (100)
#define SFI_IPC_C1_AGING_TESTMSG_NUM (100000)
#define SFI_IPC_C1_TEST_TIMEOUT (10000000)
#define SFI_IPC_C1_TEST_WAIT (2000000000)
#define DBG_NAME "[sfi_ipc_c1_test]"

/*********** debug macros **********/

static int debug_stage;
static unsigned char info_print[1024];

#define SFI_IPC_C1_INFO_PRINT(...) \
{ \
	int ret = 0;\
	ret = snprintf(info_print, 1024, __VA_ARGS__); \
	if (ret < 0 || ret > sizeof(info_print)) \
		pr_info("sprintf failed\n");\
	pr_info("%s %s stage : %d %s\n", \
	DBG_NAME, __func__, (debug_stage), info_print); \
}
#define SFI_IPC_C1_PASS_PRINT() \
{ \
	pr_info("%s %s stage : %d pass\n", \
	DBG_NAME, __func__, (debug_stage)); \
}
#define SFI_IPC_C1_FAIL_PRINT() \
{ \
	pr_info("%s %s stage : %d fail\n", \
	DBG_NAME, __func__, (debug_stage)); \
	sfi_ipc_c1_debug(); \
	goto SFI_IPC_C1_TEST_EXIT; \
}
#define SFI_IPC_C1_TEST_RESULT(criteria) \
{ \
	if (criteria) { \
		SFI_IPC_C1_PASS_PRINT(); \
	} else { \
		SFI_IPC_C1_FAIL_PRINT(); \
	} \
}

/*********** static test variables and callbacks **********/

/* task struct to make kernel thread */
static struct task_struct *ts;

/* channel name buffers to test mailbox */
static unsigned char channel_name[SFI_IPC_C1_CHANNEL + 1][SFI_IPC_C1_NAME_SIZE];

/* tx rx buffers to test mailbox */
static struct sfi_ipc_c1_buf_info tx_buf;
static struct sfi_ipc_c1_buf_info rx_buf;

/* temporal variables to test the receive process of mailbox */
static unsigned char received[SFI_IPC_C1_TEST_BIGSIZE + 1];
static volatile bool rx_done;
static unsigned int rcv_size;
static unsigned int rcv_cnt;
static unsigned int snd_cnt;

/* wait queue to wait for the opposite sending mailbox */
static wait_queue_head_t rx_wait;

/*
 * callbacks are executed in interrupt context.
 * To run complicated process, user should change the context.
 */
static void simple_test_rx_cb(struct sfi_ipc_c1_buf_info *arg)
{
	memcpy(received, arg->addr, arg->cur_size);
	rcv_size = arg->cur_size;
	++rcv_cnt;

	if (rx_done == false) {
		rx_done = true;
		wake_up_interruptible(&rx_wait);
	}
}

/*
 * callbacks are executed in interrupt context.
 * To run complicated process, user should change the context.
 */
static void multiple_test_rx_cb(struct sfi_ipc_c1_buf_info *arg)
{
	memcpy(received, arg->addr, arg->cur_size);
	++rcv_cnt;

	if (rx_done == false && rcv_cnt == SFI_IPC_C1_TESTMSG_NUM) {
		rx_done = true;
		wake_up_interruptible(&rx_wait);
	}
}

static int sfi_ipc_c1_test_prepare(void)
{
	int i;
	unsigned char tmp_char = 'B';

	/* addr and size must be set */
	tx_buf.addr = kmalloc(SFI_IPC_C1_TEST_BIGSIZE, GFP_KERNEL);
	if (IS_ERR(tx_buf.addr)) {
		return PTR_ERR(tx_buf.addr);
	}
	tx_buf.size = SFI_IPC_C1_TEST_BIGSIZE;

	/* addr and size must be set */
	rx_buf.addr = kmalloc(SFI_IPC_C1_TEST_BIGSIZE, GFP_KERNEL);
	if (IS_ERR(rx_buf.addr)) {
		return PTR_ERR(rx_buf.addr);
	}
	rx_buf.size = SFI_IPC_C1_TEST_BIGSIZE;

	init_waitqueue_head(&rx_wait);

	for (i = 0; i < SFI_IPC_C1_CHANNEL + 1; i++) {
		memset(channel_name[i], tmp_char, SFI_IPC_C1_NAME_SIZE);
		tmp_char++;
	}

	return 0;
}

static int sfi_ipc_c1_test_check_expired(unsigned long long time_start,
		unsigned long long timeout)
{
	unsigned long long time_now;

	time_now = sched_clock();
	if (time_now < (time_start + timeout)) {
		return false;
	}

	return true;
}

static int sfi_ipc_c1_test_thread(void *data)
{
	int i;
	int channel;
	int ret;
	unsigned long long timeout_ns;
	unsigned long long time_start;

	timeout_ns = SFI_IPC_C1_TEST_WAIT;

	ret = sfi_ipc_c1_test_prepare();
	if (ret) {
		goto SFI_IPC_C1_TEST_EXIT;
	}

	/* simple open without rx buf */
	debug_stage = 1;
	rx_done = false;
	memset(tx_buf.addr, 0, SFI_IPC_C1_TEST_SIZE);
	memcpy(tx_buf.addr, "open", strlen("open"));
	SFI_IPC_C1_INFO_PRINT("sending a mailbox message\n");
	channel = sfi_ipc_c1_open(channel_name[0], simple_test_rx_cb, &rx_buf);
	SFI_IPC_C1_TEST_RESULT(channel >= 0 && channel < SFI_IPC_C1_CHANNEL);

	/* send interrupt without data */
	debug_stage = 2;
	time_start = sched_clock();
	do {
		if (sfi_ipc_c1_test_check_expired(time_start, timeout_ns) == true) {
			SFI_IPC_C1_INFO_PRINT("send timed out\n");
			SFI_IPC_C1_TEST_RESULT(0);
		}
		ret = sfi_ipc_c1_send(channel, &tx_buf);
		if (ret != 0) {
			usleep_range(100, 100);
		}
	} while (ret != 0);
	SFI_IPC_C1_TEST_RESULT(ret == 0);

	/* wait for the response */
	debug_stage = 3;
	ret = wait_event_interruptible_timeout(rx_wait, rx_done,
		   nsecs_to_jiffies(SFI_IPC_C1_TEST_WAIT));
	if (ret == 0) {
		SFI_IPC_C1_INFO_PRINT("receive timed out\n");
	}
	SFI_IPC_C1_INFO_PRINT("rcvd size: %d\n", rcv_size);
	SFI_IPC_C1_TEST_RESULT(ret);

	/* open with rx buf */
	debug_stage = 4;
	tx_buf.size = SFI_IPC_C1_TEST_SIZE; /* reduce the tx size */
	rx_done = false;
	snd_cnt = 0;
	rcv_cnt = 0;
	channel = sfi_ipc_c1_open(channel_name[1], multiple_test_rx_cb, &rx_buf);
	SFI_IPC_C1_TEST_RESULT(channel >= 0 && channel < SFI_IPC_C1_CHANNEL);

	/* send interrupt with data */
	debug_stage = 5;
	SFI_IPC_C1_INFO_PRINT("sending %d mailbox message\n", SFI_IPC_C1_TESTMSG_NUM);
	/* size must be informed to the api */
	tx_buf.size = SFI_IPC_C1_TEST_SIZE;
	memset(tx_buf.addr, 0, SFI_IPC_C1_TEST_SIZE);
	memcpy(tx_buf.addr, "msg", strlen("msg"));
	for (i = 0; i < SFI_IPC_C1_TESTMSG_NUM; i++) {
		time_start = sched_clock();
		do {
			if (sfi_ipc_c1_test_check_expired(time_start, timeout_ns) == true) {
				SFI_IPC_C1_INFO_PRINT("send timed out\n");
				SFI_IPC_C1_TEST_RESULT(0);
			}

			ret = sfi_ipc_c1_send(channel, &tx_buf);
			if (ret == 0) {
				snd_cnt++;
			} else {
				usleep_range(100, 100);
			}
		} while (ret != 0);
	}
	SFI_IPC_C1_TEST_RESULT(snd_cnt == SFI_IPC_C1_TESTMSG_NUM);

	/* wait for the response */
	debug_stage = 6;
	ret = wait_event_interruptible_timeout(rx_wait, rx_done,
			nsecs_to_jiffies(SFI_IPC_C1_TEST_WAIT));
	if (ret == 0) {
		SFI_IPC_C1_INFO_PRINT("receive timed out\n");
	}
	SFI_IPC_C1_INFO_PRINT("multiple receive cnt : %d\n", rcv_cnt);
	SFI_IPC_C1_TEST_RESULT(ret);

	/* start aging test */
	debug_stage = 7;
	rx_done = false;
	rcv_cnt = 0;
	snd_cnt = 0;
	channel = sfi_ipc_c1_open(channel_name[2], simple_test_rx_cb, &rx_buf);
	SFI_IPC_C1_TEST_RESULT(channel >= 0 && channel < SFI_IPC_C1_CHANNEL);

	debug_stage = 8;
	tx_buf.size = SFI_IPC_C1_TEST_SIZE;
	memset(tx_buf.addr, 0, SFI_IPC_C1_TEST_SIZE);
	memcpy(tx_buf.addr, "aging", strlen("aging"));
	while (rcv_cnt < SFI_IPC_C1_AGING_TESTMSG_NUM ||
			snd_cnt < SFI_IPC_C1_AGING_TESTMSG_NUM) {
		if (snd_cnt < SFI_IPC_C1_AGING_TESTMSG_NUM) {
			/* send */
			time_start = sched_clock();
			do {
				if (sfi_ipc_c1_test_check_expired(time_start, timeout_ns) == true) {
					SFI_IPC_C1_INFO_PRINT("send timed out\n");
					SFI_IPC_C1_INFO_PRINT("snd_cnt: %d rcv_cnt: %d\n", snd_cnt, rcv_cnt);
					SFI_IPC_C1_TEST_RESULT(0);
				}

				ret = sfi_ipc_c1_send(channel, &tx_buf);
				if (ret == 0) {
					snd_cnt++;
				} else {
					usleep_range(100, 100);
				}
			} while (ret != 0);
		}

		if (rcv_cnt < SFI_IPC_C1_AGING_TESTMSG_NUM) {
			/* wait for receiving */
			ret = wait_event_interruptible_timeout(rx_wait, rx_done,
				nsecs_to_jiffies(SFI_IPC_C1_TEST_WAIT));
			if (ret == 0) {
				SFI_IPC_C1_INFO_PRINT("receive timed out\n");
				SFI_IPC_C1_INFO_PRINT("snd_cnt: %d rcv_cnt: %d\n", snd_cnt, rcv_cnt);
				SFI_IPC_C1_TEST_RESULT(0);
			}
			rx_done = false;
			if ((rcv_cnt % (SFI_IPC_C1_AGING_TESTMSG_NUM / 10)) == 0) {
				SFI_IPC_C1_INFO_PRINT("%s(cnt:%d)\n", received, rcv_cnt);
			}
		}
	};
	SFI_IPC_C1_TEST_RESULT(snd_cnt >= SFI_IPC_C1_AGING_TESTMSG_NUM);

	debug_stage = 9;
	SFI_IPC_C1_TEST_RESULT(rcv_cnt >= SFI_IPC_C1_AGING_TESTMSG_NUM);

	/* opening SFI_IPC_C1_CHANNEL + 1 channels */
	debug_stage = 10;
	SFI_IPC_C1_INFO_PRINT("opening multiple channels\n");
	for (i = 3; i < SFI_IPC_C1_CHANNEL + 1; i++) {
		channel = sfi_ipc_c1_open(channel_name[i], simple_test_rx_cb, &rx_buf);
	}
	SFI_IPC_C1_TEST_RESULT(1);

	msleep(500);
	sfi_ipc_c1_debug();
	msleep(500);

	/* closing all channels */
	debug_stage = 11;
	SFI_IPC_C1_INFO_PRINT("closing all channels\n");
	for (i = 0; i < SFI_IPC_C1_CHANNEL + 1; i++) {
		ret = sfi_ipc_c1_close(channel_name[i]);
		if (ret != 0) {
			break;
		}
	}
	SFI_IPC_C1_TEST_RESULT(ret == 0);

	/* finished */
	SFI_IPC_C1_INFO_PRINT("finish");

SFI_IPC_C1_TEST_EXIT:
	sfi_ipc_c1_debug();

	if (tx_buf.addr) {
		kfree(tx_buf.addr);
	}
	if (rx_buf.addr) {
		kfree(rx_buf.addr);
	}

	return 0;
}

static int __init sfi_ipc_c1_test_init(void)
{
	debug_stage = 0;
	SFI_IPC_C1_INFO_PRINT("start");
	ts = kthread_run(sfi_ipc_c1_test_thread, NULL, DBG_NAME);

	return 0;
}

static void __exit sfi_ipc_c1_test_exit(void)
{
	debug_stage = 99;
	SFI_IPC_C1_INFO_PRINT("cleanup");
}

module_init(sfi_ipc_c1_test_init);
module_exit(sfi_ipc_c1_test_exit);

MODULE_AUTHOR("MinJune Choi");
MODULE_DESCRIPTION("AP SFI CORE1 IPC test");
MODULE_LICENSE("GPL v2");

