#
# Makefile for the CMUCAL drivers.
#

# Object files in subdirectories

obj-$(CONFIG_CAL_IF)			+= cmupmucal.o
cmupmucal-y				+= cal-if.o cmucal.o ra.o vclk.o pll_spec.o cmucal-debug.o cal_data.o
cmupmucal-$(CONFIG_PMUCAL)		+= pmucal/pmucal_system.o pmucal/pmucal_local.o pmucal/pmucal_rae.o pmucal/pmucal_powermode.o
cmupmucal-$(CONFIG_PMUCAL_DBG)		+= pmucal/pmucal_dbg.o
cmupmucal-$(CONFIG_ACPM_DVFS)		+= acpm_dvfs.o fvmap.o

obj-$(CONFIG_CMU_EWF)			+= cmu_ewf.o
