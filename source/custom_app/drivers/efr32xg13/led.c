/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/*
 * \file    led.c
 * \brief   Board-specific LED functions for Wirepas EZR32 Kit board
 */

#include "led.h"
#include "vendor/efr32fg13/efr32_gpio.h"
#include "board.h"
//#include "em_gpio.h"


/** Board-dependent LED number to pin mapping */
typedef struct
{
    hal_gpio_port_e port;
    uint32_t pin;
} efm32_gpio_t;

static const efm32_gpio_t pin_map[] = BOARD_LED_PIN_LIST;


void configure_led(uint_fast8_t led_num)
{
    if (led_num >= (sizeof(pin_map) / sizeof(pin_map[0])))
    {
        // Invalid led number
        return;
    }

    hal_gpio_port_e port_num = pin_map[led_num].port;
    uint32_t pin_num = pin_map[led_num].pin;

    GPIO->P[port_num].MODEL &= ~(0xf << (pin_num * 4));
    GPIO->P[port_num].MODEL |= (0x4 << (pin_num * 4));
    hal_gpio_clear(port_num,pin_num);  // Off by default // y.f. 20/04/2020 to modify hal_gpio_clear
}

void set_led(uint_fast8_t led_num, bool state)
{
    if (led_num >= (sizeof(pin_map) / sizeof(pin_map[0])))
    {
        // Invalid led number
        return;
    }

    hal_gpio_port_e port_num = pin_map[led_num].port;
    uint32_t pin_num = pin_map[led_num].pin;

    if (state)
    {
        hal_gpio_set(port_num,pin_num);
    }
    else
    {
        hal_gpio_clear(port_num,pin_num);
    }
}

void blink_func(void)
{
     // return; // temp to be removed 
    // Toggle LED state
    if (led_state)
    {
        led_state = false;
        set_led(0, false);
        set_led(1, true);
        
        
    }
    else {
        led_state = true;
        set_led(1, false);
        set_led(0, true);

    }

}
void toggle_led(uint_fast8_t led_num)
{
  if (led_num)
    hal_gpio_toggle(pin_map[1].port,pin_map[1].pin); // toggle led 1
  else
    hal_gpio_toggle(pin_map[0].port,pin_map[0].pin); // toggle led 0
}    