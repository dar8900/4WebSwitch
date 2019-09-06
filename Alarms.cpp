#include "4WebSwitch.h"
#include "Alarms.h"
#include "Measure.h"
#include "Display.h"

ALARM_S Alarms[MAX_ALARM] = 
{
	{"Corrente"         , 		0.0,		 14.0, 	 &Measures.CurrentRMS	    ,    false, 	false,		0, 	NO_THR, 	 "Sotto corrente"           ,    "Sovra corrente"           },
	{"Tensione"         , 		0.0,		230.0, 	 &Measures.VoltageRMS	    ,    false, 	false,		0, 	NO_THR, 	 "Sotto tensione"			,    "Sovra tensione"			},
	{"Potenza attiva"   , 		0.0,	   2000.0, 	 &Measures.ActivePower	    ,    false, 	false,		0, 	NO_THR, 	 "Potenza att. sotto soglia",    "Potenza att. sopra soglia"},
	{"Potenza reattiva" , 	  -50.0,		 50.0, 	 &Measures.ReactivePower	,    false, 	false,		0, 	NO_THR, 	 "Potenza rea. sotto soglia",    "Potenza rea. sopra soglia"},
	{"Potenza apparente", 		0.0,	   3000.0, 	 &Measures.ApparentPower	,    false, 	false,		0, 	NO_THR, 	 "Potenza app. sotto soglia",    "Potenza app. sopra soglia"},
	{"Power factor"		, 	 -0.980,		0.980, 	 &Measures.PowerFactor	    ,    false, 	false,		0, 	NO_THR, 	 "PF sotto soglia"			,    "PF sopra soglia"			},
};


static void ControlAlarmsThr()
{
	int AlarmIndex = 0;
	float ActualMeasure = 0.0;
	for(AlarmIndex = 0; AlarmIndex < MAX_ALARM; AlarmIndex++)
	{
		ActualMeasure = (float)*Alarms[AlarmIndex].AssociatedMeasure;
		if(Alarms[AlarmIndex].IsEnabled)
		{
			if(!Alarms[AlarmIndex].IsActive)
			{
				if(ActualMeasure >= Alarms[AlarmIndex].HighThr)
				{
					Alarms[AlarmIndex].IsActive = true;
					Alarms[AlarmIndex].WichThr = OVER_THR;		
					Alarms[AlarmIndex].Occurences++;
				}
				if(ActualMeasure <= Alarms[AlarmIndex].LowThr)
				{
					Alarms[AlarmIndex].IsActive = true;
					Alarms[AlarmIndex].WichThr = UNDER_THR;	
					Alarms[AlarmIndex].Occurences++;
				}			
			}
			else
			{
				if(ActualMeasure > Alarms[AlarmIndex].LowThr && ActualMeasure < Alarms[AlarmIndex].HighThr)
				{
					Alarms[AlarmIndex].IsActive = false;
					Alarms[AlarmIndex].WichThr = NO_THR;
				}
			}
		}
	}
}

bool CheckAlarms()
{
	int i = 0;
	for(i = 0; i < MAX_ALARM; i++)
	{
		if(Alarms[i].IsActive)
			return true;
	}
}