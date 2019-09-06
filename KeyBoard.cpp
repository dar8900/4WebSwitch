#include "4WebSwitch.h"
#include "KeyBoard.h"

#define ADC_CONV_TO_V(AdcVal)  	((AdcVal * 3.3) / 1024)	


Chrono CheckKeyboard;
float AdcToVolt = 0.0;

uint8_t CheckButtons()
{
	uint16_t AdcVal = 0;
	AdcToVolt = 0.0;
	AdcVal = analogRead(A0);
	AdcToVolt = ADC_CONV_TO_V(AdcVal);
	// if(AdcToVolt <= 3.3 && AdcToVolt > 2.5) // 1k 10k
		// return UP;
	// else if(AdcToVolt <= 2.5 && AdcToVolt > 1.5) // 2k 10k
		// return DOWN;
	// else if(AdcToVolt <= 1.5 && AdcToVolt > 0.9) // 3k 10kR 
		// return OK;
	// else
		return NO_PRESS;
}


void TaskKeyTest()
{
	if(CheckKeyboard.hasPassed(250, true))
	{
		CheckButtons();
		Serial.println("Tensione letta: " +  String(AdcToVolt, 2));
	}
}