CC=gcc
CCOPTS=-std=gnu99
BINS=output_client

.phony: clean all

all: $(BINS)


output_client:	output_client.c serial_linux.c 
	$(CC) -$(CCOPTS) -o $@ $^  

clean:
	rm -rf *~ $(BINS) *.pgm

