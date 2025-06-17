
#include "epd.h"
#include "config.h"
#include "gpio.h"
#include "nrf52840.h"
#include "nrf_delay.h"
#include "spi.h"
#include <string.h>

void epd_ready_busy(void) {
  while (gpio_pin_read(EPD_PIN_BUSY)) {
    // Wait until the busy pin is low
    nrf_delay_ms(10);
  }
}

void epd_send_command(uint8_t cmd) {
  gpio_pin_clear(EPD_PIN_DC);
  gpio_pin_clear(SPI_PIN_CS);
  spi_send(cmd);
  gpio_pin_set(SPI_PIN_CS);
}

void epd_send_data(uint8_t data) {
  gpio_pin_set(EPD_PIN_DC);
  gpio_pin_clear(SPI_PIN_CS);
  spi_send(data);
  gpio_pin_set(SPI_PIN_CS);
}

void epd_reset(void) {
  gpio_pin_set(EPD_PIN_RST);
  nrf_delay_ms(200);
  gpio_pin_clear(EPD_PIN_RST);
  nrf_delay_ms(5);
  gpio_pin_set(EPD_PIN_RST);
  nrf_delay_ms(200);
}

void epd_set_lut(const unsigned char *lut) {
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

void epd_lut(uint8_t *lut) {
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
  epd_ready_busy();
}

void epd_lut_by_host(uint8_t *lut) {
  epd_lut((uint8_t *)lut); // lut
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
  epd_reset();
  epd_ready_busy();
  epd_send_command(0x12); // SWRESET
  epd_ready_busy();
  epd_send_command(0x01); // DRIVER_OUTPUT_CONTROL
  epd_send_data(0xF9);
  epd_send_data(0x00);
  epd_send_data(0x00);

  epd_send_command(0x11); // DATA_ENTRY_MODE_SETTING
  epd_send_data(0x03);    // X increment, Y increment

  epd_set_windowds(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
  epd_set_cursor(0, 0);

  epd_send_command(0x3C); // BorderWavefrom
  epd_send_data(0x05);

  epd_send_command(0x21); // Display Update Control
  epd_send_data(0x00);    // Full update
  epd_send_data(0x80);    // Full update

  epd_send_command(0x18); // Read built-in temperature sensor
  epd_send_data(0x80);    // Disable temperature sensor
  epd_ready_busy();

  epd_lut_by_host(WS_20_30_2IN13_V3);
}

void epd_set_windowds(uint16_t x_start, uint16_t y_start, uint16_t x_end,
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

void epd_set_cursor(uint16_t x, uint16_t y) {
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

void epd_clear(void) {
  int w = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
  int h = EPD_HEIGHT;
  int size = w * h;

  memset(buffer, 0xFF, size);

  epd_send_command(0x24);

  gpio_pin_set(EPD_PIN_DC);
  gpio_pin_clear(SPI_PIN_CS);
  spi_send_buffer(buffer, size);
  gpio_pin_set(SPI_PIN_CS);

  epd_send_command(0x22);
  epd_send_data(0xC7);
  epd_send_command(0x20);
  epd_ready_busy();
}

void epd_turn_on_display(void) {
  epd_send_command(0x12); // DISPLAY_REFRESH
  epd_send_data(0xc7);
  epd_send_command(0x20); // Activate Display Update Sequence
  epd_ready_busy();
}

void epd_display(UBYTE *Image) {
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

void epd_display_base(UBYTE *Image) {
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

void epd_turn_on_display_partial(void) {
  epd_send_command(0x22); // Display Update Control
  epd_send_data(0x0f);    // fast:0x0c, quality:0x0f, 0xcf
  epd_send_command(0x20); // Activate Display Update Sequence
  epd_ready_busy();
}

UBYTE WF_PARTIAL_2IN13_V3[159] = {
    0x0,  0x40, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x0,  0x80,
    0x80, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x40, 0x40,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x80, 0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x14, 0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x1,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x1, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,  0x0, 0x0,  0x0,  0x0,
    0x0,  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0, 0x0,  0x0, 0x22, 0x17, 0x41,
    0x00, 0x32, 0x36,
};

void epd_display_partial(UBYTE *Image) {
  UWORD Width, Height;
  Width = (EPD_2in13_V3_WIDTH % 8 == 0) ? (EPD_2in13_V3_WIDTH / 8)
                                        : (EPD_2in13_V3_WIDTH / 8 + 1);
  Height = EPD_2in13_V3_HEIGHT;

  // Reset
  gpio_pin_clear(EPD_PIN_RST);
  nrf_delay_ms(1);
  gpio_pin_set(EPD_PIN_RST);

  epd_lut_by_host(WF_PARTIAL_2IN13_V3);

  epd_send_command(0x37);
  epd_send_data(0x00);
  epd_send_data(0x00);
  epd_send_data(0x00);
  epd_send_data(0x00);
  epd_send_data(0x00);
  epd_send_data(0x40); /// RAM Ping-Pong enable
  epd_send_data(0x00);
  epd_send_data(0x00);
  epd_send_data(0x00);
  epd_send_data(0x00);

  epd_send_command(0x3C); // BorderWavefrom
  epd_send_data(0x80);

  epd_send_command(0x22); // Display Update Sequence Option
  epd_send_data(0xC0);    // Enable clock and  Enable analog
  epd_send_command(0x20); // Activate Display Update Sequence
  epd_ready_busy();

  epd_set_windowds(0, 0, EPD_2in13_V3_WIDTH - 1, EPD_2in13_V3_HEIGHT - 1);
  epd_set_cursor(0, 0);

  epd_send_command(0x24); // Write Black and White image to RAM
  for (UWORD j = 0; j < Height; j++) {
    for (UWORD i = 0; i < Width; i++) {
      epd_send_data(Image[i + j * Width]);
    }
  }
  epd_turn_on_display_partial();
}