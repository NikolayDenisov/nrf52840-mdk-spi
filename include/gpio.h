#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define SPI_PIN_MOSI 17
#define SPI_PIN_SCK 20
#define SPI_PIN_CS 22
#define EPD_PIN_DC 24
#define EPD_PIN_RST 9
#define EPD_PIN_BUSY 11
#define HDC2080_INT_PIN 13

void gpio_pin_set(uint32_t pin);
void gpio_pin_clear(uint32_t pin);
uint8_t gpio_pin_read(uint32_t pin);
void gpio_init();
void gpio_config_for_interrupt(void);

#endif