/*

  Socket class
  author : vincent jaunet
  date : 10-01-2014

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

#include "socket_api.h"

Socket WebApp;

Socket::Socket()
{
  data[0] = '\0';
  m_port = 7100;
  m_server_addr.sin_family = AF_INET;
  m_server_addr.sin_addr.s_addr = INADDR_ANY;
  m_server_addr.sin_port = htons( (unsigned short) m_port );
}


Socket::~Socket()
{
  Close();
}


void Socket::set_port(int port){
  //set the port to desired value
  m_port = port;
}


int Socket::create_UDP()
{

  //Opening socket
  m_server_fd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
  if ( m_server_fd <= 0 )
    {
      printf( "Socket_Api : Failed to create socket\n" );
      return -1;
    }

  //Binding to desired port number
  if ( bind( m_server_fd, (const sockaddr*) &m_server_addr, sizeof(sockaddr_in) ) < 0 )
    {
      perror( "Socket_Api : Failed to bind socket");
      return -1;
    }

  //setting Socket to non blocking mode
  int nonBlocking = 1;
  if ( fcntl( m_server_fd, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
    {
      printf( "failed to set non-blocking socket\n" );
      return -1;
    }

  printf( "Socket_Api : Succeed to create socket.\n" );
  return 1;

}

int Socket::create_TCP()
{

  //Opening socket in TCP mode
  m_server_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if ( m_server_fd <= 0 )
    {
      printf( "Socket_Api : Failed to create socket\n" );
      return -1;
    }

  //Binding to desired port number
  if ( bind( m_server_fd, (const sockaddr*) &m_server_addr, sizeof(sockaddr_in) ) < 0 )
    {
      printf("Socket_Api : Failed to bind socket");
      return -1;
    }

  printf("Socket_Api : Succeed to create socket.\n" );
  return 1;

}

int Socket::set_non_blocking(){
  //setting Socket to non blocking mode
  int nonBlocking = 1;
  if ( fcntl( m_server_fd, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
    {
      printf( "failed to set non-blocking socket\n" );
      return -1;
    }
  return 1;
}


int Socket::get_client() {

  //Start listening
  // Server should start listening - This enables the program to halt on accept
  // call (coming next)
  // and wait until a client connects. Also it specifies the size of pending
  // connection requests queue
  // i.e. in this case it is 5 which means 5 clients connection requests will be
  // held pending while
  listen(m_server_fd, 5);

  printf("Socket_Api : Waiting for connection...\n");
  socklen_t size = sizeof(m_client_addr);

  // Server blocks on this call until a client tries to establish connection.
  // When a connection is established, it returns a 'connected socket
  // descriptor' different
  // from the one created earlier.
  m_client_fd = accept(m_server_fd, (struct sockaddr *)&m_client_addr, &size);
  if (m_client_fd == -1)
    {
      printf("Socket_Api : Failed accepting connection\n\n");
      return -1;
    }  else {
    printf("Socket_Api : Connected\n\n");
    return 1;
  }

}


void Socket::Close(){

  if ( m_server_fd != 0 )
    {
      close( m_server_fd );
      m_server_fd = 0;
    }
}

int Socket::get_config(){
  //ask for data to the Web client
  // and parsing it
  int size = sizeof(data);
  assert( size > 0 );

  if ( m_client_fd == 0 ){
    printf("Socket is closed...");
    return -1;
  }

  printf("Socket_Api : Request for data...\n");

  std::string msg = "GET_CONF";
  int bytes_sent = send(m_client_fd, msg.c_str(), msg.length(), 0);
  if (bytes_sent < 0) {
    return -1;
  }

  printf("Socket_Api : Waiting for data...\n");


  sockaddr_in from;
  socklen_t fromLength = sizeof(from);
  int received_bytes = recvfrom( m_client_fd, data, size, 0,
				 (sockaddr*)&from,
				 &fromLength);
  if (received_bytes == -1){
    printf("received bytes = -1 \n");
    return -1;
  }

  //parse the received data stream
  parse_data();

  return 1;
}


void Socket::parse_data(){

  //convert received data so stringstream
  std::string packet( reinterpret_cast< char const* > (data));
  //printf("Socket_Api : Received : %s",packet.c_str());

  //get the first string data to get the type of command
  std::istringstream ss(packet);
  std::string sub;
  ss >> sub;

  //parsing the first substring
  do{
    if (sub == "MODE"){
    }
    else if (sub == "Y_R_KP"){
    }

  }while(ss);

}
