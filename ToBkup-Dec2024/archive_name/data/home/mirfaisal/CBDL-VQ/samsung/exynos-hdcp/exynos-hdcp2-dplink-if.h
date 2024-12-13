// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __EXYNOS_HDCP2_DPLINK_IF_H__
#define __EXYNOS_HDCP2_DPLINK_IF_H__

enum hdcp22_msg_name {
	HDCP22_MSG_RTX_W,
	HDCP22_MSG_TXCAPS_W,
	HDCP22_MSG_CERT_RX_R,
	HDCP22_MSG_RRX_R,
	HDCP22_MSG_RXCAPS_R,
	HDCP22_MSG_EKPUB_KM_W,
	HDCP22_MSG_EKH_KM_W,
	HDCP22_MSG_M_W,
	HDCP22_MSG_HPRIME_R,
	HDCP22_MSG_EKH_KM_R,
	HDCP22_MSG_RN_W,
	HDCP22_MSG_LPRIME_R,
	HDCP22_MSG_EDKEY_KS0_W,
	HDCP22_MSG_EDKEY_KS1_W,
	HDCP22_MSG_RIV_W,
	HDCP22_MSG_RXINFO_R,
	HDCP22_MSG_SEQ_NUM_V_R,
	HDCP22_MSG_VPRIME_R,
	HDCP22_MSG_RECV_ID_LIST_R,
	HDCP22_MSG_V_W,
	HDCP22_MSG_SEQ_NUM_M_W,
	HDCP22_MSG_K_W,
	HDCP22_MSG_STREAMID_TYPE_W,
	HDCP22_MSG_MPRIME_R,
	HDCP22_MSG_RXSTATUS_R,
	HDCP22_MSG_TYPE_W,
	NUM_HDCP22_MSG_NAME,
};

#define DP_HDCP22_DISABLE		0
#define DP_HDCP22_ENABLE		1
#define DP_HPD_STATUS_ZERO		2
#define DP_HDCP22_RESUMED		3

#define DP_HDCP22_STREAM_NUM		0x01

/* ERROR CODE FOR DPCD_R/W */
#define DPCD_SUCCESS			0x0000
#define DPCD_ERR_R_SIZE			0x6100
#define DPCD_ERR_R_ACCESS		0x6101
#define DPCD_ERR_W_SIZE			0x6200
#define DPCD_ERR_W_ACCESS		0x6201

/* Common */
#ifndef __iomem
typedef unsigned long io_addr_t;
#else
typedef void __iomem *io_addr_t;
#endif

struct dp_regs {
	io_addr_t link_addr;
	io_addr_t phy_addr;
	io_addr_t tca_addr;
};

struct hdcp_dp_info {
	uint32_t id;
	uint8_t pairing_ready;
	uint8_t hprime_ready;
	uint8_t rp_ready;
	uint8_t rp_ready_s;
	uint8_t reauth_req;
	uint8_t integrity_fail;
	struct dp_regs *dp_reg;
};


int hdcp_dplink_init(void);
void hdcp_dplink_config(uint32_t node, int en, uint32_t type);
int hdcp_dplink_is_enabled_hdcp22(uint32_t node);
int hdcp_dplink_recv(uint32_t id, uint32_t msg_name, uint8_t *data, size_t size);
int hdcp_dplink_send(uint32_t id, uint32_t msg_name, uint8_t *data, size_t size);
int hdcp_dplink_get_stream_info(uint32_t id, uint16_t *num, uint8_t *strm_id);

void exynos_drm_dp_register_func_to_hdcp2(u32 id, struct dp_regs *regs,
	void (*hdcp_enable)(struct dp_regs *regs, u32 en, u32 type),
	u8 (*hdcp_mst_strm_id_get)(struct dp_regs *regs),
	int (*dpcd_read)(struct dp_regs *regs, unsigned int offset, void *buffer, size_t size),
	int (*dpcd_write)(struct dp_regs *regs, unsigned int offset, void *buffer, size_t size));
#endif
