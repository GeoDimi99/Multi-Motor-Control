

#include "uart.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
//#include "../avr_common/uart.h"



//uint8_t rx_buffer[SIZE_RX_BUFFER];
//uint8_t rx_index = 0;
//uint8_t rd_index = 0;
						
// Buffer di trasmissione
//uint8_t tx_buffer[SIZE_TX_BUFFER];
//uint8_t tx_index = SIZE_TX_BUFFER - 1;
//uint8_t wr_index = 0;




// Definizione del comportamento del vettore dei interrupt, cosa deve fare l'ISR:

// - RX ISR per la UART:


void UART_init(void){
  
  // Settiamo baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;
  
  //Abilitiamo RX e TX con interrupt
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
  UCSR0B |= (1 << RXCIE0) | (1 << TXCIE0);

  //8-bit data
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); 
  
  //Settiamo le variabili del buffer
  rxHead = 0;
  rxTail = 0;
  txHead = 0;
  txTail = 0;	

}

uint8_t UART_getChar(void){
	
	//Settiamo le variabili locali
	uint8_t data;
	uint8_t tmptail;
	
	//Return se non c'è dati avviabili
	if(rxHead == rxTail) return 0;
	
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
	
	//Settiamo le variabili locali
	uint8_t tmphead;
	
	//Calcolo della nuova head
	tmphead = ((txHead + 1) & UART_BUFFER_MASK);
	
	//Attesa di spazio nel buffer
	while(tmphead == txTail);
	
	//Salva dati nel buffer
	txBuffer[tmphead] = data;
	txHead = tmphead;
	
	//Abilita UDRE interrupt to transmit data
	UCSR0B |= (1 << UDRIE0);
	
}

ISR(USART_RX_vect){
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

ISR(USART_UDRE_vect){
	
	uint8_t tmptail;

	if (txHead != txTail) {

		//Calcolo nuovo indice per il buffer
		tmptail = ((txTail + 1) & UART_BUFFER_MASK);
		txTail = tmptail;

		//Inserisci il buffer neSerial Bus
		UDR0 = txBuffer[tmptail];
	} else {

		//Disabilita UDR se non data availbale
		UCSR0B &= ~(1<<UDRIE0);
	}
}

// reads a string until the first newline or 0
// returns the size read
uint8_t UART_getString(uint8_t* buf){
  uint8_t* b0=buf; //beginning of buffer
  while(1){
    uint8_t c=UART_getChar();
    *buf=c;
    ++buf;
    // reading a 0 terminates the string
    if (c==0)
      return buf-b0;
    // reading a \n  or a \r return results
    // in forcedly terminating the string
    if(c=='\n'||c=='\r'){
      *buf=0;
      ++buf;
      return buf-b0;
    }
  }
}

void UART_putString(uint8_t* buf){
  while(*buf){
    UART_putChar(*buf);
    ++buf;
  }
}



//ISR(USART0_RX_vect) {
  //uint8_t c = UDR0;
  //rx_buffer[rx_index] = c;
  //rx_index = (rx_index + 1) % SIZE_RX_BUFFER;
  
//}

//// - TX ISR per la UART
//ISR(USART0_UDRE_vect) {

  //if(!TX_is_empty()){								//Controllo se il buffer è vuoto
	  //tx_index = (tx_index + 1) % SIZE_TX_BUFFER;   //Incremento l'indice
	  //UDR0 = tx_buffer[tx_index]; 					//Scrivo nella UART
  //}
  
//}



//// Definizione delle funzioni utilizzate per la UART:

//void UART_init(void){
  //// Set baud rate
  //UBRR0H = (uint8_t)(MYUBRR>>8);
  //UBRR0L = (uint8_t)MYUBRR;

  //UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); 				/* 8-bit data */
  //UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Abilitiamo RX e TX */

//}



//uint8_t UART_getChar(uint8_t* buf) {
  //// Attesa di recezione di un carattere
  ////printf("getChar\n");
  //if(!(UCSR0A & (1<<UDRE0))) return 0; 			// Ritorno 0 in caso insuccesso
  //printf("dopo\n");
  ////Lettura dal rx_buffer
  //*buf = rx_buffer[rd_index];		 			// Lettura dal buffer se è presente un valore
  //rd_index = (rd_index + 1) % SIZE_RX_BUFFER;   // Andiamo avanti con l'indice di lettura;
  //printf("esco\n");
  //return 1; 						 			// Ritorno 1 in caso successo
//}



//uint8_t UART_putChar(uint8_t* buf) {
 
	//if(TX_is_full()) return 0;					//Ritorno 0 in caso il buffer è pieno
	
	////Scrittura nel tx_buffer									
	//tx_buffer[wr_index] = *buf;					//Scrittura carattere
	//wr_index = (wr_index + 1) % SIZE_TX_BUFFER;	//Andiamo avanti con l'indice
  
	//return 1;									//Ritorno 1 in caso di successo
//}

