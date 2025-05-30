#pragma once
#include "../models/products.hpp"
#include "../Repository/product_repository.hpp"
#include <vector>
#include <optional>
#include <string>

class ProductService {
    ProductRepository& repo;
public:
    ProductService(ProductRepository& r): repo(r) {}

    std::vector<Product> list_all() { return repo.find_all(); }

    std::optional<Product> get(int id) { return repo.find_by_id(id); }

    bool add(const std::string& cat,
             const std::string& name,
             double price,
             int stock,
             const std::string& desc)
    {
        Product p{0, cat, name, price, stock, desc};
        return repo.add(p);
    }

    bool update(int id,
                const std::string& cat,
                const std::string& name,
                double price,
                int stock,
                const std::string& desc)
    {
        Product p{id, cat, name, price, stock, desc};
        return repo.update(p);
    }

    bool remove(int id) { return repo.remove(id); }

    double get_promotional_price(const Product& p) {
        // 先按折扣算（找出最优折扣率）
        double best_rate = 1.0;
        bool has_discount = false;
        for (auto& promo : p.promotions) {
            if (promo.type == DISCOUNT && promo.discount_rate < best_rate) {
                best_rate = promo.discount_rate;
                has_discount = true;
            }
        }
        double discounted = p.price * best_rate;

        // 单品满减逻辑：找到适用的满减优惠
        for (auto& promo : p.promotions) {
            // 如果是满减类型且金额达到阈值
            if (promo.type == REDUCTION && discounted >= promo.threshold) {
                // 如果是可叠加的满减或者没有应用过折扣（best_rate == 1.0）
                if (promo.stackable || !has_discount) {
                    discounted -= promo.reduction;
                }
            }
        }

        return discounted;
    }
    
    // 获取商品的促销标签文本
    std::string get_promotion_tags(const Product& p) {
        std::string tags;
        
        // 找出最优折扣
        double best_discount_rate = 1.0;
        double best_reduction = 0.0;
        double best_threshold = 0.0;
        
        for (auto& promo : p.promotions) {
            if (promo.type == DISCOUNT && promo.discount_rate < best_discount_rate) {
                best_discount_rate = promo.discount_rate;
            } else if (promo.type == REDUCTION) {
                // 对于满减，优先选择减免额度更高的
                if (promo.reduction > best_reduction) {
                    best_reduction = promo.reduction;
                    best_threshold = promo.threshold;
                }
            }
        }
        
        // 添加最优折扣标签
        if (best_discount_rate < 1.0) {
            tags += "[折扣:" + std::to_string(int(best_discount_rate * 10)) + "折] ";
        }
        
        // 添加满减标签
        if (best_reduction > 0) {
            tags += "[满" + std::to_string(int(best_threshold)) + "减" + std::to_string(int(best_reduction)) + "] ";
        }
        
        return tags;
    }
};
