// SPDX-License-Identifier: GPL-2.0
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include <linux/string.h>
#include <linux/module.h>
#include "cmucal.h"

#define get_node(list, idx)		(GET_IDX(list[idx].id) == idx ? \
					&list[idx] : NULL)
#define get_clk_node(list, idx)		(GET_IDX(list[idx].clk.id) == idx ? \
					&list[idx] : NULL)

struct cmucal_clk_variant clk_variant_v9 = {
	.p_cmucal_fixed_rate_list = cmucal_fixed_rate_list,
	.p_cmucal_fixed_factor_list = cmucal_fixed_factor_list,
	.p_cmucal_pll_list = cmucal_pll_list,
	.p_cmucal_mux_list = cmucal_mux_list,
	.p_cmucal_div_list = cmucal_div_list,
	.p_cmucal_gate_list = cmucal_gate_list,
	.p_cmucal_qch_list = cmucal_qch_list,
	.p_cmucal_option_list = cmucal_option_list,
	.p_cmucal_sfr_block_list = cmucal_sfr_block_list,
	.p_cmucal_sfr_list = cmucal_sfr_list,
	.p_cmucal_sfr_access_list = cmucal_sfr_access_list,
	.p_cmucal_vclk_list = cmucal_vclk_list,
	.p_acpm_vclk_list = acpm_vclk_list,
	.p_cmucal_clkout_list = cmucal_clkout_list,

	.p_cmucal_fixed_rate_size = &cmucal_fixed_rate_size,
	.p_cmucal_fixed_factor_size = &cmucal_fixed_factor_size,
	.p_cmucal_pll_size = &cmucal_pll_size,
	.p_cmucal_mux_size = &cmucal_mux_size,
	.p_cmucal_div_size = &cmucal_div_size,
	.p_cmucal_gate_size = &cmucal_gate_size,
	.p_cmucal_qch_size = &cmucal_qch_size,
	.p_cmucal_option_size = &cmucal_option_size,
	.p_cmucal_sfr_block_size = &cmucal_sfr_block_size,
	.p_cmucal_sfr_size = &cmucal_sfr_size,
	.p_cmucal_sfr_access_size = &cmucal_sfr_access_size,
	.p_cmucal_vclk_size = &cmucal_vclk_size,
	.p_acpm_vclk_size = &acpm_vclk_size,
	.p_cmucal_clkout_size = &cmucal_clkout_size,
};

struct cmucal_clk_variant clk_variant_v920_evt0 = {
	.p_cmucal_fixed_rate_list = v920_evt0_cmucal_fixed_rate_list,
	.p_cmucal_fixed_factor_list = v920_evt0_cmucal_fixed_factor_list,
	.p_cmucal_pll_list = v920_evt0_cmucal_pll_list,
	.p_cmucal_mux_list = v920_evt0_cmucal_mux_list,
	.p_cmucal_div_list = v920_evt0_cmucal_div_list,
	.p_cmucal_gate_list = v920_evt0_cmucal_gate_list,
	.p_cmucal_qch_list = v920_evt0_cmucal_qch_list,
	.p_cmucal_option_list = v920_evt0_cmucal_option_list,
	.p_cmucal_sfr_block_list = v920_evt0_cmucal_sfr_block_list,
	.p_cmucal_sfr_list = v920_evt0_cmucal_sfr_list,
	.p_cmucal_sfr_access_list = v920_evt0_cmucal_sfr_access_list,
	.p_cmucal_vclk_list = v920_evt0_cmucal_vclk_list,
	.p_acpm_vclk_list = v920_acpm_vclk_list,
	.p_cmucal_clkout_list = v920_evt0_cmucal_clkout_list,

	.p_cmucal_fixed_rate_size = &v920_evt0_cmucal_fixed_rate_size,
	.p_cmucal_fixed_factor_size = &v920_evt0_cmucal_fixed_factor_size,
	.p_cmucal_pll_size = &v920_evt0_cmucal_pll_size,
	.p_cmucal_mux_size = &v920_evt0_cmucal_mux_size,
	.p_cmucal_div_size = &v920_evt0_cmucal_div_size,
	.p_cmucal_gate_size = &v920_evt0_cmucal_gate_size,
	.p_cmucal_qch_size = &v920_evt0_cmucal_qch_size,
	.p_cmucal_option_size = &v920_evt0_cmucal_option_size,
	.p_cmucal_sfr_block_size = &v920_evt0_cmucal_sfr_block_size,
	.p_cmucal_sfr_size = &v920_evt0_cmucal_sfr_size,
	.p_cmucal_sfr_access_size = &v920_evt0_cmucal_sfr_access_size,
	.p_cmucal_vclk_size = &v920_evt0_cmucal_vclk_size,
	.p_acpm_vclk_size = &v920_acpm_vclk_size,
	.p_cmucal_clkout_size = &v920_evt0_cmucal_clkout_size,
};

struct cmucal_clk_variant clk_variant_v920_evt2 = {
	.p_cmucal_fixed_rate_list = v920_evt2_cmucal_fixed_rate_list,
	.p_cmucal_fixed_factor_list = v920_evt2_cmucal_fixed_factor_list,
	.p_cmucal_pll_list = v920_evt2_cmucal_pll_list,
	.p_cmucal_mux_list = v920_evt2_cmucal_mux_list,
	.p_cmucal_div_list = v920_evt2_cmucal_div_list,
	.p_cmucal_gate_list = v920_evt2_cmucal_gate_list,
	.p_cmucal_qch_list = v920_evt2_cmucal_qch_list,
	.p_cmucal_option_list = v920_evt2_cmucal_option_list,
	.p_cmucal_sfr_block_list = v920_evt2_cmucal_sfr_block_list,
	.p_cmucal_sfr_list = v920_evt2_cmucal_sfr_list,
	.p_cmucal_sfr_access_list = v920_evt2_cmucal_sfr_access_list,
	.p_cmucal_vclk_list = v920_evt2_cmucal_vclk_list,
	.p_acpm_vclk_list = v920_acpm_vclk_list,
	.p_cmucal_clkout_list = v920_evt2_cmucal_clkout_list,

	.p_cmucal_fixed_rate_size = &v920_evt2_cmucal_fixed_rate_size,
	.p_cmucal_fixed_factor_size = &v920_evt2_cmucal_fixed_factor_size,
	.p_cmucal_pll_size = &v920_evt2_cmucal_pll_size,
	.p_cmucal_mux_size = &v920_evt2_cmucal_mux_size,
	.p_cmucal_div_size = &v920_evt2_cmucal_div_size,
	.p_cmucal_gate_size = &v920_evt2_cmucal_gate_size,
	.p_cmucal_qch_size = &v920_evt2_cmucal_qch_size,
	.p_cmucal_option_size = &v920_evt2_cmucal_option_size,
	.p_cmucal_sfr_block_size = &v920_evt2_cmucal_sfr_block_size,
	.p_cmucal_sfr_size = &v920_evt2_cmucal_sfr_size,
	.p_cmucal_sfr_access_size = &v920_evt2_cmucal_sfr_access_size,
	.p_cmucal_vclk_size = &v920_evt2_cmucal_vclk_size,
	.p_acpm_vclk_size = &v920_acpm_vclk_size,
	.p_cmucal_clkout_size = &v920_evt2_cmucal_clkout_size,
};

struct cmucal_clk_variant clk_variant_v620_evt0 = {
	.p_cmucal_fixed_rate_list = v620_evt0_cmucal_fixed_rate_list,
	.p_cmucal_fixed_factor_list = v620_evt0_cmucal_fixed_factor_list,
	.p_cmucal_pll_list = v620_evt0_cmucal_pll_list,
	.p_cmucal_mux_list = v620_evt0_cmucal_mux_list,
	.p_cmucal_div_list = v620_evt0_cmucal_div_list,
	.p_cmucal_gate_list = v620_evt0_cmucal_gate_list,
	.p_cmucal_qch_list = v620_evt0_cmucal_qch_list,
	.p_cmucal_option_list = v620_evt0_cmucal_option_list,
	.p_cmucal_sfr_block_list = v620_evt0_cmucal_sfr_block_list,
	.p_cmucal_sfr_list = v620_evt0_cmucal_sfr_list,
	.p_cmucal_sfr_access_list = v620_evt0_cmucal_sfr_access_list,
	.p_cmucal_vclk_list = v620_evt0_cmucal_vclk_list,
	.p_acpm_vclk_list = v620_acpm_vclk_list,
	.p_cmucal_clkout_list = v620_evt0_cmucal_clkout_list,

	.p_cmucal_fixed_rate_size = &v620_evt0_cmucal_fixed_rate_size,
	.p_cmucal_fixed_factor_size = &v620_evt0_cmucal_fixed_factor_size,
	.p_cmucal_pll_size = &v620_evt0_cmucal_pll_size,
	.p_cmucal_mux_size = &v620_evt0_cmucal_mux_size,
	.p_cmucal_div_size = &v620_evt0_cmucal_div_size,
	.p_cmucal_gate_size = &v620_evt0_cmucal_gate_size,
	.p_cmucal_qch_size = &v620_evt0_cmucal_qch_size,
	.p_cmucal_option_size = &v620_evt0_cmucal_option_size,
	.p_cmucal_sfr_block_size = &v620_evt0_cmucal_sfr_block_size,
	.p_cmucal_sfr_size = &v620_evt0_cmucal_sfr_size,
	.p_cmucal_sfr_access_size = &v620_evt0_cmucal_sfr_access_size,
	.p_cmucal_vclk_size = &v620_evt0_cmucal_vclk_size,
	.p_acpm_vclk_size = &v620_acpm_vclk_size,
	.p_cmucal_clkout_size = &v620_evt0_cmucal_clkout_size,
};


struct cmucal_clk_variant *clk_variant;

unsigned int cmucal_get_list_size(unsigned int type)
{
	switch (type) {
	case FIXED_RATE_TYPE:
		return *clk_variant->p_cmucal_fixed_rate_size;
	case FIXED_FACTOR_TYPE:
		return *clk_variant->p_cmucal_fixed_factor_size;
	case PLL_TYPE:
		return *clk_variant->p_cmucal_pll_size;
	case MUX_TYPE:
		return *clk_variant->p_cmucal_mux_size;
	case DIV_TYPE:
		return *clk_variant->p_cmucal_div_size;
	case GATE_TYPE:
		return *clk_variant->p_cmucal_gate_size;
	case QCH_TYPE:
		return *clk_variant->p_cmucal_qch_size;
	case OPTION_TYPE:
		return *clk_variant->p_cmucal_option_size;
	case SFR_BLOCK_TYPE:
		return *clk_variant->p_cmucal_sfr_block_size;
	case SFR_TYPE:
		return *clk_variant->p_cmucal_sfr_size;
	case SFR_ACCESS_TYPE:
		return *clk_variant->p_cmucal_sfr_access_size;
	case VCLK_TYPE:
		return *clk_variant->p_cmucal_vclk_size;
	case CLKOUT_TYPE:
		return *clk_variant->p_cmucal_clkout_size;
	case ACPM_VCLK_TYPE:
		return *clk_variant->p_acpm_vclk_size;
	default:
		pr_info("unsupport cmucal type %x\n", type);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(cmucal_get_list_size);

void *cmucal_get_node(unsigned int id)
{
	unsigned int type = GET_TYPE(id);
	unsigned short idx = GET_IDX(id);
	void *node = NULL;

	switch (type) {
	case FIXED_RATE_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_fixed_rate_list, idx);
		break;
	case FIXED_FACTOR_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_fixed_factor_list, idx);
		break;
	case PLL_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_pll_list, idx);
		break;
	case MUX_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_mux_list, idx);
		break;
	case DIV_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_div_list, idx);
		break;
	case GATE_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_gate_list, idx);
		break;
	case QCH_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_qch_list, idx);
		break;
	case OPTION_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_option_list, idx);
		break;
	case CLKOUT_TYPE:
		node = get_clk_node(clk_variant->p_cmucal_clkout_list, idx);
		break;
	case VCLK_TYPE:
		if (IS_ACPM_VCLK(id))
			node = get_node(clk_variant->p_acpm_vclk_list, idx);
		else
			node = get_node(clk_variant->p_cmucal_vclk_list, idx);
		break;
	case ACPM_VCLK_TYPE:
		node = get_node(clk_variant->p_acpm_vclk_list, idx);
		break;
	default:
		pr_info("unsupport cmucal node %x\n", id);
	}

	return node;
}
EXPORT_SYMBOL_GPL(cmucal_get_node);

void * cmucal_get_sfr_node(unsigned int id)
{
	unsigned int type = GET_TYPE(id);
	unsigned short idx = GET_IDX(id);
	void *node = NULL;

	switch (type) {
	case SFR_BLOCK_TYPE:
		node = get_node(clk_variant->p_cmucal_sfr_block_list, idx);
		break;
	case SFR_TYPE:
		node = get_node(clk_variant->p_cmucal_sfr_list, idx);
		break;
	case SFR_ACCESS_TYPE:
		node = get_node(clk_variant->p_cmucal_sfr_access_list, idx);
		break;
	default:
		pr_info("unsupport cmucal sfr node %x\n", id);
	}

	return node;
}
EXPORT_SYMBOL_GPL(cmucal_get_sfr_node);

unsigned int cmucal_get_id(char *name)
{
	unsigned int id = INVALID_CLK_ID;
	int i;

	if (strstr(name, "MUX") || strstr(name, "MOUT")) {
		for (i = 0; i < *clk_variant->p_cmucal_mux_size; i++)
			if (!strcmp(name, clk_variant->p_cmucal_mux_list[i].clk.name)) {
				id = clk_variant->p_cmucal_mux_list[i].clk.id;
				return id;
			}
	}

	if (strstr(name, "GATE") || strstr(name, "GOUT") || strstr(name, "CLK_BLK")) {
		for (i = 0; i < *clk_variant->p_cmucal_gate_size; i++)
			if (!strcmp(name, clk_variant->p_cmucal_gate_list[i].clk.name)) {
				id = clk_variant->p_cmucal_gate_list[i].clk.id;
				return id;
			}
	}

	if (strstr(name, "DIV") || strstr(name, "DOUT") || strstr(name, "CLKCMU")) {
		for (i = 0; i < *clk_variant->p_cmucal_div_size; i++)
			if (!strcmp(name, clk_variant->p_cmucal_div_list[i].clk.name)) {
				id = clk_variant->p_cmucal_div_list[i].clk.id;
				return id;
			}
		for (i = 0; i < *clk_variant->p_cmucal_fixed_factor_size; i++)
			if (!strcmp(name, clk_variant->p_cmucal_fixed_factor_list[i].clk.name)) {
				id = clk_variant->p_cmucal_fixed_factor_list[i].clk.id;
				return id;
			}
	}

	if (strstr(name, "VCLK") || strstr(name, "dvfs")) {
		for (i = 0; i < *clk_variant->p_cmucal_vclk_size; i++)
			if (!strcmp(name, clk_variant->p_cmucal_vclk_list[i].name)) {
				id = clk_variant->p_cmucal_vclk_list[i].id;
				return id;
			}
	}

	if (strstr(name, "PLL")) {
		for (i = 0; i < *clk_variant->p_cmucal_pll_size; i++)
			if (!strcmp(name, clk_variant->p_cmucal_pll_list[i].clk.name)) {
				id = clk_variant->p_cmucal_pll_list[i].clk.id;
				return id;
			}
	}

	if (strstr(name, "IO") || strstr(name, "OSC")) {
		for (i = 0; i < *clk_variant->p_cmucal_fixed_rate_size; i++)
			if (!strcmp(name, clk_variant->p_cmucal_fixed_rate_list[i].clk.name)) {
				id = clk_variant->p_cmucal_fixed_rate_list[i].clk.id;
				return id;
			}
	}

	return id;
}
EXPORT_SYMBOL_GPL(cmucal_get_id);

unsigned int cmucal_get_id_by_addr(unsigned int addr)
{
	unsigned int id = INVALID_CLK_ID;
	unsigned int type;
	int i;

	type = addr & 0x3F00;

	if (type == 0x1800 || type == 0x1900) {
		/* DIV */
		for (i = 0; i < *clk_variant->p_cmucal_div_size; i++)
			if (addr == clk_variant->p_cmucal_div_list[i].clk.paddr) {
				id = clk_variant->p_cmucal_div_list[i].clk.id;
				return id;
			}
	} else if (type == 0x1000 || type == 0x1100) {
		/* MUX */
		for (i = 0; i < *clk_variant->p_cmucal_mux_size; i++)
			if (addr == clk_variant->p_cmucal_mux_list[i].clk.paddr) {
				id = clk_variant->p_cmucal_mux_list[i].clk.id;
				return id;
			}
	} else if (type == 0x2000 || type == 0x2100) {
		/* GATE */
		for (i = 0; i < *clk_variant->p_cmucal_gate_size; i++)
			if (addr == clk_variant->p_cmucal_gate_list[i].clk.paddr) {
				id = clk_variant->p_cmucal_gate_list[i].clk.id;
				return id;
			}
	} else {
		/* PLL */
		for (i = 0; i < *clk_variant->p_cmucal_pll_size; i++)
			if (addr == clk_variant->p_cmucal_pll_list[i].clk.paddr) {
				id = clk_variant->p_cmucal_pll_list[i].clk.id;
				return id;
			}

		/* USER_MUX */
		for (i = 0; i < *clk_variant->p_cmucal_mux_size; i++)
			if (addr == clk_variant->p_cmucal_mux_list[i].clk.paddr) {
				id = clk_variant->p_cmucal_mux_list[i].clk.id;
				return id;
			}
	}

	return id;
}
EXPORT_SYMBOL_GPL(cmucal_get_id_by_addr);

MODULE_LICENSE("GPL");
