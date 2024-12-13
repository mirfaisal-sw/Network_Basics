/* attempt to turn off warnings for the rcsid string in the source code */

#if defined(__QNXNTO__) || defined(__NetBSD__) && !defined(__CONCAT)
#include <sys/cdefs.h>
#endif

#ifndef __CONCAT
#define __CONCAT(x, y) x##y
#endif

#ifndef __IDSTRING
#if defined(__WATCOMC__)
#define __IDSTRING(__n, __s) \
        static const char __n[] = __s
#elif defined(__GNUC__)
#define __IDSTRING(__n, __s) \
        static const char __n[] __attribute__((__unused__)) = __s
#else
#define __RCSID(__s)
#endif
#endif

#ifndef __RCSID
#define __XXX(__x, __y) __CONCAT(__x, __y)
#define __RCSID(__s) __IDSTRING(__XXX(rcsid, __LINE__), "@(#)"__s)
#endif

#if defined(__WATCOMC__)
#pragma off(unreferenced);
#else
#endif
