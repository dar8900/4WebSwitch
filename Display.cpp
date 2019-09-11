#include "4WebSwitch.h"
#include "Display.h"
#include "Measure.h"
#include "KeyBoard.h"
#include "Rele.h"
#include "Web.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "Free_Fonts.h"
#include "WifiIcons.h"
#include "Alarms.h"

// #define TEST_DISPLAY	
#define BG_COLOR			TFT_BLACK

#define TOP_POS				 0
#define BOTTOM_POS(Str)		(Display.height() - Display.fontHeight())
#define LEFT_POS			 0
#define CENTER_POS(Str)		((Display.width() - Display.textWidth(String(Str)))/2)
#define RIGHT_POS(Str)		(Display.width() - Display.textWidth(String(Str)))

#define MAX_MEASURE_LINE	3

TFT_eSPI Display = TFT_eSPI();


Chrono RefreshTopbarTimer;

enum
{
	CHANGE_RELE_STATUS_ITEM = 0,
	CHANGE_PAGE_ITEM,
	MAX_RELE_PAGE_ITEMS
};

enum
{
	RESET_SELECTION = 0,
	PAGE_RESET_SEL,
	MAX_RESET_PAGE_ITEMS
};

const unsigned char *WifiIcons[] = 
{
	NoSignal_bits,
	Pessimo_bits,
	Buono_bits,
	Ottimo_bits,
};

PAGE_DESCRIPTOR DisplayPages[MAX_PAGES] = 
{
	{MEASURE_PAGE 		, "Home"          },
	{MEASURE_PAGE       , "Misure"        },
	{RELE_PAGE          , "Rele"          },
	{SETUP_PAGE         , "Setup"         },
	{ALARM_SETUP_PAGE   , "Setup allarmi" },
	{ALARM_STATUS_PAGE  , "Allarmi"       },
	{RESET_PAGE         , "Reset"         },
};

const MEASURE_PAGES MeasuresPage[MAX_MEASURE_PAGES] = 
{
	{&Measures.CurrentRMS, 			     &Measures.VoltageRMS, 			       &Measures.PowerFactor	   	       , "A" , "V"   , " "  },
	{&Measures.ActivePower, 		     &Measures.ReactivePower,			   &Measures.ApparentPower			   , "W" , "VAr" , "VA" },
	{&Measures.ActiveEnergy, 	         &Measures.ReactiveEnergy, 			   &Measures.ApparentEnergy		       , "Wh", "VArh", "VAh"},
	{&Measures.PartialActiveEnergy,      &Measures.PartialReactiveEnergy, 	   &Measures.PartialApparentEnergy	   , "Wh", "VArh", "VAh"},
	{&Measures.MaxMinAvg.MaxCurrent,     &Measures.MaxMinAvg.MaxVoltage, 	   &Measures.MaxMinAvg.MaxPowerFactor  , "A" , "V"   , " "  },
	{&Measures.MaxMinAvg.MaxActivePower, &Measures.MaxMinAvg.MaxReactivePower, &Measures.MaxMinAvg.MaxApparentPower, "W" , "VAr" , "VA" },
	{&Measures.MaxMinAvg.MinCurrent,     &Measures.MaxMinAvg.MinVoltage, 	   &Measures.MaxMinAvg.MinPowerFactor  , "A" , "V"   , " "  },
	{&Measures.MaxMinAvg.MinActivePower, &Measures.MaxMinAvg.MinReactivePower, &Measures.MaxMinAvg.MinApparentPower, "W" , "VAr" , "VA" },
	{&Measures.MaxMinAvg.CurrentAvg,     &Measures.MaxMinAvg.VoltageAvg, 	   &Measures.MaxMinAvg.PowerFactorAvg  , "A" , "V"   , " "  },
	{&Measures.MaxMinAvg.ActivePowerAvg, &Measures.MaxMinAvg.ReactivePowerAvg, &Measures.MaxMinAvg.ApparentPowerAvg, "W" , "VAr" , "VA" },
};

const char *MeasurePageTitle[MAX_MEASURE_PAGES] = 
{
	"I, V, PF",
	"Potenze",
	"Energie tot.",
	"Energie parz.",
	"Massimi I, V, PF",
	"Massimi potenze",
	"Minimi I, V, PF",
	"Minimi potenze",
	"Medie I, V, PF",
	"Medie potenze",
};

const REFORMAT ReformatTab[] = 
{
	{0.001       , 1000       , "m"},
	{0.01        , 1000       , "m"},
	{0.1         , 1000       , "m"},
	{1.0         ,  1.0       , " "},
	{10.0        ,  1.0       , " "},
	{100.0       ,  1.0       , " "},
	{1000.0      , 0.001      , "k"},
	{10000.0     , 0.001      , "k"},
	{100000.0    , 0.001      , "k"},
	{1000000.0   , 0.000001   , "M"},
	{10000000.0  , 0.000001   , "M"},
	{100000000.0 , 0.000001   , "M"},
	{1000000000.0, 0.00000001 , "G"},
	
};


const RESET_S Reset[MAX_RESET_ITEMS] = 
{
	{"Reset energie"		 ,		ResetTotalEnergy  },
	{"Reset energie parziali",      ResetPartialEnergy},
	{"Reset max e min"		 ,      ResetMaxMin       },
	{"Reset medie"	   	     ,      ResetAvg          },
	{"Restart switch"		 ,      ResetMcu          },
};


static uint8_t 	ButtonPress;
static bool 	SelPageSelected;
static uint8_t 	ActualPage = MAIN_PAGE;
static bool     RefreshBottomBar = true, PageChanged = false;

static uint8_t 	MeasurePageSelection = LINE_MEASURES;
static String   MeasurePageNumber = "";
static bool 	MeasurePageChanged = false;

static uint8_t 	RelePageItemSel;
static uint8_t 	ReleIndex;
static bool     RefreshReleChange = false;

static uint8_t  ResetOrPage;
static uint8_t  ResetItem = RESET_ENERGIES;
static bool     RefreshResetItem = false;





void DisplayInit()
{
	Display.init();
	Display.setRotation(3);
	Display.fillScreen(BG_COLOR);  
}

static void ClearScreen(bool FullScreen)
{
	int Ypos;
	if(FullScreen)
		Display.fillScreen(BG_COLOR); 
	else
	{
		Ypos = 14;			
		Display.fillRect( 0,  Ypos,  Display.width(),  Display.height() - Ypos , BG_COLOR);
	}
}

void DrawPopUp(const char *Text, uint16_t Delay)
{
	String TextStr = String(Text);
	ClearScreen(false);
	Display.setFreeFont(FMB18);
	Display.drawRoundRect(0, 14, Display.width(), Display.height() - 14, 2, TFT_WHITE);
	Display.drawString(TextStr, CENTER_POS(TextStr), (Display.height() - Display.fontHeight())/2, TFT_WHITE);
	delay(Delay);
	ClearScreen(false);
}

// Nome dello strumento, v FW, sotto ora data e icone varie
static void DrawTopInfoBar()
{
	if(RefreshTopbarTimer.hasPassed(500, true))
	{
		Display.setFreeFont(FM9);	
		Display.drawString(TimeFormatted, LEFT_POS, TOP_POS);
		Display.drawString(DateFormatted, RIGHT_POS(DateFormatted), TOP_POS);
		Display.drawXBitmap(160, 0, WifiIcons[WifiSignal], 12, 12, TFT_CYAN);
	}
}

static void DrawPageChange(int8_t Page, bool Selected)
{
	int32_t XPos = CENTER_POS(DisplayPages[Page].PageName), YPos = BOTTOM_POS(DisplayPages[Page].PageName);
	
	Display.setFreeFont(FMB9);	
	Display.fillRect( 0,  Display.height() - Display.fontHeight() - 4,  Display.width(),  Display.height() , BG_COLOR);
	Display.drawString((String)DisplayPages[Page].PageName, XPos, YPos);
	if(Selected)
		Display.drawRoundRect( XPos - 2,  YPos - 2,  Display.textWidth((String)DisplayPages[Page].PageName) + 4,  Display.fontHeight() + 2,  2,  TFT_WHITE);
	else
		Display.drawRoundRect( XPos - 2,  YPos - 2,  Display.textWidth((String)DisplayPages[Page].PageName) + 4,  Display.fontHeight() + 2,  2,  BG_COLOR);
	Display.drawString((String)DisplayPages[Page].PageName, XPos, YPos);
}



static void DrawReleStatus()
{
	const int32_t CircleRadius = 35;
	String NomePresa = "";
	Display.setFreeFont(FM9);
	for(int i = 0; i < N_RELE; i++)
	{
		NomePresa = String(i + 1);
		if(Rele.getReleStatus(i) == STATUS_OFF)
			Display.fillCircle((CircleRadius + 10) + (i * ((CircleRadius * 2) + 6)), 150, CircleRadius, TFT_RED);
		else
			Display.fillCircle((CircleRadius + 10) + (i * ((CircleRadius * 2) + 6)), 150, CircleRadius, TFT_GREEN);
		Display.drawString("Presa", (CircleRadius + 10 - Display.textWidth("Presa") / 2) + (i * (Display.textWidth("Presa") + 6 + CircleRadius - 20)), 	150 - (Display.fontHeight() * 2) - 38 );
		Display.drawString(NomePresa, (CircleRadius + 6) + (i * ((CircleRadius * 2) + 4)), 150 - (Display.fontHeight()) - 38 );
	}	
}



static void DrawMainScreen()
{
	SelPageSelected = true;
	Display.setFreeFont(FMB12);
	DrawReleStatus();
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(DisplayPages[ActualPage].PageToChange > 0)
			{
				DisplayPages[ActualPage].PageToChange--;
			}
			else
				DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
			RefreshBottomBar = true;
			break;
		case B_DOWN:
			if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
			{
				DisplayPages[ActualPage].PageToChange++;
			}
			else
				DisplayPages[ActualPage].PageToChange = 0;
			RefreshBottomBar = true;
			break;
		case B_LEFT:
			if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
			{
				DisplayPages[ActualPage].PageToChange++;
			}
			else
				DisplayPages[ActualPage].PageToChange = 0;
			RefreshBottomBar = true;
			break;			
		case B_OK:
			ActualPage = DisplayPages[ActualPage].PageToChange;
			PageChanged = true;
			break;
		default:
			break;
	}

}

static uint8_t SearchRange(double Value)
{
	int i = 0;
	for(i = 0; i < 13; i++)
	{
		if(Value < ReformatTab[i].Value)
		{
			if(i > 0)
			{
				i--;
				break;
			}
			else
				break;
		}
	}
	return i;
}

static void FormatAndPrintMeasure(uint8_t MeasurePageNumber)
{
	uint8_t Range = 0;
	String MeasureToPrint = "", UdmRF = "";
	for(int Line = 0; Line < MAX_MEASURE_LINE; Line++)
	{
		double ActualMeasure = 0.0;
		String Udm = "";

		if(Line == 0)
		{
			ActualMeasure = *MeasuresPage[MeasurePageNumber].FirstLineMeasure;
			Udm = String(MeasuresPage[MeasurePageNumber].UdmFirstLine);
		}
		else if(Line == 1)
		{
			ActualMeasure = *MeasuresPage[MeasurePageNumber].SecondLineMeasure;
			Udm = String(MeasuresPage[MeasurePageNumber].UdmSecondLine);
		}
		else
		{
			ActualMeasure = *MeasuresPage[MeasurePageNumber].ThirdLineMeasure;
			Udm = String(MeasuresPage[MeasurePageNumber].UdmThirdLine);
		}
		Range = SearchRange(ActualMeasure);
		ActualMeasure *= ReformatTab[Range].RefactorValue;
		MeasureToPrint = String(ActualMeasure, 3);
		UdmRF += String(ReformatTab[Range].Odg) + Udm;
		Display.setFreeFont(FMB18);
		Display.drawString(MeasureToPrint, CENTER_POS(MeasureToPrint), 34 + (Line * (Display.fontHeight() + 10)));
		Display.setFreeFont(FM9);
		Display.drawString(UdmRF, RIGHT_POS(UdmRF), 26 + (Line * (38)));	
	}
	Display.setFreeFont(FM9);
	String MeasurePageNumberStr = String(MeasurePageNumber + 1) + "/" + String(MAX_MEASURE_PAGES);
	Display.drawString(MeasurePageNumberStr, CENTER_POS(MeasurePageNumberStr), 210);
}


static void DrawMeasurePage()
{
	SelPageSelected = true;
	FormatAndPrintMeasure(MeasurePageSelection);
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(MeasurePageSelection > LINE_MEASURES)
			{
				MeasurePageSelection--;
			}
			else
				MeasurePageSelection = MAX_MEASURE_PAGES - 1;
			MeasurePageChanged = true;
			break;
		case B_DOWN:
			if(MeasurePageSelection < MAX_MEASURE_PAGES - 1)
			{
				MeasurePageSelection++;
			}
			else
				MeasurePageSelection = LINE_MEASURES;
			MeasurePageChanged = true;
			break;
		case B_LEFT:
			if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
			{
				DisplayPages[ActualPage].PageToChange++;
			}
			else
				DisplayPages[ActualPage].PageToChange = 0;
			RefreshBottomBar = true;
			break;		
		case B_OK:
			MeasurePageSelection = LINE_MEASURES;
			MeasurePageNumber = "";
			ActualPage = DisplayPages[ActualPage].PageToChange;
			PageChanged = true;
			break;
		default:
			break;
	}
	if(MeasurePageChanged)
	{
		RefreshBottomBar = true;
		MeasurePageChanged = false;
		ClearScreen(false);
	}
}

static void RefreshReleChangeStatus(uint8_t ReleIndex, bool ChangeStatusSel, bool *RefreshReleChange)
{
	String ReleName = "Presa " + String(ReleIndex + 1), Status = "SPENTA";
	if(*RefreshReleChange)
	{
		Display.fillRoundRect( 0,  64,  Display.width(),  140,  2,  BG_COLOR);
		*RefreshReleChange = false;
	}
	ChangeStatusSel = !ChangeStatusSel;
	if(ChangeStatusSel)
		Display.drawRoundRect( 0,  64,  Display.width(),  140,  2,  TFT_WHITE);
	else
		Display.drawRoundRect( 0,  64,  Display.width(),  140,  2,  BG_COLOR);
	Display.setFreeFont(FMB24);
	Display.drawString(ReleName, CENTER_POS(ReleName), 68);
	if(Rele.getReleStatus(ReleIndex) == STATUS_ON)
	{
		Display.setTextColor(BG_COLOR, TFT_GREEN);
		Status = "ACCESA";
	}
	else
		Display.setTextColor(TFT_WHITE, TFT_RED);
	Display.drawString(Status, CENTER_POS(ReleName), 104);
	Display.setTextColor(TFT_WHITE);
}


static void DrawRelePage()
{
	RefreshReleChangeStatus(ReleIndex, SelPageSelected, &RefreshReleChange);
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(RelePageItemSel == CHANGE_RELE_STATUS_ITEM)
			{
				if(ReleIndex > RELE_1)
					ReleIndex--;
				else
					ReleIndex = N_RELE - 1;
				RefreshReleChange = true;
			}
			else
			{
				if(DisplayPages[ActualPage].PageToChange > 0)
				{
					DisplayPages[ActualPage].PageToChange--;
				}
				else
					DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
				RefreshBottomBar = true;
			}
			break;
		case B_DOWN:
			if(RelePageItemSel == CHANGE_RELE_STATUS_ITEM)
			{
				if(ReleIndex < N_RELE - 1)
					ReleIndex++;
				else
					ReleIndex = RELE_1;
				RefreshReleChange = true;
			}
			else
			{
				if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
				{
					DisplayPages[ActualPage].PageToChange++;
				}
				else
					DisplayPages[ActualPage].PageToChange = 0;
				RefreshBottomBar = true;
			}
			break;
		case B_LEFT:
			if(RelePageItemSel < MAX_RELE_PAGE_ITEMS - 1)
				RelePageItemSel++;
			else
				RelePageItemSel = 0;
			if(RelePageItemSel == CHANGE_RELE_STATUS_ITEM)
				SelPageSelected = false;
			else
				SelPageSelected = true;
			break;				
		case B_OK:
			if(RelePageItemSel == CHANGE_RELE_STATUS_ITEM)
			{
				ToggleRele(ReleIndex);
				RefreshReleChange = true;
			}
			else
			{
				ReleIndex = RELE_1;
				RelePageItemSel = CHANGE_RELE_STATUS_ITEM;
				ActualPage = DisplayPages[ActualPage].PageToChange;
				PageChanged = true;
			}
			break;
		default:
			break;
	}

}

static void DrawSetupPage()
{
	SelPageSelected = true;
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(DisplayPages[ActualPage].PageToChange > 0)
			{
				DisplayPages[ActualPage].PageToChange--;
			}
			else
				DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
			RefreshBottomBar = true;
			break;
		case B_DOWN:
			if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
			{
				DisplayPages[ActualPage].PageToChange++;
			}
			else
				DisplayPages[ActualPage].PageToChange = 0;
			RefreshBottomBar = true;
			break;
		case B_OK:
			ActualPage = DisplayPages[ActualPage].PageToChange;
			PageChanged = true;
			break;
		default:
			break;
	}

}

static void DrawAlarmSetupPage()
{
	SelPageSelected = true;
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(DisplayPages[ActualPage].PageToChange > 0)
			{
				DisplayPages[ActualPage].PageToChange--;
			}
			else
				DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
			RefreshBottomBar = true;
			break;
		case B_DOWN:
			if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
			{
				DisplayPages[ActualPage].PageToChange++;
			}
			else
				DisplayPages[ActualPage].PageToChange = 0;
			RefreshBottomBar = true;
			break;
		case B_OK:
			ActualPage = DisplayPages[ActualPage].PageToChange;
			PageChanged = true;
			break;
		default:
			break;
	}

}

static void DrawAlarmStatusPage()
{
	SelPageSelected = true;
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(DisplayPages[ActualPage].PageToChange > 0)
			{
				DisplayPages[ActualPage].PageToChange--;
			}
			else
				DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
			RefreshBottomBar = true;
			break;
		case B_DOWN:
			if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
			{
				DisplayPages[ActualPage].PageToChange++;
			}
			else
				DisplayPages[ActualPage].PageToChange = 0;
			RefreshBottomBar = true;
			break;
		case B_OK:
			ActualPage = DisplayPages[ActualPage].PageToChange;
			PageChanged = true;
			break;
		default:
			break;
	}

}


static void RefreshResetList(uint8_t ResetItem, bool ResetSelected, bool *RefreshResetItem)
{
	String ResetItemName = String(Reset[ResetItem].ResetTitle), ResetNumber = "";
	ResetSelected = !ResetSelected;
	if(*RefreshResetItem)
	{
		ClearScreen(false);
		*RefreshResetItem = false;
	}
	if(ResetSelected)
		Display.drawRoundRect( 0,  102,  Display.width(),  122,  2,  TFT_WHITE);
	else
		Display.drawRoundRect( 0,  102,  Display.width(),  122,  2,  BG_COLOR);
	Display.setFreeFont(FMB24);
	Display.drawString(ResetItemName, CENTER_POS(ResetItemName), 104);
	Display.setFreeFont(FM9);
	ResetNumber = String(ResetItem + 1) + "/" + String(MAX_RESET_ITEMS);
	Display.drawString(ResetNumber, CENTER_POS(ResetNumber), 210);
}


static void DrawResetPage()
{
	SelPageSelected = true;
	RefreshResetList(ResetItem, SelPageSelected, &RefreshResetItem);
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(ResetOrPage == RESET_SELECTION)
			{
				if(ResetItem > RESET_ENERGIES)
					ResetItem--;
				else
					ResetItem = MAX_RESET_ITEMS - 1;
				RefreshResetItem = true;
			}
			else
			{
				if(DisplayPages[ActualPage].PageToChange > 0)
				{
					DisplayPages[ActualPage].PageToChange--;
				}
				else
					DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
				RefreshBottomBar = true;
			}
			break;
		case B_DOWN:
			if(ResetOrPage == RESET_SELECTION)
			{
				if(ResetItem < MAX_RESET_ITEMS - 1)
					ResetItem++;
				else
					ResetItem = RESET_ENERGIES;
				RefreshResetItem = true;
			}
			else
			{
				if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
				{
					DisplayPages[ActualPage].PageToChange++;
				}
				else
					DisplayPages[ActualPage].PageToChange = 0;
				RefreshBottomBar = true;
			}
			break;
		case B_LEFT:
			if(ResetOrPage < MAX_RESET_PAGE_ITEMS - 1)
				ResetOrPage++;
			else
				ResetOrPage = 0;
			if(ResetOrPage == RESET_SELECTION)
				SelPageSelected = false;
			else
				SelPageSelected = true;
			break;				
		case B_OK:
			if(ResetOrPage == RESET_SELECTION)
			{
				Reset[ResetItem].ResetFunc();
				DrawPopUp("Reset riuscito", 2000);
				RefreshResetItem = true;
			}
			else
			{
				ResetItem = RESET_ENERGIES;
				ResetOrPage = RESET_SELECTION;
				ActualPage = DisplayPages[ActualPage].PageToChange;
				PageChanged = true;
			}
			break;
		default:
			break;
	}

}

void TaskDisplay()
{
	if(PageChanged)
	{
		// Display.fillScreen(BG_COLOR); 
		ClearScreen(true);
		PageChanged = false;
		RefreshBottomBar = true;
	}		
	DrawTopInfoBar();
	switch(ActualPage)
	{
		case MAIN_PAGE:
			DrawMainScreen();
			break;
		case MEASURE_PAGE:
			DrawMeasurePage();
			break;
		case RELE_PAGE:
			DrawRelePage();
			break;
		case SETUP_PAGE:
			DrawSetupPage();
			break;
		case ALARM_SETUP_PAGE:
			DrawAlarmSetupPage();
			break;
		case ALARM_STATUS_PAGE:
			DrawAlarmStatusPage();
			break;
		case RESET_PAGE:
			DrawResetPage();
			break;
		default:
			break;
	}
	if(RefreshBottomBar)
	{
		DrawPageChange(DisplayPages[ActualPage].PageToChange, SelPageSelected);
		RefreshBottomBar = false;
	}	
	delay(100);

}