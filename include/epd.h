#ifndef EPD_H
#define EPD_H
#include <stdint.h>

#define EPD_WIDTH 122
#define EPD_HEIGHT 250

void ready_busy(void);
void epd_send_command(uint8_t cmd);
void epd_send_data(uint8_t data);
void reset(void);
void epd_init(void);
void turn_on_display(void);
void set_cursor(uint16_t x, uint16_t y);
void set_windowds(uint16_t x_start, uint16_t y_start, uint16_t x_end,
                  uint16_t y_end);
void set_lut(const unsigned char *lut);
void clear(void);

#endif