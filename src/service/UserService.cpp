#include "service/UserService.h"
#include "db/ConnectionPool.h"
#include "cache/CacheManager.h"
#include "utils/HashUtil.h"
#include <mysql.h>
#include <sstream>
#include <cstring>

namespace service {

UserService& UserService::getInstance() {
    static UserService instance;
    return instance;
}

std::string UserService::hashPassword(const std::string& password) {
    return utils::HashUtil::sha256(password);
}

model::User UserService::login(const std::string& username, const std::string& password) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    std::string hashedPassword = hashPassword(password);
    
    std::string sql = "SELECT id, username, password, phone, qq, role, status, created_at, updated_at FROM users WHERE username = '" + username + "'";
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    model::User user;
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            user.id = atoi(row[0]);
            user.username = row[1] ? row[1] : "";
            std::string dbPassword = row[2] ? row[2] : "";
            user.phone = row[3] ? row[3] : "";
            user.qq = row[4] ? row[4] : "";
            user.role = row[5] ? atoi(row[5]) : 0;
            user.status = row[6] ? atoi(row[6]) : 0;
            user.created_at = row[7] ? row[7] : "";
            user.updated_at = row[8] ? row[8] : "";
            
            if (dbPassword != hashedPassword || user.status != 1) {
                user.id = 0;
            } else {
                cache::CacheManager::getInstance().setUser(user);
            }
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return user;
}

bool UserService::registerUser(const std::string& username, const std::string& password,
                               const std::string& phone, const std::string& qq) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string checkSql = "SELECT COUNT(*) FROM users WHERE username = '" + username + "'";
    mysql_query(conn, checkSql.c_str());
    MYSQL_RES* checkResult = mysql_store_result(conn);
    int count = 0;
    if (checkResult) {
        MYSQL_ROW row = mysql_fetch_row(checkResult);
        if (row) {
            count = atoi(row[0]);
        }
        mysql_free_result(checkResult);
    }
    if (count > 0) {
        db::ConnectionPool::getInstance().releaseConnection(conn);
        return false;
    }
    
    std::string hashedPassword = hashPassword(password);
    std::string sql = "INSERT INTO users (username, password, phone, qq) VALUES ('" + 
                      username + "', '" + hashedPassword + "', '" + phone + "', '" + qq + "')";
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return success;
}

model::User UserService::getUserById(int id) {
    cache::CacheManager& cache = cache::CacheManager::getInstance();
    if (cache.hasUser(id)) {
        return cache.getUser(id);
    }
    
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT id, username, password, phone, qq, role, status, created_at, updated_at FROM users WHERE id = " + std::to_string(id);
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    model::User user;
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            user.id = atoi(row[0]);
            user.username = row[1] ? row[1] : "";
            user.password = row[2] ? row[2] : "";
            user.phone = row[3] ? row[3] : "";
            user.qq = row[4] ? row[4] : "";
            user.role = row[5] ? atoi(row[5]) : 0;
            user.status = row[6] ? atoi(row[6]) : 0;
            user.created_at = row[7] ? row[7] : "";
            user.updated_at = row[8] ? row[8] : "";
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (user.id != 0) {
        cache.setUser(user);
    }
    
    return user;
}

model::User UserService::getUserByUsername(const std::string& username) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT id, username, password, phone, qq, role, status, created_at, updated_at FROM users WHERE username = '" + username + "'";
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    model::User user;
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            user.id = atoi(row[0]);
            user.username = row[1] ? row[1] : "";
            user.password = row[2] ? row[2] : "";
            user.phone = row[3] ? row[3] : "";
            user.qq = row[4] ? row[4] : "";
            user.role = row[5] ? atoi(row[5]) : 0;
            user.status = row[6] ? atoi(row[6]) : 0;
            user.created_at = row[7] ? row[7] : "";
            user.updated_at = row[8] ? row[8] : "";
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return user;
}

bool UserService::updateProfile(int id, const std::string& phone, const std::string& qq) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "UPDATE users SET phone = '" + phone + "', qq = '" + qq + "' WHERE id = " + std::to_string(id);
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeUser(id);
    }
    
    return success;
}

std::vector<model::User> UserService::getAllUsers(int page, int pageSize) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    int offset = (page - 1) * pageSize;
    std::string sql = "SELECT id, username, phone, qq, role, status, created_at FROM users ORDER BY id DESC LIMIT " + 
                      std::to_string(offset) + ", " + std::to_string(pageSize);
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    std::vector<model::User> users;
    if (result) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != nullptr) {
            model::User user;
            user.id = atoi(row[0]);
            user.username = row[1] ? row[1] : "";
            user.phone = row[2] ? row[2] : "";
            user.qq = row[3] ? row[3] : "";
            user.role = row[4] ? atoi(row[4]) : 0;
            user.status = row[5] ? atoi(row[5]) : 0;
            user.created_at = row[6] ? row[6] : "";
            users.push_back(user);
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return users;
}

bool UserService::updateUserStatus(int id, int status) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "UPDATE users SET status = " + std::to_string(status) + " WHERE id = " + std::to_string(id);
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    
    if (success) {
        cache::CacheManager::getInstance().removeUser(id);
    }
    
    return success;
}

int UserService::getUserCount() {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT COUNT(*) FROM users";
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