#include "database_init.hpp"
#include <iostream>

void DatabaseInitializer::initialize(database& db) {
    // 用户表
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            address TEXT,
            is_admin INTEGER DEFAULT 0
        );
    )");

    // 商品表
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            category TEXT NOT NULL,
            name TEXT NOT NULL,
            price REAL NOT NULL,
            description TEXT,
            stock INTEGER NOT NULL,
            promo_id INTEGER,
            FOREIGN KEY(promo_id) REFERENCES promotions(id)
        );
    )");

    // 购物车项表
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS cart_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            product_id INTEGER NOT NULL,
            quantity INTEGER NOT NULL,
            UNIQUE(user_id, product_id),
            FOREIGN KEY(user_id) REFERENCES users(id),
            FOREIGN KEY(product_id) REFERENCES products(id)
        );
    )");

    // 订单表
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS orders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            total_price REAL NOT NULL,
            status TEXT NOT NULL CHECK(status IN ('待发货', '已发货', '已收货', '已取消')),
            address TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(user_id) REFERENCES users(id)
        );
    )");

    // 订单项表
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS order_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            order_id INTEGER NOT NULL,
            product_id INTEGER NOT NULL,
            quantity INTEGER NOT NULL,
            unit_price REAL NOT NULL,
            FOREIGN KEY(order_id) REFERENCES orders(id),
            FOREIGN KEY(product_id) REFERENCES products(id)
        );
    )");

    // 促销活动表
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS promotions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type INTEGER NOT NULL CHECK(type IN (0, 1)),
            discount_rate REAL,         -- 折扣百分比，如 0.8 表示 8 折
            threshold REAL,             -- 满减门槛，如满 300
            reduction REAL,             -- 满减额度，如减 50
            start_time TEXT NOT NULL,
            end_time TEXT NOT NULL,
            stackable INTEGER DEFAULT 1 -- 是否可与其他促销叠加
        );
    )");

    // 商品-促销 关联表
    db.exec(R"(
      CREATE TABLE IF NOT EXISTS product_promotion (
        product_id   INTEGER NOT NULL,
        promotion_id INTEGER NOT NULL,
        PRIMARY KEY(product_id, promotion_id),
        FOREIGN KEY(product_id)   REFERENCES products(id),
        FOREIGN KEY(promotion_id) REFERENCES promotions(id)
      );
    )");


    std::cout << "[INFO] 所有表初始化完成。\n";
}
