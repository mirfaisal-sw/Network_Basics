#ifndef __ASV_EXYNOSAUTO_H__
#define __ASV_EXYNOSAUTO_H__

#include <linux/io.h>
#include <soc/samsung/exynos-smc.h>
#include <soc/samsung/cal-if.h>

#define ASV_TABLE_BASE	(0x10009000)
#define ID_TABLE_BASE	(0x10000000)

struct asv_tbl_info {
	unsigned reserved_0:4;
	unsigned cpucl1_asv_group:4;
	unsigned cpucl0_asv_group:4;
	unsigned g3d1_asv_group:4;
	unsigned mif_asv_group:4;
	unsigned int_asv_group:4;
	unsigned cam_disp_asv_group:4;
	unsigned npu_asv_group:4;

	unsigned reserved_1:4;
	unsigned g3d00_asv_group:4;
	unsigned g3d01_asv_group:4;
	unsigned aud_asv_group:4;
	unsigned sfi_asv_group:4;
	unsigned reserved_00:4;
	unsigned reserved_01:4;
	unsigned reserved_02:4;

	unsigned modified_asv_reserved_00:4;
	unsigned modified_asv_reserved_01:4;
	unsigned modified_asv_reserved_02:4;
	unsigned modified_asv_reserved_03:4;
	unsigned modified_asv_reserved_04:4;
	unsigned modified_asv_reserved_05:4;
	unsigned modified_asv_reserved_06:4;
	unsigned modified_asv_reserved_07:4;

	unsigned asv_table_version:7;
	unsigned asv_group_type:1;
	unsigned asb_pgm_version:8;
	unsigned reserved_03:4;
	unsigned reserved_04:4;
	unsigned reserved_05:4;
	unsigned reserved_06:4;
};

struct id_tbl_info {
	unsigned reserved_0:12;
	unsigned product_id:20;

	unsigned reserved_1;

	unsigned reserved_2:10;
	unsigned product_line:2;
	unsigned reserved_3:4;
	unsigned char customer_ids_0:8;
	unsigned char customer_ids_1:8;

	unsigned char customer_ids_2:8;
	unsigned asb_version:8;
	unsigned char customer_ids_3:8;
	unsigned char customer_ids_4:8;

	unsigned char customer_ids_5:8;
	unsigned char customer_ids_6:8;
	unsigned short sub_rev:4;
	unsigned short main_rev:4;
	unsigned reserved_4:8;

	unsigned reserved_5;
	unsigned reserved_6;
	unsigned reserved_7;
	unsigned reserved_8;

	unsigned v9:2;
	unsigned v910:1;
	unsigned scp_sip:1;
	unsigned lp4_lp5:1;
	unsigned operating_temperature:1;
	unsigned reserved_9:26;
};

struct v920_asv_tbl_info {
	unsigned cpucl2_group:4;
	unsigned cpucl1_group:4;
	unsigned cpucl0_group:4;
	unsigned g3d_group:4;
	unsigned mif_group:4;
	unsigned int_group:4;
	unsigned cam_group:4;
	unsigned npu_group:4;

	unsigned aud_group:4;
	unsigned asv_tbl_reserved_00:4;
	unsigned asv_tbl_reserved_01:4;
	unsigned asv_tbl_reserved_02:4;
	unsigned asv_tbl_reserved_03:4;
	unsigned asv_tbl_reserved_04:4;
	unsigned asv_tbl_reserved_05:4;
	unsigned aud_modify_group:4;

	unsigned cpucl2_modify_group:4;
	unsigned cpucl1_modify_group:4;
	unsigned cpucl0_modify_group:4;
	unsigned g3d_modify_group:4;
	unsigned mif_modify_group:4;
	unsigned int_modify_group:4;
	unsigned cam_modify_group:4;
	unsigned npu_modify_group:4;

	unsigned asv_table_version:8;
	unsigned asb_pgm_version:8;
	unsigned asv_tbl_reserved_06:16;
};

struct v920_id_tbl_info {
	unsigned reserved_0:12;
	unsigned product_id:20;

	unsigned reserved_1;

	unsigned reserved_2:10;
	unsigned product_line:3;
	unsigned pte_usage:3;
	unsigned chip_id_reserved_00:8;
	unsigned chip_id_reserved_01:8;

	unsigned chip_id_reserved_02:8;
	unsigned chip_id_reserved_03:8;
	unsigned chip_id_reserved_04:8;
	unsigned chip_id_reserved_05:8;

	unsigned chip_id_reserved_06:8;
	unsigned chip_id_reserved_07:8;
	unsigned short sub_rev:4;
	unsigned short main_rev:4;
	unsigned pkg_revision:8;

	unsigned reserved_4:24;
	unsigned variant:4;
	unsigned dram:2;
	unsigned chip_id_reserved_08:2;

};
#define ASV_INFO_ADDR_CNT	(sizeof(struct asv_tbl_info) / 4)
#define ID_INFO_ADDR_CNT	(sizeof(struct id_tbl_info) / 4)
#define V920_ASV_INFO_ADDR_CNT	(sizeof(struct v920_asv_tbl_info) / 4)
#define V920_ID_INFO_ADDR_CNT	(sizeof(struct v920_id_tbl_info) / 4)

static struct asv_tbl_info asv_tbl;
static struct id_tbl_info id_tbl;
static struct v920_asv_tbl_info v920_asv_tbl;
static struct v920_id_tbl_info v920_id_tbl;

int id_get_asv_fusing(void)
{
	int val = 0;

	switch (cal_variant) {
	case EXYNOSAUTO_V9:
		val = (((id_tbl.v9 + (id_tbl.v910 << 2)) << 4) | asv_tbl.asv_table_version);
		break;
	case EXYNOSAUTO_V920_EVT0:
	case EXYNOSAUTO_V920_EVT1:
	case EXYNOSAUTO_V920_EVT2:
	case EXYNOSAUTO_V620_EVT0:
		val = ((v920_id_tbl.variant << 4) | v920_asv_tbl.asv_table_version);
		break;
	default:
		break;
	}
	return val;
}

int asv_table_init(void)
{
	int i;
	unsigned int *p_table;
	unsigned int *regs;

	p_table = (unsigned int *)&asv_tbl;

	regs = (unsigned int *)ioremap(ASV_TABLE_BASE, ASV_INFO_ADDR_CNT * sizeof(int));
	for (i = 0; i < ASV_INFO_ADDR_CNT; i++) {
		*(p_table + i) = (unsigned int)regs[i];
	}

	p_table = (unsigned int *)&id_tbl;

	regs = (unsigned int *)ioremap(ID_TABLE_BASE, ID_INFO_ADDR_CNT * sizeof(int));
	for (i = 0; i < ID_INFO_ADDR_CNT; i++)
		*(p_table + i) = (unsigned int)regs[i];

	pr_info("ASV: ASV_table_version : %d\n", asv_tbl.asv_table_version);
	pr_info("ASV: CPUCL0 Group      : %d\n", asv_tbl.cpucl0_asv_group);
	pr_info("ASV: CPUCL1 Group      : %d\n", asv_tbl.cpucl1_asv_group);
	pr_info("ASV: G3D1 Group        : %d\n", asv_tbl.g3d1_asv_group);
	pr_info("ASV: MIF Group         : %d\n", asv_tbl.mif_asv_group);
	pr_info("ASV: INT Group         : %d\n", asv_tbl.int_asv_group);
	pr_info("ASV: CAM_DISP Group    : %d\n", asv_tbl.cam_disp_asv_group);
	pr_info("ASV: NPU Group         : %d\n", asv_tbl.npu_asv_group);
	pr_info("ASV: G3D00 Group       : %d\n", asv_tbl.g3d00_asv_group);
	pr_info("ASV: G3D01 Group       : %d\n", asv_tbl.g3d01_asv_group);
	pr_info("ASV: AUD Group         : %d\n", asv_tbl.aud_asv_group);
	pr_info("ASV: SFI Group         : %d\n", asv_tbl.sfi_asv_group);
	pr_info("ASV: Product_line      : %d\n", id_tbl.product_line);
	pr_info("ASV: ASB_version       : %d\n", id_tbl.asb_version);
	pr_info("ASV: Product           : %d\n", id_tbl.v9 + (id_tbl.v910 << 2) + (id_tbl.scp_sip << 3));

	return asv_tbl.asv_table_version;
}

int v920_asv_table_init(void)
{
	int i;
	unsigned int *p_table;
	unsigned int *regs;

	p_table = (unsigned int *)&v920_asv_tbl;

	regs = (unsigned int *)ioremap(ASV_TABLE_BASE, V920_ASV_INFO_ADDR_CNT * sizeof(int));
	for (i = 0; i < V920_ASV_INFO_ADDR_CNT; i++) {
		*(p_table + i) = __raw_readl(&regs[i]);
	}

	p_table = (unsigned int *)&v920_id_tbl;

	regs = (unsigned int *)ioremap(ID_TABLE_BASE, V920_ID_INFO_ADDR_CNT * sizeof(int));
	for (i = 0; i < V920_ID_INFO_ADDR_CNT; i++) {
		*(p_table + i) = __raw_readl(&regs[i]);
	}

	/* To support SOC with missing ASV table version */
	if (!v920_asv_tbl.asv_table_version) {
		switch (cal_variant) {
		case EXYNOSAUTO_V920_EVT1:
			v920_asv_tbl.asv_table_version = 2;
			break;
		case EXYNOSAUTO_V920_EVT2:
			v920_asv_tbl.asv_table_version = 4;
			break;
		default:
			break;
		}
	}

	pr_info("ASV: ASV_table_version : %d\n", v920_asv_tbl.asv_table_version);
	pr_info("ASV: ASB_version       : %d\n", v920_asv_tbl.asb_pgm_version);
	pr_info("ASV: CPUCL0 Group      : %d\n", v920_asv_tbl.cpucl0_group);
	pr_info("ASV: CPUCL1 Group      : %d\n", v920_asv_tbl.cpucl1_group);
	pr_info("ASV: CPUCL2 Group      : %d\n", v920_asv_tbl.cpucl2_group);
	pr_info("ASV: G3D Group         : %d\n", v920_asv_tbl.g3d_group);
	pr_info("ASV: MIF Group         : %d\n", v920_asv_tbl.mif_group);
	pr_info("ASV: INT Group         : %d\n", v920_asv_tbl.int_group);
	pr_info("ASV: CAM Group         : %d\n", v920_asv_tbl.cam_group);
	pr_info("ASV: NPU Group         : %d\n", v920_asv_tbl.npu_group);
	pr_info("ASV: AUD Group         : %d\n", v920_asv_tbl.aud_group);
	pr_info("ASV: Product_line      : %d\n", v920_id_tbl.product_line);
	pr_info("ASV: Variant           : %d\n", v920_id_tbl.variant);
	pr_info("ASV: Dram              : %d\n", v920_id_tbl.dram);
	pr_info("ASV: Main_rev          : %d\n", v920_id_tbl.main_rev);
	pr_info("ASV: Sub_rev           : %d\n", v920_id_tbl.sub_rev);

	return v920_asv_tbl.asv_table_version;
}
#endif
