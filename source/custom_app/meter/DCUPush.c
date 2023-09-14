/**********************************************************************************
*																					*
* FILE NAME:																		*
*   DCUPush.c																*
*																					*
* DESCRIPTION:																		*
*   This file contains all routines that handels Data Readings Process       
*   From the DCU side.
* HISTORY																			*
*   Ferlandes Yossi																	*
* - CREATION      																	*
*   Yossi Ferlandes 22/09/2020												    *
*                                                                                   *
************************************************************************************/
#define DCU_PUSH_H
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include <stdbool.h>
#include "hal_api.h"
#include "board.h"
#include "uartdlt.h"
#include "DLTProtocol.h"
#include "api.h"
#pragma pack(1)
/* local vars defenitions ***********************************************************/
/** Time out for periodic message BASED ON 0.5 SEC*/
extern void KeypadSendReadDltMsg2Meter(USART_TypeDef *usart,uint8_t * meteradd,uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid, uint8_t BroadCast);
extern void SendStDLTRSTmsg(void);

#define MAX_LOGGER_METERS 15
void StartDCUPushInit(void);

typedef struct  {
    uint8_t Day; 
    uint8_t Hour;
    uint8_t info[35];
}meterData; 
    
typedef struct  {
  //  uint8_t NextIndexArray;
    uint8_t MeterAdd[6];
    meterData Data[24];
}MeterDataReadingStruct; 
#ifndef ST_FLASH
uint16_t SearchExistMeterLocation(uint8_t *MsgIn);
uint8_t  SearchExistRecordLocation(uint16_t LogerIndex, uint8_t *MsgIn);

static MeterDataReadingStruct MeterLoger[MAX_LOGGER_METERS]; // for now 17 meters 
#endif
extern uint8_t Hour_Req_Send_2Meter;
/*************************************************************************************/
/************************** local function defenitions *******************************/
/*************************************************************************************/

/***************************Exported Vars                                            */
/***************************Extern functions******************************************/
/*************************************************************************************/
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : DCUPushTask                                                  */
/* - DESCRIPTION     : this DCU PUSH background task                                */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     30/09/2020      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/

void DCUPushTask(void)
{
  if (Hour_Req_Send_2Meter)
    Hour_Req_Send_2Meter--;
 // return;
  // For now I don't find any thing for background task
}   

/*************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : InitMeterlogger                                              */
/* - DESCRIPTION     : This routine Initalaize meter logger                         */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     30/09/2020      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void InitMeterlogger(void)
{
#ifndef ST_FLASH
  // using internal RAM as logger
  for (uint8_t bInd = 0 ; bInd < MAX_LOGGER_METERS ; bInd++)
  {
        MeterLoger[bInd ].MeterAdd[0] =0xff;  // add cant start with 0xff
  }
#else
    SendStDLTRSTmsg();
#endif  
}   
          
          
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : WriteMeterRecord                                             */
/* - DESCRIPTION     : This routine writes meter record to logger                   */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     30/09/2020      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
bool WriteNewMeterRecord(uint8_t *MsgIn, uint16_t size)
{

#ifndef ST_FLASH

  uint16_t LogerIndex,bInd;
  DLTReadStruct * pMsg;
  pMsg = (DLTReadStruct*)MsgIn;
  MeterDataReadingStruct *pInd;
  LogerIndex = SearchExistMeterLocation(MsgIn);
  int8_t tmphour;
  if (LogerIndex == 0xffff)
  {
    // meter was not found 
    // start search for empty location if exists
    
      for (bInd =0 ; bInd < MAX_LOGGER_METERS ; bInd++)
      {
        // just look for first location empty 
          if (MeterLoger[bInd ].MeterAdd[0] == 0xff) break; // imdication for empty location for new meter.
           
                                                 
          
      }
      if (bInd >= MAX_LOGGER_METERS)       // It seems that there is no place for one more meter
                                           // just return without storring sorry limitation of RAM.
      {
       //    Usart_sendBuffer("record no place to enter\r\n",25);
                    // y.f. for debug only to be remove 12/10/2020
           return false;
      }
      
     // MeterLoger[bInd ].NextIndexArray =0;
      LogerIndex = bInd;
      // enter meter address . This is done once per each meter
      pInd = (MeterDataReadingStruct *) &MeterLoger[LogerIndex];   
      memcpy ((uint8_t*)&pInd->MeterAdd[0],&pMsg->Header.Add ,6); // copy address 
    //  Usart_sendBuffer("meter new position record entered in RAM \r\n",43);
  }
  else
  {
     pInd = (MeterDataReadingStruct *) &MeterLoger[LogerIndex];
     //Usart_sendBuffer("meter found record entered in RAM \r\n",35);
  }
  // copy only  data from the time till checksum not included 
  // before copyn substract 0x33 from each byte 
   tmphour = (pMsg->Info[1]-0x33) %24; // hour is index for data array
 //  if (pMsg->Info[0] > 0x33) return false ; // enter only current day 
 //  uint8_t *pTmp  = (uint8_t*)&pInd->Data[ pInd->NextIndexArray];
   uint8_t *pTmp  = (uint8_t*)&pInd->Data[ tmphour];
   for (uint16_t Ind = 0 ; Ind < sizeof(meterData) ; Ind++)
   {
     *(pTmp+Ind) = pMsg->Info[Ind]-0x33;
     
   }
#else
  // just send transparent message message to ST
      uint32_t time = lib_time->getTimestampCoarse(); //y.f. 13/04/21 add time stamp to record 
      memcpy ( &MsgIn[size], (uint8_t*) &time,4); 

     Usart1_sendBuffer((unsigned char *) MsgIn,size); 
 
#endif  
  
   return true;
   
}
#ifndef ST_FLASH
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : SearchExistMeterLocation                                     */
/* - DESCRIPTION     : This routine search for existing meter location in logger    */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     30/09/2020      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
uint16_t SearchExistMeterLocation(uint8_t *MsgIn)
{
  uint16_t bInd;
 
  DLTReadStruct * pMsg;
  pMsg = (DLTReadStruct*)MsgIn;
  // Start search for specific meter in logger
  for (bInd = 0 ; bInd < MAX_LOGGER_METERS ; bInd++)
  {
     if (memcmp((uint8_t*)&MeterLoger[bInd ].MeterAdd,(uint8_t*)&pMsg->Header.Add, 6) ==0)
       return bInd;
  }
  return 0xffff;   // meter was not found
}
          
          
bool PrepareResFromLogger (uint8_t *bufout )
{
  uint8_t RecordIndex;
  MeterDataReadingStruct *pInd;
  uint16_t LogerIndex = SearchExistMeterLocation(bufout);
  if (LogerIndex == 0xffff) 
  {
    //Usart_sendBuffer("meter not found for response\r\n",30);
    return false;
  }
    
  RecordIndex = SearchExistRecordLocation(LogerIndex,bufout);
  if (RecordIndex == 0xff)
  {
     //Usart_sendBuffer("meter found record no record for response \r\n",45);
     return false;
  }
  pInd = (MeterDataReadingStruct *) &MeterLoger[LogerIndex];
  KeypadSendReadDltMsg2Meter(USART0,(uint8_t*)&pInd->MeterAdd,(uint8_t*)&pInd->Data[RecordIndex], sizeof(meterData),DLTREAD_EM_READ_DATA_READINGS,0);
  
  // after sending we have to deleet record from array this is done by delete the hour in message
  MeterLoger[LogerIndex ].Data[RecordIndex].Hour =0xff;
  return true;
}
         
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : SearchExistRecordLocation                                    */
/* - DESCRIPTION     : This routine search for existing record with suitable date   */
/*                     and time not relevant if external flash is used              */
/* - INPUT           : dlt message                                                  */
/* - OUTPUT          : record index .                                               */
/* - CHANGES         :                 						    */
/* - CREATION     30/09/2020      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/          
uint8_t  SearchExistRecordLocation(uint16_t LogerIndex, uint8_t *MsgIn)
{
  uint8_t bInd;
  DLTReadStruct *pMsg;
  pMsg = (DLTReadStruct*) MsgIn ;
  // extract day and hour from msgin
  uint8_t day  = pMsg->Info[0]-0x33; 
  uint8_t hour = pMsg->Info[1]-0x33;       
  for (bInd = 0 ; bInd < 24 ; bInd++)
  {
    if ((MeterLoger[LogerIndex ].Data[bInd].Day ==  day) &&
       (MeterLoger[LogerIndex ].Data[bInd].Hour ==  hour))
      return bInd; // record was found
  }
  return 0xff; // record was not found
}
// end of using internal RAM as logger  
#endif  

