// SPDX-License-Identifier: GPL-2.0-only
/*
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
#include <linux/delay.h>

#include "exynos-hdcp2.h"
#include "exynos-hdcp2-log.h"
#include "exynos-hdcp2-dplink.h"
#include "exynos-hdcp2-dplink-if.h"
#include "exynos-hdcp2-dplink-state.h"
#include "exynos-hdcp2-dplink-auth.h"
#include "exynos-hdcp2-dplink-inter.h"

#if defined(CONFIG_HDCP2_EMULATION_MODE)
#define HDCP_AUTH_RETRY_COUNT	10
#else
#define HDCP_AUTH_RETRY_COUNT	5
#endif
#define RECVID_WAIT_RETRY_COUNT	5

static DEFINE_MUTEX(hdcp_auth_mutex);
/* current link data */
static struct hdcp_link_data *lk_data;
extern struct hdcp_dp_info hdcp_dp_info[MAX_NUM_OF_HDCP];
extern enum auth_state auth_proc_state[MAX_NUM_OF_HDCP];

/* TODO: update for multi-link */
static struct hdcp_sess_info ss[MAX_NUM_OF_HDCP];
static struct hdcp_link_info lk[MAX_NUM_OF_HDCP];

struct hdcp_sess_info *hdcp_init_session(uint32_t id);
struct hdcp_link_info *hdcp_init_link(uint32_t id);

int hdcp_dplink_get_rxinfo(uint32_t id, uint8_t *status)
{
	int ret = 0;

	ret = hdcp_dplink_recv(id, HDCP22_MSG_RXSTATUS_R, status, sizeof(uint8_t));
	hdcp_debug("node%d RxStatus: %x\n", id, *status);

	return ret;
}

int do_dplink_auth(struct hdcp_link_info *lk_handle)
{
	struct hdcp_session_node *ss_node;
	struct hdcp_link_node *lk_node;
	int ret = HDCP_SUCCESS;
	int rval = TX_AUTH_SUCCESS;
	uint32_t retry_recvid = 0;
	uint8_t typ = 0;
	/* find Session node which contains the Link */

	if (!hdcp_dp_info[lk_handle->ss_id].rp_ready) {
		ss_node = hdcp_session_list_find(lk_handle->ss_id);
		if (!ss_node)
			return HDCP_ERROR_INVALID_INPUT;

		lk_node = hdcp_link_list_find(lk_handle->lk_id, &ss_node->ss_data->ln);
		if (!lk_node)
			return HDCP_ERROR_INVALID_INPUT;

		lk_data = lk_node->lk_data;
		if (!lk_data)
			return HDCP_ERROR_INVALID_INPUT;

		if (lk_data->state != LINK_ST_H1_TX_LOW_VALUE_CONTENT)
			return HDCP_ERROR_INVALID_STATE;
	}

	hdcp_info("lk_handle->ss_id(%d) lk_handle->lk_id(%d) lk_data->lk_id(%d).\n",
		lk_handle->ss_id, lk_handle->lk_id, lk_data->id);
	/**
	 * if Upstream Content Control Function call this API,
	 * it changes state to ST_A0_DETERMINE_RX_HDCP_CAP automatically.
	 * HDCP library do not check CP desire.
	 */

	if (!lk_data) {
		hdcp_info("DP HDCP2.2 Session node(%d) is not opened.\n", lk_handle->ss_id);
		return HDCP_ERROR_INVALID_INPUT;
	}

	if (hdcp_dp_info[lk_handle->ss_id].rp_ready)
		UPDATE_LINK_STATE(lk_data, LINK_ST_A7_VERIFY_RECEIVER_ID_LIST);
	else
		UPDATE_LINK_STATE(lk_data, LINK_ST_A0_DETERMINE_RX_HDCP_CAP);

	do {
		switch (lk_data->state) {
		case LINK_ST_H1_TX_LOW_VALUE_CONTENT:
			hdcp_dplink_config(lk_handle->ss_id, DP_HDCP22_DISABLE, 0);
			return ret;
		case LINK_ST_A0_DETERMINE_RX_HDCP_CAP:
			if (dplink_determine_rx_hdcp_cap(lk_handle->ss_id, lk_data) < 0) {
				ret = HDCP_ERROR_RX_NOT_HDCP_CAPABLE;
				UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			} else
				UPDATE_LINK_STATE(lk_data, LINK_ST_A1_EXCHANGE_MASTER_KEY);
			break;
		case LINK_ST_A1_EXCHANGE_MASTER_KEY:
			rval = dplink_exchange_master_key(lk_handle->ss_id, lk_data);
			if (rval == TX_AUTH_SUCCESS) {
				UPDATE_LINK_STATE(lk_data, LINK_ST_A2_LOCALITY_CHECK);
			} else {
				/* todo: consider connection retry */
				ret = HDCP_ERROR_EXCHANGE_KM;
				UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			}
			break;
		case LINK_ST_A2_LOCALITY_CHECK:
			rval = dplink_locality_check(lk_data);
			if (rval == TX_AUTH_SUCCESS) {
				UPDATE_LINK_STATE(lk_data, LINK_ST_A3_EXCHANGE_SESSION_KEY);
			} else {
				/* todo: consider connection retry */
				ret = HDCP_ERROR_LOCALITY_CHECK;
				UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			}
			break;
		case LINK_ST_A3_EXCHANGE_SESSION_KEY:
			if (dplink_exchange_session_key(lk_data) < 0) {
				ret = HDCP_ERROR_EXCHANGE_KS;
				UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			} else {
				UPDATE_LINK_STATE(lk_data, LINK_ST_A4_TEST_REPEATER);
			}
			break;
		case LINK_ST_A4_TEST_REPEATER:
			if (dplink_evaluate_repeater(lk_data) == TRUE) {
				/* if it is a repeater, verify Rcv ID list */
				UPDATE_LINK_STATE(lk_data, LINK_ST_A6_WAIT_RECEIVER_ID_LIST);
#if defined(CONFIG_HDCP2_FUNC_TEST_MODE)
				hdcp_info("it`s func test mode (%d).\n", lk_handle->ss_id);
#else
				hdcp_info("It`s repeater link (%d)!\n", lk_handle->ss_id);
#endif
			} else {
				/* if it is not a repeater, complete authentication */
				UPDATE_LINK_STATE(lk_data, LINK_ST_A5_AUTHENTICATED);
				hdcp_info("It`s Rx link node(%d)!\n", lk_handle->ss_id);
			}
			break;
		case LINK_ST_A5_AUTHENTICATED:
			/* HDCP2.2 spec defined 200ms */
			msleep(200);

			typ = lk_data->tx_ctx.strm_typ;
			hdcp_info("LINK Authenticationed Type: %d\n", typ);
			hdcp_dplink_config(lk_handle->ss_id, DP_HDCP22_ENABLE, typ);

			/* Transmitter has completed the authentication protocol */
			ret = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, DP_HDCP22_ENABLE,
								lk_handle->ss_id, 0);
			return HDCP_SUCCESS;
		case LINK_ST_A6_WAIT_RECEIVER_ID_LIST:
			rval = dplink_wait_for_receiver_id_list(lk_data);
			if (rval == -TX_AUTH_ERROR_TIME_EXCEED) {
				if (retry_recvid < RECVID_WAIT_RETRY_COUNT) {
					/* retry hdcp authentication in case of timeout */
					hdcp_dplink_config(lk_handle->ss_id, DP_HDCP22_DISABLE, 0);
					UPDATE_LINK_STATE(lk_data, LINK_ST_A0_DETERMINE_RX_HDCP_CAP);
					hdcp_info("retry authentication node(%d). (%d)\n",
							lk_handle->ss_id, retry_recvid);
					retry_recvid++;
				} else {
					hdcp_info("exceed retry count node(%d). (%d)\n",
							lk_handle->ss_id, retry_recvid);
					ret = HDCP_ERROR_VERIFY_RECEIVER_ID_LIST;
					UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
				}
			} else if (rval == HDCP_SUCCESS) {
				UPDATE_LINK_STATE(lk_data, LINK_ST_A7_VERIFY_RECEIVER_ID_LIST);
				retry_recvid = 0;
			} else {
				/* error on recevier id list wait */
				ret = HDCP_ERROR_VERIFY_RECEIVER_ID_LIST;
				UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			}
			break;
		case LINK_ST_A7_VERIFY_RECEIVER_ID_LIST:
			if (dplink_verify_receiver_id_list(lk_data) < 0) {
				ret = HDCP_ERROR_VERIFY_RECEIVER_ID_LIST;
				UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			} else {
				UPDATE_LINK_STATE(lk_data, LINK_ST_A8_SEND_RECEIVER_ID_LIST_ACK);
			}
			break;
		case LINK_ST_A8_SEND_RECEIVER_ID_LIST_ACK:
			rval = dplink_send_receiver_id_list_ack(lk_data);
			if (rval < 0) {
				ret = HDCP_ERROR_VERIFY_RECEIVER_ID_LIST;
				UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			} else {
				UPDATE_LINK_STATE(lk_data, LINK_ST_A9_CONTENT_STREAM_MGT);
			}
			break;
		case LINK_ST_A9_CONTENT_STREAM_MGT:
			rval = dplink_stream_manage(lk_data);
			if (rval < 0) {
				ret = HDCP_ERROR_STREAM_MANAGE;
				UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			} else {
				UPDATE_LINK_STATE(lk_data, LINK_ST_A5_AUTHENTICATED);
			}
			break;
		default:
			ret = HDCP_ERROR_INVALID_STATE;
			UPDATE_LINK_STATE(lk_data, LINK_ST_H1_TX_LOW_VALUE_CONTENT);
			break;
		}
	} while (1);
}

/* todo: support multi link & multi session */
int hdcp_dplink_authenticate(uint32_t node)
{
	int ret;
	static int retry_cnt;

	struct hdcp_sess_info *ss_info;
	struct hdcp_link_info *lk_info;

	ss_info = &ss[node];
	lk_info = &lk[node];

	mutex_lock(&hdcp_auth_mutex);

	for (; retry_cnt < HDCP_AUTH_RETRY_COUNT; retry_cnt++) {

		auth_proc_state[node] = HDCP_AUTH_PROCESS_RUN;

		if (!hdcp_dp_info[node].rp_ready) {
			hdcp_clear_session(node);
			if (hdcp_session_open(ss_info)) {
				mutex_unlock(&hdcp_auth_mutex);
				return -ENOMEM;
			}

			lk_info->ss_id = ss_info->ss_id;
			if (hdcp_link_open(lk_info, HDCP_LINK_TYPE_DP)) {
				hdcp_session_close(ss_info);
				mutex_unlock(&hdcp_auth_mutex);
				return -ENOMEM;
			}

			/* if hdcp is enabled, disable it while hdcp authentication */
			if (hdcp_dplink_is_enabled_hdcp22(node))
				hdcp_dplink_config(node, DP_HDCP22_DISABLE, 0);

			dplink_clear_irqflag_all(node);
		}

		ret = do_dplink_auth(lk_info);
		if (ret == HDCP_SUCCESS) {
			hdcp_info("Success HDCP node(%d) authenticate done.\n", node);
			auth_proc_state[node] = HDCP_AUTH_PROCESS_DONE;
			retry_cnt = 0;
			mutex_unlock(&hdcp_auth_mutex);
			return 0;
		} else {
			/* auth_proc_state flag check */
			if (auth_proc_state[node] == HDCP_AUTH_PROCESS_STOP) {
				hdcp_info("Stop node%d authenticate.\n", node);
				break;
			}
			/* retry */
			dplink_clear_irqflag_all(node);
			hdcp_err("HDCP(%d) auth failed. retry(%d)\n", node, retry_cnt);
		}
	}

	retry_cnt = 0;
	auth_proc_state[node] = HDCP_AUTH_PROCESS_IDLE;
	mutex_unlock(&hdcp_auth_mutex);
	return -EACCES;
}

void hdcp_clear_session(uint32_t id)
{

	ss[id].ss_id = id;
	lk[id].ss_id = id;
	lk[id].lk_id = id;

	hdcp_link_close(&lk[id]);
	hdcp_session_close(&ss[id]);
}

MODULE_LICENSE("GPL");
