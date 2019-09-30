#include "4WebSwitch.h"
#include "Alarms.h"
#include "Measure.h"
#include "Display.h"
#include "Rele.h"
#include "Web.h"
#include "EepromSwitch.h"



Chrono AlarmInsertionDelay(Chrono::SECONDS), AlarmDisinsertionDelay(Chrono::SECONDS), AlarmReleDisconnectDelay(Chrono::SECONDS);

ALARM_S Alarms[MAX_ALARM] = 
{
	{	0.0,		  3.0, 	 &Measures.CurrentRMS	  ,    false, 	false,	 false ,	0, 	NO_THR, 0},
	{	0.0,		230.0, 	 &Measures.VoltageRMS	  ,    false, 	false,	 false ,	0, 	NO_THR, 0},
	{	0.0,	   2000.0, 	 &Measures.ActivePower	  ,    false, 	false,	 false ,	0, 	NO_THR, 0},
	{ -50.0,		 50.0, 	 &Measures.ReactivePower  ,    false, 	false,	 false ,	0, 	NO_THR, 0},
	{	0.0,	   3000.0, 	 &Measures.ApparentPower  ,    false, 	false,	 false ,	0, 	NO_THR, 0},
	{-0.980,		0.980, 	 &Measures.PowerFactor	  ,    false, 	false,	 false ,	0, 	NO_THR, 0},
};

const char *AlarmsName[MAX_ALARM] = 
{
	"Corrente"         ,
	"Tensione"         ,
	"Potenza attiva"   ,
	"Potenza reattiva" ,
	"Potenza apparente",
	"Power factor"	   ,
};

const char *UnderThrAlarmMessage[MAX_ALARM] = 
{
	"Sotto corrente"           ,
	"Sotto tensione"		   ,
	"Potenza att. sotto soglia",
	"Potenza rea. sotto soglia",
	"Potenza app. sotto soglia",
	"PF sotto soglia"		   ,
};

const char *OverThrAlarmMessage[MAX_ALARM] = 
{
	"Sovra corrente"           ,
	"Sovra tensione"		   ,
	"Potenza att. sopra soglia",
	"Potenza rea. sopra soglia",
	"Potenza app. sopra soglia",
	"PF sopra soglia"		   ,
};


bool AlarmActive, ReleAlreadyDisconnected;

void AssignAlarmsThr(uint16_t Thr, uint8_t WichParam)
{
	double AlarmThr = (double)Thr;
	switch(WichParam)
	{
		case CURRENT_HIGH_THR:
			AlarmThr /= 100;
			Alarms[CURRENT].HighThr = AlarmThr;
			break;
		case CURRENT_LOW_THR:
			AlarmThr /= 100;
			Alarms[CURRENT].LowThr = AlarmThr;
			break;
		case ACTIVE_POWER_HIGH_THR:
			AlarmThr *= 100;
			Alarms[ACTIVE_POWER].HighThr = AlarmThr;
			break;
		case REACTIVE_POWER_HIGH_THR:
			AlarmThr *= 10;
			Alarms[REACTIVE_POWER].HighThr = AlarmThr;
			break;
		case APPARENT_POWER_HIGH_THR:
			AlarmThr *= 100;
			Alarms[APPARENT_POWER].HighThr = AlarmThr;
			break;
		case PF_HIGH_THR:
			AlarmThr /= 100;
			Alarms[PF].HighThr = AlarmThr;
			break;
		case PF_LOW_THR:
			AlarmThr /= 100;
			Alarms[PF].LowThr = -AlarmThr;
			break;
		default:
			break;
	}
}

void AlarmThrInit()
{
	for(int i = CURRENT_HIGH_THR; i <= PF_LOW_THR; i++)
	{
		AssignAlarmsThr(SetupParams[i].EnumList[EepParamsValue[i]].EnumValue, i);	
	}
}

static void ControlAlarmsThr()
{
	int AlarmIndex = 0;
	double ActualMeasure = 0.0;
	for(AlarmIndex = 0; AlarmIndex < MAX_ALARM; AlarmIndex++)
	{
		ActualMeasure = *Alarms[AlarmIndex].AssociatedMeasure;
		if(AlarmIndex == PF && ActualMeasure == INVALID_PF_VALUE)
			continue;
		if(Alarms[AlarmIndex].IsEnabled)
		{
			if(!Alarms[AlarmIndex].IsActive)
			{
				if(ActualMeasure >= Alarms[AlarmIndex].HighThr)
				{
					Alarms[AlarmIndex].IsActive = true;
					Alarms[AlarmIndex].WichThr = OVER_THR;	
					Alarms[AlarmIndex].Occurences++;
					Alarms[AlarmIndex].AlarmTime = TimeInSecond;
				}
				if(ActualMeasure <= Alarms[AlarmIndex].LowThr)
				{
					Alarms[AlarmIndex].IsActive = true;
					Alarms[AlarmIndex].WichThr = UNDER_THR;	
					Alarms[AlarmIndex].Occurences++;
					Alarms[AlarmIndex].AlarmTime = TimeInSecond;
				}			
			}
			else
			{
				if(ActualMeasure > Alarms[AlarmIndex].LowThr && ActualMeasure < Alarms[AlarmIndex].HighThr)
				{
					Alarms[AlarmIndex].IsActive = false;
					Alarms[AlarmIndex].WichThr = NO_THR;
				}
				else
				{
					if((ActualMeasure <= Alarms[AlarmIndex].LowThr && Alarms[AlarmIndex].WichThr == OVER_THR) ||
						(ActualMeasure >= Alarms[AlarmIndex].HighThr && Alarms[AlarmIndex].WichThr == UNDER_THR))
						Alarms[AlarmIndex].IsActive = false;			
				}
			}
		}
		else
		{
			Alarms[AlarmIndex].IsActive = false;
			Alarms[AlarmIndex].WichThr = NO_THR;
		}
	}
}


static void AlarmsReleDisconnect()
{
	for(int AlarmIndex = 0; AlarmIndex < MAX_ALARM; AlarmIndex++)
	{
		if(Alarms[AlarmIndex].EnableDisconnection)
		{
			if(Alarms[AlarmIndex].IsActive && Alarms[AlarmIndex].WichThr == OVER_THR && 
				AlarmReleDisconnectDelay.hasPassed(5, true) && !ReleAlreadyDisconnected)
			{
				ReleAlreadyDisconnected = true;
				TurnAllRele(STATUS_OFF);
			}
		}
		else
			AlarmReleDisconnectDelay.restart();
	}
}


static bool CheckAlarms()
{
	int i = 0;
	for(i = 0; i < MAX_ALARM; i++)
	{
		if(Alarms[i].IsActive)
			return true;
	}
	return false;
}

void ResetAlarms()
{
	for(int i = 0; i < MAX_ALARM; i++)
	{
		Alarms[i].Occurences = 0;
		Alarms[i].AlarmTime = 0;
		if(Alarms[i].IsActive)
			Alarms[i].IsActive = false;
	}
	AlarmActive = false;
}

void TaskAlarm()
{
	ControlAlarmsThr();
	AlarmsReleDisconnect();
	if(CheckAlarms())
	{
		if(AlarmInsertionDelay.hasPassed(5, true) && !AlarmActive)
			AlarmActive = true;
		else if(AlarmActive)
			AlarmInsertionDelay.restart();
		AlarmDisinsertionDelay.restart();
	}
	else
	{
		AlarmInsertionDelay.restart();
		if(AlarmDisinsertionDelay.hasPassed(5, true) && AlarmActive)
			AlarmActive = false;
		else if(!AlarmActive)
			AlarmDisinsertionDelay.restart();
	}
}

