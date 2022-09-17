
#pragma once

#include <avr/io.h>
#include <stdint.h>

// digit_init: inizializzazione di pin di output (pin 2 e 3).
void digit_init(void);

// digit_write: scrittura del pin:
// - pin_mask : pin di output
// - pin_value: valore 0 o 1 da stampare 
// ATTENZIONE: si può attivare un pin alla volta, NON c'è nessun controllo per verificare se la maschera ha
// solamente un pin settato, sta al programmatore essere coerente. 
void digit_write(uint8_t pin_mask ,uint8_t pin_value);


// NOTA: I pin si possono attivare uno alla volta poiché controllano la direzione del motore
// e il motore non può andare in due direzioni diverse.
