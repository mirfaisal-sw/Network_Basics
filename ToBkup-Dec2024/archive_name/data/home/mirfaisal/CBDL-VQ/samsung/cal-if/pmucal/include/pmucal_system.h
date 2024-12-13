/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#ifndef __PMUCAL_SYSTEM_H__
#define __PMUCAL_SYSTEM_H__
#include "pmucal_common.h"
#include "pmucal_dbg.h"

struct pmucal_lpm {
	u32 id;
	struct pmucal_seq *enter;
	struct pmucal_seq *save;
	struct pmucal_seq *exit;
	struct pmucal_seq *early_wakeup;
	u32 num_enter;
	u32 num_save;
	u32 num_exit;
	u32 num_early_wakeup;
	struct pmucal_dbg_info *dbg;
};

struct pmucal_system_variant {
	unsigned int *p_pmucal_lpm_init_size;
	unsigned int *p_pmucal_lpm_list_size;
	struct pmucal_seq *p_pmucal_lpm_init;
	struct pmucal_lpm *p_pmucal_lpm_list;
};

/* Chip-independent enumeration for low-power mode
 * All power modes should be described in here.
 */
enum sys_powerdown {
	SYS_SICD,
	SYS_SICD_CPD,
	SYS_AFTR,
	SYS_STOP,
	SYS_LPD,
	SYS_LPA,
	SYS_ALPA,
	SYS_DSTOP,
	SYS_SLEEP,
	SYS_SLEEP_VTS_ON,
	SYS_SLEEP_AUD_ON,
	SYS_FAPO,
	SYS_SLEEP_USBL2,
	NUM_SYS_POWERDOWN,
};

/* APIs to be supported to PWRCAL interface */
extern int pmucal_system_enter(int mode);
extern int pmucal_system_exit(int mode);
extern int pmucal_system_earlywakeup(int mode);
extern int pmucal_system_init(void);

extern struct pmucal_seq pmucal_lpm_init_v9[];
extern struct pmucal_lpm pmucal_lpm_list_v9[];
extern unsigned int pmucal_lpm_init_size_v9;
extern unsigned int pmucal_lpm_list_size_v9;

extern struct pmucal_seq pmucal_lpm_init_v920_evt0[];
extern struct pmucal_lpm pmucal_lpm_list_v920_evt0[];
extern unsigned int pmucal_lpm_init_size_v920_evt0;
extern unsigned int pmucal_lpm_list_size_v920_evt0;

extern struct pmucal_seq pmucal_lpm_init_v620_evt0[];
extern struct pmucal_lpm pmucal_lpm_list_v620_evt0[];
extern unsigned int pmucal_lpm_init_size_v620_evt0;
extern unsigned int pmucal_lpm_list_size_v620_evt0;

extern struct pmucal_seq pmucal_lpm_init[];
extern struct pmucal_lpm pmucal_lpm_list[];
extern unsigned int pmucal_lpm_init_size;
extern unsigned int pmucal_lpm_list_size;

extern unsigned int pmucal_sys_powermode[];
extern struct pmucal_system_variant pmucal_system_variant_v9;
extern struct pmucal_system_variant pmucal_system_variant_v920_evt0;
extern struct pmucal_system_variant pmucal_system_variant_v920_evt2;
extern struct pmucal_system_variant pmucal_system_variant_v620_evt0;
extern struct pmucal_system_variant *pmucal_system_variant_model;
#endif
