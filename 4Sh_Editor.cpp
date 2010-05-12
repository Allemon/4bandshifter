#include "4Sh_Editor.h"
#include "bernspitch.h"

#include <stdio.h>

enum {
	// bitmaps
	kBackgroundId = 128,
	kFaderBodyId,
	kFaderHandleId,
	
	// positions
	//Pitch Knobs
	
	Pitch1X = 51,
	Pitch1Y = 134,

	Pitch2X = 129,
	Pitch2Y = 134,

	Pitch3X = 206,
	Pitch3Y = 134,

	Pitch4X = 284,
	Pitch4Y = 134,

	// Pitch Displays

	Pitch1DX
	Pitch1DY

	Pitch2DX
	Pitch2DY
	
	Pitch3DX
	Pitch3DY
	
	Pitch4DX
	Pitch4DY

	//Pitch/Vol Display size

	PVWidth
	PVHeight

	//Vol Knobs

	Vol1X
	Vol1Y

	Vol2X
	Vol2Y

	Vol3X
	Vol3Y

	Vol4X
	Vol4Y

	//Vol Displays

	Vol1DX
	Vol1DY

	Vol2DX
	Vol2DY

	Vol3DX
	Vol3DY

	Vol4DX
	Vol4DY
	
	//Tap Knobs

	Tap1X
	Tap1Y

	Tap2X
	Tap2Y

	Tap3X
	Tap3Y

	Tap4X
	Tap4Y

	//Tap Displays

	Tap1DX
	Tap1DY

	Tap2DX
	Tap2DY

	Tap3DX
	Tap3DY

	Tap4DX
	Tap4DY

	//Tap Display size

	TapWidth
	TapHeight

	//Button

	ButtonX
	ButtonY

};

{
	// Some colors well use

CColor kLightGray = {204, 204, 204, 0};
CColor kMedGray = {238, 238, 238, 0};
CColor kDarkGray = {51, 51, 51, 0};

}

void floatToSamples (float value, char* string)
{
	 sprintf (string, "%d", (int)(10000 * value));
}

BSEditor::BSEditor (AudioEffect *effect)
 : AEffGUIEditor (effect) 
{
	Pitch1 = Pitch2 = Pitch3 = Pitch4 = 0;
	Vol1 = Vol2 = Vol3 = Vol4 = 0;
	Tap1 = Tap2 = Tap3 = Tap4 = 0;
	Pitch1D = Pitch2D = Pitch3D = Pitch4D = 0;
	Vol1D = Vol2D = Vol3D = Vol4D = 0;
	Tap1D = Tap2D = Tap3D = Tap4D = 0;
	Discrete = 0;

	// load the background bitmap
	// we don't need to load all bitmaps, this could be done when open is called
	hBackground = new CBitmap (kBackgroundId);

	// init the size of the plugin
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = (short)hBackground->getWidth ();
	rect.bottom = (short)hBackground->getHeight ();
}

BSEditor::~BSEditor ()
{
	// free the background bitmap
	if (hBackground)
		hBackground->forget ();
	hBackground = 0;
}

bool SDEditor::open (void *ptr)
{
	// !!! always call this !!!
	AEffGUIEditor::open (ptr);
	
	//--load some bitmaps
	CBitmap* BigKnob   = new CBitmap (kBigKnobId);
	CBitmap* LilKnob = new CBitmap (kLilKnobId);
	CBitmap* D_Button = new Cbitmap (kD_ButtonId);

	//--init background frame-----------------------------------------------
	// We use a local CFrame object so that calls to setParameter won't call into objects which may not exist yet. 
	// If all GUI objects are created we assign our class member to this one. See bottom of this method.
	
	CRect size (0, 0, hBackground->getWidth (), hBackground->getHeight ());
	CFrame* lFrame = new CFrame (size, ptr, this);
	lFrame->setBackground (hBackground);

