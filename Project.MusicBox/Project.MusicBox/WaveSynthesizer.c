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

unsigned int BYTE2UINT(pBYTE src)
{
	unsigned int sum, i;
	char temp;
	for (i = 0, sum = 0; i < 8; i++)
	{
		temp = *src >> i;
		temp <<= 7;
		if (temp == '�') sum += pow(2, i);
	}
	return sum;
}

COUNTNUM getLevel(pBYTE pSampleData, COUNTNUM nObjPitch, COUNTNUM nBytePerPitch)
{
	COUNTNUM nLevel, series, nPitch, Pitch_off, Level_off;
	for (nPitch = 0, Pitch_off = 0; nPitch < nObjPitch; nPitch++, Pitch_off += nBytePerPitch);
	for (Level_off = 0, nLevel = 0, series = 1; Level_off < nBytePerPitch; Level_off++)
	{
		nLevel = series*BYTE2UINT(pSampleData + Pitch_off + Level_off);
		series <<= 8;
	}
	return nLevel;
}

void FFT_Execute(REALNUM *pSrcArray, REALNUM *pObjArray, COUNTNUM Src_off, int inv_sign, int mode_sign)//mode_sign:True means add, False means Copy by cover
{
	COUNTNUM cur, Mod, times, i, off;
	REALNUM *pFFTArray_real, *pFFTArray_imag;
	pFFTArray_real = (REALNUM *)calloc(g_fftnum, sizeof(REALNUM));
	pFFTArray_imag = (REALNUM *)calloc(g_fftnum, sizeof(REALNUM));
	times = Src_off / g_fftnum;
	Mod = Src_off % g_fftnum;
	if (Mod) times += 1;
	//loop
	for (i = 0, cur = 0; i < times; i++, cur += g_fftnum)
	{
		ZeroRealArray(pFFTArray_real, g_fftnum);
		ZeroRealArray(pFFTArray_imag, g_fftnum);
		//Copy Data SrcArray -> FFTArray
		for (off = 0; off < g_fftnum; off++)
		{
			if (i == times - 1 && Mod && cur + off >= Src_off)
				*(pFFTArray_real + off) = 0.0;//fill zero
			else
				*(pFFTArray_real + off) = *(pSrcArray + cur + off);
		}
		//FFT or iFFT
		FFT(pFFTArray_real, pFFTArray_imag, g_fftnum, inv_sign);
		//Copy or Add Data FFTArray -> ObjArray
		if (mode_sign)
		{
			for (off = 0; off < g_fftnum; off++)
			{
				if (i == times - 1 && Mod && cur + off >= Src_off) break;
				*(pObjArray + cur + off) += *(pFFTArray_real + off);
			}
		}
		else
		{
			for (off = 0; off < g_fftnum; off++)
			{
				if (i == times - 1 && Mod && cur + off >= Src_off) break;
				*(pObjArray + cur + off) = *(pFFTArray_real + off);
			}
		}
	}
	free(pFFTArray_real);
	free(pFFTArray_imag);
}

/*
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
	REALNUM *pFFTArray_real[TOTALPITCH], *pFFTArray_imag, *pSumArray;
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
	ifft_time = WsPerTs / g_fftnum;
	if (ifft_time < 1) return -1;
	Mod = WsPerTs % g_fftnum;
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
	for (ctr = 0; ctr < g_TotalPitch; ctr++)
	{
		ite_stat->pitch_cur[ctr] = 0;
		ite_stat->fft_cur[ctr] = 0;
		ite_stat->nLevel[ctr] = 0;
		ite_stat->sum_fft_cur = 0;
		ite_stat->sum_ifft_cur = 0;
		ite_stat->wave_cur = 0;
	}
	pSumArray = (REALNUM *)calloc(nTotalSample_w, sizeof(REALNUM));
	for (ctr = 0; ctr < g_TotalPitch; ctr++)
	{
		pFFTArray_real[ctr] = (REALNUM *)calloc(g_fftnum, sizeof(REALNUM));
	}
	pFFTArray_imag = (REALNUM *)calloc(g_fftnum, sizeof(REALNUM));
	ZeroRealArray(pSumArray, nTotalSample_w);
	//every TrackSample loop
	pCur = pTrackData;
	for (nSample_t = 0; nSample_t < nTotalSample_t; nSample_t++)
	{
		if (!pCur->pSampleData) return -1;
		poi = pCur->pSampleData;
		//every Pitch loop//filter trigger
		for (nPitch = 0; nPitch < nPitchPerSample; nPitch++, poi += nBytePerPitch)
		{
			//get nLevel
			for (offs = 0, nLevel = 0, series = 1; offs < nBytePerPitch; offs++)
			{
				nLevel = series*BYTE2UINT(poi + offs);
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
				for (offcur = ite_stat->pitch_cur[nPitch]; offcur + delta < g_fftnum; offcur++)
				{
					if (offcur < OriPitchData->szData[nPitch])
					{
						*(pFFTArray_real[nPitch] + offcur + delta) = (REALNUM)(*(pObjPitch + offcur)) / pow(2, sizeof(SD) * 8 - 1)*(nLevel / 255);
					}
					else						
					{
						*(pFFTArray_real[nPitch] + offcur + delta) = 0.0;
					}
				}
				if (offcur >= OriPitchData->szData[nPitch]) offcur = OriPitchData->szData[nPitch] - 1;
				ite_stat->pitch_cur[nPitch] = offcur;
				//last fft_time
				if (ctr == fft_time - 1)
				{
					ite_stat->fft_cur[nPitch] = Mod;
				}
				//imag zero
				ZeroRealArray(pFFTArray_imag, g_fftnum);
				//FFT
				if (Mod != 0 && ctr == fft_time - 1) break;
				FFT(pFFTArray_real[nPitch], pFFTArray_imag, g_fftnum, 1);
				//copy result
				delta = ite_stat->sum_fft_cur;
				for (offcur = 0; offcur < g_fftnum; offcur++)
				{
					*(pSumArray + delta + offcur) += *(pFFTArray_real[nPitch] + offcur);
				}
				ite_stat->sum_fft_cur += g_fftnum;
			}
		}
		//iFFT
		for (ctr = 0; ctr < ifft_time; ctr++)
		{
			ZeroRealArray(pFFTArray_imag, g_fftnum);
			FFT(pSumArray + (ite_stat->sum_ifft_cur), pFFTArray_imag, g_fftnum, -1);
			ite_stat->sum_ifft_cur += g_fftnum;
			offs = ite_stat->sum_ifft_cur;
			offcur = ite_stat->wave_cur;
			for (cursor = 0; cursor < g_fftnum; cursor++)
			{
				*(pWaveData + offcur + cursor) = (SD)(*(pSumArray + offs + cursor)) * pow(2, sizeof(SD) * 8 - 1);
			}
			ite_stat->wave_cur += g_fftnum;
		}
		//pCur point next TrackSample Node
		pCur = pCur->pNextNode;
	}
	free(ite_stat);
	free(pSumArray);
	for (ctr = 0; ctr < g_TotalPitch; ctr++) free(pFFTArray_real[ctr]);
	free(pFFTArray_imag);
	return 0;
}
*/

int WaveSynthesizer_low(pOPD OriPitchData, pTFH pTrackHead, pTDLL pTrackData, pWFH pWaveHead, pSD *ppWaveData)
{
	COUNTNUM nSample_t, nTotalSample_t, nSamplePerSec_t, nPitchPerSample, nBytePerPitch, nPitch, nLevel;
	COUNTNUM nSample_w, nTotalSample_w, nSamplePerSec_w, nBytePerSample_w;
	COUNTNUM WsPerTs, OriPitch_off, OriPitchLen;
	size_t nBytePerSample_t;
	pTDLL pNode;
	pSD pWaveData, pObjPitch;
	size_t szWaveData;
	REALNUM *pTempArray, *pSumArray, *pCur, QuantizeMod, CalcLevel;
	int ctr;
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
	QuantizeMod = pow(2, sizeof(SD) * 8 - 1) - 1;
	//Synthesize start
	//modfiy WaveHead
	pWaveHead->szFile += szWaveData;
	pWaveHead->szSampleData += szWaveData;
	//static
	pWaveData = (pSD)malloc(szWaveData);
	*ppWaveData = pWaveData;//return pWaveData
	pSumArray = (REALNUM *)calloc(nTotalSample_w, sizeof(REALNUM));
	ZeroRealArray(pSumArray, nTotalSample_w);
	pTempArray = (REALNUM *)calloc(nTotalSample_w, sizeof(REALNUM));
	
	//every pitch loop
	for (nPitch = 0; nPitch < g_TotalPitch; nPitch++)
	{
		//reset
		pNode = pTrackData;
		pCur = pTempArray;
		pObjPitch = OriPitchData->pitch[nPitch];
		OriPitchLen = OriPitchData->szData[nPitch] / sizeof(SD);
		OriPitch_off = OriPitchLen;
		CalcLevel = 0.0;
		ZeroRealArray(pTempArray, nTotalSample_w);
		//
		for (nSample_t = 0; nSample_t < nTotalSample_t; nSample_t++, pNode = pNode->pNextNode)
		{
			//get level of this sample_t this pitch
			nLevel = getLevel(pNode->pSampleData, nPitch, nBytePerPitch);
			if (nLevel > 0)
			{
				OriPitch_off = 0;
				CalcLevel = nLevel / 255.0;
			}
			//calc data from OriPitchData and write fft array
			for (nSample_w = 0; nSample_w < WsPerTs; nSample_w++, pCur++)
			{
				if (OriPitch_off < OriPitchLen)
				{
					*pCur = *(pObjPitch + OriPitch_off) / QuantizeMod*CalcLevel;
					OriPitch_off++;
				}
				else
				{
					*pCur = 0.0;
				}
			}
		}
		//FFT
		FFT_Execute(pTempArray, pTempArray, nTotalSample_w, 1, 0);
		//Sum Data
		for (nSample_w = 0; nSample_w < nTotalSample_w; nSample_w++)
		{
			*(pSumArray + nSample_w) += *(pTempArray + nSample_w);
		}
	}
	/*
	//calc average
	for (nSample_w = 0; nSample_w < nTotalSample_w; nSample_w++)
	{
		*(pSumArray + nSample_w) /= g_TotalPitch;
	}
	*/
	//iFFT
	FFT_Execute(pSumArray, pSumArray, nTotalSample_w, -1, 0);
	//Transfer Data
	for (nSample_w = 0; nSample_w < nTotalSample_w; nSample_w++)
	{
		*(pWaveData + nSample_w) = (SD)((*(pSumArray + nSample_w))*QuantizeMod);
	}
	//all finish all free;
	free(pTempArray);
	free(pSumArray);
	return 0;
}

