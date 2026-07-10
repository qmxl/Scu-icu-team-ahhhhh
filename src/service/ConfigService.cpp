#include "service/ConfigService.h"
#include "db/ConnectionPool.h"
#include <mysql.h>

namespace service {

ConfigService& ConfigService::getInstance() {
    static ConfigService instance;
    return instance;
}

std::string ConfigService::getConfig(const std::string& key) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT value FROM configs WHERE `key` = '" + key + "'";
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    std::string value;
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            value = row[0] ? row[0] : "";
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return value;
}

bool ConfigService::setConfig(const std::string& key, const std::string& value, const std::string& description) {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "INSERT INTO configs (`key`, value, description) VALUES ('" + key + "', '" + value + "', '" + description + "') ON DUPLICATE KEY UPDATE value = '" + value + "', description = '" + description + "'";
    bool success = mysql_query(conn, sql.c_str()) == 0;
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return success;
}

std::map<std::string, std::string> ConfigService::getAllConfigs() {
    MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
    
    std::string sql = "SELECT `key`, value FROM configs";
    mysql_query(conn, sql.c_str());
    MYSQL_RES* result = mysql_store_result(conn);
    
    std::map<std::string, std::string> configs;
    if (result) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != nullptr) {
            configs[row[0] ? row[0] : ""] = row[1] ? row[1] : "";
        }
        mysql_free_result(result);
    }
    
    db::ConnectionPool::getInstance().releaseConnection(conn);
    return configs;
}

}