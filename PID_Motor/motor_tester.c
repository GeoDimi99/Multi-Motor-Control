#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include "my_librarys/Motor.h"
#include "my_librarys/uart.h"
//#include "../avr_common/uart.h"


#define K_P 0.5f
#define K_I	0.0f
#define K_D 0.1f

//Motor* mtr;

//ISR(TIMER5_COMPA_vect) {
	//spin_once(mtr);
//}

//uint8_t UART_putChar(uint8_t* buf);
//uint8_t UART_getChar(uint8_t* buf);

// Definizione del comportamento del vettore delle interrupt



int main(void){
	//printf_init();
	uartInit(19200);
	
	
	while(1){ 
		uartSendByte('a');
		uartSendByte('\n');
		uartSendByte('\0');
		_delay_ms(1000);
	
	}
	
	return 0;
}


