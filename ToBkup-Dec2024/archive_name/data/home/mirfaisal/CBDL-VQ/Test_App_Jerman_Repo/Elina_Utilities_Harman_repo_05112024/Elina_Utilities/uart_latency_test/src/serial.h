/*****************************************************************************
 * Project              HARMAN connected Car Systems
 *
 * c) copyright         2020
 * Company              Harman International Industries, Incorporated
 *                      All rights reserved
 * Secrecy Level        STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file                serial.h
 * @author              Mir Faisal <Mir.Faisal@harman.com>
 * @ingroup             Kernel and Drivers
 *
 * This header file contains the declarations of serial read/write APIs.
 */
#ifndef SERIAL_H
#define SERIAL_H
/*****************************************************************************
 * Includes
 *****************************************************************************/
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

/*****************************************************************************
 * Defines
 *****************************************************************************/
#define DBG(M, ...)         fprintf(stderr, "%s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define clean_errno()       (errno == 0 ? "None" : strerror(errno))
#define log_err(M, ...)      fprintf(stderr, "%s:%d: errno: %s " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)


/*****************************************************************************
 * Function Declarations
 *****************************************************************************/
int serial_open(const char *port, int baud, struct termios *opts);
int serial_close(int fd, struct termios *opts);
int serial_set_xmit_fifo_size(int fd, int size);
int serial_get_xmit_fifo_size(int fd);
int serial_set_low_latency(int fd);

ssize_t  serial_writebyte(int fd, uint8_t byte);
ssize_t  serial_write(int fd, const uint8_t *buf, size_t len);
ssize_t  serial_read(int fd, uint8_t *buf, size_t len);

#endif // SERIAL_H
