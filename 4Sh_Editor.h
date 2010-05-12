#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#endif

class BSEditor: public AEffGUIEditor, public CControlListener
{
public:
	BSEditor (AudioEffect *effect);
	virtual ~BSEditor ();

	public:
	virtual bool open (void *ptr);
	virtual void close ();

	virtual void setParameter (VstInt32 index, float value);
	virtual void valueChanged (CDrawContext* context, CControl* control);

	private:

	// Controls
	
	CAnimKnob *Pitch1;
	CAnimKnob *Pitch2;
	CAnimKnob *Pitch3;
	CAnimKnob *Pitch4;
	
	CAnimKnob *Vol1;
	CAnimKnob *Vol2;
	CAnimKnob *Vol3;
	CAnimKnob *Vol4;

	CAnimKnob *Tap1;
	CAnimKnob *Tap2;
	CAnimKnob *Tap3;
	CAnimKnob *Tap4;
	
	CParamDisplay *Pitch1D;
	CParamDisplay *Pitch2D;
	CParamDisplay *Pitch3D;
	CParamDisplay *Pitch4D;
	
	CParamDisplay *Vol1D;
	CParamDisplay *Vol2D;
	CParamDisplay *Vol3D;
	CParamDisplay *Vol4D;

	CParamDisplay *Tap1D;
	CParamDisplay *Tap2D;
	CParamDisplay *Tap3D;
	CParamDisplay *Tap4D;

	CMovieButton *Discrete;

	// Bitmap
	
	CBitmap *hBackground;

	// Others
	bool bOpened;
};