#include "nrf52840.h"
#include "nrf_delay.h"
#include <stdint.h>

// Пины дисплея
#define PIN_CS 15
#define PIN_DC 16
#define PIN_RST 17
#define PIN_BUSY 21
#define PIN_MOSI 13
#define PIN_SCK 14

void gpio_init() {
  NRF_P0->DIRSET = (1 << PIN_CS) | (1 << PIN_DC) | (1 << PIN_RST) |
                   (1 << PIN_SCK) | (1 << PIN_MOSI);
  NRF_P0->DIRCLR = (1 << PIN_BUSY); // BUSY - вход
}

void spi_init() {
  NRF_SPIM0->PSEL.MOSI = PIN_MOSI;
  NRF_SPIM0->PSEL.SCK = PIN_SCK;
  NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M4;
  NRF_SPIM0->CONFIG = 0;
  NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;
}

void spi_write(uint8_t data) {
  NRF_SPIM0->TXD.PTR = (uint32_t)data;
  NRF_SPIM0->TASKS_START = 1;
  while (!NRF_SPIM0->EVENTS_END)
    ;
  NRF_SPIM0->EVENTS_END = 0;
  NRF_SPIM0->TASKS_STOP = 1;
}

void digital_write(uint32_t pin, uint8_t value) {
  if (value)
    NRF_P0->OUTSET = (1 << pin);
  else
    NRF_P0->OUTCLR = (1 << pin);
}

uint8_t digital_read(uint32_t pin) { return (NRF_P0->IN & (1 << pin)) ? 1 : 0; }

void send_command(uint8_t cmd) {
  digital_write(PIN_DC, 0); // Command mode
  digital_write(PIN_CS, 0);
  spi_write(cmd);
  digital_write(PIN_CS, 1);
}

void send_data(uint8_t data) {
  digital_write(PIN_DC, 1); // Data mode
  digital_write(PIN_CS, 0);
  spi_write(data);
  digital_write(PIN_CS, 1);
}

void reset_display() {
  digital_write(PIN_RST, 0);
  nrf_delay_ms(10);
  digital_write(PIN_RST, 1);
  nrf_delay_ms(10);
}

void wait_until_idle() {
  while (digital_read(PIN_BUSY)) {
    nrf_delay_ms(10);
  }
}

// ⚙️ Минимальная инициализация дисплея IL3897 (GDEH0213B72)
void epd_init() {
  reset_display();
  wait_until_idle();

  send_command(0x01); // POWER_SETTING
  send_data(0x03);
  send_data(0x00);
  send_data(0x2b);
  send_data(0x2b);
  send_data(0x03);

  send_command(0x06); // BOOSTER_SOFT_START
  send_data(0x17);
  send_data(0x17);
  send_data(0x17);

  send_command(0x04); // POWER_ON
  wait_until_idle();

  send_command(0x00); // PANEL_SETTING
  send_data(0xBF);    // 0xBF = 0b10111111

  send_command(0x30); // PLL_CONTROL
  send_data(0x3A);    // 3A = 100Hz

  send_command(0x61); // Resolution
  send_data(0x01);
  send_data(0x90); // 400px
  send_data(0x01);
  send_data(0x2C); // 300px

  send_command(0x82); // VCOM
  send_data(0x12);
}

void epd_clear() {
  uint32_t width = 400;
  uint32_t height = 300;
  send_command(0x10);
  for (uint32_t i = 0; i < (width * height / 8); i++) {
    send_data(0xFF); // White
  }
  send_command(0x13);
  for (uint32_t i = 0; i < (width * height / 8); i++) {
    send_data(0xFF); // White
  }
  send_command(0x12); // DISPLAY REFRESH
  wait_until_idle();
}

// ⚠️ Эта функция - заглушка! Нарисует простой прямоугольник
void epd_draw_dummy_number(uint8_t n) {
  send_command(0x13);
  for (uint32_t i = 0; i < (400 * 300 / 8); i++) {
    send_data((i / 1000) % 2 == n % 2 ? 0x00 : 0xFF);
  }
  send_command(0x12); // DISPLAY REFRESH
  wait_until_idle();
}

int main(void) {
  gpio_init();
  spi_init();

  uint8_t number = 0;

  while (1) {
    epd_init();
    epd_clear();
    epd_draw_dummy_number(number++);
    if (number > 9)
      number = 0;
    nrf_delay_ms(5000);
  }
}
