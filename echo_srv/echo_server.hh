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

using namespace std;

class Echo_server{

	int listenfd, connfd, i, maxi_, sockfd, clicount;
	struct pollfd *client;
	struct sockaddr cliaddr;
	int open_max_;
	struct sockaddr_in servaddr;
	string *websock_key;
	int close_cli_conn(int count);
	string get_string(string buf, string phrase, int length);
public:
	Echo_server(string ip, int port, int mx_open);
	int initiate_server();
};

#endif