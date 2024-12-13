/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#ifndef __PMUCAL_RAE_H__
#define __PMUCAL_RAE_H__

#ifdef PWRCAL_TARGET_LINUX
#include <linux/io.h>
#endif

/* for phy2virt address mapping */
struct p2v_map {
	phys_addr_t pa;
	void __iomem *va;
};

struct pmucal_rae_variant {
	struct p2v_map *p_pmucal_p2v_list;
	unsigned int *p_pmucal_p2v_list_size;
};

#define DEFINE_PHY(p) { .pa = p }

/* APIs to be supported to PMUCAL common logics */
extern int pmucal_rae_init(void);
extern int pmucal_rae_handle_seq(struct pmucal_seq *seq, unsigned int seq_size);
extern void pmucal_rae_save_seq(struct pmucal_seq *seq, unsigned int seq_size);
extern int pmucal_rae_restore_seq(struct pmucal_seq *seq, unsigned int seq_size);
extern int pmucal_rae_phy2virt(struct pmucal_seq *seq, unsigned int seq_size);

extern struct p2v_map pmucal_p2v_list_v9[];
extern unsigned int pmucal_p2v_list_size_v9;

extern struct p2v_map pmucal_p2v_list_v920_evt0[];
extern unsigned int pmucal_p2v_list_size_v920_evt0;

extern struct p2v_map pmucal_p2v_list[];
extern unsigned int pmucal_p2v_list_size;

extern struct p2v_map pmucal_p2v_list_v620_evt0[];
extern unsigned int pmucal_p2v_list_size_v620_evt0;

extern struct pmucal_rae_variant *pmucal_rae_variant_model;
extern struct pmucal_rae_variant pmucal_rae_variant_v9;
extern struct pmucal_rae_variant pmucal_rae_variant_v920_evt0;
extern struct pmucal_rae_variant pmucal_rae_variant_v920_evt2;
extern struct pmucal_rae_variant pmucal_rae_variant_v620_evt0;
#endif
