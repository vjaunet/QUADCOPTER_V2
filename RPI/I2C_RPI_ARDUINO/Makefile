#nom de l'executable
EXEC = test_i2c

#source : rech de .cpp
SRC=$(wildcard *.cpp)
#construction des objets
OBJ=$(SRC:.c=.o)
#-----------------------------------------------------------
#compilation :
CC=g++
LIBS= -lm #-llapack

#option de vectorisation et parallelisation auto :
OPT_para= #-O3 -vec-report0 -parallel -par-report1
#option de debugage :
OPT_Debug= -Wall

ALL:$(EXEC)

$(EXEC):$(OBJ)
	$(CC) $(OPT_para) $(OPT_Debug) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(OPT_para) $(OPT_Debug) -o $@ -c $^ $(LIBS)

clean:
	rm -rf *.o *~ *.mod
	rm -rf $(EXEC)

