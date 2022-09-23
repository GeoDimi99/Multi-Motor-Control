
#include "uart_int.h"



void UART_init(void){
  
  //Disabilitiamo le interrupt
  cli();
  
  // Settiamo baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;
  
  //Abilitiamo RX e TX con interrupt
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
  UCSR0B |= (1 << RXCIE0)| (1 << TXCIE0);

  //8-bit data
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); 
  
  //Settiamo le variabili
  rxHead = 0;
  rxTail = 0;
  txHead = 0;
  txTail = 0;

  //Abilitiamo le interrupt
  sei();

  

}


uint8_t UART_getChar(void){
	
	//Settiamo le variabili locali
	uint8_t data;
	uint8_t tmptail;
	
	//Return se non c'è dati avviabili
	//if(rxHead == rxTail) return 0;
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


void UART_putChar(uint8_t data){
	// set local variables
	unsigned char tmphead;

	// calculate new head
	tmphead = ((txHead + 1) & UART_BUFFER_MASK);

	// wait until space in buffer
	while(tmphead == txTail);

	// save data in buffer
	txBuffer[tmphead] = data;
	txHead = tmphead;

	// enable UDRE interrupt to transmit data
	UCSR0B |= (1 << UDRIE0);
	
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
      *(buf-1)=0;
      UART_getChar();
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

ISR(USART0_UDRE_vect){
	unsigned char tmptail;

	if (txHead != txTail) {

		// calculate new buffer
		tmptail = ((txTail + 1) & UART_BUFFER_MASK);
		txTail = tmptail;

		// put buffer to Serial Bus
		UDR0 = txBuffer[tmptail];
	} else {

		// disable UDR if no data availbale
		UCSR0B &= ~(1<<UDRIE0);
	}
}






