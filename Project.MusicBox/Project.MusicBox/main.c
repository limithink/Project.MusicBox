//Copyright 2016 buildworld.net. All rights reserved.

#include "main.h"

#define VERSION "1.0"//Change Version here

int main(int argc, CHAR *argv[])
{
	int ret_s, ret_u;
	int ExitFlag = 0, FileOpenedFlag = 0;
	pOPT opt = (pOPT)malloc(sizeof(OPT));
	pTFH pTrackHead = NULL;
	pTDLL pTrackData = NULL;
	pBYTE pWaveHead = NULL;
	pSD pWaveData = NULL;
	COUNTNUM nTotalSample;
	size_t nBytePerSample;
	//OriginalPitch Load + config.ini Load
	OPD OriPitchData;
	WFH *head;
	OpenAllFiles(OriPitchData, head);
	//
	printf("BuildWorld MusicBox [Version %s]\nCopyright 2016 buildworld.net. All rights reserved.\n\n", VERSION);
	while (!ExitFlag)
	{
		if (pTrackHead)
		{
			nBytePerSample = (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;
			nTotalSample = pTrackHead->szData / nBytePerSample;
			printf("\n*Opened TrackFile*\nFileHead: %d bit/pitch, %d pitch/sample, %d sample/sec\nTotalSample: %d\n\n",
				pTrackHead->nBitPerPitch,
				pTrackHead->nPitchPerSample,
				pTrackHead->nSamplePerSec,
				nTotalSample);
		}
		printf("_(:3¡¹¡Ï)_> ");
		ret_s = CommandInterpreter(opt);
		if (ret_s < 0)
		{
			switch (ret_s)
			{
			case -1:
				printf("Input illegal,not a string!\n");
				break;
			case -2:
				printf("Get opera failed!\n");
				break;
			case -3:
				printf("missing parameter\n");
				break;
			case -4:
				printf("%s is not a opera!\n", opt->reserve);
				break;
			}
			continue;
		}
		switch (opt->opera)
		{
		case OPERA_OPEN:
			ForgetEver(pTrackHead, pTrackData);
			ret_s = LoadTrackFile(opt->path, &pTrackHead, &pTrackData);
			if (ret_s < 0) printf("TrackFile Loading Fail\n");
			else printf("TrackFile Loading Finish\n");
			break;
		case OPERA_CREATE:
			ForgetEver(pTrackHead, pTrackData);
			pTrackHead = (pTFH)malloc(sizeof(TFH));
			//Initialize FileHead
			pTrackHead->tag[0] = 'b';
			pTrackHead->tag[1] = 'w';
			pTrackHead->tag[2] = 'm';
			pTrackHead->tag[3] = 'b';
			pTrackHead->tag[4] = 't';
			pTrackHead->szHead = sizeof(TFH);
			pTrackHead->szData = 0;
			pTrackHead->nBitPerPitch = opt->param1;
			pTrackHead->nPitchPerSample = opt->param2;
			pTrackHead->nSamplePerSec = opt->param3;
			//Calc
			nBytePerSample = (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;
			//head node allocate memory
			pTrackData = (pTDLL)malloc(sizeof(TDLL));
			pTrackHead->szData += (pTrackHead->nPitchPerSample)*(pTrackHead->nBitPerPitch) / 8;//first node
			pTrackData->pSampleData = (pTDLL)malloc(nBytePerSample);
			ZeroMemory(pTrackData->pSampleData, nBytePerSample);
			pTrackData->pNextNode = NULL;
			//
			strcpy(OpenedFilePath, opt->path);
			break;
		case OPERA_SAVE:
			ret_s = SaveTrackFile(NULL, pTrackHead, pTrackData, 0);
			if (ret_s < 0) printf("TrackFile Saving Fail\n");
			else printf("TrackFile Saving Finish\n");
			break;
		case OPERA_SAVEAS:
			ret_s = SaveTrackFile(opt->path, pTrackHead, pTrackData, 1);
			if (ret_s < 0) printf("TrackFile Saving Fail\n");
			else printf("TrackFile Saving Finish\n");
			break;
		case OPERA_EXIT:
			ExitFlag = 1;
			break;
		case OPERA_WAVGENERATION:
			break;
		case OPERA_PLAY:
			break;
		case OPERA_ABORT:
			break;
		case OPERA_SET:
			ret_u = Editer_set(pTrackHead, pTrackData,
				opt->param1, opt->param2, opt->param3);
			printf("operation complete!AddNewSample:%d\n", ret_u);
			break;
		case OPERA_INSERT:
			ret_s = Editer_insert(pTrackHead, pTrackData, opt->param1, opt->param2);
			if (ret_s < 0) printf("Insert Operation Failed!\n");
			else printf("Insert Operation succeed!\n");
			break;
		case OPERA_DELETE:
			ret_s = Editer_delete(pTrackHead, pTrackData, opt->param1, opt->param2);
			if (ret_s < 0) printf("Delete Operation Failed!\n");
			else printf("Delete Operation succeed!\n");
			break;
		case OPERA_FORMAT:
			ret_u = Editer_format(pTrackHead, pTrackData, opt->param1, opt->param2);
			printf("operation complete!AddNewSample:%d\n", ret_u);
			break;
		case OPERA_ECHO:
			break;
		case OPERA_REHEAD:
			pTrackHead->nBitPerPitch = opt->param1;
			pTrackHead->nPitchPerSample = opt->param2;
			pTrackHead->nSamplePerSec = opt->param3;
			break;
		case OPERA_UNDO:
			break;
		case OPERA_HELP:
			printf("wait to actualisation\n");
			break;
		default:
			printf("unknow error\n");
		}
	}
	free(opt);
	ForgetEver(pTrackHead, pTrackData);
	return 0;
}

void ForgetEver(pTFH pTrackHead, pTDLL pTrackData)
{
	if (!pTrackHead) free(pTrackHead);
	if (!pTrackData) free(pTrackData);
}

void InitWaveHead(pWFH pWaveHead, COUNTNUM nBitPerSample, COUNTNUM nSamplesPerSec, COUNTNUM nChannels)
{
	pWaveHead->RIFF_tag[0] = 'R';
	pWaveHead->RIFF_tag[1] = 'I';
	pWaveHead->RIFF_tag[2] = 'F';
	pWaveHead->RIFF_tag[3] = 'F';
	pWaveHead->szFile = sizeof(WFH);
	pWaveHead->WAVE_tag[0] = 'W';
	pWaveHead->WAVE_tag[1] = 'A';
	pWaveHead->WAVE_tag[2] = 'V';
	pWaveHead->WAVE_tag[3] = 'E';
	pWaveHead->fmt_tag[0] = 'f';
	pWaveHead->fmt_tag[1] = 'm';
	pWaveHead->fmt_tag[2] = 't';
	pWaveHead->fmt_tag[3] = ' ';
	pWaveHead->szWaveFormatStructure = 16;
	pWaveHead->FormatTag = 1;
	pWaveHead->nChannels = (UINT16)nChannels;
	pWaveHead->nSamplesPerSec = nSamplesPerSec;
	pWaveHead->nAvgBytesPerSec = nBitPerSample*nSamplesPerSec / 8;
	pWaveHead->nBlockAlign = (UINT16)nBitPerSample / 8;
	pWaveHead->nBitsPerSample = (UINT16)nBitPerSample;
	pWaveHead->data_tag[0] = 'd';
	pWaveHead->data_tag[1] = 'a';
	pWaveHead->data_tag[2] = 't';
	pWaveHead->data_tag[3] = 'a';
	pWaveHead->szSampleData = 0;
}
