#include "service/TransactionService.h"
#include "service/GoodsService.h"
#include "db/ConnectionPool.h"
#include "cache/CacheManager.h"
#include <mysql.h>
#include <sstream>
#include <cstring>
#include <ctime>
#include <chrono>

namespace service {

TransactionService& TransactionService::getInstance() {
    static TransactionService instance;
    return instance;
}

std::string TransactionService::generateTransactionId() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    struct tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &time_t_now);
#else
    localtime_r(&time_t_now, &tm_buf);
#endif
    
    char id[33];
    sprintf(id, "%04d%02d%02d%02d%02d%02d%08d", 
            tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
            tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec, rand());
    
    return std::string(id);
}

bool TransactionService::createTransaction(int goodsId, int buyerId, int sellerId) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string transactionId = generateTransactionId();
    
    std::string sql = "INSERT INTO transactions (id, goods_id, buyer_id, seller_id, status, start_time) VALUES ('" + 
                      transactionId + "', " + std::to_string(goodsId) + ", " + std::to_string(buyerId) + ", " + 
                      std::to_string(sellerId) + ", 0, NOW())";
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        GoodsService::getInstance().setGoodsStatus(goodsId, 2);
    }
    
    return success;
}

bool TransactionService::confirmTransaction(const std::string& transactionId) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "UPDATE transactions SET status = 1 WHERE id = '" + transactionId + "' AND status = 0";
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeTransaction(transactionId);
    }
    
    return success;
}

bool TransactionService::completeTransaction(const std::string& transactionId) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "UPDATE transactions SET status = 2, confirm_time = NOW() WHERE id = '" + transactionId + "' AND status = 1";
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeTransaction(transactionId);
        
        model::Transaction tx = getTransactionById(transactionId);
        if (!tx.id.empty()) {
            GoodsService::getInstance().setGoodsStatus(tx.goods_id, 3);
        }
    }
    
    return success;
}

bool TransactionService::cancelTransaction(const std::string& transactionId, const std::string& reason) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "UPDATE transactions SET status = -1, cancel_reason = '" + reason + "' WHERE id = '" + transactionId + "' AND status < 2";
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeTransaction(transactionId);
        
        model::Transaction tx = getTransactionById(transactionId);
        if (!tx.id.empty()) {
            GoodsService::getInstance().setGoodsStatus(tx.goods_id, 1);
        }
    }
    
    return success;
}

model::Transaction TransactionService::getTransactionById(const std::string& transactionId) {
    cache::CacheManager& cache = cache::CacheManager::getInstance();
    if (cache.hasTransaction(transactionId)) {
        return cache.getTransaction(transactionId);
    }
    
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT id, goods_id, buyer_id, seller_id, status, start_time, confirm_time, cancel_reason, created_at FROM transactions WHERE id = '" + transactionId + "'";
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    model::Transaction tx;
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            tx.id = row[0] ? row[0] : "";
            tx.goods_id = atoi(row[1]);
            tx.buyer_id = atoi(row[2]);
            tx.seller_id = atoi(row[3]);
            tx.status = atoi(row[4]);
            tx.start_time = row[5] ? row[5] : "";
            tx.confirm_time = row[6] ? row[6] : "";
            tx.cancel_reason = row[7] ? row[7] : "";
            tx.created_at = row[8] ? row[8] : "";
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (!tx.id.empty()) {
        cache.setTransaction(tx);
    }
    
    return tx;
}

std::vector<model::Transaction> TransactionService::getTransactionsByBuyer(int buyerId, int page, int pageSize) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    int offset = (page - 1) * pageSize;
    std::string sql = "SELECT t.id, t.goods_id, t.buyer_id, t.seller_id, t.status, t.start_time, t.confirm_time, t.cancel_reason, t.created_at, "
                      "g.name as goods_name, g.images as goods_images, g.price as goods_price, "
                      "bu.username as buyer_name, su.username as seller_name "
                      "FROM transactions t LEFT JOIN goods g ON t.goods_id = g.id "
                      "LEFT JOIN users bu ON t.buyer_id = bu.id "
                      "LEFT JOIN users su ON t.seller_id = su.id "
                      "WHERE t.buyer_id = " + std::to_string(buyerId) + " ORDER BY t.id DESC LIMIT " + std::to_string(offset) + ", " + std::to_string(pageSize);
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    std::vector<model::Transaction> transactions;
    if (result) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != nullptr) {
            model::Transaction tx;
            tx.id = row[0] ? row[0] : "";
            tx.goods_id = atoi(row[1]);
            tx.buyer_id = atoi(row[2]);
            tx.seller_id = atoi(row[3]);
            tx.status = atoi(row[4]);
            tx.start_time = row[5] ? row[5] : "";
            tx.confirm_time = row[6] ? row[6] : "";
            tx.cancel_reason = row[7] ? row[7] : "";
            tx.created_at = row[8] ? row[8] : "";
            tx.goods_name = row[9] ? row[9] : "";
            tx.goods_images = row[10] ? row[10] : "";
            tx.goods_price = row[11] ? atof(row[11]) : 0;
            tx.buyer_name = row[12] ? row[12] : "";
            tx.seller_name = row[13] ? row[13] : "";
            transactions.push_back(tx);
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return transactions;
}

std::vector<model::Transaction> TransactionService::getTransactionsBySeller(int sellerId, int page, int pageSize) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    int offset = (page - 1) * pageSize;
    std::string sql = "SELECT t.id, t.goods_id, t.buyer_id, t.seller_id, t.status, t.start_time, t.confirm_time, t.cancel_reason, t.created_at, "
                      "g.name as goods_name, g.images as goods_images, g.price as goods_price, "
                      "bu.username as buyer_name, su.username as seller_name "
                      "FROM transactions t LEFT JOIN goods g ON t.goods_id = g.id "
                      "LEFT JOIN users bu ON t.buyer_id = bu.id "
                      "LEFT JOIN users su ON t.seller_id = su.id "
                      "WHERE t.seller_id = " + std::to_string(sellerId) + " ORDER BY t.id DESC LIMIT " + std::to_string(offset) + ", " + std::to_string(pageSize);
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    std::vector<model::Transaction> transactions;
    if (result) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != nullptr) {
            model::Transaction tx;
            tx.id = row[0] ? row[0] : "";
            tx.goods_id = atoi(row[1]);
            tx.buyer_id = atoi(row[2]);
            tx.seller_id = atoi(row[3]);
            tx.status = atoi(row[4]);
            tx.start_time = row[5] ? row[5] : "";
            tx.confirm_time = row[6] ? row[6] : "";
            tx.cancel_reason = row[7] ? row[7] : "";
            tx.created_at = row[8] ? row[8] : "";
            tx.goods_name = row[9] ? row[9] : "";
            tx.goods_images = row[10] ? row[10] : "";
            tx.goods_price = row[11] ? atof(row[11]) : 0;
            tx.buyer_name = row[12] ? row[12] : "";
            tx.seller_name = row[13] ? row[13] : "";
            transactions.push_back(tx);
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return transactions;
}

int TransactionService::getTransactionCount() {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT COUNT(*) FROM transactions";
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    int count = 0;
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            count = atoi(row[0]);
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return count;
}

int TransactionService::getTransactionCountByStatus(int status) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT COUNT(*) FROM transactions WHERE status = " + std::to_string(status);
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    int count = 0;
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            count = atoi(row[0]);
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return count;
}

void TransactionService::checkAutoConfirm() {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "UPDATE transactions SET status = 2, confirm_time = NOW() WHERE status = 1 AND start_time < DATE_SUB(NOW(), INTERVAL 7 DAY)";
    mysql_query(conn, sql.c_str());
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
}

}