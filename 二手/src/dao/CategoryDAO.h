#pragma once

#include <drogon/orm/DbClient.h>
#include "model/Category.h"
#include <memory>
#include <vector>

class CategoryDAO {
public:
    static std::shared_ptr<CategoryDAO> instance();

    // 获取所有分类
    std::vector<drogon_model::campus_secondhand::CategoryPtr> findAll();

    // 获取顶级分类
    std::vector<drogon_model::campus_secondhand::CategoryPtr> findTopLevel();

    // 获取子分类
    std::vector<drogon_model::campus_secondhand::CategoryPtr> findByParentId(int parentId);

    // 根据ID查找分类
    drogon_model::campus_secondhand::CategoryPtr findById(int id);

private:
    CategoryDAO() = default;
    ~CategoryDAO() = default;
    CategoryDAO(const CategoryDAO&) = delete;
    CategoryDAO& operator=(const CategoryDAO&) = delete;
};