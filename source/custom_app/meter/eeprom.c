#include "string.h"
#include "board.h"
#include "mcu.h"
#include "vendor/efr32fg13/efr32_gpio.h"
#include "api.h"
#include "app.h"
#include "metervalve.h"
#include "WaterCalc.h"
#include "em_i2c.h"
#include "crc.h"
#include "logger.h"
#include <stdlib.h>
#define EEPROM_H
#include "eeprom.h"
#include "led.h"
#pragma pack(1)

void __attribute__((__interrupt__)) I2C0_IRQHandler(void);
void performI2CTransfer(uint8_t read_write, uint8_t size);
void EepromWriteBuf(uint16_t Address, uint8_t *DataIn, uint8_t size);
void EepromReadBuf(uint16_t Address, uint8_t size);




static uint8_t EepromState;
static uint16_t oldEEpromlogger_index;


uint8_t i2c_rxBufferIndex;
uint8_t i2c_rxBuffer[EEPROM_PAGE_SIZE];
uint8_t i2c_txBuffer[EEPROM_PAGE_SIZE+2];

typedef enum
{
	EEPROM_IDLE =0,
	READ_CONFIG,
	WRITE_CONFIG,
	READ_LOGGER_ITEM,
	WRITE_LOGGER_ITEM
}I2CSTATEMACHIN ;



#define EEPROM_ADDRESS 		0xa0
static uint8_t EepromState;

void setupI2C(void) {
	// y.f. add i2c0 for EEprom
	// Using default settings
	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	/* Enabling clock to the I2C*/
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_I2C0;
	/* Using PA0 (SDA) and PA1 (SCL) */
	GPIO_PinModeSet(gpioPortA, 0, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortA, 1, gpioModeWiredAndPullUpFilter, 1);
	/* Enable pins at location 0 */
	I2C0->ROUTE = I2C_ROUTE_SDAPEN |
	I2C_ROUTE_SCLPEN | (I2C_ROUTE_LOCATION_LOC0);
	I2C0->CTRL |= I2C_CTRL_AUTOACK | I2C_CTRL_AUTOSN;
	I2C0->CTRL |= I2C_CTRL_AUTOACK | I2C_CTRL_AUTOSN;
	/* Initializing the I2C */
	I2C_Init(I2C0, &i2cInit);
	/* Register I2C interrupt to stack  */
	Sys_clearFastAppIrq(I2C0_IRQn);
	Sys_enableFastAppIrq(I2C0_IRQn,
			 APP_LIB_SYSTEM_IRQ_PRIO_HI,
			 I2C0_IRQHandler);
	/* Setting the status flags and index */
	EepromState = EEPROM_IDLE;

	I2C_IntClear(I2C0,  I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
	I2C_IntEnable(I2C0, I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
	NVIC_EnableIRQ(I2C0_IRQn);

}


void EepromWriteBuf(uint16_t Address, uint8_t *DataIn, uint8_t size) {

	i2c_txBuffer[0] = Address >> 8 ;   // high address byte first;
	i2c_txBuffer[1] = Address & 0xff;
	memcpy(&i2c_txBuffer[2], DataIn, size);
	performI2CTransfer(I2C_FLAG_WRITE, size+2);


}
void EepromReadBuf(uint16_t Address, uint8_t size)
{
	i2c_txBuffer[0] = Address >> 8 ;   // high address byte first;
	i2c_txBuffer[1] = Address & 0xff;
	//performI2CTransfer(I2C_FLAG_WRITE_READ, size); 
	performI2CTransfer(I2C_FLAG_READ, size);  
    
}

void WriteConfig2EEprom(void)
{
	EepromState = WRITE_CONFIG;
        sMyConfig.crc = Crc_fromBuffer((uint8_t *)&sMyConfig, sizeof(sMyConfig)-2);
	EepromWriteBuf(CONFIG_ADDRESS,(uint8_t*)&sMyConfig,sizeof(WaterMeterConfigStruct) );
}
void ReadConfigEEprom(void)
{
	EepromState = READ_CONFIG;
     
	EepromReadBuf(CONFIG_ADDRESS,sizeof(WaterMeterConfigStruct));
    
        if (sMyConfig.crc != Crc_fromBuffer((uint8_t *)&sMyConfig, sizeof(sMyConfig)-2))
        {
          // write default values
          WriteConfigDefault();
        }
}

void ReadItemEEprom(void)
{
	EepromState = READ_LOGGER_ITEM;
	EepromReadBuf(LOGGER_ADDRESS+EEPROM_PAGE_SIZE*TmpLoggerIndex,sizeof(L2_WaterMeterLoggerMsg));
}

void WriteItemEEprom(void)
{
	EepromState = WRITE_LOGGER_ITEM;
	EepromWriteBuf(LOGGER_ADDRESS+EEPROM_PAGE_SIZE*sMyConfig.Eeprom_LoggerIndex,(uint8_t*) &WaterLogger[loggerIndex],sizeof(L2_WaterMeterLoggerMsg));
}


void WriteConfigDefault(void)
{
	sMyConfig.aExtendedAddress = 30006001;
	sMyConfig.FactorType =10;
	sMyConfig.MeterWakeUpTimeInterval = 3600000; // 1 hour default
	sMyConfig.MeterLoggerInterval = 3600000; 	 // 1 hour default
	sMyConfig.ShipmentDisable = 0xff;
	sMyConfig.crc = Crc_fromBuffer((uint8_t *)&sMyConfig, sizeof(sMyConfig)-2);
	WATER_VOL=0;
	MeterValveState=METER_VALVE_OPEND;
//	Persistent_setConsumtionMeterValve(WATER_VOL,MeterValveState); // save water consumption in stack persistent
	WriteConfig2EEprom();
}


void WriteLogger2Eeprom(uint16_t loggerIndex)
{
	if (loggerIndex == 23) // every day save to eeprom
	{
		WriteItemEEprom();
	}
	else
	if (oldEEpromlogger_index!= sMyConfig.Eeprom_LoggerIndex)
	{ // y.f prevent dual eeprom write in one cycle
	  oldEEpromlogger_index = ++sMyConfig.Eeprom_LoggerIndex;
	  WriteConfig2EEprom();
	}
}

void ReadLoggerEeprom (uint16_t loggerIndex)
{
	TmpLoggerIndex = loggerIndex;
	ReadItemEEprom();
}



/**************************************************************************//**
 * @brief I2C Interrupt Handler.
 *        The interrupt table is in assembly startup file startup_efm32.s
 *****************************************************************************/
void I2C0_IRQHandler(void)
{
  int status;

  status = I2C0->IF;
#if 0
  // y.f. Slave mode only
  if (status & I2C_IF_ADDR){
    /* Address Match */
    /* Indicating that reception is started */
    i2c_rxInProgress = true;
    I2C0->RXDATA;

    I2C_IntClear(I2C0, I2C_IFC_ADDR);

  }
  else
#endif

  if (status & I2C_IF_RXDATAV){
    /* Data received */
    i2c_rxBuffer[i2c_rxBufferIndex] = I2C0->RXDATA;
    i2c_rxBufferIndex++;
  }
  if(status & I2C_IEN_SSTOP){
    /* Stop received, reception is ended */
    I2C_IntClear(I2C0, I2C_IEN_SSTOP);
    if (EepromState == READ_CONFIG)
    	{
    		if (i2c_rxBufferIndex== sizeof(sMyConfig)) // do only if correct number of bytes received
    		{
    				memcpy((uint8_t*)&sMyConfig,i2c_rxBuffer, i2c_rxBufferIndex);
    		}

    	}
    else if (EepromState == READ_LOGGER_ITEM)
    	{
    		if (i2c_rxBufferIndex== sizeof(L2_WaterMeterLoggerMsg)) // do only if correct number of bytes received
    				memcpy((uint8_t*)&TmpLoggerItem,i2c_rxBuffer, i2c_rxBufferIndex);
    	}
    EepromState = EEPROM_IDLE;

  }
}

/**************************************************************************//**
 * @brief  Transmitting I2C data. Will busy-wait until the transfer is complete.
 *****************************************************************************/
void performI2CTransfer(uint8_t read_write, uint8_t size)
{
  /* Transfer structure */
  I2C_TransferSeq_TypeDef i2cTransfer;

  	i2c_rxBufferIndex = 0;
	/* Initializing I2C transfer */
	if (read_write == I2C_FLAG_READ)
	{ // write read operation
	  i2cTransfer.flags         = I2C_FLAG_READ;
	  i2cTransfer.buf[0].len    =  2;  // write size slave offset
	  i2cTransfer.buf[1].len    = size; // Read size
	}
	else
	{ // write operation
	  i2cTransfer.flags         =  I2C_FLAG_WRITE;
	  i2cTransfer.buf[0].len    =  size +2;  // write size slave offset
	  i2cTransfer.buf[1].len    =  0; // Read size slave offset

	}
	i2cTransfer.buf[0].data   = i2c_txBuffer;
	i2cTransfer.buf[1].data   = i2c_rxBuffer;

    I2C_TransferInit(I2C0, &i2cTransfer);
    /* Sending data */
  while (I2C_Transfer(I2C0) == i2cTransferInProgress){;}
 

  I2C_IntClear(I2C0,  I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  I2C_IntEnable(I2C0, I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  NVIC_EnableIRQ(I2C0_IRQn);

}


