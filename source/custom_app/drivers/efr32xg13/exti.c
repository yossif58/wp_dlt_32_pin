/* Copyright 2019 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include <stdint.h>
#include <stddef.h>

#include "hal_api.h"
#include "io.h"
#include "board.h"
#include "api.h"
#include "watercalc.h"
// Amount of channels per vector (IO port width / 2)
#define IRQ_CHANNELS_PER_VECTOR     8
// Y.F. add interrupts for the following
// sw open      port xx
//   pulser 1 	port xx
//   pulser 2 	port xx
// mag_detect 	port xx
//   USART RX     port xx

// Callbacks
static wakeup_cb_f                  m_even_callbacks[IRQ_CHANNELS_PER_VECTOR];
static wakeup_cb_f                  m_odd_callbacks[IRQ_CHANNELS_PER_VECTOR];

// Declare ISR
void __attribute__((__interrupt__)) GPIO_EVEN_IRQHandler(void);
void __attribute__((__interrupt__)) GPIO_ODD_IRQHandler(void);

static inline void clear_interrupt(uint32_t pin)
{
    /** IFC is a latch register, safe to use without bit-band */
    GPIO->IFC = (1 << pin);
}

static void pin_config(uint32_t pin, uint32_t port)
{
    if (pin < 8)
    {
        GPIO->EXTIPSELL = (GPIO->EXTIPSELL & ~(0xF << (4 * pin))) |
                            (port << (4 * pin));
    }
    else
    {
        GPIO->EXTIPSELH = (GPIO->EXTIPSELH & ~(0xF << (4 * (pin - 8)))) |
                            (port << (4 * (pin - 8)));
    }
}

void Wakeup_pinInit(wakeup_cb_f cb)
{
    /* Clear interrupt sources and flags */
    GPIO->IEN = _GPIO_IEN_RESETVALUE;
    GPIO->IFC = _GPIO_IFC_MASK;

    uint32_t    pin = BOARD_USART_RX_PIN;
    uint32_t    index = pin;
    index >>= 1;
    if((pin % 2) == 0)
    {
        /* Even IRQ (only one pin per channel supported) */
        m_even_callbacks[index] = cb;
    }
    else
    {
        /* Odd IRQ (only one pin per channel supported) */
        m_odd_callbacks[index] = cb;
    }

    pin_config(BOARD_USART_RX_PIN, BOARD_USART_GPIO_PORT);
    /* Enable interrupt source for even pins */
    Sys_clearFastAppIrq(GPIO_EVEN_IRQn);
    Sys_enableFastAppIrq(GPIO_EVEN_IRQn,
                         APP_LIB_SYSTEM_IRQ_PRIO_HI,
                         GPIO_EVEN_IRQHandler);
    Sys_clearFastAppIrq(GPIO_ODD_IRQn);
    Sys_enableFastAppIrq(GPIO_ODD_IRQn,
                         APP_LIB_SYSTEM_IRQ_PRIO_HI,
                         GPIO_ODD_IRQHandler);
}

void Wakeup_off(void)
{
    Sys_disableAppIrq(GPIO_EVEN_IRQn);
    Sys_clearFastAppIrq(GPIO_EVEN_IRQn);
    Sys_disableAppIrq(GPIO_ODD_IRQn);
    Sys_clearFastAppIrq(GPIO_ODD_IRQn);
}

void Wakeup_enableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_USART_RX_PIN, 1);
}

void Wakeup_disableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_USART_RX_PIN, 0);
}

void Wakeup_clearIrq(void)
{
    clear_interrupt(BOARD_USART_RX_PIN);
}

void Wakeup_setEdge(uint8_t flags)
{
    uint32_t enable;
    /* Setup raising edge */
    flags & EXTI_IRQ_RISING_EDGE ? (enable = 1) : (enable = 0);
    BITBAND_Peripheral(&(GPIO->EXTIRISE), BOARD_USART_RX_PIN, enable);
    /* Setup falling edge */
    flags & EXTI_IRQ_FALLING_EDGE ? (enable = 1) : (enable = 0);
    BITBAND_Peripheral(&(GPIO->EXTIFALL), BOARD_USART_RX_PIN, enable);
}

void __attribute__((__interrupt__)) GPIO_EVEN_IRQHandler(void)
{
    uint32_t pin = GPIO->IF & _GPIO_IF_EXT_MASK;
    pin &= GPIO->IEN & _GPIO_IEN_EXT_MASK;
    uint32_t index;
    for(index = 0; index < IRQ_CHANNELS_PER_VECTOR; index++)
    {
        if((pin & 0x01) == 0x01)
        {
            clear_interrupt(2 * index);
            if(m_even_callbacks[index] != NULL)
            {
                m_even_callbacks[index]();
            }
        }
        pin >>= 2;
    }
}

void __attribute__((__interrupt__)) GPIO_ODD_IRQHandler(void)
{
    uint32_t pin = GPIO->IF & _GPIO_IF_EXT_MASK;
    pin &= GPIO->IEN & _GPIO_IEN_EXT_MASK;
    pin >>= 1;
    uint32_t index;
    for(index = 0; index < IRQ_CHANNELS_PER_VECTOR; index++)
    {
        if((pin & 0x01) == 0x01)
        {
            clear_interrupt(2 * index + 1);
            if(m_odd_callbacks[index] != NULL)
            {
                m_odd_callbacks[index]();
            }
        }
        pin >>= 2;
    }
}
// y.f. 22/10/2018 add section for pulsers external interrupts
#ifdef GAS_METER
void pulser1_enableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_METER_TENTHES_CUBIC_PULSE_PIN, 1);
}
static void Pulser2_gpio_isr(void)
{
     Sys_enterCriticalSection();
     WaterTickCubicUnits++;
     Sys_exitCriticalSection();
}

void pulser2_enableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_METER_ONE_CUBIC_PULSE_PIN, 1);
}



void pulser2_disableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_METER_ONE_CUBIC_PULSE_PIN, 0);
}
void pulser1_disableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_METER_TENTHES_CUBIC_PULSE_PIN, 0);
}
void pulser1_clearIrq(void)
{
    clear_interrupt(BOARD_METER_TENTHES_CUBIC_PULSE_PIN);
}

void pulser2_clearIrq(void)
{
    clear_interrupt(BOARD_METER_ONE_CUBIC_PULSE_PIN);
}

void pulser_pinInit(wakeup_cb_f cb,uint8_t pin)
{
    /* Clear interrupt sources and flags */
    GPIO->IEN = 0;
    GPIO->IFC = 0x0000FFFF;
    uint32_t    index = pin;
    index >>= 1;
    if(pin % 2)
          /* Even IRQ (only one pin per channel supported) */
        {
                m_odd_callbacks[index] = cb; // y.f. 22/04/2020
        	pin_config(BOARD_METER_CUBIC_PULSE_TENTHES_INT_PORT,BOARD_METER_TENTHES_CUBIC_PULSE_PIN );
                /* Enable interrupt source for even pins */
                Sys_clearFastAppIrq(GPIO_ODD_IRQn);
                Sys_enableFastAppIrq(GPIO_ODD_IRQn,
                         APP_LIB_SYSTEM_IRQ_PRIO_HI,
                         GPIO_ODD_IRQHandler);
        }
        else
        {
                m_even_callbacks[index] = cb; // y.f. 22/04/2020
        	pin_config(BOARD_METER_ONE_CUBIC_INT_PORT,BOARD_METER_ONE_CUBIC_PULSE_PIN );
                /* Enable interrupt source for even pins */
                Sys_clearFastAppIrq(GPIO_EVEN_IRQn);
                Sys_enableFastAppIrq(GPIO_EVEN_IRQn,
                         APP_LIB_SYSTEM_IRQ_PRIO_HI,
                         GPIO_EVEN_IRQHandler);
        }
    
    
}

void pulser_setEdge(uint8_t flags)
{
    uint32_t enable;
    /* Setup raising edge */
    flags & EXTI_IRQ_RISING_EDGE ? (enable = 1) : (enable = 0);
    BITBAND_Peripheral(&(GPIO->EXTIRISE), BOARD_METER_TENTHES_CUBIC_PULSE_PIN, enable);
    /* Setup falling edge */
    flags & EXTI_IRQ_FALLING_EDGE ? (enable = 1) : (enable = 0);
    BITBAND_Peripheral(&(GPIO->EXTIFALL), BOARD_METER_TENTHES_CUBIC_PULSE_PIN, enable);
    flags & EXTI_IRQ_RISING_EDGE ? (enable = 1) : (enable = 0);
	BITBAND_Peripheral(&(GPIO->EXTIRISE), BOARD_METER_ONE_CUBIC_PULSE_PIN, enable);
	/* Setup falling edge */
	flags & EXTI_IRQ_FALLING_EDGE ? (enable = 1) : (enable = 0);
	BITBAND_Peripheral(&(GPIO->EXTIFALL), BOARD_METER_ONE_CUBIC_PULSE_PIN, enable);
}
/* Y.F. To be called from start point */
/* init both pulser interrupts one is tenth units  and second is the cubic meter units */
static void Pulser1_gpio_isr(void)
{
    Sys_enterCriticalSection();
    WaterTickTenth++;
    Sys_exitCriticalSection();
}
void Pulser_init(void)
{
    Sys_enterCriticalSection();
    pulser_pinInit(Pulser1_gpio_isr,BOARD_METER_ONE_CUBIC_PULSE_PIN);
    pulser_pinInit(Pulser2_gpio_isr,BOARD_METER_TENTHES_CUBIC_PULSE_PIN);
    pulser1_disableIrq();
    pulser2_disableIrq();
    pulser1_clearIrq();
    pulser2_clearIrq();
    // Expecting falling edge
    pulser_setEdge(EXTI_IRQ_FALLING_EDGE);
    pulser1_enableIrq();
    pulser2_enableIrq();
    Sys_exitCriticalSection();
}



void magnet_disableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_METER_MAGNET_INT_PIN, 1);
}
void magnet_clearIrq(void)
{
    clear_interrupt(BOARD_METER_MAGNET_INT_PIN);
}

void magnet_pinInit(wakeup_cb_f cb,uint8_t pin)
{
    /* Clear interrupt sources and flags */
    GPIO->IEN = 0;
    GPIO->IFC = 0x0000FFFF;
    uint32_t    index = pin;
    index >>= 1;
    if((pin % 2) == 0)
    {
        /* Even IRQ (only one pin per channel supported) */
        m_callbacks[index] = cb;

        pin_config(BOARD_METER_MAGNET_INT_PORT,BOARD_METER_MAGNET_INT_PIN );
    /* Enable interrupt source for even pins */
    	Sys_clearFastAppIrq(GPIO_EVEN_IRQn);
    	Sys_enableFastAppIrq(GPIO_EVEN_IRQn,
                         APP_LIB_SYSTEM_IRQ_PRIO_HI,
                         GPIO_EVEN_IRQHandler);
    }
}
void magnet_setEdge(uint8_t flags)
{
    uint32_t enable;
    /* Setup raising edge */
    flags & EXTI_IRQ_RISING_EDGE ? (enable = 1) : (enable = 0);
    BITBAND_Peripheral(&(GPIO->EXTIRISE), BOARD_METER_MAGNET_INT_PIN, enable);
    /* Setup falling edge */
    flags & EXTI_IRQ_FALLING_EDGE ? (enable = 1) : (enable = 0);
    BITBAND_Peripheral(&(GPIO->EXTIFALL), BOARD_METER_MAGNET_INT_PIN, enable);
    flags & EXTI_IRQ_RISING_EDGE ? (enable = 1) : (enable = 0);
	BITBAND_Peripheral(&(GPIO->EXTIRISE), BOARD_METER_MAGNET_INT_PIN, enable);
	/* Setup falling edge */
	flags & EXTI_IRQ_FALLING_EDGE ? (enable = 1) : (enable = 0);
	BITBAND_Peripheral(&(GPIO->EXTIFALL), BOARD_METER_MAGNET_INT_PIN, enable);
}
void Magnet_init(void)
{
    Sys_enterCriticalSection();
    magnet_pinInit(Magnet_gpio_isr,BOARD_METER_MAGNET_INT_PIN);
    magnet_disableIrq();
    magnet_clearIrq();
    // Expecting falling edge
    magnet_setEdge(EXTI_IRQ_FALLING_EDGE);
    magnet_enableIrq();
    Sys_exitCriticalSection();
}
static void Magnet_gpio_isr(void)
{
}
void SwOpen_enableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_METER_SW_OPEN_INT_PIN, 1);
}
void SwOpen_disableIrq(void)
{
    BITBAND_Peripheral(&(GPIO->IEN), BOARD_METER_SW_OPEN_INT_PIN, 1);
}
void SwOpen_clearIrq(void)
{
    clear_interrupt(BOARD_METER_SW_OPEN_INT_PIN);
}

static void SwOpen_gpio_isr(void)
{
}


void SwOpen_setEdge(uint8_t flags)
{
    uint32_t enable;
    /* Setup raising edge */
    flags & EXTI_IRQ_RISING_EDGE ? (enable = 1) : (enable = 0);
    BITBAND_Peripheral(&(GPIO->EXTIRISE), BOARD_METER_SW_OPEN_INT_PIN, enable);
    /* Setup falling edge */
    flags & EXTI_IRQ_FALLING_EDGE ? (enable = 1) : (enable = 0);
    BITBAND_Peripheral(&(GPIO->EXTIFALL), BOARD_METER_SW_OPEN_INT_PIN, enable);
    flags & EXTI_IRQ_RISING_EDGE ? (enable = 1) : (enable = 0);
	BITBAND_Peripheral(&(GPIO->EXTIRISE), BOARD_METER_SW_OPEN_INT_PIN, enable);
	/* Setup falling edge */
	flags & EXTI_IRQ_FALLING_EDGE ? (enable = 1) : (enable = 0);
	BITBAND_Peripheral(&(GPIO->EXTIFALL), BOARD_METER_SW_OPEN_INT_PIN, enable);
}
void SwOpen_init(void)
{
    Sys_enterCriticalSection();
    SwOpen_pinInit(SwOpen_gpio_isr,BOARD_METER_SW_OPEN_INT_PIN);
    SwOpen_disableIrq();
    SwOpen_clearIrq();
    // Expecting falling edge
    SwOpen_setEdge(EXTI_IRQ_FALLING_EDGE);
    SwOpen_enableIrq();
    Sys_exitCriticalSection();
}
#endif
