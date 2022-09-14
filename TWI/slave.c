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
	Slave_Addr_init(0x01, 1);
	_delay_ms(5000);

	printf("%s", Receive_Buffer);
	return 0;
}
