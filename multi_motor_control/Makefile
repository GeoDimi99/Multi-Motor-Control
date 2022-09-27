# AVR makefile stub

# to clean a folder
# from command line
#$> make clean

# to build the folder
#$> make 

# to upload a file
# from command line
# $> make <main_filename>.hex

#
# remember to give yourself the read/write permissions on the
# serial line
# $> sudo addgroup <your username> dialout
# logout/login



# !!!!!!!!!!!!!!!! MODIFY HERE !!!!!!!!!!!!!!!! 

# put here the file containing the main() routine
# to be uploaded on the avr
# you can add multiple files, they will be all generated

BINS=   slave1.elf  slave2.elf  master.elf

# put here the additional .o files you want to generate
# one .mac file for each .o should be present
OBJS= TWI.o ../avr_common/motor_librarys/Motor.o ../avr_common/motor_librarys/digital_io.o ../avr_common/motor_librarys/encoder.o ../avr_common/motor_librarys/pwm_signal.o  ../avr_common/motor_librarys/timer.o ../avr_common/uart/uart_int.o

# put here the additional header files needed for compilation
HEADERS= TWI_lib.h  ../avr_common/motor_librarys/Motor.h ../avr_common/motor_librarys/digital_io.h ../avr_common/motor_librarys/encoder.h ../avr_common/motor_librarys/pwm_signal.h  ../avr_common/motor_librarys/timer.h ../avr_common/uart/uart_int.h


# the file below contains the actual rules

# !!!!!!!!!!!!!!!! MODIFY HERE !!!!!!!!!!!!!!!! 

include ../avr_common/avr_sara.mk 




