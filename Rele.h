#ifndef RELE_H
#define RELE_H
#include <Arduino.h>
#include <ReleLib.h>


typedef struct
{
	uint8_t   OldStatus;
	uint32_t  PowerOnTime;
	uint16_t  NSwitches;
}RELE_INFO_S;

extern RELE_LIB   Rele;
extern RELE_INFO_S ReleStatistics[N_RELE];



void ReleInit(void);
void TurnOnRele(uint8_t WichRele);
void TurnOffRele(uint8_t WichRele);
void TurnAllRele(uint8_t Status);
void ToggleRele(uint8_t WichRele);
void SwichReleStatus(uint8_t WichRele, uint8_t Status);

void RefreshReleStatistics(void);

#endif