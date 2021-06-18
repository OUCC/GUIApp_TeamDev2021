#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

using namespace std;

string binary_to_hexstring(unsigned char* binary, int size) {
    stringstream ss;
    ss << hex << setfill('0');
    for(int i = 0; i < size; i++) {
        ss << setw(2) << (int)binary[i];
    }
    return ss.str();
}

string sha256(string text) {
    const char* cstr = text.c_str();
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((const char*) cstr, sizeof(cstr) - 1, digest);
    return binary_to_hexstring(digest, SHA256_DIGEST_LENGTH);
}