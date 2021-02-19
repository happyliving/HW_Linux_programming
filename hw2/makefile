all: hw2
CC = gcc
CFLAG = -c -g

hw2: sandbox.o sandbox.so
	$(CC) -o hw2 sandbox.o
sandbox.o: sandbox.c sandbox.h
	$(CC) $(CFLAG) sandbox.c
sandbox.so: sandboxso.c
	gcc -o sandbox.so -shared -fPIC sandboxso.c -ldl

clean:
	rm sandbox.o sandbox.so hw2