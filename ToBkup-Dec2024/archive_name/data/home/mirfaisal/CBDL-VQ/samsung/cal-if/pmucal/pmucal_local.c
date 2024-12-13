// SPDX-License-Identifier: GPL-2.0
/*
 * EXYNOS - CAL-IF support
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include "include/pmucal_local.h"
#include "include/pmucal_rae.h"

#ifndef PWRCAL_TARGET_LINUX
struct pmucal_pd *pmucal_blkpwr_list[PMUCAL_NUM_PDS];
#endif

struct pmucal_local_variant *pmucal_local_variant_model;

struct pmucal_local_variant pmucal_local_variant_v9 = {
	.p_pmucal_pd_list = pmucal_pd_list_v9,
	.p_pmucal_pd_list_size = &pmucal_pd_list_size_v9,
};

struct pmucal_local_variant pmucal_local_variant_v920_evt0 = {
	.p_pmucal_pd_list = pmucal_pd_list_v920_evt0,
	.p_pmucal_pd_list_size = &pmucal_pd_list_size_v920_evt0,
};

struct pmucal_local_variant pmucal_local_variant_v920_evt2 = {
	.p_pmucal_pd_list = pmucal_pd_list,
	.p_pmucal_pd_list_size = &pmucal_pd_list_size,
};

struct pmucal_local_variant pmucal_local_variant_v620_evt0 = {
	.p_pmucal_pd_list = pmucal_pd_list_v620_evt0,
	.p_pmucal_pd_list_size = &pmucal_pd_list_size_v620_evt0,
};

/**
 *  pmucal_local_enable - enables a power domain.
 *		        exposed to PWRCAL interface.
 *
 *  @pd_id: power domain index.
 *
 *  Returns 0 on success. Otherwise, negative error code.
 */
/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To enable local power domain"
 * @logic "execute PMU sequence for enabling local power domain"
 * @params
 * @param{in, pd_id, unsigned int, >=0}
 * @endparam
 * @retval{ret, int, -, >=0, <0}
 */
int pmucal_local_enable(unsigned int pd_id)
{
	int ret;

	if (pd_id >= *pmucal_local_variant_model->p_pmucal_pd_list_size) {
		pr_err("%s pd index(%d) is out of supported range (0~%d).\n",
				PMUCAL_PREFIX, pd_id, *pmucal_local_variant_model->p_pmucal_pd_list_size);
		return -EINVAL;
	}

	if (!pmucal_local_variant_model->p_pmucal_pd_list[pd_id].on) {
		pr_err("%s there is no sequence element for pd(%d) power-on.\n",
				PMUCAL_PREFIX, pd_id);
		return -ENOENT;
	}

	ret = pmucal_rae_handle_seq(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].on,
				pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_on);
	if (ret) {
		pr_err("%s %s: error on handling enable sequence. (pd_id : %d)\n",
				PMUCAL_PREFIX, __func__, pd_id);
		return ret;
	}

	if (pmucal_local_variant_model->p_pmucal_pd_list[pd_id].need_smc) {
		ret = exynos_pd_tz_restore(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].need_smc);
		if (ret) {
			pr_err("%s %s: DTZPC restore smc error. (pd_id : %d)\n",
					PMUCAL_PREFIX, __func__, pd_id);
			return ret;
		}
	}

	if (pmucal_local_variant_model->p_pmucal_pd_list[pd_id].first_on) {
		if (!pmucal_local_variant_model->p_pmucal_pd_list[pd_id].cmu_init) {
			pr_err("%s there is no sequence element for pd(%d) cmu_init.\n",
					PMUCAL_PREFIX, pd_id);
			return -ENOENT;
		}

		ret = pmucal_rae_handle_seq(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].cmu_init,
				pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_cmu_init);

		if (ret) {
			pr_err("%s %s: error on handling cmu_init sequence. (pd_id : %d)\n",
					PMUCAL_PREFIX, __func__, pd_id);
			return -ENOENT;
		}

		pmucal_local_variant_model->p_pmucal_pd_list[pd_id].first_on = false;
	} else {
		ret = pmucal_rae_restore_seq(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].save,
				pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_save);
		if (ret) {
			pr_err("%s %s: error on handling restore sequence. (pd_id : %d)\n",
					PMUCAL_PREFIX, __func__, pd_id);
			return ret;
		}
	}

	pmucal_dbg_do_profile(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].dbg, true);

	return 0;
}

/**
 *  pmucal_local_disable - disables a power domain.
 *		        exposed to PWRCAL interface.
 *
 *  @pd_id: power domain index.
 *
 *  Returns 0 on success. Otherwise, negative error code.
 */
/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To disable local power domain"
 * @logic "execute PMU sequence for disabling local power domain"
 * @params
 * @param{in, pd_id, unsigned int, >=0}
 * @endparam
 * @retval{ret, int, -, >=0, <0}
 */
int pmucal_local_disable(unsigned int pd_id)
{
	int ret, i;

	if (pd_id >= *pmucal_local_variant_model->p_pmucal_pd_list_size) {
		pr_err("%s pd index(%d) is out of supported range (0~%d).\n",
				PMUCAL_PREFIX, pd_id, *pmucal_local_variant_model->p_pmucal_pd_list_size);
		return -EINVAL;
	}

	if (!pmucal_local_variant_model->p_pmucal_pd_list[pd_id].off) {
		pr_err("%s there is no sequence element for pd(%d) power-off.\n",
				PMUCAL_PREFIX, pd_id);
		return -ENOENT;
	}

	pmucal_rae_save_seq(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].save,
				pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_save);

	if (pmucal_local_variant_model->p_pmucal_pd_list[pd_id].need_smc) {
		ret = exynos_pd_tz_save(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].need_smc);
		if (ret) {
			pr_err("%s %s: DTZPC save smc error. (pd_id : %d)\n",
					PMUCAL_PREFIX, __func__, pd_id);
			return ret;
		}
	}

	pmucal_dbg_set_emulation(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].dbg);

	ret = pmucal_rae_handle_seq(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].off,
				pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_off);
	if (ret) {
		pr_err("%s %s: error on handling disable sequence. (pd_id : %d)\n",
				PMUCAL_PREFIX, __func__, pd_id);

		for (i = 0; i < pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_save; i++) {
			pr_err("%s[0x%x] = 0x%x\n", pmucal_local_variant_model->p_pmucal_pd_list[pd_id].save[i].sfr_name,
							pmucal_local_variant_model->p_pmucal_pd_list[pd_id].save[i].offset,
							pmucal_local_variant_model->p_pmucal_pd_list[pd_id].save[i].value);
		}

		return ret;
	}

	pmucal_dbg_do_profile(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].dbg, false);

	return 0;
}

/**
 *  pmucal_local_is_enabled - checks whether a power domain is enabled or not.
 *		            exposed to PWRCAL interface.
 *
 *  @pd_id: power domain index.
 *
 *  Returns 1 when the pd is enabled, 0 when disabled.
 *  Otherwise, negative error code.
 */
/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To check status of local power domain"
 * @logic "execute PMU sequence for checking status of local power domain"
 * @params
 * @param{in, pd_id, unsigned int, >=0}
 * @endparam
 * @retval{ret, int, -, >=0, <0}
 */
int pmucal_local_is_enabled(unsigned int pd_id)
{
	int ret, i;

	if (pd_id >= *pmucal_local_variant_model->p_pmucal_pd_list_size) {
		pr_err("%s pd index(%d) is out of supported range (0~%d).\n",
				PMUCAL_PREFIX, pd_id, *pmucal_local_variant_model->p_pmucal_pd_list_size);
		return -EINVAL;
	}

	if (!pmucal_local_variant_model->p_pmucal_pd_list[pd_id].status) {
		pr_err("%s there is no sequence element for pd(%d) status.\n",
				PMUCAL_PREFIX, pd_id);
		return -ENOENT;
	}

	ret = pmucal_rae_handle_seq(pmucal_local_variant_model->p_pmucal_pd_list[pd_id].status,
				pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_status);

	if (ret) {
		pr_err("%s %s: error on checking status. (pd_id : %d)\n",
				PMUCAL_PREFIX, __func__, pd_id);
		return ret;
	}

	for (i = 0; i < pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_status; i++) {
		if (pmucal_local_variant_model->p_pmucal_pd_list[pd_id].status[i].value !=
				pmucal_local_variant_model->p_pmucal_pd_list[pd_id].status[i].mask)
			break;
	}

	if (i == pmucal_local_variant_model->p_pmucal_pd_list[pd_id].num_status)
		return 1;
	else
		return 0;
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To set whether it is default off power domain or not"
 * @logic "set first_on boolean variant as true if initial_state is off"
 * @params
 * @param{in, pd_id, unsigned int, >=0}
 * @param{in, initial_state, unsigned int, >=0}
 * @endparam
 * @noret
 */
void pmucal_local_set_first_on(unsigned int pd_id, unsigned int initial_state)
{
	if (!initial_state) {
		if (pd_id >= *pmucal_local_variant_model->p_pmucal_pd_list_size) {
			pr_err("%s pd index(%d) is out of supported range (0~%d).\n",
					PMUCAL_PREFIX, pd_id, *pmucal_local_variant_model->p_pmucal_pd_list_size);
			return;
		}

		pmucal_local_variant_model->p_pmucal_pd_list[pd_id].first_on = true;
	}
}

/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To save the TZPC information in the structure"
 * @logic "store the need_smc to the structure mapped with pd_id"
 * @params
 * @param{in, pd_id, unsigned int, >=0}
 * @param{in, need_smc, unsigned int, >=0}
 * @endparam
 * @noret
 */
void pmucal_local_set_smc_id(unsigned int pd_id, unsigned int need_smc)
{
	if (need_smc) {
		if (pd_id >= *pmucal_local_variant_model->p_pmucal_pd_list_size) {
			pr_err("%s pd index(%d) is out of supported range (0~%d).\n",
					PMUCAL_PREFIX, pd_id, *pmucal_local_variant_model->p_pmucal_pd_list_size);
			return;
		}

		pmucal_local_variant_model->p_pmucal_pd_list[pd_id].need_smc = need_smc;
	}
}

/**
 *  pmucal_local_init - Init function of PMUCAL LOCAL common logic.
 *		            exposed to PWRCAL interface.
 *
 *  Returns 0 on success. Otherwise, negative error code.
 */
/**
 * @cnotice
 * @prdcode
 * @unit_name{PMU_CAL}
 * @purpose "To map virtual and physical address"
 * @logic "store virtual address in the structure"
 * @noparam
 * @retval{ret, int, 0, >=0, <0}
 */
int pmucal_local_init(void)
{
	int ret = 0, i;

	if (!(*pmucal_local_variant_model->p_pmucal_pd_list_size)) {
		pr_err("%s %s: there is no pd list. aborting init...\n",
				PMUCAL_PREFIX, __func__);
		return -ENOENT;
	}

	/* convert physical base address to virtual addr */
	for (i = 0; i < *pmucal_local_variant_model->p_pmucal_pd_list_size; i++) {
		/* skip non-existing pd */
		if (!pmucal_local_variant_model->p_pmucal_pd_list[i].num_on &&
				!pmucal_local_variant_model->p_pmucal_pd_list[i].num_off &&
				!pmucal_local_variant_model->p_pmucal_pd_list[i].num_status)
			continue;

		ret = pmucal_rae_phy2virt(pmucal_local_variant_model->p_pmucal_pd_list[i].on,
					pmucal_local_variant_model->p_pmucal_pd_list[i].num_on);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:on, pd_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}

		ret = pmucal_rae_phy2virt(pmucal_local_variant_model->p_pmucal_pd_list[i].save,
					pmucal_local_variant_model->p_pmucal_pd_list[i].num_save);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:save, pd_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}

		ret = pmucal_rae_phy2virt(pmucal_local_variant_model->p_pmucal_pd_list[i].off,
					pmucal_local_variant_model->p_pmucal_pd_list[i].num_off);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:off, pd_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}

		ret = pmucal_rae_phy2virt(pmucal_local_variant_model->p_pmucal_pd_list[i].status,
					pmucal_local_variant_model->p_pmucal_pd_list[i].num_status);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:status, pd_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}

		ret = pmucal_rae_phy2virt(pmucal_local_variant_model->p_pmucal_pd_list[i].cmu_init,
					pmucal_local_variant_model->p_pmucal_pd_list[i].num_cmu_init);
		if (ret) {
			pr_err("%s %s: error on PA2VA conversion. seq:cmu_init, pd_id:%d. aborting init...\n",
					PMUCAL_PREFIX, __func__, i);
			goto out;
		}

#ifndef PWRCAL_TARGET_LINUX
		pmucal_blkpwr_list[i] = pmucal_local_variant_model->p_pmucal_pd_list + i;
#endif
	}

	pr_info("CAL : pmucal_local_init is done\n");

out:
	return ret;
}
