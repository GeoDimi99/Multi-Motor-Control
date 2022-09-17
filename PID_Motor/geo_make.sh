#!/bin/bash
#1. Cancellazione dei vecchi file oggetto
make clean
#2. Creazione dell file motore
make && \ 
make motor_tester.hex 



