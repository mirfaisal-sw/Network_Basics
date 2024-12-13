/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file string_def.h
 *
 * This is the ceantral header that defines the strings used in slog messages
 * This file is only included in "dev-dspipc.c"
 *
 * ALL other functions that will use this strigs include "string_def_ext.h"
 *
 * 
 * @author Andreas Abel
 * @date   09.02.2007
 *
 * Copyright (c) 2007 Harman/Becker Automotive Systems GmbH
 */

#ifndef _DSPIPC_STRING_DEF_H_INCLUDED_
#define _DSPIPC_STRING_DEF_H_INCLUDED_

#include "string_def_ext.h"

/*=========================  SLOG - Macro's   ======================================= */
/*hb_slogf_error("%s: No subdevice", __FUNCTION__);  16 */
char* NO_SUBDEVICE = "%s: No subdevice";

/*hb_slogf_error("%s: No device", __FUNCTION__);  51 */
char* NO_DEVICE = "%s: No device";

/*hb_slogf_error("%s: No chipset", __FUNCTION__); 67 */
char* NO_CHIPSET = "%s: No chipset";

/*hb_slogf_error("%s: dra6xxMemRead() failed", __FUNCTION__);  6 */
char* DRA6XXREAD_FAILED = "%s: dra6xxRead() failed";
char* DRA6XXMEMREAD_FAILED = "%s: dra6xxMemRead() failed (%d)";

/*hb_slogf_error("%s: dra6xxMemWrite() failed", __FUNCTION__);  4 (nur DM642) */
char* DRA6XXWRITE_FAILED = "%s: dra6xxWrite() failed";
char* DRA6XXMEMWRITE_FAILED = "%s: dra6xxMemWrite() failed";
char* DRA6XXMEMWRITE_FAILED_2 = "%s: dra6xxMemWrite() failed (%d)";

/*hb_slogf_error("%s: dra7xxMemRead() failed", __FUNCTION__);  6 */
char* DRA7XXREAD_FAILED = "%s: dra7xxRead() failed";
char* DRA7XXMEMREAD_FAILED = "%s: dra7xxMemRead() failed (%d)";

/*hb_slogf_error("%s: dra7xxMemWrite() failed", __FUNCTION__);  4 (nur DM642) */
char* DRA7XXWRITE_FAILED = "%s: dra7xxWrite() failed";
char* DRA7XXMEMWRITE_FAILED = "%s: dra7xxMemWrite() failed";
char* DRA7XXMEMWRITE_FAILED_2 = "%s: dra7xxMemWrite() failed (%d)";

/*hb_slogf_error("%s: dm642MemRead() failed", __FUNCTION__);  6 */
char* DM642READ_FAILED = "%s: dm642MemRead() failed";
char* DM642MEMREAD_FAILED = "%s: dm642MemRead() failed (%d)";

/*hb_slogf_error("%s: dm642MemWrite() failed", __FUNCTION__);  4 (nur DM642) */
char* DM642MEMWRITE_FAILED = "%s: dm642MemWrite() failed";
char* DM642MEMWRITE_FAILED_2 = "%s: dm642MemWrite() failed (%d)";

/* hb_slogf_error("%s: No memory", __FUNCTION__);   5 */
char* NO_MEMORY = "%s: Get invalid msg type out of queue";

/*"%s: No memory for buffer complete message"  3 */

/* hb_slogf_error("%s: Get invalid msg type out of queue",__FUNCTION__);   4 */
char* GET_INVALID_MESSAGE_TYPE_OUT_OF_QUEUE = "%s: Get invalid msg type out of queue";

/*"%s: Invalid device type" 2 */

/*hb_slogf_error("%s: Get message from queue failed (%d)",__FUNCTION__, errCode);  5 */
char* GET_MESSAGE_FROM_QUEUE_FAILED = "%s: Get message from queue failed (%d)";

/* hb_slogf_error("%s: Get Null ptr for msg", __FUNCTION__);   4 */
char* GET_NULL_PTR_FOR_MSG = "%s: Get Null ptr for msg";

/*": Drain " 13 */

/*hb_slogf_error("%s Send message failed", __FUNCTION__ );   5 */
char* SEND_MESSAGE_FAILED = "%s Send message failed";

/*hb_slogf_error("%s: get msg id failed", __FUNCTION__ );  7 */
char* GET_MESSAGE_ID_FAILED = "%s: get msg id failed";

/*hb_slogf_error("%s: %s get msg id func failed for device %s",__FUNCTION__, pDevice->pDeviceName,pSubDevice->pSubDeviceName );  4 */
char* GET_MESSAGE_ID_FUNC_FOR_DEVICE_FAILED = "%s: %s get msg id func failed for device %s";

/*"%s: Add msg msgid %d cmdid %d to queue (%s) failed"  2 */

/*hb_slogf_error("%s: Cannot find corresponding input device", __FUNCTION__ );   4 */
char* CANNOT_FIND_CORRESPONDING_INPUT_DEVICE = "%s: Cannot find corresponding input device";

/*"%s: No memory for buffer complete message"  3 */

/*hb_slogf_error("%s: Unhandled content type",__FUNCTION__ );   4 */
char* UNHANDLED_CONTENT_TYPE = "%s: Unhandled content type";

/*"%s: Get number of messages in queue failed"  2 */

/*hb_slogf_error("%s: Unlock subdevices failed", __FUNCTION__ );  8 */
char* UNLOCK_SUBDEVICE_FAILED = "%s: unlock subdevices failed";

/*hb_slogf_error("%s: Lock subdevices failed", __FUNCTION__ );  8 */
char* LOCK_SUBDEVICE_FAILED = "%s: lock subdevices failed";

/* hb_slogf_error(V_VERBOSE_MESSAGE, __FUNCTION__, "-verbose" );  3 */
char* V_VERBOSE_MESSAGE = "%s: %s Option not available in this version. If you need this Option, please contact: andreas.abel@harman.com";

/* hb_slogf_error("%s: mutex lock failed %d <%s>", __FUNCTION__, errCode, strerror( errCode) );  33 */
char* MUTEX_LOCK_FAILED = "mutex lock failed";

/* hb_slogf_error("%s: mutex unlock failed %d <%s>", __FUNCTION__, errCode, strerror( errCode) );  33 */
char* MUTEX_UNLOCK_FAILED = "mutex unlock failed";

/*hb_slogf_error("%s: Buffer queue %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode) ); */
/*hb_slogf_error("%s: Buffer queue mutex lock failed <%s>", __FUNCTION__ strerror(errCode) ); */
char* BUFFER_QUEUE_MUTEX_LOCK_FAILED = "%s: Buffer queue mutex lock failed ";

/*hb_slogf_error("%s: Buffer queue %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode) ); */
/*hb_slogf_error("%s: Buffer queue mutex unlock failed <%s>", __FUNCTION__ strerror(errCode) ); */
char* BUFFER_QUEUE_MUTEX_UNLOCK_FAILED = "%s: Buffer queue mutex unlock failed ";

/*hb_slogf_error("%s: Message type invalid", __FUNCTION__); 4 */
char* MESSAGE_TYPE_INVALID = " Message type invalid";

/*hb_slogf_error("%s: Message reply invalid", __FUNCTION__); 4 */
char* MESSAGE_REPLY_INVALID = " Message type invalid";

/*hb_slogf_error("%s: Add sub device failed", __FUNCTION__ );  8 */
char* ADD_SUBDEVICE_FAILED = "%s: Add sub device failed";

/*hb_slogf_error("%s: Rem sub device failed", __FUNCTION__ );   4 */
char* REM_SUBDEVICE_FAILED = "%s: Rem sub device failed";

/*hb_slogf_error("%s: IN sub device not found", __FUNCTION__ );  2 */
/*char* IN_SUBDEVICE_FAILED = "%s: IN sub device not found"; */

/*hb_slogf_error("%s: OUT sub device not found", __FUNCTION__ );  2 */
char* OUT_SUBDEVICE_FAILED = "%s: OUT sub device not found";

/*hb_slogf_error("%s: TimerTimeout failed (%s)", __FUNCTION__, strerror( errno ));  9 */
char* TIMERTIMEOUT_FAILED = "%s: TimerTimeout failed";

/*hb_slogf_error("Unknown unblock device type [PID=%d;TID=%d]", pCtp->info.pid, pCtp->info.tid ); 115 */
/*hb_slogf_error("Unknown unblock device type [PID/TID=%d/%d]", pCtp->info.pid, pCtp->info.tid ); 115    %s%d/%d   %s%d/%d] */
/*char* PID_TID = "[PID=%d;TID=%d]"; */
char* PID_TID = "PID/TID=";
char* STR_PID_TID = "%s [PID=%d;TID=%d]";
char* STR_STR_PID_TID = "%s: %s [PID=%d;TID=%d]";

/*hb_slogf_error("%s: Try mem object remove while object is in use (%d) id:%d",__FUNCTION__, pFoundMemObj->usageCount, pFoundMemObj->memId ); */
char* TRY_REM_MEMOBJ_WHILE_IN_USE = "%s: Try mem object remove while object is in use (%d) id:%d";

/*hb_slogf_error( "%s: Cannot open application file <%s> <%s>", __FUNCTION__, pChipset->pFileName, strerror(errno) ); */
char* CANNOT_OPEN_APPL_FILE = "%s: Cannot open application file <%s> <%s>";
/*hb_slogf_error( "%s: Cannot open bootloader file <%s> <%s>", __FUNCTION__, pChipset->pLoaderFileName, strerror(errno) ); */
char* CANNOT_OPEN_BL_FILE = "%s: Cannot open bootloader file <%s> <%s>";
/*hb_slogf_error( "%s: No application file found!!", __FUNCTION__ ); */
char* NO_APPLICATION_FILE_FOUND = "%s: No application file  found!!";
/* hb_slogf_error( "%s: No application file found!!", __FUNCTION__ ); */
char* NO_BOOTLOADER_FILE_FOUND = "%s: No bootloader file  found!!";

/*only FOR DM642 */
/*--------------------------------------------------------------------------------------------------- */
/* hb_slogf_error("%s: mutex lock failed %d <%s>", __FUNCTION__, errCode, strerror( errCode) );  */
char* MEM_ACC_MUTEX_LOCK_FAILED = "%s: Mem access mutex lock failed ";

/*hb_slogf_error("%s: Mem access %s %s", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode) ); */
char* MEM_ACC_MUTEX_UNLOCK_FAILED = "%s: Mem access mutex unlock failed ";

#endif /*_DSPIPC_STRING_DEF_H_INCLUDED_ */
