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
	string sh;

	cin >> key;

	cout << SHA1((unsigned char*)key.c_str(), key.size(), NULL) << endl;

	cout << generate_random_num() << endl;

	return 0;
}