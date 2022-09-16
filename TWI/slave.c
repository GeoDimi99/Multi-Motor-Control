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
	uint8_t data[6]; 
	data[0]= 'W';
	data[1]='o';
	data[2]='r';
	data[3]='l';
	data[4]='d';
	data[5]='\0';

	TWI_Slave_Receive_Data(0x01); 
	while (!is_TWI_ready()) { _delay_us(1);}
	printf("%s\n", Receive_Buffer);
	TWI_Slave_Transmit_Data(0x01, (void*)data, 6);
	while (!is_TWI_ready()) { _delay_us(1);}
	printf("bye bye\n");
	return 0;
}
