#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include "lib/websock_lib.hh"

int main(int argc, char const *argv[])
{
	srand((unsigned)time(NULL));
/*	string key;
	string hostname(argv[1]);
	string get_request;
	struct hostent *h;
	struct sockaddr_in sin;
	int fd;
	ssize_t remaining, n_written;
	const char* cp;
	char buf[1024];
	char server_handshake[1024];
	memset(server_handshake, '\0', 1024);
	memset(buf, '\0', 1024);

	h=gethostbyname(hostname.c_str());

	if( (fd=socket(AF_INET, SOCK_STREAM, 0))<0) {
		perror("socket error");
		return -1;
	}

	sin.sin_family=AF_INET;
	sin.sin_port=htons(80);
	sin.sin_addr=*(struct in_addr*)h->h_addr;

	if(connect(fd, (struct sockaddr*)&sin, sizeof(sin))<0) {
		perror("connect error");
		close(fd);
		return -1;
	}

	key+=generate_random_base64();
	get_request=create_opening_handshake(hostname, key);

	cp=get_request.c_str();
	remaining=get_request.size();

	while(remaining) {
		if( (n_written=send(fd, cp, remaining, 0))<=0) {
			perror("send");
			return 1;
		}
		remaining-=n_written;
		cp+=n_written;
	}

	cout << get_request << endl;
	cout << get_accept(key) << endl;

	bool server_receiving=true;
	while(server_receiving) {
		ssize_t result=recv(fd, buf, sizeof(buf), 0);
		if(buf[strlen(buf)-1]=='\n' && buf[strlen(buf)-2]=='\r' && buf[strlen(buf)-3]=='\n' && buf[strlen(buf)-4]=='\r') {
			server_receiving=false;
		}

		if(result<0) {
			perror("recv");
			close(fd);
			return 1;
		} else if(result==0) {
			break;
		}
		memcpy(server_handshake, buf, strlen(buf));
	}
	cout << approve_server_handshake(server_handshake, get_accept(key)) << endl;

	close(fd);*/
	return 0;
}