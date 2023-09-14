/* FILE NAME:
*   logger.c
*
* DESCRIPTION:
*   This file contains all routine handle the water meter logger DBS
*
* DATA STRUCTURES:
* FUNCTIONS
* HISTORY
*   Ferlandes Yossi Creation 26/12/2012 
*
******************************************************************************/

#define LOGGER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "api.h"
#include "logger.h"
#include "utils.h"
#include "watercalc.h"
#include "eeprom.h"

static uint16_t EEpromlogger_index;
#define MAX_EEPROM_RECORDS	1000
void AddItem2Logger(void)
{
	long tlong=0;
	  uint8_t bInd;
	  uint8_t dval[4]={0};
	  loggerIndex = (loggerIndex +1)% MAX_RECORDS;
	  WaterLogger[loggerIndex].Data.TimeDate = lib_time->getTimestampS();
	  for(bInd=0;bInd<4;bInd++)
	     dval[bInd]=BREAK_UINT32( tlong, bInd ) ;
	  memcpy ((unsigned char *)&( WaterLogger[loggerIndex].Data.Consumption), (unsigned char *)&dval,4);
	  WaterLogger[loggerIndex].Data.Status=alrmStatus.alarmByte;
	  WaterLogger[loggerIndex].Data.FactorType=sMyConfig.FactorType;
	  WaterLogger[loggerIndex].ReadFlag = 0;
// removed till flash	  if (loggerIndex==0)
//		  WriteLogger2Eeprom(EEpromlogger_index);
	  EEpromlogger_index=( EEpromlogger_index+1) % MAX_EEPROM_RECORDS;
 }


uint8_t GetUnreadRecordFromLogger(void)
{
   uint8_t bInd;
  // first see if the current was not read and send it 
#ifdef ReadWheels 
   return loggerIndex; // send last anycase
#endif   
   if (WaterLogger[loggerIndex].ReadFlag==0)
     
     return loggerIndex;
  for (bInd  = 0 ; bInd < MAX_RECORDS ; bInd++)
  {
   
        
     if (WaterLogger[bInd].ReadFlag==0) //
       return bInd;
  }
  return 0xff; 
}


void MarkReadItem( uint8_t bIndex )
{
  // if value of bIndex is 0xff mark the record pointed by the loggerIndex
  
  if ( bIndex == 0xff)
     WaterLogger[loggerIndex].ReadFlag=1;
  else
     WaterLogger[bIndex].ReadFlag=1;
}


void MarkUnReadItem( uint8_t bIndex )
{
  // if value of bIndex is 0xff mark the record pointed by the loggerIndex
  
  if ( bIndex == 0xff)
     WaterLogger[loggerIndex].ReadFlag=0;
  else
     WaterLogger[bIndex].ReadFlag=0;
  
}



void InitWaterLogger(void)
{
     uint8_t bInd;

    for (bInd  = 0 ; bInd < MAX_RECORDS ; bInd ++)
      WaterLogger[bInd].ReadFlag=0xff;
  
}

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : void BeaconAckMsg(void)                                      */
/* - DESCRIPTION     : i                                                            */
/* - INPUT           : opcode of the message send by meter 		    	    */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION      								    */
/*   Michael Lifshits 16/04/2012                                                    */
/*   updated by  yossi ferlandes 30/1/2/2012                                        */
/*               add logger records                 
*************************************************************************************/
void LoggerAckMsg(void)
{
  
  appStatus.getBeaconAck = 1;
  {
    MarkReadItem(LastLoggerIndexSend);
  }   

}

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : void ReadLoggerProcess(void)                                 */
/* - DESCRIPTION     : i                                                            */
/* - INPUT           : RecordNum = Num of last record hours stored in logger DBS    */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION      								    */
/*   yossi ferlandes 25/2/2012                                                      */
/*                                                                                  */
/*************************************************************************************/
void ReadLoggerProcess(uint8_t Records)
{

  while (Records)
  {
      if (loggerIndex >=Records)
        MarkUnReadItem(loggerIndex - Records );
      Records--;
  }
}
