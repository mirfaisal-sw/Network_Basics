// SPDX-License-Identifier: GPL-2.0
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <soc/samsung/cal-if.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "fvmap.h"
#include "cmucal.h"
#include "vclk.h"
#include "ra.h"

#define FVMAP_SIZE		(SZ_8K)
#define STEP_UV			(6250)

static void __iomem *fvmap_base;
static void __iomem *sram_fvmap_base;

static int init_margin_table[MAX_MARGIN_ID];
static int percent_margin_table[MAX_MARGIN_ID];

static int margin_mif;
static int margin_int;
static int margin_cpucl0;
static int margin_cpucl1;
static int margin_cpucl2;
static int margin_npu;
static int margin_aud;
static int margin_g3d00;
static int margin_g3d01;
static int margin_g3d1;
static int margin_g3d;
static int margin_cam;
static int margin_sfi;
static int volt_offset_percent;

module_param(margin_mif, int, 0);
module_param(margin_int, int, 0);
module_param(margin_cpucl0, int, 0);
module_param(margin_cpucl1, int, 0);
module_param(margin_cpucl2, int, 0);
module_param(margin_npu, int, 0);
module_param(margin_aud, int, 0);
module_param(margin_g3d00, int, 0);
module_param(margin_g3d01, int, 0);
module_param(margin_g3d1, int, 0);
module_param(margin_g3d, int, 0);
module_param(margin_cam, int, 0);
module_param(margin_sfi, int, 0);
module_param(volt_offset_percent, int, 0);

void margin_table_init(void)
{
	init_margin_table[MARGIN_MIF] = margin_mif;
	init_margin_table[MARGIN_INT] = margin_int;
	init_margin_table[MARGIN_CPUCL0] = margin_cpucl0;
	init_margin_table[MARGIN_CPUCL1] = margin_cpucl1;
	init_margin_table[MARGIN_CPUCL2] = margin_cpucl2;
	init_margin_table[MARGIN_NPU] = margin_npu;
	init_margin_table[MARGIN_AUD] = margin_aud;
	init_margin_table[MARGIN_G3D00] = margin_g3d00;
	init_margin_table[MARGIN_G3D01] = margin_g3d01;
	init_margin_table[MARGIN_G3D1] = margin_g3d1;
	init_margin_table[MARGIN_G3D] = margin_g3d;
	init_margin_table[MARGIN_CAM] = margin_cam;
	init_margin_table[MARGIN_SFI] = margin_sfi;
}

int fvmap_set_raw_voltage_table(unsigned int id, int uV)
{
	struct fvmap_header_v9 *fvmap_header_v9;
	struct fvmap_header_v920 *fvmap_header_v920;
	struct rate_volt_header_v9 *fv_table_v9;
	struct rate_volt_header_v920 *fv_table_v920;
	int num_of_lv;
	int idx, i;

	idx = GET_IDX(id);

	if (cal_variant == EXYNOSAUTO_V9) {
		fvmap_header_v9 = sram_fvmap_base;
		fv_table_v9 = sram_fvmap_base + fvmap_header_v9[idx].o_ratevolt;
		num_of_lv = fvmap_header_v9[idx].num_of_lv;

		if (num_of_lv < 0 || num_of_lv > TBL_SIZE) {
			pr_err("[fvmap] failed to parsing fvmap data.(num_of_lv = %d)\n", num_of_lv);
			return -1;
		}

		for (i = 0; i < num_of_lv; i++)
			fv_table_v9->table[i].volt += uV;
	} else if ((cal_variant == EXYNOSAUTO_V920_EVT0) || (cal_variant == EXYNOSAUTO_V920_EVT2) || (cal_variant == EXYNOSAUTO_V620_EVT0)) {
		fvmap_header_v920 = sram_fvmap_base;
		fv_table_v920 = sram_fvmap_base + fvmap_header_v920[idx].o_ratevolt;
		num_of_lv = fvmap_header_v920[idx].num_of_lv;

		if (num_of_lv < 0 || num_of_lv > TBL_SIZE) {
			pr_err("[fvmap] failed to parsing fvmap data.(num_of_lv = %d)\n", num_of_lv);
			return -1;
		}

		for (i = 0; i < num_of_lv; i++)
			fv_table_v920->table[i].volt += uV / STEP_UV;
	}

	return 0;
}

int fvmap_get_voltage_table(unsigned int id, unsigned int *table)
{
	struct fvmap_header_v9 *fvmap_header_v9;
	struct fvmap_header_v920 *fvmap_header_v920;
	struct rate_volt_header_v9 *fv_table_v9;
	struct rate_volt_header_v920 *fv_table_v920;
	int idx, i;
	int num_of_lv = -1;

	if (!IS_ACPM_VCLK(id))
		return 0;

	idx = GET_IDX(id);

	if (cal_variant == EXYNOSAUTO_V9) {
		fvmap_header_v9 = fvmap_base;
		fv_table_v9 = fvmap_base + fvmap_header_v9[idx].o_ratevolt;
		num_of_lv = fvmap_header_v9[idx].num_of_lv;

		if (num_of_lv < 0 || num_of_lv > TBL_SIZE) {
			pr_err("[fvmap] failed to parsing fvmap data.(num_of_lv = %d)\n", num_of_lv);
			return -1;
		}

		for (i = 0; i < num_of_lv; i++)
			table[i] = fv_table_v9->table[i].volt;
	} else if ((cal_variant == EXYNOSAUTO_V920_EVT0) || (cal_variant == EXYNOSAUTO_V920_EVT2) || (cal_variant == EXYNOSAUTO_V620_EVT0)) {
		fvmap_header_v920 = fvmap_base;
		fv_table_v920 = fvmap_base + fvmap_header_v920[idx].o_ratevolt;
		num_of_lv = fvmap_header_v920[idx].num_of_lv;

		if (num_of_lv < 0 || num_of_lv > TBL_SIZE) {
			pr_err("[fvmap] failed to parsing fvmap data.(num_of_lv = %d)\n", num_of_lv);
			return -1;
		}

		for (i = 0; i < num_of_lv; i++)
			table[i] = fv_table_v920->table[i].volt * STEP_UV;
	}

	return num_of_lv;

}
EXPORT_SYMBOL_GPL(fvmap_get_voltage_table);

int fvmap_get_raw_voltage_table(unsigned int id)
{
	struct fvmap_header_v9 *fvmap_header_v9;
	struct fvmap_header_v920 *fvmap_header_v920;
	struct rate_volt_header_v9 *fv_table_v9;
	struct rate_volt_header_v920 *fv_table_v920;
	int idx, i;
	int num_of_lv;
	unsigned int table[TBL_SIZE];

	idx = GET_IDX(id);

	if (cal_variant == EXYNOSAUTO_V9) {
		fvmap_header_v9 = sram_fvmap_base;
		fv_table_v9 = sram_fvmap_base + fvmap_header_v9[idx].o_ratevolt;
		num_of_lv = fvmap_header_v9[idx].num_of_lv;

		if (num_of_lv < 0 || num_of_lv > TBL_SIZE) {
			pr_err("[fvmap] failed to parsing fvmap data.(num_of_lv = %d)\n", num_of_lv);
			return -1;
		}

		for (i = 0; i < num_of_lv; i++)
			table[i] = fv_table_v9->table[i].volt;

		for (i = 0; i < num_of_lv; i++)
			printk("dvfs id : %d  %d Khz : %d uv\n", ACPM_VCLK_TYPE | id, fv_table_v9->table[i].rate, table[i]);
	} else if ((cal_variant == EXYNOSAUTO_V920_EVT0) || (cal_variant == EXYNOSAUTO_V920_EVT2) || (cal_variant == EXYNOSAUTO_V620_EVT0)) {
		fvmap_header_v920 = sram_fvmap_base;
		fv_table_v920 = sram_fvmap_base + fvmap_header_v920[idx].o_ratevolt;
		num_of_lv = fvmap_header_v920[idx].num_of_lv;

		if (num_of_lv < 0 || num_of_lv > TBL_SIZE) {
			pr_err("[fvmap] failed to parsing fvmap data.(num_of_lv = %d)\n", num_of_lv);
			return -1;
		}

		for (i = 0; i < num_of_lv; i++)
			table[i] = fv_table_v920->table[i].volt * STEP_UV;

		for (i = 0; i < num_of_lv; i++)
			printk("dvfs id : %d  %d Khz : %d uv\n", ACPM_VCLK_TYPE | id, fv_table_v920->table[i].rate, table[i]);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(fvmap_get_raw_voltage_table);

static void check_percent_margin(struct rate_volt_header_v9 *head, unsigned int num_of_lv)
{
	int org_volt;
	int percent_volt;
	int i;

	if (!volt_offset_percent)
		return;

	for (i = 0; i < num_of_lv; i++) {
		org_volt = head->table[i].volt;
		percent_volt = org_volt * volt_offset_percent / 100;
		head->table[i].volt = org_volt + rounddown(percent_volt, STEP_UV);
	}
}

static int get_vclk_id_from_margin_id(int margin_id)
{
	int size = cmucal_get_list_size(ACPM_VCLK_TYPE);
	int i;
	struct vclk *vclk;

	for (i = 0; i < size; i++) {
		vclk = cmucal_get_node(ACPM_VCLK_TYPE | i);
		if (!vclk)
			continue;

		if (vclk->margin_id == margin_id)
			return i;
	}

	return -EINVAL;
}

#define attr_percent(margin_id, type)								\
static ssize_t show_##type##_percent								\
(struct kobject *kobj, struct kobj_attribute *attr, char *buf)					\
{												\
	return snprintf(buf, PAGE_SIZE, "%d\n", percent_margin_table[margin_id]);		\
}												\
												\
static ssize_t store_##type##_percent								\
(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)		\
{												\
	int input, vclk_id;									\
												\
	if (!sscanf(buf, "%d", &input))								\
		return -EINVAL;									\
												\
	if (input < -100 || input > 100)							\
		return -EINVAL;									\
												\
	vclk_id = get_vclk_id_from_margin_id(margin_id);					\
	if (vclk_id == -EINVAL)									\
		return vclk_id;									\
	percent_margin_table[margin_id] = input;						\
	cal_dfs_set_volt_margin(vclk_id | ACPM_VCLK_TYPE, input);				\
												\
	return count;										\
}												\
												\
static struct kobj_attribute type##_percent =							\
__ATTR(type##_percent, 0600,									\
	show_##type##_percent, store_##type##_percent)

attr_percent(MARGIN_MIF, mif_margin);
attr_percent(MARGIN_INT, int_margin);
attr_percent(MARGIN_CPUCL0, cpucl0_margin);
attr_percent(MARGIN_CPUCL1, cpucl1_margin);
attr_percent(MARGIN_CPUCL2, cpucl2_margin);
attr_percent(MARGIN_NPU, npu_margin);
attr_percent(MARGIN_AUD, aud_margin);
attr_percent(MARGIN_G3D00, g3d00_margin);
attr_percent(MARGIN_G3D01, g3d01_margin);
attr_percent(MARGIN_G3D1, g3d1_margin);
attr_percent(MARGIN_G3D, g3d_margin);
attr_percent(MARGIN_CAM, cam_margin);
attr_percent(MARGIN_SFI, sfi_margin);

static struct attribute *percent_margin_attrs_v9[] = {
	&mif_margin_percent.attr,
	&int_margin_percent.attr,
	&cpucl0_margin_percent.attr,
	&cpucl1_margin_percent.attr,
	&npu_margin_percent.attr,
	&aud_margin_percent.attr,
	&g3d00_margin_percent.attr,
	&g3d01_margin_percent.attr,
	&g3d1_margin_percent.attr,
	&cam_margin_percent.attr,
	&sfi_margin_percent.attr,
	NULL,
};

static struct attribute *percent_margin_attrs_v920[] = {
	&mif_margin_percent.attr,
	&int_margin_percent.attr,
	&cpucl0_margin_percent.attr,
	&cpucl1_margin_percent.attr,
	&cpucl2_margin_percent.attr,
	&npu_margin_percent.attr,
	&aud_margin_percent.attr,
	&g3d_margin_percent.attr,
	&cam_margin_percent.attr,
	&sfi_margin_percent.attr,
	NULL,
};

static const struct attribute_group *percent_margin_group;

static const struct attribute_group percent_margin_group_v9 = {
	.attrs = percent_margin_attrs_v9,
};

static const struct attribute_group percent_margin_group_v920 = {
	.attrs = percent_margin_attrs_v920,
};

static void fvmap_copy_from_sram_v9(void __iomem *map_base, void __iomem *sram_base)
{
	struct fvmap_header_v9 *fvmap_header;
	volatile struct fvmap_header_v9 *header;
	struct rate_volt_header_v9 *old, *new;
	struct clocks *clks;
	struct pll_header *plls;
	struct vclk *vclk;
	unsigned int member_addr;
	unsigned int blk_idx;
	int size, margin;
	int i, j;

	fvmap_header = map_base;
	header = sram_base;

	size = cmucal_get_list_size(ACPM_VCLK_TYPE);

	for (i = 0; i < size; i++) {
		/* load fvmap info */
		fvmap_header[i].dvfs_type = header[i].dvfs_type;
		fvmap_header[i].num_of_lv = header[i].num_of_lv;
		fvmap_header[i].num_of_members = header[i].num_of_members;
		fvmap_header[i].num_of_pll = header[i].num_of_pll;
		fvmap_header[i].num_of_mux = header[i].num_of_mux;
		fvmap_header[i].num_of_div = header[i].num_of_div;
		fvmap_header[i].gearratio = header[i].gearratio;
		fvmap_header[i].init_lv = header[i].init_lv;
		fvmap_header[i].num_of_gate = header[i].num_of_gate;
		fvmap_header[i].reserved[0] = header[i].reserved[0];
		fvmap_header[i].reserved[1] = header[i].reserved[1];
		fvmap_header[i].block_addr[0] = header[i].block_addr[0];
		fvmap_header[i].block_addr[1] = header[i].block_addr[1];
		fvmap_header[i].block_addr[2] = header[i].block_addr[2];
		fvmap_header[i].o_members = header[i].o_members;
		fvmap_header[i].o_ratevolt = header[i].o_ratevolt;
		fvmap_header[i].o_clocks = header[i].o_clocks;

		vclk = cmucal_get_node(ACPM_VCLK_TYPE | i);
		if (vclk == NULL)
			continue;
		pr_info("dvfs_type : %s - id : %x\n",
			vclk->name, fvmap_header[i].dvfs_type);
		pr_info("  num_of_lv      : %d\n", fvmap_header[i].num_of_lv);
		pr_info("  num_of_members : %d\n", fvmap_header[i].num_of_members);

		old = sram_base + fvmap_header[i].o_ratevolt;
		new = map_base + fvmap_header[i].o_ratevolt;

		check_percent_margin(old, fvmap_header[i].num_of_lv);

		margin = init_margin_table[vclk->margin_id];
		if (margin)
			cal_dfs_set_volt_margin(i | ACPM_VCLK_TYPE, margin);

		for (j = 0; j < fvmap_header[i].num_of_members; j++) {
			clks = sram_base + fvmap_header[i].o_members;

			if (j < fvmap_header[i].num_of_pll) {
				plls = sram_base + clks->addr[j];
				member_addr = plls->addr - 0x90000000;
			} else {

				member_addr = (clks->addr[j] & ~0x3) & 0xffff;
				blk_idx = clks->addr[j] & 0x3;

				member_addr |= ((fvmap_header[i].block_addr[blk_idx]) << 16) - 0x90000000;
			}

			vclk->list[j] = cmucal_get_id_by_addr(member_addr);

			if (vclk->list[j] == INVALID_CLK_ID)
				pr_info("  Invalid addr :0x%x\n", member_addr);
			else
				pr_info("  DVFS CMU addr:0x%x\n", member_addr);
		}

		for (j = 0; j < fvmap_header[i].num_of_lv; j++) {
			new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) \n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
		}
	}
}

static void fvmap_copy_from_sram_v920(void __iomem *map_base, void __iomem *sram_base)
{
	struct fvmap_header_v920 *fvmap_header;
	volatile struct fvmap_header_v920 *header;
	struct rate_volt_header_v920 *old, *new;
	struct clocks *clks;
	struct pll_header *plls;
	struct vclk *vclk;
	unsigned int member_addr;
	unsigned int blk_idx;
	int size, margin;
	int i, j;

	fvmap_header = map_base;
	header = sram_base;

	size = cmucal_get_list_size(ACPM_VCLK_TYPE);

	for (i = 0; i < size; i++) {
		/* load fvmap info */
		fvmap_header[i].domain_id = header[i].domain_id;
		fvmap_header[i].num_of_lv = header[i].num_of_lv;
		fvmap_header[i].num_of_members = header[i].num_of_members;
		fvmap_header[i].num_of_pll = header[i].num_of_pll;
		fvmap_header[i].num_of_mux = header[i].num_of_mux;
		fvmap_header[i].num_of_div = header[i].num_of_div;
		fvmap_header[i].o_famrate = header[i].o_famrate;
		fvmap_header[i].init_lv = header[i].init_lv;
		fvmap_header[i].num_of_child = header[i].num_of_child;
		fvmap_header[i].parent_id = header[i].parent_id;
		fvmap_header[i].parent_offset = header[i].parent_offset;
		fvmap_header[i].block_addr[0] = header[i].block_addr[0];
		fvmap_header[i].block_addr[1] = header[i].block_addr[1];
		fvmap_header[i].block_addr[2] = header[i].block_addr[2];
		fvmap_header[i].o_members = header[i].o_members;
		fvmap_header[i].o_ratevolt = header[i].o_ratevolt;
		fvmap_header[i].o_clocks = header[i].o_clocks;

		fvmap_header[i].init_rate = header[i].init_rate;
		fvmap_header[i].table_ver = header[i].table_ver;
		fvmap_header[i].asv_grp = header[i].asv_grp;
		fvmap_header[i].asv_modi_volt = header[i].asv_modi_volt;
		fvmap_header[i].asv_sub_lv = header[i].asv_sub_lv;
		fvmap_header[i].dfs_en = header[i].dfs_en;
		fvmap_header[i].cs_en = header[i].cs_en;
		fvmap_header[i].fg_en = header[i].fg_en;
		fvmap_header[i].asvg_en = header[i].asvg_en;
		fvmap_header[i].o_ratevolt_cs = header[i].o_ratevolt_cs;
		fvmap_header[i].child_id[0] = header[i].child_id[0];
		fvmap_header[i].child_id[1] = header[i].child_id[1];
		fvmap_header[i].child_id[2] = header[i].child_id[2];
		fvmap_header[i].child_id[3] = header[i].child_id[3];
		fvmap_header[i].child_id[4] = header[i].child_id[4];
		fvmap_header[i].child_id[5] = header[i].child_id[5];
		fvmap_header[i].child_id[6] = header[i].child_id[6];
		fvmap_header[i].child_id[7] = header[i].child_id[7];
		fvmap_header[i].child_id[8] = header[i].child_id[8];
		fvmap_header[i].child_id[9] = header[i].child_id[9];
		fvmap_header[i].child_id[10] = header[i].child_id[10];
		fvmap_header[i].child_id[11] = header[i].child_id[11];
		fvmap_header[i].copy_col = header[i].copy_col;

		vclk = cmucal_get_node(ACPM_VCLK_TYPE | i);
		if (vclk == NULL)
			continue;
		pr_info("domain_id : %s - id : %x\n",
			vclk->name, fvmap_header[i].domain_id);
		pr_info("  num_of_lv      : %d\n", fvmap_header[i].num_of_lv);
		pr_info("  num_of_members : %d\n", fvmap_header[i].num_of_members);

		old = sram_base + fvmap_header[i].o_ratevolt;
		new = map_base + fvmap_header[i].o_ratevolt;

		margin = init_margin_table[vclk->margin_id];

		if (margin)
			cal_dfs_set_volt_margin(i | ACPM_VCLK_TYPE, margin);

		if (volt_offset_percent) {
			if ((volt_offset_percent < 100) && (volt_offset_percent > -100)) {
				percent_margin_table[i] = volt_offset_percent;
				cal_dfs_set_volt_margin(i | ACPM_VCLK_TYPE, volt_offset_percent);
			}
		}

		for (j = 0; j < fvmap_header[i].num_of_members; j++) {
			clks = sram_base + fvmap_header[i].o_members;

			if (j < fvmap_header[i].num_of_pll) {
				plls = sram_base + clks->addr[j];
				member_addr = plls->addr - 0x90000000;
			} else {

				member_addr = (clks->addr[j] & ~0x3) & 0xffff;
				blk_idx = clks->addr[j] & 0x3;

				member_addr |= ((fvmap_header[i].block_addr[blk_idx]) << 16) - 0x90000000;
			}

			vclk->list[j] = cmucal_get_id_by_addr(member_addr);

			if (vclk->list[j] == INVALID_CLK_ID)
				pr_info("  Invalid addr :0x%x\n", member_addr);
			else
				pr_info("  DVFS CMU addr:0x%x\n", member_addr);
		}

		for (j = 0; j < fvmap_header[i].num_of_lv; j++) {
			new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) \n",
				new->table[j].rate, new->table[j].volt * STEP_UV,
				volt_offset_percent);
		}
	}
}

int fvmap_init(void __iomem *sram_base)
{
	void __iomem *map_base;
	struct kobject *kobj;

	map_base = kzalloc(FVMAP_SIZE, GFP_KERNEL);

	if (cal_variant == EXYNOSAUTO_V9)
		percent_margin_group = &percent_margin_group_v9;
	else if ((cal_variant == EXYNOSAUTO_V920_EVT0) || (cal_variant == EXYNOSAUTO_V920_EVT2) || (cal_variant == EXYNOSAUTO_V620_EVT0))
		percent_margin_group = &percent_margin_group_v920;

	fvmap_base = map_base;
	sram_fvmap_base = sram_base;
	pr_info("%s:fvmap initialize 0x%lx\n", __func__, (unsigned long)sram_base);
	margin_table_init();

	if (cal_variant == EXYNOSAUTO_V9)
		fvmap_copy_from_sram_v9(map_base, sram_base);
	else if ((cal_variant == EXYNOSAUTO_V920_EVT0) || (cal_variant == EXYNOSAUTO_V920_EVT2) || (cal_variant == EXYNOSAUTO_V620_EVT0))
		fvmap_copy_from_sram_v920(map_base, sram_base);

	/* percent margin for each doamin at runtime */
	kobj = kobject_create_and_add("percent_margin", kernel_kobj);
	if (!kobj)
		pr_err("Fail to create percent_margin kboject\n");

	if (sysfs_create_group(kobj, percent_margin_group))
		pr_err("Fail to create percent_margin group\n");

	return 0;
}
EXPORT_SYMBOL_GPL(fvmap_init);

MODULE_LICENSE("GPL");
