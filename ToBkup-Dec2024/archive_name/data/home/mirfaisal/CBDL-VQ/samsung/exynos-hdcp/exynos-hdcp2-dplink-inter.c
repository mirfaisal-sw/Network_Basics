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
#include "exynos-hdcp2-dplink-inter.h"
#include "exynos-hdcp2-dplink-if.h"
#include "exynos-hdcp2-dplink-auth.h"

#define DRM_WAIT_RETRY_COUNT	1000
/* current link data */
enum auth_state auth_proc_state[MAX_NUM_OF_HDCP];
EXPORT_SYMBOL_GPL(auth_proc_state);
enum dp_state dp_hdcp_state[MAX_NUM_OF_HDCP];
EXPORT_SYMBOL_GPL(dp_hdcp_state);

int hdcp_dplink_auth_check(uint32_t node, enum auth_signal hdcp_signal)
{
	int ret = 0;

#if defined(CONFIG_HDCP2_FUNC_TEST_MODE)
	ret = exynos_smc(SMC_DRM_HDCP_FUNC_TEST, 1, 0, 0);
#endif
	switch (hdcp_signal) {
	case HDCP_DRM_OFF:
		return ret;
	case HDCP_DRM_ON:
		hdcp_info("hdcp auth_proc_state[%d]:%d\n", node, auth_proc_state[node]);
		ret = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, DP_HDCP22_DISABLE, node, 0);
		dplink_clear_irqflag_all(node);
		ret = hdcp_dplink_authenticate(node);
		hdcp_info("hdcp_dplink_authenticate node(%d) DONE ret : 0x%x\n", node, ret);
		return ret;
	case HDCP_RP_READY:
		if (auth_proc_state[node] == HDCP_AUTH_PROCESS_DONE)
			ret = hdcp_dplink_authenticate(node);
		return ret;
	default:
		ret = HDCP_ERROR_INVALID_STATE;
		break;
	}
	return ret;
}
EXPORT_SYMBOL_GPL(hdcp_dplink_auth_check);

int hdcp_dplink_get_auth_state(uint32_t node)
{
	hdcp_info("hdcp auth_proc_state[%d]:%d\n", node, auth_proc_state[node]);
	return auth_proc_state[node];
}
EXPORT_SYMBOL_GPL(hdcp_dplink_get_auth_state);

int hdcp_dplink_get_rxstatus(uint32_t node, uint8_t *status)
{
	int ret = 0;

	ret = hdcp_dplink_get_rxinfo(node, status);
	return ret;
}
EXPORT_SYMBOL_GPL(hdcp_dplink_get_rxstatus);

int hdcp_dplink_set_paring_available(uint32_t node)
{
	hdcp_info("pairing available node(%d)\n", node);
	return dplink_set_paring_available(node);
}
EXPORT_SYMBOL_GPL(hdcp_dplink_set_paring_available);

int hdcp_dplink_set_hprime_available(uint32_t node)
{
	hdcp_info("h-prime available node(%d)\n", node);
	return dplink_set_hprime_available(node);
}
EXPORT_SYMBOL_GPL(hdcp_dplink_set_hprime_available);

int hdcp_dplink_set_rp_ready(uint32_t node)
{
	hdcp_info("ready available node(%d)\n", node);
	return dplink_set_rp_ready(node);
}
EXPORT_SYMBOL_GPL(hdcp_dplink_set_rp_ready);

int hdcp_dplink_set_reauth(uint32_t node)
{
	uint64_t ret = 0;

	hdcp_info("reauth requested node(%d).\n", node);
	ret = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, DP_HDCP22_DISABLE, node, 0);
	return dplink_set_reauth_req(node);
}
EXPORT_SYMBOL_GPL(hdcp_dplink_set_reauth);

int hdcp_dplink_set_integrity_fail(uint32_t node)
{
	uint64_t ret = 0;

	hdcp_info("integrity check fail node(%d).\n", node);
	ret = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, DP_HDCP22_DISABLE, node, 0);
	return dplink_set_integrity_fail(node);
}
EXPORT_SYMBOL_GPL(hdcp_dplink_set_integrity_fail);

int hdcp_dplink_cancel_auth(uint32_t node)
{
	uint64_t ret = 0;

	hdcp_info("Cancel authenticate node(%d).\n", node);
	ret = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, DP_HPD_STATUS_ZERO, node, 0);
	auth_proc_state[node] = HDCP_AUTH_PROCESS_STOP;

	return dplink_set_integrity_fail(node);
}
EXPORT_SYMBOL_GPL(hdcp_dplink_cancel_auth);

void hdcp_dplink_clear_all(uint32_t node)
{
	uint64_t ret = 0;

	hdcp_info("HDCP flag clear node(%d)\n", node);
	ret = exynos_smc(SMC_DRM_HDCP_AUTH_INFO, DP_HDCP22_DISABLE, node, 0);
	dplink_clear_irqflag_all(node);
}
EXPORT_SYMBOL_GPL(hdcp_dplink_clear_all);

void hdcp_dplink_connect_state(uint32_t node, enum dp_state state)
{
//	uint64_t ret = 0;

	dp_hdcp_state[node] = state;
	hdcp_info("Displayport connect info node(%d) status:%d\n", node, dp_hdcp_state[node]);

	/* update needed with drm/ldfw*/
//	ret = exynos_smc(SMC_DRM_DP_CONNECT_INFO, dp_hdcp_state[node], 0, 0);
}
EXPORT_SYMBOL_GPL(hdcp_dplink_connect_state);
MODULE_LICENSE("GPL");
