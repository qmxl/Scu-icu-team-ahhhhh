#include "service/GoodsService.h"
#include "db/ConnectionPool.h"
#include "cache/CacheManager.h"
#include <mysql.h>
#include <sstream>
#include <cstring>

namespace service {

GoodsService& GoodsService::getInstance() {
    static GoodsService instance;
    return instance;
}

bool GoodsService::publishGoods(const model::Goods& goods) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "INSERT INTO goods (name, description, price, images, category, seller_id, is_locked, status) VALUES ('" +
                      goods.name + "', '" + goods.description + "', " + std::to_string(goods.price) + ", '" +
                      goods.images + "', '" + goods.category + "', " + std::to_string(goods.seller_id) + ", " +
                      std::to_string(goods.is_locked) + ", " + std::to_string(goods.status) + ")";
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return success;
}

std::vector<model::Goods> GoodsService::getGoodsList(int page, int pageSize, const std::string& category, 
                                                      const std::string& keyword, int sellerId) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    int offset = (page - 1) * pageSize;
    std::string sql = "SELECT g.id, g.name, g.description, g.price, g.images, g.category, g.seller_id, g.is_locked, g.status, g.created_at, g.updated_at, u.username as seller_name FROM goods g LEFT JOIN users u ON g.seller_id = u.id WHERE 1=1";
    
    if (!category.empty()) {
        sql += " AND g.category = '" + category + "'";
    }
    if (!keyword.empty()) {
        sql += " AND (g.name LIKE '%" + keyword + "%' OR g.description LIKE '%" + keyword + "%')";
    }
    if (sellerId > 0) {
        sql += " AND g.seller_id = " + std::to_string(sellerId);
    }
    
    sql += " ORDER BY g.id DESC LIMIT " + std::to_string(offset) + ", " + std::to_string(pageSize);
    
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    std::vector<model::Goods> goodsList;
    if (result) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != nullptr) {
            model::Goods goods;
            goods.id = atoi(row[0]);
            goods.name = row[1] ? row[1] : "";
            goods.description = row[2] ? row[2] : "";
            goods.price = atof(row[3]);
            goods.images = row[4] ? row[4] : "";
            goods.category = row[5] ? row[5] : "";
            goods.seller_id = atoi(row[6]);
            goods.is_locked = atoi(row[7]);
            goods.status = atoi(row[8]);
            goods.created_at = row[9] ? row[9] : "";
            goods.updated_at = row[10] ? row[10] : "";
            goods.seller_name = row[11] ? row[11] : "";
            goodsList.push_back(goods);
            
            cache::CacheManager::getInstance().setGoods(goods);
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return goodsList;
}

model::Goods GoodsService::getGoodsById(int id) {
    cache::CacheManager& cache = cache::CacheManager::getInstance();
    if (cache.hasGoods(id)) {
        return cache.getGoods(id);
    }
    
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT id, name, description, price, images, category, seller_id, is_locked, status, created_at, updated_at FROM goods WHERE id = " + std::to_string(id);
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    model::Goods goods;
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            goods.id = atoi(row[0]);
            goods.name = row[1] ? row[1] : "";
            goods.description = row[2] ? row[2] : "";
            goods.price = atof(row[3]);
            goods.images = row[4] ? row[4] : "";
            goods.category = row[5] ? row[5] : "";
            goods.seller_id = atoi(row[6]);
            goods.is_locked = atoi(row[7]);
            goods.status = atoi(row[8]);
            goods.created_at = row[9] ? row[9] : "";
            goods.updated_at = row[10] ? row[10] : "";
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (goods.id != 0) {
        cache.setGoods(goods);
    }
    
    return goods;
}

bool GoodsService::updateGoods(const model::Goods& goods) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "UPDATE goods SET name = '" + goods.name + "', description = '" + goods.description + 
                      "', price = " + std::to_string(goods.price) + ", images = '" + goods.images + 
                      "', category = '" + goods.category + "', is_locked = " + std::to_string(goods.is_locked) + 
                      ", status = " + std::to_string(goods.status) + " WHERE id = " + std::to_string(goods.id);
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeGoods(goods.id);
    }
    
    return success;
}

bool GoodsService::deleteGoods(int id) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "DELETE FROM goods WHERE id = " + std::to_string(id);
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeGoods(id);
    }
    
    return success;
}

bool GoodsService::setGoodsStatus(int id, int status) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "UPDATE goods SET status = " + std::to_string(status) + " WHERE id = " + std::to_string(id);
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeGoods(id);
    }
    
    return success;
}

bool GoodsService::toggleLock(int id, bool isLocked) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    int locked = isLocked ? 1 : 0;
    std::string sql = "UPDATE goods SET is_locked = " + std::to_string(locked) + " WHERE id = " + std::to_string(id);
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeGoods(id);
    }
    
    return success;
}

int GoodsService::getGoodsCount() {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT COUNT(*) FROM goods";
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

int GoodsService::getGoodsCountByCategory(const std::string& category) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT COUNT(*) FROM goods WHERE category = '" + category + "'";
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

int GoodsService::getGoodsCountBySeller(int sellerId) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT COUNT(*) FROM goods WHERE seller_id = " + std::to_string(sellerId);
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

}