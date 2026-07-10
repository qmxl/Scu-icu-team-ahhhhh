#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace db {

class ConnectionPool {
public:
    static ConnectionPool& getInstance();
    
    MYSQL* getConnection();
    void releaseConnection(MYSQL* conn);
    
    ~ConnectionPool();
    
private:
    ConnectionPool();
    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;
    
    bool init();
    void destroyPool();
    
    std::string host_ = "127.0.0.1";
    std::string user_ = "root";
    std::string password_ = "123456";
    std::string database_ = "campus_trade";
    int port_ = 3306;
    int poolSize_ = 20;
    
    std::queue<MYSQL*> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

}

#endif