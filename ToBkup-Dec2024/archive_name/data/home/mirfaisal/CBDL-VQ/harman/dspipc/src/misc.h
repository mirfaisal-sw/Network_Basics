#ifndef __MISC_H__
#define __MISC_H__

#include <asm/string.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/string.h>
#include <linux/types.h>

#define EOK 0

#define ENDIAN_LE16(__x) (__x)
#define ENDIAN_LE32(__x) (__x)
#define ENDIAN_LE64(__x) (__x)

#define PCI_IS_IO(address) (((address)&1) && 1)
#define PCI_IS_MEM(address) (!PCI_IS_IO(address))
#define PCI_IO_ADDR(address) ((address) & ~0x3)
#define PCI_MEM_ADDR(address) ((address) & ~0xf)
#define PCI_ROM_ADDR(address) ((address) & ~1)
#define PCI_IS_MMAP20(address) ((address)&0x2)
#define PCI_IS_MMAP32(address) (!((address)&0x6))
#define PCI_IS_MMAP64(address) ((address)&0x4)
#define PCI_IS_PREFETCH(address) ((address)&0x8)
#define PCI_IS_MULTIFUNC(address) ((address)&0x80)
#define PCI_DEVNO(address) ((address) >> 3)
#define PCI_FUNCNO(address) ((address)&7)
#define PCI_DEVFUNC(dev, func) ((dev << 3) | (func))
#define PCI_CLASS(class) (((class) & 0xff0000) >> 16)
#define PCI_SUBCLASS(class) (((class) & 0xff00) >> 8)
#define PCI_INTERFACE(class) ((class) & 0xff)

int getsubopt(char** optionp, const char* const* tokens, char** valuep);

#define strtoul simple_strtoul

int open(const char* fileName, int oflags);

loff_t lseek(int fd, loff_t offset, int origin);

ssize_t read(int fd, char* buf, size_t len);

void close(int fd);

char* strerror(int errCode);

#define INFINITE ((unsigned int)-1)

/* implement Message queue */
int MsgQueueInit(int* msg_queue, size_t obj_size, size_t max_num);

/* It will be blocked. Should only be used in thread. */
/* return the number of elements read in fifo. */
int MsgReceive(int msg_queue, void* buf, size_t num, unsigned int msecs);

/* can be used in interrupt */
/* return the number of elements added into fifo. */
int MsgPut(int msg_queue, void* buf, size_t num);

size_t MsgGetSize(int msg_queue);

int MsgDeinit(int msg_queue);

#endif
