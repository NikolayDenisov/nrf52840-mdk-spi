#include "twi.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include <stdbool.h>

#define TWIM_INSTANCE NRF_TWIM1

void twi_wait_stop() {
  while (!TWIM_INSTANCE->EVENTS_STOPPED) {
  }
  TWIM_INSTANCE->EVENTS_STOPPED = 0;
}

void twi_tx(uint8_t *data, uint8_t len) {
  TWIM_INSTANCE->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;
  TWIM_INSTANCE->TXD.PTR = (uint32_t)data;
  TWIM_INSTANCE->TXD.MAXCNT = len;
  TWIM_INSTANCE->TASKS_STARTTX = 1;
  twi_wait_stop();
}

void twi_rx(uint8_t *data, uint8_t len) {
  TWIM_INSTANCE->RXD.PTR = (uint32_t)data;
  TWIM_INSTANCE->RXD.MAXCNT = len;
  TWIM_INSTANCE->SHORTS = TWIM_SHORTS_LASTRX_STOP_Msk;
  TWIM_INSTANCE->TASKS_STARTRX = 1;
  twi_wait_stop();
}

void twi_write_register(uint8_t reg, uint8_t val) {
  uint8_t buf[] = {reg, val};
  TWIM_INSTANCE->ADDRESS = HDC2080_ADDRESS;
  twi_tx(buf, sizeof(buf));
}

uint8_t twi_read_register(uint8_t reg) {
  uint8_t val;
  TWIM_INSTANCE->ADDRESS = HDC2080_ADDRESS;
  twi_tx(&reg, 1);
  twi_rx(&val, 1);
  return val;
}

void twi_init(void) {
  TWIM_INSTANCE->PSEL.SCL = TWI_SCL_PIN;
  TWIM_INSTANCE->PSEL.SDA = TWI_SDA_PIN;
  TWIM_INSTANCE->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K100;
  TWIM_INSTANCE->SHORTS = 0;
  TWIM_INSTANCE->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
}