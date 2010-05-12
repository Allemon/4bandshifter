#include "bernspitch.h"
#include "math.h"
#include "stdio.h"
#include "BSedit.h"
#include "LCD.h"

extern "C" {

#include "fftw3.h"

}



#define MODE	"SINGLE"

#define N	1024



//-------------------------------------------------------------------------------------------------------
void smbPitchShift(float pitchShift1, float pitchShift2, float pitchShift3, float pitchShift4, float *vol, int *tap, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);

AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new PhaseVoc (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
PhaseVoc::PhaseVoc (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, 1, kNumParams)	// 1 program, knum parameters
{
	setNumInputs (1);		// stereo in
	setNumOutputs (1);		// stereo out
	setUniqueID ('PFuV');	// identify

	

	canProcessReplacing ();	// supports replacing output
	//canDoubleReplacing ();	// supports double precision processing

	fSemitones1 = fSemitones2 = fSemitones3 = fSemitones4 = 0;	// default to 0 semitones (fsemitones is [-12,12]
	
		vol[0] =
		vol[1] = 
		vol[2] = 
		vol[3] = 1;				    	// default full volume
	
	
	tap[0] = 1200;												//default taps
	tap[1] = 4500;
	tap[2] = 11000;
	tap[3] = 20000;

	discvalue = 1;
	
	
	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
	
	editor = new BSEdit (this); //Opens the editor, comment this line for host knob rendering
	
}

//-------------------------------------------------------------------------------------------------------
PhaseVoc::~PhaseVoc ()
{
	// nothing to do here
}

//-------------------------------------------------------------------------------------------------------
void PhaseVoc::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void PhaseVoc::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void PhaseVoc::setParameter (VstInt32 index, float value)
{
	switch (index)
	{
	
	// Semitones; from parameter [0,1] to stored value inside the plug
	
	case kpitch1parm:
		if (discvalue)								// [0,1] -> [-12,12] either in discrete or continuous mode
		{
			fSemitones1 = ((int)(value *24)-12);
		} 

		else
		{
			fSemitones1 = ((value *24)-12);
		}

		if (editor)
		{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

	case kpitch2parm:
		if (discvalue)								// [0,1] -> [-12,12] either in discrete or continuous mode
		{
			fSemitones2 = ((int)(value *24)-12);
		} 

		else
		{
			fSemitones2 = ((value *24)-12);
		}

		if (editor)
		{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

	case kpitch3parm:
		if (discvalue)								// [0,1] -> [-12,12] either in discrete or continuous mode
		{
			fSemitones3 = ((int)(value *24)-12);
		} 

		else
		{
			fSemitones3 = ((value *24)-12);
		}

		if (editor)
		{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

	case kpitch4parm:
		if (discvalue)								// [0,1] -> [-12,12] either in discrete or continuous mode
		{
			fSemitones4 = ((int)(value *24)-12);
		} 

		else
		{
			fSemitones4 = ((value *24)-12);
		}

		if (editor)
		{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

	
	// Volume multipliers

	case kvol1parm:
	case kvol2parm:
	case kvol3parm:
	case kvol4parm:
		vol[index-4] = value;
		if (editor)
			{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

		// Band tap points

	case ktap1parm:
		tap[0] = ((int) (value*tap[1]));									//[0,1] -> [0,tap[1]]
		((BSEdit *)editor)->punto->setTapParams(tap[0], 0);
		if (editor)
			{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

	case ktap2parm:
		tap[1] = ((int) (value* (tap[2] - tap[0]) + tap[0] ));				//[0,1] -> [tap[1],tap[2]]
		((BSEdit *)editor)->punto->setTapParams(tap[1], 1);
		if (editor)
			{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

	case ktap3parm:
		tap[2] = ((int) (value* (tap[3] - tap[1]) + tap[1] ));				//[0,1] -> [tap[2],tap[3]]
		((BSEdit *)editor)->punto->setTapParams(tap[2], 2);
		if (editor)
			{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

	case ktap4parm:
		tap[3] = ((int) (value* (20000 - tap[2]) + tap [2] ));				//[0,1] -> [tap[3],20000]
		{((BSEdit *)editor)->punto->setTapParams(tap[3], 3);}
		if (editor)
			{((AEffGUIEditor*)editor)->setParameter (index, value);}
		break;

	case kdiscrete:
		discvalue = value;
		
	
}
}

//-----------------------------------------------------------------------------------------
float PhaseVoc::getParameter (VstInt32 index)
{
	switch(index)
	{
	case 0:
		return ((fSemitones1/24)+0.5); // [-12,12] -> [0,1]
		break;

	case 1:
		return ((fSemitones2/24)+0.5);
		break;

	case 2:
		return ((fSemitones3/24)+0.5);
		break;

	case 3:
		return ((fSemitones4/24)+0.5);
		break;

	// Volumes

	case 4:
		return vol[0];
		break;

	case 5:
		return vol[1];
		break;

	case 6:
		return vol[2];
		break;

	case 7:
		return vol[3];
		break;

	// Band starting points

	case 8:
		return ( (float)tap[0] / (float)tap[1] );											//[0,tap0] -> [0,1]
		break;

	case 9:
		return ( (float)(tap[1] - tap[0]) / (float)(tap[2] - tap[0]) );						//[tap0,tap1] -> [0,1]
		break;

	case 10:
		return ( (float)(tap[2] - tap[1]) / (float)(tap[3] - tap[1]) );						//[tap1,tap2] -> [0,1]
		break;

	case 11:
		if (tap[2] == 20000) return (1);													// Avoid 0/0 condition
			else return ( (float)(tap[3] - tap[2]) / (float)(20000  - tap[2]) );			//[tap3,20000] -> [0,1]
		break;	

	//Discrete

	case kdiscrete:
		if (discvalue > 0) return (1);
		else return (0);
	}
}

//-----------------------------------------------------------------------------------------
void PhaseVoc::getParameterName (VstInt32 index, char* label)
{
	if (index == 0)
		vst_strncpy (label, "Shift 1", kVstMaxParamStrLen);

	if (index == 1)
		vst_strncpy (label, "Shift 2", kVstMaxParamStrLen);

	if (index == 2)
		vst_strncpy (label, "Shift 3", kVstMaxParamStrLen);

	if (index == 3)
		vst_strncpy (label, "Shift 4", kVstMaxParamStrLen);

	if (index == 4)
		vst_strncpy (label, "Vol 1", kVstMaxParamStrLen);

	if (index == 5)
		vst_strncpy (label, "Vol 2", kVstMaxParamStrLen);

	if (index == 6)
		vst_strncpy (label, "Vol 3", kVstMaxParamStrLen);

	if (index == 7)
		vst_strncpy (label, "Vol 4", kVstMaxParamStrLen);

	if (index == 8)
		vst_strncpy (label, "Tap 1", kVstMaxParamStrLen);

	if (index == 9)
		vst_strncpy (label, "Tap 2", kVstMaxParamStrLen);

	if (index == 10)
		vst_strncpy (label, "Tap 3", kVstMaxParamStrLen);

	if (index == 11)
		vst_strncpy (label, "Tap 4", kVstMaxParamStrLen);

	if (index == kdiscrete)
		vst_strncpy (label, "DISC", kVstMaxParamStrLen);
}

//-----------------------------------------------------------------------------------------
void PhaseVoc::getParameterDisplay (VstInt32 index, char* text)
{
	//int2string (int(((fSemitones *24)-12)), text, 8);
	
	if (index == 0)
		sprintf_s (text, 4, "%d", (signed int(fSemitones1)));

	if (index == 1)
		sprintf_s (text, 4, "%d", (signed int(fSemitones2)));

	if (index == 2)
		sprintf_s (text, 4, "%d", (signed int(fSemitones3)));

	if (index == 3)
		sprintf_s (text, 4, "%d", (signed int(fSemitones4)));

	//Volumes

	if (index == 4)
		sprintf_s (text, 40, "%4.2f", vol[0]);

	if (index == 5)
		sprintf_s (text, 40, "%4.2f", vol[1]);

	if (index == 6)
		sprintf_s (text, 40, "%4.2f", vol[2]);

	if (index == 7)
		sprintf_s (text, 40, "%4.2f", vol[3]);

	//Band starting points

	if (index == 8)
		sprintf_s (text, 12, "%d Hz", (signed int(tap[0])));

	if (index == 9)
		sprintf_s (text, 12, "%d Hz", (signed int(tap[1])));

	if (index == 10)
		sprintf_s (text, 12, "%d Hz", (signed int(tap[2])));

	if (index == 11)
		sprintf_s (text, 12, "%d Hz", (signed int(tap[3])));

	if (index == kdiscrete)
		sprintf_s (text, 12, "%d", discvalue);

}

//-----------------------------------------------------------------------------------------
void PhaseVoc::getParameterLabel (VstInt32 index, char* label)
{
	if (index <= 3)
		vst_strncpy (label, "Semitones", kVstMaxParamStrLen);
	
	if ((index > 3) && (index <= 7))
		vst_strncpy (label, "Volume", kVstMaxParamStrLen);

	if ((index > 7) && (index <= 11))
		vst_strncpy (label, "Band Start", kVstMaxParamStrLen);

	if (index == kdiscrete)
		vst_strncpy (label, "Discrete", kVstMaxParamStrLen);

}	

//------------------------------------------------------------------------
bool PhaseVoc::getEffectName (char* name)
{
	vst_strncpy (name, "4-Tap PitchShifter", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool PhaseVoc::getProductString (char* text)
{
	vst_strncpy (text, "4Tap", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool PhaseVoc::getVendorString (char* text)
{
	vst_strncpy (text, "Jane's Conference", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 PhaseVoc::getVendorVersion ()
{ 
	return 1000; 
}

//-----------------------------------------------------------------------------------------
void PhaseVoc::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* in1  =  inputs[0];
    //float* in2  =  inputs[1];
    float* out1 = outputs[0];
    //float* out2 = outputs[1];

				
			float pitchShift1 = pow(2., fSemitones1/12.);
			float pitchShift2 = pow(2., fSemitones2/12.);
			float pitchShift3 = pow(2., fSemitones3/12.);
			float pitchShift4 = pow(2., fSemitones4/12.);
	
			smbPitchShift(pitchShift1, pitchShift2, pitchShift3, pitchShift4, vol, tap, sampleFrames, 2048, 4, 44100, in1, out1);
			in1 = in1 + sampleFrames;
			out1 = out1 + sampleFrames;
			