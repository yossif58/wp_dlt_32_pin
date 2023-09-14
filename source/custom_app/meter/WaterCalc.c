/***********************************************************************************
*																					*
* FILE NAME:																		*
*  WaterCalc.c																			*
*																					*
* DESCRIPTION:																		*
*   This file contains all routines for WaterMeter consumtion calculation
* HISTORY																			*
*   Ferlandes Yossi																	*
* - CREATION      																	*
*   Yossi Ferlandes 5/8/2012														*
*                                                                                   *
************************************************************************************/
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "api.h"
#include "time.h"
#include "logger.h"
#include "eeprom.h"
#define WATER_CALC_H
#include "WaterCalc.h"
uint8_t Result;
app_lib_time_timestamp_coarse_t LastCheckTime;
uint8_t tamperTime=0;
bool   checktamper_port(void);
bool   OldSensorPort;
#define  MAX_QMAX_FLOW_PER_HOUR           40
#define  LEAK_HOUR_TIME                   60    // In MIN 
#define  MAX_CONSUMPTION_LEAK_6_HOURS     20    // 2 Qubs in 6 hours
#define  MAX_CONSUMPTION_LEAK_24_HOURS    24    // at least sequential 24 hours with 100 litter per hour
#define  LEAK_6_HOURS_TIME                6     // 2 Qubs in 6 hours
#define  LEAK_24_HOURS_TIME               24

#define   _kFactor1  3476	//+4% = 65536*1.25/23.568 -> replace division by multyplication and shift right
#define   _kFactor2  3337	//M.L. 17/1/12 "New Plastic from Amir"
#define  _kFactor3   3337	// = 65536*1.25/24.55
#define	_flow4sel 1290	// was 1567, units of TimerA for 60 l/hour
#define	_flow4sel2  350
#define	_divIndex     6    // y.f. shabat meter 10 times than lcd meter every unit represent 100 liter
#define	_maxFlowVal	  6	    // 10 min assumtion max = 4000 liter in one our in 10 min 4000/6 = 666 liter one tick is 100 liter => value is 666/100 = 6
#define	_maxFlowTime  10	// 10 minutes
#define	_maxFlowEnd   2		// 1 minute
#define	_minLeakVal	  1	// y.f. in 10 min at least 1 =>100 liter in 10 min
#define	_maxLeakTime  22*6	// y.f. 22 hours based on 10 min period
#define	_maxLeakEnd   2*60*2	// 2 hours
#define	_maxBackFlow  3		// max back flow 300 liters y.f.16/12/2012 change to 3 we have only 0.1 kube
#define	_minEnableTx  12	// enable RF transmission only ater _minEnableTx pulses
#define    _measureUnit  1	// display "cubic meter"
#define    _decPoint     1	// 1 digit after decimal point
#define    _autoReset    96	// hours for reset latched alarms
#define    _unitType     0x20	// 0 - Electric meter, 0x10 - Water meter, 0x20 - Gas Meter, 0x30 - Valve/Contactor  0x40 shabat water meter y.f. 16/12/2012 add shabat meter
#define		_baseFactor   24550	//24.55*1000  y.f. 13/12/2012 not used in shabat meter
#define 	_userID       0x46	// user iD from communications protocol
#define   _statusMask    0x1F
#define	_lcdRefresh   2	 // 2sec = 2000 msec
#define   _frequency     0xae  // was c1 y.f. 30/08/2012
#define   _rfPower       0x07
#define	_betwTxTime   30
#define	_betwRxTime   30
#define    _txType       0x1
#define    _idleTime     0x0
#define	_acknTxTime	  900	// time after acknowledge 900sec = 60*15 min
#define    _receveTime    32	// y.f. 8/7/2013 correct was 800  (0x320) in preivus but only lsb byte counts and equ 32







// Handling 2 different windows one of 6 hours and one of 24 hours each has its own threshold
static   uint16_t            leakTimePeriodCntr;
static   uint16_t            leak6HourArrayConsumption[LEAK_6_HOURS_TIME];
static   uint16_t            leak1HourArrayConsumptionIndex;
static   uint16_t            leak1CurrentHourConsumption;
static   uint16_t            leak24HourArrayConsumption[LEAK_24_HOURS_TIME];

/***************************************************************************************/
/*                                                                                     */
/* - FUNCTION        : CheckWaterAlarms                                                */
/* - DESCRIPTION     : This is new routine for checking water validty compare to old value */
/* - INPUT           : none.     		  			                */
/* - OUTPUT          :none.                                                             */
/* - CHANGES         :                 						        */
/* - CREATION      								        */
/*   Ferlandes Yossi         21/4/2013                                                  */
/*                                                                                      */
/****************************************************************************************/
void CheckWaterAlarms(uint8_t InitFlg)
{
  
	app_lib_time_timestamp_coarse_t  bdelata,currenttime,lasttime;
	app_lib_time_timestamp_coarse_t sTmpDateTime;

   
         // getRTCHexTime(&sTmpDateTime);
   	   	  sTmpDateTime = lib_time->getTimestampS(); // y.f. 16/10/2018
          if (InitFlg)
          {

              InitFlg =0;
              LastCheckTime = sTmpDateTime;   // enter last hour only if value was taken
              LastConsumption = WATER_VOL;
              return;
          }
          

          currenttime = sTmpDateTime;
          lasttime = LastCheckTime;
          if (currenttime < (lasttime+10)) return ; // check at intervals of at least 10 minutes 
          bdelata = MAX_QMAX_FLOW_PER_HOUR*(currenttime - lasttime ) / 3600 ;   //   y.f 17/10/2018 assume 1 second
          // check qmax 
          if ((WATER_VOL   >  LastConsumption  +bdelata+1) )                    //   y.f. inc process excluding crossing zero up less then bdelta
          {
              // check validty can't be more than 40 (4000 liters per hour) 
              // ignore last read unreasnable result
            appStatus.maxFlowAlarm=1;
            alrmStatus.ssbits.qMax=1;	// latched alarm
          }
          else
          {
            appStatus.maxFlowAlarm=0;
          }
       
          LastCheckTime =  sTmpDateTime;
          LastConsumption  = WATER_VOL ;
}

//------------------------------------------------------------------------------
//  void checkTamper(void)
//
//  DESCRIPTION:
//  Checks the Tamper input status and set Alarm 
//	in case that the input is low for debouncing time
//  Must bu run every 30 seconds interval
//------------------------------------------------------------------------------

#ifdef tamper_interrupt

void checkTamper(uint8_t bCheck1sec30sec)
{
  static uint8_t tamperTime;
  if(checktamper_port())
	tamperSt.newValue=1;
  else
	tamperSt.newValue=0;
  if(tamperSt.newValue == tamperSt.oldValue)
  {
    if ( tamperSt.curState!=tamperSt.newValue)
    { // y.f. change current state 
          tamperTime++;
          if(tamperTime >= 2) 
          {
            tamperSt.curState=tamperSt.newValue;
            if(tamperSt.curState==0){ // active low 
              
            	  alrmStatus.ssbits.tamper = 1;		// latched alarm
                  appStatus.tamper  = 1;
            }
            else
            {
                  appStatus.tamper = 0;
            }
          }
    }          
  }
  else
  {
          
	tamperTime=0;
	tamperSt.oldValue = tamperSt.newValue;
  }

  
}









void WATER_TAMPER_isr(void)
{
  bShipmentCntr++;  // y.f. check shipment sequence min 3 times in 30 seconds 

  {
    // y.f. 2/12/2012 add shipment with tamper 
      if ((bShipmentCntr >=3 ) && (bShipmentCntr < 8))
      {
    	  sMyConfig.ShipmentDisable  = 0x56;
    	  WaterLogger [loggerIndex ].ReadFlag=0;  // y.f. 7/3/2013 send last hour again be shure there is at least one record to send
    	  bShipmentCntr =0;
    	  LastShipmentCount =0;
      }       
  }    


}  
#endif  


void ResetWaterStatuses(void)
{
   
 //  maxFlowTime=0;
 //  LeakTimePeriod =0;
  
  // y.f. clear only if application alert is clear 

   // y.f 17/07/2013  clear only if application alert is also clear  
   if (appStatus.tamper == 0)
	   alrmStatus.ssbits.tamper = 0;
   if (appStatus.leakAlarm == 0)
	   alrmStatus.ssbits.leakege=0;
   if (appStatus.CCWAlarm ==0)
	   alrmStatus.ssbits.CCW = 0;
   if (appStatus.maxFlowAlarm ==0)
      alrmStatus.ssbits.qMax =0;
   if (appStatus.batteryLow == 0)
	   alrmStatus.ssbits.lowBat =0;
   
  
  // tamperCount=0;
   //backFlowCnt = BACK_FLOW;
   bShipmentCntr =0;
//   leakAlarmCntr =0;
   AlertCount =0; // y.f. 11/2/2013 
   
}

void InitWaterAlarms(void)
{
   

  // y.f. reset only latch alarms but not the status alarms 
   appStatus.leakAlarm = 0;
   appStatus.CCWAlarm =0;
   appStatus.maxFlowAlarm =0;
   appStatus.batteryLow = 0;
   appStatus.tamper = 0;  // y.f. 17/07/2013 add init val to appl alerts 
   
    
   
   alrmStatus.ssbits.tamper = 0;
   alrmStatus.ssbits.qMax =0;
   alrmStatus.ssbits.leakege=0;
   alrmStatus.ssbits.lowBat =0;
   alrmStatus.ssbits.CCW = 0;
   bShipmentCntr =0;
   AlertCount =0; // y.f. 11/2/2013 
   leakTimePeriodCntr=0;
   leak1HourArrayConsumptionIndex=0;
   leak1CurrentHourConsumption=0;
   
   memset ((unsigned char *)&leak24HourArrayConsumption,0,sizeof(leak24HourArrayConsumption));
   memset ((unsigned char *)&leak6HourArrayConsumption,0,sizeof(leak6HourArrayConsumption));
   
}


#ifdef    	tamper_interrupt
void AlertProcess(void)
{

    {  

      checkTamper(1);
      
 //     while ( sMyConfig.ShipmentDisable ==0x55)
#ifdef START_SHIPMENT      
      while ( sMyConfig.ShipmentDisable !=0x56)  // y.f. 
      {
        checkTamper(1);
        if (sMyConfig.ShipmentDisable ==0x56)
        {

        }
      }    
#endif      
    } 
//    else ResetWaterStatuses();  // y.f. 10/2/2013  check only after normal interrval time 
    LastAlertCount = AlertCount;                          
}
#endif

unsigned long get_water_ticks(uint8_t InitFlg )
{
        CheckWaterAlarms(InitFlg);        
        return WATER_HOUR ;
} 

void IncWaterTick(void)

{

  WATER_VOL = WATER_VOL +  WaterTickTenth;
  WaterTickTenth = 0;
if (WATER_VOL > 999999999)
  {
	WATER_VOL = 0;
  }
}     

bool checktamper_port(void)
{
	return 1  ; // temp to be assign  with real port 17/10/2018
}



