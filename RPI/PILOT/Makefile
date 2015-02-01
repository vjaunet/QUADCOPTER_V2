#nom de l'executable
EXEC = quad_pilot

#source : rech de .cpp
SRC=$(wildcard *.cpp)
#construction des objets
OBJ=$(SRC:.cpp=.o)

#-----------------------------------------------------------
#compilation :
CC = g++
LIBS= -lm -lrt #-llapack

#option de debugage :
OPT_Debug= -O2 -Wall

# Set DMP FIFO rate to 20Hz to avoid overflows on 3d demo.  See comments in
# MPU6050_6Axis_MotionApps20.h for details.
# F = 200/(DDMP_FIFO_RATE + 1)
#the program is not fast enough to read DMP at 200Hz => DDMP_FIFO_RATE = 1

CXXFLAGS = -DDMP_FIFO_RATE=3


ALL:$(EXEC)

$(EXEC):$(OBJ)
	$(CC) $(CXXFLAGS) $(OPT_Debug) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) $(OPT_Debug) -o $@ -c $^ $(LIBS)

clean:
	rm -rf *.o *~ *.mod
	rm -rf $(EXEC)
