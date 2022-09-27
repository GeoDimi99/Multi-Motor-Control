
#include "timer.h"


void timer5_init(int timer_duration_ms){
	
	// Cofiguraimo il time con modalità:
	// - CTC
	// - Prescaler = 1024
	TCCR5A = 0;
	TCCR5B = (1 << WGM52) | (1 << CS50) | (1 << CS52); 
	
	//Settiamo il valore OCR = (16MHz/1024)* time_duration_ms
	uint16_t ocrval=(uint16_t)(15.62*timer_duration_ms);
	OCR5A = ocrval;
	
	cli();						//Cancelliamo gli interrupt
		
	TIMSK5 |= (1<<OCIE5A);		//Abilita timer interrupt
	
	sei();						//Abilita tutti gli interrupt 
}
