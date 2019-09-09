#ifndef KEYBOAD_H
#define KEYBOAD_H

#include <Arduino.h>

typedef enum
{
	B_UP = 0,
	B_DOWN,
	B_LEFT,
	B_OK,
	NO_PRESS
}BUTTONS;

uint8_t CheckButtons(void);

void TaskKeyTest(void);

#endif