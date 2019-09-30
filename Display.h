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
	RELE_STAT,            // 6
	RESET_PAGE,           // 7
	// DEMO_MODE, 			  // 7
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
	AVG_MEASURE_PERIOD,
	SIMULATION_MODE,
	CURRENT_HIGH_THR,
	CURRENT_LOW_THR,
	ACTIVE_POWER_HIGH_THR,
	REACTIVE_POWER_HIGH_THR,
	APPARENT_POWER_HIGH_THR,
	PF_HIGH_THR,
	PF_LOW_THR,		
	MAX_SETUP_ITEMS
}SETUP_ITEMS;

typedef enum
{
	ENUM_TYPE = 0,
	VALUE_TYPE
}PARAMETER_TYPE;

enum
{
	I_LOW_THR_0_0_5 =   5,
	I_LOW_THR_0_1   =  10,
	I_LOW_THR_0_5   =  50,
	I_LOW_THR_1     = 100,
	I_LOW_THR_1_1   = 110,
	I_LOW_THR_1_5   = 150,
	MAX_I_LOW_THR	= 6
};

enum
{
	I_HIGH_THR_5    =  500,
	I_HIGH_THR_5_5  =  550,
	I_HIGH_THR_10   = 1000,
	I_HIGH_THR_10_5 = 1050,
	I_HIGH_THR_13   = 1300,
	I_HIGH_THR_16   = 1600,
	MAX_I_HIGH_THR	= 6
};


enum
{
	P_ATT_HIGH_THR_1000 = 10,
	P_ATT_HIGH_THR_1500 = 15,
	P_ATT_HIGH_THR_1800 = 18,
	P_ATT_HIGH_THR_2000 = 20,
	P_ATT_HIGH_THR_2500 = 25,
	P_ATT_HIGH_THR_2800 = 28,
	P_ATT_HIGH_THR_3000 = 30,
	P_ATT_HIGH_THR_3400 = 34,
	MAX_P_ATT_HIGH_THR	= 8
};



enum
{
	P_REA_HIGH_THR_50   =   5,
	P_REA_HIGH_THR_100  =  10,
	P_REA_HIGH_THR_250  =  25,
	P_REA_HIGH_THR_500  =  50,
	P_REA_HIGH_THR_800  =  80,
	P_REA_HIGH_THR_1000 = 100,
	MAX_P_REA_HIGH_THR	= 6
};



enum
{
	P_APP_HIGH_THR_1000 = 10,
	P_APP_HIGH_THR_1500 = 15,
	P_APP_HIGH_THR_1800 = 18,
	P_APP_HIGH_THR_2000 = 20,
	P_APP_HIGH_THR_2500 = 25,
	P_APP_HIGH_THR_2800 = 28,
	P_APP_HIGH_THR_3000 = 30,
	P_APP_HIGH_THR_3400 = 34,
	MAX_A_APP_HIGH_THR	= 8
};

enum
{
	PF_THR_500 =  50,
	PF_THR_600 =  60,
	PF_THR_700 =  70,
	PF_THR_800 =  80,
	PF_THR_850 =  85,
	PF_THR_900 =  90,
	PF_THR_950 =  95,
	PF_THR_980 =  98,	
	PF_THR_990 =  99,	
	MAX_PF_THR =  9
};



typedef struct
{
	const char *EnumTitle;
	const uint16_t EnumValue;
}ENUM_VALUE;




typedef struct
{
	const char        *ParamTitle;
	const uint16_t    MaxVal; 
	const uint16_t    MinVal;	
	const uint8_t     Type;
	const ENUM_VALUE  *EnumList;
	const char        *Udm;
	bool 	    	  RestartMcu;
}SETUP_PARAMS;

typedef enum
{
	RESET_ENERGIES = 0,
	RESET_PAR_ENERGIES,
	RESET_MAX_MIN,
	RESET_AVG,
	RESET_N_ALARMS,
	RESET_RELE_STAT,
	RESTART_MCU,
	MAX_RESET_ITEMS
}RESET_ITEMS;

extern const SETUP_PARAMS SetupParams[];

void DisplayInit(void);
void DrawWelcomePage(void);
void DrawWiFiConnPage(String SSID, String IP_popup);
void ClearScreen(bool FullScreen);
void DrawPopUp(const char *Text, uint16_t Delay);

void TaskMain(void);

#endif
