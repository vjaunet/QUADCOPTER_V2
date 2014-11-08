#ifndef I2C_class_
#define I2C_class_

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

class I2C {

private:
  const char* _devName;
  uint8_t _devAddr;

public:
  I2C(uint8_t address);
  int sendInt(int data);
  int sendBytes(uint8_t data[], uint8_t length);
  int readFloat(float &read_data);

  //Specifically made for the quadcopter
  int readRCinputs(float read_data[]);
  int sendESCs(int data[], int num);

};

extern I2C Arduino;

#endif
