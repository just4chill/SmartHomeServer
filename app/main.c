
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include "system_LPC17xx.h"
#include "uart.h"
#include "lcd.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lpc17xx_ssp.h"
#include "cpu_lpc1000.h"
#include "nrf24l01.h"

#define	_CH 			1	// Channel 0..125
#define	_Address_Width	5	// 3..5
// #define _Buffer_Size 	5	// 1..32
#define _Buffer_Size 	sizeof(unsigned long)	// 1..32

#define _DEB_DELAY 50

const char testModem[] = "AT\r\n";

void prvSetupHardware( void );

void NRF24L01_Receive(char Buf[_Buffer_Size]) {

	NRF24L01_CE_HIGH;
	Delay_us(130);

	while ((NRF24L01_Get_Status() & _RX_DR) != _RX_DR);
	NRF24L01_CE_LOW;

	NRF24L01_Read_RX_Buf(Buf, _Buffer_Size);
	NRF24L01_Clear_Interrupts();
}

void NRF24L01_Send(char Buf[_Buffer_Size]) {
	NRF24L01_Write_TX_Buf(Buf, _Buffer_Size);

	NRF24L01_RF_TX();

	while ((NRF24L01_Get_Status() & _TX_DS) != _TX_DS);

	NRF24L01_Clear_Interrupts();

}

static void lcdTask(void * pvParameters)
{
	uint32_t count = 0;
	uint8_t buff[10];
	for(;;)
	{
		count++;
		sprintf(buff, "%d",count);
		lcdClear();
		lcd_print(buff);
		vTaskDelay(1000);
	}
}

static void monitorModem(void * pvParameters)
{
	uint8_t result1, result2;
	char buff[75];
	for(;;)
	{
		uart_print(1, (char *)testModem);
		vTaskDelay(2000);
	}
}

static void rfProcess(void * pvParameters)
{
	char Buf[_Buffer_Size] = {1,2,3,4,5};
	for(;;)
	{
		if( ((LPC_GPIO1->FIOPIN) & (1 << 19)) == 0)
		{
			_delay_ms(_DEB_DELAY);
			NRF24L01_Set_Device_Mode(_TX_MODE);
			NRF24L01_Set_TX_Address("clie1", 5);
			NRF24L01_Send(Buf);
			NRF24L01_Set_Device_Mode(_RX_MODE);
			lcd_write_instruction_4d(0xC0);
			lcd_print("Switch 1");
		}
		if( ((LPC_GPIO1->FIOPIN) & (1 << 20)) == 0)
		{
			_delay_ms(_DEB_DELAY);
			NRF24L01_Set_Device_Mode(_TX_MODE);
			NRF24L01_Set_TX_Address("clie2", 5);
			NRF24L01_Write_TX_Buf(Buf, _Buffer_Size);
			NRF24L01_RF_TX();
			while ((NRF24L01_Get_Status() & _TX_DS) != _TX_DS);
			NRF24L01_Clear_Interrupts();
			lcd_write_instruction_4d(0xC0);
			lcd_print("Switch 2");
		}
		if( ((LPC_GPIO1->FIOPIN) & (1 << 21)) == 0)
		{
			_delay_ms(_DEB_DELAY);
			NRF24L01_Set_Device_Mode(_TX_MODE);
			NRF24L01_Set_TX_Address("clie3", 5);
			NRF24L01_Send(Buf);
			NRF24L01_Set_Device_Mode(_RX_MODE);
			lcd_write_instruction_4d(0xC0);
			lcd_print("Switch 3");
		}
	}
}

int main(void)
{
	/* Setup the Hardware */
	prvSetupHardware();

	/* Create the Tasks */
	// xTaskCreate(lcdTask,
	// 		(signed portCHAR *)"lcdTask",
	// 		128,
	// 		NULL,
	// 		3,
	// 		NULL);

	// xTaskCreate(monitorModem,
	// 		(signed portCHAR *)"monitorModem",
	// 		128,
	// 		NULL,
	// 		4,
	// 		NULL);

	xTaskCreate(rfProcess,
			(signed portCHAR *)"rfProcess",
			128,
			NULL,
			3,
			NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Never reach here */
	return 0;
}

void prvSetupHardware( void )
{
	/* Initialte Clock */
	SystemInit();
	SystemCoreClockUpdate();
	uart_init(0, 115200);			// Debug port
	uart_init(1, 9600);				// Modem
	uart_print(0, "System started\r\n");
	SSPInit();

	char Address[_Address_Width] = "serv1";
	NRF24L01_Init(_RX_MODE, _CH, _2Mbps, Address, _Address_Width, _Buffer_Size);

	lcd_init();
	lcd_print("Booting..");
	_delay_ms(100);
	lcdClear();
	lcd_write_instruction_4d(0x80);
	lcd_print("   RF System   ");

	LPC_GPIO1->FIODIR &= ~(1 << 19);
	LPC_GPIO1->FIODIR &= ~(1 << 20);
	LPC_GPIO1->FIODIR &= ~(1 << 21);

	LPC_GPIO1->FIOSET |= (1 << 19);
	LPC_GPIO1->FIOSET |= (1 << 20);
	LPC_GPIO1->FIOSET |= (1 << 21);
}

