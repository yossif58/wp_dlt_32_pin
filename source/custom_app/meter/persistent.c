/* Copyright 2018 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include "persistent.h"
#include <string.h>

/**
 * \brief   Persistent area that is used via lib_storage library
 */
typedef struct __attribute__ ((__packed__))
{
    // Autostart bit
    uint8_t     autostart;
    uint8_t     valvestate;
    uint32_t	totalconsumtion;
    long long   aExtendedAddress;
    // Multicast groups
 //   multicast_group_addr_t multicast_groups[MULTICAST_ADDRESS_AMOUNT];
} persistent_area_t;

// Persistent area
static persistent_area_t m_persistent_image;
// Shortcut whether area is valid
static bool m_persistent_valid;

/**
 * \brief   Load persistent area from flash if necessary
 * \return  Status of the operation
 */
static app_res_e update_image(void)
{
    if (!m_persistent_valid)
    {
        m_persistent_valid = true;
        return lib_storage->readPersistent(&m_persistent_image,
                                           sizeof(m_persistent_image));
    }
    return APP_RES_OK;
}

/**
 * \brief   Write persistent area
 */
static app_res_e write_image(void)
{
    return lib_storage->writePersistent(&m_persistent_image,
                                        sizeof(m_persistent_image));
}

app_res_e Persistent_getAutostart(bool * autostart)
{
    app_res_e retval = update_image();
    *autostart = m_persistent_image.autostart;
    return retval;
}


app_res_e Persistent_getaExtendedAddress(uint32_t *aExtendedAddress)
{
    app_res_e retval = update_image();
    *aExtendedAddress = m_persistent_image.aExtendedAddress;
    return retval;
}
app_res_e Persistent_getConsumtionMeterValve(uint32_t* totalconsumtion,uint8_t *valvestate)
{
    app_res_e retval = update_image();
    *totalconsumtion = m_persistent_image.totalconsumtion;
    *valvestate = m_persistent_image.valvestate;
    return retval;
}


app_res_e Persistent_setAutostart(bool autostart)
{
    app_res_e retval = update_image();
    m_persistent_image.autostart = autostart;
    if (retval == APP_RES_OK)
    {
        retval = write_image();
    }
    return retval;
}
app_res_e Persistent_setaExtendedAddress(uint32_t aExtendedAddress)
{
    app_res_e retval = update_image();
    m_persistent_image.aExtendedAddress = aExtendedAddress;
    if (retval == APP_RES_OK)
    {
        retval = write_image();
    }
    return retval;
}

app_res_e Persistent_setConsumtionMeterValve(uint32_t totalconsumtion,uint8_t  valvestate)
{
    app_res_e retval = update_image();
    m_persistent_image.totalconsumtion = totalconsumtion;
    m_persistent_image.valvestate = valvestate;
    if (retval == APP_RES_OK)
    {
        retval = write_image();
    }
    return retval;
}
