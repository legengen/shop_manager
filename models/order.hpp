#pragma once
#include <string>
#include <vector>

struct OrderItem {
    int product_id;
    int quantity;
    double unit_price;
};

struct Order {
    int id;
    int user_id;
    std::vector<OrderItem> items;
    double total_price;
    std::string status;    // "待发货", "已发货", "已收货", "已取消"
    std::string address;
    std::string created_at;
};
