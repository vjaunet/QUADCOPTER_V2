#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

// these have to be put before the
// other local .h files
// otherwise compiling errors show up
#include <sys/socket.h>
#include <fcntl.h>
#include <time.h>

#include "timer.h"
#include "pid.h"
#include "I2Cclass.h"
#include "SPIClass.h"
#include "dmp.h"
#include "socket_api.h"

#endif
