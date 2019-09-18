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

#define REFRESH_DELAY		500
#define LOOPS_DELAY		     50

TFT_eSPI Display = TFT_eSPI();

Chrono RefreshPage;


const unsigned char *WifiIcons[] =
{
	NoSignal_bits,
	Pessimo_bits,
	Buono_bits,
	Ottimo_bits,
};

const char * DisplayPages[MAX_PAGES] =
{
	"Home"          ,
	"Misure"        ,
	"Rele"          ,
	"Setup"         ,
	"Setup allarmi" ,
	"Allarmi"       ,
	"Reset"         ,
	"Demo"          ,
};

const MEASURE_PAGES MeasuresPage[MAX_MEASURE_PAGES] =
{
	{&Measures.CurrentRMS, 			     &Measures.VoltageRMS, 			       &Measures.PowerFactor	   	       , true, true, false},
	{&Measures.ActivePower, 		     &Measures.ReactivePower,			   &Measures.ApparentPower			   , true, true, true },
	{&Measures.ActiveEnergy, 	         &Measures.ReactiveEnergy, 			   &Measures.ApparentEnergy		       , true, true, true },
	{&Measures.PartialActiveEnergy,      &Measures.PartialReactiveEnergy, 	   &Measures.PartialApparentEnergy	   , true, true, true },
	{&Measures.MaxMinAvg.MaxCurrent,     &Measures.MaxMinAvg.MaxVoltage, 	   &Measures.MaxMinAvg.MaxPowerFactor  , true, true, false},
	{&Measures.MaxMinAvg.MaxActivePower, &Measures.MaxMinAvg.MaxReactivePower, &Measures.MaxMinAvg.MaxApparentPower, true, true, true },
	{&Measures.MaxMinAvg.MinCurrent,     &Measures.MaxMinAvg.MinVoltage, 	   &Measures.MaxMinAvg.MinPowerFactor  , true, true, false},
	{&Measures.MaxMinAvg.MinActivePower, &Measures.MaxMinAvg.MinReactivePower, &Measures.MaxMinAvg.MinApparentPower, true, true, true },
	{&Measures.MaxMinAvg.CurrentAvg,     &Measures.MaxMinAvg.VoltageAvg, 	   &Measures.MaxMinAvg.PowerFactorAvg  , true, true, false},
	{&Measures.MaxMinAvg.ActivePowerAvg, &Measures.MaxMinAvg.ReactivePowerAvg, &Measures.MaxMinAvg.ApparentPowerAvg, true, true, true },

};


const MEASURE_PAGE_LABEL_DES MeasureUdmLabel[MAX_MEASURE_PAGES] PROGMEM =
{
	{"I, V, PF"        , "A" , "V"   , " "  , "I"      , "V"      , "PF"      },
	{"Potenze"	       , "W" , "VAr" , "VA" , "P.att"  , "P.rea"  , "P.app"   },
	{"Energie tot."    , "Wh", "VArh", "VAh", "E.att"  , "E.rea"  , "E.app"   },
	{"Energie parz."   , "Wh", "VArh", "VAh", "EP Att" , "EP Rea" , "EP App"  },
	{"Massimi I, V, PF", "A" , "V"   , " "  , "Max I"  , "Max V"  , "MaxPF"   },
	{"Massimi potenze" , "W" , "VAr" , "VA" , "MaxPAtt", "MaxPRea", "MaxPApp" },
	{"Minimi I, V, PF" , "A" , "V"   , " "  , "Min I"  , "Min V"  , "MinPF"   },
	{"Minimi potenze"  , "W" , "VAr" , "VA" , "MinPAtt", "MinPRea", "MinPApp" },
	{"Medie I, V, PF"  , "A" , "V"   , " "  , "Avg I"  , "Avg V"  , "AvgPF"   },
	{"Medie potenze"   , "W" , "VAr" , "VA" , "AvgPAtt", "AvgPRea", "AvgPApp" },

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


const char *Reset[MAX_RESET_ITEMS] =
{
	"Reset energie",
	"Reset energie parziali",
	"Reset max e min",
	"Reset medie",
	"Reset n. allarmi",
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

static void TaskManagement()
{
	TaskMeasure();
	TaskAlarm();
	TaskWeb();
	RefreshReleStatistics();
}


static void WichReset(uint8_t ResetItem)
{
	switch(ResetItem)
	{
		case RESET_ENERGIES:
			ResetTotalEnergy();
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
		case RESTART_MCU:
			ResetMcu();
			break;
		default:
			break;
	}
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
	Display.drawRoundRect(0, 0, Display.width(), Display.height(), 5, TFT_WHITE);
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
	bool ExitMainScreen = false;
	while(!ExitMainScreen)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearScreen(true);
		DrawTopInfoBar();
		DrawPageChange(ActualPage, true);
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
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
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
				break;
			case B_DOWN:
				if(MeasurePage < MAX_MEASURE_PAGES - 1)
					MeasurePage++;
				else
					MeasurePage = 0;
				break;
			case B_LEFT:
				if(ActualPage < MAX_PAGES - 1)
					ActualPage++;
				else
					ActualPage = MAIN_PAGE;
				break;
			case B_OK:
				RefreshPage.stop();
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
	bool ExitRelePage = false, ReleStatusSelected = false;
	RefreshPage.restart();
	while(!ExitRelePage)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearScreen(true);
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !ReleStatusSelected);
		RefreshReleChangeStatus(ReleIndex, ReleStatusSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
			case B_DOWN:
				ReleStatusSelected = !ReleStatusSelected;
				break;
			case B_LEFT:
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
				break;
			case B_OK:
				if(ReleStatusSelected)
				{
					ToggleRele(ReleIndex);
				}
				else
				{
					RefreshPage.stop();
					ExitRelePage = true;
				}
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}
}

static void RefreshSetupPage(uint8_t SetupItem, bool SetupSelected)
{

}


static void DrawSetupPage()
{
	bool ExitSetupPage = false, SetupSelected = false;
	uint8_t SetupItem = 0;
	while(!ExitSetupPage)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearScreen(true);
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !SetupSelected);
		RefreshSetupPage(SetupItem, SetupSelected);
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
					ActualPage = 0;
				break;
			case B_OK:
				ExitSetupPage = true;
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
	bool ExitAlarmSetupPage = false, AlarmSelected = false;
	uint8_t AlarmItem = 0;
	RefreshPage.restart();
	while(!ExitAlarmSetupPage)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearScreen(true);
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !AlarmSelected);
		RefreshAlarmSetupList(AlarmItem, AlarmSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
			case B_DOWN:
				AlarmSelected = !AlarmSelected;
				break;
			case B_LEFT:
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
				break;
			case B_OK:
				if(AlarmSelected)
				{
					ModifyAlarm(AlarmItem);
				}
				else
				{
					RefreshPage.stop();
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
	Display.drawString(AlarmsName[AlarmItem], CENTER_POS(AlarmsName[AlarmItem]), 20);
	if(Alarms[AlarmItem].IsActive)
	{
		Display.drawString("ATTIVO", CENTER_POS("ATTIVO"), Display.fontHeight() + 5, TFT_YELLOW);
		if(Alarms[AlarmItem].WichThr == UNDER_THR)
		{
			Display.drawString(UnderThrAlarmMessage[AlarmItem], CENTER_POS(UnderThrAlarmMessage[AlarmItem]), Display.fontHeight() + 10);
		}
		else
		{
			Display.drawString(OverThrAlarmMessage[AlarmItem], CENTER_POS(OverThrAlarmMessage[AlarmItem]), Display.fontHeight() + 10);
		}
		String AlarmTime = FormatTime(Alarms[AlarmItem].AlarmTime, true), AlarmDate = FormatDate(Alarms[AlarmItem].AlarmTime);
		String OccurenceStr = String(Alarms[AlarmItem].Occurences);
		Display.drawString(AlarmTime, CENTER_POS(AlarmTime), Display.fontHeight() + 15);
		Display.drawString(AlarmDate, CENTER_POS(AlarmDate), Display.fontHeight() + 20);
		Display.drawString(OccurenceStr, CENTER_POS(OccurenceStr), Display.fontHeight() + 25);
	}
	else
	{
		Display.drawString("NON ATTIVO", CENTER_POS("NON ATTIVO"), Display.fontHeight() + 5, TFT_GREEN);
	}
	Display.setFreeFont(FM9);
	String AlarmNumberStr = String(AlarmItem + 1) + "/" + String(MAX_ALARM);
	Display.drawString(AlarmNumberStr, CENTER_POS(AlarmNumberStr), 200);
}


static void DrawAlarmStatusPage()
{
	bool ExitAlarmStatusPage = false, AlarmSelected = false;
	uint8_t AlarmItem = CURRENT;
	RefreshPage.restart();
	while(!ExitAlarmStatusPage)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearScreen(true);
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !AlarmSelected);
		RefreshAlarmStatus(AlarmItem, AlarmSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
			case B_DOWN:
				AlarmSelected = !AlarmSelected;
				break;
			case B_LEFT:
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
				break;
			case B_OK:
				RefreshPage.stop();
				ExitAlarmStatusPage = true;
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
	bool ExitResetPage = false, ResetSelected = false;
	uint8_t ResetItem = 0;
	RefreshPage.restart();
	while(!ExitResetPage)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearScreen(true);
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !ResetSelected);
		RefreshResetList(ResetItem, ResetSelected);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
			case B_DOWN:
				ResetSelected = !ResetSelected;
				break;
			case B_LEFT:
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
				break;
			case B_OK:
				if(ResetSelected)
				{
					WichReset(ResetItem);
					DrawPopUp("Reset eseguito", 1500);
				}
				else
				{
					RefreshPage.stop();
					ExitResetPage = true;
				}
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}
}


static void RefreshDemoAct(bool DemoActive, bool ChangeDemoStatus)
{
	String DemoStr = "DISABILITATO";
	Display.setFreeFont(FMB18);
	if(DemoActive)
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
	if(ChangeDemoStatus)
		Display.drawRoundRect( CENTER_POS(DemoStr) - 4,  70 - 4,  Display.textWidth(DemoStr) + 4,  (Display.fontHeight() + 4),  2,  TFT_WHITE);
}


static void DrawDemoActPage()
{
	bool ExitDemoPage = false, ChangeDemoStatus = false;
	bool DemoActive = EnableSimulation;
	while(!ExitDemoPage)
	{
		TaskManagement();
		if(RefreshPage.hasPassed(REFRESH_DELAY, true))
			ClearScreen(true);
		DrawTopInfoBar();
		DrawPageChange(ActualPage, !ChangeDemoStatus);
		RefreshDemoAct(DemoActive, ChangeDemoStatus);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case B_UP:
			case B_DOWN:
				ChangeDemoStatus = !ChangeDemoStatus;
				break;
			case B_LEFT:
				if(ChangeDemoStatus)
					DemoActive = !DemoActive;
				else
				{
					if(ActualPage < MAX_PAGES - 1)
						ActualPage++;
					else
						ActualPage = 0;
				}
				break;
			case B_OK:
				EnableSimulation = DemoActive;
				if(DemoActive)
					DrawPopUp("DEMO ON", 1000);
				else
					DrawPopUp("DEMO OFF", 1000);
				ExitDemoPage = true;
				break;
			default:
				break;
		}
		delay(LOOPS_DELAY);
	}
}

void TaskDisplay()
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
		case RESET_PAGE:
			DrawResetPage();
			break;
		case DEMO_MODE:
			DrawDemoActPage();
			break;
		default:
			break;
	}

}
