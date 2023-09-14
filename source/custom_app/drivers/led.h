/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file    led.h
 * \brief   Board-independent LED functions
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief Configure LED GPIO for a given LED
 */
void configure_led(uint_fast8_t led_num);

/**
 * \brief Turn the given LED on or off
 */
void blink_func(void);
void toggle_led(uint_fast8_t led_num);

void set_led(uint_fast8_t led_num, bool state);
uint8_t led_state;
