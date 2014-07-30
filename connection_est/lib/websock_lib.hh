#ifndef __WEBSOCK_LIB__
#define __WEBSOCK_LIB__

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <openssl/sha.h>
using namespace std;

unsigned int convert_to_dec(char c);
char get_base64(unsigned int c);
string generate_random_base64();
string base64_encode_sha1(string unencoded);
/*
string create_opening_handshake(string hostname, string websocket_key);
string get_accept(string websocket_key);
bool approve_server_handshake(string handshake, string expected_accept);
*/
class WebSocket {
	
};

#endif