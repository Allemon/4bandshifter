/****************************************************************************
* Modified version of Stephan M. Bernsee' code @ dspdimension.com
* Most of the code is:
*
* COPYRIGHT 1999-2003 Stephan M. Bernsee <smb [AT] dspdimension [DOT] com>
*
* Under:
* 						The Wide Open License (WOL)
*
* Permission to use, copy, modify, distribute and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice and this license appear in all source copies. 
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
* ANY KIND. See http://www.dspguru.com/wol.htm for more information.
*
*****************************************************************************/ 

#include <string.h>
#include <math.h>
#include <stdio.h>
extern "C" {

#include "fftw3.h"

}


#define M_PI 3.14159265358979323846
#define MAX_FRAME_LENGTH 8192
#define REALLY_INLINE __forceinline


REALLY_INLINE float squareroot_sse_11bits( float x );
REALLY_INLINE float arctan2(float y, float x);
REALLY_INLINE float sabs(float a);

// -----------------------------------------------------------------------------------------------------------------


void smbPitchShift(float pitchShift1, float pitchShift2, float pitchShift3, float pitchShift4, float* vol, int* tap, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata)
/*
	Routine smbPitchShift(). See top of file for explanation
	Purpose: doing pitch shifting while maintaining duration using the Short
	Time Fourier Transform.
	Author: (c)1999-2002 Stephan M. Bernsee <smb [AT] dspdimension [DOT] com>
*/
{

	static float gInFIFO[MAX_FRAME_LENGTH];
	static float gOutFIFO[MAX_FRAME_LENGTH];
	static fftwf_complex gFFTworksp[MAX_FRAME_LENGTH];
	static float gLastPhase[MAX_FRAME_LENGTH/2+1];
	static float gSumPhase[MAX_FRAME_LENGTH/2+1];
	static float gOutputAccum[2*MAX_FRAME_LENGTH];
	static float gAnaFreq[MAX_FRAME_LENGTH];
	static float gAnaMagn[MAX_FRAME_LENGTH];
	static float gSynFreq[MAX_FRAME_LENGTH];
	static float gSynMagn[MAX_FRAME_LENGTH];
	static float han_window [MAX_FRAME_LENGTH]; //cosine wavetable
	static long gRover = false, gInit = false;
	float magn, phase, tmp, window, real, imag; //instead of double
	float freqPerBin, expct; //instead of double
	long i,k, qpd, index1, index2, index3, index4, inFifoLatency, stepSize, fftFrameSize2, fadeZoneLen;
	

	fftwf_plan p, q;

	/* set up some handy variables */
	fadeZoneLen = fftFrameSize/2;
	fftFrameSize2 = fftFrameSize/2;
	stepSize = fftFrameSize/osamp;
	freqPerBin = sampleRate/(float)fftFrameSize; //
	expct = 2.*M_PI*stepSize/(float)fftFrameSize;//
	inFifoLatency = fftFrameSize-stepSize;
	if (gRover == false) gRover = inFifoLatency;

	/* initialize our static arrays */
	if (gInit == false) {
		memset(gInFIFO, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gOutFIFO, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gFFTworksp, 0, MAX_FRAME_LENGTH*sizeof(fftwf_complex));
		memset(gLastPhase, 0, MAX_FRAME_LENGTH*sizeof(float)/2);
		memset(gSumPhase, 0, MAX_FRAME_LENGTH*sizeof(float)/2);
		memset(gOutputAccum, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
		memset(gAnaFreq, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gAnaMagn, 0, MAX_FRAME_LENGTH*sizeof(float));
		
		for (k = 0; k < fftFrameSize;k++) {
			
			han_window[k]= -.5*cos(2.*M_PI*(float)k/(float)fftFrameSize)+.5;}

		gInit = true;
	}

	/* main processing loop */
	for (i = 0; i < numSampsToProcess; i++){

		/* As long as we have not yet collected enough data just read in */
		gInFIFO[gRover] = indata[i];
		outdata[i] = gOutFIFO[gRover-inFifoLatency];
		gRover++;

		/* now we have enough data for processing */
		if (gRover >= fftFrameSize) {
			gRover = inFifoLatency;

			/* do windowing and re,im interleave */

			//memset(gFFTworksp, 0, MAX_FRAME_LENGTH*sizeof(fftwf_complex));

			for (k = 0; k < fftFrameSize;k++) {
				
				//window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize)+.5;  //Pre-calculated window
				//gFFTworksp[k] = gInFIFO[k] * window;
			
			//  COMPLEX 2 COMPLEX	
				
				gFFTworksp[k][0] = gInFIFO[k] * han_window[k];
				gFFTworksp[k][1] = 0.;
			
			}


			/* ***************** ANALYSIS ******************* */
			
			//OLD CODE
			/* do transform */
			// smbFft(gFFTworksp, fftFrameSize, -1);

			//NEW for FFTW

			
			p = fftwf_plan_dft_1d(fftFrameSize, gFFTworksp, gFFTworksp, FFTW_FORWARD, FFTW_MEASURE);
			q = fftwf_plan_dft_1d(fftFrameSize, gFFTworksp, gFFTworksp, FFTW_BACKWARD, FFTW_MEASURE);

			fftwf_execute(p);

			/* this is the analysis step */
			//fftFrameSize2

			for (k = 0; k <= fftFrameSize2; k++) {

				/* de-interlace FFT buffer */
				//real = gFFTworksp[k][0];
				//imag = gFFTworksp[k][1];

				/* compute magnitude and phase */
				magn = 2.*squareroot_sse_11bits(gFFTworksp[k][0]*gFFTworksp[k][0] + gFFTworksp[k][1]*gFFTworksp[k][1]);
				phase = arctan2(gFFTworksp[k][1],gFFTworksp[k][0]);

				/* compute phase difference */
				tmp = phase - gLastPhase[k];
				gLastPhase[k] = phase;

				/* subtract expected phase difference */
				tmp -= (float)k*expct;

				/* map delta phase into +/- Pi interval */
				qpd = tmp/M_PI;
				if (qpd >= 0) qpd += qpd&1;
				else qpd -= qpd&1;
				tmp -= M_PI*(float)qpd;

				/* get deviation from bin frequency from the +/- Pi interval */
				tmp = osamp*tmp/(2.*M_PI);

				/* compute the k-th partials' true frequency */
				tmp = (float)k*freqPerBin + tmp*freqPerBin;

				/* store magnitude and true frequency in analysis arrays */
				gAnaMagn[k] = magn;
				gAnaFreq[k] = tmp;

			}



			/* ***************** PROCESSING ******************* */
			/* this does the actual pitch shifting */
			memset(gSynMagn, 0, fftFrameSize*sizeof(float));
			memset(gSynFreq, 0, fftFrameSize*sizeof(float));
			for (k = 0; k <= fftFrameSize2; k++) {

//				index = k/pitchShift; OLD

				index1 = k / pitchShift1;
				index2 = k / pitchShift2;
				index3 = k / pitchShift3;
				index4 = k / pitchShift4;

//				if (index <= fftFrameSize2) {
//					gSynMagn[k] += gAnaMagn[index];
//					gSynFreq[k] = gAnaFreq[index] * pitchShift;
//				}
				
				if (index1 <= fftFrameSize2) {
					
					if ( (gAnaFreq[index1] >= 0) && (gAnaFreq[index1] <= tap[0]) ) {
						gSynMagn[k] += gAnaMagn[index1] * vol[0] ;
						gSynFreq[k] = gAnaFreq[index1] * pitchShift1;
					}
				}

				if (index2 <= fftFrameSize2) {
					
					if ( (gAnaFreq[index2] > tap[0]) && (gAnaFreq[index1] <= tap[1]) ) {
						gSynMagn[k] += gAnaMagn[index2] * vol [1];
						gSynFreq[k] = gAnaFreq[index2] * pitchShift2;
					}
				}

				if (index3 <= fftFrameSize2) {
					
					if ( (gAnaFreq[index3] > tap[1]) && (gAnaFreq[index1] <= tap[2]) ) {
						gSynMagn[k] += gAnaMagn[index3] * vol [2];
						gSynFreq[k] = gAnaFreq[index3] * pitchShift3;
					}
				}

				if (index4 <= fftFrameSize2) {
					
					if ( (gAnaFreq[index4] > tap[2]) && (gAnaFreq[index1] <= tap[3]) ) {
						gSynMagn[k] += gAnaMagn[index4] * vol [3];
						gSynFreq[k] = gAnaFreq[index4] * pitchShift4;
					}
				}
			}

			/* ***************** SYNTHESIS ******************* */
			/* this is the synthesis step */
			for (k = 0; k <= fftFrameSize2; k++) {

				/* get magnitude and true frequency from synthesis arrays */
				magn = gSynMagn[k];
				tmp = gSynFreq[k];

				/* subtract bin mid frequency */
				tmp -= (float)k*freqPerBin;

				/* get bin deviation from freq deviation */
				tmp /= freqPerBin;

				/* take osamp into account */
				tmp = 2.*M_PI*tmp/osamp;

				/* add the overlap phase advance back in */
				tmp += (float)k*expct;

				/* accumulate delta phase to get bin phase */
				gSumPhase[k] += tmp;
				phase = gSumPhase[k];

				/* get real and imag part and re-interleave */
				gFFTworksp[k][0] = magn*cos(phase);
				gFFTworksp[k][1] = magn*sin(phase);
			} 

			/* zero negative frequencies */

			//for (k = fftFrameSize+2; k < 2*fftFrameSize; k++) gFFTworksp[k] = 0.;

			for (k = ((fftFrameSize2)+1); (k < fftFrameSize); k++) 
																	{gFFTworksp[k][0] = 0.;
			 												         gFFTworksp[k][1] = 0.;}
			//Inverse transform
			//smbFft(gFFTworksp, fftFrameSize, 1);

			fftwf_execute(q);

			/* do windowing and add to output accumulator */ 
			
			for(k=0; k < fftFrameSize; k++) {
				
				//window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize)+.5;
				
				gOutputAccum[k] += 2. * han_window[k] *(gFFTworksp[k][0])/(fftFrameSize2*osamp);

			}
			for (k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];

			/* shift accumulator */
			memmove(gOutputAccum, gOutputAccum+stepSize, fftFrameSize*sizeof(float));

			/* move input FIFO */
			for (k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
		}
	}
}

// -----------------------------------------------------------------------------------------------------------------


// ~6 clocks on Pentium M vs. ~24 for single precision sqrtf
REALLY_INLINE float squareroot_sse_11bits( float x )
{
    float z;
    _asm
    {
        rsqrtss xmm0, x
        rcpss    xmm0, xmm0
        movss    z, xmm0            // z ~= sqrt(x) to 0.038%
    }
    return z;
}

REALLY_INLINE float arctan2(float y, float x)
{
	float coeff_1;
	float coeff_2;
	float abs_y;
	float r;
	float angle;

   coeff_1 = M_PI/4;
   coeff_2 = 3*coeff_1;
   abs_y = sabs(y)+1e-10 ;     // kludge to prevent 0/0 condition
   if (x>=0)
   {
      r = (x - abs_y) / (x + abs_y);
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      r = (x + abs_y) / (abs_y - x);
      angle = coeff_2 - coeff_1 * r;
   }
   if (y < 0)
   return(-angle);     // negate if in quad III or IV
   else
   return(angle);
}

REALLY_INLINE float sabs(float a)
{
int b=(*((int *)(&a)))&0x7FFFFFFF;
return *((float *)(&b));
}