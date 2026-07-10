#include "OrderDAO.h"
#include <drogon/drogon.h>
#include <random>
#include <sstream>

std::shared_ptr<OrderDAO> OrderDAO::instance() {
    static std::shared_ptr<OrderDAO> instance = std::make_shared<OrderDAO>();
    return instance;
}

std::string generateOrderNo() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&now_time);
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d%H%M%S");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    ss << dis(gen);
    
    return ss.str();
}

drogon_model::campus_secondhand::OrderPtr OrderDAO::createOrder(int64_t buyerId,
                                                               int64_t sellerId,
                                                               int64_t productId,
                                                               const std::string& tradeMethod) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        drogon_model::campus_secondhand::Order order;
        order.setOrderNo(generateOrderNo());
        order.setBuyerId(buyerId);
        order.setSellerId(sellerId);
        order.setProductId(productId);
        order.setTradeMethod(tradeMethod);
        order.setStatus(1);

        auto result = client->insert(order);
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "创建订单失败: " << e.what();
        return nullptr;
    }
}

drogon_model::campus_secondhand::OrderPtr OrderDAO::findById(int64_t id) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        auto result = client->findFirst<drogon_model::campus_secondhand::Order>(
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "查找订单失败: " << e.what();
        return nullptr;
    }
}

drogon_model::campus_secondhand::OrderPtr OrderDAO::findByOrderNo(const std::string& orderNo) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        auto result = client->findFirst<drogon_model::campus_secondhand::Order>(
            drogon::orm::Criteria("order_no", drogon::orm::CompareOperator::EQ, orderNo)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "查找订单失败: " << e.what();
        return nullptr;
    }
}

std::vector<drogon_model::campus_secondhand::OrderPtr> OrderDAO::findByBuyerId(int64_t buyerId, int page, int pageSize) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Order>(
            drogon::orm::Criteria("buyer_id", drogon::orm::CompareOperator::EQ, buyerId),
            drogon::orm::OrderBy("created_at", drogon::orm::OrderByType::DESC),
            (page - 1) * pageSize,
            pageSize
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取买家订单失败: " << e.what();
        return {};
    }
}

std::vector<drogon_model::campus_secondhand::OrderPtr> OrderDAO::findBySellerId(int64_t sellerId, int page, int pageSize) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Order>(
            drogon::orm::Criteria("seller_id", drogon::orm::CompareOperator::EQ, sellerId),
            drogon::orm::OrderBy("created_at", drogon::orm::OrderByType::DESC),
            (page - 1) * pageSize,
            pageSize
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取卖家订单失败: " << e.what();
        return {};
    }
}

bool OrderDAO::updateStatus(int64_t id, int status) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        drogon::orm::Update update;
        update.set("status", status);
        if (status == 2) {
            update.set("completed_at", trantor::Date::now());
        }

        auto result = client->update<drogon_model::campus_secondhand::Order>(
            update,
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "更新订单状态失败: " << e.what();
        return false;
    }
}