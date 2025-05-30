#pragma once
#include <vector>
#include <optional>
#include <string>
#include "../database/database.hpp"
#include "../database/statement.hpp"
#include "../models/products.hpp"
#include "../models/promotion.hpp"

class ProductRepository {
    database& db;
public:
    ProductRepository(database& db): db(db) {}

    // 获取数据库连接
    database& get_db() const { return db; }

    // 拉取一个商品当前生效的所有促销
    std::vector<Promotion> find_active_promotions(int product_id) {
        std::vector<Promotion> res;
        statement stmt(db, R"(
SELECT p.id, p.type, p.discount_rate, p.threshold, p.reduction,
       p.start_time, p.end_time, p.stackable
  FROM promotions p
  JOIN product_promotion pp ON p.id = pp.promotion_id
 WHERE pp.product_id = ?
   AND p.start_time <= datetime('now')
   AND p.end_time   >= datetime('now')
        )");
        stmt.bind_int(1, product_id);
        while (stmt.step() == SQLITE_ROW) {
            Promotion promo;
            promo.id = stmt.get_int_col(0);
            promo.type = stmt.get_int_col(1);
            promo.discount_rate = stmt.get_double_col(2);
            promo.threshold = stmt.get_double_col(3);
            promo.reduction = stmt.get_double_col(4);
            promo.start_time = stmt.get_text_col(5);
            promo.end_time = stmt.get_text_col(6);
            promo.stackable = stmt.get_int_col(7) != 0;
            res.push_back(promo);
        }
        return res;
    }

    // 查询所有商品，并填充 promotions
    std::vector<Product> find_all() {
        std::vector<Product> products;
        statement stmt(db,
            "SELECT id, category, name, price, stock, description FROM products");
        while (stmt.step() == SQLITE_ROW) {
            Product p;
            p.id = stmt.get_int_col(0);
            p.category = stmt.get_text_col(1);
            p.name = stmt.get_text_col(2);
            p.price = stmt.get_double_col(3);
            p.stock = stmt.get_int_col(4);
            p.description = stmt.get_text_col(5);
            p.promotions = find_active_promotions(p.id);
            products.push_back(std::move(p));
        }
        return products;
    }

    // 按 ID 查询，并填充 promotions
    std::optional<Product> find_by_id(int id) {
        statement stmt(db,
            "SELECT id, category, name, price, stock, description FROM products WHERE id = ?");
        stmt.bind_int(1, id);
        if (stmt.step() == SQLITE_ROW) {
            Product p;
            p.id = stmt.get_int_col(0);
            p.category = stmt.get_text_col(1);
            p.name = stmt.get_text_col(2);
            p.price = stmt.get_double_col(3);
            p.stock = stmt.get_int_col(4);
            p.description = stmt.get_text_col(5);
            p.promotions = find_active_promotions(p.id);
            return p;
        }
        return std::nullopt;
    }

    // 添加、更新、删除等方法保留原样
    bool add(const Product& p) {
        statement stmt(db,
            "INSERT INTO products (category, name, price, stock, description) VALUES (?, ?, ?, ?, ?)");
        stmt.bind_text(1, p.category);
        stmt.bind_text(2, p.name);
        stmt.bind_double(3, p.price);
        stmt.bind_int(4, p.stock);
        stmt.bind_text(5, p.description);
        return stmt.execute();
    }

    bool update(const Product& p) {
        statement stmt(db,
            "UPDATE products SET category=?, name=?, price=?, stock=?, description=? WHERE id = ?");
        stmt.bind_text(1, p.category);
        stmt.bind_text(2, p.name);
        stmt.bind_double(3, p.price);
        stmt.bind_int(4, p.stock);
        stmt.bind_text(5, p.description);
        stmt.bind_int(6, p.id);
        return stmt.execute();
    }

    bool remove(int id) {
        statement stmt(db,
            "DELETE FROM products WHERE id = ?");
        stmt.bind_int(1, id);
        return stmt.execute();
    }
};
