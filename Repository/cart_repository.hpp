#pragma once
#include "../database/database.hpp"
#include "../database/statement.hpp"
#include "../models/cart_item.hpp"
#include <vector>

class CartRepository {
    database& db;
public:
    CartRepository(database& db): db(db) {}

    // 取出某用户所有购物车项
    std::vector<CartItem> find_by_user(int uid) {
        std::vector<CartItem> res;
        statement stmt(db,
            "SELECT user_id, product_id, quantity FROM cart_items WHERE user_id = ?");
        stmt.bind_int(1, uid);
        while (stmt.step() == SQLITE_ROW) {
            res.push_back(CartItem{
                stmt.get_int_col(0),
                stmt.get_int_col(1),
                stmt.get_int_col(2)
            });
        }
        return res;
    }

    // 插入或更新数量
    bool upsert(int user_id, int product_id, int qty) {
        statement stmt(db,
            "INSERT INTO cart_items (user_id, product_id, quantity) "
            "VALUES (?, ?, ?) "
            "ON CONFLICT(user_id, product_id) DO UPDATE SET quantity = ?");
        stmt.bind_int(1, user_id);
        stmt.bind_int(2, product_id);
        stmt.bind_int(3, qty);
        stmt.bind_int(4, qty);
        return stmt.execute();
    }

    // 删除一项
    bool remove(int user_id, int product_id) {
        statement stmt(db,
            "DELETE FROM cart_items WHERE user_id = ? AND product_id = ?");
        stmt.bind_int(1, user_id);
        stmt.bind_int(2, product_id);
        return stmt.execute();
    }
};
