#include <avr/io.h>
#include "TWI_lib.h"
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../avr_common/uart.h"
 
int main(void){
	printf_init();
	sei();                                      //abilita le interruzioni globali
	TWI_Init();                                 //inizializza TWI
	uint8_t data[] = "Hello";
	uint8_t data_len = sizeof(data)/sizeof(data[0]);
	TWI_info.error_code = TWI_NO_RELEVANT_INFO;
	TWI_Transmit_Data((void*)data, data_len, 0);
	_delay_ms(1000);
	printf("char : %s\n",(char*)Transmit_Buffer);
	return 0;
}
