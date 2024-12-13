// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 *
 * Exynos flexpmu-dbg support.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/slab.h>

#define DATA_LINE	(16)
#define DATA_IDX	(8)

#if IS_ENABLED(CONFIG_DEBUG_FS)
#define FLEXPMU_DBG_FUNC_READ(__name)		\
	exynos_flexpmu_dbg_ ## __name ## _read

#define BUF_MAX_LINE	10
#define BUF_LINE_SIZE	30
#define BUF_SIZE	(BUF_MAX_LINE * BUF_LINE_SIZE)

#define DEC_PRINT	1
#define HEX_PRINT	2

/* enum for debugfs files */
enum flexpmu_debugfs_id {
	FID_CPU_STATUS,
	FID_SEQ_STATUS,
	FID_CUR_SEQ,
	FID_SLEEP_COUNT,
	FID_SCI_FLAG,
	FID_MAX
};

const char *flexpmu_debugfs_name[FID_MAX] = {
	"cpu_status",
	"seq_status",
	"cur_sequence",
	"sleep_count",
	"sci_flag",
};

static struct dentry *flexpmu_dbg_root;

struct flexpmu_dbg_print_arg {
	char prefix[BUF_LINE_SIZE];
	int print_type;
};

struct dbgfs_info {
	int fid;
	struct dentry *den;
	struct file_operations fops;
};

static struct dbgfs_info *flexpmu_dbg_info;
#endif

/* enum for data lines */
enum data_id {
	DID_CPU_STATUS0,		/* 0 */
	DID_CPU_STATUS1,		/* 1 */
	DID_SEQ_STATUS,
	DID_CUR_SEQ0,
	DID_CUR_SEQ1,
	DID_PWR_MODE0,			/* 5 */
	DID_PWR_MODE1,
	DID_PWR_MODE2,
	DID_PWR_MODE3,
	DID_PWR_MODE4,
	DID_PWR_MODE5,
	DID_SW_FLAG,
	DID_IRQ_STATUS,			/* 12 */
	DID_IRQ_DATA,
	DID_IPC_AP_STATUS,
	DID_IPC_AP_RXDATA,
	DID_IPC_AP_TXDATA,
	DID_SOC_COUNT,
	DID_MIF_COUNT,
	DID_IPC_VTS0,
	DID_IPC_VTS1,
	DID_LOCAL_PWR,
	DID_MIF_ALWAYS_ON,		/* 22 */
	DID_AP_COUNT_SLEEP,
	DID_MIF_COUNT_SLEEP,
	DID_AP_COUNT_SICD,
	DID_MIF_COUNT_SICD,
	DID_CUR_PMD,
	DID_MAX
};

static void __iomem *flexpmu_dbg_base;

u32 acpm_get_mifdn_count(void)
{
	return __raw_readl(flexpmu_dbg_base + (DATA_LINE * DID_MIF_COUNT_SLEEP) + DATA_IDX + 4);
}
EXPORT_SYMBOL_GPL(acpm_get_mifdn_count);

u32 acpm_get_apsocdn_count(void)
{
	return __raw_readl(flexpmu_dbg_base + (DATA_LINE * DID_AP_COUNT_SLEEP) + DATA_IDX + 4);
}
EXPORT_SYMBOL_GPL(acpm_get_apsocdn_count);

u32 acpm_get_early_wakeup_count(void)
{
	return __raw_readl(flexpmu_dbg_base + (DATA_LINE * DID_AP_COUNT_SLEEP) + DATA_IDX);
}
EXPORT_SYMBOL_GPL(acpm_get_early_wakeup_count);

#define MIF_REQ_MASK		(0x00FF0000)
#define MIF_REQ_SHIFT		(16)
u32 acpm_get_mif_request(void)
{
	u32 reg = __raw_readl(flexpmu_dbg_base + (DATA_LINE * DID_SW_FLAG) + DATA_IDX + 4);

	return ((reg & MIF_REQ_MASK) >> MIF_REQ_SHIFT);

}
EXPORT_SYMBOL_GPL(acpm_get_mif_request);

#if IS_ENABLED(CONFIG_DEBUG_FS)
static ssize_t print_dataline_2(int did, struct flexpmu_dbg_print_arg *print_arg,
		ssize_t len, char *buf, int *data_count)
{
	int data[2];
	ssize_t line_len;
	int i;

	for (i = 0; i < 2; i++) {
		if (print_arg[*data_count].print_type == DEC_PRINT) {
			data[i] = __raw_readl(flexpmu_dbg_base +
					(DATA_LINE * did) + DATA_IDX + i * 4);

			line_len = snprintf(buf + len, BUF_SIZE - len, "%s : %d\n",
					print_arg[*data_count].prefix, data[i]);
			if (line_len > 0)
				len += line_len;
		} else if (print_arg[*data_count].print_type == HEX_PRINT) {
			data[i] = __raw_readl(flexpmu_dbg_base +
					(DATA_LINE * did) + DATA_IDX + i * 4);

			line_len = snprintf(buf + len, BUF_SIZE - len, "%s : 0x%x\n",
					print_arg[*data_count].prefix, data[i]);
			if (line_len > 0)
				len += line_len;
		}
		*data_count += 1;
	}

	return len;
}

static ssize_t print_dataline_8(int did, struct flexpmu_dbg_print_arg *print_arg,
		ssize_t len, char *buf, int *data_count)
{
	int data[8];
	ssize_t line_len;
	int i;

	for (i = 0; i < 8; i++) {
		if (print_arg[*data_count].print_type == DEC_PRINT) {
			data[i] = __raw_readb(flexpmu_dbg_base +
					(DATA_LINE * did) + DATA_IDX + i);

			line_len = snprintf(buf + len, BUF_SIZE - len, "%s : %d\n",
					print_arg[*data_count].prefix, data[i]);
			if (line_len > 0)
				len += line_len;
		} else if (print_arg[*data_count].print_type == HEX_PRINT) {
			data[i] = __raw_readb(flexpmu_dbg_base +
					(DATA_LINE * did) + DATA_IDX + i);

			line_len = snprintf(buf + len, BUF_SIZE - len, "%s : 0x%x\n",
					print_arg[*data_count].prefix, data[i]);
			if (line_len > 0)
				len += line_len;
		}
		*data_count += 1;
	}

	return len;
}

static ssize_t exynos_flexpmu_dbg_cpu_status_read(int fid, char *buf)
{
	ssize_t ret = 0;
	int data_count = 0;

	struct flexpmu_dbg_print_arg print_arg[BUF_MAX_LINE] = {
		{"CL0_CPU0", DEC_PRINT},
		{"CL0_CPU1", DEC_PRINT},
		{"CL0_CPU2", DEC_PRINT},
		{"CL0_CPU3", DEC_PRINT},
		{"CL1_CPU0", DEC_PRINT},
		{"CL1_CPU1", DEC_PRINT},
		{"CL1_CPU2", DEC_PRINT},
		{"CL1_CPU3", DEC_PRINT},
	};

	ret = print_dataline_8(DID_CPU_STATUS0, print_arg, ret, buf, &data_count);

	return ret;
}

static ssize_t exynos_flexpmu_dbg_seq_status_read(int fid, char *buf)
{
	ssize_t ret = 0;
	int data_count = 0;

	struct flexpmu_dbg_print_arg print_arg[BUF_MAX_LINE] = {
		{"SOC seq", DEC_PRINT},
		{"MIF seq", DEC_PRINT},
		{},
		{},
		{"nonCPU CL0", DEC_PRINT},
		{"nonCPU CL1", DEC_PRINT},
		{},
	};

	ret = print_dataline_8(DID_SEQ_STATUS, print_arg, ret, buf, &data_count);

	return ret;
}

static ssize_t exynos_flexpmu_dbg_cur_sequence_read(int fid, char *buf)
{
	ssize_t ret = 0;
	int data_count = 0;

	struct flexpmu_dbg_print_arg print_arg[BUF_MAX_LINE] = {
		{"UP Sequence", DEC_PRINT},
		{"DOWN Sequence", DEC_PRINT},
		{"Access Type", DEC_PRINT},
		{"CAL Sequence Index", DEC_PRINT},
	};

	ret = print_dataline_2(DID_CUR_SEQ0, print_arg, ret, buf, &data_count);
	ret = print_dataline_2(DID_CUR_SEQ1, print_arg, ret, buf, &data_count);

	return ret;
}

static ssize_t exynos_flexpmu_dbg_sci_flag_read(int fid, char *buf)
{
	ssize_t ret = 0;
	int sci_flag = 0;

	struct flexpmu_dbg_print_arg print_arg[BUF_MAX_LINE] = {
		{"SCI initialize flag", DEC_PRINT},
	};

	ret = print_dataline_2(DID_PWR_MODE5, print_arg, ret, buf, &sci_flag);

	return ret;
}

static ssize_t exynos_flexpmu_dbg_sleep_count_read(int fid, char *buf)
{
	ssize_t ret = 0;
	int data_count = 0;

	struct flexpmu_dbg_print_arg print_arg[BUF_MAX_LINE] = {
		{"[SLEEP] Early wakeup", DEC_PRINT},
		{"[SLEEP] SOC seq down", DEC_PRINT},
		{},
		{"[SLEEP] MIF seq down", DEC_PRINT},
	};

	ret = print_dataline_2(DID_AP_COUNT_SLEEP, print_arg, ret, buf, &data_count);
	ret = print_dataline_2(DID_MIF_COUNT_SLEEP, print_arg, ret, buf, &data_count);

	return ret;
}

static ssize_t (*flexpmu_debugfs_read_fptr[FID_MAX])(int, char *) = {
	FLEXPMU_DBG_FUNC_READ(cpu_status),
	FLEXPMU_DBG_FUNC_READ(seq_status),
	FLEXPMU_DBG_FUNC_READ(cur_sequence),
	FLEXPMU_DBG_FUNC_READ(sleep_count),
	FLEXPMU_DBG_FUNC_READ(sci_flag),
};

static ssize_t exynos_flexpmu_dbg_read(struct file *file, char __user *user_buf,
		size_t count, loff_t *ppos)
{
	struct dbgfs_info *d2f = file->private_data;
	ssize_t ret;
	char buf[BUF_SIZE] = {0,};

	if (d2f->fid < 0)
		return -EINVAL;
	ret = flexpmu_debugfs_read_fptr[d2f->fid](d2f->fid, buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, ret);
}
#endif

static int exynos_flexpmu_dbg_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct resource *res;
#if IS_ENABLED(CONFIG_DEBUG_FS)
	int i = 0;
#endif

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "%s: can not allocate mem for res\n", __func__);
		return -ENOMEM;
	}

	flexpmu_dbg_base = devm_ioremap(dev, res->start, resource_size(res));
	if (IS_ERR(flexpmu_dbg_base)) {
		dev_err(dev, "%s: can not read reg in DT\n", __func__);
		return -EINVAL;
	}

#if IS_ENABLED(CONFIG_DEBUG_FS)
	flexpmu_dbg_info = devm_kzalloc(dev, sizeof(struct dbgfs_info) * FID_MAX, GFP_KERNEL);
	if (!flexpmu_dbg_info) {
		dev_err(dev, "%s: can not allocate mem for flexpmu_dbg_info\n",
				__func__);
		return -ENOMEM;
	}

	flexpmu_dbg_root = debugfs_create_dir("flexpmu-dbg", NULL);
	if (!flexpmu_dbg_root) {
		dev_err(dev, "%s: could not create debugfs root dir\n", __func__);
		return -ENOMEM;
	}

	for (i = 0; i < FID_MAX; i++) {
		flexpmu_dbg_info[i].fid = i;
		flexpmu_dbg_info[i].fops.open = simple_open;
		flexpmu_dbg_info[i].fops.read = exynos_flexpmu_dbg_read;
		flexpmu_dbg_info[i].fops.llseek = default_llseek;
		flexpmu_dbg_info[i].den = debugfs_create_file(flexpmu_debugfs_name[i],
				0644, flexpmu_dbg_root, &flexpmu_dbg_info[i],
				&flexpmu_dbg_info[i].fops);
	}

	platform_set_drvdata(pdev, flexpmu_dbg_info);
#endif
	dev_info(dev, "probe done.\n");
	return 0;
}

static int exynos_flexpmu_dbg_remove(struct platform_device *pdev)
{
#if IS_ENABLED(CONFIG_DEBUG_FS)
	debugfs_remove_recursive(flexpmu_dbg_root);
	platform_set_drvdata(pdev, NULL);
#endif
	return 0;
}

static const struct of_device_id exynos_flexpmu_dbg_match[] = {
	{
		.compatible = "samsung,exynos-flexpmu-dbg",
	},
	{},
};

static struct platform_driver exynos_flexpmu_dbg_drv = {
	.probe		= exynos_flexpmu_dbg_probe,
	.remove		= exynos_flexpmu_dbg_remove,
	.driver		= {
		.name	= "exynos_flexpmu_dbg",
		.owner	= THIS_MODULE,
		.of_match_table = exynos_flexpmu_dbg_match,
	},
};

static int exynos_flexpmu_dbg_init(void)
{
	return platform_driver_register(&exynos_flexpmu_dbg_drv);
}
late_initcall(exynos_flexpmu_dbg_init);

MODULE_DESCRIPTION("Samsung EXYNOS series flexpmu-dbg driver");
MODULE_LICENSE("GPL v2");
