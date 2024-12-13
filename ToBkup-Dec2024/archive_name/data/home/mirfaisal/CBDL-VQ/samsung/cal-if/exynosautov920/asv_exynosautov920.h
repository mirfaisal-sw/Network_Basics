#ifndef __ASV_EXYNOSAUTOV920_H__
#define __ASV_EXYNOSAUTOV920_H__

#include <linux/io.h>
#include <soc/samsung/exynos-smc.h>

#define V920_ASV_TABLE_BASE	(0x10009000)
#define V920_ID_TABLE_BASE	(0x10000000)

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

#define V920_ASV_INFO_ADDR_CNT	(sizeof(struct v920_asv_tbl_info) / 4)
#define V920_ID_INFO_ADDR_CNT	(sizeof(struct v920_id_tbl_info) / 4)

static struct v920_asv_tbl_info v920_asv_tbl;
static struct v920_id_tbl_info v920_id_tbl;

void v920_id_get_rev(unsigned int *main_rev, unsigned int *sub_rev)
{
	*main_rev = v920_id_tbl.main_rev;
	*sub_rev =  v920_id_tbl.sub_rev;
}

int v920_id_get_product_line(void)
{
	return v920_id_tbl.product_line;
}

int v920_id_get_product(void)
{
	return v920_id_tbl.variant;
}

int v920_asv_table_init(void)
{
	int i;
	unsigned int *p_table;
	unsigned int *regs;

	p_table = (unsigned int *)&v920_asv_tbl;

	regs = (unsigned int *)ioremap(V920_ASV_TABLE_BASE, V920_ASV_INFO_ADDR_CNT * sizeof(int));
	for (i = 0; i < V920_ASV_INFO_ADDR_CNT; i++) {
		*(p_table + i) = __raw_readl(&regs[i]);
	}

	p_table = (unsigned int *)&v920_id_tbl;

	regs = (unsigned int *)ioremap(V920_ID_TABLE_BASE, V920_ID_INFO_ADDR_CNT * sizeof(int));
	for (i = 0; i < V920_ID_INFO_ADDR_CNT; i++) {
		*(p_table + i) = __raw_readl(&regs[i]);
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
