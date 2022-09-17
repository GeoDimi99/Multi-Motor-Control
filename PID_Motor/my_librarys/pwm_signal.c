
#include "pwm_signal.h"

// Utilizziamo il timer 5, perché nello standard del AVR Mega è dedicato al servo motore.
// Mettiamo le varie modalità:
// - fast PWM attraverso (WGM50 = 1, WGM52 = 1) 
// - non invertente (COM5C1 = 1) : ovvero che l'impulso avviene all'inizio del period
// - nessun prescaler (CS50 = 1).
#define TCCRA_MASK (1<<WGM10)|(1<<COM1C0)|(1<<COM1C1) 
#define TCCRB_MASK (1<<WGM12)|(1<<CS10)

void pwm_init(void){
	
	TCCR1A = TCCRA_MASK;		//Impostiamo i registri di TCCR A
	TCCR1B = TCCRB_MASK;		//Impostiamo i registri di TCCR B
	
	//Puliamo tutti i bit alti per il compere timer
	OCR1AH = 0;
	OCR1BH = 0;
	OCR1CH = 0;
	OCR1CL = 1;
	
	//Settiamo il PIN 7 PORTB in output
	const uint8_t mask = (1<<7);
	DDRB |= mask;
	
}

void pwm_set_intensity(uint8_t intensity){
	//Settiamo l'intensità
	OCR1CL = 255 - intensity;
}



