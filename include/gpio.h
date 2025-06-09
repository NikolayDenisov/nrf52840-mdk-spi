#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define PIN_MOSI 21
#define PIN_SCK 17
#define PIN_CS 16
#define PIN_DC 15
#define PIN_RST 14
#define PIN_BUSY 13

void gpio_pin_set(uint32_t pin);
void gpio_pin_clear(uint32_t pin);
uint8_t gpio_pin_read(uint32_t pin);
void gpio_init();

#endif