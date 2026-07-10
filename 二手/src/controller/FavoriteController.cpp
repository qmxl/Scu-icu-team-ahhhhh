#include "FavoriteController.h"

namespace api
{
namespace v1
{

void FavoriteController::addFavorite(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
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

        if (productId <= 0) {
            response["code"] = 400;
            response["message"] = "商品ID不能为空";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        auto favorite = FavoriteDAO::instance()->addFavorite(userId, productId);

        if (favorite) {
            response["code"] = 200;
            response["message"] = "收藏成功";
        } else {
            response["code"] = 500;
            response["message"] = "收藏失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "添加收藏异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void FavoriteController::getFavorites(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        int page = req->getOptionalParameter<int>("page", 1);
        int pageSize = req->getOptionalParameter<int>("page_size", 10);

        auto favorites = FavoriteDAO::instance()->findByUserId(userId, page, pageSize);

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& favorite : favorites) {
            Json::Value item;
            item["id"] = favorite->getId();
            item["product_id"] = favorite->getProductId();
            item["created_at"] = favorite->getCreatedAt().toDbString();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "获取收藏列表异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void FavoriteController::removeFavorite(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int productId) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");

        bool success = FavoriteDAO::instance()->removeFavorite(userId, productId);

        if (success) {
            response["code"] = 200;
            response["message"] = "取消收藏成功";
        } else {
            response["code"] = 500;
            response["message"] = "取消收藏失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "取消收藏异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void FavoriteController::checkFavorite(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int productId) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");

        bool favorited = FavoriteDAO::instance()->isFavorited(userId, productId);

        response["code"] = 200;
        response["message"] = "success";
        response["data"]["favorited"] = favorited;
    } catch (const std::exception& e) {
        LOG_ERROR << "检查收藏状态异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

}
}