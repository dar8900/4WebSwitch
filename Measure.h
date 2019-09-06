#ifndef MEASURE_H
#define MEASURE_H
#include <math.h>

#define INVALID_PF_VALUE 	2.0

#define RESET_TOTAL_ENERGIES	true
#define RESET_PARTIAL_ENERGIES	false

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
}GENERAL_MEASURES;

extern GENERAL_MEASURES Measures;

void AnalogBegin(void);
void ResetEnergy(bool TotalOrPartial);
void TaskMeasure(void);





#endif