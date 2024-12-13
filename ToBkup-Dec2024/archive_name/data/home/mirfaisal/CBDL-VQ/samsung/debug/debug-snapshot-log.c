/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/ktime.h>
#include <linux/clk-provider.h>
#include <linux/sched/clock.h>
#include <linux/ftrace.h>
#include <linux/kernel_stat.h>
#include <linux/irqnr.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>

#include <asm/stacktrace.h>
#include <soc/samsung/debug-snapshot.h>
#include "debug-snapshot-local.h"

#include <trace/events/power.h>
#include <trace/events/timer.h>
#include <trace/events/workqueue.h>
#include <trace/events/irq.h>
#include <trace/events/sched.h>
#include <trace/events/rwmmio.h>

struct dbg_snapshot_log_item dss_log_items[] = {
	[DSS_LOG_TASK_ID]	= {DSS_LOG_TASK,	{0, 0, 0, false}, },
	[DSS_LOG_WORK_ID]	= {DSS_LOG_WORK,	{0, 0, 0, false}, },
	[DSS_LOG_CPUIDLE_ID]	= {DSS_LOG_CPUIDLE,	{0, 0, 0, false}, },
	[DSS_LOG_SUSPEND_ID]	= {DSS_LOG_SUSPEND,	{0, 0, 0, false}, },
	[DSS_LOG_IRQ_ID]	= {DSS_LOG_IRQ,		{0, 0, 0, false}, },
	[DSS_LOG_HRTIMER_ID]	= {DSS_LOG_HRTIMER,	{0, 0, 0, false}, },
	[DSS_LOG_REG_ID]	= {DSS_LOG_REG,		{0, 0, 0, false}, },
	[DSS_LOG_CLK_ID]	= {DSS_LOG_CLK,		{0, 0, 0, false}, },
	[DSS_LOG_PMU_ID]	= {DSS_LOG_PMU,		{0, 0, 0, false}, },
	[DSS_LOG_FREQ_ID]	= {DSS_LOG_FREQ,	{0, 0, 0, false}, },
	[DSS_LOG_DM_ID]		= {DSS_LOG_DM,		{0, 0, 0, false}, },
	[DSS_LOG_REGULATOR_ID]	= {DSS_LOG_REGULATOR,	{0, 0, 0, false}, },
	[DSS_LOG_THERMAL_ID]	= {DSS_LOG_THERMAL,	{0, 0, 0, false}, },
	[DSS_LOG_ACPM_ID]	= {DSS_LOG_ACPM,	{0, 0, 0, false}, },
	[DSS_LOG_PRINTK_ID]	= {DSS_LOG_PRINTK,	{0, 0, 0, false}, },
};

#define DSS_FREQ_NAME_LEN	(SZ_8)
#define DSS_FREQ_MAX_SIZE	(SZ_32)

struct dbg_snapshot_log_misc dss_log_misc;
static char dss_freq_name[DSS_FREQ_MAX_SIZE][DSS_FREQ_NAME_LEN];
static int dss_freq_size;

#define dss_get_log(item)						\
long dss_get_len_##item##_log(void)					\
{									\
	return ARRAY_SIZE(dss_log->item);				\
}									\
EXPORT_SYMBOL_GPL(dss_get_len_##item##_log);				\
long dss_get_last_##item##_log_idx(void)				\
{									\
	return (atomic_read(&dss_log_misc.item##_log_idx) - 1) &	\
			(dss_get_len_##item##_log() - 1);		\
}									\
EXPORT_SYMBOL_GPL(dss_get_last_##item##_log_idx);			\
long dss_get_first_##item##_log_idx(void)				\
{									\
	int v = atomic_read(&dss_log_misc.item##_log_idx);		\
	if (v < dss_get_len_##item##_log())				\
		return 0;						\
	else								\
		return atomic_read(&dss_log_misc.item##_log_idx) &	\
			(dss_get_len_##item##_log() - 1);		\
}									\
EXPORT_SYMBOL_GPL(dss_get_first_##item##_log_idx);			\
struct item##_log *dss_get_last_##item##_log(void)			\
{									\
	return &dss_log->item[dss_get_last_##item##_log_idx()];		\
}									\
EXPORT_SYMBOL_GPL(dss_get_last_##item##_log);				\
struct item##_log *dss_get_first_##item##_log(void)			\
{									\
	return &dss_log->item[dss_get_first_##item##_log_idx()];	\
}									\
EXPORT_SYMBOL_GPL(dss_get_first_##item##_log);				\
struct item##_log *dss_get_##item##_log_by_idx(int idx)			\
{									\
	if (idx < 0 || idx >= dss_get_len_##item##_log())		\
		return NULL;						\
	return &dss_log->item[idx];					\
}									\
EXPORT_SYMBOL_GPL(dss_get_##item##_log_by_idx);				\
struct item##_log *dss_get_##item##_log_iter(int idx)			\
{									\
	if (idx < 0)							\
		idx = dss_get_len_##item##_log() - abs(idx);		\
	else if (idx >= dss_get_len_##item##_log())			\
		idx -= dss_get_len_##item##_log();			\
	return &dss_log->item[idx];					\
}									\
EXPORT_SYMBOL_GPL(dss_get_##item##_log_iter);				\
unsigned long dss_get_vaddr_##item##_log(void)				\
{									\
	return (unsigned long)dss_log->item;				\
}									\
EXPORT_SYMBOL_GPL(dss_get_vaddr_##item##_log)

#define dss_get_log_by_cpu(item)					\
long dss_get_len_##item##_log(void)						\
{									\
	return ARRAY_SIZE(dss_log->item);				\
}									\
EXPORT_SYMBOL_GPL(dss_get_len_##item##_log);				\
long dss_get_len_##item##_log_by_cpu(int cpu)				\
{									\
	if (cpu < 0 || cpu >= dss_get_len_##item##_log())		\
		return -EINVAL;						\
	return ARRAY_SIZE(dss_log->item[cpu]);				\
}									\
EXPORT_SYMBOL_GPL(dss_get_len_##item##_log_by_cpu);			\
long dss_get_last_##item##_log_idx(int cpu)				\
{									\
	if (cpu < 0 || cpu >= dss_get_len_##item##_log())		\
		return -EINVAL;						\
	return (atomic_read(&dss_log_misc.item##_log_idx[cpu]) - 1) &	\
			(dss_get_len_##item##_log_by_cpu(cpu) - 1);	\
}									\
EXPORT_SYMBOL_GPL(dss_get_last_##item##_log_idx);			\
long dss_get_first_##item##_log_idx(int cpu)				\
{									\
	int v = atomic_read(&dss_log_misc.item##_log_idx[cpu]);		\
	if (cpu < 0 || cpu >= dss_get_len_##item##_log())		\
		return -EINVAL;						\
	if (v < dss_get_len_##item##_log())				\
		return 0;						\
	else								\
		return atomic_read(&dss_log_misc.item##_log_idx[cpu]) &	\
			(dss_get_len_##item##_log_by_cpu(cpu) - 1);	\
}									\
EXPORT_SYMBOL_GPL(dss_get_first_##item##_log_idx);			\
struct item##_log *dss_get_last_##item##_log(int cpu)			\
{									\
	if (cpu < 0 || cpu >= dss_get_len_##item##_log())		\
		return NULL;						\
	if (dss_get_last_##item##_log_idx(cpu) < 0)			\
		return NULL;						\
	return &dss_log->item[cpu][dss_get_last_##item##_log_idx(cpu)];	\
}									\
EXPORT_SYMBOL_GPL(dss_get_last_##item##_log);				\
struct item##_log *dss_get_first_##item##_log(int cpu)			\
{									\
	if (cpu < 0 || cpu >= dss_get_len_##item##_log())		\
		return NULL;						\
	if (dss_get_first_##item##_log_idx(cpu) < 0)			\
		return NULL;						\
	return &dss_log->item[cpu][dss_get_first_##item##_log_idx(cpu)];\
}									\
EXPORT_SYMBOL_GPL(dss_get_first_##item##_log);				\
struct item##_log *dss_get_##item##_log_by_idx(int cpu, int idx)	\
{									\
	if (cpu < 0 || cpu >= dss_get_len_##item##_log())		\
		return NULL;						\
	if (idx < 0 || idx >= dss_get_len_##item##_log_by_cpu(cpu))	\
		return NULL;						\
	return &dss_log->item[cpu][idx];				\
}									\
EXPORT_SYMBOL_GPL(dss_get_##item##_log_by_idx);				\
struct item##_log *dss_get_##item##_log_by_cpu_iter(int cpu, int idx)	\
{									\
	if (cpu < 0 || cpu >= dss_get_len_##item##_log())		\
		return NULL;						\
	if (idx < 0)							\
		idx = dss_get_len_##item##_log_by_cpu(cpu) - abs(idx);	\
	else if (idx >= dss_get_len_##item##_log_by_cpu(cpu))		\
		idx -= dss_get_len_##item##_log_by_cpu(cpu);		\
	return &dss_log->item[cpu][idx];				\
}									\
EXPORT_SYMBOL_GPL(dss_get_##item##_log_by_cpu_iter);			\
unsigned long dss_get_vaddr_##item##_log_by_cpu(int cpu)		\
{									\
	if (cpu < 0 || cpu >= dss_get_len_##item##_log())		\
		return 0;						\
	return (unsigned long)dss_log->item[cpu];			\
}									\
EXPORT_SYMBOL_GPL(dss_get_vaddr_##item##_log_by_cpu)

dss_get_log_by_cpu(task);
dss_get_log_by_cpu(work);
dss_get_log_by_cpu(cpuidle);
dss_get_log_by_cpu(freq);
dss_get_log(suspend);
dss_get_log_by_cpu(irq);
dss_get_log_by_cpu(hrtimer);
dss_get_log_by_cpu(reg);
dss_get_log(clk);
dss_get_log(pmu);
dss_get_log(dm);
dss_get_log(regulator);
dss_get_log(thermal);
dss_get_log(acpm);
dss_get_log(print);

#define log_item_set_filed(id, log_name)			\
	log_item = &dss_log_items[DSS_LOG_##id##_ID];		\
	log_item->entry.vaddr = (size_t)(dss_log->log_name);	\
	log_item->entry.paddr = item->entry.paddr +		\
		(size_t)(dss_log->log_name) - (size_t)(dss_log);\
	log_item->entry.size = sizeof(dss_log->log_name);	\
	if (!log_item->entry.paddr || !log_item->entry.vaddr ||	\
			!log_item->entry.size)			\
		log_item->entry.enabled = false

static inline bool dbg_snapshot_is_log_item_enabled(int id)
{
	bool item_enabled = dbg_snapshot_get_item_enable(DSS_ITEM_KEVENTS);
	bool log_enabled;

	if (id < 0)
		return false;

	log_enabled = dss_log_items[id].entry.enabled;

	return dbg_snapshot_get_enable() && item_enabled && log_enabled;
}

void *dbg_snapshot_log_get_item_by_index(int index)
{
	if (index < 0 || index > ARRAY_SIZE(dss_log_items))
		return NULL;

	return  &dss_log_items[index];
}
EXPORT_SYMBOL_GPL(dbg_snapshot_log_get_item_by_index);

int dbg_snapshot_log_get_num_items(void)
{
	return ARRAY_SIZE(dss_log_items);
}
EXPORT_SYMBOL_GPL(dbg_snapshot_log_get_num_items);

int dbg_snapshot_get_freq_idx(const char *name)
{
	unsigned int i;

	for (i = 0; i < dss_freq_size; i++) {
		if (!strncmp(dss_freq_name[i], name, DSS_FREQ_NAME_LEN - 1))
			return i;
	}

	return -EFAULT;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_get_freq_idx);

void dbg_snapshot_log_output(void)
{
	unsigned long i;

	pr_info("debug-snapshot-log physical / virtual memory layout:\n");
	for (i = 0; i < ARRAY_SIZE(dss_log_items); i++) {
		if (dss_log_items[i].entry.enabled)
			pr_info("%-12s: phys:0x%zx / virt:0x%zx / size:0x%zx / en:%d\n",
				dss_log_items[i].name,
				dss_log_items[i].entry.paddr,
				dss_log_items[i].entry.vaddr,
				dss_log_items[i].entry.size,
				dss_log_items[i].entry.enabled);
	}
}

void dbg_snapshot_set_enable_log_item(const char *name, int en)
{
	struct dbg_snapshot_log_item *log_item;
	int i;

	if (!name)
		return;

	for (i = 0; i < ARRAY_SIZE(dss_log_items); i++) {
		log_item = &dss_log_items[i];
		if (log_item->name &&
				!strncmp(log_item->name, name, strlen(name))) {
			log_item->entry.enabled = en;
			pr_info("log item - %s is %sabled\n",
					name, en ? "en" : "dis");
			break;
		}
	}
}

static bool dbg_snapshot_get_enable_log_item(const char *name)
{
	struct dbg_snapshot_log_item *log_item;
	int i;

	if (!name)
		return false;

	for (i = 0; i < ARRAY_SIZE(dss_log_items); i++) {
		log_item = &dss_log_items[i];
		if (log_item->name &&
				!strncmp(log_item->name, name, strlen(name))) {
			return log_item->entry.enabled;
		}
	}

	return false;
}

static void dbg_snapshot_task(unsigned int cpu, struct task_struct *task)
{
	unsigned long i = atomic_fetch_inc(&dss_log_misc.task_log_idx[cpu]) &
		(ARRAY_SIZE(dss_log->task[0]) - 1);

	dss_log->task[cpu][i].time = local_clock();
	dss_log->task[cpu][i].task = task;
	dss_log->task[cpu][i].pid = task_pid_nr(task);
	strncpy(dss_log->task[cpu][i].task_comm, task->comm, TASK_COMM_LEN - 1);
}

static void dbg_snapshot_sched_switch(void *ignore, bool preempt,
				      struct task_struct *prev,
				      struct task_struct *next)
{
	dbg_snapshot_task(raw_smp_processor_id(), next);
}

static void dbg_snapshot_work(work_func_t fn, int en)
{
	unsigned int cpu = raw_smp_processor_id();
	unsigned long i = atomic_fetch_inc(&dss_log_misc.work_log_idx[cpu]) &
		(ARRAY_SIZE(dss_log->work[0]) - 1);

	dss_log->work[cpu][i].time = local_clock();
	dss_log->work[cpu][i].fn = fn;
	dss_log->work[cpu][i].en = en;
}

static void dbg_snapshot_wq_start(void *ignore, struct work_struct *work)
{
	dbg_snapshot_work(work->func, DSS_FLAG_IN);
}

static void dbg_snapshot_wq_end(void *ignore, struct work_struct *work,
				work_func_t func)
{
	dbg_snapshot_work(func, DSS_FLAG_OUT);
}

void dbg_snapshot_cpuidle(char *modes, unsigned int state, s64 diff, int en)
{
	unsigned int cpu = raw_smp_processor_id();
	unsigned long i;

	if (unlikely(!dbg_snapshot_is_log_item_enabled(DSS_LOG_CPUIDLE_ID)))
		return;

	i = atomic_fetch_inc(&dss_log_misc.cpuidle_log_idx[cpu]) &
		(ARRAY_SIZE(dss_log->cpuidle[0]) - 1);
	dss_log->cpuidle[cpu][i].time = cpu_clock(cpu);
	dss_log->cpuidle[cpu][i].modes = modes;
	dss_log->cpuidle[cpu][i].state = state;
	dss_log->cpuidle[cpu][i].num_online_cpus = num_online_cpus();
	dss_log->cpuidle[cpu][i].delta = (int)diff;
	dss_log->cpuidle[cpu][i].en = en;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_cpuidle);

static void dbg_snapshot_cpu_idle(void *ignore, unsigned int index,
				  unsigned int cpu)
{
	if (index == PWR_EVENT_EXIT)
		dbg_snapshot_cpuidle("C1", 0, 0, DSS_FLAG_OUT);
	else
		dbg_snapshot_cpuidle("C1", 0, 0, DSS_FLAG_IN);
}

static void dbg_snapshot_irq(int irq, void *fn, int en)
{
	unsigned int cpu = raw_smp_processor_id();
	unsigned long i = atomic_fetch_inc(&dss_log_misc.irq_log_idx[cpu]) &
		(ARRAY_SIZE(dss_log->irq[0]) - 1);
	unsigned long flags = arch_local_irq_save();

	dss_log->irq[cpu][i].time = local_clock();
	dss_log->irq[cpu][i].irq = irq;
	dss_log->irq[cpu][i].fn = fn;
	dss_log->irq[cpu][i].desc = irq_to_desc(irq);
	dss_log->irq[cpu][i].en = en;

	arch_local_irq_restore(flags);
}

static void dbg_snapshot_irq_entry(void *ignore, int irq,
				   struct irqaction *action)
{
	dbg_snapshot_irq(irq, action->handler, DSS_FLAG_IN);
}

static void dbg_snapshot_irq_exit(void *ignore, int irq,
				   struct irqaction *action, int ret)
{
	dbg_snapshot_irq(irq, action->handler, DSS_FLAG_OUT);
}

static void dbg_snapshot_hrtimer(struct hrtimer *timer, s64 now,
				 void *fn, int en)
{
	unsigned int cpu = raw_smp_processor_id();
	unsigned long i = atomic_fetch_inc(&dss_log_misc.hrtimer_log_idx[cpu]) &
		(ARRAY_SIZE(dss_log->hrtimer[0]) - 1);

	dss_log->hrtimer[cpu][i].time = local_clock();
	dss_log->hrtimer[cpu][i].now = now;
	dss_log->hrtimer[cpu][i].timer = timer;
	dss_log->hrtimer[cpu][i].fn = fn;
	dss_log->hrtimer[cpu][i].en = en;
}

static void dbg_snapshot_hrtimer_entry(void *ignore, struct hrtimer *timer,
				       ktime_t *now)
{
	dbg_snapshot_hrtimer(timer, *now, timer->function, DSS_FLAG_IN);
}

static void dbg_snapshot_hrtimer_exit(void *ignore, struct hrtimer *timer)
{
	dbg_snapshot_hrtimer(timer, 0, timer->function, DSS_FLAG_OUT);
}

__maybe_unused static void dbg_snapshot_reg(unsigned char io_type,
					unsigned int data_type,
					unsigned long long val, void *addr,
					unsigned long fn)
{
	unsigned int cpu = raw_smp_processor_id();
	unsigned long i = atomic_fetch_inc(&dss_log_misc.reg_log_idx[cpu]) &
		(ARRAY_SIZE(dss_log->reg[0]) - 1);

	dss_log->reg[cpu][i].time = local_clock();
	dss_log->reg[cpu][i].io_type = io_type;
	dss_log->reg[cpu][i].data_type = data_type;
	dss_log->reg[cpu][i].val = val;
	dss_log->reg[cpu][i].addr = (void *)virt_to_phys(addr);
	dss_log->reg[cpu][i].caller = (void *)fn;
}

__maybe_unused static void dbg_snapshot_reg_write(void *ignore,
						unsigned long fn,
						u64 val, u8 width,
						volatile void __iomem *addr)
{
	dbg_snapshot_reg('w', width, val, (void *)addr, fn);
}

__maybe_unused static void dbg_snapshot_reg_read(void *ignore,
					unsigned long fn, u8 width,
					const volatile void __iomem *addr)
{
	dbg_snapshot_reg('r', width, 0, (void *)addr, fn);
}

__maybe_unused static void dbg_snapshot_reg_post_read(void *ignore,
					unsigned long fn, u64 val, u8 width,
					const volatile void __iomem *addr)
{
	dbg_snapshot_reg('R', width, val, (void *)addr, fn);
}

static void dbg_snapshot_suspend(const char *log, struct device *dev,
				int event, int en)
{
	unsigned long i = atomic_fetch_inc(&dss_log_misc.suspend_log_idx) &
		(ARRAY_SIZE(dss_log->suspend) - 1);
	const char *tmp_log =  log ? log : "";
	const char *tmp_dev =  dev ? dev_name(dev) : "";

	dss_log->suspend[i].time = local_clock();
	dss_log->suspend[i].event = event;
	dss_log->suspend[i].core = raw_smp_processor_id();
	dss_log->suspend[i].en = en;

	strncpy(dss_log->suspend[i].log, tmp_log, SUSPEND_LOG_LEN - 1);
	strncpy(dss_log->suspend[i].dev, tmp_dev, SUSPEND_DEV_LEN - 1);
}

static void dbg_snapshot_suspend_resume(void *ignore, const char *action,
					int event, bool start)
{
	dbg_snapshot_suspend(action, NULL, event,
			start ? DSS_FLAG_IN : DSS_FLAG_OUT);
}

static void dbg_snapshot_dev_pm_cb_start(void *ignore, struct device *dev,
					const char *info, int event)
{
	dbg_snapshot_suspend(info, dev, event, DSS_FLAG_IN);
}

static void dbg_snapshot_dev_pm_cb_end(void *ignore, struct device *dev,
					int error)
{
	dbg_snapshot_suspend(NULL, dev, error, DSS_FLAG_OUT);
}

void dbg_snapshot_regulator(unsigned long long timestamp, char *f_name,
			unsigned int addr, unsigned int volt,
			unsigned int rvolt, int en)
{
	unsigned long i;

	if (!dbg_snapshot_is_log_item_enabled(DSS_LOG_REGULATOR_ID))
		return;

	i = atomic_fetch_inc(&dss_log_misc.regulator_log_idx) &
		(ARRAY_SIZE(dss_log->regulator) - 1);

	dss_log->regulator[i].time = local_clock();
	dss_log->regulator[i].cpu = raw_smp_processor_id();
	dss_log->regulator[i].acpm_time = timestamp;
	strncpy(dss_log->regulator[i].name, f_name,
			min((int)strlen(f_name), SZ_16 - 1));
	dss_log->regulator[i].reg = addr;
	dss_log->regulator[i].en = en;
	dss_log->regulator[i].voltage = volt;
	dss_log->regulator[i].raw_volt = rvolt;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_regulator);

void dbg_snapshot_thermal(void *data, unsigned int temp, char *name,
			unsigned int max_cooling)
{
	unsigned long i;

	if (!dbg_snapshot_is_log_item_enabled(DSS_LOG_THERMAL_ID))
		return;

	i = atomic_fetch_inc(&dss_log_misc.thermal_log_idx) &
		(ARRAY_SIZE(dss_log->thermal) - 1);

	dss_log->thermal[i].time = local_clock();
	dss_log->thermal[i].cpu = raw_smp_processor_id();
	dss_log->thermal[i].data = (struct exynos_tmu_data *)data;
	dss_log->thermal[i].temp = temp;
	dss_log->thermal[i].cooling_device = name;
	dss_log->thermal[i].cooling_state = max_cooling;

}
EXPORT_SYMBOL_GPL(dbg_snapshot_thermal);

void dbg_snapshot_clk(void *clock, const char *func_name, unsigned long arg, int mode)
{
	unsigned long i;

	if (!dbg_snapshot_is_log_item_enabled(DSS_LOG_CLK_ID))
		return;

	i = atomic_fetch_inc(&dss_log_misc.clk_log_idx) &
		(ARRAY_SIZE(dss_log->clk) - 1);

	dss_log->clk[i].time = local_clock();
	dss_log->clk[i].mode = mode;
	dss_log->clk[i].arg = arg;
	dss_log->clk[i].clk = (struct clk_hw *)clock;
	dss_log->clk[i].f_name = func_name;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_clk);

void dbg_snapshot_pmu(int id, const char *func_name, int mode)
{
	unsigned long i;

	if (!dbg_snapshot_is_log_item_enabled(DSS_LOG_PMU_ID))
		return;

	i = atomic_fetch_inc(&dss_log_misc.pmu_log_idx) &
		(ARRAY_SIZE(dss_log->pmu) - 1);

	dss_log->pmu[i].time = local_clock();
	dss_log->pmu[i].mode = mode;
	dss_log->pmu[i].id = id;
	dss_log->pmu[i].f_name = func_name;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_pmu);

void dbg_snapshot_freq(int type, unsigned int old_freq,
			unsigned int target_freq, int en)
{
	unsigned long i;

	if (unlikely(type < 0 || type > dss_freq_size))
		return;

	if (unlikely(!dbg_snapshot_is_log_item_enabled(DSS_LOG_FREQ_ID)))
		return;

	i = atomic_fetch_inc(&dss_log_misc.freq_log_idx[type]) &
		(ARRAY_SIZE(dss_log->freq[0]) - 1);

	dss_log->freq[type][i].time = local_clock();
	dss_log->freq[type][i].cpu = raw_smp_processor_id();
	dss_log->freq[type][i].old_freq = old_freq;
	dss_log->freq[type][i].target_freq = target_freq;
	dss_log->freq[type][i].en = en;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_freq);

void dbg_snapshot_dm(int type, unsigned int min, unsigned int max,
			s32 wait_t, s32 t)
{
	unsigned long i;

	if (!dbg_snapshot_is_log_item_enabled(DSS_LOG_DM_ID))
		return;

	i = atomic_fetch_inc(&dss_log_misc.dm_log_idx) &
		(ARRAY_SIZE(dss_log->dm) - 1);

	dss_log->dm[i].time = local_clock();
	dss_log->dm[i].cpu = raw_smp_processor_id();
	dss_log->dm[i].dm_num = type;
	dss_log->dm[i].min_freq = min;
	dss_log->dm[i].max_freq = max;
	dss_log->dm[i].wait_dmt = wait_t;
	dss_log->dm[i].do_dmt = t;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_dm);

void dbg_snapshot_acpm(unsigned long long timestamp, const char *log,
			unsigned int data)
{
	unsigned long i;
	unsigned int len;

	if (unlikely(!dbg_snapshot_is_log_item_enabled(DSS_LOG_ACPM_ID)))
		return;

	i = atomic_fetch_inc(&dss_log_misc.acpm_log_idx) &
		(ARRAY_SIZE(dss_log->acpm) - 1);

	len = min((int)strlen(log), 8);
	dss_log->acpm[i].time = local_clock();
	dss_log->acpm[i].acpm_time = timestamp;
	strncpy(dss_log->acpm[i].log, log, len);
	dss_log->acpm[i].log[len] = '\0';
	dss_log->acpm[i].data = data;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_acpm);

void dbg_snapshot_printk(const char *fmt, ...)
{
	unsigned long i;
	va_list args;
	int ret = 0;

	if (!dbg_snapshot_is_log_item_enabled(DSS_LOG_PRINTK_ID))
		return;

	i = atomic_fetch_inc(&dss_log_misc.print_log_idx) &
		(ARRAY_SIZE(dss_log->print) - 1);

	va_start(args, fmt);
	ret = vsnprintf(dss_log->print[i].log, sizeof(dss_log->print[i].log),
			fmt, args);
	if (ret < 0)
		return;
	va_end(args);

	dss_log->print[i].time = local_clock();
	dss_log->print[i].cpu = raw_smp_processor_id();
}
EXPORT_SYMBOL_GPL(dbg_snapshot_printk);

static inline void dbg_snapshot_get_sec(unsigned long long ts,
					unsigned long *sec,
					unsigned long *msec)
{
	*sec = ts / NSEC_PER_SEC;
	*msec = (ts % NSEC_PER_SEC) / USEC_PER_MSEC;
}

static void dbg_snapshot_print_last_irq(unsigned int cpu)
{
	struct dbg_snapshot_log_item *log_item = &dss_log_items[DSS_LOG_IRQ_ID];
	unsigned long idx, sec, msec;

	if (!log_item->entry.enabled)
		return;

	if (cpu >= DSS_NR_CPUS)
		return;

	idx = dss_get_last_irq_log_idx(cpu);
	if (idx >= DSS_IRQ_LOG_MAX)
		return;

	dbg_snapshot_get_sec(dss_log->irq[cpu][idx].time, &sec, &msec);

	pr_info("%-12s: [%4ld] %10lu.%06lu sec, %10s: %pS, %8s: %8d, %10s: %2d, %s\n",
			">>> irq", idx, sec, msec,
			"handler", dss_log->irq[cpu][idx].fn,
			"irq", dss_log->irq[cpu][idx].irq,
			"en", dss_log->irq[cpu][idx].en,
			(dss_log->irq[cpu][idx].en == 1) ? "[Mismatch]" : "");
}

static void dbg_snapshot_print_last_task(unsigned int cpu)
{
	struct dbg_snapshot_log_item *log_item = &dss_log_items[DSS_LOG_TASK_ID];
	unsigned long idx, sec, msec;
	struct task_struct *task;

	if (!log_item->entry.enabled)
		return;

	if (cpu >= DSS_NR_CPUS)
		return;

	idx = dss_get_last_task_log_idx(cpu);
	if (idx >= DSS_IRQ_LOG_MAX)
		return;

	dbg_snapshot_get_sec(dss_log->task[cpu][idx].time, &sec, &msec);
	task = dss_log->task[cpu][idx].task;

	pr_info("%-12s: [%4lu] %10lu.%06lu sec, %10s: %-16s, %8s: 0x%-16px, %10s: %16llu\n",
			">>> task", idx, sec, msec,
			"task_comm", (task) ? task->comm : "NULL",
			"task", task,
			"exec_start", (task) ? task->se.exec_start : 0);
}

static void dbg_snapshot_print_last_work(unsigned int cpu)
{
	struct dbg_snapshot_log_item *log_item = &dss_log_items[DSS_LOG_WORK_ID];
	unsigned long idx, sec, msec;

	if (!log_item->entry.enabled)
		return;

	if (cpu >= DSS_NR_CPUS)
		return;

	idx = dss_get_last_work_log_idx(cpu);
	if (idx >= DSS_IRQ_LOG_MAX)
		return;

	dbg_snapshot_get_sec(dss_log->work[cpu][idx].time, &sec, &msec);

	pr_info("%-12s: [%4lu] %10lu.%06lu sec, %10s: %pS, %3s: %3d %s\n",
			">>> work", idx, sec, msec,
			"work_fn", dss_log->work[cpu][idx].fn,
			"en", dss_log->work[cpu][idx].en,
			(dss_log->work[cpu][idx].en == 1) ? "[Mismatch]" : "");
}

static void dbg_snapshot_print_last_cpuidle(unsigned int cpu)
{
	struct dbg_snapshot_log_item *log_item = &dss_log_items[DSS_LOG_CPUIDLE_ID];
	unsigned long idx, sec, msec;

	if (!log_item->entry.enabled)
		return;

	if (cpu >= DSS_NR_CPUS)
		return;

	idx = dss_get_last_cpuidle_log_idx(cpu);
	if (idx >= DSS_IRQ_LOG_MAX)
		return;

	dbg_snapshot_get_sec(dss_log->cpuidle[cpu][idx].time, &sec, &msec);

	pr_info("%-12s: [%4lu] %10lu.%06lu sec, %10s: %s, %6s: %2d, %10s: %10d"
			", %12s: %2d, %3s: %3d %s\n",
			">>> cpuidle", idx, sec, msec,
			"modes", dss_log->cpuidle[cpu][idx].modes,
			"state", dss_log->cpuidle[cpu][idx].state,
			"stay time", dss_log->cpuidle[cpu][idx].delta,
			"online_cpus", dss_log->cpuidle[cpu][idx].num_online_cpus,
			"en", dss_log->cpuidle[cpu][idx].en,
			(dss_log->cpuidle[cpu][idx].en == 1) ? "[Mismatch]" : "");
}

static void dbg_snapshot_print_lastinfo(void)
{
	int cpu;

	pr_info("<last info(SYS OS)>\n");
	for (cpu = 0; cpu < DSS_NR_CPUS; cpu++) {
		pr_info("CPU ID: %d ----------------------------------\n", cpu);
		dbg_snapshot_print_last_task(cpu);
		dbg_snapshot_print_last_work(cpu);
		dbg_snapshot_print_last_irq(cpu);
		dbg_snapshot_print_last_cpuidle(cpu);
	}
}

static void dbg_snapshot_print_freqinfo(void)
{
	struct dbg_snapshot_log_item *log_item = &dss_log_items[DSS_LOG_FREQ_ID];
	unsigned long i, sec, msec;
	unsigned long old_freq, target_freq;
	long idx;

	if (!log_item->entry.enabled)
		return;

	pr_info("\n<last freq info(SYS OS)>\n");
	for (i = 0; i < dss_freq_size; i++) {
		if (!atomic_read(&dss_log_misc.freq_log_idx[i])) {
			pr_info("%10s: no information\n", dss_freq_name[i]);
			continue;
		}
		idx = dss_get_last_freq_log_idx(i);
		if (idx < 0 || idx >= DSS_FREQ_LOG_MAX)
			break;

		dbg_snapshot_get_sec(dss_log->freq[i][idx].time, &sec, &msec);
		old_freq = dss_log->freq[i][idx].old_freq;
		target_freq = dss_log->freq[i][idx].target_freq;
		pr_info("%10s[%4lu]: %10lu.%06lu sec, %12s: %5luMhz, %12s: %5luMhz, %3s: %d %s\n",
				dss_freq_name[i], idx, sec, msec,
				"old_freq", old_freq / 1000,
				"target_freq", target_freq / 1000,
				"en", dss_log->freq[i][idx].en,
				(dss_log->freq[i][idx].en == 1) ? "[Mismatch]" : "");
	}
}

#ifndef arch_irq_stat
#define arch_irq_stat() 0
#endif

static void dbg_snapshot_print_irq(void)
{
	int i, cpu;
	u64 sum = 0;

	for_each_possible_cpu(cpu) {
		sum += kstat_cpu_irqs_sum(cpu);
		if (cpu >= DSS_NR_CPUS)
			break;
	}

	sum += arch_irq_stat();

	pr_info("<irq info(SYS OS)>\n");
	pr_info("----------------------------------------------------------\n");
	pr_info("sum irq : %llu", sum);
	pr_info("----------------------------------------------------------\n");

	for_each_irq_nr(i) {
		struct irq_desc *desc = irq_to_desc(i);
		unsigned int irq_stat = 0;
		const char *name;

		if (!desc)
			continue;

		for_each_possible_cpu(cpu) {
			irq_stat += *per_cpu_ptr(desc->kstat_irqs, cpu);
			if (cpu >= DSS_NR_CPUS)
				return;
		}

		if (!irq_stat)
			continue;

		if (desc->action && desc->action->name)
			name = desc->action->name;
		else
			name = "???";
		pr_info("irq-%-4d(hwirq-%-3d) : %8u %s\n",
			i, (int)desc->irq_data.hwirq, irq_stat, name);
	}
}

void dbg_snapshot_print_log_report(void)
{
	if (unlikely(!dbg_snapshot_get_enable()))
		return;

	pr_info("==========================================================\n");
	pr_info("Panic Report(SYS OS)\n");
	pr_info("==========================================================\n");
	dbg_snapshot_print_lastinfo();
	dbg_snapshot_print_freqinfo();
	dbg_snapshot_print_irq();
	pr_info("==========================================================\n");
}

static void dbg_snapshot_init_log_header(void)
{
	struct dbg_snapshot_log_header src_header = {
		.magic = DSS_LOG_MAGIC,
		.version = DSS_LOG_HEADER_VERSION,
		.nr_cpus = DSS_NR_CPUS,
		.freq_domain_num = DSS_DOMAIN_NUM,
		.task_log_max = DSS_TASK_LOG_MAX,
		.work_log_max = DSS_WORK_LOG_MAX,
		.cpuidle_log_max = DSS_CPUIDLE_LOG_MAX,
		.suspend_log_max = DSS_SUSPEND_LOG_MAX,
		.irq_log_max = DSS_IRQ_LOG_MAX,
		.clk_log_max = DSS_CLK_LOG_MAX,
		.pmu_log_max = DSS_PMU_LOG_MAX,
		.freq_log_max = DSS_FREQ_LOG_MAX,
		.dm_log_max = DSS_DM_LOG_MAX,
		.hrtimer_log_max = DSS_HRTIMER_LOG_MAX,
		.reg_log_max = DSS_REG_LOG_MAX,
		.regulator_log_max = DSS_REGULATOR_LOG_MAX,
		.thermal_log_max = DSS_THERMAL_LOG_MAX,
		.acpm_log_max = DSS_ACPM_LOG_MAX,
		.print_log_max = DSS_PRINT_LOG_MAX,
	};

	memcpy(&dss_log->header, &src_header, sizeof(src_header));
}

void dbg_snapshot_init_log(void)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item(DSS_ITEM_KEVENTS);
	struct dbg_snapshot_log_item *log_item;
	struct device_node *np = dss_desc.dev->of_node;
	struct property *prop;
	const char *str;
	unsigned int i = 0;

	if (!item) {
		dev_err(dss_desc.dev, "Get Item is NULL\n");
		return;
	}
	if (!dbg_snapshot_get_item_enable(DSS_ITEM_KEVENTS)) {
		dev_err(dss_desc.dev, "DSS logging is disabled.\n");
		return;
	}

	if (dbg_snapshot_get_item_size(DSS_ITEM_KEVENTS)
			< sizeof(struct dbg_snapshot_log)) {
		dbg_snapshot_set_item_enable(DSS_ITEM_KEVENTS, 0);
		dev_warn(dss_desc.dev,
			"log_kevents is less than sizeof(struct dbg_snapshot_log) %lu\n",
			sizeof(struct dbg_snapshot_log));
		return;
	}

	dbg_snapshot_init_log_header();

	log_item_set_filed(TASK, task);
	log_item_set_filed(WORK, work);
	log_item_set_filed(CPUIDLE, cpuidle);
	log_item_set_filed(IRQ, irq);
	log_item_set_filed(HRTIMER, hrtimer);
	log_item_set_filed(REG, reg);
	log_item_set_filed(FREQ, freq);
	log_item_set_filed(SUSPEND, suspend);
	log_item_set_filed(CLK, clk);
	log_item_set_filed(PMU, pmu);
	log_item_set_filed(DM, dm);
	log_item_set_filed(REGULATOR, regulator);
	log_item_set_filed(THERMAL, thermal);
	log_item_set_filed(ACPM, acpm);
	log_item_set_filed(PRINTK, print);

	if (dbg_snapshot_get_enable_log_item(DSS_LOG_TASK))
		register_trace_sched_switch(dbg_snapshot_sched_switch, NULL);

	if (dbg_snapshot_get_enable_log_item(DSS_LOG_CPUIDLE))
		register_trace_cpu_idle(dbg_snapshot_cpu_idle, NULL);

	if (dbg_snapshot_get_enable_log_item(DSS_LOG_WORK)) {
		register_trace_workqueue_execute_start(dbg_snapshot_wq_start, NULL);
		register_trace_workqueue_execute_end(dbg_snapshot_wq_end, NULL);
	}
	if (dbg_snapshot_get_enable_log_item(DSS_LOG_IRQ)) {
		register_trace_irq_handler_entry(dbg_snapshot_irq_entry, NULL);
		register_trace_irq_handler_exit(dbg_snapshot_irq_exit, NULL);
	}
	if (dbg_snapshot_get_enable_log_item(DSS_LOG_HRTIMER)) {
		register_trace_hrtimer_expire_entry(dbg_snapshot_hrtimer_entry, NULL);
		register_trace_hrtimer_expire_exit(dbg_snapshot_hrtimer_exit, NULL);
	}
	if (dbg_snapshot_get_enable_log_item(DSS_LOG_SUSPEND)) {
		register_trace_suspend_resume(dbg_snapshot_suspend_resume, NULL);
		register_trace_device_pm_callback_start(dbg_snapshot_dev_pm_cb_start, NULL);
		register_trace_device_pm_callback_end(dbg_snapshot_dev_pm_cb_end, NULL);
	}

	if (dbg_snapshot_get_enable_log_item(DSS_LOG_REG)) {
		register_rwmmio_write(dbg_snapshot_reg_write, NULL);
		register_rwmmio_read(dbg_snapshot_reg_read, NULL);
		register_rwmmio_post_read(dbg_snapshot_reg_post_read, NULL);
	}

	dss_freq_size = of_property_count_strings(np, "freq_names");
	if (dss_freq_size < 0)
		dss_freq_size = 0;

	if (dss_freq_size > DSS_FREQ_MAX_SIZE)
		dss_freq_size = DSS_FREQ_MAX_SIZE;

	of_property_for_each_string(np, "freq_names", prop, str) {
		strncpy(dss_freq_name[i++], str, DSS_FREQ_NAME_LEN - 1);
	}

	dbg_snapshot_log_output();
}
