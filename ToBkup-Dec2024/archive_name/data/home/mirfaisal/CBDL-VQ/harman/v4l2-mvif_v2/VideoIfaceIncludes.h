/*****************************************************************************
 * Project         QNX AVB driver 4 Jacinto 5
 *
 * (c) copyright   2015
 * Company         Harman/Becker Automotive Systems GmbH
 *                 All rights reserved
 *                 STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file          VideoIFaceIncludes.h
 * @ingroup       driver
 * @author        Gerd Zimmermann
 *
 * Adaption of different type settings, functions (like memset), ..for the mandatory AudioIfaceConfig-Files
 *
 * \image html SampleFormat.jpg
 */

#ifndef __VIDEOIFACEINCLUDES_H
#define __VIDEOIFACEINCLUDES_H


#include <linux/string.h>

#ifdef __QNXNTO__

// use this define if your CPU use an MMU with virtual ptr management
#define VIRTUAL_PTR

// normally use these header files
#include "global.h"
#include "VideoIfaceConfig.h"

/*#elif __X86__

// normally use these header files
#include "global.h"
#include "VideoIfaceConfig.h"
*/
#else

// use this define if your CPU uses direct memory addressing for pointer
//#define PHYS_PTR
// Used for Linux App
#define VIRTUAL_PTR

// normally use these header files
#include "global.h"
#include "VideoIfaceConfig.h"


#endif




#endif /* __GLOBAL_H */
