#pragma once

#include <string>
#include <sw/redis++/redis++.h>

class RedisUtil {
public:
    // 获取单例实例
    static RedisUtil& instance();

    // 初始化Redis连接
    void init();

    // 设置字符串
    bool set(const std::string& key, const std::string& value, int expireSeconds = 0);

    // 获取字符串
    bool get(const std::string& key, std::string& value);

    // 删除键
    bool del(const std::string& key);

    // 判断键是否存在
    bool exists(const std::string& key);

    // 设置哈希值
    bool hset(const std::string& key, const std::string& field, const std::string& value);

    // 获取哈希值
    bool hget(const std::string& key, const std::string& field, std::string& value);

    // 增加浏览量
    bool incrViewCount(const std::string& productId);

    // 获取浏览量
    int getViewCount(const std::string& productId);

private:
    RedisUtil() = default;
    ~RedisUtil() = default;
    RedisUtil(const RedisUtil&) = delete;
    RedisUtil& operator=(const RedisUtil&) = delete;

    std::shared_ptr<sw::redis::Redis> redis_;
};