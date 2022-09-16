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
	uint8_t data[7]; 
	data[0]= 0x00;
	data[1]='H';
	data[2]='e';
	data[3]='l';
	data[4]='l';
	data[5]='o';
	data[6]='\0';

	TWI_Transmit_Data((void*const)data, 7, 0);
	while (!is_TWI_ready()) { _delay_us(1);}
	TWI_Read_Data(0x01, 6, 0); 
	while (!is_TWI_ready()) { _delay_us(1);}
	printf("%s\n", Receive_Buffer);
	
	return 0;
}
