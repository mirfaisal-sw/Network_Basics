#
# Makefile for Harman dspipc driver
#

KBUILD_CFLAGS += -DUSE_DSP_SHM
ifeq ($(CONFIG_SOC_DRA7XX),y)
KBUILD_CFLAGS += -DCHIP_DRA7XX
endif
ifeq ($(CONFIG_SOC_J5ECO),y)
KBUILD_CFLAGS += -DCHIP_DRA6XX
endif
KBUILD_CFLAGS += -DVERBOSE_ENABLE
KBUILD_CFLAGS += -DNEW_ROOT_POINTER_STRUCT

KBUILD_CFLAGS += -Idrivers/harman/dspipc/src
KBUILD_CFLAGS += -Idrivers/harman/dspipc/src/import/include
KBUILD_CFLAGS += -Idrivers/harman/dspipc/src/import/include/dsp
KBUILD_CFLAGS += -Idrivers/harman/dspipc/src/import/include/dsp/common

obj-$(CONFIG_DSPIPC) := dspipc.o
dspipc-objs := dev-dspipc.o
dspipc-objs += debug.o
dspipc-objs += Memobj.o
dspipc-objs += malloc_trc.o
dspipc-objs += Options.o
dspipc-objs += dspipc_pthread.o
dspipc-objs += cache_ops.o
dspipc-objs += misc.o
dspipc-objs += msgqueue.o
dspipc-objs += device.o
dspipc-objs += string_def.o
dspipc-objs += sendthread.o
dspipc-objs += subdevice.o
dspipc-objs += intthread.o
dspipc-objs += iofuncs.o
ifeq ($(CONFIG_SOC_DRA7XX),y)
dspipc-objs += dra7xx/dspipc-dra7xx.o
dspipc-objs += dra7xx/elf.o
endif
ifeq ($(CONFIG_SOC_J5ECO),y)
dspipc-objs += dra6xx/dspipc-dra6xx.o
dspipc-objs += dra6xx/coff.o
dspipc-objs += dra6xx/elf.o
endif

