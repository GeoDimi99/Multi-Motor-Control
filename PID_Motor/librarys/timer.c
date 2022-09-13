/**
 * timer.c
 */

#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void TIMER5_init(uint16_t interval) {
    cli();   												//Disabilità tutti gli interrupt
    TCCR5A = 0;  											//Disabilità tutti i segnali PWM e interrupts
    TCCR5B = (1 << WGM52) | (1 << CS50) | (1 << CS52);  	//Modalità CTC e prescaler a 1024 (16 Mhz / 1024)
    uint16_t ocrval = (uint16_t)(15.62 * interval);			//OCR value = (16MHz/1024) * time_duration
    OCR5A = ocrval;
    TIMSK5 |= (1 << OCIE5A);								//Abilità i timer interrupts 
    sei();   												//Abilita tutti gli interrupt
}
