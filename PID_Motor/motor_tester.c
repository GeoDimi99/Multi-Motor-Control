#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "my_librarys/Motor.h"
#include "my_librarys/uart_int.h"

//Parametri del controllore PID
#define K_P 0.25f
#define K_I	0.0f
#define K_D 0.1f

//Lunghezza buffer per le stringhe
#define MAX_BUF 256
#define MAX_CF  10

//Riferimento all'oggetto Motor
Motor* mtr;



int main(void){
	
	//SETUP del programma : 
	//- Inizializzazione Comunicazione 
	UART_init();
	//- Inizializzazione motore ad anello chiuso
	mtr = Motor_init(K_P, K_I, K_D);
	set_type_controller(mtr, CLOSE_LOOP);
	
	//Variabili per il programma
	uint8_t* in_str = (uint8_t*) malloc(MAX_BUF * sizeof(uint8_t));
	uint8_t* out_str = (uint8_t*) malloc(MAX_BUF * sizeof(uint8_t));
	uint8_t* cf_str = (uint8_t*) malloc(MAX_CF * sizeof(uint8_t));
	
	
	
	//LOOP del programma
	while(1){
		
		//Controllo se c'è un input da leggere
		
		
		if(UART_getString(in_str) > 1 ){
			int vel = atoi(in_str);
			int dir = vel > 0 ? RIGHT : LEFT ;
			sprintf(out_str, "Read char: %s", in_str);
			UART_putString(out_str);
			UART_putChar('\n'); 
			set_desired_velocity(mtr, abs(vel), dir);
			UART_getString(in_str);
		}
		
		//Stampa in ouput lo stato del motore
		sprintf(out_str, "Current velocity: %d,Desidered velosity: %d, Current PWM: %d", get_angular_velocity(mtr), get_desired_velocity(mtr), get_current_pwm(mtr));
		UART_putString(out_str);
		UART_putChar('\n');
		
		_delay_ms(100);
		
	}
	return 0;
}


// Definizione del comportamento del motore
ISR(TIMER5_COMPA_vect) {
	spin_once(mtr);
}


