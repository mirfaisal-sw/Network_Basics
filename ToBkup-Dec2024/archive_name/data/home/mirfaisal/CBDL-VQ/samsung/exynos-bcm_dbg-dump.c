/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by licensee as a result of using, modifying or
 * distributing this software or its derivatives.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/ktime.h>
#include <linux/io.h>
#include <linux/sched/clock.h>

#include <soc/samsung/exynos-bcm_dbg.h>
#include <soc/samsung/exynos-bcm_dbg-dump.h>

static char file_name[BCM_DUMP_FNAME_MAX_STR];

int exynos_bcm_dbg_buffer_dump(struct exynos_bcm_dbg_data *data, bool klog)
{
	void __iomem *v_addr = data->dump_addr.v_addr;
	u32 buff_size = data->dump_addr.buff_size - EXYNOS_BCM_KTIME_SIZE;
	u32 buff_cnt = 0;
	u32 dump_entry_size = sizeof(struct exynos_bcm_dump_info);
	struct exynos_bcm_dump_info *dump_info = NULL;
	u32 defined_event, ip_index;
	char *result;
	ssize_t str_size;
	u32 tmp_ktime[2];
	u64 last_ktime;

#if IS_ENABLED(CONFIG_EXYNOS_BCM_DBG_DUMP_FILE)
	struct file *fp = NULL;
#endif

	if (!data->dump_addr.p_addr) {
		BCM_ERR("%s: No memory region for dump\n", __func__);
		return -ENOMEM;
	}

	if (in_interrupt()) {
		BCM_INFO("%s: skip file dump in interrupt context\n", __func__);
		return 0;
	}

	str_size = snprintf(file_name, BCM_DUMP_FNAME_MAX_STR,
				"/var/volatile/log/result_bcm_%llu.csv",
				cpu_clock(raw_smp_processor_id()));
	if (str_size < 0) {
		BCM_ERR("%s: snprintf is failed\n", __func__);
		return -EINVAL;
	}
	result = kzalloc(sizeof(char) * BCM_DUMP_MAX_STR, GFP_KERNEL);
	if (result == NULL) {
		BCM_ERR("%s: failed allocated of result memory\n", __func__);
		return -ENOMEM;
	}

	tmp_ktime[0] = __raw_readl(v_addr);
	tmp_ktime[1] = __raw_readl(v_addr + 0x4);
	last_ktime = (((u64)tmp_ktime[1] << EXYNOS_BCM_32BIT_SHIFT) &
			EXYNOS_BCM_U64_HIGH_MASK) |
			((u64)tmp_ktime[0] & EXYNOS_BCM_U64_LOW_MASK);

	dump_info = (struct exynos_bcm_dump_info *)(v_addr + EXYNOS_BCM_KTIME_SIZE);

#if IS_ENABLED(CONFIG_EXYNOS_BCM_DBG_DUMP_FILE)
	if (data->dump_file) {
		fp = filp_open(file_name, O_WRONLY|O_CREAT|O_APPEND, 0);
		if (IS_ERR(fp)) {
			BCM_ERR("%s: name: %s filp_open fail\n", __func__, file_name);
			kfree(result);
			return IS_ERR(fp);
		}
	}
#endif

	str_size = snprintf(result, PAGE_SIZE, "last kernel time, %llu\n", last_ktime);
	if (str_size < 0) {
		BCM_ERR("%s: snprintf is failed\n", __func__);
		return -EINVAL;
	}
#if IS_ENABLED(CONFIG_EXYNOS_BCM_DBG_DUMP_FILE)
	if (data->dump_file)
		kernel_write(fp, result, str_size, &fp->f_pos);
#endif

	str_size = snprintf(result, PAGE_SIZE, "seq_no, ip_index, define_event, time, ccnt, "
			"pmcnt0, pmcnt1, pmcnt2, pmcnt3, pmcnt4, pmcnt5, pmcnt6, pmcnt7\n");
	if (str_size < 0) {
		BCM_ERR("%s: snprintf is failed\n", __func__);
		return -EINVAL;
	}
#if IS_ENABLED(CONFIG_EXYNOS_BCM_DBG_DUMP_FILE)
	if (data->dump_file)
		kernel_write(fp, result, str_size, &fp->f_pos);
#endif

	if (klog)
		pr_info("%s", result);

	while ((buff_size - buff_cnt) > dump_entry_size) {
		defined_event = BCM_CMD_GET(dump_info->dump_header,
			BCM_EVT_PRE_DEFINE_MASK, BCM_DUMP_PRE_DEFINE_SHIFT);
		ip_index = BCM_CMD_GET(dump_info->dump_header, BCM_IP_MASK, 0);
		str_size = snprintf(result, PAGE_SIZE, "%u, %u, %u, %u, "
				"%u, %u, %u, %u, %u, %u, %u, %u, %u\n",
				dump_info->dump_seq_no, ip_index, defined_event,
				dump_info->dump_time, dump_info->out_data.ccnt,
				dump_info->out_data.pmcnt[0], dump_info->out_data.pmcnt[1],
				dump_info->out_data.pmcnt[2], dump_info->out_data.pmcnt[3],
				dump_info->out_data.pmcnt[4], dump_info->out_data.pmcnt[5],
				dump_info->out_data.pmcnt[6], dump_info->out_data.pmcnt[7]);
#if IS_ENABLED(CONFIG_EXYNOS_BCM_DBG_DUMP_FILE)
	if (data->dump_file)
			kernel_write(fp, result, str_size, &fp->f_pos);
#endif
		if (str_size < 0) {
			BCM_ERR("%s: snprintf is failed\n", __func__);
			return -EINVAL;
		}
		if (klog)
			pr_info("%s", result);

		dump_info++;
		buff_cnt += dump_entry_size;
	}
	BCM_INFO("dump_entry_size %d, buff_size %d\n", dump_entry_size, buff_size);
#if IS_ENABLED(CONFIG_EXYNOS_BCM_DBG_DUMP_FILE)
	if (data->dump_file)
		filp_close(fp, NULL);
#endif
	kfree(result);

	return 0;
}

int exynos_bcm_dbg_dump(struct exynos_bcm_dbg_data *data, char *buff, size_t buff_size, loff_t off)
{
	void __iomem *ktime_ptr;
	struct exynos_bcm_dump_info *dump_start, *dump_end, *dump_entry;
	u32 defined_event, ip_index;

	u64 last_ktime;
	char *line;
	size_t full_size, buffered_size, line_start_off;
	size_t line_size, line_done_size, line_todo_size;

	if (!data->dump_addr.p_addr) {
		BCM_ERR("%s: No memory region for dump\n", __func__);
		return -ENOMEM;
	}

	line = kzalloc(sizeof(char) * BCM_DUMP_MAX_LINE_SIZE, GFP_KERNEL);
	if (line == NULL)
		return -ENOMEM;

	ktime_ptr = data->dump_addr.v_addr;
	dump_start = (struct exynos_bcm_dump_info *)(ktime_ptr + EXYNOS_BCM_KTIME_SIZE);
	dump_end = dump_start + data->dump_addr.buff_size / sizeof(struct exynos_bcm_dump_info);

	full_size = 0;
	buffered_size = 0;

	/* Start with the meta data [time stamp] & heading */
	last_ktime = __raw_readq(ktime_ptr);

	line_size = scnprintf(line, BCM_DUMP_MAX_LINE_SIZE, "last kernel time, %llu\n"
			"seq_no, ip_index, define_event, time, ccnt, pmcnt0, pmcnt1, pmcnt2, "
			"pmcnt3, pmcnt4, pmcnt5, pmcnt6, pmcnt7\n", last_ktime);
	line_start_off = full_size;
	full_size += line_size;

	if (full_size > off) {
		line_done_size = max((int)(off - line_start_off), 0);
		line_todo_size = min(line_size - line_done_size, buff_size - buffered_size);
		memcpy(buff + buffered_size, line + line_done_size, line_todo_size);
		buffered_size += line_todo_size;
		if (buffered_size == buff_size)
			goto exit;
	}

	for (dump_entry = dump_start; dump_entry < dump_end; dump_entry++) {
		defined_event = BCM_CMD_GET(dump_entry->dump_header,
			BCM_EVT_PRE_DEFINE_MASK, BCM_DUMP_PRE_DEFINE_SHIFT);
		ip_index = BCM_CMD_GET(dump_entry->dump_header, BCM_IP_MASK, 0);

		line_size = scnprintf(line, BCM_DUMP_MAX_LINE_SIZE,
			"%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u\n",
			dump_entry->dump_seq_no, ip_index, defined_event,
			dump_entry->dump_time, dump_entry->out_data.ccnt,
			dump_entry->out_data.pmcnt[0], dump_entry->out_data.pmcnt[1],
			dump_entry->out_data.pmcnt[2], dump_entry->out_data.pmcnt[3],
			dump_entry->out_data.pmcnt[4], dump_entry->out_data.pmcnt[5],
			dump_entry->out_data.pmcnt[6], dump_entry->out_data.pmcnt[7]);
		line_start_off = full_size;
		full_size += line_size;

		if (full_size > off) {
			line_done_size = max((int)(off - line_start_off), 0);
			line_todo_size = min(line_size - line_done_size, buff_size - buffered_size);
			memcpy(buff + buffered_size, line + line_done_size, line_todo_size);
			buffered_size += line_todo_size;
			if (buffered_size == buff_size)
				break;
		}
	}

exit:

	kfree(line);

	return buffered_size;
}
