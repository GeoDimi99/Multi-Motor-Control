#include <util/delay.h>
#include <stdio.h>
#include "librarys/uart.h"  //Questo inizializza la seriale per supportare la printf
#include <avr/io.h>
#include "my_librarys/encoder.h"
#include "my_librarys/pwm_signal.h"
#include "my_librarys/digital_io.h"



int main(void){
	//Inizializzo i oggetti utili
	printf_init();
	pwm_init();
	
	const uint8_t mask = (1<<4)|(1<<5);
	DDRE |= mask;
	PORTE = (1<<5);
	
	//Settiamo l'intensità iniziale:
	uint8_t intensity = 255;
	pwm_set_intensity(intensity);
	while(1){
		printf("Valore intensità: %d\n",intensity);
		_delay_ms(5000);
	}
	return 0;
	
}

//#include <util/delay.h>
//#include <stdio.h>
//#include <stdint.h>
//#include <avr/io.h>
//#include "../avr_common/uart.h"

// configuration bits for PWM
// fast PWM, 8 bit, non inverted
// output compare set low

// configuariamo i bit per PWM
// configuriamo il timer 1 (da 16 bit)
// come un timer a 8 bit
//output compare set low
//#define TCCRA_MASK (1<<WGM10)|(1<<COM1C0)|(1<<COM1C1)  //Modalità Fast PWM 8 bit(WGM10 = 1,WGM12 = 1) 
//#define TCCRB_MASK ((1<<WGM12)|(1<<CS10))              //(Per il  COMC0/COM1C1 vedere sul datashet)

//int main(void){
  //printf_init(); 

  //// useremo il timer 1
  //TCCR1A=TCCRA_MASK;
  //TCCR1B=TCCRB_MASK;
  //// clear all higher bits of output compare for timer
  //OCR1AH=0;
  //OCR1BH=0;
  //OCR1CH=0;
  //OCR1CL=1;

  //// il LED è connesso alla porta 13
  //// che è il pin 7 della porta b, e vediamo l'output
  //const uint8_t mask=(1<<7);
  //// we configure the pin as output
  //DDRB |= mask;//mask;

  //uint8_t intensity=0;
  //while(1){
    //// we write on the output compare register a value
    //// that will be proportional to the opposite of the
    //// duty_cycle
    //OCR1CL=intensity; 
    
    //printf("v %u\n", (int) OCR1CL);
    //_delay_ms(100); // from delay.h, wait 1 sec
    //intensity+=8;
  //}
  
//}
