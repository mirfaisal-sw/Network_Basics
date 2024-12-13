/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#ifndef __FVMAP_H__
#define __FVMAP_H__

/* FV(Frequency Voltage MAP) */
struct fvmap_header_v9 {
	unsigned char dvfs_type;
	unsigned char num_of_lv;
	unsigned char num_of_members;
	unsigned char num_of_pll;
	unsigned char num_of_mux;
	unsigned char num_of_div;
	unsigned short gearratio;
	unsigned char init_lv;
	unsigned char num_of_gate;
	unsigned char reserved[2];
	unsigned short block_addr[3];
	unsigned short o_members;
	unsigned short o_ratevolt;
	unsigned short o_clocks;
};

struct fvmap_header_v920 {
	unsigned char domain_id;
	unsigned char num_of_lv;
	unsigned char num_of_members;
	unsigned char num_of_pll;
	unsigned char num_of_mux;
	unsigned char num_of_div;
	unsigned short o_famrate;
	unsigned char init_lv;
	unsigned char num_of_child;
	unsigned char parent_id;
	unsigned char parent_offset;
	unsigned short block_addr[3];
	unsigned short o_members;
	unsigned short o_ratevolt;
	unsigned short o_clocks;

	unsigned int init_rate;
	unsigned char table_ver;
	unsigned char asv_grp;
	unsigned char asv_modi_volt;
	unsigned char asv_sub_lv;
	unsigned char dfs_en:4;
	unsigned char cs_en:4;
	unsigned char fg_en:4;
	unsigned char asvg_en:4;
	unsigned short o_ratevolt_cs;

	unsigned char child_id[12];
	unsigned char copy_col;
};

struct clocks {
	unsigned short addr[0];
};

struct pll_header {
	unsigned int addr;
	unsigned short o_lock;
	unsigned short level;
	unsigned int pms[0];
};

struct rate_volt_v9 {
	unsigned int rate;
	unsigned int volt;
};

struct rate_volt_v920 {
	unsigned int rate:24;
	unsigned int volt:8;
};

struct rate_volt_header_v9 {
	struct rate_volt_v9 table[0];
};

struct rate_volt_header_v920 {
	struct rate_volt_v920 table[0];
};
struct dvfs_table {
	unsigned char val[0];
};

#if IS_ENABLED(CONFIG_ACPM_DVFS)
extern int fvmap_init(void __iomem *sram_base);
extern int fvmap_get_voltage_table(unsigned int id, unsigned int *table);
#else
static inline int fvmap_init(void __iomem *sram_base)
{
	return 0;
}

static inline int fvmap_get_voltage_table(unsigned int id, unsigned int *table)
{
	return 0;
}
#endif
#endif
