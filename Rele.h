#ifndef RELE_H
#define RELE_H
#include <Arduino.h>
#include <ReleLib.h>


extern RELE_LIB   Rele;



void ReleInit(void);
void TurnOnRele(uint8_t WichRele);
void TurnOffRele(uint8_t WichRele);
void TurnAllRele(uint8_t Status);
void ToggleRele(uint8_t WichRele);



#endif