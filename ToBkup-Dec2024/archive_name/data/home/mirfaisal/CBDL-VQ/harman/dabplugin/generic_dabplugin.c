/*
 *  dabplugin.c
 *  Copyright (C) 2016 Harman International Ltd,
 *  by Vikram N <vikram.narayanarao@harman.com>
 *  Created on: 12-Feb-2016
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2.0 as published by
 * the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#undef DEBUG
#define pr_fmt(fmt) "dabplugin: "fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/kfifo.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/sched/rt.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/pm_runtime.h>

#define CREATE_TRACE_POINTS
#include <trace/events/dab_spi.h>

#define DYNAMIC_MINORS          32      /* ... up to 256 */
#define FIFO_SIZE               32768
#define BUF_SIZE                8192
#define DAB_MAX_PKT_SIZE        5472
#define DAB_NUM_BOOT_MSG        2
#define DAB_MSG_HEADER_LEN      4
#define DAB_RATELIMIT_BURST	2	/* no of messages emitted */
#define DAB_RATELIMIT_INTERVAL	(30*HZ)	/* 30 seconds */

#ifndef CONFIG_OF
static unsigned int dab_cs_gpios[4][4] = {
	[1][0] = 5,
	[2][0] = 241,
	[3][0] = 429
};

static unsigned int dab_irq_gpios[4][4] = {
	[1][0] = 90,
	[2][0] = 230,
	[3][0] = 443
};
#endif

static unsigned char dabbootmsg[DAB_NUM_BOOT_MSG + 1][DAB_MSG_HEADER_LEN] = {
	{ 0xAB, 0xBC, 0xCD, 0xDE },
	{ 0xE2, 0x00, 0x10, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00 }
};

static DECLARE_BITMAP(minors, DYNAMIC_MINORS);
static LIST_HEAD(device_list);
static DEFINE_MUTEX(device_list_lock);

struct dabspi_data {
	dev_t			devt;
	struct spi_device	*spi;
	struct list_head	device_entry;
	struct spi_transfer	t;
	struct spi_message	m;
	u8			*tx_buf;
	u8			*rx_buf;
	struct mutex		rlock, wlock;
	struct kfifo		fifo;
	struct completion	data_avail;
	wait_queue_head_t	wq_txrx;
	bool			txrx_in_progress;
	u32			pktsize;
	u32			speed_hz;
	u32			cs_gpio;
	u32			irq_gpio;
	u32			gpio_irq_num;
	u32			users;
	u32			txlen;
	s32			status;
	bool			user_write;
	u8			*bootmsg;
};

static void dab_cs_assert(unsigned int gpio)
{
	gpio_set_value(gpio, 0);
}

static void dab_cs_deassert(unsigned int gpio)
{
	gpio_set_value(gpio, 1);
}

static unsigned int dab_check_header(unsigned char *dh, int len)
{
	return dh && (dh[0] == 0xAD) && (len >= DAB_MSG_HEADER_LEN)
	       && ((((dh[2] << 8) | dh[3]) + 2) <= DAB_MAX_PKT_SIZE);
}

static void dab_finalize_txrx(struct dabspi_data *dabspi)
{
	dab_cs_deassert(dabspi->cs_gpio);
	dabspi->txrx_in_progress = false;
	wake_up_interruptible(&dabspi->wq_txrx);
	enable_irq(dabspi->gpio_irq_num);
}

static int dab_spi_xchange(struct dabspi_data *dabspi, u8 *txbuf, u8 *rxbuf,
			   void *callback, u32 txlen)
{
	dabspi->t.tx_buf = txbuf;
	dabspi->t.rx_buf = rxbuf;
	dabspi->t.len = txlen;
	spi_message_init(&dabspi->m);
	dabspi->m.complete = callback;
	dabspi->m.context = dabspi;

	spi_message_add_tail(&dabspi->t, &dabspi->m);
	trace_dab_spi_xchange(jiffies);
	return (!dabspi->spi) ? -ESHUTDOWN : spi_async(dabspi->spi, &dabspi->m);
}

static int dab_fill_kfifo(struct dabspi_data *dabspi)
{
	int res;
	static DEFINE_RATELIMIT_STATE(dab_rl, DAB_RATELIMIT_INTERVAL,
					DAB_RATELIMIT_BURST);

	print_hex_dump_debug("DAB PLUGIN: RX PKT: ", DUMP_PREFIX_OFFSET, 16,
			     1, dabspi->rx_buf, dabspi->pktsize, false);
	if (kfifo_is_full(&dabspi->fifo)) {
		if (___ratelimit(&dab_rl, "dab_fill_kfifo"))
			pr_warn("FIFO is full droping data!\n");
		res = -ENOBUFS;
		goto no_space;
	}
	res = kfifo_avail(&dabspi->fifo);
	if (res < dabspi->pktsize) {
		if (___ratelimit(&dab_rl, "dab_fill_kfifo"))
			pr_warn("No free space [A:%d - R:%d] droping data!\n",
						res, dabspi->pktsize);
		goto no_space;
	}
	res = kfifo_in(&dabspi->fifo, dabspi->rx_buf, dabspi->pktsize);
	if (res != dabspi->pktsize) {
		pr_err("Error in pushing data into fifo [pktsize=%d, bytes pushed=%d]\n",
		       dabspi->pktsize, res);
		goto end;
	}
	res = 0;
no_space:
	complete(&dabspi->data_avail);
end:
	return res;
}

static void dab_rx_pkt_callback(void *data)
{
	struct dabspi_data *dabspi = data;
	int res = dabspi->m.status;

	trace_dab_rcv_hdr_callbck(jiffies);

	if (res) {
		pr_err("Async pkt read failed [%d]", res);
		goto ret;
	}

	res = dab_fill_kfifo(dabspi);

ret:
	dabspi->status = res;
	dab_finalize_txrx(dabspi);
}

static void dab_rx_header_callback(void *data)
{
	struct dabspi_data *dabspi = data;
	int res = dabspi->m.status;
	u32 pktlen;

	trace_dab_rcv_pkt_callbck(jiffies);

	if (res) {
		pr_err("Async header read failed [%d]", res);
		goto ret;
	}

	print_hex_dump_debug("DAB PLUGIN: RX: ", DUMP_PREFIX_OFFSET, 16, 1,
			     dabspi->rx_buf, dabspi->txlen, false);

	res = dab_check_header(dabspi->rx_buf, DAB_MSG_HEADER_LEN);
	if (!res) {
		if (!dabspi->user_write)
			pr_err("Invalid Packet from DAB\n");
		res = 0;
		goto ret;
	}

	pktlen = (dabspi->rx_buf[2] << 8) | dabspi->rx_buf[3];
	pktlen += 2; /* for checksum */
	dabspi->pktsize = pktlen + DAB_MSG_HEADER_LEN;

	pr_debug("RXD Len = %d DAB PKT LEN = %d\n", dabspi->txlen, pktlen);
	if (dabspi->txlen >= dabspi->pktsize) {
		pr_debug("Full Pkt Rxd\n");
		res = dab_fill_kfifo(dabspi);
		goto ret;
	} else {
		pr_debug("Partial Pkt Rxd. Pending %d bytes\n",
			 pktlen - DAB_MSG_HEADER_LEN);
		res = dab_spi_xchange(dabspi, NULL,
				      dabspi->rx_buf + dabspi->txlen,
				      dab_rx_pkt_callback,
				      pktlen -
				      (dabspi->txlen - DAB_MSG_HEADER_LEN));
		if (res) {
			pr_err("Error reading pending data\n");
			goto ret;
		}
	}
	dabspi->status = res;
	return;

ret:
	dabspi->status = res;
	dab_finalize_txrx(dabspi);
}

static int dab_send_cmd(struct dabspi_data *dabspi)
{
	int res;
	u8 *txbuf = (dabspi->user_write) ? dabspi->tx_buf : dabspi->bootmsg;

	dab_cs_assert(dabspi->cs_gpio);
	res = dab_spi_xchange(dabspi, txbuf, dabspi->rx_buf,
			      dab_rx_header_callback, dabspi->txlen);
	if (res) {
		pr_err("xchange failed[%d]\n", res);
		goto ret;
	}
	return res;
ret:
	dab_finalize_txrx(dabspi);
	return res;
}

static irqreturn_t dab_irq_handler(int irq, void *data)
{
	struct dabspi_data *dabspi = data;
	static u64 irqs;

	disable_irq_nosync(dabspi->gpio_irq_num);
	dabspi->bootmsg = (unlikely(irqs < 2)) ? dabbootmsg[irqs] : NULL;
	dabspi->txrx_in_progress = true;
	dabspi->user_write = false;
	dabspi->txlen = DAB_MSG_HEADER_LEN;
	dab_send_cmd(dabspi);
	irqs = (likely(irqs + 1 > irqs)) ? irqs + 1 : 2;

	trace_dab_spi_irq(jiffies);
	pr_debug("IRQ == %llu\n", irqs);

	return IRQ_HANDLED;
}

static int dab_open(struct inode *i, struct file *f)
{
	struct dabspi_data *dabspi;
	int status = -ENXIO;

	mutex_lock(&device_list_lock);
	list_for_each_entry(dabspi, &device_list, device_entry) {
		if (dabspi->devt == i->i_rdev) {
			status = 0;
			break;
		}
	}
	if (status) {
		pr_debug("nothing for minor %d\n", iminor(i));
		goto err_find_dev;
	}
	if (dabspi->users != 0) {
		status = -EBUSY;
		goto err_find_dev;
	}
	dabspi->users++;
	f->private_data = dabspi;

err_find_dev:
	mutex_unlock(&device_list_lock);
	return status;
}

static ssize_t dab_read(struct file *f, char __user *buf, size_t count,
			loff_t *offset)
{
	struct dabspi_data *dabspi = f->private_data;
	ssize_t status = 0;
	unsigned int copied = 0;

	if (!(dabspi && dabspi->spi))
		return -ESHUTDOWN;
	if (count > FIFO_SIZE)
		return -EMSGSIZE;

	mutex_lock(&dabspi->rlock);

	while (kfifo_is_empty(&dabspi->fifo)) {
		if (f->f_flags & O_NONBLOCK) {
			copied = -EAGAIN;
			goto ret;
		}
		pr_debug("No Data available in Kfifo, going to wait\n");
		status = wait_for_completion_interruptible(&dabspi->data_avail);
		if (status) {
			pr_err("Read was interrupted [%zd]\n", status);
			copied = status;
			goto ret;
		}
		if(!dabspi->users)
		{
			copied = -ESHUTDOWN;
			goto ret;
		}
		if (!dabspi->spi) {
			copied = -ESHUTDOWN;
			goto ret;
		}
	}
	status = kfifo_to_user(&dabspi->fifo, buf, count, &copied);
	if (status) {
		pr_err("Error Reading %zu data from kfifo\n", count);
		copied = status;
		goto ret;
	}
ret:
	mutex_unlock(&dabspi->rlock);
	if (!copied) {
		pr_err("Error: Read Returning 0!!!\n");
		pr_err("buf[%p] count[%zu], offset[%p], res[%zd], copied[%d]\n",
		       buf, count, offset, status, copied);
		pr_err("kfifo is %s\n",
		       kfifo_is_empty(&dabspi->fifo) ? "empty" : "not empty");
		pr_err("kfifo_len = %d\n",
		       kfifo_len(&dabspi->fifo));
		pr_err("kfifo.in = %d kfifo.out = %d kfifo.mask = %d kfifo.esize = %d kfifo.data = %p\n",
			dabspi->fifo.kfifo.in,
			dabspi->fifo.kfifo.out, dabspi->fifo.kfifo.mask,
			dabspi->fifo.kfifo.esize, dabspi->fifo.kfifo.data);
	}
	return copied;
}

static ssize_t dab_write(struct file *f, const char __user *buf, size_t count,
			 loff_t *offset)
{
	struct dabspi_data *dabspi = f->private_data;
	int status = 0;

	if (!(dabspi && dabspi->spi))
		return -ESHUTDOWN;
	if (count > BUF_SIZE)
		return -EMSGSIZE;

	mutex_lock(&dabspi->wlock);

	status = copy_from_user(dabspi->tx_buf, buf, count);
	if (status) {
		pr_err("Error in accessing user buffer to write\n");
		goto unlock_n_ret;
	}
	print_hex_dump_debug("DAB PLUGIN : TX APP : ", DUMP_PREFIX_OFFSET,
			     16, 1, dabspi->tx_buf, count, false);

	disable_irq(dabspi->gpio_irq_num);
	status = wait_event_interruptible(dabspi->wq_txrx,
						!dabspi->txrx_in_progress);
	if (status) {
		pr_err("Write was interrupted while waiting [%d]\n", status);
		enable_irq(dabspi->gpio_irq_num);
		goto unlock_n_ret;
	}

	dabspi->txrx_in_progress = true;
	dabspi->txlen = count;
	dabspi->user_write = true;
	status = dab_send_cmd(dabspi);
	if (status) {
		pr_err("error while sending cmd\n");
		goto unlock_n_ret;
	}
	status = wait_event_interruptible(dabspi->wq_txrx,
						!dabspi->txrx_in_progress);
	if (status) {
		pr_err("Write was interrupted [%d]\n", status);
	}
	status = dabspi->status;

unlock_n_ret:
	mutex_unlock(&dabspi->wlock);
	return (status) ? status : count;
}

static int dab_release(struct inode *i, struct file *f)
{
	struct dabspi_data *dabspi;

	mutex_lock(&device_list_lock);
	dabspi = f->private_data;
	f->private_data = NULL;
	if(dabspi->users > 0)
		dabspi->users--;
	complete_all(&dabspi->data_avail);
	mutex_unlock(&device_list_lock);
	return 0;
}

static struct class *dabspi_class;
static int dab_major;

static int dab_add_dev(struct spi_device *spi, struct dabspi_data *dabspi)
{
	int status;
	unsigned long minor;
	struct device *dev;

	mutex_lock(&device_list_lock);
	minor = find_first_zero_bit(minors, DYNAMIC_MINORS);
	if (minor >= DYNAMIC_MINORS) {
		dev_err(&spi->dev, "no minor number available!\n");
		status = -ENODEV;
		goto end;
	}
	dabspi->devt = MKDEV(dab_major, minor);
	dev = device_create(dabspi_class, &spi->dev, dabspi->devt, dabspi,
			    "spi0");
	status = PTR_ERR_OR_ZERO(dev);
	if (status) {
		pr_err("Could not create device file for minor %lu\n", minor);
		goto end;
	}
	set_bit(minor, minors);
	list_add(&dabspi->device_entry, &device_list);
end:
	mutex_unlock(&device_list_lock);
	return status;
}

static int dab_alloc_res(struct dabspi_data *dabspi)
{
	int status;

	status = kfifo_alloc(&dabspi->fifo, FIFO_SIZE, GFP_KERNEL);
	if (status) {
		pr_err("kfifo_alloc failed\n");
		return status;
	}

	dabspi->tx_buf = kzalloc(BUF_SIZE, GFP_DMA);
	if (!dabspi->tx_buf)
		goto err_txbuf_alloc;

	dabspi->rx_buf = kzalloc(BUF_SIZE, GFP_DMA);
	if (!dabspi->rx_buf)
		goto err_rxbuf_alloc;

	return status;

err_rxbuf_alloc:
	kfree(dabspi->tx_buf);
err_txbuf_alloc:
	kfifo_free(&dabspi->fifo);
	return -ENOMEM;
}

static void dab_dealloc_res(struct dabspi_data *dabspi)
{
	kfifo_free(&dabspi->fifo);
	kfree(dabspi->tx_buf);
	kfree(dabspi->rx_buf);
}

static int dab_gpio_alloc(struct dabspi_data *dabspi)
{
	int status;
	struct irq_desc *dab_irq_desc;
	struct sched_param param = { .sched_priority = MAX_RT_PRIO - 1 };

	if (!gpio_is_valid(dabspi->cs_gpio)) {
		pr_err("Invalid cs gpio number %d\n", dabspi->cs_gpio);
		goto err_gpio_invalid;
	}
	if (!gpio_is_valid(dabspi->irq_gpio)) {
		pr_err("Invalid irq gpio number %d\n", dabspi->irq_gpio);
		goto err_gpio_invalid;
	}
	status = gpio_request(dabspi->cs_gpio, "dab_cs_pin");
	if (status) {
		pr_err("Claiming gpio pin %d failed!\n", dabspi->cs_gpio);
		goto err_gpio_invalid;
	}
	status = gpio_request(dabspi->irq_gpio, "dab_irq_pin");
	if (status) {
		pr_err("Claiming gpio pin %d failed!\n", dabspi->irq_gpio);
		goto err_gpio_invalid;
	}
	status = gpio_direction_output(dabspi->cs_gpio, 1);
	if (status) {
		pr_err("Setting direction gpio %d failed\n", dabspi->cs_gpio);
		goto err_gpio_dir;
	}
	status = gpio_direction_input(dabspi->irq_gpio);
	if (status) {
		pr_err("Setting direction gpio %d failed\n", dabspi->irq_gpio);
		goto err_gpio_dir;
	}
	dabspi->gpio_irq_num = gpio_to_irq(dabspi->irq_gpio);
	status = request_irq(dabspi->gpio_irq_num, dab_irq_handler,
			     IRQF_TRIGGER_FALLING, "dabplugin", dabspi);
	if (status) {
		pr_err("requesting irq failed for irqnum = %d (gpio num = %d)\n",
			dabspi->gpio_irq_num, dabspi->irq_gpio);
		goto err_gpio_dir;
	}
	dab_irq_desc = irq_to_desc(dabspi->gpio_irq_num);
	sched_setscheduler(dabspi->spi->master->kworker_task, SCHED_FIFO,
			   &param);

	return status;

err_gpio_dir:
	gpio_free(dabspi->cs_gpio);
	gpio_free(dabspi->irq_gpio);
err_gpio_invalid:
	status = -EIO;
	return status;
}

static void dab_gpio_dealloc(struct dabspi_data *dabspi)
{
	free_irq(dabspi->gpio_irq_num, dabspi);
	gpio_free(dabspi->cs_gpio);
	gpio_free(dabspi->irq_gpio);
}

#ifdef CONFIG_OF
static u32 previous_mode;
static void __iomem *addr;
static void dab_set_pinmux(u32 paddr, u32 val)
{
	addr = ioremap(paddr, sizeof(val));
	if (!addr) {
		pr_err("can't ioremap\n");
		return;
	}
	previous_mode = readl(addr);
	writel(val, addr);
	pr_debug("Pinmux set Reg:%x, val:%x\n", paddr, val);
}

static void dab_restore_pinmux(void)
{
	if (addr) {
		writel(previous_mode, addr);
		iounmap(addr);
	}
}

static int dab_read_of_node(struct dabspi_data *dabspi)
{
	struct device_node *dn = of_find_node_by_name(NULL, "dabplugin");
	int rc;
	u32 value, cs_reg;
	const __be32 *p = NULL;
	struct property *prop;

	rc = of_property_read_u32(dn, "dab-cs-gpio", &value);
	if (rc) {
		pr_err("%s has no valid 'dab-cs-gpio' property (%d)\n",
		       dn->full_name, rc);
		return rc;
	}
	dabspi->cs_gpio = value;

	rc = of_property_read_u32(dn, "dab-irq-gpio", &value);
	if (rc) {
		pr_err("%s has no valid 'dab-irq-gpio' property (%d)\n",
		       dn->full_name, rc);
		return rc;
	}
	dabspi->irq_gpio = value;

	prop = of_find_property(dn, "dab-cs-pinmux", NULL);
	if (prop) {
		p = of_prop_next_u32(prop, p, &cs_reg);
		if (p && of_prop_next_u32(prop, p, &value))
			dab_set_pinmux(cs_reg, value);
	}

	if (of_find_property(dn, "dab-pm-disable", NULL))
		pm_runtime_forbid(&dabspi->spi->master->dev);

	return rc;
}
#endif

static int dab_spi_probe(struct spi_device *spi)
{
	int status;
	struct dabspi_data *dabspi;

	dabspi = kzalloc(sizeof(*dabspi), GFP_KERNEL);
	if (!dabspi)
		return -ENOMEM;

	/* Initialize the driver data */
	dabspi->spi = spi;
	mutex_init(&dabspi->rlock);
	mutex_init(&dabspi->wlock);
	INIT_LIST_HEAD(&dabspi->device_entry);
	init_completion(&dabspi->data_avail);
	init_waitqueue_head(&dabspi->wq_txrx);

	dabspi->speed_hz = spi->max_speed_hz;
#ifdef CONFIG_OF
	status = dab_read_of_node(dabspi);
	if (status)
		goto err;
#else
	dabspi->cs_gpio = dab_cs_gpios[spi->master->bus_num][spi->chip_select];
	dabspi->irq_gpio =
		dab_irq_gpios[spi->master->bus_num][spi->chip_select];
#endif
	spi_set_drvdata(spi, dabspi);

	status = dab_alloc_res(dabspi);
	if (status)
		goto err;

	status = dab_gpio_alloc(dabspi);
	if (status)
		goto err_res;

	status = dab_add_dev(spi, dabspi);
	if (status)
		goto err_dev;

	return status;

err_dev:
	dab_gpio_dealloc(dabspi);
err_res:
	dab_dealloc_res(dabspi);
err:
	kfree(dabspi);
	return status;
}

static int dab_spi_remove(struct spi_device *dev)
{
	struct dabspi_data *dabspi = spi_get_drvdata(dev);

	if (!dabspi)
		goto end;

	pm_runtime_allow(&dabspi->spi->master->dev);
	synchronize_irq(dabspi->gpio_irq_num);
	dabspi->spi = NULL;
	complete_all(&dabspi->data_avail);
	wake_up_interruptible_all(&dabspi->wq_txrx);
	mutex_lock(&device_list_lock);
	list_del(&dabspi->device_entry);
	device_destroy(dabspi_class, dabspi->devt);
	clear_bit(MINOR(dabspi->devt), minors);
	mutex_unlock(&device_list_lock);
	dab_dealloc_res(dabspi);
	dab_gpio_dealloc(dabspi);
	kfree(dabspi);
end:
#ifdef CONFIG_OF
	dab_restore_pinmux();
#endif
	return 0;
}


#ifdef CONFIG_OF
static const struct of_device_id dabplugin_of_match[] = {
	{
		.compatible = "dabplugin",
	},
	{}
};
MODULE_DEVICE_TABLE(of, dabplugin_of_match);
#endif

static const struct spi_device_id dab_spi_id[] = {
	{ "dabplugin", 0 },
	{}
};
MODULE_DEVICE_TABLE(spi, dab_spi_id);

static struct spi_driver dab_spi_driver = {
	.driver			= {
		.name		= "dabplugin",
		.owner		= THIS_MODULE,
		.of_match_table = of_match_ptr(dabplugin_of_match),
	},
	.probe			= dab_spi_probe,
	.remove			= dab_spi_remove,
	.id_table		= dab_spi_id,
};

static const struct file_operations dab_fops = {
	.owner		= THIS_MODULE,
	.open		= dab_open,
	.read		= dab_read,
	.write		= dab_write,
	.release	= dab_release,
	.llseek		= no_llseek,
};

static int __init dab_spi_init(void)
{
	int status;

	dab_major = register_chrdev(0, "dabspi", &dab_fops);
	if (dab_major < 0) {
		pr_err("Device registration failed\n");
		return dab_major;
	}

	dabspi_class = class_create(THIS_MODULE, "dabspi");
	if (IS_ERR(dabspi_class)) {
		status = PTR_ERR(dabspi_class);
		goto class_fail;
	}

	status = spi_register_driver(&dab_spi_driver);
	if (status < 0)
		goto spi_reg_fail;

	return status;

spi_reg_fail:
	class_destroy(dabspi_class);
class_fail:
	unregister_chrdev(dab_major, dab_spi_driver.driver.name);
	return status;
}
module_init(dab_spi_init);

static void __exit dab_spi_exit(void)
{
	spi_unregister_driver(&dab_spi_driver);
	class_destroy(dabspi_class);
	unregister_chrdev(dab_major, dab_spi_driver.driver.name);
}
module_exit(dab_spi_exit);

MODULE_AUTHOR("Vikram N <vikram.narayanarao@harman.com>");
MODULE_DESCRIPTION("Driver for DAB SPI interface");
MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:spidabplugin");
