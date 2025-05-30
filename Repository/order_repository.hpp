#pragma once
#include "../models/order.hpp"
#include "../database/database.hpp"
#include "../database/statement.hpp"
#include <vector>
#include <optional>
#include <iostream>

class OrderRepository {
    database& db;
public:
    OrderRepository(database& db): db(db) {}

    // 创建订单主表，返回新订单 ID
    std::optional<int> create_order(int user_id, double total, const std::string& addr);

    // 插入订单明细
    bool add_order_item(int order_id, const OrderItem& item);

    // 查询某用户全部订单
    std::vector<Order> find_by_user(int user_id);
    // 查单个订单（不含 items，可以或与 find_items 联动）
    std::optional<Order> find_by_id(int order_id);


    // 修改订单状态或地址
    bool update_status(int order_id, const std::string& status);
    bool update_address(int order_id, const std::string& address);

    // 删除已收货订单
    bool delete_order(int order_id);

    // 查询单个订单明细
    std::vector<OrderItem> find_items(int order_id);
};


std::optional<int> OrderRepository::create_order(int user_id, double total, const std::string& addr) {
    statement stmt(db,
      "INSERT INTO orders (user_id, total_price, status, address) VALUES (?, ?, '待发货', ?)");
    stmt.bind_int(1, user_id);
    stmt.bind_double(2, total);
    stmt.bind_text(3, addr);
    if (!stmt.execute()) return std::nullopt;
    // sqlite3_last_insert_rowid
    int oid = sqlite3_last_insert_rowid(db.get_handle());
    return oid;
}

bool OrderRepository::add_order_item(int order_id, const OrderItem& it) {
    statement stmt(db,
      "INSERT INTO order_items (order_id, product_id, quantity, unit_price) VALUES (?, ?, ?, ?)");
    stmt.bind_int(1, order_id);
    stmt.bind_int(2, it.product_id);
    stmt.bind_int(3, it.quantity);
    stmt.bind_double(4, it.unit_price);
    return stmt.execute();
}

std::vector<Order> OrderRepository::find_by_user(int user_id) {
    std::vector<Order> res;
    statement stmt(db,
      "SELECT id, total_price, status, address, created_at FROM orders WHERE user_id = ?");
    stmt.bind_int(1, user_id);
    while (stmt.step() == SQLITE_ROW) {
        Order o;
        o.id          = stmt.get_int_col(0);
        o.user_id     = user_id;
        o.total_price = stmt.get_double_col(1);
        o.status      = stmt.get_text_col(2);
        o.address     = stmt.get_text_col(3);
        o.created_at  = stmt.get_text_col(4);
        o.items       = find_items(o.id);
        res.push_back(std::move(o));
    }
    return res;
}

std::vector<OrderItem> OrderRepository::find_items(int order_id) {
    std::vector<OrderItem> items;
    statement stmt(db,
      "SELECT product_id, quantity, unit_price FROM order_items WHERE order_id = ?");
    stmt.bind_int(1, order_id);
    while (stmt.step() == SQLITE_ROW) {
        items.push_back(OrderItem{
          stmt.get_int_col(0),
          stmt.get_int_col(1),
          stmt.get_double_col(2)
        });
    }
    return items;
}

bool OrderRepository::update_status(int order_id, const std::string& status) {
    statement stmt(db,
      "UPDATE orders SET status = ? WHERE id = ?");
    stmt.bind_text(1, status);
    stmt.bind_int(2, order_id);
    return stmt.execute();
}

bool OrderRepository::update_address(int order_id, const std::string& address) {
    statement stmt(db,
      "UPDATE orders SET address = ? WHERE id = ?");
    stmt.bind_text(1, address);
    stmt.bind_int(2, order_id);
    return stmt.execute();
}

bool OrderRepository::delete_order(int order_id) {
    // 先删除 order_items，再删除 orders
    statement stmt1(db, "DELETE FROM order_items WHERE order_id = ?");
    stmt1.bind_int(1, order_id);
    bool ok1 = stmt1.execute();
    statement stmt2(db, "DELETE FROM orders WHERE id = ?");
    stmt2.bind_int(1, order_id);
    bool ok2 = stmt2.execute();
    return ok1 && ok2;
}

std::optional<Order> OrderRepository::find_by_id(int order_id) {
    // 先查 orders 表
    statement stmt(db,
        "SELECT id, user_id, total_price, status, address, created_at "
        "FROM orders WHERE id = ?");
    stmt.bind_int(1, order_id);
    if (stmt.step() == SQLITE_ROW) {
        Order o;
        o.id          = stmt.get_int_col(0);
        o.user_id     = stmt.get_int_col(1);
        o.total_price = stmt.get_double_col(2);
        o.status      = stmt.get_text_col(3);
        o.address     = stmt.get_text_col(4);
        o.created_at  = stmt.get_text_col(5);
        o.items       = find_items(o.id);
        return o;
    }
    return std::nullopt;
}


