#ifndef EEPROM_H
#define EEPROM_H


#define DFLT_WIFI_STATUS	1
#define DFLT_SAVE_DELAY	   15

extern bool SaveAccomplished;


void EepromInit(void);

void SaveParameters(void);
void TaskEeprom(void);

#endif