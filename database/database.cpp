#include "database.hpp"
#include <iostream>
using namespace std;

database::database(const std::string& filepath): db_(nullptr){
    int rc;
    rc = sqlite3_open(filepath.c_str(), &db_);
    if(rc != SQLITE_OK){
        cerr << "opendb: failed " << sqlite3_errmsg(db_) << endl;
    }
}

database::~database(){
    if(db_) sqlite3_close(db_);
}

bool database::exec(const std::string sql){
    int rc;
    char* errmsg = nullptr;
    rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);
    if(rc != SQLITE_OK){
        cerr << "exec: failed " << errmsg << endl;
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}

sqlite3_stmt* database::prepare(const std::string sql){
    int rc;
    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "[SQLite ERROR] prepare failed: "
                  << sqlite3_errmsg(db_) << "\n"
                  << "  SQL: " << sql << "\n";
    }
    return stmt;
}

sqlite3* database::get_handle() const {
    return db_;
}