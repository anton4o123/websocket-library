all: server clean 

lib.o: ../echo/lib/websock_lib.cc ../echo/lib/websock_lib.hh
	g++ -c ../echo/lib/websock_lib.cc -o lib.o 

header.o: echo_server.cc
	g++ -c echo_server.cc -o header.o

main.o: main.cc
	g++ -c main.cc -o main.o

server: header.o main.o lib.o
	g++ header.o main.o lib.o -o server

clean:
	rm -rf *.o
