/* Copyright 2019 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */


#ifndef MCU_EFR32XG13_HAL_HFPERCLK_H_
#define MCU_EFR32XG13_HAL_HFPERCLK_H_

//#include "efr32fg13p_usart.h"

// High Frequency Peripheral Cloack freq
#define HFPERCLK_FREQ                  (getHFPERCLK())

/**
 * \brief   Get the High Frequency Peripheral Clock freq
 * \return  The HFPERCLK freq.
 *
 */
uint32_t getHFPERCLK(void);

/**
 * \brief   Enable or disable HFPERCLK
 * \param   enabled
 *          True, if HFPERCLK should be enabled
 *          False, if HFPERCLK should be disabled
 */
#ifdef DUALMCU
void enableHFPERCLK(bool enable);
#else
// y.f. 17/11/2020 add 2 more usarts 
void enableHFPERCLK(USART_TypeDef *usart,bool enable);
#endif

#endif /* MCU_EFR32XG13_HAL_HFPERCLK_H_ */
