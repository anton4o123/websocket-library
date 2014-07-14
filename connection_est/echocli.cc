#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "lib/base64_sha1.hh"
#include <openssl/sha.h>

unsigned long generate_random_num() {
	return rand();
}

int main(int argc, char const *argv[])
{
	srand((unsigned)time(NULL));
	string key;
	unsigned char* c=new unsigned char[SHA_DIGEST_LENGTH];

	cin >> key;

	c=SHA1((unsigned char*)key.c_str(), key.size(), NULL);

	for(int i=0;i<20;++i) {
		printf("%02x", c[i]);
	}
	printf("\n");
	cout << generate_random_num() << endl;

	return 0;
}