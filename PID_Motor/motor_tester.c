#include <util/delay.h>
#include <stdio.h>
#include "librarys/uart.h"  //Questo inizializza la seriale per supportare la printf
#include <avr/io.h>
#include <avr/interrupt.h>
#include "my_librarys/Motor.h"

Motor* mtr;

ISR(TIMER5_COMPA_vect) {
	spin_once(mtr);
}



int main(void){
	//Inizializzo i oggetti utili
	printf_init();

	//Inizializzazione del motore
	mtr = Motor_init(0, 0, 0);
	set_type_controller(mtr, OPEN_LOOP);
	set_desired_velocity(mtr, 136, LEFT);
	
	
	
	//Settiamo l'intensità iniziale:
	while(1){
		printf("Velocita angolare: %d\n",mtr->desired_velocity);
		_delay_ms(1000);
				
	}
	
}


