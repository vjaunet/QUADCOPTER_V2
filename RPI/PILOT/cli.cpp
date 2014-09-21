
/*
 *          cli.cpp
 *      created 01-2014
 *   author : Vincent Jaunet
 *
 * Makes possible to give PID constant values
 * through command line options
 */
#include "cli.h"

CLI cli;

CLI::CLI(){

  m_AngKp=0;
  m_AngKi=0;
  m_AngKd=0;

  m_RateKp=0;
  m_RateKi=0;
  m_RateKd=0;

}


bool CLI::CLparser(int argc, char *argv[]){
  if (argc <=1)
    {
      printf("PID constants set to defaults ...\n");
      return false;
    }

  for (int n=1;n<argc;n+=2){
    if (strcmp(argv[n],"--AngKp")==0)
      {
	m_AngKp = atof(argv[n+1]);
	break;

      } else if (strcmp(argv[n],"--AngKi")==0)
      {
	m_AngKi = atof(argv[n+1]);
	break;

      } else if (strcmp(argv[n],"--AngKd")==0)
      {
	m_AngKd = atof(argv[n+1]);
	break;

      } else if (strcmp(argv[n],"--RateKp")==0)
      {
	m_RateKp = atof(argv[n+1]);
	break;

      } else if (strcmp(argv[n],"--RateKi")==0)
      {
	m_RateKi = atof(argv[n+1]);
	break;

      } else if (strcmp(argv[n],"--RateKd")==0)
      {
	m_RateKd = atof(argv[n+1]);
	break;

      } else {
      printf("Invalid options");
      exit(1);
    }

  }//for n=1 to argc

  return true;

}

void CLI::getKPID(float KPID[6]){

  KPID[0] = m_AngKp;
  KPID[1] = m_AngKd;
  KPID[2] = m_AngKi;
  KPID[3] = m_RateKp;
  KPID[4] = m_RateKi;
  KPID[5] = m_RateKd;

}
