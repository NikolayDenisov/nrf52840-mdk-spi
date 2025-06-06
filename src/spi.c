#include "spi.h"
#include "gpio.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include <stdint.h>

void spi_init() {
  NRF_SPIM0->PSEL.SCK = PIN_SCK;
  NRF_SPIM0->PSEL.MOSI = PIN_MOSI;
  NRF_SPIM0->PSEL.CSN = PIN_CS;
  NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M1; // 1 Мбит/с
  NRF_SPIM0->CONFIG = 0x00;
  NRF_SPIM0->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);
}

void spi_tx(uint8_t data) {
  NRF_SPIM0->TXD.PTR = (uint32_t)&data;
  NRF_SPIM0->TXD.MAXCNT = 1;

  // NRF_SPIM0->EVENTS_END = 0;
  NRF_SPIM0->TASKS_START = 1;
  while (!NRF_SPIM0->EVENTS_END) {
  }
  NRF_SPIM0->EVENTS_END = 0;
}
