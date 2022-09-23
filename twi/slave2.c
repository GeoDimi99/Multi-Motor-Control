#include <avr/io.h>
#include <stdint.h>
#include "TWI_lib.h"
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <string.h>
//#include "../avr_common/uart.h"
#include "../avr_common/uart_int.h"

#define SAMPLE_command "sample"
#define GET_command "get"
#define SET_command "set"
#define APPLY_command "apply"

#define velocity_len 10
 
int main(void){
	UART_init(); 
	printf_init();
	sei();                                       //abilita le interruzioni globali
	Slave_Addr_init(0x02, 1);                    //inizializza TWI
	
	uint8_t current_velocity[velocity_len]= "none";
	uint8_t desired_velocity[velocity_len]= "none";

	while(1){
		
		TWI_Slave_Receive_Data();     //ricevo comando "sample"
		
		TWI_Slave_Receive_Data();     //ricevo comando "get"
		
		TWI_Slave_Transmit_Data(current_velocity, velocity_len);  //invio velocità
		
		TWI_Slave_Receive_Data();       //ricevo comando "set"
		
		TWI_Slave_Receive_Data();           //ricevo desired velocity 
		
		if (*Receive_Buffer != 0) strcpy(desired_velocity, Receive_Buffer);   //la salvo 
		 
		TWI_Slave_Receive_Data();             //ricevo comando "apply"

		strcpy(current_velocity, desired_velocity);  //applico la velocità
		
		UART_putString("current_velocity: ");
		UART_putString(current_velocity);
		UART_putString("\n");
	}
	return 0;
}

