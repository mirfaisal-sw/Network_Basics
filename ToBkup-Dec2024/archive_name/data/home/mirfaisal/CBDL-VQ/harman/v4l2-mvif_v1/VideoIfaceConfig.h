/*****************************************************************************
 * Project         QNX AVB driver 4 Jacinto 5
 *
 * (c) copyright   2015
 * Company         Harman/Becker Automotive Systems GmbH
 *                 All rights reserved
 *                 STRICTLY CONFIDENTIAL
 *****************************************************************************/
/**
 * @file          VideoIfaceConfig.h
 * @ingroup       driver
 * @author        Gerd Zimmermann
 *
 * Declaration of TAudioIFaceCommon, TAudioIFaceStream, TTalkerListener, some global defines and marcos
 *
 * \image html SampleFormat.jpg
 */

#ifndef __VIDEOIFACECONFIG_H
#define __VIDEOIFACECONFIG_H

/*---------------------------------------------------------------------
 * DEFINES
 *--------------------------------------------------------------------*/
// second version from 2015/12/09: 1.3
// for further information take a look at the top of the source file
#define MY_MVIF_VERSION_MAJOR			1
#define MY_MVIF_VERSION_MINOR			4

// how many concurrent audio interface can exist inside OCMC memory
#define MAX_MVIF_AREA	2

// how many Talker and Listeners we can handle
#define NUM_VIDEO_TALKER_MAX		4
#define NUM_VIDEO_LISTENER_MAX		4

#define _1722_HEADER_SIZE		64

#define NUM_TS_PTR_ENTRIES		256
#define NUM_TS_PTR_CLASSC_MAXMTU		7
#define NUM_GROUP_ENTRIES             6
#define NUM_AVTP_SOURCE_HEADER			16
#define AVTP_SOURCE_HEADER_SIZE			4
#define AVTP_SOURCE_TS_OFFSET			95238		/* 1sec / (750 * 2 * 7) in nsec means delta of ts packets for worst case bandwidth ) */

// how many concurrent audio interface can exist inside OCMC memory
#define NO_TAG					0

//--- defines for usage of TCreateMlbChannel -------------------------------------------------
// view from application
#define BUF_FIFO					0

/*---------------------------------------------------------------------
 * STRUCTURES
 *--------------------------------------------------------------------*/
/**
 * byte 0 contains minor version number, byte 1 major
 */
typedef union
{
	#define MVIF_VERSION_MAJOR			0
	#define MVIF_VERSION_MINOR			1

	uint32_t	value;
	UInt8	data[4];
}TMVIFVersion;

/**
 * This structure is the entry point for an Audio Interface (AIF) description. Access to Talkers and Listeners are made by
 * a single linked list starting from addrStreamsTalker/addrStreamsListener.
 * This structure is placed inside common memory where different CPUs have access. Therefore all members are 32bit values. Be careful using
 * other values or pointers as these may have different bit size from CPU to CPU
 */
typedef volatile struct
{
	uint32_t	statusMVIF;					/**< to identify if a common header exists in memory and if it is ready to use; use values of enum eStatusMVIF_t  */
	TMVIFVersion versionMVIF;				/**< version of the audio interface structure major and minor*/
	uint32_t	index;						/**< if more than one MVIF is used in parallel */
	uint32_t	tag;						/**< could be used for project specific info e g: 'MOST'=0x4d4f5354 or 'AVB'=0x20415642 */
	uint32_t	sizeTotalMemInByte;			/**< after creating the used memory space is updated here */
	uint32_t	addrNextMVIF;				/**< physical address of next MVIF Common module if available */

	uint32_t	mediaClockSource;			/**< who is the media clock master: DSP, M3, TalkerRefStream; use values of enum eMediaClockSource_t */
	uint32_t	mediaSampleRate;			/**< 44100, 48000; use values of enum eVideoSampleRate_t */
	int32_t		mediaClockTimer;			/**< which timer is used for Media Clock Interval */
	int32_t		mediaClockCnt;				/**< -1: invalid; start value 0; incremented each Media Master Clock Interval */
	uint32_t	mediaTimestamp;
	uint32_t	mediaTimeSec;
	uint32_t	mediaTimeNSec;

	uint32_t	memoryVideoFormatTalker;	/**< special format for Talker MVIF; use values of enum eMemoryVideoFormat_t */
	uint32_t	numberStreamsTalker;		/**< number of existing Talkers */
	uint32_t	addrStreamsTalker;			/**< physical address of talker streams if available ( 0 = unused ) */

	uint32_t	memoryVideoFormatListener;	/**< special format for Listener MVIF; use values of enum eMemoryVideoFormat_t */
	uint32_t	numberStreamsListener;		/**< number of existing Listeners */
	uint32_t	addrStreamsListener;		/**< physical address to Listener streams if available ( 0 = unused ) */


	uint32_t	dummy1;						/**< adding new variables would last in a major change!*/
	uint32_t	dummy2;						/**< so use those dummy values instead */
	uint32_t	dummy3;						/**< don't add more dummies without inc major */
	uint32_t	dummy4;						/**< as then the memory layout will change! */
	uint32_t	dummy5;
	uint32_t	dummy6;
	uint32_t	dummy7;
	uint32_t	dummy8;
	uint32_t	dummy9;
	uint32_t	dummy10;
}TVideoIFaceCommon;

typedef volatile struct
{
    uint32_t grpsrtIdx;
    uint32_t grpendIdx;
    uint32_t grpts;
    uint32_t wrappedgroup;
}t_bundle;

/**
 * This structure is the description of one Talker or Listener.
 * It is placed inside common memory where different CPUs have access. Therefore all members are 32bit values. Be careful using
 * other values or pointers as these may have different bit size from CPU to CPU
 */
typedef volatile struct
{
	uint32_t	index;						/**< for internal management, index of talker and listener at the linked list, starts at 0 and is increment with every new creation */
	uint32_t 	handle;						/**< application handle/connection id */
	uint32_t 	state;						/**< start, stop; use values of enum eStreamState_t */
	uint32_t	counter1722Msg;				/**< transmitted or received messages, could also be use as heartbeat */
	uint32_t	addrNextStream;				/**< physical address of next Talker or Listener stream if available */

	uint32_t	kindOfStream;				/**< Talker, Ref-Talker, Listener; use values of enum eKindOfStream_t */
	uint32_t	srcMacHigh;					/**< dummy1 at version 1.0-1.3: BE:  0,     0,    mac[0], mac[1]*/
	uint32_t	srcMacLow;					/**< dummy2 at version 1.0-1.3: BE: mac[2],mac[3], mac[4], mac[5]*/
	uint32_t	destMacHigh;				/**< BE:  0,     0,    mac[0], mac[1] */
	uint32_t	destMacLow;					/**< BE: mac[2],mac[3], mac[4], mac[5] */
	uint32_t	streamIdHigh;				/**< BE: id[0],id[1],id[2],id[3] */
	uint32_t	streamIdLow;				/**< BE: id[4],id[5],id[6],id[7] */
	uint32_t	maxTransitTime;				/**< .*/
	uint32_t	vlanId;						/**<  */

	uint32_t  	avtpHeader[_1722_HEADER_SIZE];		/**< used from avb stack for 1722 header */
	uint32_t  	avtpSourceHeader[NUM_AVTP_SOURCE_HEADER];		/**< used from avb stack for 1722 header */

	uint32_t	modeVideoBuffer;			/**< use values of enum eModeVideoBuffer_t */
	uint32_t	numVideoBuffer;				/**< how many audio samples/fs; use values of enum eNumSamplesInChunk_t */
	uint32_t	sizeTsPacket;				/**< how many bytes per TS Packet */

	// this and also more comes from Heramb
	uint32_t 	wrIdx;            		/**< Write index. */
	uint32_t	rdIdx;            		/**< Read index. */
	uint32_t    grpwrIdx;
	uint32_t    grprdIdx;
	uint32_t    grpwraparound;
	uint32_t    pushtime32;
	uint32_t    pulltime32;
	uint32_t 	buflength;             /**< Total size buffer in sample blocks. */
	uint32_t 	grouplength;
	uint32_t 	wraparound;   		    /**< To indicate Wrap Around. */
	uint32_t 	overwrite;        		/**< To indicate count of locations overwritten. */
	uint32_t 	arrayTSPointer[NUM_TS_PTR_ENTRIES];				/**<  */
    t_bundle    arrayTSBunch[NUM_GROUP_ENTRIES];
	uint32_t	dummy1;						/**< */
	uint32_t	dummy2;						/**< */
	uint32_t	dummy3;						/**< for future usage and backward compability ...*/
	uint32_t	dummy4;						/**<  not that one new value causes a  */
	uint32_t	dummy5;						/**< major change! */
	uint32_t	dummy6;
	uint32_t	dummy7;
	uint32_t	dummy8;
	uint32_t	dummy9;
	uint32_t	dummy10;
}TVideoIFaceStream;

/**
 * This structure is for management of existing Talkers and Listeners. Addresses and pointers are stored there not to access single linked list
 * for every fetch of these values. It has to be initialized after start up for every individual CPU. It's not part of the common memory.
 */
typedef struct
{
	uint32_t ocmcAddr;					/**< physical address in memory of Talker/Listener TAudioIFaceStream */
	void* pStream;						/**< virtual address in memory of Talker/Listener TAudioIFaceStream */
	uint32_t sizeTsPacket;				/**< how many bytes per TS Packet */

	uint32_t* addrvideobufStart;

	uint32_t physVideoBuffer;				/**< physical address in memory of buffer 1 if used ( 0 = unused )*/
	uint32_t* pVideoBuffer;				/**< virtual address in memory of buffer 1 if used ( NULL = unused )*/
	uint32_t sizeVideoBuffer;				/**< size in byte */

	uint32_t physHeaderBuffer;				/**< physical address in memory of buffer 1 if used ( 0 = unused )*/
	uint32_t* pHeaderBuffer;				/**< virtual address in memory of buffer 1 if used ( NULL = unused )*/
	uint32_t sizeHeaderBuffer;				/**< size in byte */

	uint32_t physAvtpSourceHeaderBuffer;	/**< physical address in memory of buffer 1 if used ( 0 = unused )*/
	uint32_t* pAvtpSourceHeaderBuffer;		/**< virtual address in memory of buffer 1 if used ( NULL = unused )*/
	uint32_t sizeAvtpSourceHeaderBuffer;	/**< size in byte */
}TVideoTalkerListener;

/*---------------------------------------------------------------------
 * ENUM
 *--------------------------------------------------------------------*/
typedef enum eStatusMVIF_t
{
   MVIF_STATUS_ERROR_PARAM = 0,
		   MVIF_CREATION_BY_OWNER = 0x11111111,
		   MVIF_READY_BY_OWNER = 0x22222222,
		   MVIF_UPDATED_BY_OWNER = 0x33333333,
		   MVIF_DESTROYED_BY_OWNER = 0x44444444,
		   MVIF_DESTROYED_BY_READER = 0x55555555
}eStatusMVIF;


typedef enum eMediaSampleRate_t
{
	MVIF_FS_ERROR_PARAM = 0,
			MVIF_FS_44100 = 44100,
			MVIF_FS_48000 = 48000
}eMediaSampleRate;


typedef enum eMemoryVideoFormat_t
{
	MVIF_FORMAT_ERROR_PARAM = 0,
	MVIF_MPEG_TS
}eMemoryVideoFormat;


typedef enum eKindOfMVIFStream_t
{
	MVIF_STREAM_ERROR_PARAM = 0,
	MVIF_TALKER_STREAM = 0x11111111,
	MVIF_LISTENER_STREAM = 0x33333333,
}eKindOfMVIFStream;

typedef enum eModeVideoBuffer_t
{
	MVIF_BM_ERROR_PARAM = 0,
			MVIF_BM_TS_PTR_RINGBUF = 0x11111111
}eModeVideoBuffer;


typedef enum eMVIFStreamState_t
{
	MVIF_SST_ERROR_PARAM = 0,
			MVIF_SST_INITIALIZED = 0x11111111,
			MVIF_SST_IDLE = 0x22222222,
			MVIF_SST_TALKING = 0x33333333,
			MVIF_SST_LISTENING = 0x44444444,
			MVIF_SST_STOPPED = 0x55555555,
			MVIF_SST_PARAM_CHANGED = 0x66666666
}eMVIFStreamState;
/*---------------------------------------------------------------------
 * MACROS
 *--------------------------------------------------------------------*/
/**
 * Macro to set TAudioIFaceCommon.
 * @param mCommon virtual pointer of TAudioIFaceCommon object
 * @param mSource mediaClockSource
 */
#define MVIF_SET_MEDIACLOCKSOURCE( mCommon, mSource ) \
		mCommon->mediaClockSource = (uint32_t)mSource;

/**
 * Macro to set TAudioIFaceCommon.mediaSampleRate
 * @param mCommon virtual pointer of TAudioIFaceCommon object
 * @param mSamplerate mediaSampleRate
 */
#define MVIF_SET_MEDIASAMPLERATE( mCommon, mSamplerate ) \
		mCommon->mediaSampleRate = (uint32_t)mSamplerate;

/**
 * Macro to set TAudioIFaceCommon.audioSampleSizeTalker
 * @param mCommon virtual pointer of TAudioIFaceCommon object
 * @param mSamplesize audioSampleSizeTalker
 */
#define MVIF_SET_MEDIACLOCKTIMER( mCommon, mMediaClockTimer ) \
		mCommon->mediaClockTimer = (uint32_t)mMediaClockTimer;

#define MVIF_SET_MEDIACLOCKCNT( mCommon, mMediaClockCnt ) \
		mCommon->mediaClockCnt = (uint32_t)mMediaClockCnt;

#define MVIF_SET_MEDIATIMESTAMP( mCommon, mMediaTimestamp ) \
		mCommon->mediaTimestamp = (uint32_t)mMediaTimestamp;

#define MVIF_SET_MEDIATIMESEC( mCommon, mMediaTimeSec ) \
		mCommon->mediaTimeSec = (uint32_t)mMediaTimeSec;

#define MVIF_SET_MEDIATIMENSEC( mCommon, mMediaTimeNSec ) \
		mCommon->mediaTimeNSec = (uint32_t)mMediaTimeNSec;

/**
 * Macro to set TAudioIFaceCommon.
 * @param mCommon virtual pointer of TAudioIFaceCommon object
 * @param mMemSampleformat memorySampleFormatTalker
 */
#define MVIF_SET_MEMORYVIDEOFORMATTALKER( mCommon, mMemVideoformat ) \
		mCommon->memoryVideoFormatTalker = mMemVideoformat;


/*---------------------------------------------------------------------
 * FUNCTION DECLARATION
 *--------------------------------------------------------------------*/
uint32_t mvif_Create_Common( TVideoIFaceCommon *pCommon, uint32_t physOcmc, uint32_t tag, uint32_t mvifIndex );
void mvif_Register_Common( TVideoIFaceCommon *pCommon, uint32_t physOcmc );
void mvif_Register_Talker( TVideoIFaceCommon *pCommon, uint32_t indx, TVideoIFaceStream* pStream, uint32_t physAddr );
uint32_t mvif_Create_Talker( TVideoIFaceCommon *pCommon,  uint32_t handle, uint32_t sizeTsPacket );

void mvif_Set_Status( TVideoIFaceCommon *pCommon, uint32_t creationDone, enum eStatusMVIF_t mvifStatus );
void mvif_Set_TsPacketSizeTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t sizeTsPacket );
void mvif_Set_StreamStateTalker( TVideoIFaceCommon *pCommon, uint32_t indx , enum eMVIFStreamState_t state);
void mvif_Set_destMacAddressTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t macHigh, uint32_t macLow );
void mvif_Set_streamIdTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t idHigh, uint32_t idLow );
void mvif_Get_destMacAddressTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t* macHigh, uint32_t* macLow );
void mvif_Get_streamIdTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t* idHigh, uint32_t* idLow );
enum eMVIFStreamState_t mvif_Get_StreamStateTalker( TVideoIFaceCommon *pMVIFCommon, uint32_t indx);
void mvif_Reset_TalkerStream( TVideoIFaceCommon *pCommon, uint32_t indx );
uint32_t mvif_Get_TotalTsPtrEntriesTalker( TVideoIFaceCommon *pCommon, uint32_t indx );
uint32_t mvif_Get_FreeTsPtrEntriesTalker(TVideoIFaceCommon *pCommon, uint32_t indx );
uint32_t mvif_Get_MaxPushRateTalker(TVideoIFaceCommon *pCommon, uint32_t indx );
int32_t mvif_Push_TsPtrEntriesTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t* arrayTsPtr, uint32_t numTsPtr);
uint32_t mvif_Get_MaxPullRateTalker(TVideoIFaceCommon *pCommon, uint32_t indx );
int32_t mvif_Pull_TsPtrEntriesTalker( TVideoIFaceCommon *pCommon, uint32_t indx, uint32_t* arrayTsPtr, uint32_t numTsPtr);
uint32_t mvif_Get_StreamAddrTalker( TVideoIFaceCommon *pCommon, UInt32 indx );
uint8_t* mvif_Get_HeaderPtrTalker( TVideoIFaceCommon* pCommon, UInt32 indx );
uint32_t mvif_Get_HeaderAddrTalker( TVideoIFaceCommon *pCommon, UInt32 indx );
uint32_t mvif_Get_HeaderSizeTalker( TVideoIFaceCommon *pCommon, UInt32 indx );

uint32_t mvif_Get_AvtpSourceHeaderAddrTalker( TVideoIFaceCommon *pCommon, UInt32 indx );
uint8_t* mvif_Get_AvtpSourceHeaderPtrTalker( TVideoIFaceCommon* pCommon, UInt32 indx );
uint32_t mvif_Get_AvtpSourceHeaderSizeTalker( TVideoIFaceCommon *pCommon, UInt32 indx );

uint32_t mvif_Get_TsPacketSizeTalker( TVideoIFaceCommon *pCommon, UInt32 indx );

#endif /* __AVBAUDIOIFACECONFIG_H */
