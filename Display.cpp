#include "4WebSwitch.h"
#include "Display.h"
#include "Measure.h"
#include "KeyBoard.h"
#include "Rele.h"
#include "Web.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "Free_Fonts.h"

#define TEST_DISPLAY	

#define TOP_POS(Str)		 0
#define BOTTOM_POS(Str)		(Display.height() - Display.fontHeight())
#define LEFT_POS(Str)		 0
#define CENTER_POS(Str)		((Display.width() - Display.textWidth(String(Str)))/2)
#define RIGHT_POS(Str)		(Display.width() - Display.textWidth(String(Str)))



TFT_eSPI Display = TFT_eSPI();

PAGE_DESCRIPTOR DisplayPages[MAX_PAGES] = 
{
	{0 , "Home"   },
	{0 , "Misure" },
	{0 , "Rele"   },
	{0 , "Setup"  },
	{0 , "Setup all." },
	{0 , "Allarmi"},
	{0 , "Reset"}
};


uint8_t 	ActualPage;
uint8_t 	ButtonPress;
bool 		SelPageSelected;

void DisplayInit()
{
	Display.init();
	Display.setRotation(1);
	Display.fillScreen(TFT_BLACK);  
}

static void DrawTest()
{

  Display.fillScreen(TFT_BLACK);

  // Draw some random ellipses
  for (int i = 0; i < 40; i++)
  {
    int rx = random(60);
    int ry = random(60);
    int x = rx + random(320 - rx - rx);
    int y = ry + random(240 - ry - ry);
    Display.fillEllipse(x, y, rx, ry, random(0xFFFF));
  }

  delay(2000);

}

static void DrawStringBox(String Str, bool Selected, int32_t StrXPos, int32_t StrYPos)
{
	if(Selected)
		Display.fillRoundRect( StrXPos - 2,  StrYPos - 2,  Display.textWidth(Str) + 2,  Display.fontHeight() + 2,  2,  TFT_WHITE);
	else
		Display.drawRoundRect( StrXPos - 2,  StrYPos - 2,  Display.textWidth(Str) + 2,  Display.fontHeight() + 2,  2,  TFT_WHITE);

}

// Nome dello strumento, v FW, sotto ora data e icone varie
static void DrawTopInfoBar()
{
	
}


static void DrawPageChange(int8_t Page, bool Selected)
{
	int32_t XPos = CENTER_POS(DisplayPages[Page].PageName), YPos = BOTTOM_POS(DisplayPages[Page].PageName);
	
	Display.setFreeFont(FMB12);
	Display.drawString((String)DisplayPages[Page].PageName, XPos, YPos);
	DrawStringBox((String)DisplayPages[Page].PageName, Selected, XPos, YPos);
}


static void RefreshMainScreen()
{
	const int32_t CircleRadius = 35;
	SelPageSelected = true;
	for(int i = 0; i < N_RELE; i++)
	{
		if(Rele.getReleStatus(i) == STATUS_OFF)
			Display.fillCircle((CircleRadius + 8) + (i * ((CircleRadius * 2) + 2)), 150, CircleRadius, TFT_RED);
		else
			Display.fillCircle((CircleRadius + 8) + (i * ((CircleRadius * 2) + 2)), 150, CircleRadius, TFT_GREEN);
	}
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(DisplayPages[ActualPage].PageToChange > 0)
			{
				if(DisplayPages[ActualPage].PageToChange - 1 == ActualPage)
				{
					if(DisplayPages[ActualPage].PageToChange - 2 > 0 )
						DisplayPages[ActualPage].PageToChange -= 2;
					else
						DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
				}
				else
					DisplayPages[ActualPage].PageToChange--;
			}
			else
				DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
			break;
		case B_DOWN:
			if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
			{
				if(DisplayPages[ActualPage].PageToChange + 1 == ActualPage)
				{
					if(DisplayPages[ActualPage].PageToChange + 2 < MAX_PAGES )
						DisplayPages[ActualPage].PageToChange += 2;
					else
						DisplayPages[ActualPage].PageToChange = 0;
				}
				else
					DisplayPages[ActualPage].PageToChange++;
			}
			else
				DisplayPages[ActualPage].PageToChange = 0;
			break;
		case B_OK:
			ActualPage = DisplayPages[ActualPage].PageToChange;
			break;
		default:
			break;
	}
	DrawPageChange(DisplayPages[ActualPage].PageToChange, SelPageSelected);
}

void TaskDisplay()
{
#ifdef TEST_DISPLAY
	DrawTest();		
#else
	
	DrawTopInfoBar();
	switch(ActualPage)
	{
		case MAIN_PAGE:
			RefreshMainScreen();
			break;
		case MEASURE_PAGE:
			break;
		case RELE_PAGE:
			break;
		case SETUP_PAGE:
			break;
		case ALARM_SETUP_PAGE:
			break;
		case ALARM_STATUS_PAGE:
			break;
		case RESET_PAGE:
			break;
		default:
			break;
	}
	







#endif
}