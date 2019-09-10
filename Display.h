#ifndef DISPLAY_H
#define DISPLAY_H

typedef enum
{
	MAIN_PAGE = 0,        // 0
	MEASURE_PAGE,         // 1
	RELE_PAGE,            // 2
	SETUP_PAGE,           // 3
	ALARM_SETUP_PAGE,     // 4
	ALARM_STATUS_PAGE,    // 5
	RESET_PAGE,           // 6
	MAX_PAGES 			  // 7
}MAIN_PAGES;

typedef struct
{
	int8_t PageToChange;
	const char *PageName;
	
}PAGE_DESCRIPTOR;



typedef struct
{

	double *FirstLineMeasure;
	double *SecondLineMeasure;
	double *ThirdLineMeasure;
	
	char   *UdmFirstLine;
	char   *UdmSecondLine;
	char   *UdmThirdLine;
	
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
	MIN_LINE_MEASURES,
	MAX_POWER_MEASURES,
	MIN_POWER_MEASURES,
	AVG_LINE_MEASURES,
	AVG_POWER_MEASURES,
	MAX_MEASURE_PAGES
	
}MEASURES_ITEM;


typedef struct
{
	const char *ResetTitle;
	void (*ResetFunc)(void);
}RESET_S;

typedef enum
{
	RESET_ENERGIES = 0,
	RESET_PAR_ENERGIES,
	RESET_MAX_MIN,
	RESET_AVG,
	RESTART_MCU,
	MAX_RESET_ITEMS	
}RESET_ITEMS;

void DisplayInit(void);
void DrawPopUp(const char *Text, uint16_t Delay);

void TaskDisplay(void);

#endif