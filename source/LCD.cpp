#include "vstgui.sf\vstgui\vstgui.h"
#include "lcd.h"
#include "math.h"

class lcd : public CView {
public:
	lcd(CRect &size) : CView(size) {}

     ~lcd() {}

      void draw(CDrawContext *dc) { lcd_draw(dc); setDirty(false); }
     
	  
	  void setPitchParams(float p1, int whichpitch)
	
      {
		  if (whichpitch < 4)
          pitchparm[whichpitch] = p1;
          setDirty();
      }
	  
	  void setVolParams(float v1, int whichvol)

	  {
		  if (whichvol < 4)
          volparm[whichvol] = v1;
          setDirty();
      }

	  void setTapParams(float t1, int whichtap)

	  {
		  if (whichtap < 4)
          tapparm[whichtap] = t1;
          setDirty();
      }
 
private: 
     float pitchparm[4];
	 float volparm[4];
	 float tapparm[4];
     unsigned char r[4],g[4],b[4];
	 unsigned int tap_offset[4];
	 unsigned int volume_offset[4];

	 CColor Clr[4];
	 CColor FrameColor;

	 void lcd_draw(CDrawContext *dc)

	{
	
	
	
	for (int k = 0; k < 4; k++)

	{
//Calculate colors 
		
		r[k] = (char)(((LCD_PITCHED_R - LCD_BACKGROUND_R) * (1-pitchparm[k])) + LCD_BACKGROUND_R);
		g[k] = (char)(((LCD_PITCHED_R - LCD_BACKGROUND_G) * (1-pitchparm[k])) + LCD_BACKGROUND_G);
		b[k] = (char)(((LCD_PITCHED_R - LCD_BACKGROUND_B) * (1-pitchparm[k])) + LCD_BACKGROUND_B);			
	
		Clr[k](r[k],g[k],b[k],0);
		FrameColor (0,0,0,0);

		dc->setFillColor(Clr[k]);
		dc->setFrameColor(FrameColor);


//Calculate column height, adapted to display size.

		volume_offset[k] = (int)((1 - volparm[k])*(BOTTOM_DISPLAY-TOP_DISPLAY));

		
		double taptemp;
	
		taptemp = ((tapparm[k]/20000)); // Linear Scale
		
		taptemp = taptemp * (RIGHT_DISPLAY - LEFT_DISPLAY); // Adapt to display size
		
		tap_offset[k] = (int) taptemp;

		if (k == 0)				//First endpoint, 0 to tap0
		{
			dc->drawRect((CRect ((LEFT_DISPLAY),
							 (TOP_DISPLAY + volume_offset[k]),
							 (LEFT_DISPLAY + tap_offset[k]),
							 BOTTOM_DISPLAY)),
							 kDrawFilledAndStroked);

		}

		else					//Second to fourth endpoint, tap k-1 to tap k
		{
			dc->drawRect((CRect ((LEFT_DISPLAY + tap_offset[k-1]),
							 (TOP_DISPLAY + volume_offset[k]),
							 (LEFT_DISPLAY + tap_offset[k]),
							 BOTTOM_DISPLAY)),
							 kDrawFilledAndStroked);
		}

		

	}
	 }
};
