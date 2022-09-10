#include <avr/io.h>
#include "TWIlib.h"
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>
 
int main(void){
	sei();                                       //abilita le interruzioni globali
	TWI_Init();                                  //inizializza TWI
	return 0;
}
