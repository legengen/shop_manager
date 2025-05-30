#pragma once
#include <sqlite3.h>
#include <string>

class database {
public:
    database(const std::string& filepath);
    ~database();

    bool exec(const std::string sql);
    sqlite3_stmt* prepare(const std::string sql);

    sqlite3* get_handle() const;
private:
    sqlite3* db_;
};