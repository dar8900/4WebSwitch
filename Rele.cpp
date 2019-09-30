#include "4WebSwitch.h"
#include "Rele.h"
#include <ShiftyReg.h>
#include "EepromSwitch.h"

#define N_OF_SHIFT_REG	1

#undef TEST_RELE_EXIT

SHIFTY_REG ShiftReg;
RELE_LIB   Rele;

RELE_INFO_S ReleStatistics[N_RELE];
Chrono PowerOnTimeTimer[N_RELE];

void ReleInit()
{
	Rele.begin(N_RELE);
	ShiftReg.begin(DATAPIN, CLOCKPIN, LATCHPIN, OUTPUTENABLEPIN, CLEARREGPIN, N_RELE, N_OF_SHIFT_REG);	

	TurnAllRele(STATUS_OFF);
	ShiftReg.loadAllExit(STATUS_OFF);
	
	for(int ReleIndex = 0; ReleIndex < N_RELE; ReleIndex++)
	{
		ReleStatistics[ReleIndex].OldStatus = ReleInitStatus[ReleIndex];
		SwichReleStatus(ReleIndex, ReleInitStatus[ReleIndex]);
		delay(50);
	}
#ifdef TEST_RELE_EXIT	
	for(int ReleIndex = 0; ReleIndex < N_RELE * 2; ReleIndex++)
	{
		ToggleRele(ReleIndex % N_RELE);
		delay(150);
	}
#endif
}


// void TurnOnRele(uint8_t WichRele)
// {
	// Rele.setReleStatus(WichRele, STATUS_ON);
	// ShiftReg.setSingleExit(WichRele, STATUS_ON);
	// ShiftReg.loadAllExit();
// }

// void TurnOffRele(uint8_t WichRele)
// {
	// Rele.setReleStatus(WichRele, STATUS_OFF);
	// ShiftReg.setSingleExit(WichRele, STATUS_OFF);
	// ShiftReg.loadAllExit();	
// }

void SwichReleStatus(uint8_t WichRele, uint8_t Status)
{
	if(Status != STATUS_OFF && Status != STATUS_ON)
	{
		return;
	}
	if(Rele.getReleStatus(WichRele) != Status)
	{
		Rele.setReleStatus(WichRele, Status);
		ShiftReg.setSingleExit(WichRele, Status);
		ShiftReg.loadAllExit();	
	}	
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

void RefreshReleStatistics()
{
	for(int ReleIndex = 0; ReleIndex < N_RELE; ReleIndex++)
	{
		uint8_t ActualStatus = Rele.getReleStatus(ReleIndex);
		if(ReleStatistics[ReleIndex].OldStatus != ActualStatus)
		{
			ReleStatistics[ReleIndex].OldStatus = ActualStatus;
			if(ActualStatus == STATUS_ON)
			{
				ReleStatistics[ReleIndex].NSwitches++;
			}
		}
		if(ActualStatus == STATUS_ON)
		{
			if(PowerOnTimeTimer[ReleIndex].hasPassed(1000, true))
				ReleStatistics[ReleIndex].PowerOnTime++;
		}
		else
			PowerOnTimeTimer[ReleIndex].restart();
	}
}

void ResetReleStatistics()
{
	for(int i = 0; i < N_RELE; i++)
	{
		ReleStatistics[i].NSwitches = 0;
		ReleStatistics[i].PowerOnTime = 0;		
	}
}
