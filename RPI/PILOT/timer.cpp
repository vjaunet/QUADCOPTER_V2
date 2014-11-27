/*
 * File:   Timer.cpp
 * Author: matt
 *
 * Created on 05 November 2012, 10:19
 * Modified 01-2014, vincent jaunet
 *
 * The timer class realize an action on a specific
 * clock signal. The clock oscillates at period PERIOD.
 * it uses a timer
 *
 * The action to be realized is specified in the function
 * sig_handler.
 *
 */

#include "timer.h"

#define PERIOD 1000000
#define YAW 0
#define PITCH 1
#define ROLL 2
#define DIM 3

/*Defines which PID control to use*/
#define PID_STAB
//#define PID_RATE

TimerClass Timer;
pthread_mutex_t TimerMutex_;

TimerClass::TimerClass()
{
  /* Intialize sigaction struct */
   signalAction.sa_handler = &sig_handler_;

   /* Connect a signal handler routine to the SIGALRM signal */
   sigaction(SIGALRM, &signalAction, NULL);

   /* Allocate a timer */
   timer_create(CLOCK_REALTIME, NULL, &timerId);

   started = false;
 }

 TimerClass::TimerClass(const TimerClass& orig)
 {
 }

 TimerClass::~TimerClass()
 {
 }

void TimerClass::start()
{
  timeValue_.tv_sec = 0;
  timeValue_.tv_nsec = PERIOD;
  timeToSet_.it_value = timeValue_;
  timer_settime(timerId, 0, &timeToSet_, NULL);
  started = true;
}

void TimerClass::stop()
{
  timeValue_.tv_sec = 0;
  timeValue_.tv_nsec = 0;
  timeToSet_.it_value = timeValue_;
  timer_settime(timerId, 0, &timeToSet_, NULL);
  started = false;
}

inline void TimerClass::calcdt_()
{
  oldtime_ = time_;
  clock_gettime(CLOCK_MONOTONIC, &time_);
  Timer.dt = ((static_cast <int64_t>(time_.tv_sec) * 1000000000 +
	       static_cast <int64_t>(time_.tv_nsec)) -
	      (static_cast <int64_t>(oldtime_.tv_sec) * 1000000000 +
	       static_cast <int64_t>(oldtime_.tv_nsec))) / 1000000000.0;
}

inline void TimerClass::compensate_()
{
  //Timer aims to get as close to 400Hz as possible, mostly limited by the I2C
  //bandwidth
  clock_gettime(CLOCK_MONOTONIC, &iterationtime_);
  long inttime = PERIOD - ((iterationtime_.tv_sec * 1000000000 +
			    iterationtime_.tv_nsec) - (time_.tv_sec * 1000000000
						       + time_.tv_nsec));
  if (inttime < 0)
    Timer.timeValue_.tv_nsec = 1;
  else
    Timer.timeValue_.tv_nsec = inttime;
  Timer.timeToSet_.it_value = Timer.timeValue_;
  timer_settime(timerId, 0, &timeToSet_, NULL);
}

template <class T, void(T::*member_function)()>
void* thunk(void* p)
{
  (static_cast<T*> (p)->*member_function)();
  return 0;
}

void TimerClass::sig_handler_(int signum)
{
  pthread_mutex_lock(&TimerMutex_);

  float RCinput[4],PIDout[3];
  int ESC[4]={1800,1800,1800,1800};

  //1-Get Remote and Send ESC values
  ArduSPI.transferRC(RCinput,ESC);

  printf("Received %f %f %f %f\n", RCinput[0],
  	 RCinput[1], RCinput[2], RCinput[3]);


  //2- Get attitude of the drone
  imu.getAttitude();

  // printf("ATTITUDE: %7.2f %7.2f %7.2f\n",imu.ypr[YAW],
  // 	 imu.ypr[PITCH],
  // 	 imu.ypr[ROLL]);

  //3- Timer dt
  Timer.calcdt_();
  //  printf("dt : %f \n",Timer.dt);

  //4-1 Calculate PID on attitude

  // if (abs(Timer.ypr_setpoint[YAW])<5) {
  //   Timer.ypr_setpoint[YAW] =  imu.ypr[YAW];
  // }

  #ifdef PID_STAB
  //Stabilization is only done on Pitch and Roll
  //Yaw is Rate PID only
  for (int i=1;i<DIM;i++){
    PIDout[i] =
      yprSTAB[i].update_pid_std(RCinput[i+1],
  			    imu.ypr[i],
  			    Timer.dt);
  }
  PIDout[0] = RCinput[1];

  // printf("PITCH: %7.2f %7.2f %7.2f\n",Timer.ypr_setpoint[PITCH],
  // 	 imu.ypr[PITCH],
  // 	 PIDout[PITCH]);

  // printf("ROLL: %7.2f %7.2f %7.2f\n",RCinput[ROLL+1],
  // 	 imu.ypr[ROLL],
  // 	 PIDout[ROLL]);


  for (int i=0;i<DIM;i++){
    PIDout[i] =
      yprRATE[i].update_pid_std(PIDout[i],
				imu.gyro[i],
				Timer.dt);
  }

  // printf("YAW: %7.2f %7.2f %7.2f\n",Timer.ypr_setpoint[YAW],
  // 	 imu.gyro[YAW],
  // 	 PIDout[YAW]);

  // printf("PITCH: %7.2f %7.2f %7.2f\n",Timer.ypr_setpoint[PITCH],
  // 	 imu.gyro[PITCH],
  // 	 PIDout[PITCH]);

  // printf("ROLL:  %7.2f %7.2f %7.2f\n",RCinput[ROLL+1],
  // 	 imu.gyro[ROLL],
  // 	 PIDout[ROLL]);


  #endif

  //4-2 Calculate PID on rotational rate
  #ifdef PID_RATE
  for (int i=0;i<DIM;i++){
    PIDout[i] =
      yprRATE[i].update_pid_std(RCinput[i+1],
      			    imu.gyro[i],
      			    Timer.dt);
  }
  //printf("%7.2f  %7.2f\n",imu.gyro[PITCH],Timer.PIDout[PITCH]);
  #endif

  //5- ESC update and compensate Timer
  //   if timer has not been stopped
  //  if (Timer.started){

  //compute each new ESC value
  ESC[0] = (int)(RCinput[0]*10+1000
			 + PIDout[PITCH] + PIDout[YAW]);
  ESC[1] = (int)(RCinput[0]*10+1000
			 - PIDout[PITCH] + PIDout[YAW]);
  ESC[2] = (int)(RCinput[0]*10+1000
			 + PIDout[ROLL] - PIDout[YAW]);
  ESC[3] = (int)(RCinput[0]*10+1000
			 - PIDout[ROLL] - PIDout[YAW]);

  printf("Sent : %d %d %d %d\n", ESC[0],
  	 ESC[1], ESC[2], ESC[3]);

    Timer.compensate_();
  //}

  pthread_mutex_unlock(&TimerMutex_);
}
