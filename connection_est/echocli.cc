#include <cstdio>
#include <sstream>
#include <iomanip>
#include "lib/base64_sha1.hh"
#include <openssl/sha.h>

string create_opening_handshake(string ip_address, string websocket_key) {
	string get;

	get+="GET /chat HTTP/1.1\r\n";
	get+="Host: "+ip_address+"\r\n";
	get+="Upgrade: websocket\r\n";
	get+="Connection: Upgrade\r\n";
	get+="Sec-WebSocket-Key: "+websocket_key+"\r\n";
	get+="Origin: "+ip_address+"\r\n";
	get+="Sec-WebSocket-Protocol: chat, superchat\r\n";
	get+="Sec-WebSocket-Version: 13\r\n\r\n";
	return get;
}

string get_accept(string websocket_key) {
	const char *GUID="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	unsigned char* c=new unsigned char[SHA_DIGEST_LENGTH];
	stringstream accept_hex;
	accept_hex.fill('0');
	accept_hex << hex;

	websocket_key+=GUID;
	c=SHA1((unsigned char*)websocket_key.c_str(), websocket_key.size(), NULL);
	for(int i=0;i<20;++i) {
		accept_hex << setw(2) << (unsigned int)c[i];
	}
	return base64_encode_sha1(accept_hex.str());
}

int main(int argc, char const *argv[])
{
	srand((unsigned)time(NULL));
	string key;

/*	cin >> key;

	c=SHA1((unsigned char*)key.c_str(), key.size(), NULL);

	for(int i=0;i<20;++i) {
		printf("%02x", c[i]);
	}
	printf("\n");*/

	key+=generate_random_base64();
	cout << create_opening_handshake("127.0.0.1", key) << endl;
	cout << get_accept(key) << endl;
	return 0;
}