/*****************************************************************************
 * Project              HARMAN Connected Car System
 * (c) copyright        2021
 * Company              Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                uart_test.c
 * @ingroup             Kernel and Drivers
 * @author              Prasad Lavande <Prasad.Lavande@harman.com>
 *
 * This file contains the uart test application main code.
 */

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <termios.h>
#include <assert.h>
#include <stdbool.h>

#include "log.h"

#ifdef	QNX_ENABLED
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/slog2.h>
#endif

#ifdef  DLT_LOGGING
#include <dlt/dlt.h>
#endif

#include "parsing.h"
#include "uart_test.h"
#include "baud_setting.h"


/*****************************************************************************
 * DEFINES
 *****************************************************************************/

#define	WRITE_WAIT_PERIOD_USECS         (200*USEC_PER_MSEC)


/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

static void dump_buffer(uint8_t* buffer, unsigned int len, const char *func);
static int read_from_serial(int fd, uint8_t* buf, unsigned int len);
static int write_to_serial(int fd, uint8_t* buf, unsigned int len);
static void * read_thread(void *args);
static void * read_fxn(void *args);
static void * write_thread(void *args);
static void * write_fxn(void *args);
static int set_std_baud_rate(args_t *argsp, speed_t com_speed);
static int set_baud_rate(args_t *argsp);
static int configure_uart(args_t * argsp);
static void cleanup_handler(void);

/**
 * This function converts time in microseconds to struct timespec.
 *
 * @param  usec         time in microsceonds.
 * @param  ptm          time in struct timespec.
 * @return None
 */
static inline void usec_to_timespec(unsigned int usec, struct timespec *ptm)
{
	assert(ptm != NULL);
	ptm->tv_sec = (time_t)usec_to_sec(usec);
	ptm->tv_nsec = usec_to_nsec(usec - sec_to_usec(ptm->tv_sec));
	return;
}

/**
 * This function converts the communication speed to corresponding macro
 *
 * @param  speed        value to be converted
 * @param  com_speed    standard macro corresponding to 'speed' if
 *                      'speed' is a standard OS baud rate otherwise
 *                      set to 'speed'.
 * @return EOK          if standard OS baud rate otherwise -1.
 */
static inline int convert_com_speed(unsigned int speed, speed_t *com_speed)
{
	int ret = EOK;

	assert(com_speed != NULL);

	switch (speed) {
	case 1200 :
		*com_speed = B1200;
		break;
	case 1800 :
		*com_speed = B1800;
		break;
	case 2400 :
		*com_speed = B2400;
		break;
	case 4800 :
		*com_speed = B4800;
		break;
	case 9600 :
		*com_speed = B9600;
		break;
	case 19200 :
		*com_speed = B19200;
		break;
	case 38400 :
		*com_speed = B38400;
		break;
	case 57600 :
		*com_speed = B57600;
		break;
	case 115200 :
		*com_speed = B115200;
		break;
		/* QNX supports Pre-Defined Baud rates upto B115200 */
#ifndef	QNX_ENABLED
	case 230400 :
		*com_speed = B230400;
		break;
	case 460800 :
		*com_speed = B460800;
		break;
	case 500000 :
		*com_speed = B500000;
		break;
	case 576000 :
		*com_speed = B576000;
		break;
	case 921600 :
		*com_speed = B921600;
		break;
	case 1000000 :
		*com_speed = B1000000;
		break;
	case 1152000 :
		*com_speed = B1152000;
		break;
	case 1500000 :
		*com_speed = B1500000;
		break;
	case 2000000 :
		*com_speed = B2000000;
		break;
	case 2500000 :
		*com_speed = B2500000;
		break;
	case 3000000 :
		*com_speed = B3000000;
		break;
	case 3500000 :
		*com_speed = B3500000;
		break;
	case 4000000 :
		*com_speed = B4000000;
		break;
#endif
	default:
		*com_speed = (speed_t)speed;
		ret = -1;
	}

	return ret;
}


/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

logging_t log_level = TRACE_LEVEL_INFO;
static pthread_t read_thread_t;
static args_t args;
static bool thread_should_exit = false;

#ifdef  QNX_SLOG2_LOGGING
slog2_buffer_set_config_t slog_config;
slog2_buffer_t slog_handle;
extern char *__progname;
#endif

#ifdef  DLT_LOGGING
DLT_DECLARE_CONTEXT(dlt_ctx);
#endif


/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/

/**
 * This function dumps the buffer data
 *
 * @param  buffer - pointer to data
 * @param  len - data length
 * @param  func - source function name
 * @return none
 */
static void dump_buffer(uint8_t* buffer, unsigned int len, const char *func)
{
#define	NUM_ELEM                        (8)
#define	WIDTH_PER_ELEM                  (5)
	unsigned int index = 0;
	unsigned int elem = 0;
	char buf[(NUM_ELEM - 1) * WIDTH_PER_ELEM + 1] = {0};

	assert((buffer != NULL) && (func != NULL));

	if (log_level >= TRACE_LEVEL_NOTICE) {
		log_notice("%s (Len=%d) -->", func, len);
		for (index = 0 ; (index + NUM_ELEM - 1) < len; index += NUM_ELEM) {
			log_notice("--> 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x"
				" 0x%02x 0x%02x", (unsigned int)buffer[index],
				(unsigned int)buffer[index + 1],
				(unsigned int)buffer[index + 2],
				(unsigned int)buffer[index + 3],
				(unsigned int)buffer[index + 4],
				(unsigned int)buffer[index + 5],
				(unsigned int)buffer[index + 6],
				(unsigned int)buffer[index + 7]);
		}
		for (elem = 0; (len - index) > 0 && (elem < sizeof(buf)); index++,
			elem += WIDTH_PER_ELEM) {
			snprintf(&buf[elem], WIDTH_PER_ELEM + 1, " 0x%02x",
				(unsigned int)buffer[index]);
		}
		if (elem != 0)
			log_notice("-->%s", (char *)buf);
	}
}

/**
 * Read data from the serial port
 *
 * @param  fd - file descriptor to read from
 * @param  buf - pointer to buffer to store the read data
 * @param  len - no of bytes to read
 * @return no. of bytes read on success, < 0 on failure
 */
static int read_from_serial(int fd, uint8_t* buf, unsigned int len)
{
	int ret = 0;

	assert(buf != NULL);

	ret = (int)read(fd, buf, len);
	if (ret > 0) {
		log_debug("%s: Read %d bytes", __func__, ret);
		return ret;
	}

	/* an error accured */
	if (ret == 0)
		errno = EAGAIN;
	log_error("%s: Read from fd = %d failed(ret: %d): %s", __func__, fd, ret,
		strerror(errno));

	return -1;
}

/**
 * Read Thread to read data from the serial port
 *
 * @param  args - pointer to thread argument
 * @return NULL
 */
static void * read_thread(void *args)
{
	int ret=0;
	unsigned long count = 1;
	args_t *argsp = (args_t *)args;

	assert((argsp != NULL) && (argsp->rbuf != NULL));

	while (thread_should_exit == false) {
		memset((void *)(argsp->rbuf), 0, argsp->len);
		ret = read_from_serial(argsp->fd, (void *)argsp->rbuf, argsp->len);
		if (ret < 0) {
			log_error("%s: Error in %lu Xfer(ret: %d): %s", __func__,
				count, ret, strerror(errno));
			thread_should_exit = true;
			break;
		}
		log_notice("%lu UART Read completed: transferred %d bytes",
			count, ret);
		dump_buffer(argsp->rbuf, ret, "Read Buffer");
		log_notice("\n");
		count++;
	}

	log_notice("%s: Exiting.", __func__);

	return (void *)NULL;
}

/**
 * Read Function to read data from the serial port
 *
 * @param  args - pointer to argument
 * @return NULL
 */
static void * read_fxn(void *args)
{
	int ret=0;
	unsigned int count = 1;
	args_t *argsp = (args_t *)args;

	assert((argsp != NULL) && (argsp->rbuf != NULL));

	for (count = 1; count <= argsp->packets; count++) {
		ret = read_from_serial(argsp->fd, (void *)argsp->rbuf, argsp->len);
		if (ret < 0) {
			log_error("Error in %u Xfer(ret: %d): %s", count, ret,
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		log_notice("%u UART read completed: transferred %d bytes ",
			count, argsp->len);
		dump_buffer(argsp->rbuf, argsp->len, "Buffer");
	}

	return (void *)NULL;
}

/**
 * Write data to the serial port
 *
 * @param  fd - file descriptor to write to
 * @param  buf - pointer to buffer to write the data
 * @param  len - no of bytes to write
 * @return EOK on success and -1 on failure
 */
static int write_to_serial(int fd, uint8_t* buf, unsigned int len)
{
	int ret = EOK;
	unsigned int bytes_written = 0;

	assert(buf != NULL);

	while (bytes_written < len) {
		ret = (int)write(fd, (void *)&buf[bytes_written],
			len-bytes_written);
		if (ret <= 0) {
			/* an error accured */
			if (ret == 0)
				errno = EAGAIN;
			log_error("%s: Write to fd = %d failed(ret: %d): %s",
				__func__, fd, ret, strerror(errno));
			return -1;
		}
		bytes_written += (unsigned int)ret;
		log_debug("%s: Wrote %d(total: %u) bytes", __func__, ret,
			bytes_written);
	}
	return EOK;
}

/**
 * Write Thread to write data to the serial port
 *
 * @param  args - pointer to thread argument
 * @return NULL
 */
static void * write_thread(void *args)
{
	int ret=0;
	uint8_t* wbuf;
	unsigned int len;
	unsigned int index = 0;
	unsigned long count = 1;
	struct timespec req, rem;
	args_t *argsp = (args_t *)args;

	assert((argsp != NULL) && (argsp->wbuf != NULL));

	len = argsp->len;
	usec_to_timespec(argsp->delay, &req);
	log_debug2("%s: Waiting time : %ld secs and %ld nsecs", __func__,
		(long)req.tv_sec, req.tv_nsec);

	while (thread_should_exit == false) {
		wbuf = argsp->wbuf + index * len;
		ret = write_to_serial(argsp->fd, (void *)wbuf, len);
		if (ret < 0) {
			log_error("%s: Error in %lu Xfer(ret: %d): %s", __func__,
				count, ret, strerror(errno));
			thread_should_exit = true;
			break;
		}
		log_notice("%lu UART Write completed: transferred %d bytes",
			count, len);
		dump_buffer(wbuf, len, "Write Buffer");
		count++;
		index = (index + 1) % argsp->packets;
		ret = nanosleep(&req, &rem);
		if (ret != 0) {
			log_warning("%s: Sleeping for %ld secs & %ld nsecs "
				"failed(rem: %ld secs & %ld nsecs)(ret: %d): %s",
				__func__, (long)(req.tv_sec), req.tv_nsec,
				(long)(rem.tv_sec), rem.tv_nsec, ret,
				strerror(errno));
		}
	}

	log_notice("%s: Exiting.", __func__);

	return (void *)NULL;
}

/**
 * Write Function to write data to the serial port
 *
 * @param  args - pointer to argument
 * @return NULL
 */
static void * write_fxn(void *args)
{
	int ret=0;
	uint8_t* wbuf;
	unsigned int len;
	unsigned int count = 1;
	struct timespec req, rem;
	args_t *argsp = (args_t *)args;

	assert((argsp != NULL) && (argsp->wbuf != NULL));

	len = argsp->len;
	usec_to_timespec(argsp->delay, &req);
	log_debug2("%s: Waiting time : %ld secs and %ld nsecs", __func__,
		(long)req.tv_sec, req.tv_nsec);

	for (count = 1, wbuf = argsp->wbuf; count <= argsp->packets; count++,
		wbuf += len) {
		ret = write_to_serial(argsp->fd, (void *)wbuf, len);
		if (ret < 0) {
			log_error("Error in %u Xfer(ret: %d): %s", count, ret,
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		log_notice("%u UART write completed: transferred %d bytes", count,
			len);
		dump_buffer(wbuf, len, "Buffer");
		ret = nanosleep(&req, &rem);
		if (ret != 0) {
			log_warning("%s: Sleeping for %ld secs & %ld nsecs "
				"failed(rem: %ld secs & %ld nsecs)(ret: %d): %s",
				__func__, (long)(req.tv_sec), req.tv_nsec,
				(long)(rem.tv_sec), rem.tv_nsec, ret,
				strerror(errno));
		}
	}

	return (void *)NULL;
}


/**
 * Set the standard baud rate
 *
 * @param argsp - pointer to argument structure
 * @param com_speed - baud rate to be set
 * @return EOK on success, < 0 on failure
 */
static int set_std_baud_rate(args_t *argsp, speed_t com_speed)
{
	int ret;
	struct termios options;

	assert(argsp != NULL);

	/* get the current settings of the serial port */
	if ((ret = tcgetattr(argsp->fd, &options)) < 0) {
		log_error("tcgetattr failed(ret: %d): %s", ret, strerror(errno));
		return ret;
	}
	if ((ret = cfsetispeed(&options, com_speed)) < 0) {
		log_error("cfsetispeed failed(ret: %d): %s", ret, strerror(errno));
		return ret;
	}
	if ((ret = cfsetospeed(&options, com_speed)) < 0) {
		log_error("cfsetospeed failed(ret: %d): %s", ret, strerror(errno));
		return ret;
	}
	/* set the new attributes */
	if ((ret = tcsetattr(argsp->fd, TCSANOW, &options)) < 0) {
		log_error("tcsetattr failed(ret: %d): %s", ret, strerror(errno));
		return ret;
	}

	return EOK;
}

/**
 * Set the baud rate
 *
 * @param argsp - pointer to argument structure
 * @return EOK on success, < 0 on failure
 */
static int set_baud_rate(args_t *argsp)
{
	int ret;
	speed_t com_speed;
	int std_baud_rate;

	assert(argsp != NULL);

	std_baud_rate = convert_com_speed(argsp->speed, &com_speed);
	log_info("Communication Speed '%d'(%d)", argsp->speed, com_speed);

	if (std_baud_rate == EOK) {
		ret = set_std_baud_rate(argsp, com_speed);
		if (ret < 0) {
			log_error("Setting Standard Baud Rate failed");
			return ret;
		}
	} else {
		log_info("Requested baud rate is not among OS supported standard"
			" values. Proceeding with the requested baud rate.");
#ifdef	QNX_ENABLED
		ret = set_std_baud_rate(argsp, com_speed);
		if (ret < 0) {
			log_error("Setting Custom Baud Rate failed");
			return ret;
		}
#else
		ret = set_custom_baud_rate(argsp);
		if (ret < 0) {
			log_error("Setting Custom Baud Rate failed as %s failed"
				"(ret: %d): %s", (ret == -1) ? "TCGETS2" :
				"TCSETS2", ret, strerror(errno));
			return ret;
		}
#endif	//#ifdef	QNX_ENABLED
	}

	return EOK;
}

/**
 * Configure the serial port settings
 *
 * @param argsp - pointer to argument structure
 * @return EOK on success, < 0 on failure
 */
static int configure_uart(args_t * argsp)
{
	struct termios options;
	int ret;

	assert(argsp != NULL);

	/* get the current settings of the serial port */
	if ((ret = tcgetattr(argsp->fd, &options)) < 0) {
		log_error("tcgetattr failed(ret: %d): %s", ret, strerror(errno));
		return ret;
	}

	/* enable the receiver and set local mode */
	options.c_cflag |= (CLOCAL | CREAD);

	/* disable hardware flow control */
#if	defined(QNX_ENABLED)
	options.c_cflag &= ~(IHFLOW | OHFLOW);
#else
	options.c_cflag &= ~CRTSCTS;
#endif

	/* 8bits,no parity,1 stopbit */
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag &= ~(CSTOPB | PARENB | PARODD);

	/* enable raw mode (disable canonical mode) */
	options.c_lflag &= ~(ICANON | IEXTEN | ECHO | ECHOE | ECHOK | ECHOKE |
		ECHONL| ISIG);
	options.c_oflag &= ~OPOST;
	options.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR | INPCK |
		BRKINT | PARMRK | ISTRIP | IUCLC | IMAXBEL);
	options.c_iflag |= IGNBRK | IGNPAR | IGNCR;

	/*
	 * Blocking read until 1 Byte.
	 * Vmin/Vtime settings will be overwritten by O_NONBLOCK setting.
	 */
	options.c_cc[VMIN] = 1;

	/* intercharacter timer unused */
	options.c_cc[VTIME] = 0;

	/* set the new attributes */
	if ((ret = tcsetattr(argsp->fd, TCSANOW, &options)) < 0) {
		log_error("tcsetattr failed(ret: %d): %s", ret, strerror(errno));
		return ret;
	}

	/* set the baud rate */
	ret = set_baud_rate(argsp);
	if (ret < 0) {
		log_error("Setting Baud Rate failed");
		return ret;
	}

	return EOK;
}

#if	defined(QNX_ENABLED)
static int qnx_init(void)
{
	int ret;
#ifdef  QNX_SLOG2_LOGGING
	slog_config.buffer_set_name = __progname;
	slog_config.verbosity_level = log_level;
	slog_config.num_buffers = 1;
	slog_config.buffer_config[0].num_pages = 34; /* number of 4 KB pages */
	if (slog2_register(&slog_config, &slog_handle, 0) != 0) {
		fprintf(stderr, "Error registering slogger2 buffer: %s",
			strerror(errno));
		return -1;
	}
#endif  //#ifdef  QNX_SLOG2_LOGGING

	if ((ret=ThreadCtl_r(_NTO_TCTL_IO, (void *)NULL)) != EOK) {
		log_error("ThreadCtl failed: %s", strerror(ret));
		return -1;
	}
	return EOK;
}
#endif	//#if	defined(QNX_ENABLED)

/**
 * Cleanup handler
 *
 * @param None.
 * @return None.
 */
static void cleanup_handler(void)
{
	if ((args.fd >= 0) && (close(args.fd) != 0)) {
		log_warning("Closing UART device node(%s, fd: %d) failed: %s",
			args.name, args.fd, strerror(errno));
	}
	if (args.rbuf != NULL)
		free(args.rbuf);
	if (args.wbuf != NULL)
		free(args.wbuf);
	if (args.name != NULL)
		free(args.name);

#ifdef  DLT_LOGGING
	DLT_UNREGISTER_CONTEXT(dlt_ctx);
	DLT_UNREGISTER_APP();
#endif

	return;
}

/**
 * This is the main function. It does all cofigurations & initializations.
 *
 * @param  argc - number of parameters
 * @param  argv - pointer to array of parameters
 * @return EXIT_SUCCESS - On Success, EXIT_FAILURE - On failure
 */
int main(int argc, char*argv[])
{
	int ret = 0;
	int flags = -1;
	void *(*fxn)(void *);

#if	defined(QNX_ENABLED)
	if (qnx_init() != EOK)
		exit(EXIT_FAILURE);
#endif	//#if	defined(QNX_ENABLED)

	opt_init(&args);

	if (opt_parse(argc, argv, &args) != EOK) {
		log_error("Parsing of options failed");
		exit(EXIT_FAILURE);
	}

	opt_print(&args);

#ifdef  DLT_LOGGING
	DLT_REGISTER_APP("UT", "Testing utility for uart devices");
	DLT_REGISTER_CONTEXT(dlt_ctx, "UTX", "Global Context");
#endif

	ret = atexit(cleanup_handler);
	if (ret != 0) {
		log_error("Failed to install exit handler with error: %s",
			strerror(ret));
		exit(EXIT_FAILURE);
	}

	/*
	 * O_NDELAY : On some systems, this causes the RS232 DCD signal line to
	 * be ignored. But, all I/O operations will become non-blocking.
	 */
	args.fd = open(args.name, O_RDWR | O_NDELAY | O_NOCTTY);
	if (args.fd < 0) {
		log_error("Opening UART device node(%s) failed(ret: %d): %s",
			args.name, args.fd, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Make sure device is of tty type */
	if (isatty(args.fd) == 0) {
		log_error("%s(fd: %d) is not a tty device", args.name, args.fd);
		exit(EXIT_FAILURE);
	}

	/* Make I/O operations blocking. */
	flags = fcntl(args.fd, F_GETFL);
	if (flags == -1) {
		log_error("fcntl(F_GETFL) failed with error: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	log_debug("Default file status : 0x%x", flags);

	if (fcntl(args.fd, F_SETFL, 0) == -1) {
		log_error("fcntl(F_SETFL) failed with error: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	flags = fcntl(args.fd, F_GETFL);
	if (flags == -1) {
		log_error("fcntl(F_GETFL) failed with error: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if ((flags & O_NDELAY) || (flags & O_NONBLOCK)) {
		log_error("File status update failed, status: 0x%x", flags);
		exit(EXIT_FAILURE);
	}

	log_info("Serial interface '%s' opened, fd = %d", args.name, args.fd);

	log_debug("Configuring UART settings");
	if (configure_uart(&args) < 0) {
		log_error("Configuring UART failed");
		exit(EXIT_FAILURE);
	}

	switch(args.mode) {
	case RW :
		args.packets = 256;
		args.rbuf = (uint8_t *)malloc(args.len);
		if (args.rbuf == (uint8_t *)NULL) {
			log_error("Error allocating memory for rbuf: %s",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		args.wbuf = (uint8_t *)malloc(args.packets*args.len);
		if (args.wbuf == (uint8_t *)NULL) {
			log_error("Error allocating memory for wbuf: %s",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		if ((ret= wbuffer_init(args.wbuf, args.packets, args.len)) !=
			EOK) {
			log_error("Error initializing wbuf: %d", ret);
			exit(EXIT_FAILURE);
		}
		log_notice("########## Performing Read-Write Test ##########");
		if ((ret = pthread_create(&read_thread_t, NULL, read_thread,
			(void *)&args)) != EOK) {
			log_error("Failed to create Reader Thread with error: %s",
				strerror(ret));
			exit(EXIT_FAILURE);
		}
		write_thread(&args);
#if	!defined(ANDROID_ENABLED)
		if ((ret = pthread_cancel(read_thread_t)) != EOK) {
			log_warning("Failed to cancel Reader Thread(probably "
				"exited already) with error: %s", strerror(ret));
		}
		if ((ret = pthread_join(read_thread_t, (void **)NULL)) != EOK) {
			log_error("Failed to join Reader Thread with error: %s",
				strerror(ret));
		}
#endif	//#if	!defined(ANDROID_ENABLED)
		break;

	case RD:
		log_notice("########## Performing Read Test ##########");
		if (args.packets == 0) {
			fxn = read_thread;
		} else {
			fxn = read_fxn;
		}
		args.rbuf = (uint8_t *)malloc(args.len);
		if (args.rbuf == (uint8_t *)NULL) {
			log_error("Error allocating memory for rbuf: %s",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		fxn(&args);
		break;

	case WR:
		log_notice("########## Performing Write Test ##########");
		if (args.packets == 0) {
			args.packets = 256;
			fxn = write_thread;
		} else {
			fxn = write_fxn;
		}
		args.wbuf = (uint8_t *)malloc(args.packets*args.len);
		if (args.wbuf == (uint8_t *)NULL) {
			log_error("Error allocating memory for wbuf: %s",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		if ((ret= wbuffer_init(args.wbuf, args.packets, args.len))
			!= EOK) {
			log_error("Error initializing wbuf: %d", ret);
			exit(EXIT_FAILURE);
		}
		fxn(&args);
		break;

	default:
		log_error("Wrong Input Mode(%d)..should be %d, %d or %d",
			args.mode, RD, WR, RW);
	}

	return EXIT_SUCCESS;
}
