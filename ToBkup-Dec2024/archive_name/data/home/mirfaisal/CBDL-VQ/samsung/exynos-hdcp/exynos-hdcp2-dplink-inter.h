// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __EXYNOS_HDCP2_DPLINK_INTER_H__
#define __EXYNOS_HDCP2_DPLINK_INTER_H__

/* Do hdcp2.2 authentication with DP Receiver
 * and enable encryption if authentication is succeed.
 * @return
 *  - 0: Success
 *  - ENOMEM: hdcp context open fail
 *  - EACCES: authentication fail
 */
int hdcp_dplink_get_rxstatus(u32 id, uint8_t *status);
int hdcp_dplink_set_paring_available(u32 id);
int hdcp_dplink_set_hprime_available(u32 id);
int hdcp_dplink_set_rp_ready(u32 id);
int hdcp_dplink_set_reauth(u32 id);
int hdcp_dplink_set_integrity_fail(u32 id);
int hdcp_dplink_cancel_auth(u32 id);
int hdcp_dplink_auth_check(u32 id, enum auth_signal);
void hdcp_dplink_clear_all(u32 id);
void hdcp_dplink_connect_state(u32 id, enum dp_state state);
#endif
