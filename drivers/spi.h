
#ifndef __spi_h__
#define __spi_h__

#define PIN_SO  17
#define PIN_SI  18
#define PIN_SCK 15

#define SPIF (1 << 7)

extern void spi_init(void);
extern uint8_t spi_transfer(uint8_t);

#endif