#ifndef ALARMS_H
#define ALARMS_H


typedef enum
{
	CURRENT = 0,
	VOLTAGE,
	ACTIVE_POWER,
	REACTIVE_POWER,
	APPARENT_POWER,
	PF,
	MAX_ALARM	
}ALARM_NUMBER;

enum
{
	UNDER_THR = 0,
	OVER_THR,
	NO_THR
};

typedef struct
{
	const char *AlarmName;
	float      LowThr;
	float	   HighThr;
	double 	   *AssociatedMeasure;
	bool 	   IsActive;
	bool 	   IsEnabled;
	uint32_t   Occurences;
	uint8_t    WichThr;
	const char *UnderThrAlarmMessage;
	const char *OverThrAlarmMessage;
}ALARM_S;

#endif