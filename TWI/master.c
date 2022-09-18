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
	data[0]= (0x01<<1)|0x00;
	data[1]='H';
	data[2]='e';
	data[3]='l';
	data[4]='l';
	data[5]='o';
	data[6]='\0';
	
	uint8_t data1[8]; 
	data1[0]= 0x00;
	data1[1]='W';
	data1[2]='i';
	data1[3]='n';
	data1[4]='n';
	data1[5]='e';
	data1[6]='r';
	data1[7]='\0';

	TWI_Transmit_Data((void*const)data, 7, 0); //1

	TWI_Read_Data(0x01, 6, 0); //2
	
	printf("%s\n", Receive_Buffer);
	
	TWI_Transmit_Data((void*const)data, 7, 0); //3

	TWI_Read_Data(0x01, 6, 0);  //4

	printf("%s\n", Receive_Buffer);
	
	TWI_Read_Data(0x01, 6, 0); //5
	
	printf("%s\n", Receive_Buffer);
	
	TWI_Transmit_Data((void*const)data, 7, 0); //6
	
	TWI_Transmit_Data((void*const)data1, 8, 0); //7
	
	TWI_Read_Data(0x01, 9, 0); //8
	
	printf("%s\n", Receive_Buffer);
	
	printf("bye bye\n");
	
	return 0;
}
