#pragma once
#include "../Repository/order_repository.hpp"
#include "../Repository/product_repository.hpp"
#include "../service/cart_service.hpp"
#include "../models/order.hpp"
#include "../service/product_service.hpp"
#include "../Repository/promotion_repository.hpp"
#include <optional>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>


class OrderService {
    OrderRepository& order_repo;
    ProductRepository& prod_repo;
    CartService& cart_svc;
    ProductService& prod_svc;
    PromotionRepository promo_repo;
public:
    OrderService(OrderRepository& or_, ProductRepository& pr, CartService& cr, ProductService& ps)
      : order_repo(or_), prod_repo(pr), cart_svc(cr), prod_svc(ps), promo_repo(pr.get_db()) {}

    // 获取当前时间字符串，用于查询有效促销
    std::string get_current_time() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    // 下单：从购物车取出、计算总价、扣库存、写 orders/order_items、清空购物车
    bool place_order(int user_id, const std::string& address) {
        auto list = cart_svc.list(user_id);
        if (list.empty()) return false;
        
        // 计算原始总价（无促销）
        double original_total = 0;
        // 计算应用单品促销后的总价
        double promo_total = 0;
        
        // 检查是否有任何商品应用了促销
        bool has_product_promotion = false;
        
        for (auto& [p,qty] : list) {
            // 原始价格
            original_total += p.price * qty;
            
            // 促销价格
            double promo_price = prod_svc.get_promotional_price(p);
            if (p.stock < qty) return false;
            promo_total += promo_price * qty;
            
            // 检查是否应用了促销
            if (promo_price < p.price) {
                has_product_promotion = true;
            }
        }
        
        // 应用整单满减
        std::string now = get_current_time();
        auto order_promotions = promo_repo.find_order_promotions(now);
        
        // 最终总价默认为促销后价格
        double final_total = promo_total;
        
        // 找到适用的最佳满减
        double max_reduction = 0.0;
        Promotion* best_promotion = nullptr;
        
        for (auto& promo : order_promotions) {
            if (promo.type == REDUCTION && 
                promo_total >= promo.threshold && 
                promo.reduction > max_reduction) {
                max_reduction = promo.reduction;
                best_promotion = &promo;
            }
        }
        
        // 应用满减，但要检查是否可叠加
        if (max_reduction > 0 && best_promotion) {
            // 检查是否可以叠加计算
            bool can_stack = best_promotion->stackable;
            
            // 如果有单品促销，需要检查单品促销是否都允许叠加
            if (has_product_promotion) {
                // 如果整单满减可叠加，还需检查所有商品的促销是否也允许叠加
                if (can_stack) {
                    for (auto& [p,qty] : list) {
                        // 如果商品有促销且价格有折扣
                        double promo_price = prod_svc.get_promotional_price(p);
                        if (promo_price < p.price) {
                            
                            // 查找实际生效的最优惠折扣
                            double best_rate = 1.0;
                            bool best_discount_stackable = true;
                            for (auto& promo : p.promotions) {
                                if (promo.type == DISCOUNT && promo.discount_rate < best_rate) {
                                    best_rate = promo.discount_rate;
                                    best_discount_stackable = promo.stackable;
                                }
                            }
                            
                            // 查找实际生效的最优惠满减
                            double best_reduction = 0.0;
                            bool best_reduction_stackable = true;
                            double discounted_price = p.price * best_rate;
                            
                            for (auto& promo : p.promotions) {
                                if (promo.type == REDUCTION && discounted_price >= promo.threshold) {
                                    if (promo.reduction > best_reduction) {
                                        best_reduction = promo.reduction;
                                        best_reduction_stackable = promo.stackable;
                                    }
                                }
                            }

                            
                            // 检查实际生效的促销是否可叠加
                            bool product_has_non_stackable_effective_promo = false;
                            
                            // 如果应用了折扣且折扣不可叠加
                            if (best_rate < 1.0 && !best_discount_stackable) {
                                product_has_non_stackable_effective_promo = true;
                            }
                            
                            // 如果应用了满减且满减不可叠加
                            if (best_reduction > 0 && !best_reduction_stackable) {
                                // 如果这个满减是与折扣叠加的，才考虑它的stackable属性
                                if (best_discount_stackable || best_rate == 1.0) {
                                    product_has_non_stackable_effective_promo = true;
                                }
                            }
                            
                            if (product_has_non_stackable_effective_promo) {
                                can_stack = false;
                                break;
                            }
                        }
                        if (!can_stack) break; // 找到不可叠加的促销就停止检查
                    }
                }
                
                // 如果不能叠加，比较哪种方式更优惠
                if (!can_stack) {
                    // 计算只应用整单满减的价格
                    double only_order_promo = original_total - max_reduction;
                    
                    // 如果只应用整单满减更便宜，则使用它
                    if (only_order_promo < promo_total) {
                        final_total = only_order_promo;
                    }
                    // 否则保持单品促销价格不变
                } else {
                    // 可以叠加，直接应用整单满减
                    final_total = promo_total - max_reduction;
                }
            } else {
                // 没有单品促销，直接应用整单满减
                final_total = original_total - max_reduction;
            }
        }
        
        auto oid = order_repo.create_order(user_id, final_total, address);
        if (!oid) return false;
        for (auto& [p,qty] : list) {
            double promo_price = prod_svc.get_promotional_price(p);
            // 2) 存入订单项
            order_repo.add_order_item(*oid, {p.id, qty, promo_price});
            // 3) 扣库存
            p.stock -= qty;
            prod_repo.update(p);
        }
        // 清空购物车
        for (auto& [p,qty] : list)
            cart_svc.remove(user_id, p.id);
        return true;
    }

    std::vector<Order> list_orders(int user_id) {
        return order_repo.find_by_user(user_id);
    }

    bool cancel_order(int order_id) {
        // 先查订单
        auto o = order_repo.find_by_id(order_id);
        if (!o) return false;
        if (o->status != "待发货") {
            return false;  // 只有待发货状态可取消
        }
        // 取消订单并恢复库存
        // 恢复库存：遍历 o->items，每个 item 增加回 product.stock
        for (auto& it : o->items) {
            auto p = prod_repo.find_by_id(it.product_id);
            if (p) {
                p->stock += it.quantity;
                prod_repo.update(*p);
            }
        }
        return order_repo.update_status(order_id, "已取消");
    }

    bool update_address(int order_id, const std::string& addr) {
        return order_repo.update_address(order_id, addr);
    }

    bool delete_order(int order_id) {
        // 先查询订单状态
        auto order = order_repo.find_by_id(order_id);
        if (!order) return false;
        
        // 只允许删除"已收货"状态的订单
        if (order->status != "已收货") {
            return false;
        }
        
        // 删除订单
        return order_repo.delete_order(order_id);
    }

    bool update_order_status(int order_id, const std::string& status) {
        return order_repo.update_status(order_id, status);
    }

    bool update_order_address(int order_id, const std::string& addr) {
        return order_repo.update_address(order_id, addr);
    }
};
