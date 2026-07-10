#include "RedisUtil.h"
#include <drogon/drogon.h>

RedisUtil& RedisUtil::instance() {
    static RedisUtil instance;
    return instance;
}

void RedisUtil::init() {
    auto config = drogon::app().getCustomConfig();
    std::string host = config["redis"]["host"].asString();
    int port = config["redis"]["port"].asInt();
    std::string password = config["redis"]["password"].asString();
    int db = config["redis"]["db"].asInt();

    try {
        sw::redis::ConnectionOptions opts;
        opts.host = host;
        opts.port = port;
        if (!password.empty()) {
            opts.password = password;
        }
        opts.db = db;

        redis_ = std::make_shared<sw::redis::Redis>(opts);
        LOG_INFO << "Redis连接成功";
    } catch (const std::exception& e) {
        LOG_ERROR << "Redis连接失败: " << e.what();
    }
}

bool RedisUtil::set(const std::string& key, const std::string& value, int expireSeconds) {
    if (!redis_) return false;
    try {
        redis_->set(key, value);
        if (expireSeconds > 0) {
            redis_->expire(key, std::chrono::seconds(expireSeconds));
        }
        return true;
    } catch (const std::exception& e) {
        LOG_DEBUG << "Redis set失败: " << e.what();
        return false;
    }
}

bool RedisUtil::get(const std::string& key, std::string& value) {
    if (!redis_) return false;
    try {
        auto result = redis_->get(key);
        if (result) {
            value = *result;
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        LOG_DEBUG << "Redis get失败: " << e.what();
        return false;
    }
}

bool RedisUtil::del(const std::string& key) {
    if (!redis_) return false;
    try {
        redis_->del(key);
        return true;
    } catch (const std::exception& e) {
        LOG_DEBUG << "Redis del失败: " << e.what();
        return false;
    }
}

bool RedisUtil::exists(const std::string& key) {
    if (!redis_) return false;
    try {
        return redis_->exists(key);
    } catch (const std::exception& e) {
        LOG_DEBUG << "Redis exists失败: " << e.what();
        return false;
    }
}

bool RedisUtil::hset(const std::string& key, const std::string& field, const std::string& value) {
    if (!redis_) return false;
    try {
        redis_->hset(key, field, value);
        return true;
    } catch (const std::exception& e) {
        LOG_DEBUG << "Redis hset失败: " << e.what();
        return false;
    }
}

bool RedisUtil::hget(const std::string& key, const std::string& field, std::string& value) {
    if (!redis_) return false;
    try {
        auto result = redis_->hget(key, field);
        if (result) {
            value = *result;
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        LOG_DEBUG << "Redis hget失败: " << e.what();
        return false;
    }
}

bool RedisUtil::incrViewCount(const std::string& productId) {
    if (!redis_) return false;
    try {
        std::string key = "product:view:" + productId;
        redis_->incr(key);
        return true;
    } catch (const std::exception& e) {
        LOG_DEBUG << "Redis incr失败: " << e.what();
        return false;
    }
}

int RedisUtil::getViewCount(const std::string& productId) {
    if (!redis_) return 0;
    try {
        std::string key = "product:view:" + productId;
        auto result = redis_->get(key);
        if (result) {
            return std::stoi(*result);
        }
        return 0;
    } catch (const std::exception& e) {
        LOG_DEBUG << "Redis get view count失败: " << e.what();
        return 0;
    }
}