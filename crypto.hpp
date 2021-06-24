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
    int len = str.length();
    unsigned char uc[64] = {};
    for (int i = 0; i < len; i++) {
        uc[i] = (char)str[i];
    }
    const unsigned char cuc[] = {
        uc[0], uc[1], uc[2], uc[3], uc[4], uc[5], uc[6], uc[7],
        uc[8], uc[9], uc[10], uc[11], uc[12], uc[13], uc[14], uc[15],
        uc[16], uc[17], uc[18], uc[19], uc[20], uc[21], uc[22], uc[23],
        uc[24], uc[25], uc[26], uc[27], uc[28], uc[29], uc[30], uc[31],
        uc[32], uc[33], uc[34], uc[35], uc[36], uc[37], uc[38], uc[39],
        uc[40], uc[41], uc[42], uc[43], uc[44], uc[45], uc[46], uc[47],
        uc[48], uc[49], uc[50], uc[51], uc[52], uc[53], uc[54], uc[55],
        uc[56], uc[57], uc[58], uc[59], uc[60], uc[61], uc[62], uc[63]
    };
    SHA256(cuc, len, digest);
    return binary_to_hexstring(digest, SHA256_DIGEST_LENGTH);
}

String aes256_encrypt(String text, String key) {
    string text_string = Unicode::NarrowAscii(text);
    int text_len = text_string.length();
    unsigned char text_uc[64] = {};
    for (int i = 0; i < text_len; i++) {
        text_uc[i] = (char)text_string[i];
    }
    const unsigned char text_cuc[] = {
        text_uc[0], text_uc[1], text_uc[2], text_uc[3], text_uc[4], text_uc[5], text_uc[6], text_uc[7],
        text_uc[8], text_uc[9], text_uc[10], text_uc[11], text_uc[12], text_uc[13], text_uc[14], text_uc[15],
        text_uc[16], text_uc[17], text_uc[18], text_uc[19], text_uc[20], text_uc[21], text_uc[22], text_uc[23],
        text_uc[24], text_uc[25], text_uc[26], text_uc[27], text_uc[28], text_uc[29], text_uc[30], text_uc[31],
        text_uc[32], text_uc[33], text_uc[34], text_uc[35], text_uc[36], text_uc[37], text_uc[38], text_uc[39],
        text_uc[40], text_uc[41], text_uc[42], text_uc[43], text_uc[44], text_uc[45], text_uc[46], text_uc[47],
        text_uc[48], text_uc[49], text_uc[50], text_uc[51], text_uc[52], text_uc[53], text_uc[54], text_uc[55],
        text_uc[56], text_uc[57], text_uc[58], text_uc[59], text_uc[60], text_uc[61], text_uc[62], text_uc[63]
    };

    string key_string = Unicode::NarrowAscii(key);
    int key_len = key_string.length();
    unsigned char key_uc[64] = {};
    for (int i = 0; i < key_len; i++) {
        key_uc[i] = (char)key_string[i];
    }
    const unsigned char key_cuc[] = {
        key_uc[0], key_uc[1], key_uc[2], key_uc[3], key_uc[4], key_uc[5], key_uc[6], key_uc[7],
        key_uc[8], key_uc[9], key_uc[10], key_uc[11], key_uc[12], key_uc[13], key_uc[14], key_uc[15],
        key_uc[16], key_uc[17], key_uc[18], key_uc[19], key_uc[20], key_uc[21], key_uc[22], key_uc[23],
        key_uc[24], key_uc[25], key_uc[26], key_uc[27], key_uc[28], key_uc[29], key_uc[30], key_uc[31],
        key_uc[32], key_uc[33], key_uc[34], key_uc[35], key_uc[36], key_uc[37], key_uc[38], key_uc[39],
        key_uc[40], key_uc[41], key_uc[42], key_uc[43], key_uc[44], key_uc[45], key_uc[46], key_uc[47],
        key_uc[48], key_uc[49], key_uc[50], key_uc[51], key_uc[52], key_uc[53], key_uc[54], key_uc[55],
        key_uc[56], key_uc[57], key_uc[58], key_uc[59], key_uc[60], key_uc[61], key_uc[62], key_uc[63]
    };

    unsigned char enc_out[256];

    AES_KEY enc_key;
    AES_set_encrypt_key(key_cuc, 256, &enc_key);
    AES_encrypt(text_cuc, enc_out, &enc_key);

    return binary_to_hexstring(enc_out, 256);
}

String aes256_decrypt(String hex_text, String key) {
    unsigned char binary_text[256];
    string hex_text_string = Unicode::NarrowAscii(hex_text);
    const char* cstr_text = hex_text_string.c_str();
    for (int i = 0; i < 256; i++) {
        binary_text[i] = (unsigned char)stol(hex_text_string.substr(i * 2, 2), nullptr, 16);
    }

    string key_string = Unicode::NarrowAscii(key);
    int key_len = key.length();
    unsigned char key_uc[64] = {};
    for (int i = 0; i < key_len; i++) {
        key_uc[i] = (char)key_string[i];
    }
    const unsigned char key_cuc[] = {
        key_uc[0], key_uc[1], key_uc[2], key_uc[3], key_uc[4], key_uc[5], key_uc[6], key_uc[7],
        key_uc[8], key_uc[9], key_uc[10], key_uc[11], key_uc[12], key_uc[13], key_uc[14], key_uc[15],
        key_uc[16], key_uc[17], key_uc[18], key_uc[19], key_uc[20], key_uc[21], key_uc[22], key_uc[23],
        key_uc[24], key_uc[25], key_uc[26], key_uc[27], key_uc[28], key_uc[29], key_uc[30], key_uc[31],
        key_uc[32], key_uc[33], key_uc[34], key_uc[35], key_uc[36], key_uc[37], key_uc[38], key_uc[39],
        key_uc[40], key_uc[41], key_uc[42], key_uc[43], key_uc[44], key_uc[45], key_uc[46], key_uc[47],
        key_uc[48], key_uc[49], key_uc[50], key_uc[51], key_uc[52], key_uc[53], key_uc[54], key_uc[55],
        key_uc[56], key_uc[57], key_uc[58], key_uc[59], key_uc[60], key_uc[61], key_uc[62], key_uc[63]
    };

    unsigned char dec_out[256];

    AES_KEY dec_key;
    AES_set_decrypt_key(key_cuc, 256, &dec_key);
    AES_decrypt(binary_text, dec_out, &dec_key);

    char res[256];
    for (int i = 0; i < 256; i++) {
        res[i] = dec_out[i];
    }
    return Unicode::WidenAscii(std::string(res));
}