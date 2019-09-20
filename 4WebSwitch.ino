#include "4WebSwitch.h"
#include "Rele.h"
#include "Measure.h"
#include "Web.h"
#include "KeyBoard.h"
#include "Display.h"
#include "EepromSwitch.h"
#include "Alarms.h"

#define TASK_DISPLAY
// #define TASK_KEYBOARD
#define TASK_MEASURE
#define TASK_WEB

#define FW_VERSION 	1.3

void ResetMcu()
{
	ESP.restart();
}

void setup()
{
	
#ifdef NODE_MCU
	Wire.begin(5, 4);
#endif	
	Serial.begin(115200);
	// EepromInit();
	AnalogBegin();
#ifdef TASK_DISPLAY	
	DisplayInit();
	DrawWelcomePage();
#endif	

#ifdef TASK_WEB
	WifiInit();
#endif	
	ReleInit();
}

void loop()
{

#ifdef TASK_DISPLAY
	TaskDisplay();
#endif

}