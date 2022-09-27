
#pragma once

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#define BAUD 19200
#define MYUBRR (F_CPU/16/BAUD-1)

#define UART_BUFFER_SIZE 32
#define UART_BUFFER_MASK (UART_BUFFER_SIZE - 1)


// Definizione dei buffer
static volatile unsigned char rxHead;
static volatile unsigned char rxTail;
static volatile unsigned char txHead;
static volatile unsigned char txTail;
static volatile unsigned char txBuffer[UART_BUFFER_SIZE];
static volatile unsigned char rxBuffer[UART_BUFFER_SIZE];



 

void UART_init(void);
void UART_putChar(uint8_t data);
uint8_t UART_getChar(void);
uint8_t UART_getString(uint8_t*);
void UART_putString(uint8_t*);


