#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "encoder.h"


#define PIN_MASK 0x0F     //Usiamo gli ultimi quattro bit della porta B (50-53)

volatile uint8_t previous_pins;
volatile uint8_t current_pins;
volatile uint16_t position_status;


uint8_t transition_table[] = {
	 0,			//00->00
	+1,			//00->01
	-1,			//00->10
	 0,			//00->11
	-1,			//01->00
	 0,			//01->01
	 0,			//01->10
	+1,			//01->11
	+1,			//10->00
	 0,			//10->01
	 0,			//10->10
	-1,			//10->11
	 0,			//11->00
	-1,			//11->01
	+1,			//11->10
	 0			//11->11
	
	
};


void encoder_init(void){
	cli();					//Disabilitiamo le interrupt
	
	DDRB &= ~PIN_MASK; 		//Settiamo la PIN_MASK come input per poter leggere l'encoder
	PORTB |= PIN_MASK;		//Abilitimo i pull up resistors (poichè sono porte settate in input)
	
	PCICR |= (1<< PCIE0);	//Abilitiamo Pin Change Interrupt 0, ogni cambiamento su PCINT7:0
							//causa interrupt ed eseguie la routine del PCI0 Interrupt Vector
	PCMSK0 |= PIN_MASK;		//I pin PCINT7:0 sono abilitati individualmente dal registro PCMSK0	
							//in particolare abilitiamo:
							//- PCINT0 = pin 53 Arduino
							//- PCINT1 = pin 52 Arduino
							//- PCINT2 = pin 51 Arduino
							//- PCINT3 = pin 51 Arduino
							// Vedere datasheet e foto pin Arduino per dettagli
	
	current_pins = PINB & PIN_MASK; //Lettura dei pin attuali
	previous_pins = current_pins;	//Settaggio dei pin precedenti ai pin attuali
	
	sei();					//Abilitiamo le interrupt
	
}



uint16_t encoder_read(void){
	return position_status;	//Restituiamo la posizione del encoder
}


ISR(PCINT0_vect){		
	previous_pins = current_pins;  		//Imposto i pin precedenti i pin attuali ai pin precedenti
	current_pins = PINB & PIN_MASK;		//Leggo i pin e gli imposto come attuali;
	
	//Generazione dell'indice della tabella
	uint8_t TT_index = ((PIN_MASK & previous_pins) << 2) | (PIN_MASK & current_pins);
	
	//Incremento dello stato corrente del encoder
	position_status += transition_table[TT_index];
	
	
	
}

