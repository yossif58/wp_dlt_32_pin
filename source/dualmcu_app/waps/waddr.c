/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include "waddr.h"

app_addr_t Waddr_to_Addr(w_addr_t waddr)
{
    if(waddr == WADDR_BCAST)
    {
        return APP_ADDR_BROADCAST;
    }
    else if(waddr == WADDR_ANYSINK)
    {
        return APP_ADDR_ANYSINK;
    }
    // No domain conversion is necessary
    return waddr;
}

w_addr_t Addr_to_Waddr(app_addr_t app_addr)
{
    if(app_addr == APP_ADDR_BROADCAST)
    {
        return WADDR_BCAST;
    }
    else if(app_addr == APP_ADDR_ANYSINK)
    {
        return WADDR_ANYSINK;
    }
    // No domain conversion is necessary
    return app_addr;
}

bool Waddr_addrIsValid(w_addr_t addr)
{
    // Waddr has a valid ranges of:
    // 0 (anysink) - 0x00FF FFFD (unicast address)
    // 0x8000 0001 - 0x80FF FFFD (multicast address)
    // 0xFFFF FFFF (broadcast address)

    if (addr <= WADDR_MAX)
    {
        return true;
    }
    else if (addr == WADDR_BCAST)
    {
        return true;
    }
    else if ((addr >= (WADDR_MULTICAST | 1)) &&
        (addr <= (WADDR_MULTICAST | WADDR_MAX)))
    {
        return true;
    }
    else
    {
        return false;
    }
}
