//Copyright 2016 buildworld.net. All rights reserved.
#pragma once
#include "Global.h"

void FFT(double x[], double y[], int n, int inv_sign);
void ZeroRealArray(REALNUM *arr, COUNTNUM offs);
unsigned int BYTE2UINT(pBYTE src);
COUNTNUM getLevel(pBYTE *pSampleData, COUNTNUM nObjPitch, COUNTNUM nBytePerPitch);
void FFT_Execute(REALNUM *pSrcArray, REALNUM *pObjArray, COUNTNUM Src_off, int inv_sign, int mode_sign);
int WaveSynthesizer(pOPD OriPitchData, pTFH pTrackHead, pTDLL pTrackData, pWFH pWaveHead, pSD *ppWaveData);
int WaveSynthesizer_low(pOPD OriPitchData, pTFH pTrackHead, pTDLL pTrackData, pWFH pWaveHead, pSD *ppWaveData);