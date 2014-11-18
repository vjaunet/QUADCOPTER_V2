#ifndef SPI_class_
#define SPI_class_

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

class SPI {

private:
  const char* _devName;

public:
  SPI();
  int transferBytes(uint8_t datasent[],uint8_t datarecv[], uint8_t length);

  //Specifically made for the quadcopter
  int transferRC(float read_data[],int numRC, int ESC[], int numESC);
};

extern SPI ArduSPI;

#endif
