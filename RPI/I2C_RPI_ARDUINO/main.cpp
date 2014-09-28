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
  int ESC[4];
  while (true) {

    int count;

    //--------------------------------------
    //get RC
    count = I2CtoArduino.readRCinputs(RC);
    printf("throttle = %4.2f, yaw = %4.2f, pitch = %4.2f , roll = %4.2f \n",
    	   RC[0], RC[1], RC[2], RC[3]);

    //    usleep(100000);

    //--------------------------------------
    //send ESC values
    for (int i=0;i<4;i++){
      ESC[i] = (int) RC[i] + 1500;
    }

   count = I2CtoArduino.sendESCs(ESC,4);
   printf("Send ESC count = %d \n", count);

   usleep(100000);

  }
  return (EXIT_SUCCESS);
}
