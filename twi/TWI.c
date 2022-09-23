#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI_lib.h"
#include "util/delay.h"
#include <stdio.h>
#include "util/atomic.h"
#include "../avr_common/uart.h"

void TWI_Init(){
	TWI_info.mode = Ready;
	TWI_info.error_code = TWI_SUCCESS;
	TWI_info.repeated_start = 0;
	TWSR = 0;                                 //no prescaling 
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;     //setta il bit rate
	TWCR = (1 << TWIE) | (1 << TWEN);         //abilita TWI
	TB_Index=0;                               //valore iniziale TB_Index (per sicurezza, consizìderare di togliere)
	RB_Index=0; 							  //valore iniziale RB_Index  (per sicurezza, considerare di toglierla)
}

void Slave_Addr_init(uint8_t SL_addr, uint8_t brd){
	TWI_Init(); 
	if (brd){
		TWAR= (SL_addr << 1) | 0x01;           //LSB=1 per il riconoscimento del broadcast
	}
	else {
		TWAR= (SL_addr << 1) | 0x00;
	}
}


uint8_t is_TWI_ready(){
	uint8_t is_ready; 
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
    is_ready = (TWI_info.mode == Ready);
	}
	return is_ready;
}

uint8_t TWI_Transmit_Data(void *const TR_data, uint8_t data_len, uint8_t repeated_start){
	if (data_len <= TRANSMIT_BUFLEN){
		while (!is_TWI_ready() ) { 
			_delay_us(1);
		}
		TWI_info.mode = Initializing;             
		TWI_info.repeated_start = repeated_start;
		uint8_t *data = (uint8_t *)TR_data;
		for (int i = 0; i < data_len; i++){
			Transmit_Buffer[i] = data[i];
		}
		transmit_len = data_len;
		TB_Index = 0;
		if (TWI_info.mode == Repeated_Start){
			TWDR = Transmit_Buffer[TB_Index++]; 
			TWI_Send_Transmit(); 
		}
		else{
			TWI_Send_Start();
		}
		while (!is_TWI_ready()){
			_delay_us(1);
		} 
	}
	else{ return 1; }
	return 0;
}

uint8_t TWI_Read_Data(uint8_t SL_addr, uint8_t bytes_to_read, uint8_t repeated_start){
	if (bytes_to_read < RECEIVE_BUFLEN){
		while (!is_TWI_ready()) { 
			_delay_us(1);
		}
		RB_Index = 0;
		receive_len = bytes_to_read;
		uint8_t TR_data[1];
		TR_data[0] = (SL_addr << 1) | 0x01;                           //modalità read
		TWI_Transmit_Data(TR_data, 1, repeated_start);
	}
	else return 1;
	return 0;
}

uint8_t TWI_Slave_Transmit_Data(void *const TR_data, uint8_t data_len){
	if (data_len <= TRANSMIT_BUFLEN){
		while (!is_TWI_ready()) { 
			_delay_us(1);
		}
		TWI_info.mode = Initializing;
		TB_Index=0;
		transmit_len=data_len;
		TWI_Set_Address();
		uint8_t *data = (uint8_t *)TR_data;
		for (int i =0; i< data_len; i++){
			Transmit_Buffer[i]= data[i];
		}
		while (!is_TWI_ready()) { 
			_delay_us(1);
		}
	}
	else return 1; 
	return 0; 
}

uint8_t TWI_Slave_Receive_Data(){ 
	while (!is_TWI_ready()) {
		_delay_us(1);
	} 
	TWI_info.mode=Initializing;
	RB_Index=0;
	TWI_Set_Address(); 
	while (!is_TWI_ready()) { 
		_delay_us(1);
	}
	return 0; 
}

ISR (TWI_vect){
	
	switch (TWI_STATUS){
		
		case REP_START_TRANSMITTED:
			TWI_info.mode=Repeated_Start;
		
		case START_TRANSMITTED:
			TWDR=Transmit_Buffer[TB_Index++]; 
			TWI_Send_Transmit();
			break;
			
		case SLAW_TR_ACK_RV:
			TWI_info.mode = Master_Transmitter;
			
		case M_DATA_TR_ACK_RV:
		
			if (TB_Index < transmit_len){
				TWDR=Transmit_Buffer[TB_Index++];
				TWI_Send_Transmit();
			}
			else if (TWI_info.repeated_start){
				TWI_Send_Start();
			}
			else{
				TWI_Send_Stop();
				TWI_info.error_code=TWI_SUCCESS;
				TWI_info.mode=Ready;
			}	
			break;
			
		case SLAW_TR_NACK_RV:
		
		case M_DATA_TR_NACK_RV:
			
		case SLAR_TR_NACK_RV:
		
		case ARBITRATION_LOST:
			
			if (TWI_info.mode == Initializing){
				TB_Index--; 
				TWI_Send_Start();
				break;
			}
		
			TWI_info.error_code = TWI_STATUS;
		
			if (TWI_info.repeated_start){
				TWI_Send_Start();
			}
			else{
				TWI_Send_Stop();
				TWI_info.mode=Ready;
			}	
			break;
		
		case SLAR_TR_ACK_RV:
			TWI_info.mode=Master_Receiver;
			TWI_info.error_code = TWI_NO_RELEVANT_INFO;
			if (RB_Index < receive_len -1){ 
				TWI_Send_ACK();
			}
			else{
				TWI_Send_NACK();
			}
			break;
		
		case DATA_RV_ACK_TR:
			Receive_Buffer[RB_Index++]=TWDR;
			TWI_info.error_code = TWI_NO_RELEVANT_INFO;
			if (RB_Index < receive_len -1){
				TWI_Send_ACK();
			}
			else{
				TWI_Send_NACK();
			}
			break;
		
		case DATA_RV_NACK_TR:
			Receive_Buffer[RB_Index++] = TWDR;
			TWI_info.error_code = TWI_SUCCESS;	
			if (TWI_info.repeated_start){
				TWI_Send_Start();
			}
			else{
				TWI_Send_Stop();
				TWI_info.mode = Ready;
			}
			break;
			
		case ARBITRATION_LOST_SR_ADDR:
		
		case ARBITRATION_LOST_SR_BRD:
		
		case BRDW_RV_ACK_TR:
			
		case SLAW_RV_ACK_TR:
			TWI_info.mode=Slave_Receiver;
			TWI_info.error_code = TWI_NO_RELEVANT_INFO;
			if (RB_Index < RECEIVE_BUFLEN -1){                     
				TWI_Send_ACK();
			}
			else{
				TWI_Send_NACK();
			}
			break;
			
		case DATA_BRD_RV_ACK_TR:
		
		case DATA_SLA_RV_ACK_TR:
			Receive_Buffer[RB_Index++] = TWDR; 
			TWI_info.error_code = TWI_NO_RELEVANT_INFO;
			if (RB_Index < RECEIVE_BUFLEN-1){                
				TWI_Send_ACK();
			}
			else{
				TWI_Send_NACK();
			}
			break;
			
		case DATA_BRD_RV_NACK_TR: 
		
		case DATA_SLA_RV_NACK_TR:
			Receive_Buffer[RB_Index++] = TWDR;
			TWI_info.error_code = TWI_SUCCESS;	
			if (TWI_info.repeated_start){}
			else{
				TWI_info.mode = Ready;
			}
			break;
			
		case S_DATA_TR_NACK_RV:
		
		case S_LDATA_TR_ACK_RV:
		
		case START_STOP_FOR_SLAVE:
			TWI_info.error_code = TWI_SUCCESS;
			TWI_Send_NACK();
			if (TWI_info.repeated_start){}
			else{
				TWI_info.mode = Ready;
			}
			break;
		
		case ARBITRATION_LOST_ST:
		
		case SLAR_RV_ACK_TR:
			TWI_info.mode=Slave_Transmitter;
			TWI_Set_Address();
		
		case S_DATA_TR_ACK_RV:
			TWDR= Transmit_Buffer[TB_Index++]; 
			TWI_info.error_code = TWI_NO_RELEVANT_INFO;
			if (TB_Index < transmit_len){                    
				TWI_Send_ACK();
			}
			else{
				TWI_Send_NACK();
			}
			break;
		
		case TWI_NO_RELEVANT_INFO:
			break;
		
		case TWI_ILLEGAL_START_STOP:
			TWI_Send_Stop();
			TWI_info.mode=Ready;
			break;
	}
}

