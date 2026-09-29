# Demo Wiring — Bare-Metal Peripheral Driver Suite

Target: **ATmega328P** (Arduino Uno), 16 MHz clock, UART @ 115200 8N1.
Every pin mapping below comes straight from the driver sources.

## UART (`uart_driver.c`)

| Signal | ATmega328P pin | Arduino header |
|--------|----------------|----------------|
| RXD    | PD0            | D0             |
| TXD    | PD1            | D1             |

Frame format 8N1 (8 data bits, 1 stop bit, no parity), configured in `UCSR0C`.
Connect to a USB-serial adapter (or the Uno's onboard serial) and open a
serial monitor at **115200 baud**. On boot you should see:

```text
======================================================
 Bare-Metal Peripheral Driver Suite Initialized
 Author: Vila Ram Varshit (github.com/varshitram7-oss)
======================================================
```

Type characters into the monitor — the firmware echoes each byte back with a
`[UART ECHO]:` prefix, received through the interrupt-driven RX ring buffer.

## SPI master (`spi_driver.c`)

| Signal | ATmega328P pin | Arduino header |
|--------|----------------|----------------|
| SS     | PB2            | D10            |
| MOSI   | PB3            | D11            |
| MISO   | PB4            | D12            |
| SCK    | PB5            | D13            |

Configured as SPI **Mode 0**, clock = F_CPU/16 (1 MHz at 16 MHz).
`main.c` performs a simulated sensor read: it asserts SS and clocks out
`0x75` (a WHO_AM_I register-address pattern) via `SPI_TransferByte()`.

## Timer1 PWM (`timer_driver.c`)

| Signal | ATmega328P pin | Arduino header |
|--------|----------------|----------------|
| OC1A   | PB1            | D9             |

Fast PWM, Mode 14 with `ICR1` = 39999 as TOP — exactly **50 Hz** at
16 MHz with the /8 prescaler. `main.c` sweeps the duty cycle 0–100%
in 5% steps every 100 ms.

**LED demo:** `D9 → 220 Ω resistor → LED anode`, `LED cathode → GND`.
The LED visibly ramps up and down in brightness with the duty sweep.

## Wokwi simulation

No hardware needed — the
[Wokwi demo](https://wokwi.com/projects/476481937910126593)
(Arduino Uno + LED on D9 through a 220 Ω resistor) runs this exact
firmware: press **Start Simulation** and watch the serial monitor.
