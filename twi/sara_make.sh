#!/bin/bash
#1. Cancellazione dei vecchi file oggetto
make clean
#2. Creazione dell master e dello slave 
cat ../avr_common/avr_mk_sara/master.txt > ../avr_common/avr_sara.mk && \
make && \ 
make master.hex && \
cat ../avr_common/avr_mk_sara/slave.txt > ../avr_common/avr_sara.mk && \
make && \
make slave.hex



