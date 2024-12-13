#include "pmucal/include/pmucal_common.h"
#include "pmucal/include/pmucal_local.h"
#include "pmucal/include/pmucal_rae.h"
#include "pmucal/include/pmucal_system.h"
#include "pmucal/include/pmucal_powermode.h"

#include "exynosauto9/flexpmu_cal_local_exynosauto9.h"
#include "exynosauto9/flexpmu_cal_p2vmap_exynosauto9.h"
#include "exynosauto9/flexpmu_cal_system_exynosauto9.h"
#include "exynosauto9/flexpmu_cal_powermode_exynosauto9.h"
#include "exynosauto9/flexpmu_cal_define_exynosauto9.h"

#include "exynosauto9/cmucal-node.c"
#include "exynosauto9/cmucal-qch.c"
#include "exynosauto9/cmucal-sfr.c"
#include "exynosauto9/cmucal-vclk.c"
#include "exynosauto9/cmucal-vclklut.c"
#include "exynosauto9/clkout_exynosauto9.c"
#include "exynosauto9/acpm_dvfs_exynosauto9.h"

#include "exynosautov920/evt0/pmucal/flexpmu_cal_cpu_exynosautov920.h"
#include "exynosautov920/evt0/pmucal/flexpmu_cal_local_exynosautov920.h"
#include "exynosautov920/evt0/pmucal/flexpmu_cal_p2vmap_exynosautov920.h"
#include "exynosautov920/evt0/pmucal/flexpmu_cal_system_exynosautov920.h"
#include "exynosautov920/evt0/pmucal/flexpmu_cal_define_exynosautov920.h"

#include "exynosautov920/evt0/cmucal/cmucal-node.c"
#include "exynosautov920/evt0/cmucal/cmucal-qch.c"
#include "exynosautov920/evt0/cmucal/cmucal-sfr.c"
#include "exynosautov920/evt0/cmucal/cmucal-vclk.c"
#include "exynosautov920/evt0/cmucal/cmucal-vclklut.c"
#include "exynosautov920/evt0/cmucal/clkout_exynosautov920_evt0.c"
#include "exynosautov920/acpm_dvfs_exynosautov920.h"

#include "asv_exynosauto.h"

#include "exynosautov920/evt2/pmucal/flexpmu_cal_cpu_exynosautov920.h"
#include "exynosautov920/evt2/pmucal/flexpmu_cal_local_exynosautov920.h"
#include "exynosautov920/evt2/pmucal/flexpmu_cal_p2vmap_exynosautov920.h"
#include "exynosautov920/evt2/pmucal/flexpmu_cal_system_exynosautov920.h"
#include "exynosautov920/evt2/pmucal/flexpmu_cal_define_exynosautov920.h"

#include "exynosautov920/evt2/cmucal/cmucal-node.c"
#include "exynosautov920/evt2/cmucal/cmucal-qch.c"
#include "exynosautov920/evt2/cmucal/cmucal-sfr.c"
#include "exynosautov920/evt2/cmucal/cmucal-vclk.c"
#include "exynosautov920/evt2/cmucal/cmucal-vclklut.c"
#include "exynosautov920/evt2/cmucal/clkout_exynosautov920_evt2.c"

#include "exynosautov620/evt0/cmucal/cmucal-node.c"
#include "exynosautov620/evt0/cmucal/cmucal-qch.c"
#include "exynosautov620/evt0/cmucal/cmucal-sfr.c"
#include "exynosautov620/evt0/cmucal/cmucal-vclk.c"
#include "exynosautov620/evt0/cmucal/cmucal-vclklut.c"
#include "exynosautov620/evt0/cmucal/clkout_exynosautov620_evt0.c"
#include "exynosautov620/evt0/acpm_dvfs_exynosautov620.h"

#include "exynosautov620/evt0/pmucal/flexpmu_cal_cpu_exynosautov620.h"
#include "exynosautov620/evt0/pmucal/flexpmu_cal_local_exynosautov620.h"
#include "exynosautov620/evt0/pmucal/flexpmu_cal_p2vmap_exynosautov620.h"
#include "exynosautov620/evt0/pmucal/flexpmu_cal_system_exynosautov620.h"
#include "exynosautov620/evt0/pmucal/flexpmu_cal_define_exynosautov620.h"

unsigned int frac_rpll_list[20];
unsigned int frac_rpll_size;

unsigned int exynosautov920_evt0_frac_rpll_list[] = {
	V920_EVT0_PLL_CPUCL0,
	V920_EVT0_PLL_CPUCL1,
	V920_EVT0_PLL_CPUCL2,
	V920_EVT0_PLL_G3D,
	V920_EVT0_PLL_AUD,
	V920_EVT0_PLL_AVB,
	V920_EVT0_PLL_SFI,
	V920_EVT0_PLL_ETH,
	V920_EVT0_PLL_SHARED0,
	V920_EVT0_PLL_SHARED1,
	V920_EVT0_PLL_SHARED2,
	V920_EVT0_PLL_SHARED3,
	V920_EVT0_PLL_SHARED4,
	V920_EVT0_PLL_SHARED5,
	V920_EVT0_PLL_MMC,
};

unsigned int exynosautov920_evt2_frac_rpll_list[] = {
	V920_EVT2_PLL_AUD,
	V920_EVT2_PLL_AVB,
	V920_EVT2_PLL_SHARED1,
	V920_EVT2_PLL_SHARED4,
	V920_EVT2_PLL_SHARED3,
	V920_EVT2_PLL_SHARED2,
	V920_EVT2_PLL_SHARED0,
	V920_EVT2_PLL_SHARED5,
	V920_EVT2_PLL_MMC,
	V920_EVT2_PLL_CPUCL0,
	V920_EVT2_PLL_CPUCL1,
	V920_EVT2_PLL_CPUCL2,
	V920_EVT2_PLL_G3D,
	V920_EVT2_PLL_ETH,
	V920_EVT2_PLL_SFI,
};

unsigned int exynosautov620_evt0_frac_rpll_list[] = {
	V620_EVT0_PLL_AUD,
	V620_EVT0_PLL_AVB,
	V620_EVT0_PLL_SHARED1,
	V620_EVT0_PLL_SHARED4,
	V620_EVT0_PLL_SHARED3,
	V620_EVT0_PLL_SHARED2,
	V620_EVT0_PLL_SHARED0,
	V620_EVT0_PLL_SHARED5,
	V620_EVT0_PLL_MMC,
	V620_EVT0_PLL_CPUCL0,
	V620_EVT0_PLL_CPUCL2,
	V620_EVT0_PLL_G3D,
	V620_EVT0_PLL_ETH,
	V620_EVT0_PLL_SFI,
};

#include "ra.h"

void exynosauto9_cal_data_init(void)
{
	pr_info("%s: cal data init\n", __func__);
	/* cpu inform sfr initialize */
	pmucal_sys_powermode[SYS_SLEEP] = CPU_INFORM_SLEEP_V9;
}

void exynosautov920_evt0_cal_data_init(void)
{
	int i;

	frac_rpll_size = ARRAY_SIZE(exynosautov920_evt0_frac_rpll_list);
	for (i = 0; i < frac_rpll_size; i++)
		frac_rpll_list[i] = exynosautov920_evt0_frac_rpll_list[i];

	pr_info("%s: cal data init\n", __func__);
	/* cpu inform sfr initialize */
	pmucal_sys_powermode[SYS_SLEEP] = CPU_INFORM_SLEEP_V920_EVT0;
}

void exynosautov920_evt2_cal_data_init(void)
{
	int i;

	frac_rpll_size = ARRAY_SIZE(exynosautov920_evt2_frac_rpll_list);
	for (i = 0; i < frac_rpll_size; i++)
		frac_rpll_list[i] = exynosautov920_evt2_frac_rpll_list[i];

	pr_info("%s: cal data init\n", __func__);
	/* cpu inform sfr initialize */
	pmucal_sys_powermode[SYS_SLEEP] = CPU_INFORM_SLEEP;
}

void exynosautov620_evt0_cal_data_init(void)
{
	int i;

	frac_rpll_size = ARRAY_SIZE(exynosautov620_evt0_frac_rpll_list);
	for (i = 0; i < frac_rpll_size; i++)
		frac_rpll_list[i] = exynosautov620_evt0_frac_rpll_list[i];

	pr_info("%s: cal data init\n", __func__);
	/* cpu inform sfr initialize */
	pmucal_sys_powermode[SYS_SLEEP] = CPU_INFORM_SLEEP;
}

void (*cal_data_init)(void);
