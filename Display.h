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


void DisplayInit(void);

void TaskDisplay(void);

#endif