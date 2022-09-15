/**
 * pwm.c
 */

#include "pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Configuration bits for PWM
// fast PWM, 8 bit, non inverted
// output compare set low

#define TCCRA_MASK  (1<<WGM10) | (1<<COM1C0) | (1<<COM1C1)    
#define TCCRB_MASK ((1<<WGM12) | (1<<CS10))
#define PORTB_MASK 1 << 7									//Pin 13 sulla scheda Arduino

void PWM_init(void) {
    // Usiamo il timer 1 per generare l'onda quadra
    TCCR1A = TCCRA_MASK;
    TCCR1B = TCCRB_MASK;

    OCR1AH = 0;
    OCR1BH = 0;
    OCR1CH = 0;
    OCR1CL = 1;

    DDRB |= PORTB_MASK;
}

void PWM_setDutyCycle(uint8_t value) {
    OCR1CL = 255 - value;
}
