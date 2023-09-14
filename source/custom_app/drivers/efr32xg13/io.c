/* Copyright 2019 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */


#include "hal_api.h"
#include "io.h"
#include "board.h"

void Io_init(void)
{
//    /* Enable clocks */
    CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;
    hal_gpio_set_mode(GPIOC,
                      10,
                      GPIO_MODE_OUT_PP);
    hal_gpio_set_mode(GPIOC,
                      11,
                      GPIO_MODE_OUT_PP);
    hal_gpio_clear(GPIOC,
                   10);
    hal_gpio_clear(GPIOC,
                   11);
    hal_gpio_set_mode(BOARD_UART_INT_PORT,
                      BOARD_UART_INT_PIN,
                      GPIO_MODE_DISABLED);
    hal_gpio_clear(BOARD_UART_INT_PORT,
                   BOARD_UART_INT_PIN);

 // pulser input pins

#if 0
    hal_gpio_set_mode(BOARD_METER_CUBIC_PULSE_TENTHES_INT_PORT,
    					BOARD_METER_TENTHES_CUBIC_PULSE_PIN,
						GPIO_MODE_IN_PULL);
    hal_gpio_clear(BOARD_METER_CUBIC_PULSE_TENTHES_INT_PORT,
    					BOARD_METER_TENTHES_CUBIC_PULSE_PIN);

    hal_gpio_set_mode(BOARD_METER_ONE_CUBIC_INT_PORT,
    					BOARD_METER_ONE_CUBIC_PULSE_PIN,
						GPIO_MODE_IN_PULL);
    hal_gpio_clear(BOARD_METER_ONE_CUBIC_INT_PORT,
    					BOARD_METER_ONE_CUBIC_PULSE_PIN);

 // Open sw detect (opened door)


    hal_gpio_set_mode(BOARD_METER_SW_OPEN_INT_PORT,
    					BOARD_METER_SW_OPEN_INT_PIN,
					  GPIO_MODE_IN_PULL);

    hal_gpio_set(BOARD_METER_SW_OPEN_INT_PORT,
    				BOARD_METER_SW_OPEN_INT_PIN);


    // Magnet detect (Tamper)


    hal_gpio_set_mode(BOARD_METER_MAGNET_INT_PORT,
    				BOARD_METER_MAGNET_INT_PIN,
					  GPIO_MODE_IN_PULL);
    hal_gpio_set(BOARD_METER_MAGNET_INT_PORT,
    				BOARD_METER_MAGNET_INT_PIN);

    /*  y.f i2c0 for eeprom */
    /* Using PD0 (SDA) and PD1 (SCL) */
    hal_gpio_set_mode(BOARD_METER_I2C_PORT,
      					BOARD_METER_I2C_SCL_PIN,
      					GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER);
     hal_gpio_set(BOARD_METER_I2C_PORT,
      					BOARD_METER_I2C_SCL_PIN);

     hal_gpio_set_mode(BOARD_METER_I2C_PORT,
    					BOARD_METER_I2C_SDA_PIN,
						GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER);

     hal_gpio_set(BOARD_METER_I2C_PORT,
						BOARD_METER_I2C_SDA_PIN);
	 hal_gpio_set_mode(BOARD_FLASH_ENABLE_PORT,
			 	 	 	 	 BOARD_FLASH_ENABLE_PIN,
							GPIO_MODE_OUT_PP);
	 hal_gpio_set(BOARD_FLASH_ENABLE_PORT,
			 	 	 	 	 BOARD_FLASH_ENABLE_PIN);

	 hal_gpio_set_mode(BOARD_METER_NGPI_INTRRUPT_PORT,
			 	 	 	 BOARD_METER_NGPI_INTRRUPT_PIN,
	 					  GPIO_MODE_IN_PULL);

	 hal_gpio_set(BOARD_METER_NGPI_INTRRUPT_PORT,
			 	 	 	 BOARD_METER_NGPI_INTRRUPT_PIN);

   hal_gpio_set_mode(BOARD_PWM_VALVE_DRIVER_PORT,
    					BOARD_PWM_VALVE_DRIVER_P_PIN,
						GPIO_MODE_OUT_PP);
#ifdef METER_BOARD    
    hal_gpio_set(BOARD_PWM_VALVE_DRIVER_PORT,
    					BOARD_PWM_VALVE_DRIVER_P_PIN);
#else
    hal_gpio_clear(BOARD_PWM_VALVE_DRIVER_PORT,
    					BOARD_PWM_VALVE_DRIVER_P_PIN);
#endif    
    hal_gpio_set_mode(BOARD_METER_I2C_ENABLE_PORT,
    					BOARD_METER_I2C_ENABLE_PIN,
						GPIO_MODE_OUT_PP);
    hal_gpio_set(BOARD_METER_I2C_ENABLE_PORT,
    					BOARD_METER_I2C_ENABLE_PIN);
    hal_gpio_set_mode(BOARD_PWM_VALVE_DRIVER_PORT,
    					BOARD_PWM_VALVE_DRIVER_N_PIN,
						GPIO_MODE_OUT_PP);
    hal_gpio_set(BOARD_PWM_VALVE_DRIVER_PORT,
    					BOARD_PWM_VALVE_DRIVER_N_PIN);

    hal_gpio_set_mode(BOARD_PWM_VALVE_DRIVER_PORT,
    					BOARD_PWM_VALVE_DRIVER_ENABLE_PIN,
						GPIO_MODE_OUT_PP);
    hal_gpio_set(BOARD_PWM_VALVE_DRIVER_PORT,
    					BOARD_PWM_VALVE_DRIVER_ENABLE_PIN);
//#endif
    hal_gpio_clear(BOARD_UART_INT_PORT,
                   BOARD_UART_INT_PIN);
    // y.f add 18/02/20/20
    
    hal_gpio_set_mode(GPIOF,
                      4,
                      GPIO_MODE_OUT_PP);
    hal_gpio_set_mode(GPIOF,
                      5,
                      GPIO_MODE_OUT_PP);
    hal_gpio_clear(GPIOF,
                   4);
    hal_gpio_clear(GPIOF,
                   5);
#endif // y.f. 9/6/2022 removed in new 32 pin device 

}

void Io_enableUartIrq(void)
{
    // 6/9/2022 removed in new 32 pin device

    hal_gpio_set_mode(BOARD_UART_INT_PORT,
                      BOARD_UART_INT_PIN,
                      GPIO_MODE_OUT_PP);
   
}

void Io_setUartIrq(void)
{
      // 6/9/2022 removed in new 32 pin device

    // Active low IRQ pin
    hal_gpio_clear(BOARD_UART_INT_PORT,
                   BOARD_UART_INT_PIN);
   
}

void Io_clearUartIrq(void)
{
      // 6/9/2022 removed in new 32 pin device

    // To clear we pull pin up
    hal_gpio_set(BOARD_UART_INT_PORT,
                 BOARD_UART_INT_PIN);
    
}

void Io_setModeDisabled(void)
{
      // 6/9/2022 removed in new 32 pin device

    // Disable pin
    hal_gpio_set_mode(BOARD_UART_INT_PORT,
                      BOARD_UART_INT_PIN,
                      GPIO_MODE_DISABLED);
    
}
