#ifndef HDC2080_H
#define HDC2080_H

#include <stdint.h>

#define TEMP_LOW 0x00
#define TEMP_HIGH 0x01
#define HUMID_LOW 0x02
#define HUMID_HIGH 0x03
#define INTERRUPT_DRDY 0x04
#define MEASUREMENT_CONFIG 0x0F
#define INT_CONFIG 0x0E

void hdc2080_trigger_measurement(void);
void hdc2080_reset();
void hdc2080_set_measurement_mode();
float hd2080_read_temp(void);
uint8_t hdc2080_read_interrupt_status(void);
float hdc2080_read_humidity(void);

#endif