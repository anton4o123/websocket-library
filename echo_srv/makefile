all: compile clean

server.o: server.cc
	g++ -c server.cc -o server.o

main.o: main.cc
	g++ -c main.cc -o main.o

lib.o: ../lib/websock_lib.cc 
	g++ -c ../lib/websock_lib.cc -o lib.o

compile: server.o main.o lib.o
	g++ server.o main.o lib.o -lcrypto -o server

clean:
	rm -rf *.o
