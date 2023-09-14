/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file hal_api.h
 *
 * \brief Collection of common interfaces that Wirepas HAL implements.
 *
 */

#ifndef HAL_API_H_
#define HAL_API_H_

#include <stdbool.h>

// Access to MCU
#include "mcu.h"

// USART driver
#ifdef DUALMCU // y.f. use this only for dualmcu application
#include "usart.h"
#endif
// Deep sleep control
#include "ds.h"

/**
 * \brief   Initialize HAL
 * \return  Status of operation
 */
bool HAL_Open(void);

#endif /* HAL_API_H_ */
