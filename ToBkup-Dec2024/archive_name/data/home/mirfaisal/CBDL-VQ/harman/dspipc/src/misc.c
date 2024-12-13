
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kfifo.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "debug.h"
#include "dev-dspipc.h"
#include "malloc_trc.h"
#include "misc.h"

int getsubopt(char** optionp, const char* const* tokens, char** valuep)
{
        char *endp, *vstart;
        int cnt;

        if (**optionp == '\0')
                return -1;

        /* Find end of next token. */
        endp = strchr(*optionp, ',');
        if (endp == NULL)
                endp = strchr(*optionp, '\0');

        /* Find start of value. */
        vstart = memchr(*optionp, '=', endp - *optionp);
        if (vstart == NULL)
                vstart = endp;

        /* Try to match the characters between *OPTIONP and VSTART against one of the TOKENS. */
        for (cnt = 0; tokens[cnt] != NULL; ++cnt)
                if (memcmp(*optionp, tokens[cnt], vstart - *optionp) == 0
                    && tokens[cnt][vstart - *optionp] == '\0') {
                        /* We found the current option in TOKENS. */
                        *valuep = vstart != endp ? vstart + 1 : NULL;

                        if (*endp != '\0')
                                *endp++ = '\0';
                        *optionp = endp;

                        return cnt;
                }

        /* The current suboption does not match any option. */
        *valuep = *optionp;

        if (*endp != '\0')
                *endp++ = '\0';
        *optionp = endp;

        return -1;
}

struct my_file {
        loff_t curr_pos;
        struct file* filp;
};

int open(const char* fileName, int oflags)
{
        struct my_file* pFile;

        struct file* filp;

        if (fileName == NULL || fileName[0] == 0) {
                return -1;
        }

        filp = filp_open(fileName, oflags, 0);
        if (IS_ERR(filp)) {
                DEBUG_ERROR("%s: file open %s failed!\n", __FUNCTION__, fileName);
                return -1;
        }

        DEBUG_ERROR("%s: file open %s successfully!\n", __FUNCTION__, fileName);
        pFile = (struct my_file*)MALLOC_TRC(sizeof(struct my_file));
        if (pFile == NULL) {
                filp_close(filp, NULL);
                return -1;
        }

        pFile->curr_pos = 0;
        pFile->filp = filp;

        return (int)pFile;
}

loff_t lseek(int fd, loff_t offset, int origin)
{
        mm_segment_t fs;
        loff_t of;
        struct my_file* pFile;

        if (fd == -1 || fd == 0) {
                return 0;
        }

        pFile = (struct my_file*)fd;

        fs = get_fs();
        set_fs(KERNEL_DS);
        of = vfs_llseek(pFile->filp, offset, origin);
        pFile->curr_pos = of;
        set_fs(fs);

        return of;
}

ssize_t read(int fd, char* buf, size_t len)
{
        mm_segment_t fs;
        ssize_t reads;
        struct my_file* pFile;
        loff_t old_pos;

        if (fd == -1 || fd == 0) {
                return 0;
        }

        pFile = (struct my_file*)fd;

        fs = get_fs();
        set_fs(KERNEL_DS);
        old_pos = pFile->curr_pos;
        reads = vfs_read(pFile->filp, (char __user*)buf, len, &pFile->curr_pos);
        DEBUG_ERROR("%s: file read previous pos 0x%08x, curr_pos 0x%08x, reads %d!\n", __FUNCTION__, (unsigned int)old_pos, (unsigned int)pFile->curr_pos, reads);
        set_fs(fs);

        return reads;
}

void close(int fd)
{
        struct my_file* pFile;

        if (fd == -1 || fd == 0) {
                return;
        }

        pFile = (struct my_file*)fd;

        filp_close(pFile->filp, NULL);

        FREE_TRC(pFile);
}

/* not good. */
static char errorString[256];
char* strerror(int errCode)
{
        errorString[0] = 0;
        snprintf(errorString, sizeof(errorString), "%d", errCode);
        return errorString;
}

/* Message Queue Implementation */

struct msg_queue_t {
        unsigned int mask;
        wait_queue_head_t queue;
        volatile int condition;
        struct __kfifo fifo;
        spinlock_t lock;
};

#define MASK_MSGQUEUE 0x55aa

/* implement Message queue */
int MsgQueueInit(int* msg_queue, size_t obj_size, size_t max_num)
{
        int ret;
        struct msg_queue_t* msgQueue;

        if (msg_queue == NULL || obj_size == 0 || max_num == 0) {
                return -1;
        }

        msgQueue = VMALLOC_TRC(sizeof(struct msg_queue_t));
        if (msgQueue == NULL) {
                DEBUG_ERROR("[%s]: vmalloc failed!\n", __FUNCTION__);
                return -1;
        }
        msgQueue->mask = 0;

        init_waitqueue_head(&msgQueue->queue);

        /* kfifo allocate */
        ret = __kfifo_alloc(&msgQueue->fifo, max_num, obj_size, GFP_KERNEL);
        if (ret < 0) {
                goto failed;
        }

        /*initialize spinlock */
        spin_lock_init(&msgQueue->lock);

        msgQueue->mask = MASK_MSGQUEUE;
        msgQueue->condition = 0;

        DEBUG_INFO_LEVEL(LEVEL_VVVV, "msg queue created! obj_size %d, max_num %d, addr 0x%08x\n", obj_size, max_num, (int)msgQueue);
        /* all are OK. */
        *msg_queue = (int)msgQueue;
        return 0;

failed:
        VFREE_TRC(msgQueue);
        DEBUG_ERROR("[%s]: failed!\n", __FUNCTION__);
        return ret;
}

/* It will be blocked. Should only be used in thread. */
int MsgReceive(int msg_queue, void* buf, size_t num, unsigned int msecs)
{
        int ret;
        unsigned long flags;
        unsigned long jiffs;
        struct msg_queue_t* msgQueue;

        if (msg_queue == 0 || buf == NULL || num == 0) {
                return -1;
        }

        msgQueue = (struct msg_queue_t*)msg_queue;

        if (msgQueue->mask != MASK_MSGQUEUE) {
                return -1;
        }

        if (msecs == INFINITE) {
                ret = wait_event_interruptible(msgQueue->queue, msgQueue->condition);
                if (ret >= 0) {
                        ret = 1;
                }
        } else {
                jiffs = msecs_to_jiffies(msecs);
                /* our condition is always true. */
                ret = wait_event_interruptible_timeout(msgQueue->queue, msgQueue->condition, jiffs);
        }

        if (ret == 0) {
                /* timeout  */
                DEBUG_ERROR("[%s]: timeout!\n", __FUNCTION__);
                ret = -ETIME;
        } else if (ret > 0) {
                /*  */
                spin_lock_irqsave(&msgQueue->lock, flags);
                ret = __kfifo_out(&msgQueue->fifo, buf, num);
                msgQueue->condition -= ret;
                spin_unlock_irqrestore(&msgQueue->lock, flags);
        }

        return ret;
}

/* can be used in interrupt */
int MsgPut(int msg_queue, void* buf, size_t num)
{
        int ret;
        unsigned long flags;
        struct msg_queue_t* msgQueue;

        if (msg_queue == 0 || buf == NULL || num == 0) {
                return -1;
        }

        msgQueue = (struct msg_queue_t*)msg_queue;

        if (msgQueue->mask != MASK_MSGQUEUE) {
                return -1;
        }

        /* return from the queue. read from fifo. */
        spin_lock_irqsave(&msgQueue->lock, flags);
        ret = __kfifo_in(&msgQueue->fifo, buf, num);
        spin_unlock_irqrestore(&msgQueue->lock, flags);

        if (ret > 0) {

                spin_lock_irqsave(&msgQueue->lock, flags);
                msgQueue->condition += ret;
                spin_unlock_irqrestore(&msgQueue->lock, flags);

                /* send signal to thread */
                wake_up_interruptible(&msgQueue->queue);

        } else if (ret == 0) {
                DEBUG_INFO_LEVEL(LEVEL_VV, "[%s]: fifo is full! add failed!\n", __FUNCTION__);
        }

        return ret;
}

size_t MsgGetSize(int msg_queue)
{
        size_t ret;
        unsigned long flags;
        struct msg_queue_t* msgQueue;

        if (msg_queue == 0) {
                return 0;
        }

        msgQueue = (struct msg_queue_t*)msg_queue;

        if (msgQueue->mask != MASK_MSGQUEUE) {
                return 0;
        }

        /* return from the queue. read from fifo. */
        spin_lock_irqsave(&msgQueue->lock, flags);
        ret = msgQueue->fifo.in - msgQueue->fifo.out;
        spin_unlock_irqrestore(&msgQueue->lock, flags);

        return ret;
}

int MsgDeinit(int msg_queue)
{
        struct msg_queue_t* msgQueue;

        if (msg_queue == 0) {
                return -1;
        }

        msgQueue = (struct msg_queue_t*)msg_queue;

        if (msgQueue->mask != MASK_MSGQUEUE) {
                return -1;
        }

        __kfifo_free(&msgQueue->fifo);

        VFREE_TRC(msgQueue);

        msgQueue->mask = 0;

        return 0;
}
