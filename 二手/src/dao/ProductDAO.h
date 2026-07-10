#pragma once

#include <drogon/orm/DbClient.h>
#include "model/Product.h"
#include <memory>
#include <vector>

class ProductDAO {
public:
    static std::shared_ptr<ProductDAO> instance();

    // 创建商品
    drogon_model::campus_secondhand::ProductPtr createProduct(int64_t userId,
                                                              int categoryId,
                                                              const std::string& title,
                                                              const std::string& description,
                                                              double originalPrice,
                                                              double price,
                                                              const std::string& images);

    // 根据ID查找商品
    drogon_model::campus_secondhand::ProductPtr findById(int64_t id);

    // 获取所有商品列表
    std::vector<drogon_model::campus_secondhand::ProductPtr> findAll(int page = 1, int pageSize = 10);

    // 根据分类查找商品
    std::vector<drogon_model::campus_secondhand::ProductPtr> findByCategory(int categoryId, int page = 1, int pageSize = 10);

    // 根据用户ID查找商品
    std::vector<drogon_model::campus_secondhand::ProductPtr> findByUserId(int64_t userId, int page = 1, int pageSize = 10);

    // 搜索商品
    std::vector<drogon_model::campus_secondhand::ProductPtr> search(const std::string& keyword, int page = 1, int pageSize = 10);

    // 更新商品
    bool updateProduct(int64_t id, const std::map<std::string, std::string>& fields);

    // 删除商品
    bool deleteProduct(int64_t id);

    // 更新浏览量
    bool incrementViews(int64_t id);

    // 更新商品状态（下架/售出）
    bool updateStatus(int64_t id, int status);

private:
    ProductDAO() = default;
    ~ProductDAO() = default;
    ProductDAO(const ProductDAO&) = delete;
    ProductDAO& operator=(const ProductDAO&) = delete;
};