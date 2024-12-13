// SPDX-License-Identifier: GPL-2.0
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include "include/pmucal_system.h"
#include "include/pmucal_rae.h"
#include "include/pmucal_powermode.h"

unsigned int pmucal_sys_powermode[NUM_SYS_POWERDOWN] = {0xffffffff, };

struct pmucal_system_variant *pmucal_system_variant_model;

struct pmucal_system_variant pmucal_system_variant_v9 = {
	.p_pmucal_lpm_init_size = &pmucal_lpm_init_size_v9,
	.p_pmucal_lpm_list_size = &pmucal_lpm_list_size_v9,
	.p_pmucal_lpm_init = pmucal_lpm_init_v9,
	.p_pmucal_lpm_list = pmucal_lpm_list_v9,
};

struct pmucal_system_variant pmucal_system_variant_v920_evt0 = {
	.p_pmucal_lpm_init_size = &pmucal_lpm_init_size_v920_evt0,
	.p_pmucal_lpm_list_size = &pmucal_lpm_list_size_v920_evt0,
	.p_pmucal_lpm_init = pmucal_lpm_init_v920_evt0,
	.p_pmucal_lpm_list = pmucal_lpm_list_v920_evt0,
};

struct pmucal_system_variant pmucal_system_variant_v920_evt2 = {
	.p_pmucal_lpm_init_size = &pmucal_lpm_init_size,
	.p_pmucal_lpm_list_size = &pmucal_lpm_list_size,
	.p_pmucal_lpm_init = pmucal_lpm_init,
	.p_pmucal_lpm_list = pmucal_lpm_list,
};

struct pmucal_system_variant pmucal_system_variant_v620_evt0 = {
	.p_pmucal_lpm_init_size = &pmucal_lpm_init_size_v620_evt0,
	.p_pmucal_lpm_list_size = &pmucal_lpm_list_size_v620_evt0,
	.p_pmucal_lpm_init = pmucal_lpm_init_v620_evt0,
	.p_pmucal_lpm_list = pmucal_lpm_list_v620_evt0,
};
/**
 *  pmucal_system_enter - prepares to enter a system power mode.
 *		        exposed to PWRCAL interface.
 *
 *  @mode: index of system power mode described in pmucal_system.h.
 *
 *  Returns 0 on success. Otherwise, negative error code.
 */
/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To interface from CAL_IF and PMU_CAL for suspend operation"
 * @logic "save CMU information and execute PMU sequence for suspend operation"
 * @params
 * @param{in, mode, int, >=0}
 * @endparam
 * @retval{ret, int, -, >=0, <0}
 */
int pmucal_system_enter(int mode)
{
	int ret;

	if (mode >= NUM_SYS_POWERDOWN) {
		pr_err("%s %s: mode index(%d) is out of supported range (0~%d).\n",
				PMUCAL_PREFIX, __func__, mode, NUM_SYS_POWERDOWN);
		return -EINVAL;
	}

	if (!pmucal_system_variant_model->p_pmucal_lpm_list[mode].enter) {
		pr_err("%s %s: there is no sequence element for entering mode(%d).\n",
				PMUCAL_PREFIX, __func__, mode);
		return -ENOENT;
	}

	pmucal_powermode_hint(pmucal_sys_powermode[mode]);

	pmucal_rae_save_seq(pmucal_system_variant_model->p_pmucal_lpm_list[mode].save, pmucal_system_variant_model->p_pmucal_lpm_list[mode].num_save);

	ret = pmucal_rae_handle_seq(pmucal_system_variant_model->p_pmucal_lpm_list[mode].enter,
				pmucal_system_variant_model->p_pmucal_lpm_list[mode].num_enter);
	if (ret) {
		pr_err("%s %s: error on handling enter sequence. (mode : %d)\n",
				PMUCAL_PREFIX, __func__, mode);
		return ret;
	}

	pmucal_dbg_set_emulation(pmucal_system_variant_model->p_pmucal_lpm_list[mode].dbg);
	pmucal_dbg_do_profile(pmucal_system_variant_model->p_pmucal_lpm_list[mode].dbg, false);

	return 0;
}

/**
 *  pmucal_system_exit - prepares to exit a system power mode.
 *		        exposed to PWRCAL interface.
 *
 *  @mode: index of system power mode described in pmucal_system.h.
 *
 *  Returns 0 on success. Otherwise, negative error code.
 */
/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To interface from CAL_IF and PMU_CAL for resume operation"
 * @logic "execute PMU sequence for resume operation and restore CMU information"
 * @params
 * @param{in, mode, int, >=0}
 * @endparam
 * @retval{ret, int, -, >=0, <0}
 */
int pmucal_system_exit(int mode)
{
	int ret;

	if (mode >= NUM_SYS_POWERDOWN) {
		pr_err("%s %s: mode index(%d) is out of supported range (0~%d).\n",
				PMUCAL_PREFIX, __func__, mode, NUM_SYS_POWERDOWN);
		return -EINVAL;
	}

	if (!pmucal_system_variant_model->p_pmucal_lpm_list[mode].exit) {
		pr_err("%s %s: there is no sequence element for exiting mode(%d).\n",
				PMUCAL_PREFIX, __func__, mode);
		return -ENOENT;
	}

	ret = pmucal_rae_handle_seq(pmucal_system_variant_model->p_pmucal_lpm_list[mode].exit,
				pmucal_system_variant_model->p_pmucal_lpm_list[mode].num_exit);
	if (ret) {
		pr_err("%s %s: error on handling exit sequence. (mode : %d)\n",
				PMUCAL_PREFIX, __func__, mode);
		return ret;
	}

	ret = pmucal_rae_restore_seq(pmucal_system_variant_model->p_pmucal_lpm_list[mode].save,
				pmucal_system_variant_model->p_pmucal_lpm_list[mode].num_save);
	if (ret) {
		pr_err("%s %s: error on handling restore sequence. (mode : %d)\n",
				PMUCAL_PREFIX, __func__, mode);
		return ret;
	}

	pmucal_dbg_do_profile(pmucal_system_variant_model->p_pmucal_lpm_list[mode].dbg, true);

	return 0;
}

/**
 *  pmucal_system_earlywakeup - prepares to early-wakeup from a system power mode.
 *				exposed to PWRCAL interface.
 *
 *  @mode: index of system power mode described in pmucal_system.h.
 *
 *  Returns 0 on success. Otherwise, negative error code.
 */
/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To interface from CAL_IF and PMU_CAL for early wake-up"
 * @logic "execute PMU sequence for early wake-up and restore CMU information"
 * @params
 * @param{in, mode, int, >=0}
 * @endparam
 * @retval{ret, int, -, >=0, <0}
 */
int pmucal_system_earlywakeup(int mode)
{
	int ret;

	if (mode >= NUM_SYS_POWERDOWN) {
		pr_err("%s %s: mode index(%d) is out of supported range (0~%d).\n",
				PMUCAL_PREFIX, __func__, mode, NUM_SYS_POWERDOWN);
		return -EINVAL;
	}

	if (!pmucal_system_variant_model->p_pmucal_lpm_list[mode].early_wakeup) {
		pr_err("%s %s: there is no sequence element for early_wkup mode(%d).\n",
				PMUCAL_PREFIX, __func__, mode);
		return -ENOENT;
	}

	ret = pmucal_rae_handle_seq(pmucal_system_variant_model->p_pmucal_lpm_list[mode].early_wakeup,
				pmucal_system_variant_model->p_pmucal_lpm_list[mode].num_early_wakeup);
	if (ret) {
		pr_err("%s %s: error on handling elry_wkup sequence. (mode : %d)\n",
				PMUCAL_PREFIX, __func__, mode);
		return ret;
	}

	ret = pmucal_rae_restore_seq(pmucal_system_variant_model->p_pmucal_lpm_list[mode].save,
				pmucal_system_variant_model->p_pmucal_lpm_list[mode].num_save);
	if (ret) {
		pr_err("%s %s: error on handling restore sequence. (mode : %d)\n",
				PMUCAL_PREFIX, __func__, mode);
		return ret;
	}
	pmucal_powermode_hint_clear();

	return 0;
}

/**
 *  pmucal_system_init - Init function of PMUCAL SYSTEM common logic.
 *		            exposed to PWRCAL interface.
 *
 *  Returns 0 on success. Otherwise, negative error code.
 */
/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To map virtual and physical address and initialize PMU"
 * @logic "store virtual address in the structure and execute pmucal_lpm_init"
 * @noparam
 * @retval{ret, int, 0, >=0, <0}
 */
int pmucal_system_init(void)
{
	int ret = 0, i;

	if (!(*pmucal_system_variant_model->p_pmucal_lpm_init_size) || !(*pmucal_system_variant_model->p_pmucal_lpm_list_size)) {
		pr_err("%s %s: there is no lpm init seq or lpm list. aborting init...\n",
				PMUCAL_PREFIX, __func__);
		return -ENOENT;
	}

	/* convert physical base address to virtual addr */
	for (i = 0; i < NUM_SYS_POWERDOWN; i++) {
		/* skip non-existing power mode */
		if (!pmucal_system_variant_model->p_pmucal_lpm_list[i].num_enter
					&& !pmucal_system_variant_model->p_pmucal_lpm_list[i].num_exit
					&& !pmucal_system_variant_model->p_pmucal_lpm_list[i].num_early_wakeup)
			continue;
		ret = pmucal_rae_phy2virt(pmucal_system_variant_model->p_pmucal_lpm_list[i].enter,
					pmucal_system_variant_model->p_pmucal_lpm_list[i].num_enter);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:enter, mode_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}

		ret = pmucal_rae_phy2virt(pmucal_system_variant_model->p_pmucal_lpm_list[i].save,
					pmucal_system_variant_model->p_pmucal_lpm_list[i].num_save);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:save, mode_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}
		ret = pmucal_rae_phy2virt(pmucal_system_variant_model->p_pmucal_lpm_list[i].exit,
					pmucal_system_variant_model->p_pmucal_lpm_list[i].num_exit);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:exit, mode_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}
		ret = pmucal_rae_phy2virt(pmucal_system_variant_model->p_pmucal_lpm_list[i].early_wakeup,
					pmucal_system_variant_model->p_pmucal_lpm_list[i].num_early_wakeup);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:erlywkup, mode_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}
	}
	ret = pmucal_rae_phy2virt(pmucal_system_variant_model->p_pmucal_lpm_init, *pmucal_system_variant_model->p_pmucal_lpm_init_size);
	if (ret) {
		pr_err("%s %s: error on PA2VA conversion for lpm_init seq. aborting init...\n",
				PMUCAL_PREFIX, __func__);
		goto out;
	}
	/* do default settings for PMU */
	ret = pmucal_rae_handle_seq(pmucal_system_variant_model->p_pmucal_lpm_init, *pmucal_system_variant_model->p_pmucal_lpm_init_size);
	if (ret) {
		pr_err("%s %s: error on handling lpm_init sequence.\n",
				PMUCAL_PREFIX, __func__);
		goto out;
	}
	pr_info("CAL : pmucal_system_init done\n");

out:
	return ret;
}
