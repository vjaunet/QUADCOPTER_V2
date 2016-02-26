CC      = gcc
INCLUDE = -I/usr/local/include
CFLAGS  = -Wall $(INCLUDE) -Winline -pipe

LDFLAGS = -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm


all: sofTone.o
	$(CC) -o beep softTone.o $(LDFLAGS) $(LDLIBS)

sofTone.o:
	$(CC) -c softTone.c $(LDFLAGS) $(LDLIBS)
