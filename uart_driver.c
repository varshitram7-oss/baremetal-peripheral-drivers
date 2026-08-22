/**
 * @file uart_driver.c
 * @brief Implementation of Bare-Metal Register-Level UART Driver
 * @author Vila Ram Varshit (github.com/varshitram7-oss)
 */

#include "uart_driver.h"

/* Memory-Mapped Hardware Register Addresses (Example: ATmega328P / AVR architecture) */
#define UBRR0H   (*(volatile uint8_t *)0xC5)
#define UBRR0L   (*(volatile uint8_t *)0xC4)
#define UCSR0A   (*(volatile uint8_t *)0xC0)
#define UCSR0B   (*(volatile uint8_t *)0xC1)
#define UCSR0C   (*(volatile uint8_t *)0xC2)
#define UDR0     (*(volatile uint8_t *)0xC6)

/* Bit Position Definitions */
#define RXEN0    (4)
#define TXEN0    (3)
#define RXCIE0   (7)
#define UDRE0    (5)
#define RXC0     (7)
#define UCSZ00   (1)
#define UCSZ01   (2)

static RingBuffer_t rx_ring_buffer = { .head = 0, .tail = 0 };

void UART_Init(uint32_t baudrate, uint32_t cpu_f_hz)
{
    /* 1. Calculate and set baud rate register (UBRR = (F_CPU / (16 * Baud)) - 1) */
    uint16_t ubrr_val = (uint16_t)((cpu_f_hz / (16UL * baudrate)) - 1UL);
    UBRR0H = (uint8_t)(ubrr_val >> 8);
    UBRR0L = (uint8_t)(ubrr_val & 0xFF);

    /* 2. Configure Frame Format: 8 Data Bits, 1 Stop Bit, No Parity (8N1) */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    /* 3. Enable Transmitter, Receiver, and RX Complete Interrupt */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

void UART_SendByte(uint8_t data)
{
    /* Wait for Transmit Data Register Empty flag (UDRE0) */
    while (!(UCSR0A & (1 << UDRE0)));

    /* Load byte into hardware transmit register */
    UDR0 = data;
}

void UART_SendString(const char *str)
{
    while (*str)
    {
        UART_SendByte((uint8_t)(*str));
        str++;
    }
}

bool UART_ReadByte(uint8_t *byte_out)
{
    if (rx_ring_buffer.head == rx_ring_buffer.tail)
    {
        return false; /* Buffer is empty */
    }

    *byte_out = rx_ring_buffer.buffer[rx_ring_buffer.tail];
    rx_ring_buffer.tail = (rx_ring_buffer.tail + 1) % UART_RX_BUFFER_SIZE;
    return true;
}

bool UART_IsDataAvailable(void)
{
    return (rx_ring_buffer.head != rx_ring_buffer.tail);
}

uint16_t UART_GetAvailableCount(void)
{
    if (rx_ring_buffer.head >= rx_ring_buffer.tail)
    {
        return (rx_ring_buffer.head - rx_ring_buffer.tail);
    }
    return (UART_RX_BUFFER_SIZE - (rx_ring_buffer.tail - rx_ring_buffer.head));
}

/**
 * @brief UART RX Complete Interrupt Service Routine
 */
void USART_RX_vect(void) __attribute__((signal));
void USART_RX_vect(void)
{
    uint8_t received_byte = UDR0; /* Read byte from hardware register */
    uint16_t next_head = (rx_ring_buffer.head + 1) % UART_RX_BUFFER_SIZE;

    /* Push byte to Ring Buffer if not full (prevents overwriting unread tail) */
    if (next_head != rx_ring_buffer.tail)
    {
        rx_ring_buffer.buffer[rx_ring_buffer.head] = received_byte;
        rx_ring_buffer.head = next_head;
    }
}
