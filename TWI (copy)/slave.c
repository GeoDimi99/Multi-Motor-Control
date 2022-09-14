#include <avr/io.h>
#include <stdint.h>
#include "TWI_lib.h"
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../avr_common/uart.h"
 
 
int main(void){
	printf_init();
	sei();                                       //abilita le interruzioni globali
	TWI_Init();                                  //inizializza TWI
	while (1){
		printf("%c\n", TWDR);
		_delay_ms(1000);
	}
	return 0;
}
