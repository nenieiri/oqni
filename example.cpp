#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

int main() {

  int serial_port = open("/dev/rdisk3s1", O_RDWR);

  if (serial_port < 0) {
      printf("Error opening: %s", strerror(errno));
      return 1;
  }

  char read_buf [256];
  memset(&read_buf, '\0', sizeof(read_buf));

  int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

  if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
      return 1;
  }

  printf("Read %i bytes. Received message: %s", num_bytes, read_buf);

  close(serial_port);
  return (0);
};
