#pragma once

#include <drogon/HttpController.h>
#include "dao/OrderDAO.h"
#include "dao/ProductDAO.h"

using namespace drogon;

namespace api
{
namespace v1
{
class OrderController : public drogon::HttpController<OrderController>
{
public:
    METHOD_LIST_BEGIN
    METHOD_ADD(OrderController::createOrder, "/api/v1/orders", Post, Options);
    METHOD_ADD(OrderController::getOrders, "/api/v1/orders", Get, Options);
    METHOD_ADD(OrderController::getOrder, "/api/v1/orders/{id}", Get, Options);
    METHOD_ADD(OrderController::updateOrderStatus, "/api/v1/orders/{id}/status", Put, Options);
    METHOD_LIST_END

    // 创建订单
    void createOrder(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 获取订单列表
    void getOrders(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 获取订单详情
    void getOrder(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

    // 更新订单状态
    void updateOrderStatus(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);
};
}
}