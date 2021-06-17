#include "crypto.hpp"
#include "single_data.hpp"
#include "db_exception.hpp"
#include "sql_exception.hpp"
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

using namespace CryptoPP;
using namespace std;

/**
 * @brief DBの表面上のI/Oを担当するクラス
 * 
 */
class db {
private:
    const char* DB_NAME;
    sqlite3 *conn;
    string key = "";

    string get_db_key() {
        char stmt[128] = "SELECT key FROM key;";
        sqlite3_stmt* pStmt;
        int status = sqlite3_prepare_v2(conn, stmt, -1, &pStmt, nullptr);
        if(status != SQLITE_OK) {
            throw sql_exception("couldn't prepare getting key");
        }
        vector<string> keys;
        while(sqlite3_step(pStmt) == SQLITE_ROW) {
            const unsigned char* db_key = sqlite3_column_text(pStmt, 0);
            keys.push_back(reinterpret_cast<const char*>(db_key));
        }
        sqlite3_reset(pStmt);
        sqlite3_finalize(pStmt);

        if(keys.size() > 1) {
            throw db_exception("multiple keys were found");
        }
        if(keys.size() == 0) return "";
        else return keys[0];
    }

public:
/**
 * @brief dbのコンストラクタ、ここでsqlにコネクトして情報を保持する
 * 
 * @param db_name 使用するsqlite3のDBファイル名(相対パス)
 * @exception sql_exception
 */
    db(string db_name="passwd.sqlite3") {
        DB_NAME = db_name.c_str();
        int status = sqlite3_open_v2(
            DB_NAME,
            &conn,
            SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,
            nullptr
        );
        if(status != SQLITE_OK) {
            sqlite3_close_v2(conn);
            throw sql_exception("couldn't open db named " + db_name);
        }
        status = sqlite3_exec(conn, "CREATE TABLE IF NOT EXISTS key(key TEXT);", nullptr, nullptr, nullptr);
        if(status != SQLITE_OK) {
            throw sql_exception("couldn't create table named key");
        }
        status = sqlite3_exec(conn, "CREATE TABLE IF NOT EXISTS passwds("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "service_name TEXT, "
            "url TEXT, "
            "USER_NAME TEXT, "
            "PASSWD TEXT);", nullptr, nullptr, nullptr);
        if(status != SQLITE_OK) {
            throw sql_exception("couldn't create table named passwds");
        }
    }

/**
 * @brief dbのデストラクタ、sqlの接続を切断する
 * 
 */
    virtual ~db() {
        sqlite3_close_v2(conn);
    }

/**
 * @brief PWマネージャーのパスワードが既に登録されているかを返す
 * 
 * @return true 登録されている場合
 * @return false 登録されていない場合
 * @exception db_exception("multiple keys were found") 複数のキーが登録されていた場合
 * @exception sql_exception
 */
    bool is_registered() {
        return get_db_key() != "";
    }

/**
 * @brief PWマネージャーにすでにログインしているかを返す
 * 
 * @return true 既にログインしている場合
 * @return false まだログインできていない場合
 */
    bool is_logined() {
        return key != "";
    }

/**
 * @brief PWマネージャーのパスワードを登録している
 * 
 * @param passwd 登録するパスワード
 * @exception db_exception("a key has already been registered") パスワードが既に登録されている場合
 * @exception db_exception("multiple keys were found") 複数のキーが登録されていた場合
 * @exception sql_exception
 */
    void register_passwd(string passwd) {
        string db_key = get_db_key();
        if(db_key != "") {
            throw db_exception("a key has already been registered");
        }
        string hash = sha256(passwd);
        char stmt[200];
        sprintf(stmt, "INSERT INTO key VALUES(\'%s\');", hash.c_str());
        int status = sqlite3_exec(conn, stmt, nullptr, nullptr, nullptr);
        if(status != SQLITE_OK) {
            throw sql_exception("couldn't insert key");
        }
        key = hash;
    }

/**
 * @brief PWマネージャーにログインする
 * 
 * @param passwd ログインパスワード
 * @return true パスワードが正しく、ログインに成功した場合
 * @return false パスワードが間違っていてログインに失敗した場合
 * @exception db_exception("no key has been registered") パスワードが未登録の場合
 * @exception db_exception("multiple keys were found") 複数のキーが登録されていた場合
 * @exception sql_exception
 */
    bool login(string passwd) {
        string hash = sha256(passwd);
        string db_key = get_db_key();
        if(db_key == "") {
            throw db_exception("no key has been registered");
        }
        if(hash != db_key) {
            return false;
        }
        key = hash;
        return true;
    }
};
