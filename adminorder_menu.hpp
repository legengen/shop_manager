#include "service/order_service.hpp"

class AdminOrderMenu {
    OrderService& osvc;
public:
    AdminOrderMenu(OrderService& osvc): osvc(osvc) {}

    void show_menu() {
        while (true) {
            std::cout << "\n--- 订单管理菜单（管理员）---\n";
            std::cout << "1. 修改订单状态\n";
            std::cout << "2. 修改收货地址\n";
            std::cout << "0. 返回上级菜单\n";
            int op;
            std::cin >> op;
            if (op == 0) break;

            int oid;
            std::cout << "输入订单ID: ";
            std::cin >> oid;

            if (op == 1) {
                std::string status;
                std::cout << "输入新状态（如：待发货/已发货/已收货）: ";
                std::cin >> status;
                if (osvc.update_order_status(oid, status)) {
                    std::cout << "订单状态修改成功\n";
                } else {
                    std::cout << "修改失败\n";
                }
            } else if (op == 2) {
                std::string addr;
                std::cout << "输入新地址: ";
                std::cin.ignore();
                std::getline(std::cin, addr);
                if (osvc.update_order_address(oid, addr)) {
                    std::cout << "地址修改成功\n";
                } else {
                    std::cout << "修改失败\n";
                }
            }
        }
    }
};

