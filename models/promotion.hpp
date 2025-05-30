#pragma once
#include <string>

#define DISCOUNT  0
#define REDUCTION 1


struct Promotion {
    int id;
    int type;       // "折扣" or "满减"
    double discount_rate;   // 8折写 0.8
    double threshold;       // 满减门槛
    double reduction;       // 满减额度
    std::string start_time;
    std::string end_time;
    bool stackable;
};
