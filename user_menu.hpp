#pragma once
#include "auth_service.hpp"
#include <memory>
#include <iostream>
#include <optional>

class UserMenu {
    AuthService& auth;
    std::optional<User> current_user;

public:
    UserMenu(AuthService& auth_service) : auth(auth_service) {}

    void show_menu() {
        while (true) {
            std::cout << "\n===== 用户系统 =====\n";
            std::cout << "1. 注册\n";
            std::cout << "2. 登录\n";
            std::cout << "3. 修改密码\n";
            std::cout << "4. 显示当前用户信息\n";
            std::cout << "5. 注销登录\n";
            std::cout << "0. 返回主菜单\n";
            std::cout << "请选择操作: ";
            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1: register_user(); break;
                case 2: login(); break;
                case 3: change_password(); break;
                case 4: show_current_user(); break;
                case 5: logout(); break;
                case 0: return;
                default: std::cout << "无效选项，请重试。\n"; break;
            }
        }
    }

    std::optional<User> get_current_user() const {
        return current_user;
    }

    bool is_logged_in() const {
        return current_user.has_value();
    }

    bool is_admin() const {
        return current_user && current_user->is_admin;
    }

private:
    void register_user() {
        std::string username, password, address;
        std::cout << "用户名: ";
        std::cin >> username;
        std::cout << "密码: ";
        std::cin >> password;
        std::cout << "地址: ";
        std::cin.ignore();
        std::getline(std::cin, address);
        if (auth.register_user(username, password, address)) {
            std::cout << "注册成功，请登录。\n";
        } else {
            std::cout << "注册失败，用户名可能已存在。\n";
        }
    }

    void login() {
        std::string username, password;
        std::cout << "用户名: ";
        std::cin >> username;
        std::cout << "密码: ";
        std::cin >> password;

        auto user = auth.login(username, password);
        if (user) {
            current_user = user;
            std::cout << "登录成功，欢迎 " << user->username << "！\n";
        } else {
            std::cout << "登录失败，用户名或密码错误。\n";
        }
    }

    void change_password() {
        if (!current_user) {
            std::cout << "请先登录。\n";
            return;
        }

        std::string old_pw, new_pw;
        std::cout << "请输入旧密码: ";
        std::cin >> old_pw;
        std::cout << "请输入新密码: ";
        std::cin >> new_pw;

        if (auth.change_password(current_user->id, old_pw, new_pw)) {
            std::cout << "密码修改成功。\n";
        } else {
            std::cout << "密码修改失败，旧密码错误。\n";
        }
    }

    void show_current_user() {
        if (!current_user) {
            std::cout << "当前未登录。\n";
        } else {
            std::cout << "当前登录用户: " << current_user->username
                      << " [" << (current_user->is_admin ? "管理员" : "顾客") << "]\n";
        }
    }

    void logout() {
        if (current_user) {
            std::cout << "用户 " << current_user->username << " 已注销登录。\n";
            current_user.reset();
        } else {
            std::cout << "当前没有用户登录。\n";
        }
    }
};
