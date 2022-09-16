#include <util/delay.h>
#include <stdio.h>
#include "librarys/uart.h"  //Questo inizializza la seriale per supportare la printf
#include <avr/io.h>
#include "my_librarys/encoder.h"



int main(void){
	//Inizializzo i oggetti utili
	printf_init();
	encoder_init();
	
	
	while(1){
		printf("Valore encoder: %d\n",encoder_read());
		_delay_ms(100);
	}
	
}
