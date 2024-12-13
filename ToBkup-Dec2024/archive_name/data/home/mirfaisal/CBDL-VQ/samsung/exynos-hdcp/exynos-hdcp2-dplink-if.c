// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/module.h>
#include "exynos-hdcp2.h"
#include "exynos-hdcp2-log.h"
#include "exynos-hdcp2-dplink-reg.h"
#include "exynos-hdcp2-dplink-if.h"

struct dp_regs *dp_reg;
struct hdcp_dp_info hdcp_dp_info[MAX_NUM_OF_HDCP];
extern enum auth_state auth_proc_state[MAX_NUM_OF_HDCP];

#define CONFIG_DRM_EXYNOS9_HDCP22
#if defined(CONFIG_DRM_EXYNOS9_HDCP22)
static void (*pdp_hdcp22_enable)(struct dp_regs *regs, u32 en, u32 type);
static uint8_t (*pdp_hdcp22_get_mst_streamid)(struct dp_regs *regs);
static int (*pdp_dpcd_read_for_hdcp22)(struct dp_regs *regs,
	unsigned int offset, void *buffer, size_t size);
static int (*pdp_dpcd_write_for_hdcp22)(struct dp_regs *regs,
	unsigned int offset, void *buffer, size_t size);
void exynos_drm_dp_register_func_to_hdcp2(u32 id, struct dp_regs *regs,
	void (*hdcp_enable)(struct dp_regs *regs, u32 en, u32 type),
	uint8_t (*hdcp_mst_strm_id_get)(struct dp_regs *regs),
	int (*dpcd_read)(struct dp_regs *regs, unsigned int offset, void *buffer, size_t size),
	int (*dpcd_write)(struct dp_regs *regs, unsigned int offset, void *buffer, size_t size));
#else
void pdp_hdcp22_enable(struct dp_regs *regs, u32 en, u32 type);
uint8_t pdp_hdcp22_get_mst_streamid(struct dp_regs *regs);
int pdp_dpcd_read_for_hdcp22(struct dp_regs *regs, unsigned int offset, void *data, size_t size);
int pdp_dpcd_write_for_hdcp22(struct dp_regs *regs, unsigned int offset, void *data, size_t size);
#endif

#if defined(CONFIG_HDCP2_EMULATION_MODE)

#define NETLINK_HDCP		31
#define SOCK_BUF_SIZE		(1024 * 512)
#define NETLINK_PORT		1000

typedef enum {
	PREPARE_TO_RECV = 0,
	PREPARE_TO_SEND,
	RECEIVED_DONE,
	CONFIG_REAUTH = 0xFFFF,
} nl_msg_t;

struct sock *nl_sk;
struct sk_buff sk_buf;
uint8_t dplink_wait;
uint8_t *nl_data;
struct nlmsghdr *nlh;
int hdcp22_enable;

void cb_hdcp_nl_recv_msg(struct sk_buff *skb);

int hdcp_dplink_init(void)
{
	struct netlink_kernel_cfg cfg = {
		.input	= cb_hdcp_nl_recv_msg,
	};

	nl_sk = netlink_kernel_create(&init_net, NETLINK_HDCP, &cfg);
	if (!nl_sk) {
		hdcp_err("Error creating socket.\n");
		return -EINVAL;
	}

	nl_data = (uint8_t *)kzalloc(SOCK_BUF_SIZE, GFP_KERNEL);
	if (!nl_data) {
		hdcp_err("Netlink Socket buffer alloc fail\n");
		return -EINVAL;
	}

	dplink_wait = 1;

	return 0;
}

/* callback for netlink driver */
void cb_hdcp_nl_recv_msg(struct sk_buff *skb)
{
	nlh = (struct nlmsghdr *)skb->data;

	memcpy(nl_data, (uint8_t *)nlmsg_data(nlh), nlmsg_len(nlh));
	dplink_wait = 0;
}

static void nl_recv_msg(nl_msg_t msg)
{
	/* todo: change to not a busy wait */
	while (dplink_wait) {
		hdcp_err("wait dplink_wait\n");
		msleep(1000);
	}

	dplink_wait = 1;
}

static void nl_recv_data(uint8_t *data, uint32_t size)
{
	/* todo: change to not a busy wait */
	while (dplink_wait) {
		hdcp_err("wait dplink_wait\n");
		msleep(1000);
	}

	memcpy(data, nl_data, size);

	dplink_wait = 1;
}

static int nl_send_data(uint8_t *data, uint32_t size)
{
	struct sk_buff *skb_out;
	int ret;

	skb_out = nlmsg_new(size, 0);
	if (!skb_out) {
		hdcp_err("Failed to allocate new skb\n");
		return -1;
	}
	nlh = nlmsg_put(skb_out, 0, 0, 2, size, NLM_F_REQUEST);
	if (!nlh) {
		hdcp_err("fail to nlmsg_put()\n");
		return -1;
	}

	NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
	memcpy(nlmsg_data(nlh), data, size);

	ret = nlmsg_unicast(nl_sk, skb_out, NETLINK_PORT);
	if (ret < 0) {
		hdcp_err("Error while sending to user(%d)\n", ret);
		return -1;
	}

	return 0;
}

static int nl_send_msg(nl_msg_t msg)
{
	return nl_send_data((uint8_t *)&msg, sizeof(nl_msg_t));
}

int hdcp_dplink_recv(uint32_t id, uint32_t msg_name, uint8_t *data, uint32_t size)
{
	(void) id;

	nl_send_msg(PREPARE_TO_SEND);
	nl_recv_data(data, size);
	nl_send_msg(RECEIVED_DONE);
	return 0;
}

int hdcp_dplink_send(uint32_t id, uint32_t msg_name, uint8_t *data, uint32_t size)
{
	(void) id;

	nl_send_msg(PREPARE_TO_RECV);
	nl_recv_msg(RECEIVED_DONE);
	nl_send_data(data, size);
	return 0;
}

int hdcp_dplink_is_enabled_hdcp22(uint32_t node)
{
	(void) node;
	return hdcp22_enable;
}

void hdcp_dplink_config(uint32_t node, int en, uint32_t type)
{
	(void) node;
	(void) type;

	if (!en) {
		nl_send_msg(CONFIG_REAUTH);
	}

	hdcp22_enable = en;
}
#else

/* Address define for HDCP within DPCD address space */
static unsigned int dpcd_addr[NUM_HDCP22_MSG_NAME] = {
	DPCD_ADDR_HDCP22_Rtx,
	DPCD_ADDR_HDCP22_TxCaps,
	DPCD_ADDR_HDCP22_cert_rx,
	DPCD_ADDR_HDCP22_Rrx,
	DPCD_ADDR_HDCP22_RxCaps,
	DPCD_ADDR_HDCP22_Ekpub_km,
	DPCD_ADDR_HDCP22_Ekh_km_w,
	DPCD_ADDR_HDCP22_m,
	DPCD_ADDR_HDCP22_Hprime,
	DPCD_ADDR_HDCP22_Ekh_km_r,
	DPCD_ADDR_HDCP22_rn,
	DPCD_ADDR_HDCP22_Lprime,
	DPCD_ADDR_HDCP22_Edkey0_ks,
	DPCD_ADDR_HDCP22_Edkey1_ks,
	DPCD_ADDR_HDCP22_riv,
	DPCD_ADDR_HDCP22_RxInfo,
	DPCD_ADDR_HDCP22_seq_num_V,
	DPCD_ADDR_HDCP22_Vprime,
	DPCD_ADDR_HDCP22_Rec_ID_list,
	DPCD_ADDR_HDCP22_V,
	DPCD_ADDR_HDCP22_seq_num_M,
	DPCD_ADDR_HDCP22_k,
	DPCD_ADDR_HDCP22_stream_IDtype,
	DPCD_ADDR_HDCP22_Mprime,
	DPCD_ADDR_HDCP22_RxStatus,
	DPCD_ADDR_HDCP22_Type,
};

int hdcp_dplink_init(void)
{
	/* todo */
	return 0;
}

void hdcp_dplink_config(uint32_t node, int en, uint32_t type)
{
	hdcp_info("node(%d) en(%d) typ(%d)\n", node, en, type);
	pdp_hdcp22_enable(hdcp_dp_info[node].dp_reg, en, type);
}

int hdcp_dplink_is_enabled_hdcp22(uint32_t node)
{
	(void) node;
	/* todo: check hdcp22 enable */
	return 1;
}

static void hdcp_dpcd_debug(uint8_t *data, int size)
{
	int i;

	for (i = 0; i < size; i++)
		hdcp_debug("%X  ", data[i]);
	hdcp_debug("\n");
}

static int hdcp_dpcd_read(struct dp_regs *regs,
	unsigned int offset, void *buffer, size_t size)
{
	int r_size;
	int ret;

	r_size = pdp_dpcd_read_for_hdcp22(regs, offset, buffer, size);

	if (r_size < 0) {
		hdcp_err("fail to access dpcd_read\n");
		ret = DPCD_ERR_R_ACCESS;
	} else if (r_size != size) {
		hdcp_err("fail to read request size(%ldB) read size(%dB)\n", size, r_size);
		ret = DPCD_ERR_R_SIZE;
	} else {
		ret = DPCD_SUCCESS;
		hdcp_dpcd_debug((uint8_t *)buffer, r_size);
	}
	return ret;
}

static int hdcp_dpcd_write(struct dp_regs *regs,
	unsigned int offset, void *buffer, size_t size)
{
	int w_size;
	int ret;

	w_size = pdp_dpcd_write_for_hdcp22(regs, offset, buffer, size);

	if (w_size < 0) {
		hdcp_err("fail to access dpcd_write\n");
		ret = DPCD_ERR_W_ACCESS;
	} else if (w_size != size) {
		hdcp_err("fail to write request size(%ldB) write size(%dB)\n", size, w_size);
		ret = DPCD_ERR_W_SIZE;
	} else {
		ret = DPCD_SUCCESS;
		hdcp_dpcd_debug((uint8_t *)buffer, w_size);
	}
	return ret;
}

int hdcp_dplink_recv(uint32_t id, uint32_t msg_name, uint8_t *data, size_t size)
{
	size_t i;
	size_t remain;
	int ret = 0;

	hdcp_debug("ID(%d), msg_name(%d): 0x%X, size(%ld)\n",
		id, msg_name, dpcd_addr[msg_name], size);
	if (size > DPCD_PACKET_SIZE) {
		for (i = 0; i < (size / DPCD_PACKET_SIZE); i++) {
			ret = hdcp_dpcd_read(hdcp_dp_info[id].dp_reg,
					dpcd_addr[msg_name] + i * DPCD_PACKET_SIZE,
					&data[i * DPCD_PACKET_SIZE], DPCD_PACKET_SIZE);
			if (ret)
				return ret;
		}

		remain = size % DPCD_PACKET_SIZE;

		if (remain) {
			ret = hdcp_dpcd_read(hdcp_dp_info[id].dp_reg,
					dpcd_addr[msg_name] + i * DPCD_PACKET_SIZE,
					&data[i * DPCD_PACKET_SIZE], remain);
			if (ret)
				return ret;
		}
	} else {
		ret = hdcp_dpcd_read(hdcp_dp_info[id].dp_reg, dpcd_addr[msg_name], data, size);
	}

	return ret;
}

int hdcp_dplink_send(uint32_t id, uint32_t msg_name, uint8_t *data, size_t size)
{
	size_t i;
	int ret;

	hdcp_debug("ID(%d), msg_name(%d): 0x%X, size(%ld)\n",
		id, msg_name, dpcd_addr[msg_name], size);
	if (size > DPCD_PACKET_SIZE) {
		for (i = 0; i < (size / DPCD_PACKET_SIZE); i++) {
			ret = hdcp_dpcd_write(hdcp_dp_info[id].dp_reg,
					dpcd_addr[msg_name] + i * DPCD_PACKET_SIZE,
					&data[i * DPCD_PACKET_SIZE], DPCD_PACKET_SIZE);
			if (ret)
				return ret;
		}
	} else {
		ret = hdcp_dpcd_write(hdcp_dp_info[id].dp_reg, dpcd_addr[msg_name], data, size);
	}
	return ret;
}
#endif

static uint8_t stream_id[MAX_NUM_OF_STRM] = {0x00,};
int hdcp_dplink_get_stream_info(uint32_t id, uint16_t *num, uint8_t *strm_id)
{
	uint8_t i, k;
	uint8_t strm_info;

	strm_info = pdp_hdcp22_get_mst_streamid(hdcp_dp_info[id].dp_reg);
	*num = strm_info >> 4;

	for (i = 0, k = 0; i < MAX_NUM_OF_STRM && (k < *num); i++) {
		if (strm_info & (0x1<<i)) {
			hdcp_info("stream_id[%d](%d)\n", k, stream_id[k]);
			stream_id[k++] = i + 1;
		}
	}
	memcpy(strm_id, stream_id, MAX_NUM_OF_STRM);

	return 0;
}

#if defined(CONFIG_DRM_EXYNOS9_HDCP22)
void exynos_drm_dp_register_func_to_hdcp2(u32 id, struct dp_regs *regs,
	void (*hdcp_enable)(struct dp_regs *regs, u32 en, u32 type),
	uint8_t (*hdcp_mst_strm_id_get)(struct dp_regs *regs),
	int (*dpcd_read)(struct dp_regs *regs, unsigned int offset, void *buffer, size_t size),
	int (*dpcd_write)(struct dp_regs *regs, unsigned int offset, void *buffer, size_t size))
{
	hdcp_dp_info[id].dp_reg = regs;
	hdcp_dp_info[id].id = id;
	dp_reg = regs;
	pdp_hdcp22_enable = hdcp_enable;
	pdp_hdcp22_get_mst_streamid = hdcp_mst_strm_id_get;
	pdp_dpcd_read_for_hdcp22 = dpcd_read;
	pdp_dpcd_write_for_hdcp22 = dpcd_write;
}
EXPORT_SYMBOL_GPL(exynos_drm_dp_register_func_to_hdcp2);
#else
int pdp_dpcd_read_for_hdcp22(struct dp_regs *regs, unsigned int offset, void *buffer, size_t size)
{
	return 0;
}
uint8_t pdp_hdcp22_get_mst_streamid(struct dp_regs *regs)
{
	return 0;
}
int pdp_dpcd_write_for_hdcp22(struct dp_regs *regs, unsigned int offset, void *buffer, size_t size)
{
	return 0;
}
void pdp_hdcp22_enable(struct dp_regs *regs, u32 en, u32 type)
{
	return;
}
#endif
MODULE_LICENSE("GPL");
