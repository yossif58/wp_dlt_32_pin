/* Copyright 2019 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "hal_api.h"
#include "board.h"
uint32_t getHFPERCLK()
{
    uint32_t div;
    div = 1U + ((CMU->HFPERPRESC & _CMU_HFPERPRESC_PRESC_MASK) >> _CMU_HFPERPRESC_PRESC_SHIFT);
    return 38400000U / div;
}
#ifdef DUALMCU
void enableHFPERCLK(bool enable)
{
    if(enable)
    {
        CMU->HFPERCLKEN0 |= BOARD_USART_CMU_BIT;
    }
    else
    {
        CMU->HFPERCLKEN0 &= ~(BOARD_USART_CMU_BIT);
    }
}
#else
// y.f. 17/11/2020 add more usarts 
void enableHFPERCLK(USART_TypeDef *usart,bool enable)
{

  if (usart == USART0)
  {
    if(enable)
    {
        CMU->HFPERCLKEN0 |= BOARD_USART_CMU_BIT;
    }
    else
    {
        CMU->HFPERCLKEN0 &= ~(BOARD_USART_CMU_BIT);
    }
  }
  else    if (usart == USART1)
  {
    if(enable)
    {
        CMU->HFPERCLKEN0 |= ST_USART_CMU_BIT;
    }
    else
    {
        CMU->HFPERCLKEN0 &= ~(ST_USART_CMU_BIT);
    }
  }
  else if (usart == USART2)
  {
#if 0    
    if(enable)
    {
        CMU->HFPERCLKEN0 |= WNT_USART_CMU_BIT;
    }
    else
    {
        CMU->HFPERCLKEN0 &= ~(WNT_USART_CMU_BIT);
    }
#endif
  }

  
}

#endif
