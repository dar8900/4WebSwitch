#ifndef WEB_4_SWITCH_H
#define WEB_4_SWITCH_H
#include <stdint.h>
#include <Wire.h>
#include <Chrono.h>

#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define DBG(Str) 	Serial.println(Str)
#else
#define DBG(Str)
#endif


#define NODE_MCU
// #define ARDUINO_BOARD_MCU

#define FW_VERSION 	1.06

#ifdef NODE_MCU
typedef enum
{
	DATAPIN         = 15,
	CLOCKPIN 	    = 10,
	LATCHPIN 	    =  9,
	OUTPUTENABLEPIN =  2,
	CLEARREGPIN 	=  0 
	
}PIN_NODE;
#endif

#ifdef ARDUINO_BOARD_MCU
typedef enum
{
	DATAPIN         = 2,
	CLOCKPIN 	    = 3,
	LATCHPIN 	    = 4,
	OUTPUTENABLEPIN = 5,
	CLEARREGPIN 	= 0 
	
}PIN_NODE;
#endif


typedef enum
{
	RELE_1,
	RELE_2,
	RELE_3,
	RELE_4,
	N_RELE
}RELE_NUMBER;

void ResetMcu(void);


#endif
