#ifndef __CRED_H
#define __CRED_H
/*****************************************************************************
*
*	File:	cred.h
*
******************************************************************************
*
*   Contents:	Header for the regress libs.
*
*	Date:		Dec 8, 1997.
*
*	Author:		Kirk Russell.
*
*	$Id: cred.h,v 1.2 1998/09/04 20:07:17 kirk Exp $
*
*****************************************************************************/

#ifndef __QNXNTO__
#include <limits.h>
#include <unistd.h>
#else
#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif

#include <limits.h>

#include <_pack64.h>

#if defined(__UID_T)
typedef __UID_T uid_t;
#undef __UID_T
#endif

#if defined(__GID_T)
typedef __GID_T gid_t;
#undef __GID_T
#endif
#endif

typedef struct {
        int cr_ref;                       /* not used */
        uid_t cr_uid;                     /* effective user id */
        gid_t cr_gid;                     /* effective group id */
        uid_t cr_ruid;                    /* real user id */
        gid_t cr_rgid;                    /* read group id */
        uid_t cr_suid;                    /* saved user id */
        gid_t cr_sgid;                    /* saved group id */
        int cr_ngroups;                   /* number of groups */
        gid_t cr_groups[NGROUPS_MAX + 1]; /* supplementary group list */
} ucred_t;

#ifdef __QNXNTO__
#include <_packpop.h>
#endif

#ifndef TESTUID
#define TESTUID ((uid_t)143)
#endif
#ifndef TESTGID
#define TESTGID ((gid_t)256)
#endif

#ifdef __QNXNTO__
__BEGIN_DECLS
#endif

extern int getucred(ucred_t* __cred);

#ifdef __QNXNTO__
__END_DECLS
#endif

#endif
