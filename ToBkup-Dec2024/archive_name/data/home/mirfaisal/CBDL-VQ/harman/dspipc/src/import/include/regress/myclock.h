#ifndef __MYCLOCK_H__
#define __MYCLOCK_H__

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#if defined(__QNXNTO__)
#include <stdint.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#ifndef __MYCLOCK_SECONDARY__
void snap_time(struct qtime_entry* qtp, unsigned* us)
{
        uint64_t ns;

        do {
                ns = qtp->nsec;
        } while (qtp->nsec != ns);
        *us = ns / 1000;
}
#else
extern void snap_time(struct qtime_entry* qtp, unsigned* us);
#endif

#ifndef __MYCLOCK_SECONDARY__
int calc_loops(void)
{
        int loops;
        struct qtime_entry* qtp = SYSPAGE_ENTRY(qtime);
        unsigned start;
        unsigned us;
        unsigned scale;

        loops = 0;
        snap_time(qtp, &us);
        start = us;
        do {
                ++loops;
                snap_time(qtp, &us);
        } while ((us - start) < 1000000);
        scale = 1;
        while (loops >= 100) { /* only keep two significant digits in loop count */
                scale *= 10;
                loops /= 10;
        }
        return loops * scale;
}
#else
extern int calc_loops(void);
#endif
#elif defined(__QNX__)
#include <signal.h>
#include <sys/kernel.h>
#include <sys/sched.h>
#include <sys/sendmx.h>

#define ClockTime(a, b, c) clock_gettime(a, c)
#define SchedYield() Yield()
#define calc_loops() 1000000

typedef struct _mxfer_entry iov_t;
#define MsgReceive(id, buf, len, info) Receive(id, buf, len)
#define MsgReply(id, state, buf, len) Reply(id, buf, len)
#define SETIOV(a, b, c) _setmx(a, b, c)
#define MsgSendv(id, iov, niov, riov, nriov) Sendmx(id, niov, nriov, iov, riov)
#define ConnectAttach(a, b, c, d, e) (b)
#define ChannelCreate(a) getpid();
#else
#include <sys/resource.h>
#define ClockTime(a, b, c) clock_gettime(a, c)
#define SchedYield() i_dont_know_how_to_yield()
#define setprio(pid, prio) setpriority(PRIO_PROCESS, 0, -20)
#define MAP_PHYS 0
#define calc_loops() 100000
#endif

#define tsd(t) ((double)(t.tv_sec & 0x0000ffff) * 1000 + ((double)t.tv_nsec) / 1000000.0)
struct timespec pstart;

#ifndef __MYCLOCK_SECONDARY__
unsigned
myclock(void)
{
#if defined(__QNXNTO__)
        uint64_t ts;
        ClockTime(CLOCK_REALTIME, 0, &ts);
        return ts / 1000000;

#elif defined(__QNX__)
        struct timespec ts;

        ClockTime(CLOCK_REALTIME, 0, &ts);
        return tsd(ts);
#endif
}
#else
extern unsigned myclock(void);
#endif

#ifndef __MYCLOCK_SECONDARY__
int calibrate_loop(void)
{
        char* loops;

        loops = getenv("LOOPS");
        if (loops == NULL)
                return calc_loops();
        return strtol(loops, NULL, 0);
}
#else
extern int calibrate_loop(void);
#endif

#endif
