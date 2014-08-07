#include "echo_server.hh"


int main(){
	Echo_server srv("127.0.0.1",9877,23);
	srv.initiate_server();
	
	return 0;
}