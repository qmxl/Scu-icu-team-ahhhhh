#include "ProductController.h"

namespace api
{
namespace v1
{

void ProductController::createProduct(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
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

        int categoryId = (*json)["category_id"].asInt();
        std::string title = (*json)["title"].asString();
        std::string description = (*json)["description"].asString();
        double originalPrice = (*json)["original_price"].asDouble();
        double price = (*json)["price"].asDouble();
        std::string images = (*json)["images"].asString();

        if (title.empty() || price <= 0) {
            response["code"] = 400;
            response["message"] = "商品标题和价格不能为空";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        auto product = ProductDAO::instance()->createProduct(userId, categoryId, title, description, originalPrice, price, images);

        if (product) {
            response["code"] = 200;
            response["message"] = "发布成功";
            response["data"]["id"] = product->getId();
        } else {
            response["code"] = 500;
            response["message"] = "发布失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "创建商品异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void ProductController::getProducts(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        int page = req->getOptionalParameter<int>("page", 1);
        int pageSize = req->getOptionalParameter<int>("page_size", 10);

        auto products = ProductDAO::instance()->findAll(page, pageSize);

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& product : products) {
            Json::Value item;
            item["id"] = product->getId();
            item["title"] = product->getTitle();
            item["price"] = product->getPrice();
            item["original_price"] = product->getOriginalPrice();
            item["images"] = product->getImages();
            item["views"] = product->getViews();
            item["status"] = product->getStatus();
            item["created_at"] = product->getCreatedAt().toDbString();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "获取商品列表异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void ProductController::getProduct(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) {
    Json::Value response;
    
    try {
        auto product = ProductDAO::instance()->findById(id);
        if (!product) {
            response["code"] = 404;
            response["message"] = "商品不存在";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 更新浏览量
        ProductDAO::instance()->incrementViews(id);

        response["code"] = 200;
        response["message"] = "success";
        response["data"]["id"] = product->getId();
        response["data"]["user_id"] = product->getUserId();
        response["data"]["category_id"] = product->getCategoryId();
        response["data"]["title"] = product->getTitle();
        response["data"]["description"] = product->getDescription();
        response["data"]["original_price"] = product->getOriginalPrice();
        response["data"]["price"] = product->getPrice();
        response["data"]["images"] = product->getImages();
        response["data"]["views"] = product->getViews() + 1;
        response["data"]["status"] = product->getStatus();
        response["data"]["created_at"] = product->getCreatedAt().toDbString();
    } catch (const std::exception& e) {
        LOG_ERROR << "获取商品详情异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void ProductController::updateProduct(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        
        auto product = ProductDAO::instance()->findById(id);
        if (!product) {
            response["code"] = 404;
            response["message"] = "商品不存在";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 验证权限
        if (product->getUserId() != userId) {
            response["code"] = 403;
            response["message"] = "无权限修改此商品";
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

        std::map<std::string, std::string> fields;
        if ((*json).isMember("title") && !(*json)["title"].asString().empty()) {
            fields["title"] = (*json)["title"].asString();
        }
        if ((*json).isMember("description") && !(*json)["description"].asString().empty()) {
            fields["description"] = (*json)["description"].asString();
        }
        if ((*json).isMember("price")) {
            fields["price"] = std::to_string((*json)["price"].asDouble());
        }
        if ((*json).isMember("images") && !(*json)["images"].asString().empty()) {
            fields["images"] = (*json)["images"].asString();
        }

        bool success = ProductDAO::instance()->updateProduct(id, fields);

        if (success) {
            response["code"] = 200;
            response["message"] = "更新成功";
        } else {
            response["code"] = 500;
            response["message"] = "更新失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "更新商品异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void ProductController::deleteProduct(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        
        auto product = ProductDAO::instance()->findById(id);
        if (!product) {
            response["code"] = 404;
            response["message"] = "商品不存在";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // 验证权限
        if (product->getUserId() != userId) {
            response["code"] = 403;
            response["message"] = "无权限删除此商品";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        bool success = ProductDAO::instance()->deleteProduct(id);

        if (success) {
            response["code"] = 200;
            response["message"] = "删除成功";
        } else {
            response["code"] = 500;
            response["message"] = "删除失败";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "删除商品异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void ProductController::searchProducts(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        std::string keyword = req->getOptionalParameter<std::string>("keyword", "");
        int page = req->getOptionalParameter<int>("page", 1);
        int pageSize = req->getOptionalParameter<int>("page_size", 10);

        if (keyword.empty()) {
            response["code"] = 400;
            response["message"] = "搜索关键词不能为空";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        auto products = ProductDAO::instance()->search(keyword, page, pageSize);

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& product : products) {
            Json::Value item;
            item["id"] = product->getId();
            item["title"] = product->getTitle();
            item["price"] = product->getPrice();
            item["images"] = product->getImages();
            item["views"] = product->getViews();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "搜索商品异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void ProductController::getProductsByCategory(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int categoryId) {
    Json::Value response;
    
    try {
        int page = req->getOptionalParameter<int>("page", 1);
        int pageSize = req->getOptionalParameter<int>("page_size", 10);

        auto products = ProductDAO::instance()->findByCategory(categoryId, page, pageSize);

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& product : products) {
            Json::Value item;
            item["id"] = product->getId();
            item["title"] = product->getTitle();
            item["price"] = product->getPrice();
            item["images"] = product->getImages();
            item["views"] = product->getViews();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "获取分类商品异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void ProductController::getMyProducts(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        int64_t userId = req->getAttributes()->get<int64_t>("user_id");
        int page = req->getOptionalParameter<int>("page", 1);
        int pageSize = req->getOptionalParameter<int>("page_size", 10);

        auto products = ProductDAO::instance()->findByUserId(userId, page, pageSize);

        response["code"] = 200;
        response["message"] = "success";
        response["data"] = Json::arrayValue;
        for (const auto& product : products) {
            Json::Value item;
            item["id"] = product->getId();
            item["title"] = product->getTitle();
            item["price"] = product->getPrice();
            item["status"] = product->getStatus();
            item["created_at"] = product->getCreatedAt().toDbString();
            response["data"].append(item);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "获取我的商品异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

void ProductController::uploadImage(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    
    try {
        auto files = req->getUploadedFiles();
        if (files.empty()) {
            response["code"] = 400;
            response["message"] = "请选择要上传的图片";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        auto& file = files[0];
        std::string filename = FileUtil::saveUploadedFile(file);

        if (filename.empty()) {
            response["code"] = 500;
            response["message"] = "上传失败";
            callback(HttpResponse::newHttpJsonResponse(response));
            return;
        }

        response["code"] = 200;
        response["message"] = "上传成功";
        response["data"]["url"] = "/uploads/" + filename;
    } catch (const std::exception& e) {
        LOG_ERROR << "上传图片异常: " << e.what();
        response["code"] = 500;
        response["message"] = "服务器内部错误";
    }

    callback(HttpResponse::newHttpJsonResponse(response));
}

}
}