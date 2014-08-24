#include "websock_base.hh"

string WebSock_Base::gen_accept_key(string websocket_key){
	websocket_key+="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	unsigned char* c=new unsigned char[SHA_DIGEST_LENGTH];
	stringstream accept_hex;
	accept_hex.fill('0');
	accept_hex << hex;
	
	c=SHA1((unsigned char*)websocket_key.c_str(), websocket_key.size(), NULL);
	for(int i=0;i<20;++i) {
		accept_hex << setw(2) << (unsigned int)c[i];
	}
	return base64_encode_sha1(accept_hex.str());
}


char *WebSock_Base::create_frame(string data, unsigned long *size){
	*size=6+data.size();
	short payload=0;
	short mask_index_ch=2;
	if(*size-6>125 && *size-6<65536) {
		*size+=2;
		mask_index_ch+=2;
		payload=1;
	} else if(*size-6>=65536) {
		*size+=8;
		mask_index_ch+=8;
		payload=2;
	}
	char *frame=(char*)malloc((*size+1)*sizeof(char));
	char *data_to_be_masked=(char*)malloc(sizeof(char)*(data.size()+1));
	memset(frame, '\0', *size+1);
	unsigned int mask;

	data_to_be_masked=(char*)data.c_str();
	data_to_be_masked[data.size()]='\0';

	frame[0]|=(1<<7);
	frame[0]|=1;

	if(payload==1) {
		frame[1]|=126;
		unsigned short payload_length=htobe16(data.size());
		memcpy(frame+2, &payload_length, sizeof(unsigned short));
	} else if(payload==2) {
		frame[1]|=127;
		unsigned long payload_long_length=htobe64(data.size());
		memcpy(frame+2, &payload_long_length, sizeof(unsigned long));
	} else {
		unsigned short length=data.size();
		memcpy(frame+1, &length, 1);
	}
	frame[1]|=(1<<7);

	mask=(rand()/(RAND_MAX/2)+1)*(rand());
	memcpy(frame+mask_index_ch, &mask, sizeof(unsigned int));
	memcpy(frame+mask_index_ch+4, data_to_be_masked, strlen(data_to_be_masked));
	mask_data(frame, data.size());
	
	return frame;
}


void WebSock_Base::mask_data(char* frame, unsigned long size){
	unsigned short index_ch_data=6;
	unsigned short index_ch_mask=2;

	if((frame[1]^(char)254)==0 || (frame[1]^(char)126)==0) {
		index_ch_data=8;
		index_ch_mask=4;
	} else if((frame[1]^(char)255)==0 || (frame[1]^(char)127)==0) {
		index_ch_data=14;
		index_ch_mask=10;
	}

	if(frame[1]&(1<<7)) {
		for(unsigned long i=0;i<size;++i) {
			frame[i+index_ch_data]^=frame[i%4+index_ch_mask];
		}
	}
}