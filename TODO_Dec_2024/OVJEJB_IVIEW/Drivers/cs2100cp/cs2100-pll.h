/* SPDX-License-Identifier: GPL-2.0
 *
 * Header for Cirrus Logic CS2100 PLL driver
 *
 * Copyright (C) 2023 Harman International Co., Ltd.
 *
 * Created on: 04-April-2023
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether expressed or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 */

/* PLL Registers */
#define	DEVICE_CONTROL			0x02
#define	DEVICE_CONFIGURATION_1		0x03
#define	GLOBAL_CONFIGURATION		0x05
#define	RATO_MSB_7			0x06
#define	RATO_MSB_15			0x07
#define	RATO_LSB_15			0x08
#define	RATO_LSB_7			0x09
#define	FUNCTION_CONFIGURATION_1	0x16
#define	FUNCTION_CONFIGURATION_2	0x17
#define	FUNCTION_CONFIGURATION_3	0x1E

/* Regmap I2C Configuration */
#define	BYTE_MODE			0x08
#define	WRITE_MASK			0x80
#define	MAX_REG				0x1E

#define INVALID_COMMAND			0XFFFF

/**
 * struct cs2100_prv - Private structre of the PLL
 *
 * @data: Stores value after I2C read.
 * @map: Pointer to the regmap instance.
 */
struct cs2100_prv {
	u32 data;
	struct regmap *map;
};

/**
 * struct reg_sequence high_accuracy_config - I2C configuration commands
 *
 * The configuration required to generate 73.728 MHz from 8 KHz
 */
static const struct reg_sequence high_accuracy_config[] = {
	{ DEVICE_CONFIGURATION_1,	0x61 },
	{ GLOBAL_CONFIGURATION,		0x01 },
	{ FUNCTION_CONFIGURATION_1,	0x08 },
	{ FUNCTION_CONFIGURATION_2,	0x08 },
	{ FUNCTION_CONFIGURATION_3,	0x70 },
	{ RATO_MSB_7,			0x48 },
	{ RATO_MSB_15,			0x00 },
	{ RATO_LSB_15,			0x00 },
	{ RATO_LSB_7,			0x00 },
};
