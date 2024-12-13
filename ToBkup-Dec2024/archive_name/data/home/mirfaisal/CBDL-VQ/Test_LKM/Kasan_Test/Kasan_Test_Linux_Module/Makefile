obj-m += kasan_test_am64_sys.o

ARCH=arm64
CROSS_COMPILE=aarch64-poky-linux-

all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C /data/home/mirfaisal/BMW_IDC/ES6_Codes/ES6_Artifact_176_CodeBase_19122023/sources/kernel M=`pwd` modules
 
clean:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C /data/home/mirfaisal/BMW_IDC/ES6_Codes/ES6_Artifact_176_CodeBase_19122023/sources/kernel M=`pwd` clean
