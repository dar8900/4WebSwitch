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
#include "EepromSwitch.h"

// #define TEST_DISPLAY
#define BG_COLOR			TFT_BLACK

#define TOP_POS				 0
#define BOTTOM_POS(Str)		(Display.height() - Display.fontHeight())
#define LEFT_POS			 0
#define CENTER_POS(Str)		((Display.width() - Display.textWidth(String(Str)))/2)
#define RIGHT_POS(Str)		(Display.width() - Display.textWidth(String(Str)))

#define MEASURE_IN_PAGE 	3

#define REFRESH_DELAY	     2500
#define LOOPS_DELAY		     25

#define RESTART 			true
#define NO_RESTART			false

TFT_eSPI Display = TFT_eSPI();

Chrono RefreshPage, RefreshMeasure, SaveIconTimer;


const unsigned char *WifiIcons[] =
{
	NoSignal_bits,
	Pessimo_bits,
	Buono_bits,
	Ottimo_bits,
};

const char * DisplayPages[MAX_PAGES] =
{
	"Home"            ,
	"Misure"          ,
	"Rele"            ,
	"Setup"           ,
	"Abilita allarmi" ,
	"Stato allarmi"   ,
	"Reset"           ,
};

const MEASURE_PAGES MeasuresPage[MAX_MEASURE_PAGES] =
{
	{&Measures.CurrentRMS, 			         &Measures.VoltageRMS, 			           &Measures.PowerFactor	   	            , true, true, false},
	{&Measures.ActivePower, 		         &Measures.ReactivePower,			       &Measures.ApparentPower			        , true, true, true },
	{&Measures.Energies.ActiveEnergy, 	     &Measures.Energies.ReactiveEnergy, 	   &Measures.Energies.ApparentEnergy		, true, true, true },
	{&Measures.Energies.PartialActiveEnergy, &Measures.Energies.PartialReactiveEnergy, &Measures.Energies.PartialApparentEnergy	, true, true, true },
	{&Measures.MaxMinAvg.MaxCurrent,         &Measures.MaxMinAvg.MaxVoltage, 	       &Measures.MaxMinAvg.MaxPowerFactor       , true, true, false},
	{&Measures.MaxMinAvg.MaxActivePower,     &Measures.MaxMinAvg.MaxReactivePower,     &Measures.MaxMinAvg.MaxApparentPower     , true, true, true },
	{&Measures.MaxMinAvg.MinCurrent,         &Measures.MaxMinAvg.MinVoltage, 	       &Measures.MaxMinAvg.MinPowerFactor       , true, true, false},
	{&Measures.MaxMinAvg.MinActivePower,     &Measures.MaxMinAvg.MinReactivePower,     &Measures.MaxMinAvg.MinApparentPower     , true, true, true },
	{&Measures.MaxMinAvg.CurrentAvg,         &Measures.MaxMinAvg.VoltageAvg, 	       &Measures.MaxMinAvg.PowerFactorAvg       , true, true, false},
	{&Measures.MaxMinAvg.ActivePowerAvg,     &Measures.MaxMinAvg.ReactivePowerAvg,     &Measures.MaxMinAvg.ApparentPowerAvg     , true, true, true },

};


const MEASURE_PAGE_LABEL_DES MeasureUdmLabel[MAX_MEASURE_PAGES] PROGMEM =
{
	{"I, V, PF"        , "A" , "V"   , " "  , "I"        , "V"        , "PF"        },
	{"Potenze"	       , "W" , "VAr" , "VA" , "P.att"    , "P.rea"    , "P.app"     },
	{"Energie tot."    , "Wh", "VArh", "VAh", "E.att"    , "E.rea"    , "E.app"     },
	{"Energie parz."   , "Wh", "VArh", "VAh", "EP Att"   , "EP Rea"   , "EP App"    },
	{"Massimi I, V, PF", "A" , "V"   , " "  , "Max I"    , "Max V"    , "Max PF"    },
	{"Massimi potenze" , "W" , "VAr" , "VA" , "Max PAtt" , "Max PRea" , "Max PApp"  },
	{"Minimi I, V, PF" , "A" , "V"   , " "  , "Min I"    , "Min V"    , "Min PF"    },
	{"Minimi potenze"  , "W" , "VAr" , "VA" , "Min PAtt" , "Min PRea" , "Min PApp"  },
	{"Medie I, V, PF"  , "A" , "V"   , " "  , "Avg I"    , "Avg V"    , "Avg PF"    },
	{"Medie potenze"   , "W" , "VAr" , "VA" , "Avg PAtt" , "Avg PRea" , "Avg PApp"  },

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

const ENUM_VALUE WifiEnum[2] = 
{
	{"SPENTO", DISABILITATO},
	{"ACCESO", ABILITATO},
};

const ENUM_VALUE DemoEnum[2] = 
{
	{"DISABILITATA", DISABILITATO},
	{"ABILITATA"   , ABILITATO},
};

const ENUM_VALUE CurrLowThrEnum[MAX_I_LOW_THR] = 
{
	{"0.05"  , I_LOW_THR_0_0_5},
	{"0.1"   , I_LOW_THR_0_1  },
	{"0.5"   , I_LOW_THR_0_5  },
	{"1"     , I_LOW_THR_1    },
	{"1.1"   , I_LOW_THR_1_1  },
	{"1.5"   , I_LOW_THR_1_5  },	
};

const ENUM_VALUE CurrHighThrEnum[MAX_I_HIGH_THR] = 
{
	{"5.0"   , I_HIGH_THR_5   },
	{"5.5"   , I_HIGH_THR_5_5 },
	{"10.0"  , I_HIGH_THR_10  },
	{"10.5"  , I_HIGH_THR_10_5},
	{"13.0"  , I_HIGH_THR_13  },
	{"16.0"  , I_HIGH_THR_16  },	
};

const ENUM_VALUE PAttHighThrEnum[MAX_P_ATT_HIGH_THR] = 
{
	{"1.0" , P_ATT_HIGH_THR_1000},
	{"1.5" , P_ATT_HIGH_THR_1500},
	{"1.8" , P_ATT_HIGH_THR_1800},
	{"2.0" , P_ATT_HIGH_THR_2000},
	{"2.5" , P_ATT_HIGH_THR_2500},
	{"2.8" , P_ATT_HIGH_THR_2800},	
	{"3.0" , P_ATT_HIGH_THR_3000},
	{"3.4" , P_ATT_HIGH_THR_3400},	
};

const ENUM_VALUE PReaHighThrEnum[MAX_P_REA_HIGH_THR] = 
{
	{"0.05"  , P_REA_HIGH_THR_50  },
	{"0.1"   , P_REA_HIGH_THR_100 },
	{"0.25"  , P_REA_HIGH_THR_250 },
	{"0.5"   , P_REA_HIGH_THR_500 },
	{"0.8"   , P_REA_HIGH_THR_800 },
	{"1.0"   , P_REA_HIGH_THR_1000},	
	
};

const ENUM_VALUE PAppHighThrEnum[MAX_A_APP_HIGH_THR] = 
{
	{"1.0" , P_APP_HIGH_THR_1000},
	{"1.5" , P_APP_HIGH_THR_1500},
	{"1.8" , P_APP_HIGH_THR_1800},
	{"2.0" , P_APP_HIGH_THR_2000},
	{"2.5" , P_APP_HIGH_THR_2500},
	{"2.8" , P_APP_HIGH_THR_2800},	
	{"3.0" , P_APP_HIGH_THR_3000},
	{"3.4" , P_APP_HIGH_THR_3400},	
};

const ENUM_VALUE PFLowThrEnum[MAX_PF_THR] = 
{
	{"-0.500"  , PF_THR_500},
	{"-0.600"  , PF_THR_600},
	{"-0.700"  , PF_THR_700},
	{"-0.800"  , PF_THR_800},
	{"-0.850"  , PF_THR_850},
	{"-0.900"  , PF_THR_900},	
	{"-0.950"  , PF_THR_950},
	{"-0.980"  , PF_THR_980},
	{"-0.990"  , PF_THR_990},
};

const ENUM_VALUE PFHighThrEnum[MAX_PF_THR] = 
{
	{"0.500"  , PF_THR_500},
	{"0.600"  , PF_THR_600},
	{"0.700"  , PF_THR_700},
	{"0.800"  , PF_THR_800},
	{"0.850"  , PF_THR_850},
	{"0.900"  , PF_THR_900},	
	{"0.950"  , PF_THR_950},
	{"0.980"  , PF_THR_980},
	{"0.990"  , PF_THR_990},
};

const SETUP_PARAMS SetupParams[MAX_SETUP_ITEMS] = 
{
	{"Stato WiFi"		          , ABILITATO			 	 , DISABILITATO         , ENUM_TYPE  , WifiEnum       ,  NULL  ,    RESTART},
	{"Delay salvataggio"          ,        60			 	 ,            1         , VALUE_TYPE , NULL           , "min"  , NO_RESTART},
	{"Periodo media misure"       ,      3600			 	 ,            5         , VALUE_TYPE , NULL           ,   "s"  , NO_RESTART},
	{"Simulazione"      		  , ABILITATO			 	 , DISABILITATO         , ENUM_TYPE  , DemoEnum       ,  NULL  ,    RESTART},
	{"Soglia sovra corrente"      , MAX_I_HIGH_THR - 1	 	 , 0                    , ENUM_TYPE  , CurrHighThrEnum,  "A"   , NO_RESTART},
	{"Soglia sotto corrente"      , MAX_I_LOW_THR - 1	 	 , 0                    , ENUM_TYPE  , CurrLowThrEnum ,  "A"   , NO_RESTART},
	{"Soglia sovra p. attiva"     , MAX_P_ATT_HIGH_THR - 1   , 0                  	, ENUM_TYPE  , PAttHighThrEnum,  "kW"  , NO_RESTART},
	{"Soglia sovra p. reattiva"   , MAX_P_REA_HIGH_THR - 1   , 0                    , ENUM_TYPE  , PReaHighThrEnum,  "kVAr", NO_RESTART},
	{"Soglia sovra p. apparente"  , MAX_A_APP_HIGH_THR - 1   , 0                   	, ENUM_TYPE  , PAppHighThrEnum,  "kVA" , NO_RESTART},
	{"Soglia sovra PF"            , MAX_PF_THR - 1		     , 0                    , ENUM_TYPE  , PFHighThrEnum  ,  NULL  , NO_RESTART},
	{"Soglia sotto PF"            , MAX_PF_THR - 1		     , 0		    	    , ENUM_TYPE  , PFLowThrEnum   ,  NULL  , NO_RESTART},	
};






const char *Reset[MAX_RESET_ITEMS] =
{
	"Reset energie",
	"Reset energie parziali",
	"Reset max e min",
	"Reset medie",
	"Reset allarmi",
	"Reset statistiche prese",
	"Restart switch",
};


static uint8_t 	ButtonPress = NO_PRESS;
static uint8_t 	ActualPage = MAIN_PAGE;


void DisplayInit()
{
	Display.init();
	Display.setRotation(3);
	Display.fillScreen(BG_COLOR);

}

void DrawWelcomePage()
{
	Display.setFreeFont(FMB12);
	Display.drawRoundRect(0, 0, Display.width(), Display.height(), 5, TFT_WHITE);
	Display.drawString("Home Microtech", CENTER_POS("Home Microtech"), 50);
	Display.drawString("4 Web Switch", CENTER_POS("4 Web Switch"), 100);	
}

void DrawWiFiConnPage(String SSID, String IP_popup)
{
	ClearScreen(true);
	Display.setFreeFont(FMB9);
	Display.drawRoundRect(0, 0, Display.width(), Display.height(), 5, TFT_WHITE);
	Display.drawString("Connesso a:", CENTER_POS("Connesso a:"), 50);
	Display.setFreeFont(FMB12);
	Display.drawString(SSID, CENTER_POS(SSID), 80);	
	Display.setFreeFont(FMB9);
	Display.drawString("IP:", CENTER_POS("IP:"), 150);
	Display.setFreeFont(FMB12);
	Display.drawString(IP_popup, CENTER_POS(IP_popup), 180);
	delay(2000);
	ClearScreen(true);
}

static void TaskManagement()
{
	if(RefreshMeasure.hasPassed(1000, true))
		TaskMeasure();
	TaskAlarm();
	TaskWeb();
	TaskEeprom();
	RefreshReleStatistics();
}


static void WichReset(uint8_t ResetItem)
{
	switch(ResetItem)
	{
		case RESET_ENERGIES:
			ResetTotalEnergy();
			ResetEepEnergies();
			break;
		case RESET_PAR_ENERGIES:
			ResetPartialEnergy();
			break;
		case RESET_MAX_MIN:
			ResetMaxMin();
			break;
		case RESET_AVG:
			ResetAvg();
			break;
		case RESET_N_ALARMS:
			ResetAlarms();
			break;
		case RESET_RELE_STAT:
			ResetReleStatistics();
			ResetEepReleStatistics();
			break;
		case RESTART_MCU:
			ResetMcu();
			break;
		default:
			break;
	}
}


void ClearScreen(bool FullScreen)
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

static void ClearTopBottomBar()
{
	Display.fillRect(0, 0, Display.width(), 14, BG_COLOR);
	Display.fillRect(0, Display.height() - 14, Display.width(), 14, BG_COLOR);
}

void DrawPopUp(const char *Text, uint16_t Delay)
{
	String TextStr = String(Text);
	ClearScreen(true);
	Display.setFreeFont(FMB12);
	Display.drawRoundRect(0, 0, Display.width(), Display.height(), 5, TFT_WHITE);
	Display.drawString(TextStr, CENTER_POS(TextStr), (Display.height() - Display.fontHeight())/2);
	delay(Delay);
	ClearScreen(true);
}

static void DrawTopInfoBar()
{
	String FWVers = "v" + String(FW_VERSION);
	Display.fillRect( 0,  0,  Display.width(),  13 , BG_COLOR);
	Display.setFreeFont(FM9);
	int IconsXPos = 0;
	Display.drawString(TimeFormatted, LEFT_POS, TOP_POS);
	Display.drawString(DateFormatted, RIGHT_POS(DateFormatted), TOP_POS);
	IconsXPos = Display.textWidth(TimeFormatted);
	Display.drawXBitmap(IconsXPos + 10, 0, WifiIcons[WifiSignal], 12, 12, TFT_CYAN);
	if(AlarmActive)
		Display.drawXBitmap(IconsXPos + 24, 0, Alarms_bits, 12, 12, TFT_YELLOW);
	if(SaveAccomplished)
	{
		Display.drawXBitmap(148, 0, SaveIcon_bits, 12, 12, TFT_YELLOW);	
		if(SaveIconTimer.hasPassed(2500, true))
			SaveAccomplished = false;
	}
	else
	{
		SaveIconTimer.restart();
	}
	IconsXPos = IconsXPos + 46;
	for(int i = 0; i < N_RELE; i++)
	{
		if(Rele.getReleStatus(i) == STATUS_OFF)
			Display.fillCircle(IconsXPos + (i * (2 + 4)), 6, 2, TFT_RED);
		else
			Display.fillCircle(IconsXPos + (i * (2 + 4)), 6, 2, TFT_GREEN);
	}
	if(EepParamsValue[SIMULATION_MODE] == ABILITATO)
		Display.drawString("DEMO", IconsXPos + 65, TOP_POS);
	else
		Display.drawString(FWVers, IconsXPos + 65, TOP_POS);
}

static void DrawPageChange(int8_t Page, bool Selected)
{

	Display.setFreeFont(FMB9);
	int32_t XPos = CENTER_POS(DisplayPages[Page]), YPos = BOTTOM_POS(DisplayPages[Page]);
	Display.fillRect( 0,  Display.height() - Display.fontHeight() - 4,  Display.width(),  23 , BG_COLOR);
	Display.drawString((String)DisplayPages[Page], XPos, YPos);
	if(Selected)
		Display.drawRoundRect( XPos - 2,  YPos - 2,  Display.textWidth((String)DisplayPages[Page]) + 4,  Display.fontHeight() + 2,  2,  TFT_WHITE);
	Display.drawString((String)DisplayPages[Page], XPos, YPos);
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
			Display.drawXBitmap(10 + (i * (70 + 5)), 115, IconaPresa_bits, 70, 70, TFT_RED);
		else
			Display.drawXBitmap(10 + (i * (70 + 5)), 115, IconaPresa_bits, 70, 70, TFT_GREEN);
		Display.drawString("Presa", (CircleRadius + 10 - Display.textWidth("Presa") / 2) + (i * (Display.textWidth("Presa") + 6 + CircleRadius - 20)), 	150 - (Display.fontHeight() * 2) - 38 );
		Display.drawString(NomePresa, (CircleRadius + 6) + (i * ((CircleRadius * 2) + 4)), 150 - (Display.fontHeight()) - 38 );
	}
}



static void DrawMainScreen()
{
	bool ExitMainScreen = false;
	ClearScreen(true);
	RefreshPage.restart();
	while(!ExitMainScreen)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(1000, true))
			ClearTopBottomBar();
		DrawTopInfoBar();
		DrawPageChange(ActualPage, true);
		Display.setFreeFont(FMB18);
		Display.drawString("4 WEB SWITCH", CENTER_POS("4 WEB SWITCH"), 25);
		DrawReleStatus();
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				break;
			case B_DOWN:
				break;
			case B_LEFT:
				if(ActualPage < MAX_PAGES - 1)
					ActualPage++;
				else
					ActualPage = MAIN_PAGE;
				break;
			case B_OK:
				ExitMainScreen = true;
				ClearScreen(true);
				RefreshPage.stop();
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}
}

static uint8_t SearchRange(double Value)
{
	int i = 0, TabRangeLen = (sizeof(ReformatTab)/ sizeof(ReformatTab[0]));
	for(i = 0; i < TabRangeLen; i++)
	{
		if(Value < ReformatTab[i].Value)
		{
			if(i > 0)
			{
				i--;
				break;
			}
			else
			{
				i = 3;
				break;
			}
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
			Range = SearchRange(fabs(ActualMeasure));
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
	Display.setFreeFont(FM9);
	String MeasurePageNumberStr = String(MeasurePageNumber + 1) + "/" + String(MAX_MEASURE_PAGES);
	Display.drawString(MeasurePageNumberStr, CENTER_POS(MeasurePageNumberStr), 200);
}


static void DrawMeasurePage()
{
	uint8_t MeasurePage = 0;
	bool ExitMeasurePage = false;
	RefreshPage.restart();
	while(!ExitMeasurePage)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(1500, true))
			ClearScreen(true);
		DrawTopInfoBar();
		DrawPageChange(ActualPage, true);
		RefreshMeasurePage(MeasurePage);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				if(MeasurePage > 0)
					MeasurePage--;
				else
					MeasurePage = MAX_MEASURE_PAGES - 1;
				ActualPage = MEASURE_PAGE;
				break;
			case B_DOWN:
				if(MeasurePage < MAX_MEASURE_PAGES - 1)
					MeasurePage++;
				else
					MeasurePage = 0;
				ActualPage = MEASURE_PAGE;
				break;
			case B_LEFT:
				if(ActualPage < MAX_PAGES - 1)
					ActualPage++;
				else
					ActualPage = MAIN_PAGE;
				break;
			case B_OK:
				RefreshPage.stop();
				ClearScreen(true);
				ExitMeasurePage = true;
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}
}

static void RefreshReleChangeStatus(uint8_t releIndex, bool ReleStatusSelected)
{
	String ReleName = "Presa " + String(releIndex + 1), Status = "SPENTA";
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
	if(ReleStatusSelected)
		Display.drawRoundRect( 0,  64,  Display.width(),  (Display.fontHeight() * 2) + 16,  2,  TFT_WHITE);

}


static void DrawRelePage()
{
	uint8_t ReleIndex = 0;
	bool ExitRelePage = false, ReleStatusSelected = false, Refresh = true;
	RefreshPage.restart();
	while(!ExitRelePage)
	{
		TaskManagement();
		if(Refresh)
		{
			Refresh = false;
			ClearScreen(true);
		}
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearTopBottomBar();
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !ReleStatusSelected);
		RefreshReleChangeStatus(ReleIndex, ReleStatusSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				if(ReleStatusSelected)
				{
					if(ReleIndex > 0)
						ReleIndex--;
					else
						ReleIndex = N_RELE - 1;
				}
				else
				{
					if(ActualPage > 0)
						ActualPage--;
					else
						ActualPage = MAX_PAGES - 1;
				}				
				Refresh = true;				
				break;
			case B_DOWN:
				if(ReleStatusSelected)
				{
					if(ReleIndex < N_RELE - 1)
						ReleIndex++;
					else
						ReleIndex = RELE_1;
				}
				else
				{
					if(ActualPage < MAX_PAGES - 1)
						ActualPage++;
					else
						ActualPage = MAIN_PAGE;
				}
				Refresh = true;				
				break;
			case B_LEFT:
				ReleStatusSelected = !ReleStatusSelected;
				Refresh = true;
				ActualPage = RELE_PAGE;
				break;
			case B_OK:
				if(ReleStatusSelected)
				{
					ToggleRele(ReleIndex);
					Refresh = true;
				}
				else
				{
					RefreshPage.stop();
					ClearScreen(true);
					ExitRelePage = true;
				}
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}
}

static void RefreshSetupPage(uint8_t SetupItem, bool SetupSelected, bool ChangeParams, uint16_t ParamValue)
{
	String ParamValueStr = "";
	Display.setFreeFont(FMB9);
	Display.drawString(SetupParams[SetupItem].ParamTitle, CENTER_POS(SetupParams[SetupItem].ParamTitle), 50);
	Display.setFreeFont(FMB12);
	if(SetupParams[SetupItem].Type == ENUM_TYPE)
	{
		Display.setTextColor(TFT_GREENYELLOW);
		ParamValueStr = String(SetupParams[SetupItem].EnumList[ParamValue].EnumTitle);
		Display.setTextColor(TFT_GREENYELLOW);
		Display.drawString(ParamValueStr, CENTER_POS(ParamValueStr), 100);
		Display.setTextColor(TFT_WHITE);		
	}
	else
	{
		Display.setTextColor(TFT_GREENYELLOW);
		ParamValueStr = String(ParamValue);
		Display.setTextColor(TFT_GREENYELLOW);
		Display.drawString(ParamValueStr, CENTER_POS(ParamValueStr), 100);
		Display.setTextColor(TFT_WHITE);				
	}
	if(SetupSelected)
	{
		Display.drawRoundRect(CENTER_POS(ParamValueStr) - 4, 96, Display.textWidth(ParamValueStr) + 6, Display.fontHeight() + 2 , 2, TFT_WHITE);
		if(ChangeParams)	
		{
			Display.drawRoundRect(CENTER_POS(ParamValueStr) - 10, 90, Display.textWidth(ParamValueStr) + 18, Display.fontHeight() + 15, 2, TFT_RED);			
		}
	}
	if(SetupParams[SetupItem].Udm != NULL)
	{
		Display.setFreeFont(FMB9);
		Display.drawString(SetupParams[SetupItem].Udm, CENTER_POS(SetupParams[SetupItem].Udm), 145);		
	}
	Display.setFreeFont(FM9);
	if(SetupParams[SetupItem].RestartMcu)
	{
		Display.drawString("Cambiare il parametro", CENTER_POS("Cambiare il parametro"), 160);
		Display.drawString("causerà un riavvio", CENTER_POS("causerà un riavvio"), 175);
	}
	String SetupPageN = String(SetupItem + 1) + "/" + String(MAX_SETUP_ITEMS);
	Display.drawString(SetupPageN, CENTER_POS(SetupPageN), 200);
}

static void DrawSetupPage()
{
	bool ExitSetupPage = false, SetupSelected = false, ChangeParams = false, Refresh = true;
	uint8_t SetupItem = 0;
	uint16_t ParamValue = 0;
	ParamValue = EepParamsValue[SetupItem];
	while(!ExitSetupPage)
	{
		TaskManagement();
		if(Refresh)
		{
			Refresh = false;
			ClearScreen(true);
		}
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearTopBottomBar();
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !SetupSelected);
		RefreshSetupPage(SetupItem, SetupSelected, ChangeParams, ParamValue);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				if(ChangeParams)
				{
					if(ParamValue > SetupParams[SetupItem].MinVal)
						ParamValue--;
					else
						ParamValue = SetupParams[SetupItem].MaxVal;
				}
				else
					SetupSelected = !SetupSelected;
				Refresh = true;
				ActualPage = SETUP_PAGE;
				break;
			case B_DOWN:
				if(ChangeParams)
				{
					if(ParamValue < SetupParams[SetupItem].MaxVal)
						ParamValue++;
					else
						ParamValue = SetupParams[SetupItem].MinVal;					
				}
				else
					SetupSelected = !SetupSelected;
				Refresh = true;
				ActualPage = SETUP_PAGE;
				break;
			case B_LEFT:
				if(SetupSelected)
				{
					if(SetupItem < MAX_SETUP_ITEMS - 1)
						SetupItem++;
					else
						SetupItem = 0;
					ParamValue = EepParamsValue[SetupItem];
				}
				else
				{
					if(ActualPage < MAX_PAGES - 1)
						ActualPage++;
					else
						ActualPage = 0;
				}
				Refresh = true;
				break;
			case B_OK:
				if(SetupSelected)
				{
					if(ChangeParams)
					{
						if(SetupItem >= CURRENT_HIGH_THR && SetupItem <= PF_LOW_THR)
						{
							AssignAlarmsThr(SetupParams[SetupItem].EnumList[ParamValue].EnumValue, SetupItem);	
						}
						EepParamsValue[SetupItem] = ParamValue;
						SaveParameters();
						DrawPopUp("Salvato", 1000);
						if(SetupParams[SetupItem].RestartMcu)
						{
							DrawPopUp("Riavvio...", 2000);
							ResetMcu();
						}
					}
					ChangeParams = !ChangeParams;
					Refresh = true;
				}
				else
				{
					ClearScreen(true);
					ExitSetupPage = true;					
				}
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}
}


static void RefreshAlarmSetupList(uint8_t AlarmItem, bool AlarmSelected)
{
	String AlarmItemName = String(AlarmsName[AlarmItem]), AlarmNumber = "";
	Display.setFreeFont(FMB12);
	Display.drawString(AlarmItemName, CENTER_POS(AlarmItemName), 104);
	if(AlarmSelected)
		Display.drawRoundRect( CENTER_POS(AlarmItemName) - 4,  104  - 4,  Display.textWidth(AlarmItemName) + 4,  Display.fontHeight() + 4,  2,  TFT_WHITE);
	Display.setFreeFont(FM9);
	AlarmNumber = String(AlarmItem + 1) + "/" + String(MAX_ALARM);
	Display.drawString(AlarmNumber, CENTER_POS(AlarmNumber), 200);
}

static void ModifyAlarm(uint8_t AlarmItem)
{
	bool ExitModifyAlarm = false;
	uint8_t ItemToModify = 0;
	bool Enabled = false, Disconnect = false, Refresh = true;
	String DisabledStr = "", DisconnectStr = "";
	uint16_t ExitCnt = 0;
	ButtonPress = NO_PRESS;
	Enabled = Alarms[AlarmItem].IsEnabled;
	Disconnect = Alarms[AlarmItem].EnableDisconnection;	
	while(!ExitModifyAlarm)
	{
		if(Refresh)
		{
			Refresh = false;
			ClearScreen(true);
		}
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearTopBottomBar();
		DrawTopInfoBar();

		if(Enabled)
			DisabledStr = "ABILITATO";
		else
			DisabledStr = "DISABILITATO";
		if(Disconnect)
			DisconnectStr = "ABILITATA";
		else
			DisconnectStr = "DISABILITATA";
		Display.setFreeFont(FMB9);
		Display.drawString(AlarmsName[AlarmItem],  CENTER_POS(AlarmsName[AlarmItem]), 50);
		Display.drawString("Abilita allarme",  CENTER_POS("Abilita allarme"), 90);
		Display.drawString("Abilita disconn.", CENTER_POS("Abilita disconn."), 160);
		Display.setFreeFont(FMB12);
		if(Enabled)
			Display.setTextColor(TFT_RED);
		else
			Display.setTextColor(TFT_GREEN);			
		Display.drawString(DisabledStr, CENTER_POS(DisabledStr), 120);
		if(Disconnect)
			Display.setTextColor(TFT_RED);
		else
			Display.setTextColor(TFT_GREEN);		
		Display.drawString(DisconnectStr, CENTER_POS(DisconnectStr), 190);
		Display.setTextColor(TFT_WHITE);
		if(ItemToModify == 0)
		{
			Display.drawRoundRect( CENTER_POS(DisabledStr) - 4,  120  - 4,  Display.textWidth(DisabledStr) + 4,  Display.fontHeight() + 4,  2,  TFT_WHITE);
			Display.drawRoundRect( CENTER_POS(DisconnectStr) - 4,  190  - 4,  Display.textWidth(DisconnectStr) + 4,  Display.fontHeight() + 4,  2,  BG_COLOR);
		}
		else
		{
			Display.drawRoundRect( CENTER_POS(DisabledStr) - 4,  120  - 4,  Display.textWidth(DisabledStr) + 4,  Display.fontHeight() + 4,  2,  BG_COLOR);
			Display.drawRoundRect( CENTER_POS(DisconnectStr) - 4,  190  - 4,  Display.textWidth(DisconnectStr) + 4,  Display.fontHeight() + 4,  2,  TFT_WHITE);
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
				ClearScreen(true);
				ExitModifyAlarm = true;
				break;
			default:
				break;
		}
		ExitCnt++;
		if(ExitCnt == 125)
			ExitModifyAlarm = true;
		delay(LOOPS_DELAY);
	}
}

static void DrawAlarmSetupPage()
{
	bool ExitAlarmSetupPage = false, AlarmSelected = false, Refresh = true;
	uint8_t AlarmItem = 0;
	RefreshPage.restart();
	while(!ExitAlarmSetupPage)
	{
		TaskManagement();
		if(Refresh)
		{
			Refresh = false;
			ClearScreen(true);
		}
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearTopBottomBar();
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !AlarmSelected);
		RefreshAlarmSetupList(AlarmItem, AlarmSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				if(AlarmSelected)
				{
					if(AlarmItem > 0)
						AlarmItem--;
					else
						AlarmItem = MAX_ALARM - 1;
				}
				else
				{
					if(ActualPage > 0)
						ActualPage--;
					else
						ActualPage = MAX_PAGES - 1;
				}		
				Refresh = true;		
				break;
			case B_DOWN:
				if(AlarmSelected)
				{
					if(AlarmItem < MAX_ALARM - 1)
						AlarmItem++;
					else
						AlarmItem = 0;
				}
				else
				{
					if(ActualPage < MAX_PAGES - 1)
						ActualPage++;
					else
						ActualPage = 0;
				}		
				Refresh = true;
				break;
			case B_LEFT:
				AlarmSelected = !AlarmSelected;
				Refresh = true;
				ActualPage = ALARM_SETUP_PAGE;
				break;
			case B_OK:
				if(AlarmSelected)
				{
					delay(20);
					ModifyAlarm(AlarmItem);
					Refresh = true;
				}
				else
				{
					RefreshPage.stop();
					ClearScreen(true);
					ExitAlarmSetupPage = true;
				}
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}

}

static void RefreshAlarmStatus(uint8_t AlarmItem, bool AlarmStatusSelected)
{
	Display.setFreeFont(FMB9);
	if(AlarmStatusSelected)
			Display.drawRoundRect(30,  16, 260,  180,  2,  TFT_WHITE);
	Display.drawString(AlarmsName[AlarmItem], CENTER_POS(AlarmsName[AlarmItem]), 20);
	if(Alarms[AlarmItem].IsActive)
	{
		Display.setTextColor(TFT_YELLOW);
		Display.drawString("ATTIVO", CENTER_POS("ATTIVO"), Display.fontHeight() + 30);
		if(Alarms[AlarmItem].WichThr == UNDER_THR)
		{
			Display.drawString(UnderThrAlarmMessage[AlarmItem], CENTER_POS(UnderThrAlarmMessage[AlarmItem]), Display.fontHeight() + 50);
		}
		else
		{
			Display.drawString(OverThrAlarmMessage[AlarmItem], CENTER_POS(OverThrAlarmMessage[AlarmItem]), Display.fontHeight() + 50);
		}
		String AlarmTime = "", AlarmDate = "";
		if(Alarms[AlarmItem].AlarmTime != 0)
		{
			AlarmTime = FormatTime(Alarms[AlarmItem].AlarmTime, true);
			AlarmDate = FormatDate(Alarms[AlarmItem].AlarmTime);
		}
		else
		{
			AlarmTime = "N.A.";
			AlarmDate = "N.A.";
		}
		String OccurenceStr = "N. allarmi: " + String(Alarms[AlarmItem].Occurences);
		Display.drawString(AlarmTime, CENTER_POS(AlarmTime), Display.fontHeight() + 70);
		Display.drawString(AlarmDate, CENTER_POS(AlarmDate), Display.fontHeight() + 90);
		Display.drawString(OccurenceStr, CENTER_POS(OccurenceStr), Display.fontHeight() + 110);
	}
	else
	{
		String AlarmsThrStr = "";
		Display.setTextColor(TFT_GREEN);
		Display.drawString("NON ATTIVO", CENTER_POS("NON ATTIVO"), Display.fontHeight() + 30);
		Display.setTextColor(TFT_WHITE);
		Display.setFreeFont(FM9);
		Display.drawString("Soglia sup.", CENTER_POS("Soglia sup."), Display.fontHeight() + 80);
		Display.setFreeFont(FMB9);
		AlarmsThrStr = String(Alarms[AlarmItem].HighThr, 3);
		Display.drawString(AlarmsThrStr, CENTER_POS(AlarmsThrStr), Display.fontHeight() + 95);
		Display.setFreeFont(FM9);
		Display.drawString("Soglia inf.", CENTER_POS("Soglia inf."), Display.fontHeight() + 140);
		Display.setFreeFont(FMB9);
		AlarmsThrStr = String(Alarms[AlarmItem].LowThr, 3);
		Display.drawString(AlarmsThrStr, CENTER_POS(AlarmsThrStr), Display.fontHeight() + 155);
	}
	Display.setTextColor(TFT_WHITE);
	Display.setFreeFont(FM9);
	String AlarmNumberStr = String(AlarmItem + 1) + "/" + String(MAX_ALARM);
	Display.drawString(AlarmNumberStr, CENTER_POS(AlarmNumberStr), 200);
}


static void DrawAlarmStatusPage()
{
	bool ExitAlarmStatusPage = false, AlarmSelected = false, Refresh = true;
	uint8_t AlarmItem = CURRENT;
	RefreshPage.restart();
	while(!ExitAlarmStatusPage)
	{
		TaskManagement();
		if(Refresh)
		{
			Refresh = false;
			ClearScreen(true);
		}
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearTopBottomBar();
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !AlarmSelected);
		RefreshAlarmStatus(AlarmItem, AlarmSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				if(AlarmSelected)
				{
					if(AlarmItem > 0)
						AlarmItem--;
					else
						AlarmItem = MAX_ALARM - 1;
				}
				else
				{
					if(ActualPage > 0)
						ActualPage--;
					else
						ActualPage = MAX_PAGES - 1;
				}
				Refresh = true;		
				break;
			case B_DOWN:
				if(AlarmSelected)
				{
					if(AlarmItem < MAX_ALARM - 1)
						AlarmItem++;
					else
						AlarmItem = CURRENT;
				}
				else
				{
					if(ActualPage < MAX_PAGES - 1)
						ActualPage++;
					else
						ActualPage = 0;
				}
				Refresh = true;
				break;
			case B_LEFT:
				AlarmSelected = !AlarmSelected;
				Refresh = true;
				ActualPage = ALARM_STATUS_PAGE;				
				break;
			case B_OK:
				RefreshPage.stop();
				ClearScreen(true);
				ExitAlarmStatusPage = true;
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}

}

static void RefreshReleStatisticsPage(uint8_t ReleIndex, bool ReleSelected)
{
	String NomePresa = "Presa " + String(ReleIndex + 1);
	String TurnOnTimes = "Accensioni: " + String(ReleStatistics[ReleIndex].NSwitches);
	String PowerOnTimeStr = "In funzione da: " + FormatTime(ReleStatistics[ReleIndex].PowerOnTime, true);
	Display.setFreeFont(FMB12);
	Display.setTextColor(TFT_YELLOW);
	Display.drawString(NomePresa, CENTER_POS(NomePresa), 30);
	Display.setFreeFont(FMB9);
	Display.setTextColor(TFT_WHITE);
	Display.drawString(TurnOnTimes, CENTER_POS(TurnOnTimes), 60);
	Display.drawString(PowerOnTimeStr, CENTER_POS(PowerOnTimeStr), 90);
	if(ReleSelected)
		Display.drawRoundRect(0, 25, Display.width(), 100, 4, TFT_WHITE);
}

static void DrawReleStatistics()
{
	bool ExitStatistics = false, ReleSelected = false, Refresh = true;
	uint8_t ReleIndex = 0;
	RefreshPage.restart();
	while(!ExitStatistics)
	{
		TaskManagement();
		if(Refresh)
		{
			Refresh = false;
			ClearScreen(true);
		}
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearTopBottomBar();
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !ReleSelected);
		RefreshReleStatisticsPage(ReleIndex, ReleSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				if(ReleSelected)
				{
					if(ReleIndex > 0)
						ReleIndex--;
					else
						ReleIndex = N_RELE - 1;
				}
				else
				{
					if(ActualPage > 0)
						ActualPage--;
					else
						ActualPage = MAX_PAGES - 1;
				}
				Refresh = true;		
				break;
			case B_DOWN:
				if(ReleSelected)
				{
					if(ReleIndex < N_RELE - 1)
						ReleIndex++;
					else
						ReleIndex = 0;
				}
				else
				{
					if(ActualPage < MAX_PAGES - 1)
						ActualPage++;
					else
						ActualPage = 0;
				}
				Refresh = true;
				break;
			case B_LEFT:
				ReleSelected = !ReleSelected;
				Refresh = true;
				ActualPage = RELE_STAT;				
				break;
			case B_OK:
				if(ReleSelected)
				{
					Refresh = true;
				}
				else
				{
					RefreshPage.stop();
					ClearScreen(true);
					ExitStatistics = true;
				}
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}	
}


static void RefreshResetList(uint8_t ResetItem, bool ResetSelected)
{
	String ResetItemName = String(Reset[ResetItem]), ResetNumber = "";
	Display.setFreeFont(FMB12);
	Display.drawString(ResetItemName, CENTER_POS(ResetItemName), 104);
	if(ResetSelected)
		Display.drawRoundRect( CENTER_POS(ResetItemName) - 4,  104  - 4,  Display.textWidth(ResetItemName) + 4,  Display.fontHeight() + 4,  2,  TFT_WHITE);
	Display.setFreeFont(FM9);
	ResetNumber = String(ResetItem + 1) + "/" + String(MAX_RESET_ITEMS);
	Display.drawString(ResetNumber, CENTER_POS(ResetNumber), 200);
}


static void DrawResetPage()
{
	bool ExitResetPage = false, ResetSelected = false, Refresh = true;
	uint8_t ResetItem = 0;
	RefreshPage.restart();
	while(!ExitResetPage)
	{
		TaskManagement();
		if(Refresh)
		{
			Refresh = false;
			ClearScreen(true);
		}
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearTopBottomBar();
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !ResetSelected);
		RefreshResetList(ResetItem, ResetSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
				if(ResetSelected)
				{
					if(ResetItem > 0)
						ResetItem--;
					else
						ResetItem = MAX_RESET_ITEMS - 1;
				}
				else
				{
					if(ActualPage > 0)
						ActualPage--;
					else
						ActualPage = MAX_PAGES - 1;
				}
				Refresh = true;
				break;
			case B_DOWN:
				if(ResetSelected)
				{
					if(ResetItem < MAX_RESET_ITEMS - 1)
						ResetItem++;
					else
						ResetItem = 0;
				}
				else
				{
					if(ActualPage < MAX_PAGES - 1)
						ActualPage++;
					else
						ActualPage = 0;
				}
				Refresh = true;			
				break;
			case B_LEFT:
				ResetSelected = !ResetSelected;
				Refresh = true;
				ActualPage = RESET_PAGE;				
				break;
			case B_OK:
				if(ResetSelected)
				{
					WichReset(ResetItem);
					DrawPopUp("Reset eseguito", 1500);
					if(ResetItem == RESET_ENERGIES || ResetItem == RESET_RELE_STAT)
					{
						DrawPopUp("Riavvio...", 2000);
						ResetMcu();
					}
					Refresh = true;
				}
				else
				{
					RefreshPage.stop();
					ClearScreen(true);
					ExitResetPage = true;
				}
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}
}


// static void RefreshDemoAct(bool DemoActive, bool ChangeDemoStatus)
// {
	// String DemoStr = "DISABILITATO";
	// Display.setFreeFont(FMB18);
	// if(DemoActive)
	// {
		// Display.setTextColor(TFT_RED);
		// DemoStr = "ABILITATO";
	// }
	// else
	// {
		// Display.setTextColor(TFT_GREEN);
		
	// }
	// Display.drawString(DemoStr, CENTER_POS(DemoStr), 70);
	// Display.setTextColor(TFT_WHITE);
	// if(ChangeDemoStatus)
		// Display.drawRoundRect( CENTER_POS(DemoStr) - 4,  70 - 4,  Display.textWidth(DemoStr) + 4,  (Display.fontHeight() + 4),  2,  TFT_WHITE);
// }


// static void DrawDemoActPage()
// {
	// bool ExitDemoPage = false, ChangeDemoStatus = false, Refresh = true;
	// bool DemoActive = EnableSimulation;
	// while(!ExitDemoPage)
	// {
		// TaskManagement();
		// if(Refresh)
		// {
			// Refresh = false;
			// ClearScreen(true);
		// }
		// if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			// ClearTopBottomBar();
		// DrawTopInfoBar();
		// DrawPageChange(ActualPage, !ChangeDemoStatus);
		// RefreshDemoAct(DemoActive, ChangeDemoStatus);
		// ButtonPress = CheckButtons();
		// switch(ButtonPress)
		// {
			// case B_UP:
			// case B_DOWN:
				// ChangeDemoStatus = !ChangeDemoStatus;
				// Refresh = true;
				// break;
			// case B_LEFT:
				// if(ChangeDemoStatus)
					// DemoActive = !DemoActive;
				// else
				// {
					// if(ActualPage < MAX_PAGES - 1)
						// ActualPage++;
					// else
						// ActualPage = 0;
				// }
				// Refresh = true;
				// break;
			// case B_OK:
				// if(ChangeDemoStatus)
				// {
					// EnableSimulation = DemoActive;
					// if(DemoActive)
						// DrawPopUp("DEMO ON", 1000);
					// else
						// DrawPopUp("DEMO OFF", 1000);
				// }
				// else
				// {
					// RefreshPage.stop();
					// ClearScreen(true);
					// ExitDemoPage = true;
				// }
				// break;
			// default:
				// break;
		// }
		// delay(LOOPS_DELAY);
	// }
// }

void TaskMain()
{
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
		case RELE_STAT:
			DrawReleStatistics();
			break;
		case RESET_PAGE:
			DrawResetPage();
			break;
		// case DEMO_MODE:
			// DrawDemoActPage();
			// break;
		default:
			break;
	}

}
