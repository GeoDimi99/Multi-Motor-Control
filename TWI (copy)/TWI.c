#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI_lib.h"
#include "util/delay.h"
#include <stdio.h>
#include "../avr_common/uart.h"

void TWI_Init(){
	TWI_info.mode = Ready;
	TWI_info.error_code = TWI_SUCCESS;
	TWI_info.repeated_start = 0;
	TWSR = 0;                                 //no prescaling 
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;     //setta il bit rate
	TWCR = (1 << TWIE) | (1 << TWEN);         //abilita TWI
}

void Slave_Addr_init(uint8_t addr, uint8_t brd){
	if (brd){
		TWAR= (addr << 1) | 0x01;
		TWI_Set_Address();
	}
	else{
		TWAR= (addr << 1) | 0x01;
		TWI_Set_Address();
	}
}

uint8_t is_TWI_ready(){
	if ( (TWI_info.mode == Ready) | (TWI_info.mode == Repeated_Start) ) return 1;
	else return 0;
}

uint8_t TWI_Transmit_Data(void *const TR_data, uint8_t data_len, uint8_t repeated_start){
	printf_init();
	if (data_len <= TRANSMIT_BUFLEN){
		while (!is_TWI_ready()) {_delay_us(1);}
		TWI_info.repeated_start = repeated_start;
		uint8_t *data = (uint8_t *)TR_data;
		for (int i = 0; i < data_len; i++){
			Transmit_Buffer[i] = data[i];
		}
		transmit_len = data_len;
		TB_Index = 0;
		 
		if (TWI_info.mode == Repeated_Start){
			TWI_info.mode = Initializing;
			TWDR = Transmit_Buffer[TB_Index++]; 
			TWI_Send_Transmit(); 
		}
		else{
			TWI_info.mode = Initializing;
			TWI_Send_Start();                           
		}
		
	}
	else{ return 1; }
	return 0;
}

uint8_t TWI_Read_Data(uint8_t TWI_addr, uint8_t bytes_to_read, uint8_t repeated_start){
	if (bytes_to_read < RECEIVE_BUFLEN){
		RB_Index = 0;
		receive_len = bytes_to_read;
		uint8_t TR_data[1];
		TR_data[0] = (TWI_addr << 1) | 0x01;
		TWI_Transmit_Data(TR_data, 1, repeated_start);
	}
	else{return 1;}
	return 0;
}

ISR (TWI_vect){
	
	switch (TWI_STATUS){
		
		case REP_START_TRANSMITTED:
			printf("REP_START_TRANSMITTED\n");
			TWI_info.mode=Repeated_Start;
		
		case START_TRANSMITTED:
			printf("START_TRANSMITTED\n");
			TWDR=Transmit_Buffer[TB_Index++]; 
			TWI_Send_Transmit();
			break;
			
		case SLAW_TR_ACK_RV:
			printf("SLAW_TR_ACK_RV\n");
			TWI_info.mode = Master_Transmitter;
			
		case SLAW_TR_NACK_RV:
			printf("SLAW_TR_NACK_RV\n");
		
		case M_DATA_TR_NACK_RV:
			printf("M_DATA_TR_NACK_RV\n");
			
		case M_DATA_TR_ACK_RV:
			printf("M_DATA_TR_ACK_RV\n");
		
			if (TB_Index < transmit_len){
				TWDR=Transmit_Buffer[TB_Index++];
				TWI_Send_Transmit();
			}
			else if (TWI_info.repeated_start){
				TWI_Send_Start();
			}
			else{
				TWI_Send_Stop();
			}	
			break;
		
		case SLAR_TR_NACK_RV:
			printf("SLAR_TR_NACK_RV\n");
		
		case ARBITRATION_LOST:
			printf("ARBITRATION_LOST\n");
		
			TWI_info.error_code = TWI_STATUS;
		
			if (TWI_info.repeated_start){
				TWI_Send_Start();
			}
			else{
				TWI_info.mode=Ready;
				TWI_Send_Stop();
			}	
			break;
		
		case SLAR_TR_ACK_RV:
			printf("SLAR_TR_ACK_RV\n");
			TWI_info.mode=Master_Receiver;
			if (RB_Index < receive_len -1){
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_ACK();
			}
			else{
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_NACK();
			}
			break;
		
		case DATA_RV_ACK_TR:
			printf("DATA_RV_ACK_TR\n");
			Receive_Buffer[RB_Index++]=TWDR;
			if (RB_Index < receive_len -1){
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_ACK();
			}
			else{
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_NACK();
			}
			break;
		
		case DATA_RV_NACK_TR:
			printf("DATA_RV_NACK_TR\n");
			Receive_Buffer[RB_Index++] = TWDR;
			TWI_info.error_code = TWI_SUCCESS;	
			if (TWI_info.repeated_start){
				TWI_Send_Start();
			}
			else{
				TWI_info.mode = Ready;
				TWI_Send_Stop();
			}
			break;
			
		case ARBITRATION_LOST_SR_ADDR:
			printf("ARBITRATION_LOST_SR_ADDR\n");
		
		case ARBITRATION_LOST_SR_BRD:
			printf("ARBITRATION_LOST_SR_BRD\n");
		
		case BRDW_RV_ACK_TR:
			printf("BRDW_RV_ACK_TR\n");
			
		case SLAW_RV_ACK_TR:
			printf("SLAW_RV_ACK_TR\n");
			TWI_info.mode=Slave_Receiver;
			if (RB_Index < receive_len -1){                     
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_ACK();
			}
			else{
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_NACK();
			}
			break;
			
		case DATA_BRD_RV_ACK_TR:
			printf("DATA_BRD_RV_ACK_TR\n");
		
		case DATA_SLA_RV_ACK_TR:
			printf("DATA_SLA_RV_ACK_TR\n");
			Receive_Buffer[RB_Index++] = TWDR; 
			if (RB_Index < receive_len-1){                
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_ACK();
			}
			else{
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_NACK();
			}
			break;
			
		case DATA_BRD_RV_NACK_TR:  //forse qui vanno inviati degli ACK/NACK?
			printf("DATA_BRD_RV_NACK_TR\n");
		
		case DATA_SLA_RV_NACK_TR:
			printf("DATA_SLA_RV_NACK_TR\n");
			Receive_Buffer[RB_Index++] = TWDR;
			TWI_info.error_code = TWI_SUCCESS;	
			if (TWI_info.repeated_start){}
			else{
				TWI_info.mode = Ready;
			}
			break;
			
		case S_DATA_TR_NACK_RV:
			printf("S_DATA_TR_NACK_RV\n");
		
		case S_LDATA_TR_ACK_RV:
			printf("S_LDATA_TR_ACK_RV\n");
		
		case START_STOP_FOR_SLAVE:     //forse qui vanno inviati degli ACK/NACK?
			printf("START_STOP_FOR_SLAVE\n");
			TWI_info.error_code = TWI_STATUS;	
			if (TWI_info.repeated_start){}
			else{
				TWI_info.mode = Ready;
			}
			break;
		
		case ARBITRATION_LOST_ST:
			printf("ARBITRATION_LOST_ST\n");
		
		case SLAR_RV_ACK_TR:
			printf("SLAR_RV_ACK_TR\n");
		
		case S_DATA_TR_ACK_RV:
			printf("S_DATA_TR_ACK_RV\n");
			TWI_info.mode=Slave_Transmitter;
			if (RB_Index < receive_len -1){                     
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_ACK();
			}
			else{
				TWI_info.error_code = TWI_NO_RELEVANT_INFO;
				TWI_Send_NACK();
			}
			break;
		
		case TWI_NO_RELEVANT_INFO:
			printf("TWI_NO_RELEVANT_INFO\n");
			break;
		
		case TWI_ILLEGAL_START_STOP:
			printf("TWI_ILLEGAL_START_STOP\n");
			TWI_Send_Stop();
			break;
	}
}

