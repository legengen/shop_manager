#pragma once

#include "database.hpp"

class DatabaseInitializer {
public:
    /// 初始化并创建所有需要的表结构
    static void initialize(database& db);
};
