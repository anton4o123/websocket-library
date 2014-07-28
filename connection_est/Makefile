all: cli clean

websock_lib.o: lib/websock_lib.hh lib/websock_lib.cc
	g++ -c lib/websock_lib.cc

echocli.o: echocli.cc
	g++ -c echocli.cc

cli: websock_lib.o echocli.o
	g++ websock_lib.o echocli.o -o cli -lcrypto

clean:
	rm -rf *.o