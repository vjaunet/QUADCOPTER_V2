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


#include "main.h"

//-------------------------------------
//--------- Main-----------------------

int main(int argc, char *argv[])
{
  printf("QuadCopter Pilot v2.0\n");
  printf("----------------------\n");
  printf("\n");


  //initilization of PID constants
  yprRATE[YAW].set_Kpid(3.5,0.1,0.1);
  yprRATE[PITCH].set_Kpid(2.9,0.1,0.125);
  yprRATE[ROLL].set_Kpid (2.9,0.1,0.125);
  yprSTAB[PITCH].set_Kpid(3.3,0.035,0.04);
  yprSTAB[ROLL].set_Kpid(3.3,0.035,0.04);

  //setting up IMU
  imu.set_com();
  imu.initialize();

  //setting up SPI
  ArduSPI.initialize();

  //Starting Timer
  Timer.start();

  /* Waiting fo Start command */
  while (true){

    usleep(20000);

  }//end

  return 0;
}


//
