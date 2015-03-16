
#include "LPC17xx.h"
#include "spi.h"
#include <stdint.h>

void spi_init(void)
{
	LPC_PINCON->PINSEL0 |= 0xC0000000;		// enable SPI pins
	LPC_PINCON->PINSEL1 |= 0x0000003C;

	LPC_SC->PCLKSEL0 |= 0x30000;
	LPC_SPI->SPCR     = 0x20;			// set master mode
	LPC_SPI->SPCCR    = 0x08;			// SCK =pclk/8

	LPC_GPIO0->FIODIR |= (1 << PIN_SI); // MOSI
	LPC_GPIO0->FIODIR |= (1 << PIN_SCK); // SCK
	LPC_GPIO0->FIODIR &= ~(1 << PIN_SO); // SCK

	// LPC_GPIO0->FIOCLR |= (1 << PIN_SCK);	
}

uint8_t spi_transfer(uint8_t data)
{
	LPC_SPI->SPDR = data;
	while(!((LPC_SPI->SPSR) & SPIF) )
	{
		;
	}
	return LPC_SPI->SPDR;
}