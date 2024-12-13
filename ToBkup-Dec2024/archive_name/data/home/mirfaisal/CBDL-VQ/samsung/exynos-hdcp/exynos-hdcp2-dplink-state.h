/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/smc.h>
#include <asm/cacheflush.h>
#include <soc/samsung/exynos-smc.h>
#include <linux/types.h>
#include "exynos-hdcp2.h"

static char *hdcp_session_st_str[] = {
	"ST_INIT",
	"ST_LINK_SETUP",
	"ST_END",
	NULL
};

static char *hdcp_link_st_str[] = {
	"ST_INIT",
	"ST_H0_NO_RX_ATTACHED",
	"ST_H1_TX_LOW_VALUE_CONTENT",
	"ST_A0_DETERMINE_RX_HDCP_CAP",
	"ST_A1_EXCHANGE_MASTER_KEY",
	"ST_A2_LOCALITY_CHECK",
	"ST_A3_EXCHANGE_SESSION_KEY",
	"ST_A4_TEST_REPEATER",
	"ST_A5_AUTHENTICATED",
	"ST_A6_WAIT_RECEIVER_ID_LIST",
	"ST_A7_VERIFY_RECEIVER_ID_LIST",
	"ST_A8_SEND_RECEIVER_ID_LIST_ACK",
	"ST_A9_CONTENT_STREAM_MGT",
	"ST_END",
	NULL
};

static char *hdcp_auth_state[] = {
	"HDCP_AUTH_PROCESS_INIT",
	"HDCP_AUTH_PROCESS_IDLE",
	"HDCP_AUTH_PROCESS_RUN",
	"HDCP_AUTH_PROCESS_STOP",
	"HDCP_AUTH_PROCESS_DONE",
	NULL
};

static char *hdcp_dp_conn_state[] = {
	"DP_DISCONNECT",
	"DP_CONNECT",
	"DP_HDCP_READY",
	NULL
};

static inline void UPDATE_SESSION_STATE(struct hdcp_session_data *sess, uint32_t st)
{
	printk(KERN_INFO "[HDCP2] UPDATE_SESSION_STATE node(%d): %s -> %s\n",
		 sess->id, hdcp_session_st_str[sess->state], hdcp_session_st_str[st]);
	sess->state = st;
}

static inline void UPDATE_LINK_STATE(struct hdcp_link_data *link, enum hdcp_tx_ss_state st)
{
	printk(KERN_INFO "[HDCP2] UPDATE_LINK_STATE node(%d): %s -> %s\n",
		link->id, hdcp_link_st_str[link->state], hdcp_link_st_str[st]);
	link->state = st;
}

static inline void HDCP_DP_CONN_STATE(uint8_t node, enum dp_state st)
{
	printk(KERN_INFO "[HDCP2] hdcp_dp_conn_state node%d   : %s\n",
		node, hdcp_dp_conn_state[st]);
}

static inline void HDCP_PROC_STATE(uint8_t node, enum auth_state st)
{
	printk(KERN_INFO "[HDCP2] hdcp_auth_proc_state node%d : %s\n",
		node, hdcp_auth_state[st]);
}

MODULE_LICENSE("GPL");
