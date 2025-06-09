#include "GUI_Paint.h"
#include "epd.h"
#include "gpio.h"
#include "hdc2080.h"
#include "nrf52840.h"
#include "nrf_delay.h"
#include "spi.h"
#include "twi.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH_BYTES (EPD_WIDTH / 8)
#define TEMP_X 120
#define TEMP_Y 15

#define HUMID_X 120
#define HUMID_Y 45

uint8_t framebuffer[EPD_HEIGHT][WIDTH_BYTES];

void epd_display_framebuffer(void) {
  epd_send_command(0x24); // WRITE_RAM
  for (uint16_t y = 0; y < EPD_HEIGHT; y++) {
    for (uint16_t x = 0; x < WIDTH_BYTES; x++) {
      epd_send_data(framebuffer[y][x]);
    }
  }
  epd_turn_on_display();
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
  twi_init();
  gpio_init();
  spi_init();
  epd_init();
  epd_clear();

  UBYTE *BlackImage;
  UWORD Imagesize =
      ((EPD_2in13_V3_WIDTH % 8 == 0) ? (EPD_2in13_V3_WIDTH / 8)
                                     : (EPD_2in13_V3_WIDTH / 8 + 1)) *
      EPD_2in13_V3_HEIGHT;
  BlackImage = (UBYTE *)malloc(Imagesize);
  if (BlackImage == NULL) {
    return -1;
  }

  Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90,
                 WHITE);
  Paint_Clear(WHITE);
  Paint_SetRotate(ROTATE_270);

  // Настроим датчик температуры
  hdc2080_reset();
  hdc2080_set_measurement_mode();

  // Статическая часть экрана
  Paint_DrawString_EN(0, TEMP_Y, "Temp = ", &Font24, WHITE, BLACK);
  Paint_DrawCircle(205, TEMP_Y, 4, BLACK, 1, DRAW_FILL_EMPTY);
  Paint_DrawString_EN(210, TEMP_Y, "C", &Font24, WHITE, BLACK);

  Paint_DrawString_EN(0, HUMID_Y, "Humid = ", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(210, HUMID_Y, "%", &Font24, WHITE, BLACK);

  epd_display(BlackImage); // Отрисовка фона и текста
  nrf_delay_ms(1000);

  while (1) {
    hdc2080_trigger_measurement();

    // Ожидаем готовность данных
    uint8_t status;
    do {
      status = hdc2080_read_interrupt_status();
      nrf_delay_ms(100);
    } while ((status & 0x80) == 0);

    float temperature = hd2080_read_temp();
    float humidity = hdc2080_read_humidity();

    // Очистим старое значение температуры
    Paint_DrawRectangle(TEMP_X, TEMP_Y, TEMP_X + 80, TEMP_Y + 24, WHITE,
                        DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawRectangle(HUMID_X, HUMID_Y, HUMID_X + 80, HUMID_Y + 24, WHITE,
                        DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Нарисуем новое значение
    Paint_DrawNumDecimals(TEMP_X, TEMP_Y, temperature, &Font24, 1, BLACK,
                          WHITE);

    Paint_DrawNumDecimals(HUMID_X, HUMID_Y, humidity, &Font24, 1, BLACK, WHITE);

    // Частичное обновление экрана
    epd_display_partial(BlackImage);

    nrf_delay_ms(1000);
  }

  // В реальности до сюда не дойдёт, но можно вызвать sleep
  EPD_2in13_V3_Sleep();
  return 0;
}
