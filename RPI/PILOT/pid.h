#ifndef pid
#define pid

#include <iostream>
#include <stdlib.h>
#include <fstream>

class PID {

private:
  //PID constants
  float m_Kp;
  float m_Ki;
  float m_Kd;

  //PID constants
  float m_err;
  float m_sum_err;
  float m_ddt_err;
  float m_lastInput;
  float m_outmax;
  float m_outmin;
  float m_output;

public:
  PID();
  PID(float,float,float);
  float update_pid_std(float setpt, float input, float dt);
  void  set_Kpid(float, float, float);
  void  set_windup_bounds(float, float);
  void  reset();
  float setpoint;
};


extern PID yprSTAB[3];
extern PID yprRATE[3];

#endif
