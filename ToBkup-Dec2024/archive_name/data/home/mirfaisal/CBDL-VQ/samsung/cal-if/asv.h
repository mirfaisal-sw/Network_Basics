/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#ifndef __ASV_H__
#define __ASV_H__

extern unsigned int asv_table_ver;

extern int asv_table_init(void);
extern int v920_asv_table_init(void);

extern int id_get_asv_fusing(void);
#endif
