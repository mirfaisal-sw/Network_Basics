#ifndef _REMOTE_LIB
#define _REMOTE_LIB

#include <mqueue.h>
#include <semaphore.h>

mqd_t remote_mq_open(const char*, const char*, int, ...);
int remote_mq_unlink(const char*, const char*);
int remote_pipe(const char*, int*);
sem_t* remote_sem_open(const char*, const char*, int, ...);
int remote_sem_unlink(const char*, const char*);

#endif _REMOTE_LIB
