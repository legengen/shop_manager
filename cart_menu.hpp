#pragma once
#include "service/cart_service.hpp"
#include "service/product_service.hpp"
#include "user_menu.hpp"
#include <iostream>
#include <tuple>
#include <vector>
#include <iomanip>

class CartMenu {
    CartService& cart_svc;
    ProductService& prod_svc;
    UserMenu& user_menu;
public:
    CartMenu(CartService& cs, ProductService& ps, UserMenu& um)
      : cart_svc(cs), prod_svc(ps), user_menu(um) {}

    void show_menu() {
        if (!user_menu.is_logged_in()) {
            std::cout<<"请先登录后再操作购物车。\n";
            return;
        }
        int uid = user_menu.get_current_user()->id;
        while (true) {
            std::cout<<"\n===== 购物车 =====\n"
                     <<"1. 浏览所有商品\n"
                     <<"2. 查看我的购物车\n"
                     <<"3. 添加商品到购物车\n"
                     <<"4. 从购物车删除商品\n"
                     <<"0. 返回上级\n"
                     <<"请选择: ";
            int op; std::cin>>op;
            if      (op==1) browse_products();
            else if (op==2) view_cart(uid);
            else if (op==3) add_to_cart(uid);
            else if (op==4) remove_from_cart(uid);
            else if (op==0) break;
            else std::cout<<"无效选项\n";
        }
    }

    void browse_products() {
        auto items = prod_svc.list_all();
        std::cout << "\nID\t名称\t\t价格\t\t库存\t促销信息\n";
        std::cout << "-----------------------------------------------------------\n";
        for (auto& p : items) {
            double promo_price = prod_svc.get_promotional_price(p);
            std::string promo_tags = prod_svc.get_promotion_tags(p);
            
            std::cout << p.id << '\t'
                    << std::left << std::setw(10) << p.name << '\t';
            
            if (promo_price < p.price) {
                std::cout << promo_price << " ¥"
                          << " (原价:" << p.price << " ¥)" << '\t';
            } else {
                std::cout << p.price << " ¥\t\t";
            }
            
            std::cout << p.stock << "\t" << promo_tags << "\n";
        }
        std::cout << "-----------------------------------------------------------\n";
    }

private:
    void view_cart(int uid) {
        auto list = cart_svc.list(uid);
        double total = 0;
        std::cout<<"\n商品\t数量\t单价\t小计\t促销信息\n";
        std::cout<<"------------------------------------------------------\n";
        for (auto& [p,qty] : list) {
            double promo_price = prod_svc.get_promotional_price(p);
            double sub = promo_price * qty;
            total += sub;
            std::string promo_tags = prod_svc.get_promotion_tags(p);
            
            std::cout << p.name << '\t'
                     << qty << '\t';
                     
            if (promo_price < p.price) {
                std::cout << promo_price << "(原价:" << p.price << ")" << '\t';
            } else {
                std::cout << p.price << '\t';
            }
            
            std::cout << sub << '\t' << promo_tags << "\n";
        }
        std::cout<<"------------------------------------------------------\n";
        std::cout<<"总价: "<< total <<" ¥\n";
    }

    void add_to_cart(int uid) {
        int pid, qty;
        std::cout<<"商品ID: "; std::cin>>pid;
        std::cout<<"数量: ";   std::cin>>qty;
        if (cart_svc.add(uid,pid,qty))
            std::cout<<"添加/更新成功\n";
        else
            std::cout<<"添加失败\n";
    }

    void remove_from_cart(int uid) {
        int pid;
        std::cout<<"商品ID: "; std::cin>>pid;
        if (cart_svc.remove(uid,pid))
            std::cout<<"删除成功\n";
        else
            std::cout<<"删除失败\n";
    }
};
