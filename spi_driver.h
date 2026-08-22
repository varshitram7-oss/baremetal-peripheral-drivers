/**
 * @file spi_driver.h
 * @brief Bare-Metal Register-Level SPI Master Driver
 * @author Vila Ram Varshit (github.com/varshitram7-oss)
 */

#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief SPI Clock Modes (CPOL / CPHA)
 */
typedef enum {
    SPI_MODE_0 = 0, /* CPOL=0, CPHA=0 */
    SPI_MODE_1 = 1, /* CPOL=0, CPHA=1 */
    SPI_MODE_2 = 2, /* CPOL=1, CPHA=0 */
    SPI_MODE_3 = 3  /* CPOL=1, CPHA=1 */
} SpiMode_t;

/**
 * @brief Initializes SPI peripheral in Master Mode.
 */
void SPI_Master_Init(SpiMode_t mode);

/**
 * @brief Transmits and receives a byte simultaneously over SPI (Full-Duplex).
 */
uint8_t SPI_TransferByte(uint8_t data);

/**
 * @brief Transmits a buffer of bytes over SPI.
 */
void SPI_TransmitBuffer(const uint8_t *tx_buf, uint32_t length);

/**
 * @brief Receives a buffer of bytes over SPI by sending dummy clock bytes.
 */
void SPI_ReceiveBuffer(uint8_t *rx_buf, uint32_t length);

/**
 * @brief Controls Slave Select (SS/CS) pin.
 */
void SPI_ChipSelect_Set(bool active);

#endif /* SPI_DRIVER_H */
