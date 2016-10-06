//Copyright 2016 buildworld.net. All rights reserved.
#pragma once
#include "Global.h"

int LoadTrackFile(CHAR *path, pTFH *ppTrackHead, pTDLL *ppTrackData);
int SaveTrackFile(CHAR *path, pTFH pTrackHead, pTDLL pTrackData, int IDentifier);
int OpenAllFiles(pOPD OriPitchData);