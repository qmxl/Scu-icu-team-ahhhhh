#include "CategoryDAO.h"
#include <drogon/drogon.h>

std::shared_ptr<CategoryDAO> CategoryDAO::instance() {
    static std::shared_ptr<CategoryDAO> instance = std::make_shared<CategoryDAO>();
    return instance;
}

std::vector<drogon_model::campus_secondhand::CategoryPtr> CategoryDAO::findAll() {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Category>(
            drogon::orm::OrderBy("parent_id", drogon::orm::OrderByType::ASC),
            drogon::orm::OrderBy("sort_order", drogon::orm::OrderByType::ASC)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取分类列表失败: " << e.what();
        return {};
    }
}

std::vector<drogon_model::campus_secondhand::CategoryPtr> CategoryDAO::findTopLevel() {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Category>(
            drogon::orm::Criteria("parent_id", drogon::orm::CompareOperator::EQ, 0),
            drogon::orm::OrderBy("sort_order", drogon::orm::OrderByType::ASC)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取顶级分类失败: " << e.what();
        return {};
    }
}

std::vector<drogon_model::campus_secondhand::CategoryPtr> CategoryDAO::findByParentId(int parentId) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Category>(
            drogon::orm::Criteria("parent_id", drogon::orm::CompareOperator::EQ, parentId),
            drogon::orm::OrderBy("sort_order", drogon::orm::OrderByType::ASC)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取子分类失败: " << e.what();
        return {};
    }
}

drogon_model::campus_secondhand::CategoryPtr CategoryDAO::findById(int id) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        auto result = client->findFirst<drogon_model::campus_secondhand::Category>(
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "查找分类失败: " << e.what();
        return nullptr;
    }
}