#pragma once

#include <drogon/orm/Model.h>
#include <trantor/utils/Date.h>

namespace drogon_model {
namespace campus_secondhand {

class Order : public drogon::orm::Model<Order> {
  public:
    Order() = default;

    DEFINE_FIELD_TYPES(BIGINT, VARCHAR, BIGINT, BIGINT, BIGINT, DECIMAL, TINYINT, TINYINT, VARCHAR, DATETIME, TEXT, DATETIME, DATETIME)
    DEFINE_FIELD_NAMES(id, order_no, buyer_id, seller_id, product_id, price, status, payment_method, meeting_place, meeting_time, remark, created_at, updated_at)

    MAPPING_FUNCTION(Order, "orders")

    int64_t getId() const { return getColumn("id").as<int64_t>(); }
    void setId(int64_t id) { setColumn("id", id); }

    const std::string& getOrderNo() const { return getColumn("order_no").as<std::string>(); }
    void setOrderNo(const std::string& orderNo) { setColumn("order_no", orderNo); }

    int64_t getBuyerId() const { return getColumn("buyer_id").as<int64_t>(); }
    void setBuyerId(int64_t buyerId) { setColumn("buyer_id", buyerId); }

    int64_t getSellerId() const { return getColumn("seller_id").as<int64_t>(); }
    void setSellerId(int64_t sellerId) { setColumn("seller_id", sellerId); }

    int64_t getProductId() const { return getColumn("product_id").as<int64_t>(); }
    void setProductId(int64_t productId) { setColumn("product_id", productId); }

    double getPrice() const { return getColumn("price").as<double>(); }
    void setPrice(double price) { setColumn("price", price); }

    int getStatus() const { return getColumn("status").as<int>(); }
    void setStatus(int status) { setColumn("status", status); }

    int getPaymentMethod() const { return getColumn("payment_method").as<int>(); }
    void setPaymentMethod(int paymentMethod) { setColumn("payment_method", paymentMethod); }

    const std::string& getMeetingPlace() const { return getColumn("meeting_place").as<std::string>(); }
    void setMeetingPlace(const std::string& meetingPlace) { setColumn("meeting_place", meetingPlace); }

    trantor::Date getMeetingTime() const { return getColumn("meeting_time").as<trantor::Date>(); }
    void setMeetingTime(const trantor::Date& meetingTime) { setColumn("meeting_time", meetingTime); }

    const std::string& getRemark() const { return getColumn("remark").as<std::string>(); }
    void setRemark(const std::string& remark) { setColumn("remark", remark); }

    trantor::Date getCreatedAt() const { return getColumn("created_at").as<trantor::Date>(); }
    void setCreatedAt(const trantor::Date& createdAt) { setColumn("created_at", createdAt); }

    trantor::Date getUpdatedAt() const { return getColumn("updated_at").as<trantor::Date>(); }
    void setUpdatedAt(const trantor::Date& updatedAt) { setColumn("updated_at", updatedAt); }
};

}  // namespace campus_secondhand
}  // namespace drogon_model