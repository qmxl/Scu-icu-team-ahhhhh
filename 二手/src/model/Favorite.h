#pragma once

#include <drogon/orm/Model.h>
#include <trantor/utils/Date.h>

namespace drogon_model {
namespace campus_secondhand {

class Favorite : public drogon::orm::Model<Favorite> {
  public:
    Favorite() = default;

    DEFINE_FIELD_TYPES(BIGINT, BIGINT, BIGINT, DATETIME)
    DEFINE_FIELD_NAMES(id, user_id, product_id, created_at)

    MAPPING_FUNCTION(Favorite, "favorites")

    int64_t getId() const { return getColumn("id").as<int64_t>(); }
    void setId(int64_t id) { setColumn("id", id); }

    int64_t getUserId() const { return getColumn("user_id").as<int64_t>(); }
    void setUserId(int64_t userId) { setColumn("user_id", userId); }

    int64_t getProductId() const { return getColumn("product_id").as<int64_t>(); }
    void setProductId(int64_t productId) { setColumn("product_id", productId); }

    trantor::Date getCreatedAt() const { return getColumn("created_at").as<trantor::Date>(); }
    void setCreatedAt(const trantor::Date& createdAt) { setColumn("created_at", createdAt); }
};

}  // namespace campus_secondhand
}  // namespace drogon_model