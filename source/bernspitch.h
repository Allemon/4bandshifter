//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Version 2.4		$Date:  $
//
// Category     : 
// Filename     : 
// Created by   : 
// Description  : 
//
// © 2006
//-------------------------------------------------------------------------------------------------------

#ifndef __phasevoc__
#define __phasevoc__

#include "public.sdk/source/vst2.x/audioeffectx.h"

enum
{
	
	// Parameters Tags
	kpitch1parm = 0,
	kpitch2parm,
	kpitch3parm,
	kpitch4parm,

	kvol1parm,
	kvol2parm,
	kvol3parm,
	kvol4parm,

	ktap1parm,
	ktap2parm,
	ktap3parm,
	ktap4parm,

	kdiscrete,

	kNumParams
};

#define BUF_LENGTH	8192

//-------------------------------------------------------------------------------------------------------
class PhaseVoc : public AudioEffectX
{
public:
	PhaseVoc (audioMasterCallback audioMaster);
	~PhaseVoc ();

	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	//virtual void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames);

	// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

	// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
	// int getRealValues (VstInt32 index);

public:
	int tap[4];

protected:
	float fSemitones1;
	float fSemitones2;
	float fSemitones3;
	float fSemitones4;
	float vol[4];
	int discvalue;
	//int tap[4];
	char programName[kVstMaxProgNameLen+1];
	
};

#endif
