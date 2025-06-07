
#include "epd.h"
#include "config.h"
#include "gpio.h"
#include "nrf52840.h"
#include "nrf_delay.h"
#include "spi.h"
#include <string.h>

void ready_busy(void) {
  while (gpio_read(PIN_BUSY)) {
    // Wait until the busy pin is low
    nrf_delay_ms(10);
  }
}

void epd_send_command(uint8_t cmd) {
  gpio_clear(PIN_DC);
  gpio_clear(PIN_CS);
  spi_send(cmd);
  gpio_set(PIN_CS);
}

void epd_send_data(uint8_t data) {
  gpio_set(PIN_DC);
  gpio_clear(PIN_CS);
  spi_send(data);
  gpio_set(PIN_CS);
}

void reset(void) {
  gpio_set(PIN_RST);
  nrf_delay_ms(200);
  gpio_clear(PIN_RST);
  nrf_delay_ms(5);
  gpio_set(PIN_RST);
  nrf_delay_ms(200);
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

uint8_t WS_20_30_2IN13_V3[159] = {
    0x80, 0x4A, 0x40, 0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,  0x0,  0x40,
    0x4A, 0x80, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,  0x80, 0x4A,
    0x40, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x40, 0x4A, 0x80,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0xF, 0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0xF,  0x0,  0x0,  0xF,  0x0,  0x0, 0x2, 0xF, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x1,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,  0x0,  0x0,
    0x0,  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0, 0x0, 0x0, 0x22, 0x17, 0x41,
    0x0,  0x32, 0x36};

void EPD_2IN13_V3_LUT(uint8_t *lut) {
  uint8_t count;
  epd_send_command(0x32);
  for (count = 0; count < 153; count++) {
    epd_send_data(lut[count]);
  }

  // epd_send_command(0x3f);
  // epd_send_data(*(lut + 153));
  // epd_send_command(0x03); // gate voltage
  // epd_send_data(*(lut + 154));
  // epd_send_command(0x04);      // source voltage
  // epd_send_data(*(lut + 155)); // VSH
  // epd_send_data(*(lut + 156)); // VSH2
  // epd_send_data(*(lut + 157)); // VSL
  // epd_send_command(0x2c);      // VCOM
  // epd_send_data(*(lut + 158));
}

void EPD_2in13_V3_TurnOnDisplay(void) {
  epd_send_command(0x22); // Display Update Control
  epd_send_data(0xc7);
  epd_send_command(0x20); // Activate Display Update Sequence
  ready_busy();
}

void EPD_2IN13_V2_LUT_by_host(uint8_t *lut) {
  EPD_2IN13_V3_LUT((uint8_t *)lut); // lut
  epd_send_command(0x3f);
  epd_send_data(*(lut + 153));
  epd_send_command(0x03); // gate voltage
  epd_send_data(*(lut + 154));
  epd_send_command(0x04);      // source voltage
  epd_send_data(*(lut + 155)); // VSH
  epd_send_data(*(lut + 156)); // VSH2
  epd_send_data(*(lut + 157)); // VSL
  epd_send_command(0x2c);      // VCOM
  epd_send_data(*(lut + 158));
}

void epd_init(void) {
  reset();
  ready_busy();
  epd_send_command(0x12); // SWRESET
  ready_busy();
  epd_send_command(0x01); // DRIVER_OUTPUT_CONTROL
  epd_send_data(0xF9);
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
  epd_send_data(0x80);    // Disable temperature sensor
  ready_busy();

  EPD_2IN13_V2_LUT_by_host(WS_20_30_2IN13_V3);
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
  epd_send_data(x & 0xFF);

  epd_send_command(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
  epd_send_data(y & 0xFF);
  epd_send_data((y >> 8) & 0xFF);
}

#define EPD_2in13_V3_WIDTH 122
#define EPD_2in13_V3_HEIGHT 250

#define MAX_WIDTH_BYTES ((EPD_WIDTH + 7) / 8)
#define MAX_HEIGHT (EPD_HEIGHT)
#define MAX_BUFFER_SIZE (MAX_WIDTH_BYTES * MAX_HEIGHT)

static uint8_t buffer[MAX_BUFFER_SIZE];

void EPD_2in13_V3_Clear(void) {
  int w = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
  int h = EPD_HEIGHT;
  int size = w * h;

  memset(buffer, 0xFF, size);

  epd_send_command(0x24);

  gpio_set(PIN_DC);
  gpio_clear(PIN_CS);
  spi_send_buffer(buffer, size);
  gpio_set(PIN_CS);

  epd_send_command(0x22);
  epd_send_data(0xC7);
  epd_send_command(0x20);
  ready_busy();
}

void turn_on_display(void) {
  epd_send_command(0x12); // DISPLAY_REFRESH
  epd_send_data(0xc7);
  epd_send_command(0x20); // Activate Display Update Sequence
  ready_busy();
}

void EPD_2in13_V3_Display(UBYTE *Image) {
  UWORD Width, Height;
  Width = (EPD_2in13_V3_WIDTH % 8 == 0) ? (EPD_2in13_V3_WIDTH / 8)
                                        : (EPD_2in13_V3_WIDTH / 8 + 1);
  Height = EPD_2in13_V3_HEIGHT;

  epd_send_command(0x24);
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      epd_send_data(Image[i + j * Width]);
    }
  }

  EPD_2in13_V3_TurnOnDisplay();
}

void EPD_2in13_V3_Display_Base(UBYTE *Image) {
  UWORD Width, Height;
  Width = (EPD_2in13_V3_WIDTH % 8 == 0) ? (EPD_2in13_V3_WIDTH / 8)
                                        : (EPD_2in13_V3_WIDTH / 8 + 1);
  Height = EPD_2in13_V3_HEIGHT;

  epd_send_command(0x24); // Write Black and White image to RAM
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      epd_send_data(Image[i + j * Width]);
    }
  }
  epd_send_command(0x26); // Write Black and White image to RAM
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      epd_send_data(Image[i + j * Width]);
    }
  }
  EPD_2in13_V3_TurnOnDisplay();
}