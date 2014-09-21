#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdint.h>

#include "I2Cclass.h"

// The Slave board i2c address
#define ADDRESS 0x04

int main(int argc, char** argv) {

  I2C I2CtoArduino(ADDRESS);

  float RC[4];

  while (true) {

  int count = I2CtoArduino.readFloats(RC,4);

  // printf("throttle = %f \n",RC[0]);
  // printf("count = %d \n",count);

  usleep(100000);
  }
  return (EXIT_SUCCESS);
}
