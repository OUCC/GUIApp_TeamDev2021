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
    const char* cc = str.c_str();
    const unsigned char* cuc = reinterpret_cast<const unsigned char*>(cc);
    SHA256(cuc, sizeof(cuc) - 1, digest);
    return binary_to_hexstring(digest, SHA256_DIGEST_LENGTH);
}

String aes256_encrypt(String text, String key) {
    string text_string = Unicode::NarrowAscii(text);
    const char* text_cc = text_string.c_str();
    const unsigned char* text_cuc = reinterpret_cast<const unsigned char*>(text_cc);


    string key_string = Unicode::NarrowAscii(key);
    const char* key_cc = key_string.c_str();
    const unsigned char* key_cuc = reinterpret_cast<const unsigned char*>(key_cc);

    unsigned char enc_out[256];

    AES_KEY enc_key;
    AES_set_encrypt_key(key_cuc, 256, &enc_key);
    AES_encrypt(text_cuc, enc_out, &enc_key);

    //Print << U"original:" << binary_to_hexstring(text_cuc, 256);
    //Print << U"crypted:" << binary_to_hexstring(enc_out, 256);

    return binary_to_hexstring(enc_out, 256);
}

String aes256_decrypt(String hex_text, String key) {
    unsigned char binary_text[256];
    string hex_text_string = Unicode::NarrowAscii(hex_text); //一度置かないとダングリングする
    const char* cstr_text = hex_text_string.c_str();
    int size = hex_text_string.length() / 2;
    for (int i = 0; i < size; i++) {
        binary_text[i] = (unsigned char)stol(hex_text_string.substr(i * 2, 2), nullptr, 16);
    }

    string key_string = Unicode::NarrowAscii(key); //一度置かないとダングリングする
    const char* key_cc = key_string.c_str();
    const unsigned char* key_cuc = reinterpret_cast<const unsigned char*>(key_cc);

    unsigned char dec_out[256];

    AES_KEY dec_key;
    AES_set_decrypt_key(key_cuc, 256, &dec_key);
    AES_decrypt(binary_text, dec_out, &dec_key);

    //Print << U"original:" << binary_to_hexstring(binary_text, 256);
    //Print << U"decrypted:" << binary_to_hexstring(dec_out, 256);
    return Unicode::WidenAscii(std::string(reinterpret_cast<const char*>(dec_out)));
}