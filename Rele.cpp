#include "4WebSwitch.h"
#include "Rele.h"
#include <ShiftyReg.h>

#define N_OF_SHIFT_REG	1

#define TEST_RELE_EXIT

static SHIFTY_REG ShiftReg;
RELE_LIB   Rele;

void ReleInit()
{
	Rele.begin(N_RELE);
	ShiftReg.begin(DATAPIN, CLOCKPIN, LATCHPIN, OUTPUTENABLEPIN, CLEARREGPIN, N_RELE, N_OF_SHIFT_REG);	
	TurnAllRele(STATUS_OFF);
	ShiftReg.loadAllExit(STATUS_OFF);	
}


void TurnOnRele(uint8_t WichRele)
{
	Rele.setReleStatus(WichRele, STATUS_ON);
	ShiftReg.setSingleExit(WichRele, STATUS_ON);
	ShiftReg.loadAllExit();
}

void TurnOffRele(uint8_t WichRele)
{
	Rele.setReleStatus(WichRele, STATUS_OFF);
	ShiftReg.setSingleExit(WichRele, STATUS_OFF);
	ShiftReg.loadAllExit();	
}

void TurnAllRele(uint8_t Status)
{
	if(Status != STATUS_OFF && Status != STATUS_ON)
	{
		return;
	}
	Rele.turnAllRele(Status);
	ShiftReg.loadAllExit(Status);	
}

void ToggleRele(uint8_t WichRele)
{
	if(Rele.getReleStatus(WichRele) == STATUS_OFF)
	{
		Rele.setReleStatus(WichRele, STATUS_ON);
		ShiftReg.setSingleExit(WichRele, STATUS_ON);
		ShiftReg.loadAllExit();			
	}
	else
	{
		Rele.setReleStatus(WichRele, STATUS_OFF);
		ShiftReg.setSingleExit(WichRele, STATUS_OFF);
		ShiftReg.loadAllExit();	
	}
}

