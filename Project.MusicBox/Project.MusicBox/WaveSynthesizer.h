//Copyright 2016 buildworld.net. All rights reserved.
#pragma once
#include "Global.h"

void FFT(double x[], double y[], int n, int inv_sign);
void ZeroRealArray(REALNUM *arr, COUNTNUM offs);
unsigned int BYTE2UINT(pBYTE src);
int WaveSynthesizer(pOPD OriPitchData, pTFH pTrackHead, pTDLL pTrackData, pWFH pWaveHead, pSD *ppWaveData);