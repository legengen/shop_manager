#pragma once
#include "../models/promotion.hpp"
#include "../database/database.hpp"
#include "../database/statement.hpp"
#include <vector>
#include <optional>

class PromotionRepository {
    database& db;
public:
    PromotionRepository(database& db): db(db) {}

    int add(const Promotion& p);
    bool update(const Promotion& p);
    bool remove(int id);
    std::vector<Promotion> find_active(const std::string& now);
    std::optional<Promotion> find_by_id(int id);
    bool link_product(int promotion_id, int product_id);
    
    // 查询适用于整单的满减促销
    std::vector<Promotion> find_order_promotions(const std::string& now);
};

int PromotionRepository::add(const Promotion& p) {
    statement stmt(db,
      "INSERT INTO promotions (type, discount_rate, threshold, reduction, start_time, end_time, stackable) "
      "VALUES (?, ?, ?, ?, ?, ?, ?)");
    stmt.bind_int(1, p.type);
    stmt.bind_double(2, p.discount_rate);
    stmt.bind_double(3, p.threshold);
    stmt.bind_double(4, p.reduction);
    stmt.bind_text(5, p.start_time);
    stmt.bind_text(6, p.end_time);
    stmt.bind_int(7, p.stackable ? 1 : 0);
    if (stmt.execute()) {
        return sqlite3_last_insert_rowid(db.get_handle());
    }
    return 0;
}

bool PromotionRepository::update(const Promotion& p) {
    statement stmt(db,
      "UPDATE promotions SET type=?, discount_rate=?, threshold=?, reduction=?, start_time=?, end_time=?, stackable=? "
      "WHERE id=?");
    stmt.bind_int(1, p.type);
    stmt.bind_double(2, p.discount_rate);
    stmt.bind_double(3, p.threshold);
    stmt.bind_double(4, p.reduction);
    stmt.bind_text(5, p.start_time);
    stmt.bind_text(6, p.end_time);
    stmt.bind_int(7, p.stackable ? 1 : 0);
    stmt.bind_int(8, p.id);
    return stmt.execute();
}

bool PromotionRepository::remove(int id) {
    statement stmt(db, "DELETE FROM promotions WHERE id = ?");
    stmt.bind_int(1, id);
    return stmt.execute();
}

std::vector<Promotion> PromotionRepository::find_active(const std::string& now) {
    std::vector<Promotion> res;
    statement stmt(db,
      "SELECT id, type, discount_rate, threshold, reduction, start_time, end_time, stackable "
      "FROM promotions WHERE start_time <= ? AND end_time >= ?");
    stmt.bind_text(1, now);
    stmt.bind_text(2, now);
    while (stmt.step() == SQLITE_ROW) {
        res.push_back(Promotion{
          stmt.get_int_col(0),
          stmt.get_int_col(1),
          stmt.get_double_col(2),
          stmt.get_double_col(3),
          stmt.get_double_col(4),
          stmt.get_text_col(5),
          stmt.get_text_col(6),
          stmt.get_int_col(7) != 0
        });
    }
    return res;
}

std::optional<Promotion> PromotionRepository::find_by_id(int id) {
    statement stmt(db,
      "SELECT id, type, discount_rate, threshold, reduction, start_time, end_time, stackable "
      "FROM promotions WHERE id = ?");
    stmt.bind_int(1, id);
    if (stmt.step() == SQLITE_ROW) {
        return Promotion{
          stmt.get_int_col(0),
          stmt.get_int_col(1),
          stmt.get_double_col(2),
          stmt.get_double_col(3),
          stmt.get_double_col(4),
          stmt.get_text_col(5),
          stmt.get_text_col(6),
          stmt.get_int_col(7) != 0
        };
    }
    return std::nullopt;
}

bool PromotionRepository::link_product(int promotion_id, int product_id) {
        statement stmt(db,
            "INSERT OR IGNORE INTO product_promotion (product_id, promotion_id) VALUES (?, ?)");
        stmt.bind_int(1, product_id);
        stmt.bind_int(2, promotion_id);
        return stmt.execute();
}

// 查询适用于整单的满减促销
std::vector<Promotion> PromotionRepository::find_order_promotions(const std::string& now) {
    std::vector<Promotion> res;
    statement stmt(db,
      "SELECT id, type, discount_rate, threshold, reduction, start_time, end_time, stackable "
      "FROM promotions WHERE start_time <= ? AND end_time >= ? "
      "AND type = ? AND id NOT IN (SELECT promotion_id FROM product_promotion)");
    stmt.bind_text(1, now);
    stmt.bind_text(2, now);
    stmt.bind_int(3, REDUCTION); // 只查询满减类型的促销
    while (stmt.step() == SQLITE_ROW) {
        res.push_back(Promotion{
          stmt.get_int_col(0),
          stmt.get_int_col(1),
          stmt.get_double_col(2),
          stmt.get_double_col(3),
          stmt.get_double_col(4),
          stmt.get_text_col(5),
          stmt.get_text_col(6),
          stmt.get_int_col(7) != 0
        });
    }
    return res;
}
