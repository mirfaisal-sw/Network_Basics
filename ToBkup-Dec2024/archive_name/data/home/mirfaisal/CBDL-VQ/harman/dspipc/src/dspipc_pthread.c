/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file dspipc_pthread.c
 *
 * This is the compatible implementation for linux POSIX functions
 *
 * @author Howard Yang
 * @date   12.OCT.2012
 *
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "debug.h"
#include "dev-dspipc.h"
#include "dspipc_pthread.h"
#include "malloc_trc.h"

int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr)
{
        if (mutex) {
                mutex_init(mutex);
                return 0;
        }
        return -1;
}

int pthread_mutex_lock(pthread_mutex_t* mutex)
{
        int ret;
        if (mutex) {
                ret = mutex_lock_interruptible(mutex);
                return ret;
        }
        return -1;
}

int pthread_mutex_unlock(pthread_mutex_t* mutex)
{
        if (mutex) {
                mutex_unlock(mutex);
                return 0;
        }
        return -1;
}

int pthread_mutex_destroy(pthread_mutex_t* mutex)
{
        if (mutex) {
                mutex_destroy(mutex);
                return 0;
        }
        return -1;
}

int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t* attr)
{
        if (cond) {
                sema_init(cond, 0);
                return 0;
        }
        return -1;
}

struct semaphore_waiter {
        struct list_head list;
        struct task_struct* task;
        int up;
};

static noinline void __sched __up(struct semaphore* sem)
{
        struct semaphore_waiter* waiter = list_first_entry(&sem->wait_list,
            struct semaphore_waiter, list);
        list_del(&waiter->list);
        waiter->up = 1;
        wake_up_process(waiter->task);
}

/* Since there is no condition variable in linux kernel. I just use semaphore to replace it. 
 * In the other hand, semaphore can't meet the demand of condition variable. 
 * So i just copy __up function from kernel. And make a modification 
 */
int pthread_cond_signal(pthread_cond_t* cond)
{
        if (cond) {
                unsigned long flags;
                spin_lock_irqsave((spinlock_t*)&cond->lock, flags);
                if (!list_empty(&cond->wait_list))
                        __up(cond);
                spin_unlock_irqrestore((spinlock_t*)&cond->lock, flags);
                return 0;
        }
        return -1;
}

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex)
{
        if (mutex) {
                mutex_unlock(mutex);
        }
        if (cond) {
                down(cond);
        }
        if (mutex) {
                mutex_lock(mutex);
        }
        return 0;
}

int pthread_cond_wait_timeout(pthread_cond_t* cond, pthread_mutex_t* mutex, unsigned int milliseconds)
{
        unsigned long jiff_wait;
        int ret = -1, retValue;
        if (mutex) {
                mutex_unlock(mutex);
        }

        jiff_wait = msecs_to_jiffies(milliseconds);
        if (cond) {
                ret = down_timeout(cond, jiff_wait);
        }

        if (mutex) {
                retValue = mutex_lock_interruptible(mutex);
        }

        return ret;
}

int pthread_cond_destroy(pthread_cond_t* cond)
{
        if (cond) {
                /* no need to destroy */
                /*sema_destroy(cond); */
                return 0;
        }
        return -1;
}

int pthread_attr_init(pthread_attr_t* attr)
{
        return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t* attr, int detachstate)
{
        return 0;
}

int pthread_create(pthread_t* thr, const pthread_attr_t* attr, int (*start_routine)(void*), void* arg)
{
        deviceTable_t* pDevice;
        char kthread_name[DEV_NAME_LENGTH];
        int cnt = 0;
        struct task_struct* task;
        if (thr == NULL || start_routine == NULL) {
                return -1;
        }

        *thr = 0;
        pDevice = (deviceTable_t*)arg;
        if (strcmp(pDevice->pDeviceName, "dsp") == 0) {
                strncpy(kthread_name, "IPC_DSP", DEV_NAME_LENGTH);
        } else if (strcmp(pDevice->pDeviceName, "m3") == 0) {
                strncpy(kthread_name, "IPC_CortexM", DEV_NAME_LENGTH);
        } else {
                strncpy(kthread_name, "IPC_Kernel", DEV_NAME_LENGTH);
        }
        do {
                task = kthread_create(start_routine, arg, kthread_name);
                if (IS_ERR(task)) {
                        printk(KERN_ALERT "Create ipc kernel thread failed, task=%d, cnt=%d\n", (int)task, cnt);
                        if (++cnt > 3)
                                return -1;
                }
        } while (IS_ERR(task));

        wake_up_process(task);

        *thr = (pthread_t)task;

        return 0;
}

int pthread_cancel(pthread_t thr)
{
        return 0;
}

int pthread_should_stop(void)
{
        return kthread_should_stop();
}

int pthread_sleep(unsigned int milliseconds)
{
        signed long waitJiffs;

        waitJiffs = msecs_to_jiffies(milliseconds);
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(waitJiffs);
        return 0;
}

int pthread_join(pthread_t thr, void** retval)
{
        if (thr) {
                kthread_stop((struct task_struct*)thr);
                return 0;
        }
        return -1;
}
