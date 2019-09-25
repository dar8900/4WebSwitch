#ifndef EEPROM_H
#define EEPROM_H


#define DFLT_WIFI_STATUS	          1
#define DFLT_SAVE_DELAY	  			 15
#define DFLT_AVG_MEASURE_PERIOD	     60

extern bool SaveAccomplished;
extern uint16_t EepParamsValue[];

void EepromInit(void);

void SaveParameters(void);
void TaskEeprom(void);

#endif