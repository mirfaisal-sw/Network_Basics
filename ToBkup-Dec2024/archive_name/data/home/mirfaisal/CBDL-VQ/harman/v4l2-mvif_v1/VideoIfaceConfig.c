/*****************************************************************************
 * Project         AVB MVIF
 *
 * (c) copyright   2015
 * Company         Harman/Becker Automotive Systems GmbH
 *                 All rights reserved
 *                 STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file          	VideoIfaceConfig.c
 * @ingroup       	driver
 * @author        	Heramb Kuwelkar, Gerd Zimmermann
 * @date			19.11.2015
 *
 * This are the functions to access the members of the common and Talker/Listener structures
 *
 * 1.1 	new feature: AVTP IEC61883-4 avtp source packet header ts support
 * 					mvif_Create_Talker() updated with ts packet size of one packet
 * 1.2  bugfix internal buffer management
 *
 * 1.3 new feature for multiple creation of MVIF
 */

/*---------------------------------------------------------------------
 * INCLUDES
 *--------------------------------------------------------------------*/
/**
 * Header file for adaption to different cores and OS and data types
 */
#include "VideoIfaceIncludes.h"

/*---------------------------------------------------------------------
 * DEFINES
 *--------------------------------------------------------------------*/
#define BIT_PER_BYTE			8
/*---------------------------------------------------------------------
 * EXTERNALS
 *--------------------------------------------------------------------*/

/*---------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 *--------------------------------------------------------------------*/
static int32_t PushVideoTs( TVideoIFaceStream *pStream, uint32_t * addrvideobufStart, uint32_t * videoAddresses, int pushts);
static inline uint32_t getwriteIdx( TVideoIFaceStream *pStream);
static inline uint32_t getgroupwriteIdx( TVideoIFaceStream *pStream);
static inline uint32_t getreadIdx(TVideoIFaceStream *pStream);
static inline uint32_t getgroupreadIdx(TVideoIFaceStream *pStream);
static uint32_t incWriteIdx(TVideoIFaceStream *pStream, uint32_t numVideoTs);
uint32_t incGrpWriteIdx(TVideoIFaceStream *pStream);
static int32_t PullVideoTs(TVideoIFaceStream *pStream, uint32_t * addrvideobufStart, uint32_t *videoAddresses,int pullts);
static uint32_t getreadCount(TVideoIFaceStream *pStream,int pullts);
static uint32_t getgroupreadCount(TVideoIFaceStream *pStream);
static inline void incReadIdx(TVideoIFaceStream *pStream );
static inline void incGrpReadIdx(TVideoIFaceStream *pStream);

/*---------------------------------------------------------------------
 * VARIABLES
 *--------------------------------------------------------------------*/
static void* pOcmcCommon =  NULL;
static uint32_t ocmcAddr = 0;
static TVideoTalkerListener Talker[MAX_MVIF_AREA][NUM_VIDEO_TALKER_MAX];
static uint32_t arrayCntTalker[MAX_MVIF_AREA] = { 0 };
static TVideoTalkerListener Listener[MAX_MVIF_AREA][NUM_VIDEO_LISTENER_MAX];
static uint32_t arrayCntListener[MAX_MVIF_AREA] = { 0 };
static uint32_t nextFreeOcmcAddr = 0;

/*---------------------------------------------------------------------
 * FUNCTION IMPLEMENTATION
 *--------------------------------------------------------------------*/

/**
 * Initialization of struct TVideoIFaceCommon and Talker/Listener management
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param physOcmc physical address of the TVideoIFaceCommon object
 * @param tag TVideoIFaceCommon.tag
 * @param mvifIndex index 0..x for number of different MVIFs
 * @return
 */
uint32_t mvif_Create_Common( TVideoIFaceCommon *pCommon, uint32_t physOcmc, uint32_t tag, uint32_t mvifIndex )
{
	uint32_t i;

	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( mvifIndex >= MAX_MVIF_AREA )
	{
		return 0;
	}

	pOcmcCommon = (void* )pCommon;
	ocmcAddr = physOcmc;

	memset( (void*)pCommon, 0, sizeof(TVideoIFaceCommon) );
	pCommon->tag = tag;
	pCommon->index = mvifIndex;

	pCommon->versionMVIF.data[MVIF_VERSION_MAJOR] = MY_MVIF_VERSION_MAJOR;
	pCommon->versionMVIF.data[MVIF_VERSION_MINOR] = MY_MVIF_VERSION_MINOR;

	pCommon->mediaClockTimer = 0;
	pCommon->mediaClockCnt = -1;
	pCommon->mediaTimestamp = 0;
	pCommon->mediaTimeSec = 0;
	pCommon->mediaTimeNSec = 0;

	arrayCntTalker[pCommon->index] = 0;
	for (i=0; i<NUM_VIDEO_TALKER_MAX; i++)
	{
		memset( (void*)&Talker[pCommon->index][i], 0, sizeof(TVideoTalkerListener) );
	}

	arrayCntListener[pCommon->index] = 0;
	for (i=0; i<NUM_VIDEO_LISTENER_MAX; i++)
	{
		memset( (void*)&Listener[pCommon->index][i], 0, sizeof(TVideoTalkerListener) );
	}

	nextFreeOcmcAddr = physOcmc + sizeof(TVideoIFaceCommon);

	return ocmcAddr;
}

/**
 * Initialization of struct TVideoIFaceCommon and Talker/Listener management
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param physOcmc physical address of the TVideoIFaceCommon object
 * @return
 */
void mvif_Register_Common( TVideoIFaceCommon *pCommon, uint32_t physOcmc )
{
	if ( NULL == pCommon )
	{
		return;
	}

	pOcmcCommon = (void*)pCommon;
	ocmcAddr = physOcmc;

	nextFreeOcmcAddr = physOcmc + pCommon->sizeTotalMemInByte;
}

/**
 * Registration of a Talker object in the talker local management structure Talker[]
 * You can read physical Addr/virtual Ptr/size with the following functions: mvif_Get_addrBufferTalker/mvif_Get_ptrBufferTalker/mvif_Get_sizeBufferTalker
 * Hint: this function has to be called with indx=0 to numberStreamsTalker incrementally
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of stream
 * @param pStream virtual pointer of TVideoIFaceStream object
 * @param physAddr physical address of the Talker object
 * @return
 */
void mvif_Register_Talker( TVideoIFaceCommon *pCommon, uint32_t indx, TVideoIFaceStream* pStream, uint32_t physAddr )
{
	if ( ( NULL == pCommon ) || ( NULL == pStream )  )
	{
		return;
	}

	arrayCntTalker[pCommon->index]++;
	Talker[pCommon->index][indx].ocmcAddr = physAddr;					// new talker
	Talker[pCommon->index][indx].pStream = (void*)pStream;
	Talker[pCommon->index][indx].sizeTsPacket = pStream->sizeTsPacket;

	Talker[pCommon->index][indx].addrvideobufStart = (uint32_t*)&pStream->arrayTSPointer[0];

	Talker[pCommon->index][indx].physVideoBuffer = (uint32_t)Talker[pCommon->index][indx].addrvideobufStart;
	Talker[pCommon->index][indx].physVideoBuffer -= (uint32_t)pStream;
	Talker[pCommon->index][indx].physVideoBuffer += Talker[pCommon->index][indx].ocmcAddr;

	Talker[pCommon->index][indx].pVideoBuffer = Talker[pCommon->index][indx].addrvideobufStart;
	Talker[pCommon->index][indx].sizeVideoBuffer = pStream->buflength * sizeof(uint32_t*);

	Talker[pCommon->index][indx].physHeaderBuffer = (uint32_t)&pStream->avtpHeader[0];
	Talker[pCommon->index][indx].physHeaderBuffer -= (uint32_t)pStream;
	Talker[pCommon->index][indx].physHeaderBuffer += Talker[pCommon->index][indx].ocmcAddr;

	Talker[pCommon->index][indx].pHeaderBuffer = (uint32_t*)&pStream->avtpHeader[0];
	Talker[pCommon->index][indx].sizeHeaderBuffer = _1722_HEADER_SIZE;

	Talker[pCommon->index][indx].physAvtpSourceHeaderBuffer = (uint32_t)&pStream->avtpSourceHeader[0];
	Talker[pCommon->index][indx].physAvtpSourceHeaderBuffer -= (uint32_t)pStream;
	Talker[pCommon->index][indx].physAvtpSourceHeaderBuffer += Talker[pCommon->index][indx].ocmcAddr;

	Talker[pCommon->index][indx].pAvtpSourceHeaderBuffer = (uint32_t*)&pStream->avtpSourceHeader[0];
	Talker[pCommon->index][indx].sizeAvtpSourceHeaderBuffer = sizeof(uint32_t) * NUM_AVTP_SOURCE_HEADER;

}


/**
 *
 * @param pCommon virtual pointer of TAudioIFaceCommon object
 * @param creationDone all Talker and Listener Streams are created - 1
 * @param mvifStatus enum eStatusMVIF_t
 */
void mvif_Set_Status( TVideoIFaceCommon *pCommon, uint32_t creationDone, enum eStatusMVIF_t mvifStatus )
{
	uint32_t i;
	uint32_t cnt = 0;
	volatile TVideoIFaceStream *pStream = NULL;

	if ( NULL == pCommon )
	{
		return;
	}
	pCommon->statusMVIF = mvifStatus;

	if ( ( 0 != creationDone ) && ( mvifStatus == MVIF_READY_BY_OWNER) )
	{	// this is the only trigger where we know that all Talker/Listener are created
		cnt = arrayCntTalker[pCommon->index];
		if ( cnt > 1 )
		{
			pStream = Talker[pCommon->index][cnt-1].pStream;
			pStream->addrNextStream = 0;						/* last stream */

			pStream = Talker[pCommon->index][0].pStream;
			for (i=1; i<(cnt-1); i++)
			{	// we have to update the stream address chain from first to last Talker
				pStream->addrNextStream = Talker[pCommon->index][i].ocmcAddr;
				pStream = Talker[pCommon->index][i].pStream;
			}

			#if 0
			for (i=0; i<cnt; i++)
			{	// we have to update the stream address chain from first to last Talker
				pStream = Talker[pCommon->index][i].pStream;
				logInfo("%s: Talker[%d]: 0x%08X->addrNextStream  = 0x%08X \n", APP_NAME, i, Talker[pCommon->index][i].ocmcAddr, pStream->addrNextStream );
			}
			#endif
		}

		cnt = arrayCntListener[pCommon->index];
		if ( cnt > 1 )
		{
			pStream = Listener[pCommon->index][cnt-1].pStream;
			pStream->addrNextStream = 0;						/* last stream */

			pStream = Listener[pCommon->index][0].pStream;
			for (i=1; i<(cnt-1); i++)
			{	// we have to update the stream address chain from first to last Listener
				pStream->addrNextStream = Listener[pCommon->index][i].ocmcAddr;
				pStream = Listener[pCommon->index][i].pStream;
			}
		}
	}
}


/**
 * Creation of a Talker object.
 * You can read physical Addr/virtual Ptr/size with the following functions: aif_Get_addrBufferTalker/aif_Get_ptrBufferTalker/aif_Get_sizeBufferTalker
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param handle TAudioIFaceStream.handle
 * @return physical address of the Talker object; 0= error occured, could be run out of OCMC memory
 */
uint32_t mvif_Create_Talker( TVideoIFaceCommon *pCommon,  uint32_t handle, uint32_t sizeTsPacket )
{
	uint32_t offsetTalker = 0;
	uint32_t cntTalker = 0;
	volatile TVideoIFaceStream *pStream = NULL;

	if ( NULL == pCommon )
	{
		return 0;
	}

	cntTalker = arrayCntTalker[pCommon->index];

	if ( ( NUM_VIDEO_TALKER_MAX == cntTalker ) || ( 0 == ocmcAddr) || ( NULL == pOcmcCommon) )
	{
		return 0;
	}

	offsetTalker = nextFreeOcmcAddr - ocmcAddr;

	if ( 0 == cntTalker )
	{	// this is the first Talker
		Talker[pCommon->index][0].ocmcAddr = nextFreeOcmcAddr;
		#ifdef VIRTUAL_PTR
		Talker[pCommon->index][0].pStream = pOcmcCommon + offsetTalker;		/*lint !e124*/
		#else
		Talker[pCommon->index][0].pStream = (void*)nextFreeOcmcAddr;
		#endif
		pStream = (TVideoIFaceStream *)Talker[pCommon->index][0].pStream;
		pCommon->addrStreamsTalker = Talker[pCommon->index][0].ocmcAddr;
	}
	else
	{
		pStream = (TVideoIFaceStream *)Talker[pCommon->index][cntTalker-1].pStream;		// last existing stream
		Talker[pCommon->index][cntTalker].ocmcAddr = nextFreeOcmcAddr;					// new talker
		pStream->addrNextStream = Talker[pCommon->index][cntTalker].ocmcAddr;			// set new talker in last stream
		#ifdef VIRTUAL_PTR
		Talker[pCommon->index][cntTalker].pStream = pOcmcCommon + offsetTalker;	/*lint !e124*/
		#else
		Talker[pCommon->index][cntTalker].pStream = (void*)nextFreeOcmcAddr;
		#endif
		pStream = (TVideoIFaceStream *)Talker[pCommon->index][cntTalker].pStream;
	}

	pStream->index = cntTalker;
	pStream->handle = handle;
	pStream->counter1722Msg = 0;

	pStream->kindOfStream = (uint32_t)MVIF_TALKER_STREAM;
	pStream->srcMacHigh = 0xFFFFFFFF;		// that we un-initialized streams
	pStream->srcMacLow = 0xFFFFFFFF;
	pStream->destMacHigh = 0xFFFFFFFF;		// that we un-initialized streams
	pStream->destMacLow = 0xFFFFFFFF;
	pStream->streamIdHigh = 0xFFFFFFFF;
	pStream->streamIdLow = 0xFFFF0000 + cntTalker;
	pStream->maxTransitTime = 15000000;
	pStream->vlanId = 0;

	pStream->modeVideoBuffer = (uint32_t)MVIF_BM_TS_PTR_RINGBUF;
	pStream->numVideoBuffer = NUM_TS_PTR_ENTRIES;
	pStream->sizeTsPacket = sizeTsPacket;

	// Heramb
	pStream->buflength = NUM_TS_PTR_ENTRIES;
	pStream->wrIdx = 0;
	pStream->rdIdx = 0;
	pStream->wraparound = 0;
	pStream->overwrite = 0;

	pStream->grouplength = NUM_GROUP_ENTRIES;
	pStream->grpwrIdx = 0;
	pStream->grprdIdx = 0;
	pStream->grpwraparound = 0;

	Talker[pCommon->index][cntTalker].addrvideobufStart = (uint32_t*)&pStream->arrayTSPointer[0];

	Talker[pCommon->index][cntTalker].sizeTsPacket = pStream->sizeTsPacket;

	Talker[pCommon->index][cntTalker].physVideoBuffer = (uint32_t)Talker[pCommon->index][cntTalker].addrvideobufStart;
	Talker[pCommon->index][cntTalker].physVideoBuffer -= (uint32_t)pStream;
	Talker[pCommon->index][cntTalker].physVideoBuffer += Talker[pCommon->index][cntTalker].ocmcAddr;

	Talker[pCommon->index][cntTalker].pVideoBuffer = Talker[pCommon->index][cntTalker].addrvideobufStart;
	Talker[pCommon->index][cntTalker].sizeVideoBuffer = pStream->buflength * sizeof(uint32_t*);

	// clear stream structure
	memset( (void*)Talker[pCommon->index][cntTalker].pVideoBuffer, 0, Talker[pCommon->index][cntTalker].sizeVideoBuffer );

	Talker[pCommon->index][cntTalker].physHeaderBuffer = (uint32_t)&pStream->avtpHeader[0];
	Talker[pCommon->index][cntTalker].physHeaderBuffer -= (uint32_t)pStream;
	Talker[pCommon->index][cntTalker].physHeaderBuffer += Talker[pCommon->index][cntTalker].ocmcAddr;

	Talker[pCommon->index][cntTalker].pHeaderBuffer = (uint32_t*)&pStream->avtpHeader[0];
	Talker[pCommon->index][cntTalker].sizeHeaderBuffer = _1722_HEADER_SIZE;


	Talker[pCommon->index][cntTalker].physAvtpSourceHeaderBuffer = (uint32_t)&pStream->avtpSourceHeader[0];
	Talker[pCommon->index][cntTalker].physAvtpSourceHeaderBuffer -= (uint32_t)pStream;
	Talker[pCommon->index][cntTalker].physAvtpSourceHeaderBuffer += Talker[pCommon->index][cntTalker].ocmcAddr;

	Talker[pCommon->index][cntTalker].pAvtpSourceHeaderBuffer = (uint32_t*)&pStream->avtpSourceHeader[0];
	Talker[pCommon->index][cntTalker].sizeAvtpSourceHeaderBuffer = sizeof(uint32_t) * NUM_AVTP_SOURCE_HEADER;

	nextFreeOcmcAddr += sizeof(TVideoIFaceStream );
	pCommon->sizeTotalMemInByte += sizeof(TVideoIFaceStream );
	pCommon->numberStreamsTalker++;
	cntTalker++;
	arrayCntTalker[pCommon->index] = cntTalker;

	pStream->state = (uint32_t)MVIF_SST_INITIALIZED;

	return Talker[pCommon->index][cntTalker-1].ocmcAddr;
}

/**
 * set Destination MAC address of a Stream
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx the index starts at 0 and is increment with the creation of each Talker
 * @param macHigh  BE:  0,     0,    mac[0], mac[1]
 * @param macLow   BE: mac[2],mac[3], mac[4], mac[5]
 * @return
 */
void mvif_Set_destMacAddressTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t macHigh, uint32_t macLow )
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	pStream->destMacHigh = macHigh;
	pStream->destMacLow = macLow;
}

/**
 * set StreamId of a Stream
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx the index starts at 0 and is increment with the creation of each Talker
 * @param idHigh  BE: id[0],id[1],id[2],id[3]
 * @param idLow   BE: id[4],id[5],id[6],id[7]
 * @return
 */
void mvif_Set_streamIdTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t idHigh, uint32_t idLow )
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	pStream->streamIdHigh = idHigh;
	pStream->streamIdLow = idLow;
}

/**
 * get StreamId of a Stream
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx the index starts at 0 and is increment with the creation of each Talker
 * @param idHigh  ptr location filled with BE: id[0],id[1],id[2],id[3]
 * @param idLow   ptr location filled with BE: id[4],id[5],id[6],id[7]
 * @return
 */
void mvif_Get_streamIdTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t* idHigh, uint32_t* idLow )
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	*idHigh = pStream->streamIdHigh;
	*idLow = pStream->streamIdLow;
}

/**
 * set Destination MAC address of a Stream
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx the index starts at 0 and is increment with the creation of each Talker
 * @param macHigh  BE:  0,     0,    mac[0], mac[1]
 * @param macLow   BE: mac[2],mac[3], mac[4], mac[5]
 * @return
 */
void mvif_Get_destMacAddressTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t* macHigh, uint32_t* macLow )
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	*macHigh = pStream->destMacHigh;
	*macLow = pStream->destMacLow;
}

/**
 * Function to set dedicated AVB 1722 stream state
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @oparam state  enum eStreamState_t
 * @return
 */
void mvif_Set_StreamStateTalker( TVideoIFaceCommon *pCommon, uint32_t indx , enum eMVIFStreamState_t state)
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	pStream->state =  state;
}

/**
 * set Packet Ts Size
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx the index starts at 0 and is increment with the creation of each Talker
 * @param TsSize  Packet Ts Size
 * @return
 */
void mvif_Set_TsPacketSizeTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t sizeTsPacket )
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	pStream->sizeTsPacket = sizeTsPacket;

	Talker[pCommon->index][indx].sizeTsPacket = pStream->sizeTsPacket;
}



/**
 * Function to get dedicated AVB 1722 stream state
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @oparam state  enum eStreamState_t
 * @return
 */
enum eMVIFStreamState_t mvif_Get_StreamStateTalker( TVideoIFaceCommon *pCommon, uint32_t indx)
{
	TVideoIFaceStream *pStream;
	enum eMVIFStreamState_t state;

	if ( NULL == pCommon )
	{
		return;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	state = pStream->state;
	return state;
}

/**
 * Function to reset the Ts Physical Pointer management
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 */
void mvif_Reset_TalkerStream( TVideoIFaceCommon *pCommon, uint32_t indx )
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	pStream->counter1722Msg = 0;

	pStream->buflength = NUM_TS_PTR_ENTRIES;
	pStream->wrIdx = 0;
	pStream->rdIdx = 0;
	pStream->wraparound = 0;
	pStream->overwrite = 0;
	Talker[pCommon->index][indx].addrvideobufStart = (uint32_t*)&pStream->arrayTSPointer[0];
}

/**
 * Function to get the total amount of TS ptr entries.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return total number of TS Ptr Entries
 */
uint32_t mvif_Get_TotalTsPtrEntriesTalker( TVideoIFaceCommon *pCommon, uint32_t indx )
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	return pStream->numVideoBuffer;
}

/**
 * Function to get free TS ptr entries.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 in case of error; >0 no error
 */
uint32_t mvif_Get_FreeTsPtrEntriesTalker(TVideoIFaceCommon *pCommon, uint32_t indx )
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	if ( pStream->wrIdx == pStream->rdIdx )
	{	// could also be overwrite conditions! need additional frlag for that!
		return pStream->numVideoBuffer;
	}
	else
	{
		if(pStream->wrIdx > pStream->rdIdx )
		{
			return (pStream->numVideoBuffer - ( pStream->wrIdx - pStream->rdIdx ) );
		}
		else
		{
			return ( pStream->rdIdx - pStream->wrIdx );
		}
	}
	return 0;
}

/**
 * Function to get the maximum possible push amount of Ts Physical Pointer
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t mvif_Get_MaxPushRateTalker(TVideoIFaceCommon *pCommon, uint32_t indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return NUM_TS_PTR_ENTRIES;
}

/**
 * Function to Push Video Ts Physical Pointer (for application)
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @param arrayTsPtr pointer to an array with sufficient size
 * @param numTsPtr pull count
 * @return -1 init not done; >= 0 no error
 */
int32_t mvif_Push_TsPtrEntriesTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t* arrayTsPtr, uint32_t numTsPtr)
{
	TVideoIFaceStream *pStream;

	if ( NULL == pCommon )
	{
		return -1;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return -1;
	}

	if ( numTsPtr > NUM_TS_PTR_ENTRIES )
	{
		return -1;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	pStream->pushtime32 = pCommon->dummy10;

	if ( 1 == PushVideoTs( pStream, Talker[pCommon->index][indx].addrvideobufStart, arrayTsPtr, numTsPtr ) )
	{
		return mvif_Get_FreeTsPtrEntriesTalker( pCommon, indx );

	}

	return 0;
}

/**
 * Function to get the maximum possible pull amount of Ts Physical Pointer
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t mvif_Get_MaxPullRateTalker(TVideoIFaceCommon *pCommon, uint32_t indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return NUM_TS_PTR_CLASSC_MAXMTU;
}

/**
 * Function to Pull bunch of Video Ts Physical Pointer(for driver)
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @param arrayTsPtr pointer to an array with sufficient size
 * @param numTsPtr pull count
 * @return -1 init not done; >= 0 no error
 */
int32_t mvif_Pull_TsPtrEntriesTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t* arrayTsPtr, uint32_t numTsPtr)
{
	TVideoIFaceStream *pStream;
    uint32_t numTsPacket;
	if ( NULL == pCommon )
	{
		return -1;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return -1;
	}

	if ( ( numTsPtr > NUM_TS_PTR_CLASSC_MAXMTU ) || ( 0 == numTsPtr ) )
	{
		numTsPtr = NUM_TS_PTR_CLASSC_MAXMTU;
	}

	pStream = Talker[pCommon->index][indx].pStream;

	numTsPacket = PullVideoTs( pStream, Talker[pCommon->index][indx].addrvideobufStart, arrayTsPtr,numTsPtr);

    pCommon->dummy9 = pStream->pulltime32;

	//return PullVideoTs( pStream, Talker[pCommon->index][indx].addrvideobufStart, arrayTsPtr,numTsPtr);
	return numTsPacket;
}

/**
 * Function to get Physical Address of reserved 1722 header space
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t mvif_Get_HeaderAddrTalker( TVideoIFaceCommon *pCommon, UInt32 indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return ( Talker[pCommon->index][indx].physHeaderBuffer );
}

/**
 * Function to get virtual Pointer of reserved 1722 header space
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint8_t* mvif_Get_HeaderPtrTalker( TVideoIFaceCommon* pCommon, UInt32 indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return ( (uint8_t*)Talker[pCommon->index][indx].pHeaderBuffer );
}

/**
 * Function to get size of reserved 1722 header space
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 size in byte
 */
uint32_t mvif_Get_HeaderSizeTalker( TVideoIFaceCommon *pCommon, UInt32 indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return ( Talker[pCommon->index][indx].sizeHeaderBuffer );
}

/**
 * Function to get Physical Address of reserved 1722 avtp_source_packet_header space
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t mvif_Get_AvtpSourceHeaderAddrTalker( TVideoIFaceCommon *pCommon, UInt32 indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return ( Talker[pCommon->index][indx].physAvtpSourceHeaderBuffer );
}

/**
 * Function to get virtual Pointer of reserved 1722 avtp_source_packet_header space
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint8_t* mvif_Get_AvtpSourceHeaderPtrTalker( TVideoIFaceCommon* pCommon, UInt32 indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return ( (uint8_t*)Talker[pCommon->index][indx].pAvtpSourceHeaderBuffer );
}

/**
 * Function to get size of reserved 1722 avtp_source_packet_header space
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 size in byte
 */
uint32_t mvif_Get_AvtpSourceHeaderSizeTalker( TVideoIFaceCommon *pCommon, UInt32 indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return ( Talker[pCommon->index][indx].sizeAvtpSourceHeaderBuffer );
}

/**
 * Function to get Physical Address of each Talker TVideoIFaceStream
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t mvif_Get_StreamAddrTalker( TVideoIFaceCommon *pCommon, UInt32 indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return ( Talker[pCommon->index][indx].ocmcAddr );
}

/**
 * Function to get TS Packet size (one) of given Talker Stream
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param indx index of talker stream according to creation sequence
 * @return 0 init not done; > 0 no error
 */
uint32_t mvif_Get_TsPacketSizeTalker( TVideoIFaceCommon *pCommon, UInt32 indx )
{
	if ( NULL == pCommon )
	{
		return 0;
	}

	if ( indx >= arrayCntTalker[pCommon->index] )
	{
		return 0;
	}

	return ( Talker[pCommon->index][indx].sizeTsPacket );
}

/**
 * Function to Push Video Ts.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param Push count
 */
static int32_t PushVideoTs( TVideoIFaceStream *pStream, uint32_t * addrvideobufStart, uint32_t * videoAddresses, int pushts)
{
	int32_t i = 0;
   //int j = 0;
   //Check the indexes first.
   uint32_t wridx;
   //uint32_t slots;
   int32_t retval = 1;
   if(pushts>0)
   {
     pStream->arrayTSBunch[pStream->grpwrIdx].grpsrtIdx = getwriteIdx(pStream);
     for (i=0;i<pushts;i++)
     {
       wridx = getwriteIdx(pStream);
     /* Check if overwritting is happening.Even if write is crossing read then below condition will occur when we
     indicate that there is overwriting happening but we continue writing.*/
       if((pStream->wraparound)&&(wridx>=pStream->rdIdx))
       {
    	 pStream->overwrite++;
         retval = -1;
       }

       addrvideobufStart[wridx] = videoAddresses[i];
     //printk("Address stored [%d]= %x \n",wridx,pVIFCommon->addrvideobufStart[wridx]);
       incWriteIdx(pStream,1);
     }
     if(retval == -1)
     {
        /*This logic needs review and through test.Where does read index moves when there is overwrite
        As of now read is moved to write location as it has to read previous data*/
        if (pStream->wraparound)
        {
         pStream->rdIdx = pStream->wrIdx;
        }
     }
     pStream->arrayTSBunch[pStream->grpwrIdx].grpendIdx = getwriteIdx(pStream) -1;
     pStream->arrayTSBunch[pStream->grpwrIdx].grpts = pStream->pushtime32;
     incGrpWriteIdx(pStream);
   }

   return retval;
}

/**
 * Function to get write Index
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * return WriteIdx
 */
static inline uint32_t getwriteIdx( TVideoIFaceStream *pStream)
{
    return pStream->wrIdx;
}

/**
 * Function to get group write Index
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * return WriteIdx
 */
static inline uint32_t getgroupwriteIdx( TVideoIFaceStream *pStream)
{
    return pStream->grpwrIdx;
}

/**
 * Function to get read Index.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * return ReadIdx
 */
static inline uint32_t getreadIdx(TVideoIFaceStream *pStream)
{
    return pStream->rdIdx;
}

/**
 * Function to get group read Index.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * return ReadIdx
 */
static inline uint32_t getgroupreadIdx(TVideoIFaceStream *pStream)
{
    return pStream->grprdIdx;
}

/**
 * Function to increment write index.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param increment by value
 */
static uint32_t incWriteIdx(TVideoIFaceStream *pStream, uint32_t numVideoTs)
{
	 pStream->wrIdx++;

   /*if the write index reaches end,then point it again to start of buffer*/
   if(pStream->wrIdx >= NUM_TS_PTR_ENTRIES)
   {
      //printf("Write Index = %x \n",pStream->wrIdx);
      //printf("WriteIdx wrapping around \n");
	   pStream->wrIdx=0;
	   pStream->wraparound=1;
   }

   return 1;
}

/**
 * Function to increment group write index.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param increment by value
 */
uint32_t incGrpWriteIdx(TVideoIFaceStream *pStream)
{

   pStream->grpwrIdx++;
   /*if the read index reaches end,then wrap around*/
   if(pStream->grpwrIdx >= NUM_GROUP_ENTRIES )
   {
      pStream->grpwrIdx=0;
      pStream->grpwraparound=1;

   }

   return 1;
}

/**
 * Function to Pull Video Ts.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param Pull count
 */
static int32_t PullVideoTs(TVideoIFaceStream *pStream, uint32_t * addrvideobufStart, uint32_t *videoAddresses,int pullts)
{
  uint32_t rdidx;
  int32_t readcount;
  uint32_t grpreadcount;
  uint32_t startrdidx;
  uint32_t endrdidx;
  uint32_t grprdidx;
  uint32_t start_group;
  uint32_t end_group;
  int i;
  /* Logic to determine how much can actually be read*/
  readcount=getreadCount(pStream,pullts);
   /*.So read how much is returned by getreadCount() function.Wrap Around is also handled.*/
   if((readcount>0)&&(readcount<=NUM_TS_PTR_ENTRIES))
   {
    for(i=0;i<readcount;i++)
    {
      rdidx = getreadIdx(pStream);

      if((rdidx>=0)&&(rdidx<NUM_TS_PTR_ENTRIES))
      {
          startrdidx = rdidx;
          videoAddresses[i] = addrvideobufStart[rdidx];
          incReadIdx(pStream);
      }
      else
      {
         //something is gone wrong in indexes.Do Reset.
         readcount =0;
         pStream->wrIdx =0;
         pStream->rdIdx =0;
         pStream->wraparound =0;
         break;
      }
    }
    rdidx = getreadIdx(pStream);
    endrdidx = rdidx -1;
#if 0
      grpreadcount = getgroupreadCount(pStream);
      /* Loop to find Start Group */
      for (i=0;i<grpreadcount;i++)
      {
            grprdidx = getgroupreadIdx(pStream);
            //printf("Check 1 \n");
            /* Case 1 : Read Start Idx and Read End Idx falls in within same group */
            //if((startrdidx>=pVIFCommon->grp_array[i].grpsrtIdx)&&((endrdidx<=pVIFCommon->grp_array[i].grpendIdx)))
            if(pStream->arrayTSBunch[grprdidx].grpsrtIdx < pStream->arrayTSBunch[grprdidx].grpendIdx)
            {
                //printf("Check 2 \n");
                if((startrdidx>=pStream->arrayTSBunch[grprdidx].grpsrtIdx)&&((startrdidx<=pStream->arrayTSBunch[grprdidx].grpendIdx)))
                {

                   start_group = grprdidx;
                   //match_found =1;
                   //break;

                }
                else
                {
                    incGrpReadIdx(pStream);
                }
            }
            else
            {
                //group is wrapped.
                //printf("Check 4 \n");
                if(startrdidx >= pStream->arrayTSBunch[grprdidx].grpsrtIdx)
                {
                   // printf("Check 5 \n");
                   //if((startrdidx>=pVIFCommon->grp_array[grprdidx].grpsrtIdx)&&((startrdidx<=VIDEO_BUFFER_LENGTH+pVIFCommon->grp_array[grprdidx].grpendIdx)))
                    if((startrdidx>=pStream->arrayTSBunch[grprdidx].grpsrtIdx)&&((startrdidx<NUM_TS_PTR_ENTRIES)))
                    {
                      //printf("Check 5.1 \n");
                      start_group = grprdidx;
                      //match_found =1;
                      //break;
                    }
                    else
                    {
                        incGrpReadIdx(pStream);
                    }
                }
                else if(startrdidx < pStream->arrayTSBunch[grprdidx].grpsrtIdx)
                {
                    //printf("Check 6 \n");
                    //if((startrdidx + VIDEO_BUFFER_LENGTH >=pVIFCommon->grp_array[grprdidx].grpsrtIdx)&&((startrdidx<=pVIFCommon->grp_array[grprdidx].grpendIdx)))
                   if((startrdidx<=pStream->arrayTSBunch[grprdidx].grpendIdx))
                   {
                      //printf("Check 6.1 \n");
                      start_group = grprdidx;
                      //match_found =1;
                      //break;
                   }
                   else
                   {
                       incGrpReadIdx(pStream);
                   }
                }
            }
        }

        /* Loop to find End Group */
        grpreadcount = getgroupreadCount(pStream);

        for (i=0;i<grpreadcount;i++)
        {
             //grprdidx = getreadgroupIdx_End(pVIFCommon);
             grprdidx = getgroupreadIdx(pStream);
             //printf("Check 1.1 \n");
            /* Case 1 : Read Start Idx and Read End Idx falls in within same group */
            //if((startrdidx>=pVIFCommon->grp_array[i].grpsrtIdx)&&((endrdidx<=pVIFCommon->grp_array[i].grpendIdx)))
            if(pStream->arrayTSBunch[grprdidx].grpsrtIdx < pStream->arrayTSBunch[grprdidx].grpendIdx)
            {
                //printf("Check 7 \n");
                if((endrdidx>=pStream->arrayTSBunch[grprdidx].grpsrtIdx)&&((endrdidx<=pStream->arrayTSBunch[grprdidx].grpendIdx)))
                {
                   //printf("Check 1.2 \n");
                   end_group = grprdidx;
                   //match_found =1;
                   //break;

                }
                else
                {
                    incGrpReadIdx(pStream);
                }
            }
            else
            {
               //printf("Check 8 \n");
               if(endrdidx >= pStream->arrayTSBunch[grprdidx].grpsrtIdx)
               {
                 //if((endrdidx >= pVIFCommon->grp_array[grprdidx].grpsrtIdx)&&((endrdidx <= pVIFCommon->grp_array[grprdidx].grpendIdx + VIDEO_BUFFER_LENGTH)))
                 if((endrdidx >= pStream->arrayTSBunch[grprdidx].grpsrtIdx)&&((endrdidx < NUM_TS_PTR_ENTRIES)))
                 {
                    end_group = grprdidx;
                    //match_found =1;
                    //break;
                 }
                 else
                 {
                     incGrpReadIdx(pStream);
                 }
               }
               else if(endrdidx < pStream->arrayTSBunch[grprdidx].grpsrtIdx)
               {
                   //if((endrdidx + VIDEO_BUFFER_LENGTH >= pVIFCommon->grp_array[grprdidx].grpsrtIdx)&&((endrdidx <= pVIFCommon->grp_array[grprdidx].grpendIdx )))
                   if((endrdidx <= pStream->arrayTSBunch[grprdidx].grpendIdx))
                   {
                    end_group = grprdidx;
                    //match_found =1;
                    //break;
                   }
                   else
                   {
                       incGrpReadIdx(pStream);
                   }
               }

            }
        }
        /* Add Time setting logic here */
        if(start_group == end_group)
        {
            //printk("Pull Video Timestamp belong to group %d \n",start_group);
            pStream->pulltime32 = pStream->arrayTSBunch[start_group].grpts;
        }
        else
        {
            //printk("Pull Video Timestamp belong to group %d \n",end_group);
            pStream->pulltime32 = pStream->arrayTSBunch[end_group].grpts;
        }
#endif
   }
   else
   {
     //if read value is returned negative;then handle and reset.
     readcount = 0;
     pStream->wrIdx =0;
     pStream->rdIdx =0;
     pStream->wraparound =0;
   }
   return readcount;
}

/**
 * Function to calculate how many can actually be read .
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param pullts Request to read
 * return count that can be read.
 */
static uint32_t getreadCount(TVideoIFaceStream *pStream,int pullts)
{

   uint32_t diff;
   uint32_t diff1;
   uint32_t diff2;

   if(pStream->wraparound)
   {
       /* There could be two conditions here.WriteIdx < ReadIdx or WriteIdx = = ReadIdx*/
       if((pStream->wrIdx < pStream->rdIdx)||(pStream->wrIdx == pStream->rdIdx))
       {
          diff1= pStream->buflength - pStream->rdIdx;
          diff2= pStream->wrIdx;
          diff = diff1 + diff2;
          if (diff > pullts)
          {
           return pullts;
          }
          else
          {
            return diff;
          }
       }

   }
   else
   {
       if((pStream->rdIdx == pStream->wrIdx))
       {
           return 0;
       }
       if(pStream->wrIdx > pStream->rdIdx)
       {
           diff = pStream->wrIdx - pStream->rdIdx;
           if(diff > pullts)
           {
            return pullts;
           }
           else
           {
            return diff;
           }
       }
   }

}

/**
 * Function to calculate how many can actually be read .
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param pullts Request to read
 * return count that can be read.
 */
static uint32_t getgroupreadCount(TVideoIFaceStream *pStream)
{

   uint32_t diff;
   uint32_t diff1;
   uint32_t diff2;
   if(pStream->grpwraparound)
   {
       /* There could be two conditions here.WriteIdx<ReadIdx or ReadIdx==WriteIdx*/
       if((pStream->grpwrIdx < pStream->grprdIdx)||(pStream->grpwrIdx == pStream->grpwrIdx))
       {
          diff1= pStream->grouplength - pStream->grprdIdx;
          diff2= pStream->grpwrIdx;
          diff = diff1 + diff2;
          return diff;

       }

   }
   else
   {
       if((pStream->grprdIdx == pStream->grpwrIdx))
       {
           return 0;
       }
       if(pStream->grpwrIdx > pStream->grprdIdx)
       {
           diff = pStream->grpwrIdx - pStream->grprdIdx;
           return diff;
       }
   }
}

/**
 * Function to increment read index of a stream.
 * @param pStream virtual pointer of TVideoIFaceStream pStream
 * @param numVideoTs increment by value
 */
static inline void incReadIdx(TVideoIFaceStream *pStream )
{
	 pStream->rdIdx++;

   /*if the read index reaches end,then point it again to start of buffer*/
   if(pStream->rdIdx>=NUM_TS_PTR_ENTRIES)
   {
      //printf("Read Index = %x \n",pStream->rdIdx);
      //printf("readIdx wrapping around \n");
	   pStream->rdIdx=0;
	   pStream->wraparound=0;
   }
}

/**
 * Function to increment read index.
 * @param pCommon virtual pointer of TVideoIFaceCommon object
 * @param increment by value
 */
static inline void incGrpReadIdx(TVideoIFaceStream *pStream)
{

   pStream->grprdIdx++;
   /*if the read index reaches end,then wrap around*/
   if(pStream->grprdIdx > (NUM_GROUP_ENTRIES -1))
   {
      pStream->grprdIdx=0;
      pStream->grpwraparound=0;
   }

}
