#ifndef HDC2080_H
#define HDC2080_H

#include <stdint.h>

#define TEMP_LOW 0x00
#define TEMP_HIGH 0x01
#define HUMID_LOW 0x02
#define HUMID_HIGH 0x03
#define INTERRUPT_DRDY 0x04
#define INTERRUPT_CONFIG 0x07
#define MEASUREMENT_CONFIG 0x0F
#define INT_CONFIG 0x0E

// Constants for setting sensor mode
#define TEMP_AND_HUMID 0
#define TEMP_ONLY 1
#define HUMID_ONLY 2
#define ACTIVE_LOW 0
#define ACTIVE_HIGH 1
#define LEVEL_MODE 0
#define COMPARATOR_MODE 1

// Constants for setting sample rate
#define MANUAL 0
#define TWO_MINS 1
#define ONE_MINS 2
#define TEN_SECONDS 3
#define FIVE_SECONDS 4
#define ONE_HZ 5
#define TWO_HZ 6
#define FIVE_HZ 7

void hdc2080_trigger_measurement(void);
void hdc2080_reset();
void hdc2080_set_measurement_mode(int mode);
float hd2080_read_temp(void);
uint8_t hdc2080_read_interrupt_status(void);
float hdc2080_read_humidity(void);
void enable_interrupt(void);
void set_interrupt_polarity(int polarity);
void set_interrupt_mode(int mode);
void set_rate(int rate);
void hdc2080_trigger_measurement(void);
void enable_drdy_interrupt(void);

#endif