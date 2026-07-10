#pragma once

#include <string>
#include <jwt-cpp/jwt.h>

class JwtUtil {
public:
    // 获取单例实例
    static JwtUtil& instance();

    // 初始化JWT配置
    void init();

    // 生成token
    std::string generateToken(const std::string& userId, const std::string& studentId);

    // 验证token
    bool validateToken(const std::string& token, std::string& userId);

    // 从token中获取用户ID
    std::string getUserIdFromToken(const std::string& token);

private:
    JwtUtil() = default;
    ~JwtUtil() = default;
    JwtUtil(const JwtUtil&) = delete;
    JwtUtil& operator=(const JwtUtil&) = delete;

    std::string secretKey_;
    int expireSeconds_;
};