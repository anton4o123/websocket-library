#include "lib/websock_lib.hh"

int main(int argc, char const *argv[])
{
	srand((unsigned)time(NULL));
	WebSocket wb(WEB_WS);
	char *buf=(char*)malloc(65650*sizeof(char));
	string haha;

	wb.connect_to(argv[1]);

	cin >> haha;
	wb.send_text(haha);
	wb.receive(buf);
	cout << buf << endl;
	free(buf);
	
	wb.disconnect();
	return 0;
}