#include "FavoriteDAO.h"
#include <drogon/drogon.h>

std::shared_ptr<FavoriteDAO> FavoriteDAO::instance() {
    static std::shared_ptr<FavoriteDAO> instance = std::make_shared<FavoriteDAO>();
    return instance;
}

drogon_model::campus_secondhand::FavoritePtr FavoriteDAO::addFavorite(int64_t userId, int64_t productId) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        // 先检查是否已收藏
        auto existing = client->findFirst<drogon_model::campus_secondhand::Favorite>(
            drogon::orm::Criteria("user_id", drogon::orm::CompareOperator::EQ, userId),
            drogon::orm::Criteria("product_id", drogon::orm::CompareOperator::EQ, productId)
        );
        if (existing) {
            return existing;
        }

        drogon_model::campus_secondhand::Favorite favorite;
        favorite.setUserId(userId);
        favorite.setProductId(productId);

        auto result = client->insert(favorite);
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "添加收藏失败: " << e.what();
        return nullptr;
    }
}

bool FavoriteDAO::removeFavorite(int64_t userId, int64_t productId) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        auto result = client->deleteRows<drogon_model::campus_secondhand::Favorite>(
            drogon::orm::Criteria("user_id", drogon::orm::CompareOperator::EQ, userId),
            drogon::orm::Criteria("product_id", drogon::orm::CompareOperator::EQ, productId)
        );
        return result > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "取消收藏失败: " << e.what();
        return false;
    }
}

bool FavoriteDAO::isFavorited(int64_t userId, int64_t productId) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        auto result = client->findFirst<drogon_model::campus_secondhand::Favorite>(
            drogon::orm::Criteria("user_id", drogon::orm::CompareOperator::EQ, userId),
            drogon::orm::Criteria("product_id", drogon::orm::CompareOperator::EQ, productId)
        );
        return result != nullptr;
    } catch (const std::exception& e) {
        LOG_ERROR << "检查收藏状态失败: " << e.what();
        return false;
    }
}

std::vector<drogon_model::campus_secondhand::FavoritePtr> FavoriteDAO::findByUserId(int64_t userId, int page, int pageSize) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Favorite>(
            drogon::orm::Criteria("user_id", drogon::orm::CompareOperator::EQ, userId),
            drogon::orm::OrderBy("created_at", drogon::orm::OrderByType::DESC),
            (page - 1) * pageSize,
            pageSize
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取收藏列表失败: " << e.what();
        return {};
    }
}