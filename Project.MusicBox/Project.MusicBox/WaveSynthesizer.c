//Copyright 2016 buildworld.net. All rights reserved.

#include "WaveSynthesizer.h"

void FFT(double x[], double y[], int n, int inv_sign)

{
	int i, j, k, l, m, n1, n2;
	double c, c1, e, s, s1, t, tr, ti;
	/*Calculate i = log2N*/
	for (j = 1, i = 1; i < 16; i++)
	{
		m = i;
		j = 2 * j;
		if (j == n)
			break;
	}
	/*计算每一级的输出，l为某一级，i为同一级的不同群，使用同一内存（即原位运算），将FFT与IFFT流图作出来将看的很清晰。*/
	n1 = 1;
	for (l = 1; l <= m; l++)
	{
		n1 = 2 * n1;
		n2 = n1 / 2;
		e = PI / n2;
		c = 1.0;
		s = 0.0;
		c1 = cos(e);
		s1 = -inv_sign*sin(e);
		for (j = 0; j < n2; j++)
		{
			for (i = j; i < n; i += n1)
			{
				k = i + n2;
				tr = c*x[k] - s*y[k];
				ti = c*y[k] + s*x[k];
				x[k] = x[i] - tr;
				y[k] = y[i] - ti;
				x[i] = x[i] + tr;
				y[i] = y[i] + ti;
			}
			t = c;
			c = c*c1 - s*s1;
			s = t*s1 + s*c1;
		}
	}
	/*如果是求IFFT，再除以N*/
	if (inv_sign == -1)
	{
		for (i = 0; i<n; i++)
		{
			x[i] /= n;
			y[i] /= n;
		}
	}
}

void ZeroRealArray(REALNUM *arr, COUNTNUM offs)
{
	COUNTNUM i;
	for (i = 0; i < offs; i++)
	{
		*(arr + i) = 0.0;
	}
}

int WaveSynthesizer(pOPD OriPitchData, pTFH pTrackHead, pTDLL pTrackData, pWFH pWaveHead, pSD *ppWaveData)
{
	COUNTNUM nSample_t, nTotalSample_t, nSamplePerSec_t, nPitch, nPitchPerSample, nBytePerPitch, nLevel;
	COUNTNUM nTotalSample_w, nSamplePerSec_w, nBytePerSample_w;
	COUNTNUM offs, series, WsPerTs, fft_time, ifft_time, ctr, cursor, delta, Mod;
	size_t nBytePerSample_t, offcur;
	pTDLL pCur;
	pBYTE poi;
	pSD pWaveData, pObjPitch;
	size_t szWaveData;
	REALNUM *pFFTArray_real[PITCH], *pFFTArray_imag, *pSumArray;
	pIS ite_stat;
	//format var load
	//Track
	nBytePerPitch = pTrackHead->nBitPerPitch / 8;
	nPitchPerSample = pTrackHead->nPitchPerSample;
	nBytePerSample_t = nBytePerPitch*nPitchPerSample;
	nTotalSample_t = pTrackHead->szData / nBytePerSample_t;
	nSamplePerSec_t = pTrackHead->nSamplePerSec;
	//Wave
	nSamplePerSec_w = pWaveHead->nSamplesPerSec;
	nBytePerSample_w = pWaveHead->nBitsPerSample / 8;
	//WaveSample / TrackSample
	WsPerTs = nSamplePerSec_w / nSamplePerSec_t;
	//result
	nTotalSample_w = WsPerTs*nTotalSample_t;
	szWaveData = nBytePerSample_w*nTotalSample_w;
	ifft_time = WsPerTs / FFT_NUM;
	if (ifft_time < 1) return -1;
	Mod = WsPerTs % FFT_NUM;
	if (Mod != 0) fft_time = ifft_time + 1;
	else fft_time = ifft_time;
	//Synthesize start
	//modfiy WaveHead
	pWaveHead->szFile += szWaveData;
	pWaveHead->szSampleData += szWaveData;
	//static
	pWaveData = (pSD)malloc(szWaveData);
	*ppWaveData = pWaveData;//return pWaveData
	ite_stat = (pIS)malloc(sizeof(IS));
	//Zero iteration data 
	for (ctr = 0; ctr < PITCH; ctr++)
	{
		ite_stat->pitch_cur[ctr] = 0;
		ite_stat->fft_cur[ctr] = 0;
		ite_stat->nLevel[ctr] = 0;
		ite_stat->sum_fft_cur = 0;
		ite_stat->sum_ifft_cur = 0;
		ite_stat->wave_cur = 0;
	}
	pSumArray = (REALNUM *)calloc(nTotalSample_w, sizeof(REALNUM));
	for (ctr = 0; ctr < PITCH; ctr++)
	{
		pFFTArray_real[ctr] = (REALNUM *)calloc(FFT_NUM, sizeof(REALNUM));
	}
	pFFTArray_imag = (REALNUM *)calloc(FFT_NUM, sizeof(REALNUM));
	ZeroRealArray(pSumArray, nTotalSample_w);
	//every TrackSample loop
	pCur = pTrackData;
	for (nSample_t = 0; nSample_t < nTotalSample_t; nSample_t++)
	{
		if (nSample_t != 0) pCur = pCur->pNextNode;
		poi = pCur->pSampleData;
		//every Pitch loop//filter trigger
		for (nPitch = 0; nPitch < nPitchPerSample; nPitch++, poi += nBytePerPitch)
		{
			//get nLevel
			for (offs = 0, nLevel = 0, series = 1; offs < nBytePerPitch; offs++)
			{
				nLevel += (unsigned)(*(poi + offs))*series;
				series <<= 8;
			}
			if (nLevel > 0)
			{
				ite_stat->pitch_cur[nPitch] = 0;
				ite_stat->nLevel[nPitch] = nLevel;
			}
			//copy data + FFT
			for (ctr = 0; ctr < fft_time; ctr++)
			{
				//reset delta
				delta = ite_stat->fft_cur[nPitch];
				ite_stat->fft_cur[nPitch] = 0;
				//real
				pObjPitch = OriPitchData->pitch[nPitch];
				for (offcur = ite_stat->pitch_cur[nPitch]; offcur + delta < FFT_NUM; offcur++)
				{
					if (offcur < OriPitchData->offs[nPitch])
					{
						*(pFFTArray_real[nPitch] + offcur + delta) = (REALNUM)(*(pObjPitch + offcur)) / pow(2, sizeof(SD) * 8 - 1)*(nLevel / 255);
					}
					else						
					{
						*(pFFTArray_real[nPitch] + offcur + delta) = 0.0;
					}
				}
				if (offcur >= OriPitchData->offs[nPitch]) offcur = OriPitchData->offs[nPitch] - 1;
				ite_stat->pitch_cur[nPitch] = offcur;
				//last fft_time
				if (ctr == fft_time - 1)
				{
					ite_stat->fft_cur[nPitch] = Mod;
				}
				//imag zero
				ZeroRealArray(pFFTArray_imag, FFT_NUM);
				//FFT
				if (Mod != 0 && ctr == fft_time - 1) break;
				FFT(pFFTArray_real[nPitch], pFFTArray_imag, FFT_NUM, 1);
				//copy result
				delta = ite_stat->sum_fft_cur;
				for (offcur = 0; offcur < FFT_NUM; offcur++)
				{
					*(pSumArray + delta + offcur) += *(pFFTArray_real[nPitch] + offcur);
				}
				ite_stat->sum_fft_cur += FFT_NUM;
			}
		}
		//iFFT
		for (ctr = 0; ctr < ifft_time; ctr++)
		{
			ZeroRealArray(pFFTArray_imag, FFT_NUM);
			FFT(pSumArray + (ite_stat->sum_ifft_cur), pFFTArray_imag, FFT_NUM, -1);
			ite_stat->sum_ifft_cur += FFT_NUM;
			offs = ite_stat->sum_ifft_cur;
			offcur = ite_stat->wave_cur;
			for (cursor = 0; cursor < FFT_NUM; cursor++)
			{
				*(pWaveData + offcur + cursor) = (SD)(*(pSumArray + offs + cursor)) * pow(2, sizeof(SD) * 8 - 1);
			}
			ite_stat->wave_cur += FFT_NUM;
		}
	}
	free(ite_stat);
	free(pSumArray);
	for (ctr = 0; ctr < PITCH; ctr++) free(pFFTArray_real[ctr]);
	free(pFFTArray_imag);
	return 0;
}
