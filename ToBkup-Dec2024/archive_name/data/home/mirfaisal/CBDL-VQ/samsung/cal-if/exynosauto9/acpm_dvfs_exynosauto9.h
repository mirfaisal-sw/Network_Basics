enum acpm_dvfs_id {
	dvfs_mif = ACPM_VCLK_TYPE,
	dvfs_int,
	dvfs_cpucl0,
	dvfs_cpucl1,
	dvfs_npu,
	dvfs_aud,
	dvfs_g3d00,
	dvfs_g3d01,
	dvfs_g3d1,
	dvfs_cam,
	dvfs_sfi,
};

struct vclk acpm_vclk_list[] = {
	CMUCAL_ACPM_VCLK(dvfs_mif, NULL, NULL, NULL, NULL, MARGIN_MIF),
	CMUCAL_ACPM_VCLK(dvfs_int, NULL, NULL, NULL, NULL, MARGIN_INT),
	CMUCAL_ACPM_VCLK(dvfs_cpucl0, NULL, NULL, NULL, NULL, MARGIN_CPUCL0),
	CMUCAL_ACPM_VCLK(dvfs_cpucl1, NULL, NULL, NULL, NULL, MARGIN_CPUCL1),
	CMUCAL_ACPM_VCLK(dvfs_npu, NULL, NULL, NULL, NULL, MARGIN_NPU),
	CMUCAL_ACPM_VCLK(dvfs_aud, NULL, NULL, NULL, NULL, MARGIN_AUD),
	CMUCAL_ACPM_VCLK(dvfs_g3d00, NULL, NULL, NULL, NULL, MARGIN_G3D00),
	CMUCAL_ACPM_VCLK(dvfs_g3d01, NULL, NULL, NULL, NULL, MARGIN_G3D01),
	CMUCAL_ACPM_VCLK(dvfs_g3d1, NULL, NULL, NULL, NULL, MARGIN_G3D1),
	CMUCAL_ACPM_VCLK(dvfs_cam, NULL, NULL, NULL, NULL, MARGIN_CAM),
	CMUCAL_ACPM_VCLK(dvfs_sfi, NULL, NULL, NULL, NULL, MARGIN_SFI),
};

unsigned int acpm_vclk_size = ARRAY_SIZE(acpm_vclk_list);
