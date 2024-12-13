// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <misc/provencore/ree_session.h>
#include "exynos-hdcp2.h"
#include "exynos-hdcp2-log.h"
#include "exynos-hdcp2-dplink-auth.h"
#include "exynos-hdcp2-dplink-if.h"
#include "exynos-hdcp2-dplink.h"
#include "exynos-hdcp2-dplink-if.h"
#include "exynos-hdcp2-dplink-inter.h"

#define MAX_LC_RETRY 10

extern struct hdcp_dp_info hdcp_dp_info[MAX_NUM_OF_HDCP];
static char *hdcp_msgid_str[] = {
	NULL,
	"Null message",
	"DP_AKE_Init",
	"DP_AKE_Send_Cert",
	"DP_AKE_No_Stored_km",
	"DP_AKE_Stored_km",
	"DP_AKE_Send_rrx",
	"DP_AKE_Send_H_prime",
	"DP_AKE_Send_Pairing_Info",
	"DP_LC_Init",
	"DP_LC_Send_L_prime",
	"DP_SKE_Send_Eks",
	"DP_RepeaterAuth_Send_ReceiverID_List",
	"DP_RTT_Ready",
	"DP_RTT_Challenge",
	"DP_RepeaterAuth_Send_Ack",
	"DP_RepeaterAuth_Stream_Manage",
	"DP_RepeaterAuth_Stream_Ready",
	"DP_Receiver_AuthStatus",
	"DP_AKE_Transmitter_Info",
	"DPAKE_Receiver_Info",
	NULL
};

struct dp_ake_init {
	uint8_t rtx[HDCP_AKE_RTX_BYTE_LEN];
	uint8_t txcaps[HDCP_CAPS_BYTE_LEN];
};

struct dp_ake_send_cert {
	uint8_t cert_rx[HDCP_RX_CERT_LEN];
	uint8_t rrx[HDCP_RRX_BYTE_LEN];
	uint8_t rxcaps[HDCP_CAPS_BYTE_LEN];
};

struct dp_ake_no_stored_km {
	uint8_t ekpub_km[HDCP_AKE_ENCKEY_BYTE_LEN];
};

struct dp_ake_stored_km {
	uint8_t ekh_km[HDCP_AKE_EKH_MKEY_BYTE_LEN];
	uint8_t m[HDCP_AKE_M_BYTE_LEN];
};

struct dp_ake_send_h_prime {
	uint8_t h_prime[HDCP_HMAC_SHA256_LEN];
};

struct dp_ake_send_pairing_info {
	uint8_t ekh_km[HDCP_AKE_MKEY_BYTE_LEN];
};

struct dp_lc_init {
	uint8_t rn[HDCP_RTX_BYTE_LEN];
};

struct dp_lc_send_l_prime {
	uint8_t l_prime[HDCP_HMAC_SHA256_LEN];
};

struct dp_ske_send_eks {
	uint8_t edkey_ks[HDCP_AKE_MKEY_BYTE_LEN];
	uint8_t riv[HDCP_RTX_BYTE_LEN];
	uint8_t strm_typ;
};

struct dp_rp_send_rcvid_list {
	uint8_t rx_info[HDCP_RP_RX_INFO_LEN];
	uint8_t seq_num_v[HDCP_RP_SEQ_NUM_V_LEN];
	uint8_t v_prime[HDCP_RP_HMAC_V_LEN / 2];
	uint8_t rcvid_list[HDCP_RP_RCVID_LIST_LEN];
};

struct dp_rp_send_ack {
	uint8_t v[HDCP_RP_HMAC_V_LEN / 2];
};

struct dp_rp_stream_manage {
	uint8_t seq_num_m[HDCP_RP_SEQ_NUM_M_LEN];
	uint8_t k[HDCP_RP_K_LEN];
	uint8_t streamid_type[HDCP_RP_MAX_STREAMID_TYPE_LEN];
};

struct dp_rp_stream_ready {
	uint8_t m_prime[HDCP_RP_HMAC_M_LEN];
};

struct rxinfo {
	uint8_t depth;
	uint8_t dev_count;
	uint8_t max_dev_exd;
	uint8_t max_cascade_exd;
	uint8_t hdcp20_downstream;
	uint8_t hdcp1x_downstream;
};

static uint16_t dp_htons(uint16_t x)
{
	return (
			((x & 0x00FF) << 8) |
			((x & 0xFF00) >> 8)
	       );
}

static void rxinfo_convert_arr2st(uint8_t *arr, struct rxinfo *st)
{
	uint16_t rxinfo_val;

	memcpy((uint8_t *)&rxinfo_val, arr, sizeof(rxinfo_val));
	/* convert to little endian */
	rxinfo_val = dp_htons(rxinfo_val);

	st->depth = (rxinfo_val >> DEPTH_SHIFT) & DEPTH_MASK;
	st->dev_count = (rxinfo_val >> DEV_COUNT_SHIFT) & DEV_COUNT_MASK;
	st->max_dev_exd = (rxinfo_val >> DEV_EXD_SHIFT) & DEV_EXD_MASK;
	st->max_cascade_exd = (rxinfo_val >> CASCADE_EXD_SHIFT) & CASCADE_EXD_MASK;
	st->hdcp20_downstream = (rxinfo_val >> HDCP20_DOWN_SHIFT) & HDCP20_DOWN_MASK;
	st->hdcp1x_downstream = (rxinfo_val >> HDCP1X_DOWN_SHIFT) & HDCP1X_DOWN_MASK;
}

static int is_auth_aborted(uint32_t id)
{
	/* todo: need mutex */
	if (hdcp_dp_info[id].integrity_fail || hdcp_dp_info[id].reauth_req) {
		/* clear flag */
		//dplink_clear_irqflag_all();
		dplink_clear_irqflag_all(id);
		hdcp_err("Authentication is aborted node(%d)\n", id);
		return 1;
	}

	return 0;
}

static int dp_send_protocol_msg \
		   (struct hdcp_link_data *lk, uint8_t msg_id, struct hdcp_msg_info *msg_info)
{
	int ret = TX_AUTH_SUCCESS;

	hdcp_info("Tx->Rx node(%d): %s\n", lk->id, hdcp_msgid_str[msg_id]);
	ret = dp_cap_protocol_msg(lk->id, msg_id,
			msg_info->msg,
			(size_t *)&msg_info->msg_len,
			&lk->tx_ctx,
			&lk->rx_ctx);
	if (ret) {
		hdcp_err("dp_cap_protocol_msg() fail node(%d). ret(0x%08x)\n", lk->id, ret);
		return -TX_AUTH_ERROR_MAKE_PROTO_MSG;
	}

	return TX_AUTH_SUCCESS;
}

int dp_recv_protocol_msg(struct hdcp_link_data *lk, uint8_t msg_id, struct hdcp_msg_info *msg_info)
{
	int ret = TX_AUTH_SUCCESS;

	if (msg_info->msg_len > 0) {
		/* parsing received message */
		ret = dp_decap_protocol_msg(lk->id, msg_id,
				msg_info->msg,
				msg_info->msg_len,
				&lk->tx_ctx,
				&lk->rx_ctx);
		if (ret) {
			hdcp_err("dp_decap_protocol_msg() fail node(%d). ret(0x%08x)\n",
				lk->id, ret);
			ret = -TX_AUTH_ERROR_WRONG_MSG;
		}
	}

	hdcp_info("Rx->Tx node(%d): %s\n", lk->id, hdcp_msgid_str[msg_id]);

	return ret;
}

static int dp_ake_find_masterkey(struct hdcp_link_data *lk, int *found)
{
	int ret = TX_AUTH_SUCCESS;
	uint8_t ekh_mkey[HDCP_AKE_EKH_MKEY_BYTE_LEN] = {0};
	uint8_t m[HDCP_AKE_M_BYTE_LEN] = {0};

	ret = ake_find_masterkey(lk->id, found,
			ekh_mkey, HDCP_AKE_EKH_MKEY_BYTE_LEN,
			m, HDCP_AKE_M_BYTE_LEN);
	if (ret) {
		hdcp_err("fail to find stored km node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int dp_get_hdcp_session_key(struct hdcp_link_data *lk)
{
	return 0;
}

static int dp_put_hdcp_session_key(struct hdcp_link_data *lk)
{
	return 0;
}

static int do_send_ake_init(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_ake_init *m_init;

	/* check abort state firstly,
	   if session is abored by Rx, Tx stops Authentication process
	   */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = dp_send_protocol_msg(lk, DP_AKE_INIT, &msg_info);
	if (ret < 0) {
		hdcp_err("AKE_Init fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	m_init = (struct dp_ake_init *)msg_info.msg;
	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_RTX_W,
			m_init->rtx,
			sizeof(m_init->rtx));
	if (ret) {
		hdcp_err("rtx send fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_TXCAPS_W,
			m_init->txcaps,
			sizeof(m_init->txcaps));
	if (ret) {
		hdcp_err("txcaps send fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

void parse_rxcaps_info(uint8_t *rxcaps, struct hdcp_link_data *lk)
{
	memcpy(lk->rx_ctx.caps, rxcaps, sizeof(lk->rx_ctx.caps));
	if (rxcaps[2] & DP_RXCAPS_REPEATER) {
		hdcp_info("Rx(node%d) is Repeater. rxcaps(0x%02x%02x%02x)\n",
			lk->id, rxcaps[0], rxcaps[1], rxcaps[2]);
		lk->rx_ctx.repeater = 1;
	} else {
		hdcp_info("Rx(node%d) is NOT Repeater. rxcaps(0x%02x%02x%02x)\n",
			lk->id, rxcaps[0], rxcaps[1], rxcaps[2]);
		lk->rx_ctx.repeater = 0;
	}
}

static void parse_receiver_id(uint8_t *cert, struct hdcp_link_data *lk)
{
	memcpy(lk->rx_ctx.receiver_id, cert, RECEIVER_ID_BYTE_LEN);
}

static int do_recv_ake_send_cert(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_ake_send_cert m_send_cert;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = hdcp_dplink_recv(lk->id, HDCP22_MSG_CERT_RX_R,
			m_send_cert.cert_rx,
			sizeof(m_send_cert.cert_rx));
	if (ret) {
		hdcp_err("ake_send_cert cert recv fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
		}

	ret = hdcp_dplink_recv(lk->id, HDCP22_MSG_RRX_R,
			m_send_cert.rrx,
			sizeof(m_send_cert.rrx));
	if (ret) {
		hdcp_err("ake_send_cert rrx recv fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	ret = hdcp_dplink_recv(lk->id, HDCP22_MSG_RXCAPS_R,
			m_send_cert.rxcaps,
			sizeof(m_send_cert.rxcaps));
	if (ret) {
		hdcp_err("ake_send_cert rxcaps recv fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	parse_receiver_id(m_send_cert.cert_rx, lk);
	parse_rxcaps_info(m_send_cert.rxcaps, lk);

	memcpy(msg_info.msg, &m_send_cert, sizeof(struct dp_ake_send_cert));
	msg_info.msg_len = sizeof(struct dp_ake_send_cert);
	ret = dp_recv_protocol_msg(lk, DP_AKE_SEND_CERT, &msg_info);
	if (ret < 0) {
		hdcp_err("recv AKE_Send_Cert fail node(%d)\n", lk->id);
		return -1;
	}

	return 0;
}

static int do_send_ake_nostored_km(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_ake_no_stored_km *m_nostored_km;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = dp_send_protocol_msg(lk, DP_AKE_NO_STORED_KM, &msg_info);
	if (ret < 0) {
		hdcp_err("send AKE_No_Stored_km fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	m_nostored_km = (struct dp_ake_no_stored_km *)msg_info.msg;

	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_EKPUB_KM_W,
			m_nostored_km->ekpub_km,
			sizeof(m_nostored_km->ekpub_km));
	if (ret) {
		hdcp_err("ake_no_stored_km send fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int do_send_ake_stored_km(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_ake_stored_km *m_stored_km;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = dp_send_protocol_msg(lk, DP_AKE_STORED_KM, &msg_info);
	if (ret < 0) {
		hdcp_err("send AKE_stored_km fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	m_stored_km = (struct dp_ake_stored_km *)msg_info.msg;
	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_EKH_KM_W,
			m_stored_km->ekh_km,
			sizeof(m_stored_km->ekh_km));
	if (ret) {
		hdcp_err("ake_stored_km send fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_M_W,
			m_stored_km->m,
			sizeof(m_stored_km->m));
	if (ret) {
		hdcp_err("ake_stored_km send fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int do_recv_ake_send_h_prime(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_ake_send_h_prime m_hprime;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = hdcp_dplink_recv(lk->id, HDCP22_MSG_HPRIME_R,
			m_hprime.h_prime,
			sizeof(m_hprime.h_prime));
	if (ret) {
		hdcp_err("ake_send_h_prime recv fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	memcpy(msg_info.msg, &m_hprime, sizeof(struct dp_ake_send_h_prime));
	msg_info.msg_len = sizeof(struct dp_ake_send_h_prime);
	ret = dp_recv_protocol_msg(lk, DP_AKE_SEND_H_PRIME, &msg_info);
	if (ret < 0) {
		hdcp_err("recv AKE_Send_H_Prime fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int do_recv_ake_send_pairing_info(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_ake_send_pairing_info m_pairing;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = hdcp_dplink_recv(lk->id, HDCP22_MSG_EKH_KM_R,
			m_pairing.ekh_km,
			sizeof(m_pairing.ekh_km));
	if (ret) {
		hdcp_err("ake_send_pairing_info recv fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	memcpy(msg_info.msg, &m_pairing, sizeof(struct dp_ake_send_pairing_info));
	msg_info.msg_len = sizeof(struct dp_ake_send_pairing_info);
	ret = dp_recv_protocol_msg(lk, DP_AKE_SEND_PAIRING_INFO, &msg_info);
	if (ret < 0) {
		hdcp_err("recv AKE_Send_Pairing_Info fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int do_send_lc_init(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_lc_init *m_lc_init;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = dp_send_protocol_msg(lk, DP_LC_INIT, &msg_info);
	if (ret < 0) {
		hdcp_err("send LC_init fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	m_lc_init = (struct dp_lc_init *)msg_info.msg;
	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_RN_W,
			m_lc_init->rn,
			sizeof(m_lc_init->rn));
	if (ret) {
		hdcp_err("lc_init send fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int do_recv_lc_send_l_prime(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_lc_send_l_prime m_lprime;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = hdcp_dplink_recv(lk->id, HDCP22_MSG_LPRIME_R,
			m_lprime.l_prime,
			sizeof(m_lprime.l_prime));
	if (ret) {
		hdcp_err("lc_send_l_prime recv fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	memcpy(msg_info.msg, &m_lprime, sizeof(struct dp_lc_send_l_prime));
	msg_info.msg_len = sizeof(struct dp_lc_send_l_prime);
	ret = dp_recv_protocol_msg(lk, DP_LC_SEND_L_PRIME, &msg_info);

	if (ret < 0) {
		hdcp_err("HDCP recv LC_Send_L_prime fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int do_send_ske_send_eks(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_ske_send_eks *m_ske;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = dp_send_protocol_msg(lk, DP_SKE_SEND_EKS, &msg_info);
	if (ret < 0) {
		hdcp_err("send SKE_SEND_EKS fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	m_ske = (struct dp_ske_send_eks *)msg_info.msg;
	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_EDKEY_KS0_W,
			&m_ske->edkey_ks[0],
			sizeof(m_ske->edkey_ks)/2);
	if (ret) {
		hdcp_err("SKE_send_eks send fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_EDKEY_KS1_W,
			&m_ske->edkey_ks[8],
			sizeof(m_ske->edkey_ks)/2);
	if (ret) {
		hdcp_err("SKE_send_eks send fail node(%d). ret(%x)\n", lk->id, ret);
		return -1;
	}

	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_RIV_W,
			m_ske->riv,
			sizeof(m_ske->riv));
	if (ret) {
		hdcp_err("SKE_send_eks send fail node(%d). ret(%x)\n", lk->id, ret);
		return -1;
	}

#if defined(CONFIG_HDCP2_2_ERRATA_SUPPORT)
	/* HDCP errata defined stream type.
	 * type info is send only for receiver
	 */
	if (!lk->rx_ctx.repeater) {
		ret = hdcp_dplink_send(lk->id, HDCP22_MSG_TYPE_W,
				&m_ske->strm_typ,
				sizeof(uint8_t));
		if (ret) {
			hdcp_err("HDCP : SKE_send_eks type send fail node(%d): %x\n", lk->id, ret);
			return -1;
		}
	}
#endif

	return 0;
}

#if IS_ENABLED(CONFIG_HDCP2_SRM_REVOKE_CHECK)
int hdcp_pnc_session_open(struct pnc_session **session)
{
	int ret = -1;
	int (*fn)(struct pnc_session **session);
	fn = symbol_get(pnc_session_open);
	if (fn) {
		ret = fn(session);
	} else {
		hdcp_err("Failed to get symbol: pnc_session_open!\n");
		ret = -1;
	}
	return ret;
}

void hdcp_pnc_session_close(struct pnc_session *session)
{
	void (*fn)(struct pnc_session *session);
	fn = symbol_get(pnc_session_close);
	if (fn) {
		fn(session);
	} else {
		hdcp_err("Failed to get symbol: pnc_session_close!\n");
	}
}

int hdcp_pnc_session_alloc(struct pnc_session *session, unsigned long size)
{
	int ret = -1;
	int (*fn)(struct pnc_session *session, unsigned long size);
	fn = symbol_get(pnc_session_alloc);
	if (fn) {
		ret = fn(session, size);
	} else {
		hdcp_err("Failed to get symbol: pnc_session_alloc!\n");
		ret = -1;
	}
	return ret;
}

int hdcp_pnc_session_config_by_name(struct pnc_session *session, const char *name)
{
	int ret = -1;
	int (*fn)(struct pnc_session *session, const char *name);
	fn = symbol_get(pnc_session_config_by_name);
	if (fn) {
		ret = fn(session, name);
	} else {
		hdcp_err("Failed to get symbol: pnc_session_config_by_name!\n");
		ret = -1;
	}
	return ret;
}

int hdcp_pnc_session_get_mem(struct pnc_session *session, char **ptr, unsigned long *size)
{
	int ret = -1;
	int (*fn)(struct pnc_session *session, char **ptr, unsigned long *size);
	fn = symbol_get(pnc_session_get_mem);
	if (fn) {
		ret = fn(session, ptr, size);
	} else {
		hdcp_err("Failed to get symbol: pnc_session_get_mem!\n");
		ret = -1;
	}
	return ret;
}

int hdcp_pnc_session_send_request_and_wait_response(struct pnc_session *session, uint32_t request, uint32_t timeout, uint32_t *response)
{
	int ret = -1;
	int (*fn)(struct pnc_session *session, uint32_t request, uint32_t timeout, uint32_t *response);
	fn = symbol_get(pnc_session_send_request_and_wait_response);
	if (fn) {
		ret = fn(session, request, timeout, response);
	} else {
		hdcp_err("Failed to get symbol: pnc_session_send_request_and_wait_response!\n");
		ret = -1;
	}
	return ret;
}

static int check_srm_list(uint32_t len, char *rcvid_list)
{
	int ret = 0;
	uint32_t status = 0;
	struct pnc_session *session;
	char *data = NULL;
	unsigned long size;

	ret = hdcp_pnc_session_open(&session);
	if (ret != 0) {
		hdcp_err("Failed to open session (%d)\n", ret);
		return -TX_AUTH_ERROR_RECV_PROTO_MSG;
	}
	ret = hdcp_pnc_session_alloc(session, 4096);
	if (ret != 0) {
		hdcp_err("Failed to allocate shared memory buffer (%d)\n", ret);
		hdcp_pnc_session_close(session);
		session = NULL;
		return -TX_AUTH_ERROR_RECV_PROTO_MSG;
	}
	ret = hdcp_pnc_session_config_by_name(session, "hdcp");
	hdcp_info("pnc_session_config_by_name(%s): ret=%d.\n", "hdcp", ret);
	if (ret != 0) {
		hdcp_err("Failed to configure service identifier (%d)\n", ret);
		hdcp_pnc_session_close(session);
		session = NULL;
		return -TX_AUTH_ERROR_RECV_PROTO_MSG;
	}

	ret = hdcp_pnc_session_get_mem(session, &data, &size);
	memcpy(data, &len, sizeof(len));
	memcpy(data + sizeof(len), rcvid_list, len);

	do {
		ret = hdcp_pnc_session_send_request_and_wait_response(session,
			42, NO_TIMEOUT, &status);
	} while ((ret == -ERESTARTSYS) || (ret == -EAGAIN));

	hdcp_info("pnc_session_send_request_and_wait_response(): ret=%d.\n", ret);
	if ((ret != 0) || (status != 0)) {
		hdcp_err("Failed to create channel (%d:%u)\n", ret, (unsigned int)status);
		hdcp_pnc_session_close(session);
		session = NULL;
		return -TX_AUTH_ERROR_RECV_PROTO_MSG;
	}
	ret = data[0];
	hdcp_pnc_session_close(session);
	hdcp_info("check_srm_list pnc_session_close().\n");
	return ret;
}
#endif

static int cal_rcvid_list_size(uint8_t *rxinfo, uint32_t *rcvid_size)
{
	struct rxinfo rx;
	int ret = 0;
	rxinfo_convert_arr2st(rxinfo, &rx);

	/* rx_list check */
	if (rx.max_dev_exd || rx.max_cascade_exd)
		return 1;

	*rcvid_size = rx.dev_count * HDCP_RCV_ID_LEN;
	return ret;
}

static int do_recv_rcvid_list(struct hdcp_link_data *lk)
{
	int ret;
	struct dp_rp_send_rcvid_list m_rcvid;
	uint32_t rcvid_size = 0;
	uint32_t node = lk->id;
	struct hdcp_msg_info msg_info;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(node))
		return -TX_AUTH_ERROR_ABORT;

	memset(&m_rcvid, 0x00, sizeof(m_rcvid));

	ret = hdcp_dplink_recv(node, HDCP22_MSG_RXINFO_R,
			m_rcvid.rx_info,
			sizeof(m_rcvid.rx_info));
	if (ret) {
		hdcp_err("verify_receiver_id_list rx_info rcv fail node(%d): ret(%d)\n",
			lk->id, ret);
		return -1;
	}

	ret = hdcp_dplink_recv(node, HDCP22_MSG_SEQ_NUM_V_R,
			m_rcvid.seq_num_v,
			sizeof(m_rcvid.seq_num_v));

	if (ret) {
		hdcp_err("verify_receiver_id_list seq_num_v rcv fail node(%d): ret(%d)\n",
			lk->id, ret);
		return -1;
	}

	if ((m_rcvid.seq_num_v[0] || m_rcvid.seq_num_v[1] || m_rcvid.seq_num_v[2])
		&& hdcp_dp_info[node].rp_ready < 2) {
		hdcp_err("Initial seq_num_v is non_zero node(%d).\n", lk->id);
		return -1;
	}

	ret = hdcp_dplink_recv(node, HDCP22_MSG_VPRIME_R,
			m_rcvid.v_prime,
			sizeof(m_rcvid.v_prime));
	if (ret) {
		hdcp_err("verify_receiver_id_list seq_num_v rcv fail node(%d): ret(%d)\n",
			lk->id, ret);
		return -1;
	}

	ret = cal_rcvid_list_size(m_rcvid.rx_info, &rcvid_size);
	if (ret) {
		hdcp_err("Cal_rcvid_list Fail node(%d) ! (%d)\n", lk->id, ret);
		return -1;
	}

	ret = hdcp_dplink_recv(node, HDCP22_MSG_RECV_ID_LIST_R,
			m_rcvid.rcvid_list,
			rcvid_size);
	if (ret) {
		hdcp_err("verify_receiver_id_list rcvid_list rcv fail node(%d): ret(%d)\n",
			lk->id, ret);
		return -1;
	}

#if IS_ENABLED(CONFIG_HDCP2_SRM_REVOKE_CHECK)
	ret = check_srm_list(rcvid_size, m_rcvid.rcvid_list);
	if (ret) {
		hdcp_err("verify_receiver_id_list rcvid_list fail: ret(%d)\n", ret);
		return -1;
	}
#endif
	memcpy(msg_info.msg, &m_rcvid, sizeof(struct dp_rp_send_rcvid_list));
	msg_info.msg_len = sizeof(struct dp_rp_send_rcvid_list);
	ret = dp_recv_protocol_msg(lk, DP_REPEATERAUTH_SEND_RECEIVERID_LIST, &msg_info);
	if (ret < 0) {
		hdcp_err("recv RepeaterAuth_Send_ReceiverID_List fail node(%d)\n", lk->id);
		return -1;
	}

	return 0;
}

static int do_send_rp_ack(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_rp_send_ack *m_ack;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = dp_send_protocol_msg(lk, DP_REPEATERAUTH_SEND_AKE, &msg_info);
	if (ret < 0) {
		hdcp_err("send RepeaterAuth_Send_Ack fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	m_ack = (struct dp_rp_send_ack *)msg_info.msg;
	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_V_W,
			m_ack->v,
			sizeof(m_ack->v));
	if (ret) {
		hdcp_err("V send fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int do_send_rp_stream_manage(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_rp_stream_manage *m_strm;
	uint16_t stream_num;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = dp_send_protocol_msg(lk, DP_REPEATERAUTH_STREAM_MANAGE, &msg_info);
	if (ret < 0) {
		hdcp_err("send RepeaterAuth_Stream_Manage node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	m_strm = (struct dp_rp_stream_manage *)msg_info.msg;
	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_SEQ_NUM_M_W,
			m_strm->seq_num_m,
			sizeof(m_strm->seq_num_m));
	if (ret) {
		hdcp_err("seq_num_M send fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_K_W,
			m_strm->k,
			sizeof(m_strm->k));
	if (ret) {
		hdcp_err("k send fail node(%d). ret(%x)\n", lk->id, ret);
		return -1;
	}

	stream_num = lk->tx_ctx.strm.dp.stream_num;
	ret = hdcp_dplink_send(lk->id, HDCP22_MSG_STREAMID_TYPE_W,
			m_strm->streamid_type,
			stream_num * HDCP_RP_STREAMID_TYPE_LEN);
	if (ret) {
		hdcp_err("Streamid_Type send fail node(%d). ret(%x)\n", lk->id, ret);
		return -1;
	}

	return 0;
}

static int do_recv_rp_stream_ready(struct hdcp_link_data *lk)
{
	int ret;
	struct hdcp_msg_info msg_info;
	struct dp_rp_stream_ready m_strm_ready;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	ret = hdcp_dplink_recv(lk->id, HDCP22_MSG_MPRIME_R,
			m_strm_ready.m_prime,
			sizeof(m_strm_ready.m_prime));
	if (ret) {
		hdcp_err("M' recv fail node(%d). ret(%d)\n", lk->id, ret);
		return -1;
	}

	memcpy(msg_info.msg, &m_strm_ready, sizeof(struct dp_rp_stream_ready));
	msg_info.msg_len = sizeof(struct dp_rp_stream_ready);
	ret = dp_recv_protocol_msg(lk, DP_REPEATERAUTH_STREAM_READY, &msg_info);
	if (ret < 0) {
		hdcp_err("HDCP recv RepeaterAuth_Stream_Ready fail node(%d). ret(%d)\n",
			lk->id, ret);
		return -1;
	}

	return 0;
}

static int check_h_prime_ready(struct hdcp_link_data *lk, int h_timeout)
{
	int i = 0;
	uint8_t status = 0;
	uint32_t node = lk->id;
	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	msleep(HDCP_MSG_READ_WAIT);

	/* HDCP spec is 1 sec. but we give margin enough during test */
	while (i < HDCP_HPRIME_READ_RETRY) {
		/* check abort state firstly,
		 * if session is abored by Rx, Tx stops Authentication process
		 */
		if (is_auth_aborted(node))
			return -TX_AUTH_ERROR_ABORT;

		/* received from CP_IRQ */
		if (hdcp_dp_info[node].hprime_ready) {
			/* reset flag */
			hdcp_info("hprime ready node%d within (%dms)\n",
				lk->id, (HDCP_MSG_READ_WAIT * i));
			hdcp_dp_info[node].hprime_ready = 0;
			return 0;
		}

		/* check as polling mode */
		hdcp_dplink_get_rxinfo(lk->id, &status);
		if (status & DP_RXSTATUS_HPRIME_AVAILABLE) {
			/* reset flag */
			hdcp_dp_info[node].hprime_ready = 0;
			hdcp_info("hprime ready node(%d) within (%dms)\n",
				lk->id, (HDCP_MSG_READ_WAIT * i));
			return 0;
		}

		if (h_timeout < HDCP_MSG_READ_WAIT*i)
			hdcp_err("hprime node(%d) timeout(%dms)\n",
				lk->id, (HDCP_MSG_READ_WAIT * i));

		msleep(HDCP_MSG_READ_WAIT);
		i++;
	}

	hdcp_err("hprime node(%d) timeout(%dms)\n",
		lk->id, (HDCP_MSG_READ_WAIT * i));

	return -1;
}

static int check_pairing_ready(struct hdcp_link_data *lk)
{
	uint32_t node;
	uint8_t status = 0;
	int i = 0;

	/* check abort state firstly,
	 * if session is abored by Rx, Tx stops Authentication process
	 */
	node = lk->id;

	if (is_auth_aborted(lk->id))
		return -TX_AUTH_ERROR_ABORT;

	msleep(220);
	/* HDCP spec is 200ms. but we give margin 110ms */
	while (i < 2) {
		/* check abort state firstly,
		 * if session is abored by Rx, Tx stops Authentication process
		 */
		if (is_auth_aborted(lk->id))
			return -TX_AUTH_ERROR_ABORT;

		/* received from CP_IRQ */
		if (hdcp_dp_info[node].pairing_ready) {
			/* reset flag */
			hdcp_dp_info[node].pairing_ready = 0;
			return 0;
		}

		/* check as polling mode */
		hdcp_dplink_get_rxinfo(node, &status);
		if (status & DP_RXSTATUS_PAIRING_AVAILABLE) {
			/* reset flag */
			hdcp_dp_info[node].pairing_ready = 0;
			return 0;
		}

		msleep(110);
		i++;
	}

	hdcp_err("pairing node(%d) timeout(%dms)\n", lk->id, (110 * i));
	return -1;
}

static int check_rcvidlist_ready(struct hdcp_link_data *lk)
{
	int i = 0;
	uint8_t status = 0;
	uint32_t node = lk->id;
	/* HDCP spec is 3 sec */
	while (i < 30) {
		/* check abort state firstly,
		 * if session is abored by Rx, Tx stops Authentication process
		 */
		if (is_auth_aborted(node))
			return -TX_AUTH_ERROR_ABORT;

		/* received from CP_IRQ */
		if (hdcp_dp_info[node].rp_ready > hdcp_dp_info[node].rp_ready_s) {
			/* reset flag */
			hdcp_dp_info[node].rp_ready_s = hdcp_dp_info[node].rp_ready;
			return 0;
		}

		/* check as polling mode */
		hdcp_dplink_get_rxinfo(node, &status);
		if (status & DP_RXSTATUS_READY) {
			hdcp_dp_info[node].rp_ready++;
			return 0;
		}

		msleep(110);
		i++;
	}

	hdcp_err("receiver ID list node(%d) timeout(%dms)\n", lk->id, (110 * i));
	return -TX_AUTH_ERROR_TIME_EXCEED;
}

#if IS_ENABLED(CONFIG_HDCP2_SRM_REVOKE_CHECK)
static int check_srm_revocation(struct hdcp_link_data *lk)
{
	return check_srm_list(RECEIVER_ID_BYTE_LEN, lk->rx_ctx.receiver_id);
}
#endif

int dplink_exchange_master_key(uint32_t id, struct hdcp_link_data *lk)
{
	int rval = TX_AUTH_SUCCESS;
	int key_found;
	int hprime_timeout;
	(void) id;

	do {
		/* send Tx -> Rx: AKE_init */
		if (do_send_ake_init(lk) < 0) {
			hdcp_err("send_ake_int fail node(%d)\n", lk->id);
			rval = -TX_AUTH_ERROR_SEND_PROTO_MSG;
			break;
		}

		/* HDCP spec defined 110ms as min delay after write AKE_Init */
		msleep(110);

		/* recv Rx->Tx: AKE_Send_Cert message */
		if (do_recv_ake_send_cert(lk) < 0) {
			hdcp_err("recv_ake_send_cert fail node(%d)\n", lk->id);
			rval = -TX_AUTH_ERROR_RECV_PROTO_MSG;
			break;
		}

		/* send Tx->Rx: AKE_Stored_km or AKE_No_Stored_km message */
		if (dp_ake_find_masterkey(lk, &key_found) < 0) {
			hdcp_err("find master key fail node(%d)\n", lk->id);
			rval = -TX_AUTH_ERROR_MAKE_PROTO_MSG;
			break;
		}
		if (!key_found) {
			if (do_send_ake_nostored_km(lk) < 0) {
				hdcp_err("ake_send_nostored_km fail node(%d)\n", lk->id);
				rval = -TX_AUTH_ERROR_SEND_PROTO_MSG;
				break;
			}
			hprime_timeout = HDCP_HPRIME_TIMEOUT_NOKM;
			lk->stored_km = HDCP_WITHOUT_STORED_KM;
		} else {
			if (do_send_ake_stored_km(lk) < 0) {
				hdcp_err("ake_send_stored_km fail node(%d)\n", lk->id);
				rval = -TX_AUTH_ERROR_SEND_PROTO_MSG;
				break;
			}
			hprime_timeout = HDCP_HPRIME_TIMEOUT_STKM;
			lk->stored_km = HDCP_WITH_STORED_KM;
		}

#if IS_ENABLED(CONFIG_HDCP2_SRM_REVOKE_CHECK)
		if (check_srm_revocation(lk) < 0) {
			hdcp_err("SRM revocation check failed\n");
			rval = -TX_AUTH_ERROR_RECV_PROTO_MSG;
			break;
		}
#endif

		if (check_h_prime_ready(lk, hprime_timeout) < 0) {
			hdcp_err("Cannot read H prime node(%d)\n", lk->id);
			rval = -TX_AUTH_ERROR_RECV_PROTO_MSG;
			break;
		}

		/* recv Rx->Tx: AKE_Send_H_Prime message */
		if (do_recv_ake_send_h_prime(lk) < 0) {
			hdcp_err("recv_ake_send_h_prime fail node(%d)\n", lk->id);
			rval = -TX_AUTH_ERROR_RECV_PROTO_MSG;
			break;
		}

		if (lk->stored_km == HDCP_WITHOUT_STORED_KM) {
			if (check_pairing_ready(lk) < 0) {
				hdcp_err("Cannot read pairing info node(%d)\n", lk->id);
				rval = -TX_AUTH_ERROR_RECV_PROTO_MSG;
				break;
			}

			/* recv Rx->Tx: AKE_Send_Pairing_Info message */
			if (do_recv_ake_send_pairing_info(lk) < 0) {
				hdcp_err("recv_ake_send_h_prime fail node(%d)\n", lk->id);
				rval = -TX_AUTH_ERROR_RECV_PROTO_MSG;
				break;
			}
		}
	} while (0);

	return rval;
}

int dplink_locality_check(struct hdcp_link_data *lk)
{
	int i;

	for (i = 0; i < MAX_LC_RETRY; i++) {
		/* send Tx -> Rx: LC_init */
		if (do_send_lc_init(lk) < 0) {
			hdcp_err("send_lc_init fail node(%d)\n", lk->id);
			return -TX_AUTH_ERROR_SEND_PROTO_MSG;
		}

		/* wait until max dealy */
		msleep(16);

		/* recv Rx -> Tx: LC_Send_L_Prime */
		if (do_recv_lc_send_l_prime(lk) < 0) {
			hdcp_err("recv_lc_send_l_prime fail node(%d)\n", lk->id);
			/* retry */
			continue;
		} else {
			hdcp_debug("LC success node(%d). retryed(%d)\n", lk->id, i);
			break;
		}
	}

	if (i == MAX_LC_RETRY) {
		hdcp_err("LC check fail node(%d). exceed retry count(%d)\n", lk->id, i);
		return -TX_AUTH_ERROR_RETRYCOUNT_EXCEED;
	}

	return TX_AUTH_SUCCESS;
}

int dplink_exchange_session_key(struct hdcp_link_data *lk)
{
	/* find session key from the session data */
	if (dp_get_hdcp_session_key(lk) < 0) {
		hdcp_err("dp_get_hdcp_session_key() fail node(%d)\n", lk->id);
		return -TX_AUTH_ERRRO_RESTORE_SKEY;
	}

	/* Send Tx -> Rx: SKE_Send_Eks */
	if (do_send_ske_send_eks(lk) < 0) {
		hdcp_err("send_ske_send_eks fail node(%d)\n", lk->id);
		return -TX_AUTH_ERROR_SEND_PROTO_MSG;
	}

	if (dp_put_hdcp_session_key(lk) < 0) {
		hdcp_err("put_hdcp_session_key() fail node(%d)\n", lk->id);
		return -TX_AUTH_ERROR_STORE_SKEY;
	}

	return TX_AUTH_SUCCESS;
}

int dplink_evaluate_repeater(struct hdcp_link_data *lk)
{
	if (lk->rx_ctx.repeater)
		return TRUE;
	else
		return FALSE;
}

int dplink_wait_for_receiver_id_list(struct hdcp_link_data *lk)
{
	int ret;

	ret = check_rcvidlist_ready(lk);
	if (ret < 0)
		return ret;
	else
		return TX_AUTH_SUCCESS;
}

int dplink_verify_receiver_id_list(struct hdcp_link_data *lk)
{
	/* recv Rx->Tx: RepeaterAuth_ReceiverID_List message */
	if (do_recv_rcvid_list(lk) < 0) {
		hdcp_err("recv_receiverID_list fail node(%d)\n", lk->id);
		return -TX_AUTH_ERROR_RECV_PROTO_MSG;
	}

	return TX_AUTH_SUCCESS;
}

int dplink_send_receiver_id_list_ack(struct hdcp_link_data *lk)
{
	/* Send Tx -> Rx: RepeaterAuth_Send_Ack */
	if (do_send_rp_ack(lk) < 0) {
		hdcp_err("send_rp_ack fail node(%d)\n", lk->id);
		return -TX_AUTH_ERROR_SEND_PROTO_MSG;
	}

	return TX_AUTH_SUCCESS;
}

int dplink_determine_rx_hdcp_cap(uint32_t id, struct hdcp_link_data *lk)
{
	int ret;
	uint8_t rxcaps[HDCP_CAPS_BYTE_LEN];


	ret = hdcp_dplink_recv(lk->id, HDCP22_MSG_RXCAPS_R,
			rxcaps,
			sizeof(rxcaps));
	if (ret) {
		hdcp_err("check rx caps recv fail node(%d): ret(%d)\n", lk->id, ret);
		return -1;
	}

	if (!(rxcaps[2] & DP_RXCAPS_HDCP_CAPABLE)) {
		hdcp_err("Rx is not support HDCP node(%d). rxcaps(0x%02x%02x%02x)\n",
					lk->id, rxcaps[0], rxcaps[1], rxcaps[2]);
		return -1;
	}

	if (rxcaps[0] != DP_RXCAPS_HDCP_VERSION_2) {
		hdcp_err("Rx is not HDCP2.x node(%d). rxcaps(0x%02x%02x%02x)\n",
					lk->id, rxcaps[0], rxcaps[1], rxcaps[2]);
		return -1;
	}

	return 0;
}

int dplink_stream_manage(struct hdcp_link_data *lk)
{
	/* Send Tx -> Rx: RepeaterAuth_Stream_Manage */
	if (do_send_rp_stream_manage(lk) < 0) {
		hdcp_err("send_rp_stream_manage fail node(%d)\n", lk->id);
		return -TX_AUTH_ERROR_SEND_PROTO_MSG;
	}

	/* HDCP spec define 110ms as min delay. But we give 110ms margin */
	msleep(220);

	/* recv Rx->Tx: RepeaterAuth_Stream_Ready message */
	if (do_recv_rp_stream_ready(lk) < 0) {
		hdcp_err("recv_rp_stream_ready fail node(%d)\n", lk->id);
		return -TX_AUTH_ERROR_RECV_PROTO_MSG;
	}

	return TX_AUTH_SUCCESS;
}

int dplink_set_paring_available(uint32_t id)
{
	hdcp_dp_info[id].pairing_ready = 1;
	return 0;
}

int dplink_set_hprime_available(uint32_t id)
{
	hdcp_dp_info[id].hprime_ready = 1;
	return 0;
}

int dplink_set_rp_ready(uint32_t id)
{
	hdcp_dp_info[id].rp_ready++;
	return 0;
}

int dplink_set_reauth_req(uint32_t id)
{
	hdcp_dp_info[id].reauth_req = 1;
	return 0;
}

int dplink_set_integrity_fail(uint32_t id)
{
	dplink_clear_irqflag_all(id);
	hdcp_dp_info[id].integrity_fail = 1;
	return 0;
}

void dplink_clear_irqflag_all(uint32_t id)
{
	hdcp_dp_info[id].pairing_ready = 0;
	hdcp_dp_info[id].hprime_ready = 0;
	hdcp_dp_info[id].rp_ready = 0;
	hdcp_dp_info[id].rp_ready_s = 0;
	hdcp_dp_info[id].reauth_req = 0;
	hdcp_dp_info[id].integrity_fail = 0;
}

MODULE_LICENSE("GPL");
