#pragma once
#include <string>
#include <vector>
#include "promotion.hpp"

struct Product {
    int id;
    std::string category;
    std::string name;
    double price;
    int stock;
    std::string description;

    // 当前生效的促销列表
    std::vector<Promotion> promotions;
};
