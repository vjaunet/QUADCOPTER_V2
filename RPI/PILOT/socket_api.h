#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Socket{

 private:
  sockaddr_in m_server_addr,m_client_addr;
  unsigned short m_port;
  int m_server_fd,m_client_fd;
  unsigned char data[256];
  void parse_data();


 public:
  Socket();
  ~Socket();

  void set_port(int port);
  int create_UDP();
  int create_TCP();
  int set_non_blocking();
  void Close();
  int get_client();
  int get_config();

};

extern Socket WebApp;

#endif
