/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/

/**
 * @file dspipc_pthread.h
 * 
 * This header file contains all definitions required for the dev-dspipc
 * 
 * 
 * @author Howard Yang
 * @date   11.OCT.2012
 * 
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

#ifndef __DSPIPC_MUTEX_H__
#define __DSPIPC_MUTEX_H__

#include <linux/mutex.h>
#include <linux/semaphore.h>

typedef struct mutex pthread_mutex_t;

typedef unsigned int pthread_mutexattr_t;

typedef int pthread_attr_t;

typedef struct semaphore pthread_cond_t;

typedef unsigned int pthread_condattr_t;

typedef unsigned int pthread_t;

int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr);

int pthread_mutex_lock(pthread_mutex_t* mutex);

int pthread_mutex_unlock(pthread_mutex_t* mutex);

int pthread_mutex_destroy(pthread_mutex_t* mutex);

int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t* attr);

int pthread_cond_signal(pthread_cond_t* cond);

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);

int pthread_cond_wait_timeout(pthread_cond_t* cond, pthread_mutex_t* mutex, unsigned int milliseconds);

int pthread_cond_destroy(pthread_cond_t* cond);

int pthread_attr_init(pthread_attr_t* attr);

int pthread_attr_setdetachstate(pthread_attr_t* attr, int detachstate);

int pthread_create(pthread_t* thr, const pthread_attr_t* attr, int (*start_routine)(void*), void* arg);

int pthread_cancel(pthread_t thr);

int pthread_should_stop(void);

int pthread_sleep(unsigned int milliseconds);

int pthread_join(pthread_t thr, void** retval);

#endif
