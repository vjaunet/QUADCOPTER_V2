/*

  SPI communication class


*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "SPIClass.h"

//RC inputs Scaling values
#define THR_MIN 890
#define THR_MAX 1895
#define RC_MIN 1000
#define RC_MAX 2000

// maximum angle values to be output
#define K_YAW 30
#define K_PITCH 20
#define K_ROLL 20

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

SPI ArduSPI;

SPI::SPI()
{

 _devName = "/dev/spidev0.0";
 _speed = 4000000;
 _bits = 8;
 _delay = 0;
 _mode = 0;

}

int SPI::initialize(){

  int ret = 0;
  _fd = open(_devName, O_RDWR);

  if (_fd < 0)
   printf("can't open device");

  /*
   * spi mode
   */
  ret = ioctl(_fd, SPI_IOC_WR_MODE, &_mode);
  if (ret == -1)
    printf("can't set spi mode");

  /*
   * bits per word
   */
  ret = ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &_bits);
  if (ret == -1)
    printf("can't set bits per word");


  /*
   * max speed hz
   */
  ret = ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed);
  if (ret == -1)
    printf("can't set max speed hz");

  return ret;

}

int SPI::transferBytes(uint8_t byteSent[], uint8_t byteRecv[])
{
  int ret;

  struct spi_ioc_transfer msg;
  msg.tx_buf = (unsigned long)byteSent;
  msg.rx_buf = (unsigned long)byteRecv;
  msg.len = ARRAY_SIZE(byteSent);
  msg.delay_usecs = _delay;
  msg.speed_hz = _speed;
  msg.bits_per_word = _bits;

  ret = ioctl(_fd, SPI_IOC_MESSAGE(1), &msg);
  if (ret < 1)
    printf("can't send spi message\n");

  return ret;
}

/*_________________________


  for Quadcopter

___________________________*/

int SPI::transferRC(float RCdata[], int ESC[])
{

  int numRC = ARRAY_SIZE(RCdata);
  int numESC = numRC;

  union
  {
    uint16_t ui;
    uint8_t b[2];
    float    f;
  } tx[numRC],rx[numESC];

  int count = 0;

  //use shared blocks to convert data
  for (int i=0;i<numESC;i++)
    {
      tx[i].ui = (unsigned long)ESC[i];
    }

  //transfert data through spi
  for (int i=0;i<numRC;i++){
    for (int ibyte=0; ibyte<2; ibyte++){
      transferBytes(tx[i].b, rx[i].b);
    }
    printf("%u %u %u %u\n",rx[i].ui);
  }

  //use shared blocks to convert data
  for (int i=0;i<numRC;i++)
    {
      RCdata[i] = rx[i].f;
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
