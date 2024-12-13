#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <ctype.h>
/*
 * Description	: This code implements the restricted access to the memory
 * 		via more restrictions as compaed to devmem utility. /dev/mem
		file has been a bit inappropriately used leading to kernel/HW
		issues caused by the user-space applcation. This implementation
		has been influenced from the devmem code at following link:
		https://github.com/brgl/busybox/blob/master/miscutils/devmem.c

		It has following modifications from the said code:
		a) Adds support to read/write from any file, not just from /dev/mem.
		b) Some libcalls has been substituted as the ones in the original
		tool-chain were not supported. For example, xatou has been replaced
		by strtoul.
		c) It prints information , in a bit different format. For example,
		it prints virtual and physical address in addition to the
		value has been read.
*
*
* Author	: Parikshit Pareek <parikshit.pareek@harman.com>
*/


#define MAP_MASK (sysconf(_SC_PAGE_SIZE) - 1)


#define show_usage(info) \
{ \
	printf("Error: %s\n", info);\
	printf("Usage: devmem ADDRESS(in hex format like 0x0100) [WIDTH]\n");\
	exit(1);\
}

int main(int argc, char **argv)
{

	int memfd, i, ret, choice = 0, write_off;
	void *mapped_base, *virt_addr;
	off_t dev_base;
	size_t length = 0 /*= MAP_SIZE + MEM_BASE_ADDRESS - dev_base2*/;
	unsigned long width = 8 * sizeof(int), offset_in_page;
	long int width_ip;
	uint64_t read_result;


	if (argc < 2)
		show_usage("Too few arguments")

	else if (argc > 3)
		show_usage("Too many arguments")

	memfd = open("/dev/rest_mem_access", O_RDONLY | O_SYNC);

	if (memfd == -1) {
		printf("Can't open mapping file\n");
		close(memfd);
		exit(0);
	}

	/*Address*/
	if (!argv[1])
		show_usage("No address found\n")

	if (1 != sscanf(argv[1], "0x%lx", &dev_base))
		show_usage("Put address in hex format only. starting with prefix 0x")

	length = sysconf(_SC_PAGE_SIZE);

	/* WIDTH */
	if (argv[2]) {
		if (isdigit(argv[2][0]) || argv[2][1]) {
			//width = xatou(argv[2]);
			width_ip = strtoul(argv[2], NULL, 10);
			if (width_ip < 0) {
				printf("wrong width input. set it to default value\n");
				width_ip = 8 * sizeof(int);
			}
			width = width_ip;
		}
		else {
			switch(argv[2][0] | 0x20) {
				case 'b':
					width = 0;
				break;
				case 'h':
					width = 1;
				break;
				case 'w':
					width = 2;
				break;
				case 'l':
					width = 3;
				break;
				default:
					width = 5;
			}

			static const uint8_t sizes[] = {
				8 * sizeof(char),
				8 * sizeof(short),
				8 * sizeof(int),
				8 * sizeof(long),
				8 * sizeof(int) /* bad input. fall back to default width*/
			};

			width = sizes[width];
		}

	}

	offset_in_page = (unsigned)dev_base & MAP_MASK;

	if ((offset_in_page  + width) > length)
		length *= 2;

	/*Page alighned address*/

	mapped_base = mmap(0, length, PROT_READ , MAP_FIXED |
				MAP_SHARED, memfd,
				dev_base & ~MAP_MASK);
	if (mapped_base == (void *) -1) {
		printf("Can't map the memory to user space.\n");
		exit(1);
	}

	virt_addr = (char*)mapped_base+ offset_in_page;

	switch (width) {
	case 8:
		read_result = *(volatile uint8_t*)virt_addr;
		break;
	case 16:
		read_result = *(volatile uint16_t*)virt_addr;
		break;
	case 32:
		read_result = *(volatile uint32_t*)virt_addr;
		break;
	case 64:
		read_result = *(volatile uint64_t*)virt_addr;
		break;
	default:
		munmap(mapped_base,  /*MAP_SIZE*/length);
		close(memfd);
		show_usage("bad width")
	}

	printf("phy addr 0x%lx virtual addr 0x%p value 0x%0*llX\n", dev_base,
					virt_addr, (int)(width >> 2),
					(unsigned long long)read_result);

	munmap(mapped_base, length);
	close(memfd);

	return 0;
}
