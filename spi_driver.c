/**
 * @file spi_driver.c
 * @brief Implementation of Bare-Metal Register-Level SPI Master Driver
 * @author Vila Ram Varshit (github.com/varshitram7-oss)
 */

#include "spi_driver.h"

/* Memory-Mapped SPI Registers */
#define SPCR     (*(volatile uint8_t *)0x4C) /* SPI Control Register */
#define SPSR     (*(volatile uint8_t *)0x4D) /* SPI Status Register */
#define SPDR     (*(volatile uint8_t *)0x4E) /* SPI Data Register */

#define DDRB     (*(volatile uint8_t *)0x24) /* Port B Direction Register */
#define PORTB    (*(volatile uint8_t *)0x25) /* Port B Output Register */

/* SPI Pin Positions on Port B */
#define PIN_SS   (2) /* PB2 */
#define PIN_MOSI (3) /* PB3 */
#define PIN_MISO (4) /* PB4 */
#define PIN_SCK  (5) /* PB5 */

/* SPCR Control Bits */
#define SPE      (6) /* SPI Enable */
#define MSTR     (4) /* Master Select */
#define CPOL     (3) /* Clock Polarity */
#define CPHA     (2) /* Clock Phase */
#define SPR0     (0) /* Clock Rate Select */

/* SPSR Status Bits */
#define SPIF     (7) /* SPI Interrupt Flag */

void SPI_Master_Init(SpiMode_t mode)
{
    /* 1. Set MOSI, SCK, and SS as Outputs; MISO as Input */
    DDRB |= (1 << PIN_MOSI) | (1 << PIN_SCK) | (1 << PIN_SS);
    DDRB &= ~(1 << PIN_MISO);

    /* 2. De-assert Slave Select (Active LOW, set HIGH initially) */
    PORTB |= (1 << PIN_SS);

    /* 3. Configure SPCR: Enable SPI, Master Mode, Clock Rate = F_CPU/16 */
    uint8_t spcr_val = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    if (mode == SPI_MODE_1 || mode == SPI_MODE_3) spcr_val |= (1 << CPHA);
    if (mode == SPI_MODE_2 || mode == SPI_MODE_3) spcr_val |= (1 << CPOL);

    SPCR = spcr_val;
}

uint8_t SPI_TransferByte(uint8_t data)
{
    /* 1. Start transmission by writing to SPI Data Register */
    SPDR = data;

    /* 2. Wait for transmission complete flag (SPIF) */
    while (!(SPSR & (1 << SPIF)));

    /* 3. Return received data from slave */
    return SPDR;
}

void SPI_TransmitBuffer(const uint8_t *tx_buf, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        SPI_TransferByte(tx_buf[i]);
    }
}

void SPI_ReceiveBuffer(uint8_t *rx_buf, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        rx_buf[i] = SPI_TransferByte(0xFF); /* Send dummy byte 0xFF to generate clock */
    }
}

void SPI_ChipSelect_Set(bool active)
{
    if (active)
    {
        PORTB &= ~(1 << PIN_SS); /* Pull SS LOW (Active) */
    }
    else
    {
        PORTB |= (1 << PIN_SS);  /* Pull SS HIGH (Inactive) */
    }
}
