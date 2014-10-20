#ifndef __SERVER__
#define __SERVER__

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
#include "../lib/websock_lib.hh"

struct client_info{
	int fd;
	struct sockaddr address;
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


using namespace std;

class WebSock_Server{
	int max_;
	client_info *clients;	
	bool checked;
	int listenfd, sockfd, bind_res, listen_res;

	int recv_size(int fd, int *size);
	string get_string(string buf, string phrase);
	string create_srv_handshake(int i);
	struct sockaddr_in serv_address;
	bool parse_request(string buf, int i);

public:	
	~WebSock_Server();
	WebSock_Server(string address, int port, int max_clients);
	int start();
	int accept_conn();
	int close_conn(int i);		 
};

#endif
