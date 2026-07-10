#pragma once

#include <drogon/orm/Model.h>
#include <trantor/utils/Date.h>

namespace drogon_model {
namespace campus_secondhand {

class Product : public drogon::orm::Model<Product> {
  public:
    Product() = default;

    DEFINE_FIELD_TYPES(BIGINT, BIGINT, INT, VARCHAR, TEXT, DECIMAL, DECIMAL, TEXT, TINYINT, INT, DATETIME, DATETIME, DATETIME)
    DEFINE_FIELD_NAMES(id, user_id, category_id, title, description, original_price, price, images, status, views, sold_at, created_at, updated_at)

    MAPPING_FUNCTION(Product, "products")

    int64_t getId() const { return getColumn("id").as<int64_t>(); }
    void setId(int64_t id) { setColumn("id", id); }

    int64_t getUserId() const { return getColumn("user_id").as<int64_t>(); }
    void setUserId(int64_t userId) { setColumn("user_id", userId); }

    int getCategoryId() const { return getColumn("category_id").as<int>(); }
    void setCategoryId(int categoryId) { setColumn("category_id", categoryId); }

    const std::string& getTitle() const { return getColumn("title").as<std::string>(); }
    void setTitle(const std::string& title) { setColumn("title", title); }

    const std::string& getDescription() const { return getColumn("description").as<std::string>(); }
    void setDescription(const std::string& description) { setColumn("description", description); }

    double getOriginalPrice() const { return getColumn("original_price").as<double>(); }
    void setOriginalPrice(double originalPrice) { setColumn("original_price", originalPrice); }

    double getPrice() const { return getColumn("price").as<double>(); }
    void setPrice(double price) { setColumn("price", price); }

    const std::string& getImages() const { return getColumn("images").as<std::string>(); }
    void setImages(const std::string& images) { setColumn("images", images); }

    int getStatus() const { return getColumn("status").as<int>(); }
    void setStatus(int status) { setColumn("status", status); }

    int getViews() const { return getColumn("views").as<int>(); }
    void setViews(int views) { setColumn("views", views); }

    trantor::Date getSoldAt() const { return getColumn("sold_at").as<trantor::Date>(); }
    void setSoldAt(const trantor::Date& soldAt) { setColumn("sold_at", soldAt); }

    trantor::Date getCreatedAt() const { return getColumn("created_at").as<trantor::Date>(); }
    void setCreatedAt(const trantor::Date& createdAt) { setColumn("created_at", createdAt); }

    trantor::Date getUpdatedAt() const { return getColumn("updated_at").as<trantor::Date>(); }
    void setUpdatedAt(const trantor::Date& updatedAt) { setColumn("updated_at", updatedAt); }
};

}  // namespace campus_secondhand
}  // namespace drogon_model