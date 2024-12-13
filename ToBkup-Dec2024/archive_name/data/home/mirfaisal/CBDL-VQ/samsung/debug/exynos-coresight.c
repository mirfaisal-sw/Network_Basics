// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * EXYNOS - Coresight support
 */

#include <linux/cpu.h>
#include <linux/cpu_pm.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/panic_notifier.h>
#include <asm/cputype.h>
#include <asm/smp_plat.h>

#include <soc/samsung/debug-snapshot.h>
#include <soc/samsung/exynos-pmu.h>
#include "debug-snapshot-local.h"
#include "core_regs.h"

#ifdef CONFIG_VLM
#include <nk/nkern.h>
#endif

#define SYS_READ(reg, val)	do { asm volatile("mrs %0, " #reg : "=r" (val)); } while (0)
#define SYS_WRITE(reg, val)	do { asm volatile("msr " #reg ", %0" :: "r" (val)); } while (0)

#define DBG_UNLOCK(base)	\
	do { isb(); __raw_writel(OSLOCK_MAGIC, base + DBGLAR); } while (0)
#define DBG_LOCK(base)		\
	do { __raw_writel(0x1, base + DBGLAR); isb(); } while (0)

#define DBG_REG_MAX_SIZE	(6)
#define DBG_BW_REG_MAX_SIZE	(24)
#define OS_LOCK_FLAG		(DBG_REG_MAX_SIZE - 1)
#define ITERATION		5
#define CS_MAX_CORE_CNT		(CONFIG_DEBUG_SNAPSHOT_LOG_NR_CPUS)
#define MSB_PADDING		(0xFFFFFF0000000000)
#define MSB_MASKING		(0x0000FF0000000000)

struct exynos_coresight_info {
	struct device *dev;
	void __iomem *dbg_base[CS_MAX_CORE_CNT];
	void __iomem *cti_base[CS_MAX_CORE_CNT];
	void __iomem *pmu_base[CS_MAX_CORE_CNT];
	int dbg_base_cnt;
	int cti_base_cnt;
	int pmu_base_cnt;
	int core_cnt;
	bool halt_enabled;
	void __iomem *gpr_base;
	unsigned int dbgack_mask;
	bool retention_enabled;
	unsigned long dbg_reg[CS_MAX_CORE_CNT][DBG_REG_MAX_SIZE];
	unsigned long bw_reg[DBG_BW_REG_MAX_SIZE];
};

static struct exynos_coresight_info *ecs_info;

#if IS_ENABLED(CONFIG_EXYNOS_HARDLOCKUP_WATCHDOG)
extern struct atomic_notifier_head hardlockup_notifier_list;
#endif

static inline void dbg_os_lock(void __iomem *base)
{
	__raw_writel(0x1, base + DBGOSLAR);
	isb();
}

static inline void dbg_os_unlock(void __iomem *base)
{
	isb();
	__raw_writel(0x0, base + DBGOSLAR);
}

static inline bool is_power_up(unsigned int pcpu)
{
	return __raw_readl(ecs_info->dbg_base[pcpu] + DBGPRSR) & POWER_UP;
}

static inline bool is_reset_state(unsigned int pcpu)
{
	return __raw_readl(ecs_info->dbg_base[pcpu] + DBGPRSR) & RESET_STATE;
}

struct exynos_cs_pmu_snapshot_t {
	unsigned long pc;
	int ns;
	int el;
	int vmid;
};
static struct exynos_cs_pmu_snapshot_t exynos_cs_pmu_snapshots[CS_MAX_CORE_CNT][ITERATION];

static inline int exynos_cs_get_cpu_part_num(unsigned int pcpu)
{
	u32 midr = __raw_readl(ecs_info->dbg_base[pcpu] + MIDR);

	return MIDR_PARTNUM(midr);
}

static inline bool have_pc_offset(void __iomem *base)
{
	return !(__raw_readl(base + DBGDEVID1) & 0xf);
}

static int exynos_cs_get_pmu_snapshot(unsigned int pcpu, unsigned int iter)
{
	void __iomem *dbg_base, *pmu_base;
	unsigned long val = 0, valHi;
	int ns = -1, el = -1;
	int vmid = 0;

	if (!ecs_info)
		return -ENODEV;

	dbg_base = ecs_info->dbg_base[pcpu];
	pmu_base = ecs_info->pmu_base[pcpu];
	if (!is_power_up(pcpu)) {
		dev_err(ecs_info->dev, "Power down!\n");
		return -EACCES;
	}

	if (is_reset_state(pcpu)) {
		dev_err(ecs_info->dev, "Power on but reset state!\n");
		return -EACCES;
	}

	switch (exynos_cs_get_cpu_part_num(pcpu)) {
	case ARM_CPU_PART_CORTEX_A53:
	case ARM_CPU_PART_CORTEX_A57:
		DBG_UNLOCK(dbg_base);
		dbg_os_unlock(dbg_base);

		val = __raw_readl(dbg_base + DBGPCSRlo);
		valHi = __raw_readl(dbg_base + DBGPCSRhi);

		val |= (valHi << 32L);
		if (have_pc_offset(dbg_base))
			val -= 0x8;
		if (MSB_MASKING == (MSB_MASKING & val))
			val |= MSB_PADDING;

		dbg_os_lock(dbg_base);
		DBG_LOCK(dbg_base);
		break;
	case ARM_CPU_PART_CORTEX_A55:
	case ARM_CPU_PART_CORTEX_A75:
	case ARM_CPU_PART_CORTEX_A76:
	case ARM_CPU_PART_CORTEX_A78AE:
		DBG_UNLOCK(dbg_base);
		dbg_os_unlock(dbg_base);
		DBG_UNLOCK(pmu_base);

		val = __raw_readq(pmu_base + PMUPCSR);
		val = __raw_readq(pmu_base + PMUPCSR);
		ns = (val >> 63L) & 0x1;
		el = (val >> 61L) & 0x3;
		if (MSB_MASKING == (MSB_MASKING & val))
			val |= MSB_PADDING;

		vmid = __raw_readl(pmu_base + PMUVIDSR) & 0xFF;

		DBG_LOCK(pmu_base);
		dbg_os_lock(dbg_base);
		DBG_LOCK(dbg_base);
		break;
	default:
		break;
	}

	exynos_cs_pmu_snapshots[pcpu][iter].pc = val;
	exynos_cs_pmu_snapshots[pcpu][iter].ns = ns;
	exynos_cs_pmu_snapshots[pcpu][iter].el = el;
	exynos_cs_pmu_snapshots[pcpu][iter].vmid = vmid;

	return 0;
}

static int exynos_cs_dump_pmu_snapshots(void)
{
	unsigned long flags, val;
	int vcpu, pcpu, iter;
	int el, vmid;
	int my_vmid = 0;

#ifdef CONFIG_VLM
	my_vmid = nkops.nk_id_get();
#endif

	if (!ecs_info) {
		pr_err("exynos-coresight disabled\n");
		return 0;
	}

	local_irq_save(flags);
	for (iter = 0; iter < ITERATION; iter++) {
		for_each_present_cpu(vcpu) {
			pcpu = dbg_snapshot_get_pcpu_id(vcpu);
			if (pcpu < 0)
				continue;

			exynos_cs_pmu_snapshots[pcpu][iter].pc = 0;
			if (exynos_cs_get_pmu_snapshot(pcpu, iter) < 0)
				continue;
		}
	}

	local_irq_restore(flags);
	for_each_present_cpu(vcpu) {
		pcpu = dbg_snapshot_get_pcpu_id(vcpu);
		if (pcpu < 0)
			continue;

		dev_err(ecs_info->dev,
			"vCPU[%d], pCPU[%d] saved pc value\n", vcpu, pcpu);
		for (iter = 0; iter < ITERATION; iter++) {
			val = exynos_cs_pmu_snapshots[pcpu][iter].pc;
			el = exynos_cs_pmu_snapshots[pcpu][iter].el;
			vmid = exynos_cs_pmu_snapshots[pcpu][iter].vmid;
			if (!val)
				continue;

			if (my_vmid == vmid)
				dev_err(ecs_info->dev,
					"     VMID:%d, EL%d, 0x%016zx : %pS\n",
					vmid, el, val, (void *)val);
			else
				dev_err(ecs_info->dev,
					"     VMID:%d, EL%d, 0x%016zx\n",
					vmid, el, val);
		}
	}
	return 0;
}

#if IS_ENABLED(CONFIG_EXYNOS_HARDLOCKUP_WATCHDOG)
static int exynos_cs_lockup_handler(struct notifier_block *nb,
					unsigned long l, void *core)
{
	unsigned int *vcpu = (unsigned int *)core;

	dev_err(ecs_info->dev, "Hard Locked-up on vCPU[%d]\n", *vcpu);

	return exynos_cs_dump_pmu_snapshots();
}

static struct notifier_block exynos_cs_lockup_nb = {
	.notifier_call = exynos_cs_lockup_handler,
};
#endif

static int exynos_cs_panic_handler(struct notifier_block *np,
				unsigned long l, void *msg)
{
	return exynos_cs_dump_pmu_snapshots();
}

static struct notifier_block exynos_cs_panic_nb = {
	.notifier_call = exynos_cs_panic_handler,
};

static void exynos_cs_halt_enable(unsigned int pcpu)
{
	if (!ecs_info->halt_enabled || !ecs_info->cti_base[pcpu])
		return;

	__raw_writel(OSLOCK_MAGIC, ecs_info->cti_base[pcpu] + DBGLAR);
	__raw_writel(CTICH0, ecs_info->cti_base[pcpu] + CTIGATE);
	__raw_writel(CTICH0, ecs_info->cti_base[pcpu] + CTIINEN(0));
	__raw_writel(CTICH0, ecs_info->cti_base[pcpu] + CTIOUTEN(0));
	__raw_writel(0x1, ecs_info->cti_base[pcpu]);
}

static void exynos_cs_halt_disable(unsigned int pcpu)
{
	if (ecs_info->halt_enabled && ecs_info->cti_base[pcpu])
		__raw_writel(0x0, ecs_info->cti_base[pcpu]);
}

int exynos_cs_stop_cpus(void)
{
	int pcpu, vcpu;

	if (!ecs_info || !ecs_info->halt_enabled || !ecs_info->gpr_base) {
		pr_err("exynos-coresight-halt is disabled\n");
		return -ENODEV;
	}

	local_irq_disable();
	vcpu = raw_smp_processor_id();
	pcpu = dbg_snapshot_get_pcpu_id(vcpu);
	if (pcpu < 0)
		return -EINVAL;
	dev_info(ecs_info->dev, "vcpu%d calls HALT!!\n", vcpu);

	/* gen sw dbgack for mask wdt reset */
	__raw_writel(OSLOCK_MAGIC, ecs_info->gpr_base + DBGLAR);
	isb();
	__raw_writel(ecs_info->dbgack_mask, ecs_info->gpr_base);
	isb();

	__raw_writel(CTICH0, ecs_info->cti_base[pcpu] + CTIAPPSET);

	asm(
	"	mov x0, #0x01		\n"
	"	msr oslar_el1, x0	\n"
	"	isb			\n"
	"1:	mrs x1, oslsr_el1	\n"
	"	and x1, x1, #(1<<1)	\n"
	"	cbz x1, 1b		\n"
	"	mrs x0, mdscr_el1	\n"
	"	orr x0, x0, #(1<<14)	\n"
	"	msr mdscr_el1, x0	\n"
	"	isb			\n"
	"2:	mrs x0, mdscr_el1	\n"
	"	and x0, x0, #(1<<14)	\n"
	"	cbz x0, 2b		\n"
	"	msr oslar_el1, xzr	\n"
	"	hlt #0xff	"
	:: );

	return 0;
}
EXPORT_SYMBOL(exynos_cs_stop_cpus);

static int exynos_cs_suspend_cpu(unsigned int vcpu)
{
	int idx = 0;
	void __iomem *base;
	int pcpu;

	if (!ecs_info->retention_enabled)
		return 0;

	pcpu = dbg_snapshot_get_pcpu_id(vcpu);
	if (pcpu < 0)
		return -EINVAL;

	pr_debug("%s: pcpu %d\n", __func__, pcpu);
	base = ecs_info->dbg_base[pcpu];

	DBG_UNLOCK(base);
	dbg_os_lock(base);
	SYS_READ(DBGBVR0_EL1, ecs_info->bw_reg[idx++]); /* DBGBVR */
	SYS_READ(DBGBVR1_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBVR2_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBVR3_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBVR4_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBVR5_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBCR0_EL1, ecs_info->bw_reg[idx++]); /* DBGDCR */
	SYS_READ(DBGBCR1_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBCR2_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBCR3_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBCR4_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGBCR5_EL1, ecs_info->bw_reg[idx++]);

	SYS_READ(DBGWVR0_EL1, ecs_info->bw_reg[idx++]); /* DBGWVR */
	SYS_READ(DBGWVR1_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGWVR2_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGWVR3_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGWCR0_EL1, ecs_info->bw_reg[idx++]); /* DBGDCR */
	SYS_READ(DBGWCR1_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGWCR2_EL1, ecs_info->bw_reg[idx++]);
	SYS_READ(DBGWCR3_EL1, ecs_info->bw_reg[idx++]);

	idx = 0;
	SYS_READ(MDSCR_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	SYS_READ(OSECCR_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	SYS_READ(OSDTRTX_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	SYS_READ(OSDTRRX_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	SYS_READ(DBGCLAIMCLR_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	DBG_LOCK(base);

	pr_debug("%s: pcpu %d done\n", __func__, pcpu);

	return 0;
}

static int exynos_cs_resume_cpu(unsigned int vcpu)
{
	int idx = 0;
	void __iomem *base;
	int pcpu;

	if (!ecs_info->retention_enabled)
		return 0;

	pcpu = dbg_snapshot_get_pcpu_id(vcpu);
	if (pcpu < 0)
		return -EINVAL;

	base = ecs_info->dbg_base[pcpu];
	pr_debug("%s: pcpu %d\n", __func__, pcpu);

	DBG_UNLOCK(base);
	dbg_os_lock(base);

	SYS_WRITE(MDSCR_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	SYS_WRITE(OSECCR_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	SYS_WRITE(OSDTRTX_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	SYS_WRITE(OSDTRRX_EL1, ecs_info->dbg_reg[pcpu][idx++]);
	SYS_WRITE(DBGCLAIMSET_EL1, ecs_info->dbg_reg[pcpu][idx++]);

	idx = 0;
	SYS_WRITE(DBGBVR0_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBVR1_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBVR2_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBVR3_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBVR4_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBVR5_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBCR0_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBCR1_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBCR2_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBCR3_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBCR4_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGBCR5_EL1, ecs_info->bw_reg[idx++]);

	SYS_WRITE(DBGWVR0_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGWVR1_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGWVR2_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGWVR3_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGWCR0_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGWCR1_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGWCR2_EL1, ecs_info->bw_reg[idx++]);
	SYS_WRITE(DBGWCR3_EL1, ecs_info->bw_reg[idx++]);

	dbg_os_unlock(base);
	DBG_LOCK(base);

	pr_debug("%s: %d done\n", __func__, pcpu);
	return 0;
}

static int exynos_cs_c2_notifier(struct notifier_block *self,
		unsigned long cmd, void *v)
{
	int pcpu, vcpu;

	vcpu = raw_smp_processor_id();
	pcpu = dbg_snapshot_get_pcpu_id(vcpu);
	if (pcpu < 0)
		return -EINVAL;

	switch (cmd) {
	case CPU_PM_ENTER:
		exynos_cs_suspend_cpu(vcpu);
		exynos_cs_halt_disable(pcpu);
		break;
	case CPU_PM_ENTER_FAILED:
	case CPU_PM_EXIT:
		exynos_cs_resume_cpu(vcpu);
		exynos_cs_halt_enable(pcpu);
		break;
	case CPU_CLUSTER_PM_ENTER:
		exynos_cs_halt_disable(pcpu);
		break;
	case CPU_CLUSTER_PM_ENTER_FAILED:
	case CPU_CLUSTER_PM_EXIT:
		exynos_cs_halt_enable(pcpu);
		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block exynos_cs_c2_nb = {
	.notifier_call = exynos_cs_c2_notifier,
};

static int exynos_cs_parsing_dt(struct platform_device *pdev)
{
	struct property *prop;
	struct device_node *np = pdev->dev.of_node;
	const __be32 *cur;
	u32 val, i = 0;
	int ret;

	if (dbg_snapshot_get_sjtag_status())
		return -EACCES;

	of_property_for_each_u32(np, "dbg_base", prop, cur, val) {
		if (!val)
			return -EINVAL;
		ecs_info->dbg_base[i] = devm_ioremap(&pdev->dev, val, SZ_4K);
		if (!ecs_info->dbg_base[i]) {
			dev_err(ecs_info->dev, "fail property dbg_base(%d) ioremap\n", i);
			return -ENOMEM;
		}
		i++;
	}
	ecs_info->dbg_base_cnt = i;

	i = 0;
	of_property_for_each_u32(np, "cti_base", prop, cur, val) {
		if (!val)
			return -EINVAL;
		ecs_info->cti_base[i] = devm_ioremap(&pdev->dev, val, SZ_4K);
		if (!ecs_info->cti_base[i]) {
			dev_err(ecs_info->dev, "fail property cti_base(%d) ioremap\n", i);
			return -ENOMEM;
		}
		i++;
	}
	ecs_info->cti_base_cnt = i;

	i = 0;
	of_property_for_each_u32(np, "pmu_base", prop, cur, val) {
		if (!val)
			return -EINVAL;
		ecs_info->pmu_base[i] = devm_ioremap(&pdev->dev, val, SZ_4K);
		if (!ecs_info->pmu_base[i]) {
			dev_err(ecs_info->dev, "fail property pmu_base(%d) ioremap\n", i);
			return -ENOMEM;
		}
		i++;
	}
	ecs_info->pmu_base_cnt = i;

	if ((ecs_info->dbg_base_cnt != ecs_info->cti_base_cnt) &&
			(ecs_info->cti_base_cnt != ecs_info->pmu_base_cnt)) {
		dev_err(ecs_info->dev,
				"dbg_base_cnt(%d), cti_base_cnt(%d), pmu_base_cnt(%d)\n",
				ecs_info->dbg_base_cnt, ecs_info->cti_base_cnt,
				ecs_info->pmu_base_cnt);
		return -EINVAL;
	}
	ecs_info->core_cnt = min(ecs_info->pmu_base_cnt, CS_MAX_CORE_CNT);

	ret = of_property_read_u32(np, "halt", &val);
	if (ret || !val) {
		dev_info(ecs_info->dev, "disable halt function\n");
		goto retention;
	}
	ecs_info->halt_enabled = val;


	ret = of_property_read_u32(np, "gpr_base", &val);
	if (ret) {
		dev_warn(ecs_info->dev, "no such gpr_base\n");
		return 0;
	}
	ecs_info->gpr_base = devm_ioremap(&pdev->dev, val, SZ_4K);
	if (!ecs_info->gpr_base) {
		dev_err(ecs_info->dev, "failed gpr_base ioremap\n");
		return -ENOMEM;
	}

	ret = of_property_read_u32(np, "dbgack-mask", &val);
	if (ret) {
		dev_info(ecs_info->dev, "no such dbgack-mask\n");
		return 0;
	}
	ecs_info->dbgack_mask = val;
retention:
	ret = of_property_read_u32(np, "retention", &val);
	if (ret || !val) {
		dev_info(ecs_info->dev, "disable retention debug register\n");
		return 0;
	}
	ecs_info->retention_enabled = val;

	return 0;
}

static int exynos_coresight_probe(struct platform_device *pdev)
{
	int ret = 0, pcpu, vcpu;

	ecs_info = devm_kzalloc(&pdev->dev,
			sizeof(struct exynos_coresight_info), GFP_KERNEL);
	if (!ecs_info) {
		dev_err(&pdev->dev, "can not alloc memory\n");
		ret = -ENOMEM;
		goto err;
	}
	ecs_info->dev = &pdev->dev;
	ret = exynos_cs_parsing_dt(pdev);
	if (ret < 0) {
		dev_err(&pdev->dev, "%s failed.\n", __func__);
		goto err;
	}

#if IS_ENABLED(CONFIG_EXYNOS_HARDLOCKUP_WATCHDOG)
	atomic_notifier_chain_register(&hardlockup_notifier_list,
			&exynos_cs_lockup_nb);
#endif

	if (ecs_info->halt_enabled || ecs_info->retention_enabled) {
		if (ecs_info->halt_enabled) {
			for_each_present_cpu(vcpu) {
				pcpu = dbg_snapshot_get_pcpu_id(vcpu);
				if (pcpu < 0)
					goto err;

				exynos_cs_halt_enable(pcpu);
			}
		}

		if (ecs_info->retention_enabled) {
			ret = cpuhp_setup_state_nocalls_cpuslocked(
					CPUHP_AP_ONLINE_DYN,
					"exynoscoresight:online",
					exynos_cs_resume_cpu,
					exynos_cs_suspend_cpu);
			if (ret < 0)
				goto err;
		}
		ret = cpu_pm_register_notifier(&exynos_cs_c2_nb);
		if (ret < 0)
			goto err;
	}

	atomic_notifier_chain_register(&panic_notifier_list,
			&exynos_cs_panic_nb);
	dbg_snapshot_register_debug_ops(exynos_cs_stop_cpus, NULL, NULL);
	dev_info(&pdev->dev, "%s Successful\n", __func__);
	return ret;
err:
	ecs_info = NULL;
	return ret;
}

static int exynos_coresight_remove(struct platform_device *pdev)
{
	cpu_pm_unregister_notifier(&exynos_cs_c2_nb);
	return 0;
}

static const struct of_device_id exynos_coresight_matches[] = {
	{ .compatible = "samsung,exynos-coresight", },
	{},
};
MODULE_DEVICE_TABLE(of, exynos_coresight_matches);

static struct platform_driver exynos_coresight_driver = {
	.probe		= exynos_coresight_probe,
	.remove		= exynos_coresight_remove,
	.driver		= {
		.name	= "exynos-coresight",
		.of_match_table	= of_match_ptr(exynos_coresight_matches),
	},
};
module_platform_driver(exynos_coresight_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Exynos Coresight");
