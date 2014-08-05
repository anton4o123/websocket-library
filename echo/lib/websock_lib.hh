#ifndef __WEBSOCK_LIB__
#define __WEBSOCK_LIB__

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <limits.h>
#include <endian.h>
#include <ctime>
#include <netdb.h>
#include <openssl/sha.h>
using namespace std;

const short WEB_WS=0;
const short WEB_WSS=1;
const int MAX_ACCEPT=4096;
const int CHUNK_SIZE=512;

unsigned int convert_to_dec(char c);
char get_base64(unsigned int c);
string generate_random_base64();
string base64_encode_sha1(string unencoded);

class WebSocket {
	int fd;
	short secure_;

	string create_opening_handshake(string hostname, string websocket_key);
	string get_accept(string websocket_key);
	bool approve_server_handshake(string handshake, string expected_accept);
	char *create_frame(string data);
	void mask_data(char *frame, unsigned long size);
public:
	WebSocket(short secure);
	void connect_to(string hostname);
	void send_text(string data);
	void receive(char *buf);
	void disconnect();
};

#endif