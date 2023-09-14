/**********************************************************************************
*																					*
* FILE NAME:																		*
*   WirepasProtocol.c																*
*																					*
* DESCRIPTION:																		*
*   This file contains all routines for the Meter Protocol to communicate with sink *
* HISTORY																			*
*   Ferlandes Yossi																	*
* - CREATION      																	*
*   Yossi Ferlandes25/10/2018													    *
*                                                                                   *
************************************************************************************/
#define METER_PROTOCOL_H
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "board.h"
#include "MeterProtocol.h"
#include "api.h"
#include "data.h"
#include "utils.h"
//#include "WaterCalc.h"
//#include "logger.h"
//#include "eeprom.h"
#include "led.h"
//#include "app.h"

/* local vars defenitions ***********************************************************/

/** y.f. Endpoint to send data */
#define DATA_EP        1

uint32_t MsgTxId = 0;   
#ifdef   ELECTRIC_METER
extern bool PushdebugInprogress;
#endif
extern long long 	ExtendedAddress;


/*************************************************************************************/
/************************** local function defenitions *******************************/
/*************************************************************************************/

/***************************Exported Vars                                            */
/***************************Extern functions******************************************/
void WPSendMsg(char *DataIn, int16_t Size,uint32_t address)
{

    // Create a data packet to send

    app_lib_data_to_send_t data_to_send;
    data_to_send.bytes = (const uint8_t *) DataIn;
    data_to_send.num_bytes = Size;
    data_to_send.dest_address  = address & 0xffffff; 
    data_to_send.src_endpoint  = DATA_EP;
    data_to_send.dest_endpoint = DATA_EP;
    data_to_send.qos = APP_LIB_DATA_QOS_HIGH;
    data_to_send.delay = 0;
    data_to_send.flags = APP_LIB_DATA_SEND_FLAG_NONE;
    data_to_send.tracking_id = APP_LIB_DATA_NO_TRACKING_ID;
    // Send the data packet
    lib_data->sendData(&data_to_send);
    //blink_func();
#ifdef PUSH_DEBUG
#ifdef ELECTRIC_METER 
        if ((address == 44444) ||   (PushdebugInprogress== false))  return; // terminal address 
        for (uint8_t Bindt = 0 ; Bindt < 4 ; Bindt++)
        { // y.f. 19/10/2020 search for empty location to send if no place enter in location 3
          // use remote rf terminal for debug this terminal should have constant address 44444
          // It will receive all message received and transmitted by the rf of  current meter
          if ( (debufsend[Bindt].Ready2Send!=0x55)||(Bindt ==3))
          {
            debufsend[Bindt].Delay =3;
            debufsend[Bindt].Ready2Send =0x55;
            debufsend[Bindt].size= Size;
            memcpy ((uint8_t* ) &debufsend[Bindt].info, DataIn,Size);
            break;
          }
        }
#endif
#endif
 
 
}

void SendPeriodicMsg(void)
{
 
 uint8_t btmp[250];
 char ItStatus[240];
    // Increment value to send
    	  MsgTxId++;
          sprintf (( char*)&ItStatus, "{ MAC: " );
          ConvertBinToAscii(ExtendedAddress,btmp);
          strcat (( char*)&ItStatus,( char*)btmp);
          strcat (( char*)&ItStatus, " messageid: " );
          ConvertBinToAscii(MsgTxId,btmp);
          strcat (( char*)&ItStatus,( char*)btmp);
          strcat (( char*)&ItStatus, " Consumption: " );
 //         ConvertBinToAscii(get_water_ticks(0),btmp);
 //         strcat (( char*)&ItStatus,( char*)btmp);
          strcat (( char*)&ItStatus,"}");
#ifdef ELECTRIC_SINK          
          WPSendMsg((char*)ItStatus, strlen(ItStatus),10000001); // This address is only for test
#else
          // temp y.f. for test field 18/06/2019 
   //       WPSendMsg((char*)ItStatus, strlen(ItStatus),10000010);  // other meters exept the sink itself shuld send direct to sink 
   //        WPSendMsg((char*)ItStatus, strlen(ItStatus),17943);  // temp to be removed for test only
//          10000010
          WPSendMsg((char*)ItStatus, strlen(ItStatus),APP_ADDR_ANYSINK);  // other meters exept the sink itself shuld send direct to sink 
      //      WPSendMsg((char*)ItStatus, strlen(ItStatus),44444); // 29/09/22 temp to be removed correct is app_addr_anysunc
#endif
}
