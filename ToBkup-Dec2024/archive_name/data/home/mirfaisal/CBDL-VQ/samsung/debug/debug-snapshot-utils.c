// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 */

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/notifier.h>
#include <linux/delay.h>
#include <linux/sched/clock.h>
#include <linux/sched/debug.h>
#include <linux/sched/task_stack.h>
#include <linux/context_tracking.h>
#include <linux/nmi.h>
#include <linux/init_task.h>
#include <linux/reboot.h>
#include <linux/kdebug.h>
#include <linux/timekeeping.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/input.h>
#include <linux/dma-mapping.h>
#include <linux/panic_notifier.h>
#include <linux/android_debug_symbols.h>

#include <asm/cputype.h>
#include <asm/stacktrace.h>

#include <soc/samsung/debug-snapshot.h>
#include <soc/samsung/exynos-pmu.h>
#include "debug-snapshot-local.h"
#include "system-regs.h"

#if IS_ENABLED(CONFIG_ANDROID_VENDOR_HOOKS)
#include <trace/hooks/debug.h>
#endif
#if IS_ENABLED(CONFIG_VLM)
#include <nk/nkern.h>
#endif

#define ARM_CPU_PART_HERCULES           0xD41
#define ARM_CPU_PART_HERA               0xD44

#define DSS_KMOD_STR_SZ	(256)

#define BACKTRACE_CPU_INVALID	(-1)


/*  Panic core's backtrace logging  */
static struct dbg_snapshot_backtrace_data *dss_backtrace;
atomic_t backtrace_cpu = ATOMIC_INIT(BACKTRACE_CPU_INVALID);

#if IS_ENABLED(CONFIG_VLM)
#define VCPU_TO_PCPU_INVALID	(-1)
static int dss_smp_processor_id_to_pcpu[NR_CPUS];

static void dbg_snapshot_map_smp_processor_id_to_pcpu(void *args)
{
	int *map_table = args;
	int smp_processor_id = raw_smp_processor_id();

	if (map_table[smp_processor_id] == VCPU_TO_PCPU_INVALID)
		map_table[smp_processor_id] = hyp_call_get_cpuid();
}

static void dbg_snapshot_smp_processor_id_to_pcpu_init(void)
{
	int i;

	for (i = 0; i < NR_CPUS; i++)
		dss_smp_processor_id_to_pcpu[i] = VCPU_TO_PCPU_INVALID;
}

int dbg_snapshot_get_pcpu_id(int smp_processor_id)
{
	if (smp_processor_id >= NR_CPUS)
		return -EINVAL;
	if ((dss_smp_processor_id_to_pcpu[smp_processor_id] < 0)
			&& (smp_processor_id == raw_smp_processor_id()))
		dbg_snapshot_map_smp_processor_id_to_pcpu(dss_smp_processor_id_to_pcpu);

	return dss_smp_processor_id_to_pcpu[smp_processor_id];
}
#else
int dbg_snapshot_get_pcpu_id(int smp_processor_id)
{
	return smp_processor_id;
}
#endif
EXPORT_SYMBOL_GPL(dbg_snapshot_get_pcpu_id);

struct dbg_snapshot_mmu_reg {
	long SCTLR_EL1;
	long TTBR0_EL1;
	long TTBR1_EL1;
	long TCR_EL1;
	long ESR_EL1;
	long FAR_EL1;
	long CONTEXTIDR_EL1;
	long TPIDR_EL0;
	long TPIDRRO_EL0;
	long TPIDR_EL1;
	long MAIR_EL1;
	long ELR_EL1;
	long SP_EL0;
};

extern void flush_cache_all(void);
static struct pt_regs __percpu **dss_core_reg;
static struct dbg_snapshot_mmu_reg __percpu **dss_mmu_reg;
static struct dbg_snapshot_helper_ops dss_soc_ops;

void cache_flush_all(void)
{
	flush_cache_all();
}
EXPORT_SYMBOL_GPL(cache_flush_all);

ATOMIC_NOTIFIER_HEAD(dump_task_notifier_list);

void register_dump_one_task_notifier(struct notifier_block *nb)
{
	atomic_notifier_chain_register(&dump_task_notifier_list, nb);
}
EXPORT_SYMBOL_GPL(register_dump_one_task_notifier);

static const char *dbg_snapshot_wday_to_string(int wday)
{
	static const char *day[7] = { "Sun", "Mon", "Tue", "Wed",
					"Thu", "Fri", "Sat" };
	const char *ret;

	if (wday >= 0 && wday <= 6)
		ret = day[wday];
	else
		ret = NULL;

	return ret;
}

static const char *dbg_snapshot_mon_to_string(int mon)
{
	static const char *month[12] = { "Jan", "Feb", "Mar", "Apr",
					"May", "Jun", "Jul", "Aug",
					"Sep", "Oct", "Nov", "Dec" };
	const char *ret;

	if (mon >= 0 && mon <= 11)
		ret = month[mon];
	else
		ret = NULL;

	return ret;
}

static void dbg_snapshot_backtrace_start(unsigned long pc, unsigned long lr,
					struct pt_regs *regs, void *data)
{
	struct timespec64 ts64;
	struct rtc_time tm;
	size_t size;
	u64 curr_idx;
	u64 tv_kernel;
	unsigned long rem_nsec;
	char *vaddr;

	if (dss_backtrace->stop_logging)
		return;

	size = dss_backtrace->size;
	curr_idx = dss_backtrace->curr_idx;
	vaddr = (char *)dss_backtrace->vaddr;

	ktime_get_real_ts64(&ts64);
	rtc_time64_to_tm(ts64.tv_sec - (sys_tz.tz_minuteswest * 60), &tm);

	tv_kernel = local_clock();
	rem_nsec = do_div(tv_kernel, 1000000000);
	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
				"$** *** *** *** *** *** *** *** "
				"Fatal *** *** *** *** *** *** *** **$\n");
	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
				"%s Log Time:[%s %s %d %02d:%02d:%02d UTC %d][%llu.%06lu]\n",
				(regs == NULL) ? "Panic" : "Exception",
				dbg_snapshot_wday_to_string(tm.tm_wday),
				dbg_snapshot_mon_to_string(tm.tm_mon),
				tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
				tm.tm_year + 1900, tv_kernel, rem_nsec / 1000);
	if (regs) {
		struct die_args *args = (struct die_args *)data;

		curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
					"Internal error: %s: %lx\n\n",
					args->str, args->err);
	} else {
		curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
					"Kernel panic - not syncing %s\n\n",
					(char *)data);
	}
	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
				"PC is at %pS\n", (void *)pc);
	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
				"LR is at %pS\n", (void *)lr);
	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
				"Current Executing Process:\n[CPU, %d][%s, %d][AARCH%d]\n\n",
				raw_smp_processor_id(),
				current->comm, current->pid,
				compat_user_mode(task_pt_regs(current)) ? 32 : 64);
	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
				"Backtrace:\n");

	dss_backtrace->curr_idx = curr_idx;
}

static void dbg_snapshot_backtrace_log(unsigned long where)
{
	size_t size;
	u64 curr_idx;
	char *vaddr;

	if (dss_backtrace->stop_logging)
		return;

	size = dss_backtrace->size;
	curr_idx = dss_backtrace->curr_idx;
	vaddr = (char *)dss_backtrace->vaddr;

	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
			     "%pS\n", (void *)where);
	dss_backtrace->curr_idx = curr_idx;
}

static void dbg_snapshot_backtrace_stop(void)
{
	size_t size;
	u64 curr_idx;
	char *vaddr;

	if (dss_backtrace->stop_logging)
		return;

	size = dss_backtrace->size;
	curr_idx = dss_backtrace->curr_idx;
	vaddr = (char *)dss_backtrace->vaddr;

	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
			     "0xffffffffffffffff\n");
	curr_idx += scnprintf(vaddr + curr_idx, size - curr_idx,
			     "$** *** *** *** *** *** *** *** "
			     "Fatal *** *** *** *** *** *** *** **$\n");
	dss_backtrace->curr_idx = curr_idx;
	if (curr_idx >= size)
		vaddr[size - 1] = 0;
	dss_backtrace->stop_logging = true;

	dbg_snapshot_set_val64_offset(dss_backtrace->paddr,
					DSS_OFFSET_BACKTRACE_PADDR);
	dbg_snapshot_set_val64_offset(dss_backtrace->size,
					DSS_OFFSET_BACKTRACE_SIZE);
	dbg_snapshot_set_val_offset(raw_smp_processor_id(),
					DSS_OFFSET_BACKTRACE_CPU);
	dbg_snapshot_set_val_offset(DSS_BACKTRACE_MAGIC,
					DSS_OFFSET_BACKTRACE_MAGIC);
}

static int dbg_snapshot_on_accessible_stack(const struct task_struct *tsk,
						unsigned long sp)
{
	if (tsk != current || preemptible())
		return false;
	if (sp < UINT_MAX)
		return false;

	return true;
}

static int dbg_snapshot_unwind_frame(struct task_struct *tsk,
					struct stackframe *frame)
{
	unsigned long fp = frame->fp;

	if (fp & 0xf)
		return -EINVAL;

	if (!tsk)
		tsk = current;

	if (!dbg_snapshot_on_accessible_stack(tsk, fp))
		return -EINVAL;

	/*
	 * Many stack exception processing is missing. Exception may occur
	 * during processing, but try only once to prevent recursive error.
	 */

	/*
	 * Record this frame record's values and location. The prev_fp and
	 * prev_type are only meaningful to the next unwind_frame() invocation.
	 */
	frame->fp = READ_ONCE_NOCHECK(*(unsigned long *)(fp));
	frame->pc = READ_ONCE_NOCHECK(*(unsigned long *)(fp + 8));
	frame->prev_fp = fp;

	/*
	 * Frames created upon entry from EL0 have NULL FP and PC values, so
	 * don't bother reporting these. Frames created by __noreturn functions
	 * might have a valid FP even if PC is bogus, so only terminate where
	 * both are NULL.
	 */
	if (!frame->fp && !frame->pc)
		return -EINVAL;

	return 0;
}

void dbg_snapshot_dump_backtrace(struct pt_regs *regs,
				struct task_struct *tsk, void *data)
{
	struct stackframe frame;
	int skip = 0;
	unsigned long pc = 0;
	unsigned long lr = 0;

	if (regs) {
		if (user_mode(regs))
			return;
		skip = 1;
	} else {
		return;
	}

	if (!tsk)
		tsk = current;

	if (!try_get_task_stack(tsk))
		return;

	if (tsk == current) {
		start_backtrace(&frame,
				(unsigned long)__builtin_frame_address(0),
				(unsigned long)dbg_snapshot_dump_backtrace);
	} else {
		/*
		 * task blocked in __switch_to
		 */
		start_backtrace(&frame,
				thread_saved_fp(tsk),
				thread_saved_pc(tsk));
	}

	do {
		if (!skip) {
			pc = (unsigned long)frame.pc;
		} else if (frame.fp == regs->regs[29]) {
			skip = 0;
			pc = (unsigned long)regs->pc;
		}
	} while (!pc && !dbg_snapshot_unwind_frame(tsk, &frame));

	while (!lr && !dbg_snapshot_unwind_frame(tsk, &frame)) {
		if (!skip) {
			lr = (unsigned long)frame.pc;
		} else if (frame.fp == regs->regs[29]) {
			skip = 0;
			lr = (unsigned long)regs->pc;
		}
	}

	dbg_snapshot_backtrace_start(pc, lr, regs, data);
	dbg_snapshot_backtrace_log(pc);
	dbg_snapshot_backtrace_log(lr);

	while (!dbg_snapshot_unwind_frame(tsk, &frame)) {
		/* skip until specified stack frame */
		if (!skip) {
			dbg_snapshot_backtrace_log((unsigned long)frame.pc);
		} else if (frame.fp == regs->regs[29]) {
			skip = 0;
			/*
			 * Mostly, this is the case where this function is
			 * called in panic/abort. As exception handler's
			 * stack frame does not contain the corresponding pc
			 * at which an exception has taken place, use regs->pc
			 * instead.
			 */
			dbg_snapshot_backtrace_log((unsigned long)regs->pc);
		}
	}

	dbg_snapshot_backtrace_stop();
}

static void dbg_snapshot_backtrace(struct pt_regs *regs, void *data)
{
	int old_cpu, cpu;
	int i;
	void *where;

	if (!dss_backtrace || dss_backtrace->stop_logging)
		return;

	cpu = raw_smp_processor_id();
	old_cpu = atomic_cmpxchg(&backtrace_cpu, BACKTRACE_CPU_INVALID, cpu);

	if (old_cpu != BACKTRACE_CPU_INVALID)
		return;

	if (regs) {
		dbg_snapshot_dump_backtrace(regs, NULL, data);
	} else {
		/* skip call stack related with panic */
		dbg_snapshot_backtrace_start((unsigned long)return_address(2),
					(unsigned long)return_address(3),
					regs, data);
		i = 2;
		where = return_address(i++);

		while (where) {
			dbg_snapshot_backtrace_log((unsigned long)where);
			where = return_address(i++);
		}

		dbg_snapshot_backtrace_stop();
	}
}

static void dbg_snapshot_register_backtrace(void)
{
	struct dbg_snapshot_backtrace_data *data;
	void *vaddr;
	dma_addr_t daddr;
	size_t size = SZ_4K * 2;

	data = devm_kzalloc(dss_desc.dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return;

	dma_set_mask_and_coherent(dss_desc.dev, DMA_BIT_MASK(36));
	vaddr = dma_alloc_coherent(dss_desc.dev, size, &daddr, GFP_KERNEL);
	if (!vaddr) {
		devm_kfree(dss_desc.dev, data);
		return;
	}
	memset(vaddr, 0, size);

	data->paddr = (u64)daddr;
	data->vaddr = (char *)vaddr;
	data->size = size;
	data->curr_idx = 0;
	data->stop_logging = false;

	/* clear magic */
	dbg_snapshot_set_val_offset(0, DSS_OFFSET_BACKTRACE_MAGIC);

	dss_backtrace = data;
	dev_info(dss_desc.dev,
		"backtrace: alloc success paddr:0x%llx size:0x%zx\n",
		daddr, size);
}

static void dbg_snapshot_set_core_power_stat(unsigned val, unsigned cpu)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	if (header)
		__raw_writel(val, header + DSS_OFFSET_CORE_POWER_STAT + cpu * 4);
}

static unsigned dbg_snapshot_get_core_panic_stat(unsigned cpu)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	return header ?  __raw_readl(header + DSS_OFFSET_PANIC_STAT + cpu * 4) : 0;
}

static void dbg_snapshot_set_core_panic_stat(unsigned val, unsigned cpu)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	if (header)
		__raw_writel(val, header + DSS_OFFSET_PANIC_STAT + cpu * 4);
}

static void dbg_snapshot_report_reason(unsigned int val)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	if (header)
		__raw_writel(val, header + DSS_OFFSET_EMERGENCY_REASON);
}

static unsigned int dbg_snapshot_get_report_reason(void)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	return header ? __raw_readl(header + DSS_OFFSET_EMERGENCY_REASON) : 0;
}

static void dbg_snapshot_set_wdt_caller(unsigned long addr)
{
	void __iomem *header = dbg_snapshot_get_header_vaddr();

	if (header)
		__raw_writeq(addr, header + DSS_OFFSET_WDT_CALLER);
}

int dbg_snapshot_start_watchdog(int sec)
{
	if (dss_soc_ops.start_watchdog)
		return dss_soc_ops.start_watchdog(true, 0, sec);

	return -ENODEV;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_start_watchdog);

#ifdef CONFIG_S3C2410_WATCHDOG
int dbg_snapshot_expire_watchdog(void)
{
	unsigned long addr;

	if (!dss_soc_ops.expire_watchdog) {
		dev_emerg(dss_desc.dev, "There is no wdt functions!\n");
		return -ENODEV;
	}

	addr = (unsigned long)return_address(0);

	dbg_snapshot_set_wdt_caller(addr);
	dev_emerg(dss_desc.dev, "Caller: %pS, WDTRESET right now!\n", (void *)addr);
	if (dss_soc_ops.expire_watchdog(3, 0))
		return -ENODEV;
	dbg_snapshot_spin_func();

	return -ENODEV;
}
#else
int dbg_snapshot_expire_watchdog(void)
{
	unsigned long addr;

	addr = (unsigned long)return_address(0);

	dbg_snapshot_set_wdt_caller(addr);
	dev_emerg(dss_desc.dev, "Caller: %pS, WDTRESET right now!\n", (void *)addr);

	dbg_snapshot_spin_func();

	return -ENODEV;

}
#endif
EXPORT_SYMBOL_GPL(dbg_snapshot_expire_watchdog);

#ifdef CONFIG_S3C2410_WATCHDOG
int dbg_snapshot_expire_watchdog_timeout(int tick)
{
	unsigned long addr;

	if (!dss_soc_ops.expire_watchdog) {
		dev_emerg(dss_desc.dev, "There is no wdt functions!\n");
		return -ENODEV;
	}

	addr = (unsigned long)return_address(0);

	dbg_snapshot_set_wdt_caller(addr);
	dev_emerg(dss_desc.dev, "Caller: %pS, WDTRESET right now!\n", (void *)addr);
	if (dss_soc_ops.expire_watchdog(tick, 0))
		return -ENODEV;
	return 0;
}
#else
int dbg_snapshot_expire_watchdog_timeout(int tick)
{
	unsigned long addr;

	addr = (unsigned long)return_address(0);

	dbg_snapshot_set_wdt_caller(addr);
	dev_emerg(dss_desc.dev, "Caller: %pS, WDTRESET right now!\n", (void *)addr);

	return 0;
}
#endif
EXPORT_SYMBOL_GPL(dbg_snapshot_expire_watchdog_timeout);


int dbg_snapshot_kick_watchdog(void)
{
	if (dss_soc_ops.start_watchdog)
		return dss_soc_ops.start_watchdog(false, 0, 0);

	return -ENODEV;
}
EXPORT_SYMBOL_GPL(dbg_snapshot_kick_watchdog);

#define task_contributes_to_load(task)  ((READ_ONCE(task->__state) & TASK_UNINTERRUPTIBLE) != 0 && \
		(task->flags & PF_FROZEN) == 0 && \
		(READ_ONCE(task->__state) & TASK_NOLOAD) == 0)

static void dbg_snapshot_dump_one_task_info(struct task_struct *tsk, bool is_main)
{
	char state_array[] = {'R', 'S', 'D', 'T', 't', 'X',
			'Z', 'P', 'x', 'K', 'W', 'I', 'N'};
	unsigned char idx = 0;
	unsigned long state, pc = 0;

	if ((tsk == NULL) || !try_get_task_stack(tsk))
		return;

	state = READ_ONCE(tsk->__state) | tsk->exit_state;
	pc = KSTK_EIP(tsk);
	while (state) {
		idx++;
		state >>= 1;
	}

	/*
	 * kick watchdog to prevent unexpected reset during panic sequence
	 * and it prevents the hang during panic sequence by watchedog
	 */
	touch_softlockup_watchdog();

	atomic_notifier_call_chain(&dump_task_notifier_list, 0,	(void *)tsk);
	pr_info("%8d %16llu %16llu %16llu %c(%4d) %1d %16zx %16zx %c %16s\n",
		tsk->pid, tsk->utime, tsk->stime,
		tsk->se.exec_start, state_array[idx], READ_ONCE(tsk->__state),
		task_cpu(tsk), pc, (unsigned long)tsk,
		is_main ? '*' : ' ', tsk->comm);

	if (tsk->on_cpu && tsk->on_rq && tsk->cpu != smp_processor_id())
		return;

	if (READ_ONCE(tsk->__state) == TASK_RUNNING || READ_ONCE(tsk->__state) == TASK_WAKING ||
			task_contributes_to_load(tsk))
		dump_backtrace(NULL, tsk, KERN_DEFAULT);
}

static inline struct task_struct *get_next_thread(struct task_struct *tsk)
{
	return container_of(tsk->thread_group.next, struct task_struct, thread_group);
}

static void dbg_snapshot_dump_task_info(void)
{
	struct task_struct *frst_tsk, *curr_tsk;
	struct task_struct *frst_thr, *curr_thr;

	pr_info("\n");
	pr_info(" current proc : %d %s\n",
			current->pid, current->comm);
	pr_info("--------------------------------------------------------"
			"-------------------------------------------------\n");
	pr_info("%8s %16s %16s %16s %6s %3s %16s %16s  %16s\n",
			"pid", "uTime", "sTime", "exec(ns)", "stat", "cpu",
			"user_pc", "task_struct", "comm");
	pr_info("--------------------------------------------------------"
			"-------------------------------------------------\n");

	/* processes */
	frst_tsk = &init_task;
	curr_tsk = frst_tsk;
	while (curr_tsk) {
		dbg_snapshot_dump_one_task_info(curr_tsk,  true);
		/* threads */
		if (curr_tsk->thread_group.next != NULL) {
			frst_thr = get_next_thread(curr_tsk);
			curr_thr = frst_thr;
			if (frst_thr != curr_tsk) {
				while (curr_thr != NULL) {
					dbg_snapshot_dump_one_task_info(curr_thr, false);
					curr_thr = get_next_thread(curr_thr);
					if (curr_thr == curr_tsk)
						break;
				}
			}
		}
		curr_tsk = container_of(curr_tsk->tasks.next,
					struct task_struct, tasks);
		if (curr_tsk == frst_tsk)
			break;
	}
	pr_info("--------------------------------------------------------"
			"-------------------------------------------------\n");
}

static void dbg_snapshot_save_system(void *unused)
{
	struct dbg_snapshot_mmu_reg *mmu_reg;
	unsigned int cpu = raw_smp_processor_id();
	size_t vaddr;

	mmu_reg = *per_cpu_ptr(dss_mmu_reg, cpu);
	if (!mmu_reg) {
		vaddr = (size_t)dbg_snapshot_get_header_vaddr();
		*per_cpu_ptr(dss_mmu_reg, cpu) = (struct dbg_snapshot_mmu_reg *)
			(vaddr + dss_desc.mmu_regs_offset +
			 (cpu * DSS_MMU_REG_OFFSET));
		mmu_reg = *per_cpu_ptr(dss_mmu_reg, cpu);
	}

	asm volatile ("mrs x1, SCTLR_EL1\n\t"	/* SCTLR_EL1 */
		"mrs x2, TTBR0_EL1\n\t"		/* TTBR0_EL1 */
		"stp x1, x2, [%0]\n\t"
		"mrs x1, TTBR1_EL1\n\t"		/* TTBR1_EL1 */
		"mrs x2, TCR_EL1\n\t"		/* TCR_EL1 */
		"stp x1, x2, [%0, #0x10]\n\t"
		"mrs x1, ESR_EL1\n\t"		/* ESR_EL1 */
		"mrs x2, FAR_EL1\n\t"		/* FAR_EL1 */
		"stp x1, x2, [%0, #0x20]\n\t"
		"mrs x1, CONTEXTIDR_EL1\n\t"	/* CONTEXTIDR_EL1 */
		"mrs x2, TPIDR_EL0\n\t"		/* TPIDR_EL0 */
		"stp x1, x2, [%0, #0x30]\n\t"
		"mrs x1, TPIDRRO_EL0\n\t"	/* TPIDRRO_EL0 */
		"mrs x2, TPIDR_EL1\n\t"		/* TPIDR_EL1 */
		"stp x1, x2, [%0, #0x40]\n\t"
		"mrs x1, MAIR_EL1\n\t"		/* MAIR_EL1 */
		"mrs x2, ELR_EL1\n\t"		/* ELR_EL1 */
		"stp x1, x2, [%0, #0x50]\n\t"
		"mrs x1, SP_EL0\n\t"		/* SP_EL0 */
		"str x1, [%0, 0x60]\n\t" :	/* output */
		: "r"(mmu_reg)			/* input */
		: "%x1", "memory"		/* clobbered register */
	);
}

static void _dbg_snapshot_ecc_dump(void)
{
	ERRSELR_EL1_t errselr_el1;
	ERRIDR_EL1_t erridr_el1;
	ERXSTATUS_EL1_t erxstatus_el1;
	ERXADDR_EL1_t erxaddr_el1;
	int i;

	asm volatile ("HINT #16");
	erridr_el1.reg = read_ERRIDR_EL1();
	dev_emerg(dss_desc.dev,
			"ECC error check erridr_el1.NUM = [0x%llx]\n",
			(u64)erridr_el1.field.NUM);

	for (i = 0; i < (int)erridr_el1.field.NUM; i++) {
		errselr_el1.reg = read_ERRSELR_EL1();
		errselr_el1.field.SEL = i;
		write_ERRSELR_EL1(errselr_el1.reg);

		isb();

		erxstatus_el1.reg = read_ERXSTATUS_EL1();
		if (!erxstatus_el1.field.Valid) {
			dev_emerg(dss_desc.dev,
				"ERRSELR_EL1.SEL = %d, NOT Error, ERXSTATUS_EL1 = [0x%llx]\n",
				i, erxstatus_el1.reg);
			continue;
		}

		if (erxstatus_el1.field.AV) {
			erxaddr_el1.reg = read_ERXADDR_EL1();
			dev_emerg(dss_desc.dev,	"Error Address : [0x%llx]\n",
					erxaddr_el1.reg);
		}
		if (erxstatus_el1.field.OF)
			dev_emerg(dss_desc.dev,
				"There was more than one error has occurred."
				"the other error have been discarded.\n");
		if (erxstatus_el1.field.ER)
			dev_emerg(dss_desc.dev,
				"Error Reported by external abort\n");
		if (erxstatus_el1.field.UE)
			dev_emerg(dss_desc.dev,
				"Uncorrected Error (Not deferred)\n");
		if (erxstatus_el1.field.DE)
			dev_emerg(dss_desc.dev,	"Deferred Error\n");
		if (erxstatus_el1.field.MV) {
			dev_emerg(dss_desc.dev,
				"ERXMISC0_EL1 = [0x%llx] "
				"ERXMISC1_EL1 = [0x%llx] "
				"ERXSTATUS_EL1 = [0x%llx]\n",
				read_ERXMISC0_EL1(),
				read_ERXMISC1_EL1(),
				erxstatus_el1.reg);
		}
	}
}

void dbg_snapshot_ecc_dump(void)
{
	switch (read_cpuid_part_number()) {
	case ARM_CPU_PART_CORTEX_A55:
	case ARM_CPU_PART_CORTEX_A76:
	case ARM_CPU_PART_CORTEX_A77:
	case ARM_CPU_PART_CORTEX_A78AE:
	case ARM_CPU_PART_HERCULES:
	case ARM_CPU_PART_HERA:
		_dbg_snapshot_ecc_dump();
		break;
	default:
		break;
	}
}
EXPORT_SYMBOL_GPL(dbg_snapshot_ecc_dump);

static inline void dbg_snapshot_save_core(struct pt_regs *regs)
{
	unsigned int cpu = raw_smp_processor_id();
	struct pt_regs *core_reg = *per_cpu_ptr(dss_core_reg, cpu);
	size_t vaddr;

	if (!core_reg) {
		vaddr = (size_t)dbg_snapshot_get_header_vaddr();
		*per_cpu_ptr(dss_core_reg, cpu) = (struct pt_regs *)
			(vaddr + dss_desc.core_regs_offset +
			 (cpu * DSS_CORE_REG_OFFSET));
		core_reg = *per_cpu_ptr(dss_core_reg, cpu);
	}
	if (!regs) {
		asm volatile ("str x0, [%0, #0]\n\t"
				"mov x0, %0\n\t"
				"stp x1, x2, [x0, #0x8]\n\t"
				"stp x3, x4, [x0, #0x18]\n\t"
				"stp x5, x6, [x0, #0x28]\n\t"
				"stp x7, x8, [x0, #0x38]\n\t"
				"stp x9, x10, [x0, #0x48]\n\t"
				"stp x11, x12, [x0, #0x58]\n\t"
				"stp x13, x14, [x0, #0x68]\n\t"
				"stp x15, x16, [x0, #0x78]\n\t"
				"stp x17, x18, [x0, #0x88]\n\t"
				"stp x19, x20, [x0, #0x98]\n\t"
				"stp x21, x22, [x0, #0xa8]\n\t"
				"stp x23, x24, [x0, #0xb8]\n\t"
				"stp x25, x26, [x0, #0xc8]\n\t"
				"stp x27, x28, [x0, #0xd8]\n\t"
				"stp x29, x30, [x0, #0xe8]\n\t" :
				: "r"(core_reg));
		core_reg->sp = core_reg->regs[29];
		core_reg->pc =
			(unsigned long)(core_reg->regs[30] - sizeof(unsigned int));
	} else {
		memcpy(core_reg, regs, sizeof(struct user_pt_regs));
	}

	dev_emerg(dss_desc.dev, "core register saved(CPU:%d)\n", cpu);
}

static void dbg_snapshot_save_context(struct pt_regs *regs, bool stack_dump)
{
	int cpu = raw_smp_processor_id();
	unsigned long flags;

	if (!dbg_snapshot_get_enable())
		return;

	raw_spin_lock_irqsave(&dss_desc.ctrl_lock, flags);

	/* If it was already saved the context information, it should be skipped */
	if (dbg_snapshot_get_core_panic_stat(cpu) !=  DSS_SIGN_PANIC) {
		dbg_snapshot_set_core_panic_stat(DSS_SIGN_PANIC, cpu);
		dbg_snapshot_save_system(NULL);
		dbg_snapshot_save_core(regs);
		dbg_snapshot_ecc_dump();
		dev_emerg(dss_desc.dev, "context saved(CPU:%d)\n", cpu);
	} else
		dev_emerg(dss_desc.dev, "skip context saved(CPU:%d)\n", cpu);

	if (stack_dump)
		dump_stack();

	raw_spin_unlock_irqrestore(&dss_desc.ctrl_lock, flags);

	flush_cache_all();
}

static void dbg_snapshot_dump_panic(char *str)
{
	/*  This function is only one which runs in panic function */
	if (str)
		strncpy(dbg_snapshot_get_header_vaddr() + DSS_OFFSET_PANIC_STRING,
				str, DSS_PANIC_STRING_SIZE);
}

static int dbg_snapshot_pre_panic_handler(struct notifier_block *nb,
					  unsigned long l, void *buf)
{
	static int in_panic;
	static int cpu = PANIC_CPU_INVALID;

	if (in_panic++ && cpu == raw_smp_processor_id()) {
		dev_err(dss_desc.dev, "Possible infinite panic\n");
		dbg_snapshot_expire_watchdog();
	}

	cpu = raw_smp_processor_id();

	return 0;
}

static int __dbg_snapshot_dump_module_info(const char *mod_name,
		void *mod_addr, void *data)
{
	dev_info(dss_desc.dev, "%32s - 0x%px\n", mod_name, mod_addr);

	return 0;
}

static void dbg_snapshot_dump_module_info(void)
{
	dev_info(dss_desc.dev, "\t\tModule Name\t\tLoaded Address\n");
	dev_info(dss_desc.dev, "-------------------------------------------------------\n");
	android_debug_for_each_module(__dbg_snapshot_dump_module_info, NULL);
}

static int dbg_snapshot_post_panic_handler(struct notifier_block *nb,
					   unsigned long l, void *buf)
{
	unsigned long cpu;

	if (!dbg_snapshot_get_enable())
		return 0;

	/* Again disable log_kevents */
	dbg_snapshot_set_item_enable("log_kevents", false);
	dbg_snapshot_dump_panic(buf);
	dbg_snapshot_report_reason(DSS_SIGN_PANIC);
	for_each_possible_cpu(cpu) {
		if (cpu_is_offline(cpu))
			dbg_snapshot_set_core_power_stat(DSS_SIGN_DEAD, cpu);
		else
			dbg_snapshot_set_core_power_stat(DSS_SIGN_ALIVE, cpu);
	}

	dbg_snapshot_backtrace(NULL, buf);
	dbg_snapshot_dump_module_info();
	dbg_snapshot_dump_task_info();
	dbg_snapshot_output();
	dbg_snapshot_log_output();
	dbg_snapshot_print_log_report();
	dbg_snapshot_save_context(NULL, false);

	return 0;
}

static int dbg_snapshot_end_panic_handler(struct notifier_block *nb,
					   unsigned long l, void *buf)
{
	if (dss_desc.panic_to_wdt || (num_online_cpus() > 1))
		dbg_snapshot_expire_watchdog();
	pr_alert("DSS dump completed\n");
	return 0;
}

static void show_data(unsigned long addr, int nbytes, const char *name)
{
	int i, j, nlines;
	u32 *p, data;

	/*
	 * don't attempt to dump non-kernel addresses or
	 * values that are probably just small negative numbers
	 */
	if (addr < PAGE_OFFSET || addr > -256UL)
		return;

	if (!strncmp("PC", name, strlen(name)) || !strncmp("LR", name, strlen(name)))
		pr_info("\n%s: \n", name);
	else
		pr_info("\n%s: %#lx:\n", name, addr);
	p = (u32 *)(addr & ~(sizeof(u32) - 1));
	nbytes += (addr & (sizeof(u32) - 1));
	nlines = (nbytes + 31) / 32;

	for (i = 0; i < nlines; i++) {
		pr_cont("%04lx :", (unsigned long)p & 0xffff);
		for (j = 0; j < 8; j++, p++) {
			if (get_kernel_nofault(data, p))
				pr_cont(" ********");
			else
				pr_cont(" %08X", data);
		}
		pr_cont("\n");
	}
}

static void show_extra_register_data(struct pt_regs *regs, int nbytes)
{
	unsigned int i;
	unsigned long flags;
	int ret = 0;
	raw_spin_lock_irqsave(&dss_desc.ctrl_lock, flags);

	show_data(regs->pc - nbytes, nbytes * 2, "PC");
	show_data(regs->regs[30] - nbytes, nbytes * 2, "LR");
	show_data(regs->sp - nbytes, nbytes * 2, "SP");
	for (i = 0; i < 30; i++) {
		char name[4];

		ret = snprintf(name, sizeof(name), "X%u", i);
		if (ret < 0)
			pr_err("snprintf is failed\n");
		show_data(regs->regs[i] - nbytes, nbytes * 2, name);
	}

	raw_spin_unlock_irqrestore(&dss_desc.ctrl_lock, flags);
}

static void dbg_snapshot_kmodule_clear_area(void)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item(DSS_ITEM_KMODULE);

	if (item && item->head_ptr) {
		memset(item->head_ptr, 0, item->entry.size);
		item->curr_ptr = item->head_ptr;
	}
}

static void dbg_snapshot_kmodule_logger(char *str)
{
	struct dbg_snapshot_item *item = dbg_snapshot_get_item(DSS_ITEM_KMODULE);
	size_t size;

	if (item && item->head_ptr && item->curr_ptr) {
		size = item->entry.size - (size_t)(item->curr_ptr - item->head_ptr);
		item->curr_ptr += snprintf(item->curr_ptr, size, "%s", str);
		if (item->curr_ptr - item->head_ptr >= item->entry.size)
			item->curr_ptr = item->head_ptr;
	}
}

static int __dbg_snapshot_save_kmodule_info(const char *mod_name,
		void *mod_addr, void *data)
{
	char str[DSS_KMOD_STR_SZ];

	if (snprintf(str, DSS_KMOD_STR_SZ, "%32s - 0x%px\n", mod_name, mod_addr) < 0) {
		pr_err("snprintf is failed\n");
		return -EINVAL;
	}
	dbg_snapshot_kmodule_logger(str);

	return 0;
}

static void dbg_snapshot_save_kmodule_info(void)
{
	if (!dbg_snapshot_get_item_enable(DSS_ITEM_KMODULE))
		return;

	android_debug_for_each_module(__dbg_snapshot_save_kmodule_info, NULL);
}

static int dbg_snapshot_module_handler(struct notifier_block *nb, unsigned long op,
	void *data)
{
	struct module *mod = data;
	const char *op_str;
	char str[DSS_KMOD_STR_SZ];

	if (!data) {
		dev_warn(dss_desc.dev, "%s: data is NULL", __func__);
		goto out;
	}

	op_str = (op == MODULE_STATE_LIVE) ? "Live" :
		(op == MODULE_STATE_COMING) ? "Coming" :
		(op == MODULE_STATE_GOING) ? "Going" : "Unformed";

	dev_info(dss_desc.dev, "Module %s, %s - 0x%px\n",
			mod->name, op_str, mod->core_layout.base);

	if (op == MODULE_STATE_COMING) {
		if (snprintf(str, DSS_KMOD_STR_SZ, "%32s - 0x%px\n",
				mod->name, mod->core_layout.base) < 0) {
			pr_err("snprintf is failed\n");
			goto out;
		}

		dbg_snapshot_kmodule_logger(str);
	} else if (op == MODULE_STATE_GOING) {
		dbg_snapshot_kmodule_clear_area();
		dbg_snapshot_save_kmodule_info();
	}
out:
	return NOTIFY_DONE;
}

static int dbg_snapshot_die_handler(struct notifier_block *nb,
				   unsigned long l, void *buf)
{
	struct die_args *args = (struct die_args *)buf;
	struct pt_regs *regs = args->regs;

	if (user_mode(regs))
		return NOTIFY_DONE;

	dbg_snapshot_backtrace(regs, buf);
	dbg_snapshot_save_context(regs, false);
	dbg_snapshot_set_item_enable("log_kevents", false);
	show_extra_register_data(regs, 128);

	return NOTIFY_DONE;
}

static int dbg_snapshot_restart_handler(struct notifier_block *nb,
				    unsigned long mode, void *cmd)
{
	int cpu;

	if (!dbg_snapshot_get_enable())
		return NOTIFY_DONE;

	if (dbg_snapshot_get_report_reason() == DSS_SIGN_PANIC)
		return NOTIFY_DONE;

	dev_emerg(dss_desc.dev, "normal reboot starting\n");
	dbg_snapshot_report_reason(DSS_SIGN_NORMAL_REBOOT);
	dbg_snapshot_scratch_clear();
	dev_emerg(dss_desc.dev, "normal reboot done\n");

	/* clear DSS_SIGN_PANIC when normal reboot */
	for_each_possible_cpu(cpu) {
		dbg_snapshot_set_core_panic_stat(DSS_SIGN_RESET, cpu);
	}

	flush_cache_all();

	return NOTIFY_DONE;
}

static struct notifier_block nb_restart_block = {
	.notifier_call = dbg_snapshot_restart_handler,
	.priority = INT_MAX,
};

static struct notifier_block nb_pre_panic_block = {
	.notifier_call = dbg_snapshot_pre_panic_handler,
	.priority = INT_MAX,
};

static struct notifier_block nb_post_panic_block = {
	.notifier_call = dbg_snapshot_post_panic_handler,
	.priority = (INT_MAX - 2),
};

static struct notifier_block nb_end_panic_block = {
	.notifier_call = dbg_snapshot_end_panic_handler,
	.priority = INT_MIN,
};

static struct notifier_block nb_die_block = {
	.notifier_call = dbg_snapshot_die_handler,
	.priority = INT_MAX,
};

static struct notifier_block nb_module_block = {
	.notifier_call = dbg_snapshot_module_handler,
};

void dbg_snapshot_do_dpm_policy(unsigned int policy)
{
	switch (policy) {
	case GO_DEFAULT_ID:
		break;
	case GO_PANIC_ID:
		panic("%pS", return_address(0));
		break;
	case GO_WATCHDOG_ID:
	case GO_S2D_ID:
		if (dbg_snapshot_expire_watchdog())
			panic("WDT rst fail for s2d, wdt device not probed");
		dbg_snapshot_spin_func();
		break;
	case GO_ARRAYDUMP_ID:
		if (dss_soc_ops.run_arraydump)
			dss_soc_ops.run_arraydump();
		break;
	case GO_SCANDUMP_ID:
		if (dss_soc_ops.run_scandump_mode)
			dss_soc_ops.run_scandump_mode();
		break;
	}
}
EXPORT_SYMBOL_GPL(dbg_snapshot_do_dpm_policy);

void dbg_snapshot_register_wdt_ops(void *start, void *expire, void *stop)
{
	if (start)
		dss_soc_ops.start_watchdog = start;
	if (expire)
		dss_soc_ops.expire_watchdog = expire;
	if (stop)
		dss_soc_ops.stop_watchdog = stop;

	dev_info(dss_desc.dev, "Add %s%s%sfuntions from %pS\n",
			start ? "(wdt start) " : "",
			expire ? "(wdt expire), " : "",
			stop ? "(wdt stop) " : "",
			return_address(0));
}
EXPORT_SYMBOL_GPL(dbg_snapshot_register_wdt_ops);

void dbg_snapshot_register_debug_ops(void *halt, void *arraydump,
				    void *scandump)
{
	if (halt)
		dss_soc_ops.stop_all_cpus = halt;
	if (arraydump)
		dss_soc_ops.run_arraydump = arraydump;
	if (scandump)
		dss_soc_ops.run_scandump_mode = scandump;

	dev_info(dss_desc.dev, "Add %s%s%sfuntions from %pS\n",
			halt ? "(halt) " : "",
			arraydump ? "(arraydump) " : "",
			scandump ? "(scandump mode) " : "",
			return_address(0));
}
EXPORT_SYMBOL_GPL(dbg_snapshot_register_debug_ops);

#if IS_ENABLED(CONFIG_ANDROID_VENDOR_HOOKS)
static void dbg_snapshot_ipi_stop(void *ignore, struct pt_regs *regs)
{
	dbg_snapshot_save_context(regs, oops_in_progress > 0);
}
#endif

static inline bool is_event_supported(unsigned int type, unsigned int code)
{
	if (!(dss_desc.hold_key && dss_desc.trigger_key))
		return false;

	return (type == EV_KEY) && (code == dss_desc.hold_key ||
			code == dss_desc.trigger_key);
}

static void dbg_snanpshot_event(struct input_handle *handle, unsigned int type,
		unsigned int code, int value)
{
	static bool holdkey_p;
	static int count;
	static ktime_t start;

	if (!is_event_supported(type, code))
		return;

	dev_info(dss_desc.dev, "KEY(%d) %s\n",
			code, value ? "pressed" : "released");
	/* Enter Forced Upload. Hold key first
	 * and then press trigger key twice. Other key should not be pressed.
	 */
	if (code == dss_desc.hold_key)
		holdkey_p = value ? true : false;

	if (!holdkey_p) {
		count = 0;
		holdkey_p = false;
		return;
	}

	if ((code != dss_desc.trigger_key) || !value)
		return;

	if (!count)
		start = ktime_get();
	dev_err(dss_desc.dev, "entering forced upload[%d]\n", ++count);
	if (ktime_ms_delta(ktime_get(), start) > 2 * MSEC_PER_SEC)
		count = 0;

	if (count == 2)
		panic("Crash Key");
}

static int dbg_snanpshot_connect(struct input_handler *handler,
				 struct input_dev *dev,
				 const struct input_device_id *id)
{
	struct input_handle *handle;
	int error;

	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "dss_input_handler";

	error = input_register_handle(handle);
	if (error)
		goto err_free_handle;

	error = input_open_device(handle);
	if (error)
		goto err_unregister_handle;

	return 0;

err_unregister_handle:
	input_unregister_handle(handle);
err_free_handle:
	kfree(handle);
	return error;
}

static void dbg_snanpshot_disconnect(struct input_handle *handle)
{
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

static const struct input_device_id dbg_snanpshot_ids[] = {
	{
		.flags = INPUT_DEVICE_ID_MATCH_KEYBIT,
		.evbit = { BIT_MASK(EV_KEY) },
	},
	{},
};

static struct input_handler dbg_snapshot_input_handler = {
	.event		= dbg_snanpshot_event,
	.connect	= dbg_snanpshot_connect,
	.disconnect	= dbg_snanpshot_disconnect,
	.name		= "dss_input_handler",
	.id_table	= dbg_snanpshot_ids,
};

static int __dbg_snapshot_online_cpu(void *unused)
{
#if IS_ENABLED(CONFIG_VLM)
	int vcpu = raw_smp_processor_id();
	int *map_table = dss_smp_processor_id_to_pcpu;

	dbg_snapshot_map_smp_processor_id_to_pcpu(map_table);

	pr_info("smp_processor_id[%d] : pcpu %d\n", vcpu, map_table[vcpu]);
#endif
	dbg_snapshot_save_system(NULL);

	return 0;
}

static int __dbg_snapshot_offline_cpu(void *unused)
{
#if IS_ENABLED(CONFIG_VLM)
	int vcpu = raw_smp_processor_id();

	dss_smp_processor_id_to_pcpu[vcpu] = VCPU_TO_PCPU_INVALID;
#endif
	return 0;
}

static int dbg_snapshot_online_cpu(unsigned int cpu)
{
	if (cpu == raw_smp_processor_id())
		return __dbg_snapshot_online_cpu(NULL);
	else if (!in_interrupt())
		smp_call_on_cpu(cpu, __dbg_snapshot_online_cpu, NULL, false);

	return 0;
}

static int dbg_snapshot_offline_cpu(unsigned int cpu)
{
	if (cpu == raw_smp_processor_id())
		return __dbg_snapshot_offline_cpu(NULL);
	else if (!in_interrupt())
		smp_call_on_cpu(cpu, __dbg_snapshot_offline_cpu, NULL, false);

	return 0;
}

void dbg_snapshot_init_utils(void)
{
	int i;
	size_t vaddr = (size_t)dbg_snapshot_get_header_vaddr();
	int ret;

#if IS_ENABLED(CONFIG_VLM)
	dbg_snapshot_smp_processor_id_to_pcpu_init();
#endif
	dss_mmu_reg = alloc_percpu(struct dbg_snapshot_mmu_reg *);
	dss_core_reg = alloc_percpu(struct pt_regs *);
	for_each_possible_cpu(i) {
		if (i >= DSS_NR_CPUS)
			break;

		*per_cpu_ptr(dss_mmu_reg, i) = (struct dbg_snapshot_mmu_reg *)
				(vaddr + dss_desc.mmu_regs_offset +
				 i * DSS_MMU_REG_OFFSET);
		*per_cpu_ptr(dss_core_reg, i) = (struct pt_regs *)
				(vaddr + dss_desc.core_regs_offset +
				 i * DSS_CORE_REG_OFFSET);
	}

	dbg_snapshot_register_backtrace();
	register_module_notifier(&nb_module_block);
	register_die_notifier(&nb_die_block);
	register_restart_handler(&nb_restart_block);
	atomic_notifier_chain_register(&panic_notifier_list, &nb_pre_panic_block);
	atomic_notifier_chain_register(&panic_notifier_list, &nb_post_panic_block);
	atomic_notifier_chain_register(&panic_notifier_list, &nb_end_panic_block);
#if IS_ENABLED(CONFIG_ANDROID_VENDOR_HOOKS)
	register_trace_android_vh_ipi_stop(dbg_snapshot_ipi_stop, NULL);
#endif
	ret = input_register_handler(&dbg_snapshot_input_handler);
	if (ret < 0)
		pr_err("failed to input_register_handler %d", ret);

	ret = cpuhp_setup_state(CPUHP_AP_ONLINE_DYN,
			"dbg_snapshot:online",
			dbg_snapshot_online_cpu,
			dbg_snapshot_offline_cpu);

	if (ret < 0) {
		pr_err("%s: cpuhotplug register is failed - ret:%d\n",
				__func__, ret);
	}

	dbg_snapshot_save_kmodule_info();
}
