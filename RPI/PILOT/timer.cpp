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
#include <fstream>
using namespace std;

#define PERIOD 500000

#define N_RC_CHAN 4
#define N_SERVO 4

#define K_YAW 30
#define K_PITCH 20
#define K_ROLL 20

#define RC_MIN 1050
#define RC_MAX 1900
#define THR_MIN 890
#define THR_MAX 1950

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

  //output to a log file
  //open log file
  fstream logfile;
  logfile.open("quadpilot.log", ios::out|ios::app);
  if (logfile.fail())          // Check for file creation and return error.
    {
      cout << "Error opening output.\n";
    }


  float RCinput[N_RC_CHAN],PIDout[3];
  uint16_t ESC[N_SERVO];

  //1-Get Remote values using SPI
  union bytes{
    uint8_t u8[2];
    uint16_t u16;
  } rc_union;
  uint8_t checksum=0,recv_checksum=1;

  while (checksum != recv_checksum) {

    checksum=0;

    for (int i=0;i<4;i++){
      ArduSPI.writeByte((uint8_t) (i+1)*10);
      rc_union.u8[0] = ArduSPI.rwByte((uint8_t) (i+1)*10+1);
      rc_union.u8[1] = ArduSPI.rwByte('S');
      //transaction ended
      RCinput[i] = (float) rc_union.u16;

      checksum+=rc_union.u8[0];
      checksum+=rc_union.u8[1];
    }

    //Control checksum
    ArduSPI.writeByte('C');
    recv_checksum = ArduSPI.rwByte('S');
  }

  // //outputting checksums
  // logfile << recv_checksum << ' ' << checksum << ' ';

  // //convert into PID usable values
  RCinput[0] = (RCinput[0] - THR_MIN)/(THR_MAX-THR_MIN) * 100.0;
  RCinput[1] = (RCinput[1] -(RC_MAX+RC_MIN)/2.) /
    (RC_MAX-RC_MIN) * K_YAW;
  RCinput[2] = (RCinput[2] -(RC_MAX+RC_MIN)/2.)/
    (RC_MAX-RC_MIN) * K_PITCH;
  RCinput[3] = (RCinput[3] -(RC_MAX+RC_MIN)/2.)/
    (RC_MAX-RC_MIN) * K_ROLL;

  //outputing values to logfile
  logfile << RCinput[0] << " " << RCinput[1] << " "
  	  << RCinput[2] << " " << RCinput[3] << " ";



  // printf("Received : %6.3f %6.3f %6.3f %6.3f\n", RCinput[0],
  // 	 RCinput[1], RCinput[2], RCinput[3]);


  //2- Get attitude of the drone
  while (imu.getAttitude() < 0){
  };

  //output to logfile
  logfile << imu.ypr[YAW] << " " << imu.ypr[PITCH] << " "
	  << imu.ypr[ROLL] << " "
	  << imu.gyro[YAW] << " " << imu.gyro[PITCH] << " "
	  << imu.gyro[ROLL] << " ";



  // printf("ATTITUDE: %7.2f %7.2f %7.2f\n",imu.ypr[YAW],
  // 	 imu.ypr[PITCH],
  // 	 imu.ypr[ROLL]);

  // printf("          %7.2f %7.2f %7.2f\n",imu.gyro[YAW],
  // 	 imu.gyro[PITCH],
  // 	 imu.gyro[ROLL]);

  //3- Timer dt
  Timer.calcdt_();
  //printf("dt : %f \n",Timer.dt);

  //4-1 Calculate PID on attitude
  #ifdef PID_STAB

  for (int i=1;i<DIM;i++){
    PIDout[i] =
      yprSTAB[i].update_pid_std(RCinput[i+1],
  			    imu.ypr[i],
  			    Timer.dt);
  }

  //yaw is rate PID only
  PIDout[YAW] = RCinput[YAW+1];

  // printf("PITCH: %7.2f %7.2f %7.2f\n",RCinput[PITCH+1],
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

  // printf("YAW:   %7.2f %7.2f %7.2f\n",RCinput[YAW+1],
  // 	 imu.gyro[YAW],
  // 	 PIDout[YAW]);

  // printf("PITCH: %7.2f %7.2f %7.2f\n",RCinput[PITCH+1],
  // 	 imu.gyro[PITCH],
  // 	 PIDout[PITCH]);

  printf("ROLL:  %7.2f %7.2f %7.2f\n",RCinput[ROLL+1],
  	 imu.gyro[ROLL],
  	 PIDout[ROLL]);


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

  logfile << PIDout[YAW] << " " << PIDout[PITCH] << " "
	  << PIDout[ROLL] << " ";


  //5- Send ESC update via SPI
  //compute each new ESC value
  ESC[1] = (uint16_t)(RCinput[0]*10+1000
  		      + PIDout[ROLL] - PIDout[YAW]);
  ESC[3] = (uint16_t)(RCinput[0]*10+1000
  		      - PIDout[ROLL] - PIDout[YAW]);
  ESC[0] = (uint16_t)(RCinput[0]*10+1000
  		      - PIDout[PITCH] + PIDout[YAW]);
  ESC[2] = (uint16_t)(RCinput[0]*10+1000
  		      + PIDout[PITCH] + PIDout[YAW]);

  // for (int i=0;i<3;i++){
  //  ESC[i] = 1110;
  //  }
  // ESC[0] = (uint16_t)(RCinput[0]*10+1000);
  // ESC[2] = (uint16_t)(RCinput[0]*10+1000);
  // ESC[1] = (uint16_t)(RCinput[0]*10+1000);
  // ESC[3] = (uint16_t)(RCinput[0]*10+1000);


  checksum = 0;
  for (int iesc=0;iesc < N_SERVO; iesc++) {
    ArduSPI.writeByte(ESC[iesc] & 0xff);
    checksum+=ESC[iesc] & 0xff;
    ArduSPI.writeByte((ESC[iesc] >> 8) & 0xff);
    checksum+=(ESC[iesc] >> 8) & 0xff;
    }
  ArduSPI.writeByte(checksum);
  //sending Proccess it
  ArduSPI.writeByte('P');

  // printf("    Sent : %4d %4d %4d %4d\n", ESC[0],
  // 	 ESC[1], ESC[2], ESC[3]);

  //6-compensate dt
  Timer.compensate_();

  //ouputting ESC values to logfile
  logfile << ESC[0] << " " << ESC[1] << " "
  	  << ESC[2] << " " << ESC[3] << " " << endl;

  //closing logfile
  logfile.close();

  pthread_mutex_unlock(&TimerMutex_);
  //end of interrupt

}
