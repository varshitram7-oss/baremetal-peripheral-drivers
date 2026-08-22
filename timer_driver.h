/**
 * @file timer_driver.h
 * @brief Bare-Metal Register-Level Timer and PWM Driver
 * @author Vila Ram Varshit (github.com/varshitram7-oss)
 */

#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>

/**
 * @brief Initializes 16-bit Timer 1 in Fast PWM Mode (Non-Inverting).
 */
void Timer1_PWM_Init(void);

/**
 * @brief Sets PWM Duty Cycle on Output Compare Pin.
 * @param duty_cycle_pct Duty cycle percentage (0 to 100).
 */
void Timer1_PWM_SetDutyCycle(uint8_t duty_cycle_pct);

/**
 * @brief Generates blocking millisecond delay using hardware Timer.
 */
void Timer_DelayMs(uint32_t ms);

#endif /* TIMER_DRIVER_H */
