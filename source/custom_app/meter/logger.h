/****************************************************************************************
* WaterCalc.h 																			   	*
* DESCRIPTION:																		   	*
*   This file contains ALL POWERCOM new watermetr consumption calculation  exported routints and definitions .       *
*																					   	*
* HISTORY																			 	*
*   Ferlandes Yossi  5/8/2012   Created										  		*
****************************************************************************************/
#ifndef _LOGGER_H_
#define _LOGGER_H_
#undef PUBLIC

#ifdef LOGGER_H
#define  PUBLIC
#else
#define  PUBLIC  extern
#endif

#pragma pack(1)
#define  MAX_RECORDS 24
typedef struct {
	uint8_t  Consumption;
    uint8_t  FactorType;
    uint8_t  Status;
    app_lib_time_timestamp_coarse_t TimeDate;

} loggerStruct;

typedef struct L2_WaterMeterLoggerMsg {
    
    loggerStruct Data;   
    uint8_t ReadFlag;
    }L2_WaterMeterLoggerMsg;


PUBLIC uint8_t  loggerIndex;
PUBLIC uint8_t  LastLoggerIndexSend;
PUBLIC L2_WaterMeterLoggerMsg WaterLogger[MAX_RECORDS];
PUBLIC void AddItem2Logger(void);
PUBLIC void MarkReadItem(uint8_t bIndex);
PUBLIC void MarkUnReadItem( uint8_t bIndex );
PUBLIC uint8_t GetUnreadRecordFromLogger(void);
PUBLIC void InitWaterLogger(void);
PUBLIC void LoggerAckMsg(void);
PUBLIC void ReadLoggerProcess(uint8_t Records);



#endif
