/**
 * @file uart_driver.h
 * @brief Bare-Metal Register-Level UART Driver with Circular Ring Buffer (FIFO)
 * @author Vila Ram Varshit (github.com/varshitram7-oss)
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define UART_RX_BUFFER_SIZE  (128)
#define UART_TX_BUFFER_SIZE  (128)

/**
 * @brief Circular Ring Buffer Data Structure
 */
typedef struct {
    uint8_t buffer[UART_RX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} RingBuffer_t;

/**
 * @brief Initializes UART hardware peripheral with specified baud rate.
 * @param baudrate Desired baud rate (e.g. 9600, 115200)
 * @param cpu_f_hz CPU clock frequency in Hz (e.g. 16000000UL)
 */
void UART_Init(uint32_t baudrate, uint32_t cpu_f_hz);

/**
 * @brief Transmits a single byte over UART.
 */
void UART_SendByte(uint8_t data);

/**
 * @brief Transmits a null-terminated string over UART.
 */
void UART_SendString(const char *str);

/**
 * @brief Reads a byte from the RX Ring Buffer (FIFO).
 * @param byte_out Pointer where received byte will be stored.
 * @return true if byte was retrieved, false if buffer is empty.
 */
bool UART_ReadByte(uint8_t *byte_out);

/**
 * @brief Checks if unread bytes are available in the RX Ring Buffer.
 */
bool UART_IsDataAvailable(void);

/**
 * @brief Returns the number of unread bytes in the RX Ring Buffer.
 */
uint16_t UART_GetAvailableCount(void);

#endif /* UART_DRIVER_H */
