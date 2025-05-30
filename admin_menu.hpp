#pragma once
#include "service/product_service.hpp"
#include <iostream>
#include <vector>

class AdminMenu {
    ProductService& svc;
public:
    AdminMenu(ProductService& s): svc(s) {}

    void show_menu() {
        while (true) {
            std::cout << "\n===== 管理员商品管理 =====\n"
                      << "1. 列出所有商品\n"
                      << "2. 添加商品\n"
                      << "3. 修改商品\n"
                      << "4. 删除商品\n"
                      << "0. 返回上级\n"
                      << "请选择: ";
            int op; std::cin >> op;
            if      (op==1) list();
            else if (op==2) add();
            else if (op==3) edit();
            else if (op==4) remove();
            else if (op==0) break;
            else std::cout<<"无效选项\n";
        }
    }

private:
    void list() {
        auto items = svc.list_all();
        std::cout << "\nID\t分类\t名称\t价格\t库存\t描述\n";
        for (auto& p : items) {
            std::cout << p.id << '\t'
                      << p.category << '\t'
                      << p.name << '\t'
                      << p.price << '\t'
                      << p.stock << '\t'
                      << p.description << "\n";
        }
    }

    void add() {
        std::string cat,name,desc;
        double price; int stock;
        std::cout<<"分类: ";     std::cin>>cat;
        std::cout<<"名称: ";     std::cin>>name;
        std::cout<<"价格: ";     std::cin>>price;
        std::cout<<"库存: ";     std::cin>>stock;
        std::cout<<"描述: ";     std::cin.ignore(), std::getline(std::cin, desc);
        if (svc.add(cat,name,price,stock,desc))
            std::cout<<"添加成功\n";
        else
            std::cout<<"添加失败\n";
    }

    void edit() {
        int id; std::cout<<"输入要修改的商品ID: "; std::cin>>id;
        auto p = svc.get(id);
        if (!p) { std::cout<<"未找到该商品\n"; return; }
        std::string cat,name,desc;
        double price; int stock;
        std::cout<<"新分类("<<p->category<<"): "; std::cin>>cat;
        std::cout<<"新名称("<<p->name<<"): ";     std::cin>>name;
        std::cout<<"新价格("<<p->price<<"): ";    std::cin>>price;
        std::cout<<"新库存("<<p->stock<<"): ";    std::cin>>stock;
        std::cout<<"新描述: ";                    std::cin.ignore(), std::getline(std::cin, desc);
        if (svc.update(id,cat,name,price,stock,desc))
            std::cout<<"修改成功\n";
        else
            std::cout<<"修改失败\n";
    }

    void remove() {
        int id; std::cout<<"输入要删除的商品ID: "; std::cin>>id;
        if (svc.remove(id))
            std::cout<<"删除成功\n";
        else
            std::cout<<"删除失败\n";
    }
};
