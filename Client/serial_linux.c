#include "serial_linux.h"
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int serial_set_interface_attribs(int fd, int speed, int parity) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) {					//Copia i parametri del fd e li mette in tty
    printf ("error %d from tcgetattr", errno);
    return -1;
  }
  switch (speed){									//Specifica la banda
  case 19200:
    speed=B19200;
    break;
  case 57600:
    speed=B57600;
    break;
  case 115200:
    speed=B115200;
    break;
  case 230400:
    speed=B230400;
    break;
  case 576000:
    speed=B576000;
    break;
  case 921600:
    speed=B921600;
    break;
  default:
    printf("cannot sed baudrate %d\n", speed);
    return -1;
  }
  cfsetospeed (&tty, speed);							//Setta la velocita di output
  cfsetispeed (&tty, speed);							//Setta la velocita di input
  cfmakeraw(&tty);										//Setta i parametri di input/output del terminale
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);               // Disabilità il bit di parità e disparità
  tty.c_cflag |= parity;
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;      // Imposta 8-bit chars

  if (tcsetattr (fd, TCSANOW, &tty) != 0) {		  // Setta gli attributi del fd immediatamente
    printf ("error %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}

void serial_set_blocking(int fd, int should_block) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) {
      printf ("error %d from tggetattr", errno);
      return;
  }

  tty.c_cc[VMIN]  = should_block ? 1 : 0;  				//Dichiara il numero minimo di caratteri da leggere
  tty.c_cc[VTIME] = 5;            						//Aspetta al massimo 0.5 seconds per read

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
    printf ("error %d setting term attributes", errno);
}

int serial_open(const char* name) {
  int fd = open (name, O_RDWR | O_NOCTTY | O_SYNC );		//Apre la seriale in lettura/scrittura ...
  if (fd < 0) {
    printf ("error %d opening serial, fd %d\n", errno, fd);
  }
  return fd;
}
