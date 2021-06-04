#include <crypto++/cryptlib.h>
#include <crypto++/hex.h>
#include <crypto++/sha.h>
#include <string>

using namespace std;
using namespace CryptoPP;

string sha256(string text) {
    SHA256 hash;
    string digest, res;
    hash.Update((const byte*)text.data(), text.size());
    digest.resize(hash.DigestSize());
    hash.Final((byte*)&digest[0]);
    HexEncoder encoder(new StringSink(res), false);
    StringSource(digest, true, new Redirector(encoder));
    return res;
}