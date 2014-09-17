#ifndef __CLI
#define __CLI

#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>


class CLI {

 private:
  float m_AngKp;
  float m_AngKi;
  float m_AngKd;

  float m_RateKp;
  float m_RateKi;
  float m_RateKd;


 public:
  CLI();

  bool CLparser(int, char *[]);
  void getKPID(float []);
};

extern CLI cli;

#endif
