#ifndef EPD_H
#define EPD_H
#include "config.h"
#include <stdint.h>

#define EPD_WIDTH 122
#define EPD_HEIGHT 250

#define DRIVER_OUTPUT_CONTROL 0x01

#define BOOSTER_SOFT_START_CONTROL 0x0C
#define WRITE_VCOM_REGISTER 0x2C
#define SET_DUMMY_LINE_PERIOD 0x3A
#define SET_GATE_TIME 0x3B
#define DATA_ENTRY_MODE_SETTING 0x11
#define WRITE_LUT_REGISTER 0x32

// Display resolution
#define EPD_2in13_V3_WIDTH 122
#define EPD_2in13_V3_HEIGHT 250

void epd_ready_busy(void);
void epd_send_command(uint8_t cmd);
void epd_send_data(uint8_t data);
void epd_reset(void);
void epd_init(void);
void epd_turn_on_display(void);
void epd_set_cursor(uint16_t x, uint16_t y);
void epd_set_windowds(uint16_t x_start, uint16_t y_start, uint16_t x_end,
                      uint16_t y_end);
void epd_set_lut(const unsigned char *lut);
void epd_clear(void);
void epd_lut_by_host(uint8_t *lut);

void epd_lut(uint8_t *lut);
void epd_display(UBYTE *Image);
void epd_display_base(UBYTE *Image);
void epd_turn_on_display_partial(void);
void epd_display_partial(UBYTE *Image);

#endif