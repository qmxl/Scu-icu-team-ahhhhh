#pragma once

#include <drogon/orm/Model.h>
#include <trantor/utils/Date.h>

namespace drogon_model {
namespace campus_secondhand {

class Category : public drogon::orm::Model<Category> {
  public:
    Category() = default;

    DEFINE_FIELD_TYPES(INT, VARCHAR, INT, VARCHAR, INT, TINYINT, DATETIME, DATETIME)
    DEFINE_FIELD_NAMES(id, name, parent_id, icon, sort_order, status, created_at, updated_at)

    MAPPING_FUNCTION(Category, "categories")

    int getId() const { return getColumn("id").as<int>(); }
    void setId(int id) { setColumn("id", id); }

    const std::string& getName() const { return getColumn("name").as<std::string>(); }
    void setName(const std::string& name) { setColumn("name", name); }

    int getParentId() const { return getColumn("parent_id").as<int>(); }
    void setParentId(int parentId) { setColumn("parent_id", parentId); }

    const std::string& getIcon() const { return getColumn("icon").as<std::string>(); }
    void setIcon(const std::string& icon) { setColumn("icon", icon); }

    int getSortOrder() const { return getColumn("sort_order").as<int>(); }
    void setSortOrder(int sortOrder) { setColumn("sort_order", sortOrder); }

    int getStatus() const { return getColumn("status").as<int>(); }
    void setStatus(int status) { setColumn("status", status); }

    trantor::Date getCreatedAt() const { return getColumn("created_at").as<trantor::Date>(); }
    void setCreatedAt(const trantor::Date& createdAt) { setColumn("created_at", createdAt); }

    trantor::Date getUpdatedAt() const { return getColumn("updated_at").as<trantor::Date>(); }
    void setUpdatedAt(const trantor::Date& updatedAt) { setColumn("updated_at", updatedAt); }
};

}  // namespace campus_secondhand
}  // namespace drogon_model