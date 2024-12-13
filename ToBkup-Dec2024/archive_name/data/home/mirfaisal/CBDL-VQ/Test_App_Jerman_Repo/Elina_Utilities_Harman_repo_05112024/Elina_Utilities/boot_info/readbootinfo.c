/*****************************************************************************
 * Project              HARMAN Connected Car System
 * (c) copyright        2020 - 2023
 * Company              Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
*****************************************************************************/

/*
 * This test module is to read the Boot-Info from Target
 *
 * Author: Shreeharsha.Indhudhara@harman.com
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define VERSION "1.1"
#define F_NAME	"/proc/bootinfo"
#define LEN	128

int main(int argc, char *argv[])
{
	unsigned int buf[LEN];
	unsigned char *binfo;
	int ret,i;
	int fd = 0;
	int len = 0;
	int opt = 0;

	printf("version: %s\n", VERSION);

	while((opt = getopt(argc, argv, "l:")) != -1) {
		switch(opt) {
			case 'l':
				len = atoi(optarg);
				if (len > LEN) {
					printf("specified length large, defaulting to %d\n", LEN);
					len = LEN;
				}
				break;
			default:
				printf("sample command: readbinfo -l <bytes-to-read>\n");
				return 0;
		}
	}
	if (len == 0) {
		len = LEN;
	}

	memset(buf, 0, sizeof(buf));

	fd = open(F_NAME,O_RDONLY);
	if(!fd) {
		printf("error opening file");
		return -1;
	}

	ret = read(fd, (char*)buf, len);
	printf("\nread %d bytes\n", ret);

	printf("\n\nBootinfo in int format\n");
	for (i=0; i < (len/4); i++) {
		printf("%08X ", buf[i]);
		if (!((i+1) % 4))
			printf("\n");
	}

	binfo = (char*) buf;

	printf("\n\nBootinfo in byte format\n");
	for (i=0; i< len; i++) {
		printf("binfo[%d] = 0x%02X\n", i, binfo[i]);
	}

	return 0;
}

