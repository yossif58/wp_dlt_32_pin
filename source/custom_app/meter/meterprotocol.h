/****************************************************************************************
* WaterCalc.h 																			   	*
* DESCRIPTION:																		   	*
*   This file contains ALL POWERCOM new watermetr consumption calculation  exported routints and definitions .       *
*																					   	*
* HISTORY																			 	*
*   Ferlandes Yossi  5/8/2012   Created										  		*
****************************************************************************************/
#ifndef _METER_PROTOCOL_H_
#define _METER_PROTOCOL_H_
#undef PUBLIC

#ifdef METER_PROTOCOL_H
#define  PUBLIC
#else
#define  PUBLIC  extern
#endif
//#define tamper_interrupt
#pragma pack(1)

#ifdef  PUSH_DEBUG
typedef struct  {
    uint8_t Ready2Send;
    uint8_t Delay;
    uint8_t size;
    uint8_t info[150];
}Debugpushstruct; 

PUBLIC Debugpushstruct debufsend[4];


#endif


PUBLIC void SendPeriodicMsg(void);
PUBLIC void WPSendMsg(char *DataIn, int16_t Size,uint32_t address);

#endif
