// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * EXYNOS - sfi ipc driver support to communicate with the SFI CPU
 */

#include <linux/err.h>
#include <linux/export.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/sched/clock.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <soc/samsung/sfi_ipc_ctrl.h>
#include "sfi_ipc_c1.h"

#define SFI_IPC_C1_DBG_NAME "[sfi_ipc_c1]"

static struct sfi_ipc_c1_info *sfi_ipc_c1;
static const struct sfi_ipc_c1_reg_data *sfi_c1_ofs;
void __iomem *c1_inform_regs;

/* Static functions */

static void channel_tx_set(struct sfi_ipc_c1_ch *channel, unsigned int tx_id)
{
	channel->tx_id = tx_id;
}

static void channel_rx_set(struct sfi_ipc_c1_ch *channel, unsigned int rx_id)
{
	channel->rx_id = rx_id;
}

static void channel_info_set(struct sfi_ipc_c1_ch *channel,
							 unsigned char *name_data,
							 sfi_ipc_c1_cb mailbox_rcv_cb,
							 struct sfi_ipc_c1_buf_info *rbptr)
{
	memcpy(channel->channel_name, name_data, SFI_IPC_C1_NAME_SIZE);
	channel->callback = mailbox_rcv_cb;
	channel->rx_buf = rbptr;
}

static void channel_reset(struct sfi_ipc_c1_ch *channel)
{
	channel->tx_id = -EINVAL;
	channel->rx_id = -EINVAL;
	memset(channel->channel_name, 0, SFI_IPC_C1_NAME_SIZE);
	channel->callback = NULL;
	channel->rx_buf = NULL;
	channel->send_timeout = false;
	channel->assign_rcvd = false;
	channel->assign_sent = false;
}

/* Busy wait until mailbox becomes idle state */
static int wait_for_ipc(struct sfi_ipc_c1_ch *channel)
{
	volatile int intsr0_data;
	volatile int sr0_data;
	unsigned long long timeout;
	unsigned long long now;

	timeout = sched_clock() + SFI_IPC_C1_TIMEOUT;
	/* Check INTSR0 and SR0 to make sure mailbox is in idle state */
	intsr0_data = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->intsr0);
	sr0_data = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->sr0);
	while ((intsr0_data != 0) || (sr0_data != -SFI_IPC_C1_E_NODATA)) {
		now = sched_clock();
		if (timeout <= now) {
			channel->send_timeout = true;

			return -1;
		}

		/* INTSR0 must be zero */
		intsr0_data = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->intsr0);
		/* SR0 must be -SFI_IPC_E_NODATA */
		sr0_data = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->sr0);
	}

	return 0;
}

/* Write SR0 to show no data */
static int wait_for_ack(struct sfi_ipc_c1_ch *channel, int msg_len)
{
	volatile int sr0_data;
	int ret = 0;
	unsigned long long timeout;
	unsigned long long now;

	timeout = sched_clock() + SFI_IPC_C1_TIMEOUT;
	sr0_data = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->sr0);
	while (sr0_data == msg_len) {
		/* MsgLen is modified for both success and fail */
		now = sched_clock();
		if (timeout <= now) {
			/* Opponent doesn't response */
			__raw_writel(0xFFFF0000, sfi_ipc_c1->intr + sfi_c1_ofs->intcr0);
			channel->send_timeout = true;
			break;
		}
		/* delay 1us */
		udelay(1);
		/* data in sr0 should be modified in any cases */
		sr0_data = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->sr0);
	}

	/* data in shared register is invalid anyway */
	__raw_writel(-SFI_IPC_C1_E_NODATA, sfi_ipc_c1->intr + sfi_c1_ofs->sr0);

	if (sr0_data != SFI_IPC_C1_E_HANDLED) {
		/* Message is dropped */
		ret = -1;
	}

	return ret;
}

static int send_message(struct sfi_ipc_c1_ch *channel,
						struct sfi_ipc_c1_buf_info *tbptr)
{
	int ret;
	int msg_len;
	int cpy_len;

	tbptr->cur_size = 0;

	do {
		ret = wait_for_ipc(channel);
		if (ret != 0) {
			return -ETIMEDOUT;
		}

		if (channel->send_timeout == true) {
			/* report send timedout before */
			msg_len = -SFI_IPC_C1_E_TIMEDOUT;
			__raw_writel(msg_len, sfi_ipc_c1->intr + sfi_c1_ofs->sr0);
			__raw_writel((1 << (channel->tx_id)) << 16, sfi_ipc_c1->intr + sfi_c1_ofs->intgr0);

			ret = wait_for_ack(channel, msg_len);
			if (ret != 0) {
				return -EIO;
			}

			channel->send_timeout = false;
		} else {
			int i;
			unsigned int *dst;
			unsigned int *src;

			/* send message */
			msg_len = tbptr->size - tbptr->cur_size;
			if (msg_len >= SFI_IPC_C1_SIZE) {
				cpy_len = SFI_IPC_C1_SIZE;
				src = (unsigned int *)(&tbptr->addr[tbptr->cur_size]);
			} else {
				cpy_len = msg_len;
				memset(channel->int_snd, 0, SFI_IPC_C1_SIZE);
				memcpy(channel->int_snd, &tbptr->addr[tbptr->cur_size], cpy_len);
				src = (unsigned int *)(channel->int_snd);
			}

			dst = (unsigned int *)(sfi_ipc_c1->intr + sfi_c1_ofs->sr1);
			for (i = 0; i < SFI_IPC_C1_SIZE; i += sizeof(int)) {
				__raw_writel(*src, dst);
				src++;
				dst++;
			}

			__raw_writel(msg_len, sfi_ipc_c1->intr + sfi_c1_ofs->sr0);
			__raw_writel((1 << (channel->tx_id)) << 16, sfi_ipc_c1->intr + sfi_c1_ofs->intgr0);

			ret = wait_for_ack(channel, msg_len);
			if (ret != 0) {
				return -EIO;
			}

			tbptr->cur_size += cpy_len;
			msg_len -= cpy_len;
		}
	} while (msg_len);

	return 0;
}

static int send_assign(struct sfi_ipc_c1_ch *channel)
{
	int ret;
	struct sfi_ipc_c1_ch *assign_ch;
	struct sfi_ipc_c1_buf_info assign_buf;
	struct sfi_ipc_c1_assign_msg assign_msg;

	assign_ch = &sfi_ipc_c1->channel[SFI_IPC_C1_ASSIGN];
	assign_buf.addr = (unsigned char *)&assign_msg;
	assign_buf.size = SFI_IPC_C1_ASSIGN_MSG_SIZE;

	memset(assign_msg.channel_name, 0, SFI_IPC_C1_NAME_SIZE);
	memcpy(assign_msg.channel_name, channel->channel_name, SFI_IPC_C1_NAME_SIZE);
	assign_msg.tx_id = channel->tx_id;
	assign_msg.rx_id = channel->rx_id;

	spin_lock(&sfi_ipc_c1->ipc_lock);

	ret = send_message(assign_ch, &assign_buf);
	if (ret == 0) {
		channel->assign_sent = true;
	} else {
		channel->assign_sent = false;
	}

	spin_unlock(&sfi_ipc_c1->ipc_lock);

	return ret;
}

/* Static callbacks */

/* callbacks are executed in interrupt context */
static void assign_rx_callback(struct sfi_ipc_c1_buf_info *arg)
{
	int i;
	struct sfi_ipc_c1_assign_msg *assign_msgptr;
	struct sfi_ipc_c1_ch *channel;

	if (arg == NULL || arg->addr == NULL) {
		pr_info("%s %s null input pointer\n", SFI_IPC_C1_DBG_NAME, __func__);

		return;
	}

	if (arg->cur_size != SFI_IPC_C1_ASSIGN_MSG_SIZE) {
		pr_info("%s %s size mismatch\n", SFI_IPC_C1_DBG_NAME, __func__);

		return;
	}

	assign_msgptr = (struct sfi_ipc_c1_assign_msg *)arg->addr;

	for (i = MIN_USER_CHAN; i < MAX_USER_CHAN; i++) {
		channel = &sfi_ipc_c1->channel[i];

		if (!memcmp(channel->channel_name, assign_msgptr->channel_name,
					SFI_IPC_C1_NAME_SIZE)) {
			if (assign_msgptr->tx_id >= MIN_USER_CHAN &&
					assign_msgptr->tx_id < MAX_USER_CHAN) {
				channel_rx_set(channel, assign_msgptr->tx_id);
				channel->assign_rcvd = true;
			}

			if (channel->rx_id != assign_msgptr->tx_id ||
					channel->tx_id != assign_msgptr->rx_id) {
				/* Id is not matching */
				channel->assign_sent = false;
			}
		}
	}
}

/* API */

void sfi_ipc_c1_debug(void)
{
	int i;
	struct sfi_ipc_c1_ch *channel;
	struct sfi_ipc_c1_assign_msg *assign_rxptr;

	channel = &sfi_ipc_c1->channel[0];
	assign_rxptr = (struct sfi_ipc_c1_assign_msg *)channel->rx_buf->addr;

	pr_debug("%s %s [last assign_msg] %s tx_id: %x rx_id: %x\n\n",
			SFI_IPC_C1_DBG_NAME, __func__,
			assign_rxptr->channel_name,
			assign_rxptr->tx_id, assign_rxptr->rx_id);

	for (i = MIN_USER_CHAN; i < MAX_USER_CHAN; i++) {
		channel = &sfi_ipc_c1->channel[i];

		if (channel) {
			if (channel->channel_name[0] != '\0' ||
					channel->tx_id != -EINVAL || channel->rx_id != -EINVAL) {
				pr_debug("%s %s [%d: %s] tx_id: %x, rx_id: %x hsrcvd: %d hssent: %d\n",
						SFI_IPC_C1_DBG_NAME, __func__,
						i, channel->channel_name,
						channel->tx_id, channel->rx_id,
						(int)channel->assign_rcvd, (int)channel->assign_sent);
			}
		} else {
			pr_debug("%s %s [%d] channel pointer is NULL\n",
					SFI_IPC_C1_DBG_NAME, __func__, i);
		}
	}
}
EXPORT_SYMBOL(sfi_ipc_c1_debug);

int sfi_ipc_c1_reset(void)
{
	int c1_inform_val;
	unsigned int status;

	if (sfi_c1_ofs->c_v920) {
		if (c1_inform_regs == NULL) {
			c1_inform_regs = ioremap(0x11860800, 20);

			if (IS_ERR(c1_inform_regs))
				return PTR_ERR(c1_inform_regs);
		}
		c1_inform_val = ((__raw_readl((c1_inform_regs + 0x8)) >> 16) & 0x1uL);

		if (c1_inform_val == 0x0uL) {
			pr_info("%s %s sfi hasn't booted yet\n", SFI_IPC_C1_DBG_NAME, __func__);

			return -EPERM;
		}
	} else {
		__raw_writel(1, sfi_ipc_c1->intr);
	}
	/* SFI IPC INTERRUPT PENDING CLEAR */
	status = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->intsr1);
	pr_info("%s %s still probe intsr1:%x\n", SFI_IPC_C1_DBG_NAME, __func__, status);
	__raw_writel(status, sfi_ipc_c1->intr + sfi_c1_ofs->intcr1);

	__raw_writel(-SFI_IPC_C1_E_NODATA, sfi_ipc_c1->intr + sfi_c1_ofs->sr0);
	__raw_writel(-SFI_IPC_C1_E_NODATA, sfi_ipc_c1->intr + sfi_c1_ofs->sr3);

	return 0;
}
EXPORT_SYMBOL(sfi_ipc_c1_reset);

int sfi_ipc_c1_open(char *channel_name, sfi_ipc_c1_cb mailbox_rcv_cb,
				 struct sfi_ipc_c1_buf_info *rbptr)
{
	int i;
	struct sfi_ipc_c1_ch *channel;
	unsigned char name_data[SFI_IPC_C1_NAME_SIZE + 1];
	int c1_inform_val;

	if (channel_name == NULL || mailbox_rcv_cb == NULL || rbptr == NULL) {
		pr_info("%s %s null input pointer\n", SFI_IPC_C1_DBG_NAME, __func__);

		return -EINVAL;
	}

	if (channel_name[0] == '\0') {
		pr_info("%s %s invalid channel name\n",	SFI_IPC_C1_DBG_NAME, __func__);

		return -EINVAL;
	}

	if (rbptr) {
		if (rbptr->size == 0) {
			pr_info("%s %s receive buffer size is zero\n",
					SFI_IPC_C1_DBG_NAME, __func__);

			return -EINVAL;
		}

		memset(rbptr->addr, 0, rbptr->size);
		rbptr->cur_size = 0;
	}

	if (sfi_c1_ofs->c_v920) {
		if (c1_inform_regs == NULL) {
			c1_inform_regs = ioremap(0x11860800, 20);

			if (IS_ERR(c1_inform_regs))
				return PTR_ERR(c1_inform_regs);
		}

		c1_inform_val = ((__raw_readl((c1_inform_regs + 0x8)) >> 16) & 0x1uL);

		if (c1_inform_val == 0x0uL) {
			pr_info("%s %s sfi hasn't booted yet\n", SFI_IPC_C1_DBG_NAME, __func__);

			return -EPERM;
		}
	}

	memset(name_data, 0, SFI_IPC_C1_NAME_SIZE + 1);
	strncpy(name_data, channel_name, SFI_IPC_C1_NAME_SIZE);

	spin_lock(&sfi_ipc_c1->ipc_lock);

	for (i = MIN_USER_CHAN; i < MAX_USER_CHAN; i++) {
		channel = &sfi_ipc_c1->channel[i];

		if (!memcmp(channel->channel_name, name_data, SFI_IPC_C1_NAME_SIZE)) {
			channel_tx_set(channel, i);
			channel_info_set(channel, name_data, mailbox_rcv_cb, rbptr);

			spin_unlock(&sfi_ipc_c1->ipc_lock);

			(void)send_assign(channel);

			return i;
		}
	}

	for (i = MIN_USER_CHAN; i < MAX_USER_CHAN; i++) {
		channel = &sfi_ipc_c1->channel[i];

		if (channel->tx_id == -EINVAL) {
			channel_tx_set(channel, i);
			channel_info_set(channel, name_data, mailbox_rcv_cb, rbptr);

			spin_unlock(&sfi_ipc_c1->ipc_lock);

			(void)send_assign(channel);

			return i;
		}
	}

	spin_unlock(&sfi_ipc_c1->ipc_lock);

	return -EINVAL;
}
EXPORT_SYMBOL(sfi_ipc_c1_open);

int sfi_ipc_c1_close(char *channel_name)
{
	int i;
	unsigned char name_data[SFI_IPC_C1_NAME_SIZE + 1];
	struct sfi_ipc_c1_ch *channel;

	if (channel_name == NULL) {
		pr_info("%s %s null input pointer\n", SFI_IPC_C1_DBG_NAME, __func__);

		return -EINVAL;
	}

	if (channel_name[0] == '\0') {
		pr_info("%s %s invalid channel name\n", SFI_IPC_C1_DBG_NAME, __func__);

		return -EINVAL;
	}

	memset(name_data, 0, SFI_IPC_C1_NAME_SIZE + 1);
	strncpy(name_data, channel_name, SFI_IPC_C1_NAME_SIZE);

	spin_lock(&sfi_ipc_c1->ipc_lock);

	for (i = MIN_USER_CHAN; i < MAX_USER_CHAN; i++) {
		channel = &sfi_ipc_c1->channel[i];

		if (!memcmp(channel->channel_name, name_data, SFI_IPC_C1_NAME_SIZE)) {
			channel_reset(channel);
		}
	}

	spin_unlock(&sfi_ipc_c1->ipc_lock);

	return 0;
}
EXPORT_SYMBOL(sfi_ipc_c1_close);

int sfi_ipc_c1_send(unsigned int channel_id, struct sfi_ipc_c1_buf_info *tbptr)
{
	int ret;
	struct sfi_ipc_c1_ch *channel;
	int c1_inform_val;

	if (channel_id >= MAX_USER_CHAN || tbptr == NULL) {
		pr_info("%s %s invalid input parameter\n", SFI_IPC_C1_DBG_NAME, __func__);

		return -EINVAL;
	}

	channel = &sfi_ipc_c1->channel[channel_id];
	if (channel->tx_id == -EINVAL) {
		pr_info("%s %s invalid channel\n", SFI_IPC_C1_DBG_NAME, __func__);

		return -EINVAL;
	}

	if ((tbptr->size == 0) || (tbptr->size > 0x7FFFFFFF)) {
		pr_info("%s %s invalid send size\n", SFI_IPC_C1_DBG_NAME, __func__);

		return -EINVAL;
	}

	if (sfi_c1_ofs->c_v920) {
		if (c1_inform_regs == NULL) {
			c1_inform_regs = ioremap(0x11860800, 20);

			if (IS_ERR(c1_inform_regs))
				return PTR_ERR(c1_inform_regs);
		}

		c1_inform_val = ((__raw_readl((c1_inform_regs + 0x8)) >> 16) & 0x1uL);
		if (c1_inform_val == 0x0uL) {
			pr_info("%s %s sfi hasn't booted yet\n", SFI_IPC_C1_DBG_NAME, __func__);

			return -EPERM;
		}
	}

	if (channel->assign_sent == false) {
		ret = send_assign(channel);
		if (ret) {
			pr_info("%s %s send_assign failed : %d\n", SFI_IPC_C1_DBG_NAME, __func__, ret);
			return -EIO;
		}
	}

	spin_lock(&sfi_ipc_c1->ipc_lock);

	ret = send_message(channel, tbptr);

	spin_unlock(&sfi_ipc_c1->ipc_lock);

	if (ret) {
		pr_info("%s %s send_message failed : %d\n", SFI_IPC_C1_DBG_NAME, __func__, ret);
	}

	return ret;
}
EXPORT_SYMBOL(sfi_ipc_c1_send);

/* Driver functions */

static void sfi_ipc_feed_data(struct sfi_ipc_c1_ch *channel)
{
	int msg_len;
	int cpy_len;
	volatile int sr3_data;
	unsigned int *src;
	struct sfi_ipc_c1_buf_info *feed_buf;

	if ((channel->callback == NULL) || channel->rx_buf == NULL) {
		/* Receive callback or buffer is NULL */
		__raw_writel(-SFI_IPC_C1_E_FAILED, sfi_ipc_c1->intr + sfi_c1_ofs->sr3);
		pr_info("%s %s callback: %p rx_buf: %p\n", SFI_IPC_C1_DBG_NAME, __func__,
				channel->callback, channel->rx_buf);

		return;
	}

	/* Read the mailbox message length in SFR */
	feed_buf = channel->rx_buf;
	msg_len = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->sr3);
	if (msg_len == -SFI_IPC_C1_E_TIMEDOUT) {
		/* Drop the message received until now because message is timed out */
		__raw_writel(SFI_IPC_C1_E_HANDLED, sfi_ipc_c1->intr + sfi_c1_ofs->sr3);
		feed_buf->cur_size = 0;

		return;
	} else if (msg_len > (feed_buf->size - feed_buf->cur_size)) {
		/* Message length is longer than user receive buffer capability */
		__raw_writel(-SFI_IPC_C1_E_FAILED, sfi_ipc_c1->intr + sfi_c1_ofs->sr3);
		feed_buf->cur_size = 0;
		pr_info("%s %s message length is too long, msg_len : %d buffer_len : %d\n",
				SFI_IPC_C1_DBG_NAME, __func__, msg_len,
				(int)(feed_buf->size - feed_buf->cur_size));

		return;
	}

	cpy_len = msg_len;
	if (cpy_len > SFI_IPC_C1_SIZE) {
		/* Copy length cannot be bigger than Mailbox SFR capability */
		cpy_len = SFI_IPC_C1_SIZE;
	}

	/* Copy from mailbox SFR to the user receive buffer */
	src = (unsigned int *)(sfi_ipc_c1->intr + sfi_c1_ofs->sr4);
	memcpy(&feed_buf->addr[feed_buf->cur_size], src, cpy_len);

	/* Update the used user buffer length */
	feed_buf->cur_size += cpy_len;
	msg_len -= cpy_len;

	sr3_data = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->sr3);
	if (sr3_data == -SFI_IPC_C1_E_NODATA) {
		/* Drop the message since the sender processed send time out */
		feed_buf->cur_size = 0;
		pr_info("%s %s sender processed send time out\n", SFI_IPC_C1_DBG_NAME, __func__);
	} else {
		/* Send acknowledge */
		__raw_writel(SFI_IPC_C1_E_HANDLED, sfi_ipc_c1->intr + sfi_c1_ofs->sr3);
		if (msg_len == 0) {
			/* No more incoming data left and the receive is done */
			channel->callback(feed_buf);
			feed_buf->cur_size = 0;
		}
	}
}

static void sfi_ipc_irq_distributor(unsigned int status)
{
	int i;
	struct sfi_ipc_c1_ch *channel;

	/* Find mailbox channel */
	for (i = 0; i < SFI_IPC_C1_CHANNEL; i++) {
		channel = &sfi_ipc_c1->channel[i];

		if (status & (1 << channel->rx_id)) {
			sfi_ipc_feed_data(channel);

			return;
		}
	}

	/* No channel is assigned for this irq */
	__raw_writel(-SFI_IPC_C1_E_EINVAL, sfi_ipc_c1->intr + sfi_c1_ofs->sr3);
}

static irqreturn_t sfi_ipc_irq_handler(int irq, void *data)
{
	unsigned int status;
	int c1_inform_val;

	if (sfi_c1_ofs->c_v920) {
		if (c1_inform_regs == NULL) {

			c1_inform_regs = ioremap(0x11860800, 20);

			if (IS_ERR(c1_inform_regs)) {
				return IRQ_HANDLED;
			}
		}

		c1_inform_val = ((__raw_readl((c1_inform_regs + 0x8)) >> 16) & 0x1uL);

		if (c1_inform_val == 0x0uL) {
			return IRQ_HANDLED;
		}
	}
	/* SFI IPC INTERRUPT STATUS REGISTER */
	status = __raw_readl(sfi_ipc_c1->intr + sfi_c1_ofs->intsr1);

	if (status != 0) {
		/* SFI IPC INTERRUPT PENDING CLEAR */
		__raw_writel(status, sfi_ipc_c1->intr + sfi_c1_ofs->intcr1);

		sfi_ipc_irq_distributor(status);
	} else {
		pr_info("%s %s entered without interrupt status bit\n",
				SFI_IPC_C1_DBG_NAME, __func__);
	}

	return IRQ_HANDLED;
}

static void channel_free(void)
{
	struct sfi_ipc_c1_ch *channel;

	/* free assign channel */
	channel = &sfi_ipc_c1->channel[SFI_IPC_C1_ASSIGN];
	if (channel) {
		if (channel->rx_buf) {
			if (channel->rx_buf->addr) {
				devm_kfree(sfi_ipc_c1->dev, channel->rx_buf->addr);
			}
			devm_kfree(sfi_ipc_c1->dev, channel->rx_buf);
		}
	}

	if (sfi_ipc_c1->channel) {
		devm_kfree(sfi_ipc_c1->dev, sfi_ipc_c1->channel);
	}
}

static int channel_init(void)
{
	int i;
	unsigned char name_data[SFI_IPC_C1_NAME_SIZE];
	struct sfi_ipc_c1_ch *channel;

	sfi_ipc_c1->channel = devm_kzalloc(sfi_ipc_c1->dev,
									sizeof(struct sfi_ipc_c1_ch) * SFI_IPC_C1_CHANNEL,
									GFP_KERNEL);
	if (IS_ERR(sfi_ipc_c1->channel)) {
		return PTR_ERR(sfi_ipc_c1->channel);
	}

	/* general initializing */
	for (i = 0; i < MAX_USER_CHAN; i++) {
		channel = &sfi_ipc_c1->channel[i];
		channel_reset(channel);
	}

	/* assign channel */
	channel = &sfi_ipc_c1->channel[SFI_IPC_C1_ASSIGN];
	memset(name_data, 0, SFI_IPC_C1_NAME_SIZE);
	strncpy(name_data, "ASSIGN", strlen("ASSIGN"));

	channel->rx_buf = devm_kzalloc(sfi_ipc_c1->dev,
								   sizeof(struct sfi_ipc_c1_buf_info),
								   GFP_KERNEL);
	if (IS_ERR(channel->rx_buf)) {
		return PTR_ERR(channel->rx_buf);
	}

	channel->rx_buf->addr = devm_kzalloc(sfi_ipc_c1->dev,
										 SFI_IPC_C1_ASSIGN_MSG_SIZE,
										 GFP_KERNEL);
	if (IS_ERR(channel->rx_buf->addr)) {
		return PTR_ERR(channel->rx_buf->addr);
	}

	channel->rx_buf->cur_size = 0;
	channel->rx_buf->size = SFI_IPC_C1_ASSIGN_MSG_SIZE;

	channel_tx_set(channel, SFI_IPC_C1_ASSIGN);
	channel_rx_set(channel, SFI_IPC_C1_ASSIGN);
	channel_info_set(channel, name_data, assign_rx_callback,
					 channel->rx_buf);
	channel->send_timeout = false;
	channel->assign_rcvd = true;
	channel->assign_sent = true;

	return 0;
}

static int sfi_ipc_c1_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *res;
	struct device_node *node;
	struct device *dev = &pdev->dev;
	int c1_inform_val;

	pr_info("%s %s\n", SFI_IPC_C1_DBG_NAME, __func__);

	node = pdev->dev.of_node;

	if (!node) {
		dev_err(&pdev->dev, "driver doesn't support"
				"non-dt devices\n");

		return -ENODEV;
	}

	dev_info(&pdev->dev, "sfi_ipc core1 probe\n");

	sfi_ipc_c1 = devm_kzalloc(&pdev->dev,
						   sizeof(struct sfi_ipc_c1_info), GFP_KERNEL);
	if (IS_ERR(sfi_ipc_c1)) {
		return PTR_ERR(sfi_ipc_c1);
	}

	sfi_ipc_c1->dev = &pdev->dev;

	sfi_c1_ofs = of_device_get_match_data(dev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	sfi_ipc_c1->intr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(sfi_ipc_c1->intr)) {
		return PTR_ERR(sfi_ipc_c1->intr);
	}

	if (sfi_c1_ofs->c_v920) {
		if (c1_inform_regs == NULL) {
			c1_inform_regs = ioremap(0x11860800, 20);

			if (IS_ERR(c1_inform_regs))
				return PTR_ERR(c1_inform_regs);
		}

		c1_inform_val = ((__raw_readl((c1_inform_regs + 0x8)) >> 16) & 0x1uL);

		if (c1_inform_val == 0x0uL) {
			dev_err(&pdev->dev, "sfi hasn't booted yet\n");

			return -EPERM;
		}
	}

	channel_init();

	spin_lock_init(&sfi_ipc_c1->ipc_lock);

	sfi_ipc_c1->irq = irq_of_parse_and_map(node, 0);

	/* make sure no interrupt is pending before initialization */
	sfi_ipc_c1_reset();

	ret = devm_request_irq(&pdev->dev, sfi_ipc_c1->irq, sfi_ipc_irq_handler,
						   0, dev_name(&pdev->dev), sfi_ipc_c1);
	if (ret) {
		dev_err(&pdev->dev, "failed to register sfi_ipc interrupt:%d\n", ret);

		channel_free();

		return ret;
	}

	pr_info("%s %s finished\n", SFI_IPC_C1_DBG_NAME, __func__);

	return ret;
}

static int sfi_ipc_c1_remove(struct platform_device *pdev)
{
	return 0;
}

static int sfi_ipc_c1_pm_suspend(struct device *dev)
{
	int i;
	struct sfi_ipc_c1_ch *channel;

	disable_irq(sfi_ipc_c1->irq);

	for (i = MIN_USER_CHAN; i < MAX_USER_CHAN; i++) {
		channel = &sfi_ipc_c1->channel[i];
		if (channel->rx_buf) {
			/* invalidate the incoming mailbox message */
			channel->rx_buf->cur_size = 0;
		}
		/* reset all flags */
		channel->send_timeout = false;
		channel->assign_rcvd = false;
		channel->assign_sent = false;
	}
	if (sfi_c1_ofs->c_v920 == 0) {
		sfi_ipc_c1_reset();
	}
	return 0;
}

static int sfi_ipc_c1_pm_resume(struct device *dev)
{
	if (sfi_c1_ofs->c_v920 == 0) {
		sfi_ipc_c1_reset();
	}
	enable_irq(sfi_ipc_c1->irq);

	return 0;
}

static const struct dev_pm_ops sfi_ipc_c1_pm_ops = {
	.suspend = sfi_ipc_c1_pm_suspend,
	.resume = sfi_ipc_c1_pm_resume,
};

static const struct sfi_ipc_c1_reg_data exynos910_sfi_ipc_c1 = {
	SFI_IPC_C1_DEF_V910_ARCH()
};

static const struct sfi_ipc_c1_reg_data exynos920_sfi_ipc_c1 = {
	SFI_IPC_C1_DEF_V920_ARCH()
};

static const struct of_device_id sfi_ipc_c1_match[] = {
	{ .compatible = "samsung,exynos-sfi-cr52c1", .data = (void *)&exynos910_sfi_ipc_c1},
	{.compatible = "samsung,exynos920-sfi-cr52c1", .data = (void *)&exynos920_sfi_ipc_c1},
	{},
};

static struct platform_driver exynos_sfi_ipc_c1_driver = {
	.probe  = sfi_ipc_c1_probe,
	.remove = sfi_ipc_c1_remove,
	.driver = {
		.name = "exynos-sfi-cr52c1",
		.owner = THIS_MODULE,
		.pm = &sfi_ipc_c1_pm_ops,
		.of_match_table = sfi_ipc_c1_match,
	},
};

static int __init exynos_sfi_ipc_c1_init(void)
{
	return platform_driver_register(&exynos_sfi_ipc_c1_driver);
}

arch_initcall(exynos_sfi_ipc_c1_init);
MODULE_LICENSE("GPL v2");
