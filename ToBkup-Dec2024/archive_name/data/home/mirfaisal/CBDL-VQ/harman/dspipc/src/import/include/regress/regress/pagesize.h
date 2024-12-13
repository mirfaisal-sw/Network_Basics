#ifndef __PAGESIZE_H
#define __PAGESIZE_H

#include <limits.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <unistd.h>

#ifdef __QNXNTO__
#define PAGESIZE sysconf(_SC_PAGESIZE)
#else

#ifndef PAGESIZE
#ifdef EXEC_PAGESIZE
#define PAGESIZE EXEC_PAGESIZE
#elif defined(PAGE_SIZE)
#define PAGESIZE PAGE_SIZE
#elif defined(_SC_PAGESIZE)
#define PAGESIZE sysconf(_SC_PAGESIZE)
#elif defined(_SC_PAGE_SIZE)
#define PAGESIZE sysconf(_SC_PAGE_SIZE)
#else
#define PAGESIZE getpagesize()
#endif
#endif

#endif

#endif
