#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "my_librarys/Motor.h"
#include "my_librarys/uart_int.h"

//Parametri del controllore PID
#define K_P 0.5f
#define K_I	0.0f
#define K_D 0.1f

//Lunghezza buffer per le stringhe
#define BUF_SIZE 20
#define MAX_BUF 256

//Riferimento all'oggetto Motor
Motor* mtr;



int main(void){
	
	////Setup del programma
	//UART_init();
	//mtr = Motor_init(K_P, K_I, K_D);
	
	////Variabili per il programma
	////uint8_t* ret_str = (uint8_t*) malloc(BUF_SIZE * sizeof(uint8_t));
	
	////Loop del programma
	//while(1){ 
		
		//UART_putChar('a');
		//UART_putChar('b');
		//UART_putChar('\n');
		//UART_putChar('\0');
		//_delay_ms(1000);
	
	//}
  UART_init();
  UART_putString((uint8_t*)"write repeat it\n");
  //uint8_t l [] = {'h','e','l','\0'};
  uint8_t buf[MAX_BUF];
  //UART_putString(l);
  while(1) {
	  
    UART_getString(buf);
    //UART_putString((uint8_t*)"received\n");
    UART_putString(buf);
  }
	return 0;
}


// Definizione del comportamento del motore
ISR(TIMER5_COMPA_vect) {
	spin_once(mtr);
}


