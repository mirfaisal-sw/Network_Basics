obj-m += silabs_spidev.o

ARCH=arm64
CROSS_COMPILE=aarch64-oe-linux-

all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C /data/home/mirfaisal/NTG7_AM62/NTG7_AM62_Harman_Kernel/ti_lts_5.10 C=2 M=`pwd` modules
 
clean:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C /data/home/mirfaisal/NTG7_AM62/NTG7_AM62_Harman_Kernel/ti_lts_5.10 M=`pwd` clean
