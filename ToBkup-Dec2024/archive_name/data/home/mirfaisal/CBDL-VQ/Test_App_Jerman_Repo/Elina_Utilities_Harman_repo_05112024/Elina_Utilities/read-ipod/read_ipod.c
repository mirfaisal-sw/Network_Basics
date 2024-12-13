#include <stdio.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char* argv[])
{

	char device[40] = "/dev/i2c-1";
	int fd = 0, devnum = 0;
	int addr = 0;
	int ret,i;
	int len = 3, val = 0x00;
	char buf[32];
	char reg[3];
	unsigned long funcs;
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg i2cmsgs[3];

	if(argc < 2) {
		printf("./ipod 0xaddr 0xreg\n");
		exit(1);
	}
	addr = strtol(argv[1], NULL, 16);
	reg[0] = strtol(argv[2], NULL, 16);;

	printf("./ipod 0xaddr 0xreg\n");

	msgset.nmsgs = 1;
	msgset.msgs = i2cmsgs;
	i2cmsgs[0].addr = addr;
	i2cmsgs[0].flags = 0;
	i2cmsgs[0].buf = &reg[0];
	i2cmsgs[0].len = 1;

	
	fd = open(device, O_RDWR);
	if (fd < 0) {
		printf("Failed to open %s\n",device);
		exit(1);
	}

	/* Get the adapter functionality */
	if (ioctl(fd, I2C_FUNCS, &funcs) < 0) {
		printf("Failed to get functioality\n");
		goto fail;
	}

	
	if (funcs & I2C_FUNC_I2C)
		printf("Supports repeated start\n");
	else {
		printf("Doen't support repeated start\n");
		goto fail;
	}

reinit:	ret = ioctl(fd, I2C_RDWR, &msgset);
	if (ret < 0) {
		printf("Failed to perform ipod init %s err %d\n",
			device, ret);
		usleep(500);
		goto reinit;
	} else {
		printf("ipod initiated\n");
	}


restart:	msgset.nmsgs = 1;
	msgset.msgs = i2cmsgs;
	i2cmsgs[0].addr = addr;
	i2cmsgs[0].flags = I2C_M_RD;
	i2cmsgs[0].buf = &reg[1];
	i2cmsgs[0].len = 1;
	ret = ioctl(fd, I2C_RDWR, &msgset);
	if (ret < 0) {
		printf("Failed to read ipod 0x%02x err %d\n", reg[0], ret);
		usleep(500);
		goto restart;
	} else
		printf("Read 0x%02x = 0x%02x\n", reg[0], reg[1]);



fail:
	close(fd);
	return 0;

}
