/*****************************************************************************
 * Project              HARMAN connected Car Systems
 *
 * c) copyright         2020
 * Company              Harman International Industries, Incorporated
 *                      All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                serial.c
 * @author              Mir Faisal <Mir.Faisal@harman.com>
 * @ingroup             Kernel and Drivers
 *
 * This file contains API definitions to perform serial read/write.
 */

/*****************************************************************************
 * Includes
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <linux/ioctl.h>
#include <asm/ioctls.h>

#include "serial.h"

/*****************************************************************************
 * Function or API defines
 *****************************************************************************/
ssize_t serial_writebyte(int fd, uint8_t byte)
{
    return serial_write(fd, &byte, 1);
}

ssize_t serial_write(int fd, const uint8_t *buf, size_t len)
{
    ssize_t n = write(fd, buf, len);

    if (n != len) {
        DBG("len=%ld, n=%ld, error=%d %s", len, n, errno, strerror(errno));
    }

    if (n != len) return -1;

    return n;
}

ssize_t serial_read(int fd, uint8_t *buf, size_t len)
{
    ssize_t count = 0;
    int r;
    int retry = 0;

    /*
    if (len > sizeof(buf) || len < 1) {
        fprintf(stderr, "serial_read() len=%d sizeof(buf)=%ld\n", len, sizeof(buf));
        return -1;
        }*/

    while (count < len) {
        r = read(fd, buf + count, len - count);
        //printf("read, r = %d\n", r);
        if (r < 0 && errno != EAGAIN && errno != EINTR) return -1;
        else if (r > 0) count += r;
        else {
            // no data available right now, must wait
            fd_set fds;
            struct timeval t;
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            t.tv_sec = 1;
            t.tv_usec = 0;
            r = select(fd+1, &fds, NULL, NULL, &t);
            // printf("select, r = %d\n", r);
            if (r < 0) return -1;
            if (r == 0) return count; // timeout
        }
        retry++;
        if (retry > 10000) return -100; // no input
    }

    if (count != len) {
        DBG("len=%ld, n=%ld, error=%d %s", len, count, errno, strerror(errno));
    }

    return count;
}

int serial_open(const char *port, int baud, struct termios *opts)
{
    int fd;

    DBG("opening port %s @ %d bps", port, baud);

    if (!strlen(port)) {
        log_err("invalid port name %s", port);
        return 0;
    }

    //fd = open(port, O_RDWR | O_NOCTTY);
    fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd == -1)  {
        log_err("open() failed");
        return 0;
    }

    if (tcgetattr(fd, opts) < 0) {
        log_err("tcgetattr() failed");
        return 0;
    }

    struct termios toptions;

    if (tcgetattr(fd, &toptions) < 0) {
        log_err("tcgetattr()");
        return 0;
    }

    speed_t brate = baud; // let you override switch below if needed

    switch(baud) {
        /* ## concatenate two strings in c preprocessor */
#define B(x) case x: brate=B##x; break;
        B(50);		B(75);		B(110);		B(134);		B(150);
        B(200);		B(300);		B(600);		B(1200);	B(1800);
        B(2400);	B(4800);	B(9600);	B(19200);	B(38400);
        B(57600);	B(115200);	B(230400);	B(460800);      B(500000);
        B(576000);      B(921600);      B(1000000);     B(1152000);     B(1500000);
	B(2000000);     B(2500000);     B(3000000);     B(3500000);     B(4000000);
#undef B

    default:
        log_err("unknown baud rate %d", baud);
        return 0;
        break;
    }

    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;

    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    // turn off s/w flow ctrl
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY);

    // turn on READ & ignore ctrl lines
    toptions.c_cflag |= CREAD | CLOCAL;

    // make raw
    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    toptions.c_oflag &= ~OPOST;

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]	 = 0;
    toptions.c_cc[VTIME] = 10;

    if (tcsetattr(fd, TCSANOW, &toptions) < 0) {
        log_err("tcsetattr() failed");
        return 0;
    }

    int flags = fcntl(fd, F_GETFL);
    if (fcntl(fd, F_SETFL, flags & (~O_NONBLOCK))) {
        log_err("fcntl() failed");
        return 0;
    }

    if (tcflush(fd, TCIOFLUSH) < 0) {
        log_err("tcflush() failed");
        return 0;
    }

    return fd;
}

int serial_close(int fd, struct termios *opts)
{
    if (tcsetattr(fd, TCSANOW, opts) < 0) {
        log_err("tcsetattr() failed");
    }

    return close(fd);
}

int serial_set_xmit_fifo_size(int fd, int size) {
    struct serial_struct ser_info;

    if (ioctl(fd, TIOCGSERIAL, &ser_info) < 0) {
        log_err("ioctl(TIOCGSERIAL) failed");
        return -1;
    }

    DBG("xmit_fifo_size was %d\n", ser_info.xmit_fifo_size);
    ser_info.xmit_fifo_size = size;

    if (ioctl(fd, TIOCSSERIAL, &ser_info) < 0) {
        log_err("ioctl(TIOCSSERIAL) failed");
        return -1;
    }

    return 0;
}

int serial_get_xmit_fifo_size(int fd) {
    struct serial_struct ser_info;

    if (ioctl(fd, TIOCGSERIAL, &ser_info) < 0) {
        log_err("ioctl(TIOCGSERIAL) failed");
        return -1;
    }

    return ser_info.xmit_fifo_size;
}
