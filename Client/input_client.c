#include "serial_linux.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char** argv) {
  if (argc<3) {												//Controllo di errore mancanza di parametri <filename> <banda>
    printf("usage: %s <filename> <baudrate>", argv[0]);
    return 0;
  }
  char* filename=argv[1];
  int baudrate=atoi(argv[2]);
  printf( "opening serial device [%s] ... ", filename);
  int fd=serial_open(filename);
  if (fd<=0) {
    printf ("Error\n");
    return 0;
  } else {
    printf ("Success\n");
  }
  printf( "setting baudrate [%d] ... ", baudrate);
  int attribs=serial_set_interface_attribs(fd, baudrate, 0);
  if (attribs) {
    printf("Error\n");
    return 0;
  }
  else{ printf("\n"); }

  serial_set_blocking(fd, 1);

  const int bsize=50;
  char buf[bsize];
  while(1){
    printf("Insert the desired velocity [<n> expressed in rad/sec], 'quit' to exit: ");
    scanf("%s", &buf);
    if (strcmp(buf, "quit")==0) break;
    buf[strlen(buf)+1]='\0';
    buf[strlen(buf)]='\n';
	if (strcmp(buf, "quit")==0) break;
  	ssize_t n_write= write(fd, buf, strlen(buf) + 1);
    printf("\n");
  }
  close(fd);
  return 0;
}
