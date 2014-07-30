#include "lib/websock_lib.hh"

int main(int argc, char const *argv[])
{
	srand((unsigned)time(NULL));
	WebSocket wb(WEB_WS);

	wb.connect_to(argv[1]);
	wb.send_text("haha");

	wb.disconnect();
	return 0;
}