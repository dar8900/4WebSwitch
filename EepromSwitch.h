#ifndef EEPROM_H
#define EEPROM_H




extern bool SaveAccomplished;
extern uint16_t EepParamsValue[];
extern uint8_t ReleInitStatus[];

void EepromInit(void);

void SaveParameters(void);
void TaskEeprom(void);

#endif