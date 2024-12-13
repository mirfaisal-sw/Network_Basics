#ifdef QANOBUILTIN
/*
* When QANOBUILTIN is defined, try to get gcc to stop using its builtin
* functions and use the real libc calls instead.
*
* See PR21077 -- setjmp does not to be a linkable object -- can be only
* a macro.  If setjmp disappears from x86 libc, then that is a regression.
*/

#undef alloca
#undef memcpy
#undef memcmp
#undef memset
#undef strcmp
#undef strcpy
#undef strlen
#if defined(__X86__)
#undef setjmp
#endif
#undef longjmp
#undef sigsetjmp
#undef siglongjmp

#ifdef _ALLOCA_H_INCLUDED
extern void* alloca(size_t);
#endif

#ifdef _SETJMP_H_INCLUDED
extern int sigsetjmp(sigjmp_buf, int);
extern void siglongjmp(sigjmp_buf, int);
#if defined(__X86__)
extern int setjmp(jmp_buf);
#endif
extern void longjmp(jmp_buf, int);
#endif

#define alloca (alloca)
#define memcpy (memcpy)
#define memcmp (memcmp)
#define memset (memset)
#define strcmp (strcmp)
#define strcpy (strcpy)
#define strlen (strlen)
#if defined(__X86__)
#define setjmp (setjmp)
#endif
#define longjmp (longjmp)

#endif
