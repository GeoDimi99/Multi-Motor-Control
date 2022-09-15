#include <util/delay.h>
#include <stdio.h>
#include "librarys/uart.h"  //Questo inizializza la seriale per supportare la printf
#include <avr/io.h>
#include "librarys/encoder.h"
#include "librarys/pwm.h"


int main(void){
	//Inizializzo i oggetti utili
	printf_init();
	encoder_init();
	PWM_init();
	
	int intesity = 100;
	PWM_setDutyCycle(intesity);
	
	while(1){
		printf("Valore encoder: %d, intensita: %d\n",encoder_read(), intesity);
		_delay_ms(3000);
		intesity+= 8;
		if (intesity > 255) intesity = 0;
		PWM_setDutyCycle(intesity);
	}
	
}
