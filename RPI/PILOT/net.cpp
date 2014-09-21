/*

  Socket class
  author : vincent jaunet
  date : 10-01-2013

  Description :
  The Socket class contains all the necessary routine to set
  up and use a UDP network serve on the drone waiting for
  the remote client to send request.
  The output should be parsed in order to retrieve
  the desired attitude from the remote (see parse.*)

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

#define YAW 0
#define PITCH 1
#define ROLL 2

#define STOP_PID  666
#define START_PID 1
#define INIT 2
#define SHUTDOWN 777
#define NOTHING  1515
#define UPDATE_REMOTE 0
#define UPDATE_PID_YAW_RATE 10
#define UPDATE_PID_YAW_STAB 11
#define UPDATE_PID_PR_RATE 12
#define UPDATE_PID_PR_STAB 13



#include "net.h"

Socket remote;

Socket::Socket()
{
  data[0] = '\0';
  m_port = 7100;
  m_address.sin_family = AF_INET;
  m_address.sin_addr.s_addr = INADDR_ANY;
  m_address.sin_port = htons( (unsigned short) m_port );
}


Socket::~Socket()
{
  Close();
}


void Socket::set_port(int port){
  //set the port to desired value
  m_port = port;
}


void Socket::create()
{

  //Opening socket
  m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
  if ( m_socket <= 0 )
    {
      printf( "Failed to create socket\n" );
      exit(EXIT_FAILURE);
    }

  //Binding to desired port number
  if ( bind( m_socket, (const sockaddr*) &m_address, sizeof(sockaddr_in) ) < 0 )
    {
      perror( "failed to bind socket");
      exit(EXIT_FAILURE);
    }

  //setting Socket to non blocking mode
  int nonBlocking = 1;
  if ( fcntl( m_socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
    {
      printf( "failed to set non-blocking socket\n" );
      exit(EXIT_FAILURE);
    }

  printf( "Succeed to create socket\nWaiting for Instructions...\n" );

}


void Socket::Close(){

  if ( m_socket != 0 )
    {
      close( m_socket );
      m_socket = 0;
    }
}

int Socket::get_cmd(){

  int type=0;

  //returns 1 for Start
  //returns 2 for Initialize
  //returns 666 for Exit
  //retunrs 10 for yaw PID constants
  //retunrs 11 for yawrate PID constants
  //retunrs 12 for PR PID constants
  //retunrs 13 for PRrate PID constants

  //returns 0 for rcinputs (default)

  int size = sizeof(data);
  assert( size > 0 );

  if ( m_socket == 0 )
    printf("Socket is closed...");

  int received_bytes = -1;
  sockaddr_in from;
  socklen_t fromLength = sizeof( from );

  received_bytes = recvfrom( m_socket, data, size, 0,
				 (sockaddr*)&from,
				 &fromLength);
  if (received_bytes == -1){
    //printf("received bytes = -1 \n");
    return NOTHING;
  }

  std::string packet( reinterpret_cast< char const* > (data));
  std::istringstream ss(packet);

  std::string sub;
  ss >> sub;

  do{
    if (sub == "START"){
      type = START_PID;
      break;
    }else if (sub == "STOP"){
      type = STOP_PID;
      break;
    }else if (sub == "EXIT"){
      type = SHUTDOWN;
      break;
    }else if (sub == "INIT"){
      type = INIT;
      break;
    } else if(sub == "pid"){
       ss >> sub;
       if (sub == "yaw_rate"){
	 type = UPDATE_PID_YAW_RATE;
	 break;
       }else if (sub == "yaw_stab"){
	 type = UPDATE_PID_YAW_STAB;
	 break;
       }else if (sub == "pr_stab"){
	 type = UPDATE_PID_PR_STAB;
	 break;
       }else if (sub == "pr_rate"){
	 type = UPDATE_PID_PR_RATE;
	 break;
       }
    } else { break; }
  }while(ss);

  // printf("%d\n",type);
  return(type);

}

void Socket::exec_remoteCMD()
{
  //PID variables
  float kp_,ki_,kd_;

  switch(get_cmd()){
      //returns 1 for Start(Initialize)
      //returns 2 for Initialize
      //returns 10 for yawstab PID constants
      //returns 11 for yawrate PID constants
      //returns 12 for PRstab PID constants
      //returns 13 for PRrate PID constants

  case NOTHING:
    //nothing to do here
    break;

  case SHUTDOWN:
    //On Shutdown :

    //stop Timer
    Timer.stop();
    if (Timer.started){
      printf("toto");
    }

    //close socket
    Close();

    //stop servos
    if (ESC.Is_open_blaster()){
      ESC.stopServo();
      ESC.close_blaster();
    }

    //shutdown
    system("sudo shutdown -h now");

    //exit quad_pilot
    exit(1);

    //printf("PID stopped \n");

    break;

  case UPDATE_REMOTE:
    //set rcinput values values
    parser.parse(data,Timer.thr,Timer.ypr_setpoint);
    break;

  case UPDATE_PID_YAW_STAB:
    //set pid constants YAW Stab
    parser.parse(data,kp_,ki_,kd_);
    yprSTAB[YAW].set_Kpid(kp_,ki_,kd_);
    break;

  case UPDATE_PID_YAW_RATE:
    //set pid constants YAW Rate
    parser.parse(data,kp_,ki_,kd_);
    yprRATE[YAW].set_Kpid(kp_,ki_,kd_);
    break;

  case UPDATE_PID_PR_STAB:
    //set pid constants
    parser.parse(data,kp_,ki_,kd_);
    yprSTAB[PITCH].set_Kpid(kp_,ki_,kd_);
    yprSTAB[ROLL].set_Kpid(kp_,ki_,kd_);
    //printf("PID: %7.2f %7.2f %7.2f \n",kp_,ki_,kd_);
    break;

  case UPDATE_PID_PR_RATE:
    //set pid constants
    parser.parse(data,kp_,ki_,kd_);
    yprRATE[PITCH].set_Kpid(kp_,ki_,kd_);
    yprRATE[ROLL].set_Kpid(kp_,ki_,kd_);
    //printf("PID: %7.5f %7.5f %7.5f \n",kp_,ki_,kd_);
    break;

  case INIT:
    //intialization of IMU
    if (!Timer.started && !imu.initialized){
      imu.set_com();
      imu.initialize();
    }

    //initilization of PID constants
    yprRATE[YAW].set_Kpid(3.5,0.1,0.1);
    yprRATE[PITCH].set_Kpid(2.9,0.1,0.125);
    yprRATE[ROLL].set_Kpid (2.9,0.1,0.125);
    yprSTAB[PITCH].set_Kpid(3.3,0.035,0.04);
    yprSTAB[ROLL].set_Kpid(3.3,0.035,0.04);

    break;

  case STOP_PID:
    //On exit :

    //stop Timer
    Timer.stop();

    //stop servos
    if (ESC.Is_open_blaster()){
      ESC.stopServo();
      ESC.close_blaster();
    }

    //reset PID
    for (int i=0;i<DIM;i++) yprSTAB[i].reset();
    for (int i=0;i<DIM;i++) yprRATE[i].reset();

    printf("PID Stopped \n");
    break;


  case START_PID:
    //Remote says "Start"

    if (Timer.started){
      //PID already running
      break;
    } else if (!imu.initialized){
      //IMU not initialized
      printf("DMP not Initalized\n Can't start...\n");
      break;
    }

    //Initializing ESCs
    printf("Initialization of ESC...\n");
    ESC.open_blaster();
    ESC.init();
    printf("                     ... DONE.\n");

    //Things are getting started !
    //launch the Alarm signal
    Timer.start();
    while (Timer.started){
      sleep(1000);
    }

  }//end switch

  return;
}




