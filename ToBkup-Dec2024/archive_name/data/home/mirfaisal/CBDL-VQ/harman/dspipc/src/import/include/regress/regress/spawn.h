/* $Id: spawn.h,v 1.1 1998/11/23 21:50:11 kirk Exp $ */

#ifndef _SPAWN_H_
#define _SPAWN_H_

/*
 *  This API is based on P1003.4b/D8 an unapproved IEEE Standards Draft.
 */

#ifdef _POSIX_SPAWN

#include <signal.h>
#include <sys/types.h>

#define SPAWN_FDCLOSED (-1)
#define SPAWN_NEWPGROUP (-1)

typedef struct inheritance {
        int flags; /* bit set of the following:	*/
#define SPAWN_SETGROUP (0x01)
#define SPAWN_SETSIGMASK (0x02)
#define SPAWN_SETSIGDEF (0x04)
#define SPAWN_NOZOMBIE (0)

        pid_t pgroup;        /* maps to SPAWN_SETGROUP		*/
        sigset_t sigmask;    /* maps to SPAWN_SETSIGMASK		*/
        sigset_t sigdefault; /* maps to SPAWN_SETSIGDEF		*/
} spawn_inheritance_type;

pid_t spawn(const char* path,
    const int fd_count,
    const int fd_map[],
    const struct inheritance* inherit,
    const char* argv[],
    const char* envp[]);

pid_t spawnp(const char* file,
    const int fd_count,
    const int fd_map[],
    const struct inheritance* inherit,
    const char* argv[],
    const char* envp[]);

#endif /* ! _POSIX_SPAWN */

#endif /* ! _SPAWN_H_    */
