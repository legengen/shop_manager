#pragma once
#include <sqlite3.h>
#include <string>
#include "database.hpp"

class statement {
public:
    statement(database& db, const std::string& sql);
    ~statement();

    bool bind_int(int index, int value);
    bool bind_double(int index, double value);
    bool bind_text(int index, const std::string& value);
    
    void reset();

    int step();

    bool execute();
    
    int get_int_col(int index);
    double get_double_col(int index);
    std::string get_text_col(int index);
private:
    sqlite3_stmt* stmt_;
    database& db_;
};