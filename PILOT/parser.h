#ifndef PARSER_H
#define PARSER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class Parser{

 private:

 public:

  Parser();
  ~Parser();
  void parse(unsigned char data[],float &,float []);
  void parse(unsigned char data[],float &t,float &y,float &p);

};

extern Parser parser;

#endif
