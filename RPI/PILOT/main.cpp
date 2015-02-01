/*
  ------------------
   Quadcopter Pilot
  ------------------
  author : vincent jaunet
  mail : vincent.jaunet(AT)hotmail(DOT)fr

  - Initialization of PID
  - Initialization of ESC
  - Initialization of MPU6050

  Copyright (c) <2014> <Vincent Jaunet>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <signal.h>
#include "main.h"

void stop_motors(int s){
  printf("Caught signal %d\n",s);

  ArduSPI.writeByte('S');
  for (int iesc=0;iesc < 8; iesc++) {
    ArduSPI.writeByte((uint8_t) 0);
  }
  //sending end of transaction
  ArduSPI.writeByte('P');

  exit(0);
}

void Set_default_PID_config(){
  //manual initialization of PID constants
  yprRATE[YAW].set_Kpid(3.5, 0.0, 0.0);
  for (int i=1;i<3;i++){
    yprSTAB[i].set_Kpid(2.0, 0.00, 0.0);
    yprRATE[i].set_Kpid(2.0, 0.00, 0.0);
  }
}

//-------------------------------------
//--------- Main-----------------------

int main(int argc, char *argv[])
{
  printf("QuadCopter Pilot v2.0\n");
  printf("----------------------\n");
  printf("\n");

  system("rm quadpilot.log");


  //handling of CTRL+C input
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = stop_motors;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  //setting up IMU
  imu.set_com();
  imu.initialize();

  //setting up SPI
  ArduSPI.initialize();

  //Set PID config
  Set_default_PID_config();

  //Starting Timer
  Timer.start();

  /* Sleeping everything is done via
     software interrupts  */
  while (true){

    usleep(20000);

  }//end

  return 0;
}


//
