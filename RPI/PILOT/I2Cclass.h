#ifndef I2C_class_
#define I2C_class_


class I2C {

private:
  const char* _devName;
  uint8_t _devAddr;

public:
  I2C(uint8_t address);
  int sendInt(int data);
  int sendBytes(uint8_t data[], uint8_t length);
  int readBytes(uint8_t data[], uint8_t length);

  //Specifically made for the quadcopter
  int readRCinputs(float read_data[], int num);
  int sendESCs(int data[], int num);

};

extern I2C Arduino;

#endif
