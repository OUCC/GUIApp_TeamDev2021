#include "crypto.hpp"
#include "single_data.hpp"
#include "db_exception.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <Siv3D.hpp>

using namespace std;

/**
 * @brief DBの表面上のI/Oを担当するクラス
 * 
 */
class db {
private:
    String key_db_name;
    String main_db_name;
    String key = U"";

    String get_db_key() {
        if (!FileSystem::IsFile(key_db_name)) return U"";
        Deserializer<BinaryReader> reader(key_db_name);
        if (!reader.getReader()) {
            throw Error(U"Failed to open `{}`"_fmt(key_db_name));
        }
        String db_key;
        reader(db_key);
        return db_key;
    }

public:
/**
 * @brief dbのコンストラクタ
 * 
 * @param db_name 使用するDBファイル名
 */
    db(String key_db_name = U"key.dat", String main_db_name = U"main.dat") :
        key_db_name(key_db_name), main_db_name(main_db_name) {}

/**
 * @brief PWマネージャーのパスワードが既に登録されているかを返す
 * 
 * @return true 登録されている場合
 * @return false 登録されていない場合
 * @exception db_exception 複数のキーが登録されていた場合
 */
    bool is_registered() {
        return get_db_key() != U"";
    }

/**
 * @brief PWマネージャーにすでにログインしているかを返す
 * 
 * @return true 既にログインしている場合
 * @return false まだログインできていない場合
 */
    bool is_logined() {
        return key != U"";
    }

/**
 * @brief PWマネージャーのパスワードを登録している
 * 
 * @param passwd 登録するパスワード
 * @exception db_exception パスワードが既に登録されている場合、複数のキーが登録されていた場合
 */
    void register_passwd(String passwd) {
        String db_key = get_db_key();
        if(db_key != U"") {
            throw db_exception("a key has already been registered");
        }
        db_key = sha256(passwd);
        Serializer<BinaryWriter> writer(key_db_name);
        if (!writer.getWriter()) {
            throw Error(U"Failed to open `{}`"_fmt(key_db_name));
        }
        writer(db_key);
        key = passwd;
    }

/**
 * @brief PWマネージャーにログインする
 * 
 * @param passwd ログインパスワード
 * @return true パスワードが正しく、ログインに成功した場合
 * @return false パスワードが間違っていてログインに失敗した場合
 * @exception db_exception パスワードが未登録の場合、複数のキーが登録されていた場合
 */
    bool login(String passwd) {
        String hash = sha256(passwd);
        String db_key = get_db_key();
        if(db_key == U"") {
            throw db_exception("no key has been registered");
        }
        if(hash != db_key) {
            return false;
        }
        key = passwd;
        return true;
    }
};
