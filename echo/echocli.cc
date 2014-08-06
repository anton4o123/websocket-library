#include "lib/websock_lib.hh"

int main(int argc, char const *argv[])
{
	srand((unsigned)time(NULL));
	WebSocket wb(WEB_WS);
	char *buf=(char*)malloc(65650*sizeof(char));
	string echo_str;

	wb.connect_to(argv[1]);

	while(true) {
		cin >> echo_str;
		if(!cin) {
			break;
		}
		wb.send_text(echo_str);
		wb.receive(buf);
		cout << buf << endl;
	}

	free(buf);
	
	wb.disconnect();
	return 0;
}