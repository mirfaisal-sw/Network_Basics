// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - IPs Traffic Monitor(ITMON) support
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>
#include <linux/of_irq.h>
#include <linux/delay.h>
#include <linux/pm_domain.h>
#include <linux/panic_notifier.h>
#include <soc/samsung/exynos-pmu.h>
#include <soc/samsung/exynos-itmon.h>
#include <soc/samsung/debug-snapshot.h>
#include <soc/samsung/exynos-pd.h>
#include "exynos-itmon-platform.h"
#include "exynos-itmon-local-v1.h"

const static char *itmon_v1_pathtype[] = {
	"DATA Path transaction (0x2000_0000 ~ 0xf_ffff_ffff)",
	"PERI(SFR) Path transaction (0x0 ~ 0x1fff_ffff)",
};

/* Error Code Description */
const static char *itmon_v1_errcode[] = {
	"Error Detect by the Slave(SLVERR)",
	"Decode error(DECERR)",
	"Unsupported transaction error",
	"Power Down access error",
	"Unsupported transaction",
	"Unsupported transaction",
	"Timeout error - response timeout in timeout value",
	"Invalid errorcode",
};

const static char *itmon_v1_nodestring[] = {
	"M_NODE",
	"TAXI_S_NODE",
	"TAXI_M_NODE",
	"S_NODE",
};

static struct itmon_v1_dev *g_itmon_v1;

static struct itmon_v1_rpathinfo *itmon_v1_get_rpathinfo(struct itmon_v1_dev *itmon,
						   unsigned int id,
						   char *dest_name)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_rpathinfo *rpath = NULL;
	int i;

	if (!dest_name)
		return NULL;

	for (i = 0; i < pdata->num_rpathinfo; i++) {
		if (pdata->rpathinfo[i].id == (id & pdata->rpathinfo[i].bits)) {
			if (dest_name && !strncmp(pdata->rpathinfo[i].dest_name,
						  dest_name,
						  strlen(pdata->rpathinfo[i].dest_name))) {
				rpath = (struct itmon_v1_rpathinfo *)&pdata->rpathinfo[i];
				break;
			}
		}
	}
	return rpath;
}

static struct itmon_v1_masterinfo *itmon_v1_get_masterinfo(struct itmon_v1_dev *itmon,
						 char *port_name,
						 unsigned int user)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_masterinfo *master = NULL;
	unsigned int val;
	int i;

	if (!port_name)
		return NULL;

	for (i = 0; i < pdata->num_masterinfo; i++) {
		if (!strncmp(pdata->masterinfo[i].port_name, port_name, strlen(port_name))) {
			val = user & pdata->masterinfo[i].bits;
			if (val == pdata->masterinfo[i].user) {
				master = (struct itmon_v1_masterinfo *)&pdata->masterinfo[i];
				break;
			}
		}
	}
	return master;
}

static void itmon_v1_init(struct itmon_v1_dev *itmon, bool enabled)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_nodeinfo *node;
	unsigned int offset;
	int i, j;

	for (i = 0; i < pdata->num_nodegroup; i++) {
		node = pdata->nodegroup[i].nodeinfo;
		for (j = 0; j < pdata->nodegroup[i].nodesize; j++) {
			if (node[j].type == V1_S_NODE && node[j].tmout_enabled) {
				offset = V1_OFFSET_TMOUT_REG;
				/* Enable Timeout setting */
				__raw_writel(enabled, node[j].regs + offset + V1_REG_DBG_CTL);
				/* set tmout interval value */
				__raw_writel(node[j].time_val,
						 node[j].regs + offset + V1_REG_TMOUT_INIT_VAL);
				pr_debug("Exynos ITMON - %s timeout enabled\n", node[j].name);
				if (node[j].tmout_frz_enabled) {
					/* Enable freezing */
					__raw_writel(enabled,
							 node[j].regs + offset + V1_REG_TMOUT_FRZ_EN);
				}
			}
			if (node[j].err_enabled) {
				/* clear previous interrupt of req_read */
				offset = V1_OFFSET_REQ_R;
				if (!pdata->probed || !node->retention)
					__raw_writel(1, node[j].regs + offset + V1_REG_INT_CLR);
				/* enable interrupt */
				__raw_writel(enabled, node[j].regs + offset + V1_REG_INT_MASK);

				/* clear previous interrupt of req_write */
				offset = V1_OFFSET_REQ_W;
				if (pdata->probed || !node->retention)
					__raw_writel(1, node[j].regs + offset + V1_REG_INT_CLR);
				/* enable interrupt */
				__raw_writel(enabled, node[j].regs + offset + V1_REG_INT_MASK);

				/* clear previous interrupt of response_read */
				offset = V1_OFFSET_RESP_R;
				if (!pdata->probed || !node->retention)
					__raw_writel(1, node[j].regs + offset + V1_REG_INT_CLR);
				/* enable interrupt */
				__raw_writel(enabled, node[j].regs + offset + V1_REG_INT_MASK);

				/* clear previous interrupt of response_write */
				offset = V1_OFFSET_RESP_W;
				if (!pdata->probed || !node->retention)
					__raw_writel(1, node[j].regs + offset + V1_REG_INT_CLR);
				/* enable interrupt */
				__raw_writel(enabled, node[j].regs + offset + V1_REG_INT_MASK);
				pr_debug("Exynos ITMON - %s error reporting enabled\n",
						node[j].name);
			}
			if (node[j].hw_assert_enabled) {
				offset = V1_OFFSET_HW_ASSERT;
				__raw_writel(V1_RD_RESP_INT_ENABLE | V1_WR_RESP_INT_ENABLE |
						 V1_ARLEN_RLAST_INT_ENABLE | V1_AWLEN_WLAST_INT_ENABLE,
						node[j].regs + offset + V1_REG_HWA_CTL);
			}
			if (node[j].addr_detect_enabled) {
				/* This feature is only for M_NODE */
				unsigned int tmp, val;

				offset = V1_OFFSET_HW_ASSERT;
				val = __raw_readl(node[j].regs + offset + V1_REG_HWA_CTL);
				val |= V1_INTEND_ACCESS_INT_ENABLE;
				__raw_writel(val, node[j].regs + offset + V1_REG_HWA_CTL);

				val = ((unsigned int)V1_INTEND_ADDR_START & 0xFFFFFFFF);
				__raw_writel(val, node[j].regs + offset +
						V1_REG_HWA_START_ADDR_LOW);
				val = (unsigned int)(((unsigned long)V1_INTEND_ADDR_START >> 32)
						& 0XFFFF);
				__raw_writel(val, node[j].regs + offset
						+ V1_REG_HWA_START_END_ADDR_UPPER);

				val = ((unsigned int)V1_INTEND_ADDR_END & 0xFFFFFFFF);
				__raw_writel(val, node[j].regs + offset + V1_REG_HWA_END_ADDR_LOW);
				val = ((unsigned int)(((unsigned long)V1_INTEND_ADDR_END >> 32)
							& 0XFFFF0000) << 16);
				tmp = readl(node[j].regs + offset + V1_REG_HWA_START_END_ADDR_UPPER);
				__raw_writel(tmp | val, node[j].regs + offset
						+ V1_REG_HWA_START_END_ADDR_UPPER);
			}
		}
	}
}

static void itmon_v1_enable(bool enabled)
{
	if (g_itmon_v1)
		itmon_v1_init(g_itmon_v1, enabled);
}

static void itmon_v1_pd_sync(const char *pd_name, bool en)
{
}

void itmon_v1_set_errcnt(int cnt)
{
	struct itmon_v1_platdata *pdata;

	if (g_itmon_v1) {
		pdata = g_itmon_v1->pdata;
		pdata->err_cnt = cnt;
	}
}

static void itmon_v1_post_handler_to_notifier(struct itmon_v1_dev *itmon,
					   unsigned int trans_type)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_traceinfo *traceinfo = &pdata->traceinfo[trans_type];

	/* After treatment by port */
	if (!traceinfo->port || strlen(traceinfo->port) < 1)
		return;

	itmon->notifier_info.port = traceinfo->port;
	itmon->notifier_info.master = traceinfo->master;
	itmon->notifier_info.dest = traceinfo->dest;
	itmon->notifier_info.read = traceinfo->read;
	itmon->notifier_info.target_addr = traceinfo->target_addr;
	itmon->notifier_info.errcode = traceinfo->errcode;
	itmon->notifier_info.onoff = traceinfo->onoff;
	itmon->notifier_info.pd_name = traceinfo->pd_name;

	pr_info("     +ITMON Notifier Call Information\n\n");

	/* call notifier_call_chain of itmon */
	atomic_notifier_call_chain(&itmon_notifier_list, 0, &itmon->notifier_info);

	pr_info("\n     -ITMON Notifier Call Information\n"
		"--------------------------------------------------------------------------\n");
}

static void itmon_v1_post_handler_by_dest(struct itmon_v1_dev *itmon,
					unsigned int trans_type)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_traceinfo *traceinfo = &pdata->traceinfo[trans_type];

	if (!traceinfo->dest || strlen(traceinfo->dest) < 1)
		return;

	if (traceinfo->errcode == V1_ERRCODE_TMOUT && traceinfo->snode_dirty == true) {
		if (!strncmp(traceinfo->dest, V1_DREX_COMMON_STR, strlen(V1_DREX_COMMON_STR)) ||
				!strncmp(traceinfo->dest, "CORE_DP", strlen("CORE_DP"))) {
			pdata->err_fatal = true;
		}
	}
}

static void itmon_v1_post_handler_by_master(struct itmon_v1_dev *itmon,
					unsigned int trans_type)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_traceinfo *traceinfo = &pdata->traceinfo[trans_type];

	/* After treatment by port */
	if (!traceinfo->port || strlen(traceinfo->port) < 1)
		return;

	/*
	 * if master is CPU or SCI_CCM or SCI_IRPM,
	 * then we expect any exception at CPU
	 */
	if ((!strncmp(traceinfo->port, "CPU", strlen("CPU"))) ||
		(!strncmp(traceinfo->port, "SCI_CCM", strlen("SCI_CCM"))) ||
		(!strncmp(traceinfo->port, "SCI_IRPM", strlen("SCI_IRPM")))) {
		ktime_t now, interval;

		now = ktime_get();
		interval = ktime_sub(now, pdata->last_time);
		pdata->last_time = now;
		pdata->err_cnt_by_cpu++;
		if (traceinfo->errcode == V1_ERRCODE_TMOUT &&
			traceinfo->snode_dirty == true) {
			pdata->err_cnt = V1_PANIC_ALLOWED_THRESHOLD + 1;
			pr_info("ITMON try to run PANIC, even CPU transaction detected - %s",
			itmon_v1_errcode[traceinfo->errcode]);
		} else {
			pr_info("ITMON skips CPU transaction detected -");
			pr_info("err_cnt_by_cpu: %u, interval: %lluns\n",
				pdata->err_cnt_by_cpu,
				(unsigned long long)ktime_to_ns(interval));
		}
	} else if (!strncmp(traceinfo->port, V1_CP_COMMON_STR, strlen(V1_CP_COMMON_STR))) {
		/* if master is DSP and operation is read, we don't care this */
		if (traceinfo->master && traceinfo->target_addr == V1_INVALID_REMAPPING &&
			!strncmp(traceinfo->master, "LCPUM", strlen(traceinfo->master))) {
			pr_info("ITMON skips CP's LCPUM detected\n");
		} else {
			pr_info("CP Crash sequence in progress\n");
			/* Disable busmon all interrupts */
			itmon_v1_init(itmon, false);
			pdata->crash_in_progress = true;
#if defined(CONFIG_SEC_SIPC_MODEM_IF)
			modem_force_crash_exit_ext();
#endif
		}
		pdata->err_fatal = false;
	} else {
		pdata->err_cnt++;
		pr_info("ITMON increases error count - err_cnt:%u\n", pdata->err_cnt);
	}
}

static void itmon_v1_report_timeout(struct itmon_v1_dev *itmon,
				struct itmon_v1_nodeinfo *node,
				unsigned int trans_type)
{
	unsigned int id, payload, axid, user, valid, timeout, info;
	unsigned long addr;
	char *master_name, *port_name;
	struct itmon_v1_rpathinfo *port;
	struct itmon_v1_masterinfo *master;
	int i, num = (trans_type == V1_TRANS_TYPE_READ ? SZ_128 : SZ_64);
	int rw_offset = (trans_type == V1_TRANS_TYPE_READ ? 0 : V1_REG_TMOUT_BUF_WR_OFFSET);

	pr_info("\n----------------------------------------------------------------------------------\n"
		"      ITMON Report (%s)\n"
		"----------------------------------------------------------------------------------\n"
		"      Timeout Error Occurred : Master --> %s (DRAM)\n\n",
		trans_type == V1_TRANS_TYPE_READ ? "READ" : "WRITE", node->name);
	pr_info("      TIMEOUT_BUFFER Information(NODE: %s)\n"
		"	> NUM|   BLOCK|  MASTER|VALID|TIMEOUT|      ID| PAYLOAD|   ADDRESS|   SRAM3|\n",
			node->name);

	for (i = 0; i < num; i++) {
		writel(i, node->regs + V1_OFFSET_TMOUT_REG +
				V1_REG_TMOUT_BUF_POINT_ADDR + rw_offset);
		id = readl(node->regs + V1_OFFSET_TMOUT_REG +
				V1_REG_TMOUT_BUF_ID + rw_offset);
		payload = readl(node->regs + V1_OFFSET_TMOUT_REG +
				V1_REG_TMOUT_BUF_PAYLOAD + rw_offset);
		addr = (((unsigned long)readl(node->regs + V1_OFFSET_TMOUT_REG +
				V1_REG_TMOUT_BUF_PAYLOAD_SRAM1 + rw_offset) &
				GENMASK(15, 0)) << 32ULL);
		addr |= (readl(node->regs + V1_OFFSET_TMOUT_REG +
				V1_REG_TMOUT_BUF_PAYLOAD_SRAM2 + rw_offset));
		info = readl(node->regs + V1_OFFSET_TMOUT_REG +
				V1_REG_TMOUT_BUF_PAYLOAD_SRAM3 + rw_offset);
		/* ID[5:0] 6bit : R-PATH */
		axid = id & GENMASK(5, 0);
		/* PAYLOAD[15:8] : USER */
		user = (payload & GENMASK(15, 8)) >> 8;
		/* PAYLOAD[0] : Valid or Not valid */
		valid = payload & BIT(0);
		/* PAYLOAD[19:16] : Timeout */
		timeout = (payload & (unsigned int)(GENMASK(19, 16))) >> 16;

		port = (struct itmon_v1_rpathinfo *)
				itmon_v1_get_rpathinfo(itmon, axid, node->name);
		if (port) {
			port_name = port->port_name;
			master = (struct itmon_v1_masterinfo *)
				itmon_v1_get_masterinfo(itmon, port_name, user);
			if (master)
				master_name = master->master_name;
			else
				master_name = "Unknown";
		} else {
			port_name = "Unknown";
			master_name = "Unknown";
		}
		pr_info("      > %03d|%8s|%8s|%5u|%7x|%08x|%08X|%010zx|%08x|\n",
				i, port_name, master_name, valid, timeout, id, payload, addr, info);
	}
	pr_info("----------------------------------------------------------------------------------\n");
}

static unsigned int power(unsigned int param, unsigned int num)
{
	if (num == 0)
		return 1;
	return param * (power(param, num - 1));
}

static void itmon_v1_report_traceinfo(struct itmon_v1_dev *itmon,
				struct itmon_v1_nodeinfo *node,
				unsigned int trans_type)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_traceinfo *traceinfo = &pdata->traceinfo[trans_type];
	struct itmon_v1_nodegroup *group = NULL;

	if (!traceinfo->dirty)
		return;

	pr_info("--------------------------------------------------------------------------\n"
		"      Transaction Information\n\n"
		"      > Master         : %s %s\n"
		"      > Target         : %s\n"
		"      > Target Address : 0x%lX %s\n"
		"      > Type           : %s\n"
		"      > Error code     : %s\n",
		traceinfo->port, traceinfo->master ? traceinfo->master : "",
		traceinfo->dest ? traceinfo->dest : "Unknown",
		traceinfo->target_addr,
		(unsigned int)traceinfo->target_addr == V1_INVALID_REMAPPING ?
		"(BAAW Remapped address)" : "",
		trans_type == V1_TRANS_TYPE_READ ? "READ" : "WRITE",
		itmon_v1_errcode[traceinfo->errcode]);

	if (node) {
		struct itmon_v1_tracedata *tracedata = &node->tracedata;

		pr_info("      > Size           : %u bytes x %u burst => %u bytes\n"
			"      > Burst Type     : %u (0:FIXED, 1:INCR, 2:WRAP)\n"
			"      > Level          : %s\n"
			"      > Protection     : %s\n",
			power(2, V1_BIT_AXSIZE(tracedata->ext_info_1)),
			V1_BIT_AXLEN(tracedata->ext_info_1) + 1,
			power(2, V1_BIT_AXSIZE(tracedata->ext_info_1)) *
				(V1_BIT_AXLEN(tracedata->ext_info_1) + 1),
			V1_BIT_AXBURST(tracedata->ext_info_2),
			(V1_BIT_AXPROT(tracedata->ext_info_2) & 0x1) ?
				"Privileged access" : "Unprivileged access",
			(V1_BIT_AXPROT(tracedata->ext_info_2) & 0x2) ?
				"Non-secure access" : "Secure access");

		group = node->group;
		pr_info("      > Path Type      : %s\n"
			"--------------------------------------------------------------------------\n",
			itmon_v1_pathtype[traceinfo->path_type == -1 ?
				group->bus_type : traceinfo->path_type]);

	} else {
		pr_info("--------------------------------------------------------------------------\n");
	}
}

static void itmon_v1_report_pathinfo(struct itmon_v1_dev *itmon,
				  struct itmon_v1_nodeinfo *node,
				  unsigned int trans_type)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_tracedata *tracedata = &node->tracedata;
	struct itmon_v1_traceinfo *traceinfo = &pdata->traceinfo[trans_type];

	if (!traceinfo->path_dirty) {
		pr_info("--------------------------------------------------------------------------\n"
			"      ITMON Report (%s)\n"
			"--------------------------------------------------------------------------\n"
			"      PATH Information\n",
			trans_type == V1_TRANS_TYPE_READ ? "READ" : "WRITE");
		traceinfo->path_dirty = true;
	}
	switch (node->type) {
	case V1_M_NODE:
		pr_info("      > %14s, %8s(0x%08X)\n",
			node->name, "M_NODE", node->phy_regs + tracedata->offset);
		break;
	case V1_T_S_NODE:
		pr_info("      > %14s, %8s(0x%08X)\n",
			node->name, "T_S_NODE", node->phy_regs + tracedata->offset);
		break;
	case V1_T_M_NODE:
		pr_info("      > %14s, %8s(0x%08X)\n",
			node->name, "T_M_NODE", node->phy_regs + tracedata->offset);
		break;
	case V1_S_NODE:
		pr_info("      > %14s, %8s(0x%08X)\n",
			node->name, "S_NODE", node->phy_regs + tracedata->offset);
		break;
	}
}

static void itmon_v1_report_tracedata_M_NODE(struct itmon_v1_dev *itmon,
				   struct itmon_v1_nodeinfo *node,
				   unsigned int trans_type)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_tracedata *tracedata = &node->tracedata;
	struct itmon_v1_traceinfo *traceinfo = &pdata->traceinfo[trans_type];
	struct itmon_v1_masterinfo *master;
	unsigned int errcode, axid;
	unsigned int userbit;

	errcode = V1_BIT_ERR_CODE(tracedata->int_info);
	axid = (unsigned int)V1_BIT_AXID(tracedata->int_info);
	userbit = V1_BIT_AXUSER(tracedata->ext_info_2);

	/*
	 * In this case, we can get information from M_NODE
	 * Fill traceinfo->port / target_addr / read / master
	 */
	if (V1_BIT_ERR_VALID(tracedata->int_info) && tracedata->ext_info_2) {
		/* If only detecting M_NODE only */
		traceinfo->port = node->name;
		master = (struct itmon_v1_masterinfo *)
			itmon_v1_get_masterinfo(itmon, node->name, userbit);
		if (master)
			traceinfo->master = master->master_name;
		else
			traceinfo->master = NULL;

		if (!strncmp(node->name, V1_CP_COMMON_STR, strlen(V1_CP_COMMON_STR)))
			set_bit(V1_FROM_CP, &traceinfo->from);

		traceinfo->target_addr = (((unsigned long)node->tracedata.ext_info_1
							& GENMASK(3, 0)) << 32ULL);
		traceinfo->target_addr |= node->tracedata.ext_info_0;
		traceinfo->read = tracedata->read;
		traceinfo->errcode = errcode;
		traceinfo->dirty = true;
	} else {
		if (!strncmp(node->name, "SCI_IRPM", strlen(node->name)))
			set_bit(V1_FROM_CPU, &traceinfo->from);
	}
	/* Pure M_NODE and it doesn't have any information */
	if (!traceinfo->dirty) {
		traceinfo->master = NULL;
		traceinfo->target_addr = 0;
		traceinfo->read = tracedata->read;
		traceinfo->port = node->name;
		traceinfo->errcode = errcode;
		traceinfo->dirty = true;
	}
	itmon_v1_report_pathinfo(itmon, node, trans_type);
}

static void itmon_v1_report_tracedata_S_NODE(struct itmon_v1_dev *itmon,
				   struct itmon_v1_nodeinfo *node,
				   unsigned int trans_type)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_tracedata *tracedata = &node->tracedata;
	struct itmon_v1_traceinfo *traceinfo = &pdata->traceinfo[trans_type];
	struct itmon_v1_masterinfo *master;
	struct itmon_v1_rpathinfo *port;
	unsigned int errcode, axid;
	unsigned int userbit;
	bool port_find;
	int ret = 0;

	errcode = V1_BIT_ERR_CODE(tracedata->int_info);
	axid = (unsigned int)V1_BIT_AXID(tracedata->int_info);
	userbit = V1_BIT_AXUSER(tracedata->ext_info_2);

	if (test_bit(V1_FROM_CPU, &traceinfo->from)) {
		/*
		 * This case is slave error
		 * Master is CPU cluster
		 * user & GENMASK(1, 0) = core number
		 */
		int cluster_num, core_num;

		core_num = userbit & GENMASK(1, 0);
		cluster_num = (userbit & BIT(2)) >> 2;
		ret = snprintf(traceinfo->buf, SZ_32 - 1, "CPU%d Cluster%d", core_num, cluster_num);
		if (ret < 0) {
			pr_info("snprintf failed for traceinfo->buf\n");
			return;
		}
		traceinfo->port = traceinfo->buf;
	} else if (test_bit(V1_FROM_CP, &traceinfo->from) && (!traceinfo->port))
		traceinfo->port = V1_CP_COMMON_STR;

	/* S_NODE = BUSC_DP or CORE_DP => PERI */
	if (!strncmp(node->name, "BUSC_DP", strlen(node->name)) ||
		!strncmp(node->name, "CORE_DP", strlen(node->name))) {
		port_find = true;
		traceinfo->path_type = V1_BUS_PERI;
	} else if (!strncmp(node->name, V1_DREX_COMMON_STR, strlen(V1_DREX_COMMON_STR))) {
		port_find = true;
		traceinfo->path_type = V1_BUS_DATA;
	} else {
		port_find = false;
		traceinfo->path_type = -1;
	}
	if (port_find && !test_bit(V1_FROM_CPU, &traceinfo->from)) {
		port = (struct itmon_v1_rpathinfo *)
			itmon_v1_get_rpathinfo(itmon, axid, node->name);
		/* If it couldn't find port, keep previous information */
		if (port) {
			traceinfo->port = port->port_name;
			master = (struct itmon_v1_masterinfo *)
					itmon_v1_get_masterinfo(itmon, traceinfo->port,
						userbit);
			if (master)
				traceinfo->master = master->master_name;
		}
	} else {
		/* If it has traceinfo->port, keep previous information */
		if (!traceinfo->port) {
			port = (struct itmon_v1_rpathinfo *)
					itmon_v1_get_rpathinfo(itmon, axid, node->name);
			if (port) {
				if (!strncmp(port->port_name, V1_CP_COMMON_STR,
						strlen(V1_CP_COMMON_STR)))
					traceinfo->port = V1_CP_COMMON_STR;
				else
					traceinfo->port = port->port_name;
			}
		}
		if (!traceinfo->master && traceinfo->port) {
			master = (struct itmon_v1_masterinfo *)
					itmon_v1_get_masterinfo(itmon, traceinfo->port,
						userbit);
			if (master)
				traceinfo->master = master->master_name;
		}
	}
	/* Update targetinfo with S_NODE */
	traceinfo->target_addr =
		(((unsigned long)node->tracedata.ext_info_1
		& GENMASK(3, 0)) << 32ULL);
	traceinfo->target_addr |= node->tracedata.ext_info_0;
	traceinfo->errcode = errcode;
	traceinfo->dest = node->name;
	traceinfo->dirty = true;
	traceinfo->snode_dirty = true;
	itmon_v1_report_pathinfo(itmon, node, trans_type);
	itmon_v1_report_traceinfo(itmon, node, trans_type);
}

static void itmon_v1_report_tracedata(struct itmon_v1_dev *itmon,
				   struct itmon_v1_nodeinfo *node,
				   unsigned int trans_type)
{
	struct itmon_v1_tracedata *tracedata = &node->tracedata;
	unsigned int errcode, axid;
	unsigned int userbit;

	errcode = V1_BIT_ERR_CODE(tracedata->int_info);
	axid = (unsigned int)V1_BIT_AXID(tracedata->int_info);
	userbit = V1_BIT_AXUSER(tracedata->ext_info_2);

	switch (node->type) {
	case V1_M_NODE:
		itmon_v1_report_tracedata_M_NODE(itmon, node, trans_type);
		break;
	case V1_S_NODE:
		itmon_v1_report_tracedata_S_NODE(itmon, node, trans_type);
		break;
	case V1_T_S_NODE:
	case V1_T_M_NODE:
		itmon_v1_report_pathinfo(itmon, node, trans_type);
		break;
	default:
		pr_info("Unknown Error - offset:%u\n", tracedata->offset);
		break;
	}
}

static void itmon_v1_report_hwa_rawdata(struct itmon_v1_dev *itmon,
					 struct itmon_v1_nodeinfo *node)
{
	unsigned int dbg_mo_cnt, hwa_ctl, hwa_info, hwa_int_id;

	dbg_mo_cnt = __raw_readl(node->regs +  V1_OFFSET_HW_ASSERT);
	hwa_ctl = __raw_readl(node->regs +  V1_OFFSET_HW_ASSERT + V1_REG_HWA_CTL);
	hwa_info = __raw_readl(node->regs +  V1_OFFSET_HW_ASSERT + V1_REG_HWA_INT);
	hwa_int_id = __raw_readl(node->regs + V1_OFFSET_HW_ASSERT + V1_REG_HWA_INT_ID);

	/* Output Raw register information */
	pr_info("--------------------------------------------------------------------------\n"
		"      HWA Raw Register Information(ITMON information)\n\n");
	pr_info("      > %s(%s, 0x%08X)\n"
		"      > REG(0x100~0x10C)      : 0x%08X, 0x%08X, 0x%08X, 0x%08X\n",
		node->name, itmon_v1_nodestring[node->type],
		node->phy_regs,
		dbg_mo_cnt,
		hwa_ctl,
		hwa_info,
		hwa_int_id);
}

static void itmon_v1_report_rawdata(struct itmon_v1_dev *itmon,
				 struct itmon_v1_nodeinfo *node,
				 unsigned int trans_type)
{
	struct itmon_v1_tracedata *tracedata = &node->tracedata;

	/* Output Raw register information */
	pr_info("      > %s(%s, 0x%08X)\n"
		"      > REG(0x08~0x18)        : 0x%08X, 0x%08X, 0x%08X, 0x%08X\n"
		"      > REG(0x100~0x10C)      : 0x%08X, 0x%08X, 0x%08X, 0x%08X\n",
		node->name, itmon_v1_nodestring[node->type],
		node->phy_regs + tracedata->offset,
		tracedata->int_info,
		tracedata->ext_info_0,
		tracedata->ext_info_1,
		tracedata->ext_info_2,
		tracedata->dbg_mo_cnt,
		tracedata->hwa_ctl,
		tracedata->hwa_info,
		tracedata->hwa_int_id);
}

static void itmon_v1_route_tracedata(struct itmon_v1_dev *itmon)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_traceinfo *traceinfo;
	struct itmon_v1_nodeinfo *node, *next_node;
	unsigned int trans_type;
	int i;

	/* To call function is sorted by declaration */
	for (trans_type = 0; trans_type < V1_TRANS_TYPE_NUM; trans_type++) {
		for (i = V1_M_NODE; i < NODE_TYPE; i++) {
			list_for_each_entry(node, &pdata->tracelist[trans_type], list) {
				if (i == node->type)
					itmon_v1_report_tracedata(itmon, node, trans_type);
			}
		}
		/* If there is no S_NODE information, check one more */
		traceinfo = &pdata->traceinfo[trans_type];
		if (!traceinfo->snode_dirty)
			itmon_v1_report_traceinfo(itmon, NULL, trans_type);
	}

	if (pdata->traceinfo[V1_TRANS_TYPE_READ].dirty ||
		pdata->traceinfo[V1_TRANS_TYPE_WRITE].dirty)
		pr_info("      Raw Register Information(ITMON Internal Information)\n\n");

	for (trans_type = 0; trans_type < V1_TRANS_TYPE_NUM; trans_type++) {
		for (i = V1_M_NODE; i < NODE_TYPE; i++) {
			list_for_each_entry_safe(node, next_node,
					&pdata->tracelist[trans_type], list) {
				if (i == node->type) {
					itmon_v1_report_rawdata(itmon, node, trans_type);
					/* clean up */
					list_del(&node->list);
					kfree(node);
				}
			}
		}
	}

	if (pdata->traceinfo[V1_TRANS_TYPE_READ].dirty ||
		pdata->traceinfo[V1_TRANS_TYPE_WRITE].dirty)
		pr_info("--------------------------------------------------------------------------\n");

	for (trans_type = 0; trans_type < V1_TRANS_TYPE_NUM; trans_type++) {
		itmon_v1_post_handler_to_notifier(itmon, trans_type);
		itmon_v1_post_handler_by_dest(itmon, trans_type);
		itmon_v1_post_handler_by_master(itmon, trans_type);
	}
}

static void itmon_v1_trace_data(struct itmon_v1_dev *itmon,
				struct itmon_v1_nodegroup *group,
				struct itmon_v1_nodeinfo *node,
				unsigned int offset)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_nodeinfo *new_node = NULL;
	unsigned int int_info, info0, info1, info2;
	unsigned int hwa_ctl, hwa_info, hwa_int_id, dbg_mo_cnt;
	bool read = V1_TRANS_TYPE_WRITE;
	bool req = false;

	int_info = __raw_readl(node->regs + offset + V1_REG_INT_INFO);
	info0 = __raw_readl(node->regs + offset + V1_REG_EXT_INFO_0);
	info1 = __raw_readl(node->regs + offset + V1_REG_EXT_INFO_1);
	info2 = __raw_readl(node->regs + offset + V1_REG_EXT_INFO_2);

	dbg_mo_cnt = __raw_readl(node->regs +  V1_OFFSET_HW_ASSERT);
	hwa_ctl = __raw_readl(node->regs +  V1_OFFSET_HW_ASSERT + V1_REG_HWA_CTL);
	hwa_info = __raw_readl(node->regs +  V1_OFFSET_HW_ASSERT + V1_REG_HWA_INT);
	hwa_int_id = __raw_readl(node->regs + V1_OFFSET_HW_ASSERT + V1_REG_HWA_INT_ID);

	switch (offset) {
	case V1_OFFSET_REQ_R:
		read = V1_TRANS_TYPE_READ;
		req = true;
		/* Only S-Node is able to make log to registers */
		break;
	case V1_OFFSET_REQ_W:
		read = V1_TRANS_TYPE_WRITE;
		req = true;
		/* Only S-Node is able to make log to registers */
		break;
	case V1_OFFSET_RESP_R:
		read = V1_TRANS_TYPE_READ;
		req = false;
		/* Only NOT S-Node is able to make log to registers */
		break;
	case V1_OFFSET_RESP_W:
		read = V1_TRANS_TYPE_WRITE;
		req = false;
		/* Only NOT S-Node is able to make log to registers */
		break;
	default:
		pr_info("Unknown Error - node:%s offset:%u\n", node->name, offset);
		break;
	}

	new_node = kmalloc(sizeof(struct itmon_v1_nodeinfo), GFP_ATOMIC);
	if (new_node) {
		/* Fill detected node information to tracedata's list */
		memcpy(new_node, node, sizeof(struct itmon_v1_nodeinfo));
		new_node->tracedata.int_info = int_info;
		new_node->tracedata.ext_info_0 = info0;
		new_node->tracedata.ext_info_1 = info1;
		new_node->tracedata.ext_info_2 = info2;
		new_node->tracedata.dbg_mo_cnt = dbg_mo_cnt;
		new_node->tracedata.hwa_ctl = hwa_ctl;
		new_node->tracedata.hwa_info = hwa_info;
		new_node->tracedata.hwa_int_id = hwa_int_id;

		new_node->tracedata.offset = offset;
		new_node->tracedata.read = read;
		new_node->group = group;
		if (V1_BIT_ERR_VALID(int_info))
			node->tracedata.logging = true;
		else
			node->tracedata.logging = false;

		list_add(&new_node->list, &pdata->tracelist[read]);
	} else {
		pr_info("failed to kmalloc for %s node %x offset\n",
			node->name, offset);
	}
}

void check_hw_assertion(struct itmon_v1_dev *itmon, struct itmon_v1_nodeinfo *node,
		unsigned int val, int *ret, unsigned long bit)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;

	if (node[bit].hw_assert_enabled) {
		val = __raw_readl(node[bit].regs + V1_OFFSET_HW_ASSERT +
					V1_REG_HWA_INT);
		/* if timeout_freeze is enable,
		 * HWA interrupt is able to assert without any information
		 */
		if (V1_BIT_HWA_ERR_OCCURRED(val) && (val & GENMASK(31, 1))) {
			itmon_v1_report_hwa_rawdata(itmon, &node[bit]);
			pdata->err_hwa = true;
			/* Go panic now */
			pdata->err_cnt = V1_PANIC_ALLOWED_THRESHOLD + 1;
			*ret = true;
		}
	}
}

void check_freeze_node(struct itmon_v1_dev *itmon, struct itmon_v1_nodeinfo *node,
		unsigned int val, int *ret, unsigned long bit)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	unsigned int freeze;

	if (node[bit].type == V1_S_NODE && node[bit].tmout_frz_enabled) {
		val = __raw_readl(node[bit].regs + V1_OFFSET_TMOUT_REG  +
					V1_REG_TMOUT_BUF_STATUS);
		freeze = val & (unsigned int)GENMASK(1, 0);
		if (freeze) {
			if (freeze & BIT(0))
				itmon_v1_report_timeout(itmon, &node[bit], V1_TRANS_TYPE_WRITE);
			if (freeze & BIT(1))
				itmon_v1_report_timeout(itmon, &node[bit], V1_TRANS_TYPE_READ);
			pdata->err_fatal = true;
			/* Go panic now */
			pdata->err_cnt = V1_PANIC_ALLOWED_THRESHOLD + 1;
			*ret = true;
		}
	}
}

static int itmon_v1_search_node_group(struct itmon_v1_dev *itmon,
	struct itmon_v1_nodegroup *group, bool clear)
{
	struct itmon_v1_nodeinfo *node = NULL;
	unsigned int val, offset;
	unsigned long vec, bit = 0;
	int i, ret = 0;

	/* Processing only this group and select detected node */
	if (group->phy_regs) {
		if (group->ex_table)
			vec = (unsigned long)__raw_readq(group->regs);
		else
			vec = (unsigned long)__raw_readl(group->regs);
	} else {
		vec = GENMASK(group->nodesize, 0);
	}

	if (!vec)
		goto exit;

	node = group->nodeinfo;
	for_each_set_bit(bit, &vec, group->nodesize) {
		/* exist array */
		for (i = 0; i < V1_OFFSET_NUM; i++) {
			offset = i * V1_OFFSET_ERR_REPT;
			/* Check Request information */
			val = __raw_readl(node[bit].regs + offset + V1_REG_INT_INFO);
			if (V1_BIT_ERR_OCCURRED(val)) {
				/* This node occurs the error */
				itmon_v1_trace_data(itmon, group, &node[bit], offset);
				if (clear)
					__raw_writel(1, node[bit].regs
							+ offset + V1_REG_INT_CLR);
				ret = true;
			}
		}
		/* Check H/W assertion */
		check_hw_assertion(itmon, node, val, &ret, bit);
		/* Check freeze enable node */
		check_freeze_node(itmon, node, val, &ret, bit);
	}
	itmon_v1_route_tracedata(itmon);
 exit:
	return ret;
}

static int itmon_v1_search_node_all(struct itmon_v1_dev *itmon, struct itmon_v1_nodegroup *group, bool clear)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_nodeinfo *node = NULL;
	unsigned int val, offset;
	unsigned long vec, bit = 0;
	int i, j, ret = 0;

	/* Processing all group & nodes */
	for (i = 0; i < pdata->num_nodegroup; i++) {
		group = &pdata->nodegroup[i];
		if (group->phy_regs) {
			if (group->ex_table)
				vec = (unsigned long)__raw_readq(group->regs);
			else
				vec = (unsigned long)__raw_readl(group->regs);
		} else {
			vec = GENMASK(group->nodesize, 0);
		}

		node = group->nodeinfo;
		bit = 0;

		for_each_set_bit(bit, &vec, group->nodesize) {
			for (j = 0; j < V1_OFFSET_NUM; j++) {
				offset = j * V1_OFFSET_ERR_REPT;
				/* Check Request information */
				val = __raw_readl(node[bit].regs + offset + V1_REG_INT_INFO);
				if (V1_BIT_ERR_OCCURRED(val)) {
					/* This node occurs the error */
					itmon_v1_trace_data(itmon, group, &node[bit], offset);
					if (clear)
						__raw_writel(1, node[bit].regs
								+ offset + V1_REG_INT_CLR);
					ret = true;
				}
			}
			/* Check H/W assertion */
			check_hw_assertion(itmon, node, val, &ret, bit);
			/* Check freeze enable node */
			check_freeze_node(itmon, node, val, &ret, bit);

		}
	}
	itmon_v1_route_tracedata(itmon);

	return ret;
}

static int itmon_v1_search_node(struct itmon_v1_dev *itmon, struct itmon_v1_nodegroup *group, bool clear)
{
	struct itmon_v1_platdata *pdata = itmon->pdata;
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&itmon->ctrl_lock, flags);
	memset(pdata->traceinfo, 0, sizeof(struct itmon_v1_traceinfo) * 2);
	ret = 0;

	if (group) {
		/* Processing only this group and select detected node */
		ret = itmon_v1_search_node_group(itmon, group, clear);
	} else {
		/* Processing all group & nodes */
		ret = itmon_v1_search_node_all(itmon, group, clear);
	}

	spin_unlock_irqrestore(&itmon->ctrl_lock, flags);
	return ret;
}

static irqreturn_t itmon_v1_irq_handler(int irq, void *data)
{
	struct itmon_v1_dev *itmon = (struct itmon_v1_dev *)data;
	struct itmon_v1_platdata *pdata = itmon->pdata;
	struct itmon_v1_nodegroup *group = NULL;
	bool ret;
	int i;

	/* Search itmon group */
	for (i = 0; i < pdata->num_nodegroup; i++) {
		if (irq == pdata->nodegroup[i].irq) {
			group = &pdata->nodegroup[i];
			if (group->phy_regs != 0) {
				pr_info("\nITMON Detected: %d irq, %s group, 0x%x vec, ", irq,
					group->name, __raw_readl(group->regs));
				pr_info("err_cnt:%u err_cnt_by_cpu:%u\n", pdata->err_cnt,
					pdata->err_cnt_by_cpu);
			} else {
				pr_info("\nITMON Detected: %d irq, %s group, ", irq, group->name);
				pr_info("err_cnt:%u err_cnt_by_cpu:%u\n", pdata->err_cnt,
					pdata->err_cnt_by_cpu);
			}
			break;
		}
	}

	ret = itmon_v1_search_node(itmon, NULL, true);
	if (!ret) {
		pr_info("ITMON could not detect any error\n");
	} else {
		if (pdata->err_cnt > V1_PANIC_ALLOWED_THRESHOLD)
			pdata->panic_allowed = true;
	}

	if (pdata->panic_allowed)
		panic("ITMON occurs panic, Transaction is invalid from IPs");

	return IRQ_HANDLED;
}

static int itmon_v1_logging_panic_handler(struct notifier_block *nb,
					 unsigned long l, void *buf)
{
	struct itmon_v1_panic_block *itmon_v1_panic = (struct itmon_v1_panic_block *)nb;
	struct itmon_v1_dev *itmon = itmon_v1_panic->pdev;
	int ret;

	if (!IS_ERR_OR_NULL(itmon)) {
		/* Check error has been logged */
		ret = itmon_v1_search_node(itmon, NULL, false);
		if (!ret)
			pr_info("No found error in %s\n", __func__);
		else
			pr_info("Found errors in %s\n", __func__);
	}
	return 0;
}

static int itmon_v1_probe(struct platform_device *pdev, void *param)
{
	struct itmon_v1_info_table *info = param;
	struct itmon_v1_dev *itmon;
	struct itmon_v1_panic_block *itmon_v1_panic = NULL;
	struct itmon_v1_platdata *pdata;
	struct itmon_v1_nodeinfo *node;
	unsigned int irq_option = 0, irq;
	char *dev_name;
	int ret, i, j;

	if (!info)
		return -EINVAL;

	itmon = devm_kzalloc(&pdev->dev, sizeof(struct itmon_v1_dev), GFP_KERNEL);
	if (!itmon)
		return -ENOMEM;

	itmon->dev = &pdev->dev;

	spin_lock_init(&itmon->ctrl_lock);

	pdata = devm_kzalloc(&pdev->dev, sizeof(struct itmon_v1_platdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	itmon->pdata = pdata;
	itmon->pdata->masterinfo = info->masterinfo;
	itmon->pdata->num_masterinfo = info->num_masterinfo;

	itmon->pdata->rpathinfo = info->rpathinfo;
	itmon->pdata->num_rpathinfo = info->num_rpathinfo;

	itmon->pdata->nodegroup = info->nodegroup;
	itmon->pdata->num_nodegroup = info->num_nodegroup;

	for (i = 0; i < pdata->num_nodegroup; i++) {
		dev_name = pdata->nodegroup[i].name;
		node = pdata->nodegroup[i].nodeinfo;

		if (pdata->nodegroup[i].phy_regs) {
			pdata->nodegroup[i].regs = devm_ioremap(&pdev->dev,
							 pdata->nodegroup[i].phy_regs, SZ_16K);
			if (pdata->nodegroup[i].regs == NULL) {
				dev_err(&pdev->dev, "failed to claim register region - %s\n",
					dev_name);
				return -ENOENT;
			}
		}
#ifdef MULTI_IRQ_SUPPORT_ITMON
		irq_option = IRQF_GIC_MULTI_TARGET;
#endif
		irq = irq_of_parse_and_map(pdev->dev.of_node, i);
		pdata->nodegroup[i].irq = irq;

		ret = devm_request_irq(&pdev->dev, irq,
					   itmon_v1_irq_handler, irq_option, dev_name, itmon);
		if (ret == 0) {
			dev_info(&pdev->dev, "success to register request irq%u - %s\n",
					irq, dev_name);
		} else {
			dev_err(&pdev->dev, "failed to request irq - %s\n", dev_name);
			return -ENOENT;
		}

		for (j = 0; j < pdata->nodegroup[i].nodesize; j++) {
			node[j].regs = devm_ioremap(&pdev->dev, node[j].phy_regs, SZ_16K);
			if (node[j].regs == NULL) {
				dev_err(&pdev->dev, "failed to claim register region - %s\n",
					dev_name);
				return -ENOENT;
			}
		}
	}

	itmon_v1_panic = devm_kzalloc(&pdev->dev, sizeof(struct itmon_v1_panic_block),
				 GFP_KERNEL);

	if (!itmon_v1_panic) {
		dev_err(&pdev->dev,
			"failed to allocate memory for driver's panic handler data\n");
	} else {
		itmon_v1_panic->nb_panic_block.notifier_call = itmon_v1_logging_panic_handler;
		itmon_v1_panic->pdev = itmon;
		atomic_notifier_chain_register(&panic_notifier_list,
						   &itmon_v1_panic->nb_panic_block);
	}

	platform_set_drvdata(pdev, itmon);

	for (i = 0; i < V1_TRANS_TYPE_NUM; i++)
		INIT_LIST_HEAD(&pdata->tracelist[i]);

	pdata->crash_in_progress = false;
	pdata->sysfs_s2d = true;

	itmon_v1_init(itmon, true);

	g_itmon_v1 = itmon;
	pdata->probed = true;

	dev_info(&pdev->dev, "success to probe Exynos ITMON-v1 driver\n");

	return 0;
}

static int itmon_v1_remove(struct platform_device *pdev)
{
	platform_set_drvdata(pdev, NULL);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int itmon_v1_suspend(struct device *dev)
{
	return 0;
}

static int itmon_v1_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct itmon_v1_dev *itmon = platform_get_drvdata(pdev);
	struct itmon_v1_platdata *pdata = itmon->pdata;

	/* re-enable ITMON if cp-crash progress is not starting */
	if (!pdata->crash_in_progress)
		itmon_v1_init(itmon, true);

	return 0;
}
#endif

struct itmon_platform_ops itmon_v1_ops = {
	.probe = itmon_v1_probe,
	.remove = itmon_v1_remove,
	.suspend = itmon_v1_suspend,
	.resume = itmon_v1_resume,
	.enable = itmon_v1_enable,
	.pd_sync = itmon_v1_pd_sync,
};
