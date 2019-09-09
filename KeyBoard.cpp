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
	if(AdcToVolt > 3) // 1k 10k
		return B_UP;
	else if(AdcToVolt > 2.8) // 2k 10k
		return B_DOWN;
	else if(AdcToVolt > 2.5) // 3k 10kR 
		return B_LEFT;
	else if(AdcToVolt > 2.1) // 4k 10kR 
		return B_OK;		
	else
		return NO_PRESS;
}


void TaskKeyTest()
{
	uint8_t Button = NO_PRESS;
	if(CheckKeyboard.hasPassed(80, true))
	{
		Button = CheckButtons();
		if(Button != NO_PRESS)
		{
			String Bottone = ""; 
			switch(Button)
			{
				case B_UP:
					Bottone = "UP!";
					break;
				case B_DOWN:
					Bottone = "DOWN!";
					break;
				case B_OK:
					Bottone = "OK!";
					break;					
			}
			DBG("Bottone letto: " +  Bottone);
		}
		DBG("Tensione letta: " + String(AdcToVolt, 3) + "V");
		Button = NO_PRESS;
	}
}