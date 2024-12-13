#ifndef __MYSYNC_H__
#define __MYSYNC_H__
/*
 *  Coordinate multi-thread/multi-process benchmarks.  Don't want
 *  test loops to begin in the parent whilst a thread or process is
 *  still being created (interfere with lock-step operation or have
 *  process creation time included in the timings).  So use a barrier
 *  in a shared-memory object (for non-QNX6 try to fake it by delaying).
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/neutrino.h>

static int __SYNC_fd = -1;
static pthread_barrier_t* __SYNC_barrier = MAP_FAILED;

void SYNC_INIT(char* name, int self)
{
        pthread_barrierattr_t attr;
        int result;
        char object[48];

        snprintf(object, sizeof(object), "/SYNC_%s", name);
        __SYNC_fd = shm_open(object, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        assert(__SYNC_fd != -1);
        ftruncate(__SYNC_fd, sizeof(pthread_barrier_t));
        __SYNC_barrier = mmap(NULL, sizeof(pthread_barrier_t), PROT_READ | PROT_WRITE, self ? MAP_PRIVATE : MAP_SHARED, __SYNC_fd, 0);
        assert(__SYNC_barrier != MAP_FAILED);
        shm_unlink(object);
        pthread_barrierattr_init(&attr);
        pthread_barrierattr_setpshared(&attr, self ? PTHREAD_PROCESS_PRIVATE : PTHREAD_PROCESS_SHARED);
        result = pthread_barrier_init(__SYNC_barrier, &attr, self ? 1 : 2);
        assert(result == EOK);
        pthread_barrierattr_destroy(&attr);
}
void SYNC_DESTROY(void)
{
        pthread_barrier_destroy(__SYNC_barrier);
        munmap(__SYNC_barrier, sizeof(pthread_barrier_t));
        close(__SYNC_fd);
        __SYNC_fd = -1, __SYNC_barrier = MAP_FAILED;
}
void SYNC_COORDINATE(int first, int affinity)
{
        long cpumask;

        if (affinity != 0) {
                cpumask = (affinity > 0 || first != 0) ? 0x1 : 0x2;
#if defined(__QNXNTO__)
                ThreadCtl(_NTO_TCTL_RUNMASK, (void*)cpumask);
#elif defined(__LINUX__)
                sched_setaffinity(0, sizeof(cpumask), (unsigned long*)&cpumask);
#endif
        }
        pthread_barrier_wait(__SYNC_barrier);
}
void SYNC_PRIORITY(int adjustment, pthread_attr_t* thread)
{
        struct sched_param param;

        if (thread != NULL) {
                param.sched_priority = getprio(0) + adjustment;
                pthread_attr_setschedparam(thread, &param);
                pthread_attr_setinheritsched(thread, PTHREAD_EXPLICIT_SCHED);
        } else if (adjustment != 0) {
                setprio(0, getprio(0) + adjustment);
        }
}

#endif
