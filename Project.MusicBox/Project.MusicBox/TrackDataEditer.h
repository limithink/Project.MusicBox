//Copyright 2016 buildworld.net. All rights reserved.
#pragma once
#include "Global.h"

int ZeroMemory(pBYTE poi, size_t off);
COUNTNUM Editer_set(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nObjSample, COUNTNUM nObjPitch, COUNTNUM nLevel);
int Editer_insert(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nStartSample, COUNTNUM nInsertSample);
int Editer_delete(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nStartSample, COUNTNUM nDeleteSample);
COUNTNUM Editer_format(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nStartSample, COUNTNUM nFormatSample);
int Editer_echo(pTFH pTrackHead, pTDLL pTrackData, COUNTNUM nStartSample, COUNTNUM nEchoSample);