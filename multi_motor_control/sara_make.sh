#!/bin/bash
#1. Cancellazione dei vecchi file oggetto
make clean
#2. Creazione dell master e dello slave 
cat ../avr_common/avr_mk_sara/master.txt > ../avr_common/avr_sara.mk && \
make && \ 
make master.hex && \
cat ../avr_common/avr_mk_sara/slave1.txt > ../avr_common/avr_sara.mk && \
make && \
make slave1.hex && \
cat ../avr_common/avr_mk_sara/slave2.txt > ../avr_common/avr_sara.mk && \
make && \
make slave2.hex



