#ifndef SPI_H
#define SPI_H
#include <stddef.h>
#include <stdint.h>

void spi_send(uint8_t data);
void spi_init();
void spi_send_buffer(uint8_t *buffer, size_t length);

#endif