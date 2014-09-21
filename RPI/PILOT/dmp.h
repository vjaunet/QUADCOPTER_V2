#ifndef dmp_
#define dmp_

#define YAW 0
#define PITCH 1
#define ROLL 2
#define DIM 3


class DMP {

 private:

  bool dmpReady;         // set true if DMP init was successful
  float lastval[3];
  float m_ypr_off[3];    //dmp offsets

 public:
  DMP();
  void getAttitude();
  void set_com();
  void initialize();
  bool initialized;

  float ypr[3];
  float gyro[3];
};

extern DMP imu;

#endif
