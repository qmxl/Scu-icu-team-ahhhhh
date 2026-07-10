#include "ProductDAO.h"
#include <drogon/drogon.h>

std::shared_ptr<ProductDAO> ProductDAO::instance() {
    static std::shared_ptr<ProductDAO> instance = std::make_shared<ProductDAO>();
    return instance;
}

drogon_model::campus_secondhand::ProductPtr ProductDAO::createProduct(int64_t userId,
                                                                      int categoryId,
                                                                      const std::string& title,
                                                                      const std::string& description,
                                                                      double originalPrice,
                                                                      double price,
                                                                      const std::string& images) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        drogon_model::campus_secondhand::Product product;
        product.setUserId(userId);
        product.setCategoryId(categoryId);
        product.setTitle(title);
        product.setDescription(description);
        product.setOriginalPrice(originalPrice);
        product.setPrice(price);
        product.setImages(images);
        product.setStatus(1);
        product.setViews(0);

        auto result = client->insert(product);
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "创建商品失败: " << e.what();
        return nullptr;
    }
}

drogon_model::campus_secondhand::ProductPtr ProductDAO::findById(int64_t id) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return nullptr;
    }

    try {
        auto result = client->findFirst<drogon_model::campus_secondhand::Product>(
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "查找商品失败: " << e.what();
        return nullptr;
    }
}

std::vector<drogon_model::campus_secondhand::ProductPtr> ProductDAO::findAll(int page, int pageSize) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Product>(
            drogon::orm::Criteria("status", drogon::orm::CompareOperator::EQ, 1),
            drogon::orm::OrderBy("created_at", drogon::orm::OrderByType::DESC),
            (page - 1) * pageSize,
            pageSize
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取商品列表失败: " << e.what();
        return {};
    }
}

std::vector<drogon_model::campus_secondhand::ProductPtr> ProductDAO::findByCategory(int categoryId, int page, int pageSize) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Product>(
            drogon::orm::Criteria("category_id", drogon::orm::CompareOperator::EQ, categoryId),
            drogon::orm::Criteria("status", drogon::orm::CompareOperator::EQ, 1),
            drogon::orm::OrderBy("created_at", drogon::orm::OrderByType::DESC),
            (page - 1) * pageSize,
            pageSize
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取分类商品失败: " << e.what();
        return {};
    }
}

std::vector<drogon_model::campus_secondhand::ProductPtr> ProductDAO::findByUserId(int64_t userId, int page, int pageSize) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        auto result = client->find<drogon_model::campus_secondhand::Product>(
            drogon::orm::Criteria("user_id", drogon::orm::CompareOperator::EQ, userId),
            drogon::orm::OrderBy("created_at", drogon::orm::OrderByType::DESC),
            (page - 1) * pageSize,
            pageSize
        );
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR << "获取用户商品失败: " << e.what();
        return {};
    }
}

std::vector<drogon_model::campus_secondhand::ProductPtr> ProductDAO::search(const std::string& keyword, int page, int pageSize) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return {};
    }

    try {
        std::string sql = "SELECT * FROM products WHERE status = 1 AND (title LIKE ? OR description LIKE ?) ORDER BY created_at DESC LIMIT ?, ?";
        auto result = client->execSqlSync(sql, "%" + keyword + "%", "%" + keyword + "%", (page - 1) * pageSize, pageSize);
        
        std::vector<drogon_model::campus_secondhand::ProductPtr> products;
        for (const auto& row : result) {
            auto product = std::make_shared<drogon_model::campus_secondhand::Product>();
            product->fromJson(row.toJson());
            products.push_back(product);
        }
        return products;
    } catch (const std::exception& e) {
        LOG_ERROR << "搜索商品失败: " << e.what();
        return {};
    }
}

bool ProductDAO::updateProduct(int64_t id, const std::map<std::string, std::string>& fields) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        drogon::orm::Update update;
        for (const auto& [key, value] : fields) {
            update.set(key, value);
        }

        auto result = client->update<drogon_model::campus_secondhand::Product>(
            update,
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "更新商品失败: " << e.what();
        return false;
    }
}

bool ProductDAO::deleteProduct(int64_t id) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        auto result = client->deleteRows<drogon_model::campus_secondhand::Product>(
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "删除商品失败: " << e.what();
        return false;
    }
}

bool ProductDAO::incrementViews(int64_t id) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        std::string sql = "UPDATE products SET views = views + 1 WHERE id = ?";
        auto result = client->execSqlSync(sql, id);
        return result.affectedRows() > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "更新浏览量失败: " << e.what();
        return false;
    }
}

bool ProductDAO::updateStatus(int64_t id, int status) {
    auto client = drogon::app().getDbClient();
    if (!client) {
        LOG_ERROR << "数据库连接失败";
        return false;
    }

    try {
        drogon::orm::Update update;
        update.set("status", status);
        if (status == 2) {
            update.set("sold_at", trantor::Date::now());
        }

        auto result = client->update<drogon_model::campus_secondhand::Product>(
            update,
            drogon::orm::Criteria("id", drogon::orm::CompareOperator::EQ, id)
        );
        return result > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "更新商品状态失败: " << e.what();
        return false;
    }
}