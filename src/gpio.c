
#include "gpio.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"

void gpio_pin_set(uint32_t pin) { NRF_P0->OUTSET = (1 << pin); }

void gpio_pin_clear(uint32_t pin) { NRF_P0->OUTCLR = (1 << pin); }

uint8_t gpio_pin_read(uint32_t pin) {
  return (NRF_P0->IN & (1 << pin)) ? 1 : 0;
}

void gpio_init() {
  gpio_pin_set(PIN_SCK);
  NRF_P0->DIRSET = (1 << PIN_SCK); // Set SCK as output

  gpio_pin_clear(PIN_MOSI);
  NRF_P0->DIRSET = (1 << PIN_MOSI); // Set MOSI as output

  gpio_pin_set(PIN_CS);
  NRF_P0->DIRSET = (1 << PIN_CS); // Set CS as output
  // NRF_P0->OUTSET = (1 << PIN_CS);   // Set CS high

  NRF_P0->DIRSET = (1 << PIN_RST) | (1 << PIN_DC);
  NRF_P0->DIRCLR = (1 << PIN_BUSY);
  NRF_P0->PIN_CNF[PIN_BUSY] =
      (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
  gpio_pin_clear(PIN_DC);
}