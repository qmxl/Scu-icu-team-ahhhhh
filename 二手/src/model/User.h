#pragma once

#include <drogon/orm/Model.h>
#include <drogon/orm/DbClient.h>
#include <trantor/utils/Date.h>

namespace drogon_model {
namespace campus_secondhand {

class User : public drogon::orm::Model<User> {
  public:
    User() = default;

    /**
     * @brief primary key
     */
    DEFINE_FIELD_TYPES(BIGINT, VARCHAR, VARCHAR, VARCHAR, VARCHAR, VARCHAR, VARCHAR, VARCHAR, VARCHAR, TINYINT, DATETIME, DATETIME)
    DEFINE_FIELD_NAMES(id, student_id, username, password, real_name, phone, email, avatar, department, grade, status, created_at, updated_at)

    MAPPING_FUNCTION(User, "users")

    int64_t getId() const { return getColumn("id").as<int64_t>(); }
    void setId(int64_t id) { setColumn("id", id); }

    const std::string& getStudentId() const { return getColumn("student_id").as<std::string>(); }
    void setStudentId(const std::string& studentId) { setColumn("student_id", studentId); }

    const std::string& getUsername() const { return getColumn("username").as<std::string>(); }
    void setUsername(const std::string& username) { setColumn("username", username); }

    const std::string& getPassword() const { return getColumn("password").as<std::string>(); }
    void setPassword(const std::string& password) { setColumn("password", password); }

    const std::string& getRealName() const { return getColumn("real_name").as<std::string>(); }
    void setRealName(const std::string& realName) { setColumn("real_name", realName); }

    const std::string& getPhone() const { return getColumn("phone").as<std::string>(); }
    void setPhone(const std::string& phone) { setColumn("phone", phone); }

    const std::string& getEmail() const { return getColumn("email").as<std::string>(); }
    void setEmail(const std::string& email) { setColumn("email", email); }

    const std::string& getAvatar() const { return getColumn("avatar").as<std::string>(); }
    void setAvatar(const std::string& avatar) { setColumn("avatar", avatar); }

    const std::string& getDepartment() const { return getColumn("department").as<std::string>(); }
    void setDepartment(const std::string& department) { setColumn("department", department); }

    const std::string& getGrade() const { return getColumn("grade").as<std::string>(); }
    void setGrade(const std::string& grade) { setColumn("grade", grade); }

    int getStatus() const { return getColumn("status").as<int>(); }
    void setStatus(int status) { setColumn("status", status); }

    trantor::Date getCreatedAt() const { return getColumn("created_at").as<trantor::Date>(); }
    void setCreatedAt(const trantor::Date& createdAt) { setColumn("created_at", createdAt); }

    trantor::Date getUpdatedAt() const { return getColumn("updated_at").as<trantor::Date>(); }
    void setUpdatedAt(const trantor::Date& updatedAt) { setColumn("updated_at", updatedAt); }
};

}  // namespace campus_secondhand
}  // namespace drogon_model