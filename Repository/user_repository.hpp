#pragma once
#include "../database/database.hpp"
#include "../database/statement.hpp"
#include "../models/user.hpp"
#include <optional>
#include <iostream>

class UserRepository {
    database& db;
public:
    UserRepository(database& db): db(db) {}

    std::optional<User> find_by_username(const std::string& username) {
        statement stmt(db, "SELECT id, username, password, address, is_admin FROM users WHERE username = ?");
        stmt.bind_text(1, username);
        int rc = stmt.step();
        if (rc == SQLITE_ROW) {
            return User{
                stmt.get_int_col(0),
                stmt.get_text_col(1),
                stmt.get_text_col(2),
                stmt.get_text_col(3),
                stmt.get_int_col(4) != 0
            };
        }
        return std::nullopt;
    }

    std::optional<User> find_by_id(int id) {
        statement stmt(db, "SELECT id, username, password, address, is_admin FROM users WHERE id = ?");
        stmt.bind_int(1, id);
        if (stmt.step()) {
            return User{
                stmt.get_int_col(0),
                stmt.get_text_col(1),
                stmt.get_text_col(2),
                stmt.get_text_col(3),
                stmt.get_int_col(4) != 0
            };
        }
        return std::nullopt;
    }

    bool create_user(const User& user) {
        /* std::cerr << "[DEBUG][create_user] user=" 
          << user.username << "," << user.address 
          << " is_admin=" << user.is_admin << "\n"; */
          
        statement stmt(db, "INSERT INTO users (username, password, address, is_admin) VALUES (?, ?, ?, ?)");
        
        stmt.bind_text(1, user.username);
        stmt.bind_text(2, user.password);
        stmt.bind_text(3, user.address);
        stmt.bind_int(4, user.is_admin ? 1 : 0);
        
        /* if (!ok) {
            std::cerr << "[DEBUG] create_user failed for username=" 
                  << user.username << "\n";
        } */
        return stmt.execute();
    }

    bool update_password(int user_id, const std::string& new_password) {
        statement stmt(db, "UPDATE users SET password = ? WHERE id = ?");
        stmt.bind_text(1, new_password);
        stmt.bind_int(2, user_id);
        return stmt.execute();
    }
};
