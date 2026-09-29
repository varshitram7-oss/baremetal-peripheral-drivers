# Bare-Metal Register-Level Peripheral Driver Suite (No HAL)

[![Language: C](https://img.shields.io/badge/Language-Embedded%20C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Architecture: AVR (ATmega328P)](https://img.shields.io/badge/Architecture-AVR%20(ATmega328P)-green.svg)](https://www.microchip.com/en-us/technology/avr-8-bit-microcontrollers)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

▶️ **[Try it live on Wokwi](https://wokwi.com/projects/476481937910126593)** — runs the drivers in a browser simulation (Arduino Uno). Type in the serial monitor for the UART echo; the LED sweeps with the PWM duty cycle.

*Note: the live demo uses the driver sources unmodified; only an Arduino setup()/loop() entry shim was added for the simulator.*



A modular, high-performance **Bare-Metal Peripheral Driver Suite** written from scratch in Embedded C **without vendor HAL libraries** (Hardware Abstraction Layers). Demonstrates direct memory-mapped register manipulation, interrupt service routines (**ISRs**), lock-free **Circular Ring Buffers (FIFO)**, and hardware timer configurations for **PWM generation**.

---

## 📌 1. Project Overview

In high-reliability and resource-constrained embedded systems, vendor HAL libraries introduce significant code bloat, latency overhead, and hidden blocking loops. 

This repository implements lightweight, deterministic register-level drivers for:
1. **UART Driver:** Interrupt-driven asynchronous serial communication with a thread-safe **Circular Ring Buffer (FIFO)**.
2. **SPI Driver:** High-speed full-duplex Master Mode driver supporting configurable **Clock Polarity (CPOL)** and **Clock Phase (CPHA)**.
3. **Timer & Fast PWM Driver:** Hardware timer configuration with custom prescalers, Top/Compare registers, and variable duty-cycle generation.

---

## 🗺️ 2. Driver Architecture & Memory-Mapped Registers

```text
+-------------------------------------------------------------------------+
|                         APPLICATION LAYER (main.c)                      |
+------------------------------------+------------------------------------+
                                     |
        +----------------------------+----------------------------+
        |                            |                            |
        v                            v                            v
+------------------+         +------------------+         +------------------+
|   UART Driver    |         |    SPI Driver    |         |   Timer Driver   |
| (Ring Buffer FIFO)|        |   (Master Mode)  |         |    (Fast PWM)    |
+--------+---------+         +--------+---------+         +--------+---------+
         |                            |                            |
         | (Bit-Masking Pointers)     |                            |
         v                            v                            v
+-------------------------------------------------------------------------+
|                  MICROCONTROLLER MEMORY-MAPPED REGISTERS                 |
|   - UART:  UBRR0H/L (Baud), UCSR0A/B/C (Control), UDR0 (Data)           |
|   - SPI:   SPCR (Control), SPSR (Status), SPDR (Data), DDRB/PORTB (SS)   |
|   - TIMER: TCCR1A/B (Mode), ICR1 (Period TOP), OCR1A (Duty Compare)      |
+-------------------------------------------------------------------------+
```

---

## 🔄 3. Circular Ring Buffer (FIFO) Mechanics

To prevent packet loss when data arrives over UART faster than the application loop can process it, an interrupt-driven FIFO Ring Buffer is utilized:

```text
[0] [1] [2] [3] [4] [5] [6] [7] ... [127]
     ^               ^
     | (Tail: Read)  | (Head: Written by ISR)

- Data Arrival: USART_RX_vect ISR stores byte at `buffer[head]` -> `head = (head + 1) % SIZE`
- Data Read:    Application reads byte from `buffer[tail]`     -> `tail = (tail + 1) % SIZE`
- Buffer Empty: `head == tail`
- Buffer Full:  `(head + 1) % SIZE == tail`
```

---

## 📊 4. Peripheral Specifications & Features

| Peripheral | Operating Mode | Clock / Baud | Key Registers & Concepts |
| :--- | :--- | :--- | :--- |
| **UART** | 8N1 (8 Data, No Parity, 1 Stop) | 115200 Baud | `UBRR0`, `UCSR0B` (`RXCIE0`), `UDR0`, Non-blocking FIFO Buffer |
| **SPI** | Full-Duplex Master Mode | $F_{CPU} / 16$ | `SPCR` (`SPE`, `MSTR`), `SPSR` (`SPIF`), Manual GPIO Chip-Select |
| **Timer 1**| 16-Bit Fast PWM (Mode 14) | 50 Hz (20 ms) | `TCCR1A/B` (`COM1A1`), `ICR1` (TOP), `OCR1A` (Duty Match) |

---

## 💻 5. Core Driver Code Snippets

### A. Register-Level UART Initialization
```c
void UART_Init(uint32_t baudrate, uint32_t cpu_f_hz)
{
    /* Calculate baud divisor */
    uint16_t ubrr_val = (uint16_t)((cpu_f_hz / (16UL * baudrate)) - 1UL);
    UBRR0H = (uint8_t)(ubrr_val >> 8);
    UBRR0L = (uint8_t)(ubrr_val & 0xFF);

    /* 8-bit frame format (8N1) */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    /* Enable TX, RX, and RX Complete Interrupt */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}
```

### B. Full-Duplex SPI Byte Transfer
```c
uint8_t SPI_TransferByte(uint8_t data)
{
    /* Load byte into SPI Data Register */
    SPDR = data;

    /* Wait for transmission flag (SPIF) in Status Register */
    while (!(SPSR & (1 << SPIF)));

    /* Return received byte */
    return SPDR;
}
```

---

## 🚀 6. How to Build & Run

### Prerequisites
- `avr-gcc` toolchain
- `make` utility

### Compilation
```bash
# Compile driver suite with optimizations
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Wall -Os -o drivers.elf main.c uart_driver.c spi_driver.c timer_driver.c

# Generate flashable hex file
avr-objcopy -O ihex -R .eeprom drivers.elf drivers.hex
```

---

## 🎯 7. Technical Interview Q&A

### Q1: Why is the `volatile` keyword essential for memory-mapped register pointers and ring buffer indexes?
> The `volatile` qualifier tells the compiler that the value at the address can change unexpectedly at any time due to hardware events or interrupts. Without `volatile`, the compiler optimizer might cache the register value in a CPU register or optimize out `while (!(SPSR & (1 << SPIF)))` into an infinite loop or eliminate repeated memory reads.

### Q2: How does a Circular Ring Buffer avoid race conditions without disabling interrupts?
> In a single-producer (ISR), single-consumer (main loop) ring buffer:
> - The **ISR only modifies the `head` pointer**.
> - The **main thread only modifies the `tail` pointer**.
> Because neither execution context modifies the other's pointer, data integrity is maintained without needing to disable interrupts.

### Q3: What is the difference between SPI Mode 0 and Mode 3?
> - **Mode 0:** Clock Polarity ($CPOL=0$, Idle Low), Clock Phase ($CPHA=0$, Sample on Leading/Rising edge).
> - **Mode 3:** Clock Polarity ($CPOL=1$, Idle High), Clock Phase ($CPHA=1$, Sample on Trailing/Rising edge).

---

## 👤 Author

**Vila Ram Varshit**  
- **LinkedIn:** [linkedin.com/in/ram-varshit-ece](https://www.linkedin.com/in/ram-varshit-ece/)  
- **GitHub:** [github.com/varshitram7-oss](https://github.com/varshitram7-oss)  
- **Email:** ramvarshit18@gmail.com
