#ifndef WEB_H
#define WEB_H

#include <Arduino.h>


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
extern uint32_t TimeInSecond;

void WifiInit(void);
String IPAddr(void);

String FormatTime(uint32_t TimeStamp, bool WantSeconds);
String FormatDate(uint32_t TimeStamp);

void TaskWeb(void);



#endif