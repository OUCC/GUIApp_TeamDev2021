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
    for(int i = 0; i < size; i++) {
        ss << setw(2) << (int)binary[i];
    }
    return Unicode::WidenAscii(ss.str());
}

String binary_to_hexstring(const unsigned char* binary, int size) {
    stringstream ss;
    ss << hex << setfill('0');
    for (int i = 0; i < size; i++) {
        ss << setw(2) << (int)binary[i];
    }
    return Unicode::WidenAscii(ss.str());
}

String sha256(String text) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    string str = Unicode::NarrowAscii(text);
    const unsigned char* cuc = (const unsigned char*)str.c_str();
    SHA256(cuc, str.length(), digest);
    return binary_to_hexstring(digest, SHA256_DIGEST_LENGTH);
}

String aes256_encrypt(String text, String key) {
    string text_string = Unicode::NarrowAscii(text);
    string key_string = Unicode::NarrowAscii(key);
    const unsigned char* key_cuc = (const unsigned char*)text_string.c_str();
    const unsigned char* text_cuc = (const unsigned char*)key_string.c_str();

    unsigned char enc_out[256];

    AES_KEY enc_key;
    AES_set_encrypt_key(key_cuc, 256, &enc_key);
    AES_encrypt(text_cuc, enc_out, &enc_key);

    return binary_to_hexstring(enc_out, 256);
}

String aes256_decrypt(String hex_text, String key) {
    unsigned char binary_text[256];
    string hex_text_string = Unicode::NarrowAscii(hex_text); //一度置かないとダングリングする
    int size = hex_text_string.length() / 2;
    for (int i = 0; i < size; i++) {
        binary_text[i] = (unsigned char)stol(hex_text_string.substr(i * 2, 2), nullptr, 16);
    }

    string key_string = Unicode::NarrowAscii(key); //一度置かないとダングリングする
    const unsigned char* key_cuc = (const unsigned char*)key_string.c_str();

    unsigned char dec_out[256];

    AES_KEY dec_key;
    AES_set_decrypt_key(key_cuc, 256, &dec_key);
    AES_decrypt(binary_text, dec_out, &dec_key);

    return Unicode::WidenAscii(std::string(reinterpret_cast<const char*>(dec_out)));
}