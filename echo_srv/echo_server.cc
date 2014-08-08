#include "echo_server.hh"

Echo_server::~Echo_server(){
	delete[] client;
	delete[] cli_info;
}

Echo_server::Echo_server(string ip, int port, int mx_open)
:open_max_(mx_open), cli_info(new cli_request[mx_open]), 
client(new pollfd[mx_open]), maxi_(0), clicount(-1)
{
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family =AF_INET;
	inet_pton(AF_INET, ip.c_str(), &(servaddr.sin_addr));
	servaddr.sin_port = htons(port);
}

string Echo_server::get_string(string buf, string phrase, int length){
	size_t pos = buf.find(phrase);
	if(pos == string::npos){
		return "";
	}

	int len = phrase.length();
	string key = "";
	for(int i=pos+len ;; i++){

		if((length==0) && (buf[i]==' ' && buf[i+1]==' '))
			break; 

		if((length!=0) && (i>=(pos+len+length)))
			break;

		 if(!isalnum(buf[i]))
		 	return "";
		key.push_back(buf[i]);
	}
	return key;
}

int Echo_server::close_cli_conn(int count){
	close(client[count].fd);
	client[count].fd = -1;
	clicount-=1;
	bzero(cli_info,sizeof(cli_info));
	cli_info[i].rows_checked=0;
}

bool Echo_server::parse_request(string buf){
	cli_info[i].host = get_string(buf, "Host: ", 0);
	cli_info[i].websock_key = get_string(buf,"Sec-WebSocket-Key: ",22);
	cli_info[i].websock_version = get_string(buf,"Sec-WebSocket-Version: ",0);
	
	if(cli_info[i].host != ""){
		write(client[i].fd, &cli_info[i].host[0],cli_info[i].host.length());
	}

	if(cli_info[i].websock_key != ""){
		write(client[i].fd, &cli_info[i].websock_key[0],cli_info[i].websock_key.length());
	}

	if(cli_info[i].websock_version != ""){
		write(client[i].fd, &cli_info[i].websock_version[0],2);
	}


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

		if(client[0].revents & POLLRDNORM){
			
			if (clicount==open_max_){
				printf("too many clients\n");
				return -1;
			}

			socklen_t clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, &cliaddr, &clilen))<0)
				perror("accept");
			
			for(i=1; i< open_max_; i++)
				if(client[i].fd < 0){
					client[i].fd = connfd;
					break;
				}

			client[i].events = POLLRDNORM;

			if(i>maxi_)
				maxi_ = i;
			clicount+=1;

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
						parse_request(buf);
					}
					if(--nready <=0)
						break;
				}
			}
		}
	}

	return 0;
}
