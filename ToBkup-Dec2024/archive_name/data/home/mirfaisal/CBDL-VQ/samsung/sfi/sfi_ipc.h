/* SPDX-License-Identifier: GPL-2.0 */
/*
 * sfi ipc driver support to communicate with the SFI CPU
 *
 * Copyright (C) 2013-2020 Samsung Electronics Co., Ltd.
 *
 */
#ifndef __SFI_IPC_H_
#define __SFI_IPC_H_

#include <linux/compiler_types.h>
#include <linux/device.h>
#include <linux/spinlock_types.h>
#include <linux/wait.h>
#include <soc/samsung/sfi_ipc_ctrl.h>

#define SFI_IPC_DEF_V910_ARCH(void)		\
	.c_v920 = 0,				\
	.intgr0 = 0x0008,			\
	.intcr0 = 0x000C,				\
	.intmr0 = 0x0010,				\
	.intsr0 = 0x0014,			\
	.intmsr0 = 0x0018,				\
	.intgr1 = 0x001C,				\
	.intcr1 = 0x0020,			\
	.intmr1 = 0x0024,				\
	.intsr1 = 0x0028,				\
	.intmsr1 = 0x002C,			\
	.sr0 = 0x0080,				\
	.sr1 = 0x0084,				\
	.sr2 = 0x0088,				\
	.sr3 = 0x008C,				\
	.sr4 = 0x0090,				\
	.sr5 = 0x0094,				\

#define SFI_IPC_DEF_V920_ARCH(void)		\
	.c_v920 = 1,				\
	.intgr0 = 0x0008,			\
	.intcr0 = 0x000C,				\
	.intmr0 = 0x0010,				\
	.intsr0 = 0x0014,			\
	.intmsr0 = 0x0018,				\
	.intgr1 = 0x001C,				\
	.intcr1 = 0x0020,			\
	.intmr1 = 0x0024,				\
	.intsr1 = 0x0028,				\
	.intmsr1 = 0x002C,			\
	.sr0 = 0x0100,				\
	.sr1 = 0x0104,				\
	.sr2 = 0x0108,				\
	.sr3 = 0x010C,				\
	.sr4 = 0x0110,				\
	.sr5 = 0x0114,				\

#define SFI_IPC_ASSIGN						 (0)
#define MAX_USER_CHAN						 (SFI_IPC_CHANNEL)
#define MIN_USER_CHAN						 (1)

/* Mailbox (SFI_IPC) errno shared with SFI (CR-52) */
#define SFI_IPC_E_HANDLED					 (0) /* Message handled */
#define SFI_IPC_E_FAILED					 (1) /* Message failed */
#define SFI_IPC_E_EINVAL					 (22) /* No handler assigned */
#define SFI_IPC_E_TIMEDOUT					 (60) /* Timed out */
#define SFI_IPC_E_NODATA					 (86) /* No data in shared register */

#define SFI_IPC_TIMEOUT						 (20000000) /* nsec */

#pragma pack(push, 4)
struct sfi_ipc_assign_msg {
	unsigned char channel_name[SFI_IPC_NAME_SIZE];
	unsigned int tx_id;
	unsigned int rx_id;
};
#pragma pack(pop)
#define SFI_IPC_ASSIGN_MSG_SIZE				 (sizeof(struct sfi_ipc_assign_msg))

struct sfi_ipc_ch {
	unsigned int tx_id;
	unsigned int rx_id;
	unsigned char channel_name[SFI_IPC_NAME_SIZE + 1];
	sfi_ipc_cb callback;
	struct sfi_ipc_buf_info *rx_buf;

	unsigned char int_snd[SFI_IPC_SIZE];
	bool send_timeout;
	bool assign_rcvd;
	bool assign_sent;
};

struct sfi_ipc_reg_data {
	const unsigned int c_v920;
	const unsigned int intgr0;
	const unsigned int intcr0;
	const unsigned int intmr0;
	const unsigned int intsr0;
	const unsigned int intmsr0;
	const unsigned int intgr1;
	const unsigned int intcr1;
	const unsigned int intmr1;
	const unsigned int intsr1;
	const unsigned int intmsr1;
	const unsigned int sr0;
	const unsigned int sr1;
	const unsigned int sr2;
	const unsigned int sr3;
	const unsigned int sr4;
	const unsigned int sr5;
};

struct sfi_ipc_info {
	struct device *dev;
	struct sfi_ipc_ch *channel;
	unsigned int irq;
	void __iomem *intr;

	spinlock_t ipc_lock;
};

#endif /* __SFI_IPC_H_ */
