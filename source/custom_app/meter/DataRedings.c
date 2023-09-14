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
#define DATA_READINGS_H
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "DLTProtocol.h"

/* local vars defenitions ***********************************************************/

/** Time out for periodic message BASED ON 0.5 SEC*/
#define MIN_20_PERIODIC              150   // temp for debug read value shoul be 2400  // Every 20 min start data readings process 
#define DCU_REQ_TIMEOUT              80    // Timeout for DCU TO ACK 
#define METER_REQ_TIMEOUT            4     // Time out for meter req data readings send req again 
#define MAX_RETRIES                  3     // Maximum retries for sending message to DCU or request messages to meter 
typedef enum
{
    DATA_READINGS_IDLE_STATE = 0,
    DATA_READINGS_WAIT_TIME_RES,
    DATA_READINGS_SEND_TO_DCU_WAIT_FOR_ACK 
    
}DataReadings_enum;

static uint32_t SendPeriodicCntr = 0;   
static uint8_t DataReadingsState =  DATA_READINGS_IDLE_STATE;
static uint8_t MeterReqRetry = 0;
static uint8_t tmpbuf[2];
static uint8_t hour;
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
  if (GetDataReadingsACK()
  { // Data read message was ACKED by the sink.Process has completed return to IDLE state
    SendPeriodicCntr = 0; 
    DataReadingsState = DATA_READINGS_IDLE_STATE;
    ClrDataReadingsACK();
  }
  switch (DataReadingsState)
  {
    default:
    case DATA_READINGS_IDLE_STATE:
      if ((++SendPeriodicCntr) > MIN_15_PERIODIC )
      {// start read process with time req we need hour index for the data readings commnd
          MeterReqRetry = 0;
          SendPeriodicCntr = 0; 
          ClrHour();
          DataReadingsState = DATA_READINGS_WAIT_TIME_RES;
          SendDltReadTime();
      }
      
    break;
   case DATA_READINGS_WAIT_TIME_RES:
     
      hour = GetTimeFromMeter();
      if (hour!=0xff)
      {// response time has received from meter
        
          MeterReqRetry = 0;
          SendPeriodicCntr = 0; 
          DataReadingsState = DATA_READINGS_SEND_TO_DCU_WAIT_FOR_ACK;
          tmpbuf[0]= 0;
          tmpbuf[1]= hour;
          ClrDataReadingsACK();
          SendDataReadingsReq2meter(tmpbuf);
      }
      else if ((++SendPeriodicCntr) > METER_REQ_TIMEOUT ) 
      { // send time req again
          MeterReqRetry++ ;
          if ((++MeterReqRetry < MAX_RETRIES)
          {
            SendPeriodicCntr = 0; 
            ClrHour();
            SendDltReadTime();
          }
          else     
          {// meter is not responding leave it for the next period
            SendPeriodicCntr = 0; 
            DataReadingsState = DATA_READINGS_IDLE_STATE; 
          }
      }
        
    break;
    case DATA_READINGS_SEND_TO_DCU_WAIT_FOR_ACK:
     
      if ((++SendPeriodicCntr) > DCU_REQ_TIMEOUT)
      {
        if (++MeterReqRetry < MAX_RETRIES)
        {
            ClrDataReadingsACK();
            SendPeriodicCntr = 0;
            tmpbuf[0]= 0;
            tmpbuf[1]= hour;
            SendDataReadingsReq2meter(tmpbuf);
        }
        7else
          // DCU is not responding just leave it for next period 
        {
          SendPeriodicCntr  = 0;
          DataReadingsState = DATA_READINGS_IDLE_STATE;
        }
          
      }
        
    break;
  }
}

