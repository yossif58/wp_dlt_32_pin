/* Copyright 2019 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef BOARD_BRD4158A_BOARD_H_
#define BOARD_BRD4158A_BOARD_H_
//#define DUALMCU // y.f. 23/04/2020 add for usart parity none  define
#define METER_BOARD
//#define PUSH_DEBUG
#define PUSH
#define P3
//#define ST_FLASH                        // incase using external flash with ST microcontroller
//#define DEBUG_MODE
//#define ELECTRIC_SINK
//#define REPEATER
//#define NO_METER_1
//#define NO_METER_2
#define ELECTRIC_METER 
//#define ELECTRIC_SINK
//#define GAS_METER
//#define GAS_SINK

// Y.F. st card usart1 defintions 12/11/2020 
#define ST_USART                        USART1
#define ST_USART_GPIO_PORT              GPIOB
#define ST_USART_ROUTELOC_RXLOC         USART_ROUTELOC0_RXLOC_LOC6   // Y.F. 6/9/2022
#define ST_USART_ROUTELOC_TXLOC         USART_ROUTELOC0_TXLOC_LOC6   // Y.F. 6/9/2022
#define ST_USART_TX_PIN                 11 // Y.F. 6/9/2022
#define ST_USART_RX_PIN                 12 // Y.F. 6/9/2022
#define ST_USART_CMU_BIT                CMU_HFPERCLKEN0_USART1
#define ST_UART_RX_IRQn                 USART1_RX_IRQn
#define ST_UART_TX_IRQn                 USART1_TX_IRQn

// y.f. 15/11/2020 add WNT/GATEWAY  extra usart to connect to gateway for future use 
#ifndef ELECTRIC_METER
#define WNT_USART                        USART2
#define WNT_USART_GPIO_PORT              GPIOF
#define WNT_USART_ROUTELOC_RXLOC         USART_ROUTELOC0_RXLOC_LOC19
#define WNT_USART_ROUTELOC_TXLOC         USART_ROUTELOC0_TXLOC_LOC19
#define WNT_USART_TX_PIN                 6
#define WNT_USART_RX_PIN                 7
#define WNT_USART_CMU_BIT                CMU_HFPERCLKEN0_USART2
#define WNT_UART_RX_IRQn                 USART2_RX_IRQn
#define WNT_UART_TX_IRQn                 USART2_TX_IRQn

#endif



// Waps/DCU usart defines
#define BOARD_USART                     USART0
#define BOARD_USART_GPIO_PORT           GPIOA
#ifdef P3
// Y.F. SWItch RX AND TX IN 3P

#define BOARD_USART_ROUTELOC_RXLOC      USART_ROUTELOC0_RXLOC_LOC31
#define BOARD_USART_ROUTELOC_TXLOC      USART_ROUTELOC0_TXLOC_LOC1

#define BOARD_USART_CMU_BIT             CMU_HFPERCLKEN0_USART0

#define BOARD_UART_RX_IRQn              USART0_RX_IRQn
#define BOARD_UART_TX_IRQn              USART0_TX_IRQn

#define BOARD_USART_TX_PIN              1
#define BOARD_USART_RX_PIN              0

#else

#define BOARD_USART_ROUTELOC_RXLOC      USART_ROUTELOC0_RXLOC_LOC0
#define BOARD_USART_ROUTELOC_TXLOC      USART_ROUTELOC0_TXLOC_LOC0

#define BOARD_USART_CMU_BIT             CMU_HFPERCLKEN0_USART0

#define BOARD_UART_RX_IRQn              USART0_RX_IRQn
#define BOARD_UART_TX_IRQn              USART0_TX_IRQn

#define BOARD_USART_TX_PIN              0
#define BOARD_USART_RX_PIN              1
#endif



// Interrupt pin for waps uart
#define BOARD_UART_INT_PIN              13
#define BOARD_UART_INT_PORT             GPIOD

// Led mapping:
// On Wirepas kit board: LED0 = PF4, LED1 = PF5
#define BOARD_LED_PIN_LIST {{GPIOC, 10}, {GPIOC, 11}}
//#define BOARD_PWM_VALVE_DRIVER_PORT     			GPIOA // y.f. 19/02/2020 add for vocom enable 
//#define BOARD_PWM_VALVE_DRIVER_P_PIN    			5
// Button mapping:
// No buttons on board nor on Wirepas kit board.
// Define here if want to use HAL_BUTTON=yes
// Then also need to implement hal/button.c
// An example:
//#define BOARD_BUTTON_PIN_LIST {{GPIOF, 7}}

#define BOARD_LED_PORT 								GPIOC
#define BOARD_LED_PIN_0 							10
#define BOARD_LED_PIN_1 							11
//#define BOARD_METER_CUBIC_PULSE_TENTHES_INT_PORT   	GPIOC
//#define BOARD_METER_ONE_CUBIC_INT_PORT   			GPIOC
//#define BOARD_METER_TENTHES_CUBIC_PULSE_PIN         7
//#define BOARD_METER_ONE_CUBIC_PULSE_PIN             8

//#define BOARD_ST_INT_PORT   			        GPIOC    // Y.F. 6/9/2022 REMOVED 
//#define BOARD_ST_TX_INT_PIN                             6      // Y.F. 6/9/2022 REMOVED
//#define BOARD_ST_RX_INT_PIN                             8      // Y.F. 6/9/2022 REMOVED

 /* BOARD_BRD4158A_BOARD_H_ */
#endif