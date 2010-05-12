//BSEdit.h - VSTGUI editor file, automatically generated by VSTGUI Builder

#ifndef BSEdit_H_
#define BSEdit_H_

#include "vstgui.sf\vstgui\vstgui.h"
#include "LCD.cpp"

#ifndef __vstcontrols__
#include "vstcontrols.h"
#endif

class BSEdit : public AEffGUIEditor, public CControlListener
{
  public:
    BSEdit(AudioEffect *effect);
    ~BSEdit();

  public:
	virtual bool open (void *ptr);
	virtual void close ();

	virtual void setParameter (VstInt32 index, float value);
	virtual void valueChanged (CDrawContext* context, CControl* control);
	  
    //long controlModifierClicked(CDrawContext *pContext, CControl *pControl, long button) {return 0;};

    //void setParameter(long index, float value);
    //void valueChanged(CDrawContext* context, CControl* control);
    //void idle();

	//Declaration of pointer for new LCD view    
	public:	
	lcd *punto;

	private:

	//void floatToFreq (float value, char* string);
    
	//Controls
    
	CAnimKnob *tap1;
    CAnimKnob *tap2;
    CAnimKnob *tap3;
    CAnimKnob *tap4;
    CAnimKnob *shift1;
    CAnimKnob *shift2;
    CAnimKnob *shift3;
    CAnimKnob *shift4;
    CAnimKnob *vol1;
    CAnimKnob *vol2;
    CAnimKnob *vol3;
    CAnimKnob *vol4;
    COnOffButton *discrete;
    CParamDisplay *pitchdisplay1;
    CParamDisplay *pitchdisplay2;
    CParamDisplay *pitchdisplay3;
    CParamDisplay *pitchdisplay4;
    CParamDisplay *voldisplay1;
    CParamDisplay *voldisplay2;
    CParamDisplay *voldisplay3;
    CParamDisplay *voldisplay4;
    CParamDisplay *tapdisplay1;
    CParamDisplay *tapdispaly2;
    CParamDisplay *tapdisplay3;
    CParamDisplay *tapdisplay4;

    //Bitmaps
    CBitmap *hBack;
    CBitmap *hknob_med;
    CBitmap *hMknob_med;
    CBitmap *hbutton_sm;

};

#endif
			