#pragma once
#include "user_repository.hpp"
#include <optional>
#include <string>

class AuthService {
    UserRepository& user_repo;
public:
    AuthService(UserRepository& repo): user_repo(repo) {}

    std::optional<User> login(const std::string& username, const std::string& password) {
        auto user = user_repo.find_by_username(username);
        if (user && user->password == password) {
            return user;
        }
        return std::nullopt;
    }

    bool register_user(const std::string& username, const std::string& password, const std::string& address) {
        // 避免重复用户
        if (user_repo.find_by_username(username)) {
            return false; // 用户已存在
        }
        User user{0, username, password, address, false};
        return user_repo.create_user(user);
    }

    bool change_password(int user_id, const std::string& old_pw, const std::string& new_pw) {
        auto user = user_repo.find_by_id(user_id);
        if (!user || user->password != old_pw) {
            return false;
        }
        return user_repo.update_password(user_id, new_pw);
    }
};
