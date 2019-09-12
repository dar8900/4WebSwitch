#include "4WebSwitch.h"
#include "Measure.h"
#include "Rele.h"
#include "EepromSwitch.h"
#include "Alarms.h"
#include <EEPROM.h>

#define NODE_MCU_EEPROM_SIZE	       512

#define MEASURES_SIZE					 sizeof(double)
#define RELE_STATUS_SIZE			     1
#define RELE_OCCUR_SIZE					 2
#define RELE_POWERON_SIZE				 sizeof(int)
#define ALARMS_OCCUR_SIZE				 sizeof(int)

#define FIRST_CHECK_VAR_ADDR		     0
#define ENERGIES_START_ADDR			     1
#define MAX_MIN_AVG_START_ADDR    	    51

#define RELE_STATUS_START_ADDR    	   200
#define RELE_STAT_START_ADDR 		   204

#define ALARMS_OCCUR_START_ADDR		   250



#define TIMER_EEPROM_SAVE(Min)         (Min * 60)

Chrono  EepromSaveTimer(Chrono::SECONDS);

static bool IsEmpty = false;

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

static void SaveAlarmsOccurence()
{
	int EepromAddrInit = ALARMS_OCCUR_START_ADDR;
	EEPROM.put(EepromAddrInit, Alarms[CURRENT].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;	
	EEPROM.put(EepromAddrInit, Alarms[VOLTAGE].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;
	EEPROM.put(EepromAddrInit, Alarms[ACTIVE_POWER].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;
	EEPROM.put(EepromAddrInit, Alarms[REACTIVE_POWER].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;
	EEPROM.put(EepromAddrInit, Alarms[APPARENT_POWER].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;
	EEPROM.put(EepromAddrInit, Alarms[PF].Occurences);
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

static void LoadAlarmsOccurence()
{
	int EepromAddrInit = ALARMS_OCCUR_START_ADDR;
	EEPROM.get(EepromAddrInit, Alarms[CURRENT].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;	
	EEPROM.get(EepromAddrInit, Alarms[VOLTAGE].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;
	EEPROM.get(EepromAddrInit, Alarms[ACTIVE_POWER].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;
	EEPROM.get(EepromAddrInit, Alarms[REACTIVE_POWER].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;
	EEPROM.get(EepromAddrInit, Alarms[APPARENT_POWER].Occurences);
	EepromAddrInit += ALARMS_OCCUR_SIZE;
	EEPROM.get(EepromAddrInit, Alarms[PF].Occurences);
}


static void CheckFirstGo()
{
	if(EEPROM.read(FIRST_CHECK_VAR_ADDR) == 1)
		return;
	else
		IsEmpty = true;
	DBG("Memoria non inizializzata");
	delay(1000);
	for(int i = 1; i < NODE_MCU_EEPROM_SIZE; i++)
		EEPROM.write(i, 0);
	EEPROM.write(FIRST_CHECK_VAR_ADDR, 1);
	EEPROM.commit();
	DBG("Memoria inizializzata e pronta");
	delay(1000);	
}

void EepromInit()
{
	EEPROM.begin(NODE_MCU_EEPROM_SIZE);
	CheckFirstGo();
	if(!IsEmpty)
	{
		LoadEnergies();
		LoadMaxMinAvg();
		LoadReleSatus();
		LoadReleStatistics();
		LoadAlarmsOccurence();
	}
}

void TaskEeprom()
{
	if(EepromSaveTimer.hasPassed(TIMER_EEPROM_SAVE(15), true))
	{
		SaveEnergies();
		SaveMaxMinAvg();
		SaveReleSatus();
		SaveReleStatistics();
		SaveAlarmsOccurence();
		EEPROM.commit();
	}
}