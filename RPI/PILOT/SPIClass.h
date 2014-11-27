#ifndef SPI_CLASS_
#define SPI_CLASS_

class SPI {

private:
  const char* _devName;
  uint8_t _mode;
  uint8_t _bits;
  uint32_t _speed;
  uint16_t _delay;
  int _fd;

  int transferBytes(uint8_t *datasent,uint8_t *datarecv);
  int writeByte(uint8_t datasent);
  int readByte(uint8_t datarecv);

public:
  SPI();
  int initialize();

  //Specifically made for the quadcopter
  int transferRC(float read_data[], int ESC[]);
};

extern SPI ArduSPI;

#endif
