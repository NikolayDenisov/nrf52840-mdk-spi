
#include "epd.h"
#include "gpio.h"
#include "nrf52840.h"
#include "nrf_delay.h"
#include "spi.h"

void ready_busy(void) {
  while (gpio_read(PIN_BUSY)) {
    // Wait until the busy pin is low
    nrf_delay_ms(10);
  }
}

void epd_send_command(uint8_t cmd) {
  gpio_clear(PIN_DC);
  gpio_clear(PIN_CS);
  spi_tx(cmd);
  gpio_set(PIN_CS);
}

void epd_send_data(uint8_t data) {
  gpio_set(PIN_DC);
  gpio_clear(PIN_CS);
  spi_tx(data);
  gpio_set(PIN_CS);
}

void reset(void) {
  gpio_set(PIN_RST);
  nrf_delay_ms(20);
  gpio_clear(PIN_RST);
  nrf_delay_ms(2);
  gpio_set(PIN_RST);
  nrf_delay_ms(20);
}

void set_lut(const unsigned char *lut) {
  epd_send_command(0x32); // WRITE_LUT_REGISTER
  for (uint8_t i = 0; i < 30; i++) {
    epd_send_data(lut[i]);
  }
}

const unsigned char lut_full_update[] = {
    0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E,
    0x1E, 0x1E, 0x1E, 0x1E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

void epd_init(void) {
  reset();
  nrf_delay_ms(100);
  ready_busy();
  epd_send_command(0x12); // SWRESET
  ready_busy();
  epd_send_command(0x01); // DRIVER_OUTPUT_CONTROL
  epd_send_data(0xf9);
  epd_send_data(0x00);
  epd_send_data(0x00);

  epd_send_command(0x11); // DATA_ENTRY_MODE_SETTING
  epd_send_data(0x03);    // X increment, Y increment
  set_windowds(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
  set_cursor(0, 0);
  epd_send_command(0x3C); // BorderWavefrom
  epd_send_data(0x05);
  epd_send_command(0x21); // Display Update Control
  epd_send_data(0x00);    // Full update
  epd_send_data(0x80);    // Full update
  epd_send_command(0x18); // Read built-in temperature sensor

  epd_send_data(0x80); // Disable temperature sensor
  ready_busy();

  set_lut(lut_full_update);
}

void set_windowds(uint16_t x_start, uint16_t y_start, uint16_t x_end,
                  uint16_t y_end) {
  epd_send_command(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
  epd_send_data((x_start >> 3) & 0xFF);
  epd_send_data((x_end >> 3) & 0xFF);

  epd_send_command(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
  epd_send_data(y_start & 0xFF);
  epd_send_data((y_start >> 8) & 0xFF);
  epd_send_data(y_end & 0xFF);
  epd_send_data((y_end >> 8) & 0xFF);
}

void set_cursor(uint16_t x, uint16_t y) {
  epd_send_command(0x4E); // SET_RAM_X_ADDRESS_COUNTER
  epd_send_data(x >> 3);
  epd_send_command(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
  epd_send_data(y & 0xFF);
  epd_send_data((y >> 8) & 0xFF);
}

void clear(void) {
  uint16_t width, height;
  width = (EPD_HEIGHT % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
  height = EPD_HEIGHT;

  epd_send_command(0x24); // WRITE_RAM
  for (uint16_t i = 0; i < height; i++) {
    for (uint16_t j = 0; j < width; j++) {
      epd_send_data(0xFF); // Clear with white color
    }
  }
  turn_on_display();
}

void turn_on_display(void) {
  epd_send_command(0x12); // DISPLAY_REFRESH
  epd_send_data(0xc7);
  epd_send_command(0x20); // Activate Display Update Sequence
  ready_busy();
}