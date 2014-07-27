#ifndef __BASE64_SHA1__
#define __BASE64_SHA1__

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

unsigned int convert_to_dec(char c);
char get_base64(unsigned int c);
string generate_random_base64();
string base64_encode_sha1(string unencoded);

#endif