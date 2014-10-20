#include "server.hh"

int main(){
	WebSock_Server ws("127.0.0.1",80,5);
	
	ws.start();

	int i = ws.accept_conn();

	return 0;
}
