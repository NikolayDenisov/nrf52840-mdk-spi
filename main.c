#include "nrf52840.h"
#include "nrf_delay.h"
#include <stdint.h>

#define PIN_MOSI 21
#define PIN_SCK 17
#define PIN_CS 15
#define PIN_DC 16
#define PIN_RST 14
#define PIN_BUSY 13

void gpio_init() {
  NRF_P0->DIRSET = (1 << PIN_RST) | (1 << PIN_DC) | (1 << PIN_CS);
  NRF_P0->DIRCLR = (1 << PIN_BUSY);
  NRF_P0->PIN_CNF[PIN_BUSY] =
      (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
}
static void gpio_set(uint32_t pin) { NRF_P0->OUTSET = (1 << pin); }

static void gpio_clear(uint32_t pin) { NRF_P0->OUTCLR = (1 << pin); }

static uint8_t gpio_read(uint32_t pin) {
  return (NRF_P0->IN & (1 << pin)) ? 1 : 0;
}

static void spi_init() {
  NRF_SPIM0->PSEL.SCK = PIN_SCK;
  NRF_SPIM0->PSEL.MOSI = PIN_MOSI;
  NRF_SPIM0->PSEL.MISO = 0xFFFFFFFF;
  NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M8;
  NRF_SPIM0->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos);
  NRF_SPIM0->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);
}

static void spi_tx(const uint8_t data) {
  NRF_SPIM0->TXD.PTR = (uint32_t)&data;
  NRF_SPIM0->TXD.MAXCNT = 1;
  NRF_SPIM0->TASKS_START = 1;
  while (!NRF_SPIM0->EVENTS_END) {
  }
  NRF_SPIM0->EVENTS_END = 0;
}

static void epd_send_cmd(uint8_t cmd) {
  gpio_clear(PIN_DC);
  gpio_clear(PIN_CS);
  spi_tx(cmd);
  gpio_set(PIN_CS);
}

static void epd_send_data(uint8_t data) {
  gpio_set(PIN_DC);
  gpio_clear(PIN_CS);
  spi_tx(data);
  gpio_set(PIN_CS);
}

static void epd_wait_busy() {
  while (gpio_read(PIN_BUSY))
    nrf_delay_ms(10);
}

static void epd_reset() {
  gpio_clear(PIN_RST);
  nrf_delay_ms(200);
  gpio_set(PIN_RST);
  nrf_delay_ms(200);
}

static void epd_init() {
  epd_reset();

  epd_send_cmd(0x01); // POWER SETTING
  epd_send_data(0x07);
  epd_send_data(0x00);
  epd_send_data(0x08);
  epd_send_data(0x00);
}

static void epd_display_frame(const uint8_t *black, const uint8_t *red,
                              size_t len) {
  epd_send_cmd(0x10); // Black
  for (size_t i = 0; i < len; ++i)
    epd_send_data(black[i]);

  epd_send_cmd(0x13); // Red
  for (size_t i = 0; i < len; ++i)
    epd_send_data(red[i]);

  epd_send_cmd(0x12); // Display refresh
  epd_wait_busy();
}

void epd_display_digit_1() {
  epd_send_cmd(0x10); // Начало буферизации изображения
  for (int i = 0; i < 100; i++) {
    epd_send_data(0xFF); // (упрощенно, передача пикселей, представляющих "1")
  }
  epd_send_cmd(0x12); // Обновление изображения
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

int main(void) {
  clock_initialization();
  gpio_init();
  spi_init();
  epd_init();

  epd_display_digit_1();
  while (1) {
  }
}
