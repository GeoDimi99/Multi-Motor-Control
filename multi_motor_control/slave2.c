#include <avr/io.h>
#include <stdint.h>
#include "TWI_lib.h"
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../avr_common/uart_int.h"
#include "../pid_motor/my_librarys/Motor.h"

#define SAMPLE_command "sample"
#define GET_command "get"
#define SET_command "set"
#define APPLY_command "apply"

#define VELOCITY_LEN 20

//Parametri PID
#define K_P .4f
#define K_I 0.0f
#define K_D .1f

Motor* mtr;
 
int main(void){ 
	//Settiamo lo slave
	mtr = Motor_init(K_P, K_I, K_D);
	set_type_controller(mtr, CLOSE_LOOP);
	Slave_Addr_init(0x02, 1);                    //inizializza TWI
	sei();                                       //abilita le interruzioni globali
	
	
	uint8_t current_state[VELOCITY_LEN]= "P:0, CV:0, DV:0";
	uint8_t desired_velocity[VELOCITY_LEN]= "0";

	while(1){
		//Ricevi velocita
		TWI_Slave_Receive_Data();     			//ricevo un comando 
		
		if (!strcmp(Receive_Buffer, SET_command)){
			TWI_Slave_Receive_Data();          //ricevo desired velocity 
			if (*Receive_Buffer != 0) strcpy(desired_velocity, Receive_Buffer);   //inscerisco in desired velocity
		}
		else if(!strcmp(Receive_Buffer, APPLY_command)){
			// Conversione e setta velocita
			int vel = atoi(desired_velocity);
			set_desired_velocity(mtr, vel);
		}
		else if(!strcmp(Receive_Buffer, SAMPLE_command)){
			//Lettura stato motore
			sprintf(current_state, "P:%d, CV:%d, DV:%d", mtr->angular_position, mtr->angular_velocity , mtr->desired_velocity);
		}
		else if (!strcmp(Receive_Buffer, GET_command)){
			TWI_Slave_Transmit_Data(current_state, VELOCITY_LEN); //invio velocità 
		}
		else{
			UART_putString("Non ho ricevuto un comando valido\n"); 
		}
	}
	return 0;
}

// Definizione del comportamento del motore
ISR(TIMER5_COMPA_vect) {
	spin_once(mtr);
}

