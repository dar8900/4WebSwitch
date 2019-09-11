#include "4WebSwitch.h"
#include "Measure.h"
#include "Rele.h"
#include "EepromSwitch.h"
#include <EEPROM.h>


#define MEASURES_SIZE					 8
#define RELE_STATUS_SIZE			     1
#define RELE_OCCUR_SIZE					 2
#define RELE_POWERON_SIZE				 4

#define ENERGIES_START_ADDR			     0
#define MAX_MIN_AVG_START_ADDR    	    48

#define RELE_STATUS_START_ADDR    	   200
#define RELE_STAT_START_ADDR 		   204



#define TIMER_EEPROM_SAVE(Min)         (Min * 60)

Chrono  EepromSaveTimer(Chrono::SECONDS);

static void SaveEnergies()
{
	int EepromAddrInit = ENERGIES_START_ADDR;
	EEPROM.put(EepromAddrInit, Measures.ActiveEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.ReactiveEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.ApparentEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.PartialActiveEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.PartialReactiveEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.PartialApparentEnergy);	
}

static void SaveMaxMinAvg()
{
	int EepromAddrInit = MAX_MIN_AVG_START_ADDR;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MaxCurrent);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MinCurrent);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MaxVoltage);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MinVoltage);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MaxActivePower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MinActivePower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MaxReactivePower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MinReactivePower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MaxApparentPower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MinApparentPower);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MaxPowerFactor);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.MinPowerFactor);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.CurrentAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.VoltageAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.ActivePowerAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.ReactivePowerAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.ApparentPowerAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.put(EepromAddrInit, Measures.MaxMinAvg.PowerFactorAvg);	
}

static void SaveReleSatus()
{
	int EepromAddrInit = RELE_STATUS_START_ADDR;
	EEPROM.write(EepromAddrInit, Rele.getReleStatus(RELE_1));
	EepromAddrInit += RELE_STATUS_SIZE;
	EEPROM.write(EepromAddrInit, Rele.getReleStatus(RELE_2));
	EepromAddrInit += RELE_STATUS_SIZE;
	EEPROM.write(EepromAddrInit, Rele.getReleStatus(RELE_3));
	EepromAddrInit += RELE_STATUS_SIZE;
	EEPROM.write(EepromAddrInit, Rele.getReleStatus(RELE_4));	
}

static void SaveReleStatistics()
{
	int EepromAddrInit = RELE_STAT_START_ADDR;
	EEPROM.put(EepromAddrInit, ReleStatistics[RELE_1].NSwitches);
	EepromAddrInit += RELE_OCCUR_SIZE;
	EEPROM.put(EepromAddrInit, ReleStatistics[RELE_2].NSwitches);
	EepromAddrInit += RELE_OCCUR_SIZE;
	EEPROM.put(EepromAddrInit, ReleStatistics[RELE_3].NSwitches);
	EepromAddrInit += RELE_OCCUR_SIZE;
	EEPROM.put(EepromAddrInit, ReleStatistics[RELE_4].NSwitches);	
	EepromAddrInit += RELE_OCCUR_SIZE;
	EEPROM.put(EepromAddrInit, ReleStatistics[RELE_1].PowerOnTime);
	EepromAddrInit += RELE_POWERON_SIZE;
	EEPROM.put(EepromAddrInit, ReleStatistics[RELE_2].PowerOnTime);
	EepromAddrInit += RELE_POWERON_SIZE;
	EEPROM.put(EepromAddrInit, ReleStatistics[RELE_3].PowerOnTime);
	EepromAddrInit += RELE_POWERON_SIZE;
	EEPROM.put(EepromAddrInit, ReleStatistics[RELE_4].PowerOnTime);		
}

static void LoadEnergies()
{
	int EepromAddrInit = ENERGIES_START_ADDR;
	EEPROM.get(EepromAddrInit, Measures.ActiveEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.ReactiveEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.ApparentEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.PartialActiveEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.PartialReactiveEnergy);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.PartialApparentEnergy);		
}

static void LoadMaxMinAvg()
{
	int EepromAddrInit = MAX_MIN_AVG_START_ADDR;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MaxCurrent);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MinCurrent);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MaxVoltage);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MinVoltage);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MaxActivePower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MinActivePower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MaxReactivePower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MinReactivePower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MaxApparentPower);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MinApparentPower);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MaxPowerFactor);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.MinPowerFactor);
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.CurrentAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.VoltageAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.ActivePowerAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.ReactivePowerAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.ApparentPowerAvg);	
	EepromAddrInit += MEASURES_SIZE;
	EEPROM.get(EepromAddrInit, Measures.MaxMinAvg.PowerFactorAvg);		
}

static void LoadReleSatus()
{
	int EepromAddrInit = RELE_STATUS_START_ADDR;
	uint8_t Status = 0;
	Status = EEPROM.read(EepromAddrInit);
	SwichReleStatus(RELE_1, Status);
	EepromAddrInit += RELE_STATUS_SIZE;
	Status = EEPROM.read(EepromAddrInit);
	SwichReleStatus(RELE_2, Status);
	EepromAddrInit += RELE_STATUS_SIZE;
	Status = EEPROM.read(EepromAddrInit);
	SwichReleStatus(RELE_3, Status);
	EepromAddrInit += RELE_STATUS_SIZE;
	Status = EEPROM.read(EepromAddrInit);
	SwichReleStatus(RELE_4, Status);	
	
}

static void LoadReleStatistics()
{
	int EepromAddrInit = RELE_STAT_START_ADDR;
	EEPROM.get(EepromAddrInit, ReleStatistics[RELE_1].NSwitches);
	EepromAddrInit += RELE_OCCUR_SIZE;
	EEPROM.get(EepromAddrInit, ReleStatistics[RELE_2].NSwitches);
	EepromAddrInit += RELE_OCCUR_SIZE;
	EEPROM.get(EepromAddrInit, ReleStatistics[RELE_3].NSwitches);
	EepromAddrInit += RELE_OCCUR_SIZE;
	EEPROM.get(EepromAddrInit, ReleStatistics[RELE_4].NSwitches);	
	EepromAddrInit += RELE_OCCUR_SIZE;
	EEPROM.get(EepromAddrInit, ReleStatistics[RELE_1].PowerOnTime);
	EepromAddrInit += RELE_POWERON_SIZE;
	EEPROM.get(EepromAddrInit, ReleStatistics[RELE_2].PowerOnTime);
	EepromAddrInit += RELE_POWERON_SIZE;
	EEPROM.get(EepromAddrInit, ReleStatistics[RELE_3].PowerOnTime);
	EepromAddrInit += RELE_POWERON_SIZE;
	EEPROM.get(EepromAddrInit, ReleStatistics[RELE_4].PowerOnTime);	
}


void EepromInit()
{
	EEPROM.begin(512);
	LoadEnergies();
	LoadMaxMinAvg();
	LoadReleSatus();
	LoadReleStatistics();
}

void TaskEeprom()
{
	if(EepromSaveTimer.hasPassed(TIMER_EEPROM_SAVE(5), true))
	{
		SaveEnergies();
		SaveMaxMinAvg();
		SaveReleSatus();
		SaveReleStatistics();
		EEPROM.commit();
	}
}