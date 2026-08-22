/**
 * @file timer_driver.c
 * @brief Implementation of Bare-Metal Timer & Fast PWM Driver
 * @author Vila Ram Varshit (github.com/varshitram7-oss)
 */

#include "timer_driver.h"

/* Memory-Mapped Timer 1 Registers */
#define TCCR1A   (*(volatile uint8_t *)0x80)
#define TCCR1B   (*(volatile uint8_t *)0x81)
#define ICR1     (*(volatile uint16_t *)0x86)
#define OCR1A    (*(volatile uint16_t *)0x88)
#define DDRB     (*(volatile uint8_t *)0x24)

#define PIN_OC1A (1) /* PB1 / Pin 9 */

#define COM1A1   (7)
#define WGM13    (4)
#define WGM12    (3)
#define WGM11    (1)
#define CS11     (1) /* Prescaler / 8 */

#define TOP_VALUE (39999U) /* Generates exactly 50 Hz PWM with 16 MHz Clock & /8 Prescaler */

void Timer1_PWM_Init(void)
{
    /* 1. Configure PB1 (OC1A) as Output pin */
    DDRB |= (1 << PIN_OC1A);

    /* 2. Configure Fast PWM Mode with ICR1 as TOP (Mode 14) */
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    /* 3. Set TOP value for 50 Hz (20ms period) */
    ICR1 = TOP_VALUE;

    /* 4. Set Initial Duty Cycle to 0% */
    OCR1A = 0;
}

void Timer1_PWM_SetDutyCycle(uint8_t duty_cycle_pct)
{
    if (duty_cycle_pct > 100) duty_cycle_pct = 100;

    /* Calculate compare match value based on percentage */
    uint32_t compare_val = ((uint32_t)duty_cycle_pct * TOP_VALUE) / 100UL;
    OCR1A = (uint16_t)compare_val;
}

void Timer_DelayMs(uint32_t ms)
{
    /* Simple calibrated loop for bare-metal delay */
    while (ms--)
    {
        for (volatile uint32_t i = 0; i < 1600; i++)
        {
            __asm__ volatile ("nop");
        }
    }
}
