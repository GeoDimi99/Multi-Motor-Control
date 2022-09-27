#include <avr/io.h>
#include "TWI_lib.h"
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../avr_common/uart_int.h"


#define VELOCITY_LEN 25

 
int main(void){
	UART_init();
	TWI_Init();                                 //inizializza TWI
	sei();                           						//abilita le interruzioni globali
	
	uint8_t input[TRANSMIT_BUFLEN];	
	uint8_t current_state_1[VELOCITY_LEN]= "none";         //modificare e mettere lungezza generica
	uint8_t desired_velocity_1[VELOCITY_LEN]= "none";        //modificare e mettere lungezza generica
	uint8_t current_state_2[VELOCITY_LEN]= "none";         //modificare e mettere lungezza generica
	uint8_t desired_velocity_2[VELOCITY_LEN]= "none";        //modificare e mettere lungezza generica
	uint8_t send_velocity[VELOCITY_LEN + 1];
	uint8_t addr= 0x01; 
	
	uint8_t sample[] = {0x00, 's', 'a', 'm', 'p', 'l', 'e', '\0'};     //sample in broadcast
	uint8_t get[] = { 0x00, 'g', 'e', 't','\0'};          //get in unicast
	uint8_t set[] = { 0x00,  's', 'e', 't','\0'};          //set in unicast
	uint8_t apply[] = {0x00, 'a', 'p', 'p', 'l', 'y', '\0'};           //apply in broadcast
	
	uint8_t i=0;

	while(1){
		
		UART_putString("Stato corrente motore 1: ");
		UART_putString(current_state_1);             //mando in output la velocità corrente ricevuta dal primo motore
		UART_putString("\n");
		
		UART_putString("Stato corrente motore 2: ");
		UART_putString(current_state_2);              //mando in output la velocità corrente del motore 2
		UART_putString("\n");
		
		UART_getString(desired_velocity_1);
		if (*desired_velocity_1 == 0) strcpy(desired_velocity_1, current_state_1); 
		
		UART_getString(desired_velocity_2);
		if (*desired_velocity_2 == 0) strcpy(desired_velocity_2, current_state_2); 
		
		///// COMUNICAZIONE CON IL PRIMO SLAVE /////
		
		set[0]= (0x01 << 1) | 0x00;                    
				              
		TWI_Transmit_Data(set, 5, 0);                    //invio il comando "set" al primo slave
		
		send_velocity[0]= (0x01 << 1) | 0x00;          
		
		strcpy(send_velocity + 1, desired_velocity_1);       //inserisco la velocità desiderata dentro al buffer da inviare (indirizzo + velocità desiderata)
		
		TWI_Transmit_Data(send_velocity, VELOCITY_LEN+1, 0);  //invio la velocità desiderata  al primo slave
		
		///// COMUNICAZIONE CON IL SECONDO SLAVE /////
		
		/*set[0]= (0x02 << 1) | 0x00;                    
				              
		TWI_Transmit_Data(set, 5, 0);                    //invio il comando "set" al secondo slave
		
		send_velocity[0]= (0x02 << 1) | 0x00;          
		
		strcpy(send_velocity + 1, desired_velocity_2);       //inserisco la velocità desiderata dentro al buffer da inviare (indirizzo + velocità desiderata)
		
		TWI_Transmit_Data(send_velocity, VELOCITY_LEN+1, 0);  //invio la velocità desiderata  al secondo slave*/
		
		///// COMANDO APPLY IN BROADCAST /////
	
		TWI_Transmit_Data(apply, 7, 0);                   //invio il comando "apply" 
		
		//// COMANDO SAMPLE IN BROADCAST /////
		
		TWI_Transmit_Data(sample, 8, 0);               //invio comando "sample"  
		
		///// CONTROLLO VELOCITA PRIMO MOTORE /////
		
		get[0]= (0x01 << 1) | 0x00;              
		                
		TWI_Transmit_Data(get, 5, 0);               //invio comando "get" al primo slave
		
		TWI_Read_Data(0x01, VELOCITY_LEN, 0);          //leggo la velocità corrente del primo slave
		 
		strcpy(current_state_1, Receive_Buffer);      //salvo la velocità corrente
		
		///// CONTROLLO VELOCITÀ SECONDO MOTORE /////
		
		/*get[0]= (0x02 << 1) | 0x00;              
		                
		TWI_Transmit_Data(get, 5, 0);               //invio comando "get" al primo slave
		
		TWI_Read_Data(0x02, VELOCITY_LEN, 0);          //leggo la velocità corrente del primo slave
		 
		strcpy(current_state_2, Receive_Buffer);      //salvo la velocità corrente*/
		
		UART_putString("\n"); 
		
		
	}
	
	return 0;

}
