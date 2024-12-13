
obj-$(CONFIG_V4L2_MVIF) += v4l2-mvif.o
ifeq ($(CONFIG_V4L2_DUAL_DMB),y)
v4l2-mvif-objs := iukMVIFRoles_multi_stream.o VideoIfaceConfig.o vsink_multi_stream.o v4l_mvif_intf_multi_stream.o
else
v4l2-mvif-objs := iukMVIFRoles.o VideoIfaceConfig.o vsink.o v4l_mvif_intf.o
endif



