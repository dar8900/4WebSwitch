#ifndef MEASURE_H
#define MEASURE_H
#include <math.h>

#define INVALID_PF_VALUE 	2.0

#define RESET_TOTAL_ENERGIES	true
#define RESET_PARTIAL_ENERGIES	false

typedef struct
{
	double   MaxCurrent;
	double 	 MinCurrent;
	double   MaxVoltage;
	double 	 MinVoltage;
	double   MaxActivePower;
	double 	 MinActivePower;
	double   MaxReactivePower;
	double 	 MinReactivePower;
	double   MaxApparentPower;
	double 	 MinApparentPower;
	double   MaxPowerFactor;
	double 	 MinPowerFactor;
	double 	 CurrentAvg;
	double   VoltageAvg;
	double   ActivePowerAvg;
	double   ReactivePowerAvg;
	double   ApparentPowerAvg;
	double   PowerFactorAvg;
}MAX_MIN_AVG_MEASURE;


typedef struct
{
	double   CurrentRMS;
	double   VoltageRMS;
    double   ApparentPower;
	double   ActivePower;
	double   ReactivePower;
    double   ApparentEnergy;
	double   ActiveEnergy;
	double   ReactiveEnergy;
    double   PartialApparentEnergy;
	double   PartialActiveEnergy;
	double   PartialReactiveEnergy;
    double   PowerFactor;
	MAX_MIN_AVG_MEASURE MaxMinAvg;
}GENERAL_MEASURES;

extern bool EnableSimulation;

extern GENERAL_MEASURES Measures;

void AnalogBegin(void);
void ResetTotalEnergy(void);
void ResetPartialEnergy(void);
void ResetMaxMin(void);
void ResetAvg(void);
void TaskMeasure(void);





#endif