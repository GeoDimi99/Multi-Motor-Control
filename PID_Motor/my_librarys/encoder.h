#pragma once
#include <stdint.h>


//encoder_init: inizzailizzazione della lettura del encoder sui pin 53 e 52; 
void encoder_init(void);

//read_encoder: lettura dell'encoder;
uint16_t encoder_read(void);

