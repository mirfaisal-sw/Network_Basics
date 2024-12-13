/*****************************************************************************
 * Project         BMW IuK MAIF sources
 *
 * (c) copyright   2015
 * Company         Harman/Becker Automotive Systems GmbH
 *                 All rights reserved
 *                 STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file          	iukMAIFRoles.c
 * @ingroup       	driver
 * @author        	Gerd Zimmermann
 * @date			19.11.2015
 *
 * This is an example how all the functions of VideoIfaceConfig.c can be used. The example is specific for QNX,
 * if you use another OS, you have to change QNX specific functions like 'mmap_device_memory'.
 * It is also specific for the BMW IuK Project
 *
 */


/*---------------------------------------------------------------------
 * INCLUDES
 *--------------------------------------------------------------------*/

#ifdef __QNXNTO__
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#endif

#include "global.h"
#include "iukMVIFRoles.h"

/*---------------------------------------------------------------------
 * DEFINES
 *--------------------------------------------------------------------*/
//#define CLEAR_OCMC_MVIF_AREA			/* disable for Multiple Creation Feature */

#ifndef __QNXNTO__
#define MAP_FAILED			0
#endif

#ifndef APP_NAME
#define APP_NAME		"MVIFBMWIUK"
#endif

/*---------------------------------------------------------------------
 * EXTERNALS
 *--------------------------------------------------------------------*/

/*---------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 *--------------------------------------------------------------------*/
static int32_t createTVTunerMVIF( TVideoIFaceCommon *pMVIFCommon, uint32_t memAddr, uint32_t mvifIndex );
static int32_t checkMVIFAvailable( TVideoIFaceCommon *pMVIF, uint32_t timeoutMS );
static int32_t checkMVIFVersion4Reader( uint8_t major, uint8_t minor );
static int32_t checkMVIFTalker( TVideoIFaceCommon *pMVIF, uint32_t ocmcAddr );

/*---------------------------------------------------------------------
 * VARIABLES
 *--------------------------------------------------------------------*/
void* ocmcPtrMVIFArea = MAP_FAILED;
static uint32_t ocmcMVIFAreaSize =  0;
static uint32_t ocmcInfoMAIFAddr =  0;
static uint32_t myRole = 0;
TVideoIFaceCommon* pMVIFCommon = NULL;

#ifdef USE_MVIF_CFG_OWNER
static uint32_t cntMVIFArea = 0;
#endif

/*---------------------------------------------------------------------
 * FUNCTION IMPLEMENTATION
 *--------------------------------------------------------------------*/
/**
 * Function to map the memory and do struct member inits
 * @param roleCfg  MVIF_CFG_OWNER | MVIF_CFG_READER - need to enable appropriate defines on project level
 * @param memAddr  physical address of the TVideoIFaceCommon object
 * @param memSize  Physical memory mapped size
 * @return  -1 error, 0 ok
 */
int32_t createVideoIFace( uint32_t roleCfg, uint32_t memAddr, uint32_t memSize )
{
	if ( ( 0 == memAddr ) || ( 0 == memSize ) )
	{
		return ( -1 );
	}

	if ( MAP_FAILED == ocmcPtrMVIFArea )
	{	// not mapped yet
		#ifdef VIRTUAL_PTR
		#ifdef __QNXNTO__
		ocmcPtrMVIFArea = mmap_device_memory( 0, memSize, PROT_READ | PROT_WRITE | PROT_NOCACHE,0, memAddr );
		#else
		ocmcPtrMVIFArea = (void *)ioremap( memAddr, memSize);

		if ( MAP_FAILED == ocmcPtrMVIFArea )
		{
			return ( -1 );
		}
		if(ocmcPtrMVIFArea == NULL)
        {
          //release_mem_region(memAddr, memSize);
          printk("MVIF : createVideoIFace(): ioremap() failed for addr = %x, size = %d", memAddr, memSize);
          return ( -1 );
        }
		#endif

		if ( MAP_FAILED == ocmcPtrMVIFArea )
		{
			return ( -1 );
		}
		#endif

		ocmcInfoMAIFAddr = memAddr;
		ocmcMVIFAreaSize = memSize;

		pMVIFCommon = (TVideoIFaceStream *)ocmcPtrMVIFArea;
	}

	myRole = roleCfg;		// store my Role for later uasge

	//---------------------------------------------------------------------------------------------------------------------
	if ( roleCfg & MVIF_CFG_OWNER )
	{
		#ifdef CLEAR_OCMC_MVIF_AREA
		memset( ocmcPtrMVIFArea, 0, ocmcMVIFAreaSize);
		#endif

		if ( 0 == createTVTunerMVIF( pMVIFCommon, memAddr, cntMVIFArea ) )
		{
			return -1;
		}
		cntMVIFArea++;
	}

	#ifdef USE_MVIF_CFG_READER
	if ( roleCfg & MVIF_CFG_READER )
	{
		if ( -1 == checkMVIFAvailable( pMVIFCommon, 10*1000 ) )
		{	// AIF not available
			return ( -1 );
		}

		if ( -1 == checkMVIFVersion4Reader( pMVIFCommon->versionMVIF.data[MVIF_VERSION_MAJOR], pMVIFCommon->versionMVIF.data[MVIF_VERSION_MINOR] ) )
		{	// wrong version
			return ( -1 );
		}

		/*
		if ( -1 == checkCommonObject( pMVIFCommon, memAddr ) )
		{
			return ( -1 );
		}
		 */
		if ( -1 == checkMVIFTalker( pMVIFCommon, memAddr ) )
		{
			return ( -1 );
		}

#if 0
		if ( -1 == checkMVIFListener( pMVIFCommon, memAddr ) )
		{
			return ( -1 );
		}
#endif
	}
	#endif

	return (0);
}

/**
 * Function to destroy MVIF usage
 */
void destroyVideoIFace( void )
{
	mvif_Set_Status( pMVIFCommon, 0, MVIF_DESTROYED_BY_OWNER );

	#ifdef VIRTUAL_PTR
	#ifdef __QNXNTO__
	if ( MAP_FAILED != ocmcPtrMVIFArea )
	{
		munmap_device_memory( ocmcPtrMVIFArea, ocmcMVIFAreaSize );		/*lint !e534*/
	}
	#else
	iounmap (ocmcPtrMVIFArea);
	ocmcPtrMVIFArea = NULL;
	#endif
	#endif
}

/**
 * Function to create one MVIF, all Talker TS streams are create here in statically at start up
 * This has to reflect the project needs
 * @param pMVIFCommon virtual pointer of TVideoIFaceCommon object, managed internally in this file
 * @param memAddr  physical address of the TVideoIFaceCommon object
 * @param mvifIndex index of talker stream according to creation sequence
 * @return 0 error; 1 ok
 */
static int32_t createTVTunerMVIF( TVideoIFaceCommon *pMVIFCommon, uint32_t memAddr, uint32_t mvifIndex )
{
	if ( 0 == mvif_Create_Common( pMVIFCommon, memAddr, 0, mvifIndex ) )
	{
		return 0;
	}

	// set values of common members
	mvif_Set_Status( pMVIFCommon, 0, MVIF_CREATION_BY_OWNER );

	// default settings, valid for IaK System
	MVIF_SET_MEDIASAMPLERATE( pMVIFCommon, MVIF_FS_48000 );
	MVIF_SET_MEDIACLOCKTIMER( pMVIFCommon, -1 );
	MVIF_SET_MEDIACLOCKCNT( pMVIFCommon, -1 );
	MVIF_SET_MEDIATIMESTAMP( pMVIFCommon, 0 );
	MVIF_SET_MEDIATIMESEC( pMVIFCommon, 0 );
	MVIF_SET_MEDIATIMENSEC( pMVIFCommon, 0 );
	MVIF_SET_MEMORYVIDEOFORMATTALKER( pMVIFCommon, MVIF_MPEG_TS );

	//TODO: how many talker streams needed? Which size? 188, 196, ...
	// create talker TS stream 1
	if ( 0 == mvif_Create_Talker( pMVIFCommon, 0, 188 ) )
	{
		return 0;
	}
	//TODO: these settings have to come from Audio /Video Management Application, just a default
	mvif_Set_destMacAddressTalker( pMVIFCommon, 0, 0x91E0, 0xF000FE00 );
	mvif_Set_streamIdTalker( pMVIFCommon, 0, 0x91E0F000, 0xFE150106 );
       //mvif_Set_StreamStateTalker( pMVIFCommon, 0 , MVIF_SST_TALKING);
       mvif_Set_StreamStateTalker( pMVIFCommon, 0 , MVIF_SST_IDLE);

	#ifdef MVIF_UNIT_TEST
	// create talker TS stream 2
	if ( 0 == mvif_Create_Talker( pMVIFCommon, 196 ) )
	{
		return 0;
	}
	mvif_Set_destMacAddressTalker( pMVIFCommon, 1, 0x91E0, 0xF000FE00 );
	mvif_Set_streamIdTalker( pMVIFCommon, 1, 0x91E0F000, 0xFE000007 );

	// create talker TS stream 3
	if ( 0 == mvif_Create_Talker( pMVIFCommon, 192 ) )
	{
		return 0;
	}
	mvif_Set_destMacAddressTalker( pMVIFCommon, 2, 0x91E0, 0xF000FE00 );
	mvif_Set_streamIdTalker( pMVIFCommon, 2, 0x91E0F000, 0xFE000008 );
	#endif

	mvif_Set_Status( pMVIFCommon, 1, MVIF_READY_BY_OWNER );
	return 1;
}


/**
 * Function to get the number of created Talker Ts streams
 * @return 0 init not done; > 0 no error
 */
uint32_t getMVIFNumberTalkerStream( void )
{
	return ( pMVIFCommon->numberStreamsTalker );
}

/**
 * Function to get the supported Video Mode of the created MVIF
 * @return 0 init not done; > 0 no error
 */
uint32_t getMVIFSupportedVideoFormatTalker( void )
{
	if ( NULL != pMVIFCommon )
	{
		return ( pMVIFCommon->memoryVideoFormatTalker );
	}

	return 0;
}

/**
 * Function to reset the Ts Physical Pointer management
 * @param indx index of talker stream according to creation sequence
 */
void resetMVIFTalkerStream(  uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Reset_TalkerStream( pMVIFCommon, indx );
	}

	return;
}

/**
 * Function to get the total amount of TS ptr entries.
 * @param indx index of talker stream according to creation sequence
 * @return total number of TS Ptr Entries
 */
uint32_t getMVIFTotalTsPtrEntriesTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_TotalTsPtrEntriesTalker( pMVIFCommon, indx );
	}

	return 0;
}

/**
 * Function to get free TS ptr entries.
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; >= 0 no error
 */
int32_t getMVIFFreeTsPtrEntriesTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_FreeTsPtrEntriesTalker( pMVIFCommon, indx );
	}

	return 0;
}

/**
 * Function to get the maximum possible push amount of Ts Physical Pointer
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t getMVIFMaxPushRateTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_MaxPushRateTalker( pMVIFCommon, indx );
	}

	return 0;

}

/**
 * Function to Push Video Ts Physical Pointer (for application)
 * @param indx index of talker stream according to creation sequence
 * @param arrayTsPtr pointer to an array with sufficient size
 * @param numTsPtr pull count
 * @return -1 init not done; >= 0 no error
 */
int32_t pushMVIFTsPtrEntriesTalker( uint32_t indx, uint32_t* arrayTsPtr, uint32_t numTsPtr)
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Push_TsPtrEntriesTalker( pMVIFCommon, indx, arrayTsPtr, numTsPtr );
	}

	return -1;
}

int32_t get_freeentries(uint32_t indx)
{
        if ( NULL != pMVIFCommon )
        {
                return mvif_Get_FreeTsPtrEntriesTalker(pMVIFCommon, indx);
        }

        return -1;
}


/**
 * Function to get the maximum possible pull amount of Ts Physical Pointer
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t getMVIFMaxPullRateTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_MaxPullRateTalker( pMVIFCommon, indx );
	}

	return 0;
}

/**
 * Function to Pull bunch of Video Ts Physical Pointer(for driver)
 * @param indx index of talker stream according to creation sequence
 * @param arrayTsPtr pointer to an array with sufficient size
 * @param numTsPtr pull count
 * @return -1 init not done; >= 0 no error
 */
int32_t pullMVIFTsPtrEntriesTalker( uint32_t indx, uint32_t* arrayTsPtr, uint32_t numTsPtr)
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Pull_TsPtrEntriesTalker( pMVIFCommon, indx, arrayTsPtr, numTsPtr );
	}

	return -1;
}


/**
 * Function to set dedicated AVB 1722 stream state
 * @param indx index of talker stream according to creation sequence
 * @oparam state  enum eStreamState_t
 * @return 0 init not done; > 0 no error
 */
uint32_t setMVIFStreamStateTalker( uint32_t indx, enum eMVIFStreamState_t state )
{
	if ( NULL != pMVIFCommon )
	{
		mvif_Set_StreamStateTalker( pMVIFCommon, indx, state );
		return 1;
	}

	return 0;
}

/**
 * Function to set dedicated AVB 1722 Destination MAC Address
 * @param indx index of talker stream according to creation sequence
 * @param macHigh  BE:  0,     0,    mac[0], mac[1]
 * @param macLow   BE: mac[2],mac[3], mac[4], mac[5]
 * @return 0 init not done; > 0 no error
 */
uint32_t setMVIFDestAddressTalker( uint32_t indx, uint32_t macHigh, uint32_t macLow )
{
	if ( NULL != pMVIFCommon )
	{
		mvif_Set_destMacAddressTalker( pMVIFCommon, indx, macHigh, macLow );
		return 1;
	}

	return 0;
}

/**
 * Function to set dedicated AVB 1722 Stream Id
 * @param indx    index of talker stream according to creation sequence
 * @param idHigh  BE: id[0],id[1],id[2],id[3]
 * @param idLow   BE: id[4],id[5],id[6],id[7]
 * @return 0 init not done; > 0 no error
 */
uint32_t setMVIFStreamIdTalker( uint32_t indx, uint32_t idHigh, uint32_t idLow )
{
	if ( NULL != pMVIFCommon )
	{
		mvif_Set_streamIdTalker( pMVIFCommon, indx, idHigh, idLow );
		return 1;
	}

	return 0;
}

/**
 * Function to set dedicated AVB Packet Ts Size
 * @param indx    index of talker stream according to creation sequence
 * @param TsSize  TsSize
 * @return 0 init not done; > 0 no error
 */
uint32_t setMVIFTsPacketSizeTalker( uint32_t indx, uint32_t TsSize)
{
	if ( NULL != pMVIFCommon )
	{
		mvif_Set_TsPacketSizeTalker( pMVIFCommon, indx, TsSize );
		return 1;
	}

	return 0;
}



/**
 * Function to set dedicated AVB 1722 Destination MAC Address
 * @param indx index of talker stream according to creation sequence
 * @param macHigh  ptr location filled with BE:  0,     0,    mac[0], mac[1]
 * @param macLow   ptr location filled with BE: mac[2],mac[3], mac[4], mac[5]
 * @return 0 init not done; > 0 no error
 */
uint32_t getMVIFDestAddressTalker( uint32_t indx, uint32_t* macHigh, uint32_t* macLow )
{
	if ( NULL != pMVIFCommon )
	{
		mvif_Get_destMacAddressTalker( pMVIFCommon, indx, macHigh, macLow );
		return 1;
	}

	return 0;
}

/**
 * Function to get dedicated AVB 1722 State
 * @param indx index of talker stream according to creation sequence
 * @param macHigh  ptr location filled with BE:  0,     0,    mac[0], mac[1]
 * @param macLow   ptr location filled with BE: mac[2],mac[3], mac[4], mac[5]
 * @return 0 init not done; > 0 no error
 */
enum eMVIFStreamState_t getMVIFStreamStateTalker( uint32_t indx)
{
	enum eMVIFStreamState_t state;
	if ( NULL != pMVIFCommon )
	{
		state = mvif_Get_StreamStateTalker( pMVIFCommon, indx);
		return state;
		//return 1;
	}

	return 0;
}

/**
 * Function to set dedicated AVB 1722 Stream Id
 * @param indx    index of talker stream according to creation sequence
 * @param idHigh  ptr location filled with BE: id[0],id[1],id[2],id[3]
 * @param idLow   ptr location filled with BE: id[4],id[5],id[6],id[7]
 * @return 0 init not done; > 0 no error
 */
uint32_t getMVIFStreamIdTalker( uint32_t indx, uint32_t* idHigh, uint32_t* idLow )
{
	if ( NULL != pMVIFCommon )
	{
		mvif_Get_streamIdTalker( pMVIFCommon, indx, idHigh, idLow );
		return 1;
	}

	return 0;
}


/**
 * Function to get physical address of reserved 1722 header range
 * @param indx    index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t getMVIFHeaderAddrTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_HeaderAddrTalker( pMVIFCommon, indx );
	}

	return 0;
}

/**
 * Function to get physical address of reserved 1722 header range
 * @param indx    index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint8_t* getMVIFHeaderPtrTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_HeaderPtrTalker( pMVIFCommon, indx );
	}

	return 0;
}

/**
 * Function to get physical address of reserved 1722 header range
 * @param indx    index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t getMVIFHeaderSizeTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_HeaderSizeTalker( pMVIFCommon, indx );
	}

	return 0;
}

/**
 * Function to get packet size supported by MVIF
 * @param indx    index of talker stream according to creation sequence
 * @return supported packet size
 */
uint32_t getMVIFTsPacketSizeTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_TsPacketSizeTalker( pMVIFCommon, indx );
	}

	return 0;
}

/**
 * Function to get physical stream address of each Talker stream
 * @param indx    index of talker stream according to creation sequence
 * @return supported packet size
 */
uint32_t getMVIFStreamAddrTalker( uint32_t indx )
{
	if ( NULL != pMVIFCommon )
	{
		return mvif_Get_StreamAddrTalker( pMVIFCommon, indx );
	}

	return 0;
}

TVideoIFaceCommon* getVideoIFaceCommonObject( void )
{
	return pMVIFCommon;
}

#ifdef USE_MVIF_CFG_READER
/**
 * wait until CFG_OWNER has build up the MAIF. Use a real function for usleep(), otherwise some counter fakes a time period
 * @param pAIFCommon (virtual) address where to create the MAIF
 * @param timeoutMS maximum wait time until MAIF is set to READY_BY_OWNER state
 * @return: 0: MAIF ready; -1: error
 */
static int32_t checkMVIFAvailable( TVideoIFaceCommon *pMVIF, uint32_t timeoutMS )
{
	uint32_t i;

	if ( (uint32_t)MVIF_READY_BY_OWNER == pMVIF->statusMVIF )
	{
		return 0;
	}
	else
	{
		if ( timeoutMS < 10 )
		{
			timeoutMS = 10;
		}

		for (i=0; i<timeoutMS/10; i++)
		{
#ifdef __QNX__
			usleep(10 * 1000);
#else
			//Task_sleep(10 * 1000);
#endif

			if ( (uint32_t)MVIF_READY_BY_OWNER == pMVIF->statusMVIF )
			{
				return 0;
			}
		}
	}

    printk("%s: checkMVIFAvailable() Timeout\n", APP_NAME);

	return -1;
}

/**
 * check major and minor MAIF version CFG_OWNER has used. If major is not equal, you can't use MAIF.
 * If minor is not equal, you can make adaption here in
 * @param major major version of CFG_OWNER
 * @param minor minor version of CFG_OWNER
 * @return: 0: ok; -1: error
 */
static int32_t checkMVIFVersion4Reader( uint8_t major, uint8_t minor )
{
	return 0;
}



/**
 * check all Talker Objects for valid parameter and register them inside internal management structure
 * @param pAIFCommon (virtual) address where the Common Object is located
 * @param ocmcAddr physical address where the Common Object is located
 */
static int32_t checkMVIFTalker( TVideoIFaceCommon *pMVIF, uint32_t ocmcAddr )
{
	uint32_t i;
	uint32_t cnt;
	uint32_t offset;
	TVideoIFaceStream *pStream;

	if ( NULL == pMVIF )
	{
		return (-1);
	}

	cnt = pMVIF->numberStreamsTalker;

	offset = pMVIF->addrStreamsTalker - ocmcAddr;

	pStream = (TVideoIFaceCommon *)( (uint8_t*)pMVIF + offset );		/*lint !e124*/

	if ( NULL == pStream )
	{
		return ( -1 );
	}

	for (i=0; i<cnt;i++)
	{
		//printf("%s: Talker[%d]: check values at OCMC addr %08X\n", APP_NAME, i, ocmcAddr + offset );

		mvif_Register_Talker( pMVIF, i, pStream, ocmcAddr + offset );

		//printf("%s: Talker[%d]: check passed\n", APP_NAME, i );

		//if ( 0 == pStream->addrNextStream) { printf("%s: Talker[%d]: last stream \n", APP_NAME, i );}
		//else printf("%s: Talker[%d]: next stream       %08X \n", APP_NAME, i, pStream->addrNextStream );

		// calculate address of next stream
		offset = pStream->addrNextStream - ocmcAddr;
		pStream = (TVideoIFaceCommon *)( (uint8_t*)pMVIF + offset );		/*lint !e124*/
	}

	return 0;
}

#endif


