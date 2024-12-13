// SPDX-License-Identifier: GPL-2.0
/*
 * Common Clock Framework support for exynosautov920_evt0 SoC.
 *
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include "clkout_exynosautov920_evt0.h"

#define CLKOUT0_OFFSET		(0xA00)
#define CLKOUT0_SEL_SHIFT	(8)
#define CLKOUT0_SEL_WIDTH	(6)
#define CLKOUT0_SEL_TCXO	(0x1)
#define CLKOUT0_ENABLE_SHIFT	(0)
#define CLKOUT0_ENABLE_WIDTH	(1)

#define CLKOUT1_OFFSET		(0xA0C)
#define CLKOUT1_SEL_SHIFT	(8)
#define CLKOUT1_SEL_WIDTH	(6)
#define CLKOUT1_SEL_TCXO	(0x0)
#define CLKOUT1_ENABLE_SHIFT	(0)
#define CLKOUT1_ENABLE_WIDTH	(1)

struct cmucal_clkout v920_evt0_cmucal_clkout_list[] = {
};

unsigned int v920_evt0_cmucal_clkout_size;
