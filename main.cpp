#include "database/database_init.hpp"
#include "Repository/user_repository.hpp"
#include "service/auth_service.hpp"
#include "user_menu.hpp"

#include "Repository/product_repository.hpp"
#include "service/product_service.hpp"
#include "admin_menu.hpp"

#include "Repository/cart_repository.hpp"
#include "service/cart_service.hpp"
#include "cart_menu.hpp"

#include "Repository/order_repository.hpp"
#include "service/order_service.hpp"
#include "order_menu.hpp"
#include "adminorder_menu.hpp"

#include "Repository/promotion_repository.hpp"
#include "service/promotion_service.hpp"
#include "promotion_menu.hpp"



int main() {
    database db("shop.db");
    DatabaseInitializer::initialize(db);

    UserRepository       urepo(db);
    AuthService          auth(urepo);
    UserMenu             umenu(auth);

    ProductRepository    prepo(db);
    ProductService       psvc(prepo);
    AdminMenu            amenu(psvc);

    CartRepository       crepo(db);
    CartService          csvc(crepo, prepo);
    CartMenu             cmenu(csvc, psvc, umenu);

    OrderRepository       orepo(db);
    OrderService          osvc(orepo, prepo, csvc, psvc);
    OrderMenu             omenu(osvc, umenu);

    PromotionRepository promo_repo(db);
    PromotionService prosvc(promo_repo);
    PromotionMenu promenu(prosvc);

    while (true) {
        std::cout<<"\n===== 主菜单 =====\n"
                 <<"1. 用户管理\n"
                 <<"2. 管理员商品管理\n"
                 <<"3. 购物车\n"
                 <<"4. 我的订单\n"
                 <<"5. 促销管理\n"
                 <<"6. 查看商品\n"
                 <<"7. 管理员订单管理\n"
                 <<"0. 退出\n"
                 <<"请选择: ";
        int op; std::cin>>op;
        if      (op==1) umenu.show_menu();
        else if (op==2) {
            if (umenu.is_admin()) amenu.show_menu();
            else std::cout<<"仅管理员可进入\n";
        }
        else if (op==3) cmenu.show_menu();
        else if (op==4) omenu.show_menu();
        else if (op==5) {
            if (umenu.is_admin()) promenu.show_menu();
            else std::cout<<"仅管理员可进入\n";
        }
        else if (op==6){
            cmenu.browse_products();
        } 
        else if (op==7){
            if (umenu.is_admin()) {
                AdminOrderMenu admin_omenu(osvc);
                admin_omenu.show_menu();
            }
            else std::cout<<"仅管理员可进入\n";
        }
        else if (op==0) break;
        else std::cout<<"无效选项\n";
    }
    return 0;
}
