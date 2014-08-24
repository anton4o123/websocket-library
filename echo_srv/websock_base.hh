#ifndef __WEBSOCK_BASE__
#define __WEBSOCK_BASE__

#include "../echo/lib/websock_lib.hh"

using namespace std;

class WebSock_Base
{
protected:
	string gen_accept_key(string websocket_key);
	char *create_frame(string data, unsigned long *size);
	void mask_data(char *frame, unsigned long size);	
};

#endif