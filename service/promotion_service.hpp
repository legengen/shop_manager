#pragma once
#include "../Repository/promotion_repository.hpp"
#include "../models/promotion.hpp"
#include <vector>
#include <string>

class PromotionService {
    PromotionRepository& repo;
public:
    PromotionService(PromotionRepository& r): repo(r) {}

    int add(const Promotion& p)       { return repo.add(p); }
    bool update(const Promotion& p)    { return repo.update(p); }
    bool remove(int id)                { return repo.remove(id); }
    std::vector<Promotion> active(const std::string& now) {
        return repo.find_active(now);
    }

    bool link_product(int promotion_id, int product_id) {
        return repo.link_product(promotion_id, product_id);
    }
    
    // 获取适用于整单的促销活动
    std::vector<Promotion> get_order_promotions(const std::string& now) {
        return repo.find_order_promotions(now);
    }
};
