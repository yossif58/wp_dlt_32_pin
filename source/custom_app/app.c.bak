/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/*
 * \file    app.c
 * \brief   This file is a template for writing a custom application
 */




#include <stdlib.h>
#include "string.h"
#include "hal_api.h"
#include "board.h"
#include "state.h"
#include "io.h"
#include "api.h"
#include "node_configuration.h"
#include "led.h"
#include "utils.h"
//#include "watercalc.h"
#include "meterprotocol.h"
//#include "metervalve.h"
//#include "logger.h"
#include "persistent.h"
#include "uartdlt.h"
#include "DLTprotocol.h"
//#include "eeprom.h"
   
 
#define APPLICATION_VERSION             1011            // y.f. 17/12/2019 2 msb digits represent HW version 2 lsb represent firmware version               
#define METER_CALLBACK_EXECUTION_TIME	250             // Max time for meter task in us
#define NODE_ADDRESS_NO_VALUE           0               // y.f. 11/08/2020
#define NODE_ADDRESS_SINK               1111113         // We use sMyConfig.aExtendedAddress
#define NODE_ADDRESS_METER              2222222         // We use sMyConfig.aExtendedAddress
#define REPEATER_ADD                    4440005
#define NO_METER_1_ADD                  4444447
#define NO_METER_2_ADD                  4444444
/** Period to send data */
#define DEFAULT_PERIOD_S     1 // was 4  to half second 17/11/2019
#define DEFAULT_PERIOD_US   (DEFAULT_PERIOD_S*1000*500) // y.f. 17/12/2019 change to 0.5 sec was 4 seconds  
#define PERIOD_400_MS        (1000*400)
#define PERIOD_5000_MS        (1000*5000)
#define PERIOD_200_MS         (1000*200)
#define PERIOD_2000_MS         (1000*2000)
#define PERIOD_3000_MS         (1000*3000)
/** Time needed to execute the periodic work, in us */
#define EXECUTION_TIME_US 500
/** Endpoint to send data */
#define DATA_EP         1
#ifdef PUSH
static uint32_t pushtime;

#ifdef ELECTRIC_SINK
extern void InitMeterlogger(void);
extern void DCUPushTask(void);
#else
extern void StartPushInit(bool Powerup);

extern void PushTask();  // y.f. 24/09/2020 add push task meter will send a periodic data reading commad towards sink    
#endif
#endif
#ifdef   ELECTRIC_METER
bool PushdebugInprogress = false;
#ifdef SCAN
void onScannedNborsCb(void) // y.f. 26/10/2020 send only omce information to remote terminal 44444
{
     uint8_t bufout[150];
     app_lib_state_nbor_list_t nbors_list;
     if (PushdebugInprogress==false) return;
     memset (bufout,0x55 ,132);
      if (lib_state->getNbors(&nbors_list) == APP_RES_OK)
      {
            WPSendMsg((char* ) &nbors_list, 132,44444);  // y.f. 25/10/2020
      }
      else
      {
            WPSendMsg("no neighbours found \r\n" , 24,44444);  // y.f. 25/10/2020
      }
      lib_state->startScanNbors(); 
}
#endif
#endif
long long 	ExtendedAddress;

app_lib_data_receive_res_e dataReceivedCb(
    const app_lib_data_received_t * data)
{
    if ((data->num_bytes < 1) || (data->src_address == (ExtendedAddress % 100000)) ||
        // y.f. 5/5/21 skip messages comming from the current meter 
        (data->src_address == (ExtendedAddress % 100000)) ) 
        // Data was not for this application
        return APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP;
   blink_func();
#ifdef PUSH_DEBUG
#ifdef ELECTRIC_METER
        if (PushdebugInprogress) 
        for (uint8_t Bindt = 0 ; Bindt < 4 ; Bindt++)
        { // y.f. 19/10/2020 search for empty location to send if no place enter in location 3
          // use remote rf terminal for debug this terminal should have constant address 44444
          // It will receive all message received and transmitted by the rf of  current meter
          if ( (debufsend[Bindt].Ready2Send!=0x55)||(Bindt ==3))
          {
            debufsend[Bindt].Delay = 3; // we don't want debug msg interfers the real rf message 
            debufsend[Bindt].Ready2Send =0x55;
            debufsend[Bindt].size= data->num_bytes;
            memcpy ((uint8_t* ) &debufsend[Bindt].info, (uint8_t*)data->bytes ,data->num_bytes);
            break;
          }
        }
#endif 
#endif        
#ifdef DEBUG_MODE
#ifdef ELECTRIC_SINK 
    if (DisplayState == DLT_IDLE_STATE)
    {  // y.f. send only if card establish connection with meter 
      uint8_t bufout[256];
      uint8_t address_in[20];
      ConvertBinToAscii(data->src_address,address_in);
      memcpy(&bufout," msg received from meter ",25);
      memcpy(&bufout[25],address_in,8);
      memcpy (&bufout[33],data->bytes, data->num_bytes);
      bufout[data->num_bytes+33] = 0xd;
      bufout[data->num_bytes+34] = 0xa;
      Usart_sendBuffer(bufout,data->num_bytes+34); // Y.F. 16/11/2020 
    }
#endif    
#endif
#ifndef REPEATER    
    HandleRxFromRF((uint8_t*)data->bytes ,data->num_bytes); // 26/12/2019 just blink and exit if repeater 
#endif
    return APP_LIB_DATA_RECEIVE_RES_HANDLED;
}


void Meter_Init(void);
uint32_t meter_task(void);
uint8_t bufout[256];
uint32_t HandleRxFromUart(uint32_t m_timeout_time,uint32_t DltRxTimeout_time );
bool meter_stack_init(void);


/**
 * \brief   Initialization callback for application
 *
 * This function is called after hardware has been initialized but the
 * stack is not yet running.
 *
 */

void App_init(const app_global_functions_t * functions)
{
   // Open Wirepas public API
  
       // Open HAL
    HAL_Open();
    // Initialize IO's (enable clock and initialize pins)
    Io_init();
   
    configure_led(0);
    configure_led(1);
    set_led(0, false);
    set_led(1, false);
   
    API_Open(functions);
    Meter_Init();
   
  //  lib_settings->resetAll(); // y.f. 4/2/2019 Need to reset because it makes problem with changing to other address 
//    ReadConfigEEprom(); // y.f. to be test with powercom card temp here 
#ifdef ELECTRIC_SINK
    ExtendedAddress=NODE_ADDRESS_SINK; // start value 
    custom_uart_init(USART0,9600,false);  // y.f. uart for debug&meterDLT 
    DisplayState = DLT_IDLE_STATE;
    Usart_sendBuffer("DCU START Process \r\n",21); //  Y.F. 16/11/2020 
#endif   
#ifdef ST_FLASH
    custom_uart_init(USART1,115200,false);  // y.f. uart for ST      7/4/2021 temp to restore to 115200
#endif       
#ifdef ELECTRIC_METER 
       custom_uart_init(USART0,2400,false);    // y.f. uart for debug&meterDLT 
  //        while(1); // temp to be removed 
#ifdef DEBUG_MODE
   // Usart_sendBuffer("E. METER START Process \r\n",25); // temp for debug
      uint32_t addr;
      uint8_t bufout[256];
      uint8_t address_in[20];
      lib_settings->getNodeAddress(&addr);
      ConvertBinToAscii(addr,address_in);
      memcpy(&bufout,"METER NET ADDRESS ",18);
      memcpy(&bufout[18],address_in,8);
      bufout[26] = 0xd;
      bufout[27] = 0xa;
      Usart_sendBuffer(bufout,28);  // Y.F. 16/11/2020 
#endif     
    DisplayState =  DLTREAD_EM_SERIAL_NUMBER_2400; 
    KeypadSendReadCommAddrssMsg();
     ExtendedAddress=NODE_ADDRESS_NO_VALUE; // Y.F. 11/08/2020 
  while (( DisplayState !=  DLT_IDLE_STATE) && (ExtendedAddress==NODE_ADDRESS_NO_VALUE))
    {
 //      blink_func();
       HandleRxFromUart(200000,200000); // Y.F.25/12/2018  wait here till serial num req response is received from meter  
    }   

#endif    

#ifdef NO_METER_1
#ifdef REPEATER
    ExtendedAddress=REPEATER_ADD; // start value
#else
    ExtendedAddress=NO_METER_1_ADD; // start value
#endif    
    custom_uart_init(USART0,115200,false);  // y.f. uart for debug&meterDLT 
    DisplayState = DLT_IDLE_STATE;
  
#endif    
#ifdef NO_METER_2
    ExtendedAddress=NO_METER_2_ADD; // start value 
#endif   
    set_led(0, true);
    if (meter_stack_init())
          set_led(1, true);
 
      


    
    
}
void Meter_Init(void)
{
  //      SendPeriodicMsg(); // y.f. 29/09/2020 this message is for test the communication to sink only to be removd 
#ifdef PUSH
#ifdef ELECTRIC_SINK 
  InitMeterlogger();
#else    
  StartPushInit(true);
#endif
#endif  
#ifdef GAS_METER 
        Persistent_getConsumtionMeterValve(&WATER_VOL,&MeterValveState);
        if (MeterValveState==METER_VALVE_OPEND)
		OpenValve();
	WATER_HOUR = WATER_VOL;
	WaterTickTenth = 0;
	InitWaterLogger();
        Pulser_init();  		// y.f. 22/10/2018
        SwOpen_init();
        Magnet_init();

 
#endif        
}

uint32_t meter_task(void)
{
  //     toggle_led(1); // blink led 0
#ifdef GAS_METER
	if (ledstate==false)
        {
          ledstate =true;
          hal_gpio_set(BOARD_LED_PORT,BOARD_LED_PIN_0);
        }
	else
	{
          ledstate =false;
          hal_gpio_clear(BOARD_LED_PORT,BOARD_LED_PIN_0);
	}
	//AddItem2Logger();
	Persistent_setConsumtionMeterValve(WATER_VOL,MeterValveState); // save water consumtion in stack persistent
#endif  
#ifdef NO_METER_1
#ifdef DEBUG_MODE
      app_lib_state_nbor_list_t nbors_list;
      if (lib_state->getNbors(&nbors_list) == APP_RES_OK)
      {
        Usart_sendBuffer((char*)&nbors_list,132);  // Y.F. 16/11/2020 
      }
      else
        blink_func();
#else     
#ifndef REPEATER      
        SendPeriodicMsg();
        blink_func();
     //   return PERIOD_3000_MS;
       return DEFAULT_PERIOD_US; // y.f. every 0.5 sec temp 4/08/2020
#endif        
//        return PERIOD_400_MS;
        return PERIOD_5000_MS;
#endif        
#endif
#ifdef NO_METER_2
        SendPeriodicMsg();
#endif  
#ifdef PUSH        

         if ((++pushtime % 10) == 0) 
           
#ifdef  ELECTRIC_SINK           
           DCUPushTask();   // y.f. push task every 5 sec and not 0.5 sec
#endif         
         // 18/05/21 push task also for dcu 
#ifdef ELECTRIC_METER  
   //         SendPeriodicMsg(); // send to be removed y.f. 29/9/22 
            PushTask();   // y.f. push task every 5 sec and not 0.5 sec
#ifdef PUSH_DEBUG
        if (PushdebugInprogress) 
        for (uint8_t Bindt = 0 ; Bindt < 4 ; Bindt++)
        { // y.f. 19/10/2020 search for data to send to remote rf terminal for debug 
          // this terminal should have constant address 44444
          // It will receive all message received and transmitted by the rf of  current meter
          if  (debufsend[Bindt].Ready2Send ==0x55)
          { // found data to send 
            debufsend[Bindt].Delay--; //delay at least 3 periods befor sending debug msg
            if ((debufsend[Bindt].Delay) == 0)
            {
              debufsend[Bindt].Ready2Send =0;
              WPSendMsg((char* ) &debufsend[Bindt].info, debufsend[Bindt].size,44444);  // y.f. 27/06/2019
            }
            break;
          }
        }
#endif      
#endif         
#endif         
         return (HandleRxFromUart(1,3));
     
}


uint32_t HandleRxFromUart(uint32_t m_timeout_time,uint32_t DltRxTimeout_time )
{
        if (m_escaped == true)
        { // check if uart timeout is needed 
          if (m_timeout >= m_timeout_time)
          { // message is received 
            // DLT message received check it
            HandleRxFromUART0(rx_buffer, rx_buffer_idx);
            DltRxParamInit();
          }
          else
          {
            m_timeout++ ;
            return 80000;  // 20ms delay for end of message
          }
        }
        else
        {
          if (DltRxTimeout) 
          {// check if retry messge towards meter is needed
            
            // y.f. 29/09/2020 removed confused with rx from rf blink_func();
            if ((++DltRxTimeout) > DltRxTimeout_time)
            {
              
              
              DltRxTimeout=0; // stop retry meter is not responding
              switch (DisplayState)
              {
#ifdef ELECTRIC_METER                
                case DLTREAD_EM_SERIAL_NUMBER_1200:
                  // try b.r. 2400
                     custom_uart_init(USART0,2400,false);   
                     DisplayState =  DLTREAD_EM_SERIAL_NUMBER_2400; 
                     KeypadSendReadCommAddrssMsg();
   
                  break;
                case DLTREAD_EM_SERIAL_NUMBER_2400:
                                 // try b.r. 4800
                     custom_uart_init(USART0,4800,false);  
                     DisplayState =  DLTREAD_EM_SERIAL_NUMBER_4800; 
                     KeypadSendReadCommAddrssMsg();
   
                  break;
                case DLTREAD_EM_SERIAL_NUMBER_4800:
                                 // try b.r. 9600
                     custom_uart_init(USART0,9600,false);  
                     DisplayState =  DLTREAD_EM_SERIAL_NUMBER_9600; 
                     KeypadSendReadCommAddrssMsg();
                  
                  break;
                  case DLTREAD_EM_SERIAL_NUMBER_9600:
                                 // try back in loop  b.r. 1200
                     custom_uart_init(USART0,1200,false);  
                     DisplayState =  DLTREAD_EM_SERIAL_NUMBER_1200; 
                     KeypadSendReadCommAddrssMsg();
                  
                  break;
#endif                  
                  case DLT_IDLE_STATE:
                  break;
              }

            }
           }
        } 
#ifdef ELECTRIC_SINK  
#ifdef ST_FLASH
        // y.f.18/11/2020  handle receive buffer from ST
        if (ST_m_escaped == true)
        { // check if uart timeout is needed 
          if (ST_m_timeout >= m_timeout_time)
          { // message is received from ST 
            // DLT message received check it
            HandleRxFromST(ST_rx_buffer, ST_rx_buffer_idx);
            ST_RxParamInit();
          }
          else
          {
            ST_m_timeout++ ;
            return 80000;  // 20ms delay for end of message
          }
        }
        else
        {
          if (ST_RxTimeout) 
          {// check if retry messge towards meter is needed
            if ((++ST_RxTimeout) > DltRxTimeout_time)
            {
              ST_RxTimeout=0; // stop retry meter is not responding
            }
           }
        }          
#endif
#endif        
        return DEFAULT_PERIOD_US;
}
#ifdef   ELECTRIC_METER
void DltGeTnbors(void) // 26/20/2020
{
   if (PushdebugInprogress)
   {
     PushdebugInprogress =false;
        WPSendMsg("debug msg turn off \r\n", 22,44444);  // y.f. 25/10/2020
   }
   else
   {
     
     PushdebugInprogress =true;
     WPSendMsg("debug msg and beacon rx turn on \r\n", 40,44444);  // y.f. 25/10/2020
#ifdef SCAN     
     lib_state->startScanNbors(); 
#endif       
   }
   
}


static void  setOnBeaconCb(const app_lib_state_beacon_rx_t * beacon)
{
  if (PushdebugInprogress == false ) return;
 	app_lib_state_beacon_rx_t beacon_in;
	beacon_in.address = beacon->address;
	beacon_in.rssi = beacon->rssi;
	beacon_in.txpower = beacon->txpower;
	WPSendMsg((char*)&beacon_in,sizeof(app_lib_state_beacon_rx_t),44444);
}

#endif

bool meter_stack_init(void)
{
  // start stack init only after reading serial number from meter 
#ifdef ELECTRIC_SINK    
    if (lib_settings->setNodeRole(APP_LIB_SETTINGS_ROLE_SINK|APP_LIB_SETTINGS_ROLE_FLAG_LL)
        != APP_RES_OK)
         return false ;
#else

  // y.f. 17/12/2019 add autorol 
    if (lib_settings->setNodeRole(APP_LIB_SETTINGS_ROLE_HEADNODE|APP_LIB_SETTINGS_ROLE_FLAG_LL|APP_LIB_SETTINGS_ROLE_FLAG_AUTOROLE)  // we want electric meter to response with low latency to DCU ALWAYS to be in RECEIVE ON.
        != APP_RES_OK)
         return false;
#endif  
    // Y.F. In future add more 2 zeros to inc network range and avoid colizions 27/11/2019
    if (configureNode(ExtendedAddress % 100000, // 
                      NETWORK_ADDRESS,
                      NETWORK_CHANNEL) != APP_RES_OK)
    {
        // Could not configure the node
        // It should not happen except if one of the config value is invalid
        return false;
    }
    // Y.f we can remove this it is include in the above command. 17/12/2019
    
    lib_settings->setNodeAddress(ExtendedAddress % 100000); //y.f. temp 30/12/2019 restore this to see if bug of previous meter is solved  
    lib_system->setPeriodicCb(meter_task,8000000,                                 // y.f. for first time delay of 1 sec
                            METER_CALLBACK_EXECUTION_TIME);
    
    lib_settings->setChannelMap(0xFFFFFFFE); // 17/12/2019 add by request of gwendal from WP
    // Y.F. add encryption and autentication api set lib 27//11/2019
    lib_data->setDataReceivedCb(dataReceivedCb);
    // Set callback for received broadcast messages
    lib_data->setBcastDataReceivedCb(dataReceivedCb);
#ifdef   ELECTRIC_METER      
    lib_state->setOnBeaconCb(setOnBeaconCb); // y.f. 27/10/2020
#ifdef SCAN      
    lib_state->setOnScanNborsCb(onScannedNborsCb,
#endif      
#endif    
    // Start the stack
    lib_state->startStack();
    return true;

}
