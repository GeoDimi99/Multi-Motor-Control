

#include "uart.h"

//| | | | |  | | buffer di input '\0'
//^            ^
//rd_index	rx_index   se rd_index == rx_index --> buffer vuoto


//| | | | | | | buffer di output  rd_index = (rd_index+1) % len
//           ^^                
//wr_index   tx_index  



// Definizione del comportamento del vettore dei interrupt, cosa deve fare l'ISR:

// - RX ISR per la UART:
ISR(USART0_RX_vect) {
  uint8_t c = UDR0;
  rx_buffer[rx_index] = c;
  rx_index = (rx_index + 1) % SIZE_RX_BUFFER;
  
}

// - TX ISR per la UART
ISR(USART0_UDRE_vect) {

  if(!TX_is_empty()){								//Controllo se il buffer è vuoto
	  tx_index = (tx_index + 1) % SIZE_TX_BUFFER;   //Incremento l'indice
	  UDR0 = tx_buffer[tx_index]; 					//Scrivo nella UART
  }
  
}



// Definizione delle funzioni utilizzate per la UART:

void UART_init(void){
  // Set baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;
  
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Abilitiamo le interruzioni per la UART per TX e RX*/

  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); 				/* 8-bit data */
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Abilitiamo RX e TX */

}



uint8_t UART_getChar(uint8_t* buf) {
  // Attesa di recezione di un carattere
  if(RX_is_empty()) return 0; 			// Ritorno 0 in caso insuccesso
  
  //Lettura dal rx_buffer
  *buf = rx_buffer[rd_index];		 			// Lettura dal buffer se è presente un valore
  rd_index = (rd_index + 1) % SIZE_RX_BUFFER;   // Andiamo avanti con l'indice di lettura;
  
  return 1; 						 			// Ritorno 1 in caso successo
}



uint8_t UART_putChar(uint8_t* buf) {
 
	if(TX_is_full()) return 0;					//Ritorno 0 in caso il buffer è pieno
	
	//Scrittura nel tx_buffer									
	tx_buffer[wr_index] = *buf;					//Scrittura carattere
	wr_index = (wr_index + 1) % SIZE_TX_BUFFER;	//Andiamo avanti con l'indice
  
	return 1;									//Ritorno 1 in caso di successo
}

