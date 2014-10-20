#include "server.hh"


WebSock_Server::WebSock_Server(string ip, int port, int max){
	clients = new client_info[max];
	max_ = max;
	
	for(int i=0;i<max;i++){
		clients[i].fd = -1;
	}
		

	bzero(&serv_address, sizeof(serv_address));
	serv_address.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &(serv_address.sin_addr));
	serv_address.sin_port = htons(port);
}


int WebSock_Server::start(){
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		return -1;
	}

	if(bind_res = bind(listenfd,(struct sockaddr*)&serv_address, 
		sizeof(serv_address))<0){

		perror("bind");
		return -1;
	}

	if(listen_res =  listen(listenfd, 100)<0){
		perror("listen");
		return -1;
	}
	
	cout<<"\nWebSocket SERVER started...\n";
	return 1;
}

WebSock_Server::~WebSock_Server(){
	delete [] clients; 
}

int WebSock_Server::recv_size(int fd, int *size)
{
	if(ioctl(fd,FIONREAD, size)>=0)
		return 1;
	
	perror("ioctl:");
	return -1;
}

string WebSock_Server::get_string(string buf, string phrase){
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

string WebSock_Server::create_srv_handshake(int i){
	string response = "";

	response+="HTTP/1.1 101 Switching Protocols\r\n";
	response+="Upgarde: " + clients[i].upgrade + "\r\n";
	response+="Connection: " + clients[i].connection + "\r\n";
	response+="Sec-WebSocket-Accept: "+get_accept(clients[i].websock_key) + "\r\n";
	response+="Protocol: chat\r\n\r\n";

	return response;

}

bool WebSock_Server::parse_request(string buf, int i){

	clients[i].checked = true;

	if((clients[i].host = get_string(buf, "Host: "))== ""){
		return false;
	}

	if((clients[i].upgrade = get_string(buf, "Upgrade: ")) == ""){
		return false;
	}

	if((clients[i].connection = get_string(buf, "Connection: ")) == ""){
		return false;
	}

	if((clients[i].websock_key = get_string(buf,"Sec-WebSocket-Key: ")) == ""){
		return false;
	}

	if((clients[i].websock_version = get_string(buf,"Sec-WebSocket-Version: ")) == ""){
		return false;
	}

	if((clients[i].origin = get_string(buf, "Origin: ")) == ""){
		return false;
	}	
	
	return true;

}

int WebSock_Server::accept_conn()
{
	client_info *info;
	int connfd;
	int ident;
	
	if(listenfd<0 || bind_res<0 || listen_res <0)
	{
		cout<<"\n	Error: cannot accept connection!!!\n";
		return -1;
	}	

	for(int i = 0; i<max_; i++)
	{
		if(clients[i].fd<0)
		{
			ident =i;
			info = &clients[i];
		}
	}

	socklen_t addr_size = sizeof(info->address);
	if((connfd = accept(listenfd, &(info->address), &addr_size)) < 0)
	{
		perror("accept()");
		return -1;
	}else info->fd = connfd;	
	
	fd_set rset, wset;
	FD_ZERO(&rset);
	FD_SET(connfd, &rset);

	if(select(8*sizeof(rset),&rset,NULL,NULL,NULL)<0)
	{
		perror("select()");
		close(connfd);
		info->fd = -1;
		return -1;
	}

	char *buf;
	int size;	
	bool no_error = true;


	if(FD_ISSET(connfd,&rset) && (recv_size(connfd, &size) > 0))
	{
		buf = new char[size];
		if(recv(connfd, buf, size, 0)<0)
		{	
			no_error = false;
			perror("recv()");
			
		}
		else if(parse_request(buf, ident))
		{
			string response = create_srv_handshake(ident);
			if ((send(connfd, &response[0], response.length(), 0)) < 0)
			{
				no_error = false;
				perror("send()");						
			}
		}	
	}

	delete [] buf;

	if(!no_error)
	{
		close(connfd);
		info->fd = -1;
		return -1;
	}

	cout<<"\n	Connection accepted...\n";
	return ident;
}

