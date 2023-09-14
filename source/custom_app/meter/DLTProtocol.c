/**********************************************************************************
*																					*
* FILE NAME:																		*
*   ModeBusProtocol.c																			*
*																					*
* DESCRIPTION:																		*
*   This file contains all routines for the DLT 465 protocol                      *
* HISTORY																			*
*   Ferlandes Yossi																	*
* - CREATION      																	*
*   Yossi Ferlandes 19/11/2009														*
*                                                                                  *
***********************************************************************************/
#include <stdlib.h>
#include "string.h"
#include "hal_api.h"
#include "board.h"
#include "time.h"
#include "app.h"
#include "api.h"
#define DLT_H
#include "uartdlt.h"
#include "DLTProtocol.h"
//#include "logger.h"
#include "meterprotocol.h"
//#include "eeprom.h"
//#include "usart.h"
#include "utils.h"
#include "led.h"


#define BROADCAST_MESH_ADD 0xFFFFFFFF
#define BROADCAST_DLT_ADD  0xAAAAAAAAAAAA
#define DLT_FROM_METER  0
#define DLT_FROM_DCU    1
/* local vars defenitions ***********************************************************/
#define KEYPAD_ADDR         32
#pragma pack(1)
#ifdef PUSH

uint8_t KeypadCheckReadTimeRespMsg(uint8_t *TmpTxBuf);  // Y.F. 23/09/2020 add data readings push process

void KeypadSendReadDltMsg2Meter(USART_TypeDef *usart,uint8_t * meteradd,uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid, uint8_t BroadCast);

extern bool WriteNewMeterRecord(uint8_t *MsgIn, uint16_t size);
extern bool PrepareResFromLogger (uint8_t *MsgIn );
#ifdef   ELECTRIC_METER

extern void DltGeTnbors(void);
#endif
void DCUSendReadDltMsg2Meter(uint8_t   *add,       uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid, uint8_t BroadCast);

//void KeypadSendReadDltMsg2Meter(uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid,uint8_t broadcast);
extern bool GetPushTest(void);
static bool Ackdcu=false;

static uint8_t Hour = 0xff;

#endif
extern long long 	ExtendedAddress;
uint8_t Hour_Req_Send_2Meter = 0;
bool CovertBcd2Add(uint8_t *BtmpIn ,   long long  * Maddress);
unsigned char const DLTPassUser[]     = {0x37,0x44,0x44,0x44,0x33,0x33,0x33,0x33};

unsigned char const DLTID[] = { 0x20    ,1	,1	,1,		
                                0x20	,1	,2	,1,	
                                0x20	,1	,1	,0xf,    
                                0x20	,1	,2	,0xf,    
                                0x20	,1	,0xB	,1,	
                                0x20	,1	,0xb    ,0x10,	
                                0x20	,2	,0	,1,	
                                0x20	,2	,1      ,2,
                                0x20	,2	,1	,3,	
                                0x20	,2	,0x20	,4,	
                                0x20	,3	,0xb     ,1,		//10
                                0x20	,3	,0xb     ,2,		
                                0x20	,3	,0xb     ,3,
                                0x20	,3	,0xb     ,4,
                                0x20	,3	,0xb     ,5,
                                0x20	,3	,0xb     ,6,
                                0x20	,3	,0xb     ,7,
                                0x20	,3	,0xb     ,8,
                                0x20	,3	,0xb     ,9,
                                0x20	,3	,0xb     ,0xa,
                                0x20	,3	,0xb     ,0xb,		//20
                                0x20	,3	,0xb     ,0xc,		
                                0x20	,3	,0xb     ,0xd,
                                0x20	,3	,0xb     ,0xe,
                                0x20	,3	,0xb     ,0xf,
                                0x20	,3	,0xb     ,0x10,
                                0x20	,3	,0xb     ,0x11,
                                0x20	,3	,0xb     ,0x12,
                                0x20	,3	,0xb     ,0x13,
                                0x20	,3	,0xb     ,0x14,
                                0x20	,3	,0xb     ,0x15,		//30
                                //0x20	,3	,0xb     ,0x15,		
								0x00, 0x01, 0xFF, 0x00,		//M.L.03/11/11
                                0x02, 0x80, 0x01, 0x04,
                                0x04, 0x00, 0x04, 0x01,
								0x04, 0x00, 0x01, 0x01,
//                                0x00, 0x90, 0x02, 0x00,
//                                0x04, 0x00, 0x10, 0x09,

								0x02, 0x03, 0x00, 0x00,		//M.L.
//								0x00, 0x0B, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x01,
								0x00, 0x0B, 0x00, 0x01,
								0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00,		//40
								0x00, 0x00, 0x00, 0x00,
								0x04, 0x09, 0x10, 0x01,
								0x04, 0x09, 0x11, 0x01,
								0x02, 0x80, 0x01, 0x02,
								0x00, 0x90, 0x02, 0x00,
								0x02, 0x80, 0x01, 0x06,
								0x04, 0x00, 0x01, 0x02,
								0x02, 0x80, 0x01, 0x03,
								0x04, 0x00, 0x10, 0x09,
								0x02, 0x80, 0x00, 0x02,		//50
								0x3c, 0x23, 0x00, 0x00,
								0x04, 0x05, 0x01, 0x00,
								0x02, 0x80, 0x01, 0x06,
								0x04, 0x80, 0x00, 0x01,
                                                                0x04, 0x00, 0x05, 0x03,       // y.f. 17/02/2014 add prepayment state request 
                                                                0x02, 0x80, 0x01, 0x13,       // y.f. 22/09/2020 add for push read data commmnd
                                                                0x41, 0x43, 0x4b, 0x44,       // y.f 22/09/2020
                                                                0x45, 0x45, 0x45, 0x45,        // y.f. 25/10/2020 neighbour list 
                                                                0x45, 0x45, 0x45, 0x46,       // y.f. 18/11 reset ST flash   DLTSET_ST_RESET
                                                                0x34, 0x33, 0x33, 0x39       // y.f. 2/12/ LOAD PROFILE
};	


/*************************************************************************************/
/************************** local function defenitions *******************************/

/*************************************************************************************/

#ifdef PUSH
/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : DCUSendReadDltMsg2Meter                                      */
/*                     This is done only if you are SINK it sends read message to   */
/*                     meter and only in test mode actualy all messages received by */
/*                     the sink are aend transparent to the mesh net                */
/*                                                                                  */
/* - DESCRIPTION     :                                                              */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     19/7/2011     						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void DCUSendReadDltMsg2Meter(uint8_t *add, uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid, uint8_t BroadCast)
{ 
  long long Maddress=0; 
  uint8_t bInd;
  DLTReadStruct *pMsg;
  uint8_t CheckSum  =0;
  pMsg = (DLTReadStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  pMsg->Header.ReadWriteCmd = 0x11;
  
  // enter DI code
  for (bInd = 0; bInd < 4 ; bInd++)
  {
    pMsg->Data.DICode[3-bInd] = DLTID[DLTid*4+bInd];
    pMsg->Data.DICode[3-bInd] = pMsg->Data.DICode[3-bInd] +0x33;
  
  }
   // Convert hex addres to little endiann BCD 
  if (BroadCast)
    memset((unsigned char *)&pMsg->Header.Add[0],0xaa,6); 
  else 
  {// y.f. 29/09/2020 correct enter add to 
    for (bInd = 0; bInd < 6; bInd++)
      pMsg->Header.Add[bInd] = add[bInd];
  }
  pMsg->Header.Len= DLTDdataLen+sizeof(DLTReadDataStruct);
  for (bInd = 0; bInd < DLTDdataLen; bInd++)
  {
     pMsg->Info[bInd] = pDLTDdata[bInd] +0x33;
  }
  for (bInd = 0; bInd < DLTDdataLen+sizeof(DLTWriteHeader)+sizeof(DLTReadDataStruct) ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->Info[DLTDdataLen] = CheckSum;
  pMsg->Info[DLTDdataLen+1] = 0x16;  // ETX
  CovertBcd2Add((uint8_t *) &pMsg->Header.Add , &Maddress);
  TmpTxsizeBuf = sizeof(DLTReadDataStruct) + sizeof(DLTWriteHeader)+2;
  WPSendMsg((char*)TmpTxBuf, TmpTxsizeBuf,Maddress % 100000);
     
}

#endif

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadSendReadDltMsg2Meter                                         */
/*                     This is done only if you are SINK it sends read message to   */
/*                     meter and only in test mode actualy all messages received by */
/*                     the sink are aend transparent to the mesh net                */
/*                                                                                  */
/* - DESCRIPTION     :                                                              */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     19/7/2011     						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void KeypadSendReadDltMsg2Meter(USART_TypeDef *usart,uint8_t * meteradd,uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid, uint8_t BroadCast)
{ 
  uint8_t bInd;
  DLTReadStruct *pMsg;
  uint8_t CheckSum  =0;
  pMsg = (DLTReadStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
#ifdef ELECTRIC_SINK
  pMsg->Header.ReadWriteCmd = DLT_READ_COMMAND_RESPONSE;;
#else  
  pMsg->Header.ReadWriteCmd = DLT_READ_COMMAND;
#endif  
  // enter DI code
  for (bInd = 0; bInd < 4 ; bInd++)
  {
    pMsg->Data.DICode[3-bInd] = DLTID[DLTid*4+bInd];
    pMsg->Data.DICode[3-bInd] = pMsg->Data.DICode[3-bInd] +0x33;
  
  }
   // Convert hex addres to little endiann BCD 
  if (BroadCast)
    memset((unsigned char *)&pMsg->Header.Add[0],0xaa,6); 
  else 
  {// y.f. 29/09/2020 correct enter add to 
#ifdef ELECTRIC_SINK
    memcpy ((uint8_t*)&pMsg->Header.Add ,meteradd,6); // copy address 
#else    
    for (bInd = 0; bInd < 6; bInd++)
      //     pMsg->Header.Add[bInd] = EMConfigMacADDR0[bInd]; // y.f. 29/09/2020 correct enter add to 
      pMsg->Header.Add[bInd] = EMConfigMacADDR0[5-bInd];
#endif    
  }
  pMsg->Header.Len= DLTDdataLen+sizeof(DLTReadDataStruct);
  for (bInd = 0; bInd < DLTDdataLen; bInd++)
  {
     pMsg->Info[bInd] = pDLTDdata[bInd] +0x33;
  }
  for (bInd = 0; bInd < DLTDdataLen+sizeof(DLTWriteHeader)+sizeof(DLTReadDataStruct) ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->Info[DLTDdataLen] = CheckSum;
  pMsg->Info[DLTDdataLen+1] = 0x16;  // ETX
  if (usart == USART1)
         ST_RxTimeout = 1; // Flag indicates msg is send and wait for response if response is absent msg should be resend
      else
         DltRxTimeout = 1;
  TmpTxsizeBuf = pMsg->Header.Len +sizeof(DLTWriteHeader)+2;
  DltSendBuffer(usart,TmpTxBuf,TmpTxsizeBuf); 
     
}

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadParseDltRxMsg                                          */
/* - DESCRIPTION     : this routine parse the incoming message recived from Meter   */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     27/7/2011      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
uint8_t KeypadParseDltRxMsg(uint8_t   *pDLTMsg, uint8_t   *pDLTDdata, uint16_t *pRxMsgLen)
{
  uint8_t bInd;
  DLTReadStruct *pMsg;
  uint8_t CheckSum  =0;
//  long long aExtendedAddress=0;
  for (bInd = 0 ; bInd < MAX_UART_IN; bInd++)
  { // search for STX 
    pMsg = (DLTReadStruct*)(pDLTMsg+bInd);
    if (pMsg->Header.Stx == 0x68) break;
  }
  if (  bInd >=MAX_UART_IN ) return 0;  // no stx found 
 // if (  bInd <= 12  )  return 0;  // y.f. a11/08/2020 dd min for dlt message 
//#define SEND_ANY_WAY  

  // calculate C.S.
  for (bInd = 0 ; bInd < pMsg->Header.Len+ sizeof(DLTWriteHeader)  ; bInd++)
    CheckSum = CheckSum + ((unsigned char*) pMsg)[bInd];
  // c.s. error
  if (CheckSum != ((unsigned char *)pMsg)[pMsg->Header.Len+ sizeof(DLTWriteHeader)]) return 0;
  if (((unsigned char *)pMsg)[pMsg->Header.Len+ sizeof(DLTWriteHeader)+1] != 0x16) return 0;  
  // copy the rx data 
  *pRxMsgLen = pMsg->Header.Len+ sizeof(DLTWriteHeader)+2;
  memcpy ((unsigned char *)pDLTDdata, (unsigned char *) pMsg,*pRxMsgLen);
  return (1);
}



/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadSendWriteDltMsg                                            */
/* - DESCRIPTION     :                                                              */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     19/7/2011     						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void KeypadSendWriteDltMsg(uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid, uint8_t APMNum)
{ 
  uint8_t bInd;
  DLTWriteStruct *pMsg;
  uint8_t CheckSum  =0;
 
  pMsg = (DLTWriteStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  pMsg->Header.ReadWriteCmd = 0x14;
  
  // enter DI code
  for (bInd = 0; bInd < 4 ; bInd++)
  {
    pMsg->Data.DICode[3-bInd] = DLTID[DLTid*4+bInd];
    pMsg->Data.DICode[3-bInd] = pMsg->Data.DICode[3-bInd] +0x33;
    if ((APMNum) &&  bInd==2) pMsg->Data.DICode[1] += APMNum-1;
  }    
  // copy user name and password
  memcpy ((unsigned char*)&pMsg->Data.PasswordUser, (unsigned char*) &DLTPassUser,8);
  // Convert hex addres to little endiann BCD 
 
  for (bInd = 0; bInd < 6; bInd++)
      pMsg->Header.Add[bInd] = EMConfigMacADDR0[bInd];

  pMsg->Header.Len= DLTDdataLen+sizeof(DLTWriteDataStruct);
  for (bInd = 0; bInd < DLTDdataLen; bInd++)
  {
     pMsg->Info[bInd] = pDLTDdata[bInd] +0x33;
  }
  for (bInd = 0; bInd < DLTDdataLen+sizeof(DLTWriteHeader)+sizeof(DLTWriteDataStruct) ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->Info[DLTDdataLen] = CheckSum;
  pMsg->Info[DLTDdataLen+1] = 0x16;  // ETX
 
  DltRxTimeout = 1;
  TmpTxsizeBuf = pMsg->Header.Len +sizeof(DLTWriteHeader)+2;
  DltSendBuffer(USART0,TmpTxBuf,TmpTxsizeBuf);  
}

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadSendReadCommAddrssMsg                                 */
/* - DESCRIPTION     : this routine transmits requeres address of the E. METER towards  */
/*                     the PLC to E. meter                                          */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     3/11/2011     						    */
/*   Michael Lifshits                                                                */
/*                                                                                  */
/************************************************************************************/
void KeypadSendReadCommAddrssMsg(void)
{
  uint8_t bInd;
  DLTReadCommAddStruct *pMsg;
  uint8_t CheckSum  =0;
  pMsg = (DLTReadCommAddStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  pMsg->Header.ReadWriteCmd = DLT_READ_COMM_ADDRESS;
    for (bInd = 0; bInd < 6; bInd++)
      pMsg->Header.Add[5-bInd] = 0xaa;    
  pMsg->Header.Len= 0;
  for (bInd = 0; bInd < sizeof(DLTReadCommAddStruct) -2 ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->CheckSum = CheckSum;
  pMsg->Etx= 0x16;  // 
  DltRxTimeout = 1;	
  TmpTxsizeBuf = sizeof(DLTReadCommAddStruct);
  DltSendBuffer(USART0,TmpTxBuf,TmpTxsizeBuf); 
   
}


/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadCheckReadCommAddrssRespMsg                         */
/* - DESCRIPTION     : this routine check response of read addrerss command         */
/*                     and enter response to EMConfigMacADDR0 and axtendedaddr      */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     18/12/2018      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
uint32_t KeypadCheckReadCommAddrssRespMsg(uint8_t *TmpTxBuf)
{
  uint8_t bInd;
  long long bTmpAdd  =0;
  DLTWriteCommAddStruct *pMsg;
  pMsg =  (DLTWriteCommAddStruct *)TmpTxBuf;
  if (  pMsg->Header.ReadWriteCmd != DLT_READ_COMM_ADDRESS_RESPONSE) return 0;
  for(bInd=0;bInd<6;bInd++)
  {
    EMConfigMacADDR0[bInd] =  pMsg->Header.Add[5-bInd];                                
    bTmpAdd = bTmpAdd*100;
    bTmpAdd = bTmpAdd + ConvertBCD2Hex(pMsg->Header.Add[5-bInd]);     //   Convert add from bcd+0x33 to long long 
  }   
   //set_led(0, true);
   //set_led(1, true);
   if (bTmpAdd != ExtendedAddress)
   {
      ExtendedAddress = bTmpAdd;
   // temp removed till card is ready   WriteConfig2EEprom(); // update config flash with new address y.f. 31/10/2013 
      ReconfigureNode = true;
   } 
 // temp send 115200 good message received
   
#if 0
   // y.f. 24/12/2018 send message to terminal only for test from now on the rate is 115200 to suit the VCOM only rate allowed
    custom_uart_init(115200,false);  // y.f. uart for debug&meterDLT 
    uint8_t bufout[256];
    uint8_t address_in[20];
    ConvertBinToAscii(ExtendedAddress,address_in);
    memcpy(&bufout," msg received from meter ",25);
    memcpy(&bufout[25],address_in,12);
    bufout[38] = 0xd;
    bufout[39] = 0xa;
    Usart_sendBuffer(bufout,40);  
#endif
    
  
  
  
  return 1;
}

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : uint8_t KeypadCompareDltDI(uint8_t DLTid)   		    */
/* - DESCRIPTION     : this routine compares DI field in receiver buffer with       */
/*                     parameter DLTid                                              */
/* - INPUT           : DLTid                                                        */
/* - OUTPUT          : 0 if equals, 1 if notequal                                   */
/* - CHANGES         :                                                              */               											    
/*                                                                                 */
/************************************************************************************/
uint8_t KeypadCompareDltDI(uint8_t DLTid, uint8_t *RxData)
{
	uint8_t result = 0;
	uint8_t bInd;
	DLTReadStruct *pMsg;
	pMsg = (DLTReadStruct*)RxData;

	for (bInd = 0; bInd < 4 ; bInd++)
	{
	  if(pMsg->Data.DICode[3-bInd] != (uint8_t)(DLTID[DLTid*4+bInd] +0x33)){
		result = 1;
		break;
	  }
	} 
	return result;
}


#if 0

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadSendReadDltMsg2                                        */
/* - DESCRIPTION     : The message with DLTid = 2bytes(old protocol)                */
/* - INPUT           : pDltPayload. Len of string                                   */
/* - OUTPUT          : none.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     01/05/2012   						    */
/*   Michael Lifshits                                                                */
/*                                                                                  */
/************************************************************************************/

void KeypadSendReadDltMsg2(uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid,  uint8_t BroadCast)
{ 
  uint8_t bInd;
  DLTReadStruct *pMsg;
  uint8_t CheckSum  =0;
  

  pMsg = (DLTReadStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  pMsg->Header.ReadWriteCmd = 0x01;
  
  // enter DI code

  pMsg->Data.DICode[0] = DLTID[DLTid*4];
  pMsg->Data.DICode[1] = DLTID[DLTid*4+1];

   // Convert hex addres to little endiann BCD 
  for (bInd = 0; bInd < 6; bInd++) {
      pMsg->Header.Add[bInd] = EMConfigMacADDR0[bInd];
  }
  pMsg->Header.Len= DLTDdataLen+sizeof(DLTReadDataStruct)-2;
//  for (bInd = 0; bInd < DLTDdataLen; bInd++)
//  {
//     pMsg->Info[bInd] = pDLTDdata[bInd] +0x33;
//  }
  for (bInd = 0; bInd < DLTDdataLen+sizeof(DLTWriteHeader)+sizeof(DLTReadDataStruct) - 2; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
//  pMsg->Info[DLTDdataLen] = CheckSum;
//  pMsg->Info[DLTDdataLen+1] = 0x16;  // ETX
  pMsg->Data.DICode[2] = CheckSum;
  pMsg->Data.DICode[3] = 0x16;  // ETX
  
  DltRxTimeout = 1;	
  TmpTxsizeBuf = pMsg->Header.Len +sizeof(DLTWriteHeader)+2;
#if 0  
  uartWrite(TmpTxBuf, pMsg->Header.Len +sizeof(DLTWriteHeader)+2);
  while (Txindex < TxLimit);
     UartPLCReadInit();
#endif
  DltSendBuffer(USART0,TmpTxBuf,TmpTxsizeBuf);     
}

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadSendResponseDltRxMsg                                   */
/* - DESCRIPTION     : this routine transmits response message to E. meter          */
/* - INPUT           : pDltPayload. Len of string                                   */
/*                   : pRxMsg RX message to witch the response refairs.             */
/* - OUTPUT          : tx buf .                                                     */
/* - CHANGES         :                 						    */
/* - CREATION     27/7/2011     						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
uint8_t KeypadSendResponseDltRxMsg(uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t ReadWtite,uint8_t DLTid)
{
  uint8_t bInd;
  DLTReadStruct *pMsg;
  uint8_t CheckSum  =0;
  

  pMsg = (DLTReadStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  if (ReadWtite == DLT_READ_COMMAND)
    pMsg->Header.ReadWriteCmd = 0x11;
  else
    pMsg->Header.ReadWriteCmd = 0x14;
  for (bInd = 0; bInd < 6; bInd++)
      pMsg->Header.Add[5-bInd] = EMConfigMacADDR0[bInd];
   // enter DI code
  for (bInd = 0; bInd < 4 ; bInd++)
  {
    pMsg->Data.DICode[3-bInd] = DLTID[DLTid*4+bInd];
    pMsg->Data.DICode[3-bInd] = pMsg->Data.DICode[3-bInd] +0x33;
 
  }   
   pMsg->Header.Len= DLTDdataLen+sizeof(DLTReadDataStruct);
  for (bInd = 0; bInd < DLTDdataLen; bInd++)
  {
     pMsg->Info[bInd] = pDLTDdata[bInd] +0x33;
  }
  for (bInd = 0; bInd < DLTDdataLen+sizeof(DLTWriteHeader)+sizeof(DLTReadDataStruct) ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->Info[DLTDdataLen] = CheckSum;
  pMsg->Info[DLTDdataLen+1] = 0x16;  // ETX
  DltRxTimeout = 1;	
  TmpTxsizeBuf = pMsg->Header.Len +sizeof(DLTWriteHeader)+2;
#if 0 
  uartWrite(TmpTxBuf, pMsg->Header.Len +sizeof(DLTWriteHeader)+2);
    while (Txindex < TxLimit);
     UartPLCReadInit();
#endif
  DltSendBuffer(USART0,TmpTxBuf,TmpTxsizeBuf);   
  return 1;
}


/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadSendReadCommAddrssResponseMsg                          */
/* - DESCRIPTION     : this routine transmits Comm address of the E. METER towards  */
/*                     the PLC to E. meter                                          */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     2/08/2011     						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
void KeypadSendReadCommAddrssResponseMsg(uint8_t writeOrResponse)
{
  uint8_t bInd;
  DLTWriteCommAddStruct *pMsg;
  uint8_t CheckSum  =0;
  const uint8_t selfAddress[] = {0x12,0x34,0x56,0x78,0x34,0x56};
  
  
  pMsg = (DLTWriteCommAddStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  if (writeOrResponse )
  {
    pMsg->Header.ReadWriteCmd = DLT_WRITE_COMM_ADDRESS;
    for (bInd = 0; bInd < 6; bInd++)
      pMsg->Header.Add[5-bInd] = 0xaa;    
  }    
  else
  {
    pMsg->Header.ReadWriteCmd = DLT_READ_COMM_ADDRESS_RESPONSE;
#if 0 
    for (bInd = 0; bInd < 6; bInd++)
//      pMsg->Header.Add[bInd] = EMConfigMacADDR0[bInd];  
	  pMsg->Header.Add[5-bInd] = EMConfigMacADDR0[bInd]; 
#else 
	for (bInd = 0; bInd < 6; bInd++)
	  pMsg->Header.Add[bInd] = selfAddress[bInd];
#endif
  }
  
  pMsg->Header.Len= 6;
  for (bInd = 0; bInd < 6; bInd++)
  {
//    pMsg->Add[5-bInd] = EMConfigMacADDR0[bInd];
#if 0
	pMsg->Add[bInd] = EMConfigMacADDR0[bInd] + 0x33;
#else
	pMsg->Add[bInd] = selfAddress[bInd] + 0x33;
#endif
  }
  for (bInd = 0; bInd < sizeof(DLTWriteCommAddStruct) -2 ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
 
#if 0
  pMsg->Add[6] = CheckSum;
  pMsg->Add[7] = 0x16;  // ETX
#else
  pMsg->CheckSum = CheckSum;
  pMsg->Etx = 0x16;  // ETX 
#endif
  DltRxTimeout = 1;	
  TmpTxsizeBuf = pMsg->Header.Len + sizeof(DLTWriteCommAddStruct);
#if 0   
  uartWrite(TmpTxBuf, sizeof(DLTWriteCommAddStruct));
  
    while (Txindex < TxLimit);
     UartPLCReadInit();
#endif
     
  DltSendBuffer(USART0,TmpTxBuf,TmpTxsizeBuf); 
}



#endif
#ifdef ELECTRIC_SINK  
#ifdef ST_FLASH
uint32_t HandleRxFromST(uint8_t *pDLTMsg, uint16_t len)
{
    uint8_t btmp[256];
    uint16_t size;
    size=len;
    uint8_t bInd;
    long long address=0; 
    DLTReadStruct *pMsg;
   //   Usart_sendBuffer(pDLTMsg,len); // 7/4/21 temp to be removed 
    if ( KeypadParseDltRxMsg(pDLTMsg, btmp, &size))
    { // if message is request message send trasparent to meter
      // if message is response message send message to DCU
        pMsg =(DLTReadStruct*) btmp;
        if(  pMsg->Header.ReadWriteCmd == DLT_READ_COMMAND_RESPONSE) 
        
                  Usart_sendBuffer((unsigned char *) btmp,size); 
        else
        {
          for (bInd =0 ; bInd<6 ; bInd++)
          {
              address = address*100;
              address = address + ConvertBCD2Hex(pMsg->Header.Add[5-bInd]);     //   Convert add from bcd to long long 
          }
          Hour_Req_Send_2Meter = 30; // 18/05/21 Indication that hour was not found and the FUTURE response is to send to DCU 
          WPSendMsg((char*)btmp, size,address % 100000);  // y.f. 27/06/2019
  //         Usart_sendBuffer("\r\n not DLT_READ_COMMAND_RESPONSEfrom st\r\n",34);
        }
    } 
 //   else
 //     Usart_sendBuffer("\r\nbad message receved from st\r\n",31);
    return 1;
}
#endif
#endif

uint32_t HandleRxFromUART0(uint8_t *pDLTMsg, uint16_t len)
{
    uint8_t btmp[256];
    uint16_t size;
    size=len;
#ifdef ELECTRIC_SINK
    uint8_t bInd;
    bool broadcastadd = true;
    long long address=0; 
    DLTReadStruct *pMsg;
//    uint8_t bufout[256];
//   uint8_t address_in[20];
#endif
    
    if ( KeypadParseDltRxMsg(pDLTMsg, btmp, &size))
    {
      switch ( DisplayState)
      {
          case DLTREAD_EM_SERIAL_NUMBER_1200:
          case DLTREAD_EM_SERIAL_NUMBER_2400:
          case DLTREAD_EM_SERIAL_NUMBER_4800:
          case DLTREAD_EM_SERIAL_NUMBER_9600:
            
           if (KeypadCheckReadCommAddrssRespMsg(btmp))
           {
                DisplayState =DLT_IDLE_STATE; // enter idle state 
               
           }     
  
        
                
           break;
            
            default:
                      
             // 23/09/2020  data reading process first read hour time and then read data readings message 
           {
#ifdef ELECTRIC_METER
#ifdef PUSH           
             
              uint8_t tmpHour;
              tmpHour = KeypadCheckReadTimeRespMsg(btmp);
              if (tmpHour!=0xff )
              {
                Hour = tmpHour;
                if (GetPushTest()==true) return 1; // this message is for internal affairs not to be sent to sink 
              }
#endif   
#endif              
           }
              // y.f. send all messages transparent to the mesh net node/sink


              // we have to extract the destination hex address for the mesh header
#ifdef ELECTRIC_SINK      
#ifdef PUSH     
               // y.f. 22/04/21 send only current day req to st last day is send direct to RF 
               if ((KeypadCompareDltDI(DLTREAD_EM_READ_DATA_READINGS, btmp)==0) && ((btmp[14] == 0x33))) 
               {// send response from logger if exists 
#ifdef ST_FLASH
                 // send transparent messag to st St will search message and send response back
                 // if ST find the record it will send back response message that silicon will send transparent to DCU.
                 // if ST SEND REQ message it will send transparent in RF to the specified meter
                  uint32_t time = lib_time->getTimestampCoarse(); //y.f. 13/04/21 add time stamp to record 
                  memcpy ( &btmp[size], (uint8_t*) &time,4); 
                  Usart1_sendBuffer((unsigned char *) btmp,size+4); 
                  return 1; // no need to send to meter in this stage
#else                 
                  if (PrepareResFromLogger (btmp ) )
                  { // send response to DCU with the proper meter and time
                    return 1; // no need to send to meter if record is found 
                  }
                  else
                  {
                       //Usart_sendBuffer("record was not found",19);
                    // y.f. for debug only to be remove 12/10/2020
                  }
#endif
                  
               }
#ifdef ST_FLASH
              // Y.F. 22/04/21 /reset flash commnd direct to st and not to RF
              else
                   if (KeypadCompareDltDI(DLTSET_ST_RESET, btmp)==0)
                   {
                     Usart1_sendBuffer((unsigned char *) btmp,size+4); 
                     return 1;
                   }
#endif                 
#endif                   
               pMsg =(DLTReadStruct*) btmp;
               for (bInd =0 ; bInd<6 ; bInd++)
               {
                  address = address*100;
                  address = address + ConvertBCD2Hex(pMsg->Header.Add[5-bInd]);     //   Convert add from bcd to long long 
                  if (pMsg->Header.Add[5-bInd]!= 0xaa)
                    broadcastadd=false;
                  
               }
               // Check if broadcast meter is requested if so send wirepas broadcast message 
               if (broadcastadd == true)
                                                  
                WPSendMsg((char*)btmp, size,APP_ADDR_BROADCAST);
               else
//                WPSendMsg((char*)btmp, size,address & 0xffffff);
				{
                  WPSendMsg((char*)btmp, size,address % 100000);  // y.f. 27/06/2019
	//			     Usart_sendBuffer((char*)btmp,size);
				}               
               
#else
                 // node is sending all messages to sink
                WPSendMsg((char*)btmp, size,APP_ADDR_ANYSINK);
               
#endif               
            break;
      }
  
    }
    else
    {
#ifdef DEBUG_MODE              
      
        Usart_sendBuffer("wrong message received \r\n",26);
#endif        
        return 0;
    } 

    return 1;
}
         
uint32_t HandleRxFromRF (uint8_t   *pDLTMsg, uint16_t len)
{
  uint8_t bInd;
  DLTReadStruct *pMsg;
  uint16_t size;
  uint8_t CheckSum  =0;
  for (bInd = 0 ; bInd < MAX_UART_IN; bInd++)
  { // search for STX 
    pMsg = (DLTReadStruct*)(pDLTMsg+bInd);
    if (pMsg->Header.Stx == 0x68) break;
  }
  if (  bInd >=MAX_UART_IN ) return 0; // no stx found 
  // calculate C.S.
  for (bInd = 0 ; bInd < pMsg->Header.Len+ sizeof(DLTWriteHeader)  ; bInd++)
    CheckSum = CheckSum + ((unsigned char*) pMsg)[bInd];
  // c.s. error
  if (CheckSum != ((unsigned char *)pMsg)[pMsg->Header.Len+ sizeof(DLTWriteHeader)]) return 0;
  if (((unsigned char *)pMsg)[pMsg->Header.Len+ sizeof(DLTWriteHeader)+1] != 0x16) return 0;
  size = pMsg->Header.Len+ sizeof(DLTWriteHeader)+2;
  if (size > len) return 0; // too few bytes in message received
#ifdef PUSH
#ifdef ELECTRIC_SINK
  // y.f. 22/04/21 enter only current day to flash (btmp[14] == 0x33)
    if  ((KeypadCompareDltDI(DLTREAD_EM_READ_DATA_READINGS, (uint8_t*)pMsg) ==0)&& ((pMsg->Info[0] == 0x33))) 
 
   // if (  pMsg->Header.ReadWriteCmd == DLTREAD_EM_READ_DATA_READINGS)
    {
      if (WriteNewMeterRecord((uint8_t *)pMsg,size)) // if using external flash msg is send transparent to ST
      {
 
        {
          DCUSendReadDltMsg2Meter((uint8_t*)&pMsg->Header.Add,0,0,DLTREAD_EM_READ_DATA_READINGS_ACK, 0);
          
        }
        
      }
  
      // y.f. 18/05 check if this message was  request from DCU only then proceed to send message to DCU else enter only to flash 
      // This is req from Yaniv
 //     if (Hour_Req_Send_2Meter==0) return 1; // y.f. 4/08/21 resume direct push all messages come from meter send to dcu
//      else
        Hour_Req_Send_2Meter=0;
    } // y.f. 13/04/2021 add also erase commnad to erase flash in st 
    else
    if  (KeypadCompareDltDI(DLTSET_ST_RESET, (uint8_t*)pMsg) ==0)
    { // y.f 22/04/21 send erase chip direct to st and not to rf 
          Usart1_sendBuffer((unsigned char *) pMsg,size);
 //         DCUSendReadDltMsg2Meter((uint8_t*)&pMsg->Header.Add,0,0,DLTSET_ST_RESET, 0);
          return (1);
    }  
    
#else  
  if  (KeypadCompareDltDI(DLTREAD_EM_READ_DATA_READINGS_ACK,(uint8_t*) pMsg) ==0)
//  if (  pMsg->Header.ReadWriteCmd == DLTREAD_EM_READ_DATA_READINGS_ACK)
  {// y.f. 23/09/2020 this message purpose is to stop sending mesges to Sink
    
    Ackdcu=true;
    return (1);  // This message is not transfer to meter it is for push task only 
  } 
  if  (KeypadCompareDltDI(DLTREAD_EM_READ_NEIGHBOUR_LIST,(uint8_t*) pMsg) ==0)
  {
    DltGeTnbors();  // send neigours to  
    return 1;
  }
#endif 
#endif  
  
    DltRxTimeout = 1;
    TmpTxsizeBuf = size;
    
    DltSendBuffer(USART0,(unsigned char *) pMsg,size);
  
    return (1);   
 }

void DltSendBuffer(USART_TypeDef *usart, uint8_t *TmpTxBuf,uint32_t DataSize)
{
 // Padding 2 0xfe at the beginnig of message   
  uint8_t TxBuf[256];
  memcpy ((uint8_t *)&TxBuf[2], TmpTxBuf, DataSize);
  TxBuf[0] = 0xfe;
  TxBuf[1] = 0xfe;
  if (usart== USART0)
    Usart_sendBuffer((unsigned char *) TxBuf,DataSize+2);
  else
    Usart1_sendBuffer((unsigned char *) TxBuf,DataSize+2); 
}
#ifdef PUSH
          
void SendDltReadTime(USART_TypeDef *usart)
{
    KeypadSendReadDltMsg2Meter(usart,0,0,0,DLTREAD_EM_TIME_ONLY, 0);

}
void SendStDLTRSTmsg(void)
{
   KeypadSendReadDltMsg2Meter(USART1,0,0,0,DLTSET_ST_RESET, 0);
}

void SendDataReadingsReq2meter(USART_TypeDef *usart,uint8_t *timeIndex)
{
   KeypadSendReadDltMsg2Meter(usart,0,timeIndex,2,DLTREAD_EM_READ_DATA_READINGS, 0);
}
          
bool GetDataReadingsACK(void)
{
  if (Ackdcu) 
    return true;
  else
    return false;
}
uint8_t GetTimeFromMeter(void)
{
  return Hour;
}

void ClrHour(void)
{
  Hour= 0xff;
}

void ClrDataReadingsACK(void)
{
  Ackdcu= false;
}

/************************************************************************************/
/*                                                                                  */
/* - FUNCTION        : KeypadCheckReadTimeRespMsg                                   */
/* - DESCRIPTION     : this routine check response of read time command             */
/*                     if time command res has received : return hour from message  */
/* - INPUT           : None                                                         */
/* - OUTPUT          : None.                                                        */
/* - CHANGES         :                 						    */
/* - CREATION     30/09/2020      						    */
/*   Ferlandes Yossi                                                                */
/*                                                                                  */
/************************************************************************************/
uint8_t KeypadCheckReadTimeRespMsg(uint8_t *TmpTxBuf)
{
  
 DLTReadStruct *pMsg;
 pMsg =  (DLTReadStruct *)TmpTxBuf;
  if (KeypadCompareDltDI(DLTREAD_EM_TIME_ONLY, TmpTxBuf))
    return 0xff;
  return  ConvertBCD2Hex(pMsg->Info[2]-0x33); 
}



#endif

          
bool CovertBcd2Add(uint8_t *BtmpIn ,   long long  * Maddress)
{

  for (uint8_t bInd =0 ; bInd<6 ; bInd++)
  {
    *Maddress = *Maddress*100;
    *Maddress = *Maddress + ConvertBCD2Hex(BtmpIn[5-bInd]);     //   Convert add from bcd to long long 
  }  
  return true;
}
          
          