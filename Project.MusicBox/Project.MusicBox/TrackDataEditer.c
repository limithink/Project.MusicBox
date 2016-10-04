//Copyright 2016 buildworld.net. All rights reserved.

#include "TrackDataEditer.h"

int ZeroMemory(pBYTE poi, size_t off)
{
	size_t ctr;
	for (ctr = 0; ctr < off; ctr++)
		*(poi + ctr) = 0;
	return ctr;
}

COUNTNUM Editer_set(pTFH pTrackHead, pTDLL pTrackData,
	COUNTNUM nObjSample, COUNTNUM nObjPitch, COUNTNUM nLevel)
{
	COUNTNUM nSample, nTotalSample, nPitch, cursor;
	size_t nBytePerSample, nBytePerPitch;
	pTDLL pCur;
	pBYTE poi;
	nBytePerPitch = pTrackHead->nBitPerPitch / 8;
	nBytePerSample = pTrackHead->nPitchPerSample*nBytePerPitch;
	nTotalSample = pTrackHead->szData / nBytePerSample;
	//seek node
	pCur = pTrackData;
	for (nSample = 0; nSample < nObjSample; nSample++)
	{
		if (!pCur->pNextNode)
		{
			pCur->pNextNode = (pTDLL)malloc(sizeof(TDLL));
			pCur->pNextNode->pNextNode = NULL;
			pCur->pNextNode->pSampleData = NULL;
		}
		if (nSample + 1 > nTotalSample) pTrackHead->szData += nBytePerSample;
		pCur = pCur->pNextNode;
		if (!pCur->pSampleData) pCur->pSampleData = (pBYTE)malloc(nBytePerSample);
		ZeroMemory(pCur->pSampleData, nBytePerSample);
	}
	poi = pCur->pSampleData;
	//seek ObjPitch
	for (nPitch = 0; nPitch < nObjPitch; nPitch++, poi += nBytePerPitch);
	//write data
	for (cursor = 0; cursor < nBytePerPitch; cursor++)
	{
		*(poi + cursor) = (unsigned)(nLevel % 256);
		nLevel /= 256;
	}
	return nObjSample + 1 - nTotalSample;
}

int Editer_insert(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nStartSample, COUNTNUM nInsertSample)
{
	COUNTNUM nSample, nTotalSample;
	size_t nBytePerSample;
	pTDLL pCur, pTemp;
	nBytePerSample = (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;
	nTotalSample = pTrackHead->szData / nBytePerSample;
	if (nStartSample + 1 >= nTotalSample) return -1;
	//seek node
	pCur = pTrackData;
	for (nSample = 0; nSample < nStartSample; nSample++)
	{
		if (!pCur->pNextNode) return -2;
		pCur = pCur->pNextNode;
		if (!pCur->pSampleData) pCur->pSampleData = (pBYTE)malloc(nBytePerSample);
		ZeroMemory(pCur->pSampleData, nBytePerSample);
	}
	//Insert node
	if (!pCur->pNextNode) return -3;
	pTemp = pCur->pNextNode;
	for (nSample = 0; nSample < nInsertSample; nSample++)
	{
		pCur->pNextNode = (pTDLL)malloc(sizeof(TDLL));
		pTrackHead->szData += nBytePerSample;
		pCur = pCur->pNextNode;
		pCur->pSampleData = (pBYTE)malloc(nBytePerSample);
		ZeroMemory(pCur->pSampleData, nBytePerSample);
	}
	pCur->pNextNode = pTemp;
	return 0;
}

int Editer_delete(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nStartSample, COUNTNUM nDeleteSample)
{
	COUNTNUM nSample, nTotalSample;
	size_t nBytePerSample;
	pTDLL pCur, pTemp, pPrev;
	int flag;
	nBytePerSample = (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;
	nTotalSample = pTrackHead->szData / nBytePerSample;
	if (nStartSample + 1 > nTotalSample) return -1;
	if (nStartSample + nDeleteSample > nTotalSample) return -2;
	if (nStartSample + 1 == nTotalSample) flag = 1;//delete the last node flag;
	//seek node
	pCur = pTrackData;
	for (nSample = 0; nSample < nStartSample; nSample++)
	{
		pPrev = pCur;
		if (!pCur->pNextNode) return -3;
		pCur = pCur->pNextNode;
		if (!pCur->pSampleData) pCur->pSampleData = (pBYTE)malloc(nBytePerSample);
		ZeroMemory(pCur->pSampleData, nBytePerSample);
	}
	//Delete node
	if (flag)
	{
		pTrackHead->szData -= nBytePerSample;
		free(pCur->pSampleData);
		free(pCur);
		pPrev->pNextNode = NULL;
	}
	else
	{
		for (nSample = 0; nSample < nDeleteSample; nSample++)
		{
			pTemp = pCur;
			pCur = pCur->pNextNode;
			pTrackHead->szData -= nBytePerSample;
			free(pTemp->pSampleData);
			free(pTemp);
		}
		pPrev->pNextNode = pCur;
	}
	return 0;
}

COUNTNUM Editer_format(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nStartSample, COUNTNUM nFormatSample)
{
	COUNTNUM nSample, nTotalSample;
	size_t nBytePerSample;
	pTDLL pCur;
	nBytePerSample = (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;
	nTotalSample = pTrackHead->szData / nBytePerSample;
	//seek node
	pCur = pTrackData;
	for (nSample = 0; nSample < nStartSample; nSample++)
	{
		if (!pCur->pNextNode) pCur->pNextNode = (pTDLL)malloc(sizeof(TDLL));
		if (nSample + 1 > nTotalSample) pTrackHead->szData += nBytePerSample;
		pCur = pCur->pNextNode;
		if (!pCur->pSampleData) pCur->pSampleData = (pBYTE)malloc(nBytePerSample);
		ZeroMemory(pCur->pSampleData, nBytePerSample);
	}
	//format node
	for (nSample = 0; nSample < nFormatSample; nSample++)
	{
		ZeroMemory(pCur->pSampleData, nBytePerSample);
		if (!pCur->pNextNode)
		{
			pCur->pNextNode = (pTDLL)malloc(sizeof(TDLL));
			pTrackHead->szData += nBytePerSample;
		}
		pCur = pCur->pNextNode;
	}
	return nStartSample + 1 + nFormatSample - nTotalSample;
}

int Editer_echo(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nStartSample, COUNTNUM nEchoSample)
{
	COUNTNUM nSample, nTotalSample;
	size_t nBytePerSample;
	pTDLL pCur;
	nBytePerSample = (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;
	nTotalSample = pTrackHead->szData / nBytePerSample;
	//seek node
	pCur = pTrackData;
	for (nSample = 0; nSample < nStartSample; nSample++)
	{
		if (!pCur->pNextNode) return -2;
		pCur = pCur->pNextNode;
		if (!pCur->pSampleData) pCur->pSampleData = (pBYTE)malloc(nBytePerSample);
		ZeroMemory(pCur->pSampleData, nBytePerSample);
	}
	//Echo node

}