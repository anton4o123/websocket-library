#ifndef __ECHO_SERVER__
#define __ECHO_SERVER__

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <poll.h>
#include <time.h>
#include <string>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <openssl/sha.h>
#include <sstream>
#include "websock_base.hh"

using namespace std;

class Echo_server : protected WebSock_Base{

	struct cli_request{
		string host;
		string upgrade;
		string connection;
		string websock_key;
		string websock_version;
		string origin;
		string websock_protocol;
		string websock_extension;
		bool checked;
	};

	int  i, maxi_, clicount;
	struct pollfd *client;
	int open_max_;
	struct sockaddr_in servaddr;
	cli_request *cli_info;

	string create_serv_handshake();
	void close_cli_conn(int count);
	bool parse_request(string buf);
	string get_string(string buf, string phrase);
public:
	Echo_server(string ip, int port, int mx_open);
	~Echo_server();
	int initiate_server();

};

#endif