//Copyright 2016 buildworld.net. All rights reserved.
#pragma once

#ifndef _MBCS
#define _MBCS
#endif
#define _USE_MATH_DEFINES

#include "ext_lib.h"

#define RAWCOMMAND 30
#define MAX_OPERA 10
#define MIN_OPERA 3
#define MAX_NUM_BIT 10

#define TOTALPITCH 30
#define PI M_PI
#define	FFT_NUM 8192

typedef char *pBYTE;
typedef char CHAR;

typedef unsigned int COUNTNUM;
typedef short INT16;
typedef unsigned short UINT16;
typedef unsigned int UINT32;

typedef INT16 SampleData, SD, *pSD;
typedef double REALNUM;

//TrackFileHead
typedef struct TrackFileHead
{
	char tag[8];//'bwmbt' use [5] is enough, [8] for memory align
	COUNTNUM szHead;//size of FileHead
	COUNTNUM nBitPerPitch;
	COUNTNUM nPitchPerSample;
	COUNTNUM nSamplePerSec;
	COUNTNUM szData;//size of TotalSampleData
}TFH, *pTFH;

//TrackDataLinkedList
typedef struct TrackDataLinkedList
{
	pBYTE pSampleData;
	struct TrackDataLinkedList *pNextNode;
}TDLL, *pTDLL;

//WaveFileHead
typedef struct WaveFileHead
{
	char RIFF_tag[4];
	UINT32 szFile;
	char WAVE_tag[4];
	//fmt_chunk
	char fmt_tag[4];
	UINT32 szWaveFormatStructure;
	//fmt_chunk_WaveFormatStructure
	UINT16 FormatTag;               //1 means PCM
	UINT16 nChannels;               //number of channels
	UINT32 nSamplesPerSec;          //sample rate
	UINT32 nAvgBytesPerSec;         //Byte rate
	UINT16 nBlockAlign;             //block size of data
	UINT16 nBitsPerSample;          //QuantizationBit
	//data_chunk
	char data_tag[4];
	UINT32 szSampleData;            //szSampleData + 2CH = szFile
	//Next first SampleData
}WFH, *pWFH;

//operation_data
typedef struct Operation
{
	int opera;
	COUNTNUM param1;
	COUNTNUM param2;
	COUNTNUM param3;
	CHAR path[_MAX_PATH];
	CHAR reserve[MAX_OPERA];
}OPT, *pOPT;

//OrginPitchData
typedef struct OriginalPitchData
{
	pSD pitch[TOTALPITCH];
	size_t szData[TOTALPITCH];
}OPD, *pOPD;

typedef struct iterationStatus
{
	//iteration data
	size_t pitch_cur[TOTALPITCH];
	size_t fft_cur[TOTALPITCH];
	COUNTNUM nLevel[TOTALPITCH];
	size_t sum_fft_cur;
	size_t sum_ifft_cur;
	size_t wave_cur;
}IS, *pIS;


//OPERA Msg
//operation                     Number          Param
#define OPERA_OPEN              1               // [path], 0+1
#define OPERA_CREATE            2               // [nBitPerPitch] [nPitchPerSample] [nSamplePerSec] [path], 3+1
#define OPERA_SAVE              3               //null
#define OPERA_SAVEAS            4               // [path], 0+1
#define OPERA_EXIT              5               //null

#define OPERA_WAVGENERATION     7               // [nBitPerSample] [nSamplesPerSec] [nChannels] [path], 3+1
#define OPERA_PLAY              6               // [nBitPerSample] [nSamplesPerSec] [nChannels], 3+0
#define OPERA_ABORT             8               //null

#define OPERA_SET               9               // [nObjSample] [nObjPitch] [nLevel],3+0
#define OPERA_INSERT            10              // [nStartSample] [nInsertSamle], 2+0
#define OPERA_DELETE            11              // [nStartSample] [nDeleteSample], 2+0
#define OPERA_FORMAT            12              // [nStartSample] [nFormatSample], 2+0
#define OPERA_ECHO              13              // [nStartSample] [nEchoSample], 2+0
#define OPERA_REHEAD            14              // [nBitPerPitch] [nPitchPerSample] [nSamplePerSec], 3+0
#define OPERA_UNDO              15              //null

#define OPERA_HELP              16              //null
//

//global var
CHAR g_OpenedFilePath[_MAX_PATH];
COUNTNUM g_fftnum;
COUNTNUM g_TotalPitch;