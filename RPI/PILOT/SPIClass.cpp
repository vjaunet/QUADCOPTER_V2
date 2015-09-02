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
 _speed   = 1000000;
 _bits    = 8;
 _delay   = 0;
 _mode    = 0;

}

int SPI::initialize(){

  int ret = 0;
  _fd = open(_devName, O_RDWR);

  if (_fd < 0)
   printf("SPI initialize : can't open device\n");

  /*
   * spi mode
   */
  ret = ioctl(_fd, SPI_IOC_WR_MODE, &_mode);
  if (ret == -1)
    printf("SPI initialize : can't set spi mode\n");

  /*
   * bits per word
   */
  ret = ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &_bits);
  if (ret == -1)
    printf("SPI initialize : can't set bits per word\n");


  /*
   * max speed hz
   */
  ret = ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed);
  if (ret == -1)
    printf("SPI initialize : can't set max speed hz\n");


  printf("SPI initialization finished\n");
  return ret;

}


int SPI::writeByte(uint8_t byteSent)
{
  int ret;

  uint8_t tx[1] = {byteSent};
  uint8_t rx[ARRAY_SIZE(tx)] = {0,};

  struct spi_ioc_transfer tr;
  tr.tx_buf = (unsigned long)tx;
  tr.rx_buf = (unsigned long)rx;
  tr.len = ARRAY_SIZE(tx);
  tr.delay_usecs = _delay;
  tr.speed_hz = _speed;
  tr.bits_per_word = _bits;

  ret = ioctl(_fd, SPI_IOC_MESSAGE(1), &tr);
  if (ret < 1) {
    printf("SPI writeByte : can't send spi message\n");
  }

  return ret;

}


uint8_t SPI::readByte()
{
  uint8_t tx[1] = {0};
  uint8_t rx[1] = {0};

  struct spi_ioc_transfer msg;
  msg.tx_buf = (unsigned long)tx;
  msg.rx_buf = (unsigned long)rx;
  msg.len = (uint32_t) 1;
  msg.delay_usecs = _delay;
  msg.speed_hz = _speed;
  msg.bits_per_word = _bits;

  int ret = ioctl(_fd, SPI_IOC_MESSAGE(1), &msg);
  if (ret < 1) {
    printf("SPI readByte : can't get spi message\n");
    return 0;
  }

  return rx[0];

}

uint8_t SPI::rwByte(uint8_t byteSent)
{
  uint8_t tx[1] = {byteSent};
  uint8_t rx[1] = {0};

  struct spi_ioc_transfer msg;
  msg.tx_buf = (unsigned long)tx;
  msg.rx_buf = (unsigned long)rx;
  msg.len = (uint32_t) 1;
  msg.delay_usecs = _delay;
  msg.speed_hz = _speed;
  msg.bits_per_word = _bits;

  int ret = ioctl(_fd, SPI_IOC_MESSAGE(1), &msg);
  if (ret < 1) {
    printf("SPI readByte : can't get spi message\n");
    return 0;
  }

  return rx[0];
}
