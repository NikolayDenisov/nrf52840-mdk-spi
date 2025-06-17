#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define SPI_PIN_MOSI 21
#define SPI_PIN_SCK 17
#define SPI_PIN_CS 16
#define EPD_PIN_DC 15
#define EPD_PIN_RST 14
#define EPD_PIN_BUSY 13

void gpio_pin_set(uint32_t pin);
void gpio_pin_clear(uint32_t pin);
uint8_t gpio_pin_read(uint32_t pin);
void gpio_init();

#endif