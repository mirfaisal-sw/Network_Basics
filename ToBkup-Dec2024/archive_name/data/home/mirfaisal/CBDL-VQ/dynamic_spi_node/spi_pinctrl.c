/*
 *  FILE: spi_pinctrl.c
 *
 *  Copyright (C) 2020 Harman International Ltd,
 *  by Mir Faisal <mir.faisal2@harman.com>
 *  Created on: 12-Apr-2022
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2.0 as published by
 * the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Description: This driver configures internal pullup and pulldown
 *              resistors of SPI0 pins at runtime in SOCs comaptible
 *              with AM335X register set.
 */

#include "spi_pinctrl.h"

static void spi_pin_writel(const struct spi_pinctrl *mspi_pinctrl, u32 reg,
		u32 val)
{
	writel(val, mspi_pinctrl->base + reg);
}

static u32 spi_pin_readl(const struct spi_pinctrl *mspi_pinctrl, u32 reg)
{
	return readl(mspi_pinctrl->base + reg);
}

static
int read_control_module_pin_register(struct spi_pinctrl *mspi_pinctrl,
		pin_name_t m_pin)
{
	int ret;
	u32 pin_reg;

	switch (m_pin) {
	case pin_spi0_sclk:
		pin_reg = CONF_SPI0_SCLK;
		break;

	case pin_spi0_d0:
		pin_reg = CONF_SPI0_D0;
		break;

	case pin_spi0_d1:
		pin_reg = CONF_SPI0_D1;
		break;

	case pin_spi0_cs0:
		pin_reg = CONF_SPI0_CS0;
		break;

	case pin_spi0_cs1:
		pin_reg = CONF_SPI0_CS1;
		break;

	default:
		pr_debug("Wrong pin selected\n");
		ret = -EINVAL;
		goto err;
	}

	/*Read control module pin register*/
	mspi_pinctrl->conf_module_reg.reg_word = spi_pin_readl(mspi_pinctrl,
			pin_reg);
	mspi_pinctrl->value_read =  mspi_pinctrl->conf_module_reg.reg_word;

	if (!(mspi_pinctrl->conf_module_reg.reg_bits.pud_en)) {

		pr_debug("Pin - %s , %s\n", mspi_pinctrl->pin_names[m_pin],
			 ((mspi_pinctrl->conf_module_reg.reg_bits.pu_typesel) ?
			 "pullup enabled" : "pulldown enabled"));
	} else {
		pr_debug("Pin - %s, pullup pulldown disabled\n",
			mspi_pinctrl->pin_names[m_pin]);
	}

	return 0;

err:
	pr_err("Error in reading spi0 pinctrl hardware registers\n");
	return ret;
}

void read_spi0_pinctrl_reg_for_debug(struct spi_pinctrl *mspi_pinctrl,
			u8 debug)
{
	int i_pin;
	u8 num_pins_toread = mspi_pinctrl->num_of_pins_toconfig;

	if (debug) {
		for (i_pin = 0; i_pin < num_pins_toread; i_pin++)
			read_control_module_pin_register(mspi_pinctrl, i_pin);
	}
}

static int program_internal_pullup_pulldown(
		struct spi_pinctrl *mspi_pinctrl, pin_name_t m_pin)
{
	u32 pin_reg;
	u32 write_val = 0;
	int ret = 0;

	switch (m_pin) {
	case pin_spi0_sclk:
		pin_reg = CONF_SPI0_SCLK;
		break;

	case pin_spi0_d0:
		pin_reg = CONF_SPI0_D0;
		break;

	case pin_spi0_d1:
		pin_reg = CONF_SPI0_D1;
		break;

	case pin_spi0_cs0:
		pin_reg = CONF_SPI0_CS0;
		break;

	case pin_spi0_cs1:
		pin_reg = CONF_SPI0_CS1;
		break;

	default:
		pr_err("Wrong pin selected\n");
		ret = -EINVAL;
		goto err;
	}

	/*Read control module pin register*/
	mspi_pinctrl->conf_module_reg.reg_word = spi_pin_readl(mspi_pinctrl,
			pin_reg);
	write_val = mspi_pinctrl->conf_module_reg.reg_word;

	switch (mspi_pinctrl->pud_mode) {
	case PULL_UP_ENABLE:
		/*Enable pullup or pulldown*/
		write_val &= ~CONF_PUD_EN;
		/*Select pullup*/
		write_val |= CONF_PU_TYPESEL;
		break;

	case PULL_DOWN_ENABLE:
		/*Enable pullup or pulldown*/
		write_val &= ~CONF_PUD_EN;
		/*Select pulldown*/
		write_val &= ~CONF_PU_TYPESEL;
		break;

	case PULL_UPDOWN_DISABLE:
		/*Disable pullup or pulldown*/
		write_val |= CONF_PUD_EN;
		write_val &= ~CONF_PU_TYPESEL;
		break;

	default:
		pr_err("Wrong mode selected\n");
		ret = -EINVAL;
		goto err;
	}

	/*Write into control module pin register*/
	spi_pin_writel(mspi_pinctrl, pin_reg, write_val);

err:
	return ret;
}

int program_spi0_internal_pud_pins(struct spi_pinctrl *mspi_pinctrl)
{
	int ret;
	int i_pin;

	for (i_pin = 0; i_pin < mspi_pinctrl->num_of_pins_toconfig; i_pin++) {

		ret = program_internal_pullup_pulldown(mspi_pinctrl, i_pin);
		if (ret)
			break;
	}

	return ret;
}

struct spi_pinctrl *spi0_pinctrl_alloc_and_init(pud_mode_t default_mode)
{
	struct spi_pinctrl *mspi_pinctrl;

	mspi_pinctrl = kzalloc(sizeof(*mspi_pinctrl), GFP_KERNEL);
	if (!mspi_pinctrl)
		return ERR_PTR(-ENOMEM);

	mspi_pinctrl->base = ioremap(CONTROL_MODULE_BASE_REGISTER,
			CONTROL_MODULE_MAP_SIZE);
	if (!mspi_pinctrl->base)
		return ERR_PTR(-ENOMEM);

	mspi_pinctrl->num_of_pins_toconfig = (MAX_SPI0_PINS - 1);

	/*Parameter to set internal pull up behaviour*/
	mspi_pinctrl->pud_mode = default_mode;

	mspi_pinctrl->pin_names[pin_spi0_sclk] = "SPI0_CLK_PIN";
	mspi_pinctrl->pin_names[pin_spi0_d0] = "SPI0_D0_PIN";
	mspi_pinctrl->pin_names[pin_spi0_d1] = "SPI0_D1_PIN";
	mspi_pinctrl->pin_names[pin_spi0_cs0] = "SPI0_CS0_PIN";
	mspi_pinctrl->pin_names[pin_spi0_cs1] = "SPI0_CS1_PIN";

	return mspi_pinctrl;
}

void spi0_pinctrl_release(struct spi_pinctrl **pspi_pinctrl)
{
	const struct spi_pinctrl *mspi_pinctrl = *pspi_pinctrl;

	if (mspi_pinctrl) {
		if (mspi_pinctrl->base)
			iounmap(mspi_pinctrl->base);

		kfree(*pspi_pinctrl);
		*pspi_pinctrl = NULL;
	}
}
