
#pragma once

#include <stdint.h>
#include <avr/io.h>

//pwm_init : inizializzazione dell'onda quadra sul PIN 7 PORTB (pin 13 su Arduino) in modalità:
// - fast pwm
// - non invertente
// - senza prescaler
void pwm_init(void);


//pwm_set_intensity: setta l'intensita da 0 a 255, l'intensità cresce al crescere del valore
//dato in input
void pwm_set_intensity(uint8_t intensity);
