
#pragma once

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>





#define BAUD 19200
#define MYUBRR (F_CPU/16/BAUD-1)
#define SIZE_RX_BUFFER 20
#define SIZE_TX_BUFFER 20


// Buffer di recezione


#define TX_is_empty() ((tx_index + 1) % SIZE_TX_BUFFER == wr_index)
#define TX_is_full() ((wr_index + 1) % SIZE_TX_BUFFER == tx_index)
#define RX_is_empty() (rd_index == rx_index )


 

void UART_init(void);
uint8_t UART_putChar(uint8_t* buf);
uint8_t UART_getChar(uint8_t* buf);


