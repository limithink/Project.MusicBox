//Copyright 2016 buildworld.net. All rights reserved.

#include "File_IO.h"

int LoadTrackFile(CHAR *path, pTFH *ppTrackHead, pTDLL *ppTrackData)
{
	FILE *fpTrackFile = NULL;
	pTFH pFileHead = (pTFH)malloc(sizeof(TFH));
	pTDLL pDataHead = (pTDLL)malloc(sizeof(TDLL));
	COUNTNUM nSample, nTotalSample;
	size_t nBytePerSample;
	pTDLL pCur;
	strcpy(g_OpenedFilePath, path);
	fpTrackFile = fopen(path, "rb");
	if (!fpTrackFile) return -1;
	if (fread(pFileHead, sizeof(TFH), 1, fpTrackFile) != 1) return -2;
	nBytePerSample = (pFileHead->nPitchPerSample)*(pFileHead->nBitPerPitch) / 8;
	nTotalSample = pFileHead->szData / nBytePerSample;
	//linkedlist read
	pCur = pDataHead;
	for (nSample = 0; nSample < nTotalSample; nSample++)
	{
		pCur->pSampleData = (pBYTE)malloc(nBytePerSample);
		fread(pCur->pSampleData, nBytePerSample, 1, fpTrackFile);
		pCur->pNextNode = (pTDLL)malloc(sizeof(TDLL));
		//ptr init
		pCur->pNextNode->pNextNode = NULL;
		pCur->pNextNode->pSampleData = NULL;
		//
		pCur = pCur->pNextNode;
	}
	fclose(fpTrackFile);
	*ppTrackHead = pFileHead;
	*ppTrackData = pDataHead;
	return 0;
}

int SaveTrackFile(CHAR *path, pTFH pTrackHead, pTDLL pTrackData, int IDentifier)
{
	FILE *fpTrackFile = NULL;
	COUNTNUM nSample, nTotalSample;
	size_t nBytePerSample;
	pTDLL pCur, pTemp;
	if (IDentifier) fpTrackFile = fopen(path, "wb");
	else fpTrackFile = fopen(g_OpenedFilePath, "wb");
	if (!fpTrackFile) return -1;
	if (fwrite(pTrackHead, sizeof(TFH), 1, fpTrackFile) != 1) return -2;
	nBytePerSample = (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;
	nTotalSample = pTrackHead->szData / nBytePerSample;
	//linkedlist write
	pCur = pTrackData;
	for (nSample = 0; nSample < nTotalSample; nSample++)
	{
		if (!pCur->pSampleData) return -3;//if editer had not allocation for pSampleData,it will be a bug
		fwrite(pCur->pSampleData, nBytePerSample, 1, fpTrackFile);
		pCur = pCur->pNextNode;
	}
	fclose(fpTrackFile);
	return 0;
}


int LoadPitchFiles(pOPD OriPitchData)
{
	FILE *fpWaveFile = NULL;
	char path[_MAX_PATH];
	char temp;
	size_t szData;
	int i, VerifyFlag = 0;
	for (i = 0; i < g_TotalPitch; i++)
	{
		sprintf(path, "pitch_src\\%d.wav", i + 1);
		fpWaveFile = fopen(path, "rb");
		if (!fpWaveFile)
		{
			printf("Open\"%s\"Error!\n", path);
			return -1;
		}
		//seek data chunk
		temp = NULL;
		while (!(feof(fpWaveFile)) && ftell(fpWaveFile) < 100)
		{
			while (temp != 'd')
			{
				if (!fread(&temp, 1, 1, fpWaveFile)) return -2;
			}
			if (!fread(&temp, 1, 1, fpWaveFile)) return -2;
			if (temp != 'a') continue;
			if (!fread(&temp, 1, 1, fpWaveFile)) return -2;
			if (temp != 't') continue;
			if (!fread(&temp, 1, 1, fpWaveFile)) return -2;
			if (temp != 'a') continue;
			VerifyFlag = 1;
			break;
		}
		if (!VerifyFlag) return -2;
		//get size
		if (!fread(&szData, sizeof(UINT32), 1, fpWaveFile)) return -3;
		OriPitchData->szData[i] = szData;
		//Load data
		OriPitchData->pitch[i] = (pSD)malloc(szData);
		if (!fread(OriPitchData->pitch[i], szData, 1, fpWaveFile)) return -4;
		fclose(fpWaveFile);
		fpWaveFile = NULL;//fp reset null
	}
	return 0;
}

int SaveWaveFile(CHAR *path, pWFH pWaveHead, pSD pWaveData)
{
	FILE *fpWaveFile = NULL;
	int rtn;
	fpWaveFile = fopen(path, "wb");
	if (!fpWaveFile) return -1;
	//Write WaveFileHead
	rtn = fwrite(pWaveHead, sizeof(WFH), 1, fpWaveFile);
	if (rtn != 1) return -2;
	//Write WaveData
	rtn = fwrite(pWaveData, pWaveHead->szSampleData, 1, fpWaveFile);
	if (rtn != 1) return -3;
	fclose(fpWaveFile);
	return 0;
}

