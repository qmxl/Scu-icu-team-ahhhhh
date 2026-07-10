#include "service/StatisticsService.h"
#include "service/UserService.h"
#include "service/GoodsService.h"
#include "service/TransactionService.h"
#include "db/ConnectionPool.h"
#include <mysql.h>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace service {

StatisticsService& StatisticsService::getInstance() {
    static StatisticsService instance;
    return instance;
}

std::map<std::string, int> StatisticsService::getStatistics() {
    std::map<std::string, int> stats;
    
    stats["user_count"] = UserService::getInstance().getUserCount();
    stats["goods_count"] = GoodsService::getInstance().getGoodsCount();
    stats["transaction_count"] = TransactionService::getInstance().getTransactionCount();
    stats["pending_count"] = TransactionService::getInstance().getTransactionCountByStatus(0);
    stats["confirmed_count"] = TransactionService::getInstance().getTransactionCountByStatus(1);
    stats["completed_count"] = TransactionService::getInstance().getTransactionCountByStatus(2);
    
    return stats;
}

std::string StatisticsService::getTableName(int year, int month) {
    std::stringstream ss;
    ss << "statistics_" << year << std::setw(2) << std::setfill('0') << month;
    return ss.str();
}

std::string StatisticsService::getCurrentTableName() {
    std::time_t now = std::time(nullptr);
    std::tm* tm = std::localtime(&now);
    return getTableName(tm->tm_year + 1900, tm->tm_mon + 1);
}

void StatisticsService::ensureTableExists(const std::string& tableName) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "id INT(11) PRIMARY KEY AUTO_INCREMENT,"
        "type VARCHAR(50) NOT NULL,"
        "data TEXT,"
        "date DATE NOT NULL,"
        "INDEX idx_type (type),"
        "INDEX idx_date (date)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";
    
    mysql_query(conn, sql.c_str());
    db::ConnectionPool::getInstance().releaseConnection(conn);
}

void StatisticsService::updateStatistics() {
    std::string tableName = getCurrentTableName();
    ensureTableExists(tableName);
    
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::map<std::string, int> stats = getStatistics();
    
    for (auto& pair : stats) {
        std::string sql = "INSERT INTO " + tableName + " (type, data, date) VALUES ('" + 
            pair.first + "', '" + std::to_string(pair.second) + "', CURDATE()) ON DUPLICATE KEY UPDATE data = '" + std::to_string(pair.second) + "'";
        mysql_query(conn, sql.c_str());
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
}

}