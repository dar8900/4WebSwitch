#include "4WebSwitch.h"
#include "Display.h"
#include "Measure.h"
#include "KeyBoard.h"
#include "Rele.h"
#include "Web.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "Free_Fonts.h"
#include "Icons.h"
#include "Alarms.h"

// #define TEST_DISPLAY	
#define BG_COLOR			TFT_BLACK

#define TOP_POS				 0
#define BOTTOM_POS(Str)		(Display.height() - Display.fontHeight())
#define LEFT_POS			 0
#define CENTER_POS(Str)		((Display.width() - Display.textWidth(String(Str)))/2)
#define RIGHT_POS(Str)		(Display.width() - Display.textWidth(String(Str)))

#define MEASURE_IN_PAGE 	3

TFT_eSPI Display = TFT_eSPI();


Chrono RefreshTopbarTimer, RefreshMeasurePageTimer;
Chrono ExitLoopTimeout(Chrono::SECONDS);

enum
{
	CHANGE_RELE_STATUS_ITEM = 0,
	CHANGE_PAGE_ITEM,
	MAX_RELE_PAGE_ITEMS
};

enum
{
	ALARM_SELECTION = 0,
	PAGE_ALARM_SEL,
	MAX_ALARM_PAGE_ITEM
};


enum
{
	RESET_SELECTION = 0,
	PAGE_RESET_SEL,
	MAX_RESET_PAGE_ITEMS
};

enum
{
	CHANGE_DEMO_STATUS_ITEM = 0,
	CHANGE_PAGE_DEMO_ITEM,
	MAX_DEMO_PAGE_ITEMS	
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
	{MAIN_PAGE   		, "Home"          },
	{MEASURE_PAGE       , "Misure"        },
	{RELE_PAGE          , "Rele"          },
	{SETUP_PAGE         , "Setup"         },
	{ALARM_SETUP_PAGE   , "Setup allarmi" },
	{ALARM_STATUS_PAGE  , "Allarmi"       },
	{RESET_PAGE         , "Reset"         },
	{DEMO_MODE          , "Demo"          },
};

const MEASURE_PAGES MeasuresPage[MAX_MEASURE_PAGES] = 
{
	{&Measures.CurrentRMS, 			     &Measures.VoltageRMS, 			       &Measures.PowerFactor	   	       , true, true, false},
	{&Measures.ActivePower, 		     &Measures.ReactivePower,			   &Measures.ApparentPower			   , true, true, true},
	{&Measures.ActiveEnergy, 	         &Measures.ReactiveEnergy, 			   &Measures.ApparentEnergy		       , true, true, true},
	{&Measures.PartialActiveEnergy,      &Measures.PartialReactiveEnergy, 	   &Measures.PartialApparentEnergy	   , true, true, true},
	{&Measures.MaxMinAvg.MaxCurrent,     &Measures.MaxMinAvg.MaxVoltage, 	   &Measures.MaxMinAvg.MaxPowerFactor  , true, true, false},
	{&Measures.MaxMinAvg.MaxActivePower, &Measures.MaxMinAvg.MaxReactivePower, &Measures.MaxMinAvg.MaxApparentPower, true, true, true},
	{&Measures.MaxMinAvg.MinCurrent,     &Measures.MaxMinAvg.MinVoltage, 	   &Measures.MaxMinAvg.MinPowerFactor  , true, true, false},
	{&Measures.MaxMinAvg.MinActivePower, &Measures.MaxMinAvg.MinReactivePower, &Measures.MaxMinAvg.MinApparentPower, true, true, true},
	{&Measures.MaxMinAvg.CurrentAvg,     &Measures.MaxMinAvg.VoltageAvg, 	   &Measures.MaxMinAvg.PowerFactorAvg  , true, true, false},
	{&Measures.MaxMinAvg.ActivePowerAvg, &Measures.MaxMinAvg.ReactivePowerAvg, &Measures.MaxMinAvg.ApparentPowerAvg, true, true, true},
	
};


const MEASURE_PAGE_LABEL_DES MeasureUdmLabel[MAX_MEASURE_PAGES] PROGMEM = 
{
	{"I, V, PF"        , "A" , "V"   , " "  , "I"     , "V"     , " PF"   },
	{"Potenze"	       , "W" , "VAr" , "VA" , "P.att" , "P.rea" , "P.app" },
	{"Energie tot."    , "Wh", "VArh", "VAh", "E.att" , "E.rea" , "E.app" },
	{"Energie parz."   , "Wh", "VArh", "VAh", "EP att", "EPrea" , "EPapp" },
	{"Massimi I, V, PF", "A" , "V"   , " "  , "Max I" , "Max V" , "MaxPF" },
	{"Massimi potenze" , "W" , "VAr" , "VA" , "MaxPat", "MaxPre", "MaxPap"},
	{"Minimi I, V, PF" , "A" , "V"   , " "  , "Min I" , "Min V" , "MinPF" },
	{"Minimi potenze"  , "W" , "VAr" , "VA" , "MinPat", "MinPre", "MinPap"},
	{"Medie I, V, PF"  , "A" , "V"   , " "  , "Avg I" , "Avg V" , "AvgPF" },
	{"Medie potenze"   , "W" , "VAr" , "VA" , "AvgPat", "AvgPre", "AvgPap"},
	
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
	{"Reset n. allarmi"	     ,      ResetAlarms       },
	{"Restart switch"		 ,      ResetMcu          },
};


static uint8_t 	ButtonPress;
static bool 	SelPageSelected = true;
static uint8_t 	ActualPage = MAIN_PAGE;
static bool     RefreshBottomBar = true, PageChanged = false;

static uint8_t 	MeasurePageSelection = LINE_MEASURES;
static String   MeasurePageNumber = "";
static bool 	MeasurePageChanged = false;

static uint8_t 	RelePageItemSel;
static uint8_t 	ReleIndex;
static bool     RefreshReleChange = false;

static uint8_t  AlarmOrPage = ALARM_SELECTION;
static uint8_t  AlarmItem = CURRENT;
static bool     RefreshAlarmItem = false;

static uint8_t  ResetOrPage = RESET_SELECTION;
static uint8_t  ResetItem = RESET_ENERGIES;
static bool     RefreshResetItem = false;

static uint8_t 	DemoItemSel;
static uint8_t 	DemoIndex = 1;
static bool     RefreshDemoChange = false;



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
		Display.fillRect( 0,  Ypos,  Display.width(),  Display.height() - Ypos  , BG_COLOR);
	}
}

void DrawPopUp(const char *Text, uint16_t Delay)
{
	String TextStr = String(Text);
	ClearScreen(true);
	Display.setFreeFont(FMB12);
	Display.drawRoundRect(0, 14, Display.width(), Display.height() - 14, 2, TFT_WHITE);
	Display.drawString(TextStr, CENTER_POS(TextStr), (Display.height() - Display.fontHeight())/2);
	delay(Delay);
	ClearScreen(true);
}

static void DrawTopInfoBar()
{
	Display.fillRect( 0,  0,  Display.width(),  13 , BG_COLOR);
	Display.setFreeFont(FM9);
	int IconsXPos = 0;
	Display.drawString(TimeFormatted, LEFT_POS, TOP_POS);
	Display.drawString(DateFormatted, RIGHT_POS(DateFormatted), TOP_POS);
	IconsXPos = Display.textWidth(TimeFormatted);
	Display.drawXBitmap(IconsXPos + 10, 0, WifiIcons[WifiSignal], 12, 12, TFT_CYAN);
	if(AlarmActive)
		Display.drawXBitmap(IconsXPos + 24, 0, Alarms_bits, 12, 12, TFT_YELLOW);
	IconsXPos = IconsXPos + 46;
	for(int i = 0; i < N_RELE; i++)
	{
		if(Rele.getReleStatus(i) == STATUS_OFF)
			Display.fillCircle(IconsXPos + (i * (2 + 4)), 6, 2, TFT_RED);
		else
			Display.fillCircle(IconsXPos + (i * (2 + 4)), 6, 2, TFT_GREEN);
	}
	if(EnableSimulation)
		Display.drawString("DEMO", IconsXPos + 65, TOP_POS);
}

static void DrawPageChange(int8_t Page, bool Selected)
{
	
	Display.setFreeFont(FMB9);	
	int32_t XPos = CENTER_POS(DisplayPages[Page].PageName), YPos = BOTTOM_POS(DisplayPages[Page].PageName);
	Display.fillRect( 0,  Display.height() - Display.fontHeight() - 4,  Display.width(),  23 , BG_COLOR);
	Display.drawString((String)DisplayPages[Page].PageName, XPos, YPos);
	if(Selected)
		Display.drawRoundRect( XPos - 2,  YPos - 2,  Display.textWidth((String)DisplayPages[Page].PageName) + 4,  Display.fontHeight() + 2,  2,  TFT_WHITE);
	else
		Display.drawRoundRect( XPos - 2,  YPos - 2,  Display.textWidth((String)DisplayPages[Page].PageName) + 4,  Display.fontHeight() + 2,  2,  BG_COLOR);
	Display.drawString((String)DisplayPages[Page].PageName, XPos, YPos);
}

static void CheckBottomPageSelected(uint8_t PageToGo)
{
	switch(PageToGo)
	{
		case MAIN_PAGE:
			SelPageSelected = true;
			break;
		case MEASURE_PAGE:
			SelPageSelected = true;
			break;
		case RELE_PAGE:
			SelPageSelected = false;
			break;
		case SETUP_PAGE: 
			SelPageSelected = true;
			break;
		case ALARM_SETUP_PAGE: 
			SelPageSelected = true;
			break;
		case ALARM_STATUS_PAGE:
			SelPageSelected = true;
			break;
		case RESET_PAGE:
			SelPageSelected = false;
			break;
		case DEMO_MODE:
			SelPageSelected = false;
			break;
		default:
			break;
	}
}

static void DrawReleStatus()
{
	const int32_t CircleRadius = 35;
	String NomePresa = "";
	Display.setFreeFont(FM9);
	for(int i = 0; i < N_RELE; i++)
	{
		NomePresa = String(i + 1);
		// if(Rele.getReleStatus(i) == STATUS_OFF)
			// Display.fillCircle((CircleRadius + 10) + (i * ((CircleRadius * 2) + 6)), 150, CircleRadius, TFT_RED);
		// else
			// Display.fillCircle((CircleRadius + 10) + (i * ((CircleRadius * 2) + 6)), 150, CircleRadius, TFT_GREEN);
		if(Rele.getReleStatus(i) == STATUS_OFF)
			Display.drawXBitmap(10 + (i * (70 + 5)), 115, IconaPresa_bits, 70, 70, TFT_RED);
		else
			Display.drawXBitmap(10 + (i * (70 + 5)), 115, IconaPresa_bits, 70, 70, TFT_GREEN);
		Display.drawString("Presa", (CircleRadius + 10 - Display.textWidth("Presa") / 2) + (i * (Display.textWidth("Presa") + 6 + CircleRadius - 20)), 	150 - (Display.fontHeight() * 2) - 38 );
		Display.drawString(NomePresa, (CircleRadius + 6) + (i * ((CircleRadius * 2) + 4)), 150 - (Display.fontHeight()) - 38 );
	}	
}



static void DrawMainScreen()
{
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
			CheckBottomPageSelected(ActualPage);
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


static void RefreshMeasurePage(uint8_t MeasurePageNumber)
{
	uint8_t Range = 0;
	String MeasureToPrint = "", UdmRF = "", Title = "";
	bool Refactor = true;
	Title = String(MeasureUdmLabel[MeasurePageNumber].PageTitle);
	Display.setFreeFont(FM12);
	Display.drawString(Title, CENTER_POS(Title), 24);		
	for(int Line = 0; Line < MEASURE_IN_PAGE; Line++)
	{
		double ActualMeasure = 0.0;
		String Udm = "", Label = "";
		switch(Line)
		{
			case 0:
				ActualMeasure = *MeasuresPage[MeasurePageNumber].FirstLineMeasure;
				Udm = String(MeasureUdmLabel[MeasurePageNumber].UdmFirstLine);
				Label = String(MeasureUdmLabel[MeasurePageNumber].MeasureLabelFirstLine);
				Refactor = MeasuresPage[MeasurePageNumber].ReformatFirstLine;			
				break;
			case 1:
				ActualMeasure = *MeasuresPage[MeasurePageNumber].SecondLineMeasure;
				Udm = String(MeasureUdmLabel[MeasurePageNumber].UdmSecondLine);
				Label = String(MeasureUdmLabel[MeasurePageNumber].MeasureLabelSecondLine);
				Refactor = MeasuresPage[MeasurePageNumber].ReformatSecondLine;
				break;
			case 2:
				ActualMeasure = *MeasuresPage[MeasurePageNumber].ThirdLineMeasure;
				Udm = String(MeasureUdmLabel[MeasurePageNumber].UdmThirdLine);
				Label = String(MeasureUdmLabel[MeasurePageNumber].MeasureLabelThirdLine);
				Refactor = MeasuresPage[MeasurePageNumber].ReformatThirdLine;			
				break;
			default:
				break;
		}
		if(Refactor)
		{
			Range = SearchRange(ActualMeasure);
			ActualMeasure *= ReformatTab[Range].RefactorValue;
			UdmRF = String(ReformatTab[Range].Odg) + Udm;
		}
		else
			UdmRF = Udm;
		if(ActualMeasure < 10)
			MeasureToPrint = "  " +  String(ActualMeasure, 3);
		else if(ActualMeasure < 100)
			MeasureToPrint = " " +  String(ActualMeasure, 3);
		else
			MeasureToPrint = String(ActualMeasure, 3);
		Display.setFreeFont(FMB18);
		Display.drawString(MeasureToPrint, CENTER_POS(MeasureToPrint), 74 + (Line * (Display.fontHeight() + 10)));
		Display.setFreeFont(FM9);
		Display.drawString(UdmRF, RIGHT_POS(UdmRF), 70 + (Line * (50)));	
		Display.drawString(Label, LEFT_POS, 70 + (Line * (50)));	
	}
	if(RefreshMeasurePageTimer.hasPassed(2000, true))
		Display.fillRect( 30,  73,  Display.width() - 30,  126  , BG_COLOR);
	Display.setFreeFont(FM9);
	String MeasurePageNumberStr = String(MeasurePageNumber + 1) + "/" + String(MAX_MEASURE_PAGES);
	Display.drawString(MeasurePageNumberStr, CENTER_POS(MeasurePageNumberStr), 200);
}


static void DrawMeasurePage()
{
	RefreshMeasurePage(MeasurePageSelection);
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
			CheckBottomPageSelected(ActualPage);
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

static void RefreshReleChangeStatus(uint8_t releIndex, bool ChangeStatusSel, bool *RefreshReleChange)
{
	String ReleName = "Presa " + String(releIndex + 1), Status = "SPENTA";
	if(*RefreshReleChange)
	{
		Display.fillRoundRect( 0,  64,  Display.width(),  (Display.fontHeight() * 2) + 14,  2,  BG_COLOR);
		*RefreshReleChange = false;
	}
	ChangeStatusSel = !ChangeStatusSel;
	Display.setFreeFont(FMB18);
	Display.drawString(ReleName, CENTER_POS(ReleName), 68);
	if(Rele.getReleStatus(releIndex) == STATUS_ON)
	{
		Display.setTextColor(TFT_GREEN);
		Status = "ACCESA";
	}
	else
		Display.setTextColor(TFT_RED);
	Display.drawString(Status, CENTER_POS(Status), 68 + Display.fontHeight() + 10);
	Display.setTextColor(TFT_WHITE);
	if(ChangeStatusSel)
		Display.drawRoundRect( 0,  64,  Display.width(),  (Display.fontHeight() * 2) + 16,  2,  TFT_WHITE);
	else                                                                                
		Display.drawRoundRect( 0,  64,  Display.width(),  (Display.fontHeight() * 2) + 16,  2,  BG_COLOR);
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
			}
			RefreshBottomBar = true;
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
			}
			RefreshBottomBar = true;
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
			RefreshBottomBar = true;
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
				CheckBottomPageSelected(ActualPage);
				PageChanged = true;
			}
			break;
		default:
			break;
	}

}

static void DrawSetupPage()
{

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
			CheckBottomPageSelected(ActualPage);
			PageChanged = true;
			break;
		default:
			break;
	}

}


static void RefreshAlarmSetupList(uint8_t AlarmItem, bool AlarmSelected, bool *RefreshAlarmItem)
{
	String AlarmItemName = String(AlarmsName[AlarmItem]), AlarmNumber = "";
	AlarmSelected = !AlarmSelected;
	if(*RefreshAlarmItem)
	{
		ClearScreen(false);
		*RefreshAlarmItem = false;
	}
	Display.setFreeFont(FMB12);
	Display.drawString(AlarmItemName, CENTER_POS(AlarmItemName), 104);
	if(AlarmSelected)
		Display.drawRoundRect( CENTER_POS(AlarmItemName) - 4,  104  - 4,  Display.textWidth(AlarmItemName) + 4,  Display.fontHeight() + 4,  2,  TFT_WHITE);
	else
		Display.drawRoundRect( CENTER_POS(AlarmItemName) - 4,  104  - 4,  Display.textWidth(AlarmItemName) + 4,  Display.fontHeight() + 4,  2,  BG_COLOR);
	Display.setFreeFont(FM9);
	AlarmNumber = String(AlarmItem + 1) + "/" + String(MAX_ALARM);
	Display.drawString(AlarmNumber, CENTER_POS(AlarmNumber), 200);
}

static void ModifyAlarm(uint8_t AlarmItem)
{
	bool ExitModifyAlarm = false;
	uint8_t ItemToModify = 0;
	bool Enabled = false, Disconnect = false, Refresh = false;
	String DisabledStr = "", DisconnectStr = "";
	uint16_t ExitCnt = 0;
	while(!ExitModifyAlarm)
	{
		if(Refresh)
		{
			ClearScreen(false);
			Refresh = false;
		}
		DrawTopInfoBar();
		if(ItemToModify == 0)
		{
			if(Enabled)
				DisabledStr = "ABILITATO";
			else
				DisabledStr = "DISABILITATO";
		}
		else
		{
			if(Disconnect)
				DisconnectStr = "ABILITATA";
			else
				DisconnectStr = "DISABILITATA";
		}		
		Display.setFreeFont(FMB12);
		Display.drawString(DisabledStr, CENTER_POS(DisabledStr), 50);
		Display.drawString(DisconnectStr, CENTER_POS(DisconnectStr), 130);
		if(ItemToModify == 0)
		{
			Display.drawRoundRect( CENTER_POS(DisabledStr) - 4,  50  - 4,  Display.textWidth(DisabledStr) + 4,  Display.fontHeight() + 4,  2,  TFT_WHITE);
			Display.drawRoundRect( CENTER_POS(DisconnectStr) - 4,  130  - 4,  Display.textWidth(DisconnectStr) + 4,  Display.fontHeight() + 4,  2,  BG_COLOR);
		}
		else
		{
			Display.drawRoundRect( CENTER_POS(DisabledStr) - 4,  50  - 4,  Display.textWidth(DisabledStr) + 4,  Display.fontHeight() + 4,  2,  BG_COLOR);
			Display.drawRoundRect( CENTER_POS(DisconnectStr) - 4,  130  - 4,  Display.textWidth(DisconnectStr) + 4,  Display.fontHeight() + 4,  2,  TFT_WHITE);		
		}
		
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				if(ItemToModify == 0)
				{
					Enabled = !Enabled;
				}
				else
				{
					Disconnect = !Disconnect;
				}
				Refresh = true;
				ExitCnt = 0;
				break;
			case B_DOWN:
				if(ItemToModify == 0)
				{
					Enabled = !Enabled;
				}
				else
				{
					Disconnect = !Disconnect;
				}
				Refresh = true;
				ExitCnt = 0;
				break;
			case B_LEFT:
				if(ItemToModify < 1)
					ItemToModify++;
				else
					ItemToModify = 0;
				Refresh = true;
				ExitCnt = 0;
				break;
			case B_OK:
				if(ItemToModify == 0)
				{
					Alarms[AlarmItem].IsEnabled = Enabled;
				}
				else
				{
					Alarms[AlarmItem].EnableDisconnection = Disconnect;
				}
				ClearScreen(false);
				ExitModifyAlarm = true;
				break;
			default:
				break;
		}
		ExitCnt++;
		if(ExitCnt == 125)
			ExitModifyAlarm = true;
		delay(80);
	}
}

static void DrawAlarmSetupPage()
{
	RefreshAlarmSetupList(AlarmItem, SelPageSelected, &RefreshAlarmItem);
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(AlarmOrPage == ALARM_SELECTION)
			{
				if(AlarmItem > CURRENT)
					AlarmItem--;
				else
					AlarmItem = MAX_ALARM - 1;
				RefreshAlarmItem = true;
			}
			else
			{
				if(DisplayPages[ActualPage].PageToChange > 0)
				{
					DisplayPages[ActualPage].PageToChange--;
				}
				else
					DisplayPages[ActualPage].PageToChange = MAX_PAGES - 1;
				
			}	
			RefreshBottomBar = true;
			break;
		case B_DOWN:
			if(AlarmOrPage == ALARM_SELECTION)
			{
				if(AlarmItem < MAX_ALARM - 1)
					AlarmItem++;
				else
					AlarmItem = CURRENT;
				RefreshAlarmItem = true;
			}
			else
			{		
				if(DisplayPages[ActualPage].PageToChange < MAX_PAGES - 1)
				{
					DisplayPages[ActualPage].PageToChange++;
				}
				else
					DisplayPages[ActualPage].PageToChange = 0;
			}
			RefreshBottomBar = true;
			break;
		case B_LEFT:
			if(AlarmOrPage < MAX_RESET_PAGE_ITEMS - 1)
				AlarmOrPage++;
			else
				AlarmOrPage = 0;
			if(AlarmOrPage == ALARM_SELECTION)
				SelPageSelected = false;
			else
				SelPageSelected = true;
			RefreshBottomBar = true;
			break;
		case B_OK:
			if(AlarmOrPage == ALARM_SELECTION)
			{
				ModifyAlarm(AlarmItem);
				RefreshAlarmItem = true;
				RefreshBottomBar = true;
			}
			else
			{
				AlarmItem = 0;
				RefreshAlarmItem = true;
				AlarmOrPage = ALARM_SELECTION;
				ActualPage = DisplayPages[ActualPage].PageToChange;
				CheckBottomPageSelected(ActualPage);
				PageChanged = true;
			}
			break;
		default:
			break;
	}

}

static void DrawAlarmStatusPage()
{
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
			CheckBottomPageSelected(ActualPage);
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
	Display.setFreeFont(FMB12);
	Display.drawString(ResetItemName, CENTER_POS(ResetItemName), 104);
	if(ResetSelected)
		Display.drawRoundRect( CENTER_POS(ResetItemName) - 4,  104  - 4,  Display.textWidth(ResetItemName) + 4,  Display.fontHeight() + 4,  2,  TFT_WHITE);
	else
		Display.drawRoundRect( CENTER_POS(ResetItemName) - 4,  104  - 4,  Display.textWidth(ResetItemName) + 4,  Display.fontHeight() + 4,  2,  BG_COLOR);
	Display.setFreeFont(FM9);
	ResetNumber = String(ResetItem + 1) + "/" + String(MAX_RESET_ITEMS);
	Display.drawString(ResetNumber, CENTER_POS(ResetNumber), 200);
}


static void DrawResetPage()
{
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
			RefreshBottomBar = true;
			break;				
		case B_OK:
			if(ResetOrPage == RESET_SELECTION)
			{
				Reset[ResetItem].ResetFunc();
				DrawPopUp("Reset riuscito", 2000);
				RefreshResetItem = true;
				RefreshBottomBar = true;
			}
			else
			{
				ResetItem = RESET_ENERGIES;
				ResetOrPage = RESET_SELECTION;
				ActualPage = DisplayPages[ActualPage].PageToChange;
				CheckBottomPageSelected(ActualPage);
				PageChanged = true;
			}
			break;
		default:
			break;
	}

}


static void RefreshDemoAct(uint8_t DemoActItem, bool ChangeStatusSel, bool *RefreshDemoAct)
{
	String DemoStr = "DISABILITATO";
	if(*RefreshDemoAct)
	{
		Display.fillRoundRect( 0,  64,  Display.width(),  (Display.fontHeight() * 2) + 14,  2,  BG_COLOR);
		*RefreshDemoAct = false;
	}
	ChangeStatusSel = !ChangeStatusSel;
	Display.setFreeFont(FMB18);
	if(DemoActItem == 0)
	{
		Display.setTextColor(TFT_RED);
	}
	else
	{
		Display.setTextColor(TFT_GREEN);
		DemoStr = "ABILITATO";
	}
	Display.drawString(DemoStr, CENTER_POS(DemoStr), 70);
	Display.setTextColor(TFT_WHITE);
	if(ChangeStatusSel)
		Display.drawRoundRect( CENTER_POS(DemoStr) - 4,  70 - 4,  Display.textWidth(DemoStr) + 4,  (Display.fontHeight() + 4),  2,  TFT_WHITE);
	else                                                                           
		Display.drawRoundRect( CENTER_POS(DemoStr) - 4,  70 - 4,  Display.textWidth(DemoStr) + 4,  (Display.fontHeight() + 4),  2,  BG_COLOR);
}


static void DrawDemoActPage()
{
	RefreshDemoAct(DemoIndex, SelPageSelected, &RefreshDemoChange);
	ButtonPress = CheckButtons();
	switch(ButtonPress)
	{
		case B_UP:
			if(DemoItemSel == CHANGE_DEMO_STATUS_ITEM)
			{
				if(DemoIndex > 0)
					DemoIndex--;
				else
					DemoIndex = 1;
				RefreshDemoChange = true;
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
			if(DemoItemSel == CHANGE_RELE_STATUS_ITEM)
			{
				if(DemoIndex < 1)
					DemoIndex++;
				else
					DemoIndex = 0;
				RefreshDemoChange = true;
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
			if(DemoItemSel < MAX_DEMO_PAGE_ITEMS - 1)
				DemoItemSel++;
			else
				DemoItemSel = 0;
			if(DemoItemSel == CHANGE_DEMO_STATUS_ITEM)
				SelPageSelected = false;
			else
				SelPageSelected = true;
			RefreshBottomBar = true;
			break;				
		case B_OK:
			if(DemoItemSel == CHANGE_DEMO_STATUS_ITEM)
			{
				if(DemoIndex == 0)
				{
					EnableSimulation = false;
					DrawPopUp("DEMO OFF", 1500);
				}
				else
				{
					EnableSimulation = true;
					DrawPopUp("DEMO ON", 1500);
				}	
				RefreshBottomBar = true;
				RefreshDemoChange = true;
			}
			else
			{
				DemoIndex = 0;
				DemoItemSel = CHANGE_DEMO_STATUS_ITEM;
				ActualPage = DisplayPages[ActualPage].PageToChange;
				CheckBottomPageSelected(ActualPage);
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
		ClearScreen(true);
		PageChanged = false;
		RefreshBottomBar = true;
	}		
	if(RefreshTopbarTimer.hasPassed(500, true))
	{	
		DrawTopInfoBar();
	}
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
		case DEMO_MODE:
			DrawDemoActPage();
			break;
		default:
			break;
	}
	if(RefreshBottomBar)
	{
		DrawPageChange(DisplayPages[ActualPage].PageToChange, SelPageSelected);
		RefreshBottomBar = false;
	}	
	delay(50);

}