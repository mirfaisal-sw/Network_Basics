// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * Exynos - Early Hard Lockup Detector
 *
 * Author: Hosung Kim <hosung0.kim@samsung.com>
 *
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <linux/errno.h>
#include <linux/suspend.h>
#include <linux/perf_event.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/cpu_pm.h>
#include <linux/sched/clock.h>
#include <linux/notifier.h>
#include <linux/kallsyms.h>
#include <linux/panic_notifier.h>

#include <soc/samsung/exynos-ehld.h>
#include <soc/samsung/debug-snapshot.h>

#include "debug-snapshot-local.h"
#include "core_regs.h"

//#define DEBUG

#ifdef DEBUG
#define ehld_info(f, str...) pr_info(str)
#define ehld_err(f, str...) pr_info(str)
#else
#define ehld_info(f, str...) do { if (f == 1) pr_info(str); } while (0)
#define ehld_err(f, str...)  do { if (f == 1) pr_info(str); } while (0)
#endif

#define PMPCSR_NS_MASK		BIT(63)
#define PMPCSR_EL_MASK		GENMASK(62, 61)

#define MSB_MASKING		(0x0000FF0000000000)
#define MSB_PADDING		(0xFFFFFF0000000000)
#define DBG_UNLOCK(base)	\
	do { isb(); __raw_writel(OSLOCK_MAGIC, base + DBGLAR); } while (0)
#define DBG_LOCK(base)		\
	do { __raw_writel(0x1, base + DBGLAR); isb(); } while (0)
#define DBG_OS_UNLOCK(base)	\
	do { isb(); __raw_writel(0, base + DBGOSLAR); } while (0)
#define DBG_OS_LOCK(base)	\
	do { __raw_writel(0x1, base + DBGOSLAR); isb(); } while (0)

extern struct atomic_notifier_head hardlockup_notifier_list;
extern struct atomic_notifier_head hardlockup_handler_notifier_list;

#define EHLD_MAX_CORE_CNT		(CONFIG_DEBUG_SNAPSHOT_LOG_NR_CPUS)

struct ehld_dev {
	struct device			*dev;
	unsigned int			cs_base;
	struct cpumask			cpu_mask;
	int				sjtag_en;
};

struct ehld_data {
	unsigned long long		time[NUM_TRACE];
	unsigned long long		event[NUM_TRACE];
	unsigned long long		pmpcsr[NUM_TRACE];
	int				ns[NUM_TRACE];
	int				el[NUM_TRACE];
	int				vmid[NUM_TRACE];
	unsigned long			data_ptr;
};

struct ehld_ctrl {
	struct perf_event		*event;
	struct ehld_data		data;
	void __iomem			*dbg_base;
	raw_spinlock_t			lock;
	bool				in_c2;
	bool				valid;
};

static struct ehld_ctrl g_ehld_ctrl[EHLD_MAX_CORE_CNT];
static struct ehld_dev *ehld;

static struct perf_event_attr exynos_ehld_attr = {
	.type           = PERF_TYPE_HARDWARE,
	.config         = PERF_COUNT_HW_INSTRUCTIONS,
	.size           = sizeof(struct perf_event_attr),
	.pinned         = 1,
	.disabled       = 1,
};

static void ehld_event_update(int vcpu)
{
	struct ehld_ctrl *ctrl;
	struct ehld_data *data;
	unsigned long long val;
	unsigned long flags, count;
	int pcpu;
	int ns, el, vmid;

	pcpu = dbg_snapshot_get_pcpu_id(vcpu);
	if (pcpu < 0 || pcpu >= EHLD_MAX_CORE_CNT)
		return;

	ctrl = &g_ehld_ctrl[pcpu];
	if (!ctrl->valid)
		return;

	raw_spin_lock_irqsave(&ctrl->lock, flags);
	data = &ctrl->data;
	count = ++data->data_ptr & (NUM_TRACE - 1);
	data->time[count] = cpu_clock(vcpu);

	if (cpu_is_offline(vcpu) || ctrl->in_c2 || ehld->sjtag_en) {
		data->pmpcsr[count] = 0xC2;
	} else {
		DBG_UNLOCK(ctrl->dbg_base + PMU_OFFSET);
		val = __raw_readq(ctrl->dbg_base + PMU_OFFSET + PMUPCSR);
		val = __raw_readq(ctrl->dbg_base + PMU_OFFSET + PMUPCSR);
		ns = FIELD_GET(PMPCSR_NS_MASK, val);
		el = FIELD_GET(PMPCSR_EL_MASK, val);
		if (MSB_MASKING == (MSB_MASKING & val))
			val |= MSB_PADDING;
		else
			val &= ~(PMPCSR_NS_MASK | PMPCSR_EL_MASK);

		vmid = __raw_readl(ctrl->dbg_base + PMU_OFFSET + PMUVIDSR) & 0xFF;

		data->pmpcsr[count] = val;
		data->ns[count] = ns;
		data->el[count] = el;
		data->vmid[count] = vmid;

		DBG_LOCK(ctrl->dbg_base + PMU_OFFSET);
	}

	ehld_info(0, "%s: vcpu%d: time:%llu, event:0x%llx, ns:%d, el:%d, vmid:%d, pmpcsr:%llx, c2:%d\n",
			__func__, vcpu,
			data->time[count],
			data->event[count],
			data->ns[count],
			data->el[count],
			data->vmid[count],
			data->pmpcsr[count],
			ctrl->in_c2);

	raw_spin_unlock_irqrestore(&ctrl->lock, flags);
}

void ehld_event_update_allcpu(void)
{
	int vcpu;

	for_each_cpu(vcpu, &ehld->cpu_mask)
		ehld_event_update(vcpu);
}
EXPORT_SYMBOL(ehld_event_update_allcpu);

static void ehld_event_dump(int vcpu, bool header)
{
	struct ehld_ctrl *ctrl;
	struct ehld_data *data;
	unsigned long flags, count;
	int i;
	char symname[KSYM_NAME_LEN];
	int pcpu;

	pcpu = dbg_snapshot_get_pcpu_id(vcpu);
	if (pcpu < 0 || pcpu >= EHLD_MAX_CORE_CNT)
		return;

	if (header) {
		ehld_info(1, "--------------------------------------------------------------------------\n");
		ehld_info(1, "  Exynos Early Lockup Detector Information(SYS OS)\n\n");
		ehld_info(1, "  vCPU pCPU NUM TIME             VALUE             NS EL VMID       PC\n\n");
	}

	symname[KSYM_NAME_LEN - 1] = '\0';
	ctrl = &g_ehld_ctrl[pcpu];
	if (!ctrl->valid)
		return;

	raw_spin_lock_irqsave(&ctrl->lock, flags);
	data = &ctrl->data;
	i = 0;
	do {
		count = ++data->data_ptr & (NUM_TRACE - 1);
		symname[KSYM_NAME_LEN - 1] = '\0';
		i++;

		if (sprint_symbol(symname, data->pmpcsr[count]) < 0)
			symname[0] = '\0';

		ehld_info(1, "  %4d %4d %3d %015llu  0x%015llx %2d %2d %4d 0x%016llx(%s)\n",
				vcpu, pcpu, i,
				(unsigned long long)data->time[count],
				(unsigned long long)data->event[count],
				data->ns[count],
				data->el[count],
				data->vmid[count],
				(unsigned long long)data->pmpcsr[count],
				symname);

		if (i >= NUM_TRACE)
			break;
	} while (1);
	raw_spin_unlock_irqrestore(&ctrl->lock, flags);
	ehld_info(1, "--------------------------------------------------------------------------\n");
}

void ehld_event_dump_allcpu(void)
{
	int vcpu;

	ehld_info(1, "--------------------------------------------------------------------------\n");
	ehld_info(1, "  Exynos Early Lockup Detector Information(SYS OS)\n\n");
	ehld_info(1, "  vCPU pCPU NUM TIME             VALUE             NS EL VMID       PC\n\n");

	for_each_cpu(vcpu, &ehld->cpu_mask)
		ehld_event_dump(vcpu, false);
}

void ehld_prepare_panic(void)
{
	;
}

void ehld_do_action(int cpu, unsigned int lockup_level)
{
	switch (lockup_level) {
	case EHLD_STAT_LOCKUP_WARN:
		ehld_event_dump(cpu, true);
		break;
	case EHLD_STAT_LOCKUP_SW:
	case EHLD_STAT_LOCKUP_HW:
		ehld_event_dump(cpu, true);
		panic("Watchdog detected hard LOCKUP on cpu %u by EHLD", cpu);
		break;
	}
}
EXPORT_SYMBOL(ehld_do_action);

static int ehld_hardlockup_handler(struct notifier_block *nb,
					   unsigned long l, void *p)
{
	ehld_event_dump_allcpu();

	return 0;
}

static struct notifier_block ehld_hardlockup_block = {
	.notifier_call = ehld_hardlockup_handler,
};

/* This callback is called when every 4s in each cpu */
static int ehld_hardlockup_callback_handler(struct notifier_block *nb,
						unsigned long l, void *p)
{
	int *cpu = (int *)p;

	ehld_info(0, "%s: cpu%d\n", __func__, *cpu);

	ehld_event_update_allcpu();

	return 0;
}

static struct notifier_block ehld_hardlockup_handler_block = {
		.notifier_call = ehld_hardlockup_callback_handler,
};

static int ehld_panic_handler(struct notifier_block *nb,
				unsigned long l, void *p)
{
	ehld_event_dump_allcpu();

	return 0;
}

static struct notifier_block ehld_panic_block = {
	.notifier_call = ehld_panic_handler,
};

static int ehld_c2_pm_notifier(struct notifier_block *self,
				unsigned long action, void *v)
{
	int vcpu, pcpu;
	struct ehld_ctrl *ctrl;

	vcpu = raw_smp_processor_id();
	pcpu = dbg_snapshot_get_pcpu_id(vcpu);
	if (pcpu < 0 || pcpu >= EHLD_MAX_CORE_CNT)
		return -EPERM;

	ctrl = &g_ehld_ctrl[pcpu];
	if (!ctrl->valid)
		return -EPERM;

	switch (action) {
	case CPU_PM_ENTER:
	case CPU_CLUSTER_PM_ENTER:
		ctrl->in_c2 = true;
		break;
	case CPU_PM_ENTER_FAILED:
	case CPU_PM_EXIT:
	case CPU_CLUSTER_PM_ENTER_FAILED:
	case CPU_CLUSTER_PM_EXIT:
		ctrl->in_c2 = false;
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block ehld_c2_pm_nb = {
	.notifier_call = ehld_c2_pm_notifier,
};

static void ehld_setup(void)
{
	u64 period;

	/* clock cycles per tick: */
	period = (u64)(1000000) * 1000;
	do_div(period, HZ);
	exynos_ehld_attr.sample_period = period;

	/* register cpu pm notifier for C2 */
	cpu_pm_register_notifier(&ehld_c2_pm_nb);

	/* panic_notifier_list */
	atomic_notifier_chain_register(&panic_notifier_list,
					&ehld_panic_block);
	/* hardlockup_notifier_list */
	atomic_notifier_chain_register(&hardlockup_notifier_list,
					&ehld_hardlockup_block);
	/* hardlockup_handler_notifier_list */
	atomic_notifier_chain_register(&hardlockup_handler_notifier_list,
					&ehld_hardlockup_handler_block);

	ehld->sjtag_en = dbg_snapshot_get_sjtag_status();
}

static int ehld_init_dt_parse(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device_node *child;
	int ret = 0, pcpu = 0;
	unsigned int offset, base;
	char name[SZ_16];
	struct ehld_ctrl *ctrl;

	if (of_property_read_u32(np, "cs_base", &base)) {
		ehld_info(1, "ehld: no coresight base address\n");
		return -EINVAL;
	}
	ehld->cs_base = base;

	for (pcpu = 0; pcpu < EHLD_MAX_CORE_CNT; pcpu++) {
		if (snprintf(name, sizeof(name), "cpu%d", pcpu) < 0) {
			ehld_info(1, "snprintf failed - pcpu%d\n", pcpu);
			continue;
		}
		child = of_get_child_by_name(np, (const char *)name);

		if (!child) {
			ehld_info(1, "ehld: no cpu dbg info - pcpu%d\n", pcpu);
			continue;
		}

		ret = of_property_read_u32(child, "dbg-offset", &offset);
		if (ret) {
			ehld_err(1, "ehld: no cpu dbg-offset info - pcpu%d\n",
					pcpu);
			return -EINVAL;
		}

		ctrl = &g_ehld_ctrl[pcpu];
		ctrl->dbg_base = devm_ioremap(&pdev->dev, ehld->cs_base + offset, SZ_256K);

		if (!ctrl->dbg_base) {
			ehld_err(1, "ehld: fail ioremap for dbg_base of pcpu%d\n",
					pcpu);
			return -ENOMEM;
		}

		memset((void *)&ctrl->data, 0, sizeof(struct ehld_data));
		raw_spin_lock_init(&ctrl->lock);

		ehld_info(1, "ehld: pcpu#%d, dbg_base:0x%x, total:0x%x, ioremap:0x%lx\n",
				pcpu, offset, ehld->cs_base + offset,
				(unsigned long)ctrl->dbg_base);

		of_node_put(child);

		ctrl->valid = true;
	}

	return 0;
}

static int ehld_probe(struct platform_device *pdev)
{
	int err;
	struct ehld_dev *edev;

	edev = devm_kzalloc(&pdev->dev,
				sizeof(struct ehld_dev), GFP_KERNEL);
	if (!edev)
		return -ENOMEM;

	edev->dev = &pdev->dev;

	platform_set_drvdata(pdev, edev);

	ehld = edev;

	err = ehld_init_dt_parse(pdev);
	if (err) {
		ehld_err(1, "ehld: fail setup to parse dt:%d\n", err);
		return err;
	}

	ehld_setup();

	cpumask_copy(&ehld->cpu_mask, cpu_possible_mask);

	ehld_info(1, "ehld: success to initialize\n");
	return 0;
}

static int ehld_remove(struct platform_device *pdev)
{
	platform_set_drvdata(pdev, NULL);
	return 0;
}

static const struct of_device_id ehld_dt_match[] = {
	{ .compatible	= "samsung,exynos-ehld", },
	{},
};
MODULE_DEVICE_TABLE(of, ehld_dt_match);

static struct platform_driver ehld_driver = {
	.probe = ehld_probe,
	.remove = ehld_remove,
	.driver = {
		.name = "ehld",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ehld_dt_match),
	},
};
module_platform_driver(ehld_driver);

MODULE_DESCRIPTION("Samsung Early Hard Lockup Detector");
MODULE_LICENSE("GPL v2");
