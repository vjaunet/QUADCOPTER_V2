/*

  Parser for data from QuadControl app
  ------------------------------------

  take the char data as an input and
  ouputs floating points values

  author : vincennt JAUNET


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

#include "parser.h"

Parser parser;

//constructor
Parser::Parser()
{
}

//destructor
Parser::~Parser()
{
}

void Parser::parse(unsigned char data[256],float &t,float ypr[]){
  //returns thrust, yaw, pitch and roll walues form char data

  //Processing packet
  std::string packet( reinterpret_cast< char const* > (data));
  std::istringstream ss(packet);
  //printf("%s \n", packet.c_str());

  //Getting target values from packet
  do
    {
      std::string sub;
      ss >> sub;
      float cmd;

      if (sub == "\"thr\":" ){
	ss >> sub;
	std::istringstream( sub ) >> cmd;
	t = (cmd*10 + 1080);
      }
      else if(sub == "\"yaw\":"){
	ss >> sub;
	std::istringstream( sub ) >> cmd;
	ypr[0] = cmd*3.5;
      }
      else if(sub == "\"pitch\":"){
	ss >> sub;
	std::istringstream( sub ) >> cmd;
	ypr[1] = cmd;
      }
      else if(sub == "\"roll\":"){
	ss >> sub;
	std::istringstream( sub ) >> cmd;
	ypr[2] = cmd;
      }
    } while (ss);
}


void Parser::parse(unsigned char data[],float &kp,float &ki,float &kd){
  //returns thrust, yaw, pitch and roll walues form char data

  //Processing packet
  std::string packet( reinterpret_cast< char const* > (data));
  std::istringstream ss(packet);
  //  printf("%s \n", packet.c_str());

  //Getting target values from packet
  do
    {
      std::string sub;
      ss >> sub;

      if (sub == "kp" ){
	ss >> sub;
	kp = ::atof(sub.c_str());
      }
      else if(sub == "ki"){
	ss >> sub;
	ki = ::atof(sub.c_str());
      }
      else if(sub == "kd"){
	ss >> sub;
	kd = ::atof(sub.c_str());
      }
    } while (ss);
}


