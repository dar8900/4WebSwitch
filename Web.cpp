#include "4WebSwitch.h"

#ifdef NODE_MCU

#include "Web.h"
#include "Rele.h"
#include "Measure.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <fauxmoESP.h>

#define MY_WIFI_LIST		 3
#define MAX_WIFI_DEVICE		10

// #define ALEXA

// extern String Header;
// extern String WebPage_1;
// extern String ReleTable_1;
// extern String TableSep_1;
// extern String ReleTable_2;
// extern String TableSep_2;
// extern String ReleTable_3;
// extern String TableSep_3;
// extern String ReleTable_4;
// extern String MeasureLines;

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

const char* HostName = "webSwitchStation";
const uint16_t TimeShiftUTC = 7200;

// WiFiServer  server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", TimeShiftUTC);
WEBTIME GlobalTime;

fauxmoESP AlexaControl;

String TimeFormatted = ""; 
String DateFormatted = ""; 

Chrono GetTimeTimer(Chrono::SECONDS);

Chrono ToggleRele1Timer;

// bool ClientConnected = false;
// uint8_t OldReleStatus[N_RELE];
bool WifiConnected = false;
uint8_t WifiSignal;


NETWORK_LIST WifiList[MY_WIFI_LIST] = 
{
	{"dario_idem"    , "dari9299"  },
	{"DOOM_SLAYER"   , "dari9299"  },
	{"FASTWEB-85B1EA", "6WZPAF2A99"},
};

DEVICE_FOUND_DES WifiDeviceList[MAX_WIFI_DEVICE];


const char* ReleID[N_RELE] = 
{
	"presa 1",
	"presa 2",
	"presa 3",
	"presa 4"
};


const String SignalLabel[MAX_SIGNAL_LABEL] = 
{
	"NO_SIGNAL",
	"PESSIMO",
	"BUONO",
	"OTTIMO"	
};


static bool CheckWifiCon()
{
	WifiConnected = WiFi.isConnected();
	return WifiConnected;
}

String IPAddr()
{
	String IP = "";
	IP = String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + 
			String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
	return IP;
}

static uint8_t GetWifiSignalPower()
{
	int32_t Signal = abs(WiFi.RSSI());	
	if(Signal >= 0 && Signal <= 50)
		return OTTIMO;
	else if(Signal >= 51 && Signal <= 75)
		return BUONO;
	else if(Signal >= 76 && Signal <= 90)
		return PESSIMO;
	else
		return NO_SIGNAL;
}

static String FormatDateFromWeb(time_t TimeStamp)
{
   struct tm * ti;
   ti = localtime (&TimeStamp);

   uint16_t year = (ti->tm_year + 1900) % 100;
   String yearStr = String(year);

   uint8_t month = ti->tm_mon + 1;
   String monthStr = month < 10 ? "0" + String(month) : String(month);

   uint8_t day = ti->tm_mday;
   String dayStr = day < 10 ? "0" + String(day) : String(day);

   return dayStr + "/" + monthStr + "/" + yearStr;
}

static void GetTime()
{
	timeClient.update();
	String Hour = "", Minute = "";
	uint32_t TimeInSecond = timeClient.getEpochTime();	
	GlobalTime.Hour = (uint8_t)((TimeInSecond / 3600) % 24);
	GlobalTime.Minute = (uint8_t)((TimeInSecond / 60) % 60);
	GlobalTime.Second = (uint8_t)(TimeInSecond % 60);
	if(GlobalTime.Hour > 9)
		Hour = String(GlobalTime.Hour);
	else
		Hour = "0" + String(GlobalTime.Hour);
	if(GlobalTime.Minute > 9)
		Minute = String(GlobalTime.Minute);
	else
		Minute = "0" + String(GlobalTime.Minute);	
	TimeFormatted = Hour + ":" + Minute;
	DateFormatted = FormatDateFromWeb((time_t)TimeInSecond);
	return;
}

static void AlexaInit()
{
	AlexaControl.createServer(true); 
	AlexaControl.setPort(80); 
	AlexaControl.enable(true);	
	AlexaControl.addDevice(ReleID[RELE_1]);
	AlexaControl.addDevice(ReleID[RELE_2]);
	AlexaControl.addDevice(ReleID[RELE_3]);
	AlexaControl.addDevice(ReleID[RELE_4]);
	AlexaControl.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) 
	{
		for(int ReleIndex = 0; ReleIndex < N_RELE; ReleIndex++)
		{
			if(strcmp(device_name, ReleID[ReleIndex]) == 0)
			{
				if(state)
				{
					if(Rele.getReleStatus(ReleIndex) == STATUS_OFF)
						TurnOnRele(ReleIndex);					
				}
				else
				{
					if(Rele.getReleStatus(ReleIndex) == STATUS_ON)
						TurnOffRele(ReleIndex);						
				}
				break;				
			}
		}
		// if (strcmp(device_name, ReleID[RELE_1]) == 0) 
		// {
			// if(state)
			// {
				// if(Rele.getReleStatus(RELE_1) == STATUS_OFF)
					// TurnOnRele(RELE_1);					
			// }
			// else
			// {
				// if(Rele.getReleStatus(RELE_1) == STATUS_ON)
					// TurnOffRele(RELE_1);						
			// }
		// } 
		// else if (strcmp(device_name, ReleID[RELE_2]) == 0) 
		// {
			// if(state)
			// {
				// if(Rele.getReleStatus(RELE_2) == STATUS_OFF)
					// TurnOnRele(RELE_2);					
			// }
			// else
			// {
				// if(Rele.getReleStatus(RELE_2) == STATUS_ON)
					// TurnOffRele(RELE_2);						
			// }
		// }
		// else if (strcmp(device_name, ReleID[RELE_3]) == 0)
		// {
			// if(state)
			// {
				// if(Rele.getReleStatus(RELE_3) == STATUS_OFF)
					// TurnOnRele(RELE_3);					
			// }
			// else
			// {
				// if(Rele.getReleStatus(RELE_3) == STATUS_ON)
					// TurnOffRele(RELE_3);						
			// }
		// } 
		// else if (strcmp(device_name, ReleID[RELE_4]) == 0)
		// {
			// if(state)
			// {
				// if(Rele.getReleStatus(RELE_4) == STATUS_OFF)
					// TurnOnRele(RELE_4);					
			// }
			// else
			// {
				// if(Rele.getReleStatus(RELE_4) == STATUS_ON)
					// TurnOffRele(RELE_4);						
			// }
		// } 

	});

}

static void ScanResult(int N_Device)
{
	DBG("Device trovati: " + String(N_Device));
	if(N_Device <= MAX_WIFI_DEVICE)
	{
		for(int Device = 0; Device < N_Device; Device++)
		{
			WifiDeviceList[Device].DeviceSSID = WiFi.SSID(Device);
			WifiDeviceList[Device].DeviceRSSI = WiFi.RSSI(Device);
		}
	}
	else
	{
		for(int Device = 0; Device < MAX_WIFI_DEVICE; Device++)
		{
			WifiDeviceList[Device].DeviceSSID = WiFi.SSID(Device);
			WifiDeviceList[Device].DeviceRSSI = WiFi.RSSI(Device);
		}		
	}
}

void WifiInit()
{
	int DeviceFoundIndex = 0, MyDeviceList = 0;
	bool MyDeviceFounded = false;
	WiFi.mode(WIFI_STA);

	DeviceFoundIndex = WiFi.scanNetworks();
	ScanResult(DeviceFoundIndex);
	for(DeviceFoundIndex = 0; DeviceFoundIndex < MAX_WIFI_DEVICE; DeviceFoundIndex++)
	{
		MyDeviceFounded = false;
		for(MyDeviceList = 0; MyDeviceList < MY_WIFI_LIST; MyDeviceList++)
		{
			if(WifiDeviceList[DeviceFoundIndex].DeviceSSID == String(WifiList[MyDeviceList].SSID))
			{
				DBG(WifiDeviceList[DeviceFoundIndex].DeviceSSID);
				MyDeviceFounded = true;
				break;
			}
		}
		if(MyDeviceFounded)
			break;
	}
	
	if(MyDeviceFounded)
	{
		WiFi.begin(WifiList[MyDeviceList].SSID, WifiList[MyDeviceList].Passwd);
		Serial.print("Connecting...");
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(250);
			Serial.print(".");
		}
		WiFi.hostname(HostName);
		WiFi.setAutoReconnect(true);
		
		// server.begin();
		timeClient.begin();
		DBG("IP: " + IPAddr());
		delay(1000);
		if(CheckWifiCon())
		{
#ifdef ALEXA			
			AlexaInit();
#endif			
		}
		GetTime();
		WifiSignal = GetWifiSignalPower();
	}

}




static void GesAlexa()
{
	AlexaControl.handle();
}

// static void GesClient()
// {
	// if(CheckWifiCon())
	// {
		// String request = "";
		// WiFiClient client = server.available();
		// if (!client.connected())  
		// { 
			// ClientConnected = false;
		// }
		// else
		// {
			// ClientConnected = true;
			// // Read the first line of the request
			// request = client.readStringUntil('\r');
			// DBG("Request:" + String(request));
		   
			// if( request.indexOf("RELEON_1") > 0 )  
			// {
				// TurnOnRele(RELE_1);  
				// OldReleStatus[RELE_1] = Rele.getReleStatus(RELE_1);
			// }
			// else if( request.indexOf("RELEON_2") > 0 ) 
			// { 
				// TurnOnRele(RELE_2);   
				// OldReleStatus[RELE_2] = Rele.getReleStatus(RELE_2);
			// }
			// else if( request.indexOf("RELEON_3") > 0 ) 
			// { 
				// TurnOnRele(RELE_3);   
				// OldReleStatus[RELE_3] = Rele.getReleStatus(RELE_3);
			// }
			// else if( request.indexOf("RELEON_4") > 0 ) 
			// { 
				// TurnOnRele(RELE_4);   
				// OldReleStatus[RELE_4] = Rele.getReleStatus(RELE_4);
			// }
			// else if( request.indexOf("RELEOFF_1") > 0 ) 
			// { 
				// TurnOffRele(RELE_1);   
				// OldReleStatus[RELE_1] = Rele.getReleStatus(RELE_1);
			// }
			// else if( request.indexOf("RELEOFF_2") > 0 ) 
			// { 
				// TurnOffRele(RELE_2);   
				// OldReleStatus[RELE_2] = Rele.getReleStatus(RELE_2);
			// }
			// else if( request.indexOf("RELEOFF_3") > 0 ) 
			// { 
				// TurnOffRele(RELE_3);   
				// OldReleStatus[RELE_3] = Rele.getReleStatus(RELE_3);
			// }
			// else if( request.indexOf("RELEOFF_4") > 0 ) 
			// { 
				// TurnOffRele(RELE_4);   
				// OldReleStatus[RELE_4] = Rele.getReleStatus(RELE_4);
			// }
			// else if(request.indexOf("RELE1STATUS") > 0)
			// {
				// DBG("Richiesta di status rele 1");
				// if (Rele.getReleStatus(RELE_1) == STATUS_OFF)
				// {
					// client.print( Header );
					// client.print("STATUS_OFF");
				// }
				// else
				// {
					// client.print( Header );
					// client.print("STATUS_ON");			
				// }		
			// }
			// else if(request.indexOf("RELE2STATUS") > 0 )
			// {
				// DBG("Richiesta di status rele 2");
				// if (Rele.getReleStatus(RELE_2) == STATUS_OFF)
				// {
					// client.print( Header );
					// client.print("STATUS_OFF");
				// }
				// else
				// {
					// client.print( Header );
					// client.print("STATUS_ON");			
				// }		
			// }
			// else if(request.indexOf("RELE3STATUS") > 0)
			// {
				// DBG("Richiesta di status rele 3");
				// if (Rele.getReleStatus(RELE_3) == STATUS_OFF)
				// {
					// client.print( Header );
					// client.print("STATUS_OFF");
				// }
				// else
				// {
					// client.print( Header );
					// client.print("STATUS_ON");			
				// }		
			// }
			// else if(request.indexOf("RELE4STATUS") > 0)
			// {
				// DBG("Richiesta di status rele 4");
				// if (Rele.getReleStatus(RELE_4) == STATUS_OFF)
				// {
					// client.print( Header );
					// client.print("STATUS_OFF");
				// }
				// else
				// {
					// client.print( Header );
					// client.print("STATUS_ON");			
				// }		
			// }
			// else
			// {
				// DBG("Refresh pagina");
				// client.flush();
				// client.print( Header );
				// client.print( WebPage_1 );   
				// client.print( ReleTable_1 ); 
				// client.print( TableSep_1 ); 
				// client.print( ReleTable_2 ); 
				// client.print( TableSep_2 ); 
				// client.print( ReleTable_3 ); 
				// client.print( TableSep_3 ); 
				// client.print( ReleTable_4 ); 
				// client.print( MeasureLines ); 
				// delay(1);			
			// }
		// }
	// }
// }


void TaskWeb()
{
	if(WifiConnected)
	{
#ifdef ALEXA		
		GesAlexa();
#endif		
		if(GetTimeTimer.hasPassed(15, true))
		{
			GetTime();
		}
		WifiSignal = GetWifiSignalPower();
	}
	else
	{
		CheckWifiCon();
		TimeFormatted = "--:--";
		DateFormatted = "--/--/--";
	}
}

#endif

