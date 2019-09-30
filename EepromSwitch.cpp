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

#define ENERGIES_CHECK_ADDR			     1
#define ENERGIES_START_ADDR			     2

#define MAX_MIN_AVG_CHECK_ADDR 			51
#define MAX_MIN_AVG_START_ADDR    	    52

#define RELE_STATUS_CHECK_ADDR         199
#define RELE_STATUS_START_ADDR    	   200

#define RELE_STAT_CHECK_ADD			   204
#define RELE_STAT_START_ADDR 		   205

#define ALARMS_OCCUR_CHECK_ADDR		   249
#define ALARMS_OCCUR_START_ADDR		   250

#define SETUP_PARAMS_CHECK_ADDR		   299
#define SETUP_PARAMS_ADDR			   300

#define TIMER_EEPROM_SAVE(Min)         (Min * 60)

#define DFLT_WIFI_STATUS	         ABILITATO
#define DFLT_SAVE_DELAY	  			 15
#define DFLT_AVG_MEASURE_PERIOD	     60
#define DFLT_SIM					 DISABILITATO		   
#define DFLT_I_HIGH_THR              2
#define DFLT_I_LOW_THR		         2
#define DFLT_P_ATT_HIGH_THR  	     2
#define DFLT_P_REA_HIGH_THR   		 2
#define DFLT_P_APP_HIGH_THR  		 2
#define DFLT_PF_THR  	   			 2
  	   







Chrono  EepromSaveTimer(Chrono::SECONDS);

bool SaveAccomplished;

uint16_t EepParamsValue[MAX_SETUP_ITEMS] = 
{
	ABILITATO		    ,		
	15			        ,
	60			        ,
	ABILITATO		    ,
	DFLT_I_HIGH_THR     ,  
	DFLT_I_LOW_THR		,
	DFLT_P_ATT_HIGH_THR ,
	DFLT_P_REA_HIGH_THR , 
	DFLT_P_APP_HIGH_THR ,
	DFLT_PF_THR  	   	,
	DFLT_PF_THR         ,
};

uint8_t ReleInitStatus[N_RELE];

const uint16_t DfltParamValues[MAX_SETUP_ITEMS]
{
	DFLT_WIFI_STATUS,
	DFLT_SAVE_DELAY	,
	DFLT_AVG_MEASURE_PERIOD,
	DFLT_SIM,			
	DFLT_I_HIGH_THR,     
	DFLT_I_LOW_THR,		
	DFLT_P_ATT_HIGH_THR, 
	DFLT_P_REA_HIGH_THR, 
	DFLT_P_APP_HIGH_THR, 
	DFLT_PF_THR,	
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
		EEPROM.put(EepromAddrInit, EepParamsValue[i]);
		EepromAddrInit += SETUP_PARAM_VALUE_SIZE;
	}
	EEPROM.commit();
}

static void LoadEnergies()
{
	int EepromAddrInit = ENERGIES_START_ADDR;
	if(EEPROM.read(ENERGIES_CHECK_ADDR) != 1)
	{
		EEPROM.write(ENERGIES_CHECK_ADDR, 1);
		EEPROM.put(ENERGIES_START_ADDR, Measures.Energies);
		EEPROM.commit();
	}
	EEPROM.get(ENERGIES_START_ADDR, Measures.Energies);
}

static void LoadMaxMinAvg()
{
	int EepromAddrInit = MAX_MIN_AVG_START_ADDR;
	if(EEPROM.read(MAX_MIN_AVG_CHECK_ADDR) != 1)
	{
		EEPROM.write(MAX_MIN_AVG_CHECK_ADDR, 1);
		EEPROM.put(MAX_MIN_AVG_START_ADDR, Measures.MaxMinAvg);
		EEPROM.commit();
	}
	EEPROM.get(MAX_MIN_AVG_START_ADDR, Measures.MaxMinAvg);
}

static void LoadReleSatus()
{
	int EepromAddrInit = RELE_STATUS_START_ADDR;
	if(EEPROM.read(RELE_STATUS_CHECK_ADDR) != 1)
	{
		EEPROM.write(RELE_STATUS_CHECK_ADDR, 1);
		for(int i = 0; i < N_RELE; i++)
		{
			EEPROM.put(EepromAddrInit, STATUS_OFF);
			EepromAddrInit += RELE_STATUS_SIZE;		
		}	
		EEPROM.commit();
	}	
	for(int i = 0; i < N_RELE; i++)
	{
		uint8_t Status = 0;
		EEPROM.get(EepromAddrInit, Status);
		ReleInitStatus[i] = Status;
		EepromAddrInit += RELE_STATUS_SIZE;		
	}	
	
}

static void LoadReleStatistics()
{
	int EepromAddrInit = RELE_STAT_START_ADDR;
	if(EEPROM.read(RELE_STAT_CHECK_ADD) != 1)
	{
		EEPROM.write(RELE_STAT_CHECK_ADD, 1);
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
		EEPROM.commit();
	}
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
	if(EEPROM.read(ALARMS_OCCUR_CHECK_ADDR) != 1)
	{
		EEPROM.write(ALARMS_OCCUR_CHECK_ADDR, 1);
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
		EEPROM.commit();
	}
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
	if(EEPROM.read(SETUP_PARAMS_CHECK_ADDR) != MAX_SETUP_ITEMS)
	{
		for(int i = 0; i < MAX_SETUP_ITEMS; i++)
		{
			EEPROM.put(EepromAddrInit, DfltParamValues[i]);
			EepromAddrInit += SETUP_PARAM_VALUE_SIZE;
		}	
		EEPROM.write(SETUP_PARAMS_CHECK_ADDR, MAX_SETUP_ITEMS);	
		EEPROM.commit();
	}
	for(int i = 0; i < MAX_SETUP_ITEMS; i++)
	{
		EEPROM.get(EepromAddrInit, EepParamsValue[i]);
		EepromAddrInit += SETUP_PARAM_VALUE_SIZE;
	}
}


void ResetDfltEepParams()
{
	EEPROM.write(SETUP_PARAMS_CHECK_ADDR, 0);
	EEPROM.commit();
}

void ResetEepEnergies()
{
	EEPROM.write(ENERGIES_CHECK_ADDR, 0);
	EEPROM.commit();
}

void ResetEepMaxMinAvg()
{
	EEPROM.write(MAX_MIN_AVG_CHECK_ADDR, 0);
	EEPROM.commit();
}

void ResetEepAlarms()
{
	EEPROM.write(ALARMS_OCCUR_CHECK_ADDR, 0);
	EEPROM.commit();
}

void ResetEepReleStatistics()
{
	EEPROM.write(RELE_STAT_CHECK_ADD, 0);
	EEPROM.commit();
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
		LoadReleStatistics();
		LoadAlarmsOccurence();
		LoadParameters();
	}
}

void TaskEeprom()
{
	if(EepromSaveTimer.hasPassed(TIMER_EEPROM_SAVE(EepParamsValue[EEPROM_SAVE_DELAY]), true))
	{
		// SaveEnergies();
		// SaveMaxMinAvg();
		SaveReleSatus();
		SaveReleStatistics();
		SaveAlarmsOccurence();
		
		EEPROM.commit();
		SaveAccomplished = true;
	}

}