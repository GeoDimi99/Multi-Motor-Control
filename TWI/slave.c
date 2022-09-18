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
	Slave_Addr_init(0x01, 1);  //inizializza TWI
	uint8_t data[6]; 
	data[0]= 'W';
	data[1]='o';
	data[2]='r';
	data[3]='l';
	data[4]='d';
	data[5]='\0';
	
	uint8_t data1[9]; 
	data1[0]= 'S';
	data1[1]='a';
	data1[2]='r';
	data1[3]='e';
	data1[4]='t';
	data1[5]='t';
	data1[6]='a';
	data1[7]='!';
	data1[8]='\0';
	
	TWI_Slave_Receive_Data();  //1

	printf("%s\n", Receive_Buffer);
	
	TWI_Slave_Transmit_Data((void*)data, 6); //2
	
	TWI_Slave_Receive_Data();  //3

	printf("%s\n", Receive_Buffer);
	
	TWI_Slave_Transmit_Data((void*)data, 6); //4
	
	TWI_Slave_Transmit_Data((void*)data, 6); //5
	
	TWI_Slave_Receive_Data(); //6
	
	printf("%s\n", Receive_Buffer);
	
	TWI_Slave_Receive_Data(); //7
	
	printf("%s\n", Receive_Buffer);
	
	TWI_Slave_Transmit_Data((void*)data1, 9); //8*/
    
	printf("bye bye\n");
	
	return 0;
}
