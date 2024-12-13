// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/libfdt.h>

#include <soc/samsung/debug-snapshot.h>
#include "debug-snapshot-local.h"

const char *dpm_policy[] = {
	[GO_DEFAULT_ID]		= GO_DEFAULT,
	[GO_PANIC_ID]		= GO_PANIC,
	[GO_WATCHDOG_ID]	= GO_WATCHDOG,
	[GO_S2D_ID]		= GO_S2D,
	[GO_ARRAYDUMP_ID]	= GO_ARRAYDUMP,
	[GO_SCANDUMP_ID]	= GO_SCANDUMP,
};

struct dbg_snapshot_dpm dss_dpm;
EXPORT_SYMBOL(dss_dpm);

bool dbg_snapshot_get_dpm_status(void)
{
	return dss_dpm.enabled;
}
EXPORT_SYMBOL(dbg_snapshot_get_dpm_status);

static void dbg_snapshot_dt_scan_dpm_feature(struct device_node *node)
{
	struct device_node *item;
	unsigned int val;
	struct property *prop;
	const char *method;

	dss_dpm.enabled_debug = false;
	dss_dpm.dump_mode = NONE_DUMP;

	item = of_find_node_by_name(node, "dump-mode");
	if (!item) {
		pr_info("dpm: No such ramdump node, [dump-mode] disabled\n");
		goto exit_dss;
	}

	if (of_property_read_u32(item, "enabled", &val)) {
		pr_info("dpm: No such enabled of dump-mode, [dump-mode] disabled\n");
	} else {
		if (val == FULL_DUMP) {
			pr_info("dpm: dump-mode is Full Dump\n");
			dss_dpm.enabled_debug = true;
			dss_dpm.dump_mode = val;
		} else {
			goto exit_dss;
		}
	}

	item = of_find_node_by_name(node, "event");
	if (!item) {
		pr_warn("dpm: No such methods of kernel event\n");
		goto exit_dss;
	}

	of_property_for_each_string(item, "method", prop, method) {
		if (!method) {
			pr_warn("dpm: No such methods of kevents\n");
			goto exit_dss;
		}

		dbg_snapshot_set_enable_log_item(method, true);
	}

exit_dss:
	return;
}

int dbg_snapshot_dt_scan_dpm(void)
{
	struct device_node *root, *next;
	unsigned int val;

	memset((struct dbg_snapshot_dpm *)&dss_dpm, 0,
			sizeof(struct dbg_snapshot_dpm));

	root = of_find_node_by_name(NULL, "dpm");
	if (!root) {
		pr_err("dpm: no such dpm node!\n");
		return -ENODEV;
	}

	dss_dpm.enabled = true;

	/* version */
	if (!of_property_read_u32(root, "version", &val)) {
		dss_dpm.version = val;
		pr_info("dpm: v%01d.%02d\n", val / 100, val % 100);
	} else {
		pr_info("dpm: version is not found\n");
	}

	/* feature setting */
	next = of_find_node_by_name(root, DPM_F);
	if (!next) {
		pr_warn("dpm: No such features of debug policy\n");
	} else {
		pr_warn("dpm: found features of debug policy\n");
		dbg_snapshot_dt_scan_dpm_feature(next);
	}

	return 0;
}

void dbg_snapshot_init_dpm(void)
{
	int val;

	if (dss_dpm.enabled_debug)
		dbg_snapshot_scratch_reg(DSS_SIGN_SCRATCH);

	val = dbg_snapshot_get_dpm_none_dump_mode();
	if (val > 0)
		dss_dpm.dump_mode_none = 1;
}
