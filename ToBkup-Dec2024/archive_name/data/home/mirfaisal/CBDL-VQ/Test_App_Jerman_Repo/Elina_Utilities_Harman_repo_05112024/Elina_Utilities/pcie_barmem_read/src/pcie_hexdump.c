#include <stdio.h>
#include <stdint.h>

#include "user_defines.h"

void print_bytes_bytes(uint64_t offset, uint8_t value_read)
{
	static uint32_t i;

	if ((offset%16) == 0) {
		printf("\n");
		printf("\r 0x%08x :: ", i*16);
		i++;
	}

	printf("0x%02x ", value_read);
}

void print_bytes_halfword(uint64_t offset, uint16_t value_read)
{
	static uint32_t i;

	if ((offset%8) == 0) {
		printf("\n");
		printf("\r 0x%08x :: ", i*16);
		i++;
	}

	printf("0x%04x ", value_read);
}

void print_bytes_word(uint64_t offset, uint32_t value_read)
{
	static uint32_t i;

	if ((offset%4) == 0) {
		printf("\n");
		printf("\r 0x%08x :: ", i*16);
		i++;
	}

	printf("0x%08x ", value_read);
}

void print_bytes_doubleword(uint64_t offset, uint64_t value_read)
{
	static uint32_t i;

	if ((offset%2) == 0) {
		printf("\n");
		printf("\r 0x%08x :: ", i*16);
		i++;
	}

	printf("0x%016lx ", value_read);
}
