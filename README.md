# nRF52840 + HDC2080 + E-Ink (EPD_2in13_V3) Weather Display

A simple embedded C project for the **nRF52840** microcontroller that reads temperature and humidity from the **HDC2080** sensor and displays the data on an **E-Ink EPD_2in13_V3** display.

## Features

- Temperature and humidity measurement via I2C (TWI) using HDC2080
- Display data on a 2.13" E-Ink screen via SPI
- GPIOTE interrupt to update screen on new data ready
- Energy-efficient sleep using `__WFE()` and `__SEV()` instructions
- Uses NRF SDK-style low-level register programming

---

## Hardware Components

- [nRF52840 MCU board (e.g., nRF52840-MDK)](https://wiki.makerdiary.com/nrf52840-mdk/)
- [Texas Instruments HDC2080 temperature and humidity sensor](https://www.ti.com/product/HDC2080)
- [Waveshare E-Ink 2.13" V3 display](https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT_(B))


## How It Works
1. The system initializes peripherals: TWI (I2C), SPI, GPIO, display, and sensor.
1. HDC2080 is configured to trigger interrupts when new data is available.
1. On interrupt (via GPIOTE_IRQHandler), temperature and humidity are read.
1. Display is partially updated with new readings using epd_display_partial.
1. The system sleeps in a low-power mode until the next interrupt.

## Build & Flash
```bash
git clone https://github.com/NikolayDenisov/nrf52840-mdk-spi.git
cd nrf52840-mdk-spi/
cd armgcc/
make flash
```
