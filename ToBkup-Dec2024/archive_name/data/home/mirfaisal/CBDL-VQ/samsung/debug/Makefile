#
#  Makefile for Exynos-Debug
#

obj-$(CONFIG_DEBUG_SNAPSHOT) 		:= dss.o
dss-y 					+= debug-snapshot.o \
					debug-snapshot-log.o\
					debug-snapshot-utils.o \
					debug-snapshot-dpm.o \
					debug-snapshot-pmsg.o \
					debug-snapshot-pmsg-tty.o \
					cache.o

obj-$(CONFIG_DEBUG_SNAPSHOT_DEBUG_KINFO) += debug-snapshot-debug-kinfo.o
obj-$(CONFIG_DEBUG_SNAPSHOT_SFRDUMP)	+= debug-snapshot-sfrdump.o
obj-$(CONFIG_EXYNOS_DEBUG_TEST)         += exynos-debug-test.o
obj-$(CONFIG_EXYNOS_EHLD)               += exynos-ehld.o
obj-$(CONFIG_EXYNOS_HARDLOCKUP_WATCHDOG) += hardlockup-watchdog.o
obj-$(CONFIG_EXYNOS_ADV_TRACER) 	+= eat.o
eat-y					:= exynos-adv-tracer.o

obj-$(CONFIG_EXYNOS_ADV_TRACER_S2D)	+= exynos-adv-tracer-s2d.o

obj-$(CONFIG_EXYNOS_ECC_HANDLER)	+= exynos-ecc-handler.o

obj-$(CONFIG_EXYNOS_ITMON)		+= exynos-itmon.o

exynos-itmon-y				+= exynos-itmon-platform.o \
					exynos-itmon-v1.o \
					exynos-itmon-v2.o \

exynos-itmon-$(CONFIG_SOC_EXYNOSAUTO9)	+= exynosauto9-itmon.o \
					exynosautov920-itmon.o \
					exynosautov920-evt0-itmon.o \
					exynosautov620-itmon.o \

obj-$(CONFIG_EXYNOS_CORESIGHT)         += exynos-coresight.o

obj-$(CONFIG_PANIC_FINGERPRINT)         += panic-fingerprint.o
