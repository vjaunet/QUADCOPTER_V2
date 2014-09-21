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


I2C::I2C(uint8_t address)
{

 _devName = "/dev/i2c-1";
 _devAddr = address;

}

void I2C::sendDouble(double data)
{
}

int I2C::readFloat(float &read_data)
{

  uint8_t length=4;
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


int I2C::readFloats(float read_data[], int num)
{

  uint8_t length=4*num;
  uint8_t buf[(int) length];
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

  union Sharedblock
  {
    uint8_t b[4];
    float f;
  } data;

  //use shared blocks to convert data
  for (int i=0;i<4;i++)
    {
    for (int ii=0;ii<4;ii++)
      {
	data.b[ii] = buf[ii+i*4];

      }
    read_data[i] =  data.f;
    }

  return count;
}


