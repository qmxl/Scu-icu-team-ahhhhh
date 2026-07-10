#include "CategoryController.h"

namespace api
{
namespace v1
{

void CategoryController::getCategories(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        auto categories = CategoryDAO::instance()->findAll();

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& category : categories) {
            Json::Value item;
            item["id"] = category->getId();
            item["name"] = category->getName();
            item["parent_id"] = category->getParentId();
            item["icon"] = category->getIcon();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "获取分类列表异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void CategoryController::getCategory(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) {
    Json::Value response;
    
    try {
        auto category = CategoryDAO::instance()->findById(id);
        if (!category) {
            response["code"] = 404;
            response["message"] = "分类不存在";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        response["code"] = 200;
        response["message"] = "success";
        response["data"]["id"] = category->getId();
        response["data"]["name"] = category->getName();
        response["data"]["parent_id"] = category->getParentId();
        response["data"]["icon"] = category->getIcon();
    } catch (const std::exception& e) {
        LOG_ERROR << "获取分类详情异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void CategoryController::getTopLevel(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        auto categories = CategoryDAO::instance()->findTopLevel();

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& category : categories) {
            Json::Value item;
            item["id"] = category->getId();
            item["name"] = category->getName();
            item["icon"] = category->getIcon();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "获取顶级分类异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void CategoryController::getChildren(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) {
    Json::Value response;
    
    try {
        auto categories = CategoryDAO::instance()->findByParentId(id);

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& category : categories) {
            Json::Value item;
            item["id"] = category->getId();
            item["name"] = category->getName();
            item["icon"] = category->getIcon();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "获取子分类异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

}
}