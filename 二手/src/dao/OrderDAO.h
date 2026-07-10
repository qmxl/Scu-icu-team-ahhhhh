#pragma once

#include <drogon/orm/DbClient.h>
#include "model/Order.h"
#include <memory>
#include <vector>

class OrderDAO {
public:
    static std::shared_ptr<OrderDAO> instance();

    // 创建订单
    drogon_model::campus_secondhand::OrderPtr createOrder(int64_t buyerId,
                                                          int64_t sellerId,
                                                          int64_t productId,
                                                          const std::string& tradeMethod);

    // 根据ID查找订单
    drogon_model::campus_secondhand::OrderPtr findById(int64_t id);

    // 根据订单号查找订单
    drogon_model::campus_secondhand::OrderPtr findByOrderNo(const std::string& orderNo);

    // 根据买家ID查找订单
    std::vector<drogon_model::campus_secondhand::OrderPtr> findByBuyerId(int64_t buyerId, int page = 1, int pageSize = 10);

    // 根据卖家ID查找订单
    std::vector<drogon_model::campus_secondhand::OrderPtr> findBySellerId(int64_t sellerId, int page = 1, int pageSize = 10);

    // 更新订单状态
    bool updateStatus(int64_t id, int status);

private:
    OrderDAO() = default;
    ~OrderDAO() = default;
    OrderDAO(const OrderDAO&) = delete;
    OrderDAO& operator=(const OrderDAO&) = delete;
};