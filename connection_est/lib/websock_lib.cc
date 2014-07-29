#include "websock_lib.hh"

unsigned int convert_to_dec(char c) {
	if(c>='0' && c<='9') {
		return c-48;
	} else {
		switch(c) {
			case 'a': return 10;
			case 'b': return 11;
			case 'c': return 12;
			case 'd': return 13;
			case 'e': return 14;
			case 'f': return 15;
		}
	}
}

char get_base64_char(unsigned int c) {
	if(c>=0 && c<=25) {
		return c+65;
	} else if(c>=26 && c<=51) {
		return c+71;
	} else if(c>=52 && c<=61) {
		return c-4;
	} else if(c==62) {
		return 43;
	} else if(c==63) {
		return 47;
	} else {
		cerr << "not hex" << endl;
		return -1;
	}
}

string generate_random_base64() {
	string base64;
	int ch;

	for(int i=0;i<22;++i) {
		ch=rand()/(RAND_MAX/64)+1;
		if(ch>=1 && ch<=26) {
			base64+=ch+64;
		} else if(ch>=27 && ch<=52) {
			base64+=ch+70;
		} else if(ch>=53 && ch<=62) {
			base64+=ch-5;
		} else if(ch==63) {
			base64+="+";
		} else {
			base64+="/";
		}
	}
	base64+="==";
	return base64;
}

string base64_encode_sha1(string unencoded) {
	string base;
	unsigned int num=0;
	unsigned int mask=63;

	for(int i=0;i<13;++i) {
		for(int j=0;j<3;++j) {
			num<<=4;
			num+=convert_to_dec(unencoded[0]);
			unencoded.erase(0, 1);
		}
		mask<<=6;
		base+=get_base64_char((num&mask)>>6);
		mask>>=6;
		base+=get_base64_char(num&mask);
		num=0;
	}
	num=convert_to_dec(unencoded[0]);
	num<<=2;
	base+=get_base64_char(num);
	base+='=';
	return base;
}

string create_opening_handshake(string hostname, string websocket_key) {
	string get;

	get+="GET / HTTP/1.1\r\n";
	get+="Host: "+hostname+"\r\n";
	get+="Upgrade: websocket\r\n";
	get+="Connection: Upgrade\r\n";
	get+="Sec-WebSocket-Key: "+websocket_key+"\r\n";
	get+="Origin: "+hostname+"\r\n";
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