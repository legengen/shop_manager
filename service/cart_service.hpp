#pragma once
#include "../models/cart_item.hpp"
#include "../Repository/cart_repository.hpp"
#include "../Repository/product_repository.hpp"
#include <vector>
#include <tuple>

class CartService {
    CartRepository& cart_repo;
    ProductRepository& prod_repo;
public:
    CartService(CartRepository& cr, ProductRepository& pr)
      : cart_repo(cr), prod_repo(pr) {}

    // 添加或更新
    bool add(int uid, int pid, int qty) {
        // 可做库存检查：prod_repo.find_by_id(pid)->stock >= qty
        return cart_repo.upsert(uid, pid, qty);
    }

    bool remove(int uid, int pid) {
        return cart_repo.remove(uid, pid);
    }

    // 获取详情：返回 (Product, qty)
    std::vector<std::tuple<Product,int>> list(int uid) {
        std::vector<std::tuple<Product,int>> res;
        for (auto& ci : cart_repo.find_by_user(uid)) {
            if (auto p = prod_repo.find_by_id(ci.product_id)) {
                res.emplace_back(*p, ci.quantity);
            }
        }
        return res;
    }
};
