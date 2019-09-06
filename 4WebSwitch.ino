#include "4WebSwitch.h"
#include "Rele.h"
#include "Measure.h"
#include "Web.h"
#include "KeyBoard.h"
#include "Display.h"


// #define TASK_DISPLAY
#define TASK_KEYBOARD
#define TASK_MEASURE
// #define TASK_WEB


void setup()
{
	
#ifdef NODE_MCU
	Wire.begin(5, 4);
#endif	
	Serial.begin(115200);
	AnalogBegin();
	ReleInit();
#ifdef TASK_WEB
	WifiInit();
#endif	
	
}

void loop()
{
#ifdef TASK_MEASURE
	TaskMeasure();
#endif

#ifdef TASK_WEB
	TaskWeb();
#endif

#ifdef TASK_DISPLAY
	TaskDisplay();
#endif

#ifdef TASK_KEYBOARD
	TaskKeyTest();
#endif
}