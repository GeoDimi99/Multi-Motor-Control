
#pragma once

#include <avr/io.h>
#include <stdint.h>



void DigIO_init(void);
void DigIO_REGE_setDirection(uint8_t bit, uint8_t dir);
void DigIO_REGE_setValue(uint8_t bit, uint8_t val);

