/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file cache_ops.h
 *
 * This file contains functions for cache handling. There are different
 * implementations available. Before QNX6.3.0 a special handling was 
 * required, since QNX6.3.0 a cache library is part of the OS.
 *
 * @author Bruno Achauer
 * @date   31.08.2006
 *
 * Copyright (c) 2006 Harman/Becker Automotive Systems GmbH
 */

#ifndef _CACHE_OPS_H_INCLUDED_
#define _CACHE_OPS_H_INCLUDED_

#include <linux/types.h>

/*=========================  Prototype Definitions   =========================== */

extern int hbCacheInit(void);
extern int hbCacheFini(void);

extern void hbCacheInval(void* vaddr, uint64_t paddr, size_t len);
extern void hbCacheFlush(void* vaddr, uint64_t paddr, size_t len);

#endif /*_CACHE_OPS_H_INCLUDED_ */
