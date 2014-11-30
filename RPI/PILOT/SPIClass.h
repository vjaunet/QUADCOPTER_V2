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

public:
  SPI();
  int initialize();

  int writeByte(uint8_t datasent);
  uint8_t readByte();
  uint8_t rwByte(uint8_t datasent);

};

extern SPI ArduSPI;

#endif
