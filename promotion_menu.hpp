#pragma once
#include "service/promotion_service.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

class PromotionMenu {
    PromotionService& svc;
public:
    PromotionMenu(PromotionService& s): svc(s) {}

    void show_menu() {
        while (true) {
            std::cout<<"\n===== 促销管理 =====\n"
                     <<"1. 添加商品促销\n"
                     <<"2. 添加整单满减\n"
                     <<"3. 修改促销\n"
                     <<"4. 删除促销\n"
                     <<"5. 查看生效促销\n"
                     <<"6. 关联促销到商品\n"
                     <<"7. 查看整单促销\n"
                     <<"0. 返回上级\n"
                     <<"请选择: ";
            int op; std::cin>>op;
            if      (op==1) add_product_promotion();
            else if (op==2) add_order_promotion();
            else if (op==3) edit();
            else if (op==4) del();
            else if (op==5) list_active();
            else if (op==6) link_to_product();
            else if (op==7) list_order_promotions();
            else if (op==0) break;
            else std::cout<<"无效选项\n";
        }
    }

private:
    // 获取当前时间字符串
    std::string get_current_time() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    // 添加商品促销
    void add_product_promotion() {
        Promotion p;
        std::cout<<"类型(0折扣/1满减): "; std::cin>>p.type;
        if (p.type==DISCOUNT) {
            std::cout<<"折扣率(0.8=8折): "; std::cin>>p.discount_rate;
            p.threshold = p.reduction = 0;
        } else {
            p.discount_rate = 1;
            std::cout<<"满减门槛: "; std::cin>>p.threshold;
            std::cout<<"减免额度: "; std::cin>>p.reduction;
        }
        std::cout<<"开始时间(YYYY-MM-DD HH:MM:SS，留空为当前时间): "; 
        std::cin.ignore();
        std::string start_time;
        std::getline(std::cin, start_time);
        if (start_time.empty()) {
            p.start_time = get_current_time();
        } else {
            p.start_time = start_time;
        }
        
        std::cout<<"结束时间(YYYY-MM-DD HH:MM:SS): ";                  
        std::getline(std::cin, p.end_time);
        std::cout<<"可叠加(1/0): ";               std::cin>>p.stackable;
        
        int promo_id = svc.add(p);
        if (promo_id > 0) {
            std::cout<<"添加成功，促销ID: " << promo_id << "\n"; 
            std::cout<<"请记得使用\"关联促销到商品\"功能将此促销与具体商品关联\n";
        } else {
            std::cout<<"添加失败\n";
        }
    }
    
    // 添加整单满减
    void add_order_promotion() {
        Promotion p;
        p.type = REDUCTION; // 整单满减只能是满减类型
        p.discount_rate = 1.0;
        
        std::cout<<"整单满减门槛: "; std::cin>>p.threshold;
        std::cout<<"整单减免额度: "; std::cin>>p.reduction;
        
        std::cout<<"开始时间(YYYY-MM-DD HH:MM:SS，留空为当前时间): "; 
        std::cin.ignore();
        std::string start_time;
        std::getline(std::cin, start_time);
        if (start_time.empty()) {
            p.start_time = get_current_time();
        } else {
            p.start_time = start_time;
        }
        
        std::cout<<"结束时间(YYYY-MM-DD HH:MM:SS): ";                  
        std::getline(std::cin, p.end_time);
        
        // 整单满减默认不可叠加
        p.stackable = false;
        
        int promo_id = svc.add(p);
        if (promo_id > 0) {
            std::cout<<"添加整单满减成功! ID: " << promo_id << "\n";
            std::cout<<"系统将自动应用于所有符合条件的订单\n";
        } else {
            std::cout<<"添加失败\n";
        }
    }

    void edit() {
        Promotion p;
        std::cout<<"促销ID: "; std::cin>>p.id;
        std::cout<<"类型(0折扣/1满减): "; std::cin>>p.type;
        if (p.type==DISCOUNT) {
            std::cout<<"折扣率(0.8=8折): "; std::cin>>p.discount_rate;
            p.threshold = p.reduction = 0;
        } else {
            p.discount_rate = 1;
            std::cout<<"满减门槛: "; std::cin>>p.threshold;
            std::cout<<"减免额度: "; std::cin>>p.reduction;
        }
        std::cout<<"开始时间(YYYY-MM-DD HH:MM:SS): "; std::cin.ignore(); std::getline(std::cin,p.start_time);
        std::cout<<"结束时间: ";                  std::getline(std::cin,p.end_time);
        std::cout<<"可叠加(1/0): ";               std::cin>>p.stackable;
        
        if (svc.update(p)) std::cout<<"修改成功\n"; else std::cout<<"修改失败\n";
    }

    void del() {
        int id; std::cout<<"促销ID: "; std::cin>>id;
        if (svc.remove(id)) std::cout<<"删除成功\n"; else std::cout<<"失败\n";
    }

    void list_active() {
        std::string now = get_current_time();
        std::cout << "当前时间: " << now << std::endl;
        
        auto v = svc.active(now);
        std::cout << "\n===== 当前生效的所有促销 =====\n";
        for (auto& p : v) {
            std::cout<<"ID:"<<p.id<<" 类型:";
            if (p.type==DISCOUNT) std::cout<<"折扣 率:"<<p.discount_rate*10<<"折";
            else std::cout<<"满减 满"<<p.threshold<<"减"<<p.reduction;
            std::cout<<" 期间:"<<p.start_time<<"~"<<p.end_time
                     <<" 叠加:"<<(p.stackable ? "是" : "否")<<"\n";
        }
        if (v.empty()) {
            std::cout << "当前没有生效的促销活动\n";
        }
    }

    void link_to_product() {
        int promo_id, prod_id;
        std::cout << "促销ID: ";    std::cin >> promo_id;
        std::cout << "商品ID: ";    std::cin >> prod_id;
        if (svc.link_product(promo_id, prod_id)) {
            std::cout << "关联成功。\n";
        } else {
            std::cout << "关联失败。\n";
        }
    }
    
    void list_order_promotions() {
        std::string now = get_current_time();
        std::cout << "当前时间: " << now << std::endl;
        
        auto v = svc.get_order_promotions(now);
        std::cout << "\n===== 当前生效的整单满减 =====\n";
        for (auto& p : v) {
            std::cout << "ID:" << p.id 
                      << " 满" << p.threshold 
                      << "减" << p.reduction
                      << " 期间:" << p.start_time << "~" << p.end_time << "\n";
        }
        if (v.empty()) {
            std::cout << "当前没有生效的整单满减\n";
        }
    }
};


