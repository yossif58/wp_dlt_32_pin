/****************************************************************************************
* WaterCalc.h 																			*
* DESCRIPTION:																		   	*
*   This file contains ALL POWERCOM new gasmeter eeprom exported functions and vars     *
* HISTORY																			 	*
*   Ferlandes Yossi  1/11/2018   Created		     								  		*
****************************************************************************************/
#ifndef _EEPROML_H_
#define _EEPROML_H_
#undef PUBLIC

#ifdef EEPROM_H
#define  PUBLIC
#else
#define  PUBLIC  extern
#endif
//#define tamper_interrupt
#pragma pack(1)
#define PUSH
//#define ST_FLASH
//#define DEBUG_MODE
//#define ELECTRIC_SINK
//#define REPEATER
//#define NO_METER_1
//#define NO_METER_2
#define ELECTRIC_METER
//#define ELECTRIC_SINK
//#define GAS_METER
//#define GAS_SINK


#define EEPROM_PAGE_SIZE	 32
#define CONFIG_ADDRESS		 0x1000
#define LOGGER_ADDRESS           0x1100
#define SHIFT_ENABELE		 0x55

typedef struct {
	long long 	aExtendedAddress;
	uint32_t 	MeterWakeUpTimeInterval;
	uint32_t 	MeterLoggerInterval;
	uint8_t 	ShipmentDisable;
	uint8_t 	FactorType;
	uint16_t	Eeprom_LoggerIndex;
	uint16_t 	crc;
} WaterMeterConfigStruct;


uint8_t NodeType; 
L2_WaterMeterLoggerMsg TmpLoggerItem;
uint16_t TmpLoggerIndex;
// only for test RAM avilable uint8_t LoggerBuf[15008];

WaterMeterConfigStruct sMyConfig;
#if 0
y.f. 22/04/2020 removed till card is with flash
PUBLIC void setupI2C(void);
PUBLIC void WriteConfig2EEprom(void);
PUBLIC void WriteConfigDefault(void);
PUBLIC void ReadConfigEEprom(void);
PUBLIC void WriteLogger2Eeprom(uint16_t loggerIndex);
PUBLIC void ReadLoggerEeprom (uint16_t loggerIndex);
#endif



#endif






