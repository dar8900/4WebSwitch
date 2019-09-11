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
	float      LowThr;
	float	   HighThr;
	double 	   *AssociatedMeasure;
	bool 	   IsActive;
	bool 	   IsEnabled;
	bool 	   EnableDisconnection;
	uint32_t   Occurences;
	uint8_t    WichThr;
}ALARM_S;

extern bool AlarmActive;
extern ALARM_S Alarms[MAX_ALARM];
extern const char *AlarmsName[MAX_ALARM];
extern const char *UnderThrAlarmMessage[MAX_ALARM];
extern const char *OverThrAlarmMessage[MAX_ALARM];

void TaskAlarm(void);

#endif