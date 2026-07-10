#pragma once

#include <drogon/HttpController.h>
#include "dao/ProductDAO.h"
#include "dao/CategoryDAO.h"
#include "utils/FileUtil.h"

using namespace drogon;

namespace api
{
namespace v1
{
class ProductController : public drogon::HttpController<ProductController>
{
public:
    METHOD_LIST_BEGIN
    METHOD_ADD(ProductController::createProduct, "/api/v1/products", Post, Options);
    METHOD_ADD(ProductController::getProducts, "/api/v1/products", Get, Options);
    METHOD_ADD(ProductController::getProduct, "/api/v1/products/{id}", Get, Options);
    METHOD_ADD(ProductController::updateProduct, "/api/v1/products/{id}", Put, Options);
    METHOD_ADD(ProductController::deleteProduct, "/api/v1/products/{id}", Delete, Options);
    METHOD_ADD(ProductController::searchProducts, "/api/v1/products/search", Get, Options);
    METHOD_ADD(ProductController::getProductsByCategory, "/api/v1/products/category/{categoryId}", Get, Options);
    METHOD_ADD(ProductController::getMyProducts, "/api/v1/products/my", Get, Options);
    METHOD_ADD(ProductController::uploadImage, "/api/v1/products/upload", Post, Options);
    METHOD_LIST_END

    // 创建商品
    void createProduct(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 获取商品列表
    void getProducts(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 获取商品详情
    void getProduct(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

    // 更新商品
    void updateProduct(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

    // 删除商品
    void deleteProduct(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

    // 搜索商品
    void searchProducts(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 根据分类获取商品
    void getProductsByCategory(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int categoryId);

    // 获取我的商品
    void getMyProducts(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 上传图片
    void uploadImage(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};
}
}