/*********************************************************************
 * Project         QNX AVB MVIF
 *
 * (c) copyright   2010
 * Company         Harman/Becker Automotive Systems GmbH
 *                 All rights reserved
 *                 Secrecy Level STRICTLY CONFIDENTIAL
 *********************************************************************/
/**
 * @file          global.h
 * @ingroup       driver
 * @author        Gerd Zimmermann
 *
 * Global declarations.
 */

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "types.h"

#define MVIF_CFG_OWNER                  0x00000001
//#define MVIF_ADDR_TVTUNER               0x40328000

#if defined (CONFIG_SOC_J6FAMILY) || defined (CONFIG_IUKRAM_HIGH)
#define MVIF_SIZE_TVTUNER				0x10000
#define MVIF_ADDR_TVTUNER				0x40350000
#elif defined (CONFIG_SOC_J5ENTRY) || defined (CONFIG_NTG6_CSB)
#define MVIF_SIZE_TVTUNER				0x4000
#define MVIF_ADDR_TVTUNER				0x40308000
#endif

#define TS_PACKET_PHYS_ADDR             0x40360000

#define V4L2_RATELIMIT_BURST	3	/* no of messages emitted */
#define V4L2_RATELIMIT_INTERVAL	(30*HZ)	/* 30 seconds */

struct mvif_ops {
    int (*set_packet_size)(unsigned int x);
    int (*update_mac)(unsigned int mac_low, unsigned int mac_high);
    int (*update_streamid)(unsigned int streamid_low,
		unsigned int streamid_high);
    int (*create_video_interface)(unsigned int rolecfg,
        unsigned int memaddr, unsigned int memsize);
    int (*push_entry)(unsigned int indx,
        unsigned int *arrayTsPtr, unsigned int numTsPtr);
};

void vsink_cleanup_module(void);
int vsink_init_module(void);
int vsink_multi_stream_init_module(unsigned int);
int vsink_register_mvif(struct mvif_ops *mvifops);
#ifdef __QNXNTO__
#include "stdio.h"
// in this example hbas logger functions are not used
#ifndef logInfo
#define logInfo				printf
#endif
#endif

#define APP_NAME	"MVIFExample"

//#define MVIF_UNIT_TEST					/* test of 3 talker streams */

#define USE_MVIF_CFG_OWNER				/* features to take the role of CFG_OWNER */
#define USE_MVIF_CFG_READER				/* features to take the role of CFG_READER */

#define J6
//#define J5


#endif /* __GLOBAL_H */
