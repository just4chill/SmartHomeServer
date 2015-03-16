
#ifndef __radio_h__
#define __radio_h__

#define CS_PIN_NO 10

#define HIGH_CS LPC_GPIO0->FIOSET |= (1 << CS_PIN_NO)
#define LOW_CS  LPC_GPIO0->FIOCLR |= (1 << CS_PIN_NO)

#define CHIP_NOT_READY ((LPC_GPIO0->FIOPIN & (1 << PIN_SO)) >> PIN_SO)

#define LENGTH_FIELD_INDEX 0
#define ADDR_FIELD_INDEX   1
#define DATA_FIELD_INDEX   2

#define RADIO_TX_BUFFER_BYTES 32
#define RADIO_RX_BUFFER_BYTES 32

extern void radio_init(void);
extern uint8_t radio_reset(void);
extern uint8_t radio_write_strobe(uint8_t);
extern uint8_t radio_write_single(uint8_t, uint8_t);
extern uint8_t radio_write_burst(uint8_t, uint8_t *, uint8_t);
extern uint8_t radio_read_single(uint8_t);
extern uint8_t radio_read_status(uint8_t);
extern void radio_read_burst(uint8_t, uint8_t *, uint8_t);
extern uint8_t radio_transmit(uint8_t *, uint8_t);
extern uint8_t radio_transmit_packet(uint8_t *, uint8_t, uint8_t);
extern uint8_t radio_receive_packet(uint8_t *, uint8_t);
extern uint8_t radio_update_settings(void);

#endif