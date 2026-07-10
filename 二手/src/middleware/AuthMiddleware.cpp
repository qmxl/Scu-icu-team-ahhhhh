#include "AuthMiddleware.h"

void AuthMiddleware::invoke(const drogon::HttpRequestPtr& req,
                            drogon::MiddlewareNextCallback&& nextCb,
                            drogon::MiddlewareCallback&& cb) {
    std::string path = req->getPath();
    
    // 检查是否为公开路径
    if (isPublicPath(path)) {
        nextCb();
        return;
    }

    // 获取Authorization header
    std::string authHeader = req->getHeader("Authorization");
    if (authHeader.empty()) {
        auto res = drogon::HttpResponse::newHttpResponse();
        res->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
        res->setBody(R"({"code":401,"message":"未登录"})");
        cb(res);
        return;
    }

    // 提取token（Bearer xxx）
    if (authHeader.substr(0, 7) != "Bearer ") {
        auto res = drogon::HttpResponse::newHttpResponse();
        res->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
        res->setBody(R"({"code":401,"message":"无效的token格式"})");
        cb(res);
        return;
    }

    std::string token = authHeader.substr(7);
    std::string userId;

    // 验证token
    if (!JwtUtil::instance().validateToken(token, userId)) {
        auto res = drogon::HttpResponse::newHttpResponse();
        res->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
        res->setBody(R"({"code":401,"message":"token无效或已过期"})");
        cb(res);
        return;
    }

    // 将用户ID存入请求属性
    req->setAttribute("userId", userId);
    nextCb();
}

bool AuthMiddleware::isPublicPath(const std::string& path) {
    std::vector<std::string> publicPaths = {
        "/api/auth/login",
        "/api/auth/register",
        "/api/products",
        "/api/products/search",
        "/api/categories",
        "/",
        "/index.html",
        "/login.html",
        "/register.html"
    };

    for (const auto& publicPath : publicPaths) {
        if (path == publicPath || path.rfind(publicPath, 0) == 0) {
            return true;
        }
    }
    return false;
}