/****************************************************************************************
* ModeBusProtocol.H 																			   	*
* DESCRIPTION:																		   	*
*   This file contains ALL 	ModeBusProtocol packet exported routints and definitions .  
*
*																					   	*
* HISTORY																			 	*
*   Ferlandes Yossi   22/11/2009     Created										  		*
****************************************************************************************/
#ifndef _DLT_PROTOCOL_H
#define _DLT_PROTOCOL_H

#undef PUBLIC
#ifdef DLT_H
#define  PUBLIC
#else
#define  PUBLIC  extern
#endif

#pragma pack(1)
#define DLT_READ_COMMAND                0x11
#define DLT_READ_COMM_ADDRESS           0x13
#define DLT_WRITE_COMMAND               0x14
#define DLT_WRITE_COMM_ADDRESS          0x15
#define DLT_READ_COMMAND_RESPONSE  	0x91
#define DLT_READ_COMM_ADDRESS_RESPONSE  0x93
#define DLT_MAX_RETRIES 10
#define MAX_INFO_LEN                    150 // Y.F. 22/10/2020 WAS 50 


typedef struct  {
          uint8_t DICode[4];
          uint8_t PasswordUser[8];
        
}DLTWriteDataStruct;
typedef struct  {
          uint8_t DICode[4];
        
}DLTReadDataStruct;

typedef struct  {
  uint8_t Stx;
  uint8_t Add[6];
  uint8_t Stx1;
  uint8_t ReadWriteCmd;
  uint8_t Len;
}DLTWriteHeader;
typedef struct  {
  uint8_t CheckSum;
  uint8_t Etx;
}DLTTail;

typedef struct  {
    DLTWriteHeader Header;
    DLTWriteDataStruct  Data;
    uint8_t Info[MAX_INFO_LEN];
}DLTWriteStruct; 

typedef struct  {
    DLTWriteHeader Header;
    DLTReadDataStruct  Data;
    uint8_t Info[MAX_INFO_LEN];
}DLTReadStruct; 

typedef struct  {
    DLTWriteHeader Header;
    uint8_t Add[6];
    uint8_t CheckSum;
    uint8_t Etx;
}DLTWriteCommAddStruct; 


typedef struct  {
    DLTWriteHeader Header;
    uint8_t CheckSum;
    uint8_t Etx;
}DLTReadCommAddStruct; 


typedef enum
{
  DLT_IDLE_STATE =0,
  DLTREAD_EM_SERIAL_NUMBER_1200,
  DLTREAD_EM_SERIAL_NUMBER_2400,
  DLTREAD_EM_SERIAL_NUMBER_4800,
  DLTREAD_EM_SERIAL_NUMBER_9600
}DltState;

  
typedef enum
{
  
 DLTREAD_WM_READING_DATA= 0,
 DLTREAD_GM_READING_DATA,
 DLTREAD_WM_PREVIOUS_DATA,
 DLTREAD_GM_PREVIOUS_DATA,
 DLTREAD_APM_READING_DATA,
 DLTREAD_APM_EVENT_LOG,
 DLTWRITE_RF_PARAMETER,
 DLTWRITE_WM_GM_TRANSMIT_TIME_INTERVAL,
 DLTWRITE_WM_GM_DATE_TIME,
 DLTWRITE_KEYPAD_PASSWORD,
 DLTWRITE_READ_APM_RELAY,			//10
 DLTWRITE_APM_RELAY_SCHEDULE_SINGLE,
 DLTWRITE_APM_RELAY_SCHEDULE_DAILY,
 DLTWRITE_APM_POWER_LIMIT_NORMAL,
 DLTWRITE_APM_POWER_LIMIT_CRITICAL,
 DLTWRITE_APM_LOAD_CONTROL_MODE,
 DLTWRITE_APM_DIGITAL_I_O,
 DLTREAD_APM_EVENT_LOG_1,
 DLTREAD_APM_SWITCH_TIME,
 DLTREAD_APM_ENERGY_CONSUMPTION,
 DLTREAD_APM_VERSION,				//20
 DLTWRITE_READ_APM_DATE_AND_TIME,
 DLTWRITE_READ_APM_DEMAND_INTERVAL,
 DLTREAD_APM_CURRENT_DEMAND,
 DLTREAD_APM_CURRENT_TARIFF_PERIOD,
 DLTREAD_APM_ENERGY_COST,
 DLTREAD_APM_REGISTER,
 DLTWRITE_DLTREAD_APM_REGISTER,
 DLTREAD_APM_VOLTAGE,
 DLTREAD_APM_CURRENT,
 DLTREAD_APM_POWER_FACTOR,			//30
 DLTREAD_EM_ENERGY_DATA,
  DLTREAD_EM_TARIF_INDEX,
  DLTREAD_EM_SERIAL_NUMBER,
  DLTREAD_EM_DATE_AND_TIME,
  DLTREAD_EM_POWER,
  DLTREAD_EM_ENERGY_TOTAL_CONSUMPTION_CURRENT_MONTH,
  DLTREAD_EM_ENERGY_TOTAL_CONSUMPTION_PREVIOUS_MONTH,
  DLTREAD_EM_ENERGY_COST_CURRENT_MONTH,
  DLTREAD_EM_ENERGY_COST_PREVIOUS_MONTH,
  DLTREAD_WM_TARIF,					//40
  DLTREAD_GM_TARIF,
  DLTREAD_POWER_LIMIT_NORMAL,
  DLTREAD_POWER_LIMIT_CRITICAL,
  DLTREAD_EM_EVENT_LOG,
  DLTREAD_EM_BALANCE_CREDIT,
  DLTREAD_EM_LAST_CHARGE_CREDIT_AND_TOKEN,
  DLTREAD_EM_TIME_ONLY,
  DLTREAD_EM_STS_PARAMETER,
  DLTWRITE_EM_STS_PARAMETER,
  DLTREAD_EM_METER_FREQ,				//50
  DLTREAD_EM_TARIF_NUMBER,
  DLTREAD_EM_CURRENT_TARIF,
  DLTREAD_EM_LAST_CREDIT_AND_TOKEN_DATE,
  DLTREAD_EM_SW_VERSION,
  DLTREAD_EM_PREPAYMENT_ON,                            // Y.F. 17/02/2014 ADD prepayment available for choosing between enerjy or credit 
  DLTREAD_EM_READ_DATA_READINGS,                       // Y.F. 22/09/2020 Add data readings comnd for push mechanism in WP system 
  DLTREAD_EM_READ_DATA_READINGS_ACK,                    // Y.F. ADD new dlt commnd not from standart dlt. "ACKD" 
  DLTREAD_EM_READ_NEIGHBOUR_LIST,                       // Y.F. ADD new dlt commnd not from standart dlt. NEIGHBOUR LIST 
  DLTSET_ST_RESET ,                                      // y.f. 18/11/2020 new DLT commanf between Silicon and St
  DLTREAD_EM_LOAD_PROFILE                               // 60   y.f. 2/12/2020  
   
}DIname;
PUBLIC void SendDltReadTime(USART_TypeDef *usart);  // Y.F. 23/09/2020 add data readings push process
PUBLIC void SendDataReadingsReq2meter(USART_TypeDef *usart,uint8_t *timeIndex);  // Y.F. 23/09/2020 add data readings push process
PUBLIC bool GetDataReadingsACK(void);  // Y.F. 23/09/2020 add data readings push process
PUBLIC void ClrDataReadingsACK(void); // Y.F. 23/09/2020 add data readings push process
PUBLIC uint8_t GetTimeFromMeter(void);   // Y.F. 23/09/2020 add data readings push process
PUBLIC void ClrHour(void);         // Y.F. 23/09/2020 add data readings push process

PUBLIC void KeypadSendReadCommAddrssMsg(void);
PUBLIC uint8_t KeypadParseDltRxMsg(uint8_t   *pDLTMsg, uint8_t   *pDLTDdata, uint16_t *pRxMsgLen);

PUBLIC  unsigned char DisplayState;
PUBLIC  uint8_t EMConfigMacADDR0[6];
PUBLIC  uint32_t  DltRxTimeout; 
PUBLIC  uint32_t  ST_RxTimeout; 
PUBLIC  uint8_t TmpTxBuf[MAX_UART_IN];  // keep last message buffer for rfetry
PUBLIC  uint32_t TmpTxsizeBuf;          // keep last message size 
PUBLIC  uint32_t HandleRxFromUART0(uint8_t   *pDLTMsg, uint16_t len);
#ifdef ELECTRIC_SINK  
#ifdef ST_FLASH
PUBLIC  uint32_t HandleRxFromST(uint8_t   *pDLTMsg, uint16_t len);
#endif
#endif
PUBLIC  uint32_t HandleRxFromRF (uint8_t   *pDLTMsg, uint16_t len);
PUBLIC  void DltSendBuffer(USART_TypeDef *usart,uint8_t *TmpTxBuf,uint32_t DataSize);
PUBLIC uint32_t KeypadCheckReadCommAddrssRespMsg(uint8_t *TmpTxBuf);

#ifdef  PUBLIC
PUBLIC   bool ReconfigureNode;
#else
   bool ReconfigureNode=false;
#endif   


#if 0

PUBLIC  void KeypadSendWriteDltMsg(uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid, uint8_t APMNum);
PUBLIC  void KeypadSendReadDltMsg2(uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid,uint8_t broadcast);
PUBLIC  uint8_t PrepaymentAvilable;

#endif

#endif
