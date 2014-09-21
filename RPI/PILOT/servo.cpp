/*

   Servo Class

   author : vincent JAUNET
   mail : vincent.jaunet@hotmail.fr
   date : 10-01-2013

   Description:
   the Servo class is a collection of routine used to control
   the motors through the electronic speed controllers.
   ESC and Servo use the same communication protocole: PWM. That is
   the reason why the servoblaster module is being used here. This allows
   to get rid of an external PIC controller to generate such PWM signal.

   The routines in the Serco class do the follwing:
   - open the /dev/servoblaster file in order to write in it
   - initlaize the ESC by sending a PWM of 1ms (~ throttle = 0)
   - write the desired PWM values

*/


#include "servo.h"

Servo ESC;

Servo::Servo()
{
  m_servoId[0] = 0;
  m_servoId[1] = 1;
  m_servoId[2] = 2;
  m_servoId[3] = 3;

  fid_servo=NULL;
}

bool Servo::Is_open_blaster()
{
  if(fid_servo==NULL) {
    //printf("servoblaster not open !!!");
    return false;
  }

  return true;
}

void Servo::open_blaster()
{
  fid_servo=fopen("/dev/servoblaster","w");
  if (fid_servo==NULL){
    printf("Opening /dev/servoblaster failed \n");
    exit(2);
  }
}

void Servo::close_blaster()
{
  if (fid_servo==NULL){
     printf("/dev/servoblaster not opened \n");
     return;
  }
  fclose(fid_servo);
  fid_servo=NULL;
}


void Servo::init()
{
  if(fid_servo==NULL) return;

  //initialisation of ESC
  for (int i=0;i<4;i++){
    servoval[i]=SERVO_MIN;
  };

  setServo();
  sleep(1);
}

void Servo::update(float throttle, float PIDoutput[DIM])
{
  servoval[0] =(int)(throttle + PIDoutput[PITCH] + PIDoutput[YAW]);
  servoval[2] =(int)(throttle - PIDoutput[PITCH] + PIDoutput[YAW]);
  servoval[1] =(int)(throttle - PIDoutput[ROLL]  - PIDoutput[YAW]);
  servoval[3] =(int)(throttle + PIDoutput[ROLL]  - PIDoutput[YAW]);
  setServo();
}

void Servo::stopServo()
{
  for (int i=0;i<4;i++){
    servoval[i]=SERVO_MIN;
  };

  setServo();

}

void Servo::setServo()
{
  if (Is_open_blaster()){
    for (int i=0;i<4;i++){
      fprintf(fid_servo, "%d=%dus\n",m_servoId[i], servoval[i]);
      fflush(fid_servo);
    }
  } else {
    printf("Servoblaster not opened \n");
  }
}
