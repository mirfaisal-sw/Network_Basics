#obj-$(CONFIG_SPI_DYNAMIC_NODE)  += dynamic_spi_node.o

obj-m  += dynamic_spi_node_ctrl.o
dynamic_spi_node_ctrl-y := dynamic_spi_node.o spi_pinctrl.o

#obj-m += gpio-interrupt.o
#obj-m += hwreg_rw.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C /home/mir/My_Workspace/mir-board-kernel M=`pwd` modules
 
clean:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C /home/mir/My_Workspace/mir-board-kernel M=`pwd` clean
