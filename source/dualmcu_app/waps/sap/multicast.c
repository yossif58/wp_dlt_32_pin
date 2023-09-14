/* Copyright 2018 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include "multicast.h"
#include "waddr.h"
#include <string.h>
#include "persistent.h"

/**
 * \brief   Convert packed multicast address to app addr structure
 * \param   addr
 *          Packed address
 * \return  App address
 */
static app_addr_t mcast_addr_to_app_addr(multicast_group_addr_t * addr)
{
    app_addr_t retval = 0;
    memcpy(&retval, addr, sizeof(multicast_group_addr_t));
    return retval;
}

/**
 * \brief   Convert waddr to multicast address
 * \param   from
 *          Waps address (in unaligned pointer format)
 * \param   to
 *          Where conversion is done
 * \return  true: Conversion ok, false: Faulty conversion (invalid address)
 */
static bool waddr_to_mcast_addr(const uint8_t * from,
                                multicast_group_addr_t * to)
{
    w_addr_t address;
    memcpy (&address, from, sizeof(w_addr_t));

    // Unable to set values > WADDR_MAX. 0 (i.e. anysink) is ok because it is
    // don't care value
    if (address > WADDR_MAX)
    {
        return false;
    }
    else
    {
        memcpy(to, from, sizeof(multicast_group_addr_t));
        return true;
    }
}

/**
 * \brief   Convert multicast address to waddr
 * \param   from
 *          Multicast address used
 * \param   to
 *          Where conversion is done
 */
static void mcast_addr_to_waddr(const multicast_group_addr_t * from,
                                uint8_t * to)
{
    w_addr_t value = 0;
    memcpy (&value, from, sizeof(multicast_group_addr_t));
    if (value > WADDR_MAX)
    {
        // Replace factory default value by 0 value
        value = 0;
    }
    memcpy (to, &value, sizeof(w_addr_t));
}

bool Multicast_isGroupCb(app_addr_t group_addr)
{
    multicast_group_addr_t addresses[MULTICAST_ADDRESS_AMOUNT];

    if (Persistent_getGroups(&addresses[0]) != APP_RES_OK)
    {
        // Failure, not a member of the group
        return false;
    }

    for (uint_fast8_t i=0; i < MULTICAST_ADDRESS_AMOUNT; i++)
    {
        // Skip invalid addresses
        app_addr_t groupaddr = mcast_addr_to_app_addr(&addresses[i]);
        if ((groupaddr == WADDR_ANYSINK) || (groupaddr > WADDR_MAX))
        {
            continue;
        }
        if (groupaddr == group_addr)
        {
            return true;
        }
    }
    return false;
}

app_res_e Multicast_setGroups(const uint8_t * groups)
{
    app_res_e retval = APP_RES_OK;
    // Storage groups used
    multicast_group_addr_t stgroups[MULTICAST_ADDRESS_AMOUNT];

    // Check through the addresses that they are valid (0 used as no-group)
    for (uint_fast8_t i=0; i < MULTICAST_ADDRESS_AMOUNT; i++)
    {
        if (!waddr_to_mcast_addr(&groups[i * sizeof(w_addr_t)], &stgroups[i]))
        {
            return APP_RES_INVALID_VALUE;
        }
        retval = Persistent_setGroups(&stgroups[0]);
    }

    return retval;
}

app_res_e Multicast_getGroups(uint8_t * groups)
{
    multicast_group_addr_t persistent_groups[MULTICAST_ADDRESS_AMOUNT];

    app_res_e retval = Persistent_getGroups(&persistent_groups[0]);

    if (retval == APP_RES_OK)
    {
        for (uint_fast8_t i=0; i < MULTICAST_ADDRESS_AMOUNT; i++)
        {
            mcast_addr_to_waddr(&persistent_groups[i], groups);
            groups += sizeof(w_addr_t);
        }
    }
    return retval;
}
