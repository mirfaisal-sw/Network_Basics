/*********************************************************************
 * Project         BMW IuK MAIF sources
 *
 * (c) copyright   2015
 * Company         Harman/Becker Automotive Systems GmbH
 *                 All rights reserved
 *                 STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file          	iukMAIFRoles.h
 * @ingroup       	driver
 * @author        	Gerd Zimmermann
 * @date			19.11.2015
 */

//#if 0
//#if 1		// for doxygen generation

#ifndef __IUK_MVIFROLES_H
#define __IUK_MVIFROLES_H

#include "VideoIfaceConfig.h"
#include "VideoIfaceIncludes.h"

/**
 * The role 'CFG_OWNER' creates common object and all stream objects in OCMC memory and sets the parameter
 * describing the sample exchange
 * this option can only be used if the global define 'USE_MAIF_CFG_OWNER' is define somewhere
 */
#ifdef USE_MVIF_CFG_OWNER
#define MVIF_CFG_OWNER				0x00000001
#endif

/**
 * the role 'CFG_READER' uses the parameter of common and stream objects. It has no possibility to change parameter
 * this option can only be used if the global define 'USE_MVIF_CFG_READER' is define somewhere
 */
#ifdef USE_MVIF_CFG_READER
#define MVIF_CFG_READER				0x00000002
#endif

/**
 * The role 'CFG_DUMP' prints the parameter of common and all stream objects. This is an option for an ARM tool because
 * ARM can easily print messages
 * this option can only be used if the global define 'USE_MVIF_CFG_DUMP' is define somewhere
 */
#ifdef USE_MVIF_CFG_DUMP
#define MVIF_CFG_DUMP				0x00000004
#endif

/**
 * The role STREAM MANAGER starts and stops the streams and sets dynamic parameter like address and stream id.
 * The high level Audio Manager will use this role
 * this option can only be used if the global define 'USE_MVIF_STREAM_MANAGER' is define somewhere
 */
#ifdef USE_MVIF_STREAM_MANAGER
#define MVIF_STREAM_MANAGER				0x00000008
#endif

/**
 * The role 'CFG_TEST' gets the parameter of common objects. This is an option for an ARM tool
 * this option can only be used if the global define 'USE_MVIF_CFG_TEST' is define somewhere
 */
#ifdef USE_MVIF_CFG_TEST
#define MVIF_CFG_TEST				0x00000010
#endif

/**
 * The role GLOBAL_MEDIA_CLOCK_MASTER provides the Media Clock for the complete Infotainment system
 * Only one device can have this role in the system and this one has to provide the Talker Reference Stream
 * this option can only be used if the global define 'USE_GLOBAL_MEDIA_CLOCK_MASTER' is define somewhere
 */
#ifdef USE_GLOBAL_MEDIA_CLOCK_MASTER
#define GLOBAL_MEDIA_CLOCK_MASTER				0x80000000
#endif

/**
 * The role MVIF_LOCAL_MEDIA_CLOCK_MASTER provides the MVIF clock where the timestamps are derived from
 * this option can only be used if the global define 'USE_MVIF_LOCAL_MEDIA_CLOCK_MASTER' is define somewhere
 */
#ifdef USE_MVIF_LOCAL_MEDIA_CLOCK_MASTER
#define MVIF_LOCAL_MEDIA_CLOCK_MASTER				0x40000000
#endif

// there are MAX_AIF_AREA allowed, each needs a 4 byte tag for access from the upper layer


#define ALL_INDEX				(uint32_t)-1


/*---------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 *--------------------------------------------------------------------*/
int32_t createVideoIFace( uint32_t roleCfg, uint32_t ocmcAddr, uint32_t ocmcSize );
void destroyVideoIFace( void );
void resetMVIFTalkerStream(  uint32_t indx );
uint32_t getMVIFNumberTalkerStream( void );
uint32_t getMVIFSupportedVideoFormatTalker( void );
uint32_t getMVIFTotalTsPtrEntriesTalker( uint32_t indx );
int32_t getMVIFFreeTsPtrEntriesTalker( uint32_t indx );
uint32_t getMVIFMaxPullRateTalker( uint32_t indx );
uint32_t getMVIFMaxPushRateTalker( uint32_t indx );
int32_t pushMVIFTsPtrEntriesTalker( uint32_t indx, uint32_t* arrayTsPtr, uint32_t numTsPtr);
int32_t pullMVIFTsPtrEntriesTalker( uint32_t indx, uint32_t* arrayTsPtr, uint32_t numTsPtr);
uint32_t setMVIFTsPacketSizeTalker( uint32_t indx, uint32_t TsSize);
uint32_t setMVIFStreamStateTalker( uint32_t indx, enum eMVIFStreamState_t state );
uint32_t setMVIFDestAddressTalker( uint32_t indx, uint32_t macHigh, uint32_t macLow );
uint32_t setMVIFStreamIdTalker( uint32_t indx, uint32_t idHigh, uint32_t idLow );
uint32_t getMVIFDestAddressTalker( uint32_t indx, uint32_t* macHigh, uint32_t* macLow );
uint32_t getMVIFStreamIdTalker( uint32_t indx, uint32_t* idHigh, uint32_t* idLow );
enum eMVIFStreamState_t getMVIFStreamStateTalker( uint32_t indx);
uint32_t getMVIFHeaderAddrTalker( uint32_t indx );
uint8_t* getMVIFHeaderPtrTalker( uint32_t indx );
uint32_t getMVIFHeaderSizeTalker( uint32_t indx );

uint32_t getMVIFTsPacketSizeTalker( uint32_t indx );
uint32_t getMVIFStreamAddrTalker( uint32_t indx );
TVideoIFaceCommon* getVideoIFaceCommonObject( void );
int32_t get_freeentries(uint32_t indx);
#endif
