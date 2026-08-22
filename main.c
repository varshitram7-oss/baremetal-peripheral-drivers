/**
 * @file main.c
 * @brief Integration Test Application for Bare-Metal Peripheral Drivers
 * @author Vila Ram Varshit (github.com/varshitram7-oss)
 */

#include <stdio.h>
#include "uart_driver.h"
#include "spi_driver.h"
#include "timer_driver.h"

#define CPU_FREQ_HZ (16000000UL) /* 16 MHz Clock */

int main(void)
{
    /* 1. Initialize Peripherals */
    UART_Init(115200, CPU_FREQ_HZ);
    SPI_Master_Init(SPI_MODE_0);
    Timer1_PWM_Init();

    UART_SendString("\r\n======================================================\r\n");
    UART_SendString("   Bare-Metal Peripheral Driver Suite Initialized     \r\n");
    UART_SendString("   Author: Vila Ram Varshit (github.com/varshitram7-oss)\r\n");
    UART_SendString("======================================================\r\n\r\n");

    uint8_t duty = 0;
    uint8_t received_byte = 0;

    while (1)
    {
        /* 1. UART Echo with Ring Buffer (FIFO) */
        if (UART_ReadByte(&received_byte))
        {
            UART_SendString("[UART ECHO]: ");
            UART_SendByte(received_byte);
            UART_SendString("\r\n");
        }

        /* 2. PWM Duty Cycle Sweep */
        Timer1_PWM_SetDutyCycle(duty);
        duty = (duty + 5) % 105;

        /* 3. Simulated SPI Sensor Read */
        SPI_ChipSelect_Set(true);
        uint8_t sensor_reg_val = SPI_TransferByte(0x75); /* Read WHO_AM_I register */
        SPI_ChipSelect_Set(false);

        Timer_DelayMs(100);
    }

    return 0;
}
