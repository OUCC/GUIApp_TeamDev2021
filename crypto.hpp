#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <Siv3D.hpp>

using namespace std;

String binary_to_hexstring(unsigned char* binary, int size) {
    stringstream ss;
    ss << hex << setfill('0');
    for(int i = 0; i < size; i++) ss << setw(2) << (int)binary[i];
    return Unicode::WidenAscii(ss.str());
}

String binary_to_hexstring(const unsigned char* binary, int size) {
    stringstream ss;
    ss << hex << setfill('0');
    for (int i = 0; i < size; i++) ss << setw(2) << (int)binary[i];
    return Unicode::WidenAscii(ss.str());
}

String sha256(String text) {
    char c[64] = {};
    unsigned char digest[SHA256_DIGEST_LENGTH];
    string str = Unicode::NarrowAscii(text);
    strncpy(c, str.c_str(), sizeof(c));

    const unsigned char* cuc = (const unsigned char*)c;
    SHA256(cuc, str.length(), digest);

    return binary_to_hexstring(digest, SHA256_DIGEST_LENGTH);
}

String aes256_encrypt(String text, String key) {
    char text_c[64] = {};
    string text_string = Unicode::NarrowAscii(text);
    strncpy(text_c, text_string.c_str(), sizeof(text_c));
    const unsigned char* text_cuc = (const unsigned char*)text_c;

    char key_c[64] = {};
    string key_string = Unicode::NarrowAscii(key);
    strncpy(key_c, key_string.c_str(), sizeof(key_c));
    const unsigned char* key_cuc = (const unsigned char*)key_c;

    unsigned char enc_out[256];
    AES_KEY enc_key;
    AES_set_encrypt_key(key_cuc, 256, &enc_key);
    AES_encrypt(text_cuc, enc_out, &enc_key);

    return binary_to_hexstring(enc_out, 256);
}

String aes256_decrypt(String hex_text, String key) {
    unsigned char binary_text[256];
    string hex_text_string = Unicode::NarrowAscii(hex_text);
    for (int i = 0; i < 256; i++) {
        binary_text[i] = (unsigned char)stol(hex_text_string.substr(i * 2, 2), nullptr, 16);
    }

    char key_c[64] = {};
    string key_string = Unicode::NarrowAscii(key);
    strncpy(key_c, key_string.c_str(), sizeof(key_c));
    const unsigned char* key_cuc = (const unsigned char*)key_c;

    unsigned char dec_out[256];
    AES_KEY dec_key;
    AES_set_decrypt_key(key_cuc, 256, &dec_key);
    AES_decrypt(binary_text, dec_out, &dec_key);

    char res[256];
    for (int i = 0; i < 256; i++) res[i] = dec_out[i];

    return Unicode::WidenAscii(std::string(res));
}