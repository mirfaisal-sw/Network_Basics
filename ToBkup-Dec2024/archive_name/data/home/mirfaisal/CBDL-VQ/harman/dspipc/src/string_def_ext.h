/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/

/**
 * @file string_def_ext.h
 *
 * This is the header that is used in all files that uses the string defines.
 * 
 *
 * 
 * @author Andreas Abel
 * @date   09.02.2007
 *
 * Copyright (c) 2007 Harman/Becker Automotive Systems GmbH
 */

#ifndef _DSPIPC_STRING_DEF_EXT_H_INCLUDED_
#define _DSPIPC_STRING_DEF_EXT_H_INCLUDED_

/*=========================  SLOG - Macro's   ======================================= */
/*hb_slogf_error("%s: No subdevice", __FUNCTION__);  16 */
extern char* NO_SUBDEVICE;

/*hb_slogf_error("%s: No device", __FUNCTION__);  51 */
extern char* NO_DEVICE;

/*hb_slogf_error("%s: No chipset", __FUNCTION__); 67 */
extern char* NO_CHIPSET;

/*hb_slogf_error("%s: dra6xxMemRead() failed", __FUNCTION__);  6 */
extern char* DRA6XXREAD_FAILED;
extern char* DRA6XXMEMREAD_FAILED;

/*hb_slogf_error("%s: dra6xxMemWrite() failed", __FUNCTION__);  4 (nur DRA6XX) */
extern char* DRA6XXWRITE_FAILED;
extern char* DRA6XXMEMWRITE_FAILED;
extern char* DRA6XXMEMWRITE_FAILED_2;

/*hb_slogf_error("%s: dra7xxMemRead() failed", __FUNCTION__);  6 */
extern char* DRA7XXREAD_FAILED;
extern char* DRA7XXMEMREAD_FAILED;

/*hb_slogf_error("%s: dra7xxMemWrite() failed", __FUNCTION__);  4 (nur DRA7XX) */
extern char* DRA7XXWRITE_FAILED;
extern char* DRA7XXMEMWRITE_FAILED;
extern char* DRA7XXMEMWRITE_FAILED_2;

/*hb_slogf_error("%s: dm642MemRead() failed", __FUNCTION__);  6 */
extern char* DM642READ_FAILED;
extern char* DM642MEMREAD_FAILED;

/*hb_slogf_error("%s: dm642MemWrite() failed", __FUNCTION__);  4 (nur DM642) */
extern char* DM642MEMWRITE_FAILED;
extern char* DM642MEMWRITE_FAILED_2;

/* hb_slogf_error("%s: No memory", __FUNCTION__);   5 */
extern char* NO_MEMORY;

/*"%s: No memory for buffer complete message"  3 */

/* hb_slogf_error("%s: Get invalid msg type out of queue",__FUNCTION__);   4 */
extern char* GET_INVALID_MESSAGE_TYPE_OUT_OF_QUEUE;

/*"%s: Invalid device type" 2 */

/*hb_slogf_error("%s: Get message from queue failed (%d)",__FUNCTION__, errCode);  5 */
extern char* GET_MESSAGE_FROM_QUEUE_FAILED;

/* hb_slogf_error("%s: Get Null ptr for msg", __FUNCTION__);   4 */
extern char* GET_NULL_PTR_FOR_MSG;

/*": Drain " 13 */

/*hb_slogf_error("%s Send message failed", __FUNCTION__ );   5 */
extern char* SEND_MESSAGE_FAILED;

/*hb_slogf_error("%s: get msg id failed", __FUNCTION__ );  7 */
extern char* GET_MESSAGE_ID_FAILED;

/*hb_slogf_error("%s: %s get msg id func failed for device %s",__FUNCTION__, pDevice->pDeviceName,pSubDevice->pSubDeviceName );  4 */
extern char* GET_MESSAGE_ID_FUNC_FOR_DEVICE_FAILED;

/*"%s: Add msg msgid %d cmdid %d to queue (%s) failed"  2 */

/*hb_slogf_error("%s: Cannot find corresponding input device", __FUNCTION__ );   4 */
extern char* CANNOT_FIND_CORRESPONDING_INPUT_DEVICE;

/*"%s: No memory for buffer complete message"  3 */

/*hb_slogf_error("%s: Unhandled content type",__FUNCTION__ );   4 */
extern char* UNHANDLED_CONTENT_TYPE;

/*"%s: Get number of messages in queue failed"  2 */

/*hb_slogf_error("%s: unlock subdevices failed", __FUNCTION__ );  8 */
extern char* UNLOCK_SUBDEVICE_FAILED;

/*hb_slogf_error("%s: lock subdevices failed", __FUNCTION__ );  8 */
extern char* LOCK_SUBDEVICE_FAILED;

/* hb_slogf_error(V_VERBOSE_MESSAGE, __FUNCTION__, "-verbose" );  3 */
extern char* V_VERBOSE_MESSAGE;

/* hb_slogf_error("%s: mutex lock failed %d <%s>", __FUNCTION__, errCode, strerror( errCode) );  33 */
extern char* MUTEX_LOCK_FAILED;

/* hb_slogf_error("%s: mutex unlock failed %d <%s>", __FUNCTION__, errCode, strerror( errCode) );  33 */
extern char* MUTEX_UNLOCK_FAILED;

/*hb_slogf_error("%s: Buffer queue %s <%s>", __FUNCTION__, MUTEX_LOCK_FAILED, strerror(errCode) ); */
/*hb_slogf_error("%s: Buffer queue mutex lock failed <%s>", __FUNCTION__ strerror(errCode) ); */
extern char* BUFFER_QUEUE_MUTEX_LOCK_FAILED;

/*hb_slogf_error("%s: Buffer queue %s <%s>", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode) ); */
/*hb_slogf_error("%s: Buffer queue mutex unlock failed <%s>", __FUNCTION__ strerror(errCode) ); */
extern char* BUFFER_QUEUE_MUTEX_UNLOCK_FAILED;

/*hb_slogf_error("%s: Message type invalid", __FUNCTION__); 4 */
extern char* MESSAGE_TYPE_INVALID;

/*hb_slogf_error("%s: Message reply invalid", __FUNCTION__); 4 */
extern char* MESSAGE_REPLY_INVALID;

/*hb_slogf_error("%s: Add sub device failed", __FUNCTION__ );  8 */
extern char* ADD_SUBDEVICE_FAILED;

/*hb_slogf_error("%s: Rem sub device failed", __FUNCTION__ );   4 */
extern char* REM_SUBDEVICE_FAILED;

/*hb_slogf_error("%s: IN sub device not found", __FUNCTION__ );  2 */
/*extern char* IN_SUBDEVICE_FAILED; */

/*hb_slogf_error("%s: OUT sub device not found", __FUNCTION__ );  2 */
extern char* OUT_SUBDEVICE_FAILED;

/*hb_slogf_error("%s: TimerTimeout failed (%s)", __FUNCTION__, strerror( errno ));  9 */
extern char* TIMERTIMEOUT_FAILED;

/*hb_slogf_error("Unknown unblock device type [PID=%d;TID=%d]", pCtp->info.pid, pCtp->info.tid ); 115 */
extern char* PID_TID;
extern char* STR_PID_TID;
extern char* STR_STR_PID_TID;

/*hb_slogf_error("%s: Try mem object remove while object is in use (%d) id:%d",__FUNCTION__, pFoundMemObj->usageCount, pFoundMemObj->memId ); */
extern char* TRY_REM_MEMOBJ_WHILE_IN_USE;

/*hb_slogf_error( "%s: Cannot open application file <%s> <%s>", __FUNCTION__, pChipset->pFileName, strerror(errno) ); */
extern char* CANNOT_OPEN_APPL_FILE;
/*hb_slogf_error( "%s: Cannot open bootloader file <%s> <%s>", __FUNCTION__, pChipset->pLoaderFileName, strerror(errno) ); */
extern char* CANNOT_OPEN_BL_FILE;
/*hb_slogf_error( "%s: No application file found!!", __FUNCTION__ ); */
extern char* NO_APPLICATION_FILE_FOUND;
/* hb_slogf_error( "%s: No bootloader file found!!", __FUNCTION__ ); */
extern char* NO_BOOTLOADER_FILE_FOUND;

/*only FOR DM642 */
/*--------------------------------------------------------------------------------------------------- */
/* hb_slogf_error("%s: mutex lock failed %d <%s>", __FUNCTION__, errCode, strerror( errCode) );  */
extern char* MEM_ACC_MUTEX_LOCK_FAILED;

/*hb_slogf_error("%s: Mem access %s %s", __FUNCTION__, MUTEX_UNLOCK_FAILED, strerror(errCode) ); */
extern char* MEM_ACC_MUTEX_UNLOCK_FAILED;

#endif /*_DSPIPC_STRING_DEF_EXT_H_INCLUDED_ */
