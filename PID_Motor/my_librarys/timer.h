
#pragma once

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// timer5_init : setta il timer 5 che ogni time_duration_ms genera una interrupt, per poter
// poter sfruttare tale interrupt bisogna settare la macro ISR(TIMER5_COMPA_vect) {...}
// definita nella libreria avr/interrupt.h, quindi è importante ricordarsi di importare la 
// tale libreria quando si definisce la macro. 
void timer5_init(int time_duration_ms);
