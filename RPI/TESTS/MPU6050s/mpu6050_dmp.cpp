#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "mpu6050_dmp.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    //initialize device
    printf("Initializing I2C devices...\n");
    mpu.initialize();

    // verify connection
    printf("Testing device connections...\n");
    printf(mpu.testConnection() ? "MPU6050 connection successful\n"
	   : "MPU6050 connection failed\n");

    // load and configure the DMP
    printf("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        printf("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        printf("DMP ready!\n");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        printf("DMP Initialization failed (code %d)\n", devStatus);
    }
}

// ================================================================
// ===                      INITIALIZATION OF MPU               ===
// ================================================================

void MPU_init(){

  //Let the MPU run as long as ypr drifts

  printf("Running initialization process...\n");

  int i;
  float ypr_o[3];
  float err=0.;

  for (i=0;i<3;i++)
    ypr_o[i] = 1000;

  do {
    err=0.;

    fifoCount = mpu.getFIFOCount();

    if (fifoCount == 1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
      printf("FIFO overflow!\n");

      // otherwise, check for DMP data ready
      //interrupt (this should happen frequently)
    } else if (fifoCount <= 1024) {

      mpu.getFIFOBytes(fifoBuffer, packetSize);

      //get new ypr values
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    }

    //printf("ypr  %7.2f %7.2f %7.2f    \n", ypr[0] * 180/M_PI, ypr[1] *
    //	     180/M_PI, ypr[2] * 180/M_PI);


      for (i=0;i<3;i++){
	err += fabs(ypr[i] - ypr_o[i])*180/M_PI;
	ypr_o[i] = ypr[i];
      }

      //printf("Error = %7.2f \n",err);

      //wait for MPU to let MPU update register
      usleep((DMP_FIFO_RATE)*5000);

  } while (err > 1e-8);

  printf("Initialization is done :\n");
  printf("-- ypr0  %7.2f %7.2f %7.2f\n",
	 ypr[0]*180/M_PI, ypr[1]*180/M_PI,ypr[2]*180/M_PI);

  usleep((DMP_FIFO_RATE)*5000);

}


// ================================================================
// ===                    MAIN ROUTINES                         ===
// ================================================================

void MPU_setup(){
  setup();
  MPU_init();
}


void MPU_getYPR(float ypr[3]) {

  // if programming failed, don't try to do anything
  if (!dmpReady){
    return;}

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  if (fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (fifoCount < 1024) {
    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    //    printf("ypr  %7.2f %7.2f %7.2f    \n", ypr[0] * 180/M_PI, ypr[1] *
    //	   180/M_PI, ypr[2] * 180/M_PI);
  }
}
