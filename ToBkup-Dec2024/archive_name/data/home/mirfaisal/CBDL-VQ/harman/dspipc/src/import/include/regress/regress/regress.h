#ifndef __REGRESS_H
#define __REGRESS_H
/*****************************************************************************
*
*	File:	regress.h
*
******************************************************************************
*
*   Contents:	Header for the regress libs.
*
*	Date:		Aug 29, 1997.
*
*	Author:		Kirk Russell.
*
*	$Id: regress.h,v 1.19 2004/09/17 15:14:36 kirk Exp $
*
*****************************************************************************/
#include <inttypes.h>
#include <stdarg.h>
struct inheritance;

#ifdef __QNXNTO__
#if defined(__PID_T)
typedef __PID_T pid_t;
#undef __PID_T
#endif
#if defined(__SIZE_T)
typedef __SIZE_T size_t;
#undef __SIZE_T
#endif
#if defined(__IOVEC_T)
typedef __IOVEC_T iov_t;
#undef __IOVEC_T
#endif
#else
#include <sys/uio.h>
#endif

#ifndef _MAX_PATH
#define _MAX_PATH 256
#endif

/* Round a value to the lowest integer less than it that is a multiple of
   the required alignment.  Avoid using division in case the value is
   negative.  Assume the alignment is a power of two.  */
#define FLOOR_ROUND(VALUE, ALIGN) ((VALUE) & ~((ALIGN)-1))

/* Similar, but round to the next highest integer that meets the
   alignment.  */
#define CEIL_ROUND(VALUE, ALIGN) (((VALUE) + (ALIGN)-1) & ~((ALIGN)-1))

/* flags for get??addrinfo() */
#define ADDRINFO_USER (0x0001)
#define ADDRINFO_KERN (0x0002)
#define ADDRINFO_SYSP (0x0004)
#define ADDRINFO_ALGN (0x0008)

#if defined(__cplusplus) || defined(__CPLUSPLUS__)
extern "C" {
#endif

extern int _prnt(const char* __fmt, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 1, 2)));
#else
    ;
#endif
extern int _sprnt(char* __buf, unsigned __buflen, const char* __fmt, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 3, 4)));
#else
    ;
#endif
extern int _svprnt(char* __buf, unsigned __buflen, const char* __fmt, va_list __args)
#ifdef __GNUC__
    __attribute__((format(printf, 3, 0)));
#else
    ;
#endif
#ifdef VARIANT_mp
extern void _mpstart();
#endif

extern void x_ill();
extern int ikercall();
extern int ikercall_r();
extern int copyfile(const char* __in, const char* __out);
extern int mkpty2(char* __slavename, int* __fdm, int* __fds, const char* __class, const char* __count);
extern int mkpty(char* __slavename, int* __fdm, int* __fds);
extern int getprocaddrinfo(int* __num, iov_t** __regions);
extern int getwraddrinfo(int __flags, int* __num, iov_t** __regions, char*** __names);
extern int getrdaddrinfo(int __flags, int* __num, iov_t** __regions, char*** __names);
extern void busywait(int __seconds);
extern int pattern_fill(void* __addr, int __len, const char* __pat, size_t __patlen);
extern int pattern_verify(const void* __addr, int __len, const char* __pat, size_t __patlen);
extern int getisstatic(const char* path, int* isstatic);
extern pid_t spork(const char* __path, int __fd_count,
    const int __fd_map[], const struct inheritance* __i,
    char* const __argv[], char* const __envp[],
    int (*execcmd)(const char*, char* const __argv[], char* const __envp[]));
extern pid_t regress_getsid(pid_t);
extern void (*regress_signal(int, void (*)(int)))(int);
extern void iwannauseswap(void);
extern int getlibcname(char* name, int len);
int useram(uint64_t, int**, int*);
extern void* __SysCpupageGet(int index); /* hidden libc call */

#if defined(__cplusplus) || defined(__CPLUSPLUS__)
};
#endif

#endif
