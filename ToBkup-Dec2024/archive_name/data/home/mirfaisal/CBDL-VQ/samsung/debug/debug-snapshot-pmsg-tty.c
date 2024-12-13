// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Based on ttynull.c:
 *	Copyright (C) 2019 Axis Communications AB
 */

#include <linux/console.h>
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/mutex.h>

#include "debug-snapshot-local.h"

static const struct tty_port_operations dss_pmsg_tty_port_ops;
static struct tty_driver *dss_pmsg_tty_driver;
static struct tty_port dss_pmsg_tty_port;

static int dss_pmsg_tty_open(struct tty_struct *tty, struct file *filp)
{
	return tty_port_open(&dss_pmsg_tty_port, tty, filp);
}

static void dss_pmsg_tty_close(struct tty_struct *tty, struct file *filp)
{
	tty_port_close(&dss_pmsg_tty_port, tty, filp);
}

static void dss_pmsg_tty_hangup(struct tty_struct *tty)
{
	tty_port_hangup(&dss_pmsg_tty_port);
}

static int dss_pmsg_tty_write(struct tty_struct *tty, const unsigned char *buf,
			 int count)
{
	char arch_time_buf[SZ_256];
	int arch_time_buf_offset = 0;

	arch_time_buf_offset +=
		dss_print_time(dss_arch_timer_get_ns(), arch_time_buf);

	dbg_snapshot_hook_logger(arch_time_buf, arch_time_buf_offset,
			DSS_ITEM_PLATFORM_ID);
	dbg_snapshot_hook_logger(buf, count, DSS_ITEM_PLATFORM_ID);

	return count;
}

static unsigned int dss_pmsg_tty_write_room(struct tty_struct *tty)
{
	return 65536;
}

static const struct tty_operations dss_pmsg_tty_ops = {
	.open = dss_pmsg_tty_open,
	.close = dss_pmsg_tty_close,
	.hangup = dss_pmsg_tty_hangup,
	.write = dss_pmsg_tty_write,
	.write_room = dss_pmsg_tty_write_room,
};

static struct tty_driver *dss_pmsg_tty_device(struct console *c, int *index)
{
	*index = 0;
	return dss_pmsg_tty_driver;
}

static struct console dss_pmsg_tty_console = {
	.name = "pmsg_tty_console",
	.device = dss_pmsg_tty_device,
};

int dss_pmsg_tty_init(void)
{
	struct tty_driver *driver;
	int ret;

	driver = tty_alloc_driver(1, TTY_DRIVER_UNNUMBERED_NODE);
	if (IS_ERR(driver))
		return PTR_ERR(driver);

	tty_port_init(&dss_pmsg_tty_port);
	dss_pmsg_tty_port.ops = &dss_pmsg_tty_port_ops;

	driver->driver_name = "pmsg_tty";
	driver->name = "pmsg_tty";
	driver->type = TTY_DRIVER_TYPE_CONSOLE;
	driver->init_termios = tty_std_termios;
	driver->init_termios.c_oflag = OPOST | OCRNL | ONOCR | ONLRET;
	tty_set_operations(driver, &dss_pmsg_tty_ops);
	tty_port_link_device(&dss_pmsg_tty_port, driver, 0);

	ret = tty_register_driver(driver);
	if (ret < 0) {
		tty_driver_kref_put(driver);
		tty_port_destroy(&dss_pmsg_tty_port);
		return ret;
	}

	dss_pmsg_tty_driver = driver;
	register_console(&dss_pmsg_tty_console);

	return 0;
}

void dss_pmsg_tty_exit(void)
{
	unregister_console(&dss_pmsg_tty_console);
	tty_unregister_driver(dss_pmsg_tty_driver);
	tty_driver_kref_put(dss_pmsg_tty_driver);
	tty_port_destroy(&dss_pmsg_tty_port);
}
