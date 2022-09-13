/**
 * encoder.c
 */

#include "encoder.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define PIN_MASK 0x03		//Utilizziamo i pin 50,51,52,53 sulla scheda Arduino

// Sttiamo volatile per settare le letture in RAM al posto dei registri
volatile uint8_t portb_prev_reading;
volatile uint8_t portb_curr_reading;

volatile uint16_t encoder_counts;

static const int8_t _transition_table []= {
    0,  //00->00
    1,  //00->01
   -1,  //00->10
    0,  //00->11
   -1,  //01->00
    0,  //01->01
    0,  //01->10
    1,  //01->11
    1,  //10->00
    0,  //10->01
    0,  //10->10
   -1,  //10->11
    0,  //11->00
   -1,  //11->01
    1,  //11->10
    0   //11->11
};

void encoder_init(void) {
    
    cli();								// Blocchiamo le interruzioni
	// Usiamo la pota B per gli encoders
    DDRB &= ~PIN_MASK; 					// settiamo i pin come input
    PORTB |= PIN_MASK; 					// sui restanti pin abilitiamo i pull up resistors
    
    // Installiamo le interrupts
    PCICR |= (1 << PCIE0); 				// Settiamo le interrupt sul cambiamento , visto per PCMSK0
    PCMSK0 |= PIN_MASK; 				// Settiamo PCINT0 a triggerare un interrupt quando cambia lo stato
    
    // Inizializziamo le variabili di encoder
    portb_prev_reading = PIN_MASK;
    portb_curr_reading = PIN_MASK;
    encoder_counts = 0;
    
    sei();								//Abilita le interruzioni
}

uint16_t encoder_read(void) {
    return encoder_counts;				//Restituisci il contatore del encoder
}

ISR(PCINT0_vect) {
    portb_prev_reading = portb_curr_reading;
    portb_curr_reading = PINB;

    // Genera un indice per accedere alla tabella 
    uint8_t idx = (portb_prev_reading & PIN_MASK) << 2 | (portb_curr_reading & PIN_MASK);  
    // Incrementa il valore del encoder con il valore giusto. 
    encoder_counts += _transition_table[idx]; 
}
