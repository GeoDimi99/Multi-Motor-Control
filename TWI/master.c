#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWIlib.h"
#include "util/delay.h"
//#include "FDCC_LCD.h"

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


int main(int argc, char** argv) {
	//commento per fare il commit
	return 0;
}
