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
#include <linux/i2c-dev.h>

#include "I2Cclass.h"

#define TRUE 1
#define FALSE 0

#define ARDUINO_ADDRESS 0x11

//For RC inputs Scaling
#define THR_MIN 890
#define THR_MAX 1895
#define RC_MIN 1000
#define RC_MAX 2000
#define K_YAW 30
#define K_PITCH 20
#define K_ROLL 20

I2C Arduino(ARDUINO_ADDRESS);

I2C::I2C(uint8_t address)
{

 _devName = "/dev/i2c-1";
 _devAddr = address;

}

int I2C::sendInt(int data)
{

  //convert int to bytes[2]
  union Sharedblock
  {
    uint8_t b[2];
    int i;
  } data_u;

  data_u.i=data;

  return sendBytes(data_u.b,2);

}

int I2C::sendBytes(uint8_t data[], uint8_t length)
{
  int8_t count = 0;

  int fd = open(_devName, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
    return(FALSE);
  }

  if (ioctl(fd, I2C_SLAVE, _devAddr) < 0) {
    fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
    close(fd);
    return(FALSE);
    }

  count =  write(fd, data, length);


  if (count < 0) {
    fprintf(stderr, "Failed to write device(%d): %s\n", _devAddr, ::strerror(errno));
    close(fd);
    return(FALSE);
  } else if (count != length) {
    fprintf(stderr, "Short write to device, expected %d, got %d\n", length, count);
    close(fd);
    return(FALSE);
  }
  close(fd);

  return count;
}

int I2C::readBytes(uint8_t data[], uint8_t length)
{
  int8_t count = 0;

  int fd = open(_devName, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
    return(FALSE);
  }

  if (ioctl(fd, I2C_SLAVE, _devAddr) < 0) {
    fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
    close(fd);
    return(FALSE);
    }

  count =  read(fd, data, length);

  if (count < 0) {
    fprintf(stderr, "Failed to write device(%d): %s\n", _devAddr, ::strerror(errno));
    close(fd);
    return(FALSE);
  } else if (count != length) {
    fprintf(stderr, "Short write to device, expected %d, got %d\n", length, count);
    close(fd);
    return(FALSE);
  }
  close(fd);

  return count;
}



/*_________________________
  for Quadcopter

___________________________*/

int I2C::readRCinputs(float RCdata[], int num)
{

  uint8_t length=num*2 ;
  uint8_t *buf   =new uint8_t[length];

  union
  {
    uint16_t ui;
    uint8_t b[2];
  } data[4];


  int count = readBytes(buf,length);

  //use shared blocks to convert data
  for (int i=0;i<4;i++)
    {
    for (int ii=0;ii<2;ii++)
      {
  	data[i].b[ii] = buf[ii+2*i];
      }
    RCdata[i] = (float)data[i].ui;
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


int I2C::sendESCs(int data[], int num)
{

  //convert int to bytes[2]
  //declare array of unions and fill up
  union int_byt
  {
    uint8_t b[2];
    int i;
  } *data2 = new int_byt[num];

  for (int i=0; i<num; i++)
    {
      data2[i].i = data[i];
    }

  //convert to table of bytes
  uint8_t *buf = new uint8_t[2*num];
  for (int i=0; i<num; i++)
    {
      for (int ib=0; ib<2; ib++)
	{
	  buf[2*i+ib] = data2[i].b[ib];
	}
    }

  int count;
  do {
  count = sendBytes(buf, (uint8_t) 2*num);
    //if (count == 0 ) exit(0);
    usleep(1000);
  }while ( count !=  2*num );

}
