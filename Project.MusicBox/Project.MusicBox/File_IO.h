//Copyright 2016 buildworld.net. All rights reserved.
#pragma once
#include "Global.h"

int LoadTrackFile(CHAR *path, pTFH *ppTrackHead, pTDLL *ppTrackData);
int SaveTrackFile(CHAR *path, pTFH pTrackHead, pTDLL pTrackData, int IDentifier);
int LoadPitchFiles(pOPD OriPitchData);
int SaveWaveFile(CHAR *path, pWFH pWaveHead, pSD pWaveData);