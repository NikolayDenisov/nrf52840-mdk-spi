#include "GUI_Paint.h"
#include "epd.h"
#include "gpio.h"
#include "nrf52840.h"
#include "nrf_delay.h"
#include "spi.h"
#include <stdint.h>
#include <stdlib.h>

#define DRIVER_OUTPUT_CONTROL 0x01

#define BOOSTER_SOFT_START_CONTROL 0x0C
#define WRITE_VCOM_REGISTER 0x2C
#define SET_DUMMY_LINE_PERIOD 0x3A
#define SET_GATE_TIME 0x3B
#define DATA_ENTRY_MODE_SETTING 0x11
#define WRITE_LUT_REGISTER 0x32

const uint8_t font5x7[][5] = {
    // только A-Z, пробел, H, E, L, O, W, R, D — минимум для "HELLO WORLD"
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00},
    ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F},
    ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41},
    ['L'] = {0x7F, 0x40, 0x40, 0x40, 0x40},
    ['O'] = {0x3E, 0x41, 0x41, 0x41, 0x3E},
    ['W'] = {0x7F, 0x02, 0x0C, 0x02, 0x7F},
    ['R'] = {0x7F, 0x09, 0x19, 0x29, 0x46},
    ['D'] = {0x7F, 0x41, 0x41, 0x22, 0x1C},
};
#define WIDTH_BYTES (EPD_WIDTH / 8)
uint8_t framebuffer[EPD_HEIGHT][WIDTH_BYTES];

void draw_char(uint8_t x, uint8_t y, char c) {
  if (c < 32 || c > 127)
    c = ' ';
  for (uint8_t col = 0; col < 5; col++) {
    uint8_t line = font5x7[(uint8_t)c][col];
    for (uint8_t row = 0; row < 7; row++) {
      if (line & (1 << row)) {
        uint16_t byte_index = x / 8;
        framebuffer[y + row][byte_index] &= ~(1 << (7 - (x % 8)));
      }
    }
    x++;
  }
}

void draw_text(uint8_t x, uint8_t y, const char *str) {
  while (*str) {
    draw_char(x, y, *str++);
    x += 6; // 5px + 1px пробел
  }
}
void epd_display_framebuffer(void) {
  epd_send_command(0x24); // WRITE_RAM
  for (uint16_t y = 0; y < EPD_HEIGHT; y++) {
    for (uint16_t x = 0; x < WIDTH_BYTES; x++) {
      epd_send_data(framebuffer[y][x]);
    }
  }
  turn_on_display();
}
void clock_initialization(void) {
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
  }

  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
  }
}

void EPD_2in13_V3_Sleep(void) {
  epd_send_command(0x10); // enter deep sleep
  epd_send_data(0x01);
  nrf_delay_ms(100);
}

int main(void) {
  clock_initialization();
  gpio_init();
  spi_init();
  epd_init();
  EPD_2in13_V3_Clear();

  UBYTE *BlackImage;
  UWORD Imagesize =
      ((EPD_2in13_V3_WIDTH % 8 == 0) ? (EPD_2in13_V3_WIDTH / 8)
                                     : (EPD_2in13_V3_WIDTH / 8 + 1)) *
      EPD_2in13_V3_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    // Debug("Failed to apply for black memory...\r\n");
    return -1;
  }

  Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90,
                 WHITE);
  Paint_Clear(WHITE);

  Paint_SetRotate(ROTATE_270);

  Paint_DrawString_EN(0, 15, "Temp = ", &Font24, WHITE, BLACK);
  Paint_DrawNumDecimals(120, 15, 25.2, &Font24, 1, BLACK, WHITE);
  Paint_DrawCircle(200, 15, 4, BLACK, 1, DRAW_FILL_EMPTY);
  Paint_DrawString_EN(205, 15, "C", &Font24, WHITE, BLACK);

  EPD_2in13_V3_Display(BlackImage);

  nrf_delay_ms(5000);
  epd_init();
  EPD_2in13_V3_Clear();
  EPD_2in13_V3_Sleep();
}
