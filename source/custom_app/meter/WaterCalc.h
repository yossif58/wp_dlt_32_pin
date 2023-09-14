/****************************************************************************************
* WaterCalc.h 																			   	*
* DESCRIPTION:																		   	*
*   This file contains ALL POWERCOM new watermetr consumption calculation  exported routints and definitions .       *
*																					   	*
* HISTORY																			 	*
*   Ferlandes Yossi  5/8/2012   Created										  		*
****************************************************************************************/
#ifndef _POWERCOM_WATER_CALC_H_
#define _POWERCOM_WATER_CALC_H_
#undef PUBLIC

#ifdef WATER_CALC_H
#define  PUBLIC
#else
#define  PUBLIC  extern
#endif
//#define tamper_interrupt
#pragma pack(1)



#define LEAK_TIME_PERIOD        10  // 10 min 

PUBLIC void Pulser_init(void);
PUBLIC void Magnet_init(void);
PUBLIC void SwOpen_init(void);
PUBLIC uint32_t WATER_HOUR;
PUBLIC uint32_t WATER_VOL;
PUBLIC uint32_t LastConsumption;

struct appState
{
  unsigned	tick1sec: 1;
  unsigned	txEnable: 1;
  unsigned	maxFlowAlarm: 1;
  unsigned	leakAlarm: 1;
  unsigned	batteryLow: 1;
  unsigned	downLoad:1;
 // unsigned	waterUpdate1:1;
  unsigned	CCWAlarm: 1;
  unsigned      tamper:1;    // y.f. add tamper 17/07/2013
//  unsigned	watertimeupdated:1;
//  unsigned	waterNoUpdate:1;
  unsigned	waterUpdate1:1;
  unsigned	waterUpdate2:1;
  unsigned	waterUpdate3:1;
  unsigned	lcdUpdate:1;
  unsigned	valveStatus:1;
  unsigned	getBeaconAck:1;
  unsigned	flashUpdate1:1;
  unsigned	flashUpdate2:1;
};


struct tampState
{
  unsigned	newValue: 1;
  unsigned  oldValue: 1;
  unsigned	curState: 1;
};

PUBLIC struct tampState tamperSt;

PUBLIC	struct appState appStatus;        
PUBLIC  volatile uint32_t BACK_FLOW;
union al_Status
{
 	uint8_t	alarmByte;
	struct al_Bits
	{
	  unsigned	tamper: 1;
	  unsigned	CCW: 1;		// back flow
	  unsigned  leakege: 1;
	  unsigned	qMax: 1;	// max flow
	  unsigned	lowBat: 1;
	}ssbits;
};

PUBLIC volatile uint32_t WaterTickTenth;
PUBLIC volatile uint32_t WaterTickCubicUnits;
PUBLIC uint16_t AlertCount;
PUBLIC uint16_t LastShipmentCount;
PUBLIC uint16_t LastAlertCount;
PUBLIC uint8_t   SWunitType;
PUBLIC uint8_t   SWdivIndex;
PUBLIC uint8_t bShipmentCntr;
PUBLIC union al_Status alrmStatus;
PUBLIC const uint32_t	divFactor;
PUBLIC volatile long long LCD_WATER;
PUBLIC unsigned long get_water_ticks(uint8_t InitFlg );
PUBLIC void IncWaterTick(void);

void AlertProcess(void);

#ifdef tamper_interrupt
void WATER_TAMPER_isr(void);
void checkTamper(uint8_t bCheck1sec30sec);
#endif


#ifdef LCD_EXIST
PUBLIC void checkTxEnable(void);
#endif

//PUBLIC void checkCCW(void);
//PUBLIC void autoReset(void);
PUBLIC void WATER_SENSOR_1_isr(void);
PUBLIC void WATER_SENSOR_2_isr(void);
PUBLIC void WATER_SENSOR_3_isr(void);
PUBLIC void WATER_SENSOR_4_isr(void);
PUBLIC void ResetWaterStatuses(void);
PUBLIC void CheckWaterAlarms(uint8_t InitFlg);
PUBLIC void InitWaterAlarms(void);

#endif
