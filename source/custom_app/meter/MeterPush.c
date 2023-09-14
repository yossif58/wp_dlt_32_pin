/**********************************************************************************
*																					*
* FILE NAME:																		*
*   DataReadings.c																*
*																					*
* DESCRIPTION:																		*
*   This file contains all routines that handels Data Readings Process       
*   The meter intialize the data readings message every 15 minutes towards  the DCU.
* HISTORY																			*
*   Ferlandes Yossi																	*
* - CREATION      																	*
*   Yossi Ferlandes 22/09/2020												    *
*                                                                                   *
************************************************************************************/
#define METER_PUSH_H
//#include <stdlib.h>
#include "stdio.h"
//#include "stdint.h"
#include "string.h"
#include <stdbool.h>
#include "hal_api.h"
#include "uartdlt.h"
#include "board.h"
//#include "logger.h"
//#include "eeprom.h"
#include "DLTProtocol.h"
#pragma pack(1)
/* local vars defenitions ***********************************************************/
/** Time out for periodic message BASED ON 5 SEC*/

#define MIN_20_PERIODIC              48   // temp for debug read value shoul be 2400  // Every 20 min start data readings process 
#define DCU_REQ_TIMEOUT              8    // Timeout for DCU TO ACK 
#define METER_REQ_TIMEOUT            4     // Time out for meter req data readings send req again 
#define MAX_RETRIES                  3     // Maximum retries for sending message to DCU or request messages to meter 
typedef enum
{
    /** Block request was successful */
    DATA_READINGS_IDLE_STATE = 0,
    DATA_READINGS_WAIT_TIME_RES,
    DATA_READINGS_SEND_TO_DCU_WAIT_FOR_ACK 
    
}DataReadings_enum;
void StartPushInit(bool Powerup);
void     SendHourReq(uint8_t tmphour);
uint8_t  SearchNotAckedHours(void);

static uint32_t SendPeriodicCntr = 0;   
static uint8_t DataReadingsState =  DATA_READINGS_IDLE_STATE;
static uint8_t MeterReqRetry = 0;
static uint8_t tmpbuf[2];
static uint8_t hourinprocess=0xff;
static uint8_t currenthour=0xff;
static bool dayly_hours_acked[24]; // y.f. 26/04/21 add dayly array hours for complimentation of not send hours since 00:00 
extern long long 	ExtendedAddress;

/*************************************************************************************/
/************************** local function defenitions *******************************/
/*************************************************************************************/

/***************************Exported Vars                                            */
/***************************Extern functions******************************************/
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : PushTask                                                     */
/* - DESCRIPTION     : this task is called from meter task every periodic time      */
/*                     it handels the push mechanisem. Meter will initiate          */
/*                     data readings command towards sink. It will need the exact   */
/*                     hour. so it will first send read time command. and only after*/
/*                     receiving the time it will send the data readings command.   */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     24/09/2020      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void PushTask(void)
{
  uint8_t hour,tmphour1;
  if (GetDataReadingsACK())
  { // Data read message has ACKED by the sink process has completed start new period after hour changed
     dayly_hours_acked[hourinprocess] = true;   
     if (currenthour == 0) // 27.04.21 completion is till start of day 00:00 so at 24:00 we skip all last day hours and start with all false.
     {
        StartPushInit(true);
        dayly_hours_acked[0] = true;
     }
      else
      {
        StartPushInit(false);
         // y.f. 26/04/21 add send completion hours after resume connection  
        tmphour1 =   (SearchNotAckedHours());
        if (tmphour1 !=0xff)
        {  // if not all previous hours acked
          DataReadingsState = DATA_READINGS_SEND_TO_DCU_WAIT_FOR_ACK;
          SendHourReq(tmphour1) ;
        }
      }
   
  }
  else
  switch (DataReadingsState)
  {
    default:
    case DATA_READINGS_IDLE_STATE:
      if ((++SendPeriodicCntr) > (MIN_20_PERIODIC + (ExtendedAddress % 15))) // y.f. 5/5/21
      {// start read process with time req we need hour index for the data readings commnd
          MeterReqRetry = 0;
          SendPeriodicCntr = 0; 
          DataReadingsState = DATA_READINGS_WAIT_TIME_RES;
          SendDltReadTime(USART0);
      }
      
    break;
   case DATA_READINGS_WAIT_TIME_RES:
      hour = GetTimeFromMeter();
      if (hour!=0xff)   
      {
        if (hour == currenthour) 
           StartPushInit(false); // // y.f. no need to send req  dataif it  was acked for the current hour
       else
       {
            currenthour=hour; // y.f. 28/04/21 
            if (currenthour == 0) // 28.04.21 completion is till start of day 00:00 so at 24:00 we skip all last day hours and start with all true.
              StartPushInit(true);
            MeterReqRetry = 0;
            SendPeriodicCntr = 0; 
            DataReadingsState = DATA_READINGS_SEND_TO_DCU_WAIT_FOR_ACK;
            SendHourReq(hour); // y.f. 26/04/2021 
        }
      }
      else if ((++SendPeriodicCntr) > METER_REQ_TIMEOUT ) 
      { // send time req again
          MeterReqRetry++ ;
          if ((++MeterReqRetry < MAX_RETRIES))
          {
            SendPeriodicCntr = 0; 
            ClrHour();
            SendDltReadTime(USART0);
          }
          else     
          {// meter is not responding leave it for the next period
            StartPushInit(false);
          }
      }
        
    break;
    case DATA_READINGS_SEND_TO_DCU_WAIT_FOR_ACK:
     
      if ((++SendPeriodicCntr) > (uint32_t )(DCU_REQ_TIMEOUT+ (ExtendedAddress % 3))) // y.f. 5/5/ add dif time ExtendedAddress
      {
        if (++MeterReqRetry < MAX_RETRIES)
        {
            SendPeriodicCntr = 0;
            SendHourReq(hourinprocess); //26/04/21
 
        }
        else
          // DCU is not responding just leave it for next hour 
        {
          StartPushInit(false);
        }
          
      }
        
    break;
  }
}
bool GetPushTest(void)
{
  if ( DataReadingsState == DATA_READINGS_WAIT_TIME_RES)
       return true;
  else return false;
}
void StartPushInit(bool Powerup)
{
  MeterReqRetry = 0;
  SendPeriodicCntr = 0;
  ClrDataReadingsACK();
  ClrHour();
  DataReadingsState = DATA_READINGS_IDLE_STATE; 
  if (Powerup)
    memset (dayly_hours_acked,false ,24);
//  srand((uint16_t)(ExtendedAddress));  
}
void SendHourReq(uint8_t tmphour)
{
  //26/4/21
            hourinprocess=tmphour;
            tmpbuf[0]= 0;
            tmpbuf[1]= hourinprocess;
            ClrDataReadingsACK();
            SendDataReadingsReq2meter(USART0,tmpbuf);
            
}
uint8_t  SearchNotAckedHours(void)
{
   uint8_t bInd;
   for (bInd = 0 ; bInd < currenthour ; bInd++)
     if (dayly_hours_acked[bInd] == false) return bInd; 
       return 0xff; // all hours send 
     
}