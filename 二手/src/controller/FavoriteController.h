#pragma once

#include <drogon/HttpController.h>
#include "dao/FavoriteDAO.h"

using namespace drogon;

namespace api
{
namespace v1
{
class FavoriteController : public drogon::HttpController<FavoriteController>
{
public:
    METHOD_LIST_BEGIN
    METHOD_ADD(FavoriteController::addFavorite, "/api/v1/favorites", Post, Options);
    METHOD_ADD(FavoriteController::getFavorites, "/api/v1/favorites", Get, Options);
    METHOD_ADD(FavoriteController::removeFavorite, "/api/v1/favorites/{productId}", Delete, Options);
    METHOD_ADD(FavoriteController::checkFavorite, "/api/v1/favorites/{productId}/check", Get, Options);
    METHOD_LIST_END

    // 添加收藏
    void addFavorite(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 获取收藏列表
    void getFavorites(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // 取消收藏
    void removeFavorite(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int productId);

    // 检查是否已收藏
    void checkFavorite(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int productId);
};
}
}