all: server clean 

header.o: echo_server.cc
	g++ -c echo_server.cc -o header.o

main.o: main.cc
	g++ -c main.cc -o main.o

server: header.o main.o
	g++ header.o main.o -o server

clean:
	rm -rf *.o
