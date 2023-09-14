#include "board.h"
#include "mcu.h"
#include "vendor/efr32fg13/efr32_gpio.h"
#include "api.h"
#include "app.h"
#if 0
y.f this section of valve is to be modified acording to EFR regs when water meter and GAS meter are relevant
#define METERVALV_H
#include "metervalve.h"
#include <stdlib.h>

/** Is PWM Initialized */
static bool m_positive_initialized = false;
static bool m_negative_initialized = false;



void Pwm_valve_set_value(uint8_t BOARD_PWM_PIN,uint32_t pwm_value);
void Pwm_valve_init(uint8_t BOARD_PWM_PIN);


void OpenValve(void)
{
	if (MeterValveState==METER_VALVE_OPEND)    return; // no need to open valve is already opened
	Pwm_valve_init(BOARD_PWM_VALVE_DRIVER_P_PIN);
	Pwm_valve_set_value(BOARD_PWM_VALVE_DRIVER_P_PIN,50);
	MeterValveState = METER_VALVE_OPEND;
	lib_system->setPeriodicCb(CloseValve,1000000,100);
}

uint32_t CloseValve(void)
{
	if (MeterValveState!=METER_VALVE_CLOSED)
	{

		Pwm_valve_set_value(BOARD_PWM_VALVE_DRIVER_P_PIN,0);
		MeterValveState = METER_VALVE_CLOSED;

	}
	return APP_LIB_SYSTEM_STOP_PERIODIC;
}



void Pwm_valve_init(uint8_t BOARD_PWM_PIN)
{
    uint8_t shift_output;

    // Enable TIMER2
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER2;

#if (BOARD_PWM_PIN > 7)
    shift_output = (BOARD_PWM_PIN - 8) * 4;
    GPIO->P[BOARD_PWM_VALVE_DRIVER_PORT].MODEH &= ~(0xf << shift_output);
    GPIO->P[BOARD_PWM_VALVE_DRIVER_PORT].MODEH |= (0x4 << shift_output);
#else
    shift_output = (BOARD_PWM_PIN) * 4;
    GPIO->P[BOARD_PWM_VALVE_DRIVER_PORT].MODEL &= ~(0xf << shift_output);
    GPIO->P[BOARD_PWM_VALVE_DRIVER_PORT].MODEL |= (0x4 << shift_output);
#endif
   hal_gpio_clear(BOARD_PWM_VALVE_DRIVER_PORT,BOARD_PWM_PIN);  // On by default y.f 22/04/2020 change to ha_gpio_clear for efe

    // Add prescaler to have a 250 KHz frequency
    TIMER2->CTRL |= (TIMER_CTRL_PRESC_DIV64 | TIMER_CTRL_DEBUGRUN);

    // Configure TOP value to configure global period
    // With 100, it gives a GP of 2,5KHz and an easy configuration
    TIMER2->TOP = 100;

    // Configure timer CNT
    TIMER2->CNT = 0;

    // Set timer1 channel 2 in PWM mode
    if (BOARD_PWM_PIN == BOARD_PWM_VALVE_DRIVER_P_PIN)
    {
    	TIMER2->CC[1].CTRL = TIMER_CC_CTRL_MODE_PWM;   // y.f. pa13 timer2 cc1
    	m_positive_initialized = true;
        m_negative_initialized = false;
    }
    else
    {
    	TIMER2->CC[0].CTRL = TIMER_CC_CTRL_MODE_PWM;  // y.f. pa12 timer2 cc0
    	m_positive_initialized = false;
        m_negative_initialized = true;
    }

}

void Pwm_valve_set_value(uint8_t BOARD_PWM_PIN,uint32_t pwm_value)
{
    if ((pwm_value > 100 ) || ( (!m_positive_initialized)&&(BOARD_PWM_PIN==BOARD_PWM_VALVE_DRIVER_P_PIN))
    		|| ( (!m_negative_initialized)&&(BOARD_PWM_PIN==BOARD_PWM_VALVE_DRIVER_N_PIN)))
        return;

    if (pwm_value == 100)
    {
        // Disable output
        TIMER2->ROUTE &= ~(TIMER_ROUTE_LOCATION_LOC1 | TIMER_ROUTE_CC2PEN);

        // Stop timer
        TIMER2->CMD = TIMER_CMD_STOP;

        // Disable timer 2
        CMU->HFPERCLKEN0 &= ~(CMU_HFPERCLKEN0_TIMER2);
        lib_system->disableDeepSleep(false);

        // Set output to 1
        hal_gpio_set(BOARD_PWM_VALVE_DRIVER_PORT,BOARD_PWM_PIN);  // y.f 22/04/2020 change to ha_gpio_clear for efe

      //  GPIO->P[BOARD_PWM_VALVE_DRIVER_PORT].DOUTSET = 1 << BOARD_PWM_PIN;

    }
    else if (pwm_value == 0)
    {
        // Disable output
        TIMER2->ROUTE &= ~(TIMER_ROUTE_LOCATION_LOC1 | TIMER_ROUTE_CC2PEN);
        // Stop timer
        TIMER2->CMD = TIMER_CMD_STOP;

        // Disable timer 2
        CMU->HFPERCLKEN0 &= ~(CMU_HFPERCLKEN0_TIMER2);
        lib_system->disableDeepSleep(false);

        // Set output to 0
        hal_gpio_clear(BOARD_PWM_VALVE_DRIVER_PORT,BOARD_PWM_PIN);  // y.f 22/04/2020 change to ha_gpio_clear for efe
        hal_gpio_clear(BOARD_PWM_VALVE_DRIVER_PORT,BOARD_PWM_VALVE_DRIVER_ENABLE_PIN);  //  y.f 22/04/2020 change to ha_gpio_clear for efe

       // GPIO->P[BOARD_PWM_VALVE_DRIVER_PORT].DOUTCLR = 1 << BOARD_PWM_PIN;
       // GPIO->P[BOARD_PWM_VALVE_DRIVER_PORT].DOUTCLR = 1 << BOARD_PWM_VALVE_DRIVER_ENABLE_PIN;  // y.f. 21/10/2018 DISABLE valve operation

    }
    else
    {
        lib_system->disableDeepSleep(true);
        // Enable TIMER2
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER2;

        // Configure pwm timer
        TIMER2->CC[2].CCV = pwm_value;
        TIMER2->CMD = TIMER_CMD_START;

        // Set output route
        TIMER2->ROUTE |= (TIMER_ROUTE_LOCATION_LOC1 | TIMER_ROUTE_CC2PEN);
        hal_gpio_set(BOARD_PWM_VALVE_DRIVER_PORT,BOARD_PWM_VALVE_DRIVER_ENABLE_PIN);  // y.f 22/04/2020 change to ha_gpio_clear for efe

       // GPIO->P[BOARD_PWM_VALVE_DRIVER_PORT].DOUTSET = 1 << BOARD_PWM_VALVE_DRIVER_ENABLE_PIN;  // y.f. 21/10/2018 ENABLE valve operation
    }


}
#endif
