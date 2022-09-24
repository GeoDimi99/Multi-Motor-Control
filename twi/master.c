#include <avr/io.h>
#include "TWI_lib.h"
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../avr_common/uart_int.h"

#define velocity_len 10

 
int main(void){
	UART_init();
	printf_init();
	sei();                                      //abilita le interruzioni globali
	TWI_Init();                                 //inizializza TWI
	
	uint8_t input[TRANSMIT_BUFLEN];	
	uint8_t current_velocity[velocity_len]= "none";         //modificare e mettere lungezza generica
	uint8_t desired_velocity[velocity_len]= "none";        //modificare e mettere lungezza generica
	uint8_t send_velocity[11];
	uint8_t addr= 0x01; 
	
	uint8_t sample[] = {0x00, 's', 'a', 'm', 'p', 'l', 'e', '\0'};     //sample in broadcast
	uint8_t get[] = { (0x01 << 1) | 0x00, 'g', 'e', 't','\0'};          //get in unicast
	uint8_t set[] = { (0x01 << 1) | 0x00,  's', 'e', 't','\0'};          //set in unicast
	uint8_t apply[] = {0x00, 'a', 'p', 'p', 'l', 'y', '\0'};           //apply in broadcast
	send_velocity[0]= (0x01 << 1) | 0x00;
	
	uint8_t i=0;

	while(1){
		
		UART_putString("Inserire in motore [1/2]:\n");
		UART_getString(input);
		if (!strcmp(input, "1")){
			addr=0x01;
			get[0]= (0x01 << 1) | 0x00;
			set[0]= (0x01 << 1) | 0x00;
			send_velocity[0]= (0x01 << 1) | 0x00;
		}
		else{
			addr=0x02;
			get[0]= (0x02 << 1) | 0x00;
			set[0]= (0x02 << 1) | 0x00;
			send_velocity[0]= (0x02 << 1) | 0x00;
		}
		
		UART_putString("Inserire la velocita desiderata...\n");
		UART_getString(desired_velocity);
		if (*desired_velocity == 0) strcpy(desired_velocity, current_velocity); 
		
		
		TWI_Transmit_Data(sample, 8, 0);               //invio comando "sample"  
		                
		TWI_Transmit_Data(get, 5, 0);               //invio comando "get"
		
		TWI_Read_Data(addr, velocity_len, 0);          //leggo la velocità corrente
		 
		strcpy(current_velocity, Receive_Buffer);      //salvo la velocità corrente
				              
		TWI_Transmit_Data(set, 5, 0);                    //invio il comando "set" 
		
		strcpy(send_velocity + 1, desired_velocity);       //inserisco la velocità desiderata dentro al buffer da inviare (indirizzo + velocità desiderata)
		
		TWI_Transmit_Data(send_velocity, velocity_len+1, 0);  //invio la velocità desiderata  
	
		TWI_Transmit_Data(apply, 7, 0);                   //invio il comando "apply" 
		
		strcpy(current_velocity, desired_velocity);         //imposto velocità corrente == velocità desiderata
		
		UART_putString("current_velocity: ");
		UART_putString(current_velocity);
		UART_putString("\n");
		
	}
	
	return 0;

}
