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

    virtual ~db() {
        sqlite3_close_v2(conn);
    }

    bool is_registered() {
        return get_db_key() != "";
    }

    bool is_logined() {
        return key != "";
    }

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
