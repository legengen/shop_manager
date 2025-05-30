#pragma once
#include "service/order_service.hpp"
#include "user_menu.hpp"
#include <iostream>
#include <vector>

class OrderMenu {
    OrderService& svc;
    UserMenu& um;
public:
    OrderMenu(OrderService& os, UserMenu& umenu)
      : svc(os), um(umenu) {}

    void show_menu() {
        if (!um.is_logged_in()) {
            std::cout<<"请先登录。\n"; return;
        }
        int uid = um.get_current_user()->id;
        while (true) {
            std::cout<<"\n===== 订单管理 =====\n"
                     <<"1. 下单\n"
                     <<"2. 查看我的订单\n"
                     <<"3. 取消订单\n"
                     <<"4. 更新收货地址\n"
                     <<"5. 删除已收货订单\n"
                     <<"0. 返回上级\n"
                     <<"请选择: ";
            int op; std::cin>>op;
            if      (op==1) checkout(uid);
            else if (op==2) view(uid);
            else if (op==3) cancel(uid);
            else if (op==4) update_addr(uid);
            else if (op==5) del(uid);
            else if (op==0) break;
            else std::cout<<"无效选项\n";
        }
    }

private:
    void checkout(int uid) {
        std::string addr;
        std::cout<<"请输入收货地址: ";
        std::cin.ignore(); std::getline(std::cin, addr);
        if (svc.place_order(uid, addr)) std::cout<<"下单成功\n";
        else std::cout<<"下单失败\n";
    }

    void view(int uid) {
        auto orders = svc.list_orders(uid);
        for (auto& o : orders) {
            std::cout<<"订单ID: "<<o.id<<" 状态: "<<o.status
                     <<" 总价: "<<o.total_price
                     <<" 地址: "<<o.address
                     <<" 时间: "<<o.created_at<<"\n";
            for (auto& it : o.items) {
                std::cout<<"  产品"<<it.product_id
                         <<" 数量"<<it.quantity
                         <<" 单价"<<it.unit_price<<"\n";
            }
        }
    }

    void cancel(int uid) {
        int oid;
        std::cout<<"订单ID: "; std::cin>>oid;
        if (svc.cancel_order(oid)) std::cout<<"取消成功\n";
        else std::cout<<"取消失败\n";
    }

    void update_addr(int uid) {
        int oid; std::cout<<"订单ID: "; std::cin>>oid;
        std::string addr;
        std::cout<<"新地址: "; std::cin.ignore(); std::getline(std::cin, addr);
        if (svc.update_address(oid, addr)) std::cout<<"更新成功\n";
        else std::cout<<"更新失败\n";
    }

    void del(int uid) {
        int oid; std::cout<<"订单ID: "; std::cin>>oid;
        if (svc.delete_order(oid)) std::cout<<"删除成功\n";
        else std::cout<<"删除失败\n";
    }
};
