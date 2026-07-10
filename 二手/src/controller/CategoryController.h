#pragma once

#include <drogon/HttpController.h>
#include "dao/CategoryDAO.h"

using namespace drogon;

namespace api
{
namespace v1
{
class CategoryController : public drogon::HttpController<CategoryController>
{
public:
    METHOD_LIST_BEGIN
    METHOD_ADD(CategoryController::getCategories, "/api/v1/categories", Get, Options);
    METHOD_ADD(CategoryController::getCategory, "/api/v1/categories/{id}", Get, Options);
    METHOD_ADD(CategoryController::getTopLevel, "/api/v1/categories/top", Get, Options);
    METHOD_ADD(CategoryController::getChildren, "/api/v1/categories/{id}/children", Get, Options);
    METHOD_LIST_END

    // 获取所有分类
    void getCategories(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 获取分类详情
    void getCategory(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

    // 获取顶级分类
    void getTopLevel(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 获取子分类
    void getChildren(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);
};
}
}