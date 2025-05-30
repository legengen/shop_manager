#pragma once
#include <string>

struct User {
    int id;
    std::string username;
    std::string password;
    std::string address;
    bool is_admin;
};
