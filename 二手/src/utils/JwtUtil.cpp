#include "JwtUtil.h"
#include <drogon/drogon.h>

JwtUtil& JwtUtil::instance() {
    static JwtUtil instance;
    return instance;
}

void JwtUtil::init() {
    auto config = drogon::app().getCustomConfig();
    secretKey_ = config["jwt"]["secret"].asString();
    expireSeconds_ = config["jwt"]["expireSeconds"].asInt();
}

std::string JwtUtil::generateToken(const std::string& userId, const std::string& studentId) {
    auto now = std::chrono::system_clock::now();
    auto expiresAt = now + std::chrono::seconds(expireSeconds_);

    auto token = jwt::create()
        .set_issuer("campus_secondhand")
        .set_type("JWT")
        .set_payload_claim("user_id", jwt::claim(std::string(userId)))
        .set_payload_claim("student_id", jwt::claim(studentId))
        .set_issued_at(now)
        .set_expires_at(expiresAt)
        .sign(jwt::algorithm::hs256(secretKey_));

    return token;
}

bool JwtUtil::validateToken(const std::string& token, std::string& userId) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256(secretKey_))
            .with_issuer("campus_secondhand");

        verifier.verify(decoded);

        userId = decoded.get_payload_claim("user_id").as_string();
        return true;
    } catch (const std::exception& e) {
        LOG_DEBUG << "JWT验证失败: " << e.what();
        return false;
    }
}

std::string JwtUtil::getUserIdFromToken(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        return decoded.get_payload_claim("user_id").as_string();
    } catch (const std::exception& e) {
        LOG_DEBUG << "从token获取用户ID失败: " << e.what();
        return "";
    }
}