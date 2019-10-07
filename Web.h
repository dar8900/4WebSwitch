#ifndef WEB_H
#define WEB_H

#include <Arduino.h>

#define MAX_WIFI_DEVICE		10

typedef enum
{
	NO_SIGNAL = 0,
	PESSIMO,
	BUONO,
	OTTIMO,
	MAX_SIGNAL_LABEL
}WIFI_POWER;

typedef struct
{
	const char *SSID;
	const char* Passwd;
}NETWORK_LIST;

typedef struct
{
	String DeviceSSID;
	int32_t DeviceRSSI;
}DEVICE_FOUND_DES;

extern bool WifiConnected;
extern String TimeFormatted;
extern String DateFormatted;
extern uint8_t WifiSignal;
extern uint32_t TimeInSecond;
extern uint8_t MaxWifiDeviceFounded;
extern DEVICE_FOUND_DES WifiDeviceList[];
extern NETWORK_LIST MyNetworksList[];
extern uint8_t MyDeviceConnected;

void WifiInit(void);
String IPAddr(void);
uint8_t GetWifiSignalPower(int32_t RssiSignal);

String FormatTime(uint32_t TimeStamp, bool WantSeconds);
String FormatDate(uint32_t TimeStamp);


void TaskWeb(void);



#endif