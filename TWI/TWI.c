#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWIlib.h"
#include "util/delay.h"

void TWI_Init()
{
	TWI_info.mode = Ready;
	TWI_info.error_code = 0xFF;
	TWI_info.repeated_start = 0;
	TWSR = 0;                                 //no prescaling 
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;     //setta il bit rate
	TWCR = (1 << TWIE) | (1 << TWEN);         //abilita TWI
}

uint8_t is_TWI_ready(){
	if ( (TWI_info.mode == Ready) | (TWI_info.mode == Repeated_Start) ) return 1;
	else return 0;
}

uint8_t TWI_Transmit_Data(void *const TR_data, uint8_t data_len, uint8_t repeated_start){
	if (data_len <= TRANSMIT_BUFLEN){
		while (!is_TWI_ready()) {_delay_us(1);}       //aspetta finché TWI non è pronto
		TWI_info.repeated_start = repeated_start;     //imposta se la trasmissione sta avvenendo a seguito di una repeated start
		uint8_t* data = (uint8_t*)TR_data;            
		for (int i = 0; i < data_len; i++){
			Transmit_Buffer[i] = data[i];             //copia i dati nel buffer di trasmissione
		}
		transmit_len = data_len;                      //dichiara la lunghezza dei dati da trasmettere
		TB_Index = 0;                                 //inizializza index del buffer di trasmissione
		
		if (TWI_info.mode == Repeated_Start){         //se la trasmissione avviene con repeated start non è necessario inviare la condizione di start
			TWI_info.mode = Initializing;
			TWDR = Transmit_Buffer[TB_Index++];       //inserisce il dato da trasmettere nel data register
			TWI_Send_Transmit();                      // Invia il dato
		}
		else{                                         //se la trasmissione avviene per la prima volta, si manda la condizione di start
			TWI_info.mode = Initializing;
			TWI_Send_Start();
		}
		
	}
	else{ 
		perror("Error: data_len must be <= TRANSMIT_BUFLEN");
		return 1; 
	}
	return 0;
}

ISR (TWI_vect)
{
	switch (TWI_STATUS)
	{
		// ----\/ ---- MASTER TRANSMITTER OR WRITING ADDRESS ----\/ ----  //
		
		case TWI_MT_SLAW_ACK:                                        //è stato trasmesso l'indirizzo dello slave+ il bit W, si è ricevuto l'ACK              
			TWI_info.mode = Master_Transmitter;                     //setta la modalità: Master_Trasmitter
			//TWI_Send_Start() *****da inserire???????****
		case TWI_START_SENT:   //*****siamo sicuri che non vada messa al primo posto???*****  //è stata inviata una condizione di start              
		case TWI_MT_DATA_ACK:                                      // un byte è stato trasmesso, si è ricevuto l'ACK
			if (TB_Index < transmit_len){                            // se ci sono altri byte da inviare, si predismpone per farlo
				TWDR = Transmit_Buffer[TB_Index++];                 // carica il dato nel buffer
				TWI_Send_Transmit();                                // invia il dato
			}
			// This transmission is complete however do not release bus yet
			else if (TWI_info.repeated_start){                       //se tutti i byte sono stati inviati e si vuole eseguire una repeated start
				TWI_info.error_code = TWI_SUCCESS;                    //dichiara il successo della trasmissione
				//TWI_info.mode= Repeated_Start; *****da inserire???????****
				TWI_Send_Start();
			}
			else{                                                    //altrimenti si vuole concludere la trasmissione
				TWI_info.mode = Ready;                               //dichiara che TWI è pronto
				TWI_info.error_code = TWI_SUCCESS;                   //dichiara il successo della trasmissione
				TWI_Send_Stop();
			}
			break;
		
		// ----\/ ---- MASTER RECEIVER ----\/ ----  //
		
		case TWI_MR_SLAR_ACK:                                         //è stato trasmesso l'indirizzo dello slave+ il bit R, si è ricevuto l'ACK
				TWI_info.mode = Master_Receiver;                      //setta la modalità: Master receiver
			if (RB_Index < receive_len-1){                           // se è rimasto più di un byte da leggere 
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;      
				TWI_Send_ACK();                                      //invia un ACK
			}
			else{                                                     //se è stato ricevuto uno ed un solo byte
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;          
				TWI_Send_NACK();                                      //*******perché??????******
			}
			break;
		
		case TWI_MR_DATA_ACK:                                         //è stato ricevuto il dato, l'ACK è stato inviato

			/// -- HANDLE DATA BYTE --- ///
			Receive_Buffer[RB_Index++] = TWDR;
			if (RB_Index < receive_len-1){                            //se c'è più di un byte da leggere, ricevi il byte e manda l'ACK
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_ACK();
			}
			else{                                                     //altrimenti se è stato ricevuto uno ed un solo byte
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_NACK();                                      //*******perché??????******
			}
			break;
		
		case TWI_MR_DATA_NACK:                                        // è stato ricevuto l'ultimo dato, il NACK è stato trasmesso
		
			/// -- HANDLE DATA BYTE --- ///
			Receive_Buffer[RB_Index++] = TWDR;	
			if (TWI_info.repeated_start){                             //verifico se avverrà un'ulteriore trasmissione
				TWI_info.error_code = TWI_SUCCESS;
				TWI_Send_Start();
			}
			else{
				TWI_info.mode = Ready;                                //dichiaro che TWI è pronto
				TWI_info.error_code = TWI_SUCCESS;                    //dichiaro che è avvenuto 
				TWI_Send_Stop();
			}
			break;
		
		// ----\/ ---- MT and MR common ----\/ ---- //
		
		case TWI_MR_SLAR_NACK:                                      // SLA+R transmitted, NACK received
		case TWI_MT_SLAW_NACK:                                      // SLA+W transmitted, NACK received
		case TWI_MT_DATA_NACK:                                      // Data byte has been transmitted, NACK received
		case TWI_LOST_ARBIT:                                        // Arbitration has been lost
			// Return error and send stop and set mode to ready
			if (TWI_info.repeated_start){				
				TWI_info.error_code = TWI_STATUS;
				TWI_Send_Start();
			}
			// All transmissions are complete, exit
			else{
				TWI_info.mode = Ready;
				TWI_info.error_code = TWI_STATUS;
				TWI_Send_Stop();
			}
			break;
		case TWI_REP_START_SENT: // Repeated start has been transmitted
			// Set the mode but DO NOT clear TWINT as the next data is not yet ready
			TWI_info.mode = Repeated_Start;
			break;
		
		// ----\/ ---- SLAVE RECEIVER ----\/ ----  //
		
		// TODO  IMPLEMENT SLAVE RECEIVER FUNCTIONALITY
		
		// ----\/ ---- SLAVE TRANSMITTER ----\/ ----  //
		
		// TODO  IMPLEMENT SLAVE TRANSMITTER FUNCTIONALITY
		
		// ----\/ ---- MISCELLANEOUS STATES ----\/ ----  //
		case TWI_NO_RELEVANT_INFO: // It is not really possible to get into this ISR on this condition
								   // Rather, it is there to be manually set between operations
			break;
		case TWI_ILLEGAL_START_STOP: // Illegal START/STOP, abort and return error
			TWI_info.error_code = TWI_ILLEGAL_START_STOP;
			TWI_info.mode = Ready;
			TWI_Send_Stop();
			break;
	}
	
}

