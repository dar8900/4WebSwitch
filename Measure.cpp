#include "4WebSwitch.h"
#include "Measure.h"
#include "ADS1115.h"
#include "EepromSwitch.h"
#include "Display.h"


#define SIM_WAVEFORMS

#ifdef SIM_WAVEFORMS

#define TO_RADIANTS(Angle)     (Angle*M_PI/180)

#define SIM_V_PEAK(V)	       (V * sqrt(2))
#define SIM_I_PEAK(I)	       (I * sqrt(2))

#define SIM_V_DELAY(Grad)      ((Grad * DEG_TO_RAD)
#define SIM_I_DELAY(Grad)	   ((Grad * DEG_TO_RAD)

#define SIM_FREQ(Frq)   	   (2 * PI * (Frq))

#define DFLT_PEAK_I			    5.0
#define DFLT_DELAY_I		    0.0

// #define REQUEST_DELAY
// #define REQUEST_PEAK

#endif


#define ADC_CURRENT_EXIT  			  ADS1115_MUX_P0_NG
#define ADC_VOLTAGE_EXIT			  ADS1115_MUX_P1_NG

#define ADC_SAMPLE	   	  			  120
#define MAX_SAMPLING_WINDOW		      8
#define ADC_SAMPLING_RATE			  32//(ADC_SAMPLE / MAX_SAMPLING_WINDOW)

#define BURDEN_R_VALUE	 			  47
#define VOLTAGE_VOLT_CORRECTION		  1.265
#define PEAK_V(V)					  (V * sqrt(2))

#define TARP_VOLTAGE				  200.0
#define TARP_CURRENT         	      0.07

#ifdef ARDUINO_BOARD_MCU
#define MIN_ANALOG_V_READ  0.0001875
#endif

#ifdef NODE_MCU
#define MIN_ANALOG_V_READ  0.00003125
#endif

ADS1115 AnalogBoard(ADS1115_DEFAULT_ADDRESS);

Chrono CalcEnergyTimer;
uint16_t OldEnableSim;


double   AdcCurrentValues[ADC_SAMPLING_RATE], AdcVoltageValues[ADC_SAMPLING_RATE], CurrentRmsAcc, VoltageRmsAcc, VoltageRmsAvgAcc, ActivePowerRmsAcc;
uint32_t EnergyAccumulatorCnt, AvgCounter;
double   ApparentEnergyAccumulator, ActiveEnergyAccumulator, ReactiveEnergyAccumulator;
double   CurrentAvgAcc, VoltageAvgAcc, ActivePowerAvgAcc, ReactivePowerAvgAcc, ApparentPowerAvgAcc, PowerFactorAvgAcc;
GENERAL_MEASURES Measures;

uint8_t  SamplingWindow = 0;

Chrono CalcAvgTimer(Chrono::SECONDS);

#ifdef SIM_WAVEFORMS
static double SimVoltageWave[ADC_SAMPLE];
static double SimCurrentWave[ADC_SAMPLE];
double UserPeakI = DFLT_PEAK_I, UserDelayI = DFLT_DELAY_I;
Chrono TimerPrintDBG(Chrono::SECONDS);

#endif


static void CalcMaxMinAvg()
{
	if(Measures.CurrentRMS > Measures.MaxMinAvg.MaxCurrent)
		Measures.MaxMinAvg.MaxCurrent = Measures.CurrentRMS;

	if(Measures.CurrentRMS < Measures.MaxMinAvg.MinCurrent && Measures.MaxMinAvg.MinCurrent != 0)
		Measures.MaxMinAvg.MinCurrent = Measures.CurrentRMS;
	else if(Measures.MaxMinAvg.MinCurrent == 0)
		Measures.MaxMinAvg.MinCurrent = Measures.CurrentRMS;

	if(Measures.VoltageRMS > Measures.MaxMinAvg.MaxVoltage)
		Measures.MaxMinAvg.MaxVoltage = Measures.VoltageRMS;

	if(Measures.VoltageRMS < Measures.MaxMinAvg.MinVoltage && Measures.MaxMinAvg.MinVoltage != 0)
		Measures.MaxMinAvg.MinVoltage = Measures.VoltageRMS;
	else if(Measures.MaxMinAvg.MinVoltage == 0)
		Measures.MaxMinAvg.MinVoltage = Measures.VoltageRMS;

	if(Measures.ActivePower > Measures.MaxMinAvg.MaxActivePower)
		Measures.MaxMinAvg.MaxActivePower = Measures.ActivePower;

	if(Measures.ActivePower < Measures.MaxMinAvg.MinActivePower && Measures.MaxMinAvg.MinActivePower != 0)
		Measures.MaxMinAvg.MinActivePower = Measures.ActivePower;
	else if(Measures.MaxMinAvg.MinActivePower == 0)
		Measures.MaxMinAvg.MinActivePower = Measures.ActivePower;

	if(Measures.ReactivePower > Measures.MaxMinAvg.MaxReactivePower)
		Measures.MaxMinAvg.MaxReactivePower = Measures.ReactivePower;

	if(Measures.ReactivePower < Measures.MaxMinAvg.MinReactivePower && Measures.MaxMinAvg.MinReactivePower != 0)
		Measures.MaxMinAvg.MinReactivePower = Measures.ReactivePower;
	else if(Measures.MaxMinAvg.MinReactivePower == 0)
		Measures.MaxMinAvg.MinReactivePower = Measures.ReactivePower;

	if(Measures.ApparentPower > Measures.MaxMinAvg.MaxApparentPower)
		Measures.MaxMinAvg.MaxApparentPower = Measures.ApparentPower;

	if(Measures.ApparentPower < Measures.MaxMinAvg.MinApparentPower && Measures.MaxMinAvg.MinApparentPower != 0)
		Measures.MaxMinAvg.MinApparentPower = Measures.ApparentPower;
	else if(Measures.MaxMinAvg.MinApparentPower == 0)
		Measures.MaxMinAvg.MinApparentPower = Measures.ApparentPower;

	if(Measures.PowerFactor > Measures.MaxMinAvg.MaxPowerFactor)
		Measures.MaxMinAvg.MaxPowerFactor = Measures.PowerFactor;

	if(Measures.PowerFactor < Measures.MaxMinAvg.MinPowerFactor && Measures.MaxMinAvg.MinPowerFactor != 0)
		Measures.MaxMinAvg.MinPowerFactor = Measures.PowerFactor;
	else if(Measures.MaxMinAvg.MinPowerFactor == 0)
		Measures.MaxMinAvg.MinPowerFactor = Measures.PowerFactor;

	CurrentAvgAcc += Measures.CurrentRMS;
	VoltageAvgAcc += Measures.VoltageRMS;
	ActivePowerAvgAcc += Measures.ActivePower;
	ReactivePowerAvgAcc += Measures.ReactivePower;
	ApparentPowerAvgAcc += Measures.ApparentPower;
	PowerFactorAvgAcc += Measures.PowerFactor;
	AvgCounter++;

	if(CalcAvgTimer.hasPassed(EepParamsValue[AVG_MEASURE_PERIOD], true))
	{
		if(AvgCounter != 0)
		{
			Measures.MaxMinAvg.CurrentAvg       = (CurrentAvgAcc / AvgCounter);
			Measures.MaxMinAvg.VoltageAvg       = (VoltageAvgAcc / AvgCounter);
			Measures.MaxMinAvg.ActivePowerAvg   = (ActivePowerAvgAcc / AvgCounter);
			Measures.MaxMinAvg.ReactivePowerAvg = (ReactivePowerAvgAcc / AvgCounter);
			Measures.MaxMinAvg.ApparentPowerAvg = (ApparentPowerAvgAcc / AvgCounter);
			Measures.MaxMinAvg.PowerFactorAvg   = (PowerFactorAvgAcc / AvgCounter);
		}
		CurrentAvgAcc = 0.0;
		VoltageAvgAcc = 0.0;
		ActivePowerAvgAcc = 0.0;
		ReactivePowerAvgAcc = 0.0;
		ApparentPowerAvgAcc = 0.0;
		PowerFactorAvgAcc = 0.0;
		AvgCounter = 0;
	}
}


#ifdef SIM_WAVEFORMS
static void CalcSimCurrentVoltage(bool First)
{
	int Sample = 0;
	double VRms = 0.0, IRms = 0.0, RealPower = 0.0;
	String DelayI = "", PeakI = "";

	for(Sample = 0; Sample < ADC_SAMPLE; Sample++)
	{
		SimVoltageWave[Sample] = SIM_V_PEAK(220)*sin(TO_RADIANTS(Sample * 2.88));
		SimCurrentWave[Sample] = SIM_I_PEAK(UserPeakI)*sin(TO_RADIANTS((Sample * 2.88)) - TO_RADIANTS(30));
		VRms += (SimVoltageWave[Sample] * SimVoltageWave[Sample]);
		IRms += (SimCurrentWave[Sample] * SimCurrentWave[Sample]);
		RealPower += (SimCurrentWave[Sample] * SimVoltageWave[Sample]);
	}
	RealPower /= ADC_SAMPLE;
	VRms /= ADC_SAMPLE;
	IRms /= ADC_SAMPLE;
	VRms = sqrt(VRms);
	IRms = sqrt(IRms);
	if(IRms <= 0.05)
		IRms = 0.0;
	if(VRms <= TARP_VOLTAGE)
		VRms = 0.0;
	Measures.ActivePower = RealPower;
	Measures.CurrentRMS = IRms;
	Measures.VoltageRMS = VRms;
	Measures.ApparentPower = Measures.CurrentRMS * Measures.VoltageRMS;
	Measures.ReactivePower = Measures.ApparentPower - Measures.ActivePower;
	if(Measures.ApparentPower != 0.0)
		Measures.PowerFactor = Measures.ActivePower / Measures.ApparentPower;
	else
		Measures.PowerFactor = INVALID_PF_VALUE;
	CalcMaxMinAvg();
	// if(TimerPrintDBG.hasPassed(5, true))
	// {
		// DBG("");
		// DBG("Tensione RMS simulata: " + String(Measures.VoltageRMS, 3) + "V");
		// DBG("Corrent RMS simulata: " + String(Measures.CurrentRMS, 3) + "A");
		// DBG("Potenza apparente simulata: " + String(Measures.ApparentPower, 3) + "VA");
		// DBG("Potenza attiva simulata: " + String(Measures.ActivePower, 3) + "W");
		// DBG("Potenza reattiva simulata: " + String(Measures.ReactivePower, 3) + "VAr");
		// DBG("PF simulato: " + String(Measures.PowerFactor, 3));
		// DBG("");
	// }
}
#endif

void AnalogBegin()
{
// #ifndef SIM_WAVEFORMS
	AnalogBoard.initialize();
	AnalogBoard.setMode(ADS1115_MODE_CONTINUOUS);
	AnalogBoard.setRate(ADS1115_RATE_860);
	AnalogBoard.setGain(ADS1115_MV_2P048);
// #else
	// CalcSimCurrentVoltage(true);
// #endif
}

static void CalcMeasure()
{
	int Sample = 0;
	double	VoltageMax = 0.0;
	AnalogBoard.setMultiplexer(ADC_VOLTAGE_EXIT);
	for(int i = 0 ; i < ADC_SAMPLING_RATE; i++)
	{
		AdcVoltageValues[i] = AnalogBoard.getVolts(true) - VOLTAGE_VOLT_CORRECTION;
		if(VoltageMax < AdcVoltageValues[i])
			VoltageMax = AdcVoltageValues[i];
		VoltageRmsAcc += (AdcVoltageValues[i] * AdcVoltageValues[i]);
		
		delayMicroseconds(1200);
		// AnalogBoard.setMultiplexer(ADC_CURRENT_EXIT);
		// delayMicroseconds(1200);
	}		
	VoltageRmsAcc /= ADC_SAMPLING_RATE;
	VoltageRmsAcc = sqrt(VoltageRmsAcc);
	if(VoltageMax > 0.1)
		VoltageRmsAvgAcc += ((VoltageRmsAcc * PEAK_V(220)) / VoltageMax);
	else 
		VoltageRmsAvgAcc += ((VoltageRmsAcc * PEAK_V(220)) / 1);
	VoltageRmsAcc = 0.0;
	SamplingWindow++;
	if(SamplingWindow == MAX_SAMPLING_WINDOW)
	{
		Measures.VoltageRMS = VoltageRmsAvgAcc / SamplingWindow;
		VoltageRmsAvgAcc = 0.0;
		SamplingWindow = 0; 
		if(Measures.VoltageRMS < TARP_VOLTAGE)
			Measures.VoltageRMS = 0.0;
		else
			Measures.VoltageRMS = floor(Measures.VoltageRMS * 1000) / 1000;
	}
	
	// bool ZeroVFound = false;
	// double ActivePowerTemp = 0.0;
	// ZeroVFound = SearchZeroV();
	// if(ZeroVFound)
	// {
		// // Tempo impiegato teoricamente 50ms
		// for(Sample = (SamplingWindow * ADC_SAMPLING_RATE); Sample < (ADC_SAMPLING_RATE + (SamplingWindow * ADC_SAMPLING_RATE)); Sample++)
		// {
			// AnalogBoard.setMultiplexer(ADC_VOLTAGE_EXIT);
			// AdcVoltageValues[Sample] = AnalogBoard.getVolts(true) - VOLTAGE_VOLT_CORRECTION;
			// delayMicroseconds(1200);
			
			// AnalogBoard.setMultiplexer(ADC_CURRENT_EXIT);
			// AdcCurrentValues[Sample] = AnalogBoard.getVolts(true);
			// delayMicroseconds(1200);
			// CurrentRmsAcc += (AdcCurrentValues[Sample] * AdcCurrentValues[Sample]);
			// VoltageRmsAcc += (AdcVoltageValues[Sample] * AdcVoltageValues[Sample]);
			// ActivePowerTemp = ((AdcCurrentValues[Sample] / BURDEN_R_VALUE * 1000) * (AdcVoltageValues[Sample] * PEAK_V(220.0) / VOLTAGE_VOLT_CORRECTION * 1.84));
			// ActivePowerRmsAcc += (ActivePowerTemp * ActivePowerTemp);
		// }
		// DBG("Zero V trovato");
		// SamplingWindow++;
		// if(SamplingWindow == MAX_SAMPLING_WINDOW)
		// {
			// DBG("Raggiunte le 5 finestre di campionamento");
			// SamplingWindow = 0;
			// CurrentRmsAcc /= ADC_SAMPLE;
			// CurrentRmsAcc = sqrt(CurrentRmsAcc);
			// VoltageRmsAcc /= ADC_SAMPLE;
			// VoltageRmsAcc = sqrt(VoltageRmsAcc);
			// ActivePowerRmsAcc /= ADC_SAMPLE;
			// ActivePowerRmsAcc = sqrt(ActivePowerRmsAcc);
			// Measures.ActivePower = ActivePowerRmsAcc;
			// ActivePowerRmsAcc = 0.0;
			// Measures.VoltageRMS = (VoltageRmsAcc * PEAK_V(220.0) / VOLTAGE_VOLT_CORRECTION  * 1.84);
			// DBG(Measures.VoltageRMS);
			// Measures.CurrentRMS = (CurrentRmsAcc / BURDEN_R_VALUE) * 1000;
			// CurrentRmsAcc = 0.0;
			// VoltageRmsAcc = 0.0;
			// if(Measures.CurrentRMS <= TARP_CURRENT || Measures.VoltageRMS <= TARP_VOLTAGE)
			// {
				// if(Measures.CurrentRMS <= TARP_CURRENT)
					// Measures.CurrentRMS = 0.0;
				// if(Measures.VoltageRMS <= TARP_VOLTAGE)
					// Measures.VoltageRMS = 0.0;
				// Measures.ActivePower = 0.0;
				// Measures.ApparentPower = 0.0;
				// Measures.ReactivePower = 0.0;
				// Measures.PowerFactor = INVALID_PF_VALUE;
			// }
			// else
			// {
				// Measures.ApparentPower = Measures.CurrentRMS * Measures.VoltageRMS;
				// Measures.ReactivePower = Measures.ApparentPower - Measures.ActivePower;
				// if(Measures.ApparentPower != 0.0)
					// Measures.PowerFactor = Measures.ActivePower / Measures.ApparentPower;
				// else
					// Measures.PowerFactor = INVALID_PF_VALUE;
			// }
		// }
	// }
	CalcMaxMinAvg();
}


static void CalcEnergy()
{
	ApparentEnergyAccumulator += Measures.ApparentPower;
	ActiveEnergyAccumulator   += Measures.ActivePower;
	ReactiveEnergyAccumulator += Measures.ReactivePower;
	EnergyAccumulatorCnt++;
	if(CalcEnergyTimer.hasPassed(1000, true))
	{
		if(EnergyAccumulatorCnt != 0)
		{
			Measures.Energies.ApparentEnergy 	   += ((ApparentEnergyAccumulator / EnergyAccumulatorCnt) / 3600);
			Measures.Energies.ActiveEnergy 		   += ((ActiveEnergyAccumulator / EnergyAccumulatorCnt)/ 3600);
			Measures.Energies.ReactiveEnergy        += ((ReactiveEnergyAccumulator / EnergyAccumulatorCnt)/ 3600);
			Measures.Energies.PartialApparentEnergy += ((ApparentEnergyAccumulator / EnergyAccumulatorCnt)/ 3600);
			Measures.Energies.PartialActiveEnergy   += ((ActiveEnergyAccumulator / EnergyAccumulatorCnt)/ 3600);
			Measures.Energies.PartialReactiveEnergy += ((ReactiveEnergyAccumulator / EnergyAccumulatorCnt)/ 3600);
		}
		ApparentEnergyAccumulator = 0.0;
		ActiveEnergyAccumulator   = 0.0;
		ReactiveEnergyAccumulator = 0.0;
		EnergyAccumulatorCnt      = 0;
	}

}

static void ResetMeasure()
{
  Measures.CurrentRMS            = 0.0;
  Measures.VoltageRMS   	       = 0.0;
  Measures.ApparentPower		     = 0.0;
  Measures.ActivePower           = 0.0;
  Measures.ReactivePower         = 0.0;
  Measures.PowerFactor		       = 0.0;
  ResetTotalEnergy();
  ResetMaxMin();
  ResetAvg();
}



void ResetTotalEnergy()
{
	Measures.Energies.ApparentEnergy        = 0.0;
	Measures.Energies.ActiveEnergy          = 0.0;
	Measures.Energies.ReactiveEnergy        = 0.0;
	Measures.Energies.PartialApparentEnergy = 0.0;
	Measures.Energies.PartialActiveEnergy   = 0.0;
	Measures.Energies.PartialReactiveEnergy = 0.0;
}

void ResetPartialEnergy()
{
	Measures.Energies.PartialApparentEnergy = 0.0;
	Measures.Energies.PartialActiveEnergy   = 0.0;
	Measures.Energies.PartialReactiveEnergy = 0.0;
}

void ResetMaxMin()
{
	Measures.MaxMinAvg.MaxCurrent       = 0.0;
	Measures.MaxMinAvg.MinCurrent       = 0.0;
	Measures.MaxMinAvg.MaxVoltage       = 0.0;
	Measures.MaxMinAvg.MinVoltage       = 0.0;
	Measures.MaxMinAvg.MaxActivePower   = 0.0;
	Measures.MaxMinAvg.MinActivePower   = 0.0;
	Measures.MaxMinAvg.MaxReactivePower = 0.0;
	Measures.MaxMinAvg.MinReactivePower = 0.0;
	Measures.MaxMinAvg.MaxApparentPower = 0.0;
	Measures.MaxMinAvg.MinApparentPower = 0.0;
	Measures.MaxMinAvg.MaxPowerFactor   = 0.0;
	Measures.MaxMinAvg.MinPowerFactor   = 0.0;
}

void ResetAvg()
{
	Measures.MaxMinAvg.CurrentAvg       = 0.0;
	Measures.MaxMinAvg.VoltageAvg       = 0.0;
	Measures.MaxMinAvg.ActivePowerAvg   = 0.0;
	Measures.MaxMinAvg.ReactivePowerAvg = 0.0;
	Measures.MaxMinAvg.ApparentPowerAvg = 0.0;
	Measures.MaxMinAvg.PowerFactorAvg   = 0.0;
}

void TaskMeasure()
{
	if(EepParamsValue[SIMULATION_MODE] == DISABILITATO)
		CalcMeasure();
	else
		CalcSimCurrentVoltage(false);

	CalcEnergy();
	if(OldEnableSim != EepParamsValue[SIMULATION_MODE])
	{
		OldEnableSim = EepParamsValue[SIMULATION_MODE];
		ResetMeasure();
	}
}
