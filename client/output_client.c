#include "serial_linux.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  if (argc<3) {												//Controllo di errore mancanza di parametri <filename> <banda>
    printf("usage: %s <filename> <baudrate>", argv[0]);
    return 0;
  }
  char* filename=argv[1];
  int baudrate=atoi(argv[2]);
  printf( "opening serial device [%s] ... \n", filename);
  int fd=serial_open(filename);
  if (fd<=0) {
    printf ("Error\n");
    return 0;
  } else {
    printf ("Success\n");
  }
  printf( "setting baudrate [%d] ... \n", baudrate);
  int attribs=serial_set_interface_attribs(fd, baudrate, 0);
  if (attribs) {
    printf("Error\n");
    return 0;
  }

  serial_set_blocking(fd, 1);

  const int bsize=10;
  char buf[bsize];
  while (1) {
    int n_read=read(fd, buf, bsize);
    for (int i=0; i<n_read; ++i) {
      printf("%c", buf[i]);
    }
  }
}

