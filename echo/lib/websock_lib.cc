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
	return 16;//error
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

string WebSocket::create_opening_handshake(string hostname, string websocket_key) {
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

string WebSocket::get_accept(string websocket_key) {
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

bool WebSocket::approve_server_handshake(string handshake, string expected_accept) {
	string delim("\r\n");
	string token;
	size_t pos=0;
	bool http=false, key=false;

	while((pos=handshake.find(delim))!=string::npos) {
		token=handshake.substr(0, pos);
		if(token.find("HTTP/1.1 101")!=string::npos) {
			http=true;
		}

		if(token.find("Sec-WebSocket-Accept")!=string::npos) {
			token.erase(0, 22);
			if(token==expected_accept) {
				key=true;
			}
		}
		handshake.erase(0, pos+delim.length());
	}
	
	return key && http;
}

void WebSocket::mask_data(char *frame, unsigned long size) {
	unsigned short index_ch_data=6;
	unsigned short index_ch_mask=2;

	if((frame[1]^(char)254)==0 || (frame[1]^(char)126)==0) {
		index_ch_data=8;
		index_ch_mask=4;
	} else if((frame[1]^(char)255)==0 || (frame[1]^(char)127)==0) {
		index_ch_data=14;
		index_ch_mask=10;
	}

	if(frame[1]&(1<<7)) {
		for(unsigned long i=0;i<size;++i) {
			frame[i+index_ch_data]^=frame[i%4+index_ch_mask];
		}
	}
}

char *WebSocket::create_frame(string data, unsigned long *size) {
	*size=6+data.size();
	short payload=0;
	short mask_index_ch=2;
	if(*size-6>125 && *size-6<65536) {
		*size+=2;
		mask_index_ch+=2;
		payload=1;
	} else if(*size-6>=65536) {
		*size+=8;
		mask_index_ch+=8;
		payload=2;
	}
	char *frame=(char*)malloc((*size+1)*sizeof(char));
	char *data_to_be_masked=(char*)malloc(sizeof(char)*(data.size()+1));
	memset(frame, '\0', *size+1);
	unsigned int mask;

	data_to_be_masked=(char*)data.c_str();
	data_to_be_masked[data.size()]='\0';

	frame[0]|=(1<<7);
	frame[0]|=1;

	if(payload==1) {
		frame[1]|=126;
		unsigned short payload_length=htobe16(data.size());
		memcpy(frame+2, &payload_length, sizeof(unsigned short));
	} else if(payload==2) {
		frame[1]|=127;
		unsigned long payload_long_length=htobe64(data.size());
		memcpy(frame+2, &payload_long_length, sizeof(unsigned long));
	} else {
		unsigned short length=data.size();
		memcpy(frame+1, &length, 1);
	}
	frame[1]|=(1<<7);

	mask=(rand()/(RAND_MAX/2)+1)*(rand());
	memcpy(frame+mask_index_ch, &mask, sizeof(unsigned int));
	memcpy(frame+mask_index_ch+4, data_to_be_masked, strlen(data_to_be_masked));
	mask_data(frame, data.size());
	
	return frame;
}

WebSocket::WebSocket(short secure)
: secure_(secure) {}

void WebSocket::connect_to(string hostname) {
	struct hostent *h;
	struct sockaddr_in sin;
	string key;
	string get_request;
	const char *cp;
	ssize_t n_written;
	char buf[MAX_ACCEPT];

	h=gethostbyname(hostname.c_str());
	if( (fd=socket(AF_INET, SOCK_STREAM, 0))<0) {
		perror("socket error");
		return;
	}

	sin.sin_family=AF_INET;
	sin.sin_addr=*(struct in_addr*)h->h_addr;
	if(secure_==WEB_WS) {
		sin.sin_port=htons(80);
	} else if(secure_==WEB_WSS) {
		sin.sin_port=htons(443);
	}

	if(connect(fd, (struct sockaddr*)&sin, sizeof(sin))<0) {
		perror("connect error");
		return;
	}

	key+=generate_random_base64();
	get_request+=create_opening_handshake(hostname, key);
	cp=get_request.c_str();

	ssize_t remaining=get_request.size();
	while(remaining) {
		if( (n_written=send(fd, cp, remaining, 0))<=0) {
			perror("send");
			close(fd);
			return;
		}
		remaining-=n_written;
		cp+=n_written;
	}

	bool server_receiving=true;
	while(server_receiving) {
		ssize_t result=recv(fd, buf, sizeof(buf), 0);
		if(buf[strlen(buf)-1]=='\n' && buf[strlen(buf)-2]=='\r' && buf[strlen(buf)-3]=='\n' && buf[strlen(buf)-4]=='\r') {
			server_receiving=false;
		}

		if(result<0) {
			perror("recv");
			close(fd);
			return;
		} else if(result==0) {
			break;
		}
	}

	if(approve_server_handshake(buf, get_accept(key))) { 
		cout << "connected" << endl;
	} else {
		cout << "connection failed" << endl;
		close(fd);
	}
}

void WebSocket::send_text(string data) {
	unsigned long size=0;
	char *frame=create_frame(data, &size);

	write(fd, frame, size);
	free(frame);
}

void WebSocket::receive(char *buf) {
//	read(fd, buf, 10000);
	unsigned short frame_size=2;
	unsigned long data_size=0;
	unsigned short buf_change=0;
	unsigned long remaining=0;

	read(fd, buf, 2);
	buf_change+=2;

	if((buf[1]^254)==0 || (buf[1]^126)==0) {
		read(fd, buf+buf_change, 2);
		buf_change+=2;
		frame_size+=2;
		memcpy(&data_size, buf+2, 2);
		data_size=be16toh(data_size);
	} else if((buf[1]^255)==0 || (buf[1]^127)==0) {
		read(fd, buf+buf_change, 8);
		buf_change+=8;
		frame_size+=8;
		memcpy(&data_size, buf+2, 8);
		data_size=be64toh(data_size);
	} else {
		data_size=buf[1];
	}

	if((buf[1]&(1<<7))) {
		remaining+=4;
		frame_size+=4;
		data_size-128;
	}

	remaining+=data_size;
	unsigned long check;
	while(remaining) {
		check=read(fd, buf+buf_change, remaining);
		remaining-=check;
		buf_change+=check;
	}

	mask_data(buf, data_size);
	memmove(buf, buf+frame_size, data_size);
	buf[data_size]='\0';
}

void WebSocket::disconnect() {
	close(fd);
}