/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/*
 * \file    app.c
 * \brief   This file is a template to Dual MCU API app for all paltforms
 */
#include <stdlib.h>
#include "hal_api.h"
#include "io.h"
#include "waps.h"
#include "api.h"
#include "waps/sap/multicast.h"
#include "../util/node_configuration.h"


#ifdef WITH_FRAG
uint16_t m_min_sdu_size = 0;
uint16_t m_curr_sdu_size = 0;
#endif
// Interface config
const app_interface_config_s m_interface_config =
{
    .interface = APP_UART_INT,
    .baudrate = UART_BAUDRATE,
    .flow_ctrl = UART_FLOWCONTROL
};

void newAppConfigCb(const uint8_t * bytes,
                    uint8_t seq,
                    uint16_t interval)
{
    Waps_sinkUpdated(seq, bytes, interval);
}

static void dataSentCb(const app_lib_data_sent_status_t * status)
{
    Waps_packetSent(status->tracking_id,
                    status->src_endpoint,
                    status->dest_endpoint,
                    status->queue_time,
                    status->dest_address,
                    status->success);
}

static void remoteStatusReceivedCb(const app_lib_otap_remote_status_t * bl_status)
{
    Waps_receiveBlStatus(bl_status);
}
#ifdef WITH_FRAG
void sduReceivedCb(void)
{
    Waps_rcvSduInd();
}
#endif
void onScannedNborsCb(void)
{
    Waps_onScannedNbors();
}

/**
 * \brief   Initialization callback for application
 *
 * This function is called after hardware has been initialized but the
 * stack is not yet running.
 *
 */
void App_init(const app_global_functions_t * functions)
{
    // Open public API
    API_Open(functions);

    // Open HAL
    HAL_Open();


    // Initialize IO's (enable clock and initialize pins)
    Io_init();

    // Initialize the Dual-MCU API protocol
    Waps_init();
    Usart_sendBuffer("dualmcu test yossi rate 115200", 30);
    //register callbacks
    lib_data->setDataReceivedCb(Waps_receiveUnicast);
    lib_data->setBcastDataReceivedCb(Waps_receiveBcast);
    lib_data->setDataSentCb(dataSentCb);
    lib_data->setNewAppConfigCb(newAppConfigCb);
    lib_settings->registerGroupQuery(Multicast_isGroupCb);
#ifdef WITH_FRAG
    m_min_sdu_size = lib_data->getDataMaxNumBytes() + 1;
    m_curr_sdu_size = m_min_sdu_size;
    lib_sdu->setSduReceivedCb(sduReceivedCb);
#endif
    lib_otap->setRemoteStatusReceivedCb(remoteStatusReceivedCb);
    lib_state->setOnScanNborsCb(onScannedNborsCb,
                                APP_LIB_STATE_SCAN_NBORS_ONLY_REQUESTED);
//#if 0    
 // y.f. add identification for the node 
//#define ELECTRIC_SINK
    
#ifdef ELECTRIC_SINK    
    if (lib_settings->setNodeRole(APP_LIB_SETTINGS_ROLE_SINK|APP_LIB_SETTINGS_ROLE_FLAG_LL)
        != APP_RES_OK)
         return;
#else
    if (lib_settings->setNodeRole(APP_LIB_SETTINGS_ROLE_HEADNODE|APP_LIB_SETTINGS_ROLE_FLAG_LL)  // we want electric meter to response with low latency to DCU ALWAYS to be in RECEIVE ON.
        != APP_RES_OK)
         return;
#endif  
     
    if (configureNode(10000010, // 
                      NETWORK_ADDRESS,
                      NETWORK_CHANNEL) != APP_RES_OK)
    {
        // Could not configure the node
        // It should not happen except if one of the config value is invalid
        return;
    }
    
//#endif
    
}
