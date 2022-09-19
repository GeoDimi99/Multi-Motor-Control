
#include "uart_int.h"



void UART_init(void){
  
  //Disabilitiamo le interrupt
  cli();
  
  // Settiamo baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;
  
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
  
  //Abilitiamo RX e TX con interrupt
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
  UCSR0B |= (1 << RXCIE0); //|(1 << TXCIE0);

  //8-bit data
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); 
  
  //Settiamo le variabili
  rxHead = 0;
  rxTail = 0;

  //Abilitiamo le interrupt
  sei();

  

}


uint8_t UART_getChar(void){
	
	//Settiamo le variabili locali
	uint8_t data;
	uint8_t tmptail;
	
	//Return se non c'è dati avviabili
	//if(rxHead == rxTail) return 0
	while (rxHead == rxTail);
	
	// Calcolo lunghezza dati
	tmptail = ((rxTail + 1) & UART_BUFFER_MASK);
	rxTail = tmptail;
	
	//Prendi dati dal buffer
	data = rxBuffer[tmptail];
	
	//Abilita RX interrupt
	UCSR0B |= (1 << RXCIE0);
	
	return data;
}

void UART_putChar(uint8_t c){
  // wait for transmission completed, looping on status bit
  while ( !(UCSR0A & (1<<UDRE0)) );

  // Start transmission
  UDR0 = c;
}




// Legge una stringa finche non incontra un newline o 0
// ritorna il valore letto
uint8_t UART_getString(uint8_t* buf){
  uint8_t* b0=buf; //beginning of buffer
  while(1){
    uint8_t c=UART_getChar();
    *buf=c;
    ++buf;
    
    // Controlla se la stringa termina
    if (c==0)
      return buf-b0;
    // controlla \n  o  \r e return la lunghezza della stringa 
    if(c=='\n'||c=='\r'){
      *buf=0;
      ++buf;
      return buf-b0;
    }
  }
}

//Scrive una stringa
void UART_putString(uint8_t* buf){
  while(*buf){
    UART_putChar(*buf);
    ++buf;
  }
}

ISR(USART0_RX_vect){
	uint8_t data;
	uint8_t tmphead;
	
	data = UDR0;
	
	//Calcolo di un nuovo indice del buffer
	tmphead = ((rxHead + 1) & UART_BUFFER_MASK);
	
	// BUFFER OVERFLOW!
	if (tmphead == rxTail ) {

		// disable rx interrput
		UCSR0B &= ~(1<<RXCIE0);
		return;
	} else {

		// save new index
		rxHead = tmphead;

		// save data in buffer
		rxBuffer[tmphead] = data;
	}
}





