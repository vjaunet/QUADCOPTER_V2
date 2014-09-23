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

#define _RCInputsNum 4;

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
    fprintf(stderr, "Failed to write device(%d): %s\n", count, ::strerror(errno));
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

int I2C::readFloat(float &read_data)
{

  uint8_t length=1;
  int8_t count = 0;

  union Sharedblock
  {
    uint8_t b[4]; // utiliser char parts[4] pour port série
    float f;
  } data;

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

  //  count = read(fd, data.b, length);
  count = read(fd, data.b, length);

  if (count < 0) {
    fprintf(stderr, "Failed to read device(%d): %s\n", count, ::strerror(errno));
    close(fd);
    return(-1);
  } else if (count != length) {
    fprintf(stderr, "Short read  from device, expected %d, got %d\n", length, count);
    close(fd);
    return(-1);
  }
  close(fd);

  //use shared blocks to convert data
  read_data = data.f;

  return count;

}


int I2C::readRCinputs(float read_data[])
{

  uint8_t length=16 ;
  uint8_t buf[16];
  int8_t count = 0;

  union
  {
    float f;
    uint8_t b[4];
  } data[4];


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

  count = read(fd, buf, length);

  if (count < 0) {
    fprintf(stderr, "Failed to read device(%d): %s\n", count, ::strerror(errno));
    close(fd);
    return(-1);
  } else if (count != length) {
    fprintf(stderr, "Short read  from device, expected %d, got %d\n", length, count);
    close(fd);
    return(-1);
  }
  close(fd);

  //use shared blocks to convert data
  for (int i=0;i<4;i++)
    {
    for (int ii=0;ii<4;ii++)
      {
  	data[i].b[ii] = buf[ii+4*i];
      }
    read_data[i] =  data[i].f;
    }

    printf("throttle = %f, yaw = %f, pitch = %f , roll = %f \n",
	   read_data[0], read_data[1],
	   read_data[2], read_data[3]);


  return count;
}


