//Copyright 2016 buildworld.net. All rights reserved.
#pragma once
#include "Global.h"

#include "CommandInterpreter.h"
#include "File_IO.h"
#include "TrackDataEditer.h"
#include "WaveSynthesizer.h"

int ForgetEver(pTFH pTrackHead, pTDLL pTrackData, pWFH pWaveHead, pSD pWaveData);
void InitWaveHead(pWFH pWaveHead, COUNTNUM nBitPerSample, COUNTNUM nSamplesPerSec, COUNTNUM nChannels);