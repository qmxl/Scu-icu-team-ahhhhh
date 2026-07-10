#include "PasswordUtil.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>

std::string PasswordUtil::hashPassword(const std::string& password) {
    // 生成随机盐
    unsigned char salt[16];
    RAND_bytes(salt, sizeof(salt));

    // 组合密码和盐
    std::string saltedPassword = password + std::string(reinterpret_cast<char*>(salt), sizeof(salt));

    // 使用SHA-256哈希
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(saltedPassword.c_str()), saltedPassword.size(), hash);

    // 将盐和哈希转换为十六进制字符串
    std::stringstream ss;
    for (int i = 0; i < sizeof(salt); ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(salt[i]);
    }
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

bool PasswordUtil::verifyPassword(const std::string& password, const std::string& hash) {
    // 提取盐（前32个字符）
    std::string saltHex = hash.substr(0, 32);
    std::string originalHash = hash.substr(32);

    // 将盐从十六进制转换为字节
    unsigned char salt[16];
    for (int i = 0; i < 16; ++i) {
        std::string byteStr = saltHex.substr(i * 2, 2);
        salt[i] = static_cast<unsigned char>(std::stoi(byteStr, nullptr, 16));
    }

    // 使用相同的盐哈希密码
    std::string saltedPassword = password + std::string(reinterpret_cast<char*>(salt), sizeof(salt));

    unsigned char hashResult[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(saltedPassword.c_str()), saltedPassword.size(), hashResult);

    // 将结果转换为十六进制
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hashResult[i]);
    }

    return ss.str() == originalHash;
}