/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef WADDR_H_
#define WADDR_H_

#include "waps_frames.h"
#include "api.h"

/**
 * \file    waddr.h
 *          Address mapping between internal and external addresses
 *
 * \note    Size of w_addr_t is defined in waps_frames.
 */

/** Broadcast address symbol */
#define WADDR_BCAST     (w_addr_t)(-1)

/** Any sink symbol */
#define WADDR_ANYSINK   (w_addr_t)(0)

/** Highest valid unicast node address for a device */
#define WADDR_MAX       (w_addr_t)(0xFFFFFD)

/** Multicast bit for address */
#define WADDR_MULTICAST (w_addr_t)(0x80000000)

/**
 * \brief   Convert address from WAPS to APP domain
 * \param   waddr
 *          Waps address to convert
 * \return  Converted address value
 */
app_addr_t Waddr_to_Addr(w_addr_t waddr);

/**
 * \brief   Convert address from APP to WAPS domain
 * \param   app_addr
 *          Application address to convert
 * \return  Converted address value
 */
w_addr_t Addr_to_Waddr(app_addr_t app_addr);

/**
 * \brief   Way to check if waddr is valid or not
 * \param   addr
 *          Address to check
 * \return  true, address is valid.
 */
bool Waddr_addrIsValid(w_addr_t addr);

#endif /* WADDR_H_ */
