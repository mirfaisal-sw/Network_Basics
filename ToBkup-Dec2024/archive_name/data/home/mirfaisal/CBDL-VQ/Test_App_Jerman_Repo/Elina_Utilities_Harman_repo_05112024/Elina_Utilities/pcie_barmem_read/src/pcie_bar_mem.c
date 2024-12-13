

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>
#include <getopt.h>

#include "user_defines.h"

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

uint32_t _cl_bytes_to_read;
char *_cl_slot;
uint8_t _cl_bar_num;
uint8_t _cl_access_width = 32;

/* Usage */
static void show_usage(void)
{
	printf("\nUsage: pci_barmem -s <device>\n" \
		"  -s <device>		Slot/device (as per lspci)\n" \
		"  -b <BAR>		Base address region (BAR) to access, eg. 0 for BAR0\n" \
		"  -w <access_width>   Acees width to read BAR memory, dafault is 32 bits\n" \
		"  -n [Bytes]		To read only few bytes, as compared to full BAR region\n" \
		"  -h                  Help (this message)\n");
}

int get_pages_to_map(unsigned long int num_of_bytes)
{
	uint32_t num_of_pages = num_of_bytes/MAP_SIZE;

	return (num_of_bytes % MAP_SIZE) ? (num_of_pages += 1) : num_of_pages;
}

int main(int argc, char **argv)
{
	int opt;
	void *map_base, *virt_mapped_addr;
	uint64_t i_loop;
	uint32_t num_pages_to_map = 0;
	uint8_t type_width;
	uint64_t read_value;

	int status;
	struct stat statbuf;
	device_t device;
	device_t *dev = &device;

	/* Clear the structure fields */
	memset(dev, 0, sizeof(device_t));

	static struct option long_options[] = {
		{"slot", required_argument, NULL, 's'},
		{"bar_num", required_argument, NULL, 'b'},
		{"num_bytes", required_argument, NULL, 'n'},
		{"access_width", required_argument, NULL, 'w'},
		{"help", no_argument, NULL, 'h'},
		{}
	};

	while ((opt = getopt_long(argc, argv, "s:b:n:w:h", long_options, NULL)) != -1) {
		switch (opt) {
		case 's':
			_cl_slot = optarg;
			break;

		case 'b':
			/* Defaults to BAR0 if not provided */
			_cl_bar_num = atoi(optarg);
			break;

		case 'n':
			_cl_bytes_to_read = atoi(optarg);
			printf("No. of bytes to read - %d\n", _cl_bytes_to_read);
			break;

		case 'w':
			_cl_access_width = atoi(optarg);
			break;

		case 'h':
			show_usage();
			return -1;

		default:
			show_usage();
			exit(EXIT_FAILURE);
		}
	}

	if (_cl_slot == 0) {
		printf("Invalid slot, correct format is AAAA:BB:DD.F\n");
		return -1;
	}

	/* ------------------------------------------------------------
	 * Open and map the PCI region
	 * ------------------------------------------------------------
	 */

	/* Extract the PCI parameters from the slot string */
	status = sscanf(_cl_slot, "%4x:%2x:%2x.%1x",
			&dev->domain, &dev->bus, &dev->slot, &dev->function);
	if (status != 4) {
		printf("Error parsing slot information!\n");
		show_usage();
		return -1;
	}

	dev->bar = _cl_bar_num;

	/* Convert to a sysfs resource filename and open the resource */
	snprintf(dev->filename, 99, "/sys/bus/pci/devices/%04x:%02x:%02x.%1x/resource%d",
			dev->domain, dev->bus, dev->slot, dev->function, dev->bar);

	dev->fd = open(dev->filename, O_RDWR | O_SYNC);
	if (dev->fd < 0) {
		printf("Open failed for file '%s': errno %d, %s\n",
			dev->filename, errno, strerror(errno));
		return -1;
	}

	/* PCI memory size */
	status = fstat(dev->fd, &statbuf);
	if (status < 0) {
		printf("fstat() failed: errno %d, %s\n",
			errno, strerror(errno));
		return -1;
	}
	dev->size = statbuf.st_size;

	/*If command line option "_cl_bytes_to_read" is absent,
	 * then read full BAR region.
	 */
	if (_cl_bytes_to_read == 0) {
		_cl_bytes_to_read = dev->size;
		/* Map */
		dev->maddr = (unsigned char *)mmap(
			NULL,
			(size_t)(dev->size),
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			dev->fd,
			0);
	} else {
		num_pages_to_map = get_pages_to_map(_cl_bytes_to_read);
		/* Map */
		dev->maddr = (unsigned char *)mmap(
			NULL,
			(size_t)(num_pages_to_map*MAP_SIZE),
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			dev->fd,
			0);
	}

	if (dev->maddr == (unsigned char *)MAP_FAILED) {
		printf("failed (mmap returned MAP_FAILED)\n");
		printf("I/O mapped BARs are not supported by this tool\n");
		dev->maddr = 0;
		close(dev->fd);
		return -1;
	}

	/*
	 * Print Initial Message
	 */
	printf("pcie_barmem application:  \n");
	printf("Accessing BAR-%d of size - %d bytes\n", dev->bar, dev->size);

	/*Read BAR memory region*/
	for (i_loop = 0; (i_loop*type_width) < _cl_bytes_to_read; i_loop++) {

		switch (_cl_access_width) {

		case 8:
			type_width = 1;
			virt_mapped_addr = dev->maddr + (i_loop*type_width);
			read_value = *(uint8_t *)virt_mapped_addr;

			/* Call print hexdump function.*/
			print_bytes_bytes(i_loop, read_value);
			break;

		case 16:
			type_width = 2;
			virt_mapped_addr = dev->maddr + (i_loop*type_width);
			read_value = *(uint16_t *)virt_mapped_addr;

			/* Call print hexdump function.*/
			print_bytes_halfword(i_loop, read_value);
			break;

		case 32:
			type_width = 4;
			virt_mapped_addr = dev->maddr + (i_loop*type_width);
			read_value = *(uint32_t *)virt_mapped_addr;

			/* Call print hexdump function.*/
			print_bytes_word(i_loop, read_value);
			break;

		case 64:
			type_width = 8;
			virt_mapped_addr = dev->maddr + (i_loop*type_width);
			read_value = *(uint64_t *)virt_mapped_addr;

			/* Call print hexdump function.*/
			print_bytes_doubleword(i_loop, read_value);
			break;

		default:
			printf("Invalid access width\n");
			printf("Valid access bit widths are: 8, 16, 32, 64.\n");
			break;
		}
	}

	printf("\n");
	if (_cl_bytes_to_read == 0)
		munmap(dev->maddr, dev->size);
	else
		munmap(dev->maddr, (num_pages_to_map*MAP_SIZE));

	close(dev->fd);
	return 0;
}
