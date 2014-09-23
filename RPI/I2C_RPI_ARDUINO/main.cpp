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
    //get RC
    int count = I2CtoArduino.readRCinputs(RC);
    printf("throttle = %f, yaw = %f, pitch = %f , roll = %f \n",
    	   RC[0], RC[1], RC[2], RC[3]);

    usleep(100000);

    //send ESC values
    int ESC = 1200;
    union Sharedblock
    {
      uint8_t b[2];
      int i;
    } data;

    data.i = ESC;

    //I2CtoArduino.sendBytes(data.b,2);
    //I2CtoArduino.sendInt(ESC);

    //usleep(10000);

  }
  return (EXIT_SUCCESS);
}
