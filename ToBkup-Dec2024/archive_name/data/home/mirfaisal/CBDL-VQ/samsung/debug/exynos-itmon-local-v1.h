/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * IPs Traffic Monitor(ITMON)-v1 Header for Samsung Exynos SOC
 */

#ifndef _EXYNOS_ITMON_V1_H_
#define _EXYNOS_ITMON_V1_H_

#include <linux/platform_device.h>
#include <linux/panic_notifier.h>
#include <soc/samsung/exynos-itmon.h>

#define V1_OFFSET_TMOUT_REG		(0x2000)
#define V1_OFFSET_REQ_R			(0x0)
#define V1_OFFSET_REQ_W			(0x20)
#define V1_OFFSET_RESP_R		(0x40)
#define V1_OFFSET_RESP_W		(0x60)
#define V1_OFFSET_ERR_REPT		(0x20)
#define V1_OFFSET_HW_ASSERT		(0x100)
#define V1_OFFSET_NUM			(0x4)

#define V1_REG_INT_MASK			(0x0)
#define V1_REG_INT_CLR			(0x4)
#define V1_REG_INT_INFO			(0x8)
#define V1_REG_EXT_INFO_0		(0x10)
#define V1_REG_EXT_INFO_1		(0x14)
#define V1_REG_EXT_INFO_2		(0x18)

#define V1_REG_DBG_CTL			(0x10)
#define V1_REG_TMOUT_INIT_VAL		(0x14)
#define V1_REG_TMOUT_FRZ_EN		(0x18)
#define V1_REG_TMOUT_BUF_WR_OFFSET	(0x20)

#define V1_REG_TMOUT_BUF_STATUS		(0x1C)
#define V1_REG_TMOUT_BUF_POINT_ADDR	(0x20)
#define V1_REG_TMOUT_BUF_ID		(0x24)
#define V1_REG_TMOUT_BUF_PAYLOAD	(0x28)
#define V1_REG_TMOUT_BUF_PAYLOAD_SRAM1	(0x30)
#define V1_REG_TMOUT_BUF_PAYLOAD_SRAM2	(0x34)
#define V1_REG_TMOUT_BUF_PAYLOAD_SRAM3	(0x38)

#define V1_REG_HWA_CTL			(0x4)
#define V1_REG_HWA_INT			(0x8)
#define V1_REG_HWA_INT_ID		(0xC)
#define V1_REG_HWA_START_ADDR_LOW	(0x10)
#define V1_REG_HWA_END_ADDR_LOW		(0x14)
#define V1_REG_HWA_START_END_ADDR_UPPER	(0x18)

#define V1_RD_RESP_INT_ENABLE		(1 << 0)
#define V1_WR_RESP_INT_ENABLE		(1 << 1)
#define V1_ARLEN_RLAST_INT_ENABLE	(1 << 2)
#define V1_AWLEN_WLAST_INT_ENABLE	(1 << 3)
#define V1_INTEND_ACCESS_INT_ENABLE	(1 << 4)

#define V1_BIT_HWA_ERR_OCCURRED(x)	(((x) & (0x1 << 0)) >> 0)
#define V1_BIT_HWA_ERR_CODE(x)		(((x) & (0xF << 1)) >> 28)

#define V1_BIT_ERR_CODE(x)		(((x) & (0xF << 28)) >> 28)
#define V1_BIT_ERR_OCCURRED(x)		(((x) & (0x1 << 27)) >> 27)
#define V1_BIT_ERR_VALID(x)		(((x) & (0x1 << 26)) >> 26)
#define V1_BIT_AXID(x)			(((x) & (0xFFFF)))
#define V1_BIT_AXUSER(x)		(((x) & (0xFFFF << 16)) >> 16)
#define V1_BIT_AXBURST(x)		(((x) & (0x3)))
#define V1_BIT_AXPROT(x)		(((x) & (0x3 << 2)) >> 2)
#define V1_BIT_AXLEN(x)			(((x) & (0xF << 16)) >> 16)
#define V1_BIT_AXSIZE(x)		(((x) & (0x7 << 28)) >> 28)

#define V1_M_NODE			(0)
#define V1_T_S_NODE			(1)
#define V1_T_M_NODE			(2)
#define V1_S_NODE			(3)
#define V1_NODE_TYPE			(4)

#define V1_ERRCODE_SLVERR		(0)
#define V1_ERRCODE_DECERR		(1)
#define V1_ERRCODE_UNSUPPORTED		(2)
#define V1_ERRCODE_POWER_DOWN		(3)
#define V1_ERRCODE_UNKNOWN_4		(4)
#define V1_ERRCODE_UNKNOWN_5		(5)
#define V1_ERRCODE_TMOUT		(6)

#define V1_BUS_DATA			(0)
#define V1_BUS_PERI			(1)
#define V1_BUS_PATH_TYPE		(2)

#define V1_TRANS_TYPE_WRITE		(0)
#define V1_TRANS_TYPE_READ		(1)
#define V1_TRANS_TYPE_NUM		(2)

#define V1_FROM_CP			(0)
#define V1_FROM_CPU			(2)
#define V1_FROM_M_NODE			(4)

#define V1_CP_COMMON_STR		"CP_"
#define V1_DREX_COMMON_STR		"DREX_IRPS"

#define V1_TMOUT			(0xFFFFF)
#define V1_TMOUT_TEST			(0x1)

#define V1_PANIC_ALLOWED_THRESHOLD	(0x2)
#define V1_INVALID_REMAPPING		(0x03000000)

/* This value will be fixed */
#define V1_INTEND_ADDR_START		(0)
#define V1_INTEND_ADDR_END		(0)

struct itmon_v1_rpathinfo {
	unsigned int id;
	char *port_name;
	char *dest_name;
	unsigned int bits;
	unsigned int shift_bits;
};

struct itmon_v1_masterinfo {
	char *port_name;
	unsigned int user;
	char *master_name;
	unsigned int bits;
};

struct itmon_v1_nodegroup;

struct itmon_v1_traceinfo {
	char *port;
	char *master;
	char *dest;
	unsigned long target_addr;
	unsigned int errcode;
	bool read;
	bool onoff;
	char *pd_name;
	bool path_dirty;
	bool snode_dirty;
	bool dirty;
	unsigned long from;
	int path_type;
	char buf[SZ_32];
};

struct itmon_v1_tracedata {
	unsigned int int_info;
	unsigned int ext_info_0;
	unsigned int ext_info_1;
	unsigned int ext_info_2;
	unsigned int dbg_mo_cnt;
	unsigned int hwa_ctl;
	unsigned int hwa_info;
	unsigned int hwa_int_id;
	unsigned int offset;
	bool logging;
	bool read;
};

struct itmon_v1_nodeinfo {
	unsigned int type;
	char *name;
	unsigned int phy_regs;
	void __iomem *regs;
	unsigned int time_val;
	bool tmout_enabled;
	bool tmout_frz_enabled;
	bool err_enabled;
	bool hw_assert_enabled;
	bool addr_detect_enabled;
	char *pd_name;
	bool retention;
	struct itmon_v1_tracedata tracedata;
	struct itmon_v1_nodegroup *group;
	struct list_head list;
};

struct itmon_v1_nodegroup {
	int irq;
	char *name;
	unsigned int phy_regs;
	bool ex_table;
	void __iomem *regs;
	struct itmon_v1_nodeinfo *nodeinfo;
	unsigned int nodesize;
	unsigned int bus_type;
};

struct itmon_v1_platdata {
	const struct itmon_v1_rpathinfo *rpathinfo;
	const struct itmon_v1_masterinfo *masterinfo;
	struct itmon_v1_nodegroup *nodegroup;
	int num_rpathinfo;
	int num_masterinfo;
	int num_nodegroup;
	struct itmon_v1_traceinfo traceinfo[V1_TRANS_TYPE_NUM];
	struct list_head tracelist[V1_TRANS_TYPE_NUM];
	unsigned int err_cnt;
	unsigned int err_cnt_by_cpu;
	ktime_t last_time;
	bool panic_allowed;
	bool crash_in_progress;
	unsigned int sysfs_tmout_val;
	bool sysfs_scandump;
	bool sysfs_s2d;
	bool err_fatal;
	bool err_hwa;
	bool probed;
};

struct itmon_v1_dev {
	struct device *dev;
	struct itmon_v1_platdata *pdata;
	const struct of_device_id *match;
	int irq;
	int id;
	void __iomem *regs;
	spinlock_t ctrl_lock;
	struct itmon_notifier notifier_info;
};

struct itmon_v1_panic_block {
	struct notifier_block nb_panic_block;
	struct itmon_v1_dev *pdev;
};

struct itmon_v1_info_table {
	const struct itmon_v1_rpathinfo *rpathinfo;
	const struct itmon_v1_masterinfo *masterinfo;
	struct itmon_v1_nodegroup *nodegroup;
	int num_rpathinfo;
	int num_masterinfo;
	int num_nodegroup;
};

extern struct itmon_platform_ops itmon_v1_ops;

#endif /*_EXYNOS_ITMON_V1_H_*/
