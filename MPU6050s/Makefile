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
OPT_Debug= -Wall -g -O2

# Set DMP FIFO rate to 20Hz to avoid overflows on 3d demo.  See comments in
# MPU6050_6Axis_MotionApps20.h for details.
# F = 200/(DDMP_FIFO_RATE + 1)

CXXFLAGS = -DDMP_FIFO_RATE=1


ALL:$(EXEC)

$(EXEC):$(OBJ)
	$(CC) $(CXXFLAGS) $(OPT_Debug) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) $(OPT_Debug) -o $@ -c $^ $(LIBS)

clean:
	rm -rf *.o *~ *.mod
	rm -rf $(EXEC)
