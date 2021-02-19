all: hw1
CC = gcc
CFLAG = -c -Wall 

hw1: main.o util.o
	$(CC) -o hw1 main.o util.o
main.o: main.c util.h
	$(CC) $(CFLAG) main.c
util.o: util.c util.h
	$(CC) $(CFLAG) util.c

clean:
	rm main.o util.o