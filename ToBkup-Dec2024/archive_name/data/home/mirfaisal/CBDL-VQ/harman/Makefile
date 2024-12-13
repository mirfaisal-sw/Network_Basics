
# Makefile for the harman drivers.
#

# Each configuration option enables a list of files.

obj-$(CONFIG_DAB_PLUGIN)	+= dabplugin/
obj-$(CONFIG_SPI_PNP_PLUGIN)    += pnp_plugin/
obj-$(CONFIG_SPI_DYNAMIC_NODE)	+= dynamic_spi_node/
obj-$(CONFIG_SPI_OMAP24XX_SLAVE)	+= slave_mcspi/
obj-$(CONFIG_DSPIPC)           += dspipc/
obj-$(CONFIG_TMS320C674x_MMU_FAULT_HANDLER)	+= dsp_mmu/
ifeq ($(CONFIG_V4L2_V2),y)
obj-$(CONFIG_V4L2_MVIF)        += v4l2-mvif_v2/
else ifeq ($(CONFIG_V4L2_V1),y)
obj-$(CONFIG_V4L2_MVIF)        += v4l2-mvif_v1/
endif

