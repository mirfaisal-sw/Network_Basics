

#include <stdint.h>

/* PCI device */
typedef struct {
	/* Base address region */
	unsigned int bar;

	/* Slot info */
	unsigned int domain;
	unsigned int bus;
	unsigned int slot;
	unsigned int function;

	/* Resource filename */
	char         filename[100];

	/* File descriptor of the resource */
	int          fd;

	/* Memory mapped resource */
	unsigned char *maddr;
	unsigned int   size;
	unsigned int   offset;

	/* PCI physical address */
	unsigned int   phys;

	/* Address to pass to read/write (includes offset) */
	unsigned char *addr;
} device_t;

void print_bytes_bytes(uint64_t offset, uint8_t value_read);
void print_bytes_halfword(uint64_t offset, uint16_t value_read);
void print_bytes_word(uint64_t offset, uint32_t value_read);
void print_bytes_doubleword(uint64_t offset, uint64_t value_read);
