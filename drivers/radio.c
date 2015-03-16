
#include <stdint.h>
#include "radio.h"
#include "spi.h"
#include "LPC17xx.h"
#include "cc2500_reg.h"
#include <string.h>
#include "delay.h"

uint8_t radio_tx_buff[RADIO_TX_BUFFER_BYTES];
uint8_t radio_rx_buff[RADIO_RX_BUFFER_BYTES];

void radio_init(void)
{
	// CS Pin
	LPC_GPIO0->FIODIR |= (1 << CS_PIN_NO);
	LPC_GPIO0->FIOSET |= (1 << CS_PIN_NO);

	radio_reset();
	radio_update_settings();
}

uint8_t radio_reset(void)
{
	uint8_t result;
	HIGH_CS;
	LOW_CS;
	HIGH_CS;
	// 41 us delay
	_delay_us(500);
	LOW_CS;
	while(CHIP_NOT_READY);
	// Write reset strobe
	result = radio_write_strobe(TI_CCxxx0_SRES);
	while(CHIP_NOT_READY);
	HIGH_CS;
	return result;
}

uint8_t radio_write_strobe(uint8_t _strobe)
{
	uint8_t result;
	LOW_CS;
	while(CHIP_NOT_READY);	
	result = spi_transfer(_strobe);
	HIGH_CS;
	return result;
}

uint8_t radio_write_single(	uint8_t addr, 
							uint8_t dat){
	uint8_t result;
	LOW_CS;
	while(CHIP_NOT_READY);	
	spi_transfer(addr);
	result = spi_transfer(dat);
	HIGH_CS;
	return result;

}

uint8_t radio_write_burst(	uint8_t addr,
							uint8_t * buff,
							uint8_t len){
	uint8_t result, i;
	LOW_CS;
	while(CHIP_NOT_READY);	
	spi_transfer(addr | TI_CCxxx0_WRITE_BURST);
	for(i = 0;i < len;i++)
	{
		result = spi_transfer(buff[i]);
		buff[i] = result;
	}
	HIGH_CS;
	return result;
}



uint8_t radio_read_single(uint8_t addr)
{
	uint8_t result;
	LOW_CS;
	while(CHIP_NOT_READY);	
	spi_transfer(addr | TI_CCxxx0_READ_SINGLE);
	result = spi_transfer(0);
	HIGH_CS;
	return result;
}

uint8_t radio_read_status(uint8_t addr)
{
	uint8_t result;
	LOW_CS;
	while(CHIP_NOT_READY);	
	spi_transfer(addr | TI_CCxxx0_READ_BURST);
	result = spi_transfer(0);
	HIGH_CS;
	return result;
}

void radio_read_burst(	uint8_t addr,
							uint8_t * buff,
							uint8_t len){
	uint8_t result, i;
	LOW_CS;
	while(CHIP_NOT_READY);	
	spi_transfer(addr | TI_CCxxx0_READ_BURST);
	for(i = 0;i < len;i++)
	{
		buff[i] = spi_transfer(0);
	}
	HIGH_CS;
}


uint8_t radio_transmit(	uint8_t * txbuff,
						uint8_t len){

	uint8_t result;
	radio_write_strobe(TI_CCxxx0_SIDLE);
	radio_write_burst(TI_CCxxx0_TXFIFO, txbuff, len);

	result = radio_write_strobe(TI_CCxxx0_STX);
	return result;
}

uint8_t radio_transmit_packet(	uint8_t * txbuff,
								uint8_t len,
								uint8_t destination){
	uint8_t result;
	radio_tx_buff[LENGTH_FIELD_INDEX] = len;
	radio_tx_buff[ADDR_FIELD_INDEX] = destination;
	memcpy(&radio_tx_buff[DATA_FIELD_INDEX], txbuff, len);
	result = radio_transmit(radio_tx_buff,(len + DATA_FIELD_INDEX));
	return result;
}

uint8_t radio_receive_packet(	uint8_t * rxbuff,
								uint8_t len){
	return 0;
}

uint8_t radio_update_settings(void)
{
	radio_write_single(TI_CCxxx0_IOCFG2, 0x29); // GDO2 output pin config.
	radio_write_single(TI_CCxxx0_IOCFG1, 0x2E); // GDO0 output pin config.
	radio_write_single(TI_CCxxx0_IOCFG0, 0x06); // GDO0 output pin config.
	radio_write_single(TI_CCxxx0_FIFOTHR, 0x07);
	radio_write_single(TI_CCxxx0_SYNC1, 0xD3);
	radio_write_single(TI_CCxxx0_SYNC0, 0x91);
	
	radio_write_single(TI_CCxxx0_PKTLEN, 0xFF); // Packet length.
	radio_write_single(TI_CCxxx0_PKTCTRL1, 0x04); // Packet automation control.
	radio_write_single(TI_CCxxx0_PKTCTRL0, 0x05); // Packet automation control.
	radio_write_single(TI_CCxxx0_ADDR, 0x59); // Device address.
	radio_write_single(TI_CCxxx0_CHANNR, 0x00); // Channel number.
	radio_write_single(TI_CCxxx0_FSCTRL1, 0x08); // Freq synthesizer control.
	radio_write_single(TI_CCxxx0_FSCTRL0, 0x00); // Freq synthesizer control.
	radio_write_single(TI_CCxxx0_FREQ2, 0x5D); // Freq control word, high byte
	radio_write_single(TI_CCxxx0_FREQ1, 0x93); // Freq control word, mid byte.
	radio_write_single(TI_CCxxx0_FREQ0, 0xB1); // Freq control word, low byte.
	radio_write_single(TI_CCxxx0_MDMCFG4, 0x86); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG3, 0x83); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG2, 0x03); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG1, 0x22); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG0, 0xF8); // Modem configuration.
	radio_write_single(TI_CCxxx0_DEVIATN, 0x44); // Modem dev (when FSK mod en)
	radio_write_single(TI_CCxxx0_MCSM2 , 0x07); //MainRadio Cntrl State Machine
	radio_write_single(TI_CCxxx0_MCSM1 , 0x30); //MainRadio Cntrl State Machine
	radio_write_single(TI_CCxxx0_MCSM0 , 0x18); //MainRadio Cntrl State Machine
	radio_write_single(TI_CCxxx0_FOCCFG, 0x16); // Freq Offset Compens. Config
	radio_write_single(TI_CCxxx0_BSCFG, 0x6C); // Bit synchronization config.
	radio_write_single(TI_CCxxx0_AGCCTRL2, 0x03); // AGC control.
	radio_write_single(TI_CCxxx0_AGCCTRL1, 0x40); // AGC control.
	radio_write_single(TI_CCxxx0_AGCCTRL0, 0x91); // AGC control.
	radio_write_single(TI_CCxxx0_FREND1, 0x56); // Front end RX configuration.
	radio_write_single(TI_CCxxx0_FREND0, 0x10); // Front end RX configuration.
	radio_write_single(TI_CCxxx0_FSCAL3, 0xA9); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL2, 0x0A); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL1, 0x00); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL0, 0x11); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSTEST, 0x59); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_TEST2, 0x88); // Various test settings.
	radio_write_single(TI_CCxxx0_TEST1, 0x31); // Various test settings.
	radio_write_single(TI_CCxxx0_TEST0, 0x0B); // Various test settings.
	return 0;
}