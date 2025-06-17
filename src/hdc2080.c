#include "hdc2080.h"
#include "nrf_delay.h"
#include "twi.h"

void hdc2080_trigger_measurement(void) {
  uint8_t config_content = twi_read_register(MEASUREMENT_CONFIG);
  config_content |= 0x01;
  twi_write_register(MEASUREMENT_CONFIG, config_content);
}

void hdc2080_reset() {
  twi_write_register(INT_CONFIG, 0x80);
  nrf_delay_ms(50);
}

void hdc2080_set_measurement_mode() {
  twi_write_register(MEASUREMENT_CONFIG, 0xF9);
  nrf_delay_ms(130);
}

float hd2080_read_temp(void) {
  uint8_t byte[2];
  uint16_t temp;

  byte[0] = twi_read_register(TEMP_LOW);
  byte[1] = twi_read_register(TEMP_HIGH);

  temp = ((uint16_t)byte[1] << 8) | byte[0];

  return ((float)temp) * 165.0f / 65536.0f - 40.5f;
}

float hdc2080_read_humidity(void) {
  uint8_t byte[2];
  uint16_t humidity;
  byte[0] = twi_read_register(HUMID_LOW);
  byte[1] = twi_read_register(HUMID_HIGH);
  humidity = (unsigned int)byte[1] << 8 | byte[0];
  return (float)(humidity) / 65536 * 100;
}

uint8_t hdc2080_read_interrupt_status(void) {
  uint8_t reg_contents;
  reg_contents = twi_read_register(INTERRUPT_DRDY);
  return reg_contents;
}