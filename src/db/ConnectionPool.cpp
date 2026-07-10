#include "db/ConnectionPool.h"
#include <iostream>

namespace db {

ConnectionPool::ConnectionPool() {
    init();
}

ConnectionPool::~ConnectionPool() {
    destroyPool();
}

ConnectionPool& ConnectionPool::getInstance() {
    static ConnectionPool instance;
    return instance;
}

bool ConnectionPool::init() {
    for (int i = 0; i < poolSize_; ++i) {
        MYSQL* conn = mysql_init(nullptr);
        if (conn) {
            mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");
            if (mysql_real_connect(conn, host_.c_str(), user_.c_str(), 
                                  password_.c_str(), database_.c_str(), 
                                  port_, nullptr, 0)) {
                pool_.push(conn);
            } else {
                std::cerr << "Failed to connect to MySQL: " << mysql_error(conn) << std::endl;
                mysql_close(conn);
            }
        }
    }
    return !pool_.empty();
}

void ConnectionPool::destroyPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (!pool_.empty()) {
        MYSQL* conn = pool_.front();
        pool_.pop();
        mysql_close(conn);
    }
}

MYSQL* ConnectionPool::getConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (pool_.empty()) {
        cond_.wait(lock);
    }
    
    MYSQL* conn = pool_.front();
    pool_.pop();
    
    return conn;
}

void ConnectionPool::releaseConnection(MYSQL* conn) {
    if (conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.push(conn);
        cond_.notify_one();
    }
}

}
