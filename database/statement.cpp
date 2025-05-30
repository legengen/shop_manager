#include "statement.hpp"
#include <iostream>

statement::statement(database& db, const std::string& sql): db_(db) {
    stmt_ = db.prepare(sql);
}

statement::~statement() {
    sqlite3_finalize(stmt_);
}

bool statement::bind_int(int index, int value) {
    return sqlite3_bind_int(stmt_, index, value) == SQLITE_OK;
}

bool statement::bind_double(int index, double value) {
    return sqlite3_bind_double(stmt_, index, value) == SQLITE_OK;
}

bool statement::bind_text(int index, const std::string& value) {
    int rc = sqlite3_bind_text(stmt_, index, value.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "[DEBUG][bind_text] idx="<<index
                  <<" rc="<< rc <<": "<< sqlite3_errmsg(sqlite3_db_handle(stmt_)) << "\n";
    }
    return rc == SQLITE_OK;
}

void statement::reset() {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
}

int statement::step() {
    return sqlite3_step(stmt_);
}

int statement::get_int_col(int index) {
    return sqlite3_column_int(stmt_, index);
}

double statement::get_double_col(int index) {
    return sqlite3_column_double(stmt_, index);
}

std::string statement::get_text_col(int index) {
    const char* txt = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, index));
    return txt ? std::string(txt) : std::string("");
}

bool statement::execute() {
    int rc = sqlite3_step(stmt_);
    if (rc == SQLITE_DONE) return true;
    std::cerr << "[SQLite ERROR] execute failed: "
              << sqlite3_errmsg(sqlite3_db_handle(stmt_)) << "\n";
    return false;
}