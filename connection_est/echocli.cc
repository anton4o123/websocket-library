#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

unsigned long generate_random_num() {
	return rand();
}

unsigned int convert_to_dec(char c) {
	if(c>='0' && c<='9') {
		return c-48;
	} else {
		switch(c) {
			case 'a': return 10;
			case 'b': return 11;
			case 'c': return 12;
			case 'd': return 13;
			case 'e': return 14;
			case 'f': return 15;
		}
	}
}

char get_base64_char(unsigned int c) {
	if(c>=0 && c<=25) {
		return c+65;
	} else if(c>=26 && c<=51) {
		return c+71;
	} else if(c>=52 && c<=61) {
		return c-4;
	} else if(c==62) {
		return 43;
	} else if(c==63) {
		return 47;
	} else {
		cerr << "not hex" << endl;
		exit(-1);
	}
}

string base64_encode_sha1(string unencoded) {
	string base;
	unsigned int num=0;
	unsigned int mask=63;

	for(int i=0;i<13;++i) {
		for(int j=0;j<3;++j) {
			num<<=4;
			num+=convert_to_dec(unencoded[0]);
			unencoded.erase(0, 1);
		}
		mask<<=6;
		base+=get_base64_char((num&mask)>>6);
		mask>>=6;
		base+=get_base64_char(num&mask);
		num=0;
	}
	num=convert_to_dec(unencoded[0]);
	num<<=2;
	base+=get_base64_char(num);
	return base;
}

int main(int argc, char const *argv[])
{
	srand((unsigned)time(NULL));

	cout << base64_encode_sha1("1d29ab734b0c9585240069a6e4e3e91b61da1969") << endl;
	cout << generate_random_num() << endl;
	return 0;
}