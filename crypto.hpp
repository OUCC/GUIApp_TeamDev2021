#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <Siv3D.hpp>

using namespace std;

String binary_to_hexstring(unsigned char* binary, int size) {
    stringstream ss;
    ss << hex << setfill('0');
    for(int i = 0; i < size; i++) {
        ss << setw(2) << (int)binary[i];
    }
    return Unicode::FromUTF8(ss.str());
}

String sha256(String text) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    const char* cc = text.toUTF8().c_str();
    const unsigned char* cuc = reinterpret_cast<const unsigned char*>(cc);
    SHA256(cuc, sizeof(cuc) - 1, digest);
    return binary_to_hexstring(digest, SHA256_DIGEST_LENGTH);
}