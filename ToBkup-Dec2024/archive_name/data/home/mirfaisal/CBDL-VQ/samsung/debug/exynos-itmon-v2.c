// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * IPs Traffic Monitor(ITMON) Driver v2 for Samsung Exynos SOC
 * By Hosung Kim (hosung0.kim@samsung.com)
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>
#include <linux/of_irq.h>
#include <linux/delay.h>
#include <linux/pm_domain.h>
#include <linux/of_reserved_mem.h>
#include <linux/sched/clock.h>
#include <linux/panic_notifier.h>
#include <soc/samsung/exynos-pd.h>
#include <soc/samsung/exynos-itmon.h>
#include <soc/samsung/debug-snapshot.h>
#include <soc/samsung/debug-snapshot-log.h>
#include "exynos-itmon-platform.h"
#include "exynos-itmon-local-v2.h"

static const char * const itmon_v2_pathtype[] = {
	"DATA Path transaction",
	"Configuration(SFR) Path transaction",
};

/* Error Code Description */
static const char * const itmon_v2_errcode[] = {
	"Error Detect by the Slave(SLVERR)",
	"Decode error(DECERR)",
	"Unsupported transaction error",
	"Power Down access error",
	"Inteded Access Violation",
	"Unsupported transaction error",
	"Timeout error - response timeout in timeout value",
	"Unsupported transaction error",
	"Protocol Checker Error",
	"Protocol Checker Error",
	"Protocol Checker Error",
	"Protocol Checker Error",
	"Protocol Checker Error",
	"Protocol Checker Error",
	"Protocol Checker Error",
	"Timeout error - freeze mode",
};

static const char * const itmon_v2_node_string[] = {
	"M_NODE",
	"TAXI_S_NODE",
	"TAXI_M_NODE",
	"S_NODE",
};

static const char * const itmon_v2_cpu_node_string[] = {
	"CLUSTER0_P",
	"M_CPU",
	"SCI_IRPM",
	"SCI_CCM",
	"CCI",
};

static const unsigned int itmon_v2_invalid_addr[] = {
	0x03000000,
	0x04000000,
};

static struct itmon_v2_dev *g_itmon_v2;

static const struct of_device_id itmon_v2_dt_match[] = {
	{.compatible = "samsung,exynos-itmon-v2",
	 .data = NULL,},
	{},
};
MODULE_DEVICE_TABLE(of, itmon_v2_dt_match);

struct itmon_v2_nodeinfo *itmon_v2_get_nodeinfo_by_group(struct itmon_v2_dev *itmon,
						  struct itmon_v2_nodegroup *group,
						  const char *name)
{
	struct itmon_v2_nodeinfo *node = group->nodeinfo;
	int i;

	for (i = 0; i < group->nodesize; i++)
		if (!strncmp(node[i].name, name, strlen(name)))
			return &node[i];

	return NULL;
}

struct itmon_v2_nodeinfo *itmon_v2_get_nodeinfo(struct itmon_v2_dev *itmon,
						 struct itmon_v2_nodegroup *group,
						 const char *name)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_nodeinfo *node = NULL;
	int i;

	if (!name)
		return NULL;

	if (group) {
		return itmon_v2_get_nodeinfo_by_group(itmon, group, name);
	}
	for (i = 0; i < pdata->num_nodegroup; i++) {
		group = &pdata->nodegroup[i];
		node = itmon_v2_get_nodeinfo_by_group(itmon, group, name);
		if (node)
			return node;
	}

	return NULL;
}

struct itmon_v2_nodeinfo *itmon_v2_get_nodeinfo_group_by_eid(struct itmon_v2_dev *itmon,
							  struct itmon_v2_nodegroup *group,
							  u32 err_id)
{
	struct itmon_v2_nodeinfo *node = group->nodeinfo;
	int i;

	for (i = 0; i < group->nodesize; i++) {
		if (node[i].err_id == err_id)
			return &node[i];
	}

	return NULL;
}

struct itmon_v2_nodeinfo *itmon_v2_get_nodeinfo_by_eid(struct itmon_v2_dev *itmon,
						 struct itmon_v2_nodegroup *group,
						 int path_type, u32 err_id)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_nodeinfo *node = NULL;
	int i;

	if (group) {
		return itmon_v2_get_nodeinfo_group_by_eid(itmon, group, err_id);
	}
	for (i = 0; i < pdata->num_nodegroup; i++) {
		group = &pdata->nodegroup[i];
		if (group->path_type != path_type)
			continue;
		node = itmon_v2_get_nodeinfo_group_by_eid(itmon, group, err_id);
		if (node)
			return node;
	}

	return NULL;
}

struct itmon_v2_nodeinfo *itmon_v2_get_nodeinfo_group_by_tmout_offset(struct itmon_v2_dev *itmon,
							  struct itmon_v2_nodegroup *group,
							  u32 tmout_offset)
{
	struct itmon_v2_nodeinfo *node = group->nodeinfo;
	int i;

	for (i = 0; i < group->nodesize; i++) {
		if (node[i].tmout_offset == tmout_offset)
			return &node[i];
	}

	return NULL;
}

struct itmon_v2_nodeinfo *itmon_v2_get_nodeinfo_by_tmout_offset(struct itmon_v2_dev *itmon,
						 struct itmon_v2_nodegroup *group,
						 u32 tmout_offset)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_nodeinfo *node = NULL;
	int i;

	if (group) {
		return itmon_v2_get_nodeinfo_group_by_tmout_offset(itmon, group, tmout_offset);
	} else {
		for (i = 0; i < pdata->num_nodegroup; i++) {
			group = &pdata->nodegroup[i];
			node = itmon_v2_get_nodeinfo_group_by_tmout_offset(itmon, group, tmout_offset);
			if (node)
				return node;
		}
	}

	return NULL;
}

static struct itmon_v2_rpathinfo *itmon_v2_get_rpathinfo(struct itmon_v2_dev *itmon,
						   unsigned int id,
						   char *dest_name)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	int i;

	if (!dest_name)
		return NULL;

	for (i = 0; i < pdata->num_rpathinfo; i++) {
		if (pdata->rpathinfo[i].id == (id & pdata->rpathinfo[i].bits)) {
			if (!strncmp(pdata->rpathinfo[i].dest_name, dest_name,
				  strlen(pdata->rpathinfo[i].dest_name)))
				return &pdata->rpathinfo[i];
		}
	}

	return NULL;
}

static char *itmon_v2_get_masterinfo(struct itmon_v2_dev *itmon,
				     char *port_name,
				     u32 user)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	int i;

	if (!port_name)
		return NULL;

	for (i = 0; i < pdata->num_masterinfo; i++) {
		if ((user & pdata->masterinfo[i].bits) ==
			pdata->masterinfo[i].user) {
			if (!strncmp(pdata->masterinfo[i].port_name,
				port_name, strlen(port_name)))
				return pdata->masterinfo[i].master_name;
		}
	}
	return NULL;
}

/* need to reconfiguable to the address */
static void itmon_v2_en_addr_detect(struct itmon_v2_dev *itmon,
				struct itmon_v2_nodegroup *group,
				struct itmon_v2_nodeinfo *node,
				bool en)
{
	void __iomem *reg = group->regs;
	u32 tmp, val, offset, i;
	char *name;

	if (node) {
		if (node->type == M_NODE || node->type == T_M_NODE)
			offset = V2_PRTCHK_M_CTL(node->prtchk_offset);
		else
			offset = V2_PRTCHK_S_CTL(node->prtchk_offset);

		val = __raw_readl(reg + offset) | V2_INTEND_ACCESS_INT_EN;
		__raw_writel(val, reg + offset);

		val = ((unsigned int)V2_INTEND_ADDR_START & U32_MAX);
		__raw_writel(val, reg + V2_PRTCHK_START_ADDR_LOW);

		val = (unsigned int)(((unsigned long)V2_INTEND_ADDR_START >> 32) & U16_MAX);
		__raw_writel(val, reg + V2_PRTCHK_START_END_ADDR_UPPER);

		val = ((unsigned int)V2_INTEND_ADDR_END & 0xFFFFFFFF);
		__raw_writel(val, reg + V2_PRTCHK_END_ADDR_LOW);
		val = ((unsigned int)(((unsigned long)V2_INTEND_ADDR_END >> 32)
					& 0XFFFF0000) << 16);
		tmp = readl(reg + V2_PRTCHK_START_END_ADDR_UPPER);
		__raw_writel(tmp | val, reg + V2_PRTCHK_START_END_ADDR_UPPER);
		name = node->name;
	} else {
		node = group->nodeinfo;
		for (i = 0; i < group->nodesize; i++) {
			if (node[i].type == M_NODE || node[i].type == T_M_NODE)
				offset = V2_PRTCHK_M_CTL(node[i].prtchk_offset);
			else
				offset = V2_PRTCHK_S_CTL(node[i].prtchk_offset);

			val = readl(reg + offset) | V2_INTEND_ACCESS_INT_EN;
			writel(val, reg + offset);

			val = ((unsigned int)V2_INTEND_ADDR_START & U32_MAX);
			writel(val, reg + V2_PRTCHK_START_ADDR_LOW);

			val = (unsigned int)(((unsigned long)V2_INTEND_ADDR_START >> 32) & U16_MAX);
			writel(val, reg + V2_PRTCHK_START_END_ADDR_UPPER);

			val = ((unsigned int)V2_INTEND_ADDR_END & 0xFFFFFFFF);
			writel(val, reg + V2_PRTCHK_END_ADDR_LOW);
			val = ((unsigned int)(((unsigned long)V2_INTEND_ADDR_END >> 32)
						& 0XFFFF0000) << 16);
			tmp = readl(reg + V2_PRTCHK_START_END_ADDR_UPPER);
			writel(tmp | val, reg + V2_PRTCHK_START_END_ADDR_UPPER);

		}
		name = group->name;
	}
	dev_dbg(itmon->dev, "ITMON - %s addr detect %sabled\n",
				name, en == true ? "en" : "dis");
}

static void itmon_v2_en_prt_chk(struct itmon_v2_dev *itmon,
				struct itmon_v2_nodegroup *group,
				struct itmon_v2_nodeinfo *node,
				bool en)
{
	void __iomem *reg = group->regs;
	u32 offset, val = 0, i;
	char *name;

	if (en)
		val = V2_RD_RESP_INT_EN | V2_WR_RESP_INT_EN |
		     V2_ARLEN_RLAST_INT_EN | V2_AWLEN_WLAST_INT_EN;

	if (node) {
		if (node->prtchk_offset == NOT_SUPPORT)
			return;
		if (node->type == M_NODE || node->type == T_M_NODE)
			offset = V2_PRTCHK_M_CTL(node->prtchk_offset);
		else
			offset = V2_PRTCHK_S_CTL(node->prtchk_offset);

		writel(val, reg + offset);
		name = node->name;
	} else {
		node = group->nodeinfo;
		for (i = 0; i < group->nodesize; i++) {
			if (node[i].prtchk_offset == NOT_SUPPORT)
				continue;
			if (node[i].type == M_NODE || node[i].type == T_M_NODE)
				offset = V2_PRTCHK_M_CTL(node[i].prtchk_offset);
			else
				offset = V2_PRTCHK_S_CTL(node[i].prtchk_offset);

			writel(val, reg + offset);
		}
		name = group->name;
	}
	dev_dbg(itmon->dev, "ITMON - %s hw_assert %sabled\n",
			name, en == true ? "en" : "dis");
}

static void itmon_v2_en_err_report(struct itmon_v2_dev *itmon,
				struct itmon_v2_nodegroup *group,
				struct itmon_v2_nodeinfo *node,
				bool en)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	void __iomem *reg = group->regs;
	u32 val = 0, i;
	char *name;

	if (!pdata->probed)
		writel(1, reg + V2_ERR_LOG_CLR);

	val = readl(reg + V2_ERR_LOG_EN_NODE);

	if (node) {
		if (en)
			val |= BIT(node->id);
		else
			val &= ~BIT(node->id);
		writel(val, reg + V2_ERR_LOG_EN_NODE);
		name = node->name;
	} else {
		node = group->nodeinfo;
		for (i = 0; i < group->nodesize; i++) {
			if (en)
				val |= (1 << node[i].id);
			else
				val &= ~(1 << node[i].id);
		}
		writel(val, reg + V2_ERR_LOG_EN_NODE);
		name = group->name;
	}
	dev_dbg(itmon->dev, "ITMON - %s error reporting %sabled\n",
			name, en == true ? "en" : "dis");
}

static void itmon_v2_en_timeout(struct itmon_v2_dev *itmon,
				struct itmon_v2_nodegroup *group,
				struct itmon_v2_nodeinfo *node,
				bool en)
{
	void __iomem *reg = group->regs;
	u32 offset, val = 0, i;
	char *name;

	if (node) {
		if (node->tmout_offset == NOT_SUPPORT)
			return;
		offset = V2_TMOUT_CTL(node->tmout_offset);

		/* Enabled */
		val = (en << 0);
		val |= (node->tmout_frz_en << 1);
		val |= (node->time_val << 4);

		writel(val, reg + offset);
		name = node->name;
	} else {
		node = group->nodeinfo;
		for (i = 0; i < group->nodesize; i++) {
			if (node[i].type != S_NODE || node[i].tmout_offset == NOT_SUPPORT)
				continue;
			offset = V2_TMOUT_CTL(node[i].tmout_offset);

			/* en */
			val = (en << 0);
			val |= (node[i].tmout_frz_en << 1);
			val |= (node[i].time_val << 4);

			writel(val, reg + offset);
		}
		name = group->name;
	}
	dev_dbg(itmon->dev, "ITMON - %s timeout %sabled\n",
			name, en == true ? "en" : "dis");
}

int itmon_v2_en_by_name(const char *name, bool en)
{
	struct itmon_v2_nodeinfo *node;
	struct itmon_v2_nodegroup *group;

	if (!g_itmon_v2)
		return -ENODEV;

	node = itmon_v2_get_nodeinfo(g_itmon_v2, NULL, name);
	if (!node) {
		dev_err(g_itmon_v2->dev, "%s node is not found\n", name);
		return -ENODEV;
	}

	group = node->group;
	if (!group) {
		dev_err(g_itmon_v2->dev, "%s node's group is  not found\n", name);
		return -ENODEV;
	}

	if (group->pd_support && !group->pd_status) {
		dev_err(g_itmon_v2->dev, "%s group - %s node NOT pd on\n",
			group->name, node->name);
		return -EIO;
	}

	itmon_v2_en_err_report(g_itmon_v2, group, node, en);
	itmon_v2_en_prt_chk(g_itmon_v2, group, node, en);
	if (node->type == S_NODE)
		itmon_v2_en_timeout(g_itmon_v2, group, node, en);

	return 0;
}
EXPORT_SYMBOL(itmon_v2_en_by_name);

static void itmon_v2_en_global(struct itmon_v2_dev *itmon,
				struct itmon_v2_nodegroup *group,
				int int_en,
				int err_log_en,
				int tmout_en,
				int prtchk_en,
				int fixed_det_en)
{
	u32 val = (int_en) | (err_log_en << 1) | (tmout_en << 2) |
			(prtchk_en << 3) | (fixed_det_en << 5);

	writel(val, group->regs + V2_DBG_CTL);
}

static void itmon_v2_init_by_group(struct itmon_v2_dev *itmon,
				struct itmon_v2_nodegroup *group,
				bool en)
{
	struct itmon_v2_nodeinfo *node;
	int i;

	node = group->nodeinfo;
	for (i = 0; i < group->nodesize; i++) {
		if (en) {
			if (node[i].err_en)
				itmon_v2_en_err_report(itmon, group, &node[i], en);
			if (node[i].prt_chk_en)
				itmon_v2_en_prt_chk(itmon, group, &node[i], en);
		} else {
			/* as default en */
			itmon_v2_en_err_report(itmon, group, &node[i], en);
			itmon_v2_en_prt_chk(itmon, group, &node[i], en);
		}
		if (node[i].type == S_NODE && node[i].tmout_en)
			itmon_v2_en_timeout(itmon, group, &node[i], en);
		if (node[i].addr_detect_en && node[i].prtchk_offset != NOT_SUPPORT)
			itmon_v2_en_addr_detect(itmon, group, &node[i], en);
	}
}

static void itmon_v2_init(struct itmon_v2_dev *itmon, bool en)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_nodegroup *group;
	int i;

	for (i = 0; i < pdata->num_nodegroup; i++) {
		group = &pdata->nodegroup[i];
		if (group->pd_support && !group->pd_status)
			continue;
		itmon_v2_en_global(itmon, group, en, en, en, en, en);
		if (pdata->def_en)
			itmon_v2_init_by_group(itmon, group, en);
	}

	pdata->en = en;
	dev_info(itmon->dev, "itmon %sabled\n", pdata->en ? "en" : "dis");
}

static void itmon_v2_pd_sync(const char *pd_name, bool en)
{
	struct itmon_v2_dev *itmon = g_itmon_v2;
	struct itmon_v2_platdata *pdata;
	struct itmon_v2_nodegroup *group;
	int i;

	if (!itmon || !itmon->pdata || !itmon->pdata->probed)
		return;

	pdata = itmon->pdata;

	for (i = 0; i < pdata->num_nodegroup; i++) {
		group = &pdata->nodegroup[i];
		if (group->pd_support &&
			!strncmp(pd_name, group->pd_name, strlen(pd_name))) {
			dev_dbg(itmon->dev,
				"%s: pd_name:%s enabled:%x, pd_status:%x\n",
				__func__, pd_name, en, group->pd_status);
			if (group->pd_status != en) {
				if (en) {
					itmon_v2_en_global(itmon, group, en, en, en, en, en);
					if (!pdata->def_en)
						itmon_v2_init_by_group(itmon, group, en);
				}
				group->pd_status = en;
			}
		}
	}
}

static void itmon_v2_en(bool en)
{
	if (g_itmon_v2)
		itmon_v2_init(g_itmon_v2, en);
}

static int itmon_v2_notifier_handler(struct itmon_v2_dev *itmon,
					   struct itmon_v2_traceinfo *info,
					   unsigned int trans_type)
{
	int ret = 0;

	/* After treatment by port */
	if ((!info->src || strlen(info->src) < 1) ||
		(!info->dest || strlen(info->dest) < 1))
		return -1;

	itmon->notifier_info.port = info->src;
	itmon->notifier_info.master = info->master;
	itmon->notifier_info.dest = info->dest;
	itmon->notifier_info.read = info->read;
	itmon->notifier_info.target_addr = info->target_addr;
	itmon->notifier_info.errcode = info->err_code;
	itmon->notifier_info.onoff = info->onoff;

	pr_err("----------------------------------------------------------------------------------\n\t\t+ITMON Notifier Call Information\n");

	/* call notifier_call_chain of itmon */
	ret = atomic_notifier_call_chain(&itmon_notifier_list,
				0, &itmon->notifier_info);

	pr_err("\t\t-ITMON Notifier Call Information\n----------------------------------------------------------------------------------\n");

	return ret;
}

static void itmon_v2_post_handler(struct itmon_v2_dev *itmon, bool err)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	unsigned long ts = pdata->last_time;
	unsigned long rem_nsec = do_div(ts, 1000000000);
	unsigned long cur_time = local_clock();
	unsigned long delta;

	delta = pdata->last_time == 0 ? 0 : cur_time - pdata->last_time;

	dev_err(itmon->dev, "Before ITMON: [%5lu.%06lu], delta: %lu, last_errcnt: %d\n",
		(unsigned long)ts, rem_nsec / 1000, delta, pdata->last_errcnt);

	/* delta < 1s */
	if (delta > 0 && delta < 1000000000UL) {
		pdata->last_errcnt++;
		if (pdata->last_errcnt > V2_ERR_THRESHOLD)
			dbg_snapshot_do_dpm_policy(GO_S2D_ID);
	} else {
		pdata->last_errcnt = 0;
	}

	pdata->last_time = cur_time;
}

static int itmon_v2_parse_cpuinfo_by_name(struct itmon_v2_dev *itmon,
				       const char *name,
				       unsigned int userbit,
				       char *cpu_name)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	int core_num = 0, el2 = 0, strong = 0, res = 0, i;

	if (cpu_name == NULL)
		return 0;

	for (i = 0; i < (int)ARRAY_SIZE(itmon_v2_cpu_node_string); i++) {
		if (!strncmp(name, itmon_v2_cpu_node_string[i],
			strlen(itmon_v2_cpu_node_string[i]))) {
			if (userbit & BIT(0))
				el2 = 1;
			if (DSS_NR_CPUS > 8) {
				if (!(userbit & BIT(1)))
					strong = 1;
				core_num = ((userbit & (0xF << 2)) >> 2);
			} else {
				core_num = ((userbit & (0x7 << 1)) >> 1);
				strong = 0;
			}
			if (pdata->cpu_parsing) {
				res = snprintf(cpu_name, sizeof(cpu_name) - 1,
					"CPU%d%s%s", core_num, el2 == 1 ? "EL2" : "",
					strong == 1 ? "Strng" : "");
			} else {
				res = snprintf(cpu_name, sizeof(cpu_name) - 1, "CPU");
			}
			if (res < 0) {
				pr_warn("snprintf failed: %d\n", res);
				return 0;
			}
			return 1;
		}
	};

	return 0;
}

static void itmon_v2_report_timeout(struct itmon_v2_dev *itmon,
				struct itmon_v2_nodegroup *group)
{
	unsigned int id, payload0, payload1, payload2, payload3, payload4, payload5;
	unsigned int axid, valid, timeout;
	unsigned long addr, user;
	int i, num = SZ_128;
	int rw_offset = 0;
	u32 axburst, axprot, axlen, axsize, domain;
	unsigned int trans_type;
	unsigned long tmout_frz_stat;
	unsigned int tmout_offset;
	struct itmon_v2_nodeinfo *node;
	unsigned int bit = 0;

	if (group)
		dev_info(itmon->dev, "group %s frz_support %s\n",
				group->name, group->frz_support ? "YES" : "NO");
	else
		return;

	if (!group->frz_support)
		return;

	tmout_frz_stat = readl(group->regs + V2_OFFSET_TMOUT_REG);

	if (tmout_frz_stat == 0x0)
		return;

	dev_info(itmon->dev, "TIMEOUT_FREEZE_STATUS 0x%08lx", tmout_frz_stat);

	for_each_set_bit(bit, &tmout_frz_stat, group->nodesize) {
		tmout_offset = (1 << bit) >> 1;
		node = itmon_v2_get_nodeinfo_group_by_tmout_offset(itmon, group, tmout_offset);

		pr_err("\nITMON Report Timeout Error Occurred : Master --> %s\n\n",
				node->name);
		pr_err("> NUM| TYPE|    MASTER BLOCK|          MASTER|VALID|TMOUT|    ID|DOMAIN|SIZE|PROT|         ADDRESS|PAYLOAD0|PAYLOAD1|PAYLOAD2|PAYLOAD3|PAYLOAD4|PAYLOAD5");

		for (trans_type = V2_TRANS_TYPE_WRITE; trans_type < V2_TRANS_TYPE_NUM; trans_type++) {
			num = (trans_type == V2_TRANS_TYPE_READ ? SZ_128 : SZ_64);
			rw_offset = (trans_type == V2_TRANS_TYPE_READ ? 0 : V2_TMOUT_RW_OFFSET);
			for (i = 0; i < num; i++) {
				struct itmon_v2_rpathinfo *port = NULL;
				char *master_name = NULL;
				char cpu_name[16];

				/*
				TODO : rename PAYLOADn registers
				PAYLOAD0 -> PAYLOAD_FROM_BUFFER
				PAYLOAD1 -> PAYLOAD_FROM_SRAM_1
				PAYLOAD2 -> PAYLOAD_FROM_SRAM_2
				PAYLOAD3 -> PAYLOAD_FROM_SRAM_3
				PAYLOAD4 -> USER_FROM_BUFFER_0
				PAYLOAD5 -> USER_FROM_BUFFER_1
				*/

				writel(i | (bit << 16), group->regs + V2_OFFSET_TMOUT_REG + V2_TMOUT_POINT_ADDR + rw_offset);
				id = readl(group->regs + V2_OFFSET_TMOUT_REG + V2_TMOUT_ID + rw_offset);
				payload0 = readl(group->regs + V2_OFFSET_TMOUT_REG +
						V2_TMOUT_PAYLOAD_0 + rw_offset);
				payload1 = readl(group->regs + V2_OFFSET_TMOUT_REG +
						V2_TMOUT_PAYLOAD_1 + rw_offset);
				payload2 = readl(group->regs + V2_OFFSET_TMOUT_REG +
						V2_TMOUT_PAYLOAD_2 + rw_offset);	/* mid/small: payload 1 */
				payload3 = readl(group->regs + V2_OFFSET_TMOUT_REG +
						V2_TMOUT_PAYLOAD_3 + rw_offset);	/* mid/small: payload 2 */
				payload4 = readl(group->regs + V2_OFFSET_TMOUT_REG +
						V2_TMOUT_PAYLOAD_4 + rw_offset);	/* mid/small: payload 3 */
				payload5 = readl(group->regs + V2_OFFSET_TMOUT_REG +
						V2_TMOUT_PAYLOAD_5 + rw_offset);

				timeout = (payload0 & (unsigned int)(GENMASK(7, 4))) >> 4;
				user = ((unsigned long)payload4 << 32ULL) | payload5;
				addr = (((unsigned long)payload1 & GENMASK(15, 0)) << 32ULL) | payload2;
				axid = id & GENMASK(5, 0); /* ID[5:0] 6bit : R-PATH */
				valid = payload0 & BIT(0); /* PAYLOAD[0] : Valid or Not valid */
				axburst = V2_TMOUT_BIT_AXBURST(payload3);
				axprot = V2_TMOUT_BIT_AXPROT(payload3);
				axlen = (payload3 & (unsigned int)GENMASK(7, 4)) >> 4;
				axsize = (payload3 & (unsigned int)GENMASK(18, 16)) >> 16;
				domain = (payload3 & BIT(19));
				if (group->path_type == V2_CONFIG) {
					port = itmon_v2_get_rpathinfo(itmon, axid, "CONFIGURATION");
					if (port)
						itmon_v2_parse_cpuinfo_by_name(itmon, port->port_name, user, cpu_name);
					master_name = cpu_name;
				} else {
					port = itmon_v2_get_rpathinfo(itmon, axid, node->name);
					if (port)
						master_name = itmon_v2_get_masterinfo(itmon, port->port_name, user);
				}

				if (valid)
					pr_err("> %03d|%5s|%16s|%16s|%5u|%5x|%06x|%6s|%4llu|%4s|%016zx|%08x|%08x|%08x|%08x|%08x|%08x\n",
							i, (trans_type == V2_TRANS_TYPE_READ) ? "READ" : "WRITE",
							port ? port->port_name : V2_NO_NAME,
							master_name ? master_name : V2_NO_NAME,
							valid, timeout, id, domain ? "SHARE" : "NSHARE",
							int_pow(2, axsize * (axlen + 1)),
							axprot & BIT(1) ? "NSEC" : "SEC",
							addr, payload0, payload1, payload2, payload3, payload4, payload5);
			}
		}
	}
}

static void itmon_v2_report_rawdata(struct itmon_v2_dev *itmon,
				 struct itmon_v2_tracedata *data)
{
	struct itmon_v2_nodeinfo *m_node = data->m_node;
	struct itmon_v2_nodeinfo *det_node = data->det_node;
	struct itmon_v2_nodegroup *group = data->group;

	/* Output Raw register information */
	pr_err("\tRaw Register Information ---------------------------------------------------\n\n");

	if (m_node && det_node) {
		pr_err("\t> M_NODE     : %s(%s, id: %03u)\n"
			"\t> DETECT_NODE: %s(%s, id: %03u)\n",
			m_node->name, itmon_v2_node_string[m_node->type], m_node->err_id,
			det_node->name, itmon_v2_node_string[det_node->type], det_node->err_id);
	}

	pr_err("\t> BASE       : %s(0x%08llx)\n"
		"\t> INFO_0     : 0x%08X\n"
		"\t> INFO_1     : 0x%08X\n"
		"\t> INFO_2     : 0x%08X\n"
		"\t> INFO_3     : 0x%08X\n"
		"\t> INFO_4     : 0x%08X\n"
		"\t> INFO_5     : 0x%08X\n"
		"\t> INFO_6     : 0x%08X\n",
		group->name, group->phy_regs,
		data->info_0,
		data->info_1,
		data->info_2,
		data->info_3,
		data->info_4,
		data->info_5,
		data->info_6);
}

static void itmon_v2_report_traceinfo(struct itmon_v2_dev *itmon,
				   struct itmon_v2_traceinfo *info,
				   unsigned int trans_type)
{
	if (!info->dirty)
		return;

	pr_err("\n----------------------------------------------------------------------------------\n"
		"\tTransaction Information\n\n"
		"\t> Master         : %s %s\n"
		"\t> Target         : %s\n"
		"\t> Target Address : 0x%llX %s\n"
		"\t> Type           : %s\n"
		"\t> Error code     : %s\n",
		info->src, info->master ? info->master : "",
		info->dest ? info->dest : V2_NO_NAME,
		info->target_addr,
		info->baaw_prot == true ? "(BAAW Remapped address)" : "",
		trans_type == V2_TRANS_TYPE_READ ? "READ" : "WRITE",
		itmon_v2_errcode[info->err_code]);

	pr_err("\n----------------------------------------------------------------------------------\n"
		"\t\t> Size           : %llu bytes x %u burst => %llu bytes\n"
		"\t\t> Burst Type     : %u (0:FIXED, 1:INCR, 2:WRAP)\n"
		"\t\t> Level          : %s\n"
		"\t\t> Protection     : %s\n"
		"\t\t> Path Type      : %s\n\n",
		int_pow(2, info->axsize), info->axlen + 1,
		int_pow(2, info->axsize) * (info->axlen + 1),
		info->axburst,
		info->axprot & BIT(0) ? "Privileged" : "Unprivileged",
		info->axprot & BIT(1) ? "Non-secure" : "Secure",
		itmon_v2_pathtype[info->path_type]);
}

static void itmon_v2_report_pathinfo(struct itmon_v2_dev *itmon,
				  struct itmon_v2_tracedata *data,
				  struct itmon_v2_traceinfo *info,
				  unsigned int trans_type)

{
	struct itmon_v2_nodeinfo *m_node = data->m_node;
	struct itmon_v2_nodeinfo *det_node = data->det_node;

	if (!m_node || !det_node)
		return;

	if (!info->path_dirty) {
		dev_err(itmon->dev,
			"\n----------------------------------------------------------------------------------\n\t\t"
			"ITMON Report (%s)\n"
			"----------------------------------------------------------------------------------\n\t"
			"PATH Information\n\n",
			trans_type == V2_TRANS_TYPE_READ ? "READ" : "WRITE");
		info->path_dirty = true;
	}
	pr_info("\t> M_NODE     : %14s, %8s(id: %u)\n",
		m_node->name, itmon_v2_node_string[m_node->type], m_node->err_id);
	pr_info("\t> DETECT_NODE: %14s, %8s(id: %u)\n",
		det_node->name, itmon_v2_node_string[det_node->type], det_node->err_id);
}

static int itmon_v2_parse_cpuinfo(struct itmon_v2_dev *itmon,
			       struct itmon_v2_tracedata *data,
			       struct itmon_v2_traceinfo *info,
			       unsigned int userbit)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_nodeinfo *m_node = data->m_node;
	int core_num = 0, el2 = 0, strong = 0, i, ret = 0;

	for (i = 0; i < (int)ARRAY_SIZE(itmon_v2_cpu_node_string); i++) {
		if (!strncmp(m_node->name, itmon_v2_cpu_node_string[i],
			strlen(itmon_v2_cpu_node_string[i]))) {
			if (userbit & BIT(0))
				el2 = 1;
			if (DSS_NR_CPUS > 8) {
				if (!(userbit & BIT(1)))
					strong = 1;
				core_num = ((userbit & (0xF << 2)) >> 2);
			} else {
				core_num = ((userbit & (0x7 << 1)) >> 1);
				strong = 0;
			}
			if (pdata->cpu_parsing) {
				ret = snprintf(info->buf, sizeof(info->buf) - 1,
					"CPU%d %s %s", core_num, el2 == 1 ? "EL2" : "",
					strong == 1 ? "Strong" : "");
			} else {
				ret = snprintf(info->buf, sizeof(info->buf) - 1, "CPU");
			}
			if (ret < 0) {
				pr_warn("snprintf is failed for info->buf");
				info->buf[0] = '\0';
			}
			info->master = info->buf;
			return 1;
		}
	};

	return 0;
}

static void itmon_v2_parse_traceinfo(struct itmon_v2_dev *itmon,
				   struct itmon_v2_tracedata *data,
				   unsigned int trans_type)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_nodegroup *group = data->group;
	struct itmon_v2_traceinfo *new_info = NULL;
	u64 axuser;
	int i;

	if (!data->m_node || !data->det_node)
		return;

	new_info = kzalloc(sizeof(struct itmon_v2_traceinfo), GFP_ATOMIC);
	if (!new_info) {
		dev_err(itmon->dev, "failed to kmalloc for %s -> %s\n",
			data->m_node->name, data->det_node->name);
		return;
	}

	axuser = data->info_5;
	new_info->m_id = data->m_id;
	new_info->s_id = data->det_id;
	new_info->m_node = data->m_node;
	new_info->s_node = data->det_node;
	new_info->src = data->m_node->name;
	new_info->dest = data->det_node->name;
	new_info->master = itmon_v2_get_masterinfo(itmon, new_info->m_node->name, axuser);
	if (new_info->master == NULL)
		new_info->master = "";

	if (group->path_type == V2_CONFIG)
		itmon_v2_parse_cpuinfo(itmon, data, new_info, axuser);

	/* Common Information */
	new_info->path_type = group->path_type;
	new_info->target_addr = (((u64)data->info_3 & GENMASK(15, 0)) << 32ULL);
	new_info->target_addr |= data->info_2;
	new_info->err_code = data->err_code;
	new_info->dirty = true;
	new_info->axsize = V2_AXSIZE(data->info_3);
	new_info->axlen = V2_AXLEN(data->info_3);
	new_info->axburst = V2_AXBURST(data->info_4);
	new_info->axprot = V2_AXPROT(data->info_4);
	new_info->baaw_prot = false;

	for (i = 0; i < (int)ARRAY_SIZE(itmon_v2_invalid_addr); i++) {
		if (new_info->target_addr == itmon_v2_invalid_addr[i]) {
			new_info->baaw_prot = true;
			break;
		}
	}
	data->ref_info = new_info;
	list_add_tail(&new_info->list, &pdata->infolist[trans_type]);
}

static void itmon_v2_analyze_errlog(struct itmon_v2_dev *itmon)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_traceinfo *info, *next_info;
	struct itmon_v2_tracedata *data, *next_data;
	unsigned int trans_type;

	/* Parse */
	for (trans_type = 0; trans_type < V2_TRANS_TYPE_NUM; trans_type++) {
		list_for_each_entry(data, &pdata->datalist[trans_type], list)
			itmon_v2_parse_traceinfo(itmon, data, trans_type);
	}

	/* Report and Clean-up traceinfo */
	for (trans_type = 0; trans_type < V2_TRANS_TYPE_NUM; trans_type++) {
		list_for_each_entry_safe(info, next_info, &pdata->infolist[trans_type], list) {
			info->path_dirty = false;
			itmon_v2_notifier_handler(itmon, info, trans_type);
			list_for_each_entry_safe(data, next_data, &pdata->datalist[trans_type], list) {
				if (data->ref_info == info)
					itmon_v2_report_pathinfo(itmon, data, info, trans_type);
			}
			itmon_v2_report_traceinfo(itmon, info, trans_type);
			list_del(&info->list);
			kfree(info);
		}
	}

	/* Report Raw all tracedata and Clean-up tracedata and node */
	for (trans_type = 0; trans_type < V2_TRANS_TYPE_NUM; trans_type++) {
		list_for_each_entry_safe(data, next_data, &pdata->datalist[trans_type], list) {
			itmon_v2_report_rawdata(itmon, data);
			list_del(&data->list);
			kfree(data);
		}
	}
}

static void *itmon_v2_collect_errlog(struct itmon_v2_dev *itmon,
			    struct itmon_v2_nodegroup *group)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_tracedata *new_data = NULL;
	void __iomem *reg = group->regs;

	new_data = kzalloc(sizeof(struct itmon_v2_tracedata), GFP_ATOMIC);
	if (!new_data) {
		dev_err(itmon->dev, "failed to kmalloc for %s group\n", group->name);
		return NULL;
	}

	new_data->info_0 = readl(reg + V2_ERR_LOG_INFO_0);
	new_data->info_1 = readl(reg + V2_ERR_LOG_INFO_1);
	new_data->info_2 = readl(reg + V2_ERR_LOG_INFO_2);
	new_data->info_3 = readl(reg + V2_ERR_LOG_INFO_3);
	new_data->info_4 = readl(reg + V2_ERR_LOG_INFO_4);
	new_data->info_5 = readl(reg + V2_ERR_LOG_INFO_5);
	new_data->info_6 = readl(reg + V2_ERR_LOG_INFO_6);
	new_data->en_node = readl(reg + V2_ERR_LOG_EN_NODE);

	new_data->m_id = V2_MNODE_ID(new_data->info_0);
	new_data->det_id = V2_DET_ID(new_data->info_0);

	new_data->m_node = itmon_v2_get_nodeinfo_by_eid(itmon, NULL,
					group->path_type, new_data->m_id);
	new_data->det_node = itmon_v2_get_nodeinfo_by_eid(itmon, NULL,
					group->path_type, new_data->det_id);

	new_data->read = !V2_RW(new_data->info_0);
	new_data->err_code = V2_ERR_CODE(new_data->info_0);
	new_data->ref_info = NULL;
	new_data->group = group;

	list_add_tail(&new_data->list, &pdata->datalist[new_data->read]);

	return (void *)new_data;
}

static int itmon_v2_pop_errlog(struct itmon_v2_dev *itmon,
			       struct itmon_v2_nodegroup *group,
			       bool clear)
{
	struct itmon_v2_tracedata *data;
	void __iomem *reg = group->regs;
	u32 num_log, max_log = 64;
	int ret = 0;

	num_log = readl(reg + V2_ERR_LOG_STAT);

	while (num_log) {
		data = itmon_v2_collect_errlog(itmon, group);
		if (!data)
			return -EPERM;
		writel(1, reg + V2_ERR_LOG_POP);
		num_log = readl(group->regs + V2_ERR_LOG_STAT);
		ret++;

		switch (data->err_code) {
		case V2_ERR_TMOUT:
		/* Timeout */
		break;
		case V2_ERR_PRTCHK_ARID_RID ... V2_ERR_PRTCHK_AWLEN_WLAST_NL:
		/* Protocol Checker */
		break;
		}
		max_log--;

		if (max_log == 0)
			break;
	};

	if (clear)
		writel(1, reg + V2_ERR_LOG_CLR);

	return ret;
}

static int itmon_v2_search_errlog(struct itmon_v2_dev *itmon,
			     struct itmon_v2_nodegroup *group,
			     bool clear)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	int i, ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&itmon->ctrl_lock, flags);

	if (group) {
		ret = itmon_v2_pop_errlog(itmon, group, clear);
		itmon_v2_report_timeout(itmon, group);
	} else {
		/* Processing all group & nodes */
		for (i = 0; i < pdata->num_nodegroup; i++) {
			group = &pdata->nodegroup[i];
			if (group->pd_support && !group->pd_status)
				continue;
			ret += itmon_v2_pop_errlog(itmon, group, clear);
			itmon_v2_report_timeout(itmon, group);
		}
	}
	if (ret)
		itmon_v2_analyze_errlog(itmon);

	spin_unlock_irqrestore(&itmon->ctrl_lock, flags);
	return ret;
}

static irqreturn_t itmon_v2_irq_handler(int irq, void *data)
{
	struct itmon_v2_dev *itmon = (struct itmon_v2_dev *)data;
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct itmon_v2_nodegroup *group = NULL;
	bool ret;
	int i;

	/* Search itmon group */
	for (i = 0; i < (int)pdata->num_nodegroup; i++) {
		group = &pdata->nodegroup[i];
		if (!group->pd_support) {
			dev_err(itmon->dev, "%d irq, %s group, pd %s, %x errors",
				irq, group->name, "on",
				readl(group->regs + V2_ERR_LOG_STAT));
		} else {
			dev_err(itmon->dev, "%d irq, %s group, pd %s, %x errors",
				irq, group->name,
				group->pd_status ? "on" : "off",
				group->pd_status ? readl(group->regs + V2_ERR_LOG_STAT) : 0);
		}
	}

	ret = itmon_v2_search_errlog(itmon, NULL, true);
	if (!ret)
		dev_err(itmon->dev, "No errors found %s\n", __func__);
	else
		dev_err(itmon->dev, "Error detected %s, %d\n", __func__, ret);

	itmon_v2_post_handler(itmon, ret);

	return IRQ_HANDLED;
}

static int itmon_v2_logging_panic_handler(struct notifier_block *nb,
				     unsigned long l, void *buf)
{
	struct itmon_v2_panic_block *itmon_v2_panic = (struct itmon_v2_panic_block *)nb;
	struct itmon_v2_dev *itmon = itmon_v2_panic->pdev;
	int ret;

	if (!IS_ERR_OR_NULL(itmon)) {
		/* Check error has been logged */
		ret = itmon_v2_search_errlog(itmon, NULL, true);
		if (!ret)
			dev_info(itmon->dev, "No errors found %s\n", __func__);
		else
			dev_err(itmon->dev, "Error detected %s, %d\n", __func__, ret);
	}
	return 0;
}

static int itmon_v2_load_keepdata(struct itmon_v2_dev *itmon)
{
	struct itmon_v2_platdata *pdata = itmon->pdata;
	struct reserved_mem *rmem;
	struct device_node *rmem_np;
	struct device *dev = itmon->dev;
	struct itmon_v2_keepdata *kdata;
	pgprot_t prot = __pgprot(PROT_NORMAL);
	int page_size, i;
	struct page *page, **pages;
	unsigned long flags = VM_NO_GUARD | VM_MAP;
	void *vaddr;

	rmem_np = of_parse_phandle(dev->of_node, "memory-region", 0);
	if (!rmem_np) {
		dev_warn(dev, "there is no phandle for memory-region\n");
		return -EINVAL;
	}

	rmem = of_reserved_mem_lookup(rmem_np);
	if (!rmem) {
		dev_err(dev, "failed to acquire memory region\n");
		return -ENOMEM;
	}

	page_size = rmem->size / PAGE_SIZE;
	pages = kzalloc(sizeof(struct page *) * page_size, GFP_KERNEL);
	page = phys_to_page(rmem->base);

	for (i = 0; i < page_size; i++)
		pages[i] = page++;

	vaddr = vmap(pages, page_size, flags, prot);
	kfree(pages);
	if (!vaddr) {
		dev_err(dev, "paddr:%llx page_size:%x failed to vmap\n",
				rmem->base, page_size);
		return -ENOMEM;
	}

	kdata = (struct itmon_v2_keepdata *)(vaddr);

	if (kdata->magic != 0xBEEFBEEF) {
		dev_err(dev, "Invalid Keep data of ITMON\n");
		return -EFAULT;
	}

	if (kdata->num_nodegroup > V2_MAX_NODEGROUP) {
		dev_err(dev, "%d is invalid num_nodegroup\n", kdata->num_nodegroup);
		return -EINVAL;
	}

	dev_info(dev, "reserved - base: 0x%llx, magic: 0x%x, rpathinfo: 0x%llx,"
		      "masterinfo: 0x%llx, nodegroup: 0x%llx\n",
			(u64)kdata->base,
			kdata->magic,
			kdata->mem_rpathinfo,
			kdata->mem_masterinfo,
			kdata->mem_nodegroup);

	pdata->rpathinfo = (struct itmon_v2_rpathinfo *)
		((u64)vaddr + (u64)(kdata->mem_rpathinfo - kdata->base));
	pdata->num_rpathinfo = kdata->num_rpathinfo;

	pdata->masterinfo = (struct itmon_v2_masterinfo *)
		((u64)vaddr + (u64)(kdata->mem_masterinfo - kdata->base));
	pdata->num_masterinfo = kdata->num_masterinfo;

	pdata->nodegroup = (struct itmon_v2_nodegroup *)
		((u64)vaddr + (u64)(kdata->mem_nodegroup - kdata->base));
	pdata->num_nodegroup = kdata->num_nodegroup;

	for (i = 0; i < (int)pdata->num_nodegroup; i++) {
		u64 offset = (((u64)(pdata->nodegroup[i].nodeinfo_phy))
						- (u64)(kdata->base));
		pdata->nodegroup[i].nodeinfo =
			(struct itmon_v2_nodeinfo *)((u64)vaddr + (u64)offset);
	}

	return 0;
}

static void itmon_v2_parse_dt(struct itmon_v2_dev *itmon)
{
	struct device_node *np = itmon->dev->of_node;
	struct device_node *child_np;
	struct itmon_v2_platdata *pdata = itmon->pdata;

	if (of_property_read_bool(np, "no-def-en")) {
		dev_info(itmon->dev, "No default enable support\n");
		pdata->def_en = false;
	} else {
		pdata->def_en = true;
	}

	if (of_property_read_bool(np, "support-irq-oring")) {
		dev_info(itmon->dev, "only 1 irq support\n");
		pdata->irq_oring = true;
	} else {
		pdata->irq_oring = false;
	}

	if (of_property_read_bool(np, "no-support-cpu-parsing")) {
		dev_info(itmon->dev, "CPU Parser is not support\n");
		pdata->cpu_parsing = false;
	} else {
		pdata->cpu_parsing = true;
	}

	child_np = of_get_child_by_name(np, "dbgc");
	if (!child_np) {
		dev_info(itmon->dev, "dbgc is not found\n");
		return;
	}
}

static void itmon_v2_set_irq_affinity(struct itmon_v2_dev *itmon,
				   unsigned int irq, unsigned long affinity)
{
	struct cpumask affinity_mask;
	unsigned long bit;
	char *buf = (char *)__get_free_page(GFP_KERNEL);

	cpumask_clear(&affinity_mask);

	if (affinity) {
		for_each_set_bit(bit, &affinity, nr_cpu_ids)
			cpumask_set_cpu(bit, &affinity_mask);
	} else {
		cpumask_copy(&affinity_mask, cpu_online_mask);
	}
	irq_set_affinity_hint(irq, &affinity_mask);

	cpumap_print_to_pagebuf(true, buf, &affinity_mask);
	dev_dbg(itmon->dev, "affinity of irq%d is %s", irq, buf);
	free_page((unsigned long)buf);
}

static bool itmon_v2_get_pd_status(const char *pd_name)
{
	struct exynos_pm_domain *pd = exynos_pd_lookup_name(pd_name);
	int pd_status;

	if (!pd) {
		pr_err("%s) %s is not exist\n", __func__, pd_name);
		return false;
	}

	pd_status = exynos_pd_status(pd);
	if (pd_status <= 0)
		return false;

	return true;
}

static int itmon_v2_probe(struct platform_device *pdev, void *param)
{
	struct itmon_v2_info_table *info = param;
	struct itmon_v2_dev *itmon;
	struct itmon_v2_panic_block *itmon_v2_panic = NULL;
	struct itmon_v2_platdata *pdata;
	struct itmon_v2_nodeinfo *node;
	unsigned int irq, val = 0;
	char *dev_name;
	int ret, i, j;

	if (!info)
		return -EINVAL;

	itmon = devm_kzalloc(&pdev->dev,
				sizeof(struct itmon_v2_dev), GFP_KERNEL);
	if (!itmon)
		return -ENOMEM;

	itmon->dev = &pdev->dev;

	spin_lock_init(&itmon->ctrl_lock);

	pdata = devm_kzalloc(&pdev->dev,
				sizeof(struct itmon_v2_platdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	itmon->pdata = pdata;

	if (itmon_v2_load_keepdata(itmon) < 0) {
		pdata->masterinfo = info->masterinfo;
		pdata->num_masterinfo = info->num_masterinfo;

		pdata->nodegroup = info->nodegroup;
		pdata->num_nodegroup = info->num_nodegroup;

		pdata->rpathinfo = info->rpathinfo;
		pdata->num_rpathinfo = info->num_rpathinfo;

		dev_info(&pdev->dev,
			 "registers itmon-v2 information from platform data\n");
	}

	itmon_v2_parse_dt(itmon);

	for (i = 0; i < (int)pdata->num_nodegroup; i++) {
		dev_name = pdata->nodegroup[i].name;
		node = pdata->nodegroup[i].nodeinfo;

		if (pdata->nodegroup[i].phy_regs) {
			pdata->nodegroup[i].regs =
				devm_ioremap(&pdev->dev,
						 pdata->nodegroup[i].phy_regs, SZ_16K);
			if (pdata->nodegroup[i].regs == NULL) {
				dev_err(&pdev->dev,
					"failed to claim register region - %s\n",
					dev_name);
				return -ENOENT;
			}
		}

		for (j = 0; j < pdata->nodegroup[i].nodesize; j++)
			node[j].group = &pdata->nodegroup[i];

		irq = irq_of_parse_and_map(pdev->dev.of_node, i);
		pdata->nodegroup[i].irq = V2_SET_IRQ(irq);
		if (!irq)
			continue;

		ret = of_property_read_u32_index(pdev->dev.of_node, "interrupt-affinity", i, &val);
		if (ret) {
			dev_err(&pdev->dev, "failed to get property interrupt-affinity(%d)\n", i);
			return -EINVAL;
		}
		itmon_v2_set_irq_affinity(itmon, irq, val);

		pdata->nodegroup[i].irq |= V2_SET_AFFINITY(val);

		if (pdata->nodegroup[i].pd_support) {
			pdata->nodegroup[i].pd_status =
				itmon_v2_get_pd_status(pdata->nodegroup[i].pd_name);

			dev_dbg(&pdev->dev,
				 "%s, pd_name: %s, status:%s\n",
				 pdata->nodegroup[i].name,
				 pdata->nodegroup[i].pd_name,
				 pdata->nodegroup[i].pd_status ? "on" : "off");
		}

		irq_set_status_flags(irq, IRQ_NOAUTOEN);
		disable_irq(irq);
		ret = devm_request_irq(&pdev->dev, irq,
				       itmon_v2_irq_handler,
				       IRQF_NOBALANCING, dev_name, itmon);
		if (ret == 0) {
			dev_info(&pdev->dev,
				 "success to register request irq%u - %s\n",
				 irq, dev_name);
		} else {
			dev_err(&pdev->dev, "failed to request irq - %s\n",
				dev_name);
			return -ENOENT;
		}
	}
	itmon_v2_panic = devm_kzalloc(&pdev->dev, sizeof(struct itmon_v2_panic_block),
				 GFP_KERNEL);

	if (!itmon_v2_panic) {
		dev_err(&pdev->dev, "failed to allocate memory for driver's panic handler data\n");
	} else {
		itmon_v2_panic->nb_panic_block.notifier_call = itmon_v2_logging_panic_handler;
		itmon_v2_panic->pdev = itmon;
		atomic_notifier_chain_register(&panic_notifier_list,
					       &itmon_v2_panic->nb_panic_block);
	}

	platform_set_drvdata(pdev, itmon);

	itmon_v2_init(itmon, true);

	pdata->last_time = 0;
	pdata->last_errcnt = 0;

	g_itmon_v2 = itmon;

	for (i = 0; i < V2_TRANS_TYPE_NUM; i++) {
		INIT_LIST_HEAD(&pdata->datalist[i]);
		INIT_LIST_HEAD(&pdata->infolist[i]);
	}

	pdata->probed = true;
	dev_info(&pdev->dev, "success to probe Exynos ITMON-v2 driver\n");

	for (i = 0; i < (int)pdata->num_nodegroup; i++)
		if (pdata->nodegroup[i].irq)
			enable_irq(V2_GET_IRQ(pdata->nodegroup[i].irq));

	return 0;
}

static int itmon_v2_remove(struct platform_device *pdev)
{
	platform_set_drvdata(pdev, NULL);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int itmon_v2_suspend(struct device *dev)
{
	dev_info(dev, "%s is called\n", __func__);
	return 0;
}

static int itmon_v2_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct itmon_v2_dev *itmon = platform_get_drvdata(pdev);
	struct itmon_v2_platdata *pdata = itmon->pdata;
	int i;

	for (i = 0; i < (int)pdata->num_nodegroup; i++) {
		unsigned int irq = pdata->nodegroup[i].irq;

		if (irq)
			itmon_v2_set_irq_affinity(itmon, V2_GET_IRQ(irq), V2_GET_AFFINITY(irq));
	}
	itmon_v2_init(itmon, true);

	dev_info(dev, "%s is called\n", __func__);
	return 0;
}
#endif

struct itmon_platform_ops itmon_v2_ops = {
	.probe = itmon_v2_probe,
	.remove = itmon_v2_remove,
	.suspend = itmon_v2_suspend,
	.resume = itmon_v2_resume,
	.enable = itmon_v2_en,
	.pd_sync = itmon_v2_pd_sync,
};
