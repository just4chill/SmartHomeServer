
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include "system_LPC17xx.h"
#include "uart.h"
#include "spi.h"
#include "radio.h"
#include "cc2500_reg.h"
#include "lcd.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char testModem[] = "AT\r\n";

void prvSetupHardware( void );

static void lcdTask(void * pvParameters)
{
	uint32_t count = 0;
	uint8_t buff[10];
	for(;;)
	{
		sprintf(buff, "H:%d,T:%d", uart1_rx_head, uart1_rx_tail);
		lcd_print(buff);
		vTaskDelay(1000);
		lcdClear();
		vTaskDelay(1);
	}
}

static void monitorModem(void * pvParameters)
{
	uint8_t result1, result2;
	char buff[75];
	for(;;)
	{
		 uart_print(1, (char *)testModem);
		// result1 = radio_read_status(TI_CCxxx0_PARTNUM);
		// result2 = radio_read_status(TI_CCxxx0_VERSION);
		// sprintf(buff, "%#02x,%#02x,%d,%d \r\n", result1, result2, uart1_rx_head, uart1_rx_tail);
		// uart_print(0, buff);
		vTaskDelay(2000);
	}
}

int main(void)
{
	/* Setup the Hardware */
	prvSetupHardware();

	/* Create the Tasks */
	xTaskCreate(lcdTask,
			(signed portCHAR *)"lcdTask",
			128,
			NULL,
			3,
			NULL);

	xTaskCreate(monitorModem,
			(signed portCHAR *)"monitorModem",
			128,
			NULL,
			4,
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
	// spi_init();
	// radio_init();
	lcd_init();
	lcd_print("Booting..");
	_delay_ms(100);
	lcdClear();
	LPC_GPIO0->FIODIR |= (1 << 21);
}

