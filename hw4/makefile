all: hw4
CC = gcc
CFLAG = -c -Wall 

hw4: simple_http_server.o util.o
	$(CC) -o hw4 simple_http_server.o util.o
simple_http_server.o: simple_http_server.c util.h
	$(CC) $(CFLAG) simple_http_server.c
util.o: util.c util.h
	$(CC) $(CFLAG) util.c

clean:
	rm simple_http_server.o util.o