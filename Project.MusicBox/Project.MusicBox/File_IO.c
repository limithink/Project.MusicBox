//Copyright 2016 buildworld.net. All rights reserved.

#include "File_IO.h"

FILE *fpTrackFile, *fpWaveFile;

int LoadTrackFile(CHAR *path, pTFH *ppTrackHead, pTDLL *ppTrackData)
{
	pTFH pFileHead = (pTFH)malloc(sizeof(TFH));
	pTDLL pDataHead = (pTDLL)malloc(sizeof(TDLL));
	COUNTNUM nSample, nTotalSample;
	size_t nBytePerSample;
	pTDLL pCur;
	strcpy(OpenedFilePath, path);
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
		pCur = pCur->pNextNode;
	}
	free(pCur);
	fclose(fpTrackFile);
	fpTrackFile = NULL;
	*ppTrackHead = pFileHead;
	*ppTrackData = pDataHead;
	return 0;
}

int SaveTrackFile(CHAR *path, pTFH pTrackHead, pTDLL pTrackData, int IDentifier)
{
	COUNTNUM nSample, nTotalSample;
	size_t nBytePerSample;
	pTDLL pCur, pTemp;
	if (IDentifier) fpTrackFile = fopen(path, "wb");
	else fpTrackFile = fopen(OpenedFilePath, "wb");
	if (!fpTrackFile) return -1;
	if (fwrite(pTrackHead, sizeof(TFH), 1, fpTrackFile) != 1) return -2;
	nBytePerSample = (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;
	nTotalSample = pTrackHead->szData / nBytePerSample;
	free(pTrackHead);
	//linkedlist write
	pCur = pTrackData;
	for (nSample = 0; nSample < nTotalSample; nSample++)
	{
		if (!pCur->pSampleData) return -3;//if editer had not allocation for pSampleData,it will be a bug
		fwrite(pCur->pSampleData, nBytePerSample, 1, fpTrackFile);
		free(pCur->pSampleData);
		pTemp = pCur;
		pCur = pCur->pNextNode;
		free(pTemp);
	}
	fclose(fpTrackFile);
	fpTrackFile = NULL;
	return 0;
}


int OpenAllFiles(OPD OriPitchData, WFH *head) {
	FILE *fp;
	char fname[25];
	char str[32];
	char *pitchdata;
	size_t subchunk1size; // head size
	size_t subchunk2size; // pitchdata data size

	for (int i = 1; i <= PITCH; i++) {
		str[0] = '\0';
		pitchdata = NULL;

		sprintf(fname, "Track30\\%d.wave", i);//循环从1.wav开始30个相对路径
		fp = fopen(fname, "r");//open a pitch file
		if (!fp) {
			printf("open file unsuccessful");
			return NULL;
		}

		//skip "RIFF"头数据
		fseek(fp, 8, SEEK_SET);
		fread(str, sizeof(char), 7, fp);
		str[7] = '\0';
		if (strcmp(str, "WAVEfmt")) {
			fprintf(stderr, "The file is not in WAVE format!\n");
			return NULL;
		}
		//skip "fmt"头数据
		fseek(fp, 16, SEEK_SET);
		fread((size_t*)(&subchunk1size), 4, 1, fp);//得到subchunk1size

		fseek(fp, 20 + subchunk1size, SEEK_SET);// 跳过头文件确认data标识
		fread(str, 1, 4, fp);
		str[4] = '\0';
		if (strcmp(str, "data")) {
			printf("filed data start point!\n");
			return NULL;
		}

		//获得数据大小，pitch数据指针
		fseek(fp, 20 + subchunk1size + 4, SEEK_SET);
		fread((size_t)(OriPitchData.offs[i]), 4, 1, fp);
		fread((unsigned int*)(&subchunk2size), 4, 1, fp);

		pitchdata = (char*)malloc(sizeof(char)*subchunk2size);
		if (!pitchdata) {
			fprintf(stderr, "Memory alloc failed!\n");
			return NULL;
		}
		fseek(fp, 20 + subchunk1size + 8, SEEK_SET);
		fread(pitchdata, 1, subchunk2size, fp);
		OriPitchData.pitch[i] = pitchdata;
		printf("%d", i);
		fclose(fp);
	}
	return 0;
}
