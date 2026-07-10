#include "httplib.h"
#include "json.hpp"
#include "service/UserService.h"
#include "service/GoodsService.h"
#include "service/TransactionService.h"
#include "service/ConfigService.h"
#include "service/StatisticsService.h"
#include "db/ConnectionPool.h"
#include <iostream>
#include <thread>
#include <chrono>

using json = nlohmann::json;

void handleCORS(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

json parse(const std::string& body) {
    try {
        return json::parse(body);
    } catch (...) {
        return json();
    }
}

void autoConfirmThread() {
    while (true) {
        service::TransactionService::getInstance().checkAutoConfirm();
        std::this_thread::sleep_for(std::chrono::hours(1));
    }
}

void statisticsThread() {
    service::StatisticsService::getInstance().updateStatistics();
    while (true) {
        service::StatisticsService::getInstance().updateStatistics();
        std::this_thread::sleep_for(std::chrono::hours(1));
    }
}

std::string getStatusText(int status) {
    if (status == -1) return "canceled";
    if (status == 0) return "pending";
    if (status == 1) return "confirmed";
    return "completed";
}

int main() {
    std::cout << "Initializing database connection..." << std::endl;
    db::ConnectionPool::getInstance();
    
    httplib::Server server;
    
    server.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        handleCORS(res);
        res.status = 200;
    });
    
    server.Post("/api/user/login", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        std::string username = data["username"].is_string() ? data["username"].get<std::string>() : "";
        std::string password = data["password"].is_string() ? data["password"].get<std::string>() : "";
        
        model::User user = service::UserService::getInstance().login(username, password);
        
        if (user.id > 0) {
            res.set_content(json({
                {"code", 200},
                {"message", "login success"},
                {"data", json({
                    {"id", user.id},
                    {"username", user.username},
                    {"phone", user.phone},
                    {"qq", user.qq},
                    {"role", user.role}
                })}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 401},
                {"message", "username or password error"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/user/register", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        std::string username = data["username"].is_string() ? data["username"].get<std::string>() : "";
        std::string password = data["password"].is_string() ? data["password"].get<std::string>() : "";
        std::string phone = data["phone"].is_string() ? data["phone"].get<std::string>() : "";
        std::string qq = data["qq"].is_string() ? data["qq"].get<std::string>() : "";
        
        bool success = service::UserService::getInstance().registerUser(username, password, phone, qq);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "register success"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "register failed, username may exist"}
            }).dump(), "application/json");
        }
    });
    
    server.Get("/api/user/profile", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        int id = std::stoi(req.get_param_value("id"));
        
        model::User user = service::UserService::getInstance().getUserById(id);
        
        if (user.id > 0) {
            res.set_content(json({
                {"code", 200},
                {"data", json({
                    {"id", user.id},
                    {"username", user.username},
                    {"phone", user.phone},
                    {"qq", user.qq},
                    {"role", user.role},
                    {"status", user.status},
                    {"created_at", user.created_at}
                })}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 404},
                {"message", "user not found"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/user/update", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        int id = data["id"].is_number() ? data["id"].get<int>() : 0;
        std::string phone = data["phone"].is_string() ? data["phone"].get<std::string>() : "";
        std::string qq = data["qq"].is_string() ? data["qq"].get<std::string>() : "";
        
        bool success = service::UserService::getInstance().updateProfile(id, phone, qq);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "update success"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "update failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Get("/api/user/list", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        int page = std::stoi(req.get_param_value("page"));
        int pageSize = std::stoi(req.get_param_value("pageSize"));
        
        std::vector<model::User> users = service::UserService::getInstance().getAllUsers(page, pageSize);
        int count = service::UserService::getInstance().getUserCount();
        
        json userList = json::array();
        for (auto& user : users) {
            userList.push_back(json({
                {"id", user.id},
                {"username", user.username},
                {"phone", user.phone},
                {"qq", user.qq},
                {"role", user.role},
                {"status", user.status},
                {"created_at", user.created_at}
            }));
        }
        
        res.set_content(json({
            {"code", 200},
            {"data", userList},
            {"total", count}
        }).dump(), "application/json");
    });
    
    server.Post("/api/user/status", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        int id = data["id"].is_number() ? data["id"].get<int>() : 0;
        int status = data["status"].is_number() ? data["status"].get<int>() : 0;
        
        bool success = service::UserService::getInstance().updateUserStatus(id, status);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "operation success"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "operation failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/goods/publish", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        
        model::Goods goods;
        goods.name = data["name"].is_string() ? data["name"].get<std::string>() : "";
        goods.description = data["description"].is_string() ? data["description"].get<std::string>() : "";
        goods.price = data["price"].is_number() ? data["price"].get<double>() : 0;
        goods.images = data["images"].is_string() ? data["images"].get<std::string>() : "";
        goods.category = data["category"].is_string() ? data["category"].get<std::string>() : "";
        goods.seller_id = data["seller_id"].is_number() ? data["seller_id"].get<int>() : 0;
        bool showContact = data["show_contact"].is_boolean() ? data["show_contact"].get<bool>() : true;
        goods.is_locked = showContact ? 0 : 1;
        goods.status = 1;
        
        bool success = service::GoodsService::getInstance().publishGoods(goods);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "publish success"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "publish failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Get("/api/goods/list", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        int page = std::stoi(req.get_param_value("page"));
        int pageSize = std::stoi(req.get_param_value("pageSize"));
        std::string category = req.get_param_value("category");
        std::string keyword = req.get_param_value("keyword");
        int sellerId = 0;
        if (!req.get_param_value("sellerId").empty()) {
            sellerId = std::stoi(req.get_param_value("sellerId"));
        }
        
        std::vector<model::Goods> goodsList = service::GoodsService::getInstance().getGoodsList(page, pageSize, category, keyword, sellerId);
        int count = service::GoodsService::getInstance().getGoodsCount();
        
        json list = json::array();
        for (auto& goods : goodsList) {
            list.push_back(json({
                {"id", goods.id},
                {"name", goods.name},
                {"description", goods.description},
                {"price", goods.price},
                {"images", goods.images},
                {"category", goods.category},
                {"seller_id", goods.seller_id},
                {"seller_name", goods.seller_name},
                {"is_locked", goods.is_locked},
                {"status", goods.status},
                {"created_at", goods.created_at}
            }));
        }
        
        res.set_content(json({
            {"code", 200},
            {"data", json({
                {"list", list},
                {"total", count}
            })}
        }).dump(), "application/json");
    });
    
    server.Get("/api/goods/detail", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        int id = std::stoi(req.get_param_value("id"));
        
        model::Goods goods = service::GoodsService::getInstance().getGoodsById(id);
        
        if (goods.id > 0) {
            model::User seller = service::UserService::getInstance().getUserById(goods.seller_id);
            
            res.set_content(json({
                {"code", 200},
                {"data", json({
                    {"id", goods.id},
                    {"name", goods.name},
                    {"description", goods.description},
                    {"price", goods.price},
                    {"images", goods.images},
                    {"category", goods.category},
                    {"seller_id", goods.seller_id},
                    {"seller_name", seller.username},
                    {"seller_phone", goods.is_locked ? "" : seller.phone},
                    {"seller_qq", goods.is_locked ? "" : seller.qq},
                    {"is_locked", goods.is_locked},
                    {"status", goods.status},
                    {"created_at", goods.created_at}
                })}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 404},
                {"message", "goods not found"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/goods/update", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        
        model::Goods goods;
        goods.id = data["id"].is_number() ? data["id"].get<int>() : 0;
        goods.name = data["name"].is_string() ? data["name"].get<std::string>() : "";
        goods.description = data["description"].is_string() ? data["description"].get<std::string>() : "";
        goods.price = data["price"].is_number() ? data["price"].get<double>() : 0;
        goods.images = data["images"].is_string() ? data["images"].get<std::string>() : "";
        goods.category = data["category"].is_string() ? data["category"].get<std::string>() : "";
        goods.is_locked = data["is_locked"].is_number() ? data["is_locked"].get<int>() : 0;
        goods.status = data["status"].is_number() ? data["status"].get<int>() : 0;
        
        bool success = service::GoodsService::getInstance().updateGoods(goods);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "update success"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "update failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/goods/status", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        int id = data["id"].is_number() ? data["id"].get<int>() : 0;
        int status = data["status"].is_number() ? data["status"].get<int>() : 0;
        
        bool success = service::GoodsService::getInstance().setGoodsStatus(id, status);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "operation success"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "operation failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/goods/lock", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        int id = data["id"].is_number() ? data["id"].get<int>() : 0;
        bool isLocked = data["is_locked"].is_boolean() ? data["is_locked"].get<bool>() : false;
        
        bool success = service::GoodsService::getInstance().toggleLock(id, isLocked);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", isLocked ? "locked" : "unlocked"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "operation failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/goods/delete", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        int id = data["id"].is_number() ? data["id"].get<int>() : 0;
        
        bool success = service::GoodsService::getInstance().deleteGoods(id);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "delete success"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "delete failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/transaction/create", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        int goodsId = data["goods_id"].is_number() ? data["goods_id"].get<int>() : 0;
        int buyerId = data["buyer_id"].is_number() ? data["buyer_id"].get<int>() : 0;
        int sellerId = data["seller_id"].is_number() ? data["seller_id"].get<int>() : 0;
        
        bool success = service::TransactionService::getInstance().createTransaction(goodsId, buyerId, sellerId);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "apply success, waiting for seller confirm"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "apply failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/transaction/confirm", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        std::string transactionId = data["id"].is_string() ? data["id"].get<std::string>() : "";
        
        bool success = service::TransactionService::getInstance().confirmTransaction(transactionId);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "confirmed, waiting for buyer"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "confirm failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/transaction/complete", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        std::string transactionId = data["id"].is_string() ? data["id"].get<std::string>() : "";
        
        bool success = service::TransactionService::getInstance().completeTransaction(transactionId);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "transaction completed"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "operation failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Post("/api/transaction/cancel", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        std::string transactionId = data["id"].is_string() ? data["id"].get<std::string>() : "";
        std::string reason = data["reason"].is_string() ? data["reason"].get<std::string>() : "";
        
        bool success = service::TransactionService::getInstance().cancelTransaction(transactionId, reason);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "transaction canceled"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "cancel failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Get("/api/transaction/list", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        int page = std::stoi(req.get_param_value("page"));
        int pageSize = std::stoi(req.get_param_value("pageSize"));
        int userId = std::stoi(req.get_param_value("userId"));
        std::string type = req.get_param_value("type");
        
        std::vector<model::Transaction> transactions;
        if (type == "buyer") {
            transactions = service::TransactionService::getInstance().getTransactionsByBuyer(userId, page, pageSize);
        } else {
            transactions = service::TransactionService::getInstance().getTransactionsBySeller(userId, page, pageSize);
        }
        
        json list = json::array();
        for (auto& tx : transactions) {
            std::string goodsImage = "";
            if (!tx.goods_images.empty()) {
                size_t comma = tx.goods_images.find(',');
                goodsImage = tx.goods_images.substr(0, comma);
            }
            
            std::string statusText = "";
            switch(tx.status) {
                case 0: statusText = "pending"; break;
                case 1: statusText = "trading"; break;
                case 2: statusText = "completed"; break;
                case -1: statusText = "cancelled"; break;
                default: statusText = "unknown";
            }
            
            list.push_back(json({
                {"id", tx.id},
                {"goods_id", tx.goods_id},
                {"goods_name", tx.goods_name},
                {"goods_image", goodsImage},
                {"price", tx.goods_price},
                {"buyer_id", tx.buyer_id},
                {"buyer_name", tx.buyer_name},
                {"seller_id", tx.seller_id},
                {"seller_name", tx.seller_name},
                {"status", tx.status},
                {"status_text", statusText},
                {"created_at", tx.created_at},
                {"confirm_time", tx.confirm_time},
                {"cancel_reason", tx.cancel_reason}
            }));
        }
        
        int count = 0;
        MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
        std::string field = (type == "buyer" ? "buyer_id" : "seller_id");
        std::string sql = "SELECT COUNT(*) FROM transactions WHERE " + field + " = " + std::to_string(userId);
        mysql_query(conn, sql.c_str());
        MYSQL_RES* result = mysql_store_result(conn);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) count = atoi(row[0]);
            mysql_free_result(result);
        }
        db::ConnectionPool::getInstance().releaseConnection(conn);
        
        res.set_content(json({
            {"code", 200},
            {"data", json({
                {"list", list},
                {"total", count}
            })}
        }).dump(), "application/json");
    });
    
    server.Get("/api/config/list", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        
        auto configs = service::ConfigService::getInstance().getAllConfigs();
        
        json list = json::array();
        for (auto& pair : configs) {
            list.push_back(json({
                {"key", pair.first},
                {"value", pair.second}
            }));
        }
        
        res.set_content(json({
            {"code", 200},
            {"data", list}
        }).dump(), "application/json");
    });
    
    server.Post("/api/config/update", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        json data = parse(req.body);
        std::string key = data["key"].is_string() ? data["key"].get<std::string>() : "";
        std::string value = data["value"].is_string() ? data["value"].get<std::string>() : "";
        std::string description = data["description"].is_string() ? data["description"].get<std::string>() : "";
        
        bool success = service::ConfigService::getInstance().setConfig(key, value, description);
        
        if (success) {
            res.set_content(json({
                {"code", 200},
                {"message", "config update success"}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 500},
                {"message", "update failed"}
            }).dump(), "application/json");
        }
    });
    
    server.Get("/api/statistics", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        
        auto stats = service::StatisticsService::getInstance().getStatistics();
        
        json result;
        for (auto& pair : stats) {
            result[pair.first] = pair.second;
        }
        
        res.set_content(json({
            {"code", 200},
            {"data", result}
        }).dump(), "application/json");
    });
    
    server.Get("/api/statistics/data", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        
        std::string type = req.get_param_value("type");
        int year = std::stoi(req.get_param_value("year"));
        
        if (type == "overview") {
            auto stats = service::StatisticsService::getInstance().getStatistics();
            json result;
            for (auto& pair : stats) {
                result[pair.first] = pair.second;
            }
            res.set_content(json({
                {"code", 200},
                {"data", result}
            }).dump(), "application/json");
        } else if (type == "goods_monthly") {
            MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
            std::string sql = "SELECT MONTH(created_at) as month, COUNT(*) as value FROM goods WHERE YEAR(created_at) = " + std::to_string(year) + " GROUP BY MONTH(created_at) ORDER BY month";
            mysql_query(conn, sql.c_str());
            MYSQL_RES* result = mysql_store_result(conn);
            
            json list = json::array();
            if (result) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(result)) != nullptr) {
                    list.push_back(json({
                        {"month", atoi(row[0])},
                        {"value", atoi(row[1])}
                    }));
                }
                mysql_free_result(result);
            }
            db::ConnectionPool::getInstance().releaseConnection(conn);
            res.set_content(json({
                {"code", 200},
                {"data", list}
            }).dump(), "application/json");
        } else if (type == "transaction_monthly") {
            MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
            std::string sql = "SELECT MONTH(created_at) as month, COUNT(*) as value FROM transactions WHERE YEAR(created_at) = " + std::to_string(year) + " GROUP BY MONTH(created_at) ORDER BY month";
            mysql_query(conn, sql.c_str());
            MYSQL_RES* result = mysql_store_result(conn);
            
            json list = json::array();
            if (result) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(result)) != nullptr) {
                    list.push_back(json({
                        {"month", atoi(row[0])},
                        {"value", atoi(row[1])}
                    }));
                }
                mysql_free_result(result);
            }
            db::ConnectionPool::getInstance().releaseConnection(conn);
            res.set_content(json({
                {"code", 200},
                {"data", list}
            }).dump(), "application/json");
        } else if (type == "category") {
            MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
            std::string sql = "SELECT category as name, COUNT(*) as value FROM goods GROUP BY category ORDER BY value DESC";
            mysql_query(conn, sql.c_str());
            MYSQL_RES* result = mysql_store_result(conn);
            
            json list = json::array();
            if (result) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(result)) != nullptr) {
                    list.push_back(json({
                        {"name", row[0] ? row[0] : ""},
                        {"value", atoi(row[1])}
                    }));
                }
                mysql_free_result(result);
            }
            db::ConnectionPool::getInstance().releaseConnection(conn);
            res.set_content(json({
                {"code", 200},
                {"data", list}
            }).dump(), "application/json");
        } else if (type == "stats_daily") {
            MYSQL* conn = db::ConnectionPool::getInstance().getConnection();
            
            json list = json::array();
            for (int month = 1; month <= 12; month++) {
                std::string tableName = service::StatisticsService::getInstance().getTableName(year, month);
                
                std::string checkSql = "SHOW TABLES LIKE '" + tableName + "'";
                mysql_query(conn, checkSql.c_str());
                MYSQL_RES* checkResult = mysql_store_result(conn);
                if (checkResult && mysql_num_rows(checkResult) > 0) {
                    std::string sql = "SELECT date, type, data FROM " + tableName + " ORDER BY date";
                    mysql_query(conn, sql.c_str());
                    MYSQL_RES* result = mysql_store_result(conn);
                    
                    if (result) {
                        MYSQL_ROW row;
                        while ((row = mysql_fetch_row(result)) != nullptr) {
                            list.push_back(json({
                                {"date", row[0] ? row[0] : ""},
                                {"type", row[1] ? row[1] : ""},
                                {"data", row[2] ? row[2] : ""}
                            }));
                        }
                        mysql_free_result(result);
                    }
                }
                if (checkResult) mysql_free_result(checkResult);
            }
            
            db::ConnectionPool::getInstance().releaseConnection(conn);
            res.set_content(json({
                {"code", 200},
                {"data", list}
            }).dump(), "application/json");
        } else {
            res.set_content(json({
                {"code", 400},
                {"message", "invalid type"}
            }).dump(), "application/json");
        }
    });
    
    server.set_base_dir("e:/trae 文件/test01/frontend");
    
    server.Get("/api/statistics/update", [](const httplib::Request& req, httplib::Response& res) {
        handleCORS(res);
        service::StatisticsService::getInstance().updateStatistics();
        res.set_content(json({
            {"code", 200},
            {"message", "statistics updated"}
        }).dump(), "application/json");
    });
    
    std::thread(autoConfirmThread).detach();
    std::thread(statisticsThread).detach();
    
    std::cout << "Server running on http://localhost:8080" << std::endl;
    std::cout << "Pre-initializing database connections..." << std::endl;
    db::ConnectionPool::getInstance();
    std::cout << "Database connections ready!" << std::endl;
    
    server.listen("0.0.0.0", 8080);
    
    return 0;
}