#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include "my_librarys/Motor.h"
#include "my_librarys/uart.h"
#include "../avr_common/uart.h"


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
	printf_init();
	UART_init();
	
	uint8_t ret_string [20];
	
	while(1){
		int r = 0;
		while(UART_getChar(&ret_string[r]) && ret_string[r] != '\0') r++;
		
		for(int i = 0; i < r ; i++) printf("%d\n",ret_string[i]); 
		
		int w = 0;
		while(UART_putChar(&ret_string[w]) && ret_string[w] != '\0') w++;
	}
	
	return 0;
}


