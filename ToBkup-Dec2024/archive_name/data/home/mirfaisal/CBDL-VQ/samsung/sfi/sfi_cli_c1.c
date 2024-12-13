// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * EXYNOS - sfi cli driver support to communicate with the SFI CPU
 */

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_reserved_mem.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/string.h>
#include <linux/uaccess.h> /* copy_from_user */
#include <linux/slab.h> /* kmalloc */
#include <asm/io.h> /*ioremap */
#include <linux/syscalls.h>
#include <linux/delay.h> /* msleep */
#include <linux/workqueue.h> /* work queue */
#include <soc/samsung/sfi_ipc_ctrl.h>

static unsigned int sfi_dram_phyaddr;
static unsigned int sfi_dram_size;

static void *exynos_sfimem_request_region(unsigned long addr, unsigned int size)
{
	int i;
	unsigned int num_pages = (size >> PAGE_SHIFT);
	pgprot_t prot = pgprot_writecombine(PAGE_KERNEL);
	struct page **pages = NULL;
	void *v_addr = NULL;

	if (!addr) {
		return NULL;
	}

	pages = kmalloc_array(num_pages, sizeof(struct page *), GFP_ATOMIC);
	if (!pages) {
		return NULL;
	}

	for (i = 0 ; i < num_pages ; i++) {
		pages[i] = phys_to_page(addr);
		addr += PAGE_SIZE;
	}

	v_addr = vmap(pages, num_pages, VM_MAP, prot);
	kfree(pages);

	return v_addr;
}

#define SFI_CLI_C1_NAME "sfi_cl2"
#define SFI_CLI_C1_TRANSFER   (0)
#define SFI_CLI_C1_SET_TOUT   (1)
#define SFI_CLI_C1_RESERVED   (2)
#define SFI_CLI_C1_IPC_RESET  (3)

#define SFI_CLI_C1_CMD_BUF_SIZE (512)
#define SFI_CLI_C1_LOG_BUF_SIZE (0x10000) /* 64KB */
#define SFI_CLI_C1_DRAM_QUEUE_SIZE (0x40000) /* 256KB */
#define SFI_CLI_C1_CMD_WAIT_RESPONSE_TIME_LIMIT_MS 10000 /* 10 second */
#define SFI_CLI_C1_CMD_WAIT_SLEEP_TIME_MS  (10) /* 10ms */
#define SFI_CLI_C1_SHARED_RAM_SIZE (0x100000) /* 1MB */

enum sfi_cli_c1_state_type {
	SFI_CLI_C1_STATE_NOT_INIT,
	SFI_CLI_C1_STATE_CH_CREATED,
	SFI_CLI_C1_STATE_CH_CONNECTED,
	SFI_CLI_C1_STATE_INIT_FAILED
};

struct sfi_cli_c1_ch_type {
	enum sfi_cli_c1_state_type state;
	int ch_no;
	char *name;
	int is_wait_response;
	unsigned int confirmed_test_result_cnt;
	unsigned int rcv_cb_cnt;
	struct sfi_ipc_c1_buf_info ipc_rx_buff_info;
	struct sfi_ipc_c1_buf_info ipc_tx_buff_info;
};

static char user_cmd_buff[SFI_CLI_C1_CMD_BUF_SIZE+4];
static char user_log_buff[2][SFI_CLI_C1_LOG_BUF_SIZE+4];
static unsigned int user_log_buff_idx[2];
static struct sfi_cli_c1_ch_type cli_ch;
static int cli_timeout_ms = SFI_CLI_C1_CMD_WAIT_RESPONSE_TIME_LIMIT_MS;

struct c1_dram_queue_status_type {
	unsigned int read_tail;
	volatile unsigned int write_tail;
};

struct c1_dram_queue_type {
	struct c1_dram_queue_status_type cmd_info;
	struct c1_dram_queue_status_type log_info[2];
	int log_buff_idx;
	volatile int log_buff_use[2];
	int test_return_value;
	unsigned int test_return_update_cnt;
	char cmd_buff[SFI_CLI_C1_DRAM_QUEUE_SIZE];
	char log_buff[2][SFI_CLI_C1_DRAM_QUEUE_SIZE];
};
struct c1_dram_queue_type c1_dummpy_dram_queue;
struct c1_dram_queue_type *c1_dram_queue;

static struct workqueue_struct *print0_wq;
static struct workqueue_struct *print1_wq;

static struct cr52printwork_info {
	struct work_struct print_task;
	int context;
} cr52print_work[2];

static void check_test_result(void)
{
	if (c1_dram_queue->test_return_update_cnt != cli_ch.confirmed_test_result_cnt) {
		cli_ch.confirmed_test_result_cnt = c1_dram_queue->test_return_update_cnt;
		cli_ch.is_wait_response = 0;
	}
}

static void print_cr52c1_log(struct work_struct *work)
{
	int i;
	char *log_ptr;
	int st_idx;
	int end_idx;
	struct cr52printwork_info *work_info;
	unsigned int context;

	work_info = container_of(work, struct cr52printwork_info, print_task);
	context = work_info->context;

	pr_devel("[sfi_cli_c1] %s enter cb_cnt: %d context: %d\n", __func__,
			cli_ch.rcv_cb_cnt, context);

	if (c1_dram_queue->log_info[context].read_tail >
			c1_dram_queue->log_info[context].write_tail) {
		c1_dram_queue->log_info[context].read_tail = 0;
		pr_info("[sfi_cli_c1] %s page rolled\n", __func__);
	}

	st_idx = c1_dram_queue->log_info[context].read_tail;
	end_idx = c1_dram_queue->log_info[context].write_tail;

	if (end_idx - st_idx > SFI_CLI_C1_LOG_BUF_SIZE) {
		pr_info("[sfi_cli_c1] %s cannot show entire log buffer at once\n", __func__);
		c1_dram_queue->log_info[context].read_tail = c1_dram_queue->log_info[context].write_tail;
		return ;
	}

	/* prevent idx overrun */
	if (end_idx - st_idx + user_log_buff_idx[context] > SFI_CLI_C1_LOG_BUF_SIZE)
		user_log_buff_idx[context] = 0;


	if (st_idx != end_idx) {
		log_ptr = c1_dram_queue->log_buff[context];

		for (i = st_idx; i < end_idx; i++) {
			while (c1_dram_queue->log_buff_use[context]) {
				/* do nothing */
			}
			user_log_buff[context][user_log_buff_idx[context]++] = log_ptr[i];
			c1_dram_queue->log_info[context].read_tail = i + 1;

			if (log_ptr[i] == 0x0A) {
				user_log_buff[context][user_log_buff_idx[context]] = 0;
				pr_info("[CR-52 Core1] %s", user_log_buff[context]);
				user_log_buff_idx[context] = 0;
			}
		}
		check_test_result();
	}
}

static void sfi_cli_c1_ipc_rx_cb(struct sfi_ipc_c1_buf_info *arg)
{
	cli_ch.rcv_cb_cnt++;

	(void)queue_work(print0_wq, &cr52print_work[0].print_task);
	(void)queue_work(print1_wq, &cr52print_work[1].print_task);
}

static void cli_open(void)
{
	if (cli_ch.state == SFI_CLI_C1_STATE_NOT_INIT) {
		cli_ch.ch_no = sfi_ipc_c1_open(cli_ch.name, sfi_cli_c1_ipc_rx_cb,
									&(cli_ch.ipc_rx_buff_info));

		if (cli_ch.ch_no < 0 || cli_ch.ch_no >= SFI_IPC_C1_CHANNEL) { /*invalid ch number */
			pr_info("[sfi_cli_c1] %s : invalid ipc ch number assigned %d \n",
					__func__, cli_ch.ch_no);
		} else {
			pr_info("[sfi_cli_c1] %s : ipc ch opened ch No. %d\n",
					__func__, cli_ch.ch_no);

			cli_ch.state = SFI_CLI_C1_STATE_CH_CREATED;
		}
	}
}

static void cli_send_dram_addr(void)
{
	unsigned int dram_addr;
	int ret;

	/* send dram addr */
	dram_addr = sfi_dram_phyaddr;
	memcpy(cli_ch.ipc_tx_buff_info.addr, &dram_addr, sizeof(int));

	pr_info("[sfi_cli_c1] %s : dram addr is 0x%08x \n", __func__, dram_addr);

	ret = sfi_ipc_c1_send(cli_ch.ch_no, &cli_ch.ipc_tx_buff_info);

	if (ret) {
		pr_info("[sfi_cli_c1] %s : ipc channel created but not connected ret : %d\n",
				__func__, -ret);
	} else {
		pr_info("[sfi_cli_c1] %s : ipc channel connected \n", __func__);
		cli_ch.state = SFI_CLI_C1_STATE_CH_CONNECTED;
	}
}

static void cli_free_resource(void)
{
	if (cli_ch.ipc_rx_buff_info.addr) {
		kfree(cli_ch.ipc_rx_buff_info.addr);
		cli_ch.ipc_rx_buff_info.addr = NULL;
	}

	if (cli_ch.ipc_tx_buff_info.addr) {
		kfree(cli_ch.ipc_tx_buff_info.addr);
		cli_ch.ipc_tx_buff_info.addr = NULL;
	}

	if (c1_dram_queue) {
		vunmap(c1_dram_queue);
		c1_dram_queue = NULL;
	}
}

static int cli_ipc_init(void)
{
	/* buffer init */
	void *sfi_dram_viraddr;
	sfi_dram_viraddr = exynos_sfimem_request_region(sfi_dram_phyaddr,
					   SFI_CLI_C1_SHARED_RAM_SIZE);

	if (sfi_dram_viraddr == NULL) {
		pr_info("[sfi_cli_c1] %s : cannot initialization , dram virtual addr is null \n",
				__func__);
		cli_ch.state = SFI_CLI_C1_STATE_INIT_FAILED;
		return -1;
	} else {
		pr_info("[sfi_cli_c1] %s : dram virtual addr covert success.. \n", __func__);
		c1_dram_queue = (struct c1_dram_queue_type *)sfi_dram_viraddr;
	}
	memset(c1_dram_queue, 0, sizeof(struct c1_dram_queue_type));
	pr_info("[sfi_cli_c1] %s : size of dram : %ld \n", __func__,
			sizeof(struct c1_dram_queue_type));

	memset(user_cmd_buff, 0, SFI_CLI_C1_CMD_BUF_SIZE+4);
	memset(&user_log_buff[0], 0, SFI_CLI_C1_LOG_BUF_SIZE+4);
	memset(&user_log_buff[1], 0, SFI_CLI_C1_LOG_BUF_SIZE+4);
	user_log_buff_idx[0] = 0;
	user_log_buff_idx[1] = 0;

	/* cli ch init */
	cli_ch.name = SFI_CLI_C1_NAME;
	cli_ch.state = SFI_CLI_C1_STATE_NOT_INIT;
	cli_ch.is_wait_response = 0;
	cli_ch.confirmed_test_result_cnt = 0;
	cli_ch.ipc_rx_buff_info.addr = kmalloc(SFI_CLI_C1_CMD_BUF_SIZE, GFP_KERNEL);
	cli_ch.ipc_rx_buff_info.size = SFI_CLI_C1_CMD_BUF_SIZE;
	cli_ch.ipc_tx_buff_info.addr = kmalloc(SFI_CLI_C1_CMD_BUF_SIZE, GFP_KERNEL);
	cli_ch.ipc_tx_buff_info.size = sizeof(int);

	if ((cli_ch.ipc_rx_buff_info.addr == NULL) ||
		(cli_ch.ipc_tx_buff_info.addr == NULL)) {

		cli_free_resource();
		pr_info("[sfi_cli_c1] %s : cannot initialize , rx, tx buff addr is null \n",
				__func__);
		cli_ch.state = SFI_CLI_C1_STATE_INIT_FAILED;
		return -1;
	}

	print0_wq = create_workqueue("Print0_CR52");
	print1_wq = create_workqueue("Print1_CR52");
	cr52print_work[0].context = 0;
	INIT_WORK(&cr52print_work[0].print_task, print_cr52c1_log);
	cr52print_work[1].context = 1;
	INIT_WORK(&cr52print_work[1].print_task, print_cr52c1_log);

	/* channel open */
	cli_open();

	/* dram addr send */
	cli_send_dram_addr();

	return 0;
}

static int cmdlen(char *cmd_str)
{
	int cmd_len = 0;
	/* detect '\n' under BUF_SIZE */
	while (cmd_len != SFI_CLI_C1_CMD_BUF_SIZE && cmd_str[cmd_len] != '\n') {
		cmd_len++;
	}
	 /* write confirmed null */
	cmd_str[++cmd_len] = 0;
	return cmd_len;
}
static int cli_cmd_transfer(char *cmd_str)
{
	int i;
	int send_ret;
	int str_len = cmdlen(cmd_str);
	pr_info("[sfi_cli_c1] cmd length: %d\n", str_len);
	printk("[CR-52 Core1] %s\n", cmd_str);

	cli_open();
	cli_send_dram_addr();

	if (cli_ch.state == SFI_CLI_C1_STATE_CH_CONNECTED) {
		if (c1_dram_queue->cmd_info.write_tail + str_len > SFI_CLI_C1_DRAM_QUEUE_SIZE) {
			c1_dram_queue->cmd_info.write_tail = 0;
		}
		memcpy(&c1_dram_queue->cmd_buff[c1_dram_queue->cmd_info.write_tail],
			   cmd_str, str_len);

		c1_dram_queue->cmd_info.write_tail += str_len;
		cli_ch.is_wait_response = 1;
		cli_ch.confirmed_test_result_cnt = c1_dram_queue->test_return_update_cnt;

		pr_info("[sfi_cli_c1] %s : read tail: %d write tail %d\n",
				__func__,
				c1_dram_queue->cmd_info.read_tail, c1_dram_queue->cmd_info.write_tail);

		send_ret = sfi_ipc_c1_send(cli_ch.ch_no,
					 &cli_ch.ipc_tx_buff_info); /* tx_buff has dram addr info */

		if (send_ret) {
			pr_info("[sfi_cli_c1] %s : cli is disconnected. %d\n", __func__, -send_ret);
			cli_ch.is_wait_response = 0;
			 /* ignore failed cmd */
			c1_dram_queue->cmd_info.read_tail = c1_dram_queue->cmd_info.write_tail;
			cli_ch.state = SFI_CLI_C1_STATE_NOT_INIT;
			return -1;
		}
		for (i = 0 ; i < cli_timeout_ms ;
			 i += SFI_CLI_C1_CMD_WAIT_SLEEP_TIME_MS) {
			msleep(SFI_CLI_C1_CMD_WAIT_SLEEP_TIME_MS);
			if (cli_ch.is_wait_response == 0) { /* receive callback invoked */
				return 0;	/* test invoke success */
			}

			check_test_result();
		}
		cli_ch.is_wait_response = 0;
		cli_ch.state = SFI_CLI_C1_STATE_NOT_INIT;
		pr_info("[sfi_cli_c1] %s : cli timeout occurred.. connection retry mode on..\n",
				__func__);
		return -1; /* time_out */
	} else {
		pr_info("[sfi_cli_c1] %s : cli is not connected.. %d \n",
				__func__, cli_ch.state);
	}
	return -2; /* not_connected */
}

/*
 * cmd : sfi cli cmd type
 * arg : 512 byte cmd buffer pointer
 */
long sfi_cli_c1_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret;
	int timeout;
	pr_info("[sfi_cli_c1] %s\n", __func__);
	switch (cmd) {
	case SFI_CLI_C1_TRANSFER:
		if (copy_from_user(user_cmd_buff, (void __user *)arg,
						   SFI_CLI_C1_CMD_BUF_SIZE)) {
			return -1;
			}
		pr_info("[sfi_cli_c1] cmd: %s \n", user_cmd_buff);
		if (cli_cmd_transfer(user_cmd_buff)) {
			return -1;
		}
		pr_info("[sfi_cli_c1] test result : %d \n",
				c1_dram_queue->test_return_value);

		/* received test result from CR-52 return */
		return c1_dram_queue->test_return_value;
		break;
	case SFI_CLI_C1_SET_TOUT:
		if (copy_from_user(user_cmd_buff, (void __user *)arg,
						   SFI_CLI_C1_CMD_BUF_SIZE)) {
			return -1;
			}
		timeout = *(int *)user_cmd_buff;
		cli_timeout_ms = timeout * 1000;
		pr_info("[sfi_cli_c1] current timeout value: %ds\n", timeout);
		break;
	case SFI_CLI_C1_RESERVED:

		cli_ch.state = SFI_CLI_C1_STATE_NOT_INIT;
		/* channel open */
		cli_open();

		/* dram addr send */
		cli_send_dram_addr();
		pr_info("[sfi_cli_c1] reserved function..\n");
		break;
	case SFI_CLI_C1_IPC_RESET:
		ret = sfi_ipc_c1_reset();
		pr_info("[sfi_cli_c1] ipc reset called.. ret: %d\n", ret);
		break;
	default:
		pr_info("[sfi_cli_c1] unknown function..\n");
	}
	return 0;
}

struct file_operations sfi_cli_c1_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = sfi_cli_c1_ioctl,
};

static struct miscdevice sfi_cli_c1_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= SFI_CLI_C1_NAME,
	.fops		= &sfi_cli_c1_fops,
};

static int sfi_cli_c1_probe(struct platform_device *pdev)
{
	int result;

	struct reserved_mem *rmem;
	struct device_node *rmem_np;

	rmem_np = of_parse_phandle(pdev->dev.of_node, "memory-region", 0);
	if (!rmem_np) {
		dev_err(&pdev->dev, "no such memory-region\n");
		return -ENODEV;
	}

	rmem = of_reserved_mem_lookup(rmem_np);
	if (!rmem) {
		dev_err(&pdev->dev, "no such reserved mem of node name %s\n",
				pdev->dev.of_node->name);
		return -ENODEV;
	}

	if (!rmem->base || !rmem->size)
		return -EPROBE_DEFER;

	sfi_dram_phyaddr = rmem->base;
	sfi_dram_size =  rmem->size;

	result = misc_register(&sfi_cli_c1_miscdev);
	if (result) {
		dev_err(&pdev->dev, "failed to register misc driver\n");
		return result;
	}

	cli_ipc_init();

	return 0;
}

static int sfi_cli_c1_remove(struct platform_device *pdev)
{
	sfi_ipc_c1_close(cli_ch.name);

	cli_free_resource();

	misc_deregister(&sfi_cli_c1_miscdev);

	return 0;
}

static const struct of_device_id of_sfi_cli_c1_ids[] = {
	{
		.compatible = "samsung,exynos-sfi-cli-c1",
		.data = NULL,
	}, {
		/* sentinel */
	}
};

static struct platform_driver sfi_cli_c1_driver = {
	.probe		= sfi_cli_c1_probe,
	.remove		= sfi_cli_c1_remove,
	.driver = {
		.name	= "sfi_cli_c1",
		.of_match_table = of_match_ptr(of_sfi_cli_c1_ids),
	}
};

static int __init sfi_cli_c1_init(void)
{
	pr_info("[sfi_cli_c1] %s\n", __func__);

	return platform_driver_register(&sfi_cli_c1_driver);
}

static void __exit sfi_cli_c1_exit(void)
{
	pr_info("[sfi_cli_c1] %s\n", __func__);

	platform_driver_unregister(&sfi_cli_c1_driver);
}

module_init(sfi_cli_c1_init);
module_exit(sfi_cli_c1_exit);

MODULE_LICENSE("GPL v2");
