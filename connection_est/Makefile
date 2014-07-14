all: echo clean

base64_sha1.o: lib/base64_sha1.hh lib/base64_sha1.cc
	g++ -c lib/base64_sha1.cc

main.o: echocli.cc
	g++ -c echocli.cc -o main.o

echo: base64_sha1.o main.o
	g++ base64_sha1.o main.o -o echo -lcrypto

clean:
	rm -rf *.o