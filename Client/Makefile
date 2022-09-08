CC=gcc
CCOPTS=-std=gnu99
BINS=output_client input_client

.phony: clean all

all: $(BINS)

input_client:	input_client.c serial_linux.c
	$(CC) -$(CCOPTS) -o $@ $^

output_client:	output_client.c serial_linux.c
	$(CC) -$(CCOPTS) -o $@ $^

clean:
	rm -rf *~ $(BINS) *.pgm

