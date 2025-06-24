
#include "gpio.h"
#include "nrf.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"

void gpio_pin_set(uint32_t pin) { NRF_P0->OUTSET = (1 << pin); }

void gpio_pin_clear(uint32_t pin) { NRF_P0->OUTCLR = (1 << pin); }

uint8_t gpio_pin_read(uint32_t pin) {
  return (NRF_P0->IN & (1 << pin)) ? 1 : 0;
}

void gpio_init() {
  gpio_pin_set(SPI_PIN_SCK);
  NRF_P0->DIRSET = (1 << SPI_PIN_SCK); // Set SCK as output

  gpio_pin_clear(SPI_PIN_MOSI);
  NRF_P0->DIRSET = (1 << SPI_PIN_MOSI); // Set MOSI as output

  gpio_pin_set(SPI_PIN_CS);
  NRF_P0->DIRSET = (1 << SPI_PIN_CS); // Set CS as output
  // NRF_P0->OUTSET = (1 << PIN_CS);   // Set CS high

  NRF_P0->DIRSET = (1 << EPD_PIN_RST) | (1 << EPD_PIN_DC);
  NRF_P0->DIRCLR = (1 << EPD_PIN_BUSY);
  NRF_P0->PIN_CNF[EPD_PIN_BUSY] =
      (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
  gpio_pin_clear(EPD_PIN_DC);
}

void gpio_config_for_interrupt(void) {
  NRF_P1->PIN_CNF[HDC2080_INT_PIN] =
      (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
      (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
      (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);

  NRF_GPIOTE->CONFIG[0] =
      (HDC2080_INT_PIN << GPIOTE_CONFIG_PSEL_Pos) |
      (1 << GPIOTE_CONFIG_PORT_Pos) |
      (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
      (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos);

  NRF_GPIOTE->EVENTS_IN[0] = 0; // Clear event
  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN0_Msk;
  NVIC_EnableIRQ(GPIOTE_IRQn); // Enable GPIOTE interrupt
}