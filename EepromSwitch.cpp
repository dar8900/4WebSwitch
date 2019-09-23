#include "4WebSwitch.h"
#include "Measure.h"
#include "Rele.h"
#include "EepromSwitch.h"
#include "Alarms.h"
#include "Display.h"
#include <EEPROM.h>

#define NODE_MCU_EEPROM_SIZE	         512

#define MEASURES_SIZE					 sizeof(double)
#define RELE_STATUS_SIZE			     1
#define RELE_OCCUR_SIZE					 2
#define RELE_POWERON_SIZE				 sizeof(int)
#define ALARMS_OCCUR_SIZE				 sizeof(int)
#define SETUP_PARAM_VALUE_SIZE		     2

#define FIRST_CHECK_VAR_ADDR		     0
#define ENERGIES_START_ADDR			     1
#define MAX_MIN_AVG_START_ADDR    	    51

#define RELE_STATUS_START_ADDR    	   200
#define RELE_STAT_START_ADDR 		   204

#define ALARMS_OCCUR_START_ADDR		   250

#define SETUP_PARAMS_ADDR			   300

#define TIMER_EEPROM_SAVE(Min)         (Min * 60)



Chrono  EepromSaveTimer(Chrono::SECONDS);

bool SaveAccomplished;

const uint16_t DfltParamValues[MAX_SETUP_ITEMS]
{
	DFLT_WIFI_STATUS,
	DFLT_SAVE_DELAY	,
	DFLT_AVG_MEASURE_PERIOD,
};


static bool IsEmpty = false;

static void SaveEnergies()
{
	int EepromAddrInit = ENERGIES_START_ADDR;
	EEPROM.put(ENERGIES_START_ADDR, Measures.Energies);

}

static void SaveMaxMinAvg()
{
	int EepromAddrInit = MAX_MIN_AVG_START_ADDR;
	EEPROM.put(MAX_MIN_AVG_START_ADDR, Measures.MaxMinAvg);
}

static void SaveReleSatus()
{
	int EepromAddrInit = RELE_STATUS_START_ADDR;
	for(int i = 0; i < N_RELE; i++)
	{
		EEPROM.put(EepromAddrInit, Rele.getReleStatus(i));
		EepromAddrInit += RELE_STATUS_SIZE;		
	}
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

void SaveParameters()
{
	int EepromAddrInit = SETUP_PARAMS_ADDR;
	for(int i = 0; i < MAX_SETUP_ITEMS; i++)
	{
		EEPROM.put(EepromAddrInit, SetupParams[i].Value);
		EepromAddrInit += SETUP_PARAM_VALUE_SIZE;
	}
	EEPROM.commit();
}

static void LoadEnergies()
{
	int EepromAddrInit = ENERGIES_START_ADDR;
	EEPROM.get(ENERGIES_START_ADDR, Measures.Energies);
}

static void LoadMaxMinAvg()
{
	int EepromAddrInit = MAX_MIN_AVG_START_ADDR;
	EEPROM.get(MAX_MIN_AVG_START_ADDR, Measures.MaxMinAvg);
}

static void LoadReleSatus()
{
	int EepromAddrInit = RELE_STATUS_START_ADDR;
	for(int i = 0; i < N_RELE; i++)
	{
		uint8_t Status = 0;
		EEPROM.get(EepromAddrInit, Status);
		SwichReleStatus(i, Status);
		EepromAddrInit += RELE_STATUS_SIZE;		
		delay(1);
	}	
	
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

static void LoadParameters()
{
	int EepromAddrInit = SETUP_PARAMS_ADDR;
	for(int i = 0; i < MAX_SETUP_ITEMS; i++)
	{
		EEPROM.get(EepromAddrInit, SetupParams[i].Value);
		EepromAddrInit += SETUP_PARAM_VALUE_SIZE;
	}
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
		// LoadEnergies();
		// LoadMaxMinAvg();
		LoadReleSatus();
		// LoadReleStatistics();
		// LoadAlarmsOccurence();
		LoadParameters();
	}
}

void TaskEeprom()
{
	if(EepromSaveTimer.hasPassed(TIMER_EEPROM_SAVE(SetupParams[EEPROM_SAVE_DELAY].Value), true))
	{
		// SaveEnergies();
		// SaveMaxMinAvg();
		SaveReleSatus();
		// SaveReleStatistics();
		// SaveAlarmsOccurence();
		
		EEPROM.commit();
		SaveAccomplished = true;
	}

}