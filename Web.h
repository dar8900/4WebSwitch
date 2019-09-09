#ifndef WEB_H
#define WEB_H

#include <Arduino.h>

typedef struct
{
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint8_t Day;
	uint8_t Month;
	uint16_t Year;
}WEBTIME;


typedef enum
{
	NO_SIGNAL = 0,
	PESSIMO,
	BUONO,
	OTTIMO,
	MAX_SIGNAL_LABEL
}WIFI_POWER;

extern bool WifiConnected;
extern String TimeFormatted;
extern String DateFormatted;
extern uint8_t WifiSignal;

void WifiInit(void);
String IPAddr(void);

void TaskWeb(void);



#endif