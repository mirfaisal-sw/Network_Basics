# SPDX-License-Identifier: GPL-2.0
obj-$(CONFIG_EXYNOS_ASV_ARM)	+= exynos5422-asv.o

obj-$(CONFIG_EXYNOS_CHIPID)	+= exynos-chipid.o exynos-asv.o
obj-$(CONFIG_EXYNOS_CHIPID_V2)	+= exynos-chipid_v2.o
obj-$(CONFIG_EXYNOS_PMU)	+= exynos-pmu.o
obj-$(CONFIG_EXYNOS_PMU_IF)	+= exynos-pmu-if.o
obj-$(CONFIG_EXYNOS_FLEXPMU_DBG)	+= exynos-flexpmu-dbg.o

obj-$(CONFIG_EXYNOS_BTS)		+= bts/
obj-$(CONFIG_EXYNOS_PD)				+= pd/
obj-$(CONFIG_EXYNOS_PMU_ARM_DRIVERS)	+= exynos3250-pmu.o exynos4-pmu.o \
					exynos5250-pmu.o exynos5420-pmu.o
obj-$(CONFIG_EXYNOS_PM_DOMAINS) += pm_domains.o
obj-$(CONFIG_EXYNOS_PM)         += pm/
obj-$(CONFIG_EXYNOS_REGULATOR_COUPLER) += exynos-regulator-coupler.o

obj-$(CONFIG_SAMSUNG_PM_CHECK)	+= s3c-pm-check.o
obj-$(CONFIG_SAMSUNG_PM_DEBUG)	+= s3c-pm-debug.o

# ACPM
obj-$(CONFIG_EXYNOS_ACPM)		+= acpm/

# SFI
obj-$(CONFIG_EXYNOS_SFI_IPC)	+= sfi/

# ECT
obj-$(CONFIG_ECT)		+= ect_parser.o

# CAL
obj-$(CONFIG_CAL_IF)		+= cal-if/

# BCM
obj-$(CONFIG_EXYNOS_BCM_DBG)	+= exynos-bcm_dbg.o exynos-bcm_dbg-dt.o
obj-$(CONFIG_EXYNOS_BCM_DBG_DUMP)	+= exynos-bcm_dbg-dump.o

# Exynosauto DRAM boot Log
obj-$(CONFIG_EXYNOS_DRAM_BOOTLOG) += exynos-drambootlog.o

# Exynosauto Boottime Log
obj-$(CONFIG_EXYNOS_BOOTTIME_LOG) += exynos-boottimelog.o
obj-$(CONFIG_EXYNOS_BOOTTIME_ESTIMATED_BY_SMC) += exynos-kernel-boottime-by-smc.o
obj-$(CONFIG_EXYNOS_BOOTTIME_ESTIMATED_BY_KERNEL_SYSTEM) += exynos-kernel-boottime-by-system.o

# EXYNOS_PM_QOS
obj-$(CONFIG_EXYNOS_PM_QOS)	+= exynos_pm_qos.o

# DEBUG
obj-$(CONFIG_EXYNOS_DEBUG)      += debug/

# Firmware Secureboot
obj-$(CONFIG_EXYNOS_FIRMWARE_SECUREBOOT) += exynos-fw-sb.o

# SSP
obj-$(CONFIG_EXYNOS_SSP) += exynos-ssp.o

# secmem
obj-$(CONFIG_EXYNOS_CONTENT_PATH_PROTECTION) += exynos-secmem.o

# Exynos Secure Log
obj-$(CONFIG_EXYNOS_SECURE_LOG) += exynos-seclog.o

# Detector for ppmpu failure
obj-$(CONFIG_EXYNOS_PPMPU_FAIL_DETECTOR) += exynos-ppmpu.o

# Exynos HDCP
obj-$(CONFIG_EXYNOS_HDCP2) += exynos-hdcp/

