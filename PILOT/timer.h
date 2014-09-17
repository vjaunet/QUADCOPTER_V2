/*
 * File:   Timer.h
 * Author: matt
 *
 * Created on 05 November 2012, 10:19
 * Modified 01-2014, vincent jaunet
 */

#ifndef TIMER_H
#define TIMER_H

#include <signal.h>
#include <iostream>
#include <stdint.h>
#include <time.h>

#include "net.h"
#include "servo.h"
#include "pid.h"
#include "dmp.h"

class TimerClass
{
 public:
  TimerClass();
  TimerClass(const TimerClass& orig);
  virtual ~TimerClass();

  void start();
  void stop();

  float dt;
  timer_t timerId; //id  for the posix timer
  struct sigaction signalAction; //signal action handler struct
  bool started;

  float thr, ypr_setpoint[3];

 private:
  static void sig_handler_(int signum);
  struct itimerspec timeToSet_;  //time to be set
  struct timespec timeValue_;    //timer expiration value
  struct timespec timeInterval_; //timer period

  timespec oldtime_;
  timespec time_;
  timespec iterationtime_;

  void calcdt_();
  void compensate_();

  //PID variables
  float kp_,ki_,kd_;
  float PIDout[3];

};

extern TimerClass Timer;
#endif        /* TIMER_H */
