#pragma once

#include <string>

class PasswordUtil {
public:
    // 加密密码（使用BCrypt）
    static std::string hashPassword(const std::string& password);

    // 验证密码
    static bool verifyPassword(const std::string& password, const std::string& hash);
};