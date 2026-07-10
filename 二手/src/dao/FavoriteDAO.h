#pragma once

#include <drogon/orm/DbClient.h>
#include "model/Favorite.h"
#include <memory>
#include <vector>

class FavoriteDAO {
public:
    static std::shared_ptr<FavoriteDAO> instance();

    // 添加收藏
    drogon_model::campus_secondhand::FavoritePtr addFavorite(int64_t userId, int64_t productId);

    // 取消收藏
    bool removeFavorite(int64_t userId, int64_t productId);

    // 判断是否已收藏
    bool isFavorited(int64_t userId, int64_t productId);

    // 获取用户收藏列表
    std::vector<drogon_model::campus_secondhand::FavoritePtr> findByUserId(int64_t userId, int page = 1, int pageSize = 10);

private:
    FavoriteDAO() = default;
    ~FavoriteDAO() = default;
    FavoriteDAO(const FavoriteDAO&) = delete;
    FavoriteDAO& operator=(const FavoriteDAO&) = delete;
};