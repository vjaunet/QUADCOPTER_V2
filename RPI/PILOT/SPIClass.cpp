#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "SPIClass.h"

//For RC inputs Scaling
#define THR_MIN 890
#define THR_MAX 1895
#define RC_MIN 1000
#define RC_MAX 2000
#define K_YAW 30
#define K_PITCH 20
#define K_ROLL 20

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

SPI ArduSPI;

SPI::SPI()
{

 _devName = "/dev/spidev0.0";

}

int SPI::transferBytes(uint8_t byteSent[], uint8_t byteRecv[] ,uint8_t length)
{
  int ret;

  uint8_t rx[ARRAY_SIZE(tx)] = {0, };
  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)tx,
    .rx_buf = (unsigned long)rx,
    .len = ARRAY_SIZE(tx),
    .delay_usecs = delay,
    .speed_hz = speed,
    .bits_per_word = bits,
  };

  return count;
}

/*_________________________


  for Quadcopter

___________________________*/

int SPI::transferRC(float RCdata[], int numRC, int ESC[], int numESC )
{
  union
  {
    uint32_t b;
    float    f;
  } tx[numRC],rx[numESC];

  int count = 0;

  //use shared blocks to convert data
  for (int i=0;i<numRC;i++)
    {
      RCdata[i] = rx[i].f;
    }

  for (int i=0;i<numESC;i++)
    {
      tx[i].b = (unsigned long)ESC[i];
    }

  //convert into PID usable values
  RCdata[0] = (RCdata[0] - THR_MIN)/(THR_MAX-THR_MIN) * 100.0;
  RCdata[1] = (RCdata[1] -(RC_MAX+RC_MIN)/2.) /
    (RC_MAX-RC_MIN) * K_YAW;
  RCdata[2] = (RCdata[2] -(RC_MAX+RC_MIN)/2.)/
    (RC_MAX-RC_MIN) * K_PITCH;
  RCdata[3] = (RCdata[3] -(RC_MAX+RC_MIN)/2.)/
    (RC_MAX-RC_MIN) * K_ROLL;

  return count;
}
