#pragma once

#include <drogon/HttpMiddleware.h>
#include "utils/JwtUtil.h"

class AuthMiddleware : public drogon::HttpMiddleware {
public:
    void invoke(const drogon::HttpRequestPtr& req,
                drogon::MiddlewareNextCallback&& nextCb,
                drogon::MiddlewareCallback&& cb) override;

private:
    // 检查是否为不需要鉴权的路径
    bool isPublicPath(const std::string& path);
};