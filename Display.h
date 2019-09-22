#ifndef DISPLAY_H
#define DISPLAY_H

enum
{
	DISABILITATO = 0,
	ABILITATO
};




typedef enum
{
	MAIN_PAGE = 0,        // 0
	MEASURE_PAGE,         // 1
	RELE_PAGE,            // 2
	SETUP_PAGE,           // 3
	ALARM_SETUP_PAGE,     // 4
	ALARM_STATUS_PAGE,    // 5
	RESET_PAGE,           // 6
	DEMO_MODE, 			  // 7
	MAX_PAGES 			  // 8
}MAIN_PAGES;




typedef struct
{

	double *FirstLineMeasure;
	double *SecondLineMeasure;
	double *ThirdLineMeasure;

	bool   ReformatFirstLine;
	bool   ReformatSecondLine;
	bool   ReformatThirdLine;

}MEASURE_PAGES;

typedef struct
{
	double Value;
	double RefactorValue;
	char   *Odg;
}REFORMAT;

typedef enum
{
	LINE_MEASURES = 0,
	POWER_MEASURES,
	TOTAL_ENERGIES,
	PARTIAL_ENERGIES,
	MAX_LINE_MEASURES,
	MAX_POWER_MEASURES,
	MIN_LINE_MEASURES,
	MIN_POWER_MEASURES,
	AVG_LINE_MEASURES,
	AVG_POWER_MEASURES,
	MAX_MEASURE_PAGES

}MEASURES_ITEM;

typedef struct
{
	const char *PageTitle;
	const char *UdmFirstLine;
	const char *UdmSecondLine;
	const char *UdmThirdLine;
	const char *MeasureLabelFirstLine;
	const char *MeasureLabelSecondLine;
	const char *MeasureLabelThirdLine;
}MEASURE_PAGE_LABEL_DES;



typedef struct
{
	const char *MenuTitle;
	void (*MenuFunc)(void);
}MENU_ITEM_S;


typedef enum
{
	WIFI_STATUS = 0,
	EEPROM_SAVE_DELAY,
	MAX_SETUP_ITEMS
}SETUP_ITEMS;

typedef enum
{
	ENUME_TYPE = 0,
	VALUE_TYPE
}PARAMETER_TYPE;

typedef struct
{
	const char *EnumTitle;
	const uint8_t EnumValue;
}ENUM_VALUE;

typedef struct
{
	const char    *ParamTitle;
	uint16_t       Value;
	const uint16_t MaxVal; 
	const uint16_t MinVal;	
	const uint8_t  Type;
	const ENUM_VALUE  *EnumList;
	const char *Udm;
}SETUP_PARAMS;

typedef enum
{
	RESET_ENERGIES = 0,
	RESET_PAR_ENERGIES,
	RESET_MAX_MIN,
	RESET_AVG,
	RESET_N_ALARMS,
	RESTART_MCU,
	MAX_RESET_ITEMS
}RESET_ITEMS;

extern SETUP_PARAMS SetupParams[];


void DisplayInit(void);
void DrawWelcomePage(void);
void DrawWiFiConnPage(String SSID, String IP_popup);
void ClearScreen(bool FullScreen);
void DrawPopUp(const char *Text, uint16_t Delay);

void TaskDisplay(void);

#endif
