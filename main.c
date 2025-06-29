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

UBYTE *BlackImage;
uint8_t framebuffer[EPD_HEIGHT][WIDTH_BYTES];

static void epd_display_framebuffer(void) {
  epd_send_command(0x24); // WRITE_RAM
  for (uint16_t y = 0; y < EPD_HEIGHT; y++) {
    for (uint16_t x = 0; x < WIDTH_BYTES; x++) {
      epd_send_data(framebuffer[y][x]);
    }
  }
  epd_turn_on_display();
}

static void clock_initialization(void) {
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
    ;

  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    ;
}

static void epd_sleep(void) {
  epd_send_command(0x10); // Enter deep sleep
  epd_send_data(0x01);
  nrf_delay_ms(100);
}

static void draw_static_ui(void) {
  Paint_DrawString_EN(0, TEMP_Y, "Temp = ", &Font24, WHITE, BLACK);
  Paint_DrawCircle(205, TEMP_Y, 4, BLACK, 1, DRAW_FILL_EMPTY);
  Paint_DrawString_EN(210, TEMP_Y, "C", &Font24, WHITE, BLACK);

  Paint_DrawString_EN(0, HUMID_Y, "Humid = ", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(210, HUMID_Y, "%", &Font24, WHITE, BLACK);
}

static void update_sensor_values_on_display(float temperature, float humidity) {
  Paint_DrawRectangle(TEMP_X, TEMP_Y, TEMP_X + 80, TEMP_Y + 24, WHITE,
                      DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawRectangle(HUMID_X, HUMID_Y, HUMID_X + 80, HUMID_Y + 24, WHITE,
                      DOT_PIXEL_1X1, DRAW_FILL_FULL);

  Paint_DrawNumDecimals(TEMP_X, TEMP_Y, temperature, &Font24, 1, BLACK, WHITE);
  Paint_DrawNumDecimals(HUMID_X, HUMID_Y, humidity, &Font24, 1, BLACK, WHITE);
  epd_display_partial(BlackImage);
}

void GPIOTE_IRQHandler(void) {
  if (NRF_GPIOTE->EVENTS_IN[0]) {
    NRF_GPIOTE->EVENTS_IN[0] = 0;

    uint8_t status =
        hdc2080_read_interrupt_status(); // Чтение сбрасывает прерывание
    if ((status & 0x80) == 0) {
      return;
    }
    float temperature = hd2080_read_temp();
    float humidity = hdc2080_read_humidity();

    update_sensor_values_on_display(temperature, humidity);
  }
}

static void sensor_init(void) {
  hdc2080_reset();
  set_rate(ONE_HZ);
  enable_drdy_interrupt();
  enable_interrupt();
  set_interrupt_polarity(ACTIVE_HIGH);
  hdc2080_set_measurement_mode(TEMP_AND_HUMID);
}

static int setup_display_buffer(void) {
  UWORD ImageSize = ((EPD_2in13_V3_WIDTH + 7) / 8) * EPD_2in13_V3_HEIGHT;
  BlackImage = (UBYTE *)malloc(ImageSize);
  if (!BlackImage) {
    return -1;
  }

  Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90,
                 WHITE);
  Paint_Clear(WHITE);
  Paint_SetRotate(ROTATE_270);

  return 0;
}

int main(void) {
  // clock_initialization(); // Включите при необходимости
  twi_init();
  gpio_init();
  sensor_init();
  gpio_config_for_interrupt();
  spi_init();
  epd_init();
  epd_clear();

  if (setup_display_buffer() < 0) {
    return -1;
  }

  draw_static_ui();
  epd_display(BlackImage);

  nrf_delay_ms(1000);
  hdc2080_trigger_measurement();

  while (true) {
    __WFE(); // Ожидание события
    __SEV(); // Установка события
    __WFE(); // Повторное ожидание (паттерн WFE-SEV-WFE для пробуждения)
  }

  return 0;
}
