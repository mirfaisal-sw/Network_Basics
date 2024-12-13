/**
 * @file hbslog.h
 *
 * This file contains the global slog definitions
 *
 *  @author        Sven Behnsen
 *  @date          1.7.2002
 *
 * Copyright (c) 2002-2005 Harman/Becker Automotive Systems GmbH
 */

#ifndef __HBSLOG_H_INCLUDED_
#define __HBSLOG_H_INCLUDED_

#include <errno.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/slogcodes.h>

/**
 * set level of severity used inside the slog macros for slogger
 */
extern unsigned char slogSeverity;

/**
 * char pointer for __progname resolution
 */
extern const char* __progname;

/**
 * log levels
 * 0 Programm Flow
 * 1 Monitoring
 * 2 Timing Metric
 * 3 Statistics 
 * 4-15 reserved 
 */

#define _SLOG_SETCODE_NEW(ma, comp, log) ((ma) | (((comp & 0xff) | ((log & 0xf) << 4)) << 20))

/**
 * Macro definition for SLOGC_BECKER
 */
#define _SLOGC_BECKER __C(20001, 0)

/**
 * Macro definition for SLOGC_SYSTEM
 */
#define _SLOGC_SYSTEM __C(20002, 0)

/**
 * Macro definition for SLOGC_NAVI
 */
#define _SLOGC_NAVI __C(20003, 0)

/**
 * Macro definition for SLOGC_MMI
 */
#define _SLOGC_MMI __C(20004, 0)

/**
 * Macro definition for SLOGBECKER( minor ) method
 */
#define SLOGBECKER(comp, log) _SLOG_SETCODE_NEW(_SLOGC_BECKER, comp, log)

/**
 * Macro definítion for SLOGSYSTEM( minor ) method
 */
#define SLOGSYSTEM(comp, log) _SLOG_SETCODE_NEW(_SLOGC_SYSTEM, comp, log)

/**
 * Macro definition for SLOGNAVI( minor ) method
 */
#define SLOGNAVI(comp, log) _SLOG_SETCODE_NEW(_SLOGC_NAVI, comp, log)

/**
 * Macro definition for SLOGMMI( minor )method
 */
#define SLOGMMI(comp, log) _SLOG_SETCODE_NEW(_SLOGC_MMI, comp, log)

/* _SLOG_SHUTDOWN  0   /* Shut down the system NOW. eg: for OEM use */ * /
    /* _SLOG_CRITICAL  1   /* Unexpected unrecoverable error. eg: hard disk error */ * /
    /* _SLOG_ERROR     2   /* Unexpected recoverable error. eg: needed to reset a hw controller */ * /
    /* _SLOG_WARNING   3   /* Expected error. eg: parity error on a serial port */ * /
    /* _SLOG_NOTICE    4   /* Warnings. eg: Out of paper */ * /
    /* _SLOG_INFO      5   /* Information. eg: Printing page 3 */ * /
    /* _SLOG_DEBUG1    6   /* Debug messages eg: Normal detail */ * /
    /* _SLOG_DEBUG2    7   /* Debug messages eg: Fine detail */ * /

/*      NAME                    CODE               SEVERITY       LENGTH/PARAMETER */
/**
 * Macro for SL_PROCESS_START
 */
#define SL_PROCESS_START SLOGBECKER(1, 0), _SLOG_INFO

/**
 * Macro for SL_PROCESS_TERMINATE
 */
#define SL_PROCESS_TERMINATE SLOGBECKER(2, 0), _SLOG_INFO

/**
 * Macro definition for output error message SLE_SYSTICKER_NOTFOUND
 */
#define SLE_SYSTICKER_NOTFOUND SLOGBECKER(3, 0), _SLOG_ERROR, "Systicker daemon not found"

/**
 * Macro definition for output error message SLE_THREADPOOLINIT
 */
#define SLE_THREADPOOLINIT SLOGBECKER(4, 0), _SLOG_CRITICAL, "Unable to initialise thread pool"

/**
 * Macro definition for output error message SLE_MALLOC_ERRROR
 */
#define SLE_MALLOC_ERRROR SLOGBECKER(5, 0), _SLOG_CRITICAL, "malloc() error (%d bytes for %s)"

/**
 * Macro definition for output error message SLE_OPENFAILURE
 */
#define SLE_OPENFAILURE SLOGBECKER(6, 0), _SLOG_ERROR, "open() failed file=%s (%s)"

/**
 * Macro definition for output error message SLE_NAMEATTACH
 */
#define SLE_NAMEATTACH SLOGBECKER(7, 0), _SLOG_CRITICAL, "Unable to attach name '%s'"

/**
 * Macro definition for output status message SLI_GOTSIGNAL
 */
#define SLI_GOTSIGNAL SLOGBECKER(8, 0), _SLOG_INFO, "Got Signal (%d)"

    /*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */* */
/* Assigned codes System */
/* 0     global codes */

/**
 * Macro to define codenumber for hbnobss (SB)
 */
/* 10   hbnobss (SB) */
#define SLOGC_HBNOBSS(log) SLOGSYSTEM(10, log)

/**
 * Macro to define codenumber for systicker (SB)
 */
/* 20   systicker (SB) */
#define SLOGC_SYSTICKER(log) SLOGSYSTEM(20, log)

/**
 * Macro to define codenumber for fpgalib (DD)
 */
/* 30  fpgalib (DD) */
#define SLOGC_FPGALIB(log) SLOGSYSTEM(30, log)

/**
 * Macro to define codenumber for HDD manager
 */
/* 40  hddmgr */
#define SLOGC_HDDMGR(log) SLOGSYSTEM(40, log)

/**
 * Macro to define codenumber for dev-dspipc
 */
/* 50  dev-dspipc */
#define SLOGC_DSPIPC(log) SLOGSYSTEM(50, log)

/**
 * Macro to define codenumber for devb-hbatapi
 */
/* 60  devb-hbatapi */
#define SLOGC_ATAPI(log) SLOGSYSTEM(60, log)

/**
 * Macro to define codenumber for devg-s2p.so
 */
/* 70   devg-s2p.so */
#define SLOGC_SCARLET(log) SLOGSYSTEM(70, log)

/**
 * Macro to define codenumber for hbconfdb
 */
/* 80   hbconfdb */
#define SLOGC_HBCONFDB(log) SLOGSYSTEM(80, log)

/**
 * Macro to define codenumber for dev-most
 */
/* 90   dev-most */
#define SLOGC_MOST(log) SLOGSYSTEM(90, log)

/**
 * Macro to define codenumber for devn-most
 */
/* 100  devn-most */
#define SLOGC_MOST_DLL(log) SLOGSYSTEM(100, log)

/**
 * Macro to define codenumber for dev-ipc
 */
/* 110   dev-ipc */
#define SLOGC_IPC(log) SLOGSYSTEM(110, log)

/**
 * Macro to define codenumber for GPSDevice
 */
/* 120   GPSDevice */
#define SLOGC_GPSDEVICE(log) SLOGSYSTEM(120, log)

/**
 * Macro to define codenumber for SPI
 */
/* 130   SPI */
#define SLOGC_SPI(log) SLOGSYSTEM(130, log)

/**
 * Macro to define codenumber for QKCP
 */
/* 140   QKCP */
#define SLOGC_QKCP(log) SLOGSYSTEM(140, log)

/**
 * Macro to define codenumber for IOSubsystem
 */
/* 150  IOSubsystem */
#define SLOGC_IOS(log) SLOGSYSTEM(150, log)

/**
 * Macro to define codenumber for I2C
 */
/* 160  I2C */
#define SLOGC_I2C(log) SLOGSYSTEM(160, log)

/**
 * Macro to define codenumber for DEVB-M6
 */
/* 170  DEVB-M6 */
#define SLOGC_M6(log) SLOGSYSTEM(170, log)

/**
 * Macro to define codenumber for network-connector
 */
/* 180  network-connector */
#define SLOGC_NETCONNECT(log) SLOGSYSTEM(180, log)

/**
 * Macro to define codenumber for dev-ser8250hb
 */
/* 190  devc-ser8250hb */
#define SLOGC_SER8250HB(log) SLOGSYSTEM(190, log)

/**
 * Macro to define codenumber for dev-adjmanager
 */
/* 200  dev-adjmanager */
#define SLOGC_ADJMANAGER(log) SLOGSYSTEM(200, log)

/**
 * Macro to define codenumber for dev-iso7816
 */
/* 210  dev-iso7816 */
#define SLOGC_ISO7816(log) SLOGSYSTEM(210, log)

/**
 * Macro to define codenumber for dev-dsp
 */
/* 220  dev-dsp */
#define SLOGC_DSP(log) SLOGSYSTEM(220, log)

/**
 * Macro to define codenumber for dev-sdc
 */
/* 230  dev-sdc */
#define SLOGC_SDC(log) SLOGSYSTEM(230, log)

/**
 * Macro to define codenumber for dev-prjspec
 */
/* 240  dev-prjspec */
#define SLOGC_PRJSPEC(log) SLOGSYSTEM(240, log)

/**
 * Macro to define codenumber for dev-proxy
 */
/* 250  dev-proxy */
#define SLOGC_PROXY(log) SLOGSYSTEM(250, log)

    /*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */* */
                                                                                        /* Assigned codes NAVI */

    /*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */*/*/ */* */
    /* Assigned codes MMI */

    /**
   H/B version of the slogi functionality. All paramters will be passed to
   the slogi call. slogi is only called if the global severity is equal or
   greater than the specified one.

   @param code A combination of a major and minor code, built with hbslog macros
   @param sev The severity of the log message
   @param nargs The number of integers to send. A maximum of 32 integers is allowed.
   @return void
 */
    /*lint -esym(515,hb_slogi) function has variable parameter list */
    void
    hb_slogi(int code, int sev, int nargs, ...);

#ifdef HBSLOG_MAIN /* include only if included from main file */
void hb_slogi(int code, int sev, int nargs, ...)
{
        int ret = 0;
        va_list arglist;
        if (sev <= slogSeverity) {
                va_start(arglist, nargs);
                ret = slogi(code, sev, nargs, arglist);
                if (ret == -1) {
                        perror("Fatal: Could not write slog event");
                }
                va_end(arglist);
        }
        return;
}
#endif /* HBSLOG_MAIN */

/**
   H/B version of the slogb functionality. All paramters will be passed to
   the slogb call. slogb is only called if the global severity is equal or
   greater than the specified one.

   @param code A combination of a major and minor code, built with hbslog macros
   @param sev The severity of the log message
   @param data A block of raw data
   @param size Size of the raw data
   @return void
 */
void hb_slogb(int code, int sev, void* data, int size);

#ifdef HBSLOG_MAIN /* include only if included from main file */
void hb_slogb(int code, int sev, void* data, int size)
{
        int ret = 0;
        if (sev <= slogSeverity) {
                ret = slogb(code, sev, data, size);
                if (ret == -1) {
                        perror("Fatal: Could not write slog event");
                }
        }
        return;
}
#endif /* HBSLOG_MAIN */

/**
   H/B version of the slogf functionality. All paramters will be passed to
   the slogf call. The program name string is added in front of the format
   string to determine the source of the message in the slog. slogf is only
   called if the global severity is equal or greater than the specified one.

   @param code A combination of a major and minor code, built with hbslog macros
   @param sev The severity of the log message
   @param fmt A standard printf() string followed by printf() arguments
   @return void
*/
void hb_slogf(int code, int sev, const char* fmt, ...);

#ifdef HBSLOG_MAIN /* include only if included from main file */
void hb_slogf(int code, int sev, const char* fmt, ...)
{
        int ret = 0;
        va_list arglist;
        char* named_fmt;
        int progname_len;
        progname_len = strlen(__progname);
        if (sev <= slogSeverity) {
                if ((named_fmt = (char*)malloc(strlen(fmt) + progname_len + 3)) != NULL) {
                        strncpy(&named_fmt[0], __progname, progname_len);
                        named_fmt[progname_len] = ':';
                        named_fmt[progname_len + 1] = ' ';
                        strncpy(&named_fmt[progname_len + 2], fmt, strlen(fmt) + 1);
                        /*lint -e{516} type conflict in GNU header files, cannot resolved by lint */
                        va_start(arglist, fmt);
                        ret = vslogf(code, sev, named_fmt, arglist);
                        free(named_fmt);
                        if (ret == -1) {
                                perror("Fatal: Could not write slog event");
                        }
                        va_end(arglist);
                }
        }
        return;
}

static void hb_slogf_intern(int code, int sev, const char* fmt, va_list arglist)
{
        int ret = 0;
        char* named_fmt;
        int progname_len;
        progname_len = strlen(__progname);
        if (sev <= slogSeverity) {
                if ((named_fmt = (char*)malloc(strlen(fmt) + progname_len + 3)) != NULL) {
                        strncpy(&named_fmt[0], __progname, progname_len);
                        named_fmt[progname_len] = ':';
                        named_fmt[progname_len + 1] = ' ';
                        strncpy(&named_fmt[progname_len + 2], fmt, strlen(fmt) + 1);
                        ret = vslogf(code, sev, named_fmt, arglist);
                        free(named_fmt);
                        if (ret == -1) {
                                perror("Fatal: Could not write slog event");
                        }
                }
        }
        return;
}
#endif /* HBSLOG_MAIN */

/**
   Simplified H/B call to send an error entry to the slog. The function will
   pass the fmt string and the paramters to the hb_slogf function. It will use
   the SLOGBECKER(0,0) id with the severity _SLOG_ERROR.

   @param fmt A standard printf() string followed by printf() arguments
   @return void
*/
void hb_slogf_error(const char* fmt, ...);

#ifdef HBSLOG_MAIN /* include only if included from main file */
void hb_slogf_error(const char* fmt, ...)
{
        va_list arglist;
        va_start(arglist, fmt);
        hb_slogf_intern(SLOGBECKER(0, 0), _SLOG_ERROR, fmt, arglist);
        va_end(arglist);
        return;
}
#endif /* HBSLOG_MAIN */

/**
   Simplified H/B call to send an warning entry to the slog. The function will
   pass the fmt string and the paramters to the hb_slogf function. It will use
   the SLOGBECKER(0,0) id with the severity _SLOG_WARNING.

   @param fmt A standard printf() string followed by printf() arguments
   @return void
*/
void hb_slogf_warning(const char* fmt, ...);

#ifdef HBSLOG_MAIN /* include only if included from main file */
void hb_slogf_warning(const char* fmt, ...)
{
        va_list arglist;
        va_start(arglist, fmt);
        hb_slogf_intern(SLOGBECKER(0, 0), _SLOG_WARNING, fmt, arglist);
        va_end(arglist);
        return;
}
#endif /* HBSLOG_MAIN */

/**
   Simplified H/B call to send an info entry to the slog. The function will
   pass the fmt string and the paramters to the hb_slogf function. It will use
   the SLOGBECKER(0,0) id with the severity _SLOG_INFO.

   @param fmt A standard printf() string followed by printf() arguments
   @return void
*/
void hb_slogf_info(const char* fmt, ...);

#ifdef HBSLOG_MAIN /* include only if included from main file */
void hb_slogf_info(const char* fmt, ...)
{
        va_list arglist;
        va_start(arglist, fmt);
        hb_slogf_intern(SLOGBECKER(0, 0), _SLOG_INFO, fmt, arglist);
        va_end(arglist);
        return;
}
#endif /* HBSLOG_MAIN */

/**
   Simplified H/B call to print a process start message to the slog. The
   function will call the hbslogf function using the SLOGBECKER(1,0) id with the
   severity _SLOG_INFO.

   @return void
*/
void hbSlogProcessStart(void);

#ifdef HBSLOG_MAIN /* include only if included from main file */
void hbSlogProcessStart(void)
{
        hb_slogf(SL_PROCESS_START, "process started PID=0x%08X", getpid());
        return;
}
#endif /* HBSLOG_MAIN */

/**
   Simplified H/B call to print a process termination message to the slog. The
   function will call the hbslogf function using the SLOGBECKER(2,0) id with the
   severity _SLOG_INFO.

   @return void
*/
void hbSlogProcessTerminate(void);

#ifdef HBSLOG_MAIN /* include only if included from main file */
void hbSlogProcessTerminate(void)
{
        hb_slogf(SL_PROCESS_TERMINATE, "process terminated PID=0x%08X", getpid());
        return;
}
#endif /* HBSLOG_MAIN */

#endif /* __HBSLOG_H_INCLUDED_ */
