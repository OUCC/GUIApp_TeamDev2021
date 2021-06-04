#include "crypto.hpp"
#include "single_data.hpp"
#include "sql_exception.hpp"
#include <iostream>
#include <sqlite3.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace CryptoPP;
using namespace std;

class db {
private:
    const char* DB_NAME;
    sqlite3 *conn;
    string key = "";

    bool has_table(string name) {
        char stmt[128];
        sqlite3_stmt* pStmt;
        sprintf(stmt, "SELECT COUNT(*) FROM sqlite_master WHERE TYPE=\'table\' "
            "AND name = \'%s\';", name.c_str());
        int status = sqlite3_prepare_v2(conn, stmt, -1, &pStmt, nullptr);
        if(status != SQLITE_OK) {
            throw sql_exception("couldn't prepare finding table named " + name);
        }
        status = sqlite3_step(pStmt);
        if(status != SQLITE_ROW) {
            throw sql_exception("couln't count table named " + name);
        }
        bool res = sqlite3_column_int(pStmt, 0);
        sqlite3_reset(pStmt);
        sqlite3_finalize(pStmt);
        return res;
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
        if(!has_table("key")) {
            status = sqlite3_exec(conn, "CREATE TABLE key(key INTEGER);", nullptr, nullptr, nullptr);
            if(status != SQLITE_OK) {
                throw sql_exception("couldn't create table named key");
            }
        }
        if(!has_table("passwds")) {
            status = sqlite3_exec(conn, "CREATE TABLE passwds("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "service_name TEXT, "
                "url TEXT, "
                "USER_NAME TEXT, "
                "PASSWD TEXT);", nullptr, nullptr, nullptr);
            if(status != SQLITE_OK) {
                throw sql_exception("couldn't create table named key");
            }
        }
    }

    virtual ~db() {
        sqlite3_close_v2(conn);
    }

    void login(string passwd) {
        key = sha256(passwd);
    }
};
