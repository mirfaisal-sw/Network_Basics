// SPDX-License-Identifier: GPL-2.0
/*
 * I2C access driver for Cirrus Logic CS2100 PLL
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

#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/regmap.h>

#include "cs2100-pll.h"

/**
 * lock_status_show - Sysfs read function for PLL lock status
 *
 * @dev: Pointer to device structure.
 * @attr: Pointer to device attribute structure.
 * @buf: Buffer holds the value that can be accessed in user-space.
 *
 * This function exposes the lock status of the PLL to the user.
 * Bit 7 of 0x02 register indicates the current status.
 * If set, the PLL is unlocked; otherwise, it is locked.
 *
 * Return: The number of characters written successfully to buf.
 */
static ssize_t lock_status_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int ret;
	u32 value;
	struct cs2100_prv *prv = dev_get_drvdata(dev);

	ret = regmap_read(prv->map, DEVICE_CONTROL, &value);
	if (ret) {
		dev_err(dev, "regmap_read() failed: %d\n", ret);
		return ret;
	}
	ret = regmap_read(prv->map, DEVICE_CONTROL, &value);
	if (ret) {
		dev_err(dev, "regmap_read() failed: %d\n", ret);
		return ret;
	}

	return scnprintf(buf, sizeof(value), "%x", value);
};

/**
 * pll_reg_show - Sysfs read function for I2C read.
 *
 * @dev: Pointer to device structure.
 * @attr: Pointer to device attribute structure.
 * @buf: Buffer holds the value that can be accessed in user-space.
 *
 * This function updates the buf with data read from the
 * register using I2C.
 *
 * Return: The number of characters written successfully to buf.
 */
static ssize_t pll_reg_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	struct cs2100_prv *prv = dev_get_drvdata(dev);

	if (prv->data == 0xffff)
		return scnprintf(buf, 16, "%s", "Invalid Command");
	else
		return scnprintf(buf, sizeof(prv->data), "%x", prv->data);
};

/**
 * pll_reg_store - Sysfs store to form I2C read/write.
 *
 * @dev: Pointer to device structure.
 * @attr: Pointer to device attribute structure.
 * @buf: Buffer holds the value that is sent from user-space.
 * @count: Length of the data received from user space.
 *
 * This function performs i2c read or write based on the command received.
 *
 * Return: The length of data received from the user-space.
 */
static ssize_t pll_reg_store(struct device *dev,
			     struct device_attribute *attr,
			     const char *buf, size_t count)
{
	int ret;
	u32 reg, value;
	struct cs2100_prv *prv = dev_get_drvdata(dev);

	if (!strncmp(buf, "read", 4) &&
	    (sscanf(buf + 5, "%x", &reg) == 1)) {
		ret = regmap_read(prv->map, reg, &prv->data);
		if (ret) {
			dev_err(dev, "regmap_read() failed: %d\n", ret);
			return ret;
		}
	} else if (!strncmp(buf, "write", 5) &&
		   (sscanf(buf + 6, "%x %x", &reg, &value) == 2)) {
		ret = regmap_write(prv->map, reg, value);
		if (ret) {
			dev_err(dev, "regmap_write() failed: %d\n", ret);
			return ret;
		}

		ret = regmap_read(prv->map, reg, &prv->data);
		if (ret) {
			dev_err(dev, "regmap_read() failed: %d\n", ret);
			return ret;
		}
	} else {
		dev_err(dev, "Invalid command");
		prv->data = INVALID_COMMAND;
	}

	return count;
};

static DEVICE_ATTR_RO(lock_status);
static DEVICE_ATTR_RW(pll_reg);

/**
 * cs2100_writeable_reg - Check writeable register range.
 *
 * @dev: Pointer to device structure.
 * @reg: Register to be checked.
 *
 * This function is automatically called to check whether the register
 * can be written to or not.
 *
 * Return: True if it is range, else False.
 */
static bool cs2100_writeable_reg(struct device *dev, unsigned int reg)
{
	if ((reg >= 0x01 && reg <= 0x09) ||
	    (reg >= 0x16 && reg <= 0x1E))
		return true;
	return false;
}

/**
 * cs2100_readable_reg - Check readable register range.
 *
 * @dev: Pointer to device structure.
 * @reg: Register to be checked.
 *
 * This function is automatically called to check whether the register
 * can be read or not.
 *
 * Return: True if it is range, else False.
 */
static bool cs2100_readable_reg(struct device *dev, unsigned int reg)
{
	if ((reg >= 0x01 && reg <= 0x09) ||
	    (reg >= 0x16 && reg <= 0x1E))
		return true;
	return false;
}

/**
 * cs2100_sysfs_register - Creates the required sysfs attributes.
 *
 * @dev: Pointer to device structure.
 *
 * This function creates a sysfs entry for lock status and i2c read/write.
 *
 * Return: The length of data received from the user-space.
 */
static int cs2100_sysfs_register(struct device *dev)
{
	int ret;

	if (!dev) {
		pr_info("Device not found:%s\n", __func__);
		return -EINVAL;
	}

	ret = device_create_file(dev, &dev_attr_pll_reg);
	if (ret) {
		dev_err(dev, "Failed to create sysfs:pll_reg:%d\n", ret);
		return ret;
	}

	ret = device_create_file(dev, &dev_attr_lock_status);
	if (ret) {
		dev_err(dev, "Failed to create sysfs:lock_status:%d\n", ret);
		/* Clean-up of sysfs entry */
		device_remove_file(dev, &dev_attr_pll_reg);
	}

	return ret;
}

/**
 * cs2100_sysfs_remove - Clean up for sysfs attributes.
 *
 * @dev: Pointer to device structure.
 *
 * This function cleans up the sysfs entries.
 */
static void cs2100_sysfs_remove(struct device *dev)
{
	device_remove_file(dev, &dev_attr_pll_reg);
	device_remove_file(dev, &dev_attr_lock_status);
}

static int cs2100_i2c_probe(struct i2c_client *client,
			    const struct i2c_device_id *ids)
{
	int ret;
	struct cs2100_prv *prv;
	struct regmap_config config;
	/* setup the regmap configuration */
	memset(&config, 0, sizeof(config));
	config.reg_bits = BYTE_MODE;
	config.val_bits = BYTE_MODE;
	config.write_flag_mask = WRITE_MASK;
	config.max_register = MAX_REG;
	config.writeable_reg = cs2100_writeable_reg;
	config.readable_reg = cs2100_readable_reg;

	prv  = devm_kzalloc(&client->dev, sizeof(*prv), GFP_KERNEL);
	if (!prv) {
		dev_err(&client->dev, "Failed to create Private data");
		return -ENOMEM;
	}

	i2c_set_clientdata(client, prv);

	prv->map = devm_regmap_init_i2c(client, &config);
	if (IS_ERR(prv->map)) {
		dev_err(&client->dev, "Regmap init failed:%ld\n", PTR_ERR(prv->map));
		return PTR_ERR(prv->map);
	}

	ret = regmap_multi_reg_write(prv->map, high_accuracy_config,
				     ARRAY_SIZE(high_accuracy_config));
	if (ret) {
		dev_err(&client->dev, "Regmap PLL configuration failed: %d\n", ret);
		return ret;
	}

	ret = cs2100_sysfs_register(&client->dev);
	if (ret) {
		dev_err(&client->dev, "Failed to create sysfs:%d\n", ret);
		return ret;
	}

	dev_info(&client->dev, "Driver probed successfully\n");

	return 0;
}

static int cs2100_i2c_remove(struct i2c_client *client)
{
	cs2100_sysfs_remove(&client->dev);
	dev_info(&client->dev, "cs2100 driver is removed");

	return 0;
}

static int cs2100_suspend(struct device *dev)
{
	/* No Low power mode or sleep */
	pr_alert("MARKER STR SUSPEND START - CS2100:%s\n", __func__);
	pr_alert("MARKER STR SUSPEND END - CS2100:%s\n", __func__);

	return 0;
}

static int cs2100_resume(struct device *dev)
{
	int ret;
	struct cs2100_prv *prv = dev_get_drvdata(dev);

	if (!prv)
		return -ENODATA;

	pr_alert("MARKER STR RESUME START-CS2100:%s\n",__func__);

	ret = regmap_multi_reg_write(prv->map, high_accuracy_config,
				     ARRAY_SIZE(high_accuracy_config));
	if (ret)
		dev_err(dev, "Regmap PLL configuration failed after S2R: %d\n", ret);

	pr_alert("MARKER STR RESUME END-CS2100:%s, status: %s, ret: %d\n",
		 __func__, ret ? "FAILED" : "SUCCESS", ret);

	return ret;
};

static const struct dev_pm_ops cs2100_pm_ops = {
	.suspend = cs2100_suspend,
	.resume = cs2100_resume,
};

static const struct of_device_id cs2100_i2c_of_match_table[] = {
	{ .compatible = "cirrus,cs2100-cp", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, cs2100_i2c_of_match_table);

static const struct i2c_device_id cs2100_i2c_id_table[] = {
	{ "cs2100", 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, cs2100_i2c_id_table);

static struct i2c_driver cs2100_i2c_driver = {
	.driver		= {
		.name	= "cs2100",
		.owner	= THIS_MODULE,
		.of_match_table	= cs2100_i2c_of_match_table,
		.pm = &cs2100_pm_ops,
	},
	.probe		= cs2100_i2c_probe,
	.remove		= cs2100_i2c_remove,
	.id_table	= cs2100_i2c_id_table,
};
module_i2c_driver(cs2100_i2c_driver);

MODULE_AUTHOR("Abid Ali");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CS2100 Cirrus Logic PLL Driver");
