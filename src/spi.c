#include "spi.h"
#include "gpio.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include <stdint.h>

void spi_init() {
  gpio_set(PIN_SCK);
  NRF_P0->DIRSET = (1 << PIN_SCK); // Set SCK as output

  gpio_clear(PIN_MOSI);
  NRF_P0->DIRSET = (1 << PIN_MOSI); // Set MOSI as output

  gpio_set(PIN_CS);
  NRF_P0->DIRSET = (1 << PIN_CS); // Set CS as output
  // NRF_P0->OUTSET = (1 << PIN_CS);   // Set CS high

  NRF_SPIM0->PSEL.SCK = PIN_SCK;
  NRF_SPIM0->PSEL.MOSI = PIN_MOSI;

  NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M4
                         << SPIM_FREQUENCY_FREQUENCY_Pos;

  NRF_SPIM0->CONFIG = (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos) |
                      (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos) |
                      (SPIM_CONFIG_CPHA_Leading << SPIM_CONFIG_CPHA_Pos);

  NRF_SPIM0->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);
  gpio_set(PIN_CS);
}

void spi_send(uint8_t data) {
  NRF_SPIM0->TXD.PTR = (uint32_t)&data;
  NRF_SPIM0->TXD.MAXCNT = 1;

  NRF_SPIM0->TASKS_START = 1;
  while (!NRF_SPIM0->EVENTS_END) {
  }
  NRF_SPIM0->EVENTS_END = 0;
}
