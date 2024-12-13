// SPDX-License-Identifier: GPL-2.0
/*
 *  Driver for Marvell 88Q222x PHYs
 *
 *  Copyright (C) 2023 Harman International Ltd.
 */

#include <linux/module.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/of.h>
#include <linux/delay.h>

/* Operation succeeded */
#define Q222X_OK				0
/* Operation failed */
#define Q222X_FAIL				1

#define MRVL_APHY_ID				0x0032
#define MRVL_Q222X_PRODUCT_ID			0x0000
#define MRVL_Q222X_A0				0x0001
#define MRVL_Q222X_B0				0x0001

/* Current Speed */
#define MRVL_Q222X_1000BASE_T1			0x0001
#define MRVL_Q222X_100BASE_T1			0x0000

/* Operation mode */
#define MRVL_Q222X_MASTER			0x0001
#define MRVL_Q222X_SLAVE			0x0000

/* Auto-Negotiation Controls - reg 7.0x0200 */
#define MRVL_Q222X_AN_DISABLE			0x0000
#define MRVL_Q222X_AN_RESET			0x8000
#define MRVL_Q222X_AN_ENABLE			0x1000
#define MRVL_Q222X_AN_RESTART			0x0200

/* Auto-Negotiation Status - reg 7.0x0201 */
#define MRVL_Q222X_AN_COMPLETE			0x0020

/* Auto-Negotiation Flags - reg 7.0x0203 */
#define MRVL_Q222X_AN_FE_ABILITY		0x0020
#define MRVL_Q222X_AN_GE_ABILITY		0x0080
#define MRVL_Q222X_AN_PREFER_MASTER		0x0010

#define PHY_ID_88Q222X				0x002b0b21
#define PHY_NAME				"88Q222X"
#define MRVL_ORGANIZATION_ID			0x2B
#define MRVL_APHY_OP_MASTER			1

/**
 * get_speed - Get current data rate.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 *
 * This function will check if the PHY is operating at 1000 BASE-T1
 * or 100 BASE-T1.
 *
 * Return: 0x1 if 1000 BASE-T1, 0x0 if 100 BASE-T1.
 */
static u16 get_speed(struct phy_device *phydev)
{
	return (phy_read_mmd(phydev, 1, 0x0834) & 0x0001);
}

/**
 * rgmii_reset - Perform RGMII reset.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 *
 * This function resets the RGMII link between the ethernet controller and
 * the PHY.
 *
 * Return: void.
 */
static void rgmii_reset(struct phy_device *phydev)
{
	/* reset rgmii and disable tx_disable feature */
	phy_write_mmd(phydev, 3, 0x8000, 0x8000);

	/* clear rgmii reset and disable tx_disable feature */
	phy_write_mmd(phydev, 3, 0x8000, 0x0000);
}

/**
 * set_master_slave - Set Master/Slave mode of the PHY by software.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 * @force_master: Non-zero for master else its slave.
 *
 * This function will set the operation of the PHY as master or slave.
 *
 * Return: void.
 */
static void set_master_slave(struct phy_device *phydev, u16 force_master)
{
	u16 reg_data = phy_read_mmd(phydev, 1, 0x0834);

	if (force_master != 0x0)
		reg_data |= 0x4000;
	else
		reg_data &= 0xbfff;
	phy_write_mmd(phydev, 1, 0x0834, reg_data);
}

/**
 * check_link - Check link status of GE/speed 1000 or FE/speed 100.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 *
 * Get link status including PCS (local/remote) and local PMA to see if
 * it's ready to send traffic.
 *
 * Return: True if link is up, false otherwise.
 */
static bool check_link(struct phy_device *phydev)
{
	u16 reg_data, reg_data2, reg_data3 = 0;

	if (get_speed(phydev) == MRVL_Q222X_1000BASE_T1) {
		/* Read twice: register latches low value */
		reg_data = phy_read_mmd(phydev, 3, 0x0901);
		reg_data = phy_read_mmd(phydev, 3, 0x0901);
		/* local and remote receiver status */
		reg_data2 = phy_read_mmd(phydev, 7, 0x8001);
		/* local receiver status 2 */
		reg_data3 = phy_read_mmd(phydev, 3, 0xfd9d);

		return (0x0004 == (reg_data & 0x0004))	&& (0x3000 == (reg_data2 & 0x3000)) &&
			(0x0010 == (reg_data3 & 0x0010))  ? true : false;
	} else {
		/* link */
		reg_data = phy_read_mmd(phydev, 3, 0x8109);
		/* local and remote receiver status */
		reg_data2 = phy_read_mmd(phydev, 3, 0x8108);
		/* local receiver status 2 */
		reg_data3 = phy_read_mmd(phydev, 3, 0x8230);

		return (0x0004 == (reg_data & 0x0004)) && (0x3000 == (reg_data2 & 0x3000)) &&
			(0x0001 == (reg_data3 & 0x0001)) ? true : false;
	}
}

/**
 * q222x_soft_reset - Software Reset procedure for PHY.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 *
 * This function performs the software reset of the PHY based
 * on the operating speed.
 *
 * Return: void.
 */
static int q222x_soft_reset(struct phy_device *phydev)
{
	u32 speed = 0;

	speed = get_speed(phydev);
	if (speed == SPEED_100) {
		/* soft reset */
		phy_write_mmd(phydev, 3, 0x0900, 0x8000);
		phy_write_mmd(phydev, 3, 0xffe4, 0x000c);
	} else {
		/* enable RESET of DCL */
		phy_write_mmd(phydev, 3, 0xfe1b, 0x0048);
		/* soft reset */
		phy_write_mmd(phydev, 3, 0x0900, 0x8000);
		phy_write_mmd(phydev, 3, 0xffe4, 0x000c);
		/* disable RESET of DCL */
		phy_write_mmd(phydev, 3, 0xfe1b, 0x0058);
	}

	return 0;
}

/**
 * q222x_pre_init - Initialization sequence for the PHY.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 *
 * This functions performs the necessary initialization configuration for the
 * PHY.
 *
 * Return: void.
 */
static void q222x_pre_init(struct phy_device *phydev)
{
	u32 i = 0;

	/* enable txdac */
	phy_write_mmd(phydev, 3, 0x8033, 0x6801);
	/* disable ANEG */
	phy_write_mmd(phydev, 7, 0x0200, MRVL_Q222X_AN_DISABLE);
	/* set IEEE power down */
	phy_write_mmd(phydev, 1, 0x0000, 0x0840);
	/* exit standby state(internal state) */
	phy_write_mmd(phydev, 3, 0xfe1b, 0x0048);
	/* set power management state breakpoint (internal state) */
	phy_write_mmd(phydev, 3, 0xffe4, 0x06b6);
	/* exit IEEE power down */
	phy_write_mmd(phydev, 1, 0x0000, 0x0000);
	phy_write_mmd(phydev, 3, 0x0000, 0x0000);

	while (i < 5) {
		if (phy_read_mmd(phydev, 3, 0xffe4) == 0x06ba)
			break;
		i++;
		usleep_range(1000, 1050);
	}
}

/**
 * init_q222x_fe - Initialize for 100 BASE-T1.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 * @op_mode: Mode of operation.
 *
 * This functions performs the necessary initialization configuration for the
 * PHY to operate in 100 BASE-T1 mode.
 *
 * Return: void.
 */
static void init_q222x_fe(struct phy_device *phydev, int op_mode)
{
	u32 reg_data = 0;

	q222x_pre_init(phydev);

	/* update Initial FFE Coefficients  */
	phy_write_mmd(phydev, 3, 0xfbba, 0x0cb2);
	phy_write_mmd(phydev, 3, 0xfbbb, 0x0c4a);

	/* update speed and mode */
	reg_data = phy_read_mmd(phydev, 1, 0x0834);
	reg_data = (reg_data & 0xfff0);
	if (op_mode == MRVL_APHY_OP_MASTER)
		reg_data |= 0x4000;
	else
		reg_data &= 0xbfff;
	phy_write_mmd(phydev, 1, 0x0834, reg_data);

	q222x_soft_reset(phydev);
}

/**
 * init_q222x_ge - Initialize for 1000 BASE-T1.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 * @op_mode: Mode of operation.
 *
 * This functions performs the necessary initialization configuration for the
 * PHY to operate in 1000 BASE-T1 mode.
 *
 * Return: void.
 */
static void init_q222x_ge(struct phy_device *phydev, int op_mode)
{
	u32 reg_data = 0;

	q222x_pre_init(phydev);

	/* send_s detection threshold, slave and master */
	phy_write_mmd(phydev, 7, 0x8032, 0x2020);
	phy_write_mmd(phydev, 7, 0x8031, 0x0a28);
	phy_write_mmd(phydev, 7, 0x8031, 0x0c28);
	/* disable DCL calibration during tear down */
	phy_write_mmd(phydev, 3, 0xffdb, 0xfc10);
	/* disable RESET of DCL*/
	phy_write_mmd(phydev, 3, 0xfe1b, 0x0058);

	/* update speed and mode */
	reg_data = phy_read_mmd(phydev, 1, 0x0834);
	reg_data = (reg_data & 0xfff0) | 0x0001;
	if (op_mode == MRVL_APHY_OP_MASTER)
		reg_data |= 0x4000;
	else
		reg_data &= 0xbfff;
	phy_write_mmd(phydev, 1, 0x0834, reg_data);

	reg_data = phy_read_mmd(phydev, 3, 0x8021);
	reg_data = reg_data | (1 << 11) | (1 << 12);
	phy_write_mmd(phydev, 3, 0x8021, reg_data);

	q222x_soft_reset(phydev);
}

/**
 * q222x_dts_init - Initialize the phy based on the phydev of_node.
 * @phydev: Pointer to the phy_device structure which represents the PHY.
 *
 * Set and/or override some configuration registers based on the
 * marvell,88q222x property stored in the of_node for the phydev.
 * marvell,88q222x = <speed master>,...;
 * speed: 1000Mbps or 100Mbps.
 * mode: 1-master, 0-slave.
 *
 * Return: 0 if success and error value on failure.
 */
static int q222x_dts_init(struct phy_device *phydev)
{
	u32 speed  = SPEED_1000;
	u32 mode = 0;
	const __be32 *paddr;
	int len;

	if (!phydev->mdio.dev.of_node)
		return -ENOMEM;

	paddr = of_get_property(phydev->mdio.dev.of_node,
				"marvell,88q222x", &len);
	if (!paddr)
		return -ENOMEM;

	speed = be32_to_cpup(paddr);
	mode = be32_to_cpup(paddr + 1);

	/* Set speed */
	if (speed == SPEED_100)
		init_q222x_fe(phydev, mode);
	else
		init_q222x_ge(phydev, mode);
	/* Set master or slave */
	set_master_slave(phydev, mode);

	return 0;
}

static int q222x_timing_init(struct phy_device *phydev)
{
	/* force to disable tx&rx delay
	 * bit15:tx delay
	 * bit14:rx delay
	 */
	phy_write_mmd(phydev, 4, 0xa001, 0xc000);

	/* Software Reset of T-unit */
	rgmii_reset(phydev);

	return 0;
}

static int q222x_config_init(struct phy_device *phydev)
{
	u16 ret;

	phylink_set(phydev->supported, 1000baseT_Full);
	phylink_set(phydev->supported, 100baseT_Full);
	phydev->autoneg = AUTONEG_DISABLE;

	phydev_info(phydev, "%s: dts init and timing init!\n", PHY_NAME);
	ret = q222x_dts_init(phydev);
	phydev_info(phydev, "Q222X: q222x_dts_init return %d\n", ret);
	q222x_timing_init(phydev);
	phydev->state = PHY_READY;

	return 0;
}

static int q222x_read_status(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	char *inactive[2] = { "P_STATE=LINK_DOWN", NULL };
	char **uevent_envp = inactive;

	phydev->duplex = 1;
	phydev->pause = 0;

	uevent_envp = inactive;
	if (check_link(phydev)) {
		phydev->link = 1;
	} else {
		kobject_uevent_env(&dev->kobj, KOBJ_CHANGE, uevent_envp);
		phydev->link = 0;
	}
	if (get_speed(phydev) == MRVL_Q222X_1000BASE_T1)
		phydev->speed = SPEED_1000;
	else
		phydev->speed = SPEED_100;

	return 0;
}

static int q222x_resume(struct phy_device *phydev)
{
	int ret;

	if (phydev->state == PHY_HALTED) {
		/* Reconfigure PHY as power is lost in suspend */
		phydev_info(phydev, "%s: dts init and timing init!\n", PHY_NAME);
		ret = q222x_dts_init(phydev);
		phydev_info(phydev, "Q222X: q222x_dts_init return %d\n", ret);
		q222x_timing_init(phydev);
	}

	return 0;
}

static int q222x_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static int q222x_aneg_done(struct phy_device *phydev)
{
	return 0;
}

static int q222x_match_phy_device(struct phy_device *phydev)
{
	phydev_info(phydev, "trying %s device match,c45_ids=0x%x!\n",
		    PHY_NAME, phydev->c45_ids.device_ids[1]);
	return (phydev->c45_ids.device_ids[1] & 0xfffffff0) == (PHY_ID_88Q222X & 0xfffffff0);
}

static struct phy_driver marvell_88q222x_driver[] = {
	{
		.phy_id		= PHY_ID_88Q222X,
		.phy_id_mask	= 0xfffffff0,
		.name		= "Marvell 88Q222X",
		.soft_reset	= q222x_soft_reset,
		.config_init	= q222x_config_init,
		.config_aneg	= q222x_config_aneg,
		.aneg_done	= q222x_aneg_done,
		.match_phy_device = q222x_match_phy_device,
		.read_status	= q222x_read_status,
		.suspend	= genphy_suspend,
		.resume		= q222x_resume,
	}
};

module_phy_driver(marvell_88q222x_driver);
