#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define DEFAULT_RST_TIME 60

#define WATCHDOG_IOCTL_BASE     'W'

#define WDIOC_SETTIMEOUT        _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_SETPRETIMEOUT     _IOWR(WATCHDOG_IOCTL_BASE, 8, int)

static const char default_file[] = "/dev/watchdog";


static void ping_wdog(int fd, char *buf, int size)
{

	write(fd, buf, size);

	if (0 > write(fd, buf, size))
		perror("Error while writing");
}

int main(int argc, char **argv)
{

	const char *file = default_file;
	int rst_timeout = 0, pre_timeout = 0, fd = -1, ret = -1;
	char buf[] = "owl";
	char c[] = "y";

	fd = open(file, O_RDWR);

	if (fd == -1) {
		printf("Failed to open file %s. Exiting.", file);
		exit(errno);
	}

	printf("Enter reset timeout\n");
	scanf("%d", &rst_timeout);
	printf("Enter pre-timeout. Time before reset you need interrupt\n");
	printf(" Enter 0 in case you dont want pre-time interrupt\n");
	scanf("%d", &pre_timeout);

	if (!rst_timeout) {
		printf("Changing reset-tmeout to default value(60 sec)\n");
		rst_timeout = DEFAULT_RST_TIME;
	}

	if (pre_timeout && pre_timeout >= rst_timeout) {
		printf("Adjust pre-timeput to 1 sec\n");
		pre_timeout = 1;
	}

	/*Program the reset -timeout value*/
	printf("pre-timeout %d and reset timeout %d\n",
				pre_timeout, rst_timeout);

	ret = ioctl(fd, WDIOC_SETTIMEOUT, &rst_timeout);

	if (ret < 1)
		perror("Error during set_time");

	if (pre_timeout  < 1) {
		printf("Pre timeout less than 1 sec. Jump to pinging\n");
		goto ping;
	}

	ret = ioctl(fd, WDIOC_SETPRETIMEOUT, &pre_timeout);

	if (ret < 1)
		perror("Error during pre_set_time");
ping:

	ret = write(fd, buf, sizeof(buf));

	if (ret < 1)
		perror("Error while writing");

	while (1) {
		printf("Press y to ping watchdog. Someting else to exit\n");
		scanf("%s", c);

		if (c[0] == 'y')
			ping_wdog(fd, buf, sizeof(buf));
		else {
			printf("Exiting pinging\n");
			break;
		}

	}
}
