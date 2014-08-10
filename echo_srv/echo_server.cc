#include "echo_server.hh"


Echo_server::~Echo_server(){
	delete[] client;
	delete[] cli_info;
}

string Echo_server::create_serv_handshake(){
	string response = "";

	response+="HTTP/1.1 101 Switching Protocols\r\n";
	response+="Upgarde: " + cli_info[i].upgrade + "\r\n";
	response+="Connection: " + cli_info[i].connection + "\r\n";
	response+="Sec-WebSocket-Accept: "+base64_encode_sha1(cli_info[i].websock_key) + "\r\n";
	response+="Protocol: chat\r\n";

	return response;

}

Echo_server::Echo_server(string ip, int port, int mx_open)
:open_max_(mx_open), cli_info(new cli_request[mx_open+1]), 
client(new pollfd[mx_open+2]), maxi_(0), clicount(-1)
{
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family =AF_INET;
	inet_pton(AF_INET, ip.c_str(), &(servaddr.sin_addr));
	servaddr.sin_port = htons(port);
}

string Echo_server::get_string(string buf, string phrase){
	size_t pos = buf.find(phrase);

	if(pos == string::npos){
		return "";
	}

	int len = phrase.length();
	string key = "";

	for(int i=pos+len ;; i++){ 

		if(buf[i]=='\r' || buf[i]=='\n' || i == buf.length())
			break;

		key.push_back(buf[i]);
	}
	return key;
}

void Echo_server::close_cli_conn(int count){
	
	if(close(client[count].fd)<0)
		cout << "Close() Error !!!"<<endl;

	client[count].fd = -1;
	clicount-=1;

	bzero(cli_info,sizeof(cli_info));
	cli_info[i].rows_checked=0;
}

bool Echo_server::parse_request(string buf){

	cli_info[i].host = get_string(buf, "Host: ");
	cli_info[i].upgrade = get_string(buf, "Upgrade: ");
	cli_info[i].connection = get_string(buf, "Connection: ");
	cli_info[i].websock_key = get_string(buf,"Sec-WebSocket-Key: ");
	cli_info[i].websock_version = get_string(buf,"Sec-WebSocket-Version: ");
	cli_info[i].origin = get_string(buf, "Origin: ");

	if(cli_info[i].host == ""){
		return false;
	}

	if(cli_info[i].upgrade == ""){
		return false;
	}

	if(cli_info[i].connection == ""){
		return false;
	}

	if(cli_info[i].origin == ""){
		return false;
	}

	if(cli_info[i].websock_key == ""){
		return false;
	}

	if(cli_info[i].websock_version == ""){
		return false;
	}	
	
	cli_info[i].rows_checked = 1;

	return true;

}


int Echo_server::initiate_server(){
		if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket:");
		return -1;
	}
	

	
	if(bind(listenfd,(struct sockaddr*)&servaddr, sizeof(servaddr))){
		perror("bind");
		return -1;
	}

	if(listen(listenfd, 100) < 0){
		perror("listen:");
		return -1;
	}

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;

	for(i=1; i<open_max_; i++){
		client[i].fd = -1;
		cli_info[i].rows_checked = 0;
	}

	int nready;
	for (;;){
		nready = poll(client, maxi_+1, -1);

		if((client[0].revents & POLLRDNORM) && maxi_ <= open_max_){
			printf("MAXI%d\n",maxi_);
			// if (maxi_==open_max_){
			// 	printf("too many clients\n");
			// 	return -1;
			// }

			socklen_t clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, &cliaddr, &clilen))<0)
				perror("accept");
			
			for(i=1; i< open_max_+2; i++)
				if(client[i].fd < 0){
					client[i].fd = connfd;
					break;
				}

			client[i].events = POLLRDNORM;

			if(i>maxi_)
				maxi_ = i;

		}

		for(i=1; i<=maxi_; i++){

			if((sockfd = client[i].fd) < 0)
				continue;

			if (client[i].revents & (POLLRDNORM | POLLERR)){
				int size,n;
				
				if(ioctl(client[i].fd, FIONREAD, &size)>=0){
					string buf(size, 0);

					if ((n = read(client[i].fd, &buf[0], size))<0){

						if (errno == ECONNRESET){
							close_cli_conn(i);
						} else
							perror("read error");

					}else if (n==0){

						close_cli_conn(i);

					} else{
						if(parse_request(buf)){
							printf("Client: %d\n",i);
							cout<< cli_info[i].host << endl;
							cout<< cli_info[i].upgrade <<  endl;
							cout<< cli_info[i].connection<<  endl;

							cout<< cli_info[i].websock_key <<  endl;
							cout<< cli_info[i].origin <<  endl;

							cout<< cli_info[i].websock_version <<endl;

							
						}else{
							close_cli_conn(i);
						}
					}
					if(--nready <=0)
						break;
				}
			}
		}
	}

	return 0;
}