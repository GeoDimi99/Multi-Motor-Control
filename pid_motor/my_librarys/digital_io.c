
#include "digital_io.h"


void digit_init(void){
	
	//Mettiamo i pin 2 e 3 in output 
	const uint8_t mask = (1<<4)|(1<<5);
	DDRE |= mask;
	
}

void digit_write(uint8_t pin_mask ,uint8_t pin_value){
	
	if(pin_value) PORTE = pin_mask;   //Setto il pin a 1 (quindi escono 5v)
	else 		  PORTE = 0;		  //Setto il pin a 0 (quindi escono 0v)

}
