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
  void sendDouble(double send_data);
  int  readFloat(float &read_data);
  int  readFloats(float read_data[],int num);

};

#endif
