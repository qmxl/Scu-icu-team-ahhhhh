#include <drogon/drogon.h>
#include "utils/JwtUtil.h"
#include "utils/RedisUtil.h"

int main() {
    // 加载配置文件
    drogon::app().loadConfigFile("./config/config.json");

    // 初始化JWT工具
    JwtUtil::instance().init();

    // 初始化Redis连接
    RedisUtil::instance().init();

    // 设置静态文件服务
    drogon::app().setDocumentRoot("./resources");

    // 注册中间件
    drogon::app().registerMiddleware(std::make_shared<AuthMiddleware>());

    // 运行应用
    drogon::app().run();

    return 0;
}