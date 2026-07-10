#include "OrderController.h"

namespace api
{
namespace v1
{

void OrderController::createOrder(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        
        auto json = req->getJsonObject();
        if (!json) {
            response["code"] = 400;
            response["message"] = "请求参数错误";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        int64_t productId = (*json)["product_id"].asInt64();
        std::string tradeMethod = (*json)["trade_method"].asString();

        if (productId <= 0) {
            response["code"] = 400;
            response["message"] = "商品ID不能为空";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 获取商品信息
        auto product = ProductDAO::instance()->findById(productId);
        if (!product) {
            response["code"] = 404;
            response["message"] = "商品不存在";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 不能购买自己的商品
        if (product->getUserId() == userId) {
            response["code"] = 400;
            response["message"] = "不能购买自己发布的商品";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 商品状态必须是上架状态
        if (product->getStatus() != 1) {
            response["code"] = 400;
            response["message"] = "商品已下架或已售出";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 创建订单
        auto order = OrderDAO::instance()->createOrder(userId, product->getUserId(), productId, tradeMethod);

        if (order) {
            // 更新商品状态为已售出
            ProductDAO::instance()->updateStatus(productId, 2);

            response["code"] = 200;
            response["message"] = "下单成功";
            response["data"]["id"] = order->getId();
            response["data"]["order_no"] = order->getOrderNo();
        } else {
            response["code"] = 500;
            response["message"] = "下单失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "创建订单异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void OrderController::getOrders(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        std::string type = req->getOptionalParameter<std::string>("type", "buyer");
        int page = req->getOptionalParameter<int>("page", 1);
        int pageSize = req->getOptionalParameter<int>("page_size", 10);

        std::vector<drogon_model::campus_secondhand::OrderPtr> orders;
        if (type == "seller") {
            orders = OrderDAO::instance()->findBySellerId(userId, page, pageSize);
        } else {
            orders = OrderDAO::instance()->findByBuyerId(userId, page, pageSize);
        }

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& order : orders) {
            Json::Value item;
            item["id"] = order->getId();
            item["order_no"] = order->getOrderNo();
            item["product_id"] = order->getProductId();
            item["trade_method"] = order->getTradeMethod();
            item["status"] = order->getStatus();
            item["created_at"] = order->getCreatedAt().toDbString();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "获取订单列表异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void OrderController::getOrder(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        
        auto order = OrderDAO::instance()->findById(id);
        if (!order) {
            response["code"] = 404;
            response["message"] = "订单不存在";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 验证权限
        if (order->getBuyerId() != userId && order->getSellerId() != userId) {
            response["code"] = 403;
            response["message"] = "无权限查看此订单";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        response["code"] = 200;
        response["message"] = "success";
        response["data"]["id"] = order->getId();
        response["data"]["order_no"] = order->getOrderNo();
        response["data"]["buyer_id"] = order->getBuyerId();
        response["data"]["seller_id"] = order->getSellerId();
        response["data"]["product_id"] = order->getProductId();
        response["data"]["trade_method"] = order->getTradeMethod();
        response["data"]["status"] = order->getStatus();
        response["data"]["created_at"] = order->getCreatedAt().toDbString();
    } catch (const std::exception& e) {
        LOG_ERROR << "获取订单详情异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void OrderController::updateOrderStatus(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        
        auto order = OrderDAO::instance()->findById(id);
        if (!order) {
            response["code"] = 404;
            response["message"] = "订单不存在";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 验证权限（卖家可以确认订单完成）
        if (order->getSellerId() != userId) {
            response["code"] = 403;
            response["message"] = "无权限操作此订单";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        auto json = req->getJsonObject();
        if (!json) {
            response["code"] = 400;
            response["message"] = "请求参数错误";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        int status = (*json)["status"].asInt();

        bool success = OrderDAO::instance()->updateStatus(id, status);

        if (success) {
            response["code"] = 200;
            response["message"] = "更新成功";
        } else {
            response["code"] = 500;
            response["message"] = "更新失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "更新订单状态异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

}
}