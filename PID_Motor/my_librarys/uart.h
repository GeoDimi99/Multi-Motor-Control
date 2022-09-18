
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
#define TX_is_empty() ((tx_index + 1) % SIZE_TX_BUFFER == wr_index)
#define TX_is_full() ((wr_index + 1) % SIZE_TX_BUFFER == tx_index)
#define RX_is_empty() (rd_index == rx_index )

// Buffer di recezione
uint8_t rx_buffer[SIZE_RX_BUFFER];
uint8_t rx_index = 0;
uint8_t rd_index = 0;
						
// Buffer di trasmissione
uint8_t tx_buffer[SIZE_TX_BUFFER];
uint8_t tx_index = SIZE_TX_BUFFER - 1;
uint8_t wr_index = 0;


 

void UART_init(void);
uint8_t UART_putChar(uint8_t* buf);
uint8_t UART_getChar(uint8_t* buf);


