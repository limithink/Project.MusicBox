//Copyright 2016 buildworld.net. All rights reserved.
#pragma once
#include "Global.h"

#include "CommandInterpreter.h"
#include "File_IO.h"
#include "TrackDataEditer.h"
#include "WaveSynthesizer.h"

void ForgetEver(pTFH pTrackHead, pTDLL pTrackData);
void InitWaveHead(pWFH pWaveHead, COUNTNUM nBitPerSample, COUNTNUM nSamplesPerSec, COUNTNUM nChannels);