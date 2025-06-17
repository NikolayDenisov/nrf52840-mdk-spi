#include "spi.h"
#include "gpio.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include <stddef.h>
#include <stdint.h>

void spi_init() {
  NRF_SPIM0->PSEL.SCK = SPI_PIN_SCK;
  NRF_SPIM0->PSEL.MOSI = SPI_PIN_MOSI;

  NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M4
                         << SPIM_FREQUENCY_FREQUENCY_Pos;

  NRF_SPIM0->CONFIG = (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos) |
                      (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos) |
                      (SPIM_CONFIG_CPHA_Leading << SPIM_CONFIG_CPHA_Pos);

  NRF_SPIM0->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);
  gpio_pin_set(SPI_PIN_CS);
}

void spi_send(uint8_t data) {
  NRF_SPIM0->TXD.PTR = (uint32_t)&data;
  NRF_SPIM0->TXD.MAXCNT = 1;

  NRF_SPIM0->TASKS_START = 1;
  while (!NRF_SPIM0->EVENTS_END) {
  }
  NRF_SPIM0->EVENTS_END = 0;
}

void spi_send_buffer(uint8_t *buffer, size_t length) {
  NRF_SPIM0->TXD.PTR = (uint32_t)buffer;
  NRF_SPIM0->TXD.MAXCNT = length;
  NRF_SPIM0->TASKS_START = 1;
  while (!NRF_SPIM0->EVENTS_END) {
  }
  NRF_SPIM0->EVENTS_END = 0;
}
