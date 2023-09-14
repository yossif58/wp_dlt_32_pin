#ifndef _PWM_H__
#define _PWM_H__
#undef PUBLIC

#ifdef METERVALV_H
#define  PUBLIC
#else
#define  PUBLIC  extern
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief   Initialize Pwm module
 */
void Pwm_init();

/**
 * \brief   Set the duty cycle of the pwm
 * \param   pwm_value
 *          New pwm value in %
 * \return  True if value changed
 */
PUBLIC uint8_t  MeterValveState;
#define METER_VALVE_CLOSED  0
#define METER_VALVE_OPEND   1
void OpenValve(void);
uint32_t CloseValve(void);


#endif
