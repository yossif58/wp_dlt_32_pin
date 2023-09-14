/* Copyright 2018 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef _PERSISTENT_H_
#define _PERSISTENT_H_

#include <stdint.h>
#include <stdbool.h>
#include "api.h"


/**
 * \brief   Get autostart bit
 * \param   autostart
 *          out: true: autostart is enabled, false: autostart is disabled
 * \return  Status of the operation
 */
app_res_e Persistent_getAutostart(bool * autostart);
app_res_e Persistent_getaExtendedAddress(uint32_t *aExtendedAddress);
app_res_e Persistent_getConsumtionMeterValve(uint32_t* totalconsumtion,uint8_t *valvestate); // y.f. add consumption to persistent area (after MULTICAST_ADDRESS_AMOUNT to 5)
app_res_e Persistent_setConsumtionMeterValve(uint32_t totalconsumtion, uint8_t valvestate);   // y.f. add consumption to persistent area (after MULTICAST_ADDRESS_AMOUNT to 5)
app_res_e Persistent_setaExtendedAddress(uint32_t aExtendedAddress);

/**
 * \brief   Set autostart bit
 * \param   autostart
 *          true: autostart is enabled, false: autostart is disabled
 * \return  Status of the operation
 */
app_res_e Persistent_setAutostart(bool autostart);

/**
 * \brief   Get multicast groups
 * \param   addr
 *          Multicast addresses stored in storage
 * \return  Status of the operation
 */
//app_res_e Persistent_getGroups(multicast_group_addr_t * addr);

/**
 * \brief   Set multicast groups
 * \param   addr
 *          Multicast addresses stored in storage
 * \return  Status of the operation
 */
//app_res_e Persistent_setGroups(multicast_group_addr_t * addr);

#endif /* SOURCE_DUALMCU_APP_WAPS_SAP_PERSISTENT_H_ */
